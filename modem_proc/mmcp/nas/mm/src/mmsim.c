/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2011 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmsim.c_v   1.18   11 Jul 2002 11:12:14   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mmsim.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/01/12   am      Back off timer code
07/01/10   zren    Updated EMM to get eplmn list from REG upon loading sim data
04/13/10   MNK     Minor name changes 
03/03/10   MNK     TIN support 
05/14/01   jca     Added stubbed functions for get/save GMM SIM data.
06/20/01   sbk     Added support for security context management
06/19/01   sbk     Coded mmsim_get_ps_location_information and
                   mmsim_save_ps_location_information functions
08/03/01   jca     Removed call to send RRC_PAGING_CTRL_REQ on load_sim_data.
09/18/01   sbk     Intertask primitives are logged and QXDM logging is supported
10/19/01   sbk     Print IMSI. MSG on failed read of PSLOCI is changed MSG_HIGH_DS
11/02/01   sbk     Send log packet on reading IMSI
12/06/01   jca     Now sending UE_DYNAMIC_ID log packet when TMSI reset.
12/19/01   jca     Now waiting until LU procedure completed before clearing                   mm_possible_imsi_attach flag.
05/02/02   ks      Kicking the Watch dog report before reading/writing SIM.
05/02/02   vdr     Now setting correctly security context identifier when MM
                   deletes LU status
07/02/02   mks     Now sending SIM updates to RR
07/11/02   ks      Added checking for GSDI_STATUS in mmsim_write_loci() after
                   the SIM write.
08/11/02   mks     Now sending SIM updates to RRC,obsoleted RRC_PAGING_CTRL_REQ
08/13/02   mks     Added function mm_sim_load_access_class_info () to cache the
                   UE access class information, removed the redundant functions
                   mm_sim_get_lo_access_class () & mm_sim_get_hi_access_class().
08/30/02   mks     Added utility functions to access BCCH/CPBCCH information in
                   SIM, Fixed the SIM read failure issue with CPBCCH file item.
09/05/02   mks     Now invalidating the PSLOCI if the Routing Area update status
                   in SIM is not GMM_GU1_UPDATED.
10/01/02   ks      Added support for the new GSDI interfaces.
11/14/02   ks      Updated mmsim_save_ps_location_information and
                   mmsim_get_ps_location_information with no GPRS in SIM
11/15/02   mks     Merged BCHH allocation fixes ( BCCH is list is not supported
                   as it is not used by RR) from the branch.
11/25/02   mks     Added support for sending SIM updates to both active and
                   inactive RATs. Removed the call to gsdi_get_card_mode() in
                   mm_load_sim_data ().
12/12/02   ks      Modified function mmsim_sim_valid, it takes parameter
                   sys_srv_domain_e_type to check the validity of the SIM.
12/16/02   jca     Replaced function mmsim_sim_valid() with the code we reviewed.
12/18/02   vdr     Added NV access to the GPRS LOCI when GSM SIM does not support
                   GPRS
02/07/03   jca     Now sending RRC_INVALIDATE_SIM_DATA_REQ when SIM declared
                   invalid for both CS & PS service.
02/10/03   cd      Fixed Lint errors
05/09/03   vdr     Fixed GPRS LOCI handling for SIM cards which do not support GPRS
06/16/03   jca     Now consistently ERR_FATAL if attempt to allocate memory fails.
11/25/03   jca     Added some debug MSGs.
12/03/03   vdr     Added support for gmm_ptmsi_sig_tobe_deleted indicator
01/22/04   vdr     Removed FEATURE_GSM_GPRS_NV_IF feature definition
07/07/04   cd      Deleted obsolete function mm_get_ef_kc and fixed lint errors
12/17/04   vdr     Now calling mm_send_rrc_sim_update_req() with a parameter
                   identifying the exact domain this request is belonging to
12/23/04   vdr     Corrected invalidation of auth_ps_ciphering_algorithm upon failure
                   to read an NV
02/01/05   jca     Invalidate PSLOCI information on powerup only if the RAI value
                   indicates it has been "deleted" as per 24.008 section 4.1.3.2.
02/15/05   ks      Added function is_mcc_invalid() which checks validity of MCC digits,
                   Now deleting PSLOCI if the MCC is not valid.
02/14/05   sm      Setting the Location update status in sim correctly
                   for U3 ROAMING_NOT_ALLOWED
05/26/05   abe     Setting the flag send_location_status to TRUE for starting
                   the location information notification to GSTK.
06/30/05   up      Update the current IMSI in NV if the IMSI is changed.
08/08/05   abe     Population of global variable mm_hplmn_from_imsi is removed
                   for the feature FEATURE_ENHANCED_NW_SELECTION.
09/06/05   up      Set the location_status_evt flag to TRUE when PSLOCI is set.
11/28/05   abe      Guarded the GSTK call with feature flag FEATURE_GSTK
01/06/06   hj      Now deleting only CS Security context for a particular RAT
                   when location update fails.
02/10/09   RI      Updated the deletion of EPS context for IMSI mismatch
08/25/11   jbk     API to check if IMSI/PLMN ID is valid
09/08/11   jbk     Restricting IMSI validation to just HPLMN portion
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
12/15/11   gps     invalidate sim if wrong data present in IMSI
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
#include "timers_v.h"
#include "gs_v.h"
#include "dog_hb_rex.h"
#include "mm_v.h"
#include "rrcmmif.h"
#include "rrccmd.h"
#include "mm_rr.h"
#include "mm_umts_v.h"
#include "mm_mm_v.h"
#include "err.h"
#include "gmm_v.h"
#include "mm_sim.h"
#include "naslog_v.h"
#include "mmauth.h"
#include "gmm_int_v.h"
#include "ghdi_exp.h"
#include "ghdi_exp_int.h"
#include "fs_public.h"
#include "mcfg_fs.h"
#include "mm_coord.h" 
#include <stringl/stringl.h>
#ifdef FEATURE_GSTK
#include "mm_gstk.h"
#endif


#ifdef FEATURE_DUAL_SIM
#define mm_ghdi_nvmem_read(a,b) ghdi_nvmem_read_per_subs(a,b,mm_nv_context_id)
#define mm_ghdi_nvmem_write(a,b) ghdi_nvmem_write_per_subs(a,b,mm_nv_context_id)
#else
#define mm_ghdi_nvmem_read(a,b) ghdi_nvmem_read(a,b)
#define mm_ghdi_nvmem_write(a,b) ghdi_nvmem_write(a,b)
#endif

#ifdef FEATURE_LTE
#include "emm.h"
#include "emm_database.h"
#include "emm_security.h"
#include "emm_sim_handler.h"
#include "emm_utility.h"
#include "sys_eplmn_list_v.h"
#endif
#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE))
 #error code not present
#endif // TEST_FRAMEWORK

#if(defined(TEST_FRAMEWORK) && defined(FEATURE_LTE))
 #error code not present
#endif // TEST_FRAMEWORK

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
#ifdef FEATURE_TRIPLE_SIM
nas_mmgsdi_info_type mm_sim_mmgsdi_info_sim[MAX_NAS_STACKS] = {{0x00,0x00,MMGSDI_GW_PROV_PRI_SESSION,MMGSDI_MAX_SLOT_ID_ENUM},
                                                           {0x00,0x00,MMGSDI_GW_PROV_SEC_SESSION,MMGSDI_MAX_SLOT_ID_ENUM},
                                                           {0x00,0x00,MMGSDI_GW_PROV_TER_SESSION,MMGSDI_MAX_SLOT_ID_ENUM}};
#define mm_sim_mmgsdi_info mm_sim_mmgsdi_info_sim[mm_as_id]

#elif defined(FEATURE_SGLTE_DUAL_SIM)
nas_mmgsdi_info_type mm_sim_mmgsdi_info_sim[MAX_NAS_STACKS] = {{0x00,0x00,MMGSDI_GW_PROV_PRI_SESSION,MMGSDI_MAX_SLOT_ID_ENUM},
                                                           {0x00,0x00,MMGSDI_GW_PROV_SEC_SESSION,MMGSDI_MAX_SLOT_ID_ENUM},
                                                           {0x00,0x00,MMGSDI_GW_PROV_PRI_SESSION,MMGSDI_MAX_SLOT_ID_ENUM}};
#define mm_sim_mmgsdi_info mm_sim_mmgsdi_info_sim[mm_as_id]

#else
nas_mmgsdi_info_type mm_sim_mmgsdi_info_sim[MAX_NAS_STACKS] = {{0x00,0x00,MMGSDI_GW_PROV_PRI_SESSION,MMGSDI_MAX_SLOT_ID_ENUM},
                                                           {0x00,0x00,MMGSDI_GW_PROV_SEC_SESSION,MMGSDI_MAX_SLOT_ID_ENUM}};
#define mm_sim_mmgsdi_info mm_sim_mmgsdi_info_sim[mm_as_id]
#endif
mmgsdi_perso_status_enum_type mm_sim_perso_state_sim[MAX_NAS_STACKS];

#define mm_sim_perso_state mm_sim_perso_state_sim[mm_as_id]

mmgsdi_app_enum_type mm_sim_card_mode_sim[MAX_NAS_STACKS];

extern boolean mm_waiting_for_open_session_cnf_sim[MAX_NAS_STACKS];
#define mm_waiting_for_open_session_cnf mm_waiting_for_open_session_cnf_sim[mm_as_id]
#else
nas_mmgsdi_info_type mm_sim_mmgsdi_info = {0x00,0x00,MMGSDI_GW_PROV_PRI_SESSION,MMGSDI_MAX_SLOT_ID_ENUM};

mmgsdi_perso_status_enum_type mm_sim_perso_state;

mmgsdi_app_enum_type mm_sim_card_mode;

extern boolean mm_waiting_for_open_session_cnf;
#endif


#ifdef FEATURE_FEMTO_CSG
#define        MM_SIM_ADMIN_LENGTH  4

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
sys_csg_list_s_type         mm_sim_operator_csg_list_sim[MAX_NAS_STACKS]  = {{0},{0}};
#define                            mm_sim_operator_csg_list    mm_sim_operator_csg_list_sim[mm_as_id]

sys_csg_list_s_type         mm_sim_allowed_csg_list_sim[MAX_NAS_STACKS]  = {{0},{0}};
#define                            mm_sim_allowed_csg_list    mm_sim_allowed_csg_list_sim[mm_as_id]

boolean                           mm_using_allowed_csg_nv_sim[MAX_NAS_STACKS]  = {FALSE, FALSE};
#define                           mm_using_allowed_csg_nv  mm_using_allowed_csg_nv_sim[mm_as_id]

mmgsdi_rec_num_type   mm_sim_operator_csg_num_records_sim[MAX_NAS_STACKS];
#define                           mm_sim_operator_csg_num_records mm_sim_operator_csg_num_records_sim[mm_as_id]

mmgsdi_rec_num_type   mm_sim_allowed_csg_num_records_sim[MAX_NAS_STACKS];
#define                           mm_sim_allowed_csg_num_records  mm_sim_allowed_csg_num_records_sim[mm_as_id]

mmgsdi_len_type            mm_sim_operator_csg_record_length_sim[MAX_NAS_STACKS] ;
#define                           mm_sim_operator_csg_record_length  mm_sim_operator_csg_record_length_sim[mm_as_id]

mmgsdi_len_type            mm_sim_allowed_csg_record_length_sim[MAX_NAS_STACKS];
#define                           mm_sim_allowed_csg_record_length  mm_sim_allowed_csg_record_length_sim[mm_as_id]

byte                                 mm_sim_operator_csg_list_read_started_sim[MAX_NAS_STACKS]  = {FALSE, FALSE};
#define                           mm_sim_operator_csg_list_read_started mm_sim_operator_csg_list_read_started_sim[mm_as_id]

byte                                 mm_sim_allowed_csg_list_read_started_sim[MAX_NAS_STACKS]  = {FALSE, FALSE};
#define mm_sim_allowed_csg_list_read_started mm_sim_allowed_csg_list_read_started_sim[mm_as_id]

mm_sim_display_indicator_list_s_type  mm_sim_csg_indicator_list_sim[MAX_NAS_STACKS] ;
#define  mm_sim_csg_indicator_list mm_sim_csg_indicator_list_sim[mm_as_id]

byte                                mm_ad_csg_indicator_sim[MAX_NAS_STACKS]  = {0,0};
#define                           mm_ad_csg_indicator mm_ad_csg_indicator_sim[mm_as_id]

sys_csg_list_s_type        mm_sim_active_operator_csg_list_sim[MAX_NAS_STACKS]  = {{0},{0}};
#define                            mm_sim_active_operator_csg_list mm_sim_active_operator_csg_list_sim[mm_as_id]

sys_csg_list_s_type        mm_sim_active_allowed_csg_list_sim[MAX_NAS_STACKS]  = {{0}, {0}};
#define                           mm_sim_active_allowed_csg_list mm_sim_active_allowed_csg_list_sim[mm_as_id]

mm_sim_display_indicator_list_s_type  mm_sim_active_csg_indicator_list_sim[MAX_NAS_STACKS] = {{0}, {0}};
#define   mm_sim_active_csg_indicator_list mm_sim_active_csg_indicator_list_sim[mm_as_id]

boolean                           mm_sim_csg_display_service_sim[MAX_NAS_STACKS] = {FALSE, FALSE};
#define                            mm_sim_csg_display_service mm_sim_csg_display_service_sim[mm_as_id]

boolean mm_sim_ocsgl_present_sim[MAX_NAS_STACKS] = {FALSE, FALSE};
#define mm_sim_ocsgl_present mm_sim_ocsgl_present_sim[mm_as_id]

uint16                     mm_sim_csg_list_read_sqn_sim[MAX_NAS_STACKS] = {0, 0};
#define                    mm_sim_csg_list_read_sqn mm_sim_csg_list_read_sqn_sim[mm_as_id]
#else
sys_csg_list_s_type             mm_sim_operator_csg_list = {0};
sys_csg_list_s_type             mm_sim_allowed_csg_list = {0};

boolean                 mm_using_allowed_csg_nv = FALSE;
mmgsdi_rec_num_type                       mm_sim_operator_csg_num_records;
mmgsdi_rec_num_type                       mm_sim_allowed_csg_num_records;

mmgsdi_len_type                       mm_sim_operator_csg_record_length;
mmgsdi_len_type                       mm_sim_allowed_csg_record_length;

byte                       mm_sim_operator_csg_list_read_started = FALSE;
byte                       mm_sim_allowed_csg_list_read_started = FALSE;

mm_sim_display_indicator_list_s_type  mm_sim_csg_indicator_list;
byte                       mm_ad_csg_indicator = 0;

sys_csg_list_s_type             mm_sim_active_operator_csg_list = {0};
sys_csg_list_s_type             mm_sim_active_allowed_csg_list = {0};
mm_sim_display_indicator_list_s_type  mm_sim_active_csg_indicator_list;

boolean                              mm_sim_csg_display_service = FALSE;
boolean                              mm_sim_ocsgl_present = FALSE;
uint16                               mm_sim_csg_list_read_sqn = 0;
#endif /*FEATURE_DUAL_SIM || FEATURE_SGLTE*/
#endif /*FEATURE_FEMTO_CSG*/

#if defined(FEATURE_DUAL_SIM) ||defined(FEATURE_SGLTE)
uint8 mm_sim_max_subscription = 1;
#endif 

#define PSLOCI_LEN 14
#define RA_UPDATE_OFFSET 13

extern boolean reg_nv_is_imsi_switch_enabled(void);

#ifdef FEATURE_FEMTO_CSG
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)

extern boolean mmsim_camped_csg_add_to_white_list_sim[MAX_NAS_STACKS];
#define mmsim_camped_csg_add_to_white_list mmsim_camped_csg_add_to_white_list_sim[mm_as_id]

#else

