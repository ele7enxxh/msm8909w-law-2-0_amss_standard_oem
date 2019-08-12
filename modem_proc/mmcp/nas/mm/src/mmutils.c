/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001,2002,2003-2011 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmutils.c_v   1.26   17 Jul 2002 12:30:10   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmutils.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/18/14   gps     Replaced rand() function with ran_next()    
06/03/01   sbk     Global variables are renamed to use mm_ prefix
06/08/01   jca     Rewrote function mm_check_for_detach().
08/09/01   jc      Add changes for ota logging.
08/14/01   jc      Add FPOS to sizing of log packets.
08/15/01   ab      Renamed CM to CNM.
08/16/01   ab      Recovered code in 1.7 that were over written in 1.8.
08/22/01   jca     Fixed compilation warning.
09/18/01   sbk     Implemented functions to log packets
                   Rehashed reading IMEI and IMEISV values
10/19/01   sbk     Functions added to do plmn matches. Dummy RRC funcs to
                   avoid linker errors. Fixed a bug in creating STATIC ID log packet
11/02/01   sbk     Send log packet on reading IMEI and IMEISV.
11/16/01   jca     Added missing fields to log packets.
02/19/02   jca     Recoded to match NAS log packet name redefinitions.
04/11/02   jca     Fixed PLMN comparison bug reporting during IOT.
05/02/02   jca     Fixed memcpy bug in mm_read_imei().
05/02/02   vdr     Now the CKSN is set correctly according to current network mode
                   and current CN domain ID
06/05/02   ks      Added support for Dual mode operation.
07/17/02   ks      Replaced read_coded_classmark_2 with gs_read_coded_classmark_2.
07/23/02   ks      Added mm_rat_change_requested and mm_service_change_requested
                   functions to check if a RAT/Service change is requested.
08/02/02   ks      Created new function mm_convert_to_reg_cause() to conver the
                   MM reject to the REG specific cause
08/13/02   mks     Added mm_has_valid_ptmsi() to validate the stored PTMSI.
08/23/02   ks      changed the reg cause to MMR_CAUSE_FAILURE_REMAIN_ON_PLMN
                   when LR reject cause "LA_NOT_ALLOWED" is received.
09/04/02   vdr     Fixed determination of the CKSN
11/06/02   jca     Added function mm_update_mm_serving_plmn_info().
11/09/02   cd      Added msg_high when a PLMN mistmach is found
11/14/02   cd      In mm_check_held_service_request() corrected initialization of
                   ota_msg_length. Fixed lint errors
11/26/02   jca     Modified function mm_update_mm_serving_plmn_info().
12/05/02   jca     Changes to support PS_ONLY<->CS_ONLY mode switches in GSM mode.
12/12/02   ks      Replaced mm_sim_valid with function mmsim_sim_valid.
12/13/02   mks     Included header file mm_sim.h to fix compiler warning
01/13/03   jca     Added missing case in function mm_update_mm_serving_plmn_info().
01/27/03   jca     Changes to support PS_ONLY<->CS/PS mode switching.
                   Changes to support IMSI Detach via Combined GPRS Detach procedure.
02/10/03   cd      Fixed Lint errors
03/21/03   mks     Corrected mm_plmn_match () to handle the cases where MNC is only
                   2 digit long.
05/14/03   ab      Added function to send event log to capture OTA msg flow
05/29/03   jca     Set gmm_follow_on_request when ms_op_mode changes from
                   CS_ONLY to CS_PS to support PS on demand functionality.
06/18/03   jca     Now handling flag gmm_deregistered when ms_op_mode is set to
                   SYS_SRV_DOMAIN_CS_PS in function mm_update_mm_serving_plmn_info().
08/25/03   ks      Replaced MMR_CAUSE_FAILURE_REMAIN_ON_PLMN cause with
                   MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED in mm_convert_to_reg_cause()
09/25/03   mks     Now  sending MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED when MM/GMM
                   reject  cause  is  #12  (LA not allowed)  and  the feature
                   FEATURE_FORBID_LAI_UPON_REGIONAL_ROAMING_REJECT is enabled.
10/02/03   cd      Added check for ePLMN when comparing serving and requested PLMNs
01/28/04   ab      Move the event log function ahead of OTA log function to avoid any
                   log dependencies, CR38467.
02/05/04   ks      Deleted setting the gmm_follow_on flag here, as it is now
                   being set in mmcoord.c which is passed from REG.
05/10/04   ab      Added EVENT_MM_STATE, EVENT_GMM_STATE and
                       EVENT_MM_PLMN_INFORMATION event logging.
05/14/04   cd      Enhancements to CS FOR.
                   Fixed Lint errors
06/07/04   jca     Ensure mm_serving_plmn.info.service_status is set to STATUS_LIMITED
                   when CS service is available but UE not registered for CS service.
07/16/04   cd      Corrected processing of CM_SERVICE_REQ when follow_on_proceed is not set.
10/06/04   jca     When SIM is invalid for CS+PS, always over-ride service_status
                   to LIMITED_SERVICE and reg_domain to NONE in status to REG layer.
10/12/04   ab      Corrected the bit shift (<< 4) error in mmsend_gmm_state_log_packet().
01/18/05   sv      Sirius: Added utility functions to get: a) Periodic LU status and b) GPRS Attach Status.
01/27/05   sv      Removing feature guard for the utility functions provided for Sirius.
02/18/05   sm      Added support for EVENT_RAT_CHANGE
03/02/05   sm      Corrected the call to event_report_payload in mmsend_rat_change_event
04/01/05   hj      Changed mm_send_ota_msg_event_packet to enable SM Messages in
                   EVENT_NAS_MESSAGE_SENT and EVENT_NAS_MESSAGE_RECEIVED.
04/20/05   jca     Now block Detach procedure due to access class restrictions.
06/06/05   abe     Added code for CCBS support
06/15/05   HS      Added code for FEATURE_CCBS support for CC messages
06/20/05   hj      Added support for new events EVENT_SMGMM_REQUEST_SENT,
                   EVENT_SMGMM_REJECT_RECEIVED.
08/08/05   abe     Reference to the global variable mm_hplmn_from_imsi is
                   removed from the function mm_is_plmn_an_hplmn(). Used
                   the function reg_sim_hplmn_get() to get the HPLMN
                   for the feature FEATURE_ENHANCED_NW_SELECTION.
08/22/05   ks      If the SIM is already set to invalid for CS+PS, leave it
                   it CS invalid if the sim_state is invalid for CS only.
09/16/05   up      Updated mm_update_mm_serving_plmn_info to send the GSTK
                   event when service status is changed.
09/16/05   vdr     Now sending CM SERVICE REQUEST addressed by IMSI if that request has been
                   initiated during Emergancy Call establishment and INTER RAT W2G reselection                          
09/16/05   ajt     Added mm_parse_emergency_number to parse emergency numbers
                   from EMERGENCY Number List IE and store it in emergency_number_list.
11/28/05  abe      Guarded the GSTK call with feature flag FEATURE_GSTK
11/30/05   jca     Modified mmsend_request_reject_event() function prototype for
                   strong type checking and eliminating LINT errors.
02/03/06   hs      Fixed errors introduced due to using RVDS
06/14/60   up      New argument to mm_update_mm_serving_plmn_info is added
                   to block/unblock GSTK events.
06/14/06   nr      New function mm_get_mmcc_rej_cause is added.
06/16/06   up      Switch case for LCS event is put under FEATURE_MMGPS.
07/14/06   pk     Updated for Feature DSAC
03/21/07   NR      Removing mm_cnm_service_type global variable
03/03/08   ab  Modified mm_get_cs_cell_access to cs cell acess from DSAC only when mode is umts
07/04/11   jbk     Propagate MM_STD_RADIO_UNAVAILABLE to MN/CNM 
08/25/11   jbk      API to check if PLMN ID is valid
10/04/11   sla     Foreground search timer value is changed to T3212 timer value
10/10/11   gps     Corrected the log values for gmm substate and gmm update status
10/15/11  rajesh   Support to set CSMO bit in CM serv request when LAI is different
12/06/11   gps     Remembering DSAC information of the cell where last registration was attempted
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include <stdio.h>
#include <stdlib.h>
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "ms.h"
#include "timers_v.h"
#include "gs_v.h"
#include "ghdi_exp.h"
#include "ghdi_exp_int.h"
#include "libprot.h"
#include "mm_v.h"
#include "err.h"
#include "gmm_v.h"
#include "mm_umts_v.h"
#include "log.h"
#include "naslog_v.h"
#include "mm_coord.h"
#include "gmm_int_v.h"
#include "rrcmmif.h"
#include "mmauth.h"
#include "cause.h"
#include "mm_sim.h"
#include "sys_eplmn_list_v.h"
#include "sm_v.h"
#include "mm_rr.h"
#include "reg_sim_v.h"
#include "naslog_v.h"
#include "ran.h"
#include "reg_mode.h"
#include "fs_lib.h"
#include <stringl/stringl.h>
#include "reg_state.h"
#include "mcfg_fs.h"
#ifdef FEATURE_GSTK
#include "mm_gstk.h"
#endif
#include "dsumtsps.h"
#if defined(FEATURE_LTE)
#include "mm_iei.h"
#include "emm_rrc_if.h"
#include "emm_utility.h"
#endif
#include "PrngML.h"
#ifdef FEATURE_SEGMENT_LOADING
#include "mcfg_seg_load.h"
#include "ITDSCDMA.h"
#include "IWCDMA.h"
#include "IWCDMA_ext.h"
#endif
#ifdef FEATURE_NAS_REL10
#include "reg_nv.h"
#endif
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TDSCDMA)
#include "policyman.h"
#endif 
#if (defined(FEATURE_DUAL_SIM) && defined(FEATURE_LTE)) || defined(FEATURE_SGLTE)
#include "reg_mm.h"
#endif
/* Gets included from core\api\securemsm\crypto */
#include "secapi.h"

#define EMERGENCY_NUM_LIST_IE_LENGTH 50
#ifdef FEATURE_SGLTE
extern mm_sglte_stack_activity_status mm_sglte_current_stack_status;
extern boolean mm_sglte_mode;
extern mm_as_id_e_type mm_cs_stack_id;

#endif

#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
byte mm_stored_imei_sim[MAX_NAS_STACKS][MAX_IMEI_LEN+1] = {{0},{0},{0}};
#define mm_stored_imei (mm_stored_imei_sim[mm_as_id])
byte mm_stored_imeisv_sim[MAX_NAS_STACKS][MAX_IMEI_LEN+2] = {{0},{0},{0}};
#define mm_stored_imeisv (mm_stored_imeisv_sim[mm_as_id])

#elif defined(FEATURE_DUAL_SIM)  && defined(FEATURE_DUAL_IMEI)
byte mm_stored_imei_sim[MAX_AS_IDS][MAX_IMEI_LEN+1] = {{0},{0}};
#define mm_stored_imei (mm_stored_imei_sim[mm_sub_id])
byte mm_stored_imeisv_sim[MAX_AS_IDS][MAX_IMEI_LEN+2] = {{0},{0}};
#define mm_stored_imeisv (mm_stored_imeisv_sim[mm_sub_id])

#else
byte mm_stored_imei[MAX_IMEI_LEN+1] = {0};
byte mm_stored_imeisv[MAX_IMEI_LEN+2] = {0};
#endif

#ifdef FEATURE_LTE

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern sys_radio_access_tech_e_type mm_previous_rat_sim[MAX_NAS_STACKS];
#define mm_previous_rat mm_previous_rat_sim[mm_as_id]
#else
extern sys_radio_access_tech_e_type mm_previous_rat ;
#endif
#endif
#ifdef FEATURE_LTE
extern mm_emm_debug_buffer_s_type mm_emm_debug_buffer[MM_DEBUG_MESSAGE_BUFFER_SIZE];
extern dword mm_emm_debug_buffer_index ;
#endif

#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE)) 
 #error code not present
#endif // TEST_FRAMEWORK


#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE))
 #error code not present
#endif // TEST_FRAMEWORK
#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/

#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE) 
#define sys_eplmn_list_equivalent_plmn(a) sys_eplmn_per_nas_stacks_list_equivalent_plmn(a,(sys_modem_as_id_e_type)mm_sub_id,\
                                                                                             (sys_modem_as_id_e_type)mm_as_id)
extern boolean mm_dual_standby_mode;
extern boolean mm_sglte_mode;
#ifndef FEATURE_LTE
extern sys_mode_pref_e_type mm_sim_mode_pref_sim[MAX_NAS_STACKS];
#endif
extern byte mm_last_paging_cause_sim[MAX_NAS_STACKS];
#define  mm_last_paging_cause mm_last_paging_cause_sim[mm_as_id]

#else
extern byte mm_last_paging_cause;
#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern boolean mm_need_to_process_sim_avail_req_sim[MAX_NAS_STACKS];
extern boolean mm_waiting_for_open_session_cnf_sim[MAX_NAS_STACKS];
extern boolean mm_mn_sglte_svlte_mode;
#else
extern boolean mm_need_to_process_sim_avail_req;
extern boolean mm_waiting_for_open_session_cnf;
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#define sys_eplmn_list_get(a,b) sys_eplmn_per_nas_stacks_list_get(a,b,(sys_modem_as_id_e_type)mm_as_id)
#endif
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
void ghdi_ds_read_imeisv(byte *imeisv,sys_modem_as_id_e_type as_id);
#endif
extern void reg_state_log_variables(void);
extern void gs_read_coded_classmark_2_per_nas_stacks(
  classmark_2_ie_T  *classmark_2_ptr,
  classmark_params_T   *params_ptr,
  sys_modem_as_id_e_type stack_id_nas
);

extern boolean full_srv_req_in_emerg;
extern boolean emergency_call_after_lu_in_lpm;
extern void mm_log_mm_msg_info(byte pd,byte msg_id);

#ifdef FEATURE_FEMTO_CSG
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
boolean mmsim_camped_csg_add_to_white_list_sim[MAX_NAS_STACKS] = {0};
#define mmsim_camped_csg_add_to_white_list mmsim_camped_csg_add_to_white_list_sim[mm_as_id]
#else
boolean mmsim_camped_csg_add_to_white_list = FALSE;
#endif
#endif


/*===========================================================================

FUNCTION MM_READ_IMEI

DESCRIPTION
  This function gets the imei from ghdi, ensuring that the check digit is set
  to zero, as only zero is sent over the air. The function assumes that
  ghdi_read_imei writes a 15 digit IMEI preceded by a length field ( 8 ) and
  a type + odd/even indication nibble

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_read_imei( byte * write_ptr )
{
    ghdi_status_T  status = GHDI_SUCCESS;
   /* Read and cache the IMEI */
   if( mm_stored_imei[0] == 0)
   {
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
      status = ghdi_ds_read_imei(mm_stored_imei,(sys_modem_as_id_e_type)mm_sub_id);
#else
      status = ghdi_read_imei(mm_stored_imei);
#endif

      if(status != GHDI_SUCCESS)
      {
         MSG_ERROR_DS(MM_SUB, "=MM= Error reading IMEI", 0, 0,0 );
      }
      else
      {
         if (mm_stored_imei[0] == MAX_IMEI_LEN)
         {
            /* -----------------------------------------------------
            ** Ensure that the last digit of the IMEI is set to zero
            ** (see TS 23.003 section 6.2.1)
            ** ----------------------------------------------------- */
            mm_stored_imei[8] &= 0x0F;
         }

         /* Read and cache the IMEISV also */
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_IMEI)
         ghdi_ds_read_imeisv(mm_stored_imeisv,(sys_modem_as_id_e_type)mm_sub_id);
#else
         read_imeisv(mm_stored_imeisv);
#endif

         /* Send a log packet w/ IMEI and IMEISV */
         mmsend_nas_ue_static_id_log_packet();
      }
   }

   if( mm_stored_imei[0] == MAX_IMEI_LEN)
   {
      /*
       * If write_ptr is NULL, then this function is called
       * to read and cache the IMEI
       */
      if (write_ptr != NULL)
      {
         memscpy( write_ptr, MAX_IMEI_LEN+1, mm_stored_imei, MAX_IMEI_LEN+1 );
      }
   }
   else
   {
      MSG_ERROR_DS(MM_SUB, "=MM= IMEI wrong size %d from ghdi", mm_stored_imei[0], 0,0 );
   }
}


/*===========================================================================

FUNCTION MM_GET_RANDOM_VALUE

DESCRIPTION
  This function returns a random integer in the range 0 to
  max_rand_value
  It uses the library function PrngML_getdata() which returns a pseudo
  random number in the range 0 to RAND_MAX

  Now, if we require a random number from a range greater than
  0..RAND_MAX, then need to get two random numbers, one for the
  high part, and one for the low.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

dword mm_get_random_value( dword max_rand_value )
{
   dword    return_value = 0; /* Default to 0 */
   boolean  found;
   uint64   temp_return_value;
   uint64   temp_rand_val;
   uint64   temp_max_rand_value;
   uint32   random_num;

   temp_max_rand_value= (uint64)max_rand_value;

   if (max_rand_value == 0 )    /* trap silly value */
   {
      return_value = 0;
   }
   else
   {
      found = FALSE;

      while (!found )
      {

         /*
          * get number in range 0 - 2**62-1 (on x86 anyway...)
          * (range 0 - 2**30-1 on c166
          */

         if (PRNGML_ERROR_NONE != PrngML_getdata((uint8*)&random_num, (uint16) sizeof(random_num)))
         {
            /* Error: failed to get a random number */
            MSG_ERROR_DS(MM_SUB, "=MM=  Failed to get a random number", 0,0,0);
            return return_value;
         }

         /*
          * check if we can just do mod max_rand_value by checking that the
          * number is less than the biggest integer multiple of the
          * max_rand_value which is less than the biggest rand_val we can get
          */

         temp_rand_val = (uint64)random_num;

         if ( temp_rand_val <
             ((((uint64)(RAND_MAX + 1UL)*(uint64)(RAND_MAX + 1UL)) /
                  temp_max_rand_value )) * temp_max_rand_value
             )
         {

            temp_return_value = temp_rand_val % temp_max_rand_value;

            return_value = (uint32)temp_return_value;

            if ( return_value < max_rand_value )
            {
               found = TRUE;
            }
            else
            {
               MSG_ERROR_DS(MM_SUB, "=MM= algorithmic error, retrying...", 0,0,0);
            }
         }
      }
   }
   return return_value;
}


/*===========================================================================

FUNCTION MM_SECAPI_GET_RANDOM

DESCRIPTION
  This function returns a random unsigned integer using secapi_get_random
  which internally uses the library function PrngML_getdata() which returns a pseudo
  random number in the range 0 to RAND_MAX

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_secapi_get_random( uint8 *ran_temp, uint16 length )
{

   /* Max Buf Length allowed in secapi_get_random is max(uint16) -10 */
   #define MAX_BUF_LENGTH_ALLOWED ((1<<16) -11) 
     
   length = (uint16)MIN(length, MAX_BUF_LENGTH_ALLOWED);

   /* Get secure random number from secapi_get_random API.
   * If it returns E_SUCCESS, then API successfully returned random number.
   * else, API failed to generate a random number.
   * Make the process crash for this fatal error.
   * */    
   if(E_SUCCESS != secapi_get_random(SECAPI_SECURE_RANDOM, ran_temp, length))
   {
     MSG_FATAL_DS(MM_SUB,"=MM= secapi_get_random did not return successfully ", 0, 0, 0);
   }
  
}

/*===========================================================================

FUNCTION MM_PUT_AS_ID

DESCRIPTION
  This function passes the SIM id on which particular primitive is sent

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

sys_modem_stack_id_e_type mm_set_cm_stack_id(mm_as_id_e_type as_id)
{
  sys_modem_stack_id_e_type stack_id = SYS_MODEM_STACK_ID_1;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE)
  {
    if(as_id != mm_sub_id)
    {
      stack_id = SYS_MODEM_STACK_ID_2;
    }
  }
#endif
  return stack_id;
}

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
/*===========================================================================

FUNCTION MM_PUT_AS_ID

DESCRIPTION
  This function passes the SIM id on which particular primitive is sent

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_tag_as_id( IMH_T* primitive)
{
  /*
  ** mm_sub_id indicates the sub ID which is used with access stratum
  ** mm_as_id indicates the stack ID used between NAS and upper layers. 
  ** Going forward when both SGLTE and Dual SIM is working together we will 
  ** have three stacks in NAS and CM and two stacks in access stratum. Design here
  ** is to take care of such scenarios.
  */
  switch (primitive->message_set)
  {
    case MS_MM_REG :
      {
        switch (primitive->message_id)
        {
          case MMR_MM_INFORMATION_IND:
            ((mmr_mm_information_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_PLMN_SEARCH_CNF:
            ((mmr_plmn_search_cnf_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_REG_CNF:
            ((mmr_reg_cnf_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_SERVICE_IND:
            ((mmr_service_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_SIM_AVAILABLE_CNF:
            ((mmr_sim_available_cnf_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_SIM_NOT_AVAILABLE_CNF:
            ((mmr_sim_not_available_cnf_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_STOP_MODE_CNF:
            ((mmr_stop_mode_cnf_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
             break;
          case MMR_CAMPED_IND:
            ((mmr_camped_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_EMERGENCY_NUM_LIST_IND:
            ((mmr_emergency_num_list_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_CELL_SERVICE_IND:
            ((mmr_cell_service_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_LTE_CELL_INFO_IND:
            ((mmr_lte_cell_info_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_LTE_PESUDO_RESEL_LOW_PRIORITY_IND:
            ((mmr_rrc_pseudo_resel_low_priority_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
#ifdef FEATURE_HSPA_CALL_STATUS_IND
          case MMR_HSPA_CALL_STATUS_IND:
            ((mmr_hspa_call_status_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
#endif
          case MMR_PH_STATUS_CHANGE_CNF:
            ((mmr_ph_status_change_cnf_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;

#ifdef FEATURE_OOSC_SERVICE_STATUS_UI_UPDATE
          case MMR_CONN_MODE_SERVICE_STATUS_IND:
            ((mmr_conn_mode_service_status_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
#endif
          case MMR_REG_REJECT_IND:
            ((mmr_reg_reject_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_DS_STAT_CHGD_CNF:/*NO SIM ID for this primitive*/
            break;
          case MMR_CELL_ACCESS_IND:
            ((mmr_cell_access_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_ATTACH_FAILED_IND:                               
            ((mmr_attach_failed_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;

#ifdef FEATURE_DUAL_SIM
          case MMR_MULTIMODE_SUBS_CHGD_CNF:
           break;
#endif
#ifdef FEATURE_LTE
          case MMR_UPDATE_CURR_RAT_LIST_IND:
            ((mmr_update_curr_rat_list_ind_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
#endif
          case MMR_PS_DETACH_CNF:
            ((mmr_ps_detach_cnf_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_SUITABLE_SEARCH_END_IND:
            ((mmr_suitable_search_end_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_DISABLE_BPLMN_IND:
            ((mmr_disable_bplmn_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_FPLMN_LIST_CHANGE_IND:
            ((mmr_fplmn_list_change_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_SET_DRX_CNF:
            ((mmr_set_drx_cnf_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_GET_DRX_CNF:
            ((mmr_get_drx_cnf_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_HOME_NODEB_NAME_IND:
            ((mmr_home_nodeb_name_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;

          case MMR_STACK_DEACT_IND :
          case MMR_STACK_ACT_IND  :
          case MMR_SERVICE_STAT_IND :
            break;
#ifdef FEATURE_FEMTO_CSG
          case MMR_CSG_SEARCH_TRIGGER_IND:
            ((mmr_csg_search_trigger_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_CSG_LIST_READ_COMPLETE_IND:
            ((mmr_csg_list_read_complete_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
#endif
          case MMR_T3346_TIMER_IND:
            ((mmr_t3346_timer_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_REG_REQ_IGNORED_IND:
            break;
#ifdef FEATURE_LTE
          case MMR_RRC_SIB6_INFO_IND:
            ((mmr_rrc_sib6_info_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
          case MMR_RRC_SIB7_INFO_IND:
            ((mmr_rrc_sib7_info_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;
#endif

          case MMR_PSM_READY_RSP:
            ((mmr_psm_ready_rsp_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;

          case MMR_PSM_READY_IND:
            ((mmr_psm_ready_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_as_id;
            break;

          default:
#ifdef FEATURE_DUAL_SIM
            MSG_FATAL_DS(MM_SUB, "=MM= Sending unexpected REG cmd %d", primitive->message_id,0,0);
#else
            MSG_ERROR_DS( MM_SUB, "=MM= Sending unexpected REG cmd %d", primitive->message_id, 0,0 );
#endif
        }
        break;
      }

    case MS_MM_RR :
      {
#ifdef FEATURE_DUAL_SIM
        switch (primitive->message_id)
        {
          case RR_ABORT_REQ:
            ((rr_abort_req_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_DATA_REQ:
            ((rr_data_req_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
             mm_log_mm_msg_info(((rr_data_req_T*)primitive)->layer3_message[0],
                                 ((rr_data_req_T*)primitive)->layer3_message[1]);
             break;
          case RR_DEACT_REQ:
            ((rr_deact_req_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            mm_msg_info = (byte)(((rr_deact_req_T*)primitive)->deact_reason);
            break;
          case RR_EST_REQ:
            ((rr_est_req_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
             mm_log_mm_msg_info(((rr_est_req_T*)primitive)->layer3_message[0],
                                ((rr_est_req_T*)primitive)->layer3_message[1]);                                 
          
            break;
          case RR_PLMN_LIST_REQ:
            ((rr_plmn_list_req_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_PLMN_SELECT_REQ:
            ((rr_plmn_select_req_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_FORBIDDEN_LAI_UPDATE_REQ:
            ((rr_forbidden_lai_update_req_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_SIM_INSERTED_REQ:
            ((rr_sim_inserted_req_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_INVALIDATE_SIM_DATA_REQ:
            ((rr_invalidate_sim_data_req_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_STOP_GSM_MODE_REQ:
            ((rr_stop_gsm_mode_req_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            mm_msg_info = (byte)(((rr_stop_gsm_mode_req_T*)primitive)->rr_mm_gsm_stop_reason);
            break;
          case RR_SIM_UPDATE_REQ:
            ((rr_sim_update_req_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_ACT_REQ:/*NO SIM ID for this primitive*/
            break;
          case RR_EQ_PLMN_LIST_CHANGE_IND:
            ((rr_eq_plmn_list_change_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_BPLMN_SEARCH_ABORT_REQ:
            ((rr_bplmn_search_abort_req_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_CHANGE_MODE_IND :
            ((rr_change_mode_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
#ifdef FEATURE_GSM_GPRS
          case RR_GMM_GPRS_STATE_CHANGE_REQ:
            ((rr_gmm_gprs_state_change_req_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_GMM_GPRS_TLLI_ASSIGNMENT_REQ:
            ((rr_gmm_gprs_tlli_assignment_req_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_GMM_READY_TIMER_STATE_REQ:
            ((rr_gmm_ready_timer_state_req_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_GMM_GPRS_MS_RA_CAP_IE_IND:
            ((rr_gmm_gprs_ms_ra_cap_ie_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_GMM_START_MM_NON_DRX_IND:
            ((rr_gmm_start_mm_non_drx_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_GMM_STOP_MM_NON_DRX_IND:
            ((rr_gmm_stop_mm_non_drx_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
#ifdef FEATURE_NAS_FREE_FLOAT
          case RR_GMM_GPRS_SERVICE_REQ:
            ((rr_gmm_gprs_service_req_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
#endif
          case RR_GMM_GPRS_SERVICE_CNF:
            ((rr_gmm_gprs_service_cnf_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_GMM_GPRS_NV_PARAMS_IND:/*NO SIM ID for this primitive*/
            ((rr_gmm_gprs_nv_params_ind_t*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_GMM_GPRS_CCCH_CAMP_IND:
            ((rr_gmm_gprs_ccch_camp_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_GMM_GPRS_PCCCH_CAMP_REQ:
            ((rr_gmm_gprs_pccch_camp_req_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_GMM_GPRS_AUTH_FAIL_IND:
            ((rr_gmm_gprs_auth_fail_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_NW_SEL_MODE_RESET_IND:
            ((rr_nw_sel_mode_reset_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_ACTIVATION_RSP:
            ((rr_activation_rsp_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
#endif  /* FEATURE_GSM_GPRS */

#ifdef FEATURE_GPRS_PS_HANDOVER
          case RR_PSHO_CNF:
            ((rr_psho_cnf_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
#endif
          case RR_DS_STATUS_CHANGE_IND:/*NO SIM ID for this primitive*/
            break;
          case RR_PS_ABORT_REQ:
            ((rr_ps_abort_req_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_PS_RESUME_IND:
            ((rr_ps_resume_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_CSFB_BLIND_REDIR_TO_LTE_REQ:
            ((rr_csfb_blind_redir_to_lte_req_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_LAI_REJECT_LIST_IND:
            ((rr_lai_reject_list_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case RR_CSFB_CALL_STATUS_IND:
#if (defined(FEATURE_DUAL_SIM) && defined(FEATURE_LTE))
            ((rr_csfb_call_status_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif
            break;
          case RR_LTE_RESELECTION_ALLOWED_STATUS_IND:
#if (defined(FEATURE_DUAL_SIM) && defined(FEATURE_LTE))
            ((rr_lte_reselection_allowed_status_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif
            break;
#ifdef FEATURE_DUAL_SIM
          case RR_MULTIMODE_SUBS_CHGD_REQ:
            break;
#endif
#if defined FEATURE_SGLTE || defined FEATURE_1XSRLTE || defined FEATURE_SVLTE_DUAL_SIM
          case RR_UE_MODE_IND:
            ((rr_ue_mode_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id; 
             break;
#endif 
           case RR_PSEUDO_LTE_LIST_UPDATE_IND:                
             ((rr_pseudo_lte_list_update_ind_T*)primitive)->as_id  = (sys_modem_as_id_e_type)mm_sub_id;
             break;

         case RR_SERVICE_STATUS_IND:
            ((rr_service_status_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
              break;

#ifdef FEATURE_FEMTO_CSG
            case RR_CSG_WHITE_LIST_UPDATE_IND:
              ((rr_csg_white_list_update_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
              break;
#endif 
#ifdef FEATURE_DUAL_DATA
          case RR_DATA_PRIORITY_IND:
            ((rr_data_priority_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
#endif
          case RR_MT_CSFB_FAILURE_IND:
            ((rr_mt_csfb_failure_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
              break;

          default:
            MSG_FATAL_DS( MM_SUB, "=MM= Sending unexpected RR cmd %d", primitive->message_id, 0,0 );
        }
#endif
        break;

      }

    case MS_CC_MM :
    case MS_CC_MM_PS :
      {
#ifdef FEATURE_DUAL_SIM
        switch (primitive->message_id)
        {
          case MMCNM_DATA_IND:/*DATA IND handled differntly*/
            break;
          case MMCNM_EST_REJ:
            ((mmcnm_est_rej_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case MMCNM_EST_CNF :
            ((mmcnm_est_cnf_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case MMCNM_REL_IND:
            ((mmcnm_rel_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case MMCNM_REL_CNF:
            ((mmcnm_rel_cnf_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case MMCC_REEST_CNF:
            ((mmcc_reest_cnf_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case MMCC_SYNC_IND:
            ((mmcc_sync_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case MMCNM_UNIT_DATA_IND:/*UNIT DATA IND handled differntly*/
            break;
          case MMCC_REEST_REJ:
            ((mmcc_reest_rej_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
#ifdef FEATURE_CCBS
          case MMCNM_PROMPT_IND:
            ((mmcnm_prompt_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
#endif
          case MMCC_SIG_CONN_REL_IND:
            ((mmcc_sig_conn_rel_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            break;
          case MMCNM_SRVCC_HANDOVER_COMPLETE_IND:
#if (defined(FEATURE_DUAL_SIM) && defined(FEATURE_LTE))
            ((mmcnm_srvcc_handover_complete_ind_T*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
#endif
            break;
          default:
            MSG_FATAL_DS( MM_SUB, "=MM= Sending unexpected CNM cmd %d", primitive->message_id, 0,0 );
        }
        
#endif
        break;
      }

      case MS_CM_MM :
      {
        switch (primitive->message_id)
        {
          case CM_CIPHER_IND:
            ((cm_cipher_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            ((cm_cipher_ind_s_type*)primitive)->stack_id = mm_set_cm_stack_id(mm_as_id);
            break;
          case CM_MM_PS_DATA_SUSPEND_IND:
            ((cm_mm_ps_data_suspend_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            ((cm_mm_ps_data_suspend_ind_s_type*)primitive)->stack_id = mm_set_cm_stack_id(mm_as_id);
            break;
          case CM_MT_CALL_PAGE_FAIL_IND:
            ((cm_mm_mt_call_fail_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            ((cm_mm_mt_call_fail_ind_s_type*)primitive)->stack_id = mm_set_cm_stack_id(mm_as_id);
            break;
          case CM_BEARER_CTXT_TRANSFER_IND: 
#if (defined(FEATURE_DUAL_SIM) && defined(FEATURE_LTE))
            ((cm_bearer_ctxt_transfer_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            ((cm_bearer_ctxt_transfer_ind_s_type*)primitive)->stack_id = mm_set_cm_stack_id(mm_as_id);
#endif
            break;
#ifdef FEATURE_LTE
          case CM_MM_SRVCC_HANDOVER_FAILURE_IND: 
            ((cm_mm_srvcc_handover_failure_ind_s_type*)primitive)->as_id = (sys_modem_as_id_e_type)mm_sub_id;
            ((cm_mm_srvcc_handover_failure_ind_s_type*)primitive)->stack_id = mm_set_cm_stack_id(mm_as_id);
            break;
#endif 
          default:
#ifdef FEATURE_DUAL_SIM
            MSG_FATAL_DS( MM_SUB, "=MM= Sending unexpected CM cmd %d", primitive->message_id, 0,0 );
#else
            MSG_ERROR_DS( MM_SUB, "=MM= Sending unexpected CM cmd %d", primitive->message_id, 0,0 );
#endif
        }
        break;
      }
    case MS_MM_RRC:/*RRC CMD handled differently*/
    case MS_MM_TDSRRC:
      break;
    case MS_GSDI:/* GSDI Messages handled differently*/
      break;
    default:
      MSG_ERROR_DS( MM_SUB, "=MM= Sending unexpected cmd %d", primitive->message_set, 0,0 );
      break;
  }
}

#endif

/*===========================================================================

FUNCTION MM_SEND_MESSAGE

DESCRIPTION
  This function sends a message to a queue and handle the status return

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_send_message( IMH_T * p_message, gs_queue_id_T queue)
{
   gs_status_T status;

   if ( NULL == p_message )
   {
     /* Sanity Check */
     MSG_ERROR_DS(MM_SUB,"p_message was NULL: Q=%d",queue,0,0);
     return;
   }

#ifdef FEATURE_SGLTE
   if((MM_SUB_IS_SGLTE) &&
      (p_message->message_set == MS_CC_MM) && (mm_as_id != mm_cs_stack_id))
   {
     MSG_HIGH_DS_2(MM_SUB," =MM=  Ignoring MM/CNM message %d on PS stack %d",p_message->message_id,mm_as_id);
     return;
   }

#endif
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   mm_tag_as_id(p_message);
#endif

   /* Add sent messages to the debug buffer */
   mm_add_message_to_debug_buffer(p_message->message_set,
                                                             p_message->message_id,
                                                             FROM_MM
                                                             ,(sys_modem_as_id_e_type)mm_as_id
                                                             );

   status = gs_send_message( queue, (void *)p_message, TRUE );

   if (status != GS_SUCCESS)
   {
      MSG_ERROR_DS( MM_SUB, "=MM= unable to send message, gs status %d", status, 0,0);
   }
}


/*===========================================================================

FUNCTION MM_CHECK_FOR_DETACH

DESCRIPTION
  This function checks if an IMSI detach is required.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_check_for_detach( void )
{
   mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_NONE);
   if ((mm_nreg_req_received) &&
       (mm_location_information.location_update_status == UPDATED) &&
       (mm_system_information.ATT) &&
       (mm_check_if_cs_cell_access_allowed(cs_cell_access) == TRUE))
   {
      /* ------------------------------------------
      ** Send an IMSI_DETACH message to the network
      ** ------------------------------------------ */
      mm_state_control( MM_STD_MS_DEACTIVATED_ATTACH_ALLOWED );
   }
}


/*===========================================================================

FUNCTION MM_PROCESS_DIAGNOSTIC

DESCRIPTION
  This function processs to deal with MM_status reports. There is no specific
  action to be performed, except diagnostic reporting. not used
  in normal service.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_process_diagnostic(mm_cmd_type *msg_p)
{
   (void) msg_p;      /* avoid lint warning */
   MSG_HIGH_DS_0(MM_SUB,"=MM= MM_STATUS message received");
}

/*=========================================================================== 

FUNCTION MM_SEND_NV_PARAMS_TO_LOWER_LAYERS

DESCRIPTION
     Sends nv paramaters to lower layers. needed to fix the issue related with power cycle and gcf flag.

DEPENDENCIES
     None

 RETURN VALUE
     None

 SIDE EFFECTS 

=======================================================================*/


void mm_send_nv_params_to_lower_layers( void )
{
  rr_gmm_gprs_nv_params_ind_t nv_parm_msg;
  /* populate the values for this structure*/
  nv_parm_msg.message_header.message_set = MS_MM_RR;
  nv_parm_msg.message_header.message_id = (int) RR_GMM_GPRS_NV_PARAMS_IND;
  PUT_IMH_LEN( sizeof( rr_gmm_gprs_nv_params_ind_t ) - sizeof(IMH_T),
                &nv_parm_msg.message_header );

  nv_parm_msg.nv_disable_call_type = (uint16)gmm_nv_disable_call_type;

  /* Set the GPRS NV parameters in the RR_ACT_REQ primitive */
  nv_parm_msg.nv_multislot_class    = gmm_multislot_class_nv; 
  nv_parm_msg.nv_non_drx_timer      = gmm_non_drx_timer;
  nv_parm_msg.nv_split_paging_cycle = gmm_split_paging_cycle;
  nv_parm_msg.nv_anite_gcf_flag     = gmm_anite_gcf_flag;
  nv_parm_msg.nv_gea_supported      = gmm_gea_support;
 
#ifdef FEATURE_GSM_EGPRS

  nv_parm_msg.nv_edge_feature_support = gmm_edge_feature_support;

 /* Send EDGE feature support info to RR only if EDGE is turned ON */
  if (gmm_edge_feature_support)
  {
     nv_parm_msg.nv_edge_multislot_class = gmm_edge_multislot_class_nv;
     nv_parm_msg.nv_edge_8psk_power_class = gmm_edge_8psk_power_class;
     nv_parm_msg.nv_edge_8psk_power_capability = gmm_edge_8psk_power_capability;
  }
#endif /* FEATURE_GSM_EGPRS */

#ifdef FEATURE_REL_4_SUPPORT
#ifdef FEATURE_GSM_GERAN_FEATURE_PACK_1
  nv_parm_msg.nv_edge_nw_ass_cell_change = gmm_edge_nw_ass_cell_change;
  nv_parm_msg.nv_interrat_nacc_support = gmm_interrat_nacc_support;
#endif /* FEATURE_GSM_GERAN_FEATURE_PACK_1 */
#endif /* FEATURE_REL_4_SUPPORT */

#ifdef FEATURE_GSM_DTM
  nv_parm_msg.nv_dtm_feature_support    = gmm_nv_dtm_feature_support;
  nv_parm_msg.nv_dtm_multislot_subclass = gmm_nv_dtm_multislot_subclass_nv;
#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_GSM_EDTM
  nv_parm_msg.nv_edtm_feature_support     = gmm_nv_edtm_feature_support;
#endif /* FEATURE_GSM_EDTM */

#ifdef FEATURE_GSM_GPRS_MSC34
  nv_parm_msg.nv_hmsc_gprs_coded_ms_class    = (byte) gmm_set_multislot_class (gmm_multislot_class_nv);
#ifdef FEATURE_GSM_EGPRS
  nv_parm_msg.nv_hmsc_egprs_coded_ms_class   = (byte) gmm_set_multislot_class (gmm_edge_multislot_class_nv);
#endif
#endif /* FEATURE_HMSC34*/

  mm_send_message( &nv_parm_msg.message_header, GS_QUEUE_RR );
}

/*===========================================================================

FUNCTION MM_CHECK_PENDING_REQUESTS

DESCRIPTION
  This function is called when MM-idle state is entered from WAIT FOR NETWORK
  COMMAND. This checks if there were any CNM_SERVICE_REQUESTs which were
  requested but not yet sent due to MM being busy. If present, the request
  is sent, assuming the updtate status is UPDATED

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_check_pending_requests( void )
{
   byte                       *write_ptr;
   boolean                    error_indication;
   byte                       ciph_key_seq_num;

   if ( mm_held_cnm_service_req.present != FALSE )    /* contains a message */
   {
      // If NV full_srv_req_in_emerg is enabled, then we need to do LU before we can place the emergency
      // call. After one failed attempt/ reject, NAS should retry once more. If LU fails on the second attempt
      // as well, then reject the call.
      if ((( mm_location_information.location_update_status == UPDATED )&&
           ( memcmp( mm_serving_lai,
                     mm_location_information.lai,
                     LAI_SIZE ) == 0 ))
           ||
          ((mm_held_cnm_service_req.establishment_cause == (int)MM_RR_EMERGENCY_CALL)&&
           (full_srv_req_in_emerg == FALSE))
           ||
          ((mm_held_cnm_service_req.establishment_cause == (int)MM_RR_EMERGENCY_CALL)&&
           (emergency_call_after_lu_in_lpm == TRUE))
           ||
          ((mm_held_cnm_service_req.establishment_cause == (int)MM_RR_EMERGENCY_CALL)&&
           (full_srv_req_in_emerg == TRUE)&& (mm_lu_attempt_counter == 0))
          )
      {
         ciph_key_seq_num =
              auth_get_ciphering_key_sequence_number(
                                    mm_held_cnm_service_req.security_context) ;

         /*
          * clear out old cipher key sequence number
          */

         mm_held_cnm_service_req.message.cipher_key_seq_no_service_type &= 0x0f;

         /*
          * insert the new value
          */

         mm_held_cnm_service_req.message.cipher_key_seq_no_service_type |=
            (byte) (ciph_key_seq_num << 4);

         write_ptr = &mm_held_cnm_service_req.message.data[0];

         error_indication = FALSE;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
         {
           classmark_params_T classmark_params;
           classmark_params.gsm_supported   = mm_gsm_supported;
           classmark_params.wcdma_supported = (mm_wcdma_supported||mm_tdscdma_supported);
           classmark_params.wcdma_in_use    = is_umts_mode();
           classmark_params.MSCR99          = mm_system_information.mscr;
           classmark_params.as_id           = (sys_modem_as_id_e_type)mm_sub_id;
           gs_read_coded_classmark_2_per_nas_stacks(
            (classmark_2_ie_T *)write_ptr,
            &classmark_params,(sys_modem_as_id_e_type)mm_as_id );
         }
#else
         gs_read_coded_classmark_2( (classmark_2_ie_T *)write_ptr, mm_gsm_supported,
                                  (mm_wcdma_supported||mm_tdscdma_supported), is_umts_mode(), mm_system_information.mscr );
#endif

         /* note, NO IEI present */

         /*
          * point write_ptr to the next free byte after the classmark data
          */

         write_ptr += mm_held_cnm_service_req.message.data[0] + 1;

         if (has_valid_tmsi() && (!((mm_held_cnm_service_req.present) &&
                                    (mm_held_cnm_service_req.establishment_cause == (int) MM_RR_EMERGENCY_CALL) &&
                                    ((mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_RESELECTION) ||
                                     (mm_held_cnm_service_req.interat_status == MM_AS_INTERRAT_CC_ORDER))))
             && (mm_location_information.location_update_status == UPDATED)
             && (memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0))
         {
            /* Copy TMSI and advance the pointer */
            write_ptr += mm_pack_tmsi(write_ptr);
         }
         else
         {

            /*
             * Get IMSI instead
             */
            if (mmsim_sim_valid(SYS_SRV_DOMAIN_CS_ONLY))
            {
               /* Copy IMSI and advance the pointer */
               write_ptr += mm_pack_imsi(write_ptr);
            }
            else
            {
               if ( mm_held_cnm_service_req.establishment_cause == (int)MM_RR_EMERGENCY_CALL )
               {

                  mm_read_imei( write_ptr );

                  write_ptr += ( *write_ptr ) + 1;

               }
               else
               {
                  error_indication = TRUE;
               }
            }
         }

#if defined(FEATURE_LTE)
         if(mm_held_cnm_service_req.include_additional_update_ie)
         {
           mm_held_cnm_service_req.include_additional_update_ie = FALSE;
           *write_ptr = (IEI_ADDITIONAL_UPDATE_PARAM << 4) & 0xF0;
           *write_ptr = *write_ptr | 0x02;
           write_ptr = write_ptr+1;
         }
#endif

         /*
          * Check that we can proceed with connection request
          */

         PUT_IMH_LEN( ( unsigned char )
                        ( ( ( ( byte * )write_ptr ) -
                            ( ( byte * ) &mm_held_cnm_service_req.message ) ) ) -
                                 sizeof( IMH_T ),
                      &mm_held_cnm_service_req.message.msg_head.IMH );


         if ( error_indication == FALSE )
         {
            /* -------------------------------------------
            ** Check to see if there is a RRC connection and the
            ** MM state is other than IDLE.  It is possible that
            ** MM has just processed a CLOSE_SESSION_IND and
            ** although the connection is still open, it has
            ** transitioned to IDLE state.
            * --------------------------------------------*/
            if ((rrc_connection_status == MMRRC_ACTIVE) && (mm_state != MM_IDLE))
            {
               if ((mm_follow_on_proceed)&& (mm_state == MM_LOCATION_UPDATE_INITIATED))
               {
                   mm_held_cnm_service_req.message.msg_head.IMH.message_id = (int)RR_DATA_REQ;

                   mm_send_ota_message( (byte *)&mm_held_cnm_service_req.message,
                                        (dword)((byte *)write_ptr - (byte *)&mm_held_cnm_service_req.message)
                                         - sizeof(IMH_T));

                   mm_state_control( MM_STD_LU_ACCEPT_FOLLOW_ON );

                   mm_follow_on_proceed = FALSE;

                   mm_start_timer( TIMER_T3230, DEFAULT_TIMEOUT );

                   MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent CM SERVICE REQUEST");

                   mm_held_cnm_service_req.present = FALSE;
                   mm_follow_on_request = FALSE;

               }
               else if (mm_state == MM_CONNECTION_ACTIVE)
               {
                 MSG_HIGH_DS_0(MM_SUB, "=MM= MM sent CM SERVICE REQUEST");
                 mm_held_cnm_service_req.message.msg_head.IMH.message_id = (int)RR_DATA_REQ;
                 mm_send_ota_message( (byte *)&mm_held_cnm_service_req.message,
                                        (dword)((byte *)write_ptr - (byte *)&mm_held_cnm_service_req.message)
                                         - sizeof(IMH_T));

                 mm_state_control( MM_STD_SERVICE_REQUEST_SENT );
                 mm_held_cnm_service_req.present = FALSE; 
               } 
               else if (!mm_follow_on_proceed)
               {
                   MSG_ERROR_DS(MM_SUB, "=MM= Pending CM_SERVICE_REQ - FOP=0 mm_state %d",mm_state,0,0);

               }
            }
            else
            {
               mm_initiate_rr_connection( (establishment_cause_T)mm_held_cnm_service_req.establishment_cause,
                                          (IMH_T *)&mm_held_cnm_service_req.message );

               mm_state_control( MM_STD_MM_CONNECTION_REQUEST );
            }
         }

      }
      // If NV full_srv_req_in_emerg is enabled, then we need to do LU before we can place the emergency
      // call. After one failed attempt/ reject, NAS should retry once more. If LU fails on the second attempt
      // as well, then reject the call.
      else if ((mm_held_cnm_service_req.establishment_cause == (int)MM_RR_EMERGENCY_CALL)&&
           (full_srv_req_in_emerg == TRUE)&& (mm_lu_attempt_counter > 1))
      {
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
            mm_send_mmcnm_est_rej(
               AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL ,MMCC_SMS_CS);
#else
            mm_send_mmcnm_est_rej(
               AS_REJECT_CAUSE, AS_REJ_LOW_LEVEL_FAIL );
#endif
      }
   }
}


/*===========================================================================

FUNCTION IS_OF_MM_MSG_TYPE

DESCRIPTION
  This function checks if a message is an MM peer-to-peer message and if it is
  checks it for the given message type

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean is_of_mm_msg_type(byte type, byte* msg)
{
   mm_msg_head_T *hd = (mm_msg_head_T*)msg;
   /* Is this a data ind */
   if( hd->IMH.message_set == (int)MS_MM_RR && (hd->IMH.message_id == (int)RR_DATA_IND
      || hd->IMH.message_id == (int)MM_DATA_IND) )
   {
      /* is this a mm message */
      if( GET_PD(hd->skip_pd) == PD_MM)
      {
         if( hd->type == type)
         {
            return TRUE;
         }
      }
   }
   return FALSE;
}


/*===========================================================================

FUNCTION MM_PACK_TMSI

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

uint32 mm_pack_tmsi(byte *write_ptr)
{
   /* Fill in the TMSI*/
   *write_ptr = TMSI_SIZE + 1;    /* Length of TMSI + Type field */
   write_ptr++;

   /* Code as TMSI */
   *write_ptr = 0xF4;
   write_ptr++;

   /* Copy TMSI */
   (void)memscpy( write_ptr, TMSI_SIZE, mm_location_information.tmsi, TMSI_SIZE );
   write_ptr += TMSI_SIZE;

   return TMSI_SIZE + 2;
}
/*===========================================================================

FUNCTION MM_PACK_PTMSI

DESCRIPTION
  
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

#ifdef FEATURE_NAS_REL11
uint32 mm_pack_ptmsi(byte *write_ptr)
{
   /*Fill in the PTMSI*/
   *write_ptr = PTMSI_SIZE + 1;    /* Length of PTMSI + Type field */
   write_ptr++;

   /* Code as PTMSI */
   *write_ptr = 0xF4;
   write_ptr++;

   /* Copy PTMSI */
   (void)memscpy( write_ptr, PTMSI_SIZE, gmm_stored_ptmsi.digit, PTMSI_SIZE );
   write_ptr += PTMSI_SIZE;

   return PTMSI_SIZE + 2;
}
#endif




/*===========================================================================

FUNCTION MM_PACK_IMSI

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

uint32 mm_pack_imsi(byte *write_ptr)
{
   *write_ptr = mm_stored_imsi.length;
   write_ptr++;

   (void)memscpy(write_ptr, IMSI_LEN_IN_BYTES, mm_stored_imsi.data, mm_stored_imsi.length);

   write_ptr += mm_stored_imsi.length;

   return mm_stored_imsi.length + 1;
}


/*===========================================================================

FUNCTION MM_SEND_OTA_MSG_LOG_PACKET

DESCRIPTION
  Sends the NAS signalling message log packet to diag.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_ota_msg_event_packet(byte   direction,
                                  uint32 msg_size,
                                  byte   *msg_ptr)  /* L3 message */
{
   nas_generic_identity_type id;
   boolean log_event = FALSE;

   /* Get PD and TI */
   if((msg_size != 0) && (msg_ptr != NULL))
   {
      id.set = GET_PD(*msg_ptr);

      id.channel = GET_TI(*msg_ptr);

      if (msg_size > 1)
      {
        id.id = *(msg_ptr+1);  /* message type */
      }
      else
      {
        id.id = NULL;
      }
   }
   else
   {
      id.set     = NULL;
      id.id      = NULL;
      id.channel = NULL;
   }

   /* Get OTA message type */
   switch (id.set)
   {
      case PD_CC:
         id.id &= 0x3F; /* MASK off bits 8 & 7 */
         switch ( id.id )
         {
            case NAS_OTA_ALERTING:
            case NAS_OTA_CALL_CONFIRMED:
            case NAS_OTA_CALL_PROCEEDING:
            case NAS_OTA_CONNECT:
            case NAS_OTA_CONNECT_ACK:
            case NAS_OTA_EMERGENCY_SETUP:
            case NAS_OTA_PROGRESS:
            case NAS_OTA_SETUP:
            case NAS_OTA_MODIFY:
            case NAS_OTA_MODIFY_COMPLETE:
            case NAS_OTA_MODIFY_REJECT:
            case NAS_OTA_USER_INFORMATION:
            case NAS_OTA_HOLD:
            case NAS_OTA_HOLD_ACK:
            case NAS_OTA_HOLD_REJECT:
            case NAS_OTA_RETRIEVE:
            case NAS_OTA_RETRIEVE_ACK:
            case NAS_OTA_RETRIEVE_REJ:
            case NAS_OTA_DISCONNECT:
            case NAS_OTA_RELEASE:
            case NAS_OTA_RELEASE_COMPLETE:
            case NAS_OTA_CONGESTION_CONTROL:
            case NAS_OTA_NOTIFY:
            case NAS_OTA_STATUS_MSG:
            case NAS_OTA_STATUS_ENQUIRY:
            case NAS_OTA_START_DTMF:
            case NAS_OTA_STOP_DTMF:
            case NAS_OTA_STOP_DTMF_ACK:
            case NAS_OTA_START_DTMF_ACK:
            case NAS_OTA_START_DTMF_REJ:
            case NAS_OTA_FACILITY:
#ifdef FEATURE_CCBS
            case NAS_OTA_CC_ESTABLISHMENT:
            case NAS_OTA_CC_ESTABLISHMENT_CONFIRMED:
            case NAS_OTA_RECALL:
            case NAS_OTA_START_CC:
#endif /* FEATURE_CCBS */
               log_event = TRUE;
               break;
            default:
               break;
         }
         break;

      case PD_SMS:
         id.id &= 0x3F; /* MASK off 8, 7 bit */
         switch ( id.id )
         {
            case NAS_OTA_CP_DATA:
            case NAS_OTA_CP_ACK:
            case NAS_OTA_CP_ERROR:
               log_event = TRUE;
               break;
            default:
               break;
         }
         break;

      case PD_SS:
         id.id &= 0x3F; /* MASK off 8, 7 bit */
         switch ( id.id )
         {
            case NAS_OTA_FACILITY:
            case NAS_OTA_REGISTER:
            case NAS_OTA_RELEASE_COMPLETE:
               log_event = TRUE;
               break;
            default:
               break;
         }
         break;

      case PD_MM:
         switch ( id.id )
         {
            case IMSI_DETACH_INDICATION:
            case LOCATION_UPDATING_ACCEPT:
            case LOCATION_UPDATING_REJECT:
            case LOCATION_UPDATING_REQUEST:
            case AUTHENTICATION_REJECT:
            case AUTHENTICATION_REQUEST:
            case AUTHENTICATION_RESPONSE:
            case IDENTITY_REQUEST:
            case IDENTITY_RESPONSE:
            case TMSI_REALLOCATION_COMMAND:
            case TMSI_REALLOCATION_COMPLETE:
            case CM_SERVICE_ACCEPT:
        #ifdef FEATURE_CCBS
            case CM_SERVICE_PROMPT:
        #endif
            case CM_SERVICE_REJECT:
            case CM_SERVICE_ABORT:
            case CM_SERVICE_REQUEST:
            case CM_REESTABLISHMENT_REQUEST:
            case ABORT:
            case MM_STATUS:
            case MM_INFORMATION:
            case AUTHENTICATION_FAILURE:
            case PAGING_RESPONSE:
               log_event = TRUE;
               break;
            default:
               break;
         }
         break;

      case PD_GMM:
         switch ( id.id )
         {
            case GMM_ATTACH_REQUEST:
            case GMM_ATTACH_ACCEPT:
            case GMM_ATTACH_COMPLETE:
            case GMM_ATTACH_REJECT:
            case GMM_DETACH_REQUEST:
            case GMM_DETACH_ACCEPT:
            case GMM_ROUTING_AREA_UPDATE_REQUEST:
            case GMM_ROUTING_AREA_UPDATE_ACCEPT:
            case GMM_ROUTING_AREA_UPDATE_COMPLETE:
            case GMM_ROUTING_AREA_UPDATE_REJECT:
            case GMM_SERVICE_REQUEST:
            case GMM_SERVICE_ACCEPT:
            case GMM_SERVICE_REJECT:
            case GMM_PTMSI_REALLOCATION_COMMAND:
            case GMM_PTMSI_REALLOCATION_COMPLETE:
            case GMM_AUTHENTICATION_AND_CIPHERING_REQUEST:
            case GMM_AUTHENTICATION_AND_CIPHERING_RESPONSE:
            case GMM_AUTHENTICATION_AND_CIPHERING_REJECT:
            case GMM_AUTHENTICATION_AND_CIPHERING_FAILURE:
            case GMM_IDENTITY_REQUEST:
            case GMM_IDENTITY_RESPONSE:
            case GMM_STATUS:
            case GMM_INFORMATION:
               log_event = TRUE;
               break;
           default:
              break;
         }
         break;
       /* -----------------------------------------------------------------------
       ** Enable SM OTA messages to be printed as part of EVENT_NAS_MESSAGE_SENT
       ** and EVENT_NAS_MESSAGE_RECEIVED
       ** ----------------------------------------------------------------------*/
      case PD_GPRS_SM:
         switch (id.id)
         {
            case ACTIVATE_PDP_REQUEST:
            case ACTIVATE_PDP_ACCEPT:
            case ACTIVATE_PDP_REJECT:
            case REQUEST_PDP_ACTIVATION:
            case REQUEST_PDP_ACTIVATION_REJ:
            case DEACTIVATE_PDP_REQUEST:
            case DEACTIVATE_PDP_ACCEPT:
            case NETWORK_MODIFY_PDP_REQUEST:
            case MS_MODIFY_PDP_ACCEPT:
            case MS_MODIFY_PDP_REQUEST:
            case MODIFY_PDP_ACCEPT:
            case MODIFY_PDP_REJECT:
            case ACTIVATE_SEC_PDP_REQUEST:
            case ACTIVATE_SEC_PDP_ACCEPT:
            case ACTIVATE_SEC_PDP_REJECT:
            case SM_STATUS:
            case REQUEST_SEC_PDP_ACTIVATION:
            case REQUEST_SEC_PDP_ACTIVATION_REJ:

               log_event = TRUE;
               break;
            default:
               break;
         }/*end switch (PD_GPRS_SM) */
         break;

      default:
         log_event = FALSE;
         break;

   }/* end switch(set) */

   if ( TRUE == log_event )
   {
#ifdef FEATURE_DUAL_SIM
      byte event_packet[1+sizeof(nas_generic_identity_type)];

      event_packet[0] = (byte) mm_as_id;
      memscpy(&event_packet[1], sizeof(nas_generic_identity_type),(void *)&id,sizeof(nas_generic_identity_type));

      if ( mm_dual_standby_mode )
      {
        if ( direction == UE_TO_CN_DIRECTION )
        {
           event_report_payload(EVENT_DS_NAS_MESSAGE_SENT, 4, (void *)event_packet);
        }
        else
        {
           event_report_payload(EVENT_DS_NAS_MESSAGE_RECEIVED, 4, (void *)event_packet);
        }
      }
      else
#endif
      {
        if ( direction == UE_TO_CN_DIRECTION )
        {
           event_report_payload(EVENT_NAS_MESSAGE_SENT, 3, (void *)&id);
        }
        else
        {
           event_report_payload(EVENT_NAS_MESSAGE_RECEIVED, 3, (void *)&id);
        }
      }

   }
}


/*===========================================================================

FUNCTION MM_SEND_OTA_MSG_LOG_PACKET

DESCRIPTION
  Sends the NAS signalling message log packet to diag.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_send_ota_msg_log_packet(byte   direction,
                                uint32 msg_size,
                                byte   *msg_ptr)
{
   LOG_UMTS_NAS_OTA_MESSAGE_LOG_PACKET_C_type *log_ptr;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   LOG_UMTS_DS_NAS_OTA_MESSAGE_LOG_PACKET_C_type *ds_log_ptr;
#endif

   uint32                                   pkt_size;

   /* Log events */
   mm_send_ota_msg_event_packet(direction, msg_size, msg_ptr);

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if ( mm_dual_standby_mode  || mm_sglte_mode)
   {
     /* Calculate log packet to be sent */
     pkt_size = FPOS(LOG_UMTS_DS_NAS_OTA_MESSAGE_LOG_PACKET_C_type, nas_message) + msg_size;

     /* Allocate log packet */
     ds_log_ptr = (LOG_UMTS_DS_NAS_OTA_MESSAGE_LOG_PACKET_C_type *)
                log_alloc(LOG_UMTS_DS_NAS_OTA_MESSAGE_LOG_PACKET_C, pkt_size);

     if (ds_log_ptr == NULL)
     {
        /* Could not allocate buffer */
        return;
     }

     /* Set packet elements */
     ds_log_ptr->as_id = (uint8) mm_as_id;
     ds_log_ptr->nas_message_direction = direction;
     ds_log_ptr->nas_message_length    = msg_size;

     memscpy((byte*)(ds_log_ptr->nas_message), LOG_MAX_NAS_OTA_MESSAGE_SIZE,
            msg_ptr,
                msg_size);

     /* Send the packet */
     log_commit(ds_log_ptr);

   }
   else
#endif
   {
     /* Calculate log packet to be sent */
     pkt_size = FPOS(LOG_UMTS_NAS_OTA_MESSAGE_LOG_PACKET_C_type, nas_message) + msg_size;

     /* Allocate log packet */
     log_ptr = (LOG_UMTS_NAS_OTA_MESSAGE_LOG_PACKET_C_type *)
                log_alloc(LOG_UMTS_NAS_OTA_MESSAGE_LOG_PACKET_C, pkt_size);

     /* Check to see if log packet allocated */
     if (log_ptr == NULL)
     {
        /* Could not allocate buffer */
        return;
     }

     /* Set packet elements */
     log_ptr->nas_message_direction = direction;
     log_ptr->nas_message_length    = msg_size;

     memscpy((byte*)(log_ptr->nas_message), LOG_MAX_NAS_OTA_MESSAGE_SIZE,
            msg_ptr,
                msg_size);

     /* Send the packet */
     log_commit(log_ptr);

   }

   return;
}


/*===========================================================================

FUNCTION MMSEND_MM_INFO_EVENT_PACKET

DESCRIPTION
  This function sends the MM info event packet to DIAG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmsend_mm_info_event_packet(void)
{
   byte event_packet[16];
   byte as_id_size = 0;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if ( mm_dual_standby_mode || mm_sglte_mode)
   {
     event_packet[0] = (byte) mm_as_id;
     as_id_size = 1;
   }
#endif

   /* PLMN Info */
   event_packet[as_id_size+0] = (byte)mm_serving_plmn.info.plmn.identity[0];
   event_packet[as_id_size+1] = (byte)mm_serving_plmn.info.plmn.identity[1];
   event_packet[as_id_size+2] = (byte)mm_serving_plmn.info.plmn.identity[2];

   /* LAI */
   event_packet[as_id_size+3] = (byte)mm_serving_plmn.lac.lac[0];
   event_packet[as_id_size+4] = (byte)mm_serving_plmn.lac.lac[1];

   /* RAC */
   event_packet[as_id_size+5] = (byte)mm_serving_plmn.rac;

   /* RAT, Capability, Service State & Mode */
   event_packet[as_id_size+6]  = ((byte)mm_serving_plmn.info.active_rat & 1) << 7;
   event_packet[as_id_size+6] |= ((byte)mm_serving_plmn.info.plmn_service_capability & 7) << 4;
   event_packet[as_id_size+6] |= ((byte)mm_serving_plmn.mm_service_state & 3) << 2;
   event_packet[as_id_size+6] |= ((byte)mm_serving_plmn.gmm_service_state & 3);

   /* Network & MS Mode */
   event_packet[as_id_size+7]  = ((byte)mm_serving_plmn.network_op_mode & 0x0f) << 4;
   event_packet[as_id_size+7] |= ((byte)mm_serving_plmn.ms_op_mode & 0x0f);

   /* Registered Domain */
   event_packet[as_id_size+8] = (byte)mm_serving_plmn.info.reg_service_domain;

#ifdef FEATURE_FEMTO_CSG
   /*  CSG ID and white_list*/
   memscpy(&event_packet[as_id_size+9], 16-(as_id_size +9),&mm_serving_plmn.info.csg_info.csg_id, 4);
  
   if(mm_is_csg_in_operator_list(mm_serving_plmn.info.plmn, mm_serving_plmn.info.csg_info.csg_id,FALSE) ||
       mm_is_csg_in_allowed_list(mm_serving_plmn.info.plmn, mm_serving_plmn.info.csg_info.csg_id))
   {
     event_packet[as_id_size+13] = 1;
   }
   else
   {
     event_packet[as_id_size+13] = 0;
   }
#endif 

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if ( mm_dual_standby_mode || mm_sglte_mode)
   {
#ifdef FEATURE_FEMTO_CSG
     event_report_payload(EVENT_DS_PLMN_INFORMATION, 15, (void *)event_packet);
#else
     event_report_payload(EVENT_DS_PLMN_INFORMATION, 10, (void *)event_packet);
#endif 
   }
   else
#endif
   {
#ifdef FEATURE_FEMTO_CSG
     event_report_payload(EVENT_PLMN_INFORMATION, 14, (void *)event_packet);
#else
     event_report_payload(EVENT_PLMN_INFORMATION, 9, (void *)event_packet);
#endif 
   }

}




/*===========================================================================

FUNCTION MMSEND_MM_STATE_LOG_PACKET

DESCRIPTION
  This function sends the MM state log packet to DIAG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmsend_mm_state_log_packet(void)
{
   LOG_UMTS_NAS_MM_STATE_LOG_PACKET_C_type *log_ptr;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   LOG_UMTS_DS_NAS_MM_STATE_LOG_PACKET_C_type *ds_log_ptr;
#endif


   byte event_packet[3];
   byte as_id_size = 0;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if ( mm_dual_standby_mode || mm_sglte_mode)
   {
     event_packet[0] = (byte) mm_as_id;
     as_id_size = 1;
   }
#endif

   /* MM State Event */
   event_packet[as_id_size+0] = (mm_idle_substate & 0x7) << 5;
   event_packet[as_id_size+0] |= mm_state & 0x1F;
   event_packet[as_id_size+1] = mm_location_information.location_update_status;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if ( mm_dual_standby_mode  || mm_sglte_mode)
   {
     event_report_payload(EVENT_DS_MM_STATE, 3, (void *)event_packet);
   }
   else
#endif
   {
     event_report_payload(EVENT_MM_STATE, 2, (void *)event_packet);
   }

   /* MM Events */
   mmsend_mm_info_event_packet();

   /* MM Logs */
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if ( mm_dual_standby_mode  || mm_sglte_mode)
   {
     ds_log_ptr = (LOG_UMTS_DS_NAS_MM_STATE_LOG_PACKET_C_type *)
                log_alloc(LOG_UMTS_DS_NAS_MM_STATE_LOG_PACKET_C,
                          sizeof(LOG_UMTS_DS_NAS_MM_STATE_LOG_PACKET_C_type));

     if(!ds_log_ptr)
     {
        /* Could not allocate memory */
        return;
     }

     ds_log_ptr->as_id = (uint8) mm_as_id;
     ds_log_ptr->mm_state_log = (uint8)mm_state;
     ds_log_ptr->mm_idle_substate_log = (uint8)mm_idle_substate;
     ds_log_ptr->mm_update_status = mm_location_information.location_update_status;

     log_commit(ds_log_ptr);
   }
   else
#endif
   {
     log_ptr = (LOG_UMTS_NAS_MM_STATE_LOG_PACKET_C_type *)log_alloc(LOG_UMTS_NAS_MM_STATE_LOG_PACKET_C,
        sizeof(LOG_UMTS_NAS_MM_STATE_LOG_PACKET_C_type));

     if(!log_ptr)
     {
        /* Could not allocate memory */
        return;
     }

     log_ptr->mm_state_log = (uint8)mm_state;
     log_ptr->mm_idle_substate_log = (uint8)mm_idle_substate;
     log_ptr->mm_update_status = mm_location_information.location_update_status;

     log_commit(log_ptr);
   }

}


/*===========================================================================

FUNCTION MMSEND_NAS_UE_STATIC_ID_LOG_PACKET

DESCRIPTION
  This function sends the UE static id log packet to DIAG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mmsend_nas_ue_static_id_log_packet(void)
{
   LOG_UMTS_NAS_UE_STATIC_ID_LOG_PACKET_C_type *log_ptr;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   LOG_UMTS_DS_NAS_UE_STATIC_ID_LOG_PACKET_C_type *ds_log_ptr;

   if ( mm_dual_standby_mode || mm_sglte_mode)
   {
     ds_log_ptr = (LOG_UMTS_DS_NAS_UE_STATIC_ID_LOG_PACKET_C_type *)
                log_alloc(LOG_UMTS_DS_NAS_UE_STATIC_ID_LOG_PACKET_C,
                          sizeof(LOG_UMTS_DS_NAS_UE_STATIC_ID_LOG_PACKET_C_type));

     if(!ds_log_ptr)
     {
        /* Could not allocate memory */
        return;
     }

     ds_log_ptr->as_id = (uint8) mm_as_id;
     ds_log_ptr->imsi_length = mm_stored_imsi.length;
     memscpy((void*)ds_log_ptr->imsi,LOG_MAX_IMSI_SIZE, mm_stored_imsi.data, mm_stored_imsi.length);

     memscpy((void*)ds_log_ptr->imei,LOG_IMEI_SIZE, mm_stored_imei, MAX_IMEI_LEN + 1);
     memscpy((void*)ds_log_ptr->imei_sv, LOG_IMEI_SV_SIZE, mm_stored_imeisv, MAX_IMEI_LEN+1+1);

     log_commit(ds_log_ptr);

   }
   else
#endif
   {
     log_ptr = (LOG_UMTS_NAS_UE_STATIC_ID_LOG_PACKET_C_type *)log_alloc(LOG_UMTS_NAS_UE_STATIC_ID_LOG_PACKET_C,
        sizeof(LOG_UMTS_NAS_UE_STATIC_ID_LOG_PACKET_C_type));

     if(!log_ptr)
     {
        /* Could not allocate memory */
        return;
     }

     log_ptr->imsi_length = mm_stored_imsi.length;
     memscpy((void*)log_ptr->imsi,LOG_MAX_IMSI_SIZE, mm_stored_imsi.data, mm_stored_imsi.length);

     memscpy((void*)log_ptr->imei,LOG_IMEI_SIZE, mm_stored_imei, MAX_IMEI_LEN + 1);
     memscpy((void*)log_ptr->imei_sv, LOG_IMEI_SV_SIZE, mm_stored_imeisv, MAX_IMEI_LEN+1+1);

     log_commit(log_ptr);
   }
}


/*===========================================================================

FUNCTION MMSEND_MM_INFO_LOG_PACKET

DESCRIPTION
  This function sends the MM info log packet to DIAG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mmsend_mm_info_log_packet(void)
{
   LOG_UMTS_MM_INFO_LOG_PACKET_C_type *log_ptr;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   LOG_UMTS_DS_MM_INFO_LOG_PACKET_C_type *ds_log_ptr;

   if ( mm_dual_standby_mode  || mm_sglte_mode)
   {
     ds_log_ptr = (LOG_UMTS_DS_MM_INFO_LOG_PACKET_C_type *)
                log_alloc(LOG_UMTS_DS_MM_INFO_LOG_PACKET_C,
                          sizeof(LOG_UMTS_DS_MM_INFO_LOG_PACKET_C_type));

     if(!ds_log_ptr)
     {
        /* Could not allocate memory */
        return;
     }

     ds_log_ptr->as_id = (uint8) mm_as_id;
     ds_log_ptr->network_operation_mode = (int)mm_serving_plmn.network_op_mode;
     ds_log_ptr->service_type = (uint8) mm_serving_plmn.info.plmn_service_capability;
     memscpy((void*)ds_log_ptr->selected_plmn_id.plmn_id, PLMN_ID_SIZE, &mm_serving_plmn.info.plmn,
        PLMN_ID_SIZE);
     memscpy((void*)ds_log_ptr->location_area_id.lai,LOCATION_AREA_ID_SIZE, mm_serving_plmn.lac.lac,
        LOCATION_AREA_ID_SIZE);
     ds_log_ptr->routing_area_code = mm_serving_plmn.rac;
     ds_log_ptr->num_available_plmns = 0;

     /* Log the PLMN search information */

     log_commit(ds_log_ptr);

   }
   else
#endif
   {
     log_ptr = (LOG_UMTS_MM_INFO_LOG_PACKET_C_type *)log_alloc(LOG_UMTS_MM_INFO_LOG_PACKET_C,
        sizeof(LOG_UMTS_MM_INFO_LOG_PACKET_C_type));

     if(!log_ptr)
     {
        /* Could not allocate memory */
        return;
     }

     log_ptr->network_operation_mode = (int)mm_serving_plmn.network_op_mode;
     log_ptr->service_type = (uint8) mm_serving_plmn.info.plmn_service_capability;
     memscpy((void*)log_ptr->selected_plmn_id.plmn_id,PLMN_ID_SIZE, &mm_serving_plmn.info.plmn,
        PLMN_ID_SIZE);
     memscpy((void*)log_ptr->location_area_id.lai, LOCATION_AREA_ID_SIZE, mm_serving_plmn.lac.lac,
        LOCATION_AREA_ID_SIZE);
     log_ptr->routing_area_code = mm_serving_plmn.rac;
     log_ptr->num_available_plmns = 0;

#ifdef FEATURE_FEMTO_CSG
     log_ptr->csg_id = mm_serving_plmn.info.csg_info.csg_id;
#else 
     log_ptr->csg_id = -1;
#endif 
     /* Log the PLMN search information */

     log_commit(log_ptr);
   }

   MSG_HIGH_DS_3(MM_SUB, "=MM= NW Op mode %d, service_type %d, rac %d", mm_serving_plmn.network_op_mode,
            mm_serving_plmn.info.plmn_service_capability, mm_serving_plmn.rac);

   MSG_HIGH_DS_3(MM_SUB, "=MM= Selected PLMN %X %X %X", mm_serving_plmn.info.plmn.identity[0],
      mm_serving_plmn.info.plmn.identity[1], mm_serving_plmn.info.plmn.identity[2]);

   MSG_HIGH_DS_2(MM_SUB, "=MM= Selected LAC %X %X", mm_serving_plmn.lac.lac[0], mm_serving_plmn.lac.lac[1]);

}


/*===========================================================================

FUNCTION MMSEND_NAS_UE_DYNAMIC_ID_LOG_PACKET

DESCRIPTION
  This function sends NAS UR dynamic id log packet to DIAG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mmsend_nas_ue_dynamic_id_log_packet(uint8 id_type)
{
   LOG_UMTS_NAS_UE_DYNAMIC_ID_LOG_PACKET_C_type *log_ptr;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   LOG_UMTS_DS_NAS_UE_DYNAMIC_ID_LOG_PACKET_C_type *ds_log_ptr;

   if ( mm_dual_standby_mode || mm_sglte_mode)
   {
     ds_log_ptr = (LOG_UMTS_DS_NAS_UE_DYNAMIC_ID_LOG_PACKET_C_type *)
                log_alloc(LOG_UMTS_DS_NAS_UE_DYNAMIC_ID_LOG_PACKET_C,
                          sizeof(LOG_UMTS_DS_NAS_UE_DYNAMIC_ID_LOG_PACKET_C_type));

     if(!ds_log_ptr)
     {
        /* Could not allocate memory */
        return;
     }

     ds_log_ptr->as_id = (uint8) mm_as_id;
     if(id_type == LOG_TMSI_TYPE)
     {
        ds_log_ptr->identity_type = id_type;
        memscpy((void*)ds_log_ptr->identity, LOG_TMSI_SIZE,
               mm_location_information.tmsi, TMSI_SIZE);
     }
     else if(id_type == LOG_PTMSI_TYPE)
     {
        ds_log_ptr->identity_type = id_type;
        memscpy((void*)ds_log_ptr->identity, LOG_TMSI_SIZE,
               gmm_stored_ptmsi.digit, PTMSI_SIZE);
     }

     log_commit(ds_log_ptr);

   }
   else
#endif
   {
     log_ptr = (LOG_UMTS_NAS_UE_DYNAMIC_ID_LOG_PACKET_C_type *)log_alloc(LOG_UMTS_NAS_UE_DYNAMIC_ID_LOG_PACKET_C,
        sizeof(LOG_UMTS_NAS_UE_DYNAMIC_ID_LOG_PACKET_C_type));

     if(!log_ptr)
     {
        /* Could not allocate memory */
        return;
     }

     if(id_type == LOG_TMSI_TYPE)
     {
        log_ptr->identity_type = id_type;
        memscpy((void*)log_ptr->identity, LOG_TMSI_SIZE,
               mm_location_information.tmsi, TMSI_SIZE);
     }
     else if(id_type == LOG_PTMSI_TYPE)
     {
        log_ptr->identity_type = id_type;
        memscpy((void*)log_ptr->identity,LOG_TMSI_SIZE,
               gmm_stored_ptmsi.digit, PTMSI_SIZE);
     }

     log_commit(log_ptr);

   }
}


/*===========================================================================

FUNCTION MMSEND_GMM_STATE_LOG_PACKET

DESCRIPTION
  This function sends GMM state log packet to DIAG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mmsend_gmm_state_log_packet(void)
{
   LOG_UMTS_NAS_GMM_STATE_LOG_PACKET_C_type *log_ptr;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   LOG_UMTS_DS_NAS_GMM_STATE_LOG_PACKET_C_type *ds_log_ptr;
#endif

   byte event_packet[3];
   byte as_id_size = 0;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if ( mm_dual_standby_mode || mm_sglte_mode)
   {
     event_packet[0] = (byte) mm_as_id;
     as_id_size = 1;
   }
#endif

   /* GMM State Event */
   event_packet[as_id_size+0] = ((byte)gmm_state) << 4;
   /* GMM SUBSTATE is mapped with offset 1 at QXDM */
   event_packet[as_id_size+0] |= (byte)gmm_substate;
   event_packet[as_id_size+1] =  (byte)gmm_update_status+1;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if ( mm_dual_standby_mode  || mm_sglte_mode)
   {
     event_report_payload(EVENT_DS_GMM_STATE, 3, (void *)event_packet);
   }
   else
#endif
   {
     event_report_payload(EVENT_GMM_STATE, 2, (void *)event_packet);
   }

   /* GMM State Log */
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if ( mm_dual_standby_mode  || mm_sglte_mode)
   {
     ds_log_ptr = (LOG_UMTS_DS_NAS_GMM_STATE_LOG_PACKET_C_type *)
                log_alloc(LOG_UMTS_DS_NAS_GMM_STATE_LOG_PACKET_C,
                          sizeof(LOG_UMTS_DS_NAS_GMM_STATE_LOG_PACKET_C_type));

     if(!ds_log_ptr)
     {
        /* Could not allocate memory */
        return;
     }

     ds_log_ptr->as_id = (uint8) mm_as_id;
     ds_log_ptr->gmm_state_log         = (int)gmm_state;
     ds_log_ptr->gmm_substate_log      = (int)gmm_substate;
     ds_log_ptr->gmm_update_status_log = gmm_update_status;

     log_commit(ds_log_ptr);

   }
   else
#endif
   {
     log_ptr = (LOG_UMTS_NAS_GMM_STATE_LOG_PACKET_C_type *)log_alloc(LOG_UMTS_NAS_GMM_STATE_LOG_PACKET_C,
        sizeof(LOG_UMTS_NAS_GMM_STATE_LOG_PACKET_C_type));

     if(!log_ptr)
     {
        /* Could not allocate memory */
        return;
     }

     log_ptr->gmm_state_log         = (int)gmm_state;
     log_ptr->gmm_substate_log      = (int)gmm_substate;
     log_ptr->gmm_update_status_log = gmm_update_status;

     log_commit(log_ptr);

   }
}

void mmsend_nas_reg_event_packet(nas_event_type nas_event,byte Status,byte end_reason,byte detach_type,byte detach_cause)
{

byte event_packet[6];
static byte mm_lu_last_endreason;
event_id_enum_type event_nas = EVENT_DROP_ID;


  event_packet[0] = (byte)mm_serving_plmn.info.active_rat;
  event_packet[2] = (byte)Status;

  switch(nas_event)
  {
    case PROC_NAS_LAU_EVENT:
    { 
      event_nas = EVENT_NAS_LAU;
      event_packet[1] =  (byte)mm_get_lu_cause();
      if(mm_lu_last_endreason == PROC_END_TIME_OUT && (end_reason == PROC_END_LOWER_LAYER_FAILURE|| end_reason == PROC_END_REJECTED))
      {
        mm_lu_last_endreason=end_reason;
        return;
      }
      mm_lu_last_endreason=end_reason;
      event_packet[3] =   end_reason;
      if(end_reason == PROC_END_REJECTED && Status == PROC_END)
      {
        event_packet[4] =   mm_lu_reject_info.reject_cause;
      }
      else
      {
        event_packet[4] =   0xff;
      } 
    }
    break;
    case PROC_NAS_ATTACH_EVENT:
    {
      event_nas = EVENT_NAS_ATTACH;
      event_packet[1] =   gmm_attach_type;
      event_packet[3] =   end_reason;
      if(end_reason == PROC_END_REJECTED && Status == PROC_END)
      {
       event_packet[4] =   gmm_reject_cause; 
      }
      else
      {
        event_packet[4] =   0xff;
      }
    }
    break;
    case PROC_NAS_RAU_EVENT:
    {
      event_nas = EVENT_NAS_RAU;
      event_packet[1] =   gmm_ra_update_type;
      event_packet[3] =   end_reason;
      if(end_reason == PROC_END_REJECTED && Status == PROC_END)
      {
        event_packet[4] =   gmm_reject_cause;   
      }
      else
      {
        event_packet[4] =   0xff;
      }   
    }
    break;
    case PROC_NAS_MO_DETACH_EVENT: 
    {
       event_nas = EVENT_NAS_MO_DETACH;
       event_packet[1] = gmm_detach_type;
       event_packet[3] =   end_reason;
       event_packet[4] =   0xff;
    }  
    break;
    case PROC_NAS_MT_DETACH_EVENT:
    {
        event_nas = EVENT_NAS_MT_DETACH ;
        event_packet[1] = detach_type;
        event_packet[3] = end_reason;
        event_packet[4] =   detach_cause;
    }
    break;
    case PROC_NAS_IMSI_DETACH_EVENT:
    {
        event_nas = EVENT_NAS_MO_DETACH;
        event_packet[1] = IMSI_DETACH; 
        event_packet[3] = end_reason;
        event_packet[4] =   0xff;
    }
    break;

    default:
        break;
  }

 MSG_HIGH_DS_3(MM_SUB, "=MM= NAS sent event type %d  RAT %d  %d ", (byte)nas_event,
         (byte)(mm_serving_plmn.info.active_rat),Status);


#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
       if ( mm_dual_standby_mode || mm_sglte_mode)
       {
         event_packet[5] = (byte) mm_as_id;
       }
         MSG_HIGH_DS_3(MM_SUB, "=MM= NAS sent event type %d  RAT %d  AS ID %d  ", (byte)nas_event,
         (byte)(mm_serving_plmn.info.active_rat),mm_as_id);

#endif
           
  #if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if ( mm_dual_standby_mode  || mm_sglte_mode)
   {
     event_report_payload(event_nas, 6, (void *)event_packet);
   }
   else
#endif
   {
     event_report_payload(event_nas, 5, (void *)event_packet);
   }
}


/*=======================================?====================================
FUNCTION MMSEND_TIMERY_EXPIRY_EVENT

DESCRIPTION
  This function sends mm timer expiry events to DIAG

DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS 
  None
 =========================================?==================================*/
 void mmsend_timer_expiry_event(byte timer_id)
 {
   byte event_packet[2];

#ifdef FEATURE_DUAL_SIM
   event_packet[0] = (byte) mm_as_id;
#else
   event_packet[0] = 0;
#endif
   event_packet[1] = timer_id;
 
   event_report_payload(EVENT_MM_TIMER_EXPIRY, 2, (void *)event_packet);
 }
 /*===========================================================================

FUNCTION MM_PRINT_OUTGOING_OTA_MESSAGE

DESCRIPTION
  This function prints a MSG_HIGH message for each outgoing MM OTA message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_print_outgoing_ota_message(byte PD, byte message_type)
{
   if( PD == PD_MM)
   {
      switch(message_type)
      {
         case LOCATION_UPDATING_REQUEST:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent LOCATION_UPDATING_REQUEST to RRC");
         break;

         case AUTHENTICATION_RESPONSE:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent AUTHENTICATION_RESPONSE to RRC");
         break;

         case AUTHENTICATION_FAILURE:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent AUTHENTICATION_FAILURE to RRC");
         break;

         case CM_REESTABLISHMENT_REQUEST:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent CM_REESTABLISHMENT_REQUEST to RRC");
         break;

         case CM_SERVICE_ABORT:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent CM_SERVICE_ABORT to RRC");
         break;

         case CM_SERVICE_REQUEST:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent CM_SERVICE_REQUEST to RRC");
         break;

         case IDENTITY_RESPONSE:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent IDENTITY_RESPONSE to RRC");
         break;

         case IMSI_DETACH_INDICATION:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent IMSI_DETACH_INDICATION to RRC");
         break;

         case MM_STATUS:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent MM_STATUS to RRC");
         break;

         case TMSI_REALLOCATION_COMPLETE:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent TMSI_REALLOCATION_COMPLETE to RRC");
         break;

         default:
         MSG_HIGH_DS_1(MM_SUB ,"=MM= MM sent unknown %x OTA msg to RRC", message_type);
         break;
      }
   }
   else if(PD == PD_RR)
   {
      switch(message_type)
      {
         case PAGING_RESPONSE:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent PAGING_RESPONSE to RRC");
         break;

         default:
         MSG_HIGH_DS_0(MM_SUB,"=MM= MM sent unknown OTA message to RRC");
         break;
      }
   }
}


/*===========================================================================

FUNCTION MM_IS_PLMN_AN_HPLMN

DESCRIPTION
  This function determines if the input PLMN is a HPLMN.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_is_plmn_an_hplmn
(
  sys_plmn_id_s_type plmn 
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  ,sys_modem_as_id_e_type as_id
#endif
)
{
   boolean matched = FALSE;

   sys_plmn_id_s_type mm_hplmn_from_imsi;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   mm_hplmn_from_imsi = reg_sim_per_subs_hplmn_get(as_id);
#else
   mm_hplmn_from_imsi = reg_sim_hplmn_get();
#endif

   if ((plmn.identity[0] == mm_hplmn_from_imsi.identity[0]) &&
       (plmn.identity[2] == mm_hplmn_from_imsi.identity[2]) &&
       ((plmn.identity[1] & 0xf) == (mm_hplmn_from_imsi.identity[1] & 0xf)))
   {
      /* -------------------------------------------
      ** MCC matches and first 2 digits of MNC match
      ** ------------------------------------------- */
      if ((plmn.identity[1] & 0xf0) == 0xf0)
      {
         /* ------------------------------------------------------------
         ** If 3rd MNC digit in plmn == 0xF then only 2 MNC digits exist
         ** ------------------------------------------------------------ */
         matched = TRUE;
      }
      else
      {
         /* -----------------------------------------------------------------------------
         ** If plmn has valid 3rd MNC digit, then it must match with IMSI's 3rd MNC digit
         ** to be considered as HPLMN
         ** ----------------------------------------------------------------------------- */
         if ((plmn.identity[1] & 0xf0) == (mm_hplmn_from_imsi.identity[1] & 0xf0))
         {
            matched = TRUE;
         }
      }
   }

   return matched;
}


/*===========================================================================

FUNCTION MM_PLMN_MATCH

DESCRIPTION
  This function matches two plmns for equality.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_plmn_match( sys_plmn_id_s_type bcch_plmn, sys_plmn_id_s_type req_plmn )
{
   boolean matched;

   matched = sys_plmn_match (req_plmn, bcch_plmn);

   if (!matched)
   {
       MSG_HIGH_DS_0(MM_SUB, "=MM= PLMN mistmatch");
       MSG_HIGH_DS_3(MM_SUB, "=MM= Serving PLMN is %X %X %X",
           bcch_plmn.identity[0],bcch_plmn.identity[1],bcch_plmn.identity[2]);
       MSG_HIGH_DS_3(MM_SUB, "=MM= Requested PLMN is %X %X %X",
           req_plmn.identity[0],req_plmn.identity[1],req_plmn.identity[2]);


       if (sys_eplmn_list_equivalent_plmn(req_plmn) && 
           sys_eplmn_list_equivalent_plmn(bcch_plmn))
       {
          MSG_HIGH_DS_0(MM_SUB, "=MM= Serving PLMN is in the ePLMN list");
          matched = TRUE;
       }
   }
   return matched;
}


/*===========================================================================

FUNCTION MM_RAT_CHANGE_REQUESTED

DESCRIPTION
  This function matches two RATs and checks if there is a request to change.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_rat_change_requested(sys_radio_access_tech_e_type rat)
{
  boolean rat_change_req = FALSE;

  if ((mm_serving_plmn.info.active_rat == SYS_RAT_GSM_RADIO_ACCESS) ||
      (mm_serving_plmn.info.active_rat == SYS_RAT_TDS_RADIO_ACCESS) ||
      (mm_serving_plmn.info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS))
  {
    if (mm_serving_plmn.info.active_rat != rat)
    {
      rat_change_req = TRUE;

      MSG_HIGH_DS_2( MM_SUB, "=MM= RAT Change Req %d to %d 0=GSM/1=UMTS",
                 mm_serving_plmn.info.active_rat, rat);
    }
  }

  return rat_change_req;
}


/*===========================================================================

FUNCTION MM_CONVERT_TO_REG_CAUSE

DESCRIPTION
  This function Converts the received MM Reject cause to the REG cause.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
mmr_cause_e_type mm_convert_to_reg_cause( byte mm_rej_cause)
{
  switch (mm_rej_cause)
  {
    case PLMN_NOT_ALLOWED:
      return MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED;

    case LA_NOT_ALLOWED:
#ifdef FEATURE_FORBID_LAI_UPON_REGIONAL_ROAMING_REJECT
      /* ---------------------------------------------------------
      ** This FEATURE was introduced to support a customer request
      ** for a non-spec compliant PLMN selection on receipt of
      ** reject cause LA_NOT_ALLOWED
      ** --------------------------------------------------------- */
      return MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED;
#else
      return MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
#endif

    case NATIONAL_ROAMING_NOT_ALLOWED:
      return MMR_CAUSE_FAILURE_INITIATE_PLMN_SELECTION;

    case NO_SUITABLE_CELLS_IN_LA:
      return MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED;

    case GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN:
      return MMR_CAUSE_FAILURE_PLMN_NOT_ALLOWED_FOR_GPRS_SERVICES;

#ifdef FEATURE_FEMTO_CSG
    case CSG_NOT_AUTHORIZED:
      if(mm_serving_csg_id != SYS_CSG_ID_INVALID)
      {
        return MMR_CAUSE_FAILURE_LAI_NOT_ALLOWED;
      }
      else
      {
        return MMR_CAUSE_NOT_SPECIFIED;
      }
#endif 
    default:
      return MMR_CAUSE_NOT_SPECIFIED;
  }
}


/*===========================================================================

FUNCTION MM_SERVICE_CHANGE_REQUESTED

DESCRIPTION
  This function matches two RATs and checks if there is a request to change.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_service_change_requested(sys_srv_domain_e_type req_serv)
{
  boolean service_change_req = FALSE;

  if (mm_serving_plmn.ms_op_mode != req_serv)
  {
      service_change_req = TRUE;

      MSG_HIGH_DS_2( MM_SUB, "=MM= SERV Change Req %d to %d 1/2=PS", mm_serving_plmn.ms_op_mode, req_serv);
  }

  return service_change_req;
}


/*===========================================================================

FUNCTION IS_NAS_REQ_PLMN_AN_HPLMN

DESCRIPTION
  Is this PLMN a Home PLMN? Used by RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean Is_nas_req_plmn_an_hplmn(rrc_plmn_identity_type plmn)
{
#if defined FEATURE_DUAL_SIM
  MSG_ERROR_DS( MM_SUB, "=MM= DUAL SIM: Wrong API called Is_nas_req_plmn_an_hplmn()", 0,0,0 );
  return FALSE;
#elif defined FEATURE_SGLTE
   sys_plmn_id_s_type req_plmn =
      mmumts_convert_rrc_plmn_id_to_nas_plmn_id(plmn);
  return mm_is_plmn_an_hplmn( req_plmn, mm_ps_stack_id );
#else
   sys_plmn_id_s_type req_plmn =
      mmumts_convert_rrc_plmn_id_to_nas_plmn_id(plmn);

   return mm_is_plmn_an_hplmn( req_plmn );
#endif
}


/*===========================================================================

FUNCTION IS_BCCH_PLMN_AN_HPLMN

DESCRIPTION
  Is this broadcasted PLMN, a HPLMN?. This function is used by RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean Is_bcch_plmn_an_hplmn(rrc_plmn_identity_type plmn)
{
#if defined FEATURE_DUAL_SIM

  MSG_ERROR_DS( MM_SUB, "=MM= DUAL SIM: Wrong API called Is_bcch_plmn_an_hplmn()", 0,0,0 );
  return mm_per_subs_is_bcch_plmn_an_hplmn( plmn,(sys_modem_as_id_e_type)mm_sub_id );
#elif defined FEATURE_SGLTE
   sys_plmn_id_s_type req_plmn =
      mmumts_convert_rrc_plmn_id_to_nas_plmn_id(plmn);
  return mm_is_plmn_an_hplmn( req_plmn, mm_ps_stack_id );
#else
   sys_plmn_id_s_type bcch_plmn =
      mmumts_convert_rrc_plmn_id_to_nas_plmn_id(plmn);

   return mm_is_plmn_an_hplmn( bcch_plmn );
#endif
}

#ifdef FEATURE_DUAL_SIM
boolean mm_per_subs_is_bcch_plmn_an_hplmn
(
  rrc_plmn_identity_type plmn,
  sys_modem_as_id_e_type sub_id
)
{
   sys_plmn_id_s_type bcch_plmn =
      mmumts_convert_rrc_plmn_id_to_nas_plmn_id(plmn);

   if(IS_NOT_VALID_SUB_ID(sub_id))
   {
     MSG_ERROR_DS(MM_SUB, "=MM= Invalid AS ID:%d", sub_id+1,0,0);
     return FALSE;
   }

   return mm_is_plmn_an_hplmn( bcch_plmn,sub_id );
}
#endif


/*===========================================================================

FUNCTION MM_HAS_VALID_PTMSI

DESCRIPTION
  This function determines the validity of the stored PTMSI

DEPENDENCIES
  None

RETURN VALUE
  TRUE on success
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_has_valid_ptmsi(void)
{
   dword index;

   for (index = 0; index < PTMSI_SIZE; index ++)
   {
      if (gmm_stored_ptmsi.digit[index] != 0xff)
      {
         return TRUE;
      }
   }
   return FALSE;
}



/*===========================================================================

FUNCTION MM_HAS_VALID_PTMSI_SIGNATURE

DESCRIPTION
  This function determines the validity of the stored PTMSI SIGNATURE

DEPENDENCIES
  None

RETURN VALUE
  TRUE on success
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_has_valid_ptmsi_signature(void)
{
  byte index;

  for (index=0; index < PTMSI_SIGNATURE_SIZE; index++)
  {
    if (gmm_stored_ptmsi_signature.value[index] != 0xFF)
    {
      return TRUE;
    }
  }
  return FALSE;
}


/*===========================================================================

FUNCTION MM_UPDATE_MM_SERVING_PLMN_INFO

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_update_mm_serving_plmn_info
(
  sys_sim_state_e_type  sim_state,
  sys_srv_domain_e_type reg_service_domain,
  boolean               gmm_deregistered,
  sys_srv_status_e_type service_status,
  boolean               is_gstk_event_required
)
{
  sys_srv_status_e_type prev_service_status = mm_serving_plmn.info.service_status;
  byte                  curr_lai[LAI_SIZE];

  if(mm_sim_card_mode != MMGSDI_APP_UNKNOWN)
  {
    if (sim_state == SYS_SIM_STATE_CS_INVALID)
    {
      if ((mm_serving_plmn.info.sim_state == SYS_SIM_STATE_PS_INVALID) ||
          (mm_serving_plmn.info.sim_state == SYS_SIM_STATE_CS_PS_INVALID))
      {
        mm_serving_plmn.info.sim_state = SYS_SIM_STATE_CS_PS_INVALID;
        (void) mmgsdi_session_manage_illegal_subscription(mm_sim_mmgsdi_info.session_id,
                                                          MMGSDI_SESSION_APP_IS_ILLEGAL);
      }
      else
      {
        mm_serving_plmn.info.sim_state = SYS_SIM_STATE_CS_INVALID;
      }
    }
    else
    {
      mm_serving_plmn.info.sim_state = sim_state;
    }
  }

  switch (mm_serving_plmn.ms_op_mode)
  {
    case SYS_SRV_DOMAIN_PS_ONLY:
      if (gmm_deregistered)
      {
        mm_serving_plmn.info.reg_service_domain = reg_service_domain;
        mm_serving_plmn.info.service_status     = service_status;
      }
      else
      {
        switch (reg_service_domain)
        {
          case SYS_SRV_DOMAIN_NO_SRV:
            if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) ||
                (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
            {
              mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_PS_ONLY;
            }
            else if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
                     (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_NO_SRV))
            {
              mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;
            }
            break;

          case SYS_SRV_DOMAIN_CS_ONLY:
            if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) ||
                (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
            {
              mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_CS_PS;
            }
            else if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
                     (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_NO_SRV))
            {
              mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_CS_ONLY;
            }
            break;

          case SYS_SRV_DOMAIN_CS_PS:
            mm_serving_plmn.info.reg_service_domain = reg_service_domain;
            mm_serving_plmn.info.service_status     = service_status;
            break;

          default:
            MSG_ERROR_DS( MM_SUB, "=MM= Unexpected reg_service_domain %d", reg_service_domain, 0,0 );
            break;
        }
      }
      break;

    case SYS_SRV_DOMAIN_CS_PS:
    case SYS_SRV_DOMAIN_CS_ONLY:
      if (gmm_deregistered)
      {
        mm_serving_plmn.info.reg_service_domain = reg_service_domain;
        mm_serving_plmn.info.service_status     = service_status;
      }
      else
      {
        switch (reg_service_domain)
        {
          case SYS_SRV_DOMAIN_NO_SRV:
            if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) ||
                (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
            {
              mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_PS_ONLY;
            }
            else if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
                     (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_NO_SRV))
            {
              mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;
            }
            break;

         case SYS_SRV_DOMAIN_CS_ONLY:
            if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) ||
                (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
            {
              mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_CS_PS;
            }
            else if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
                     (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_NO_SRV))
            {
              mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_CS_ONLY;
            }
            break;

         case SYS_SRV_DOMAIN_PS_ONLY:
            if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
                (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS))
            {
              mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_CS_PS;
            }
            else if ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY) ||
                     (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_NO_SRV))
            {
              mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_PS_ONLY;
            }
            break;

          case SYS_SRV_DOMAIN_CS_PS:
            mm_serving_plmn.info.reg_service_domain = reg_service_domain;
            mm_serving_plmn.info.service_status     = service_status;
            break;

          default:
            MSG_ERROR_DS( MM_SUB, "=MM= Unexpected reg_service_domain %d", reg_service_domain, 0,0 );
            break;
        }
      }

      if (gmm_deregistered)
      {
        mm_serving_plmn.info.service_status = service_status;
      }
      else if ((mm_serving_plmn.gmm_service_state == SERVICE_AVAILABLE) &&
               (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_PS_ONLY))
      {
        mm_serving_plmn.info.service_status = SYS_SRV_STATUS_SRV;
      }
      else
      {
        mm_serving_plmn.info.service_status = service_status;
      }
      break;

    default:
      MSG_ERROR_DS( MM_SUB, "=MM= Unexpected ms_op_mode %d", mm_serving_plmn.ms_op_mode, 0,0 );
      break;
  }

  if ((mm_serving_plmn.info.service_status == SYS_SRV_STATUS_SRV) &&
      (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_NO_SRV))
  {
    mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION);  
    if (((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY) ||
         (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS)) &&
        (mm_serving_plmn.mm_service_state == SERVICE_AVAILABLE) &&
        (cs_cell_access != MM_AS_ACCESS_NO_CALLS))
    {
      mm_serving_plmn.info.service_status = SYS_SRV_STATUS_LIMITED;
    }
    else
    {
      mm_serving_plmn.info.service_status = SYS_SRV_STATUS_NO_SRV;
    }
  }
  else if ((mm_serving_plmn.info.service_status     == SYS_SRV_STATUS_SRV) &&
           (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) &&
           (!mmsim_sim_valid( SYS_SRV_DOMAIN_CS_ONLY )))
  {
    mm_serving_plmn.info.reg_service_domain = SYS_SRV_DOMAIN_NO_SRV;
    mm_serving_plmn.info.service_status     = SYS_SRV_STATUS_LIMITED;
  }  
  else if ((mm_serving_plmn.info.service_status == SYS_SRV_STATUS_LIMITED) &&
           (mm_serving_plmn.mm_service_state == SERVICE_NOT_AVAILABLE) &&
           (mm_serving_plmn.gmm_service_state == SERVICE_NOT_AVAILABLE))
  {
    mm_serving_plmn.info.service_status = SYS_SRV_STATUS_NO_SRV;
  }

  if ((mm_state == MM_IDLE) && (is_gstk_event_required == TRUE))
  {
    curr_lai[0] = mm_serving_plmn.info.plmn.identity[0];
    curr_lai[1] = mm_serving_plmn.info.plmn.identity[1];
    curr_lai[2] = mm_serving_plmn.info.plmn.identity[2];

    curr_lai[3] = mm_serving_plmn.lac.lac[0];
    curr_lai[4] = mm_serving_plmn.lac.lac[1];

    if ((memcmp( mm_previous_serving_lai, curr_lai, LAI_SIZE ) != 0) ||
        (prev_service_status != mm_serving_plmn.info.service_status))
    {
#ifdef FEATURE_GSTK
      mm_send_gstk_mm_idle_info();
#endif
    }
  }
}
/*===========================================================================

FUNCTION MM_UPDATE_LAST_REG_DSAC_INFO

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_update_last_reg_dsac_info(void)
{
    mm_last_reg_dsac_information.cell_access = mm_system_information.cell_access;
    mm_last_reg_dsac_information.dsac_present = mm_system_information.dsac_present;
    mm_last_reg_dsac_information.cs_cell_access = mm_system_information.cs_cell_access;
    mm_last_reg_dsac_information.ps_cell_access = mm_system_information.ps_cell_access;
}


#ifndef FEATURE_WCDMA

/* The following functions are defined here to avoid linker errors
 * while compiling for targets other than WCDMA. Calling any of these
 * function when FEATURE_WCDMA is not defined is a FATAL error
 */
#ifndef FEATURE_SEGMENT_LOADING
rrc_cmd_type *rrc_get_mm_cmd_buf( void )
{
  MSG_FATAL_DS(MM_SUB, "=MM= rrc_get_mm_cmd_buf: Not WCDMA!", 0,0,0);
  return NULL;
}
void * rrc_malloc_for_external_cmd(size_t size)
{
  (void) size;
  MSG_FATAL_DS(MM_SUB,"rrc_malloc_for_external_cmd: Not WCDMA!", 0,0,0);
  return NULL;
}
void rrc_put_mm_cmd(rrc_cmd_type *cmd_buf)
{
   MSG_FATAL_DS(MM_SUB, "=MM= rrc_put_mm_cmd: Not WCDMA!", 0,0,0);
}
void rrc_free_cmd_buf(rrc_cmd_type *cmd_buf)
{
   MSG_FATAL_DS(MM_SUB, "=MM= rrc_free_cmd_buf: Not WCDMA!", 0,0,0);
}
#else
rrc_cmd_type *IWCDMA_rrc_get_mm_cmd_buf(interface_t* ptr,  void )
{
  MSG_FATAL_DS(MM_SUB, "=MM= rrc_get_mm_cmd_buf: Not WCDMA!", 0,0,0);
  return NULL;
}
void * IWCDMA_rrc_malloc_for_external_cmd(interface_t* ptr, size_t size)
{
  (void) size;
  ERR_FATAL("rrc_malloc_for_external_cmd: Not WCDMA!", 0,0,0);
  return NULL;
}
void IWCDMA_rrc_put_mm_cmd(interface_t* ptr, rrc_cmd_type *cmd_buf)
{
   MSG_FATAL_DS(MM_SUB, "=MM= rrc_put_mm_cmd: Not WCDMA!", 0,0,0);
}
void IWCDMA_rrc_free_cmd_buf(interface_t* ptr, rrc_cmd_type *cmd_buf)
{
   MSG_FATAL_DS(MM_SUB, "=MM= rrc_free_cmd_buf: Not WCDMA!", 0,0,0);
}
#endif
#endif

/* Sirius: specific utility functions */
boolean mm_get_periodic_lu_status(void)
{
#ifdef FEATURE_NAS_REL10
   if(mm_is_per_ms_t3212_present)
   {
     if (mm_stored_per_ms_t3212_value != 0 )
     {
          return (TRUE);
     }
   }
   else
#endif
   if( mm_system_information.T3212_timer_value != 0 )
   {
       return (TRUE);
   }
   return (FALSE);
}

boolean mm_get_gprs_attach_status(void)
{
   /* --------------------------------------------------
   ** If GMM state is any of the Registered states, GPRS
   ** Attach status is ON, else OFF.
   ** ------------------------------------------------ */
   if ( (gmm_state != GMM_NULL) &&
        (gmm_state != GMM_DEREGISTERED) &&
        (gmm_state != GMM_DEREGISTERED_INITIATED) )
   {
     return (TRUE);
   }
   else
   {
     /* GMM is in Deregisted state */
     return (FALSE);
   }
}


/*===========================================================================

FUNCTION MMSEND_RAT_CHANGE_EVENT

DESCRIPTION
  This function sends RAT change event to DIAG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mmsend_rat_change_event()
{

   byte event_packet[2];
   byte as_id_size = 0;
   sys_radio_access_tech_e_type new_rat;
   sys_radio_access_tech_e_type old_rat = SYS_RAT_NONE;

#ifdef FEATURE_LTE
  if(mm_previous_rat == mm_serving_plmn.info.active_rat)
  {
    return;
  }
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if ( mm_dual_standby_mode  || mm_sglte_mode)
   {
     event_packet[0] = (byte) mm_as_id;
     as_id_size = 1;
   }
#endif

   new_rat = mm_serving_plmn.info.active_rat;

   if ((new_rat != SYS_RAT_UMTS_RADIO_ACCESS) &&
       (new_rat != SYS_RAT_GSM_RADIO_ACCESS) &&
       (new_rat != SYS_RAT_TDS_RADIO_ACCESS) &&
       (new_rat != SYS_RAT_LTE_RADIO_ACCESS))
   {
      return;
   }

#ifdef FEATURE_LTE
  old_rat = mm_previous_rat;
#else
   if ((new_rat == SYS_RAT_UMTS_RADIO_ACCESS) || (new_rat == SYS_RAT_TDS_RADIO_ACCESS))
   {
      old_rat = SYS_RAT_GSM_RADIO_ACCESS;
   }
   else if (new_rat == SYS_RAT_GSM_RADIO_ACCESS)
   {
#ifdef FEATURE_RAT_PRIORITY_LIST
      if (mm_checkif_rat_pri_list_has_rat( &mm_rat_pri_list_info, SYS_RAT_TDS_RADIO_ACCESS ))
      {
        old_rat = SYS_RAT_TDS_RADIO_ACCESS;
      }
      else
      {
        old_rat = SYS_RAT_UMTS_RADIO_ACCESS;
      }
#else
      old_rat = SYS_RAT_UMTS_RADIO_ACCESS;
#endif
   }
#endif

   /* RAT change Event */
   event_packet[as_id_size+0] = (byte)new_rat;
   event_packet[as_id_size+0] |= (((byte)old_rat) << 4);

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   if ( mm_dual_standby_mode  || mm_sglte_mode)
   {
     event_report_payload(EVENT_DS_RAT_CHANGE, 2, (void *)event_packet);
   }
   else
#endif
   {
     event_report_payload(EVENT_RAT_CHANGE, 1, (void *)event_packet);
   }
  /*Set the previous rat to new rat, so that it helps in identifying 
   the RAT_CHANGE_EVENT in the next function call*/
#ifdef FEATURE_LTE
  mm_previous_rat = new_rat;
#endif
}

/*===========================================================================

FUNCTION MMSEND_REQUEST_REJECT_EVENT

DESCRIPTION
  This function sends the Request sent or Reject received along with the cause
  to DIAG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmsend_request_reject_event
(
  byte                     request,
  smgmm_event_message_type message_type,
  byte                     cause
)
{

  byte event_payload[2];

  event_payload[0] = (byte) message_type;
  event_payload[1] = cause;

  if(request == TRUE)
  {
     event_report_payload(EVENT_SMGMM_REQUEST_SENT,2,(void*)event_payload);
  }
  else
  {
     event_report_payload(EVENT_SMGMM_REJECT_RECEIVED,2,(void*)event_payload);
  }

}


/*===========================================================================

FUNCTION MM_PARSE_EMERGENCY_NUMBER

DESCRIPTION
  This function parses emergency numbers from EMERGENCY Number List IE and stores
  it in emergency_number_list.

DEPENDENCIES
  None

RETURN VALUE
  emergency_number_list (through pointer) containing list of emergency numbers

SIDE EFFECTS
  None

===========================================================================*/
void mm_parse_emergency_number (
           mmr_emergency_num_list_s_type* emergency_number_list,
           byte*                          emergency_number_list_ie)
{
   byte *temp = emergency_number_list_ie;
   byte list_length = *temp++;
   byte count = 0;
   byte *emergency_list_ie_end = 0;
   mmr_emergency_num_s_type *emergency_number;
   memset(emergency_number_list,0x00,sizeof(mmr_emergency_num_list_s_type));

   /*************************************************************************
     * TS 24.008, section 10.5.3.13 Emergency Number List
     * The Emergency Number List IE is a type 4 information element with a minimum length of 5 octets 
     * and a maximum length of 50 octets. 
     **************************************************************************/
   if(list_length > (EMERGENCY_NUM_LIST_IE_LENGTH-2))
   {
     MSG_ERROR_DS( MM_SUB, "=MM= Emergency Number list len %d reduced to max allowed %d ", list_length,
                  (EMERGENCY_NUM_LIST_IE_LENGTH-2),0);
     list_length = EMERGENCY_NUM_LIST_IE_LENGTH-2;
   }

   emergency_list_ie_end = temp + list_length;

   while (temp < emergency_list_ie_end)
   {
       emergency_number = &(emergency_number_list->emergency_number[count]);
       emergency_number->length = *temp++;

       /*************************************************************************
       *TS 24.007, section 11.4.2
       * -- It is not a syntactical error that a type 4 standard IE specifies in 
       * its length indicator a greater length than possible according to the 
       * value part specification: extra bits shall be ignored.
       *
       *TS 24.007, section 11.2.1.1.4
       *A type 4 standard information element has format LV or TLV. Its LI precedes 
       *the value part, which consists of zero, one, or more octets; if present, 
       *its IEI has one octet length and precedes the LI. 
       *
       *Hence, this is a type 4 information element.
       **************************************************************************/ 
       if ((temp+MIN(emergency_number->length,(MMR_EMERGENCY_BCD_NUM_LENGTH+1))) > emergency_list_ie_end)
       {
         MSG_ERROR_DS(MM_SUB,"Emergency Number #%d is out of IE boundary", count+1,0,0);
         emergency_number->length = 0; /* Discard this number only */
         break;
       }
       if(emergency_number->length < 2)
       {
         MSG_ERROR_DS(MM_SUB,"Emergency Number length %d is less than spec defined length 2", 
             emergency_number->length,0,0);
         count = 0; /* Discard the complete list */
         break;
       }

       emergency_number->type = (*temp++) & (0x1F);

       /*************************************************************************
       *TS 24.007, section 11.4.2
       * -- It is not a syntactical error that a type 4 standard IE specifies in 
       * its length indicator a greater length than possible according to the 
       * value part specification: extra bits shall be ignored.
       *
       *TS 24.007, section 11.2.1.1.4
       *A type 4 standard information element has format LV or TLV. Its LI precedes 
       *the value part, which consists of zero, one, or more octets; if present, 
       *its IEI has one octet length and precedes the LI. 
       *
       *Hence, this is a type 4 information element.
       **************************************************************************/ 
       if((emergency_number->length - 1) > MMR_EMERGENCY_BCD_NUM_LENGTH)
       {
         MSG_ERROR_DS(MM_SUB,"Emergency Number length is more than spec defined length %d", 
             MMR_EMERGENCY_BCD_NUM_LENGTH,0,0);
         temp+= emergency_number->length - 1;
         emergency_number->length = 0; /* Discard this number only */
         continue;
       }
       memscpy(emergency_number->number, MMR_EMERGENCY_BCD_NUM_LENGTH, temp, emergency_number->length - 1);
       temp+= emergency_number->length - 1;
       count++;
   }

   emergency_number_list->emergency_number_count = count;
}

/*===========================================================================

FUNCTION MM_NULLIFY_MMR_EMERGENCY_NUM_LIST_IF_NEEDED

DESCRIPTION
  This function sends null emergency list to REG if PLMN country code has changed
  and new PLMN is not an Invalid PLMN, Invalid LAI.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/


void mm_nullify_mmr_emergency_num_list_if_needed
(
   byte previous_plmn[],
   byte new_plmn[]
)
{
   byte invalid_plmn[PLMN_SIZE] = { 0xff, 0xff, 0xff};
    
   if ( (is_plmn_country_changed(previous_plmn, new_plmn)) &&       
        (memcmp(previous_plmn, invalid_plmn, PLMN_SIZE)) &&
        (memcmp(new_plmn, invalid_plmn, PLMN_SIZE)) )
   {
     mmr_emergency_num_list_s_type emergency_number_list;
     memset(&emergency_number_list, 0, sizeof(mmr_emergency_num_list_s_type));
     mm_send_mmr_emergency_num_list_ind(&emergency_number_list);
   }
}


/*===========================================================================

FUNCTION IS_PLMN_COUNTRY_CHANGED

DESCRIPTION
  This function returns TRUE if country code of the two PLMN Ids passed are
  different, FALSE otherwise.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean is_plmn_country_changed(byte prev_plmn[], byte curr_plmn[])
{
   byte prev_country_digit_1 = prev_plmn[0] & 0x0F;
   byte prev_country_digit_2 = (prev_plmn[0] & 0xF0) >> 4;
   byte prev_country_digit_3 = prev_plmn[1] & 0x0F;   
   byte curr_country_digit_1 = curr_plmn[0] & 0x0F;
   byte curr_country_digit_2 = (curr_plmn[0] & 0xF0) >> 4;
   byte curr_country_digit_3 = curr_plmn[1] & 0x0F;
 
/*24.008: 4.4.4.6
The emergency number(s) received in the Emergency Number List IE are valid only in networks with in the same country 
as in the cell on which this IE is received. If no list is contained in the LOCATION UPDATING ACCEPT message, 
then the stored list in the mobile equipment shall be kept, except if the mobile equipment has successfully registered to a PLMN 
within a country different from that of the PLMN that sent the list.*/ 

#ifdef FEATURE_NAS_REL11
   sys_mcc_type     prev_mcc = 100*prev_country_digit_1 + 10*prev_country_digit_2 + prev_country_digit_3;
   sys_mcc_type     curr_mcc = 100*curr_country_digit_1 + 10*curr_country_digit_2 + curr_country_digit_3;

   if((prev_mcc==curr_mcc)
         ||((prev_mcc >= 310) && (prev_mcc <= 316) && (curr_mcc >= 310) && (curr_mcc <= 316))
         || ((prev_mcc >= 404) && (prev_mcc <= 406) && (curr_mcc >= 404) && (curr_mcc <= 406)))
#else
   if ( (prev_country_digit_1 == curr_country_digit_1) &&
        (prev_country_digit_2 == curr_country_digit_2) &&
        (prev_country_digit_3 == curr_country_digit_3) )
#endif        
   {
       return FALSE;
   }     
   
   else
   {  
       return TRUE;
   }
}

/*===========================================================================

FUNCTION MM_UPDATE_MMR_EMERGENCY_NUM_LIST_IF_NEEDED

DESCRIPTION
  This function sends emergency list to REG if it contains one or more emergency
  number.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/


void mm_update_mmr_emergency_num_list_if_needed(
             mmr_emergency_num_list_s_type* emergency_number_list)
{
    if ( emergency_number_list->emergency_number_count > 0 )
    {
        mm_send_mmr_emergency_num_list_ind(emergency_number_list);
    }
}

/*===========================================================================

FUNCTION mm_get_mmcc_rej_cause

DESCRIPTION
  This function returns cause type and value 
  for MMCC_REES_REJ.

DEPENDENCIES
  None

RETURN VALUE
  cause type and value.

SIDE EFFECTS
  None

===========================================================================*/

void mm_get_mmcc_rej_cause(mm_event_T event, reject_type_enum_T * cause_type, reject_cause_T  *cause_value)
{
   switch(event)
    {
      case MM_STD_SERVICE_REJECT:
      case MM_STD_SERVICE_REJECT_CAUSE_4_6:
               *cause_type=OTA_REJECT_CAUSE; 
               *cause_value=OTHER_CAUSE;
               break;
               
      case MM_STD_RR_CONNECTION_RELEASED:
      case MM_STD_RANDOM_ACCESS_FAILURE:
      case MM_STD_LOW_LEVEL_FAIL:
      case MM_STD_LOW_LEVEL_FAIL_REDIAL_NOT_ALLOWED:       
#ifdef FEATURE_DUAL_SIM
      case MM_STD_RADIO_UNAVAILABLE:
#endif
               *cause_type=AS_REJECT_CAUSE; 
               *cause_value=AS_REJ_LOW_LEVEL_FAIL;       
               break;
               
#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
      case MM_STD_LCS_IN_PROGRESS:
               *cause_type=OTA_REJECT_CAUSE; 
               *cause_value=OTHER_CAUSE;       
               break;  
#endif
               
      default:         
               MSG_ERROR_DS( MM_SUB, "=MM= Unexpected event for resestblishment rej, event= %d",event, 0,0 );
               break;
    }
}
/*===========================================================================

FUNCTION MM_GET_CS_CELL_ACCESS

DESCRIPTION
  This function returns cell access status for CS Domain.

  Call this fundtion with the below values as orguments.

  For CS registration :PROCEDURE_CS_REGISTRATION
  For Cs paging response : PROCEDURE_CS_PAGING_RSP
  For any other purpose : PROCEDURE_NONE


DEPENDENCIES
  None

RETURN VALUE
  Applied Access Class Restriction for CS Domain in current cell.

SIDE EFFECTS
  None

===========================================================================*/

mm_as_cell_access_status_T mm_get_cs_cell_access
(
  nas_procedure_for_cell_access_T nas_procedure
)
{
  mm_as_cell_access_status_T cs_cell_access = MM_AS_ACCESS_ALL_CALLS;

  
  switch (nas_procedure)
  {
    case PROCEDURE_CS_REGISTRATION:
       if((mm_system_information.ppac_present == TRUE) && is_wcdma_mode())
       {
          MSG_HIGH_DS_1(MM_SUB ,"PPAC says CS registration allowed is %d ", 
                 mm_system_information.cs_reg_allowed);
          if (mm_system_information.cs_reg_allowed == FALSE)
          {
           cs_cell_access = MM_AS_ACCESS_EMERGENCY_ONLY;
          }
       }
#ifdef FEATURE_DSAC
       else if ((mm_system_information.dsac_present == TRUE) && is_umts_mode())
       {
         cs_cell_access = mm_system_information.cs_cell_access;
       }
       else
       {
         cs_cell_access = mm_system_information.cell_access;
       }
#else
       else
       {
         cs_cell_access = mm_system_information.cell_access;
       }
#endif 
       break;
    case PROCEDURE_CS_PAGING_RSP:
       if ((mm_system_information.ppac_present == TRUE) && is_wcdma_mode())
       {
          if ((mm_system_information.page_rsp_allowed == MM_AS_CS_PS_ALLOWED) ||
              (mm_system_information.page_rsp_allowed == MM_AS_CS_ALLOWED))
          {
            MSG_HIGH_DS_1(MM_SUB ,"PPAC allows cs paging response %d ", 
                   mm_system_information.page_rsp_allowed);
          }
#ifdef FEATURE_DSAC
          else if ((mm_system_information.dsac_present == TRUE) && is_umts_mode())
          {
             cs_cell_access = mm_system_information.cs_cell_access;
          }
          else
          {
             cs_cell_access = mm_system_information.cell_access;
          }
#else
          else
          {
             cs_cell_access = mm_system_information.cell_access;
          }
#endif
       }
#ifdef FEATURE_DSAC
       else if ((mm_system_information.dsac_present == TRUE) && is_umts_mode())
       {
         cs_cell_access = mm_system_information.cs_cell_access;
       }
       else
       {
         cs_cell_access = mm_system_information.cell_access;
       }
#else
       else
       {
         cs_cell_access = mm_system_information.cell_access;
       }
#endif
       break;
    default:
#ifdef FEATURE_DSAC
       if ((mm_system_information.dsac_present == TRUE) && is_umts_mode())
       {
         cs_cell_access = mm_system_information.cs_cell_access;
       }
       else
       {
         cs_cell_access = mm_system_information.cell_access;
       }
#else
       {
         cs_cell_access = mm_system_information.cell_access;
       }
#endif
       break;
  }
  return cs_cell_access;
}
/*===========================================================================

FUNCTION MM_GET_LAST_REG_CS_CELL_ACCESS

DESCRIPTION
  This function returns cell access status for CS Domain of last registartion.

DEPENDENCIES
  None

RETURN VALUE
  Applied Access Class Restriction for CS Domain in current cell.

SIDE EFFECTS
  None

===========================================================================*/

mm_as_cell_access_status_T mm_get_last_reg_cs_cell_access(void)
{
  mm_as_cell_access_status_T cs_cell_access = MM_AS_ACCESS_ALL_CALLS;

#ifdef FEATURE_DSAC

 

  if (mm_last_reg_dsac_information.dsac_present == TRUE && is_umts_mode())
  {
    cs_cell_access = mm_last_reg_dsac_information.cs_cell_access;
  }
  else
  {
    cs_cell_access = mm_last_reg_dsac_information.cell_access;
  }
#else
  cs_cell_access = mm_last_reg_dsac_information.cell_access;
#endif
  return cs_cell_access;
}
/*===========================================================================

FUNCTION MM_CHECK_IF_CS_CELL_ACCESS_BARRED

DESCRIPTION
       This function checks if CS Access is allowed in the current cell.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if
    (a) Network has barred(*) CS Access with Domain Specific Access Restrictions. OR
    (b) Network has barred UE from accessing cell with Access Class Restrictions.
  FALSE otherwise.
(*) Barring means access restrictions set to Emergency or No Calls.

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_check_if_cs_cell_access_barred(mm_as_cell_access_status_T cs_cell_access)
{
  if ( (cs_cell_access == MM_AS_ACCESS_NO_CALLS) ||
       (cs_cell_access == MM_AS_ACCESS_EMERGENCY_ONLY))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION MM_CHECK_IF_CS_CELL_ACCESS_ALLOWED

DESCRIPTION
       This function checks if CS Access is allowed in the current cell.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if
    (a) Network has not barred(*) CS Access with Domain Specific Access Restrictions. OR
    (b) Network has not barred UE from accessing cell with Access Class Restrictions.
  FALSE otherwise.
(*) Barring means access restrictions set to Emergency or No Calls.

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_check_if_cs_cell_access_allowed(mm_as_cell_access_status_T cs_cell_access)
{
  if ( (cs_cell_access == MM_AS_ACCESS_ALL_CALLS) ||
       (cs_cell_access == MM_AS_ACCESS_NORMAL_ONLY))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION MM_GET_CS_CELL_ACCESS_FROM_RRC_SERVICE_INFO

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  cs_cell_access

SIDE EFFECTS
  None

===========================================================================*/
mm_as_cell_access_status_T mm_get_cs_cell_access_from_rrc_service_info
(
  rrc_service_info_type* service_info
)
{
  mm_as_cell_access_status_T cs_cell_access = MM_AS_ACCESS_ALL_CALLS;

#ifdef FEATURE_DSAC
  MSG_HIGH_DS_3(MM_SUB, "=MM= DSAC_PRESENT: %d CS_CELL_ACCESS: %d CELL_ACCESS: %d",
               service_info->dsac_present,
               service_info->cs_cell_access,
               service_info->cell_access);

  if (service_info->dsac_present == TRUE)
  {
    cs_cell_access = service_info->cs_cell_access;
  }
  else if (service_info->dsac_present == FALSE)
  {
    cs_cell_access = service_info->cell_access;
  }
#else
  cs_cell_access = service_info->cell_access;
#endif
  return cs_cell_access;
}
mm_as_page_rsp_e_type  mm_get_page_resp_info_from_rrc_service_info
(
   rrc_service_info_type* service_info
)
{
  mm_as_page_rsp_e_type page_resp = MM_AS_NONE;

  MSG_HIGH_DS_3(MM_SUB, "=MM= PPAC_PRESENT: %d CS_REG_ALLOWED: %d PS_REG_ALLOWED: %d",
               service_info->ppac_present,
               service_info->cs_reg_allowed,
               service_info->ps_reg_allowed);
  MSG_HIGH_DS_1(MM_SUB ,"=MM= PAGING_ALLOWED: %d ",
               service_info->page_rsp_allowed);
  
  if( service_info->ppac_present)
  {
   page_resp = service_info->page_rsp_allowed;
  }
  return page_resp;
}
/*===========================================================================

FUNCTION MM_GET_PS_CELL_ACCESS_FROM_RRC_SERVICE_INFO

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  ps_cell_access

SIDE EFFECTS
  None

===========================================================================*/
mm_as_cell_access_status_T mm_get_ps_cell_access_from_rrc_service_info
(
  rrc_service_info_type* service_info
)
{
  mm_as_cell_access_status_T ps_cell_access = MM_AS_ACCESS_ALL_CALLS;

#ifdef FEATURE_DSAC
  MSG_HIGH_DS_3(MM_SUB, "=MM= DSAC_PRESENT: %d PS_CELL_ACCESS: %d CELL_ACCESS: %d",
               service_info->dsac_present,
               service_info->ps_cell_access,
               service_info->cell_access);

  if (service_info->dsac_present == TRUE)
  {
    ps_cell_access = service_info->ps_cell_access;
  }
  else if (service_info->dsac_present == FALSE)
  {
    ps_cell_access = service_info->cell_access;
  }
#else
  ps_cell_access = service_info->cell_access;
#endif
  return ps_cell_access;
}



#ifdef FEATURE_DSAC

/*===========================================================================

FUNCTION MM_INVALIDATE_DSAC_INFO

DESCRIPTION
  This function invalidates DSAC information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_invalidate_dsac_info(void)
{
  mm_system_information.dsac_present = FALSE;
  mm_system_information.cs_cell_access = MM_AS_ACCESS_ALL_CALLS;
  mm_system_information.ps_cell_access = MM_AS_ACCESS_ALL_CALLS;
  mm_last_reg_dsac_information.dsac_present = FALSE;
  mm_last_reg_dsac_information.cs_cell_access = MM_AS_ACCESS_ALL_CALLS;
  mm_last_reg_dsac_information.ps_cell_access = MM_AS_ACCESS_ALL_CALLS;
  mm_last_procedure_to_update_registration = MM_NONE;
  mm_ps_access_barred_to_unbarred = FALSE;
  mm_cs_access_barred_to_unbarred = FALSE;
}
/*===========================================================================

FUNCTION MM_INVALIDATE_DSAC_INFO

DESCRIPTION
  This function invalidates PPAC information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_invalidate_ppac_info(void)
{
  mm_system_information.ppac_present = FALSE;
  mm_system_information.page_rsp_allowed = MM_AS_CS_PS_ALLOWED;;
  mm_system_information.cs_reg_allowed = FALSE;
  mm_system_information.ps_reg_allowed = FALSE;
}
/*===========================================================================

FUNCTION MM_UPDATE_CELL_ACCESS_INFO_FROM_RRC

DESCRIPTION
  This function copies Cell Access information from RRC Message to Global
  Variables used by MM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_update_cell_access_info_from_rrc(rrc_service_info_type* rrc_service_info)
{
  mm_system_information.cell_access = rrc_service_info->cell_access;
  mm_system_information.dsac_present = rrc_service_info->dsac_present;
  mm_system_information.cs_cell_access = rrc_service_info->cs_cell_access;
  mm_system_information.ps_cell_access = rrc_service_info->ps_cell_access;
}


/*===========================================================================

FUNCTION MM_UPDATE_PROCESS_DSAC_INFO_FROM_RRC_SERVICE_CNF

DESCRIPTION
  This function processes Domain Specific Cell Access information from RRC
  SERVICE CNF.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_process_dsac_info_from_rrc_service_cnf(rrc_service_info_type* rrc_service_info)
{
  mm_process_dsac_info_from_rrc_service_ind(rrc_service_info);
}

/*===========================================================================

FUNCTION MM_UPDATE_PROCESS_DSAC_INFO_FROM_RRC

DESCRIPTION
  This function processes Domain Specific Cell Access information from RRC Message
  RRC_SERVICE_IND/CNF/IND_FROM_OTA and updates NMO if needed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_process_dsac_info_from_rrc(rrc_service_info_type* rrc_service_info)
{
  mm_update_cell_access_info_from_rrc(rrc_service_info);
}


/*===========================================================================

FUNCTION MM_CHECK_IF_EITHER_CS_OR_PS_CELL_ACCESS_BARRED

DESCRIPTION
  This function checks if either CS or PS Domain access is barred.

DEPENDENCIES
  None

RETURN VALUE
  NMO of currently camped network

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_check_if_either_cs_or_ps_cell_access_barred()
{
  boolean return_value = FALSE;
  if(mm_system_information.dsac_present == TRUE)
  {
    if((mm_system_information.cs_cell_access == MM_AS_ACCESS_NO_CALLS) ||
       (mm_system_information.ps_cell_access == MM_AS_ACCESS_NO_CALLS))
    {
      return_value = TRUE;
    }
  }
  return return_value;
}
/*===========================================================================

FUNCTION mm_update_ppac_info_from_rrc

DESCRIPTION
  This function updated the nas ppac information with the one received from
  rrc
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_update_ppac_info_from_rrc(rrc_service_info_type* rrc_service_info)
{
  mm_system_information.ppac_present = rrc_service_info->ppac_present;
  mm_system_information.page_rsp_allowed = rrc_service_info->page_rsp_allowed;
  mm_system_information.cs_reg_allowed =rrc_service_info->cs_reg_allowed;
  mm_system_information.ps_reg_allowed =rrc_service_info->ps_reg_allowed;
  MSG_HIGH_DS_4(MM_SUB, "=MM= ppac_present: %d page_rsp_allowed: %d cs_reg_allowed: %d  ps_reg_allowed: %d ",
                mm_system_information.ppac_present, mm_system_information.page_rsp_allowed,
                mm_system_information.cs_reg_allowed,mm_system_information.ps_reg_allowed);
}
/*===========================================================================

FUNCTION MM_UPDATE_PROCESS_DSAC_INFO_FROM_RRC_SERVICE_IND

DESCRIPTION
  This function processes Domain Specific Cell Access information from RRC
  SERVICE IND/IND FROM OTA.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_process_dsac_info_from_rrc_service_ind(rrc_service_info_type* rrc_service_info)
{
  boolean is_prev_dsac_info_valid;
  mm_as_cell_access_status_T prev_cs_cell_access, prev_ps_cell_access;

  /*Store previous DSAC Status*/
  mm_get_prev_dsac_info(&is_prev_dsac_info_valid,
                        &prev_cs_cell_access, &prev_ps_cell_access);

  mm_process_dsac_info_from_rrc(rrc_service_info);

  mm_update_ppac_info_from_rrc(rrc_service_info);

  mm_process_cs_access_change(is_prev_dsac_info_valid, prev_cs_cell_access);

  mm_process_ps_access_change(is_prev_dsac_info_valid, prev_ps_cell_access);
}

/*===========================================================================

FUNCTION MM_GET_PREV_DSAC_INFO

DESCRIPTION
  This function obtains current DSAC info and stores.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/


void mm_get_prev_dsac_info
(
  boolean                       *is_prev_dsac_info_valid,
  mm_as_cell_access_status_T    *prev_cs_cell_access,
  mm_as_cell_access_status_T    *prev_ps_cell_access
)
{
  *is_prev_dsac_info_valid = mm_system_information.dsac_present;
  *prev_cs_cell_access = mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION);
  *prev_ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION);

  MSG_HIGH_DS_3(MM_SUB, "=MM= Prev DSAC Info: %d Prev CS Access: %d Prev PS Access: %d",
    *is_prev_dsac_info_valid, *prev_cs_cell_access, *prev_ps_cell_access);
}

/*===========================================================================

FUNCTION MM_PROCESS_CS_ACCESS_CHANGE

DESCRIPTION
  This function checks if CS Domain access gets unbarred in NMO 1 network.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_process_cs_access_change
(
  boolean                    is_prev_dsac_info_valid,
  mm_as_cell_access_status_T prev_cs_cell_access
)
{
  mm_as_cell_access_status_T curr_cs_cell_access = mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION);
  mm_as_cell_access_status_T prev_reg_cs_cell_access = mm_get_last_reg_cs_cell_access();

  if ((mm_check_if_cs_cell_access_allowed(curr_cs_cell_access)) && 
      ((mm_check_if_cs_cell_access_barred(prev_cs_cell_access))|| (mm_check_if_cs_cell_access_barred(prev_reg_cs_cell_access))))
  {
    /*CS Access Barred -> Unbarred*/
    if ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
        ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) ||
         (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_ONLY)))
    {
      mm_cs_access_barred_to_unbarred = TRUE;
    }
  }
}

/*===========================================================================

FUNCTION MM_PROCESS_PS_ACCESS_CHANGE

DESCRIPTION
  This function checks if PS Domain access gets unbarred in NMO 1 network.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_process_ps_access_change
(
  boolean                    is_prev_dsac_info_valid,
  mm_as_cell_access_status_T prev_ps_cell_access
)
{
  mm_as_cell_access_status_T curr_ps_cell_access = gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION);

  if (gmm_check_if_ps_cell_access_allowed(curr_ps_cell_access) &&
      gmm_check_if_ps_cell_access_barred(prev_ps_cell_access))
  {
    /*PS Access Barred -> Unbarred*/
    if ((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) &&
        ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) ||  
         (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_PS_ONLY))) 
    {
      mm_ps_access_barred_to_unbarred = TRUE;
    } 
  }
}

#endif

sys_gw_conn_status_e_type mm_per_nas_stacks_get_connection_status
(
  sys_modem_as_id_e_type sub_id,
  sys_modem_as_id_e_type      stack_id_nas
)
{
#if !defined(FEATURE_DUAL_SIM) && !defined (FEATURE_SGLTE)
  (void)sub_id;
  (void)stack_id_nas;
  return mm_get_connection_status();
#else
  gmm_state_type gmm_state_local;
  byte           mm_state_local;  
  sys_srv_domain_e_type ms_op_mode_local;
  byte           is_gmm_registering = FALSE;
  byte           is_uplink_data_available = FALSE;
  
#ifdef FEATURE_LTE
  emm_ctrl_data_type *emm_ctrl_data_ptr = emm_db_get_ctrl_data_per_subs(sub_id);
#endif

  if(IS_NOT_VALID_STACK_ID(stack_id_nas) ||
     IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB, "=MM= Invalid STACK ID:%d or SUB ID:%D", stack_id_nas+1,sub_id+1,0);
    return SYS_GW_CONN_STATUS_IDLE;
  }

  gmm_state_local = gmm_state_sim[stack_id_nas];
  mm_state_local = mm_state_sim[stack_id_nas];
  ms_op_mode_local = mm_serving_plmn_sim[stack_id_nas].ms_op_mode;
 
#ifdef FEATURE_LTE
    if( is_lte_mode_per_nas_stacks(stack_id_nas))
    {
      emm_connection_state_type connection_state = EMM_IDLE_STATE;
      connection_state = emm_ctrl_data_ptr->emm_connection_state;
      if (!((connection_state == EMM_IDLE_STATE) || ((connection_state == EMM_RELEASING_RRC_CONNECTION_STATE) 
                                                          && (emm_ctrl_data_ptr->aborting_conn == FALSE))))
      {
        return SYS_GW_CONN_STATUS_CONN;
      }
      else if(mm_plmn_search_in_progress_sim[stack_id_nas] == TRUE)
      {
        return SYS_GW_CONN_STATUS_BPLMN_SRCH;
      }
      else
      {
        return SYS_GW_CONN_STATUS_IDLE;
      }

    }
    else
#endif
    if(is_umts_mode_per_nas_stacks(stack_id_nas) || is_gsm_mode_per_nas_stacks(stack_id_nas))
    {

      if(gmm_state_local == GMM_REGISTERED_INITIATED ||
        gmm_state_local == GMM_DEREGISTERED_INITIATED ||
        gmm_state_local == GMM_ROUTING_AREA_UPDATING_INITIATED)
      {
        is_gmm_registering = TRUE;
      }

      if(((ms_op_mode_local == SYS_SRV_DOMAIN_PS_ONLY) ||
         (ms_op_mode_local == SYS_SRV_DOMAIN_CS_PS)) && 
#ifdef FEATURE_DUAL_SIM
         sm_is_pdp_active(
#ifdef FEATURE_SGLTE
                   MM_IS_IN_SGLTE_MODE_SUB(sub_id) ? (sys_modem_as_id_e_type)mm_ps_stack_id : stack_id_nas
#else
                   stack_id_nas
#endif
                         ) &&
#else
         sm_is_pdp_active() &&
#endif
        (gmm_substate_sim[stack_id_nas] == GMM_NORMAL_SERVICE) &&
        ((is_gsm_mode_per_nas_stacks(stack_id_nas) && dsumtsps_is_ps_data_available() && (gmm_is_ps_active_sim[stack_id_nas] == TRUE))
#if defined(FEATURE_LTE)
          || (is_umts_mode_per_nas_stacks(stack_id_nas) && (rrc_connection_status_sim[stack_id_nas] != MMRRC_IDLE) && (mm_rrc_pch_state_info.is_in_pch_state != TRUE))
#endif
        ))
      {
        is_uplink_data_available = TRUE;
      }

      if( (is_gmm_registering == TRUE) ||
          (mm_sms_is_active_sim[stack_id_nas] == TRUE) ||
          ( (mm_state_local != MM_IDLE) &&
            (mm_state_local != MM_NULL) ) ||
          (is_uplink_data_available == TRUE)
#ifdef FEATURE_WRLF_SYSTEM_SEL
             || (mm_wcdma_rlf_state_sim[stack_id_nas] != WCDMA_RLF_IDLE)
#endif  
         )
    
      { 
        return SYS_GW_CONN_STATUS_CONN; 
      }
      else if(mm_plmn_search_in_progress_sim[stack_id_nas])
      {
        return SYS_GW_CONN_STATUS_BPLMN_SRCH;
      }
      else
      {
        return SYS_GW_CONN_STATUS_IDLE;
      }
    }
    else
    {
      return SYS_GW_CONN_STATUS_NONE;
    }
#endif
}

sys_gw_conn_status_e_type mm_per_stacks_get_connection_status
(
  sys_modem_as_id_e_type sub_id,
  sys_modem_stack_id_e_type stack_id
)
{
  sys_modem_as_id_e_type stack_id_nas = mm_get_nas_stack_id(sub_id,stack_id);
  return mm_per_nas_stacks_get_connection_status(sub_id,stack_id_nas);
}

sys_gw_conn_status_e_type mm_per_subs_get_connection_status
(
  sys_modem_as_id_e_type as_id
)
{
  (void)as_id;
  //temp function for compilation dependency issues which will be removed later once CM modifies the code
  return SYS_GW_CONN_STATUS_IDLE;
}




sys_gw_conn_status_e_type mm_get_connection_status
(
  void
)
{
  gmm_state_type gmm_state_local = gmm_state;
  byte           mm_state_local  = mm_state;
  byte           is_gmm_registering = FALSE;
  byte           is_uplink_data_available = FALSE;
 #ifdef FEATURE_LTE
 emm_ctrl_data_type     *emm_ctrl_data_ptr = NULL; 
  if( is_lte_mode())
  {
    emm_connection_state_type connection_state = EMM_IDLE_STATE;
    emm_ctrl_data_ptr = emm_db_get_ctrl_data();
    ASSERT( emm_ctrl_data_ptr != NULL );

    connection_state = EMM_GET_CONNECTION_STATE();
    if (!((connection_state == EMM_IDLE_STATE) || ((connection_state == EMM_RELEASING_RRC_CONNECTION_STATE) 
                                                          && (emm_ctrl_data_ptr->aborting_conn == FALSE))))
    {
      return SYS_GW_CONN_STATUS_CONN;
    }
    else if(mm_plmn_search_in_progress == TRUE)
    {
      return SYS_GW_CONN_STATUS_BPLMN_SRCH;
    }
    else
    {
      return SYS_GW_CONN_STATUS_IDLE;
    }
  }
  else
#endif
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS) 
  if ( is_umts_mode() || is_gsm_mode())
  {

    if(gmm_state_local == GMM_REGISTERED_INITIATED ||
        gmm_state_local == GMM_DEREGISTERED_INITIATED ||
        gmm_state_local == GMM_ROUTING_AREA_UPDATING_INITIATED)
    {
      is_gmm_registering = TRUE; /* GMM is either registering or deregistering, don't allow BSR */
    }

    if(
#ifdef FEATURE_DUAL_SIM     
         sm_is_pdp_active((sys_modem_as_id_e_type)mm_as_id) 
#else
         sm_is_pdp_active() 
#endif
         && (gmm_substate == GMM_NORMAL_SERVICE)
         && ( ( is_gsm_mode() && (gmm_is_ps_active == TRUE) && dsumtsps_is_ps_data_available())
#if defined(FEATURE_LTE)
             || (is_umts_mode() && (rrc_connection_status != MMRRC_IDLE) && (mm_rrc_pch_state_info.is_in_pch_state != TRUE))
#endif
       ))
    {
       /* Don't allow BSR, if PDP is active, GMM substate is Normal SERVICE,
	* and, in G, we don't have uplink data available or
	*      in W, RRC connection is active and RRC isn't in PCH state.
	*/
       is_uplink_data_available = TRUE;
    }

    /* Don't allow BSR, if gmm is registering/dereg, or WRLF,
     * or data session is active, or SMS/voice call is active/establishing/re-estab, or
     * MM is registering
     */
    if( (is_gmm_registering == TRUE) ||
        (mm_sms_is_active == TRUE) ||
        (is_uplink_data_available == TRUE) ||
        ( (mm_state_local != MM_IDLE) &&
            (mm_state_local != MM_NULL) )
#ifdef FEATURE_WRLF_SYSTEM_SEL
             || (mm_wcdma_rlf_state != WCDMA_RLF_IDLE)
#endif  
         )
    {
      return SYS_GW_CONN_STATUS_CONN; 
    }
    else if(mm_plmn_search_in_progress)
    {
      return SYS_GW_CONN_STATUS_BPLMN_SRCH;
    }
    else
    {
      return SYS_GW_CONN_STATUS_IDLE;
    }
  }
  else
#endif
  {
    return SYS_GW_CONN_STATUS_NONE;
  }
}

/*===========================================================================

FUNCTION mm_check_connection_active

DESCRIPTION
  This function checks if UE is in connection active state

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if UE is both RRC connected and either in CELL_DCH or CELL_FACH state.

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_check_connection_active
(
  sys_modem_as_id_e_type sub_id,
  sys_modem_as_id_e_type stack_id_nas
)
{
  boolean conn_active = FALSE;
  if((mm_per_nas_stacks_get_connection_status((sys_modem_as_id_e_type)sub_id,
                   (sys_modem_as_id_e_type)stack_id_nas) == SYS_GW_CONN_STATUS_CONN))
  {
    conn_active = TRUE;
  }
  
#if defined(FEATURE_LTE)
  MSG_HIGH_DS_2(MM_SUB, "=MM= mm_check_connection_active returning %d is_in_pch_state : %d",
                                           conn_active, mm_rrc_pch_state_info.is_in_pch_state);
#else
  MSG_HIGH_DS_1(MM_SUB, "=MM= mm_check_connection_active returning %d" ,
                                           conn_active);
#endif
  return conn_active;
}



/*===========================================================================

FUNCTION mm_is_call_or_registration_ongoing

DESCRIPTION
  This function checks if CS connection or GMM registration is ongoing

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if CS connection or GMM registration is ongoing, False otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_is_call_or_registration_ongoing
(
  void
)
{
  /*
  ** If MM state is not IDLE and not NULL or if GMM is in either performing RAU or Attach then
  ** return TRUE, return FALSE otherwise
  */
  if((mm_state != MM_IDLE &&
      mm_state != MM_NULL) ||
     (gmm_state == GMM_REGISTERED_INITIATED||
      gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED))
  {
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION MM_PER_NAS_STACKS_IS_MO_SMS_IN_PROGRESS

DESCRIPTION
  This function checks whether MO SMS is going on or not.
  This function is internal to NAS and uses the stack id in NAS format.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_per_nas_stacks_is_mo_sms_in_progress
(
  sys_modem_as_id_e_type stack_id_nas
)
{
#if !defined(FEATURE_DUAL_SIM) && !defined (FEATURE_SGLTE)
  (void)stack_id_nas;
  return mm_is_mo_sms_in_progress();
#else
  boolean status = FALSE;

  if (IS_NOT_VALID_STACK_ID(stack_id_nas))
  {
    MSG_ERROR_DS(MM_SUB, "=MM= mm_per_nas_stacks_is_mo_sms_in_progress - Invalid STACK ID: %d",
                                                stack_id_nas+1,0,0);
    return FALSE;
  }

  if((((mm_cnm_srv_est_cause_sim[stack_id_nas] == MM_RR_SHORT_MESSAGE) ||
       (mm_cnm_srv_est_cause_sim[stack_id_nas] == MM_RR_SUPPLEMENTARY_SERVICE)) &&
       ((mm_state_sim[stack_id_nas] == MM_WAIT_FOR_RR_CONNECTION_MM) ||
        (mm_state_sim[stack_id_nas] == MM_WAIT_FOR_OUTGOING_MM_CONNECTION) ||
        (mm_state_sim[stack_id_nas] == MM_CONNECTION_ACTIVE) ||
        (mm_state_sim[stack_id_nas] == MM_WAIT_FOR_NETWORK_COMMAND))) ||
       ((mm_sms_is_active_sim[stack_id_nas]) &&
        (gmm_state_sim[stack_id_nas] == GMM_SERVICE_REQUEST_INITIATED ||
         gmm_state_sim[stack_id_nas] == GMM_REGISTERED)))
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM=MO SMS IN PROGRESS");
    status = TRUE;
  }

  return status;
#endif
}


/*===========================================================================

FUNCTION MM_PER_STACKS_IS_MO_SMS_IN_PROGRESS

DESCRIPTION
  This function checks whether MO SMS is going on or not.
  This function is called by CM with stack ID and SUB ID in CM <-> NAS interface format.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_per_stacks_is_mo_sms_in_progress
(
  sys_modem_as_id_e_type sub_id,
  sys_modem_stack_id_e_type stack_id
)
{

  sys_modem_as_id_e_type stack_id_nas = mm_get_nas_stack_id(sub_id,stack_id);
  return mm_per_nas_stacks_is_mo_sms_in_progress(stack_id_nas);
}


/*===========================================================================

FUNCTION MM_IS_MO_SMS_IN_PROGRESS

DESCRIPTION
  This function checks whether MO SMS is going on or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_per_subs_is_mo_sms_in_progress
(
  sys_modem_as_id_e_type as_id
)
{
  (void)as_id;
  //temp function for compilation dependency issues which will be removed later once CM modifies the code
  return FALSE;
}

boolean mm_is_mo_sms_in_progress(void)
{

   boolean status = FALSE;

   if((((mm_cnm_srv_est_cause == MM_RR_SHORT_MESSAGE) ||
        (mm_cnm_srv_est_cause == MM_RR_SUPPLEMENTARY_SERVICE)) &&
       ((mm_state == MM_WAIT_FOR_RR_CONNECTION_MM) ||
        (mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION) ||
        (mm_state == MM_CONNECTION_ACTIVE) ||
        (mm_state == MM_WAIT_FOR_NETWORK_COMMAND))) ||
      ((mm_sms_is_active) &&
       (gmm_state == GMM_SERVICE_REQUEST_INITIATED ||
        gmm_state == GMM_REGISTERED)))
   {
     MSG_HIGH_DS_0(MM_SUB,"=MM=MO SMS IN PROGRESS");
     status = TRUE;
   }

   return status;
}

/*===========================================================================

FUNCTION MM_CONVERT_GMM_TO_SM_CAUSE

DESCRIPTION
  This function converts GMM cause codes to SM cause codes

DEPENDENCIES
  None

RETURN VALUE
  sm cause code.

SIDE EFFECTS
  None

===========================================================================*/
sm_cm_status_T mm_convert_gmm_to_sm_cause(byte gmm_cause)
{
  sm_cm_status_T sm_cause;

  switch(gmm_cause)
  {
     case ILLEGAL_MS:
        sm_cause = SM_ILLEGAL_MS;
        break;
    case ILLEGAL_ME:
        sm_cause = SM_ILLEGAL_ME;
        break;
    case GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED:
        sm_cause = SM_GPRS_SERVICES_AND_NON_GPRS_SERVICES_NOT_ALLOWED;
        break;
    case GPRS_SERVICES_NOT_ALLOWED:
        sm_cause = SM_GPRS_SERVICES_NOT_ALLOWED;
        break;  
    case MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK:
        sm_cause = SM_MS_IDENTITY_CANNOT_BE_DERIVED_BY_THE_NETWORK;
        break;
    case IMPLICITLY_DETACHED:
        sm_cause = SM_IMPLICITLY_DETACHED;
        break;
    case PLMN_NOT_ALLOWED:
        sm_cause = SM_PLMN_NOT_ALLOWED;
        break;
    case LA_NOT_ALLOWED:
        sm_cause = SM_LA_NOT_ALLOWED;
        break;
    case GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN:
        sm_cause = SM_GPRS_SERVICES_NOT_ALLOWED_IN_THIS_PLMN;
        break; 
    case CONGESTTION:
        sm_cause = SM_CONGESTION;
        break;
    case NO_PDP_CONTEXT_ACTIVATED:
        sm_cause = SM_NO_PDP_CONTEXT_ACTIVATED;
        break;
    case ACCESS_CLASS_DSAC_REJECTION:        
        sm_cause = SM_ACCESS_CLASS_DSAC_REJECTION;
        break;
    case NATIONAL_ROAMING_NOT_ALLOWED:
        sm_cause = SM_NATIONAL_ROAMING_NOT_ALLOWED;
        break;
    case NO_SUITABLE_CELLS_IN_LA:
        sm_cause = SM_NO_SUITABLE_CELLS_IN_LA;
        break;
    case ABORT_SERVICE_NOT_AVAILABLE:
        sm_cause = SM_ABORT_SERVICE_NOT_AVAILABLE;
        break;
    default:
        sm_cause = PPD_UNKNOWN_REASON;        
   }

  return sm_cause;
}

/*===========================================================================

FUNCTION MM_CONVERT_GMM_TO_SM_CAUSE

DESCRIPTION
  This function converts GMM cause codes to SM cause codes

DEPENDENCIES
  None

RETURN VALUE
  sm cause code.

SIDE EFFECTS
  None

===========================================================================*/
gmmsm_fast_dormancy_cnf_cause_e_type mm_convert_rrc_to_gmmsm_cause(rrc_fast_dormancy_cnf_status_e_type rrc_cause)
{
  gmmsm_fast_dormancy_cnf_cause_e_type gmmsm_cause;

  switch(rrc_cause)
  {
     case RRC_FAST_DORMANCY_SUCCESS_APP_NO_RETRY:
        gmmsm_cause = GMMSM_FAST_DORMANCY_SUCCESS_APP_NO_RETRY;
        break;
    case RRC_FAST_DORMANCY_SUCCESS_APP_RETRY:
        gmmsm_cause = GMMSM_FAST_DORMANCY_SUCCESS_APP_RETRY;
        break;
    case RRC_FAST_DORMANCY_FAILURE_APP_RETRY:
        gmmsm_cause = GMMSM_FAST_DORMANCY_FAILURE_APP_RETRY;
        break;
    default:
        gmmsm_cause = GMMSM_FAST_DORMANCY_FAILURE_APP_RETRY;        
   }

  return gmmsm_cause;
}

/*===========================================================================

FUNCTION MM_GET_CELL_SERVICE_CAPABILTIES

DESCRIPTION
  This function is used by REG to query cell capabilties.
  
DEPENDENCIES
  None

RETURN VALUE
  mm cell service capabilities

SIDE EFFECTS
  None

===========================================================================*/
void  mm_get_cell_service_capabilties
(
  mmr_cell_service_ind_s_type *mmr_cell_capability
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  ,sys_modem_as_id_e_type as_id
#endif
)
{

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  if (IS_NOT_VALID_STACK_ID(as_id))
  {
    MSG_ERROR_DS(MM_SUB,"mm_get_cell_service_capabilties - Invalid AS ID: %d", as_id+1,0,0);
    return;
  }
#endif

  #ifdef FEATURE_GSM_DTM
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  if(mm_serving_plmn_sim[as_id].dtm_supported)
#else
  if(mm_serving_plmn.dtm_supported)
#endif
  {
    mmr_cell_capability->dtm_support = SYS_DTM_SUPPORT_AVAIL;
  }
  else
  #endif
  {
    mmr_cell_capability->dtm_support = SYS_DTM_SUPPORT_NOT_AVAIL;
  }

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  if(mm_serving_plmn_sim[as_id].egprs_supported)
#else
  if(mm_serving_plmn.egprs_supported)
#endif
  {
    mmr_cell_capability->egprs_support = SYS_EGPRS_SUPPORT_AVAIL;
  }  
  else
  {
    mmr_cell_capability->egprs_support = SYS_EGPRS_SUPPORT_NOT_AVAIL;    
  }

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  mmr_cell_capability->hsdpa_hsupa_support = mm_serving_plmn_sim[as_id].hsdpa_hsupa_ind;
#else
  mmr_cell_capability->hsdpa_hsupa_support = mm_serving_plmn.hsdpa_hsupa_ind;
#endif

  return;
}

/*===========================================================================

FUNCTION mm_get_nas_stack_id

DESCRIPTION
  This function dervies the NAS internal stack id
  based on sub_id and stack_id passed by CM
  
DEPENDENCIES
  None

RETURN VALUE
  mm_get_nas_stack_id

SIDE EFFECTS
  None

===========================================================================*/
sys_modem_as_id_e_type mm_get_nas_stack_id
(
  sys_modem_as_id_e_type sub_id,
  sys_modem_stack_id_e_type stack_id
)
{
#ifdef FEATURE_LTE
  sys_modem_as_id_e_type stack_id_nas = sub_id;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  if(IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_FATAL_DS(MM_SUB, "=MM= INVALID SUB_ID : %d used for an API call",sub_id+1,0,0);
  }
  if(MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    if(stack_id == SYS_MODEM_STACK_ID_1)
    {
      stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
    }
    else
    {
      stack_id_nas = (sys_modem_as_id_e_type)mm_cs_stack_id;
    }
  }
#endif
  return stack_id_nas;
#else
  (void) stack_id;
  return sub_id;
#endif
}

/*===========================================================================

FUNCTION MM_GET_SUB_CAPABILITY

DESCRIPTION
  This function returns the sub capability
  
DEPENDENCIES
  None

RETURN VALUE
  subs_capability_e_type

SIDE EFFECTS
  None

===========================================================================*/
subs_capability_e_type mm_get_sub_capability(sys_modem_as_id_e_type sub_id)
{
  subs_capability_e_type sub_cap = SUBS_CAPABILITY_MULTIMODE;
#ifdef FEATURE_DUAL_SIM
  if(!MM_SUB_IS_SGLTE_SUB(sub_id))
  {
    sub_cap = mm_sub_capability_sim[sub_id];
  }
#endif
  return sub_cap;
}


/*===========================================================================

FUNCTION MM_GET_CELL_SERVICE_CAPABILTIES

DESCRIPTION
  This function Sets the signal to CM.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_set_signal_for_cm(uint16 bit_mask)
{
 
  if (!mm_client_activated_group)
  {
    MSG_MED_DS(MM_SUB, "=MM= Client is not interested in any NAS info",0,0,0);
  }
  else
  {
    bit_mask = bit_mask & 0x003F; 

    if (mm_client_activated_group & bit_mask)
    { 
      if (cm_per_subs_stacks_stats_set_modem_info_signal(SYS_MODEM_MODULE_NAS, (sys_modem_as_id_e_type)mm_sub_id, mm_set_cm_stack_id(mm_as_id)) == TRUE)
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= NAS set the signal to CM");
      }
      else
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= Not able to set the signal to CM"); 
      }
      rex_enter_crit_sect(&mm_ftd_crit_sect);      
      mm_nas_changed_group |= bit_mask;
      rex_leave_crit_sect(&mm_ftd_crit_sect);
      MSG_HIGH_DS_1(MM_SUB ,"=MM= NAS bit mask set is %d",mm_nas_changed_group);
    }
  }
}

#if !defined(FEATURE_DUAL_SIM) && !defined (FEATURE_SGLTE)

/*===========================================================================

FUNCTION MM_GET_CELL_SERVICE_CAPABILTIES

DESCRIPTION
  This function Sets NAS client activated group.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_set_nas_bit_mask(uint16 bit_mask)
{
  rex_enter_crit_sect(&mm_ftd_crit_sect);
  /* Currently 6 bits out of 16 bit mask are used for
  6 different NAS Info groups 
  So masking of the rest of the unused bits*/

  mm_client_activated_group = bit_mask & 0x003F; 

  rex_leave_crit_sect(&mm_ftd_crit_sect);
}

/*===========================================================================

FUNCTION MM_GET_CELL_SERVICE_CAPABILTIES

DESCRIPTION
  This function is called by CM to get the NAS stats info.
  
DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_get_nas_stats_info(sys_nas_information_s_type* nas_information)
{
  boolean result = FALSE;

  if (nas_information == NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= Invalid NAS information");
  }
  else
  {
    rex_enter_crit_sect(&mm_ftd_crit_sect);

    nas_information->changed_bitmask =  mm_nas_changed_group;

    if (mm_nas_changed_group & SYS_NAS_ATTACH_TYPE_MASK)
    {
      nas_information->attach_type = gmm_attach_type;
      mm_nas_changed_group &= ~SYS_NAS_ATTACH_TYPE_MASK;
    }
    if (mm_nas_changed_group & SYS_NAS_CS_REJECT_CAUSE_MASK)
    {
      nas_information->cs_reject_cause = mm_lu_reject_info.reject_cause;
      mm_nas_changed_group &= ~SYS_NAS_CS_REJECT_CAUSE_MASK;
    }
    if (mm_nas_changed_group & SYS_NAS_PS_REJECT_CAUSE_MASK)
    {
      nas_information->ps_reject_cause = gmm_reject_cause;
      mm_nas_changed_group &= ~SYS_NAS_PS_REJECT_CAUSE_MASK;
    }
    if (mm_nas_changed_group & SYS_NAS_REGISTRATION_INFO_MASK)
    {
      memscpy(nas_information->reg_nw_info.tmsi,SYS_TMSI_SIZE,mm_location_information.tmsi,TMSI_SIZE);
      memscpy(nas_information->reg_nw_info.ptmsi,SYS_PTMSI_SIZE,gmm_stored_ptmsi.digit,PTMSI_SIZE);
      nas_information->reg_nw_info.gprs_attach_status= ((gmm_state == GMM_NULL ) ||
                                                        (gmm_state == GMM_DEREGISTERED ) ||
                                                        (gmm_state == GMM_REGISTERED_INITIATED ))?FALSE:TRUE;
      memscpy(nas_information->reg_nw_info.rplmn.identity, sizeof(sys_plmn_id_s_type), mm_location_information.lai,3);
      mm_nas_changed_group &= ~SYS_NAS_REGISTRATION_INFO_MASK;
    }
    if (mm_nas_changed_group & SYS_NAS_PERIODIC_TIMER_MASK)
    {
#ifdef FEATURE_NAS_REL10
    if(mm_is_per_ms_t3212_present)
    {
      nas_information->periodic_timers.T3212_value= mm_stored_per_ms_t3212_value;
    }
    else
#endif
    {
      nas_information->periodic_timers.T3212_value= mm_system_information.T3212_timer_value;
    }
      nas_information->periodic_timers.T3312_value= gmm_t3312_timer_value;
      mm_nas_changed_group &= ~SYS_NAS_PERIODIC_TIMER_MASK;
    }
    if (mm_nas_changed_group & SYS_NAS_PS_CALL_INFO_MASK)
    {
      sm_get_ps_call_info (&(nas_information->ps_call_info));
      mm_nas_changed_group &= ~SYS_NAS_PS_CALL_INFO_MASK;
    }
    nas_information->sys_mode = mm_convert_sys_rat_to_acq_mode(mm_serving_plmn.info.active_rat);

    result = TRUE;

    rex_leave_crit_sect(&mm_ftd_crit_sect);
 
  }

  return result;
} 
#endif 

#ifdef FEATURE_LTE
/*===========================================================================

FUNCTION MM_GET_LAST_UPDATED_LAI_IN_LTE

DESCRIPTION
  GRR should call below NAS API to get the current updated LAC information in LTE registration and 
  compare whether any GSM cell associated with this LAC to be removed from redirection list.
  
DEPENDENCIES
  None

RETURN VALUE
  boolean - Return - FALSE - LAI not updated, TRUE - LAI updated 

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_get_last_updated_lai_in_lte (inter_task_lai_T *rr_lai, const sys_modem_as_id_e_type as_id)
{
   boolean return_update_status = FALSE;
   emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;
   
   emm_ctrl_data_ptr = emm_db_get_ctrl_data_per_subs((mm_as_id_e_type)as_id);
   
   ASSERT(emm_ctrl_data_ptr != NULL);

   if (emm_ctrl_data_ptr->emm_cs_loci_info.lai_exists == TRUE )
   {
     rr_lai->PLMN_id.identity[0] = emm_ctrl_data_ptr->emm_cs_loci_info.lai[0];
     rr_lai->PLMN_id.identity[1] = emm_ctrl_data_ptr->emm_cs_loci_info.lai[1];
     rr_lai->PLMN_id.identity[2] = emm_ctrl_data_ptr->emm_cs_loci_info.lai[2];
     rr_lai->location_area_code[0] = emm_ctrl_data_ptr->emm_cs_loci_info.lai[3];
     rr_lai->location_area_code[1] = emm_ctrl_data_ptr->emm_cs_loci_info.lai[4];

     return_update_status = TRUE;
   }

   MSG_HIGH_DS_3(MM_LOCAL_SUB, "=MM= Status = %d Serving LAC %X %X", return_update_status, emm_ctrl_data_ptr->emm_cs_loci_info.lai[3], emm_ctrl_data_ptr->emm_cs_loci_info.lai[4]);
   
   return return_update_status;
}

#endif

void mm_per_subs_set_nas_bit_mask(uint16 bit_mask, sys_modem_as_id_e_type as_id)
{
  //temp function for compilation dependency issues which will be removed later once CM modifies the code
  (void)as_id;
  (void)bit_mask;
  return;
}

/*===========================================================================

FUNCTION MM_PER_NAS_STACKS_SET_NAS_BIT_MASK

DESCRIPTION
  This function Sets NAS client activated group.
  This function is internal to NAS and expects input for stack id in NAS stack id format.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_per_nas_stacks_set_nas_bit_mask(uint16 bit_mask, sys_modem_as_id_e_type stack_id_nas)
{
#if !defined(FEATURE_DUAL_SIM) && !defined (FEATURE_SGLTE)
  (void)stack_id_nas;
  mm_set_nas_bit_mask(bit_mask);
#else
  /* Currently 6 bits out of 16 bit mask are used for
  6 different NAS Info groups 
  So masking of the rest of the unused bits*/


  if(IS_NOT_VALID_STACK_ID(stack_id_nas))
  {
     MSG_ERROR_DS(MM_SUB, "=MM= Error! Set NAS BIT MASK, incorrect STACK ID = %d",stack_id_nas,0,0); 
  }
  else
  {
    rex_enter_crit_sect(&mm_ftd_crit_sect);
    mm_client_activated_group_sim[stack_id_nas] = bit_mask & 0x003F; 
    rex_leave_crit_sect(&mm_ftd_crit_sect);
  }

#endif
}


/*===========================================================================

FUNCTION MM_PER_STACKS_SET_NAS_BIT_MASK

DESCRIPTION
  This function Sets NAS client activated group.
  This function is called by CM
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_per_stacks_set_nas_bit_mask
(
  uint16 bit_mask,
  sys_modem_as_id_e_type sub_id,
  sys_modem_stack_id_e_type  stack_id
)
{
  sys_modem_as_id_e_type stack_id_nas = mm_get_nas_stack_id(sub_id,stack_id);
  mm_per_nas_stacks_set_nas_bit_mask(bit_mask,stack_id_nas);
}



/*===========================================================================

FUNCTION mm_per_nas_stacks_get_nas_stats_info

DESCRIPTION
  This function gets the NAS stats info.
  This function is internal to NAS and uses stack ID in NAS format.
  
DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_per_nas_stacks_get_nas_stats_info
(
  sys_nas_information_s_type* nas_information,
    sys_modem_as_id_e_type sub_id,
  sys_modem_as_id_e_type as_id
)
{
#if !defined(FEATURE_DUAL_SIM) && !defined (FEATURE_SGLTE)
  (void)as_id;
  (void)sub_id;
  return mm_get_nas_stats_info(nas_information);
#else
  boolean result = FALSE;

  if (nas_information == NULL)
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= Invalid NAS information");
  }
  else
  {
    if(IS_NOT_VALID_STACK_ID(as_id) ||
       IS_NOT_VALID_SUB_ID(sub_id))
    {
      MSG_ERROR_DS(MM_SUB, "=MM= Error! Get NAS STATS INFO, incorrect STACK_ID = %d or SUB ID",
                                                                                as_id,sub_id,0);
    }
    else
    {
       rex_enter_crit_sect(&mm_ftd_crit_sect);
       nas_information->changed_bitmask = mm_nas_changed_group_sim[as_id];
   
       if (mm_nas_changed_group_sim[as_id] & SYS_NAS_ATTACH_TYPE_MASK)
       {
         nas_information->attach_type = gmm_attach_type_sim[as_id];
         mm_nas_changed_group_sim[as_id] &= ~SYS_NAS_ATTACH_TYPE_MASK;
       }
       if (mm_nas_changed_group_sim[as_id] & SYS_NAS_CS_REJECT_CAUSE_MASK)
       {
         nas_information->cs_reject_cause = mm_lu_reject_info_sim[as_id].reject_cause;
         mm_nas_changed_group_sim[as_id] &= ~SYS_NAS_CS_REJECT_CAUSE_MASK;
       }
       if (mm_nas_changed_group_sim[as_id] & SYS_NAS_PS_REJECT_CAUSE_MASK)
       {
         nas_information->ps_reject_cause = gmm_reject_cause_sim[as_id];
         mm_nas_changed_group_sim[as_id] &= ~SYS_NAS_PS_REJECT_CAUSE_MASK;
       }
       if (mm_nas_changed_group_sim[as_id] & SYS_NAS_REGISTRATION_INFO_MASK)
       {
         memscpy(nas_information->reg_nw_info.tmsi,SYS_TMSI_SIZE,mm_location_information_sim[as_id].tmsi,TMSI_SIZE);
         memscpy(nas_information->reg_nw_info.ptmsi,SYS_PTMSI_SIZE,gmm_stored_ptmsi_sim[as_id].digit,PTMSI_SIZE);
         nas_information->reg_nw_info.gprs_attach_status= ((gmm_state_sim[as_id] == GMM_NULL ) ||
                                                           (gmm_state_sim[as_id] == GMM_DEREGISTERED ) ||
                                                           (gmm_state_sim[as_id] == GMM_REGISTERED_INITIATED ))?FALSE:TRUE;
         memscpy(nas_information->reg_nw_info.rplmn.identity,3, mm_location_information_sim[as_id].lai,3);
         mm_nas_changed_group_sim[as_id] &= ~SYS_NAS_REGISTRATION_INFO_MASK;
       }
       if (mm_nas_changed_group_sim[as_id] & SYS_NAS_PERIODIC_TIMER_MASK)
       {
         nas_information->periodic_timers.T3212_value= mm_system_information_sim[as_id].T3212_timer_value;
         nas_information->periodic_timers.T3312_value= gmm_t3312_timer_value_sim[as_id];
         mm_nas_changed_group_sim[as_id] &= ~SYS_NAS_PERIODIC_TIMER_MASK;
       }
       if (mm_nas_changed_group_sim[as_id] & SYS_NAS_PS_CALL_INFO_MASK)
       {
#ifdef FEATURE_DUAL_SIM
         boolean as_id_match = (as_id == sm_as_id);
#else
         boolean as_id_match = (as_id == (sys_modem_as_id_e_type)mm_ps_stack_id);
#endif
         if(as_id_match == TRUE)
         {
           sm_get_ps_call_info (&(nas_information->ps_call_info));
           mm_nas_changed_group_sim[as_id] &= ~SYS_NAS_PS_CALL_INFO_MASK;
         }
       }
       nas_information->sys_mode = mm_convert_sys_rat_to_acq_mode(mm_serving_plmn_sim[as_id].info.active_rat);
       result = TRUE;
       rex_leave_crit_sect(&mm_ftd_crit_sect);
    }
  }

  return result;
#endif
} 

boolean mm_per_subs_get_nas_stats_info(sys_nas_information_s_type* nas_information, sys_modem_as_id_e_type as_id)
{
  (void)nas_information;
  (void)as_id;
  //temp function for compilation dependency issues which will be removed later once CM modifies the code
  return FALSE;
}

/*===========================================================================

FUNCTION mm_per_stacks_get_nas_stats_info

DESCRIPTION
  This function is called by CM to get the NAS stats info.
  This function gets sub_id and stack_id in CM <-> NAS and uses
  these values to retrive information from correct NAS stack.
  
DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_per_stacks_get_nas_stats_info
(
  sys_nas_information_s_type* nas_information, 
  sys_modem_as_id_e_type sub_id,
  sys_modem_stack_id_e_type stack_id
)
{
  sys_modem_as_id_e_type stack_id_nas = mm_get_nas_stack_id(sub_id,stack_id);
  return mm_per_nas_stacks_get_nas_stats_info(nas_information,sub_id,stack_id_nas);
}

#ifdef FEATURE_NAS_ECALL
/*===========================================================================

FUNCTION MM_PERFORM_ECALL_INACTIVITY

DESCRIPTION
  This function is used by MM to do Ecall inactivity Procedure
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_perform_ecall_inactivity(void)
{
  mmr_service_ind_s_type     mmr_service_ind;
  mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_NONE);
     
  if (mm_state == MM_NULL )
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM=ECALL Inactivity cannot be performed in state %d ", mm_state);
  }
  else if (mm_state == MM_IDLE )
  {
    mm_stop_timer(TIMER_T3211);
    mm_stop_timer(TIMER_T3212);
    mm_stop_timer(TIMER_T3213);


    if ((mm_system_information.ATT) &&
        ((mm_idle_substate == MM_PLMN_SEARCH_NORMAL_SERVICE) ||
         (mm_idle_substate == MM_NORMAL_SERVICE)) &&
        ((mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_ONLY) ||
         (mm_serving_plmn.info.reg_service_domain == SYS_SRV_DOMAIN_CS_PS)) &&
         (mm_check_if_cs_cell_access_allowed(cs_cell_access) == TRUE) &&
        (mm_serving_plmn.mm_service_state == SERVICE_AVAILABLE))
    { 
                  
      mm_state_control( MM_STD_MS_DEACTIVATED_ATTACH_ALLOWED );
                 
    }                   
    else if (mm_idle_substate != MM_PLMN_SEARCH)
    {
      if (mm_serving_plmn.mm_service_state == SERVICE_NOT_AVAILABLE)
      {
        mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                      SYS_SRV_DOMAIN_NO_SRV,
                                      FALSE,
                                      SYS_SRV_STATUS_NO_SRV,
                                      TRUE );
    
        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
      }
      else
      {
        mm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                      SYS_SRV_DOMAIN_NO_SRV,
                                      FALSE,
                                      SYS_SRV_STATUS_LIMITED,
                                      TRUE );
        mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_FAILURE_REMAIN_ON_PLMN;
      }

      mm_substate_control(MM_STD_ECALL_INACTIVE);

      mm_delete_lu_status(UPDATING_DISABLED);

      if (mm_reg_waiting_for_reg_cnf)
      {
        MSG_HIGH_DS_0(MM_SUB,"=MM=Sending MMR_REG_CNF ");

        mmcoord_mmr_reg_cnf.service_state = mm_serving_plmn.info;
        mm_send_mmr_reg_cnf();
        mm_reg_waiting_for_reg_cnf = FALSE;
        mm_ready_to_send_reg_cnf   = FALSE;

      }
      else
      {
        MSG_HIGH_DS_0(MM_SUB,"=MM=Sending MMR_SERVIC_IND ");
        mmr_service_ind.cause =  mmcoord_mmr_reg_cnf.cause;
        mmr_service_ind.service_state = mm_serving_plmn.info;
        mm_send_mmr_service_ind( &mmr_service_ind );
      }
                 
    }

    mm_call_est_cause = NO_CNM_SERVICE;
    mm_need_to_perform_ecall_inactivity = FALSE;
  } 
  else
  {
    mm_need_to_perform_ecall_inactivity = TRUE;
  }
  return;
}
#endif

#ifdef FEATURE_RAT_PRIORITY_LIST

/*===========================================================================

FUNCTION mm_rat_pri_list_match

DESCRIPTION
  This function is used to check if two rat priority list parameters are equal.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if rat priority lists match, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_rat_pri_list_match
(
  const sys_rat_pri_list_info_s_type    *rat_pri_list_info_1, 
  const sys_rat_pri_list_info_s_type    *rat_pri_list_info_2
)
{
 uint32 i,j;
  boolean is_rat_found;
  
  if(rat_pri_list_info_1->num_items != rat_pri_list_info_2->num_items)
     return FALSE;

  for (i = 0; i < rat_pri_list_info_1->num_items; i++)
  {
     is_rat_found = FALSE;
     for (j = 0; j < rat_pri_list_info_2->num_items; j++)
     {
        if(rat_pri_list_info_1->priority_list_info[i].acq_sys_mode == 
              rat_pri_list_info_2->priority_list_info[j].acq_sys_mode)
        {
          if(rat_pri_list_info_1->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE)
          {
             if( SYS_LTE_BAND_MASK_CHK_IF_EQUAL(&rat_pri_list_info_1->priority_list_info[i].band_cap.lte_band_cap,
                                       &rat_pri_list_info_2->priority_list_info[j].band_cap.lte_band_cap)== TRUE)
             {
               is_rat_found = TRUE;
               break;
             }
          }
          else if((rat_pri_list_info_1->priority_list_info[i].band_cap.chgwt_band_cap ==
              rat_pri_list_info_2->priority_list_info[j].band_cap.chgwt_band_cap))  
          {
            is_rat_found = TRUE;
            break;
          }
        }
     }
     if(!is_rat_found)
     {
        return FALSE;
     }
  }
  return TRUE;
}


/*===========================================================================

FUNCTION mm_rat_pri_list_gw_match

DESCRIPTION
  This function compares whether the two rat priority list parameters passed
  are same with respect to GW only.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE on success, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_rat_pri_list_gw_match
(
  const sys_rat_pri_list_info_s_type    *rat_pri_list_info_1, 
  const sys_rat_pri_list_info_s_type    *rat_pri_list_info_2
)
{
  uint32 i;
  boolean is_wrat_found_in_list1, is_grat_found_in_list1;  
  boolean is_wrat_found_in_list2, is_grat_found_in_list2;
  boolean is_trat_found_in_list1, is_trat_found_in_list2;

  is_wrat_found_in_list1 = FALSE;
  is_grat_found_in_list1 = FALSE;
  is_trat_found_in_list1 = FALSE;
  for (i = 0; i < rat_pri_list_info_1->num_items; i++)
  {
    if(rat_pri_list_info_1->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_WCDMA)
    {
      is_wrat_found_in_list1 = TRUE;
    }
    else if(rat_pri_list_info_1->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_GSM)
    {
      is_grat_found_in_list1 = TRUE;
    }
    else if(rat_pri_list_info_1->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_TDS)
    {
      is_trat_found_in_list1 = TRUE;
    }
    if ((is_wrat_found_in_list1 && is_grat_found_in_list1) ||
        (is_trat_found_in_list1 && is_grat_found_in_list1))
      break;
  }

  is_wrat_found_in_list2 = FALSE;
  is_grat_found_in_list2 = FALSE;
  is_trat_found_in_list2 = FALSE;
  for (i = 0; i < rat_pri_list_info_2->num_items; i++)
  {
    if(rat_pri_list_info_2->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_WCDMA)
    {
      is_wrat_found_in_list2 = TRUE;
    }
    else if(rat_pri_list_info_2->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_GSM)
    {
      is_grat_found_in_list2 = TRUE;
    }
    else if(rat_pri_list_info_2->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_TDS)
    {
      is_trat_found_in_list2 = TRUE;
    }
    if ((is_wrat_found_in_list2 && is_grat_found_in_list2) ||
        (is_trat_found_in_list2 && is_grat_found_in_list2))
      break;
  }

  if ((is_wrat_found_in_list1 != is_wrat_found_in_list2) ||
      (is_trat_found_in_list1 != is_trat_found_in_list2) ||
      (is_grat_found_in_list1 != is_grat_found_in_list2))
    return FALSE;

  return TRUE;
}


/*===========================================================================

FUNCTION mm_check_if_rat_pri_list_contains_gw

DESCRIPTION
  This function checks if the rat priority list parameter passed has
  both GW rats.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE on success, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_check_if_rat_pri_list_contains_gw
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_info 
)
{
  uint32 i;
  boolean is_wrat_found_in_list, is_grat_found_in_list, is_trat_found_in_list;  
  is_wrat_found_in_list = FALSE;
  is_grat_found_in_list = FALSE;
  is_trat_found_in_list = FALSE;
  
  for (i = 0; i < rat_pri_list_info->num_items; i++)
  {
    if(rat_pri_list_info->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_WCDMA)
    {
      is_wrat_found_in_list = TRUE;
    }
    else if(rat_pri_list_info->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_GSM)
    {
      is_grat_found_in_list = TRUE;
    }
    else if(rat_pri_list_info->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_TDS)
    {
      is_trat_found_in_list = TRUE;
    }

    if ((is_wrat_found_in_list && is_grat_found_in_list) ||
        (is_trat_found_in_list && is_grat_found_in_list))
      return TRUE;
  }

  return FALSE;
}


/*===========================================================================

FUNCTION mm_checkif_rat_list_superset

DESCRIPTION
  This function is used to check if the first rat priority list parameter passed is a 
  superset of the second rat priority list parameter.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if successful, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_checkif_rat_pri_list_superset
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_info_message, 
  const sys_rat_pri_list_info_s_type *rat_pri_list_info
)
{
  uint32 i,j;
  boolean is_rat_found;

  if ((rat_pri_list_info_message->num_items < rat_pri_list_info->num_items) || 
      (reg_mode_rat_pri_list_match(rat_pri_list_info_message,rat_pri_list_info )))
  {
    return FALSE;
  }

  for (i = 0; i < rat_pri_list_info->num_items; i++)
  {
    is_rat_found = FALSE;
    for (j = 0; j < rat_pri_list_info_message->num_items; j++)
    {
      if(rat_pri_list_info_message->priority_list_info[j].acq_sys_mode == 
          rat_pri_list_info->priority_list_info[i].acq_sys_mode) 
      {
        if(rat_pri_list_info_message->priority_list_info[j].acq_sys_mode == SYS_SYS_MODE_LTE)
        {
          if(SYS_LTE_BAND_MASK_CHK_IF_SUBSET(&rat_pri_list_info_message->priority_list_info[j].band_cap.lte_band_cap,&rat_pri_list_info->priority_list_info[i].band_cap.lte_band_cap) == TRUE)
          {
            is_rat_found = TRUE;
            break;
          }
        }
        else if(((rat_pri_list_info->priority_list_info[i].band_cap.chgwt_band_cap & 
               rat_pri_list_info_message->priority_list_info[j].band_cap.chgwt_band_cap) ==
              rat_pri_list_info->priority_list_info[i].band_cap.chgwt_band_cap))
        {
          is_rat_found = TRUE;
          break;
        }
      }
    }
    if(!is_rat_found)
    {
      return FALSE;
    }
  }
  return TRUE;
}

/*===========================================================================

FUNCTION mm_convert_sys_rat_to_acq_mode

DESCRIPTION
  This function converts the radio access techonology passed to
  corresponding acquisition mode.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns the sys acquisition mode.

SIDE EFFECTS
  None

===========================================================================*/
sys_sys_mode_e_type mm_convert_sys_rat_to_acq_mode
(
  const sys_radio_access_tech_e_type rat
)
{
  if(rat == SYS_RAT_GSM_RADIO_ACCESS)
    return SYS_SYS_MODE_GSM;
   
  if(rat == SYS_RAT_UMTS_RADIO_ACCESS)
    return SYS_SYS_MODE_WCDMA;
   
  if(rat == SYS_RAT_LTE_RADIO_ACCESS)
    return SYS_SYS_MODE_LTE;

  if (rat == SYS_RAT_TDS_RADIO_ACCESS)
    return SYS_SYS_MODE_TDS;    

  return SYS_SYS_MODE_MAX;
                 
}


/*===========================================================================

FUNCTION mm_checkif_rat_pri_list_has_rat

DESCRIPTION
  This function verifies if the passed rat is present in the
  rat priority list parameter.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if successful, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_checkif_rat_pri_list_has_rat
(
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info,
  const sys_radio_access_tech_e_type rat
)
{
  uint32 i;
  boolean is_rat_found;
  sys_sys_mode_e_type  acq_sys_mode;
  
  acq_sys_mode = mm_convert_sys_rat_to_acq_mode(rat);
  is_rat_found = FALSE;
  for (i = 0; i < rat_pri_list_info->num_items; i++)
  {
    if(rat_pri_list_info->priority_list_info[i].acq_sys_mode == acq_sys_mode)
    {
      is_rat_found = TRUE;
      break;
    }
  }
  if(!is_rat_found)
  {
    return FALSE;
  }
  return TRUE;
}


/*===========================================================================

FUNCTION mm_get_band_capacity

DESCRIPTION
  This function gets the corresponding band for the acquisition mode passed
  from the rat priority list.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns the corresponding band if present, else returns SYS_BAND_MASK_EMPTY.

SIDE EFFECTS
  None

===========================================================================*/
sys_band_mask_e_type mm_get_band_capacity
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_info,
  const sys_sys_mode_e_type acq_sys_mode
)
{
  uint32 i;

  for (i = 0; i < rat_pri_list_info->num_items; i++)
  {
    if(rat_pri_list_info->priority_list_info[i].acq_sys_mode == acq_sys_mode)
    {
      return (rat_pri_list_info->priority_list_info[i].band_cap.chgwt_band_cap);
    }
  }
  return SYS_BAND_MASK_EMPTY;
}

/*===========================================================================

FUNCTION mm_checkif_rat_added_to_rat_pri_list

DESCRIPTION
  This function cheks if the request rat is present in list1 and not present in list2
  
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if successful, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_checkif_rat_added_to_rat_pri_list
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_info_new, 
  const sys_rat_pri_list_info_s_type *rat_pri_list_info_old,
  const sys_radio_access_tech_e_type req_rat
)
{
  uint32 i;
  boolean is_req_rat_present_in_new_list = FALSE;
  boolean is_req_rat_present_in_old_list = FALSE;
  sys_sys_mode_e_type  acq_sys_mode;
  
  acq_sys_mode = mm_convert_sys_rat_to_acq_mode(req_rat);

  for (i = 0; i < rat_pri_list_info_new->num_items; i++)
  {
    if(rat_pri_list_info_new->priority_list_info[i].acq_sys_mode == acq_sys_mode)
    {
      is_req_rat_present_in_new_list = TRUE;
      break;
    }
  }

  for (i = 0; i < rat_pri_list_info_old->num_items; i++)
  {
    if(rat_pri_list_info_old->priority_list_info[i].acq_sys_mode == acq_sys_mode)
    {
      is_req_rat_present_in_old_list = TRUE;
      break;
    }
  }

  if(!is_req_rat_present_in_old_list && is_req_rat_present_in_new_list)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

#endif /* FEATURE_RAT_PRIORITY_LIST*/

#ifdef FEATURE_DUAL_SIM
/*************************************************************
FUNCTION mm_abort_connection_due_to_mplmn_on_other_subs

DESCRIPTION
  This function checks if RAT other than current RAT has connection setup for PS and abort it
  for MPLMN srch is triggered on current RAT

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
 *************************************************************/
void  mm_abort_connection_due_to_mplmn_on_other_subs( void )
{
  mm_as_id_e_type      mm_other_as_id, mm_tmp_as_id;
  mm_tmp_as_id = mm_as_id;

  if (MMCNM_IS_MODE_DUAL_ACTIVE && (mm_mn_sglte_svlte_mode == FALSE))
  {
      return;
  }
  for( mm_other_as_id = MM_AS_ID_1; mm_other_as_id < (mm_as_id_e_type)MAX_NAS_STACKS ; mm_other_as_id++ )
  {
    if( mm_other_as_id != mm_tmp_as_id )
    {
      mm_as_id = mm_other_as_id;
      mm_sub_id = mm_sub_id_stack[mm_as_id];
      if((is_gsm_mode() == TRUE)&& 
         ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) ||
         (mm_serving_plmn.ms_op_mode ==  SYS_SRV_DOMAIN_PS_ONLY)))
      {
        mm_send_rr_ps_abort_req(MM_RR_PS_ABORT_MANUAL_PLMN_SEARCH);
      }
#ifdef FEATURE_LTE
      if(is_lte_mode())
      {
        emm_ctrl_data_ptr = emm_db_get_ctrl_data();
        if(((EMM_GET_CONNECTION_STATE() == EMM_WAITING_FOR_RRC_CONFIRMATION_STATE ) ||
          ( EMM_GET_CONNECTION_STATE() == EMM_CONNECTED_STATE ))&&
          ( emm_ctrl_data_ptr->emm_sms_active == FALSE )&&
          ( emm_mmtel_status( emm_ctrl_data_ptr ) == FALSE )&&
          ((( EMM_GET_STATE() == EMM_SERVICE_REQUEST_INITIATED )&&
          ( emm_ctrl_data_ptr->call_type !=  SYS_LTE_REESTAB_CALLTYPE_VOLTE)&&
          ( emm_ctrl_data_ptr->esr_type_in_progress == NAS_EMM_CSFB_NONE)&&
          ( emm_ctrl_data_ptr->emc_srv_status == LTE_NAS_EMC_SRV_NONE))||
          (EMM_GET_STATE() == EMM_REGISTERED)||
          (EMM_GET_STATE() == EMM_DEREGISTERED)))
        {
          MSG_HIGH_DS_0(MM_SUB,"=EMM= Abort RRC connection for manual plmn serach request");
          emm_rrc_abort_connection(emm_ctrl_data_ptr, FALSE,LTE_RRC_CONN_ABORT_CAUSE_NORMAL) ;
        }
      }
      else
#endif
      if((is_umts_mode() == TRUE) &&
         ((mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) ||
          (mm_serving_plmn.ms_op_mode ==  SYS_SRV_DOMAIN_PS_ONLY)) &&
         (rrc_connection_status != MMRRC_IDLE ) &&
         (mm_sms_is_active == FALSE)&&
         (gmm_state != GMM_REGISTERED_INITIATED)&&
         (gmm_state != GMM_ROUTING_AREA_UPDATING_INITIATED)&&
         (gmm_state != GMM_DEREGISTERED_INITIATED)&&
         (cs_session_status == MMRRC_IDLE)&&
         (mm_is_mt_page_in_progress() == FALSE)&&
         (mm_state == MM_IDLE))
      {
        MSG_HIGH_DS_0(MM_SUB,"=MM= Received Manual PLMN search req,aborting RRC established for MO data ");
        mm_abort_rr(RRC_PS_DOMAIN_CN_ID, FALSE);
      }
    }
  }
  mm_as_id = mm_tmp_as_id;
  mm_sub_id = mm_sub_id_stack[mm_as_id];
#ifdef FEATURE_LTE
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
#endif
}
#endif


#ifdef FEATURE_LTE

/*===========================================================================

FUNCTION mm_get_lte_band_capacity

DESCRIPTION
  This function gets the corresponding band for the acquisition mode passed
  from the rat priority list.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns the corresponding band if present, else returns SYS_BAND_MASK_EMPTY.

SIDE EFFECTS
  None

===========================================================================*/
sys_lte_band_mask_e_type mm_get_lte_band_capacity
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_info
)
{
  uint32 i;

  for (i = 0; i < rat_pri_list_info->num_items; i++)
  {
    if(rat_pri_list_info->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE)
    {
      return (rat_pri_list_info->priority_list_info[i].band_cap.lte_band_cap);
    }
  }
  return SYS_LTE_BAND_MASK_CONST_NONE;
}

/*===========================================================================

FUNCTION  mm_check_if_rat_removed_from_rat_pri_list

DESCRIPTION
  This function checks if the requested rat is removed from the new rat list and  is present in 
  old one
  
DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_check_if_rat_removed_from_rat_pri_list
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_info_new, 
  const sys_rat_pri_list_info_s_type *rat_pri_list_info_old,
  const sys_radio_access_tech_e_type rat
)
{
  if((mm_checkif_rat_pri_list_has_rat(rat_pri_list_info_old,rat)) &&
     !(mm_checkif_rat_pri_list_has_rat(rat_pri_list_info_new,rat)))
  {
    return TRUE;
  }
  return FALSE;
}


/*===========================================================================

FUNCTION  mm_remove_rat_from_rat_pri_list

DESCRIPTION
  Removes the requested rat from the rat priority list passed as
  the parameter 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_remove_rat_from_rat_pri_list
(
  sys_rat_pri_list_info_s_type *rat_pri_list_info,
  const sys_sys_mode_e_type rat
)
{
  uint32 i,j;
  
  rat_pri_list_info->num_items = MIN(rat_pri_list_info->num_items,SYS_MAX_ACQ_PREF_RAT_LIST_NUM);
  for (i= 0; i < rat_pri_list_info->num_items; i++)
  {
    if(rat_pri_list_info->priority_list_info[i].acq_sys_mode == rat)
    {
      for(j = i+1; j < rat_pri_list_info->num_items; j++)
      {
        rat_pri_list_info->priority_list_info[j-1] = rat_pri_list_info->priority_list_info[j];
      }
      rat_pri_list_info->num_items--;
      break;
    }
  }
}

/*===========================================================================

FUNCTION  mm_is_csfb_call_allowed

DESCRIPTION - This function checks if the call will be rejected or processed.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/

boolean mm_is_csfb_call_allowed(void)  
{
  mm_as_cell_access_status_T cs_cell_access = mm_get_cs_cell_access(PROCEDURE_NONE);
  byte cnm_service_request_service_type;
  boolean error_indication=FALSE;
  #ifdef FEATURE_MODEM_HEAP
    cnm_service_request_service_type=mm_csfb_service_req_message_ptr->cmd.mmcnm_est_req.CNM_service_type;    
  #else
    cnm_service_request_service_type=mm_csfb_service_req_message->cmd.mmcnm_est_req.CNM_service_type;
  #endif
  MSG_HIGH_DS_2( MM_SUB, "=MM= CSFB call validity, Cs cell access %d,Service Reqeust Type %d",
                                             cs_cell_access,cnm_service_request_service_type);
  switch (cs_cell_access)
  {
       case MM_AS_ACCESS_EMERGENCY_ONLY:
       if (cnm_service_request_service_type != EMERGENCY_CALL_ESTABLISHMENT)
       {
          error_indication = TRUE;
       }
       break;
       case MM_AS_ACCESS_NO_CALLS:
       if ((cnm_service_request_service_type != EMERGENCY_CALL_ESTABLISHMENT) ||
           ((is_gsm_mode()) && (!mm_retry_em_call_on_access_blocked_cell)))
       {
          error_indication = TRUE;
       }
       case MM_AS_ACCESS_ALL_CALLS:
       case MM_AS_ACCESS_NORMAL_ONLY:
       default:
        break;
  }
  return(error_indication);
}


#endif
/*===========================================================================

FUNCTION mm_log_mm_reg_variables

DESCRIPTION
  This function will be called by CM/MMOC module for logging MM, EMM and REG variable.
  This will help mmoc crash debugging.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

void mm_log_mm_reg_variables()
{
  int i=0,index;
#ifdef FEATURE_LTE
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

#ifdef FEATURE_DUAL_SIM
  /*need to find the multimode sub so that correct value can be assigned to emm_ctrl_data_ptr*/
  mm_as_id_e_type sub_id = MM_AS_ID_1;
  for (sub_id = MM_AS_ID_1; sub_id < (mm_as_id_e_type)MAX_AS_IDS; sub_id++)
  {
    if(mm_sub_capability_sim[sub_id] == SUBS_CAPABILITY_MULTIMODE)
    {
      emm_ctrl_data_ptr = emm_db_get_ctrl_data_per_subs(sub_id);
      break;
    }
  }
#else
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
#endif
  ASSERT(emm_ctrl_data_ptr != NULL);
#endif

  MSG_HIGH_DS_0(MM_SUB,"=MM= NAS LOGGING MM REG Variables");

  reg_state_log_variables();
#ifdef FEATURE_LTE
  i = mm_emm_debug_buffer_index;
  while ((--i>= 0) && (mm_emm_debug_buffer_index - i< 5))
  {
    if((mm_emm_debug_buffer[i].mm_emm_message_direction == TO_MM) ||
       (mm_emm_debug_buffer[i].mm_emm_message_direction == FROM_MM))
    {
      MSG_HIGH_DS_3(mm_emm_debug_buffer[i].as_id, "Direction: %d MSG SET: %d ID: %d",
           mm_emm_debug_buffer[i].mm_emm_message_direction,
           mm_emm_debug_buffer[i].debug_buffer_type.mm_info_debug_buffer.message_set,
           mm_emm_debug_buffer[i].debug_buffer_type.mm_info_debug_buffer.message_id);
    }
    else
    {
      MSG_HIGH_DS_3(mm_emm_debug_buffer[i].as_id, "Direction: %d UMID: %d EMM state: %d",
           mm_emm_debug_buffer[i].mm_emm_message_direction,
           mm_emm_debug_buffer[i].debug_buffer_type.emm_info_debug_buffer.msg_umid,
           mm_emm_debug_buffer[i].present_emm_state);
    }
  }
#endif
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
  for(i = 0; i < MAX_NAS_STACKS; i++)
#endif  
  {
    boolean local_sim_avail = mm_need_to_process_sim_avail_req_sim[i];
    boolean local_open_session = mm_waiting_for_open_session_cnf_sim[i];

    MSG_HIGH_DS_3( i, "=MM= mm_need_to_process_sim_avail_req/waiting_for_open=%2x, session_type = %d, mm_sim_card_mode=%d", 
         ((local_sim_avail<<4)| local_open_session), 
         mm_sim_mmgsdi_info_sim[i].session_type, mm_sim_card_mode_sim[i]);
#ifndef FEATURE_LTE
    MSG_HIGH_DS_3( i, "=MM= mm_sim_mode_pref=%d, mm_dual_standby_mode=%d, mm_ue_pref_rat = %d", 
                              mm_sim_mode_pref_sim[i], mm_dual_standby_mode, mm_ue_pref_rat_sim[i]);
#endif
    MSG_HIGH_DS_3( i, "=MM= Active Rat=%d(0=gsm, 1=wcdma, 2=lte), mm_waiting_for_deact_cnf =%d, mm_waiting_for_ph_status_change_cnf =%d", 
                              mm_serving_plmn_sim[i].info.active_rat, mm_waiting_for_deact_cnf_sim[i],
                              mm_waiting_for_ph_status_change_cnf_sim[i]);

    MSG_HIGH_DS_3( i, "=MM= mm_waiting_for_stop_mode_cnf=%d, mm_waiting_for_service_cnf=%d, mm_last_active_rat_stopped=%d", 
                  mm_waiting_for_stop_mode_cnf_sim[i], mm_waiting_for_service_cnf_sim[i],
                  mm_last_active_rat_stopped_sim[i]);
    if(i< MAX_AS_IDS)    
    {    
      MSG_HIGH_DS_2( i, "=MM= mm_sub_capability=%d, reg_sub_config=%d ", 
                  mm_sub_capability_sim[i], reg_sub_config_sim[i]);                  
    }
  }
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE)
  {
    MSG_HIGH_DS_3(MM_SUB,"=MM= NAS SGLTE current stack status = %d  sglte mode = %d,mm_cs_stack_id= %d  ",
                                        mm_sglte_current_stack_status,mm_sglte_mode,mm_cs_stack_id);
    MSG_HIGH_DS_2(MM_SUB,"=MM= reg_sglte_ue_mode = %d  reg_cs_stack_id = %d ",
                                        reg_sglte_ue_mode,reg_cs_stack_id);                                        
  }
#endif
#ifdef FEATURE_LTE
  MSG_HIGH_DS_3(MM_SUB,"=EMM= EMM State = %d, EMM Substate = %d,  EMM Connection State = %d", 
                            emm_ctrl_data_ptr->emm_state, emm_ctrl_data_ptr->emm_substate,
                            emm_ctrl_data_ptr->emm_connection_state);

  MSG_HIGH_DS_2(MM_SUB,"=EMM= LRRC Active = %d, Last LRRC deact conf status = %d", 
                            emm_ctrl_data_ptr->rrc_active, emm_ctrl_data_ptr->last_rrc_deactivate_cnf.status);

  MSG_HIGH_DS_2(MM_SUB,"=EMM= LRRC deact req reason =%d, LRRC deact req pending =%d", 
                            emm_ctrl_data_ptr->rrc_deact_req.deact_reason, emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending);

  MSG_HIGH_DS_3(MM_SUB,"=EMM= LRRC aborting conn = %d, Registered serv domain = %d, Ignore GW activation ind = %d", 
                            emm_ctrl_data_ptr->aborting_conn, emm_ctrl_data_ptr->srv_domain_registered,
                            emm_ctrl_data_ptr->ignore_GW_activation_ind);
#endif

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
  for(i = 0; i < MAX_NAS_STACKS; i++)
#endif  
  {
#ifdef FEATURE_LTE
    if(mm_ps_detach_state_sim[i] != MM_PS_DETACH_STATE_NONE)
    {
         MSG_HIGH_DS_0(MM_SUB,"=MM= CM is waiting for PS_DETACH_CNF");
    }   
#endif
    if(mm_waiting_for_deact_cnf_sim[i])
    {
      MSG_HIGH_DS_1(i,"=MM= RAT %d (0=GSM, 1=WCDMA, 2=LTE, 3=TDS) was not able to respond to NAS for DEACT_REQ ",mm_serving_plmn_sim[i].info.active_rat);
    }
    else if(mm_waiting_for_stop_mode_cnf_sim[i]) 
    {
      MSG_HIGH_DS_1(i,"=MM= RAT %d (0=GSM, 1=WCDMA, 2=LTE, 3=TDS) was not able to respond to NAS for STOP_MODE_REQ/DEACTIVATE_REQ",\
                                                                                                mm_serving_plmn_sim[i].info.active_rat);
    }
    else if(mm_waiting_for_service_cnf_sim[i]) 
    {
      MSG_HIGH_DS_1(i,"=MM= RAT %d (0=GSM, 1=WCDMA, 2=LTE, 3=TDS) was not able to respond to NAS for SERVICE_REQ ",mm_serving_plmn_sim[i].info.active_rat);
    }
    else if(mm_waiting_for_ph_status_change_cnf_sim[i]) 
    {
      MSG_HIGH_DS_1(i,"=MM= RAT %d (0=GSM, 1=WCDMA, 2=LTE, 3=TDS) was not able to respond to NAS for DEACT_REQ/DEACTIVATE_REQ ",mm_serving_plmn_sim[i].info.active_rat);
    }
    else if((rrc_connection_status_sim[i] == MMRRC_RELEASE_IN_PROGRESS) 
#ifdef FEATURE_LTE
    || (mm_serving_plmn_sim[i].info.active_rat == SYS_RAT_LTE_RADIO_ACCESS && (emm_ctrl_data_ptr->aborting_conn))
#endif
    ) 
    {
      MSG_HIGH_DS_1(i,"=MM= RAT %d (0=GSM, 1=WCDMA, 2=LTE, 3=TDS) was not able to respond to NAS for ABORT_REQ",mm_serving_plmn_sim[i].info.active_rat);
    }
#ifdef FEATURE_LTE
    else if((mm_serving_plmn_sim[i].info.active_rat == SYS_RAT_LTE_RADIO_ACCESS) && (emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending))
    {
      MSG_HIGH_DS_0(MM_SUB,"=MM= LTE was not able to respond to NAS for LTE_RRC_DEACT_REQ ");
    }
#endif
    else 
    {
     index = reg_debug_buffer_index-1;
   
     while(index>=0 && (reg_debug_buffer[index].as_id != i))
     {
       index--;
     }
     if(index>=0)
     {
  
       switch(reg_debug_buffer[index].message_id)
       {
          case CM_SIM_AVAILABLE_CNF:
          MSG_HIGH_DS_0(i,"=MM= NAS successfully sent CM_SIM_AVAILABLE_CNF to CM");
          break;

          case CM_STOP_MODE_CNF:
          MSG_HIGH_DS_0(i,"=MM= NAS successfully sent CM_STOP_MODE_CNF to CM");
          break;

          case CM_PH_STATUS_CHGD_CNF:
          MSG_HIGH_DS_0(i,"=MM= NAS successfully sent CM_PH_STATUS_CHGD_CNF to CM");
          break;

          case CM_PS_DETACH_CNF:
          MSG_HIGH_DS_0(i,"=MM= NAS successfully sent CM_PS_DETACH_CNF to CM");
          break;

          default:
          MSG_HIGH_DS_2(i,"=MM= Last message sent by NAS is message set: %d , message id: %d ", \
             reg_debug_buffer[reg_debug_buffer_index-1].message_set,reg_debug_buffer[reg_debug_buffer_index-1].message_id);
          break;
       }
     }       
    }
  }
}


#else

void mm_log_mm_reg_variables()
{

#ifdef FEATURE_LTE
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif

  MSG_HIGH_DS_0(MM_SUB,"=MM= NAS LOGGING MM REG Variables");

  reg_state_log_variables();

  MSG_HIGH_DS_3(MM_SUB,"=MM= mm_need_to_process_sim_avail_req=%d, mm_waiting_for_open_session_cnf=%d, session_type = %d", 
                 mm_need_to_process_sim_avail_req,mm_waiting_for_open_session_cnf, mm_sim_mmgsdi_info.session_type);

  MSG_HIGH_DS_2(MM_SUB,"=MM= mm_sim_card_mode=%d, mm_ue_pref_rat=%d", mm_sim_card_mode, mm_ue_pref_rat);

  MSG_HIGH_DS_3(MM_SUB,"=MM= Active Rat=%d(0=gsm, 1=wcdma, 2=lte), mm_waiting_for_deact_cnf =%d, mm_waiting_for_ph_status_change_cnf =%d", 
                            mm_serving_plmn.info.active_rat, mm_waiting_for_deact_cnf ,
                            mm_waiting_for_ph_status_change_cnf );

  MSG_HIGH_DS_3(MM_SUB,"=MM= mm_waiting_for_stop_mode_cnf=%d, mm_waiting_for_service_cnf=%d, mm_last_active_rat_stopped=%d", 
                mm_waiting_for_stop_mode_cnf, mm_waiting_for_service_cnf, mm_last_active_rat_stopped );

#ifdef FEATURE_LTE
  MSG_HIGH_DS_3(MM_SUB,"=EMM= EMM State = %d, EMM Substate = %d,  EMM Connection State = %d", 
                            emm_ctrl_data_ptr->emm_state, emm_ctrl_data_ptr->emm_substate,
                            emm_ctrl_data_ptr->emm_connection_state);

  MSG_HIGH_DS_2(MM_SUB,"=EMM= LRRC Active = %d, Last LRRC deact conf status = %d", 
                            emm_ctrl_data_ptr->rrc_active, emm_ctrl_data_ptr->last_rrc_deactivate_cnf.status);

  MSG_HIGH_DS_2(MM_SUB,"=EMM= LRRC deact req reason =%d, LRRC deact req pending =%d", 
                            emm_ctrl_data_ptr->rrc_deact_req.deact_reason, emm_ctrl_data_ptr->rrc_deact_req.deact_req_pending);

  MSG_HIGH_DS_3(MM_SUB,"=EMM= LRRC aborting conn = %d, Registered serv domain = %d, Ignore GW activation ind = %d", 
                            emm_ctrl_data_ptr->aborting_conn, emm_ctrl_data_ptr->srv_domain_registered,
                            emm_ctrl_data_ptr->ignore_GW_activation_ind);

  if(mm_ps_detach_state != MM_PS_DETACH_STATE_NONE)
  {
       MSG_HIGH_DS_0(MM_SUB,"=MM= CM is waiting for PS_DETACH_CNF");
  }
#endif
  if(mm_waiting_for_deact_cnf)
  {
    MSG_HIGH_DS_1(MM_SUB,"=MM= RAT %d (0=GSM, 1=WCDMA, 2=LTE, 3=TDS) was not able to respond to NAS for DEACT_REQ ",mm_serving_plmn.info.active_rat);
  }
  else if(mm_waiting_for_stop_mode_cnf) 
  {
    MSG_HIGH_DS_1(MM_SUB,"=MM= RAT %d (0=GSM, 1=WCDMA, 2=LTE, 3=TDS) was not able to respond to NAS for STOP_MODE_REQ/DEACTIVATE_REQ ",
                                                               mm_serving_plmn.info.active_rat);
  }
  else if(mm_waiting_for_service_cnf) 
  {
    MSG_HIGH_DS_1(MM_SUB,"=MM= RAT %d (0=GSM, 1=WCDMA, 2=LTE, 3=TDS) was not able to respond to NAS for SERVICE_REQ ",mm_serving_plmn.info.active_rat);
  }
  else if(mm_waiting_for_ph_status_change_cnf) 
  {
    MSG_HIGH_DS_1(MM_SUB,"=MM= RAT %d (0=GSM, 1=WCDMA, 2=LTE, 3=TDS) was not able to respond to NAS for DEACT_REQ/DEACTIVATE_REQ ",mm_serving_plmn.info.active_rat);
  }
  else if((rrc_connection_status == MMRRC_RELEASE_IN_PROGRESS)
#ifdef FEATURE_LTE
  || (is_lte_mode() && (emm_ctrl_data_ptr->aborting_conn))
#endif
  )
  {
    MSG_HIGH_DS_1(MM_SUB,"=MM= RAT %d (0=GSM, 1=WCDMA, 2=LTE, 3=TDS) was not able to respond to NAS for ABORT_REQ",mm_serving_plmn.info.active_rat);
  }
#ifdef FEATURE_LTE
  else if(is_lte_mode() && (emm_ctrl_data_ptr->rrc_deact_req.deact_cnf_pending))
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= LTE was not able to respond to NAS for LTE_RRC_DEACT_REQ ");
  }
#endif
  else 
  {
    switch(reg_debug_buffer[reg_debug_buffer_index-1].message_id)
    {
       case CM_SIM_AVAILABLE_CNF:
       MSG_HIGH_DS_0(MM_SUB,"=MM= NAS successfully sent CM_SIM_AVAILABLE_CNF to CM");
       break;

       case CM_STOP_MODE_CNF:
       MSG_HIGH_DS_0(MM_SUB,"=MM= NAS successfully sent CM_STOP_MODE_CNF to CM");
       break;

       case CM_PH_STATUS_CHGD_CNF:
       MSG_HIGH_DS_0(MM_SUB,"=MM= NAS successfully sent CM_PH_STATUS_CHGD_CNF to CM");
       break;

       case CM_PS_DETACH_CNF:
       MSG_HIGH_DS_0(MM_SUB,"=MM= NAS successfully sent CM_PS_DETACH_CNF to CM");
       break;

       default:
       MSG_HIGH_DS_2(MM_SUB,"=MM= Last message sent by NAS is message set: %d , message id: %d ", \
          reg_debug_buffer[reg_debug_buffer_index-1].message_set,reg_debug_buffer[reg_debug_buffer_index-1].message_id);
       break;
    }
  }
 }
#endif
/*===========================================================================

FUNCTION MM_CONVERT_TO_REG_ACCESS_INFO

DESCRIPTION
  This function converts mm_as_cell_access_status_T into 
  sys_cell_access_status_e_type
  
DEPENDENCIES
  None

RETURN VALUE
  sys_cell_access_status_e_type

SIDE EFFECTS
  None

===========================================================================*/
sys_cell_access_status_e_type  mm_convert_to_reg_access_info(mm_as_cell_access_status_T mm_cell_access_status)
{

  sys_cell_access_status_e_type sys_cell_access_status;
  switch(mm_cell_access_status)
  {
    case MM_AS_ACCESS_NORMAL_ONLY:
        sys_cell_access_status = SYS_CELL_ACCESS_NORMAL_ONLY;
        break;
    case MM_AS_ACCESS_EMERGENCY_ONLY:
        sys_cell_access_status = SYS_CELL_ACCESS_EMERGENCY_ONLY;
        break;
    case MM_AS_ACCESS_NO_CALLS:
        sys_cell_access_status = SYS_CELL_ACCESS_NO_CALLS;
        break;
    case MM_AS_ACCESS_ALL_CALLS:
        sys_cell_access_status = SYS_CELL_ACCESS_ALL_CALLS;
        break;
    default:
        sys_cell_access_status = SYS_CELL_ACCESS_ALL_CALLS;
        break;
 
  }
  return sys_cell_access_status;
}
/*===========================================================================

FUNCTION IS_LOW_PRIO_DEBUG_BUFFER_MESSAGE

DESCRIPTION
  This function Filters out the messages that doesn't need to be added in the mm debug buffer.
  All the Message Set and IDs will be ignored so that only helpful messages are added in the 
  mm debug buffer
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

NOTE
Filters out messages that are not required to be stored in debug buffer
===========================================================================*/

boolean is_low_prio_debug_buffer_message
(
  byte message_set,
  dword message_id
)
{
  boolean low_prio_message = FALSE;
  switch (message_set)
  {
    case MS_MM_RR:
      {
        switch(message_id)
        {
#ifdef FEATURE_NAS_FREE_FLOAT
          case RR_LAI_REJECT_LIST_IND :
#endif
          case RR_SIM_UPDATE_REQ :
          case RR_GMM_GPRS_MS_RA_CAP_IE_IND :
          case RR_GMM_START_MM_NON_DRX_IND :
          case RR_GMM_STOP_MM_NON_DRX_IND :
          case RR_RESELECTION_IND :
          case RR_EQ_PLMN_LIST_CHANGE_IND :
               low_prio_message = TRUE;
               break;
          default:
               break;
         }
         break;
       }
    case MS_MM_REG:
      {
        switch(message_id)
         {
           case MMR_CELL_ACCESS_IND:
               low_prio_message = TRUE;
               break;
           case MMR_HSPA_CALL_STATUS_IND:
           case MMR_CLEAR_LAI_REJECT_LIST_REQ:
           case MMR_EMERGENCY_NUM_LIST_IND :
           case MMR_CELL_SERVICE_IND :
               low_prio_message = TRUE;
               break;
           default:
               break;
         }
         break;
      }
    case MS_GSDI:
      low_prio_message =TRUE;
      break;
    case MS_MM_RRC:
      {
        switch(message_id)
        {
#ifdef FEATURE_NAS_FREE_FLOAT
          case RRC_LAI_REJECT_LIST_IND :
#endif
          case RRC_SIM_UPDATE_REQ :
          case RRC_EQ_PLMN_LIST_CHANGE_IND :
          case RRC_LTE_RESELECTION_ALLOWED_STATUS_IND :
               low_prio_message = TRUE;
               break; 
          default:
               break;
        }
        break;
      }        
    default:
      break;
  }
  return low_prio_message;
}
/*===========================================================================

FUNCTION MM_ADD_MESSAGE_TO_DEBUG_BUFFER

DESCRIPTION
  This function adds messages that MM received and sent to the mm_debug_buffer.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

NOTE
  Parameter "mm_emm_message_direction" indicates whether the particular message is
  sent or received by MM/EMM.

===========================================================================*/
void mm_add_message_to_debug_buffer
(
  byte message_set, 
  dword message_id,
  mm_emm_message_direction_type_T mm_emm_message_direction,
  sys_modem_as_id_e_type as_id
)
{
  if(!(is_low_prio_debug_buffer_message(message_set,message_id)))
  {
    if(mm_emm_debug_buffer_index >= MM_DEBUG_MESSAGE_BUFFER_SIZE)
    {
      mm_emm_debug_buffer_index = 0;
    }
    mm_emm_debug_buffer[mm_emm_debug_buffer_index].debug_buffer_type.mm_info_debug_buffer.message_set = message_set;
    mm_emm_debug_buffer[mm_emm_debug_buffer_index].debug_buffer_type.mm_info_debug_buffer.message_id = message_id;
    mm_emm_debug_buffer[mm_emm_debug_buffer_index].present_mm_state= mm_state;
    mm_emm_debug_buffer[mm_emm_debug_buffer_index].present_gmm_state= gmm_state;
#ifdef FEATURE_LTE
    mm_emm_debug_buffer[mm_emm_debug_buffer_index].present_emm_state = EMM_GET_STATE();
#endif
    mm_emm_debug_buffer[mm_emm_debug_buffer_index].mm_emm_message_direction = mm_emm_message_direction;
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
    mm_emm_debug_buffer[mm_emm_debug_buffer_index].as_id = as_id;
    mm_emm_debug_buffer[mm_emm_debug_buffer_index].msg_info = mm_msg_info;
    mm_msg_info = 0;    
#endif
    mm_emm_debug_buffer_index++;
  }
}
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
void mm_update_debug_buffer(void )
{
  if(mm_emm_debug_buffer_index == 0)
  {
    mm_emm_debug_buffer[MM_DEBUG_MESSAGE_BUFFER_SIZE-1].as_id = (sys_modem_as_id_e_type)mm_as_id;
    mm_emm_debug_buffer[MM_DEBUG_MESSAGE_BUFFER_SIZE-1].present_mm_state = mm_state;
    mm_emm_debug_buffer[MM_DEBUG_MESSAGE_BUFFER_SIZE-1].present_gmm_state = gmm_state;
  }
  else if(mm_emm_debug_buffer_index > 0)
  {
    mm_emm_debug_buffer[mm_emm_debug_buffer_index - 1].as_id = (sys_modem_as_id_e_type)mm_as_id;
    mm_emm_debug_buffer[mm_emm_debug_buffer_index - 1].present_mm_state = mm_state;
    mm_emm_debug_buffer[mm_emm_debug_buffer_index - 1].present_gmm_state = gmm_state;
  }
}
#endif



/*===========================================================================

FUNCTION EMM_ADD_MESSAGE_TO_DEBUG_BUFFER

DESCRIPTION
  This function adds messages that MM received and sent to the emm_debug_buffer.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

NOTE
  Parameter "mm_emm_message_direction" indicates whether the particular message is
  sent or received by MM/EMM.

===========================================================================*/
#ifdef FEATURE_LTE
void emm_add_message_to_debug_buffer
(
  msgr_umid_type    msg_umid,
  mm_emm_message_direction_type_T mm_emm_message_direction,
  sys_modem_as_id_e_type as_id
)
{
  if(mm_emm_debug_buffer_index >= MM_DEBUG_MESSAGE_BUFFER_SIZE)
  {
    mm_emm_debug_buffer_index = 0;
  }
  mm_emm_debug_buffer[mm_emm_debug_buffer_index].debug_buffer_type.emm_info_debug_buffer.msg_umid= msg_umid;
  mm_emm_debug_buffer[mm_emm_debug_buffer_index].present_emm_state = EMM_GET_STATE();
  mm_emm_debug_buffer[mm_emm_debug_buffer_index].present_mm_state= mm_state;
  mm_emm_debug_buffer[mm_emm_debug_buffer_index].present_gmm_state= gmm_state;
  mm_emm_debug_buffer[mm_emm_debug_buffer_index].mm_emm_message_direction = mm_emm_message_direction;
 #if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  mm_emm_debug_buffer[mm_emm_debug_buffer_index].as_id = as_id;
  mm_emm_debug_buffer[mm_emm_debug_buffer_index].msg_info = mm_msg_info;
  mm_msg_info = 0;
#endif
  mm_emm_debug_buffer_index++;
}
#endif

/*===========================================================================

FUNCTION MM_PER_NAS_STACKS_IS_MT_PAGE_IN_PROGRESS

===========================================================================*/
boolean  mm_per_nas_stacks_is_mt_page_in_progress
(
  sys_modem_as_id_e_type stack_id_nas
)
{
#if !defined(FEATURE_DUAL_SIM) && !defined (FEATURE_SGLTE)
  (void)stack_id_nas;
  return mm_is_mt_page_in_progress();
#else
  boolean result = FALSE;

  if (IS_NOT_VALID_STACK_ID(stack_id_nas))
  {
    MSG_ERROR_DS(MM_SUB, "=MM= mm_per_nas_stacks_is_mt_page_in_progress - Invalid AS ID: %d", stack_id_nas+1,0,0);
    return FALSE;
  }

#ifdef FEATURE_LTE
  if(mm_csfb_state_sim[stack_id_nas] == MM_CSFB_STATE_MT_CALL_ORIG)
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM=CSFB MT CS PAGING IN PROGRESS");
    result = TRUE;
  }
#endif
  if(((mm_serving_plmn_sim[stack_id_nas].info.active_rat == SYS_RAT_GSM_RADIO_ACCESS) &&
       (mm_last_paging_cause_sim[stack_id_nas] == (byte)CS_PAGING)) ||
       ((mm_serving_plmn_sim[stack_id_nas].info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS ||
         mm_serving_plmn_sim[stack_id_nas].info.active_rat == SYS_RAT_TDS_RADIO_ACCESS) &&
         (mm_last_paging_cause_sim[stack_id_nas] != 0xFF)))
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM=MT CS PAGING IN PROGRESS");
    result = TRUE;
  }
  return result;
#endif
}

/*===========================================================================

FUNCTION MM_PER_STACKS_IS_MT_PAGE_IN_PROGRESS

===========================================================================*/
boolean  mm_per_stacks_is_mt_page_in_progress
(
  sys_modem_as_id_e_type sub_id,
  sys_modem_stack_id_e_type stack_id
)
{
  sys_modem_as_id_e_type stack_id_nas = mm_get_nas_stack_id(sub_id,stack_id);
  return mm_per_nas_stacks_is_mt_page_in_progress(stack_id_nas);
}

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
boolean  mm_per_subs_is_mt_page_in_progress(sys_modem_as_id_e_type as_id)
{
 sys_modem_as_id_e_type stack_id_nas = as_id;
  if(IS_NOT_VALID_SUB_ID(as_id))
  {
    MSG_FATAL_DS(MM_SUB, "=MM= INVALID SUB_ID : %d used for an API call",as_id+1,0,0);
  }
  if(MM_SUB_IS_SGLTE_SUB(as_id))
  {
    stack_id_nas = (sys_modem_as_id_e_type)mm_ps_stack_id;
  }
  return mm_per_nas_stacks_is_mt_page_in_progress(stack_id_nas);
}
#endif
/*===========================================================================

FUNCTION MM_SUB_IS_DDS_SUB

DESCRIPTION
  This function checks whether passed subscription is DDS subscription or not

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_DUAL_SIM
boolean mm_sub_is_dds_sub(sys_modem_as_id_e_type as_id)
{
  if(gmm_state_sim[as_id] == GMM_NULL)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}
#endif
/*===========================================================================

FUNCTION MM_IS_MT_PAGE_IN_PROGRESS

===========================================================================*/

boolean mm_is_mt_page_in_progress()
{
  boolean result = FALSE;

  if((((mm_serving_plmn.info.active_rat == SYS_RAT_GSM_RADIO_ACCESS) &&
           (mm_last_paging_cause == (byte)CS_PAGING)) ||
         ((mm_serving_plmn.info.active_rat == SYS_RAT_UMTS_RADIO_ACCESS ||
           mm_serving_plmn.info.active_rat == SYS_RAT_TDS_RADIO_ACCESS) &&
           (mm_last_paging_cause != 0xFF)))
#ifdef FEATURE_LTE
         ||(mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG)
#endif 
           )
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM=MT CS PAGING IN PROGRESS");
    result = TRUE;
  }

  return result;
}

#ifdef FEATURE_ENHANCED_NW_SELECTION

/*===========================================================================

FUNCTION MM_IS_LAI_IN_FOREGROUND_SEARCH_LAI_TIMER_LIST

DESCRIPTION
  This function checks whether serving lai is in  foreground search lai timer list or not

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_is_lai_in_foreground_search_lai_timer_list(byte *lai)
{
  boolean result = FALSE;
  byte  i;
  for (i = 0; (i < mm_foreground_search_lai_timer_list.length) && (i < MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH); i++)
  {
    if (!memcmp(mm_foreground_search_lai_timer_list.lai_timer_list[i].lai,
                          lai,LAI_SIZE) )
    {
      result = TRUE;
      MSG_HIGH_DS_0(MM_SUB,"=MM= LAI is in fore ground search lai list");
      break;
    }
  }
  return result;
}
/*===========================================================================

FUNCTION MM_ADD_LAI_TO_FOREGROUND_SEARCH_LAI_TIMER_LIST

DESCRIPTION
  This function adds the serving lai to foreground search lai timer list 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_add_lai_to_foreground_search_lai_timer_list(byte *foreground_search_lai)
{
  byte i;

  if (!mm_is_lai_in_foreground_search_lai_timer_list(foreground_search_lai) )
  {
    /*
    ** The LAI is not already contained in the fore ground search done LAIs 
    ** list so simply add it to the bottom of the list.
    */
    if ( mm_foreground_search_lai_timer_list.length< MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH )
    {
      mm_foreground_search_lai_timer_list.length++;
    }
    else
    {
      mm_foreground_search_lai_timer_list.length = MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH;
      for (i = 0; i < (byte)(mm_foreground_search_lai_timer_list.length - 1) ; i++)
      {
         memscpy( mm_foreground_search_lai_timer_list.lai_timer_list[i].lai,LAI_SIZE,
                       mm_foreground_search_lai_timer_list.lai_timer_list[i+1].lai,
                       LAI_SIZE);
         mm_foreground_search_lai_timer_list.lai_timer_list[i].rac = mm_foreground_search_lai_timer_list.lai_timer_list[i+1].rac;
      }
    }

    memscpy( mm_foreground_search_lai_timer_list.lai_timer_list[mm_foreground_search_lai_timer_list.length-1].lai,
                  LAI_SIZE,foreground_search_lai,
                       LAI_SIZE);
    MSG_HIGH_DS_1(MM_SUB ,"=MM= Serving LAI is added to FOREGROUND SEARCH LAI TIMER LIST.length  %d", 
                                                     mm_foreground_search_lai_timer_list.length);
    /* Store LU reject cause for each failure attempts*/
    memscpy( mm_foreground_search_lai_timer_list.lai_timer_list[mm_foreground_search_lai_timer_list.length-1].reject_cause,
                  MAX_LU_ATTEMPT_SIZE, mm_lu_reject_cause,
                  MAX_LU_ATTEMPT_SIZE);

    for (i=0; i< MAX_LU_ATTEMPT_SIZE; i++)
    {
      mm_lu_reject_cause[i] = 0xFF;
    }

#ifdef FEATURE_NAS_REL10
    if(mm_is_per_ms_t3212_present)
    {
      mm_start_foreground_search_timer(mm_stored_per_ms_t3212_value);
    }
    else
#endif
    {
      mm_start_foreground_search_timer(mm_system_information.T3212_timer_value);
    }
#ifdef FEATURE_WCDMA
     mm_send_rrc_forbidden_lai_update();
#endif

#ifdef FEATURE_GSM
     mm_send_rr_forbidden_lai_update();
#endif
  }
}
/*===========================================================================

FUNCTION MM_REMOVE_LAI_FROM_FOREGROUND_SEARCH_LAI_TIMER_LIST

DESCRIPTION
  This function removes the serving lai from foreground search lai timer list 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_remove_lai_from_foreground_search_lai_list(byte * serving_lai)
{
  byte i;
  boolean found = FALSE;
 
  for(i = 0; (i < mm_foreground_search_lai_timer_list.length) && (i < MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH); i++)
  {
    if (!memcmp(mm_foreground_search_lai_timer_list.lai_timer_list[i].lai,
                          serving_lai,LAI_SIZE) )
    {
      found = TRUE;
      break;
    }
  }

  if(found == TRUE)
  {
    mm_foreground_search_lai_timer_list.length--;
    for (; (i < mm_foreground_search_lai_timer_list.length) && (i < MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH-1); i++)
    {
        mm_foreground_search_lai_timer_list.lai_timer_list[i] =
             mm_foreground_search_lai_timer_list.lai_timer_list[i+1];
    }
#ifdef FEATURE_WCDMA
    mm_send_rrc_forbidden_lai_update();
#endif

#ifdef FEATURE_GSM
     mm_send_rr_forbidden_lai_update();
#endif
    MSG_HIGH_DS_0(MM_SUB,"=MM= Serving LAI is removed from FOREGROUND SEARCH lai list");
  }
  if (mm_foreground_search_lai_timer_list.length == 0)
  {
    mm_stop_timer(TIMER_FOREGROUND_SEARCH);
  }
}
/*===========================================================================

FUNCTION MM_CLEAR_FOREGROUND_SEARCH_LAI_TIMER_LIST

DESCRIPTION
  This function CLEARS  foreground search lai timer list 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_clear_foreground_search_lai_list(void)
{
  MSG_HIGH_DS_0(MM_SUB,"=MM= Cleared FOREGROUND SEARCH lai list");
  mm_foreground_search_lai_timer_list.length =0;

  mm_stop_timer(TIMER_FOREGROUND_SEARCH);
#ifdef FEATURE_WCDMA
  mm_send_rrc_forbidden_lai_update();
#endif

#ifdef FEATURE_GSM
  mm_send_rr_forbidden_lai_update();
#endif 
}
/*===========================================================================

FUNCTION MM_PROCESS_FOREGROUND_SEARCH_LAI_TIMER_EXPIRY

DESCRIPTION
  This function handles the foreground search timer expiry

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_process_foreground_seach_tmer_expiry(void)
{
  byte i = 0;
  byte index = 0;
  dword  min_timer_count = 0;

  while( (i < mm_foreground_search_lai_timer_list.length) && (i < MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH))
  {
    if (mm_foreground_search_lai_timer_list.lai_timer_list[i].timer_count == 0)
    {
      index =i;
      mm_foreground_search_lai_timer_list.length--;
      for(; ( index <mm_foreground_search_lai_timer_list.length) &&((index + 1) < MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH); index++)
      {
        mm_foreground_search_lai_timer_list.lai_timer_list[index] = mm_foreground_search_lai_timer_list.lai_timer_list[index+1];
      }
    }
    else
    {
      i++;
    }
  }

  for(i=0; (i < mm_foreground_search_lai_timer_list.length) &&(i < MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH); i++)
  {
    if((min_timer_count == 0) ||
       (min_timer_count > mm_foreground_search_lai_timer_list.lai_timer_list[i].timer_count))
    {
      min_timer_count = mm_foreground_search_lai_timer_list.lai_timer_list[i].timer_count;
    }
  }

  if(mm_foreground_search_lai_timer_list.length != 0)
  {
    for(i=0; (i < mm_foreground_search_lai_timer_list.length) &&(i < MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH); i++)
    {
      mm_foreground_search_lai_timer_list.lai_timer_list[i].timer_count = 
          (mm_foreground_search_lai_timer_list.lai_timer_list[i].timer_count - min_timer_count);
    }
    mm_start_foreground_search_timer(min_timer_count);
  }
#ifdef FEATURE_WCDMA
  mm_send_rrc_forbidden_lai_update();
#endif

#ifdef FEATURE_GSM
  mm_send_rr_forbidden_lai_update();
#endif 
}
/*===========================================================================

FUNCTION MM_GET_FOREGROUND_SEARCH_LAI_TIMER_EXPIRY_LAI

DESCRIPTION
  This function gives the lai whose foreground search timer got expired

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_get_foreground_timer_expiry_lai(byte *foreground_timer_expiry_lai)
{

  uint32 i = 0;

  while ( (i < mm_foreground_search_lai_timer_list.length) && (i < MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH))
  {
    if (mm_foreground_search_lai_timer_list.lai_timer_list[i].timer_count == 0)
    {
      if (mm_is_lai_in_foreground_search_lai_timer_list(mm_foreground_search_lai_timer_list.lai_timer_list[i].lai))
      {
        memscpy(foreground_timer_expiry_lai,LAI_SIZE,mm_foreground_search_lai_timer_list.lai_timer_list[i].lai,LAI_SIZE);
        break;
      }
    }
    else
    {
      i++;
    }
  }
}

/*===========================================================================

FUNCTION MM_PRINT_LU_REJECT_CAUSE_FOR_LAI_IN_FOREGROUND_SEARCH_LAI_LIST

DESCRIPTION
  This function gives the reject cause on each failure attempts for lai present in foreground list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_print_lu_reject_cause_for_lai_in_foreground_search_lai_list(byte * serving_lai)
{
  byte    i,j;
  boolean found = FALSE;
  dword   timeout_value =0;

  for(i = 0; (i < mm_foreground_search_lai_timer_list.length) && (i < MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH); i++)
  {
    if (!memcmp(mm_foreground_search_lai_timer_list.lai_timer_list[i].lai,
                          serving_lai,LAI_SIZE) )
    {
      found = TRUE;
      break;
    }
  }
  
  if(found == TRUE)
  {
    MSG_HIGH_DS_2(MM_SUB, "=MM= Serving LAI [%X %X] is in FOREGROUND SEARCH LAI TIMER LIST",mm_foreground_search_lai_timer_list.lai_timer_list[i].lai[3],mm_foreground_search_lai_timer_list.lai_timer_list[i].lai[4]);
                                                                                
    for (j = 0; j < MAX_LU_ATTEMPT_SIZE; j++)
    {
      MSG_HIGH_DS_2(MM_SUB, "=MM= LU fail Attempt : %d with reject cause : %d", j, mm_foreground_search_lai_timer_list.lai_timer_list[i].reject_cause[j]);
    }                   
              /* Find out how much timeout is left */
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
     timeout_value = (dword) gs_enquire_timer( get_mm_timer_queue(),
                                                MM_TIMER_TAG_AS_ID(TIMER_FOREGROUND_SEARCH));
#else
     timeout_value = (dword) gs_enquire_timer( get_mm_timer_queue(), TIMER_FOREGROUND_SEARCH );
#endif

     timeout_value += mm_foreground_search_lai_timer_list.lai_timer_list[i].timer_count;

     MSG_HIGH_DS_1(MM_SUB ,"=MM= Foreground search timer value remaining for serving LAI : %d ", 
                                                                                             timeout_value);

   }
  
}

/*===========================================================================

FUNCTION MM_REMOVE_PS_REGISTERED_LAI_FROM_FOREGROUND_SEARCH_LAI_TIMER_LIST

DESCRIPTION
  This function removes PS registered LAI from forbidden list and informs updated foreground search lai  list to lower layer

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_remove_ps_registered_lai_from_foreground_search_lai_list(void)
{
    uint32 i = 0,j = 0;
    boolean found = FALSE;

    for(i = 0; (i < mm_foreground_search_lai_timer_list.length) && (i < MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH); i++)
    {
      if (!memcmp(mm_foreground_search_lai_timer_list.lai_timer_list[i].lai,
                  mm_serving_lai,LAI_SIZE) )
      {
        found = TRUE;
        break;
      } 
    }
  
    if (found == TRUE && (mm_foreground_search_lai_timer_list.lai_timer_list[i].is_ps_registered == FALSE))
    {
      for (j = 0; (j < mm_foreground_search_lai_timer_list.length) && (j < MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH); j++)
      {
        if (mm_foreground_search_lai_timer_list.lai_timer_list[j].is_ps_registered == TRUE)
        {
          mm_foreground_search_lai_timer_list.lai_timer_list[j].is_ps_registered = FALSE;
        }
      }

      MSG_HIGH_DS_0(MM_SUB,"=MM= Inform PS Attach/RAU succesful LAI as non-forbidden to lower layer");
      mm_foreground_search_lai_timer_list.lai_timer_list[i].is_ps_registered = TRUE;

#ifdef FEATURE_WCDMA
      mm_send_rrc_forbidden_lai_update();
#endif

#ifdef FEATURE_GSM
      mm_send_rr_forbidden_lai_update();
#endif

    }
}

/*===========================================================================

FUNCTION MM_INFORM_UPDATED_FOREGROUND_SEARCH_LIST

DESCRIPTION
  This function sends the original forbidden LAI after restoring the PS registered forbidden LAI to  foreground search lai timer list 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_inform_updated_foreground_search_list(void)
{
  uint32 i = 0;
  boolean status = FALSE;

 if (mm_serving_plmn.info.active_rat != SYS_RAT_LTE_RADIO_ACCESS)
 {
   for (i = 0; (i < mm_foreground_search_lai_timer_list.length) && (i < MAX_FOREGROUND_SEARCH_LAI_TIMER_LIST_LENGTH); i++)
   {
     if (mm_foreground_search_lai_timer_list.lai_timer_list[i].is_ps_registered == TRUE)
     {
       MSG_HIGH_DS_0(MM_SUB,"=MM= Restore forbidden list ");
       mm_foreground_search_lai_timer_list.lai_timer_list[i].is_ps_registered = FALSE;
       status = TRUE;
     }
   }
 }
 else
 {
   MSG_HIGH_DS_0(MM_SUB,"=MM= Retain original forbidden list ");
   status = TRUE;
 }
 
  if(status)
  {
#ifdef FEATURE_WCDMA
    mm_send_rrc_forbidden_lai_update();
#endif

#ifdef FEATURE_GSM
    mm_send_rr_forbidden_lai_update();
#endif
  }
}

#endif

/*===========================================================================

FUNCTION MM_IS_PLMN_VALID

DESCRIPTION
  This function checks if MCC/MNC does not consist of hexadecimal digits.
  ASN encoder is known to crash on violation, but spec allows hexadecimal digit.
  Table 10.5.3/3GPP TS 24.008 : The UE must send the MCC containing the hexadecimal digit.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

NOTE
  None

===========================================================================*/

boolean mm_is_plmn_valid
(
  sys_plmn_id_s_type  plmn
)
{
  return (boolean)( ( (byte)(plmn.identity[0] & 0x0F) <= 0x9 ) &&
                    ( (byte)(plmn.identity[0] / 0x10) <= 0x9 ) &&
                    ( (byte)(plmn.identity[1] & 0x0F) <= 0x9 ) &&
                    ( (byte)(plmn.identity[2] & 0x0F) <= 0x9 ) &&
                    ( (byte)(plmn.identity[2] / 0x10) <= 0x9 ) &&
                    ( ((byte)(plmn.identity[1] / 0x10) <= 0x9 ) || ((byte)(plmn.identity[1] / 0x10) == 0xF )));
}
#ifdef FEATURE_DUAL_SIM
/*===========================================================================

FUNCTION MM_PER_SUBS_IS_CURRENT_PLMN_FPLMN

DESCRIPTION
  This function checks if current PLMN is FPLMN or not

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN

SIDE EFFECTS
  None

NOTE
  None

===========================================================================*/
boolean mm_per_subs_is_current_plmn_fplmn(sys_modem_as_id_e_type as_id)
{
  boolean is_plmn_forbidden = FALSE;
  if(IS_NOT_VALID_AS_ID(as_id))
  {
    MSG_ERROR_DS(MM_SUB, "=MM= mm_per_subs_is_current_plmn_fplmn : Invalid AS ID:%d", as_id+1,0,0);
    return is_plmn_forbidden;
  } 
  if ((reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn,as_id )) ||
     (mm_check_forbidden_national_roaming( &gmm_current_lai )) ||
     (mm_check_forbidden_regional_roaming( &gmm_current_lai )) ||
     (reg_sim_per_subs_plmn_forbidden(mm_serving_plmn.info.plmn,as_id ))||
     (gmm_check_gprs_forbidden_rai(mm_serving_plmn.info.plmn,mm_serving_plmn.lac,mm_serving_plmn.rac))
     )
     {
        MSG_HIGH_DS_3(MM_SUB, "=MM= Selected PLMN %X %X %X is FPLMN", mm_serving_plmn.info.plmn.identity[0], mm_serving_plmn.info.plmn.identity[1], mm_serving_plmn.info.plmn.identity[2]);
        is_plmn_forbidden = TRUE;
     }
     else
     {
       MSG_HIGH_DS_3(MM_SUB, "=MM= Selected PLMN %X %X %X is not a FPLMN", mm_serving_plmn.info.plmn.identity[0], mm_serving_plmn.info.plmn.identity[1], mm_serving_plmn.info.plmn.identity[2]);        
     }
  return is_plmn_forbidden;
}
#else
/*===========================================================================

FUNCTION MM_IS_CURRENT_PLMN_FPLMN

DESCRIPTION
  This function checks if current PLMN is FPLMN or not

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN

SIDE EFFECTS
  None

NOTE
  None

===========================================================================*/

boolean mm_is_current_plmn_fplmn ( void )
{
    boolean is_plmn_forbidden = FALSE;
    if ((reg_sim_plmn_gprs_forbidden( mm_serving_plmn.info.plmn )) ||
        (mm_check_forbidden_national_roaming( &gmm_current_lai )) ||
        (mm_check_forbidden_regional_roaming( &gmm_current_lai )) ||
        (reg_sim_plmn_forbidden(mm_serving_plmn.info.plmn))||
        (gmm_check_gprs_forbidden_rai(mm_serving_plmn.info.plmn,mm_serving_plmn.lac,mm_serving_plmn.rac))
        )
        {
          MSG_HIGH_DS_3(MM_SUB, "=MM= Selected PLMN %x %x %x is FPLMN", mm_serving_plmn.info.plmn.identity[0], mm_serving_plmn.info.plmn.identity[1], mm_serving_plmn.info.plmn.identity[2]);
          is_plmn_forbidden = TRUE;
        }
        else
        {
          MSG_HIGH_DS_3(MM_SUB, "=MM= Selected PLMN %x %x %x is not a FPLMN", mm_serving_plmn.info.plmn.identity[0], mm_serving_plmn.info.plmn.identity[1], mm_serving_plmn.info.plmn.identity[2]);        
        }
    return is_plmn_forbidden;
}
#endif
/*===========================================================================

  FUNCTION:  mm_mem_alloc

===========================================================================*/
/*!
    @brief
    Allocates a block of size bytes from the heap.

    @return
    Returns a pointer to the newly allocated block, or NULL if the block
    could not be allocated.

*/
/*=========================================================================*/
void* mm_mem_alloc( size_t size /*!< Number of bytes to allocate */ )
{
#ifndef FEATURE_MODEM_HEAP
  return gs_alloc( size );
#else
  return modem_mem_calloc( 1, size, MODEM_MEM_CLIENT_NAS );
#endif
}


/*===========================================================================

  FUNCTION:  mm_check_for_null_ptr

===========================================================================*/
/*!
    @brief
    calls msg_fatal if calling parameter is a null ptr.

    @return

*/
/*=========================================================================*/
void mm_check_for_null_ptr(void* msg_p)
{
  if(msg_p == NULL)
  {
    MSG_FATAL_DS(MM_SUB,"=MM= Error: Not able to Allocate MM Buffer", 0, 0, 0);
  }
}

/*===========================================================================

FUNCTION  MM_GET_RRC_CMD_BUF

DESCRIPTION
  This function returns a rrc_cmd_type buffer to the calling function.

  This function dynamically allocates memory for the necessary buffer.

  The freeing is done by the function that dequeues and processes this
  command buffer.

  Calling function MUST check for a NULL return value and take necessary
  action.

DEPENDENCIES
  None 

RETURN VALUE
  Returns a buffer of type rrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS
  None

===========================================================================*/
#if (defined FEATURE_WCDMA || defined FEATURE_TDSCDMA)
rrc_cmd_type *mm_get_rrc_cmd_buf( sys_radio_access_tech_e_type rat )
{
  rrc_cmd_type *rrc_cmd_ptr = NULL;
#ifdef FEATURE_SEGMENT_LOADING
  interface_t *ptr_W = get_wcdma_interface ();
  if ((rat == SYS_RAT_TDS_RADIO_ACCESS) && get_tdscdma_interface())
  {
    rrc_cmd_ptr = (rrc_cmd_type*) mm_mem_alloc( sizeof( rrc_cmd_type ));
  }
  /* Note that there are cases where the command will be sent to WRRC
  ** even if the active RAT is GSM or LTE
  */
  else if ((rat == SYS_RAT_UMTS_RADIO_ACCESS) && (ptr_W != NULL))
  {  
    rrc_cmd_ptr = IWCDMA_rrc_get_mm_cmd_buf(ptr_W);
  }
  else
  {      
    if (ptr_W != NULL)
    {
        rrc_cmd_ptr = IWCDMA_rrc_get_mm_cmd_buf(ptr_W);
    }
    else
    {
        rrc_cmd_ptr = (rrc_cmd_type*) mm_mem_alloc( sizeof( rrc_cmd_type ));
    }
   }
#else
  if (rat == SYS_RAT_TDS_RADIO_ACCESS)
  {
    rrc_cmd_ptr = (rrc_cmd_type*) mm_mem_alloc( sizeof( rrc_cmd_type ));
  }
  /* Note that there are cases where the command will be sent to WRRC
  ** even if the active RAT is GSM or LTE
  */
  else
  {  
    rrc_cmd_ptr = rrc_get_mm_cmd_buf();
  }
#endif
  return rrc_cmd_ptr;
}
#endif /* FEATURE_WCDMA */

#ifdef FEATURE_LTE
/*===========================================================================

FUNCTION  mm_is_lte_reselection_allowed

DESCRIPTION
        This function checks if resleciton to LTE is allowed or not. Reslection from UMTS/GSM to LTE is allowed if:
             a) UE camps on GPRS forbidden PLMN
             b) UE camps on PLMN which has GPRS forbidden PLMN as EPLM


DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_is_lte_reselection_allowed(sys_plmn_id_s_type plmn_id)
{
  boolean lte_forbidden = TRUE;

  sys_plmn_id_s_type eplmn_rplmn;
  sys_plmn_list_s_type eplmn_list;
  uint32 index;

  /*Block LTE reselection if UE is camped on GPRS forbidden PLMN*/
  if(
#ifdef FEATURE_DUAL_SIM
    reg_sim_plmn_gprs_forbidden(plmn_id,(sys_modem_as_id_e_type)mm_sub_id)
#else
    reg_sim_plmn_gprs_forbidden(plmn_id)
#endif
     || (mm_serving_plmn.info.sim_state == SYS_SIM_STATE_PS_INVALID)
#ifdef FEATURE_DUAL_DATA
     || (mm_is_volte_registration_allowed(plmn_id, TRUE) == FALSE)
#endif
     )
  {
    lte_forbidden = FALSE;
  }
  else
  {
    /* Block LTE reselection if camped PLMN and a GPRS forbidden PLMN is present in EPLMN list*/
    if(sys_eplmn_list_equivalent_plmn(plmn_id))
    {
      sys_eplmn_list_get(&eplmn_list, &eplmn_rplmn);
 
#ifdef FEATURE_DUAL_SIM
      if(reg_sim_plmn_gprs_forbidden(eplmn_rplmn,(sys_modem_as_id_e_type)mm_sub_id))
#else
      if(reg_sim_plmn_gprs_forbidden(eplmn_rplmn))
#endif
      {
        lte_forbidden = FALSE;     
      }
      else
      {
        for(index = 0; index < eplmn_list.length; index++)
        {
#ifdef FEATURE_DUAL_SIM
          if(reg_sim_plmn_gprs_forbidden(eplmn_list.plmn[index],(sys_modem_as_id_e_type)mm_sub_id))
#else
          if(reg_sim_plmn_gprs_forbidden(eplmn_list.plmn[index]))
#endif
          {
            lte_forbidden = FALSE;
            break;
          }          
        }
      }
    }
  }
  MSG_HIGH_DS_1(MM_SUB ,"=MM= LTE reselection status = %d", lte_forbidden);
  return lte_forbidden;
}

#ifdef FEATURE_DUAL_DATA
/*===========================================================================

FUNCTION  mm_is_volte_registration_allowed

DESCRIPTION
        This function checks if X2L/L2L Inter-RAT or LTE selection or LTE/PS registration is allowed or not when LTE is in non DDS SUB. Reslection from UMTS/GSM to LTE is allowed if:
             a) UE camps on PLMN present in “non DDS VOLTE allowed PLMN list”
             b) UE camps on PLMN which is equivalent PLMN to a PLMN present in "“non DDS VOLTE allowed PLMNs”.
             c) PM will return whether current PLMN is present in "non DDS VOLTE allowed PLMN list” 

           When LTE is non DDS SUB , NAS will enable LTE capability in OTA path & mode preference to Access Stratum.
                      a. On Non-EPLMN to “non DDS VOLTE allowed PLMNs” 
                                  i. NAS will disable LTE reselection and L-RRC will reject redirection based on PM API.

                      b. On EPLMN to “non DDS VOLTE allowed PLMNs” 
                                  i. NAS will keep LTE reselection/redirection allowed.
                                 iii. LTE RRC will use PM API to reject any inter-RATto PLMN which is not poresent in “non DDS VOLTE allowed PLMN list” .

DEPENDENCIES
 PM will  return whether current queried PLMN is present in “non DDS VOLTE allowed PLMNs”.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_is_volte_registration_allowed(sys_plmn_id_s_type plmn_id, boolean eplmn_allowed)
{
  boolean ps_allowed = FALSE;
  const char  *BooleanName  = "nas:Lte_Only_Operator";
  const char  *pPlmnSetName = "nondds_volte_plmns";
  boolean     status = FALSE, value  = FALSE;

  sys_plmn_id_s_type eplmn_rplmn;
  sys_plmn_list_s_type eplmn_list;
  uint32 index;

  /*Follow Legacy behavior in case LTE is DDS subscription or 
      "VoLTE on Non-DDS SUB" feature is disabled */

  status = policyman_get_subs_boolean_value(BooleanName,(sys_modem_as_id_e_type)mm_sub_id ,&value);
  
  if ((mm_is_volte_only_ps == FALSE) ||
      (status == FALSE || value == FALSE))
  {
    ps_allowed = TRUE;
  }
  else if (policyman_is_plmn_in_list(pPlmnSetName,&plmn_id, (sys_modem_as_id_e_type)mm_sub_id ))
  {
    /*Allow X2L/L2L Inter-RAT or selection or LTE/PS registration 
         if camped PLMN is present in "Non DDS VOLTE allowed PLMN list" */
    ps_allowed = TRUE;
  }
  else if (eplmn_allowed)
  {
    /*Allow X2L/L2L Inter-RAT or selection or LTE/PS registration if any of the
         equivalent PLMN of camped PLMN is present in "Non DDS VOLTE allowed PLMN list" */

    if(sys_eplmn_list_equivalent_plmn(plmn_id))
    {
      sys_eplmn_list_get(&eplmn_list, &eplmn_rplmn);
 
      if (policyman_is_plmn_in_list(pPlmnSetName,&eplmn_rplmn, (sys_modem_as_id_e_type)mm_sub_id ))
      {
        ps_allowed = TRUE;
      }
      else
      {
        for(index = 0; index < eplmn_list.length; index++) 
        {
          if (policyman_is_plmn_in_list(pPlmnSetName,&eplmn_list.plmn[index], (sys_modem_as_id_e_type)mm_sub_id ))
          {
            ps_allowed = TRUE;
            break;
          }          
        }
      }
    }
  }

  MSG_HIGH_DS_2(MM_SUB ,"=MM= VoLTE Registration allowed = %d volte_only = %d", ps_allowed, mm_is_volte_only_ps);
  return ps_allowed;
}
#endif

#endif

/*===========================================================================

FUNCTION  mm_add_plmn_to_fplmn_list

DESCRIPTION
If PLMN is not present in FPLMN list the this function add PLMN to FPLMN list and send change indication to REG


DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void mm_add_plmn_to_fplmn_list
(
  sys_plmn_id_s_type plmn
)
{

#ifdef FEATURE_DUAL_SIM
  if(reg_sim_add_plmn_to_fplmn_list( mm_serving_plmn.info.plmn, (sys_modem_as_id_e_type)mm_sub_id))
#else
  if (reg_sim_add_plmn_to_fplmn_list(plmn)) 
#endif /*FEATURE_DUAL_SIM */
  {  
  #ifdef FEATURE_LTE 
    mm_send_mmr_fplmn_list_change_ind(SYS_FORBIDDEN_LIST_TYPE_FPLMN);
  #endif /*FEATURE_LTE*/
  }
}


/*===========================================================================

FUNCTION  mm_remove_plmn_from_fplmn_list

DESCRIPTION
If PLMN is present in FPLMN list the this function remove PLMN to FPLMN list and send change indication to REG


DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void mm_remove_plmn_from_fplmn_list
(
  sys_plmn_id_s_type plmn
)
{

#ifdef FEATURE_DUAL_SIM
  if (reg_sim_remove_plmn_from_fplmn_list( mm_serving_plmn.info.plmn, (sys_modem_as_id_e_type)mm_sub_id))
#else
  if (reg_sim_remove_plmn_from_fplmn_list(plmn))
#endif /*FEATURE_DUAL_SIM */
  {   
  #ifdef FEATURE_LTE 
    mm_send_mmr_fplmn_list_change_ind(SYS_FORBIDDEN_LIST_TYPE_FPLMN);
  #endif /*FEATURE_LTE*/
    MSG_HIGH_DS_3(MM_SUB, "=MM= Removing PLMN 0x%2x 0x%2x 0x%2x from forbidden lists",
	          plmn.identity[0],plmn.identity[1],plmn.identity[2]);
  }
}



/*===========================================================================

FUNCTION  mm_add_plmn_to_gprs_fplmn_list

DESCRIPTION
If PLMN is not present in GPRS FPLMN list then this function add PLMN to GPRS FPLMN list and send change indication to REG


DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void mm_add_plmn_to_gprs_fplmn_list
(
  sys_plmn_id_s_type plmn
)
{
#ifdef FEATURE_DUAL_SIM
  if (reg_sim_add_plmn_to_gprs_fplmn_list( mm_serving_plmn.info.plmn, (sys_modem_as_id_e_type)mm_sub_id))
#else
  if (reg_sim_add_plmn_to_gprs_fplmn_list(plmn))
#endif /*FEATURE_DUAL_SIM */
  {
  #ifdef FEATURE_LTE 
    mm_send_mmr_fplmn_list_change_ind(SYS_FORBIDDEN_LIST_TYPE_GPRS_FPLMN);
  #endif /*FEATURE_LTE*/
  }
}


#ifdef FEATURE_FEMTO_CSG

/*===========================================================================

FUNCTION  mm_is_csg_in_allowed_list

DESCRIPTION


DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_is_csg_in_allowed_list
(
  sys_plmn_id_s_type plmn_id,
  sys_csg_id_type   csg_id
)
{
  uint16  index;
  boolean found = FALSE;

  if(csg_id == SYS_CSG_ID_INVALID)
  {
    found = TRUE;
  }  
  else 
  {
    for (index = 0; index < mm_sim_active_allowed_csg_list.length; index++)
    {
      if((csg_id == mm_sim_active_allowed_csg_list.csg[index].csg_id) &&
          (sys_plmn_match(plmn_id, mm_sim_active_allowed_csg_list.csg[index].plmn)))
      {
        found = TRUE;
        break;
      }
    }
  }

  return found;
}

/*===========================================================================

FUNCTION  mm_is_csg_in_operator_list

DESCRIPTION

  This function checks if CSG,PLMN is present in operator CSG list

DEPENDENCIES
  None 

RETURN VALUE
  TRUE: If CSG,PLMN is present
  FALSE: IF CSG,PLMN is not present

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_is_csg_in_operator_list
(
  sys_plmn_id_s_type plmn_id,
  sys_csg_id_type   csg_id,
  boolean           allow_rejected
)
{
  uint16  index;
  boolean found = FALSE;

  if(csg_id == SYS_CSG_ID_INVALID)
  {
    found = TRUE;
  }
  else 
  {
    for (index = 0; index < mm_sim_active_operator_csg_list.length; index++)
    {
      if((csg_id == mm_sim_active_operator_csg_list.csg[index].csg_id) &&
          (sys_plmn_match(plmn_id, mm_sim_active_operator_csg_list.csg[index].plmn)) &&
          ((mm_sim_active_operator_csg_list.csg[index].timer_count == MM_TIMER_DISABLED)
            || (allow_rejected == TRUE)))
      {
        found = TRUE;
        break;
      }
    }
  }
  return found;
}


/*===========================================================================

FUNCTION  mm_is_csg_in_white_list

DESCRIPTION

  This function finds out if CSG,PLMN is present in white list

DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_is_csg_in_white_list
(
  sys_plmn_id_s_type plmn_id,
  sys_csg_info_s_type *csg_info
)
{
  boolean found = FALSE;

  if((csg_info->csg_id == SYS_CSG_ID_INVALID) || (csg_info->hybrid_cell == TRUE))
  {
    found = TRUE;
  }
  else if(mm_is_csg_in_allowed_list(plmn_id, csg_info->csg_id) ||
     mm_is_csg_in_operator_list(plmn_id, csg_info->csg_id,FALSE))
  {
    found = TRUE;
  }
  return found;
}

/*===========================================================================

FUNCTION  mm_csg_plmn_display_requirment

DESCRIPTION



DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_csg_plmn_display_requirment(sys_plmn_id_s_type plmn)
{
  uint16  index;

  boolean  display_all = TRUE;
  boolean found = FALSE;

  for(index = 0; index < mm_sim_active_csg_indicator_list.length; index++)
  {
    if(sys_plmn_match(plmn, mm_sim_active_csg_indicator_list.list[index].plmn)) 
    {
      if(mm_sim_active_csg_indicator_list.list[index].indicator == 1)
      {
        display_all = FALSE;
      }
      found = TRUE;
      break;
    }
  }

  if(found == FALSE)
  {
    if(mm_ad_csg_indicator == 1)
    {
      display_all = FALSE;
    }
  }
  return display_all;
}

/*===========================================================================

FUNCTION  mm_prioritize_csg_list

DESCRIPTION
This function prirotizes CSG list as per order given in 23.122


DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void mm_prioritize_csg_list
(
  sys_detailed_plmn_list_s_type  *available_csg_p
)
{

  uint32  temp_list_length = 0;

  uint32  i;

  uint32  j;
  sys_detailed_plmn_list_info_s_type temp_csg_info;


  /*Get the CSG belonging to allowed list*/
  for(i = 0; i < mm_sim_active_allowed_csg_list.length; i++)
  {
     for(j = temp_list_length; j < available_csg_p->length; j++)
     {
       if(sys_plmn_match(available_csg_p->info[j].plmn, mm_sim_active_allowed_csg_list.csg[i].plmn) 
          && (available_csg_p->info[j].csg_info.csg_id == mm_sim_active_allowed_csg_list.csg[i].csg_id)
          && (mm_csg_plmn_display_requirment(available_csg_p->info[j].plmn))
       )
       {
            temp_csg_info = available_csg_p->info[temp_list_length];
            available_csg_p->info[temp_list_length] = available_csg_p->info[j];
            available_csg_p->info[j] = temp_csg_info;
            
            available_csg_p->info[temp_list_length].list_category = SYS_DETAILED_PLMN_LIST_ALLOWED_CSG;
            temp_list_length++;
       }
     }
  }

  for(i = 0; i < mm_sim_active_operator_csg_list.length; i++)
  {
     if(mm_sim_active_operator_csg_list.csg[i].timer_count != MM_TIMER_DISABLED)
     {
       continue;
     }
     for(j = temp_list_length; j < available_csg_p->length; j++)
     {
       if(sys_plmn_match(available_csg_p->info[j].plmn, mm_sim_active_operator_csg_list.csg[i].plmn) 
          && (available_csg_p->info[j].csg_info.csg_id == mm_sim_active_operator_csg_list.csg[i].csg_id))
       {
            temp_csg_info = available_csg_p->info[temp_list_length];
            available_csg_p->info[temp_list_length] = available_csg_p->info[j];
            available_csg_p->info[j] = temp_csg_info;
            
            available_csg_p->info[temp_list_length].list_category = SYS_DETAILED_PLMN_LIST_OPERATOR_CSG;
            temp_list_length++;
       }
     }
  }

  for(j = temp_list_length; j < available_csg_p->length; j++)
  {
    if(mm_csg_plmn_display_requirment(available_csg_p->info[j].plmn))
    {
      available_csg_p->info[j].list_category = SYS_DETAILED_PLMN_LIST_OTHER_CSG;
    }
    else
    {
      if(j < (available_csg_p->length - 1))
      {
        available_csg_p->info[j] = available_csg_p->info[available_csg_p->length - 1];
        j--;
      }
      available_csg_p->length = available_csg_p->length - 1;     
    }
  }  
 
}

/*===========================================================================

FUNCTION  mm_add_csg_to_forbidden_operator_list

DESCRIPTION
This function add CSG to fobidden operator list

DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
boolean mm_add_csg_to_forbidden_operator_list
(
  sys_plmn_id_s_type  plmn,
  sys_csg_id_type   csg_id
)
{
  uint16 index = 0;
  dword  curr_timer_count, remain_timer_count;
  boolean list_modified = FALSE;
 
  for(index = 0; index < mm_sim_active_operator_csg_list.length; index++)
  {
    if((csg_id == mm_sim_active_operator_csg_list.csg[index].csg_id) &&
          (sys_plmn_match(plmn, mm_sim_active_operator_csg_list.csg[index].plmn))) 
    {
      if(mm_sim_active_operator_csg_list.csg[index].timer_count == MM_TIMER_DISABLED)
      {
         curr_timer_count = (dword) gs_enquire_timer( get_mm_timer_queue(), TIMER_FORBIDDEN_OPERATOR_CSG_LIST);

        if(curr_timer_count == 0)
        {
          mm_start_timer(TIMER_FORBIDDEN_OPERATOR_CSG_LIST, DEFAULT_TIMEOUT);
          curr_timer_count = mm_get_timeout(TIMER_FORBIDDEN_OPERATOR_CSG_LIST);
        }
        remain_timer_count = mm_get_timeout(TIMER_FORBIDDEN_OPERATOR_CSG_LIST) - curr_timer_count;
        MSG_HIGH_DS_0(MM_SUB,"Added to forbidden CSG operator list");
        mm_sim_active_operator_csg_list.csg[index].timer_count = remain_timer_count;
        list_modified = TRUE;
      }
      break;
    }
  }  
  return list_modified;
}


/*===========================================================================

FUNCTION  mm_timer_frobidden_csg_list_expiry

DESCRIPTION
This function handles timer expiry for temp blockign operator CSG list

DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

void mm_timer_forbidden_csg_list_expiry
(
  void
)
{
  uint16 index = 0;
  boolean list_modified = FALSE;
  dword least_time = MM_TIMER_DISABLED;

#ifdef FEATURE_LTE
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif
  for(index = 0; index < mm_sim_active_operator_csg_list.length; index++)
  {
    if(mm_sim_active_operator_csg_list.csg[index].timer_count == 0)
    {
       mm_sim_active_operator_csg_list.csg[index].timer_count = MM_TIMER_DISABLED;
       list_modified = TRUE;
    }
    else if(mm_sim_active_operator_csg_list.csg[index].timer_count != MM_TIMER_DISABLED)
    {
      if(least_time == MM_TIMER_DISABLED)
      {
        least_time = mm_sim_active_operator_csg_list.csg[index].timer_count;
      }
      else if(least_time > mm_sim_active_operator_csg_list.csg[index].timer_count)
      {
        least_time = mm_sim_active_operator_csg_list.csg[index].timer_count;
      }
    }
  }
  /*if timer is not disabled restart timer*/
  if(least_time != MM_TIMER_DISABLED)
  {
    for(index = 0; index < mm_sim_active_operator_csg_list.length; index++)
    {
      if(mm_sim_active_operator_csg_list.csg[index].timer_count != MM_TIMER_DISABLED)
      {
        mm_sim_active_operator_csg_list.csg[index].timer_count -= least_time;
      }
    }
    mm_start_timer(TIMER_FORBIDDEN_OPERATOR_CSG_LIST, DEFAULT_TIMEOUT);
  }

  if(list_modified)
  {
    mm_output_detailed_csg_list();
    mm_send_rrc_csg_white_list_ind(&mm_sim_active_operator_csg_list, &mm_sim_active_allowed_csg_list);
    mm_send_rr_csg_white_list_ind();
#ifdef FEATURE_LTE
    emm_send_rrc_system_update(emm_ctrl_data_ptr);
#endif   
    mmsend_mm_csg_white_list_log_packet(NAS_LOG_OCSGL_TYPE, &mm_sim_active_operator_csg_list); 
  }
}


/*===========================================================================

FUNCTION  mm_delete_csg_from_white_list

DESCRIPTION
This function deletes CSG,PLMN from white list. If CSG is present in OCSGL then forbidden timer will be started. 
If CSG is present in ACSGL then it will be removed from list. 

DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void mm_delete_csg_from_white_list
(
  sys_plmn_id_s_type plmn_id,
  sys_csg_info_s_type  *csg_info
)
{

  uint16  index;
  boolean allowed_list_modified = FALSE;
  boolean operator_list_modified = FALSE;

  sys_csg_id_type  csg_id = csg_info->csg_id;
#ifdef FEATURE_LTE
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif

  if(csg_id == SYS_CSG_ID_INVALID)
  {
    return;
  }

  /* If CSG in present in operator CSG list then forbidd that... */
  if(mm_is_csg_in_operator_list(plmn_id, csg_id,FALSE))
  {
     operator_list_modified = mm_add_csg_to_forbidden_operator_list(plmn_id, csg_id);
  }
  
  for(index = 0; index < mm_sim_active_allowed_csg_list.length; index++)
  {
    if((mm_sim_active_allowed_csg_list.csg[index].csg_id == csg_id) &&
        (sys_plmn_match(plmn_id,mm_sim_active_allowed_csg_list.csg[index].plmn)))
    {
      /*Mark CSG ID as invalid*/
      MSG_HIGH_DS_0(MM_SUB," =MM= Deleted CSG from allowed CSG list");
      mm_sim_active_allowed_csg_list.csg[index].csg_id = SYS_CSG_ID_INVALID;
      allowed_list_modified = TRUE;
      if(mm_using_allowed_csg_nv == FALSE)
      {
        mmsim_update_allowed_csg_list(plmn_id, csg_info, MMGSDI_UPDATE_DELETE_CSG_ID);
      }
    }
  }

  if((allowed_list_modified == TRUE) ||(operator_list_modified == TRUE) )
  {
    mm_output_detailed_csg_list();
    mm_send_rrc_csg_white_list_ind(&mm_sim_active_operator_csg_list, &mm_sim_active_allowed_csg_list);
    mm_send_rr_csg_white_list_ind();
#ifdef FEATURE_LTE
    emm_send_rrc_system_update(emm_ctrl_data_ptr);
#endif 
  }

  if(allowed_list_modified == TRUE)
  {
    mmsend_mm_csg_white_list_log_packet(NAS_LOG_ACSGL_TYPE, &mm_sim_active_allowed_csg_list);
  }

  if(operator_list_modified == TRUE)
  {
    mmsend_mm_csg_white_list_log_packet(NAS_LOG_OCSGL_TYPE, &mm_sim_active_operator_csg_list);
  }
  return;
}

/*===========================================================================

FUNCTION  mm_add_csg_to_white_list

DESCRIPTION
This functions add CSG to white list. If forbidden timer is running for CSG in OCSGL then timer is stopped. 
Otherwise entry is added to ACSGL

DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void mm_add_csg_to_white_list
(
  sys_plmn_id_s_type  plmn_id,
  sys_csg_info_s_type  *csg_info
)
{
  uint16 index;
  boolean found = FALSE;
  boolean operator_list_modified = FALSE;
  boolean allowed_list_modified = FALSE;
  sys_csg_list_info_s_type    *temp_csg;
  uint16 temp_csg_len;

  sys_csg_id_type  csg_id = csg_info->csg_id;

#ifdef FEATURE_LTE
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif

  if(mm_is_csg_in_white_list(plmn_id, csg_info))
  {
    return;
  }

  /*If CSG,PLMN Id are present in operator CSG list then disable timer*/
  for(index = 0; index < mm_sim_active_operator_csg_list.length; index++)
  {
    if((mm_sim_active_operator_csg_list.csg[index].csg_id == csg_id) &&
        (sys_plmn_match(plmn_id,mm_sim_active_operator_csg_list.csg[index].plmn)))
    {
      mm_sim_active_operator_csg_list.csg[index].timer_count = MM_TIMER_DISABLED;
      found = TRUE;
      operator_list_modified = TRUE;
      break;
    }
  }

  /*if not found in operator CSG list then add to to allowed CSG list */
  if(found == FALSE) 
  {
    for(index = 0; index < mm_sim_active_allowed_csg_list.length; index++)
   {
     /*If allowed CSG list has Invalid CSG then add CSG to that location*/
     if(mm_sim_active_allowed_csg_list.csg[index].csg_id == SYS_CSG_ID_INVALID)
     {
       mm_sim_active_allowed_csg_list.csg[index].csg_id = csg_id;
       mm_sim_active_allowed_csg_list.csg[index].plmn = plmn_id;
       allowed_list_modified = TRUE;
       found = TRUE;
       /*Update allowed CSG list if using from SIM*/
       if(mm_using_allowed_csg_nv == FALSE)
       {
         mmsim_update_allowed_csg_list(plmn_id, csg_info, MMGSDI_UPDATE_ADD_CSG_ID);
       }
       break;
     }
   }
  }

  if(found == FALSE)
  {
    if(mm_sim_active_allowed_csg_list.length < SYS_CSG_LIST_MAX_LENGTH)
    {
      temp_csg_len = mm_sim_active_allowed_csg_list.length + 5;
      if(temp_csg_len > SYS_CSG_LIST_MAX_LENGTH)
      {
        temp_csg_len = SYS_CSG_LIST_MAX_LENGTH;
      }  
      temp_csg = (sys_csg_list_info_s_type*)modem_mem_alloc(sizeof(sys_csg_list_info_s_type)*temp_csg_len, MODEM_MEM_CLIENT_NAS);
             
      mm_check_for_null_ptr((void*)temp_csg );
      
      if(mm_sim_active_allowed_csg_list.csg != NULL)
      {
        memscpy(temp_csg, sizeof(sys_csg_list_info_s_type)*temp_csg_len, mm_sim_active_allowed_csg_list.csg, sizeof(sys_csg_list_info_s_type)*mm_sim_active_allowed_csg_list.length);
      }
      
      temp_csg[mm_sim_active_allowed_csg_list.length].csg_id = csg_id;
      temp_csg[mm_sim_active_allowed_csg_list.length].plmn = plmn_id;

      mm_sim_active_allowed_csg_list.length++;

      for(index = mm_sim_active_allowed_csg_list.length; index < temp_csg_len; index++)
      {
        temp_csg[index].csg_id = SYS_CSG_ID_INVALID;
      }

      mm_sim_active_allowed_csg_list.length = temp_csg_len;

      if(mm_sim_active_allowed_csg_list.csg != NULL)
      {
        modem_mem_free(mm_sim_active_allowed_csg_list.csg, MODEM_MEM_CLIENT_NAS);
      }
      mm_sim_active_allowed_csg_list.csg = temp_csg;
      allowed_list_modified = TRUE;

     /*Update allowed CSG list if using from SIM*/
     if(mm_using_allowed_csg_nv == FALSE)
     {
       mmsim_update_allowed_csg_list(plmn_id, csg_info, MMGSDI_UPDATE_ADD_CSG_ID);
     }
 
    }
    else
    {
      MSG_HIGH_DS_0(MM_SUB,"Reached MAX list length for allowed CSG, not adding to list");
    }
  }

 
  if((allowed_list_modified == TRUE) ||(operator_list_modified == TRUE) )
  {
    mm_output_detailed_csg_list();
    mm_send_rrc_csg_white_list_ind(&mm_sim_active_operator_csg_list, &mm_sim_active_allowed_csg_list);
    mm_send_rr_csg_white_list_ind();
#ifdef FEATURE_LTE
    emm_send_rrc_system_update(emm_ctrl_data_ptr);
#endif 
  }

  if(allowed_list_modified == TRUE)
  {
	if(mm_sim_allowed_csg_list_read_started == TRUE)
    {
      mmsim_camped_csg_add_to_white_list = TRUE;
    }  
    mmsend_mm_csg_white_list_log_packet(NAS_LOG_ACSGL_TYPE, &mm_sim_active_allowed_csg_list);
  }

  if(operator_list_modified == TRUE)
  {
    mmsend_mm_csg_white_list_log_packet(NAS_LOG_OCSGL_TYPE, &mm_sim_active_operator_csg_list);
  }
}

/*===========================================================================

FUNCTION  mm_output_detailed_csg_list

DESCRIPTION
  Function to output detailed CSG list


DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void mm_output_detailed_csg_list(void)
{
  uint16 index;

  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  mcc;
  uint32  mnc;



  if(mm_sim_active_operator_csg_list.length != 0 && mm_sim_active_operator_csg_list.csg != NULL)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= Operator CSG list (length = %d)",mm_sim_active_operator_csg_list.length);
     
    for(index = 0; index < mm_sim_active_operator_csg_list.length; index++)
    {
      sys_plmn_get_mcc_mnc
      (
        mm_sim_active_operator_csg_list.csg[index].plmn,
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &mcc,
        &mnc
      );
      if(mnc_includes_pcs_digit)
      {
        MSG_HIGH_DS_3(MM_SUB,"=MM=  PLMN = %03lu-%03lu   CSG Id = %lx",mcc, mnc, mm_sim_active_operator_csg_list.csg[index].csg_id);
      }
      else
      {
        MSG_HIGH_DS_3(MM_SUB,"=MM=  PLMN = %03lu-%02lu    CSG iD = %lx",mcc, mnc, mm_sim_active_operator_csg_list.csg[index].csg_id);
      }
      MSG_HIGH_DS_1(MM_SUB,"=MM=              Timer = %10lx", mm_sim_active_operator_csg_list.csg[index].timer_count);
     }
   }


  MSG_HIGH_DS_1(MM_SUB ,"Operator CSG display Indicator list (Length = %d)", mm_sim_active_csg_indicator_list.length);
  for(index = 0; index < mm_sim_active_csg_indicator_list.length; index++)
  {
    sys_plmn_get_mcc_mnc
     (
       mm_sim_active_csg_indicator_list.list[index].plmn,
       &plmn_id_is_undefined,
       &mnc_includes_pcs_digit,
       &mcc,
       &mnc
     );
      if(mnc_includes_pcs_digit)
      {
        MSG_HIGH_DS_3(MM_SUB,"=MM=  PLMN = %03lu-%03lu   Indication = %d",mcc, mnc, mm_sim_active_csg_indicator_list.list[index].indicator);
      }
      else
      {
        MSG_HIGH_DS_3(MM_SUB,"=MM=  PLMN = %03lu-%02lu    Indication = %d",mcc, mnc, mm_sim_active_csg_indicator_list.list[index].indicator);
      }
  }

  MSG_HIGH_DS_1(MM_SUB ,"=MM= Allowed CSG list: (Length = %d)", mm_sim_active_allowed_csg_list.length);
  if(mm_sim_active_allowed_csg_list.length != 0 && mm_sim_active_allowed_csg_list.csg != NULL)
  {
    for(index = 0; index < mm_sim_active_allowed_csg_list.length; index++)
    {
      sys_plmn_get_mcc_mnc
      (
        mm_sim_active_allowed_csg_list.csg[index].plmn,
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &mcc,
        &mnc
      );
      if(mnc_includes_pcs_digit)
      {
        MSG_HIGH_DS_3(MM_SUB,"=MM=  PLMN = %03lu-%03lu    CSG Id = %lx", 
                               mcc, mnc, mm_sim_active_allowed_csg_list.csg[index].csg_id);
      }
      else
      {
        MSG_HIGH_DS_3(MM_SUB,"=MM=  PLMN = %03lu-%02lu    CSG Id =  %lx", 
                             mcc, mnc, mm_sim_active_allowed_csg_list.csg[index].csg_id);
      }
     }
   }
}

/*===========================================================================

FUNCTION MMSEND_MM_CSG_LIST_LOG_PACKET

DESCRIPTION
  This function sends the MM info log packet to DIAG
  type = 0 should be passed for Operator CSG list
  type = 1 should be used for allowed CSG list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mmsend_mm_csg_white_list_log_packet(uint8 type, sys_csg_list_s_type *csg_list)
{
   LOG_UMTS_NAS_CSG_WHITE_LIST_LOG_PACKET_C_type *log_ptr;
   uint8    num_csg = 0;
   uint8    i = 0;

   if(type != NAS_LOG_OCSGL_TYPE && type != NAS_LOG_ACSGL_TYPE)
   {
     return;
   }

   log_ptr = (LOG_UMTS_NAS_CSG_WHITE_LIST_LOG_PACKET_C_type *)log_alloc(LOG_UMTS_NAS_CSG_WHITE_LIST_LOG_PACKET_C,
        sizeof(LOG_UMTS_NAS_CSG_WHITE_LIST_LOG_PACKET_C_type));

   if(!log_ptr)
   {
      /* Could not allocate memory */
      return;
   }
   log_ptr->log_version = 1;
   log_ptr->type = type;

   if(type == NAS_LOG_OCSGL_TYPE)
   {
     for (i = 0; i < csg_list-> length && num_csg < LOG_MAX_CSG_LIST ; i++)
     {
       if(csg_list->csg[i].timer_count == MM_TIMER_DISABLED &&
            csg_list->csg[i].csg_id != SYS_CSG_ID_INVALID)
      {
        log_ptr->csg_info[num_csg].csg_id = csg_list->csg[i].csg_id;
        log_ptr->csg_info[num_csg].plmn_id.plmn_id[0] = csg_list->csg[i].plmn.identity[0];
        log_ptr->csg_info[num_csg].plmn_id.plmn_id[1] = csg_list->csg[i].plmn.identity[1];
        log_ptr->csg_info[num_csg].plmn_id.plmn_id[2] = csg_list->csg[i].plmn.identity[2];
        num_csg++;
      }
     }
   }
   else
   {
     for (i = 0; i < csg_list-> length && num_csg < LOG_MAX_CSG_LIST ; i++)
     {
       if(csg_list->csg[i].csg_id != SYS_CSG_ID_INVALID)
      {
        log_ptr->csg_info[num_csg].csg_id = csg_list->csg[i].csg_id;
        log_ptr->csg_info[num_csg].plmn_id.plmn_id[0] = csg_list->csg[i].plmn.identity[0];
        log_ptr->csg_info[num_csg].plmn_id.plmn_id[1] = csg_list->csg[i].plmn.identity[1];
        log_ptr->csg_info[num_csg].plmn_id.plmn_id[2] = csg_list->csg[i].plmn.identity[2];
        num_csg++;
      }
     }
   }

   log_ptr->num_csg = num_csg;
   log_commit(log_ptr);

}



/*===========================================================================

FUNCTION MMSEND_MM_CSG_AVAILABLE_LIST_LOG_PACKET

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mmsend_mm_csg_available_list_log_packet(sys_detailed_plmn_list_s_type  *available_csg_p)
{
   LOG_UMTS_NAS_CSG_AVAILABLE_LIST_LOG_PACKET_C_type *log_ptr;
   uint8    num_csg = 0;
   uint8    i = 0;

   log_ptr = (LOG_UMTS_NAS_CSG_AVAILABLE_LIST_LOG_PACKET_C_type *)log_alloc(LOG_UMTS_NAS_CSG_AVAILABLE_LIST_LOG_PACKET_C,
        sizeof(LOG_UMTS_NAS_CSG_AVAILABLE_LIST_LOG_PACKET_C_type));

   if(!log_ptr)
   {
      /* Could not allocate memory */
      return;
   }
   log_ptr->log_version = 1;

   for(i = 0; (i < available_csg_p->length && i < SYS_PLMN_LIST_MAX_LENGTH); i++)
   {
     log_ptr->csg_info[i].plmn.plmn_id[0] = available_csg_p->info[i].plmn.identity[0];
     log_ptr->csg_info[i].plmn.plmn_id[1] = available_csg_p->info[i].plmn.identity[1];
     log_ptr->csg_info[i].plmn.plmn_id[2] = available_csg_p->info[i].plmn.identity[2];

     log_ptr->csg_info[i].rat = available_csg_p->info[i].rat;

     log_ptr->csg_info[i].csg_id = available_csg_p->info[i].csg_info.csg_id;
     num_csg++;
   }

   log_ptr->num_csg = num_csg;
   log_commit(log_ptr);

}


/*===========================================================================

FUNCTION mm_check_camped_csg_not_white_list

DESCRIPTION
This function check if camped csg became forbidden and takes necessry actions

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mm_check_camped_csg_not_white_list(void)
{
#ifdef FEATURE_WCDMA
  mmr_service_ind_s_type  mmr_service_ind;
  rrc_cmd_type *rrc_cmd_p = NULL;

  if(is_umts_mode())
  {
    if((mmcoord_pending_reg_message.cmd.hdr.message_id == (int) MMR_REG_REQ) &&
       (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode != SYS_NETWORK_SELECTION_MODE_LIMITED) &&
       (!mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info)) &&
       (mm_serving_plmn.info.service_status !=  SYS_SRV_STATUS_NO_SRV) &&
       (mm_idle_substate != MM_PLMN_SEARCH ) && (!mm_waiting_for_service_cnf) &&
       (!mm_waiting_for_stop_mode_cnf) &&  !(mm_waiting_for_deact_cnf))
    {      
      if(rrc_connection_status != MMRRC_IDLE)
      {
        if(rrc_connection_status != MMRRC_RELEASE_IN_PROGRESS)
        {
          /* If in active CS call then  close session PS session is open and set GMM sub state to limited*/
          if(mm_state == MM_CONNECTION_ACTIVE)
          {
            gmm_set_substate(GMM_LIMITED_SERVICE);
            if((ps_session_status == MMRRC_ACTIVE) || 
               (ps_session_status == MMRRC_EST_IN_PROGRESS))
            {
              rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );

              if (rrc_cmd_p == NULL)
          {
                MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
              }

              rrc_cmd_p->cmd_hdr.cmd_id = RRC_CLOSE_SESSION_REQ;
              rrc_cmd_p->cmd.close_sess_req.cn_domain_id = RRC_PS_DOMAIN_CN_ID;

              rrc_cmd_p->cmd.close_sess_req.close_session_cause = RRC_OTHERS;
              MSG_HIGH_DS_0(MM_SUB, "=MM= Sending RRC_CLOSE_SESSION_REQ");

              send_cmd_to_rrc( rrc_cmd_p, mm_serving_plmn.info.active_rat );
              
            }
          }
          else
          {
            /*If CS call is not active then abort RRC connection*/
          rrc_cmd_p = mm_get_rrc_cmd_buf( mm_serving_plmn.info.active_rat );
          if (rrc_cmd_p == NULL)
          {
            MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
          }               
          rrc_cmd_p->cmd_hdr.cmd_id = RRC_ABORT_REQ;
            rrc_cmd_p->cmd.rrc_abort_req.delay_sig_con_release = TRUE;
          rrc_cmd_p->cmd.rrc_abort_req.auth_failed_bar_cells = FALSE;
            rrc_cmd_p->cmd.rrc_abort_req.hard_abort = FALSE;                
          rrc_cmd_p->cmd.rrc_abort_req.abort_cause = RRC_OTHERS;
          send_cmd_to_rrc( rrc_cmd_p, mm_serving_plmn.info.active_rat );
          }
        }
      }
      else
      {
        /* If in idle then declare no service, CM will send new service request*/
        MSG_HIGH_DS_0(MM_SUB, "=MM= CSG: Camped CSG removed from white list, sending no service to CM");
        mm_serving_plmn.info.service_status = SYS_SRV_STATUS_NO_SRV;
        mm_serving_plmn.info.no_service_cause = SYS_NO_SVC_CAUSE_NORMAL;
        mm_serving_plmn.mm_service_state  = SERVICE_NOT_AVAILABLE;
        mm_serving_plmn.gmm_service_state  = SERVICE_NOT_AVAILABLE;
      
        if (mm_reg_waiting_for_reg_cnf)
        {
          mmcoord_mmr_reg_cnf.service_state  = mm_serving_plmn.info;
          mmcoord_mmr_reg_cnf.cause = MMR_CAUSE_NOT_SPECIFIED;
          mm_send_mmr_reg_cnf();
  
          mm_reg_waiting_for_reg_cnf = FALSE;
          mm_ready_to_send_reg_cnf   = FALSE;
        }
        else
        {
          mmr_service_ind.service_state = mm_serving_plmn.info;
          mmr_service_ind.cause = MMR_CAUSE_NOT_SPECIFIED;
          mm_send_mmr_service_ind( &mmr_service_ind );
        }
      }
    }
  }
#endif /* #ifdef FEATURE_WCDMA */
}

/*===========================================================================
FUNCTION  mm_is_csg_in_rejected_list

DESCRIPTION
  This function checks if CSG,PLMN is present in rejected CSG list

DEPENDENCIES
  None 

RETURN VALUE
  TRUE: If CSG,PLMN is present
  FALSE: IF CSG,PLMN is not present

SIDE EFFECTS
  None
===========================================================================*/
boolean mm_is_csg_in_rejected_list
(
  sys_plmn_id_s_type plmn_id,
  sys_csg_id_type   csg_id
)
{
  uint16  index;
  boolean result = FALSE;

  if((csg_id == SYS_CSG_ID_INVALID) ||
     (mm_rejected_csg_list.csg == NULL))
  {
    result = FALSE;
  }
  else
  {
    for(index = 0; index < mm_rejected_csg_list.length; index++)
    {
      if((csg_id == mm_rejected_csg_list.csg[index].csg_id) &&
          (sys_plmn_match(plmn_id, mm_rejected_csg_list.csg[index].plmn)))
      {
        result = TRUE;
      }
    }
  }
  return result;
}

/*===========================================================================
FUNCTION  mm_add_csg_to_rejected_list

DESCRIPTION
 This function adds a CSG,PLMN to the rejected CSG list

DEPENDENCIES
  None 

RETURN VALUE
  TRUE: If CSG is added to the rejected CSG list
  FALSE: If CSG is not added to the rejected CSG list

SIDE EFFECTS
  None
===========================================================================*/
boolean mm_add_csg_to_rejected_list
(
  sys_plmn_id_s_type plmn_id,
  sys_csg_id_type   csg_id,
  byte              reject_cause
)
{
  boolean result = FALSE;

  if((csg_id == SYS_CSG_ID_INVALID)
     ||
     (mm_csg_selection_config != SYS_CSG_SELECTION_CONFIG_1)
     ||
     ((reject_cause != NATIONAL_ROAMING_NOT_ALLOWED)&&
      (reject_cause != NO_SUITABLE_CELLS_IN_LA) &&
      (reject_cause != AUTH_AUTHENTICATION_UNACCEPTABLE))
     ||
     (mm_is_csg_in_rejected_list(plmn_id, csg_id) == TRUE))
  {
    result = FALSE;
  }
  else if(mm_rejected_csg_list.csg == NULL)
  {
    mm_rejected_csg_list.length = 0;
    /* allocate memory */
    mm_rejected_csg_list.csg = (sys_csg_list_info_s_type*)
                                  modem_mem_calloc(1,
                                              sizeof(sys_csg_list_info_s_type),
                                              MODEM_MEM_CLIENT_NAS);
    mm_check_for_null_ptr((void*)mm_rejected_csg_list.csg);

    /* add CSG to the rejected list */
    mm_rejected_csg_list.csg[mm_rejected_csg_list.length].plmn = plmn_id;
    mm_rejected_csg_list.csg[mm_rejected_csg_list.length].csg_id = csg_id;
    mm_rejected_csg_list.length = mm_rejected_csg_list.length + 1;
    result = TRUE;
  }
  else if(mm_rejected_csg_list.length == MM_REJECTED_CSG_LIST_MAX_LENGTH)
  {
    /* remove the oldest rejected CSG entry */
    memsmove((void*)&mm_rejected_csg_list.csg[0],
             sizeof(sys_csg_list_info_s_type) * 
              MM_REJECTED_CSG_LIST_MAX_LENGTH,
             (void*)&mm_rejected_csg_list.csg[1],
             sizeof(sys_csg_list_info_s_type) *
              (MM_REJECTED_CSG_LIST_MAX_LENGTH - 1));
    memset((void*)
              (&mm_rejected_csg_list.csg[MM_REJECTED_CSG_LIST_MAX_LENGTH - 1]),
                0,sizeof(sys_csg_list_info_s_type));
    mm_rejected_csg_list.csg[mm_rejected_csg_list.length - 1].plmn = plmn_id;
    mm_rejected_csg_list.csg[mm_rejected_csg_list.length - 1].csg_id = csg_id;
    result = TRUE;
  }
  else
  {
    /* reallocate memory */
    mm_rejected_csg_list.csg = (sys_csg_list_info_s_type*)
                             modem_mem_realloc((void*)mm_rejected_csg_list.csg,
                                              sizeof(sys_csg_list_info_s_type)*
                                             (mm_rejected_csg_list.length + 1),
                                              MODEM_MEM_CLIENT_NAS);
    mm_check_for_null_ptr((void*)mm_rejected_csg_list.csg);
    
    /* zero initialize the memory allocated for the new element */
    memset((void*)(mm_rejected_csg_list.csg + mm_rejected_csg_list.length),
           0,
           sizeof(sys_csg_list_info_s_type));

    /* add CSG to the rejected list */
    mm_rejected_csg_list.csg[mm_rejected_csg_list.length].plmn = plmn_id;
    mm_rejected_csg_list.csg[mm_rejected_csg_list.length].csg_id = csg_id;
    mm_rejected_csg_list.length = mm_rejected_csg_list.length + 1;
    result = TRUE;
  }

  if(result == TRUE)
  {
    boolean plmn_id_is_undefined;
    boolean mnc_includes_pcs_digit;
    uint32  mcc;
    uint32  mnc;

    sys_plmn_get_mcc_mnc
    (
      plmn_id,
      &plmn_id_is_undefined,
      &mnc_includes_pcs_digit,
      &mcc,
      &mnc
    );

    if(mnc_includes_pcs_digit)
    {
      MSG_HIGH_DS_3(MM_SUB,
       "=MM= CSG ID %d, PLMN(%03lu-%03lu) added to rejected CSG list",
                           csg_id,mcc,mnc);
    }
    else
    {
      MSG_HIGH_DS_3(MM_SUB,
       "=MM= CSG ID %d, PLMN(%03lu-%02lu) added to rejected CSG list",
                           csg_id,mcc,mnc);
    }
  }
  return result;
}

/*===========================================================================
FUNCTION  mm_clear_rejected_csg_list

DESCRIPTION
This function clears the rejected CSG list

DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void mm_clear_rejected_csg_list
(
  void
)
{
  if(mm_rejected_csg_list.csg == NULL)
  {
    return;
  }

  mm_rejected_csg_list.length = 0;
  modem_mem_free((void*)mm_rejected_csg_list.csg,MODEM_MEM_CLIENT_NAS);
  MSG_HIGH_DS_1(MM_SUB ,"=MM= Rejected CSG list cleared (Length = %d)",
                               mm_rejected_csg_list.length);
  mm_rejected_csg_list.csg = NULL;
}

/*===========================================================================
FUNCTION  mm_remove_csg_from_rejected_list

DESCRIPTION
 This function removes the specified CSG,PLMN entry from the rejected CSG list

DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void mm_remove_csg_from_rejected_list
(
  sys_plmn_id_s_type plmn_id,
  sys_csg_id_type   csg_id
)
{
  uint16  index;
  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  mcc;
  uint32  mnc;

  if((csg_id == SYS_CSG_ID_INVALID) ||
     (mm_rejected_csg_list.csg == NULL))
  {
    return;
  }

  for(index = 0; index < mm_rejected_csg_list.length; index++)
  {
    if((csg_id == mm_rejected_csg_list.csg[index].csg_id) &&
        (sys_plmn_match(plmn_id, mm_rejected_csg_list.csg[index].plmn)))
    {
      if(mm_rejected_csg_list.length == 1)
      {
        mm_clear_rejected_csg_list();
        return;
      }

      if(index == (mm_rejected_csg_list.length - 1))
      {
        mm_rejected_csg_list.length = mm_rejected_csg_list.length - 1;
      }
      else
      {
        memsmove((void*)&mm_rejected_csg_list.csg[index],
                sizeof(sys_csg_list_info_s_type) *
                  (mm_rejected_csg_list.length - index),
                (void*)&mm_rejected_csg_list.csg[index + 1],
                sizeof(sys_csg_list_info_s_type) *
                  (mm_rejected_csg_list.length - index - 1));
        mm_rejected_csg_list.length = mm_rejected_csg_list.length - 1;
      }

      /* reallocate memory for remaining elements */
      mm_rejected_csg_list.csg = (sys_csg_list_info_s_type*)
                             modem_mem_realloc((void*)mm_rejected_csg_list.csg,
                                              sizeof(sys_csg_list_info_s_type)*
                                                (mm_rejected_csg_list.length),
                                              MODEM_MEM_CLIENT_NAS);
      if(mm_rejected_csg_list.csg == NULL)
      {
        MSG_FATAL_DS(MM_SUB,"=MM= Error: Not able to Reallocate Buffer", 0, 0, 0);
      }

      sys_plmn_get_mcc_mnc
      (
        plmn_id,
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &mcc,
        &mnc
      );

      if(mnc_includes_pcs_digit)
      {
        MSG_HIGH_DS_3(MM_SUB,
         "=MM= CSG ID %d, PLMN(%03lu-%03lu) removed from rejected CSG list",
                             csg_id,mcc,mnc);
      }
      else
      {
        MSG_HIGH_DS_3(MM_SUB,
         "=MM= CSG ID %d, PLMN(%03lu-%02lu) removed from rejected CSG list",
                             csg_id,mcc,mnc);
      }
      break;
    }
  }
}

/*===========================================================================
FUNCTION  mm_is_csg_search_allowed

DESCRIPTION
 This function accepts CSG search type, CSG selection configuration as
 parameters and decides if CSG search should be allowed.
 Search result is also populated to REG_MM_PLMN_SEARCH_ABORTED in case of
 search failure

DEPENDENCIES
  None 

RETURN VALUE
  TRUE: If CSG search allowed
  FALSE: If CSG search not allowed

SIDE EFFECTS
  None
===========================================================================*/
boolean mm_is_csg_search_allowed
(
  sys_csg_search_type_e_type          csg_search_type,
  sys_csg_selection_config_e_type     config_option,
  reg_mm_plmn_search_status_e_type    *search_result
)
{
  uint32 i;
  boolean result = FALSE;

  if(csg_search_type == SYS_CSG_SEARCH_TYPE_LEGACY)
  {
    result = TRUE;
  }
  else
  {
    switch(config_option)
    {
      case SYS_CSG_SELECTION_CONFIG_1:
        /* Reject CSG search with search failure if CSG search type is App init and:
             MM state is MM_WAIT_FOR_RR_CONNECTION_LU or MM_LOCATION_UPDATE_INITIATED
             GMM state is GMM_REGISTERED_INITIATED or GMM_ROUTING_AREA_UPDATING_INITIATED
             EMM state is EMM_REGISTERED_INITIATED or EMM_TRACKING_AREA_UPDATING_INITIATED */

        if(csg_search_type == SYS_CSG_SEARCH_TYPE_APP_INIT)
        {
          if(is_gsm_mode() || is_umts_mode())
          {
            if((mm_state == MM_WAIT_FOR_RR_CONNECTION_LU) || 
               (mm_state == MM_LOCATION_UPDATE_INITIATED) || 
               (gmm_state == GMM_REGISTERED_INITIATED) || 
               (gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED))
            {
              *search_result = REG_MM_PLMN_SEARCH_ABORTED;
              result = FALSE;
            }
          }
#ifdef FEATURE_LTE
          else if(is_lte_mode())
          {
            if((EMM_GET_STATE() == EMM_REGISTERED_INITIATED) || 
               (EMM_GET_STATE() == EMM_TRACKING_AREA_UPDATING_INITIATED))
            {
              *search_result = REG_MM_PLMN_SEARCH_ABORTED;
              result = FALSE;
            }
          }
#endif
        }

        if(*search_result != REG_MM_PLMN_SEARCH_ABORTED)
        {
          /* 1.Reject CSG search if UE is camped on a CSG cell and has full service
             2.Reject CSG search if UE is camped on a hybrid cell but the CSG ID is not
               present in white list
             3.Reject CSG search if OCSGL is present and empty else allow search if:
             a) CSG search type is App initated and at least one entry in the operator
                list is not present in the rejected CSG list and
                timer TIMER_FORBIDDEN_OPERATOR_CSG_LIST is disabled for this csg entry  
             b) CSG search type is User initiated */
          if((mm_serving_plmn.info.service_status != SYS_SRV_STATUS_SRV) ||
             (mm_serving_plmn.info.csg_info.csg_id == SYS_CSG_ID_INVALID)||
             ((mm_serving_plmn.info.csg_info.hybrid_cell == TRUE) && 
              (mm_is_csg_in_white_list(mm_serving_plmn.info.plmn,
                                       &mm_serving_plmn.info.csg_info) == FALSE)))
          {
            if(mm_sim_ocsgl_present == TRUE)
            {
              if(mm_sim_active_operator_csg_list.length < 1)
              {
                result = FALSE;
              }
              else if(csg_search_type == SYS_CSG_SEARCH_TYPE_APP_INIT)
              {
                for(i=0; i<mm_sim_active_operator_csg_list.length; i++)
                {
                  if((mm_is_csg_in_rejected_list
                      (
                        mm_sim_active_operator_csg_list.csg[i].plmn,
                        mm_sim_active_operator_csg_list.csg[i].csg_id
                      ) == FALSE)
                     &&
                     (mm_sim_active_operator_csg_list.csg[i].timer_count ==
                                                      MM_TIMER_DISABLED))
                  {
                    result = TRUE;
                    break;
                  }
                }
              }
              else
              {
                result = TRUE;
              }
            }
            /* EFOCSGL not present */
            else
            {
              /* Reject search if CSG search type is App initiated and allowed CSG list is
                 empty, else allow search if at least one entry in the allowed CSG list is
                 not present in the rejected CSG list */
              if(csg_search_type == SYS_CSG_SEARCH_TYPE_APP_INIT)
              {
                if(mm_sim_active_allowed_csg_list.length < 1)
                {
                  result = FALSE;
                }
                else
                {
                  for(i=0; i<mm_sim_active_allowed_csg_list.length; i++)
                  {
                    if( ( /* Check if the CSG was rejected for Reject Causes #13, #15 or AUTH_AUTHENTICATION_UNACCEPTABLE  */
                          mm_is_csg_in_rejected_list
                          (
                            mm_sim_active_allowed_csg_list.csg[i].plmn,
                            mm_sim_active_allowed_csg_list.csg[i].csg_id
                          ) == FALSE
                        ) 
                        && /* Check if the CSG was rejected for Reject Cause #25 */
                        (mm_sim_active_allowed_csg_list.csg[i].csg_id != SYS_CSG_ID_INVALID)
                      )
                    {
                      result = TRUE;
                      break;
                    }
                  }
                  if(result == FALSE)
                  {
                    MSG_HIGH_DS_0(MM_SUB, "=MM= All CSGs in Allowed list are currently rejected");
                  }
                }
              }
              /* Allow search when CSG search type is User initiated */
              else
              {
                result = TRUE;
              }          
            }
          }
        }
        else
        {
          MSG_HIGH_DS_2(MM_SUB, "=MM= Aborting CSG Search. MM/GMM = %d/%d/",
                                                                     mm_state, gmm_state);
#ifdef FEATURE_LTE
          MSG_HIGH_DS_1(MM_SUB, "=MM= Aborting CSG Search. EMM State = %d", EMM_GET_STATE());
#endif
        }
        break;

      default:
        MSG_ERROR_DS_1(MM_SUB, "=MM= Invalid CSG selection config %d",
                                           config_option);
        result = FALSE;
        break;
    }/* end of switch(config_option) */
  }
  return result;
}

/*===========================================================================
FUNCTION  mm_process_csg_search_list

DESCRIPTION
 

DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void mm_process_csg_search_list
(
  sys_csg_search_type_e_type     csg_search_type,
  sys_detailed_plmn_list_s_type  *available_csg_p
)
{
  mmsend_mm_csg_available_list_log_packet(available_csg_p);

  switch(csg_search_type)
  {
    case SYS_CSG_SEARCH_TYPE_LEGACY:
      mm_prioritize_csg_list(available_csg_p);
      break;

    case SYS_CSG_SEARCH_TYPE_USER_INIT:
    case SYS_CSG_SEARCH_TYPE_APP_INIT:
      mm_sort_csg_list_config_1(available_csg_p);
      break;

    default:
      MSG_ERROR_DS(MM_SUB, "=MM= Invalid CSG search type",0,0,0);
      break;
  }
}

/*===========================================================================
FUNCTION  mm_sort_csg_cells_by_signal_strength

DESCRIPTION
 This function sorts the CSG entries in the list by the order of decreasing
 signal strength. Uses bubble sort algorithm.

DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
static void mm_sort_csg_cells_by_signal_strength
(
  sys_detailed_plmn_list_s_type *available_csg_p, 
  uint32 min_idx,
  uint32 max_idx
)
{
  uint32 i = 0, j = 0;
  sys_detailed_plmn_list_info_s_type temp_csg_info;

  if(min_idx < max_idx)
  {
    for(i = min_idx; i < max_idx; i++)
    {
      for(j = min_idx; j < min_idx + max_idx - i; j++)
      {
        /* Signal_strength is returned as the absolute
           value of a negative number from Access Stratum.
           Therefore swap if available_csg_p->info[j].signal_strength >
           available_csg_p->info[j+1].signal_strength */
        if(available_csg_p->info[j].signal_strength > 
                                    available_csg_p->info[j+1].signal_strength)
        {
          temp_csg_info = available_csg_p->info[j];
          available_csg_p->info[j] = available_csg_p->info[j+1];
          available_csg_p->info[j+1] = temp_csg_info;
        }
      }
    }
  }
}

/*===========================================================================
FUNCTION  mm_sort_csg_list_config_1

DESCRIPTION
 This function sorts the CSG search list returned by AS as per CSG
 selection configuration #1

DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void mm_sort_csg_list_config_1
(
  sys_detailed_plmn_list_s_type  *available_csg_p
)
{
  sys_detailed_plmn_list_info_s_type temp_csg_info;
  boolean allow_rejected = FALSE;
  uint32 i = 0;

  MSG_HIGH_DS_1(MM_SUB ,"=MM= Available CSG search list length = %d",
                                                  available_csg_p->length);
  if(available_csg_p->length == 0)
  {
    return;
  }

  allow_rejected = (mm_csg_search_type == SYS_CSG_SEARCH_TYPE_APP_INIT) ?
                                                       FALSE : TRUE;
  /* Check if EFOCSGL is present */  
  if(mm_sim_ocsgl_present == TRUE)
  {
    /* Check if Operator CSG list is not empty */
    if(mm_sim_active_operator_csg_list.length > 0)
    {
      for(i = 0; i < available_csg_p->length; i++)
      {
        /* Remove CSG entry from the available list if:
             CSG entry is not present in the Operator CSG list
                          OR
             CSG search type is App initiated and CSG entry is present
             in the Operator CSG list and also present in the Rejected
             CSG list. Allow rejected CSGs for CSG search type User initiated */
        if(
            (mm_is_csg_in_operator_list(available_csg_p->info[i].plmn,
                         available_csg_p->info[i].csg_info.csg_id, 
                         allow_rejected) == FALSE)
            ||
            ((allow_rejected == FALSE) &&
                (mm_is_csg_in_rejected_list(available_csg_p->info[i].plmn,
                      available_csg_p->info[i].csg_info.csg_id) == TRUE))
          )
        {
          if(i < (available_csg_p->length - 1))
          {
            available_csg_p->info[i] = 
                           available_csg_p->info[available_csg_p->length - 1];
            i--;
          }
          available_csg_p->length = available_csg_p->length - 1;
        }
        else
        {
          available_csg_p->info[i].list_category = 
                                         SYS_DETAILED_PLMN_LIST_OPERATOR_CSG;
        }
      }/* end for loop */

      /* Sort CSGs in decreasing signal strength order */
      if(available_csg_p->length > 0)
      {
        mm_sort_csg_cells_by_signal_strength
        (
          available_csg_p, 0, available_csg_p->length - 1
        );
      }
      else
      {
        MSG_HIGH_DS_0(MM_SUB, "=MM= Available CSG search list empty");
      }
    }
    /* Operator CSG list is empty */
    else
    {
      available_csg_p->length = 0;
      MSG_ERROR_DS(MM_SUB, "=MM= Operator CSG list is empty",0,0,0);
    }
  }
  /* If EFOCSGL not present */
  else
  {
    for(i = 0; i < available_csg_p->length; i++)
    {
      /* Remove CSG entry if:
           CSG plmn is forbidden
                 OR
           CSG search type is app initiated and CSG is present in rejected
           CSG list */
      if(
#ifdef FEATURE_DUAL_SIM
         (reg_sim_per_subs_plmn_forbidden(available_csg_p->info[i].plmn,(sys_modem_as_id_e_type)mm_sub_id) == TRUE)
#else
         (reg_sim_plmn_forbidden(available_csg_p->info[i].plmn) == TRUE)
#endif
         ||
         ((allow_rejected == FALSE) &&
          (mm_is_csg_in_rejected_list(available_csg_p->info[i].plmn,
              available_csg_p->info[i].csg_info.csg_id) == TRUE)))
      {
        if(i < (available_csg_p->length - 1))
        {
          available_csg_p->info[i] = 
                         available_csg_p->info[available_csg_p->length - 1];
          i--;
        }
        available_csg_p->length = available_csg_p->length - 1;
      }
    }/* end for loop  */

    if(available_csg_p->length > 0)
    {
      uint32 allowed_count = 0;

      /* Place the Allowed CSG entries on top of the available list */
      for(i = 0; i < available_csg_p->length; i++)
      {
        if(mm_is_csg_in_allowed_list(available_csg_p->info[i].plmn,
                       available_csg_p->info[i].csg_info.csg_id) == TRUE)
        {
          available_csg_p->info[i].list_category = 
                                   SYS_DETAILED_PLMN_LIST_ALLOWED_CSG;
          temp_csg_info = available_csg_p->info[allowed_count];
          available_csg_p->info[allowed_count] = available_csg_p->info[i];
          available_csg_p->info[i] = temp_csg_info;
          allowed_count++;
        }
        else
        {
          available_csg_p->info[i].list_category = 
                                     SYS_DETAILED_PLMN_LIST_OTHER_CSG;
        }
      }/* end of for loop */

      if(allow_rejected == FALSE)
      {
        available_csg_p->length = allowed_count;
        MSG_HIGH_DS_0(MM_SUB, "=MM= OTHER CSGs filtered out");
      }

      /* Sort allowed CSGs by decreasing signal strength order */
      if(allowed_count > 0)
      {
        mm_sort_csg_cells_by_signal_strength
        (
          available_csg_p, 0, (allowed_count - 1)
        );
      }

      /* Sort OTHER CSGs by decreasing signal strength order */
      if(allow_rejected == TRUE)
      {
        mm_sort_csg_cells_by_signal_strength
        (
          available_csg_p, 
          allowed_count,
          (available_csg_p->length - 1)
        );
      }
    }
    else
    {
      MSG_HIGH_DS_0(MM_SUB, "=MM= Available CSG search list empty");
    }
  }
}
#endif 

/*===========================================================================

FUNCTION  mm_remove_plmn_from_gprs_fplmn_list

DESCRIPTION
If PLMN is present in GPRS FPLMN list then this function remove PLMN from GPRS FPLMN list and send change indication to REG


DEPENDENCIES
  None 

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void mm_remove_plmn_from_gprs_fplmn_list
(
  sys_plmn_id_s_type plmn
)
{
#ifdef FEATURE_DUAL_SIM
  if (reg_sim_remove_plmn_from_gprs_fplmn_list( mm_serving_plmn.info.plmn, (sys_modem_as_id_e_type)mm_sub_id))
#else
  if (reg_sim_remove_plmn_from_gprs_fplmn_list(plmn))
#endif /*FEATURE_DUAL_SIM || FEATURE_SGLTE*/ 
  {
  #ifdef FEATURE_LTE 
    mm_send_mmr_fplmn_list_change_ind(SYS_FORBIDDEN_LIST_TYPE_GPRS_FPLMN);
  #endif /*FEATURE_LTE*/
    MSG_HIGH_DS_3(MM_SUB, "=MM= Removing PLMN 0x%2x 0x%2x 0x%2x from forbidden lists",
                  plmn.identity[0],plmn.identity[1],plmn.identity[2]);
  }
}

#ifdef FEATURE_NAS_REL10
/*===========================================================================
FUNCTION      MM_IS_PLMN_EQUIVALENT_T3246_PLMN

DESCRIPTION
 it would check if plmn is same as t3246 or is in eplmn list
 
DEPENDENCIES
  None 

RETURN VALUE
  TRUE   -  If plmn is t3246 or in eplmn list
  FALSE  -  If plmn is a new plmn and not in eplmn list
 
SIDE EFFECTS
  None
===========================================================================*/
boolean mm_is_plmn_equivalent_t3246_plmn(void)
{
  sys_plmn_id_s_type   last_t3246_plmn;
 
  memscpy(&last_t3246_plmn, sizeof(sys_plmn_id_s_type), &mm_3gpp_t3246_plmn_id, 
                                  sizeof(sys_plmn_id_s_type));

  /* if last attempted plmn is same as previous plmn or is equivalent
     do not attempt registration */
  if((!sys_plmn_id_is_undefined(last_t3246_plmn)) &&
     (!sys_plmn_id_is_undefined(mm_serving_plmn.info.plmn))&&
     ((sys_plmn_match(mm_serving_plmn.info.plmn, last_t3246_plmn) == TRUE)
     ||
     ((sys_eplmn_list_equivalent_plmn(last_t3246_plmn)   == TRUE) &&
      (sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn) == TRUE))))
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================
FUNCTION      MM_IS_LU_TO_BE_DONE_DUE_TO_T3346

DESCRIPTION
 It would check if LU has to be initiated in NMO1 netowrk while T3346 is running
 
  **The normal location updating procedure shall also be started when the MS is both IMSI attached for GPRS
  **and non-GPRS services and enters a new routing area where the network operates 
  **in network operation mode I and timer T3346 is running.

  **The MS shall also start the normal location updating procedure when the network 
  **is operating in network operation mode I, T3346 is running, update status is
  **not U1 UPDATED and the user manually selects the current PLMN.
 
DEPENDENCIES
  None 

RETURN VALUE
  TRUE   -  Initiate LU in NMO1 netowrk
  FALSE  -  No LU due to 3346 running.
 
SIDE EFFECTS
  None
===========================================================================*/

boolean mm_is_lu_to_be_done_due_to_t3346(void)
{
  boolean is_lu_required = FALSE;
  boolean is_serving_rai_changed = FALSE;
#ifdef FEATURE_FEMTO_CSG
  boolean is_manual_csg_selection_success = FALSE;
#endif
  if ((gmm_rai_changed( gmm_previous_serving_rai.location_area_code,
                        gmm_previous_serving_rai.routing_area_code,
                        mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
      (gmm_plmn_changed( gmm_previous_serving_rai.plmn_id,
                         mm_serving_plmn.info.plmn)))
  {
    is_serving_rai_changed = TRUE;
    MSG_HIGH_DS_0(MM_SUB, "=MM= RAI changed");
  }
    
#ifdef FEATURE_FEMTO_CSG
  if((mmcoord_pending_reg_message.cmd.mmr_reg_req.csg_id == mm_serving_plmn.info.csg_info.csg_id) &&
     (sys_plmn_match(mm_serving_plmn.info.plmn, mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn)) &&
     (mm_serving_plmn.info.csg_info.csg_id != SYS_CSG_ID_INVALID))
  {
    is_manual_csg_selection_success = TRUE;
  }
#endif 

  if(mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
  {
    if((mm_timer_status[TIMER_T3346] == TIMER_ACTIVE) &&
       (mm_serving_plmn.ms_op_mode == SYS_SRV_DOMAIN_CS_PS) &&
       (((mm_serving_plmn.network_op_mode == NETWORK_OP_MODE_1) && 
       (((is_serving_rai_changed) && (gmm_state == GMM_REGISTERED) &&
         (mm_gs_association_established)) ||
        ((mm_manual_mode_user_selection_procedure &&
          (sys_plmn_match(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn,
                          gmm_3gpp_t3346_plmn_id) )) &&
         (mm_location_information.location_update_status == NOT_UPDATED) &&
         (mm_gs_association_established))
#ifdef FEATURE_FEMTO_CSG
         ||
         ((mm_manual_mode_user_selection_procedure) &&
          (is_manual_csg_selection_success) && 
          (!mm_is_csg_in_white_list(mm_serving_plmn.info.plmn, &mm_serving_plmn.info.csg_info)) &&
          (mm_gs_association_established))
#endif
))
#if defined(FEATURE_LTE)
         ||
         (((memcmp( mm_location_information.lai, mm_serving_lai, LAI_SIZE ) == 0) &&
#ifndef FEATURE_NAS_REL11
            (emm_ctrl_data_ptr->TIN == NAS_MM_TIN_GUTI) &&
            (emm_ctrl_data_ptr->emm_guti_valid == TRUE) &&
#endif
            (mm_is_lu_to_be_initiated_t3346_ltogw == TRUE)&&
            (mm_csfb_state == MM_CSFB_STATE_NULL)
            )


#ifdef FEATURE_SGLTE
          && MM_IS_SGLTE_ON_PS_STACK_ID)
#else
       )
#endif
#endif
      ))
    {
       /*24.008: 4.4.1
        The normal location updating procedure shall also be started when the MS, configured to use CS fallback and SMS over SGs, 
        or SMS over SGs only, enters a GERAN or UTRAN cell after intersystem change from S1 mode to Iu or A/Gb mode, if timer T3346 is running, 
        and the location area of the current cell is the same as the stored location area.*/
      /**----------------------------------------------------------------------
    **The normal location updating procedure shall also be started when the MS is both IMSI attached for GPRS
    **and non-GPRS services and enters a new routing area where the network operates 
    **in network operation mode I and timer T3346 is running.
    **(24.008 section 4.4.1) ------------------------------------------------*/ 
   
      /**------------------------------------------------------------------------
    **The MS shall also start the normal location updating procedure when the network 
    **is operating in network operation mode I, T3346 is running, update status is
    **not U1 UPDATED and the user manually selects the current PLMN.
    **(24.008 section 4.4.1) ------------------------------------------------*/

      is_lu_required = TRUE;
      MSG_HIGH_DS_0(MM_SUB, "=MM= LU has to be initiated due to T3346 running in NMO1");
    }
    if ((!((gmm_rai_changed( gmm_stored_rai.location_area_code,
                             gmm_stored_rai.routing_area_code,
                             mm_serving_plmn.lac, mm_serving_plmn.rac )) ||
           (gmm_plmn_changed(gmm_stored_rai.plmn_id, mm_serving_plmn.info.plmn))) &&
          (gmm_update_status == GMM_GU1_UPDATED) &&
          (gmm_state == GMM_REGISTERED))
#if defined(FEATURE_LTE)
        &&
        ((emm_ctrl_data_ptr->TIN != NAS_MM_TIN_GUTI) 
/*4.1.1.7:At an inter-system change from S1 mode to A/Gb mode, if the timer T3346 is running and the TIN indicates "RAT related TMSI", 
the MS shall set the GPRS update status to GU2 NOT UPDATED and enter state GMM-REGISTERED.ATTEMPTING-TO-UPDATE.*/
#ifdef FEATURE_NAS_REL11
           && (emm_ctrl_data_ptr->TIN != NAS_MM_TIN_RAT_RELATED_TMSI)
#endif
        )
#endif
       )
    {
      gmm_set_substate( GMM_NORMAL_SERVICE);
      gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                       mm_serving_plmn.info.reg_service_domain,
                                       FALSE,
                                       SYS_SRV_STATUS_SRV,
                                       TRUE);
    }
    else if (gmm_is_plmn_equivalent_t3346()) 
    {
      if(gmm_state == GMM_DEREGISTERED)
      {
        gmm_set_substate( GMM_ATTEMPTING_TO_ATTACH);
      }
      else 
      {
        gmm_set_substate( GMM_ATTEMPTING_TO_UPDATE);
      }
      gmm_update_status = GMM_GU2_NOT_UPDATED;

      mmsim_save_ps_location_information( gmm_stored_ptmsi,
                                          gmm_stored_ptmsi_signature,
                                          gmm_stored_rai,
                                          gmm_update_status );

      gmm_update_mm_serving_plmn_info( mm_serving_plmn.info.sim_state,
                                       mm_serving_plmn.info.reg_service_domain,
                                       FALSE,
                                       SYS_SRV_STATUS_LIMITED,
                                       TRUE );
    }
  }
  return is_lu_required;
}

/*===========================================================================
FUNCTION    MM_WRITE_BACKOFF_REMAINING_INFO_TO_EFS

DESCRIPTION
  write the remainig t3246/t3346 value and PLMN info to the efs

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mm_write_backoff_remaining_info_to_efs(void)
{
  int32 efs_status;

  mm_backoff_nv_info_s_type backoff_timer;

  memset(&backoff_timer, 0x00 , sizeof(mm_backoff_nv_info_s_type));

  /* To store the remaining T3346 value in case of 3GPP2 operators */
  if(mm_timer_status[TIMER_T3346] == TIMER_ACTIVE)
  {
    gmm_t3346_backoff_timer_value = 0;
     /* Find out how much timeout is left */
    gmm_t3346_backoff_timer_value = (dword) gs_enquire_timer_ext ( 
                                                 get_mm_timer_queue(), 
                                                 TIMER_T3346,
                                                 TRUE
                                              );
     
    mm_stop_timer(TIMER_T3346);
  }
  
  if(mm_timer_status[TIMER_T3246] == TIMER_ACTIVE)
  {   
    mm_t3246_backoff_timer_value = 0;

        /* Find out how much timeout is left */
    mm_t3246_backoff_timer_value = (dword) gs_enquire_timer ( get_mm_timer_queue(), 
                                    #ifdef FEATURE_DUAL_SIM
                                           MM_TIMER_TAG_AS_ID(TIMER_T3246) 
                                    #else
                                           TIMER_T3246
                                    #endif
                                           );
    mm_stop_timer(TIMER_T3246);
  }
  
#ifdef FEATURE_DUAL_SIM
  if(!MM_SUB_IS_SGLTE &&
     (mm_sub_id == MM_AS_ID_2))
  {

    backoff_timer.t3246_value = mm_t3246_backoff_timer_value;
    backoff_timer.t3346_value = gmm_t3346_backoff_timer_value;

    memscpy(&backoff_timer.t3246_plmn, sizeof(sys_plmn_id_s_type), &mm_3gpp_t3246_plmn_id, 
    sizeof(sys_plmn_id_s_type));

    memscpy(&backoff_timer.t3346_plmn, sizeof(sys_plmn_id_s_type), &gmm_3gpp_t3346_plmn_id, 
    sizeof(sys_plmn_id_s_type));

    /* update the value in efs */   
    if((efs_status = mcfg_fs_write("/nv/item_files/modem/nas/mm_backoff_remaining_info", 
                      &backoff_timer,
                      sizeof(mm_backoff_nv_info_s_type), MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR, MCFG_FS_ALLPERMS, 
                      MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_sub_id))!= MCFG_FS_STATUS_OK)
    {
       MSG_ERROR_DS(MM_SUB,"failed to update the t3246 value in efs for SIM2, status = %d",efs_status,0,0);
    } 
    else
    {
      MSG_HIGH_DS_8(MM_SUB, "=MM= Updated t3346 timer val from NV - %lu , t3246 timer val from NV - %lu Updated t3346 backoff PLMN with val %X,%X,%X Updated t3246 backoff PLMN with val %X,%X,%X", backoff_timer.t3346_value, backoff_timer.t3246_value,backoff_timer.t3346_plmn.identity[0],backoff_timer.t3346_plmn.identity[1],backoff_timer.t3346_plmn.identity[2],backoff_timer.t3246_plmn.identity[0], backoff_timer.t3246_plmn.identity[1],backoff_timer.t3246_plmn.identity[2]);
    }
  }
#endif
  if(mm_sub_id == MM_AS_ID_1)
  {
    memset(&backoff_timer, 0x00 , sizeof(mm_backoff_nv_info_s_type));
    backoff_timer.t3246_value = mm_t3246_backoff_timer_value;
    backoff_timer.t3346_value = gmm_t3346_backoff_timer_value;

    memscpy(&backoff_timer.t3246_plmn, sizeof(sys_plmn_id_s_type), &mm_3gpp_t3246_plmn_id, 
    sizeof(sys_plmn_id_s_type));

    memscpy(&backoff_timer.t3346_plmn, sizeof(sys_plmn_id_s_type), &gmm_3gpp_t3346_plmn_id, 
    sizeof(sys_plmn_id_s_type));

    /* update the value in efs */   
    if((efs_status = mcfg_fs_write("/nv/item_files/modem/nas/mm_backoff_remaining_info", 
                      &backoff_timer,
                      sizeof(mm_backoff_nv_info_s_type), MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR, MCFG_FS_ALLPERMS, 
                      MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_sub_id))!= MCFG_FS_STATUS_OK)
    {
       MSG_ERROR_DS(MM_SUB,"failed to update the t3246 value in efs for SIM1, status= %d",efs_status,0,0);
    } 
    else
    {
       MSG_HIGH_DS_8(MM_SUB, "=MM= Updated t3346 timer val from NV - %lu , t3246 timer val from NV - %lu Updated t3346 backoff PLMN with val %X,%X,%X Updated t3246 backoff PLMN with val %X,%X,%X", backoff_timer.t3346_value, backoff_timer.t3246_value,backoff_timer.t3346_plmn.identity[0],backoff_timer.t3346_plmn.identity[1],backoff_timer.t3346_plmn.identity[2],backoff_timer.t3246_plmn.identity[0], backoff_timer.t3246_plmn.identity[1],backoff_timer.t3246_plmn.identity[2]);
    }
  }
} /*end of mm_timer_t3246_remaining_to_efs*/

#endif
#ifdef FEATURE_NAS_REL11
/*===========================================================================
FUNCTION    MM_IS_RRC_ACCESS_CLASS_11_to_15

DESCRIPTION
  check ACCESS CLASS is 11-15 or not

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean mm_is_rrc_access_class_11_to_15(void)
{
  if(((mm_hi_access_class & RRC_ACCESS_CLASS_11_BIT) == RRC_ACCESS_CLASS_11_BIT)||
    ((mm_hi_access_class & RRC_ACCESS_CLASS_12_BIT) == RRC_ACCESS_CLASS_12_BIT)||
    ((mm_hi_access_class & RRC_ACCESS_CLASS_13_BIT) == RRC_ACCESS_CLASS_13_BIT)||
    ((mm_hi_access_class & RRC_ACCESS_CLASS_14_BIT) == RRC_ACCESS_CLASS_14_BIT)||
    ((mm_hi_access_class & RRC_ACCESS_CLASS_15_BIT) == RRC_ACCESS_CLASS_15_BIT))
  {
    return TRUE;
  }
  return FALSE;
}
#endif
#ifdef FEATURE_SGLTE
/*************************************************************
FUNCTION mm_is_cs_timer

DESCRIPTION
  To find MM timers related to CS stack in SGLTE mode which PS stack wouldnt maintain

DEPENDENCIES
  None

RETURN VALUE
  TRUE if MM timer is related to CS stack

SIDE EFFECTS
  None
 *************************************************************/
boolean mm_is_cs_timer(timer_id_T timer_id)
{
  boolean return_val = FALSE;

  switch (timer_id)
  {
    case TIMER_T3210:
    case TIMER_T3211:
    case TIMER_T3212:
    case TIMER_T3213:
    case TIMER_T3220:
    case TIMER_T3230:
    case TIMER_T3240:
    case TIMER_T3246:
    case TIMER_T3214:
    case TIMER_T3216:
    case TIMER_T3218:
    case TIMER_T3241:
    case TIMER_T3242:
    case TIMER_T3243:
#ifdef FEATURE_DUAL_SIM
    case TIMER_MM_RADIO_RETRY:
#endif
    case TIMER_SCRI_PAGING:
        return_val = TRUE;
      break;
    default:
      break;
  }

  return return_val;
}
#endif

#if defined (FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
#ifdef FEATURE_DUAL_DATA
/*************************************************************
FUNCTION mm_read_max_active_data

DESCRIPTION
  This function reads max active data from PM

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None
 *************************************************************/
void mm_read_max_active_data(void)
{
  policyman_status_t        status;
  policyman_item_t const    *pItem;
  policyman_item_id_t const id = POLICYMAN_ITEM_DEVICE_CONFIGURATION;

  status = policyman_get_items_block(&id, 1, &pItem);
  if(POLICYMAN_SUCCEEDED(status))
  {
    status = policyman_device_config_max_active_data(pItem, &mm_max_active_data);
    if(POLICYMAN_FAILED(status))
    {
      mm_max_active_data = 1;
    }
    policyman_item_release(pItem);
  }
   MSG_HIGH_DS_1(MM_SUB, "max_active_data = %d", mm_max_active_data);
}
#endif

/*************************************************************
FUNCTION mm_read_device_configuration

DESCRIPTION
  This function reads device configuration from PM

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None
 *************************************************************/
void mm_read_device_configuration
(
  void
)
{
#ifdef FEATURE_DUAL_SIM
  policyman_status_t        status, status_1;
  policyman_item_t const    *pItem;
  policyman_item_id_t const id = POLICYMAN_ITEM_DEVICE_CONFIGURATION;
  size_t                  num_of_sims = 1;
  size_t                  num_of_active = 1;  
  sys_subs_feature_t  sub_feature_mode = SYS_SUBS_FEATURE_MODE_NORMAL;
  mm_as_id_e_type    as_id;

  mmode_nv_setting_for_device_mode = SYS_MODEM_DEVICE_MODE_SINGLE_SIM;
  
  status = policyman_get_items_block(&id, 1, &pItem);
  if (POLICYMAN_SUCCEEDED(status))
  {
    status = policyman_device_config_num_sims(pItem, &num_of_sims);
    status_1 = policyman_device_config_max_active(pItem, &num_of_active);

    if(POLICYMAN_SUCCEEDED(status) && POLICYMAN_SUCCEEDED(status_1))
    {
      mm_sim_max_subscription = num_of_sims;
#ifdef FEATURE_TRIPLE_SIM
      if(num_of_sims == 3)
      {
        mmode_nv_setting_for_device_mode = SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY;
      }
      else 
#endif 
      if(num_of_sims == 2)
      {
#ifdef FEATURE_DUAL_ACTIVE
        if(num_of_active > 1)
        {
          mmode_nv_setting_for_device_mode = SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE;
        }
        else
#endif 
        {
          mmode_nv_setting_for_device_mode = SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY;
        }
      }
    }
    for(as_id = MM_AS_ID_1; ((uint8)as_id < num_of_sims && num_of_sims <= MAX_AS_IDS); as_id++)
    {
      mm_sub_config_sim[as_id] = SYS_OVERALL_FEATURE_MODE_MULTISIM;
      status = policyman_device_config_get_subs_feature(pItem, 0, (size_t)as_id, &sub_feature_mode);
      if(POLICYMAN_SUCCEEDED(status))
      {
        if(sub_feature_mode == SYS_SUBS_FEATURE_MODE_SGLTE)
        {
          mm_sub_config_sim[as_id] = SYS_OVERALL_FEATURE_MODE_SGLTE;
          mm_mn_sglte_svlte_mode = TRUE;
        }
        else if (sub_feature_mode == SYS_SUBS_FEATURE_MODE_SVLTE) 
        {
          mm_mn_sglte_svlte_mode = TRUE;
        }
      }
    }
    policyman_item_release(pItem);
  }
  else
  {
    MSG_HIGH_DS_0(MM_SUB,"Reading of policy manager failed");
    for(as_id = MM_AS_ID_1; as_id < (mm_as_id_e_type)MAX_AS_IDS; as_id++)
    {
      mm_sub_config_sim[as_id] = SYS_OVERALL_FEATURE_MODE_MULTISIM;
    }
  }

#ifdef FEATURE_DUAL_DATA
  mm_read_max_active_data();
#endif

#endif

}

#endif

/*************************************************************
FUNCTION mm_get_mscr_system_information

DESCRIPTION
  This function gets mscr information from mm_system_information.

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None
 *************************************************************/
byte mm_get_mscr_system_information
(
  sys_modem_as_id_e_type sub_id
)
{
  if((sub_id > SYS_MODEM_AS_ID_NONE) && (sub_id < MAX_AS_IDS) )
  {
#ifdef FEATURE_DUAL_SIM
#if defined(FEATURE_SGLTE)
    if(MM_SUB_IS_SGLTE_SUB(sub_id))
    {
      return (mm_system_information_sim[mm_cs_stack_id].mscr);
    }
    else
#endif
    {
      return (mm_system_information_sim[sub_id].mscr);
    }
#elif defined (FEATURE_SGLTE)
    return (mm_system_information_sim[mm_cs_stack_id].mscr);
#else
    (void)sub_id;
    return mm_system_information.mscr;
#endif
  }
  else
  {
      return 0;
  }
}


/*************************************************************
FUNCTION mm_get_nv_force_mscr_r99

DESCRIPTION
  This function gets mscr NV information 

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None
 *************************************************************/
ue_force_rel_mode_type  mm_get_nv_force_mscr_r99
(
  sys_modem_as_id_e_type sub_id
)
{
  if((sub_id > SYS_MODEM_AS_ID_NONE) && (sub_id < MAX_AS_IDS) )
  {
#ifdef FEATURE_DUAL_SIM
     return (mm_nv_force_mscr_r99_sim[sub_id]);
#else
    (void)sub_id;
    return mm_nv_force_mscr_r99;
#endif
  }
  else
  {
      return NV_FORCE_UE_REL99;
  }

}

void mm_update_cell_status(rrc_abort_cause_e_type abort_cause)
{  
  if((mm_system_information.ppac_present == TRUE) && is_umts_mode())
  {
      if(abort_cause == RRC_AC_NOT_ALLOWED_CS_DOMAIN)
      {
        if((mm_state == MM_WAIT_FOR_NETWORK_COMMAND) &&(mm_last_paging_cause != 0xFF))
        {
          MSG_HIGH_DS_1(MM_SUB ," cs_reg_allowed should not be updated as paging is in progress , mm_last_paging_cause %d ", mm_last_paging_cause);
        }
        else
        {
          mm_system_information.cs_reg_allowed = FALSE;
        }       
      }
      MSG_HIGH_DS_1(MM_SUB ,"Set CS Cell access : %d ", mm_system_information.cs_reg_allowed);
  }
#ifdef FEATURE_DSAC
  else if ((mm_system_information.dsac_present == TRUE) && is_umts_mode())
  {
      if(abort_cause == RRC_AC_NOT_ALLOWED_CS_DOMAIN)
      {
        mm_system_information.cs_cell_access = MM_AS_ACCESS_EMERGENCY_ONLY;
      }
   
      MSG_HIGH_DS_1(MM_SUB ,"Set CS Cell access : %d  ",mm_system_information.cs_reg_allowed);
  }
  else
  {
      mm_system_information.cell_access = MM_AS_ACCESS_EMERGENCY_ONLY;
  }
#else
  else
  {
      mm_system_information.cell_access = MM_AS_ACCESS_EMERGENCY_ONLY;
  }
#endif 
}

/*************************************************************
FUNCTION mm_convert_rat_to_tech_id

DESCRIPTION
  This function converts field of enumeration type sys_radio_access_tech_e_type
  into enumeration type sys_radio_access_tech_e_type

DEPENDENCIES
  None

Input comes from
  sys_radio_access_tech_e_type  rat_arg

Output goes into
  sys_sys_mode_e_type *tech_id_arg

RETURN VALUE
  None


SIDE EFFECTS
  None
 *************************************************************/

void mm_convert_rat_to_tech_id(sys_radio_access_tech_e_type  rat_arg, sys_sys_mode_e_type *tech_id_arg)
{

    if(rat_arg == SYS_RAT_GSM_RADIO_ACCESS)
    {
      *tech_id_arg = SYS_SYS_MODE_GSM;
    }
    else if(rat_arg == SYS_RAT_UMTS_RADIO_ACCESS)
    {
      *tech_id_arg = SYS_SYS_MODE_WCDMA;
    }
    else if(rat_arg == SYS_RAT_LTE_RADIO_ACCESS)
    {
      *tech_id_arg = SYS_SYS_MODE_LTE;
    }
    else if(rat_arg == SYS_RAT_TDS_RADIO_ACCESS)
    {
      *tech_id_arg = SYS_SYS_MODE_TDS;
    }
}

#ifdef FEATURE_SGLTE
/*************************************************************
FUNCTION mm_sglte_activation_ind_allowed

DESCRIPTION
  This function checks if activation indication is allowed in SGLTE mode

DEPENDENCIES
  None

Input comes from
  none

RETURN VALUE
  boolean


SIDE EFFECTS
  None
 *************************************************************/

boolean mm_sglte_activation_ind_allowed(void)
{
  boolean activation_ind_allowed = TRUE;
  if(MM_SUB_IS_SGLTE && 
     (mm_as_id == mm_ps_stack_id) && 
     (mm_cs_stack_id != mm_ps_stack_id))
  {
     if((mm_waiting_for_deact_cnf_sim[mm_cs_stack_id] == TRUE) || 
        (mm_waiting_for_stop_mode_cnf_sim[mm_cs_stack_id] == TRUE) ||
        (mm_waiting_for_service_cnf_sim[mm_cs_stack_id] == TRUE) ||
        (mm_plmn_search_in_progress_sim[mm_cs_stack_id] == TRUE))
     {
       MSG_HIGH_DS_0(MM_SUB,"=MM= User action pending on CS stack - Ignoring ACTIVATION_IND on PS stack and GERAN aborts reselction");
       activation_ind_allowed = FALSE;
     }
     else if( mm_state_sim[mm_cs_stack_id] == MM_WAIT_FOR_RR_CONNECTION_IMSI_DETACH ||
              mm_state_sim[mm_cs_stack_id] == MM_IMSI_DETACH_INITIATED||
              mm_nreg_req_received_sim[mm_cs_stack_id] == TRUE)
     {
       MSG_HIGH_DS_1(MM_SUB,"=MM= IMSI detach being processed in CS stack - Ignoring ACTIVATION_IND on PS stack and processing stop mode req OR mm_nreg_req_received = %d",mm_nreg_req_received_sim[mm_cs_stack_id]);
       mmcoord_process_stop_mode_on_hybrid_stack(); 
       activation_ind_allowed = FALSE;
     }
  }
  return activation_ind_allowed;
}
#endif

#ifdef FEATURE_SGLTE_DUAL_SIM
/*===========================================================================

FUNCTION 

DESCRIPTION
  This function is used to determine if PS transfer should begin. This
  is based on quering if there is a active CS call and DTM supported
  
DEPENDENCIES
  None

RETURN VALUE
   False if there is a active CS call and DTM supported: do not allow 
   PS transfer. True otherwise - allow PS tranfserf
  
SIDE EFFECTS
  None

===========================================================================*/
boolean  mm_get_verify_ps_transfer_conditions
(

  sys_modem_as_id_e_type as_id
)
{

  if (IS_NOT_VALID_STACK_ID(as_id))
  {
    MSG_ERROR_DS(MM_LOCAL_SUB," Invalid AS ID: %d", as_id+1,0,0);
    return FALSE;
  }

#ifdef FEATURE_GSM_DTM
  if(mm_serving_plmn_sim[as_id].dtm_supported)
  {
      /* Allow PS transfer */
      MSG_HIGH_DS_0(MM_LOCAL_SUB, " DTM supported, allow PS transfer");
      return TRUE;
  }
#endif  
  /* Determine if the CS call is active on GSM */
  if ((mm_serving_plmn_sim[as_id].info.active_rat == SYS_RAT_GSM_RADIO_ACCESS) &&
      (rrc_connection_status_sim[as_id] == MMRRC_ACTIVE))
  {
       MSG_HIGH_DS_0(MM_LOCAL_SUB, " Disallow PS transfer");
       /* Disallow PS transfer */       
       return FALSE;
  }
#ifdef FEATURE_SGLTE
  if (MM_IS_IN_SGLTE_DUAL_STACK_MODE && ((mm_waiting_for_stop_mode_cnf_sim[as_id]==TRUE)||
      (mm_nreg_req_received_sim[as_id]==TRUE )))
  {
    MSG_HIGH_DS_0(MM_LOCAL_SUB, "=MM= Disallow PS transfer as Stop mode or nreg received is pending");           
    return FALSE;
  }
#endif
  MSG_HIGH_DS_0(MM_LOCAL_SUB, " Allow PS transfer ");  
  return TRUE;
}
#endif


/*===========================================================================

FUNCTION: mm_cs_registration_vote_for_reselection()

DESCRIPTION:    Vote No RESEL if LAU is on going or PAGE is being responded

DEPENDENCIES:

PARAMETERS: as_id

RETURN VALUE:

SIDE EFFECTS:
  None
===========================================================================*/

nas_vote_for_reselection_e_type mm_cs_registration_vote_for_reselection(sys_modem_as_id_e_type as_id)
{

   nas_vote_for_reselection_e_type ret_val     = NAS_VOTE_FOR_RESEL_TRUE;

#if defined(FEATURE_DUAL_SIM)

   MSG_HIGH_DS_1(MM_SUB, "=MM= mm_cs_registration_vote_for_reselection - Recd as_id = %d", as_id);
   if (IS_NOT_VALID_AS_ID(as_id))
   {
     ret_val = NAS_VOTE_FOR_RESEL_TRUE;
   }

   /* If a decision is not made yet, check if MM is in LU proc */
   else if( (mm_state_sim[as_id] == MM_WAIT_FOR_RR_CONNECTION_LU ||
      mm_state_sim[as_id] == MM_LOCATION_UPDATE_INITIATED))
   {
     MSG_HIGH_DS_1(MM_SUB, "=MM= mm_cs_registration_vote_for_reselection votes %d, LU in progress", NAS_VOTE_FOR_RESEL_FALSE);
     ret_val = NAS_VOTE_FOR_RESEL_FALSE;
   }

   else if( mm_controls_resel_dec_mt_cs_call_sim[as_id] == TRUE)
   {
     MSG_HIGH_DS_1(MM_SUB, "=MM= mm_cs_registration_vote_for_reselection votes %d, MT page in progress", NAS_VOTE_FOR_RESEL_FALSE);
     ret_val = NAS_VOTE_FOR_RESEL_FALSE;
   }

#else

   /* If a decision is not made yet, check if MM is in LU proc */
   if((mm_state == MM_WAIT_FOR_RR_CONNECTION_LU ||
      mm_state == MM_LOCATION_UPDATE_INITIATED))
   {
     MSG_HIGH_DS_1(MM_SUB, "=MM= mm_cs_registration_vote_for_reselection votes %d, LU in progress", NAS_VOTE_FOR_RESEL_FALSE);
     ret_val = NAS_VOTE_FOR_RESEL_FALSE;
   }
   /* Check MM connection for MT call is not being made */
   else if(mm_controls_resel_dec_mt_cs_call)
   {
     MSG_HIGH_DS_1(MM_SUB, "=MM= mm_cs_registration_vote_for_reselection votes %d, MT page in progress", NAS_VOTE_FOR_RESEL_FALSE);
     ret_val = NAS_VOTE_FOR_RESEL_FALSE;
   }
#endif

   MSG_HIGH_DS_1(MM_SUB, "mm_cs_registration_vote_for_reselection returns %d",ret_val);
   return ret_val;

}
/*************************************************************
FUNCTION mm_log_mm_msg_info

DESCRIPTION
  This is to log message info before adding to debug buffer

DEPENDENCIES
  None

Input

RETURN VALUE
  None


SIDE EFFECTS
  None
 *************************************************************/

void mm_log_mm_msg_info(byte pd,byte msg_id)
{
  byte temp_pd = 0; 
  /* For logging message cause  and PD in 1 bye, decided for temp mapping
         00 ->reserved , 01--> SM, 02--> MM , 03 --> GMM  
         mm_msg_info--   | 7|6 |      | 5 |4  |3  |2  | 1 |0|
                                 <temp_PD>  <msg id        >   */
  if (pd == PD_MM)
  {
    temp_pd = 2;
  }
  else if (pd == PD_GMM)
  {
    temp_pd = 3;
  }
  else if (pd == PD_GPRS_SM)
  {
    temp_pd = 1;
  }
  if (temp_pd!=0)
  {
    mm_msg_info =(byte) ((temp_pd<<6)|(msg_id & 0x3F));
  }
}

/*===========================================================================

FUNCTION: mm_clear_manual_mode_user_selection_proc_flags

DESCRIPTION:   To clear MM/GMM/EMM manual_mode_user_selection flag

DEPENDENCIES:

PARAMETERS: 

RETURN VALUE:

SIDE EFFECTS:
  None
===========================================================================*/
void mm_clear_manual_mode_user_selection_proc_flags()
{
  mm_manual_mode_user_selection_procedure = FALSE;
  gmm_manual_mode_user_selection_procedure = FALSE;
#ifdef FEATURE_LTE
  mm_set_emm_manual_selection_flag(FALSE);
#endif
}
/*************************************************************
FUNCTION mm_check_other_mm_cnm_connections

DESCRIPTION
  This is a DUAL_SIM function. This API checks if there is any other
  subscription for which MM state is 14. If yes, abort connection establishment
  procedure on that subscription.

DEPENDENCIES
  None

Input

RETURN VALUE
  None


SIDE EFFECTS
  None
 *************************************************************/


#ifdef FEATURE_DUAL_SIM
void mm_check_other_mm_cnm_connections()
{
   sys_modem_as_id_e_type as_id_ctr, buffer_mm_as_id = (sys_modem_as_id_e_type)mm_as_id;
   if(MMCNM_IS_MODE_DUAL_STANDBY)

   {

     for(as_id_ctr = SYS_MODEM_AS_ID_1; as_id_ctr < MAX_NAS_STACKS; as_id_ctr++)
     {
        if(as_id_ctr != buffer_mm_as_id)
        {
          mm_as_id = as_id_ctr;
          mm_sub_id = mm_sub_id_stack[mm_as_id];
          if(mm_state == MM_WAIT_FOR_RR_CONNECTION_MM)
          {
            MSG_HIGH_DS(MM_SUB,"Sending MO call reject and aborting connection as MT call already in progress on other sub",0,0,0);
            mm_state_control(MM_STD_LOW_LEVEL_FAIL);
            mm_abort_rr( RRC_CS_DOMAIN_CN_ID, FALSE );
          }
#ifdef FEATURE_LTE
          else if(is_lte_mode())
          {    
            mm_abort_service_request_if_needed();
          }
          else if(mm_csfb_state != MM_CSFB_STATE_NULL) 
          {
            MSG_HIGH_DS_0(MM_SUB,"Sending MO call reject as MT call already in progress on other sub");
            mm_csfb_rej_pending_call();
          }
#endif
        }
     }
     mm_as_id = buffer_mm_as_id;
     mm_sub_id = mm_sub_id_stack[mm_as_id];
   
   }
}
#endif

/*************************************************************
FUNCTION is_ps_call_active_while_srv_status_SRV

DESCRIPTION
  This function returns the value TRUE or FALSE.
  TRUE: If MM received REG_REQ(RAT priority list is superset of previous one) in MM state#9, while PS call was active(UE is having svc) and no svc_domain change or rat change was requested through new svc_req.
  FALSE: Otherwise

DEPENDENCIES
  None

RETURN VALUE
True or False.

SIDE EFFECTS
  None
 *************************************************************/
boolean is_ps_call_active_while_srv_status_SRV(mm_cmd_type *message)
{

boolean result = FALSE;
    if(
#ifndef FEATURE_RAT_PRIORITY_LIST
#if defined (FEATURE_WTOG_BACKGROUND_PLMN_SEARCH) || defined (FEATURE_WTOW_BACKGROUND_PLMN_SEARCH)
         (
          
#ifdef FEATURE_DUAL_SIM
          (sm_is_pdp_active((sys_modem_as_id_e_type)mm_sub_id)) &&
#else
          (sm_is_pdp_active()) &&
#endif
           (mm_serving_plmn.info.service_status == SYS_SRV_STATUS_SRV) &&
           
             ((message->cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC ||
            message->cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL) &&
           (!mm_rat_change_requested( message->cmd.mmr_reg_req.rat )) &&
           (!mm_service_change_requested( message->cmd.mmr_reg_req.req_service_domain )) &&
           ((mm_ue_band_class_pref & message->cmd.mmr_reg_req.band_pref) == mm_ue_band_class_pref) && 
              (mm_already_camped( message))))
#endif
#else
         (
         
#ifdef FEATURE_DUAL_SIM
         (sm_is_pdp_active((sys_modem_as_id_e_type)mm_sub_id)) &&
#else
         (sm_is_pdp_active()) &&
#endif
           (mm_serving_plmn.info.service_status == SYS_SRV_STATUS_SRV) &&
           
            ((message->cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC ||
            message->cmd.mmr_reg_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL) &&
           (!mm_rat_change_requested( message->cmd.mmr_reg_req.rat )) &&
           (!mm_service_change_requested( message->cmd.mmr_reg_req.req_service_domain )) &&
           (mm_checkif_rat_pri_list_superset(&message->cmd.mmr_reg_req.rat_pri_list_info,
                                                                            &mm_rat_pri_list_info)) 
#ifdef FEATURE_LTE
            && 
           (!mm_checkif_rat_added_to_rat_pri_list(&message->cmd.mmr_reg_req.rat_pri_list_info,
                                                    &mm_rat_pri_list_info,SYS_RAT_LTE_RADIO_ACCESS))
#endif  
            && 
             (mm_already_camped( message))))
#endif /* FEATURE_RAT_PRIORITY_LIST */
    )
    {
      result= TRUE;
    }

      return result;
    
}

#ifdef FEATURE_DUAL_SIM
/*************************************************************
FUNCTION mm_check_mplmn_in_progress_on_other_subs

DESCRIPTION
  This function checks if RAT other than current RAT has manual plmn search in progress

DEPENDENCIES
  None

RETURN VALUE
  True if there is mplmn search on other RAT,otherwise return False

SIDE EFFECTS
  None
 *************************************************************/
boolean  mm_check_mplmn_in_progress_on_other_subs( void )
{
  mm_as_id_e_type      mm_other_as_id, mm_tmp_as_id;
  boolean in_progress = FALSE;
  mm_tmp_as_id = mm_as_id;

  if (MMCNM_IS_MODE_DUAL_ACTIVE && (mm_mn_sglte_svlte_mode == FALSE))
  {
      return in_progress;
  }
  for( mm_other_as_id = MM_AS_ID_1; mm_other_as_id < (mm_as_id_e_type)MAX_NAS_STACKS ; mm_other_as_id++ )
  {
    if( mm_other_as_id != mm_tmp_as_id )
    {
      mm_as_id = mm_other_as_id;
      mm_sub_id = mm_sub_id_stack[mm_as_id];
#ifdef FEATURE_LTE
      if(is_lte_mode() == TRUE)
      {
        emm_ctrl_data_ptr = emm_db_get_ctrl_data();
        if( emm_ctrl_data_ptr->plmn_search_in_progress == TRUE )
        {
          MSG_HIGH_DS_0(MM_SUB,"=EMM= Manual PLMN search in progress on L RAT ");
          in_progress = TRUE ;
          break;
        }
      }
      else 
#endif
      if(((is_umts_mode() == TRUE )||
               (is_gsm_mode() == TRUE ))&&
              (mm_waiting_for_list_cnf_type == SYS_NETWORK_SELECTION_MODE_MANUAL ) &&
              (mm_plmn_search_in_progress == TRUE))
      {
        MSG_HIGH_DS_0(MM_SUB,"=EMM= Manual PLMN search in progress on W/T/G RAT ");
        in_progress = TRUE ;
        break;
      }
    }
  }
  mm_as_id = mm_tmp_as_id;
  mm_sub_id = mm_sub_id_stack[mm_as_id];
#ifdef FEATURE_LTE
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
#endif
  return in_progress;
}
#endif

/*===========================================================================
Function: nasdiag_plmn_list_event
Description:

       This function sends a diag event when the PLMN NETWORK LIST arrives.

============================================================================*/

void  nasdiag_plmn_list_event
(
        sys_detailed_plmn_list_s_type *plmn_list_ptr,
           
        sys_modem_as_id_e_type    as_id
)
{
  uint32 i, j=1;
  byte event_packet[71];
  memset(event_packet, 0, 71);

  if(plmn_list_ptr == NULL )
  {  
    return;
  }
  else
  {
    MSG_HIGH_DS_1(MM_SUB, "=MM= No of PLMNs in Available PLMN List = %d", plmn_list_ptr->length);
  }

  event_packet[0] = (byte)as_id;
  for( i=0; ((i< plmn_list_ptr->length) && (i<SYS_PLMN_LIST_MAX_LENGTH)) && ((j>=1) && (j<=71)); i++)
  {
    j=1;
    memscpy(&(event_packet[j]), 3, plmn_list_ptr->info[i].plmn.identity, 3);
    j=j+3;
    event_packet[j++] = (byte)plmn_list_ptr->info[i].rat;
    event_packet[j++] = (byte)plmn_list_ptr->info[i].plmn_service_capability;
    event_packet[j++] = (byte)plmn_list_ptr->info[i].plmn_forbidden;
    event_packet[j++] = (byte)plmn_list_ptr->info[i].list_category;
    event_packet[j++] = (byte)plmn_list_ptr->info[i].signal_quality;
    memscpy(&(event_packet[j]), 4, &(plmn_list_ptr->info[i].signal_strength), 4);
    j=j+4;
    memscpy(&(event_packet[j]), 4, &(plmn_list_ptr->info[i].csg_info.csg_id), 4);
    j=j+4;
    event_packet[j++] = (byte)plmn_list_ptr->info[i].csg_info.hnb_name.length;
    memscpy(&(event_packet[j]), 48, plmn_list_ptr->info[i].csg_info.hnb_name.name, 48);
    j=j+48;
    event_packet[j++] = (byte)plmn_list_ptr->info[i].csg_info.hybrid_cell;
    memscpy(&(event_packet[j]), 4, &(plmn_list_ptr->info[i].earfcn), 4);
    event_report_payload( EVENT_NAS_PLMN_FOUND, 71, (void*)event_packet);
  }
}

#ifdef FEATURE_NAS_RAU_ON_MS_RAC_CHANGE
void mm_save_rat_pri_list_sent_to_nw(void)
{
  memscpy(&mm_rat_pri_list_info_to_nw,sizeof(sys_rat_pri_list_info_s_type),&mm_rat_pri_list_info,
         sizeof(sys_rat_pri_list_info_s_type));
}
#endif

/*************************************************************
FUNCTION mm_in_conn_management_state

DESCRIPTION
  This is a DUAL_SIM function. This API checks if there is any other
  subscription for which MM state is 14. If yes, abort connection establishment
  procedure on that subscription.

DEPENDENCIES
  None

Input

RETURN VALUE
  None


SIDE EFFECTS
  None
 *************************************************************/

boolean mm_in_conn_management_state()
{

   if((mm_state == MM_WAIT_FOR_RR_CONNECTION_MM) ||
            (mm_state == MM_WAIT_FOR_OUTGOING_MM_CONNECTION) ||
            (mm_state == MM_WAIT_FOR_ADDITIONAL_OUTGOING_MM_CONNECTION) ||
            (mm_state == MM_WAIT_FOR_REESTABLISH_DECISION) ||
            (mm_state == MM_WAIT_FOR_RR_CONNECTION_REESTABLISHMENT) ||
            (mm_state == MM_REESTABLISHMENT_INITIATED) ||
            (mm_state == MM_CONNECTION_ACTIVE) ||
            (mm_state == MM_WAIT_FOR_NETWORK_COMMAND)
#ifdef FEATURE_CCBS
                                               ||
            (mm_state == MM_PROCESS_CM_SERVICE_PROMPT)
#endif /* FEATURE_CCBS */
      )
   {
      return TRUE;
   }
   return FALSE;

}

/*************************************************************
FUNCTION mm_is_mplmn_search_in_progress

DESCRIPTION
  This function returns the value TRUE or FALSE.
  TRUE: If Manual PLMN search in ongoing 
  FALSE: Otherwise

DEPENDENCIES
  None

RETURN VALUE
True or False.

SIDE EFFECTS
  None
*************************************************************/
boolean mm_is_mplmn_search_in_progress(void)
{
  boolean result = FALSE;
  if((mm_conn_mode_manual_search_enabled == TRUE)&& 
#ifdef FEATURE_DUAL_SIM
     ((mm_check_mplmn_in_progress_on_other_subs() == TRUE)||
#endif
     ((mm_plmn_search_in_progress == TRUE)&&
     (mm_waiting_for_list_cnf_type == SYS_NETWORK_SELECTION_MODE_MANUAL))
#ifdef FEATURE_DUAL_SIM 
     )
#endif
    )
  {
    result = TRUE;
  }
  return result;
}

/*************************************************************
FUNCTION mm_is_registration_allowed_on_selected_plmn

DESCRIPTION
  This function returns the value TRUE or FALSE.
  TRUE: If registration is allowed on camped PLMN
  FALSE: Otherwise

DEPENDENCIES
  None

RETURN VALUE
True or False.

SIDE EFFECTS
  None
*************************************************************/
boolean mm_is_registration_allowed_on_selected_plmn(mm_cmd_type *message )
{
  inter_task_lai_T       found_lai;
  boolean is_cell_forbidden = TRUE;
  boolean routine_man_selection_failed = FALSE;
  boolean is_nonforbid_plmn = TRUE;
  boolean lu_required = FALSE, attach_required = FALSE;
  boolean result = FALSE;
  serving_plmn_info_T* serving_plmn_ptr;

#ifndef FEATURE_MODEM_HEAP
  serving_plmn_ptr = (serving_plmn_info_T*)gs_alloc(sizeof(serving_plmn_info_T));
#else
  serving_plmn_ptr = (serving_plmn_info_T*)modem_mem_calloc(1, sizeof(serving_plmn_info_T), MODEM_MEM_CLIENT_NAS);
#endif
  if(serving_plmn_ptr == NULL)
  {
    MSG_FATAL_DS(MM_SUB,"=MM= Error: Not able to Allocate MM Buffer", 0, 0, 0);
  }
 
  serving_plmn_ptr->info.plmn = mmumts_convert_rrc_plmn_id_to_nas_plmn_id(
      message->cmd.rrc_service_cnf.service_info.selected_plmn.plmn_id );

  if (message->cmd.rrc_service_cnf.service_info.gsm_map_common_info_incl)
  {
    serving_plmn_ptr->lac.lac[0] =
      message->cmd.rrc_service_cnf.service_info.gsm_map_common_info.value[0];
    serving_plmn_ptr->lac.lac[1] =
      message->cmd.rrc_service_cnf.service_info.gsm_map_common_info.value[1];
  }

  found_lai.PLMN_id.identity[0] = serving_plmn_ptr->info.plmn.identity[0];
  found_lai.PLMN_id.identity[1] = serving_plmn_ptr->info.plmn.identity[1];
  found_lai.PLMN_id.identity[2] = serving_plmn_ptr->info.plmn.identity[2];
  
  found_lai.location_area_code[0] = serving_plmn_ptr->lac.lac[0];
  found_lai.location_area_code[1] = serving_plmn_ptr->lac.lac[1];
  
#ifndef FEATURE_MODEM_HEAP
  gs_free(serving_plmn_ptr);
#else
  modem_mem_free(serving_plmn_ptr, MODEM_MEM_CLIENT_NAS);
#endif
  serving_plmn_ptr = NULL;
  
  if ((!mm_check_forbidden_national_roaming( &found_lai )) &&
      (!mm_check_forbidden_regional_roaming( &found_lai )) &&
#ifdef FEATURE_DUAL_SIM
      (!reg_sim_per_subs_plmn_forbidden( mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id)) &&
      (!(reg_sim_plmn_gprs_forbidden(mm_serving_plmn.info.plmn,(sys_modem_as_id_e_type)mm_sub_id) &&
#else
      (!reg_sim_plmn_forbidden( mm_serving_plmn.info.plmn )) &&
      (!(reg_sim_plmn_gprs_forbidden(mm_serving_plmn.info.plmn ) &&
#endif        
      (mmcoord_pending_reg_message.cmd.mmr_reg_req.req_service_domain == SYS_SRV_DOMAIN_PS_ONLY))))
  {
    is_cell_forbidden = FALSE;
    MSG_HIGH_DS_0(MM_SUB,"=MM= Cell is NOT FORBIDDEN");
  }
  
  if (!((PLMN_MATCH( found_lai.PLMN_id.identity,
                     mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn.identity )) ||
        (sys_eplmn_list_equivalent_plmn(mmcoord_pending_reg_message.cmd.mmr_reg_req.plmn) && 
         sys_eplmn_list_equivalent_plmn(mm_serving_plmn.info.plmn)))  &&
      (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode ==
        SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))
  {
    routine_man_selection_failed = TRUE;
    MSG_HIGH_DS_0(MM_SUB,"=MM= Different PLMN found during MAN_LIMITED_ACQ");
  }
  
  if ((routine_man_selection_failed) ||
     ((((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                        SYS_NETWORK_SELECTION_MODE_AUTOMATIC )||
      (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                        SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION)) ||
      ((!gmm_manual_mode_user_selection_procedure)&&
       ((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                        SYS_NETWORK_SELECTION_MODE_MANUAL) ||
        (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                        SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION))))&& 
     (is_cell_forbidden)))
  {
    is_nonforbid_plmn = FALSE;
  }
  
  /*
  ** Check for access class blocking restrictions and stop a LU procedure if necessary
  */
  lu_required = mm_check_if_cs_cell_access_allowed(mm_get_cs_cell_access(PROCEDURE_CS_REGISTRATION));
  attach_required = gmm_check_if_ps_cell_access_allowed(gmm_get_ps_cell_access(PROCEDURE_PS_REGISTRATION));
  
  if(((is_nonforbid_plmn) &&(lu_required || attach_required)) ||
      (((mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                        SYS_NETWORK_SELECTION_MODE_LIMITED) ||
         (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                        SYS_NETWORK_SELECTION_MODE_LIMITED_ROUTINE_ACQUISITION) ||
         (mmcoord_pending_reg_message.cmd.mmr_reg_req.network_selection_mode == 
                        SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)) &&
       (mmcoord_pending_reg_message.cmd.mmr_reg_req.cs_for_request == TRUE)))
  {
    result = TRUE;
  }
  return result;
}
/*************************************************************
FUNCTION mm_get_optimized_ps_signaling_connection_release

DESCRIPTION
  This function returns the value of mm_optimised_ps_signalling_connection_release

DEPENDENCIES
  None

RETURN VALUE
True or False.

SIDE EFFECTS
  None
*************************************************************/
boolean mm_get_optimized_ps_signaling_connection_release()
{
  return mm_optimised_ps_signalling_connection_release;
}
#ifdef FEATURE_DUAL_SIM
boolean mm_check_lu_allowed(void)
{
  sys_modem_as_id_e_type mm_other_as_id;
  for(mm_other_as_id = SYS_MODEM_AS_ID_1; mm_other_as_id < MAX_AS_IDS; mm_other_as_id++)
  {
    if((mm_other_as_id != mm_sub_id)  && (mm_serving_plmn_sim[mm_other_as_id].ms_op_mode == SYS_SRV_DOMAIN_CS_PS)  &&
        (reg_mode_is_usim_compliant_for_cm (mm_other_as_id))&&
        (mm_serving_plmn_sim[mm_other_as_id].info.active_rat == SYS_RAT_GSM_RADIO_ACCESS) && 
		  (t3310_timeout_counter_sim[mm_other_as_id] == 0) && (gprs_attach_attempt_counter_sim[mm_other_as_id] == 0) && 
		  (((gmm_state_sim[mm_other_as_id] == GMM_REGISTERED_INITIATED) &&
        (gmm_power_on_attach_completed_sim[mm_other_as_id] == FALSE)) || 
			((gmm_state_sim[mm_other_as_id] == GMM_REGISTERED) && (gmm_last_signalling_message_type_sim[mm_other_as_id] == GMM_ATTACH_COMPLETE)))
      )
    {
      return FALSE;
    }
  }
  return TRUE;
}
#endif
sys_activation_allowed_e_type mm_per_subs_is_DO_activation_allowed
(
  sys_modem_as_id_e_type sub_id
)
{
  sys_radio_access_tech_e_type active_rat;
  sys_rat_pri_list_info_s_type rat_list;
#ifdef FEATURE_1XSRLTE
  emm_ctrl_data_type *emm_ctrl_data_ptr;
#endif
  sys_activation_allowed_e_type activation_allowed;
#ifdef FEATURE_1XSRLTE
  sys_modem_as_id_e_type other_sub_id = SYS_MODEM_AS_ID_1;
#endif

#ifdef FEATURE_DUAL_SIM
  if(IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS(MM_SUB, "=MM= Invalid SUB ID:%D", sub_id+1,0,0);
    return SYS_ACTIVATION_ALLOWED_CONTINUE;
  }
#ifdef FEATURE_1XSRLTE
  if(sub_id == SYS_MODEM_AS_ID_1)
  {
    other_sub_id = SYS_MODEM_AS_ID_2;
  }
#endif
NAS_ENTER_CRIT_SECT(mm_rat_prioritylist_crit_sec);
  active_rat = mm_serving_plmn_sim[sub_id].info.active_rat;
  rat_list = mm_rat_pri_list_info_sim[sub_id];
NAS_EXIT_CRIT_SECT(mm_rat_prioritylist_crit_sec);

#ifdef FEATURE_1XSRLTE
  emm_ctrl_data_ptr = emm_db_get_ctrl_data_per_subs((mm_as_id_e_type)other_sub_id);
#endif
#else
NAS_ENTER_CRIT_SECT(mm_rat_prioritylist_crit_sec);
  active_rat = mm_serving_plmn.info.active_rat;
  rat_list = mm_rat_pri_list_info;
NAS_EXIT_CRIT_SECT(mm_rat_prioritylist_crit_sec);
#ifdef FEATURE_1XSRLTE
  emm_ctrl_data_ptr = emm_db_get_ctrl_data();
#endif
#endif

  if((active_rat == SYS_RAT_NONE)
#ifdef FEATURE_1XSRLTE
      ||
     (emm_ue_is_in_srlte_state(emm_ctrl_data_ptr))
#endif
     )
  {
    activation_allowed = SYS_ACTIVATION_ALLOWED_CONTINUE;
  }
  else if((active_rat == SYS_RAT_GSM_RADIO_ACCESS) &&
          (rat_list.num_items == 1) &&
          (rat_list.priority_list_info[0].acq_sys_mode == SYS_SYS_MODE_GSM))
  {
    activation_allowed = SYS_ACTIVATION_ALLOWED_WAIT_AND_CONTINUE;
  }
  else
  {
    activation_allowed =  SYS_ACTIVATION_ALLOWED_WAIT;
  }
  MSG_HIGH_DS_1(MM_SUB, "=MM= mm_per_subs_is_DO_activation_allowed returning %d", activation_allowed);
  return activation_allowed;
}


/*===========================================================================
FUNCTION  mm_check_ready_for_psm

DESCRIPTION: This function checks if MM is ready to enter PSM mode

DEPENDENCIES
  None

RETURN VALUE
  PSM ready status

SIDE EFFECTS
  None
===========================================================================*/
sys_psm_status_e_type mm_check_ready_for_psm(void)
{
  sys_psm_status_e_type status = SYS_PSM_STATUS_READY;
  byte           is_gmm_registering = FALSE;
  byte           is_ps_call_active  = FALSE;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM_GPRS) 
  if ( is_umts_mode() || is_gsm_mode())
  {
    if(gmm_state == GMM_REGISTERED_INITIATED ||
        gmm_state == GMM_DEREGISTERED_INITIATED ||
        gmm_state == GMM_ROUTING_AREA_UPDATING_INITIATED)
    {
      is_gmm_registering = TRUE; /* GMM is either registering or deregistering, don't allow PSM mode */
    }
   
    if ((
#ifdef FEATURE_DUAL_SIM     
         sm_is_pdp_active((sys_modem_as_id_e_type)mm_as_id) 
#else
         sm_is_pdp_active() 
#endif
         && 
         (gmm_substate == GMM_NORMAL_SERVICE) &&
         (dsumtsps_is_ps_data_available() ||
          ((is_gsm_mode() && (gmm_is_ps_active == TRUE)) ||
           (is_umts_mode() && (rrc_connection_status != MMRRC_IDLE))))) ||
         sm_check_pdp_procedure_pending())
    {
       /* Don't allow to enter PSM mode , if RAB is active or PS signalling connection is pending*/
       is_ps_call_active = TRUE;
    }

    if (((is_gmm_registering == TRUE) ||
          (mm_state != MM_IDLE) ||
          (rrc_connection_status != MMRRC_IDLE) ||
          (mm_sms_is_active == TRUE) ||
          (is_ps_call_active == TRUE)
#ifdef FEATURE_LTE
            || ((mm_csfb_state == MM_CSFB_STATE_MO_CALL_ORIG) ||
                (mm_csfb_state == MM_CSFB_STATE_MT_CALL_ORIG))
#endif

         ) /* Don't allow to enter PSM mode , if UE is in W-RLF and suitable search end indication is not received */
#ifdef FEATURE_WRLF_SYSTEM_SEL
         && ((mm_wcdma_rlf_state == WCDMA_RLF_IDLE) ||
             (mm_received_suitable_search_end_ind == FALSE))
#endif
      )
    {
      status = SYS_PSM_STATUS_NOT_READY_NOT_IDLE; 
    }
  }
#ifdef FEATURE_LTE 
  else if (is_lte_mode() &&
           (mm_wcdma_rlf_state != WCDMA_RLF_IDLE) &&
           (mm_received_suitable_search_end_ind == FALSE))
  {
    /* Don't allow to enter PSM mode , if active RAT is LTE and suitable search end indication is not received in WRLF */
    status = SYS_PSM_STATUS_NOT_READY_NOT_IDLE;
  }
#endif 
#endif

  return status;
}




#ifdef FEATURE_TDSCDMA
/*===========================================================================
Function: mm_suspend_policyman

Description:
This function sends suspend indication to policman

============================================================================*/

void mm_suspend_policyman()
{
  if(mm_policy_man_suspended == FALSE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= Policyman Suspended");
    policyman_suspend_by_client(POLICYMAN_CLIENT_NAS);
    mm_policy_man_suspended = TRUE;
  }
}

/*===========================================================================
Function: mm_resume_policyman

Description:
This function sends resume indication to policman

============================================================================*/
void mm_resume_policyman()
{
  if(mm_policy_man_suspended == TRUE)
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= Policyman Resumed");
    policyman_resume_by_client(POLICYMAN_CLIENT_NAS);
    mm_policy_man_suspended = FALSE;
  }
}
#endif