extern boolean mmsim_camped_csg_add_to_white_list;
#endif
#endif

/* reserved mccs which belong to gcf*/
mm_mcc_data_type reserved_mcc_tbl[] =
{
  {0x00, 0x00, 0x01}, {0x00, 0x00, 0x02}, {0x00, 0x00, 0x03},
  {0x00, 0x00, 0x04}, {0x00, 0x00, 0x05}, {0x00, 0x00, 0x06},
  {0x00, 0x00, 0x07}, {0x00, 0x00, 0x08}, {0x00, 0x00, 0x09},
  {0x00, 0x01, 0x00}, {0x00, 0x01, 0x01}, {0x00, 0x01, 0x02}
};

/* ----------------------------
** Define watchdog report handle
** ---------------------------- */
#undef  DOG_MM_RPT
#define DOG_MM_RPT        mm_dog_rpt_var
extern dog_report_type    mm_dog_rpt_var;      /* Initial Safety */

/*===========================================================================

FUNCTION IS_RAI_DELETED

DESCRIPTION
  This function checks if a input RAI value indicates it has been deleted.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/

boolean is_rai_deleted( routing_area_id_T rai )
{
  /* ------------------------------------------------------
  ** Reserved value used when no valid RAI exists in the MS
  ** (TS 23.003 4.1 and 4.2)
  ** ------------------------------------------------------ */
  if ((rai.location_area_code.lac[0] == 0xFF) &&
      (rai.location_area_code.lac[1] == 0xFE) &&
      (rai.routing_area_code         == 0xFF))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}


/*===========================================================================

FUNCTION IS_MCC_INVALID

DESCRIPTION
  This function checks if a input RAI value indicates it has been deleted.

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None

===========================================================================*/

boolean is_mcc_invalid( sys_plmn_id_s_type plmn_id)
{
  byte mcc[3];

  /* ----------------------------------------
   ** PLMN octet 1 = MCC digit 2 | MCC digit 1
   ** ---------------------------------------- */
   mcc[0] = plmn_id.identity[0] & 0x0F;
   mcc[1] = (plmn_id.identity[0] & 0xF0) >> 4;

   /* ----------------------------------------
   ** PLMN octet 2 = MNC digit 3 | MCC digit 3
   ** ---------------------------------------- */
   mcc[2] = plmn_id.identity[1] & 0x0F;
  
  /* ------------------------------------------------------
  ** MCC is invalid if it is not in the range (0...9)
  ** ------------------------------------------------------ */
  if ((mcc[0]      > 0x9) ||
     (mcc[1]      > 0x9) ||
     (mcc[2]      > 0x9)) 
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}



/*===========================================================================

FUNCTION MMSIM_MARK_SIM_AS_ILLEGAL

DESCRIPTION
  This function marks the SIM as illegal and informs both GSDI and RR layers

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mmsim_mark_sim_as_illegal(void)
{
#ifdef FEATURE_WCDMA 
  rrc_cmd_type *rrc_cmd;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mm_serving_plmn.info.sim_state = SYS_SIM_STATE_CS_PS_INVALID;

  (void) mmgsdi_session_manage_illegal_subscription(mm_sim_mmgsdi_info.session_id,
                                                        MMGSDI_SESSION_APP_IS_ILLEGAL);

  /* -------------------------------------------------------------
  ** SIM illegal event needs to be sent to both GSM & WCDMA RATs
  ** (if they are supported) independent of which RAT is currently
  ** active.
  ** ------------------------------------------------------------- */

  if (mm_wcdma_supported)
  {
    if(mm_is_multimode_sub())
    {
#ifdef FEATURE_WCDMA    
      rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_UMTS_RADIO_ACCESS );

      if (rrc_cmd == NULL)
      {
        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
      }

      rrc_cmd->cmd_hdr.cmd_id = RRC_INVALIDATE_SIM_DATA_REQ;

      send_cmd_to_rrc( rrc_cmd, SYS_RAT_UMTS_RADIO_ACCESS );
#endif /* FEATURE_WCDMA */
    }
  }

  if (mm_tdscdma_supported)
  {
    if(mm_is_multimode_sub())
    {
#ifdef FEATURE_WCDMA
      rrc_cmd = mm_get_rrc_cmd_buf( SYS_RAT_TDS_RADIO_ACCESS );

      if (rrc_cmd == NULL)
      {
        MSG_FATAL_DS( MM_SUB, "=MM= ERROR: Unable to allocate memory", 0,0,0 );
      }

      rrc_cmd->cmd_hdr.cmd_id = RRC_INVALIDATE_SIM_DATA_REQ;

      send_cmd_to_rrc( rrc_cmd, SYS_RAT_TDS_RADIO_ACCESS );
#endif /* FEATURE_WCDMA */
    }
  }

  if (mm_gsm_supported)
  {
    mm_send_rr_sim_removed_req();
  }
}


/*===========================================================================

FUNCTION MM_DELETE_LU_STATUS

DESCRIPTION
  This function deletes TMSI, LAI, and ciphering key sequence number
  in the SIM, and set the update status to either NOT UPDATED or
  ROAMING NOT ALLOWED, depending on the input parameter value

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mm_delete_lu_status( byte update_status )
{
   if( (update_status == NOT_UPDATED) || (update_status == ROAMING_NOT_ALLOWED) 
#ifdef FEATURE_NAS_ECALL
       || (update_status == UPDATING_DISABLED)
#endif
     )
   {
      /*
       * clear LAI and TMSI in location_information
       */

      /*---------------------------------------------------------------
       TS 24.008 Table 10.5.3.  If the LAI is deleted, keep the
       deleted LAI's MCC and MNC.  LAC is set to 0xfffe ie all 1's with
       the least significant bit set to 0
       ----------------------------------------------------------------*/

      mm_location_information.lai[3] = 0xff;
      mm_location_information.lai[4] = 0xfe;    /* deleted LAC = 0xfffe */
#ifdef FEATURE_SGLTE
      if(MM_SUB_IS_SGLTE)
      {
        mm_location_information_sim[MM_SGLTE_STACK_2].lai[3] = 0xff;
        mm_location_information_sim[MM_SGLTE_STACK_2].lai[4] = 0xfe;
      }
#endif

      if (has_valid_tmsi())
      {
        /*--------------------------------------------------
        TS 23.003 2.4
        A deleted TMSI is indicated by setting all bits 1
        ---------------------------------------------------*/
        (void)memset( mm_location_information.tmsi, 0xFF, TMSI_SIZE );
#ifdef FEATURE_SGLTE
        if(MM_SUB_IS_SGLTE)
        {
          (void)memset( mm_location_information_sim[MM_SGLTE_STACK_2].tmsi, 0xFF, TMSI_SIZE );
        }
#endif

#ifdef FEATURE_DUAL_SIM
        if(MM_MODEM_STATISTICS_SUB1)
#endif
        {
          mm_set_signal_for_cm(SYS_NAS_REGISTRATION_INFO_MASK);
        }
      }

      /* --------------------------------------------
      ** Send the log packet which updates TMSI value
      ** -------------------------------------------- */
      mmsend_nas_ue_dynamic_id_log_packet( LOG_TMSI_TYPE );

      mm_location_information.location_update_status = update_status;

      /*
       * copy location information to SIM
       */
      mmsim_write_loci();

      /*-------------------------------------------------
       Delete the security keys by setting CKSN to 0x07
       -------------------------------------------------*/

      MSG_HIGH_DS_1(MM_SUB ,"=MM= MM deleting CS only Key, type=%d", update_status);
      auth_delete_cksn(AUTH_GSM_SECURITY_CONTEXT);     
      auth_delete_cksn(AUTH_CS_GPRS_SECURITY_CONTEXT);

      MSG_MED_DS(MM_SUB, "=MM= KEY_NOW_INVALIDATED", 0,0,0);
   }
   else
   {
      MSG_ERROR_DS(MM_SUB, "=MM= The update status is invalid argument", 0,0,0);
   }
}


/*===========================================================================

FUNCTION MMSIM_WRITE_LOCI

DESCRIPTION
  This function updates the memory LOCI information to the SIM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mmsim_write_loci()
{
   byte  sim_loci[MMGSDI_LOCI_LEN];
   mmgsdi_file_enum_type   sim_file_name_loci = MMGSDI_NO_FILE_ENUM;

   sys_plmn_id_s_type plmn;

   /*
    * copy location information to SIM
    * An intermediate buffer is required as some compilers put
    * padding bytes into the location_information struct, which the SIM
    * card does not want.
    */

   (void)memscpy(sim_loci, MMGSDI_LOCI_LEN,mm_location_information.tmsi, TMSI_SIZE);
   (void)memscpy(sim_loci + MMGSDI_LAI_OFFSET, MMGSDI_LOCI_LEN-MMGSDI_LAI_OFFSET, mm_location_information.lai, LAI_SIZE);
   sim_loci[MMGSDI_LUS_OFFSET - 1] = mm_location_information.tmsi_time;
#ifdef FEATURE_NAS_ECALL
   sim_loci[MMGSDI_LUS_OFFSET] = (mm_location_information.location_update_status == UPDATING_DISABLED)?NOT_UPDATED:
                                 mm_location_information.location_update_status;
#else
   sim_loci[MMGSDI_LUS_OFFSET] = mm_location_information.location_update_status;
#endif
   if (mm_location_information.location_update_status == ROAMING_NOT_ALLOWED)
   {
      sim_loci[MMGSDI_LUS_OFFSET] = LOCATION_AREA_NOT_ALLOWED;
   }

   /* send the log packet as LU status is getting updated */
   mmsend_mm_state_log_packet();

  if (mm_sim_card_mode == MMGSDI_APP_SIM)
   {
      sim_file_name_loci = MMGSDI_GSM_LOCI;
   }
   else if (mm_sim_card_mode == MMGSDI_APP_USIM)
   {
      sim_file_name_loci = MMGSDI_USIM_LOCI;
   }
   else
   {
      sim_file_name_loci = MMGSDI_NO_FILE_ENUM;
      MSG_ERROR_DS(MM_SUB, "=MM= Invalid card mode %d", mm_sim_card_mode, 0,0);
      return;
   }

   if( mm_sim_write( sim_file_name_loci,
                   sim_loci,
                   MMGSDI_LOCI_LEN ) != TRUE)
   {
      MSG_ERROR_DS(MM_SUB, "=MM= Unable to write LOCI to SIM",0,0,0);
   }
   memscpy(plmn.identity,sizeof(sys_plmn_id_s_type),mm_location_information.lai,PLMN_SIZE);
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
    reg_sim_update_cs_rplmn(plmn,(sys_modem_as_id_e_type)mm_as_id);
#else
   reg_sim_update_cs_rplmn(plmn);
#endif

   /* ------------------------------------------------------------
   ** Both GSM  &  WCDMA  RATs  (if they are supported) need to be
   ** updated with the latest TMSI for receiving pages independent
   ** of which RAT is currently active.
   ** ------------------------------------------------------------ */

   if( mm_wcdma_supported )
   {
      mm_send_rrc_sim_update_req( RRC_CS_DOMAIN_CN_ID, SYS_RAT_UMTS_RADIO_ACCESS );
   }

   if (mm_tdscdma_supported)
   {
      mm_send_rrc_sim_update_req( RRC_CS_DOMAIN_CN_ID, SYS_RAT_TDS_RADIO_ACCESS );
   }

   if ( mm_gsm_supported )
   {
      mm_send_rr_sim_update_req();
   }

   /* ---------------------------------------------------------------
   ** We have either attached to the network or changed update status
   ** to NOT_UPDATED, in either case we can now clear this flag
   ** --------------------------------------------------------------- */
   

#ifdef FEATURE_GSTK
   /* Location Status Event to SIM may be required since LOCI is updated */
   if (!reg_nv_is_imsi_switch_enabled())
   {
     mm_gstk_set_location_status_evt (TRUE);
   }
#endif

}


/*===========================================================================
FUNCTION       MM_LOAD_CSLOCI_DATA

DESCRIPTION   read the csloci data from sim/usim

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mm_load_csloci_data
(
  void
)
{
   boolean                 status;
   mmgsdi_file_enum_type   sim_file_name_loci = MMGSDI_NO_FILE_ENUM;
   byte                    sim_loci[MMGSDI_LOCI_LEN];
   location_information_T  location_information;

   memset((void *)&sim_loci[0], 0, sizeof(sim_loci));
   /* reset the buffer */
   memset((void *)&location_information, 0, sizeof(location_information_T));

   /* check the sim card tupe */
   if( mm_sim_card_mode == MMGSDI_APP_SIM)
   {
     sim_file_name_loci = MMGSDI_GSM_LOCI;
   }
   else if( mm_sim_card_mode == MMGSDI_APP_USIM)
   {
     sim_file_name_loci = MMGSDI_USIM_LOCI;
   }
   else
   {
     MSG_ERROR_DS(MM_SUB, "=MM= Invalid SIM card mode %d", 0,0,0);
     mm_sim_card_mode = MMGSDI_APP_UNKNOWN;
     return;
   }

   status = mm_sim_read( sim_file_name_loci,
                         sim_loci,
                         MMGSDI_LOCI_LEN);

   if(status == TRUE)
   {
      sys_plmn_id_s_type  plmn;
      (void)memscpy(location_information.tmsi, TMSI_SIZE, sim_loci, TMSI_SIZE);

      MSG_HIGH_DS_3(MM_SUB,"=MM= TMSI: %X %X %X", location_information.tmsi[0],
                                 location_information.tmsi[1],
                                 location_information.tmsi[2] );

      MSG_HIGH_DS_1(MM_SUB,"=MM= TMSI: %X", location_information.tmsi[3]);

      (void)memscpy(location_information.lai, LAI_SIZE,
             sim_loci + MMGSDI_LAI_OFFSET, LAI_SIZE);

      MSG_HIGH_DS_3(MM_SUB, "=MM= PLMN: %X %X %X", location_information.lai[0],
                                  location_information.lai[1],
                                  location_information.lai[2] );

      MSG_HIGH_DS_2(MM_SUB, "=MM= LAI: %X %X", location_information.lai[3],
                               location_information.lai[4]);

      location_information.tmsi_time = sim_loci[MMGSDI_LUS_OFFSET - 1];
      location_information.location_update_status = sim_loci[MMGSDI_LUS_OFFSET];

      if (location_information.location_update_status == LOCATION_AREA_NOT_ALLOWED)
      {
         location_information.location_update_status = ROAMING_NOT_ALLOWED;
      }

      memscpy(plmn.identity,sizeof(sys_plmn_id_s_type), location_information.lai,PLMN_SIZE);

      if((location_information.location_update_status == UPDATED) &&
         !mm_is_plmn_valid(plmn))
      {
        mm_delete_lu_status( NOT_UPDATED );

        memscpy((void *)&location_information, sizeof( location_information_T), (void *)&mm_location_information, 
                                              sizeof( location_information_T));
      }

      MSG_HIGH_DS_1(MM_SUB ,"=MM= UPDATE_STATUS: %X", 
                            location_information.location_update_status);
   }
   else
   {
     MSG_ERROR_DS(MM_SUB,"SIM/USIM read failed for LOCI file, GSDI status %d",status,0,0);
   }
}

/*===========================================================================

FUNCTION MMSIM_GET_PS_LOCATION_INFORMATION

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  P-TMSI
  P-TMSI Signature
  Routing Area Identification
  Routing Area Update Status

SIDE EFFECTS
  None

===========================================================================*/

void mmsim_get_ps_location_information
(
  gmm_ptmsi_type           *ptmsi,            /* P-TMSI                      */
  gmm_ptmsi_signature_type *ptmsi_signature,  /* P-TMSI signature value      */
  routing_area_id_T        *rai,              /* Routing area identification */
  gmm_update_status_type   *ra_update_status  /* Routing area update status  */
)
{
   ghdi_status_T nv_status = GHDI_SUCCESS ;
   boolean        mmgsdi_status = TRUE;
   byte psloci[PSLOCI_LEN];

   (void) memset( psloci, 0xFF, PSLOCI_LEN );
  
   /*
    * Get the sim file name based on the sim mode.
    * The buffer read is of same format for both GPRS and UMTS
    */
   if( mm_sim_card_mode == MMGSDI_APP_SIM)
   {
      if( auth_gsm_gprs_keys_present )
      {
         mmgsdi_status = mm_sim_read(MMGSDI_GSM_LOCIGPRS, psloci, PSLOCI_LEN);
      }
      else
      {
        if (mm_imsi_is_native)
        {
          if ((nv_status = mm_ghdi_nvmem_read(
               NV_GSM_LOCIGPRS_I,(ghdi_nvmem_data_T *)psloci)) != GHDI_SUCCESS)
          {
            MSG_ERROR_DS(MM_SUB, "=MM= Unable to access NV to read GSM GPRS LOCI: %d",nv_status,0,0) ;
            return ;
          }
        }
        else
        {
          nv_status = GHDI_FAILURE ;

#ifdef FEATURE_SGLTE
          if(MM_SUB_IS_SGLTE)
          {
            HYBRID_STACK_DATA_COPY(auth_gsm_gprs_cksn,0x07)
            HYBRID_STACK_DATA_COPY(auth_ps_ciphering_algorithm,0xFF)
          }
          else
#endif
          {
            auth_gsm_gprs_cksn = 0x07 ;
            auth_ps_ciphering_algorithm = 0xFF ;
          }
        }
      }
   }
   else if( mm_sim_card_mode == MMGSDI_APP_USIM)
   {
      mmgsdi_status = mm_sim_read(MMGSDI_USIM_PSLOCI, psloci, PSLOCI_LEN);
   }
   else
   {
      MSG_ERROR_DS(MM_SUB, "=MM= SIM card mode is not of known mode (GSM or USIM)", 0,0,0);
      return;
   }

   if ((nv_status != GHDI_SUCCESS) || (mmgsdi_status != TRUE))
   {
      gmm_delete_ps_location_information();
      *ra_update_status = GMM_GU3_ROUTING_AREA_NOT_ALLOWED;
      MSG_HIGH_DS_1(MM_SUB ,"=MM= Err: Failed to read PSLOCI , NV status %d ",  nv_status);
   }
   else /* On Successful read */
   {
      /* Copy Routing area update status */
      *ra_update_status = psloci[RA_UPDATE_OFFSET];

      MSG_HIGH_DS_1( MM_SUB, "=MM= RA update status: %d", *ra_update_status);

      /* Copy PTMSI */
      (void)memscpy(ptmsi->digit, PTMSI_SIZE, psloci, PTMSI_SIZE);

      MSG_HIGH_DS_3(MM_SUB, "=MM= PTMSI: %X %X %X",ptmsi->digit[0],
                                 ptmsi->digit[1],
                                 ptmsi->digit[2] );

      MSG_HIGH_DS_1(MM_SUB, "=MM= PTMSI: %X",ptmsi->digit[3]);

      /* Copy PTMSI Signature */
      (void)memscpy(ptmsi_signature->value, PTMSI_SIGNATURE_SIZE, psloci+PTMSI_SIZE, PTMSI_SIGNATURE_SIZE);

      /* Copy RAI */
      (void)memscpy((byte *)rai, sizeof(routing_area_id_T), psloci+PTMSI_SIZE+PTMSI_SIGNATURE_SIZE, RAI_SIZE);

      /* Send the log packet which updates gmm_update_status */
      mmsend_gmm_state_log_packet();

      /* Send the log packet which updates TMSI value */
      mmsend_nas_ue_dynamic_id_log_packet(LOG_PTMSI_TYPE);

      if (is_rai_deleted( *rai ))
      {
        /* ------------------------------------------------------------
        ** If the RAI value indicates it has been "deleted", invalidate
        ** the remaining PSLOCI data (see 24.008 section 4.1.3.2)
        ** ------------------------------------------------------------ */        
        MSG_HIGH_DS_0(MM_SUB, "=MM= Invalid RAI, deleting PSLOCI");
        gmm_delete_ps_location_information();
      }
      else if (is_mcc_invalid( (*rai).plmn_id))
      {
        /* ---------------------------------------------------------------
        ** RAI is valid, but the PSLOCI will also be deleted if the MCC is invalid
        ** or beyond the range (0...9), otherwise ASN decoder would fail.
        ** --------------------------------------------------------------*/
        MSG_HIGH_DS_0(MM_SUB, "=MM= Invalid MCC, deleting PSLOCI");
        gmm_delete_ps_location_information();        
      }
   }

}


/*===========================================================================

FUNCTION MMSIM_SAVE_PS_LOCATION_INFORMATION

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmsim_save_ps_location_information
(
  gmm_ptmsi_type           ptmsi,            /* P-TMSI                      */
  gmm_ptmsi_signature_type ptmsi_signature,  /* P-TMSI signature value      */
  routing_area_id_T        rai,              /* Routing area identification */
  gmm_update_status_type   ra_update_status  /* Routing area update status  */
)
{
   boolean        mmgsdi_status = FALSE;
   ghdi_status_T nv_status ;


   byte psloci[PSLOCI_LEN] = {0};

   /* Copy PTMSI into buffer */
   (void)memscpy(psloci, PSLOCI_LEN, ptmsi.digit, PTMSI_SIZE);

   /* Copy PTMSI Signature into buffer */
   (void)memscpy(psloci+PTMSI_SIZE, PSLOCI_LEN-PTMSI_SIZE, ptmsi_signature.value, PTMSI_SIGNATURE_SIZE);

   /* Copy RAI into buffer */
   (void)memscpy(psloci+PTMSI_SIZE+PTMSI_SIGNATURE_SIZE, PSLOCI_LEN-(PTMSI_SIZE+PTMSI_SIGNATURE_SIZE), (byte *)&rai, RAI_SIZE);

   psloci[RA_UPDATE_OFFSET] = ra_update_status;

   /* Send the log packet which updates gmm_update_status */
   mmsend_gmm_state_log_packet();

   /* Send the log packet which updates PTMSI value */
   mmsend_nas_ue_dynamic_id_log_packet(LOG_PTMSI_TYPE);

   /*
    * Write to the appropriate EF based on the sim mode.
    * The buffer to write is of same format for both GPRS and UMTS
    */

   if( mm_sim_card_mode == MMGSDI_APP_SIM)
   {
      if( auth_gsm_gprs_keys_present )
      {
         mmgsdi_status = mm_sim_write(MMGSDI_GSM_LOCIGPRS, psloci, PSLOCI_LEN);
      }
      else
      {
        if ((nv_status = mm_ghdi_nvmem_write(
               NV_GSM_LOCIGPRS_I,(ghdi_nvmem_data_T *)psloci)) != GHDI_SUCCESS)
        {
          MSG_ERROR_DS(MM_SUB, "=MM= Unable to access NV to save GSM GPRS LOCI: %d",nv_status,0,0) ;
        }
      }
   }
   else if( mm_sim_card_mode == MMGSDI_APP_USIM)
   {
      mmgsdi_status = mm_sim_write(MMGSDI_USIM_PSLOCI, psloci, PSLOCI_LEN);
   }
   else
   {
      MSG_ERROR_DS(MM_SUB, "=MM= SIM card mode is not of known mode (GSM or USIM)", 0,0,0);
      return;
   }
   if ( ( !mmgsdi_status ) &&
        ( auth_gsm_gprs_keys_present ) )
   {
      MSG_ERROR_DS(MM_SUB, "=MM= Writing PSLOCI to SIM failed, GSDI status %d",  mmgsdi_status,0,0);
   }
   else
   {
      /* -------------------------------------------------------------
      ** SIM update event needs to be sent to both GSM & WCDMA RATs
      ** (if they are supported) independent of which RAT is currently
      ** active.
      ** ------------------------------------------------------------- */
      if ( mm_wcdma_supported )
      {
         mm_send_rrc_sim_update_req( RRC_PS_DOMAIN_CN_ID, SYS_RAT_UMTS_RADIO_ACCESS );
      }

      if (mm_tdscdma_supported)
      {
         mm_send_rrc_sim_update_req( RRC_PS_DOMAIN_CN_ID, SYS_RAT_TDS_RADIO_ACCESS );
      }

      /* Notify PSLOCI updates to RR */
      if ( mm_gsm_supported )
      {
         mm_send_rr_sim_update_req();
      }

#ifdef FEATURE_GSTK
      /* Location Status Event to SIM may be required since LOCI is updated */
      if (!reg_nv_is_imsi_switch_enabled())
      {
        mm_gstk_set_location_status_evt (TRUE);
      }
#endif

   }

   if (gmm_ptmsi_sig_tobe_deleted)
   {
     byte i ;
     boolean ptmsi_signature_present = FALSE;

     for (i=0; i < PTMSI_SIGNATURE_SIZE; i++)
     {
       if (gmm_stored_ptmsi_signature.value[i] != 0xFF)
       {
         ptmsi_signature_present = TRUE;
         break;
       }
     } /* end for (i=0; i < PTMSI_SIGNATURE_SIZE; i++) */

     if (!ptmsi_signature_present)
     {
       gmm_ptmsi_sig_tobe_deleted = FALSE ;

     }
   }
}


/*===========================================================================

FUNCTION MMSIM_SIM_VALID

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  TRUE  -
  FALSE -

SIDE EFFECTS
  None

===========================================================================*/

boolean mmsim_sim_valid( sys_srv_domain_e_type domain )
{
  boolean status = FALSE;

  /* ---------------------------------------------------
  ** Return status based on the domain and the SIM state
  ** --------------------------------------------------- */
  switch (domain)
  {
    case SYS_SRV_DOMAIN_CS_ONLY:
      if ((mm_serving_plmn.info.sim_state == SYS_SIM_STATE_AVAILABLE ) ||
          (mm_serving_plmn.info.sim_state == SYS_SIM_STATE_PS_INVALID))
      {
        status = TRUE;
      }
      break;

    case SYS_SRV_DOMAIN_PS_ONLY:
      if ((mm_serving_plmn.info.sim_state == SYS_SIM_STATE_AVAILABLE ) ||
          (mm_serving_plmn.info.sim_state == SYS_SIM_STATE_CS_INVALID))
      {
        status = TRUE;
      }
      break;

    case SYS_SRV_DOMAIN_CS_PS:
      if ((mm_serving_plmn.info.sim_state == SYS_SIM_STATE_AVAILABLE ) ||
          (mm_serving_plmn.info.sim_state == SYS_SIM_STATE_CS_INVALID) ||
          (mm_serving_plmn.info.sim_state == SYS_SIM_STATE_PS_INVALID))
      {
        status = TRUE;
      }
      break;

    default:
      MSG_FATAL_DS( MM_SUB, "=MM= Unknown SIM validation type", 0,0,0 );
  }

  return status;
}


/*===========================================================================

FUNCTION MM_LOAD_SIM_DATA

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean mm_load_sim_data( void )
{
   boolean                 status;
   mmgsdi_file_enum_type   sim_file_name_imsi = MMGSDI_NO_FILE_ENUM,
                           sim_file_name_loci = MMGSDI_NO_FILE_ENUM,
                           sim_file_name_access_class = MMGSDI_NO_FILE_ENUM, 
                           sim_file_name_sst = MMGSDI_NO_FILE_ENUM,
                           sim_file_name_admin = MMGSDI_NO_FILE_ENUM;
   byte                    sim_loci[MMGSDI_LOCI_LEN];
   mmgsdi_len_type         file_length = 0;
   boolean                 mmgsdi_status = FALSE;
   ghdi_status_T           nv_status;
   byte                    access_class[2] = {0 };
   byte                    sst_buffer[13]  = {0 };    // add a new define if needed
   byte                    ms_opmode = 0;
   byte                    mnc_len = 2;
   imsi_data_T             imsi_data;
   location_information_T  location_information;
   byte                    imsi_from_nv[8]={0};
   mm_mcc_data_type imsi_mcc = {0,0,0};
#ifdef FEATURE_GAN
   #error code not present
#endif

#ifdef FEATURE_LTE
  lte_nas_emm_usim_serv_table_type  ust_log;

  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_LTE)
  emm_ctrl_data_ptr->imsi_not_native = FALSE;
#endif
   /* store all the information locally, only if all file reads succeed
      copy the info to globals
   */

   memset((void *)&sim_loci[0], 0, sizeof(sim_loci));
   memset( (void *)&imsi_data, 0, sizeof(imsi_data_T));

   memset( (void *)&location_information, 0, sizeof(location_information_T));


   /* Get the sim file name based on the sim mode */
   if( mm_sim_card_mode == MMGSDI_APP_SIM)
   {
      sim_file_name_imsi = MMGSDI_GSM_IMSI;
      sim_file_name_loci = MMGSDI_GSM_LOCI;
      sim_file_name_access_class = MMGSDI_GSM_ACC;
      sim_file_name_sst = MMGSDI_GSM_SST;
      sim_file_name_admin = MMGSDI_GSM_AD;
      
   }
   else if( mm_sim_card_mode == MMGSDI_APP_USIM)
   {
      sim_file_name_imsi = MMGSDI_USIM_IMSI;
      sim_file_name_loci = MMGSDI_USIM_LOCI;
      sim_file_name_access_class = MMGSDI_USIM_ACC;
      sim_file_name_sst = MMGSDI_USIM_UST;
      sim_file_name_admin = MMGSDI_USIM_AD;
   }
   else
   {
      MSG_ERROR_DS(MM_SUB, "=MM= Invalid SIM card mode %d", 0,0,0);
#ifdef FEATURE_SGLTE
      if(MM_SUB_IS_SGLTE)
      {
        HYBRID_STACK_DATA_COPY(mm_sim_card_mode,MMGSDI_APP_UNKNOWN)
      }
      else
#endif
      {
        mm_sim_card_mode = MMGSDI_APP_UNKNOWN;
      }

      return FALSE;
   }

   /* Step 1: Read the IMSI from card. Read the IMSI from NV.
              If the two do not match, update the NV with the one 
              from card.
              Delete any security contexts saved in NV.
   */

   status = mm_sim_read( sim_file_name_imsi,
                         (byte *) &imsi_data,
                         MMGSDI_IMSI_LEN
                         );

   if ( !status )
   {
      /*------------------------------------------------------
       If failed to read IMSI, it should be considered that
       the SIM is invalid
       ------------------------------------------------------*/
      MSG_ERROR_DS(MM_SUB,"SIM/USIM read failed for IMSI file, GSDI status %d",  status,0,0);
      return FALSE;
   }
   else
   {
     byte index=0;
     boolean imsi_illegal = FALSE;
     rrc_imsi_gsm_map_type  imsi_data_digits;
 
     MSG_HIGH_DS_8(MM_SUB, "=MM= SIM IMSI[0] = %d  SIM IMSI[1] = %d  SIM IMSI[2] = %d  SIM IMSI[3] = %d  \
                   SIM IMSI[4] = %d  SIM IMSI[5] = %d  SIM IMSI[6] = %d  SIM IMSI[7] = %d",
                   imsi_data.data[0],imsi_data.data[1],imsi_data.data[2],imsi_data.data[3],imsi_data.data[4],
                   imsi_data.data[5],imsi_data.data[6],imsi_data.data[7]);
     imsi_data_digits = convert_imsi_to_digits(imsi_data);

     if (imsi_data_digits.num_digits == 0 ||
         imsi_data.length == 0 ||
         imsi_data.length > IMSI_LEN_IN_BYTES)
     {
       imsi_illegal = TRUE;
     }
     else
     {
       /* 
      Validate only MCC & MNC of HPLMN here
      */

       for(;index < imsi_data_digits.num_digits;index++)
       {
         if(imsi_data_digits.value[index] >= 0x0A )
         {
           imsi_illegal = TRUE;
           break;
         }
         
       }
     }

     if (imsi_illegal)
     {
        mmsim_mark_sim_as_illegal();
        /*------------------------------------------------------
        The IMSI length shall not exceed 15 digits (8 bytes)
        otherwise SIM should be considered as invalid
        ------------------------------------------------------*/
        MSG_ERROR_DS(MM_SUB,"IMSI is illegal (len=%d) IMSI_DIGIT[%d]: %d", 
           imsi_data.length,index+1,imsi_data_digits.value[index]);
        return FALSE;
     }
     else
     {
              /* -----------------------------------------------------------
               Parse MCC from the IMSI:
              B0    B1        B2       B3      B4     B5   B6   B7	  B8
			 LEN  MCC1PB   MCC3MCC2 MNC2MNC1 D1MNC3  D3D2 D5D4 D7D6  D9D8
                -------------------------------------------------------------*/

       imsi_mcc.mcc1 = imsi_data.data[0] & 0xF0;
       imsi_mcc.mcc1 >>= 4;
       imsi_mcc.mcc2 = imsi_data.data[1] & 0x0F ;
       imsi_mcc.mcc3 = imsi_data.data[1] & 0xF0;;
       imsi_mcc.mcc3 >>= 4;

      	  
       MSG_HIGH_DS_3(MM_SUB, "=MM= MCC value in the IMSI: %x%x%x", imsi_mcc.mcc1, imsi_mcc.mcc2, imsi_mcc.mcc3);
       gmm_anite_gcf_flag = FALSE;
       for(index = 0;
           index < (sizeof(reserved_mcc_tbl)/
                    sizeof(mm_mcc_data_type));
           index++)
	   {
         if(!memcmp(&imsi_mcc,
                    &reserved_mcc_tbl[index],
                    sizeof(mm_mcc_data_type)))
		 {
           gmm_anite_gcf_flag = TRUE;
           MSG_HIGH_DS_0(MM_SUB, "=MM= MCC match with reserved MCCs found, GCF FLAG = true");
           break;
		 }
	   }
	 }
     mm_send_nv_params_to_lower_layers();
   }

  if ((nv_status = mm_ghdi_nvmem_read(NV_GSMUMTS_IMSI_I,
                      (ghdi_nvmem_data_T *)imsi_from_nv)) != GHDI_SUCCESS)
  {
    MSG_ERROR_DS(MM_SUB,"Unable to access NV to read stored IMSI: %d",nv_status,0,0) ;
  }

  mm_imsi_is_native = (memcmp((void *)imsi_from_nv,
                   (void *)imsi_data.data,imsi_data.length) == 0) ;

  /*
  ** Update the current IMSI in NV if the IMSI is changed.
  */
  if (!mm_imsi_is_native)
  {
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_LTE)
    /*Set the imsi not native flag for non multimode stack so that the relevant EMM
    ctrl data can be reset on subscription change */
    if(mm_is_multimode_sub() == FALSE)
    {
      emm_ctrl_data_ptr->imsi_not_native = TRUE;
    }
#endif
#ifdef FEATURE_NAS_REL10
    gmm_t3346_backoff_timer_value = 0;
    mm_t3246_backoff_timer_value  = 0;
#endif

#ifdef FEATURE_LTE
    /*Notify T3402 clients of the T3402 value*/
    if(mm_is_multimode_sub() == TRUE)
    {
      MSG_HIGH_DS_0(MM_SUB,"=EMM= Notify T3402 default value to clients");
      emm_ctrl_data_ptr->t3402_value = EMM_DEFAULT_T3402;
      emm_ctrl_data_ptr->t3402_attach_rej_param.is_t3402_recv_attach_rej = FALSE;
      emm_send_t3402_changed_ind(emm_ctrl_data_ptr->t3402_value);
    }
    if(emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_8 == TRUE)
    {
      MSG_HIGH_DS_0(MM_SUB,"=EMM= Resetting is_accepted_on_lte flag due to sim change and writing it back to efs ");
      emm_ctrl_data_ptr->is_accepted_on_lte = FALSE;
      emm_write_lte_accepted_status_to_efs(emm_ctrl_data_ptr);  
    }
    if(emm_ctrl_data_ptr->emm_plmn_block_on_rej_15_roaming== TRUE)
    {
      MSG_HIGH_DS_0(MM_SUB,"=EMM= Resetting is_accepted_lte_plmn flag due to sim change and writing it back to efs ");
      sys_plmn_undefine_plmn_id(&emm_ctrl_data_ptr->emm_lte_accepted_vplmn);
      emm_write_lte_accepted_vplmn_status_to_efs(emm_ctrl_data_ptr);  
    }
    emm_set_and_write_tin_type(NAS_MM_TIN_NONE);
    /* MNK: */
    //Also delete TIN from EFS. delete from the spec means, set this to NONE ?
#endif
    if (mm_ghdi_nvmem_write(NV_GSMUMTS_IMSI_I, (ghdi_nvmem_data_T *)imsi_data.data) != GHDI_SUCCESS)
    {
      MSG_ERROR_DS(MM_SUB,"Unable to access NV to save IMSI",0,0,0) ;
    }
    mm_send_sm_unblock_all_apn();
  }

  /* delete the security context since IMSI is different now */
  if (!mm_imsi_is_native)  
  {
#ifdef FEATURE_SGLTE
    if(MM_SUB_IS_SGLTE)
    {
      HYBRID_STACK_DATA_COPY(auth_gsm_gprs_cksn,0x07)
      HYBRID_STACK_DATA_COPY(auth_ps_ciphering_algorithm,0xFF)
    }
    else    
#endif
    {
      auth_gsm_gprs_cksn = 0x07 ;    
      auth_ps_ciphering_algorithm = 0xFF ;
    }

#ifdef FEATURE_LTE
    if(mm_is_multimode_sub())
    {
      /* delete EPSNSC and EPSLOCI from EFS and invalidate mobility info */
      MSG_HIGH_DS_0(MM_SUB,"=EMM= Deleting EPSNSC and EPSLOCI from EFS ");
      emm_delete_efs_security_context();
      (void)mcfg_fs_delete( "/nvm/alpha/modem/nas/lte_nas_eps_loci", MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_sub_id);
      emm_invalidate_mobility_info(EMM_STATUS_NOT_UPDATED, 
                                   SYS_SIM_STATE_AVAILABLE, 
                                   emm_ctrl_data_ptr, FALSE);
    }
#endif 

#ifdef FEATURE_FEMTO_CSG
    mcfg_fs_delete("/nv/item_files/modem/nas/allowed_csg_list", MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_sub_id);
#endif /*FEATURE_FEMTO_CSG*/
  }

  MSG_HIGH_DS_3(MM_SUB,"IMSI: %x %x %x", imsi_data.length,imsi_data.data[0],
                             imsi_data.data[1]);
  MSG_HIGH_DS_3(MM_SUB,"IMSI: %x %x %x", imsi_data.data[2],imsi_data.data[3],
                             imsi_data.data[4]);
  MSG_HIGH_DS_3(MM_SUB,"IMSI: %x %x %x", imsi_data.data[5],imsi_data.data[6],
                             imsi_data.data[7]);


   /* Step 2: Initialize the LOCI.
              PS LOCI and EPS LOCI are not read until the 
              service table is read from the card
   */


   status = mm_sim_read( sim_file_name_loci,
                         sim_loci,
                         MMGSDI_LOCI_LEN
                        );

   if (status == TRUE )
   {
      sys_plmn_id_s_type  plmn;
      (void)memscpy(location_information.tmsi, TMSI_SIZE, sim_loci, TMSI_SIZE);

      MSG_HIGH_DS_3(MM_SUB,"=MM= TMSI: %X %X %X", location_information.tmsi[0],
                                 location_information.tmsi[1],
                                 location_information.tmsi[2] );

      MSG_HIGH_DS_1(MM_SUB,"=MM= TMSI: %X", location_information.tmsi[3]);

      (void)memscpy(location_information.lai, LAI_SIZE,
             sim_loci + MMGSDI_LAI_OFFSET,
             LAI_SIZE);

      MSG_HIGH_DS_3(MM_SUB, "=MM= PLMN: %X %X %X", location_information.lai[0],
                                  location_information.lai[1],
                                  location_information.lai[2] );

      MSG_HIGH_DS_2(MM_SUB, "=MM= LAI: %X %X", location_information.lai[3],
                               location_information.lai[4]);

      location_information.tmsi_time = sim_loci[MMGSDI_LUS_OFFSET - 1];
      location_information.location_update_status = sim_loci[MMGSDI_LUS_OFFSET];

      if (location_information.location_update_status == LOCATION_AREA_NOT_ALLOWED)
      {
         location_information.location_update_status = ROAMING_NOT_ALLOWED;
      }

      memscpy(plmn.identity,sizeof(sys_plmn_id_s_type),location_information.lai,PLMN_SIZE);

      if((location_information.location_update_status == UPDATED) &&
         !mm_is_plmn_valid(plmn))
      {
        mm_delete_lu_status( NOT_UPDATED );

        memscpy( (void *)&location_information, sizeof( location_information_T), (void *)&mm_location_information, sizeof( location_information_T));
      }

      MSG_HIGH_DS_1(MM_SUB ,"=MM= UPDATE_STATUS: %X", location_information.location_update_status);

   }
   else
   {
      MSG_ERROR_DS(MM_SUB,"SIM/USIM read failed for LOCI file, GSDI status %d",  status,0,0);
   }


  /* Step 3: Read the UE access class information */
  /* Cache the UE access class information */
  mmgsdi_status = mm_sim_read(sim_file_name_access_class, access_class, 2);
  
  if (!mmgsdi_status)
  {
    MSG_ERROR_DS(MM_SUB,"SIM/USIM read failed for Access Class, GSDI status %d",  status,0,0);
    return FALSE;
  }

  /* Step 4: Read the Service table 
  */

  mmgsdi_status = (mmgsdi_session_read_cache_file_size (mm_sim_mmgsdi_info.session_id,
                                          sim_file_name_sst,
                                          &file_length) == MMGSDI_SUCCESS);
  if( !mmgsdi_status )
  {
     MSG_ERROR_DS(MM_SUB,"SIM/USIM read failed for Service table",0,0,0) ;
     return FALSE;
  }
  mmgsdi_status = mm_sim_read ( sim_file_name_sst, 
                                sst_buffer, 
                                MIN(file_length,(int)(sizeof(sst_buffer))));
  if( !mmgsdi_status )
  {
     MSG_ERROR_DS(MM_SUB,"SIM/USIM read failed for Service table",0,0,0) ;
     return FALSE;
  }

  /* Get current SIM/USIM file name according to SIM card mode */
  if (mm_sim_card_mode == MMGSDI_APP_SIM)
  {
#ifdef FEATURE_SGLTE
    if(MM_SUB_IS_SGLTE)
    {
      HYBRID_STACK_DATA_COPY(auth_gsm_access,TRUE)
    }
    else
#endif
    {
      auth_gsm_access = TRUE ;
    }

    /* Check on the availability of the GPRS related SIM security resources */
    auth_gsm_gprs_keys_present =
       ((sst_buffer[9] & GPRS_ALLOCATED_N_ACTIVATED_MASK) == GPRS_ALLOCATED_N_ACTIVATED_MASK) ;
#ifdef FEATURE_SGLTE
    if(MM_SUB_IS_SGLTE)
    {
      HYBRID_STACK_DATA_COPY(auth_gsm_gprs_keys_present,auth_gsm_gprs_keys_present)
    }
#endif     

  } else if (mm_sim_card_mode == MMGSDI_APP_USIM)
  {
    /* Read GSM Access indicator (service 27) */
    auth_gsm_access = (sst_buffer[3] & 0x04) >> 2 ;

    auth_gsm_gprs_keys_present = auth_gsm_access ;

#ifdef FEATURE_SGLTE
    if(MM_SUB_IS_SGLTE)
    {
      HYBRID_STACK_DATA_COPY(auth_gsm_gprs_keys_present,auth_gsm_gprs_keys_present)
      HYBRID_STACK_DATA_COPY(auth_gsm_access,auth_gsm_access)
    }
#endif  

    /* Read GSM Security Context indicator (service 38) */
    auth_usim_gsm_security_context = (sst_buffer[4] & 0x20) >> 5 ;
    
#ifdef FEATURE_SGLTE
    if(MM_SUB_IS_SGLTE)
    {
      HYBRID_STACK_DATA_COPY(auth_usim_gsm_security_context,auth_usim_gsm_security_context)
    }
#endif

#ifdef FEATURE_LTE
    /* read the EFS to determine whether LTE is supported or not */
    // lte_supported = TRUE;

    // status = efs_get( "nvm/alpha/modem/nas/lte_supported", &lte_supported, sizeof(boolean) );
    // if( status != 0 )
    // {
    //    lte_supported = FALSE;
     // }
    /* read the EPS file bits status - EPS LOCI, EPS NSC */
    /* log UST info */
    /*if( lte_supported )
    {*/

    memset((void *)&ust_log, 0, sizeof(ust_log));

    // ust_log.log_version = log_version;
    memscpy( ust_log.ust_value, LTE_NAS_UST_LEN, sst_buffer, LTE_NAS_UST_LEN);

    send_emm_usim_serv_table_log_info(&ust_log);
  
    /* Read Service #85 for EPS MM info storage*/
    emm_ctrl_data_ptr->eps_storage_supported =( ( (sst_buffer[10] & 0x10) >> 4  == 1 )? TRUE : FALSE );
    /*}*/
#endif
  }

#ifdef FEATURE_FEMTO_CSG
  if(mm_nv_csg_support > SYS_CSG_SUPPORT_DISABLED)
  {
    mm_sim_csg_display_service = ( ( (sst_buffer[11] & 0x08) >> 3  == 1 )? TRUE : FALSE );
    MSG_HIGH_DS_1(MM_SUB ,"=MM= CSG: SIM Display Service %d", mm_sim_csg_display_service);
  }
#endif 
  /* Step 5: Read ADMIN Data from SIM */

#ifdef FEATURE_GAN
  #error code not present
#else
  mmgsdi_status = mm_sim_read(sim_file_name_admin, &ms_opmode, 1);
  if( !mmgsdi_status )
  {
    MSG_ERROR_DS(MM_SUB, "Failure reading ADMIN Data from SIM/USIM ",0,0,0);
    return FALSE;
  }
#endif

  /* start saving the globals, if we get here */

  mm_lo_access_class = RRC_LO_ACCESS_CLASS_ABSENT;
  mm_hi_access_class = 0;

  if( IS_BIT_SET( access_class[1], 0) )
     mm_lo_access_class = RRC_ACCESS_CLASS_0;
  else if( IS_BIT_SET( access_class[1], 1))
     mm_lo_access_class = RRC_ACCESS_CLASS_1;
  else if( IS_BIT_SET( access_class[1], 2))
     mm_lo_access_class = RRC_ACCESS_CLASS_2;
  else if( IS_BIT_SET( access_class[1], 3))
     mm_lo_access_class = RRC_ACCESS_CLASS_3;
  else if( IS_BIT_SET( access_class[1], 4))
     mm_lo_access_class = RRC_ACCESS_CLASS_4;
  else if( IS_BIT_SET( access_class[1], 5))
     mm_lo_access_class = RRC_ACCESS_CLASS_5;
  else if( IS_BIT_SET( access_class[1], 6))
     mm_lo_access_class = RRC_ACCESS_CLASS_6;
  else if( IS_BIT_SET( access_class[1], 7))
     mm_lo_access_class = RRC_ACCESS_CLASS_7;

  /* Look into the 2nd byte*/
  else if( IS_BIT_SET( access_class[0], 0))
     mm_lo_access_class = RRC_ACCESS_CLASS_8;

  else if( IS_BIT_SET( access_class[0], 1))
     mm_lo_access_class = RRC_ACCESS_CLASS_9;

  if( IS_BIT_SET( access_class[0], 3))
     mm_hi_access_class |= RRC_ACCESS_CLASS_11_BIT;
  if( IS_BIT_SET( access_class[0], 4))
     mm_hi_access_class |= RRC_ACCESS_CLASS_12_BIT;
  if( IS_BIT_SET( access_class[0], 5))
     mm_hi_access_class |= RRC_ACCESS_CLASS_13_BIT;
  if( IS_BIT_SET( access_class[0], 6))
     mm_hi_access_class |= RRC_ACCESS_CLASS_14_BIT;
  if( IS_BIT_SET( access_class[0], 7))
     mm_hi_access_class |= RRC_ACCESS_CLASS_15_BIT;

  /* mm_location_information */
#ifdef FEATURE_SGLTE
  if(MM_SUB_IS_SGLTE)
  {
    HYBRID_STACK_DATA_MEMS_COPY(&mm_location_information,sizeof( location_information_T),&location_information,sizeof( location_information_T))
    HYBRID_STACK_DATA_MEMS_COPY(&mm_stored_imsi, sizeof( imsi_data_T),&imsi_data, sizeof( imsi_data_T))
    memscpy( (void *)mm_sim_access_class_sim[MM_SGLTE_STACK_2], MM_SIM_EFACC_SIZE,(void *)access_class, sizeof(access_class));
    memscpy( (void *)mm_sim_access_class_sim[mm_as_id], MM_SIM_EFACC_SIZE,(void *)access_class, sizeof(access_class));
    HYBRID_STACK_DATA_COPY(mm_ms_opmode,ms_opmode)
    HYBRID_STACK_DATA_COPY(mm_mnc_length,mnc_len)
  }
  else
#endif
  {
    memscpy( (void *)&mm_location_information, sizeof( location_information_T), (void *)&location_information, sizeof( location_information_T));

    /* mm_stored_imsi */
    memscpy( ( void *)&mm_stored_imsi, sizeof( imsi_data_T), (void *)&imsi_data, sizeof( imsi_data_T));

    /* Access class */
    memscpy( (void *)mm_sim_access_class, MM_SIM_EFACC_SIZE,(void *)access_class, sizeof(access_class));

    /* UE operational mode */
    mm_ms_opmode = ms_opmode;

    /* MNC length */
    mm_mnc_length = mnc_len;
  }

  /* gmm ptmsi is stored here */
  gmm_load_psloci_data();

#ifdef FEATURE_LTE
  
  /* Load EPS LOCI */
  if(mm_is_multimode_sub() == TRUE)
  {
    (void)emm_load_epsloci_data(emm_ctrl_data_ptr);
  }
  emm_ctrl_data_ptr->imsi = lte_convert_imsi_to_digits(mm_stored_imsi) ;
  emm_ctrl_data_ptr->access_class = ((word)access_class[0]<<8) | (word)access_class[1] ;
#endif 

  /* Read and cache the IMEI again with correct mapping of Slot_id */
  mm_stored_imei[0] = 0;
  mm_read_imei(NULL);

  mmsend_nas_ue_static_id_log_packet();

  /* Send the log packet as LU status is getting updated */
  mmsend_mm_state_log_packet();

  /* Send the log packet which updates TMSI value */
  mmsend_nas_ue_dynamic_id_log_packet(LOG_TMSI_TYPE);

#ifdef FEATURE_FEMTO_CSG
  if((mm_nv_csg_support > SYS_CSG_SUPPORT_DISABLED) &&
     (mm_is_multimode_sub() == TRUE))
  {
    mm_sim_read_csg_list();
  }
#endif 
  return TRUE;

}


boolean mm_sim_read(mmgsdi_file_enum_type ef, byte *data, dword data_length)
{
  mmgsdi_return_enum_type status = MMGSDI_ERROR;
  mmgsdi_data_type read_data;

  /* Kick the watchdog */
  /* Report to the Dog HB */
  dog_hb_report(DOG_MM_RPT);

  read_data.data_ptr = data;
  read_data.data_len = data_length;

  status = mmgsdi_session_read_cache
  (
    mm_sim_mmgsdi_info.session_id,
    ef,
    read_data
  );

  if(status != MMGSDI_SUCCESS)
  {
    MSG_ERROR_DS(MM_SUB, "=MM= Unable to read from EF %d (MMGSDI err = %d)",ef,status,0) ;
  }

  return (boolean)(status == MMGSDI_SUCCESS);
}

boolean mm_sim_write(mmgsdi_file_enum_type ef, byte *data, dword data_length)
{
  mmgsdi_return_enum_type status = MMGSDI_ERROR;
  mmgsdi_data_type write_data;

   /* Kick the watchdog */
   /* Report to the Dog HB */
   dog_hb_report(DOG_MM_RPT);

  write_data.data_ptr = data;
  write_data.data_len = data_length;

  status = mmgsdi_session_write_cache
  (
    mm_sim_mmgsdi_info.session_id,
    ef,
    write_data
  );

  if(status != MMGSDI_SUCCESS)
  {
    MSG_ERROR_DS(MM_SUB, "=MM= Unable to write to EF %d (MMGSDI err = %d)",ef,status,0) ;
  }

  return (boolean)(status == MMGSDI_SUCCESS);
}

#ifdef FEATURE_ENHANCED_NW_SELECTION 
/*=============================================================================
FUNCTION mmsim_is_ef_rat_present

DESCRIPTION
  This function Will tell whether EF RAT is present or not in the USIM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/
boolean mmsim_is_ef_rat_present(void)
{
    byte               rat_mode_pref;
    boolean            result = FALSE; 

  if( mm_sim_card_mode == MMGSDI_APP_USIM)
  {
     result = mm_sim_read(MMGSDI_USIM_7F66_PROP1_RAT, &rat_mode_pref,1);
     MSG_MED_DS(MM_SUB, "=MM= MMGSDI status - %d", result, 0, 0);
  }
  return result;
}
#endif

#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
sys_modem_as_id_e_type mm_sim_get_as_id
(
  mmgsdi_session_id_type session_id
)
{
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  if(mm_sim_mmgsdi_info_sim[SYS_MODEM_AS_ID_1].session_id == session_id)
  {
    as_id = SYS_MODEM_AS_ID_1;
  }
  else if(mm_sim_mmgsdi_info_sim[SYS_MODEM_AS_ID_2].session_id == session_id)
  {
    as_id = SYS_MODEM_AS_ID_2;
  }
#if defined(FEATURE_TRIPLE_SIM)
  else if(mm_sim_mmgsdi_info_sim[SYS_MODEM_AS_ID_3].session_id == session_id)
  {
    as_id = SYS_MODEM_AS_ID_3;
  }
#endif
  else
  {
    MSG_FATAL_DS(MM_SUB, "=MM= Error: session ID not mapped to any AS ID", 0,0,0);
  }
  return as_id;
}

/*============================================================================
 Function mm_sim_session_matching_with_other_sub

DESCRIPTION
  This function swaps mmgsdi info at NAS in dual sim baselines depending on session type indicated by CM

DEPENDENCIES
  None

RETURN VALUE
  None
==============================================================================*/

boolean mm_sim_session_matching_with_other_sub( mmgsdi_session_type_enum_type session_type)
{
  sys_modem_as_id_e_type as_id;
  for (as_id = SYS_MODEM_AS_ID_1; as_id < MAX_AS_IDS; as_id++)
  {
    if((as_id != (sys_modem_as_id_e_type)mm_sub_id) && (mm_sim_mmgsdi_info_sim[as_id].session_type== session_type ))
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*=============================================================================
FUNCTION nas_swap_mmgsdi_info

DESCRIPTION
  This function swaps mmgsdi info at NAS in dual sim baselines depending on session type indicated by CM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The data at index 0 & 1 pointed by info will get swapped
==============================================================================*/
void nas_swap_mmgsdi_info(mm_as_id_e_type cur_as_id, nas_mmgsdi_info_type *info, boolean *flag, mmgsdi_session_type_enum_type session_type)
{
  nas_mmgsdi_info_type buff;
  boolean flag_buff;
  sys_modem_as_id_e_type as_id;
  for (as_id = SYS_MODEM_AS_ID_1; as_id < MAX_NAS_STACKS; as_id++)
  {
    if((as_id != (sys_modem_as_id_e_type)cur_as_id) && (info[as_id].session_type == session_type ))
    {
      break;      
    }
  }
  if((info != NULL) && (flag != NULL))
  {
    memscpy(&buff, sizeof(nas_mmgsdi_info_type), &info[cur_as_id], sizeof(nas_mmgsdi_info_type));
    memscpy(&info[cur_as_id],sizeof(nas_mmgsdi_info_type), &info[as_id], sizeof(nas_mmgsdi_info_type));
    memscpy(&info[as_id],sizeof(nas_mmgsdi_info_type), &buff,   sizeof(nas_mmgsdi_info_type));

    flag_buff = flag[cur_as_id];
    flag[cur_as_id]   = flag[as_id];
    flag[as_id]   = flag_buff;
  }
}
#endif

void mm_sim_mmgsdi_generic_cb
(
  mmgsdi_return_enum_type       status,
  mmgsdi_cnf_enum_type          cnf,
  const mmgsdi_cnf_type        *cnf_ptr
)
{
  mm_cmd_type *mm_cmd_p;
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
#endif

#ifdef FEATURE_DUAL_SIM
  as_id = SYS_MODEM_AS_ID_NONE;
#endif

  if (cnf_ptr == NULL)
  {
     MSG_ERROR_DS(MM_SUB, "=MM= MMGSDI returned cnf_ptr as NULL",0,0,0);
     return;
  }

  switch (cnf)
  {
    case MMGSDI_SESSION_OPEN_CNF:
#ifdef FEATURE_DUAL_SIM
      rex_enter_crit_sect(&mm_sim_crit_sect);
      if(mm_sim_mmgsdi_info_sim[SYS_MODEM_AS_ID_1].session_type 
           == (mmgsdi_session_type_enum_type) cnf_ptr->response_header.client_data)
      { 
        as_id = SYS_MODEM_AS_ID_1;
      }
      else if(mm_sim_mmgsdi_info_sim[SYS_MODEM_AS_ID_2].session_type 
               == (mmgsdi_session_type_enum_type) cnf_ptr->response_header.client_data)
      {
        as_id = SYS_MODEM_AS_ID_2;
      }
#ifdef FEATURE_TRIPLE_SIM
      else if(mm_sim_mmgsdi_info_sim[SYS_MODEM_AS_ID_3].session_type 
               == (mmgsdi_session_type_enum_type) cnf_ptr->response_header.client_data)
      {
        as_id = SYS_MODEM_AS_ID_3;
      }
#endif
      else
      {
        MSG_FATAL_DS(MM_LOCAL_SUB, "=MM= Invalid session type: %d", 
          cnf_ptr->response_header.client_data,0,0);
      }
      memscpy(&mm_sim_mmgsdi_info_sim[as_id].session_id, sizeof(mmgsdi_session_id_type), &(cnf_ptr->session_open_cnf.session_id),
                                        sizeof(mmgsdi_session_id_type));
      MSG_HIGH_DS_3(MM_LOCAL_SUB, "=MM= MMGSDI session open cnf Status %d Session ID: 0x%x %x", status,
      (dword)(mm_sim_mmgsdi_info_sim[as_id].session_id >> 32), (dword)(mm_sim_mmgsdi_info_sim[as_id].session_id & 0xFFFFFFFF));
      rex_leave_crit_sect(&mm_sim_crit_sect);
#else
#ifdef FEATURE_SGLTE
      memscpy(&mm_sim_mmgsdi_info_sim[SYS_MODEM_AS_ID_1].session_id, sizeof(mmgsdi_session_id_type), &(cnf_ptr->session_open_cnf.session_id),
                                        sizeof(mmgsdi_session_id_type));
      MSG_HIGH_DS_3(MM_SUB, "=MM= MMGSDI session open cnf Status %d Session ID: 0x%x %x", status,
      (dword)(mm_sim_mmgsdi_info_sim[SYS_MODEM_AS_ID_1].session_id >> 32), (dword)(mm_sim_mmgsdi_info_sim[SYS_MODEM_AS_ID_1].session_id & 0xFFFFFFFF));
#else
      memscpy(&mm_sim_mmgsdi_info.session_id, sizeof(mmgsdi_session_id_type), &(cnf_ptr->session_open_cnf.session_id),
                                        sizeof(mmgsdi_session_id_type));
      MSG_HIGH_DS_3(MM_SUB, "=MM= MMGSDI session open cnf Status %d Session ID: 0x%x %x", status,
      (dword)(mm_sim_mmgsdi_info.session_id >> 32), (dword)(mm_sim_mmgsdi_info.session_id & 0xFFFFFFFF));
#endif
#endif

      /*
      ** Get a command buffer for the MM task.
      */
      if ((mm_cmd_p = mm_get_cmd_buf()) != NULL)
      {
        /*
        ** Construct the SIM_MM_SUBS_READY_IND command.
        */
        mm_cmd_p->cmd.sim_subs_ready_ind.message_header.message_set = MS_GSDI;
        mm_cmd_p->cmd.sim_subs_ready_ind.message_header.message_id  = SIM_MM_SUBS_READY_IND;

        mm_cmd_p->cmd.sim_subs_ready_ind.message_header.message_len_lsb =
                   (sizeof(sim_subs_ready_ind_s_type) - sizeof(IMH_T)) % 256;
        mm_cmd_p->cmd.sim_subs_ready_ind.message_header.message_len_msb =
                   (sizeof(sim_subs_ready_ind_s_type) - sizeof(IMH_T)) / 256;

#ifdef FEATURE_DUAL_SIM
        mm_cmd_p->cmd.sim_subs_ready_ind.as_id = as_id;
#endif

        /*
        ** Put on MM command queue
        */
        mm_put_cmd(mm_cmd_p);
      }
      else
      {
        MSG_FATAL_DS(MM_SUB,"=MM= Error: Not able to Allocate MM Buffer", 0, 0, 0);
      }
      break;
    
#ifdef FEATURE_FEMTO_CSG

    case MMGSDI_GET_FILE_ATTR_CNF:
      if ((mm_cmd_p = mm_get_cmd_buf()) != NULL)
      {
        MSG_HIGH_DS_0(MM_SUB,"=MM= SIM Get file attribute confirm");
        /*
        ** Construct the SIM_MM_SUBS_READY_IND command.
        */
        mm_cmd_p->cmd.mmgsdi_get_file_attr_cnf.message_header.message_set = MS_GSDI;
        mm_cmd_p->cmd.mmgsdi_get_file_attr_cnf.message_header.message_id  = SIM_MM_GET_FILE_ATTR_CNF;

        mm_cmd_p->cmd.mmgsdi_get_file_attr_cnf.message_header.message_len_lsb =
                   (sizeof(mm_sim_get_file_attr_cnf_s_type) - sizeof(IMH_T)) % 256;
        mm_cmd_p->cmd.mmgsdi_get_file_attr_cnf.message_header.message_len_msb =
                   (sizeof(mm_sim_get_file_attr_cnf_s_type) - sizeof(IMH_T)) / 256;

         memscpy(&mm_cmd_p->cmd.mmgsdi_get_file_attr_cnf.cnf_data, sizeof(mmgsdi_get_file_attr_cnf_type), &cnf_ptr->get_file_attr_cnf, sizeof(mmgsdi_get_file_attr_cnf_type));
  #ifdef FEATURE_DUAL_SIM
         mm_cmd_p->cmd.mmgsdi_get_file_attr_cnf.as_id = (sys_modem_as_id_e_type)(cnf_ptr->response_header.client_data & 0xFF);
  #endif 
        /*
        ** Put on MM command queue
        */
        mm_put_cmd(mm_cmd_p);
      }
      else
      {
        MSG_FATAL_DS(MM_SUB,"=MM= Error: Not able to Allocate MM Buffer", 0, 0, 0);
      }

      break;
    case MMGSDI_READ_CNF:  

      if ((mm_cmd_p = mm_get_cmd_buf()) != NULL)
      {
        MSG_HIGH_DS_0(MM_SUB,"=MM= SIM read file confirm");
        /*
        ** Construct the SIM_MM_SUBS_READY_IND command.
        */
        mm_cmd_p->cmd.mmgsdi_read_file_cnf.message_header.message_set = MS_GSDI;
        mm_cmd_p->cmd.mmgsdi_read_file_cnf.message_header.message_id  = SIM_MM_READ_FILE_CNF;

        mm_cmd_p->cmd.mmgsdi_read_file_cnf.message_header.message_len_lsb =
                   (sizeof(mm_sim_read_file_cnf_s_type) - sizeof(IMH_T)) % 256;
        mm_cmd_p->cmd.mmgsdi_read_file_cnf.message_header.message_len_msb =
                   (sizeof(mm_sim_read_file_cnf_s_type) - sizeof(IMH_T)) / 256;

       memscpy(&mm_cmd_p->cmd.mmgsdi_read_file_cnf.cnf_data,  sizeof(mmgsdi_read_cnf_type), &cnf_ptr->read_cnf, sizeof(mmgsdi_read_cnf_type));

       mm_cmd_p->cmd.mmgsdi_read_file_cnf.cnf_data.read_data.data_ptr = NULL;
 
       if(status == MMGSDI_SUCCESS)
       {
         mm_cmd_p->cmd.mmgsdi_read_file_cnf.cnf_data.read_data.data_ptr = 
                       (uint8 *)modem_mem_alloc(sizeof( uint8 ) * cnf_ptr->read_cnf.read_data.data_len, MODEM_MEM_CLIENT_NAS);
  
         mm_check_for_null_ptr((void*)mm_cmd_p->cmd.mmgsdi_read_file_cnf.cnf_data.read_data.data_ptr );
  
         memscpy(mm_cmd_p->cmd.mmgsdi_read_file_cnf.cnf_data.read_data.data_ptr, sizeof( uint8 ) * cnf_ptr->read_cnf.read_data.data_len,
                            cnf_ptr->read_cnf.read_data.data_ptr,  cnf_ptr->read_cnf.read_data.data_len);
       }
  #ifdef FEATURE_DUAL_SIM
         mm_cmd_p->cmd.mmgsdi_read_file_cnf.as_id = (sys_modem_as_id_e_type)(cnf_ptr->response_header.client_data & 0xFF);
  #endif 

        /*
        ** Put on MM command queue
        */
       mm_put_cmd(mm_cmd_p);
      }
      else
      {
        MSG_FATAL_DS(MM_SUB,"=MM= Error: Not able to Allocate MM Buffer", 0, 0, 0);
      }
     break;

#endif 
    default:
      MSG_HIGH_DS_1(MM_SUB ,"=MM= MMGSDI event %d not handled at MM", cnf);
      break;
  }
  
}

/*===========================================================================
FUNCTION NAME 
         MM_SIM_REFRESH_EVENT_CB

DESCRIPTION
  Callback function from MMGSDI generated session events.

RETURN VALUE
   NONE

===========================================================================*/

void mm_sim_session_generic_event_cb
(
  const mmgsdi_event_data_type *event
)
{
#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
  sys_modem_as_id_e_type as_id = mm_sim_get_as_id(event->session_id);
#endif

  if (event->evt == MMGSDI_SESSION_CHANGED_EVT)
  {
    if (event->data.session_changed.activated == FALSE)
    {
      /* Stop SIM read/writes till SIM AVAILABLE REQ */
      MSG_HIGH_DS_0(MM_LOCAL_SUB, "=MM= MMGSDI Session Deactivated - SIM card marked unknown");
#ifdef FEATURE_SGLTE
      if(MM_SUB_IS_SGLTE_SUB(as_id))
      {
        mm_sim_card_mode_sim[as_id] = MMGSDI_APP_UNKNOWN;
        mm_sim_card_mode_sim[MM_SGLTE_STACK_2] = MMGSDI_APP_UNKNOWN;
      }
      else
#endif
      {
#ifdef FEATURE_DUAL_SIM
        mm_sim_card_mode_sim[as_id] = MMGSDI_APP_UNKNOWN;
		ghdi_clear_slot_mapping((sys_modem_as_id_e_type)as_id);
#else
        mm_sim_card_mode = MMGSDI_APP_UNKNOWN;
#endif
      }


#ifdef FEATURE_SGLTE
      if(MM_SUB_IS_SGLTE_SUB(as_id))
      {
        mm_sim_perso_state_sim[as_id] = MMGSDI_PERSO_STATUS_NONE;
        mm_sim_perso_state_sim[MM_SGLTE_STACK_2] = MMGSDI_PERSO_STATUS_NONE;
      }
      else
#endif
      {
#ifdef FEATURE_DUAL_SIM
        mm_sim_perso_state_sim[as_id] = MMGSDI_PERSO_STATUS_NONE;      
#else
        mm_sim_perso_state = MMGSDI_PERSO_STATUS_NONE;
#endif
      }
    }
  }
  else if(event->evt == MMGSDI_PERSO_EVT)
  {
    if(
#ifdef FEATURE_DUAL_SIM
       (mm_sim_perso_state_sim[as_id] != MMGSDI_PERSO_STATUS_DONE)
#else
        (mm_sim_perso_state != MMGSDI_PERSO_STATUS_DONE)
#endif
       )
    {
#ifdef FEATURE_DUAL_SIM
      mm_sim_perso_state_sim[as_id] = event->data.perso.status;
#else
      mm_sim_perso_state = event->data.perso.status;
#endif
      MSG_HIGH_DS_1(MM_LOCAL_SUB, "=MM= Perso event %d is received ",event->data.perso.status);
    }  
  } 
}

boolean mm_sim_open_session
(
  mmgsdi_session_type_enum_type session_type,
  mmgsdi_client_id_type client_id
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  ,sys_modem_as_id_e_type as_id
#endif
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;
  
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  if ((session_type != MMGSDI_GW_PROV_PRI_SESSION) &&
      (session_type != MMGSDI_GW_PROV_SEC_SESSION) 
#ifdef FEATURE_TRIPLE_SIM
      &&(session_type != MMGSDI_GW_PROV_TER_SESSION)
#endif
     )
  {
    MSG_ERROR_DS(MM_LOCAL_SUB, "=MM= Invalid session type: %d",session_type,0,0);
    mm_sim_card_mode_sim[as_id]= MMGSDI_APP_UNKNOWN;
    return FALSE;
  }
  else
  {
    /* set this flag before storing client ID so that MM task wouldnt open session again (if preempted/in multi-threaded env) */
    mm_waiting_for_open_session_cnf_sim[as_id] = TRUE;
    mm_sim_mmgsdi_info_sim[as_id].session_type = session_type;
    mm_sim_mmgsdi_info_sim[as_id].client_id = client_id;

    mmgsdi_status = mmgsdi_session_open( 
                                        mm_sim_mmgsdi_info_sim[as_id].client_id,
                                        mm_sim_mmgsdi_info_sim[as_id].session_type,
                                        0,
                                        mm_sim_session_generic_event_cb,
                                        FALSE,
                                        mm_sim_mmgsdi_generic_cb,
                                        (mmgsdi_client_data_type) mm_sim_mmgsdi_info_sim[as_id].session_type
                                       );
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_FATAL_DS(MM_SUB,"=MM= MMGSDI Registration session open cmd fail : STATUS %d",
                  (uint8) mmgsdi_status, 0, 0);
    }
  }

#else
  mm_waiting_for_open_session_cnf = TRUE;
  mm_sim_mmgsdi_info.session_type = session_type;
  mm_sim_mmgsdi_info.client_id = client_id;

  mmgsdi_status = mmgsdi_session_open( 
                                      mm_sim_mmgsdi_info.client_id,
                                      mm_sim_mmgsdi_info.session_type,
                                      0,
                                      mm_sim_session_generic_event_cb,
                                      FALSE,
                                      mm_sim_mmgsdi_generic_cb,
                                      0
                                     );
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_FATAL_DS(MM_SUB,"=MM= MMGSDI Registration session open cmd fail : STATUS %d",
                (uint8) mmgsdi_status, 0, 0);
  }

#endif

  return TRUE;
}

#ifdef FEATURE_FEMTO_CSG

/*===========================================================================

FUNCTION mmsim_decode_length

DESCRIPTION
     This function decodes the length according to ISO/IEC 8825

DEPENDENCIES
  None 

RETURN VALUE
  Length

SIDE EFFECTS


===========================================================================*/
int mmsim_decode_length(byte *len, mmgsdi_len_type *offset) 
{

  //len should point to the first byte of length field
  int decoded_len = 0;
  int first_byte = 0;
  int i = 0;
  int number_of_octets = 0;

   
  ASSERT( len != NULL );

  ASSERT ( offset != NULL );

  first_byte = *len++;

  if( first_byte & 0x80 )
  { 
    //Multi-byte length
    number_of_octets = first_byte & 0x7F;
    for(i = 0; i < number_of_octets; i++ ){
      decoded_len *= 256;
      decoded_len += *len++;
    }
    *offset = (mmgsdi_len_type)number_of_octets;
  } else{
    //Single byte length
    decoded_len = first_byte;
    *offset =  1;
  }
  return decoded_len;
}

/*===========================================================================

FUNCTION mmsim_csg_list_read_complete

DESCRIPTION
  This function checks if read for both the operator and allowed CSG list is complete and then 
  copy list to active csg lists and send list to WRRC/LRRC

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/

void mmsim_csg_list_read_complete(void)
{
  uint16 index;

#ifdef FEATURE_LTE
  emm_ctrl_data_type *emm_ctrl_data_ptr = NULL;

  emm_ctrl_data_ptr = emm_db_get_ctrl_data();

  ASSERT(emm_ctrl_data_ptr != NULL);
#endif
   /* If reading for both are files are complete then send list to AS*/
  if(mm_sim_operator_csg_list_read_started == FALSE && 
     mm_sim_allowed_csg_list_read_started == FALSE &&
     (mm_is_multimode_sub() == TRUE))
  {

    /*mm_sim_operator_csg_list.csg should be non NULL if operator list reading was done
      Copying read list to active list */
    if(mm_sim_operator_csg_list.csg != NULL)
    {
      if(mm_sim_active_operator_csg_list.csg != NULL)
      {
        modem_mem_free(mm_sim_active_operator_csg_list.csg, MODEM_MEM_CLIENT_NAS);
        mm_sim_active_operator_csg_list.csg = NULL;
      }
      if(mm_sim_operator_csg_list.length > 0)
      {
        mm_sim_active_operator_csg_list.csg = 
                            (sys_csg_list_info_s_type*)modem_mem_alloc(sizeof(sys_csg_list_info_s_type) * mm_sim_operator_csg_list.length,  MODEM_MEM_CLIENT_NAS);
       
	mm_check_for_null_ptr((void*) mm_sim_active_operator_csg_list.csg );
		
        for(index = 0; index < mm_sim_operator_csg_list.length; index++)
        {
          mm_sim_active_operator_csg_list.csg[index] = mm_sim_operator_csg_list.csg[index];
        }        
      }
      mm_sim_active_operator_csg_list.length = mm_sim_operator_csg_list.length;
      modem_mem_free(mm_sim_operator_csg_list.csg, MODEM_MEM_CLIENT_NAS);
      mm_sim_operator_csg_list.csg = NULL;
    }

     /*mm_sim_csg_indicator_list.list should be non NULL if operator list reading was done
      Copying read list to active list */
    if(mm_sim_csg_indicator_list.list != NULL)
    {
      if(mm_sim_active_csg_indicator_list.list != NULL)
      {
        modem_mem_free(mm_sim_active_csg_indicator_list.list, MODEM_MEM_CLIENT_NAS);
        mm_sim_active_csg_indicator_list.list = NULL;
      }
      if(mm_sim_csg_indicator_list.length > 0)
      {
        mm_sim_active_csg_indicator_list.list = 
          (mm_sim_display_indicator_info_s_type*) modem_mem_alloc(sizeof(mm_sim_display_indicator_info_s_type) * mm_sim_csg_indicator_list.length,  MODEM_MEM_CLIENT_NAS);
		
	mm_check_for_null_ptr((void*) mm_sim_active_csg_indicator_list.list );

      }
      for(index = 0; index < mm_sim_csg_indicator_list.length; index++)
      {
        mm_sim_active_csg_indicator_list.list[index] = mm_sim_csg_indicator_list.list[index];
      }
      mm_sim_active_csg_indicator_list.length = mm_sim_csg_indicator_list.length;
      modem_mem_free(mm_sim_csg_indicator_list.list, MODEM_MEM_CLIENT_NAS);
      mm_sim_csg_indicator_list.list = NULL;
   }

     /*mm_sim_allowed_csg_list.csg should be non NULL if allowed list reading was done
     Copying read list to active list */
    if(mm_sim_allowed_csg_list.csg != NULL)
    {
      if(mm_sim_active_allowed_csg_list.csg != NULL)
      {
        modem_mem_free(mm_sim_active_allowed_csg_list.csg, MODEM_MEM_CLIENT_NAS);
        mm_sim_active_allowed_csg_list.csg = NULL;
      }
      if(mm_sim_allowed_csg_list.length > 0)
      {
        mm_sim_active_allowed_csg_list.csg = 
                            (sys_csg_list_info_s_type*)modem_mem_alloc(sizeof(sys_csg_list_info_s_type) * mm_sim_allowed_csg_list.length,  MODEM_MEM_CLIENT_NAS);
		
        mm_check_for_null_ptr((void*)mm_sim_active_allowed_csg_list.csg );
     
        for(index = 0; index < mm_sim_allowed_csg_list.length; index++)
        {
          mm_sim_active_allowed_csg_list.csg[index] = mm_sim_allowed_csg_list.csg[index];
        }
      }
      mm_sim_active_allowed_csg_list.length = mm_sim_allowed_csg_list.length;
      modem_mem_free(mm_sim_allowed_csg_list.csg, MODEM_MEM_CLIENT_NAS);
      mm_sim_allowed_csg_list.csg = NULL;
	  if (mmsim_camped_csg_add_to_white_list == TRUE && mm_serving_plmn.info.service_status == SYS_SRV_STATUS_SRV)
	  {
	    mm_add_csg_to_white_list(mm_serving_plmn.info.plmn,&mm_serving_plmn.info.csg_info);
		mmsim_camped_csg_add_to_white_list = FALSE;
	  }
    }

    mm_output_detailed_csg_list();
     /*send white list */
    mm_send_rrc_csg_white_list_ind(&mm_sim_active_operator_csg_list, &mm_sim_active_allowed_csg_list);
    mm_send_rr_csg_white_list_ind();
#ifdef FEATURE_LTE
    emm_send_rrc_system_update(emm_ctrl_data_ptr);
#endif 
    mm_check_camped_csg_not_white_list();
    mmsend_mm_csg_white_list_log_packet(NAS_LOG_OCSGL_TYPE, &mm_sim_active_operator_csg_list);
    mmsend_mm_csg_white_list_log_packet(NAS_LOG_ACSGL_TYPE, &mm_sim_active_allowed_csg_list);
    mm_send_mmr_csg_list_read_complete_ind();
  }
}

/*===========================================================================

FUNCTION mmsim_read_allowed_csg_list

DESCRIPTION
  This function read allowed list from EFS 

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void mm_sim_read_allowed_csg_from_nv(sys_csg_list_s_type *csg_list)
{
  byte *csg_data;

  mcfg_fs_status_e_type  status = MCFG_FS_STATUS_EFS_ERR;

  uint16 data_size;
  uint16 offset = 0, index;


  data_size = (SYS_CSG_LIST_MAX_LENGTH * sizeof(sys_csg_list_info_s_type)) + sizeof(uint16);

  csg_data = (byte *) modem_mem_alloc(data_size, MODEM_MEM_CLIENT_NAS);

  mm_check_for_null_ptr((void*)csg_data );

  status = mcfg_fs_read("/nv/item_files/modem/nas/allowed_csg_list", 
                           csg_data, data_size, MCFG_FS_TYPE_EFS,(mcfg_fs_sub_id_e_type)mm_sub_id);
  if(status == MCFG_FS_STATUS_OK)
  {
    MSG_HIGH_DS_0(MM_SUB, "=MM= CSG: Reading ALLOWED CSFG list from NV");
    csg_list->length = csg_data[0] | csg_data[1] << 8;
    
    offset = 2;
   
    for(index = 0; (index < csg_list->length) && (csg_list->length <= SYS_CSG_LIST_MAX_LENGTH); index++)
    {
      csg_list->csg[index].plmn.identity[0] = csg_data[offset];
      csg_list->csg[index].plmn.identity[1] = csg_data[offset+1];
      csg_list->csg[index].plmn.identity[2] = csg_data[offset+2];
      offset += 3;
     
      csg_list->csg[index].csg_id = csg_data[offset];
      csg_list->csg[index].csg_id  |= csg_data[offset+1] << 8;
      csg_list->csg[index].csg_id  |= csg_data[offset+2] << 16;
      csg_list->csg[index].csg_id  |= csg_data[offset+3] << 24;
      offset += 4;
    }
  }
  else
  {
    MSG_HIGH_DS_1(MM_SUB, "=MM= CSG: Reading ALLOWED CSFG list from NV failed, status = %d", status);
  }
  modem_mem_free(csg_data, MODEM_MEM_CLIENT_NAS);
}


/*===========================================================================

FUNCTION mm_sim_write_allowed_csg_to_nv

DESCRIPTION
  This function write allowed list from EFS 

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void mm_sim_write_allowed_csg_to_nv(sys_csg_list_s_type *csg_list)
{
  byte *csg_data;

  mcfg_fs_status_e_type efs_status = MCFG_FS_STATUS_EFS_ERR;
  
  uint16 data_size;
  uint16 offset = 0, index;


  data_size = (SYS_CSG_LIST_MAX_LENGTH * sizeof(sys_csg_list_info_s_type)) + sizeof(uint16);

  csg_data = (byte *) modem_mem_alloc(data_size, MODEM_MEM_CLIENT_NAS);

  mm_check_for_null_ptr((void*)csg_data );

  csg_data[0] = (byte)csg_list->length;
  csg_data[1] = (byte)(csg_list->length >> 8);
  offset = 2;
 
  for(index = 0; (index < csg_list->length) && (csg_list->length <= SYS_CSG_LIST_MAX_LENGTH); index++)
  {
    csg_data[offset] = csg_list->csg[index].plmn.identity[0];
    csg_data[offset+1] = csg_list->csg[index].plmn.identity[1];
    csg_data[offset+2] = csg_list->csg[index].plmn.identity[2];
    offset += 3;
   
    csg_data[offset+0] = (byte)csg_list->csg[index].csg_id;
    csg_data[offset+1] = (byte)(csg_list->csg[index].csg_id >> 8);
    csg_data[offset+2] = (byte)(csg_list->csg[index].csg_id >> 16);
    csg_data[offset+3] = (byte)(csg_list->csg[index].csg_id >> 24);
    offset += 4;
  }
  if((efs_status = mcfg_fs_write("/nv/item_files/modem/nas/allowed_csg_list" , 
                           csg_data,
                           data_size, MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR, MCFG_FS_ALLPERMS,
                           MCFG_FS_TYPE_EFS, (mcfg_fs_sub_id_e_type)mm_sub_id)) != MCFG_FS_STATUS_OK)
  {
    MSG_ERROR_DS(MM_SUB, "=MM= CSG: Wrote allowed CSG list to NV failed, status = %d", efs_status, 0, 0 );
  }
  else
  {  
    MSG_HIGH_DS_0(MM_SUB, "=MM=  Wrote allowed CSG list to NV");
  }

  modem_mem_free(csg_data, MODEM_MEM_CLIENT_NAS);



}
/*===========================================================================

FUNCTION mmsim_start_read_allowed_csg_list

DESCRIPTION
  This function start reading for allowed CSG list and reads attribute for file

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void mmsim_start_read_allowed_csg_list(void)
{

  mmgsdi_return_enum_type status = MMGSDI_ERROR;

  mmgsdi_access_type file_name;

  mmgsdi_client_data_type client_data = 0;


  if(mm_sim_card_mode == MMGSDI_APP_SIM)
  {
    return;
  }

  mm_sim_allowed_csg_num_records = 0;
  //Allocate memory for temp allowed csg list
  if(mm_sim_allowed_csg_list.csg == NULL)
  {
    mm_sim_allowed_csg_list.csg = 
           (sys_csg_list_info_s_type*)modem_mem_alloc(sizeof(sys_csg_list_info_s_type)*SYS_CSG_LIST_MAX_LENGTH,  MODEM_MEM_CLIENT_NAS);

   mm_check_for_null_ptr((void*)mm_sim_allowed_csg_list.csg );
  }
  mm_sim_allowed_csg_list.length = 0;

  file_name.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_name.file.file_enum = MMGSDI_USIM_ACSGL;

#ifdef FEATURE_DUAL_SIM
  client_data = (mmgsdi_client_data_type)mm_as_id;
#endif 

  /* Map the csg list read sequence number in bits 9-32 of client_data */
  client_data = client_data | (mm_sim_csg_list_read_sqn << 8);

  status = mmgsdi_session_get_file_attr(mm_sim_mmgsdi_info.session_id, 
                                              file_name, 
                                              mm_sim_mmgsdi_generic_cb, 
                                              client_data);
  
  //If reading of attribute fails then it means file is not present in SIM
  if(status != MMGSDI_SUCCESS)
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= CSG: Allowed CSG not read from SIM, using from NV");
    mm_sim_allowed_csg_list_read_started = FALSE;
    mm_using_allowed_csg_nv = TRUE;
    mm_sim_read_allowed_csg_from_nv(&mm_sim_allowed_csg_list);
  }
  else
  {
    mm_using_allowed_csg_nv = FALSE;
    mm_sim_allowed_csg_list_read_started = TRUE;
  }
  mmsim_csg_list_read_complete();
}



/*===========================================================================

FUNCTION mmsim_read_operator_csg_list

DESCRIPTION
  This function start reading for operator CSG list 

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void mmsim_start_read_operator_csg_list(void)
{
  mmgsdi_return_enum_type status = MMGSDI_ERROR;
  mmgsdi_access_type file_name;
  byte               admin_data[MM_SIM_ADMIN_LENGTH];
  mmgsdi_client_data_type client_data = 0;

  mm_sim_operator_csg_num_records = 0;
  mm_ad_csg_indicator = 0;
  file_name.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_name.file.file_enum = MMGSDI_USIM_OCSGL;

  memset((void *)&admin_data[0], 0, sizeof(admin_data));

  if(mm_sim_card_mode == MMGSDI_APP_SIM)
  {
    return;
  }

  /*Read admin data for display indicator*/
  if(mm_sim_csg_display_service == TRUE)
  {
    if(mm_sim_read(MMGSDI_USIM_AD, admin_data, MM_SIM_ADMIN_LENGTH))
    {
      /* Refer to 31.102 section 4.2.18*/
      mm_ad_csg_indicator = (admin_data[2] >> 1) & 0x01;
      MSG_HIGH_DS_1(MM_SUB ,"=MM= CSG: Admin CSG display  indicator = %d", mm_ad_csg_indicator);
    }
  }

  if(mm_sim_operator_csg_list.csg == NULL)
  {
    mm_sim_operator_csg_list.csg = 
               (sys_csg_list_info_s_type*)modem_mem_alloc(sizeof(sys_csg_list_info_s_type) * SYS_CSG_LIST_MAX_LENGTH,  MODEM_MEM_CLIENT_NAS);
    mm_check_for_null_ptr((void*)mm_sim_operator_csg_list.csg );   
  }
  if(mm_sim_csg_indicator_list.list == NULL)
  {
    mm_sim_csg_indicator_list.list  = 
               (mm_sim_display_indicator_info_s_type*)modem_mem_alloc(sizeof(mm_sim_display_indicator_info_s_type) * SYS_CSG_LIST_MAX_LENGTH,  MODEM_MEM_CLIENT_NAS);
    mm_check_for_null_ptr((void*)mm_sim_csg_indicator_list.list );

  }
  mm_sim_operator_csg_list.length = 0;
  
  mm_sim_csg_indicator_list.length = 0;

#ifdef FEATURE_DUAL_SIM
  client_data = (mmgsdi_client_data_type)mm_as_id;
#endif 

  /* Map the csg list read sequence number in bits 9-32 of client_data */
  client_data = client_data | (mm_sim_csg_list_read_sqn << 8);

  status = mmgsdi_session_get_file_attr(mm_sim_mmgsdi_info.session_id, 
                                              file_name, 
                                              mm_sim_mmgsdi_generic_cb, 
                                              client_data);

  
  if(status != MMGSDI_SUCCESS)
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= CSG: Operator CSG list not found in SIM");
    mm_sim_ocsgl_present = FALSE;
    mm_sim_operator_csg_list_read_started = FALSE;    
  }
  else
  {
    mm_sim_ocsgl_present = TRUE;
    mm_sim_operator_csg_list_read_started = TRUE;
  }

   mmsim_csg_list_read_complete();
}

/*===========================================================================

FUNCTION mmsim_read_operator_csg_list_record

DESCRIPTION
  This function parses the read record

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void mmsim_parse_read_csg_list_tlv
(
  mmgsdi_file_enum_type  file_name,
  sys_csg_list_s_type *csg_list, 
  mmgsdi_read_data_type *read_data, 
  mmgsdi_len_type *data_read_offset
)
{
  uint8 *data = read_data->data_ptr;

  mmgsdi_len_type sim_offset = *data_read_offset; 
  int  length_tlv; 
  sys_plmn_id_s_type plmn;
  sys_csg_id_type   csg_id;
  mmgsdi_len_type offset;

  if(data[sim_offset] != 0xA0)
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= Error in CSG list - Skipping");
    *data_read_offset = read_data->data_len; 
    return;
  }
  sim_offset++;

  length_tlv = mmsim_decode_length(data+sim_offset, &offset);
  sim_offset += offset;

  if(length_tlv > read_data->data_len - sim_offset)
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= Error in CSG list - Skipping");
    *data_read_offset = read_data->data_len; 
    return;
  }
  /*read PLMN TAG*/
  if(data[sim_offset] != 0x80)
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= Error in CSG list - Skipping");
    *data_read_offset = read_data->data_len; 
    return;
  }
  sim_offset++;

  /*read PLMN length*/
  if(mmsim_decode_length(data+sim_offset, &offset)!= 3)
  {
    MSG_HIGH_DS_0(MM_SUB,"=MM= Error in CSG list - Skipping");
    *data_read_offset = read_data->data_len; 
    return;  
  }

  sim_offset += offset;

  /*read PLMN Id*/
  plmn.identity[0] = data[sim_offset];
  plmn.identity[1] = data[sim_offset+1];
  plmn.identity[2] = data[sim_offset+2];
  sim_offset += 3;
  MSG_HIGH_DS_3(MM_SUB, "=MM= PLMN ID = %x %x %x", plmn.identity[0], plmn.identity[1], plmn.identity[2]);

  /*Read CSG infomration */
  while(data[sim_offset] == 0x81)
  {
    sim_offset ++;
    if(mmsim_decode_length(data+sim_offset, &offset) != 6)
    {
      MSG_HIGH_DS_0(MM_SUB,"=MM= Error in CSG list - Skipping");
      *data_read_offset = read_data->data_len; 
      return;
    }
    sim_offset += offset;

    /*skip hnb name and type*/
    sim_offset += 2;

    csg_id = 0;

    csg_id = csg_id | (sys_csg_id_type)data[sim_offset] <<19;
    csg_id = csg_id | (sys_csg_id_type)data[sim_offset+1] << 11;
    csg_id = csg_id | (sys_csg_id_type)data[sim_offset+2] << 3;
    csg_id = csg_id | (sys_csg_id_type)data[sim_offset+3] >> 5;

    MSG_HIGH_DS_1(MM_SUB ,"=MM= CSG ID = % d", csg_id);

    if(csg_list->length < SYS_CSG_LIST_MAX_LENGTH)
    {
      csg_list->csg[csg_list->length].plmn = plmn;
      csg_list->csg[csg_list->length].csg_id = csg_id;
      csg_list->csg[csg_list->length].timer_count = MM_TIMER_DISABLED;
      csg_list->length++;
    }
    else 
    {
      MSG_HIGH_DS_0(MM_SUB, "=MM= CSG list full, skipping");
    }

    /*Increment legnth for CSG id and s*/
    sim_offset += 4;
  }
  
  /* Read display indicator TAG */
  if(file_name == MMGSDI_USIM_OCSGL)
  {
    if(data[sim_offset] == 0x82)
    {
      if(mm_sim_csg_display_service == TRUE)
      {
        if(mm_sim_csg_indicator_list.length < SYS_CSG_LIST_MAX_LENGTH)
        {
          mm_sim_csg_indicator_list.list[mm_sim_csg_indicator_list.length].indicator = data[sim_offset+2];
          mm_sim_csg_indicator_list.list[mm_sim_csg_indicator_list.length].plmn = plmn;
          mm_sim_csg_indicator_list.length += 1;
        }
      }
      sim_offset += 3;
    }
  }
  *data_read_offset = sim_offset;
  return;
} 

/*===========================================================================

FUNCTION mmsim_read_operator_csg_list_record

DESCRIPTION
  This function process record read from SIM card and send read for next record

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void mmsim_read_operator_csg_list_record(mmgsdi_read_cnf_type  *ptr)
{
  mmgsdi_rec_num_type next_record = -1;
  
  mmgsdi_access_type file_name;

  mmgsdi_return_enum_type status = MMGSDI_ERROR;

  mmgsdi_len_type   offset = 0;

  uint8  *data;

  mmgsdi_client_data_type client_data = 0;

  if(mm_sim_operator_csg_list.csg == NULL)
  {
    return;
  }

  if(ptr == NULL)
  {
    if(mm_sim_operator_csg_num_records > 0)
    {
      next_record = 1;
    }
  }
  else if(ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= OCSGL record read error, Status = %d", ptr->response_header.mmgsdi_status);
    next_record = -1;
    mm_sim_operator_csg_list.length = 0;
    mm_sim_csg_indicator_list.length = 0;   
  }
  else
  {
   if(ptr->read_data.data_len > 0)
   {
      data = ptr->read_data.data_ptr;

     while(offset < ptr->read_data.data_len)
     {
        if(data[offset] == 0xA0)
        {
          mmsim_parse_read_csg_list_tlv(MMGSDI_USIM_OCSGL, &mm_sim_operator_csg_list, &ptr->read_data, &offset);
        }
        else
        {
          break;
        }
     }
   }
   if(ptr->accessed_rec_num < mm_sim_operator_csg_num_records)
   {
     next_record = ptr->accessed_rec_num + 1;
   }
  }

  /*Read next record*/
  if(next_record != -1)
  {
    file_name.access_method = MMGSDI_EF_ENUM_ACCESS;
    file_name.file.file_enum = MMGSDI_USIM_OCSGL;

 #ifdef FEATURE_DUAL_SIM
  client_data = (mmgsdi_client_data_type)mm_as_id;
#endif 
   
    /* Map the csg list read sequence number in bits 9-32 of client_data */
    client_data = client_data | (mm_sim_csg_list_read_sqn << 8);
   
    status = mmgsdi_session_read_record(mm_sim_mmgsdi_info.session_id,
                                              file_name,
                                              next_record,
                                              mm_sim_operator_csg_record_length,
                                              mm_sim_mmgsdi_generic_cb,
                                              client_data); 

   if(status != MMGSDI_SUCCESS)
   {
     mm_sim_operator_csg_list_read_started = FALSE;
   }
  }
  else
  {
    mm_sim_operator_csg_list_read_started = FALSE;
  }

  mmsim_csg_list_read_complete();

}



/*===========================================================================

FUNCTION mmsim_read_allowed_csg_list_record

DESCRIPTION
  This function process record read from SIM card and send read for next record

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void mmsim_read_allowed_csg_list_record(mmgsdi_read_cnf_type  *ptr)
{
  mmgsdi_rec_num_type next_record = -1;
  
  mmgsdi_access_type file_name;

  mmgsdi_return_enum_type status = MMGSDI_ERROR;

  mmgsdi_len_type   offset = 0;

  uint8  *data;

  mmgsdi_client_data_type client_data = 0;

  if(mm_sim_allowed_csg_list.csg == NULL)
  {
    return;
  }
  if(ptr == NULL)
  {
    if(mm_sim_allowed_csg_num_records > 0)
    {
      next_record = 1;
    }
  }
  else if(ptr->response_header.mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_HIGH_DS_1(MM_SUB ,"=MM= ACSGL record read error, Status = %d", ptr->response_header.mmgsdi_status);
    next_record = -1;
    mm_sim_allowed_csg_list.length = 0;
  }
  else
  {
   if(ptr->read_data.data_len > 0)
   {
      data = ptr->read_data.data_ptr;

     while(offset < ptr->read_data.data_len)
     {
        if(data[offset] == 0xA0)
        {
          mmsim_parse_read_csg_list_tlv(MMGSDI_USIM_ACSGL, &mm_sim_allowed_csg_list, &ptr->read_data, &offset);
        }
        else
        {
          break;
        }
     }
   }
   if(ptr->accessed_rec_num < mm_sim_allowed_csg_num_records)
   {
     next_record = ptr->accessed_rec_num + 1;
   }
  }

  /*Read next record*/
  if(next_record != -1)
  {
    file_name.access_method = MMGSDI_EF_ENUM_ACCESS;
    file_name.file.file_enum = MMGSDI_USIM_ACSGL;

#ifdef FEATURE_DUAL_SIM
    client_data = (mmgsdi_client_data_type)mm_as_id;
#endif 
   
    /* Map the csg list read sequence number in bits 9-32 of client_data */
    client_data = client_data | (mm_sim_csg_list_read_sqn << 8);

    status = mmgsdi_session_read_record(mm_sim_mmgsdi_info.session_id,
                                              file_name,
                                              next_record,
                                              mm_sim_allowed_csg_record_length,
                                              mm_sim_mmgsdi_generic_cb,
                                              client_data); 
   if(status != MMGSDI_SUCCESS)
   {
     mm_sim_allowed_csg_list_read_started = FALSE;
   }
  }
  else
  {
    mm_sim_allowed_csg_list_read_started = FALSE;
  }

  mmsim_csg_list_read_complete();

}

/*===========================================================================

FUNCTION mmsim_process_get_file_attr_cnf

DESCRIPTION
  This function process get file attr cnf 

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void mmsim_process_get_file_attr_cnf(mm_cmd_type *message)
{
  mmgsdi_get_file_attr_cnf_type *cnf_data = &message->cmd.mmgsdi_get_file_attr_cnf.cnf_data;

  mmgsdi_file_enum_type  file_enum = cnf_data->access.file.file_enum;

  mmgsdi_return_enum_type status = cnf_data->response_header.mmgsdi_status;

  /* CSG list read sequence number is mapped to bits 9-32 (MSBs 2-4) of client_data.
     Bits 1-8 are mapped to AS ID */
  uint16 csg_list_read_sqn_num = 
                        (uint16)(cnf_data->response_header.client_data >> 8);

  /* Ignore file_attr_cnf if received csg list read sequence number doesn't match the 
     sent csg list read sequence number*/
  if(mm_sim_csg_list_read_sqn != csg_list_read_sqn_num)
  {
    MSG_HIGH_DS_3(MM_SUB, "=MM= SQN mismatch. Ignore file_attr_cnf.Rx SQN = %d, Sent SQN = %d, file_enum = %d",
                   csg_list_read_sqn_num,mm_sim_csg_list_read_sqn,file_enum);
    return;
  }

  switch(file_enum)
  {
    case MMGSDI_USIM_OCSGL:
      if(status == MMGSDI_SUCCESS)
      {
        mm_sim_operator_csg_num_records = cnf_data->file_attrib.file_info.linear_fixed_file.num_of_rec;
        MSG_HIGH_DS_1(MM_SUB ,"=MM= CSG: Operator CSG list : num_recodrs = %d", mm_sim_operator_csg_num_records); 
        mm_sim_operator_csg_record_length = cnf_data->file_attrib.file_info.linear_fixed_file.rec_len;
        mmsim_read_operator_csg_list_record(NULL);
      }
      else
      {
        MSG_HIGH_DS_1(MM_SUB ,"=MM= CSG: Operator CSG list get attribute read failed, status = %d", status);
        mm_sim_operator_csg_list_read_started = FALSE;
        mm_sim_ocsgl_present = FALSE;
        mmsim_csg_list_read_complete();
      }
      break;

    case MMGSDI_USIM_ACSGL:
      if(status == MMGSDI_SUCCESS)
      {
        mm_sim_allowed_csg_num_records = cnf_data->file_attrib.file_info.linear_fixed_file.num_of_rec;
        MSG_HIGH_DS_1(MM_SUB ,"=MM= CSG: Allowed CSG list : num_recodrs = %d", mm_sim_allowed_csg_num_records); 
        mm_sim_allowed_csg_record_length = cnf_data->file_attrib.file_info.linear_fixed_file.rec_len;
        mmsim_read_allowed_csg_list_record(NULL);
      }
      else 
      {
        MSG_HIGH_DS_1(MM_SUB ,"=MM= CSG: Allowed CSG list get attribute read failed, status = %d, Read from NV", status);
        mm_sim_allowed_csg_list_read_started = FALSE;
        mm_using_allowed_csg_nv = TRUE;
        mm_sim_read_allowed_csg_from_nv(&mm_sim_allowed_csg_list);
        mmsim_csg_list_read_complete();
      }
      break;

    default:
        MSG_HIGH_DS_1(MM_SUB ,"=MM= Error Received attribute for file %d", file_enum);
  }
}


void mmsim_process_read_file_cnf(mm_cmd_type *message)
{
  mmgsdi_read_cnf_type *cnf_data = &message->cmd.mmgsdi_read_file_cnf.cnf_data;

  mmgsdi_file_enum_type  file_enum = cnf_data->access.file.file_enum;

  /* CSG list read sequence number is mapped to bits 9-32 (MSBs 2-4) of client_data.
     Bits 1-8 are mapped to AS ID */
  uint16 csg_list_read_sqn_num = 
                        (uint16)(cnf_data->response_header.client_data >> 8);

  /* Ignore reading csg record if received csg list read sequence number doesn't match the 
     sent csg list read sequence number*/
  if(mm_sim_csg_list_read_sqn != csg_list_read_sqn_num)
  {
    MSG_HIGH_DS_3(MM_SUB, "=MM= SQN mismatch. Skip record read.Rx SQN = %d, Sent SQN = %d, file_enum = %d",
                   csg_list_read_sqn_num,mm_sim_csg_list_read_sqn,file_enum);
    return;
  }

  switch(file_enum)
  {
    case MMGSDI_USIM_OCSGL:
      MSG_HIGH_DS_0(MM_SUB,"=MM= operator CSG record read");
      mmsim_read_operator_csg_list_record(cnf_data);
      break;
   
    case MMGSDI_USIM_ACSGL:
      MSG_HIGH_DS_0(MM_SUB,"=MM= Allowed CSG record read");
      mmsim_read_allowed_csg_list_record(cnf_data);
      break;
  
    default:
      MSG_HIGH_DS_1(MM_SUB ,"=MM= Reading record for file %d not supported", file_enum);
      break;
   }

   if(cnf_data->read_data.data_ptr != NULL)
   {
     modem_mem_free(cnf_data->read_data.data_ptr,  MODEM_MEM_CLIENT_NAS);
   }
}


/*===========================================================================

FUNCTION mmsim_read_csg_list

DESCRIPTION
  This function starts reading of allowed and operator csg list

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void mm_sim_read_csg_list(void)
{
  mm_sim_allowed_csg_list_read_started = TRUE;
  mm_sim_operator_csg_list_read_started = TRUE;

  mmsim_start_read_allowed_csg_list();
  mmsim_start_read_operator_csg_list();
}

/*===========================================================================

FUNCTION mmsim_update_allowed_csg_list

DESCRIPTION
  This function is used for updating allowed csg list in SIM

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void mmsim_update_allowed_csg_list
(
  sys_plmn_id_s_type plmn_id,
  sys_csg_info_s_type  *csg_info,
  mmgsdi_update_acsgl_enum_type type
)
{

  mmgsdi_plmn_csg_id_type              plmn_csg_id;
  mmgsdi_data_type                     hnb_name;

  plmn_csg_id.plmn_id_val[0] = (uint8)plmn_id.identity[0];
  plmn_csg_id.plmn_id_val[1] = (uint8)plmn_id.identity[1];
  plmn_csg_id.plmn_id_val[2] = (uint8)plmn_id.identity[2];

  plmn_csg_id.csg_id = csg_info->csg_id;
  hnb_name.data_len = csg_info->hnb_name.length;
  hnb_name.data_ptr = csg_info->hnb_name.name;

  mmgsdi_session_update_acsgl_record(
    mm_sim_mmgsdi_info.session_id,
    type,
    plmn_csg_id,
    hnb_name,
    mm_sim_mmgsdi_generic_cb,
    NULL
    );

}

/*===========================================================================

FUNCTION mmsim_reset_csg_data

DESCRIPTION
  This function is used to reset the CSG data in cache

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void mmsim_reset_csg_data(void)
{
  mm_sim_allowed_csg_list_read_started = FALSE;
  mm_sim_operator_csg_list_read_started = FALSE;

  mm_sim_csg_list_read_sqn = (mm_sim_csg_list_read_sqn + 1);

  if(mm_sim_operator_csg_list.csg != NULL)
  {
    modem_mem_free(mm_sim_operator_csg_list.csg, MODEM_MEM_CLIENT_NAS);
    mm_sim_operator_csg_list.csg = NULL;
    mm_sim_operator_csg_num_records = 0;
    mm_sim_operator_csg_list.length = 0;
    mm_sim_operator_csg_record_length = 0;
    mm_sim_ocsgl_present = FALSE;
  }

  if(mm_sim_active_operator_csg_list.csg != NULL)
  {
    modem_mem_free(mm_sim_active_operator_csg_list.csg, MODEM_MEM_CLIENT_NAS);
    mm_sim_active_operator_csg_list.csg = NULL;
    mm_sim_active_operator_csg_list.length = 0;
  }

  if(mm_sim_csg_indicator_list.list != NULL)
  {
    modem_mem_free(mm_sim_csg_indicator_list.list, MODEM_MEM_CLIENT_NAS);
    mm_sim_csg_indicator_list.list = NULL;
    mm_sim_csg_indicator_list.length = 0;
  }

  if(mm_sim_active_csg_indicator_list.list != NULL)
  {
    modem_mem_free(mm_sim_active_csg_indicator_list.list, MODEM_MEM_CLIENT_NAS);
    mm_sim_active_csg_indicator_list.list = NULL;
    mm_sim_active_csg_indicator_list.length = 0;
  }

  if(mm_sim_allowed_csg_list.csg != NULL)
  {
    modem_mem_free(mm_sim_allowed_csg_list.csg, MODEM_MEM_CLIENT_NAS);
    mm_sim_allowed_csg_list.csg = NULL;
    mm_sim_allowed_csg_num_records = 0;
    mm_sim_allowed_csg_list.length = 0;
    mm_sim_allowed_csg_record_length = 0;
  }

  if(mm_sim_active_allowed_csg_list.csg != NULL)
  {
    modem_mem_free(mm_sim_active_allowed_csg_list.csg, MODEM_MEM_CLIENT_NAS);
    mm_sim_active_allowed_csg_list.csg = NULL;
    mm_sim_active_allowed_csg_list.length = 0;
  }
}
#endif /*FEATURE_FEMTO_CSG*/

/*===========================================================================

FUNCTION MM_GET_IMSI

DESCRIPTION
  This function returns IMSI as stored in NAS context but returns null data if SIM is not available

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS
  imsi_out will have IMSI value

===========================================================================*/

void mm_get_imsi(imsi_data_T* imsi_out)
{
  if(imsi_out != NULL)
  {
#ifdef FEATURE_DUAL_SIM
    MSG_ERROR_DS( MM_SUB, "=MM= DUAL SIM: Wrong API called mm_get_imsi()", 0,0,0 );
    memset((void *)imsi_out, 0x00, sizeof(imsi_data_T));
#else
    if(mm_serving_plmn.info.sim_state == SYS_SIM_STATE_NOT_AVAILABLE)
    {
      memset((void *)imsi_out, 0x00, sizeof(imsi_data_T));
    }
    else
    {
      memscpy((void *)imsi_out,sizeof(imsi_data_T), (void *)&mm_stored_imsi, sizeof(imsi_data_T));
    }
#endif
  }
}

/*===========================================================================

FUNCTION MM_DS_GET_IMSI

DESCRIPTION
  DUAL SIM: This function returns IMSI as stored in NAS context but returns null data if SIM 
  is not available. 

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS
  imsi_out will have IMSI value

===========================================================================*/
void mm_ds_get_imsi
(
  imsi_data_T* imsi_out,
  sys_modem_as_id_e_type as_id
)
{
  if(imsi_out != NULL)
  {
#ifdef FEATURE_DUAL_SIM
    boolean status = FALSE;
    if(!IS_NOT_VALID_AS_ID(as_id))
    {
      if(mm_serving_plmn_sim[as_id].info.sim_state != SYS_SIM_STATE_NOT_AVAILABLE)
      {
        status = TRUE;
      }
    }

    if(!status)
    {
      MSG_ERROR_DS(MM_LOCAL_SUB, "=MM= Invalid AS ID:%d / SIM NA", 
                   as_id+1,0,0);
      memset((void *)imsi_out, 0x00, sizeof(imsi_data_T));
    }
    else
    {
      memscpy((void *)imsi_out,sizeof(imsi_data_T), (void *)&mm_stored_imsi_sim[as_id], sizeof(imsi_data_T));
    }
#else
    MSG_ERROR_DS(MM_LOCAL_SUB, "=MM= mm_ds_get_imsi - Wrong API called", 0,0,0);
    memset((void *)imsi_out, 0x00, sizeof(imsi_data_T));
#endif
  }
}
/*===========================================================================
FUNCTION      MM_UPDATE_SIM_STATE

DESCRIPTION
  updates the sim state
 
DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mm_update_sim_state
(
  sys_sim_state_e_type  sim_state
)
{
  if(mm_sim_card_mode == MMGSDI_APP_UNKNOWN)
  {
    return;
  }

  MSG_HIGH_DS_2(MM_SUB, "SIM RE-VALIDATE, new SIM state = %d previous SIM state %d",
                                         sim_state,mm_serving_plmn.info.sim_state);
  mm_serving_plmn.info.sim_state = sim_state;
}

