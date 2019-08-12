/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  C A L L   M A N A G E R


          S U P P L E M E N T A R Y   S E R V I C E   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager Sups Object functions for WCDMA/GSM.

  The Sups Object is responsible for:
  1. Processing clients' call independent sups commands.
  2. Processing Lower Layer replies for clients' sups commands.
  3. Processing Lower Layer call independent sups related notifications.
  4. Notifying the client list of sups events.


EXTERNALIZED FUNCTIONS

  cmsups_init
    Initializing the sups object.

  cmsups_client_cmd_proc
    Process call independent sups commands from the clients.

  cmsups_rpt_proc
    Process Lower Layer sups notifications/command replies.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmsups_init() must be called to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2001 - 2006, 2009 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmwsups.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/10/13   ar      checking value of sups_info_ptr
07/03/13   dk      KW error fixed in cmwsups_cmd_copy_fields_1
06/24/13   dk      KW error fix in cmwsups_cmd_copy_fields_1()
05/24/13   ss      Sups LCS_MOLR memory fix
04/23/13   xs      USSD detect support
01/07/13   qf      FR 2564 - World mode support for DSDA
11/22/12   xs      Added PS supplemetary service support
10/30/12   xs      PS supplementary service support
05/04/12   anm     Fix to Return error for sups command with GW no subs
02/15/12   vk      Replace usage of AEE library function with corresponding 
                   CoreBSP library
04/29/11   anm    Assigning proper AS ID to USSD phase1 req
04/21/11   sv      Correct Lint Errors
04/11/11   am      Cleaning stubs and runnning qtf SUTE
01/25/11   rm      Updating correct sups params with FEATURE_MODEM_HEAP_PHASE_2
12/23/10   sv      Integrate DSDS feature
12/17/2010 gm      Changed user_data parameter to const type in CC callback.
11/15/10   rm      Memory optimization changes
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
06/10/10   ak      Mainlining memory optimization changes.
06/02/10   rn      send default cc_generic_params
03/18/10   am      Fixing Compilation Warning
10/14/09   sg      ZI memory reduction. Mmode should call new Heap Manager API to 
                   allocate memory dynamically.
08/26/09   rm      When in limited service, do not return srv_status_error 
                   for 'Location notofication' sups command
08/07/09   rm      CMI phase-2: Removing featurisation from /api files
                   Removed including cause_v.h
07/26/09   rn      Centralised call control changes
05/26/09   mp      Updated SUPS event notification in SUSPEND state
04/06/09   mj      Update BOOLEAN macro w/ namespace prefix to CM_BOOLEAN
03/24/09   sv      updated to cmtaski.h
03/23/09   rm      Adding _v.h files from NAS
02/17/09   ks      Copying L3 data to sups_info ptr for more sups cnf
12/17/08   jd      Fixed Lint errors
09/05/08   rn      FDN checking option in call control
08/07/07   ic      Lint clean up
05/16/07   cl      Fix typo in include file AEEstd.h
05/02/07   rp      Adding FEATURE_CM_LCS
04/15/07   rp      Removing FEATURE_MMGPS
02/23/07   pk      Banned API lint fixes
02/08/07   rp      Copying L3 data to sups_info ptr for interrogate cnf.
01/19/07   pk      Reverting previous lint fixes
01/12/07   pk      Lint fixes
12/19/06   ka      Adding support to process UCS2 in CM_SUPS_CMD_PROCESS_USS.
11/09/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
09/29/06   rp/ka   Added code to initialize all the fields in sups object.
09/13/06   rp      Added code to copy Layer 3 message from MN to sups object
                   and sups client object and featurized under
                   FEATURE_SS_L3_DATA
08/21/08   ka      Fixing cmsups_ussd_phase1_req() by adding a call to
                   cm_mn_send_cmd().
07/04/06   ka      Changes to support call control on PS calls.
01/03/06   ic      Fixed compile error in cmsups_ussd_phase1_req()
                   <argument 2 to 'strncpy'>: implicit cast of pointer to
                   non-equal pointer
                   Fixed compile error in cmsups_ussd_phase1_req()
                   <argument 1 to 'cm_util_gsm_alphabet_to_ascii'>: implicit
                   cast of pointer to non-equal pointer
                   Correcting fields being sent for ussd phase 1 request.
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup
12/13/05   ic      Fixed compiler warnings
11/08/05   sk      Separated CM reports and commands.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
08/02/05   ic      Print value that caused error in default switch case
07/25/05   sk      Replaced (for fixing CR 67893)
                   cm_util_ascii_to_def_alphabet() with
                   cm_util_ascii_to_gsm_alphabet().
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
04/06/05   ka      Change #define name
02/09/05   ic      Lint cleanup
01/21/05   ka      Changing encoding scheme for CM_USSD_DCS_UNSPECIFIED to
                   be the same as 7bit encoding.
01/12/05   ka      Support for 7bit/8bit/unspec USSD data coding scheme.
11/15/04   ic      Lint cleanup with CM_DEBUG on
09/13/04   ka      Handling CM_CC_RESULT_NOT_ALLOWED in sups call control
                   call back.
08/26/04   sbs     Added externalAddress and nameString IEs to MOLR_Invoke /
                   LocationNotification SS messages
08/17/04   ic      Undoing P4 change list #121638
08/04/04   ic      CM Lint clean up - cmwsups.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/27/04   ws      Add Call Control support for SUPS.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
                   Added cmwsups_dummy() to get rid of ADS 1.2 compiler warning
07/07/04   ka      Fixed Lint errors.
05/27/04   ic      In cmsups_client_cmd_proc() call cmsups_client_cmd_err()
                   irrespective of whether error occured or not (client is
                   supposed to be always notified about command processing
                   status)
05/14/04   ka      Fixed Lint errors.
03/16/04   ka      Merged changes from 6250. Fixed Lint errors.
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
01/13/04   ka      Merged changes from MSM6200
12/19/03   RI      Added setting of USS data present field when a Phase 1
                   response is sent to clients in a Phase 2 structure.
11/19/03   sbs     Added LCS MOLR Completed API function
07/08/03   sbs/lt  Added support for Location Services related SS messages
06/20/03   prk     Featured out copy_fwd_to_number() to remove compiler warning.
                   Currently not used but may be used in the future.
06/04/03   RI      Modified the processing of forwarded to number information
                   in the SS confirmations.
04/28/03   AT      Lint cleanup.
04/11/03   prk     Removed references to FEATURE_SUPS.
03/13/03   RI      Updated the processing of USS indication in
                   cmsups_rpt_proc().
01/21/03   ws      Updated copyright information for 2003
11/14/02   vt      Merged changes from GW mainline.
                   [RI]Updated the processing of USSD confirmations.
05/17/02   PRK     Removed comment.
05/16/02   RI      Corrected USS problems.
05/07/02   PRK     Renamed to cmwsups.c.
04/25/02   RI      Added processing of CM_PROCESS_UNSTRUCTURED_SS_DATA_CONF.
03/11/02   RI      Modifications as per new interface definitions for
                   call independent ss messages.
                   - confirmation messages are processed based on boolean
                   flag 'success'
02/04/02   RI      Removed mn_cm_exp_t.h inlcude file.
                   - removed the IEI processing in the messages received
                   from the lower layer (MN) and setting of IEI fields
                   in the messages sent to the lower layer (MN).
01/14/02   PRK     Checked service status flag instead of service state flag
                   in function cmsups_client_cmd_check.
                   Replaced prefix cmutil_ with cm_util_.
11/28/01   RI      Commented out the password fields in activate and deactivate
                   SS requests.
11/21/01   RI      Fixed ADS compiler errors.
11/05/01   RI      Modified handling of IEI fields in SUPS commands
                   and events.
                   Added copy_ss_info_fields, copy_ss_error,
                   copy_bsg_list, copy_ff_list, copy_fwd_to_number.
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                   Made modification due to srv_state, moving from
                   phone group to serving system group (FEATURE_NEWSS).
09/25/01   RI      Initial release for WCDMA/GSM.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(766,customer.h) */
#include "mmcp_variation.h"
#include "customer.h"
#include "cmsups.h"

#ifdef CM_GW_SUPPORTED
#include "comdef.h"
#include <string.h>

#include <stringl/stringl.h>

#include "cmdbg.h"
#include "cmclient.h"
#include "cmph.h"
#include "cmmsc.h"
#include "cmmsc_sglte.h"
#include "cmutil.h"
#include "cmss.h"
#include "cmtaski.h"
#include "cmcall.h"
#include "mn_cnmtask.h"
#include "mn_cnmtask_v.h"
#include "cause.h"

#ifdef CM_DEBUG
#error code not present
#endif
#include "cmipappi.h"
#include "cmregprxi.h"

#include "cmefs.h"
#include <stringl/stringl.h>

#include "cmsups.h"
#include "cmpmprx.h"
#include "cmwsups.h"
#include "wms.h"
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


static boolean                           uss_phase1_msg = FALSE;

#define CFIS_RECORD_SIZE  16

#ifdef FEATURE_SUPS_RETRY
static void cmwsups_list_clear( 
    cm_ss_e_type ss
    /* SS for this record is required
    */
);
#endif

/*===========================================================================

FUNCTION cmsups_nv_refresh

DESCRIPTION
  Reading NV items 


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsups_nv_refresh(

   uint8             refresh_subs_mask 
    /* Subs mask whose NVs need to be refreshed
    */
 
)
{

  cmsups_s_type    *sups_ptr  =  cmsups_ptr();
      /* get the pointer to the sups object to be initialized */
  int32 read_size = 0;
  uint8 i = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*                       READ NV ITEMS                                   */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* Read the supplementary service domain pref
  */
  read_size = sizeof (sys_supp_srv_domain_pref_e_type);
  
  for(i = 0; i < MIN(ARR_SIZE(sups_ptr->sups_svc_domain_pref), cmpm_ptr()->device_mode.nSims); i++)
  {
    if(refresh_subs_mask & BM(i))
    {
      if ( cmefs_get ( CMEFS_SUPP_SRV_DOMAIN_PREF,
                     (byte*)&sups_ptr->sups_svc_domain_pref[i],
                     read_size,
                     (sys_modem_as_id_e_type)i))
      {
        CM_MSG_MED_2 ("cmipsups: sub %d supplement_service_domain_pref= %d",\
                    i, sups_ptr->sups_svc_domain_pref[i]);
      }
      else
      {
        /* set default value to AUTO */
        sups_ptr->sups_svc_domain_pref[i] = SYS_SUPP_SRV_DOMAIN_PREF_AUTO;
        CM_MSG_MED_1 ("cmipsups: sub %d set default PS sups srv domain pref AUTO", i);
      }
    }
  }
  /* Read the reject Sups command interrogate OIR/ CLIR EFS settings
  */
  for(i = 0; i < cmph_get_ph_sub_config_size(); i++)
  {
    if(refresh_subs_mask & BM(i))
    {
      if ( cmefs_get(CMEFS_REJECT_OIR_CLIR_INTERROGATION,
                     (byte*)&sups_ptr->sups_reject_oir_clir_interrogation[i],
                     sizeof(cmsups_is_reject_oir_clir_e_type),
                     (sys_modem_as_id_e_type)i)
        )
      {
       CM_MSG_HIGH_2("cmipsups: Sub %d reject_oir_clir_interrogation = %d", 
                     i, sups_ptr->sups_reject_oir_clir_interrogation[i]);
      }
      else /* NV not set */
      {
        sups_ptr->sups_reject_oir_clir_interrogation[i] = CMSUPS_SS_REJECT_NONE;
        CM_MSG_HIGH_1("cmipsups: Sub %d read failed, Using Default CMSUPS_SS_REJECT_NONE", i);
      }
    }
  }

} //cmsups_nv_refresh
/*===========================================================================

FUNCTION CMSUPS_INIT

DESCRIPTION
  Initializing the sups object.

  This function must be called before the sups object
  is being used.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsups_init( void )
{

  cmsups_s_type    *sups_ptr  =  cmsups_ptr();
      /* get the pointer to the sups object to be initialized */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sups_ptr != NULL );

  /* Verify that the object was not already initialized.
  ** and then mark object as initialized.
  */
  CM_ASSERT( ! CM_INIT_CHECK(sups_ptr->init_mask) );
  CM_INIT_MARK( sups_ptr->init_mask );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Reset all the fields of the sups object */
  sups_ptr->sups_client_ptr = CM_CLIENT_PTR_NONE;


  sups_ptr->sups_info = NULL;  
  sups_ptr->num_pending_sups_cmds = 0;

  #ifdef FEATURE_SUPS_RETRY
  sups_ptr->sups_info_list = NULL;
  sups_ptr->sups_info_list_head = NULL;

  sups_ptr->hybr_gw_sups_info = NULL; 
  sups_ptr->hybr_gw_sups_info_list = NULL;
  sups_ptr->hybr_gw_sups_info_list_head = NULL;

  sups_ptr->hybr_gw3_sups_info = NULL;
  sups_ptr->hybr_gw3_sups_info_list = NULL;
  sups_ptr->hybr_gw3_sups_info_list_head = NULL;
  #endif

  sups_ptr->uss_info = NULL;
  sups_ptr->hybr_gw_uss_info = NULL;
  sups_ptr->hybr_gw3_uss_info = NULL;

  sups_ptr->mt_uss_info.is_sups_cmd_mt_ussd_active = FALSE;
  sups_ptr->hybr_gw_mt_uss_info.is_sups_cmd_mt_ussd_active = FALSE;
  sups_ptr->hybr_gw3_mt_uss_info.is_sups_cmd_mt_ussd_active = FALSE;

  /* Reset pending flag for erase confirmation
  */
  sups_ptr->ccbs_erase_pend_indx.is_erase_cc_conf_pend = FALSE;

  /* Read EFS*/
  cmsups_nv_refresh(SYS_MODEM_AS_ID_ALL_ACTIVE_MASK);

}

/*===========================================================================

FUNCTION  cmsups_msim_resume_data

DESCRIPTION
  Resume data up processing sups reports.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmsups_msim_resume_data (
    cm_ss_e_type    cm_ss
)
{
  sys_modem_as_id_e_type  asubs_id = cmph_map_cm_ss_to_as_id(cm_ss);
  
  if (cmph_is_msim() && cmss_ptr()->ue_init_ps_data_suspend != CM_SS_NONE && 
  	  cmss_ptr()->ue_init_ps_data_suspend != CM_SS_MAX)
  {
    if(cm_ss != cmss_ptr()->ue_init_ps_data_suspend &&
	  	 asubs_id != cmph_map_cm_ss_to_as_id(cmss_ptr()->ue_init_ps_data_suspend))
    {
      if((cm_call_is_there_call_on_other_sub(asubs_id, (CM_CALL_TYPE_VOICE_MASK | CM_CALL_TYPE_EMERG_MASK))) ||
         (wms_msg_mo_sms_busy(asubs_id) == TRUE)
        )
      {
        CM_MSG_HIGH_0("Skip resume due to ongoing voice/emergency/SMS activity on non DDS SUB");
        return;
      }
      
      switch(cmss_ptr()->ue_init_ps_data_suspend)
      {
        #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
        case CM_SS_HYBR_3: 
			cmss_process_hybr_3_data_suspend_rpt(FALSE);
			break;
		#endif

		case CM_SS_HYBR_2:
			if(!cmph_is_sxlte())
			{
			  cmss_process_hybr_2_data_suspend_rpt(FALSE);
			}
			else
			{
			  cmss_process_data_suspend_rpt(FALSE);
			}
			break;

		case CM_SS_MAIN:
		default:
			cmss_process_data_suspend_rpt(FALSE);
			break;
      }
	  cmss_ptr()->ue_init_ps_data_suspend = CM_SS_NONE;
	  
    }
  }
}

/*===========================================================================

FUNCTION  cmsups_update_asubs_id

DESCRIPTION
  Update asubs_id of sups information

DEPENDENCIES
  none

RETURN VALUE
  a pointer to the sups information

SIDE EFFECTS
  none

===========================================================================*/
static void cmsups_update_asubs_id (
    sys_modem_as_id_e_type     as_id,
    cm_sups_info_s_type        **sups_info_pptr
)
{ 
  if (sups_info_pptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
    return;
  }
  
  *sups_info_pptr = cmsups_ensure_sups_info_ptr_asubs_id(as_id);
  
  if (cmph_is_msim())
  {
    (**sups_info_pptr).asubs_id = as_id;
  }
  else
  {
    (**sups_info_pptr).asubs_id = SYS_MODEM_AS_ID_1;
  }
}

/*===========================================================================

FUNCTION  cmsups_update_cfu_indicator_for_bs_code

DESCRIPTION
  Update EF-CFIS for CFWD cases when Thin-ui is enabled.
  This is in accordance with 3GPP TS 31.102, section 4.2.64 - EF CFIS

DEPENDENCIES
  SUPS info should have the appropriate information received from network

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_CM_UPDATE_EF_CFIS
static void cmsups_update_cfu_indicator_for_bs_code (

  byte           *indicator,
  byte            bs_code,
  bsg_code_type   bs_type,
  boolean         is_active
)
{
  uint8 mask = 0;

  CM_ASSERT(indicator != NULL);

  if( bs_type == BS_CODE )
  {
    mask = (bs_code == allBearerServices) ? 0x10 : 0;
  }
  else
  {
    switch(bs_code)
    {
      case allSpeechTransmissionservices:
      case telephony:
        mask = 0x01;
        break;
    
      case allFacsimileTransmissionServices:
      case facsimileGroup3AndAlterSpeech:
      case automaticFacsimileGroup3:
      case facsimileGroup4:
        mask = 0x02;
        break;
    
      case allDataTeleservices:
        mask = 0x04;
        break;
    
      case allShortMessageServices:
      case shortMessageMT_PP:
      case shortMessageMO_PP:
        mask = 0x08;
        break;
    
      case allTeleservices:
        mask = 0x0F;
        break;
    
      case allTeleservices_ExeptSMS:
        mask = 0x07;
        break;
    
      default:
        break;
    }
  }

  *indicator = is_active ? (*indicator | mask) : (*indicator & (~mask));

  CM_MSG_HIGH_6("CFU initial bs_type=%d, Indicator=%d, mask=%d, CFU isactive=%d, Indicator=%d for bs_code=%d",
          bs_type, *indicator, mask, is_active, *indicator, bs_code );

}
#endif


/*===========================================================================

FUNCTION cmsups_check_and_update_cfis

DESCRIPTION
  Update EF-CFIS for CFWD cases when Thin-ui is enabled.
  This is in accordance with 3GPP TS 31.102, section 4.2.64 - EF CFIS

DEPENDENCIES
  SUPS info should have the appropriate information received from network

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_CM_UPDATE_EF_CFIS
static void cmsups_check_and_update_cfis (

  cm_sups_info_s_type           *sups_info_ptr,
  cm_sups_event_e_type           sups_evt
)
{
  boolean num_updated=FALSE, is_active=FALSE;
  mmgsdi_session_type_enum_type  session_type = MMGSDI_GW_PROV_PRI_SESSION;
  mmgsdi_file_enum_type          file_name = MMGSDI_USIM_CFIS;
  uint8                          rec_data[CFIS_RECORD_SIZE], i=0;
  byte                           bs_code = telephony;
  bsg_code_type                  bs_type= TS_CODE;
  sys_modem_as_id_e_type    asubs_id = SYS_MODEM_AS_ID_1;
  mmgsdi_write_data_type         write_data;
  ie_forwarding_feature_list_T   *fwd_info = NULL;
  basic_service_group_list_T     *bsg_list;
  boolean                        is_EF_write_req = TRUE, is_query_or_register = FALSE;
  boolean                        is_cfu_ind_updated = TRUE;

  CM_ASSERT(sups_info_ptr != NULL);

  if (cmph_is_msim())
  {
    asubs_id = sups_info_ptr->asubs_id;
  }

  /* Check if this is one of CFU, All Call forwarding */
  if( ! ((sups_info_ptr->ss_code == (uint8) cfu) ||
         (sups_info_ptr->ss_code == (uint8) allForwardingSS)) )
  {
    CM_MSG_HIGH_1( "SS Code: %d. No need to write to EF", sups_info_ptr->ss_code );
    return;
  }

  /* Proceed further only if this has a useful conf type */
  if( (sups_info_ptr->conf_type == CC_CAUSE_INFO) ||
      (sups_info_ptr->conf_type == SS_ERROR_INFO) )
  {
    /* Error conf_types. No need to write data to EF */
    CM_MSG_HIGH_1( "Conf Type: %d. No need to write to EF", sups_info_ptr->conf_type );
    return;
  }

  /* Retrieve the session type */
  if ( cm_mmgsdi_ssn_tbl_get_type_for_asubs_id_gw(asubs_id, &session_type)!= CM_MMGSDI_SUCCESS )
  {
    CM_MSG_HIGH_1("Session Entry not found for asubs_id %d",asubs_id);
  }
  
  /* Set the bs_code */
  if(sups_info_ptr->basic_service.present)
  {
    if(sups_info_ptr->basic_service.code_type == TS_CODE)
    {
      bs_code = sups_info_ptr->basic_service.bs_code;
    }
    else
    {
      bs_code = allBearerServices;
      bs_type = BS_CODE;
    }
  }

  /* Set the default active flag based on the sups event */
  is_active = (sups_evt == CM_SUPS_EVENT_ACTIVATE_CONF) ? TRUE:FALSE;

  /* Check if this is a query or register request which normally doesn't modify the CFU status */
  is_query_or_register = ((sups_evt == CM_SUPS_EVENT_INTERROGATE_CONF) || 
                        (sups_evt == CM_SUPS_EVENT_REGISTER_CONF));


  /* Read the first CFIS record */
  if(!cmmmgsdi_read(session_type, file_name, 1, CFIS_RECORD_SIZE, rec_data, sizeof(rec_data), cyclic))
  {
    CM_MSG_HIGH_0("Read record failed for CFIS");
    return;
  }

  switch(sups_info_ptr->conf_type)
  {
    case FWD_INFO:
    case FWD_FEATURE_LIST_INFO:
      if(sups_info_ptr->fwd_feature_list.present)
      {
        fwd_info = &sups_info_ptr->fwd_feature_list;
      }
      else if(sups_info_ptr->forwarding_info.present)
      {
        fwd_info = &sups_info_ptr->forwarding_info.forwarding_feature_list;
      }
      else
      {
        CM_MSG_HIGH_0("FWD list info not present");
        is_cfu_ind_updated = FALSE;
      }

      if(fwd_info != NULL)
      {
        for (i=0; i<fwd_info->forwarding_feature_list_length; i++)
        {
          if( fwd_info->forwarding_feature[i].basic_service.present )
          {
            if(fwd_info->forwarding_feature[i].basic_service.code_type == TS_CODE)
            {
              bs_code = fwd_info->forwarding_feature[i].basic_service.bs_code;
              bs_type = TS_CODE;
            }
            else
            {
              bs_code = allBearerServices;
              bs_type = BS_CODE;
            }
          }
          if(fwd_info->forwarding_feature[i].ss_status.present)
          {
            is_active = fwd_info->forwarding_feature[i].ss_status.cm_ss_active;
          }
          else
          {
            is_active = (sups_evt == CM_SUPS_EVENT_ACTIVATE_CONF) ? TRUE:FALSE;
          }
          /* Update the CFU Indicator status  for the given basic service */
          cmsups_update_cfu_indicator_for_bs_code(&rec_data[1], bs_code,bs_type,is_active);
        
          /* Number needs to be updated only once */
          if((fwd_info->forwarding_feature[i].forwarded_to_number[0].length > 0) && (!num_updated))
          {
            rec_data[2] = fwd_info->forwarding_feature[i].forwarded_to_number[0].length;
            memscpy( &rec_data[3], ((ARR_SIZE(rec_data) - 3)*sizeof(rec_data[0])),
                     fwd_info->forwarding_feature[i].forwarded_to_number[0].data,
                      MIN(fwd_info->forwarding_feature[i].forwarded_to_number[0].length,11));
            num_updated = TRUE;
          }
        }
      }
      break;

    case SS_DATA_INFO:
      if(sups_info_ptr->ss_data.present)
      {
        if(sups_info_ptr->ss_data.cm_ss_status.present)
        {
          is_active = sups_info_ptr->ss_data.cm_ss_status.cm_ss_active;
        }
        bsg_list = &sups_info_ptr->ss_data.basic_service_group_list;
        if(bsg_list->present)
        {
          for(i=0;i < bsg_list->list_length;i++)
          {
            if( bsg_list->list[i].code_type == TS_CODE)
            {
              bs_code = bsg_list->list[i].bs_code;
              bs_type = TS_CODE;
            }
            else
            {
              bs_code = allBearerServices;
              bs_type = BS_CODE;
            }
            /* Update the CFU Indicator status  for the given basic service */
            cmsups_update_cfu_indicator_for_bs_code(&rec_data[1], bs_code,bs_type,is_active);
          }
        }
        else
        {
          is_cfu_ind_updated = FALSE;
        }
      }
      else
      {
        is_cfu_ind_updated = FALSE;
      }
      break;
 
    case SS_STATUS_INFO:
      if(sups_info_ptr->ss_status.present)
      {
        is_active = sups_info_ptr->ss_status.cm_ss_active;
        /* Update the CFU Indicator status  for the given basic service */
        cmsups_update_cfu_indicator_for_bs_code(&rec_data[1], bs_code, bs_type, is_active);
      }
      else
      {
        is_cfu_ind_updated = FALSE;
      }
      break;

    case NO_INFO:
    {
      if ( (sups_evt == CM_SUPS_EVENT_REGISTER_CONF) && (sups_info_ptr->ss_success) )
      {
        /* If network sends sups_evet registration success with conf type, NO_INFO
        ** update the EFS 
        */ 
        cmsups_update_cfu_indicator_for_bs_code(&rec_data[1], bs_code, bs_type, TRUE);
      }
      else
      {
        is_cfu_ind_updated = FALSE;
      }
      break;
    }

    default:
      CM_MSG_HIGH_1("Conf type %d not handled here",sups_info_ptr->conf_type);
      is_EF_write_req = FALSE;
      break;
  }

  if(!num_updated && sups_info_ptr->forwarded_to_number.present && 
     (sups_info_ptr->forwarded_to_number.length > 0))
  {
    rec_data[2] = sups_info_ptr->forwarded_to_number.length;
    memscpy( &rec_data[3], ((ARR_SIZE(rec_data) - 3)*sizeof(rec_data[0])),
             sups_info_ptr->forwarded_to_number.data,
             MIN(sups_info_ptr->forwarded_to_number.length,11) );
    num_updated = TRUE;
  }

  if(num_updated)
  {
    char ascii_buf[(2*CM_CALLED_PARTY_BCD_NO_LENGTH)+1] = {0};
    (void)cm_util_bcd_num_to_ascii_num(&rec_data[2],(byte *)ascii_buf, sizeof(ascii_buf));
    CM_MSG_HIGH_1("CFU num registered %s",ascii_buf);
  }

  /* If network hasn't sent any data, try to update EF with available data as we received success */
  if(!is_cfu_ind_updated)
  {
    if(is_query_or_register)
    {
      /* Dont update anything as we don't have relevant info */
      is_EF_write_req = FALSE;
    }
    else
    {
      /* This is Activate/Deactivate/Erase case and we got success.
                   Update the cfu indicator for bs_code as requested  by user */
      cmsups_update_cfu_indicator_for_bs_code(&rec_data[1], bs_code,bs_type,is_active);
    }
  }

  CM_MSG_HIGH_1( "is_EF_write_req = %d", is_EF_write_req );
  if(is_EF_write_req)
  {
    /* Data read and updated. Now write it back to SIM */
    write_data.data_len = CFIS_RECORD_SIZE;
    write_data.data_ptr = rec_data;
    
    /* Write the data back to SIM */
    if ( cmmmgsdi_write ( asubs_id, file_name, CM_MMGSDI_SUBS_TYPE_GW, write_data))
    {
      CM_MSG_HIGH_1("CFIS record written successfully for asubs_id=%d", asubs_id);
    }
    else
    {
      CM_MSG_HIGH_0("Write record failed for CFIS");
    }
  }
}/* cmsups_check_and_update_cfis */
#endif

/* <EJECT> */

/*===========================================================================

FUNCTION CMSUPS_MEM_FREE_SUPS_INFO

DESCRIPTION
  De allocates the memory held by sups_info_ptr

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsups_mem_free_sups_info(

  cmsups_s_type   *sups_ptr
    /* Pointer to sups obj to be freed */
)
{
  CM_ASSERT( sups_ptr != NULL )

  if ( sups_ptr->sups_info != NULL )
  {
    cm_mem_free ( sups_ptr->sups_info );
    sups_ptr->sups_info = NULL;
  }

  if ( sups_ptr->hybr_gw_sups_info != NULL)
  {
    cm_mem_free ( sups_ptr->hybr_gw_sups_info );
    sups_ptr->hybr_gw_sups_info = NULL;
  }


  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if ( sups_ptr->hybr_gw3_sups_info != NULL)
  {
    cm_mem_free ( sups_ptr->hybr_gw3_sups_info );
    sups_ptr->hybr_gw3_sups_info = NULL;
  }
  #endif
}

#ifdef FEATURE_SUPS_RETRY
/*===========================================================================

FUNCTION CMWSUPS_MEM_FREE_SUPS_LIST_INFO

DESCRIPTION
  De allocates the memory held by sups_info_ptr

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwsups_mem_free_sups_list_info(

  cmwsups_info_list_s_type   *list,
    /* Pointer to sups obj to be freed */

  cmwsups_info_list_s_type** list_head
  
)
{
  CM_ASSERT( list != NULL )
  CM_ASSERT(list_head != NULL)

  
  if(list->prev == NULL)
  {
    /* Set list head to next */
    *list_head = list->next;
    if(list->next != NULL)
      (list->next)->prev = NULL;
  }
  else
  {
     (list->prev)->next = list->next;
     if(list->next != NULL)
       (list->next)->prev = list->prev;
  }

  cm_mem_free(list);

  return;
}
#endif

/* <EJECT> */
/*===========================================================================

FUNCTION CMSUPS_GET_PHASE2_MSG

DESCRIPTION
  Return a pointer to Phase 2 ussd request message sent.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

static cm_process_unstructured_ss_req_T  *cmsups_get_phase2_msg( void )
{
  static cm_process_unstructured_ss_req_T  uss_phase2_msg;
  /* Process Unstructured SS request message */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return &uss_phase2_msg;
}


/* <EJECT> */

#ifdef FEATURE_SUPS_RETRY
/*===========================================================================

FUNCTION cmwsups_cmd_copy_fields_1

DESCRIPTION
  Copy the active fields of a sups command into the sups object.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmwsups_cmd_copy_fields_1(

    cmsups_s_type               *sups_ptr,
        /* Pointer to the sups object */

    const cm_sups_cmd_info_s_type    *cmd_info_ptr
        /* Pointer to a sups command */

)
{
  cm_sups_info_s_type               *sups_info_ptr = NULL;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sups_ptr != NULL )
  CM_ASSERT( cmd_info_ptr != NULL )

  if (sups_ptr == NULL || cmd_info_ptr == NULL)
    return;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if( cmph_map_subs_to_ss(cmd_info_ptr->cmd_subs_id) == CM_SS_HYBR_3 )
  {
    if (sups_ptr->hybr_gw3_sups_info == NULL)
    {
      CM_ERR_0("sups_ptr->hybr_gw3_sups_info = NULL");      
    } 
    else
    {
      sups_info_ptr = sups_ptr->hybr_gw3_sups_info;

    *(&(sups_ptr->hybr_gw3_sups_info_list->sups_cmd_info)) = *cmd_info_ptr;

      sups_ptr->sups_client_ptr = sups_ptr->hybr_gw3_sups_info_list->sups_client_ptr;
      sups_info_ptr->cc_result_alpha = sups_ptr->hybr_gw3_sups_info_list->cc_result_alpha;
      sups_info_ptr->mod_ss_params.call_control_result = sups_ptr->hybr_gw3_sups_info_list->cc_result;
    }
  }
  else if( cmph_map_subs_to_ss(cmd_info_ptr->cmd_subs_id) == CM_SS_HYBR_2 )
  {
    if (sups_ptr->hybr_gw_sups_info == NULL)
    {
      CM_ERR_0("sups_ptr->hybr_gw_sups_info = NULL");      
    } 
    else
    {
      sups_info_ptr = sups_ptr->hybr_gw_sups_info;

    *(&(sups_ptr->hybr_gw_sups_info_list->sups_cmd_info)) = *cmd_info_ptr;

      sups_ptr->sups_client_ptr = sups_ptr->hybr_gw_sups_info_list->sups_client_ptr;
      sups_info_ptr->cc_result_alpha = sups_ptr->hybr_gw_sups_info_list->cc_result_alpha;
      sups_info_ptr->mod_ss_params.call_control_result = sups_ptr->hybr_gw_sups_info_list->cc_result;
    }
  }
  else
  {
    if (sups_ptr->sups_info == NULL)
    {
      CM_ERR_0("sups_ptr->sups_info = NULL");
    } 
    else 
    {
    sups_info_ptr = sups_ptr->sups_info;

    *(&(sups_ptr->sups_info_list->sups_cmd_info)) = *cmd_info_ptr;

      sups_ptr->sups_client_ptr = sups_ptr->sups_info_list->sups_client_ptr;
      sups_info_ptr->cc_result_alpha = sups_ptr->sups_info_list->cc_result_alpha;
      sups_info_ptr->mod_ss_params.call_control_result = sups_ptr->sups_info_list->cc_result;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (sups_info_ptr == NULL)	 	 
  {
    CM_ERR_0("sups_info_ptr = NULL");	 	 
  }	 	 
  else	 	 
  {	 	 
    cmsups_cmd_copy_fields_into_info (sups_info_ptr, cmd_info_ptr);
  }

}

/*===========================================================================

FUNCTION CMWSUPS_INFO_LIST_ALLOC

DESCRIPTION
  Allocate a sups info buffer if corresponding buffer is not found for the given ss_ref and 
  return the found / allocated buffer.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated sups info struct.

SIDE EFFECTS
  none

===========================================================================*/
static cmwsups_info_list_s_type  *cmwsups_info_list_find_alloc( 
    cm_ss_e_type ss,
    /* SS for this record is required
    */
    
    uint8 ref
    /* ss_ref or invoke_id, depending on sups_cmd_type.
    ** ss_ref for MO and Invote id for MT
    */
)
{
  cmwsups_info_list_s_type  *list_ptr, *prev_list_ptr;
  dword                       uptime              = time_get_uptime_secs();
  
  #if defined FEATURE_MMODE_TRIPLE_SIM || defined FEATURE_MMODE_SXLTE_G
  if( ss == CM_SS_HYBR_3 )
  {

    list_ptr = cmsups_ptr()->hybr_gw3_sups_info_list_head;
  }
  else
  #endif
  #if defined FEATURE_MMODE_DUAL_SIM
  if( ss == CM_SS_HYBR_2 )
  {

    list_ptr = cmsups_ptr()->hybr_gw_sups_info_list_head;
  }
  else
  #else
  SYS_ARG_NOT_USED(ss);
  #endif
  {
    list_ptr = cmsups_ptr()->sups_info_list_head;
  }

  /* Parse till entry found with ss_ref or end of list */
  prev_list_ptr = list_ptr;
  while(list_ptr != NULL)
  {
    if(list_ptr->sups_cmd_info.ss_ref == ref)
    {
      CM_MSG_HIGH_1("SUPS record found, ss_ref %d",
                          list_ptr->sups_cmd_info.ss_ref);
      
      return (list_ptr);
    }
    prev_list_ptr = list_ptr;
    list_ptr = list_ptr->next;
  }

  /* No entry found with ss_ref. Allocate now */
  if( (list_ptr = (cmwsups_info_list_s_type *)cm_mem_malloc(
                                          sizeof(cmwsups_info_list_s_type))) == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    list_ptr->next = NULL;
    list_ptr->prev = prev_list_ptr;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Initialize sups cmd info
    */
    memset((void*)&(list_ptr->sups_cmd_info),0,sizeof(cm_sups_cmd_info_s_type));
    
    CM_MSG_HIGH_1("New SUPS record ss ref %d", ref);
    list_ptr->sups_cmd_info.ss_ref = ref;
    list_ptr->next_orig_uptime = 0;
    list_ptr->sr_start_uptime = uptime;
  }

  if(prev_list_ptr == NULL)
  {
     /* If first entry is created, set the list head */
     #if defined FEATURE_MMODE_TRIPLE_SIM || defined FEATURE_MMODE_SXLTE_G
     if( ss == CM_SS_HYBR_3 )
     {
   
       cmsups_ptr()->hybr_gw3_sups_info_list_head = list_ptr;
       
     }
     else
     #endif
     #if defined FEATURE_MMODE_DUAL_SIM
     if( ss == CM_SS_HYBR_2 )
     {
   
       cmsups_ptr()->hybr_gw_sups_info_list_head = list_ptr;
       
     }
     else
     #endif
     {
       cmsups_ptr()->sups_info_list_head= list_ptr;
     }
  }
  else
  {
    prev_list_ptr->next = list_ptr;
  }

  return (list_ptr);

}

/*===========================================================================

FUNCTION CMWSUPS_INFO_LIST_ALLOC

DESCRIPTION
  Allocate a sups info buffer if corresponding buffer is not found for the given ss_ref and 
  return the found / allocated buffer.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated sups info struct.

SIDE EFFECTS
  none

===========================================================================*/
static cmwsups_info_list_s_type  *cmwsups_info_list_find( 
    cm_ss_e_type ss,
    /* SS for this record is required
    */
    
    uint8 ref
    /* ss_ref or invoke_id, depending on sups_cmd_type.
    ** ss_ref for MO and Invote id for MT
    */
)
{
  cmwsups_info_list_s_type  *list_ptr;
  /* Removed the prev_list_ptr which stores the previous list_ptr
     value at points marked with #------# marked points */
  
  #if defined FEATURE_MMODE_TRIPLE_SIM || defined FEATURE_MMODE_SXLTE_G
  if( ss == CM_SS_HYBR_3 )
  {

    list_ptr = cmsups_ptr()->hybr_gw3_sups_info_list_head;
  }
  else
  #endif
  #if defined FEATURE_MMODE_DUAL_SIM
  if( ss == CM_SS_HYBR_2 )
  {

    list_ptr = cmsups_ptr()->hybr_gw_sups_info_list_head;
  }
  else
  #else
  SYS_ARG_NOT_USED(ss);
  #endif
  {
    list_ptr = cmsups_ptr()->sups_info_list_head;
  }

  /* Parse till entry found with ss_ref or end of list */

  /* #------# */
  while(list_ptr != NULL)
  {
    if(list_ptr->sups_cmd_info.ss_ref == ref)
    {
      CM_MSG_HIGH_1("SUPS record found, ss_ref %d",
                          list_ptr->sups_cmd_info.ss_ref);
      
      return (list_ptr);
    }
    /* #------# */
    list_ptr = list_ptr->next;
  }

  CM_MSG_HIGH_1("SUPS record NOT found for ss_ref %d",
                          ref);
  return NULL;
}
#endif

/*===========================================================================

FUNCTION CMSUPS_CLIENT_CMD_ERR

DESCRIPTION
  Notify clients of a specified SUPS command error.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmsups_client_cmd_err(

    const cm_sups_cmd_s_type       *sups_cmd_ptr,
        /* Pointer to a CM command */

    cm_sups_cmd_err_e_type    cmd_err
        /* Indicate the sups command error */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sups_cmd_ptr != NULL );


  /* verify the command type */
  CM_ASSERT( sups_cmd_ptr->cmd_type  == CM_CMD_TYPE_SUPS );

  CM_ASSERT( BETWEEN( cmd_err, CM_SUPS_CMD_ERR_NONE, CM_SUPS_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If commanding client supplied a callback function,
     notify this client of command error status.
  */
  if( sups_cmd_ptr->is_modem_cc_api == TRUE )
  {
    /* With feature defined, for these commands, 
    ** only new callback is supported
    */
    if(sups_cmd_ptr->cmd_cc_cb_func != NULL)
    {
      sups_cmd_ptr->cmd_cc_cb_func( sups_cmd_ptr->data_block_ptr,
                                    sups_cmd_ptr->cmd,
                                    cmd_err,
                                    sups_cmd_ptr->cmd_info.cmd_err_cause,
                                    sups_cmd_ptr->cc_result_alpha);
    }
  }
  else
  {
    if( sups_cmd_ptr->cmd_cb_func != NULL )
    {
      sups_cmd_ptr->cmd_cb_func( sups_cmd_ptr->data_block_ptr,
                                 sups_cmd_ptr->cmd,
                                 cmd_err );
    }
  }


   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If error status is other than NO ERROR,
     notify the client list of command error, as well.
  */
  if( cmd_err != CM_SUPS_CMD_ERR_NOERR )
  {
    CM_MSG_HIGH_3( "sups cmd err, cmd=%d, err=%d, client=%ld",
             sups_cmd_ptr->cmd, cmd_err, sups_cmd_ptr->client_id );
    cmclient_list_sups_cmd_err_ntfy( cmd_err, sups_cmd_ptr );
  }

}

/*===========================================================================
 
 FUNCTION is_uss_cmd_already_present
 
 DESCRIPTION
   Notify clients of a specified SUPS command error.
 
 DEPENDENCIES
   SUPS object must have already been initialized with
   cmsups_init().
 
 RETURN VALUE
   SS reference of the existing USS command
   0 if not found
 
 SIDE EFFECTS
   none
 
 ===========================================================================*/
 static uint8 cmwsups_is_uss_cmd_already_present(cm_ss_e_type  ss)
 {
 
   cmsups_s_type          *sups_ptr         = cmsups_ptr();
   cmwsups_info_list_s_type *sups_list   = NULL;
   uint8                  uss_cmd = 0;
   /* Get the sups command pointer */
   
   #ifdef FEATURE_SUPS_RETRY
 
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   
   #if defined FEATURE_MMODE_TRIPLE_SIM || defined FEATURE_MMODE_SXLTE_G
   if (ss == CM_SS_HYBR_3)
   {
     sups_list = sups_ptr->hybr_gw3_sups_info_list_head;
   }
   else
   #endif
   #if defined FEATURE_MMODE_DUAL_SIM
   if (ss == CM_SS_HYBR_2)
   {
     sups_list = sups_ptr->hybr_gw_sups_info_list_head;
   }
   else
   #endif
   {
     sups_list = sups_ptr->sups_info_list_head;
   }
 	
   if(sups_list != NULL)
   {
     while(sups_list != NULL)
     {
       if (sups_list->cmd == CM_SUPS_CMD_PROCESS_USS)
       {
         uss_cmd = sups_list->sups_cmd_info.ss_ref;
         break;
       }
       sups_list = sups_list->next; /* Move to next record */
     }
   }
   
   #endif
   
   return uss_cmd;
 
 }

static boolean cmsups_is_wlan_sups_cmd(cm_sups_cmd_e_type cmd)
{
#ifdef FEATURE_IP_CALL
  /*USSD go with voice RAT*/
  if(cmipsups_is_ussd_sups_cmd(cmd))
  {
    if(cmipapp_find_ussd_support_app() != CMIPAPP_ID_INVALID &&
      cmipapp_find_active_app_sys_mode(CM_CALL_TYPE_VOICE) == SYS_SYS_MODE_WLAN)
    {
      return TRUE;
    }
  }
  /*IMS do cmipapp_cap_reg for sups command over UT, not for USSD*/
  else
  {
    if(cmipapp_find_active_app(SYS_SYS_MODE_WLAN, CM_CALL_TYPE_SUPS) != CMIPAPP_ID_INVALID)
    {
      return TRUE;
    }
  }
#endif
  return FALSE;
}
/*===========================================================================

FUNCTION  cmsups_client_cmd_check

DESCRIPTION
  Check for sups command parameter errors and whether a specified command
  is allowed in the current state of the call/phone.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  CM_SUPS_CMD_ERR_NONE if command is allowed in the current state
  of the call/phone, specific cm_sups_cmd_err_e_type otherwise.

SIDE EFFECTS
  None

===========================================================================*/
static cm_sups_cmd_err_e_type  cmsups_client_cmd_check(

    cm_sups_cmd_s_type          *sups_cmd_ptr
        /* Pointer to a CM command */
)
{

  cm_sups_cmd_err_e_type   cmd_err       = CM_SUPS_CMD_ERR_NOERR;
      /* Initialize command error to NO-ERROR */

  cmph_s_type              *ph_ptr = cmph_ptr();
      /* Pointer to the phone object */

  cmss_s_type              *ss_ptr = cmss_ptr();
      /* Pointer to the serving system object */

  cm_ss_e_type             ss = CM_SS_MAIN;
  sys_modem_as_id_e_type   asubs_id = sups_cmd_ptr->cmd_info.cmd_subs_id;

  sd_ss_mode_pref_e_type    comm_mode=SD_SS_MODE_PREF_NONE;
  sd_ss_band_pref_e_type    comm_band=SD_SS_BAND_PREF_NONE;
  sd_ss_band_pref_e_type    comm_tds_band=SD_SS_BAND_PREF_NONE;
  sys_lte_band_mask_e_type  comm_lte_band = SYS_LTE_BAND_MASK_CONST_NONE;
  sys_srv_status_e_type      srv_status;
  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  CM_ASSERT( sups_cmd_ptr != NULL );

  CM_ASSERT( sups_cmd_ptr->cmd_type == CM_CMD_TYPE_SUPS );

  CM_ASSERT( ph_ptr != NULL);

  CM_ASSERT( ss_ptr != NULL);

  CM_MSG_HIGH_2( "cmsups_cmd_check, cmd=%d, subs_id=%d", 
                           sups_cmd_ptr->cmd, asubs_id );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* check cmd is already in error or not */
  if (sups_cmd_ptr->cmd_info.cmd_err != CM_SUPS_CMD_ERR_NOERR)
  {
     return sups_cmd_ptr->cmd_info.cmd_err;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(asubs_id == SYS_MODEM_AS_ID_NONE)
  {
    return CM_SUPS_CMD_ERR_SUBS_ID_P;
  }
  ss = cmph_map_subs_to_ss(asubs_id);

  /* Check whether the command is possible.
  */
  if((!cmsups_is_wlan_sups_cmd(sups_cmd_ptr->cmd))&&(ph_ptr->oprt_mode != SYS_OPRT_MODE_ONLINE))
  {
    /* Phone must be online to originate the command.
    */
    return CM_SUPS_CMD_ERR_OFFLINE_S;
  }


  if( sups_cmd_ptr->client_ptr == CM_CLIENT_PTR_NONE )
  {
        /* Invalid Client ID */
    cmd_err  = CM_SUPS_CMD_ERR_CLIENT_ID_P;

    CM_ERR_0( "CM_SUPS_CMD_ERR_CLIENT_ID_P");
  }
  else if ((sups_cmd_ptr->client_ptr)->init_mask != CM_INIT_MASK)
  {
    cmd_err = CM_SUPS_CMD_ERR_CLIENT_ID_P;

    CM_ERR_0( "CM_SUPS_CMD_ERR_CLIENT_ID_P");
  }

  if(cmph_is_lte_capable_on_sub(asubs_id))
  {
    #if defined FEATURE_IP_CALL
    /* Check sups command route */
    cmsups_ptr()->sups_cmd_route = cmipsups_route_check(sups_cmd_ptr->cmd, asubs_id);

    /*As of now, REG PWD isn't supported over IMS. So instead of sending it to IMS, reject here itself*/
    if((cmsups_ptr()->sups_cmd_route == CMSUPS_CMD_ALLOW_IMS)&&
      (sups_cmd_ptr->cmd == CM_SUPS_CMD_REG_PASSWORD))
    {
      CM_MSG_HIGH_0("ROUTE_FAIL for SUPS_REG_PWD on IMS");
      return CM_SUPS_CMD_ERR_ROUTE_FAILURE;
    }

    /* Basically, first check whether cmd type is IMS applicable USSD code */
    if(cmipsups_is_ussd_sups_cmd(sups_cmd_ptr->cmd))
    {
      /*It means that VoLTE (Normal/Emergency) Call is up*/
      if(cmsups_ptr()->sups_cmd_route == CMSUPS_CMD_REJECT)
      {
        return CM_SUPS_CMD_ERR_USSD_REJECTED;
      }
    }
    else /* non USSD Supplimenraty Services */
    {
      /* If CS route is calculated during ongoing VOLTE call, reject all non-USSD Sups request */
      if(cmsups_ptr()->sups_cmd_route == CMSUPS_CMD_ALLOW_CS
        && cmcall_is_present_ip_call()!= CM_CALL_ID_INVALID)
      {
        return CM_SUPS_CMD_ERR_REJECT_CS_ON_VOLTE;
      }

      if(cmsups_ptr()->sups_cmd_route == CMSUPS_CMD_REJECT)
      {
        cmd_err = CM_SUPS_CMD_ERR_ROUTE_FAILURE;
        CM_MSG_HIGH_1("CM_SUPS_CMD_ERR_ROUTE_FAILURE %d",cmsups_ptr()->sups_cmd_route);
      }
    }
    #else
    cmsups_ptr()->sups_cmd_route = CMSUPS_CMD_ALLOW_CS;
    #endif
  }
  else
  {
    cmsups_ptr()->sups_cmd_route = CMSUPS_CMD_ALLOW_CS;
  }
  CM_MSG_HIGH_2("sups_cmd=%d, route=%d",sups_cmd_ptr->cmd, cmsups_ptr()->sups_cmd_route);

  {
    /*skip service status checking for sups command over IMS(Ut interface), not including USSD*/
    if( ss == CM_SS_HYBR_2 )
    {
      srv_status = ss_ptr->info.gw_srv_status;
      cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_HYBR_2);
    }
    else
    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    if( ss == CM_SS_HYBR_3 )
    {
      srv_status = ss_ptr->info.gw3_srv_status;
      cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_HYBR_3);
    }
    else
    #endif
    {
      srv_status = ss_ptr->info.srv_status;
      cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_MAIN);
    }

    if(( (srv_status != SYS_SRV_STATUS_SRV ) || \
        (cmregprx_stack_info_ptr->state != CMREGPRX_STATE_ACTIVE)||
        (cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_STOP_CNF)) && 
        /*for wlan UT interface or USSD over wlan, no service status checking*/
        (!cmsups_is_wlan_sups_cmd(sups_cmd_ptr->cmd))
      )
    {
      CM_MSG_HIGH_3("srv_status=%d, cmregprx->state=%d,substate=%d",srv_status, cmregprx_stack_info_ptr->state,
                   cmregprx_stack_info_ptr->substate);
      /* If sups command is location notification and service is limited,
      ** do not return error.
      */
      if (!(sups_cmd_ptr->cmd == CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES 
            &&
            srv_status == SYS_SRV_STATUS_LIMITED
           )
         )
      {
        return CM_SUPS_CMD_ERR_SRV_STATE_S;
      }
    }
  }
  /* return error for processing MO supplementry services when the mode preference is LTE only and the domain is CS_PS     
  */
  /* get mode and band capabilities of PM policy via state machine.  */
  cmph_get_comm_mode_band_capab_with_pm(&comm_mode, &comm_band, &comm_lte_band, &comm_tds_band, asubs_id);

  if((cmsups_ptr()->sups_cmd_route == CMSUPS_CMD_ALLOW_CS) &&
       (((comm_mode & SD_SS_MODE_PREF_TDS_GSM_WCDMA ) == SD_SS_MODE_PREF_NONE )||
             cmcall_is_lte_only_mode(cmph_map_sd_mode_pref_to_cm_mode_pref(comm_mode),\
             cmph_map_sd_band_pref_to_cm_band_pref(comm_band),\
             comm_lte_band,\
             cmph_map_sd_tds_band_pref_to_cm_tds_band_pref(comm_tds_band))))
  {
    CM_MSG_HIGH_3("sups_cmd rejected comm_mode=%d,comm_band=0x%08x %08x, comm_lte_bands=",\
                   comm_mode,QWORD_HIGH(comm_band),QWORD_LOW(comm_band));
    cm_print_lte_band_mask(comm_lte_band);
    return CM_SUPS_CMD_ERR_SRV_STATE_S;
  }


  return cmd_err;
}


/*===========================================================================

FUNCTION CMSUPS_USSD_PHASE1_REQ

DESCRIPTION
  Send a phase1 USSD message

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmsups_ussd_phase1_req(

    cm_ss_e_type      ss
      /* ss on which the req needs to be sent */
)
{

  mn_cnm_cmd_type            *mn_cmd_ptr;
    /* MN command pointer */

  cm_sups_info_s_type        *sups_info_ptr;
      /* Pointer to the sups information */

  byte                        ascii_string [MAX_USS_CHAR_ASCII];
      /* Storage for character string converted from 7 bit
      **
      ** CM api does not allow more than MAX_USS_CHAR_ASCII size of ussd data.
      ** CM always receives ussd in ascii and performs encoding
      ** on them. Ascii string converted out of a 7bit encoded string cannot
      ** be more than MAX_USS_CHAR_ASCII.
      */

  cm_process_unstructured_ss_req_T *uss_phase2_msg_ptr = cmsups_get_phase2_msg ();
    /* Buffered phase 2 request */

  cm_process_unstructured_ss_data_T *mn_ss_data_ptr    = NULL;
    /* Points to the SS data being sent to MN */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sups_info_ptr = cmsups_get_sups_info_ptr(ss);

  /* allocate a buffer */
  mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_PROCESS_UNSTRUCTURED_SS_DATA);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy command information fields. */
  mn_ss_data_ptr = &mn_cmd_ptr->cmd.cm_process_unstructured_ss_data;

  /* ss reference */
  mn_ss_data_ptr->cm_ss_ref            = sups_info_ptr->ss_ref;
  mn_ss_data_ptr->ss_User_Data.present = uss_phase2_msg_ptr->uss_data.present;

  /*lint -save -e641 Converting enum to int */
  if (uss_phase2_msg_ptr->uss_data.present)
  {
    /* ussd phase1 request accepts only IA-5 string. This is same as ASCII.
    ** Convert 7bit encoded data into 8-bit, for all other encodings copy
    ** data as it is.
    */
    if(((uss_phase2_msg_ptr->uss_data.uss_data_coding_scheme &
                           CM_USSD_ALPHABET_MASK) == CM_USSD_DCS_7_BIT)
       ||
       ((uss_phase2_msg_ptr->uss_data.uss_data_coding_scheme &
                           CM_USSD_ALPHABET_MASK) == CM_USSD_DCS_UNSPECIFIED)
      )
    {

      /* Ascii buffer should be of size ((7-bit_buffer * 8/7) + 1 ) bytes
      */
      mn_ss_data_ptr->ss_User_Data.size
       = cm_util_gsm_alphabet_to_ascii_internal ( ascii_string,sizeof(ascii_string),
                                         uss_phase2_msg_ptr->uss_data.ussData,
                                         uss_phase2_msg_ptr->uss_data.size );

      if (mn_ss_data_ptr->ss_User_Data.size <= MAX_SS_USER_DATA_CHAR )
      {
        if (strlcpy( mn_ss_data_ptr->ss_User_Data.ss_UserData,
                      (char*)ascii_string,
                     sizeof(mn_ss_data_ptr->ss_User_Data.ss_UserData))
                     >= sizeof(mn_ss_data_ptr->ss_User_Data.ss_UserData)) 
        {
          CM_ERR_0("Wrong ascii_string received");
        }
      }
      else
      {
        CM_ERR_1 (" ss_user_Data size exceeds MAX_SS_USER_DATA_CHAR %d",
                  mn_ss_data_ptr->ss_User_Data.size);
      }

    }
    else
    {
      mn_ss_data_ptr->ss_User_Data.size = uss_phase2_msg_ptr->uss_data.size;

      memscpy(mn_ss_data_ptr->ss_User_Data.ss_UserData,
         sizeof(mn_ss_data_ptr->ss_User_Data.ss_UserData),
         uss_phase2_msg_ptr->uss_data.ussData,
         MIN(sizeof(uss_phase2_msg_ptr->uss_data.ussData),uss_phase2_msg_ptr->uss_data.size));

    }

  }
  else
  {
    CM_ERR_0 (" uss_phase2_msg not present to send phase1 ussd");
  }
  /*lint -restore -e641 Converting enum to int */

  mn_cmd_ptr->cmd.cm_process_unstructured_ss_data.as_id  
    = cmph_determine_as_id_to_ll_per_ss(ss);

  /* send the command to MN */
  cm_mn_send_cmd(mn_cmd_ptr);

}


#ifdef FEATURE_SUPS_RETRY
/*===========================================================================

FUNCTION cmsups_client_cmd_resend

DESCRIPTION
  Resends a client sups commands to MN.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  TRUE if CM needs to wait for a replay from MN before continuing
  processing this command, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static void  cmsups_client_cmd_resend(

    cmwsups_info_list_s_type  *sups_info_list_ptr
            /* Pointer to the sups object */

    
)
{
  mn_cnm_cmd_type         *mn_cmd_ptr;
    /* MN command pointer */

  uint8                    num_bytes = 0;
    /* Number of actual bytes required for USSD data */

  cm_ss_e_type             ss                  = CM_SS_MAIN;

  cm_process_unstructured_ss_req_T *uss_phase2_msg_ptr = cmsups_get_phase2_msg();
    /* Points to storage for phase2 msg */

  cmsups_s_type *sups_ptr = cmsups_ptr();

  cm_sups_cmd_info_s_type  *sups_cmd_info_ptr = NULL;
  byte                            calculated_size = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sups_info_list_ptr != NULL );
  CM_ASSERT( sups_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get SS for given asubs_id */
  ss = cmph_map_subs_to_ss(sups_info_list_ptr->sups_cmd_info.cmd_subs_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_MMODE_SXLTE_G) || defined(FEATURE_MMODE_TRIPLE_SIM)
  if( ss == CM_SS_HYBR_3 )
  {
    /* Allocate memory for sups_info_ptr */
    if (sups_ptr->hybr_gw3_sups_info == NULL)
    {
      sups_ptr->hybr_gw3_sups_info = cmsups_info_alloc(); 
      cmsups_init_sups_info(sups_ptr->hybr_gw3_sups_info);
    }
    sups_ptr->hybr_gw3_sups_info->asubs_id = cmph_ptr()->hybr_3_stack_info.asubs_id;

    sups_ptr->hybr_gw3_sups_info_list = sups_info_list_ptr;
    
  }
  else 
  #endif
  if( ss == CM_SS_HYBR_2 )
  {
    /* Allocate memory for sups_info_ptr */
    if (sups_ptr->hybr_gw_sups_info == NULL)
    {
      sups_ptr->hybr_gw_sups_info = cmsups_info_alloc(); 
      cmsups_init_sups_info(sups_ptr->hybr_gw_sups_info);
    }
    sups_ptr->hybr_gw_sups_info->asubs_id = cmph_ptr()->hybr_2_stack_info.asubs_id;

    sups_ptr->hybr_gw_sups_info_list = sups_info_list_ptr;
    
  }
  else
  {
    /* Allocate memory for sups_info_ptr */
    if (sups_ptr->sups_info == NULL)
    {
      sups_ptr->sups_info = cmsups_info_alloc(); 
      cmsups_init_sups_info(sups_ptr->sups_info);  
    }
    sups_ptr->sups_info->asubs_id = cmph_ptr()->main_stack_info.asubs_id;

    sups_ptr->sups_info_list = sups_info_list_ptr;
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  sups_cmd_info_ptr    = &(sups_info_list_ptr->sups_cmd_info);
  
  switch( sups_info_list_ptr->cmd )
  {
                     /*-------------------------*/
                     /*    RegisterSS request   */
                     /*-------------------------*/

  case CM_SUPS_CMD_REGISTER:

    /* allocate a buffer */
    mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_REGISTER_SS_REQ );

    /* ss reference */
    mn_cmd_ptr->cmd.cm_register_ss_req.cm_ss_ref = sups_cmd_info_ptr->ss_ref;

    /* ss code */
    mn_cmd_ptr->cmd.cm_register_ss_req.ss_code = sups_cmd_info_ptr->ss_code;

    /* Copy command information fields. */

    mn_cmd_ptr->cmd.cm_register_ss_req.basic_service = sups_cmd_info_ptr->basic_service;

    mn_cmd_ptr->cmd.cm_register_ss_req.forwarded_to_number =
                sups_cmd_info_ptr->forwarded_to_number;

    mn_cmd_ptr->cmd.cm_register_ss_req.forwarded_to_subaddress =
                             sups_cmd_info_ptr->forwarded_to_subaddress;

    mn_cmd_ptr->cmd.cm_register_ss_req.cm_no_reply_condition_timer =
                sups_cmd_info_ptr->nr_timer;

    mn_cmd_ptr->cmd.cm_register_ss_req.as_id  = sups_cmd_info_ptr->cmd_subs_id;

    /* send the command to MN */
    cm_mn_send_cmd(mn_cmd_ptr);

    /* save a copy in the sups object and let the clients know */

    cmwsups_cmd_copy_fields_1( sups_ptr, sups_cmd_info_ptr );

    cmsups_event (ss, CM_SUPS_EVENT_REGISTER);

    break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                    /*-------------------------*/
                    /*    EraseSS request      */
                    /*-------------------------*/

  case CM_SUPS_CMD_ERASE:

    /* allocate a buffer */
    mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_ERASE_SS_REQ);

    mn_cmd_ptr->cmd.cm_erase_ss_req.cm_ss_ref = sups_cmd_info_ptr->ss_ref;

    mn_cmd_ptr->cmd.cm_erase_ss_req.ss_code = sups_cmd_info_ptr->ss_code;

    mn_cmd_ptr->cmd.cm_erase_ss_req.basic_service =
                          sups_cmd_info_ptr->basic_service;

    mn_cmd_ptr->cmd.cm_erase_ss_req.as_id  = sups_cmd_info_ptr->cmd_subs_id;

    /* send the command to MN */
    cm_mn_send_cmd(mn_cmd_ptr);

    /* save a copy in the sups object and let the clients know */
    cmwsups_cmd_copy_fields_1( sups_ptr, sups_cmd_info_ptr );

    cmsups_event (ss, CM_SUPS_EVENT_ERASE);

    break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                  /*-------------------------*/
                  /*    ActivateSS request   */
                  /*-------------------------*/

  case CM_SUPS_CMD_ACTIVATE:

    /* allocate a buffer */
    mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_ACTIVATE_SS_REQ);

    /* ss reference */
    mn_cmd_ptr->cmd.cm_activate_ss_req.cm_ss_ref = sups_cmd_info_ptr->ss_ref;

    /* ss code */
    mn_cmd_ptr->cmd.cm_activate_ss_req.ss_code = sups_cmd_info_ptr->ss_code;

    /* Copy command information fields. */
    mn_cmd_ptr->cmd.cm_activate_ss_req.basic_service =
                sups_cmd_info_ptr->basic_service;

    mn_cmd_ptr->cmd.cm_activate_ss_req.as_id  = sups_cmd_info_ptr->cmd_subs_id;

    /* send the command to MN */
    cm_mn_send_cmd(mn_cmd_ptr);

    /* save a copy in the sups object and let the clients know */
    cmwsups_cmd_copy_fields_1( sups_ptr, sups_cmd_info_ptr );

    cmsups_event (ss, CM_SUPS_EVENT_ACTIVATE);

    break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*-------------------------*/
                   /*    DeactivateSS request */
                   /*-------------------------*/

  case CM_SUPS_CMD_DEACTIVATE:

    /* allocate a buffer */
    mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_DEACTIVATE_SS_REQ);

    /* ss reference */
    mn_cmd_ptr->cmd.cm_deactivate_ss_req.cm_ss_ref =
                 sups_cmd_info_ptr->ss_ref;

    /* ss code */
    mn_cmd_ptr->cmd.cm_deactivate_ss_req.ss_code =
                 sups_cmd_info_ptr->ss_code;

    /* Copy command information fields. */

    mn_cmd_ptr->cmd.cm_deactivate_ss_req.basic_service =
                sups_cmd_info_ptr->basic_service;

    mn_cmd_ptr->cmd.cm_deactivate_ss_req.as_id  = sups_cmd_info_ptr->cmd_subs_id;

    /* send the command to MN */
    cm_mn_send_cmd(mn_cmd_ptr);

    /* save a copy in the sups object and let the clients know */
    cmwsups_cmd_copy_fields_1( sups_ptr, sups_cmd_info_ptr );

    cmsups_event (ss, CM_SUPS_EVENT_DEACTIVATE);

    break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                   /*-------------------------*/
                   /*   InterrogateSS request */
                   /*-------------------------*/

  case CM_SUPS_CMD_INTERROGATE:
    /* allocate a buffer */
    mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_INTERROGATE_SS_REQ);

    /* Copy command information fields. */

    /* ss reference */
    mn_cmd_ptr->cmd.cm_interrogate_ss_req.cm_ss_ref =
                      sups_cmd_info_ptr->ss_ref;

    /* ss code */
    mn_cmd_ptr->cmd.cm_interrogate_ss_req.ss_code =
                      sups_cmd_info_ptr->ss_code;

    mn_cmd_ptr->cmd.cm_interrogate_ss_req.basic_service =
                      sups_cmd_info_ptr->basic_service;

    mn_cmd_ptr->cmd.cm_interrogate_ss_req.as_id  = sups_cmd_info_ptr->cmd_subs_id;

    /* send the command to MN */
    cm_mn_send_cmd(mn_cmd_ptr);

    /* save a copy in the sups object and let the clients know */
    cmwsups_cmd_copy_fields_1( sups_ptr, sups_cmd_info_ptr );

    cmsups_event(ss, CM_SUPS_EVENT_INTERROGATE);

    break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                   /*-----------------------------*/
                   /*  Register Password request  */
                   /*-----------------------------*/

  case CM_SUPS_CMD_REG_PASSWORD:

    /* allocate a buffer */
    mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_REGISTER_PASSWORD_REQ);

    /* Copy command information fields. */

    /* ss reference */
    mn_cmd_ptr->cmd.cm_register_password_req.cm_ss_ref =
          sups_cmd_info_ptr->ss_ref;

    mn_cmd_ptr->cmd.cm_register_password_req.ss_code =
          sups_cmd_info_ptr->code;

    mn_cmd_ptr->cmd.cm_register_password_req.ss_password =
          sups_cmd_info_ptr->ss_password;

    mn_cmd_ptr->cmd.cm_register_password_req.ss_new_password =
          sups_cmd_info_ptr->ss_new_password;

    mn_cmd_ptr->cmd.cm_register_password_req.ss_new_password_again =
          sups_cmd_info_ptr->ss_new_password_again;

    mn_cmd_ptr->cmd.cm_register_password_req.as_id  = sups_cmd_info_ptr->cmd_subs_id;

    /* send the command to MN */
    cm_mn_send_cmd(mn_cmd_ptr);

    /* save a copy in the sups object and let the clients know */
    cmwsups_cmd_copy_fields_1( sups_ptr, sups_cmd_info_ptr );

    cmsups_event (ss, CM_SUPS_EVENT_REG_PASSWORD);

    break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                   /*-----------------------------*/
                   /*    Process Unstructured SS  */
                   /*-----------------------------*/

  case CM_SUPS_CMD_PROCESS_USS:

    /* allocate a buffer */
    mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_PROCESS_UNSTRUCTURED_SS_REQ);

    /* Copy command information fields. */

    /* ss reference */
    mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.cm_ss_ref =
      sups_cmd_info_ptr->ss_ref;

    /* ss operation */
    mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.ss_operation =
                                            sups_cmd_info_ptr->ss_operation;

    CM_MSG_HIGH_2( "CM_SUPS_CMD_PROCESS_USS Size: %d bytes, DCC: %d",
                   sups_cmd_info_ptr->uss_data.size,
                   sups_cmd_info_ptr->uss_data.uss_data_coding_scheme );

      calculated_size = sups_cmd_info_ptr->uss_data.size;

    /*lint -save -e641 Converting enum to int */
    if(((sups_cmd_info_ptr->uss_data.uss_data_coding_scheme &
                           CM_USSD_ALPHABET_MASK) == CM_USSD_DCS_7_BIT)
       ||
       ((sups_cmd_info_ptr->uss_data.uss_data_coding_scheme &
                           CM_USSD_ALPHABET_MASK) == CM_USSD_DCS_UNSPECIFIED)
      )
    {
            /* MAX_USS_CHAR is what MN can support
       */
       if (sups_cmd_info_ptr->uss_data.size > MAX_USS_CHAR_ASCII)
       {
         sups_cmd_info_ptr->uss_data.size = MAX_USS_CHAR_ASCII;
         CM_MSG_HIGH_1 ("Truncating USSD msg to %d bytes", MAX_USS_CHAR);
       }

      /* convert to the default alphabet */
      num_bytes = cm_util_ascii_to_gsm_alphabet(
            mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.uss_data.ussData,
            sups_cmd_info_ptr->uss_data.ussData,
            sups_cmd_info_ptr->uss_data.size );

       calculated_size = num_bytes;
    }
    /* Usage of CM_SUPS_CMD_PROCESS_USS has been to encode ussd data in CM
    ** and then send. EXCEPTION is being made to UCS2 where data being
    ** received is in encoded form. It is a TEMPORARY fix and needs to be
    ** changed with a new field that says if encoding has already been done.
    */
    else if(((sups_cmd_info_ptr->uss_data.uss_data_coding_scheme &
                                CM_USSD_ALPHABET_MASK) == CM_USSD_DCS_8_BIT)
             ||
            ((sups_cmd_info_ptr->uss_data.uss_data_coding_scheme &
                                 CM_USSD_ALPHABET_MASK) == CM_USSD_DCS_UCS2)
           )

    {
        if(calculated_size > MAX_USS_BYTES)
      {
          calculated_size = MAX_USS_BYTES;
        CM_MSG_HIGH_1 ("Truncating USSD msg to %d bytes", MAX_USS_BYTES);
      }

      memscpy( mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.uss_data.ussData,
               sizeof(mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.uss_data.ussData),
               sups_cmd_info_ptr->uss_data.ussData,
                  calculated_size);
    }
    else
    {
      CM_ERR_0 (" USSD Data coding scheme error");
    }
    /*lint -restore 641 Converting enum to int */

    mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.uss_data.size =
                      calculated_size;

    mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.
                                 uss_data.uss_data_coding_scheme =
                    sups_cmd_info_ptr->uss_data.uss_data_coding_scheme;

    mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.uss_data.present = TRUE;
    sups_cmd_info_ptr->uss_data.present = TRUE;

    /* also save a copy of the USSD in case there is a phase
       compatibility problem */

      memset (uss_phase2_msg_ptr, 0, sizeof(cm_process_unstructured_ss_req_T));

     /* Copy ss ref */
     uss_phase2_msg_ptr->cm_ss_ref =
                  mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.cm_ss_ref;

     /* Copy ss operation */
     uss_phase2_msg_ptr->ss_operation =
                  mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.ss_operation;


     /* Copy ss data */
     *(&(uss_phase2_msg_ptr->uss_data)) = *(&(mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.uss_data));

    mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.as_id = sups_cmd_info_ptr->cmd_subs_id;

    /* send the command to MN */
    cm_mn_send_cmd(mn_cmd_ptr);
    CM_MSG_HIGH_4( "uss data sent to clients and NAS. present:%d coding scheme: %d size: %d uss data: %s",
                  sups_cmd_info_ptr->uss_data.present,
                  sups_cmd_info_ptr->uss_data.uss_data_coding_scheme,
		  		  calculated_size, sups_cmd_info_ptr->uss_data.ussData );

    /* save a copy in the sups object and let the clients know */
    cmwsups_cmd_copy_fields_1( sups_ptr, sups_cmd_info_ptr );

    cmsups_event (ss, CM_SUPS_EVENT_PROCESS_USS);

    break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    /*-------------------------*/
                    /*    ReleaseSS request    */
                    /*-------------------------*/

  case CM_SUPS_CMD_RELEASE:

    /* allocate a buffer */
    mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_RELEASE_SS_REQ);

    /* Copy command information fields. */

    mn_cmd_ptr->cmd.cm_release_ss_req.cm_ss_ref =
          sups_cmd_info_ptr->invoke_id;

    mn_cmd_ptr->cmd.cm_release_ss_req.ss_operation =
          sups_cmd_info_ptr->ss_operation;

    mn_cmd_ptr->cmd.cm_release_ss_req.as_id  = sups_cmd_info_ptr->cmd_subs_id;

    /* send the command to MN */
    cm_mn_send_cmd(mn_cmd_ptr);

    /* save a copy in the sups object and let the clients know */
    cmwsups_cmd_copy_fields_1( sups_ptr, sups_cmd_info_ptr );

    cmsups_event (ss, CM_SUPS_EVENT_RELEASE);

    break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                     /*-------------------------*/
                     /*   Get Password response */
                     /*-------------------------*/

  case CM_SUPS_CMD_GET_PASSWORD_RES:

    /* allocate a buffer */
    mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_GET_PASSWORD_RES);

    mn_cmd_ptr->cmd.cm_get_password_res.invoke_id =
                                             sups_cmd_info_ptr->invoke_id;

    mn_cmd_ptr->cmd.cm_get_password_res.ss_password =
    sups_cmd_info_ptr->ss_password;

    mn_cmd_ptr->cmd.cm_get_password_res.as_id    = sups_cmd_info_ptr->cmd_subs_id;

    /* send the command to MN */
    cm_mn_send_cmd(mn_cmd_ptr);

    /* save a copy in the sups object and let the clients know */
    cmwsups_cmd_copy_fields_1( sups_ptr, sups_cmd_info_ptr );

    cmsups_event (ss, CM_SUPS_EVENT_GET_PASSWORD_RES);

    break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*----------------------------*/
                     /*  Unstructured SS response  */
                     /*----------------------------*/

  case CM_SUPS_CMD_USS_RES:

    /* allocate a buffer */
    mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_UNSTRUCTURED_SS_RES);

    /* Copy command information fields. */
    mn_cmd_ptr->cmd.cm_unstructured_ss_res.invoke_id =
                                              sups_cmd_info_ptr->invoke_id;

    if( sups_cmd_info_ptr->uss_res_type == CM_USS_RES_SS_ERROR )
    {
      mn_cmd_ptr->cmd.cm_unstructured_ss_res.uss_res_data.data.cm_ss_error =
              sups_cmd_info_ptr->ss_error;
          mn_cmd_ptr->cmd.cm_unstructured_ss_res.uss_res_data.data_type = SS_ERROR_INFO;
    }
    else if ( sups_cmd_info_ptr->uss_res_type == CM_USS_RES_SS_DATA )
    {
      mn_cmd_ptr->cmd.cm_unstructured_ss_res.uss_res_data.data.uss_data =
                sups_cmd_info_ptr->uss_data;

      mn_cmd_ptr->cmd.cm_unstructured_ss_res.uss_res_data.data_type = USS_DATA_INFO;
    }

    mn_cmd_ptr->cmd.cm_unstructured_ss_res.as_id   = sups_cmd_info_ptr->cmd_subs_id;

    /* send the command to MN */
    cm_mn_send_cmd(mn_cmd_ptr);

    /* save a copy in the sups object and let the clients know */
    cmwsups_cmd_copy_fields_1( sups_ptr, sups_cmd_info_ptr );

    cmsups_event (ss, CM_SUPS_EVENT_USS_RES);

    break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                    /*-----------------------------------*/
                    /*  Unstructured SS Notify response  */
                    /*-----------------------------------*/

  case CM_SUPS_CMD_USS_NOTIFY_RES:

    /* allocate a buffer */
    mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_UNSTRUCTURED_SS_NOTIFY_RES);

    /* Copy command information fields. */
    mn_cmd_ptr->cmd.cm_unstructured_ss_notify_res.invoke_id =
                                              sups_cmd_info_ptr->invoke_id;


    if( sups_cmd_info_ptr->uss_res_type == CM_USS_RES_SS_ERROR)
    {  
      mn_cmd_ptr->cmd.cm_unstructured_ss_notify_res.cm_ss_error =
                sups_cmd_info_ptr->ss_error;
    }

    mn_cmd_ptr->cmd.cm_unstructured_ss_notify_res.as_id  = sups_cmd_info_ptr->cmd_subs_id;

    /* send the command to MN */
    cm_mn_send_cmd(mn_cmd_ptr);

    /* save a copy in the sups object and let the clients know */
    cmwsups_cmd_copy_fields_1( sups_ptr, sups_cmd_info_ptr );

    cmsups_event (ss, CM_SUPS_EVENT_USS_NOTIFY_RES);

    break;

  #if (defined(FEATURE_CM_MMGPS) || defined(FEATURE_CM_LCS))
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                   /*-----------------------------*/
                   /*      lcs-MOLR request       */
                   /*-----------------------------*/

  case CM_SUPS_CMD_LCS_MOLR:

    /* allocate a buffer */
    mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_LCS_MOLR_REQ);

    /* Copy command information fields. */

    /* ss reference */
    mn_cmd_ptr->cmd.cm_lcs_molr_req.cm_ss_ref =
          sups_cmd_info_ptr->ss_ref;

    mn_cmd_ptr->cmd.cm_lcs_molr_req.molr_type =
          sups_cmd_info_ptr->molr_type;

    mn_cmd_ptr->cmd.cm_lcs_molr_req.location_method =
          sups_cmd_info_ptr->location_method;

    mn_cmd_ptr->cmd.cm_lcs_molr_req.lcs_qos =
          sups_cmd_info_ptr->lcs_qos;

    mn_cmd_ptr->cmd.cm_lcs_molr_req.external_id =
          sups_cmd_info_ptr->external_id;

    mn_cmd_ptr->cmd.cm_lcs_molr_req.mlc_number =
          sups_cmd_info_ptr->mlc_number;

    mn_cmd_ptr->cmd.cm_lcs_molr_req.gps_assistance_data =
          sups_cmd_info_ptr->gps_assistance_data;

    mn_cmd_ptr->cmd.cm_lcs_molr_req.supported_gad_shapes =
          sups_cmd_info_ptr->supported_gad_shapes;

    mn_cmd_ptr->cmd.cm_lcs_molr_req.as_id  = sups_cmd_info_ptr->cmd_subs_id;

    /* send the command to MN */
    cm_mn_send_cmd(mn_cmd_ptr);

    /* save a copy in the sups object and let the clients know */
    cmwsups_cmd_copy_fields_1( sups_ptr, sups_cmd_info_ptr );

    cmsups_event (ss, CM_SUPS_EVENT_LCS_MOLR);

    break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*-----------------------------*/
                   /*      lcs-MOLR completed     */
                   /*-----------------------------*/

  case CM_SUPS_CMD_LCS_MOLR_COMPLETED:

    /* allocate a buffer */
    mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_LCS_MOLR_COMPLETED);

    /* Copy command information fields. */

    /* ss reference */
    mn_cmd_ptr->cmd.cm_lcs_molr_completed.cm_ss_ref =
          sups_cmd_info_ptr->ss_ref;

    mn_cmd_ptr->cmd.cm_lcs_molr_completed.as_id  = sups_cmd_info_ptr->cmd_subs_id;

    /* send the command to MN */
    cm_mn_send_cmd(mn_cmd_ptr);

    /* save a copy in the sups object and let the clients know */
    cmwsups_cmd_copy_fields_1( sups_ptr, sups_cmd_info_ptr );

    cmsups_event (ss, CM_SUPS_EVENT_LCS_MOLR_COMPLETED);

    break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                    /*--------------------------------------*/
                    /*  LCS Location Notification response  */
                    /*--------------------------------------*/

  case CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES:

    /* allocate a buffer */
    mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_LCS_LOCATION_NOTIFICATION_RES);

    mn_cmd_ptr->cmd.cm_lcs_location_notification_res.invoke_id =
                                             sups_cmd_info_ptr->invoke_id;

    mn_cmd_ptr->cmd.cm_lcs_location_notification_res.location_notification_res =
    sups_cmd_info_ptr->location_notification_res;

    mn_cmd_ptr->cmd.cm_lcs_location_notification_res.as_id  = sups_cmd_info_ptr->cmd_subs_id;

    /* send the command to MN */
    cm_mn_send_cmd(mn_cmd_ptr);

    /* save a copy in the sups object and let the clients know */
    cmwsups_cmd_copy_fields_1( sups_ptr, sups_cmd_info_ptr );

    cmsups_event (ss, CM_SUPS_EVENT_LCS_LOCATION_NOTIFICATION_RES);

    break;
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                       /*-------------------------*/
                     /*    EraseCCEntry request   */
                     /*-------------------------*/

  case CM_SUPS_CMD_ERASE_CC_ENTRY:

    #if defined(FEATURE_CCBS)
    /* allocate a buffer */
    mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_ERASE_CC_ENTRY_REQ );

    /* ss reference */
    mn_cmd_ptr->cmd.cm_erase_cc_entry_req.cm_ss_ref
                                          = sups_cmd_info_ptr->ss_ref;

    /* ss code */
    mn_cmd_ptr->cmd.cm_erase_cc_entry_req.ss_code = sups_cmd_info_ptr->ss_code;

    /* Deactivate one or all ccbs indx requests
    ** CM_CCBS_INDX_INVALID - maps to deactivating all requests
    ** ccbs_index is valid from 1 to 5 (22.030 Table B.1) ,
    */
    if (sups_cmd_info_ptr->ccbs_indx == CM_CCBS_INDX_INVALID)
    {
      mn_cmd_ptr->cmd.cm_erase_cc_entry_req.ccbs_index.present
                                           = FALSE;

      CM_MSG_HIGH_0 ("ERASE_CC_ENTRY for all ccbs indexes");
    }
    else
    {
      mn_cmd_ptr->cmd.cm_erase_cc_entry_req.ccbs_index.present
                                           = TRUE;

      mn_cmd_ptr->cmd.cm_erase_cc_entry_req.ccbs_index.ccbs_Index
                                           = sups_cmd_info_ptr->ccbs_indx;

      CM_MSG_HIGH_1 ("ERASE_CC_ENTRY for ccbs index ",
                                       sups_cmd_info_ptr->ccbs_indx);
    }

    CM_MSG_HIGH_1 ("is_erase_cc_conf_pend flag = %d",
                 sups_ptr->ccbs_erase_pend_indx.is_erase_cc_conf_pend);

    /* Save the ccbs indx requested for deactivation to clear
    ** phone object ccbs indx store on confirmation
    */
    if (!sups_ptr->ccbs_erase_pend_indx.is_erase_cc_conf_pend)
    {
      sups_ptr->ccbs_erase_pend_indx.is_erase_cc_conf_pend = TRUE;
      sups_ptr->ccbs_erase_pend_indx.cm_ss_ref = sups_cmd_info_ptr->ss_ref;
      sups_ptr->ccbs_erase_pend_indx.ccbs_indx = sups_cmd_info_ptr->ccbs_indx;
    }
    else
    {
      CM_ERR_0 ("Simult ccbs deact requests arrived");
    }

    mn_cmd_ptr->cmd.cm_erase_cc_entry_req.as_id  = sups_cmd_info_ptr->cmd_subs_id;

    /* send the command to MN */
    cm_mn_send_cmd(mn_cmd_ptr);

    /* save a copy in the sups object and let the clients know */
    cmwsups_cmd_copy_fields_1( sups_ptr, sups_cmd_info_ptr );

    /*
    ** if ss_code is ccbs then deactivation gets converted into EraseCCEntry
    ** request (24.093 section 4.4).
    ** Since client sends a deactivation request, deactivate sups event is
    ** sent.
    */
    cmsups_event (ss, CM_SUPS_EVENT_DEACTIVATE);

    #endif

    break;


  default:
    CM_ERR_1( "Unknown SUPS command %d for RETRY",
             sups_info_list_ptr->cmd );
    break;


  } /* end switch(sups_cmd_ptr->cmd )... */

}

#endif

/*===========================================================================

FUNCTION cmsups_create_interrogate_cnf_internal

DESCRIPTION
return interrogate cnf if UE based call waiting is used. local NV value is returned. no need to send to 
lower layers

RETURN VALUE

SIDE EFFECTS
none

===========================================================================*/
void cmsups_create_interrogate_cnf_internal(cm_sups_cmd_info_s_type *sups_cmd_info_ptr)
{
  cm_rpt_type               *cm_rpt_ptr          = NULL;
  cm_interrogate_ss_conf_T  *interrogate_ss_conf = NULL;
  boolean                   is_ue_cw_enabled;
  sys_modem_as_id_e_type    asubs_id = sups_cmd_info_ptr->cmd_subs_id;


  is_ue_cw_enabled = cmph_get_volte_sub_info_ptr(asubs_id)->ue_based_cw.is_enabled;

  cm_rpt_ptr = cm_get_cmd_buf ();
  interrogate_ss_conf = &cm_rpt_ptr->cmd.interrogate_conf;
  cm_rpt_ptr->hdr.cmd                        = CM_INTERROGATE_SS_CONF;
  interrogate_ss_conf->as_id                 = asubs_id;
  interrogate_ss_conf->cm_ss_ref             = sups_cmd_info_ptr->ss_ref;
  interrogate_ss_conf->success               = TRUE;
  if(sups_cmd_info_ptr->basic_service.present || is_ue_cw_enabled)
  {
    interrogate_ss_conf->res_type              = BSG_LIST_INFO;
    interrogate_ss_conf->InterrogateSS_Res.basic_service_group_list.present = TRUE;
    interrogate_ss_conf->InterrogateSS_Res.basic_service_group_list.list_length = 1;
    interrogate_ss_conf->InterrogateSS_Res.basic_service_group_list.list[0].bs_code = allSpeechTransmissionservices;
    interrogate_ss_conf->InterrogateSS_Res.basic_service_group_list.list[0].code_type = TS_CODE;
	interrogate_ss_conf->InterrogateSS_Res.basic_service_group_list.list[0].present = TRUE;
  }
  else
  {
    interrogate_ss_conf->res_type              = SS_STATUS_INFO;
    interrogate_ss_conf->InterrogateSS_Res.cm_ss_status.present = TRUE;
    interrogate_ss_conf->InterrogateSS_Res.cm_ss_status.cm_ss_active = is_ue_cw_enabled;
  }
  CM_MSG_HIGH_3( "res_type=%d, ue_cw=%d, ss_ref=%d",
                 interrogate_ss_conf->res_type,
                 is_ue_cw_enabled,
                 interrogate_ss_conf->cm_ss_ref );

  cm_put_cmd (cm_rpt_ptr);

  return;
}
/*===========================================================================

FUNCTION cmsups_create_activate_cnf_internal

DESCRIPTION
activate Call waiting when UE based CW is enabled

RETURN VALUE

SIDE EFFECTS
none

===========================================================================*/
void cmsups_create_activate_cnf_internal(cm_sups_cmd_info_s_type *sups_cmd_info_ptr)
{
  cm_rpt_type *cm_rpt_ptr           = NULL;
  sys_modem_as_id_e_type asubs_id   = sups_cmd_info_ptr->cmd_subs_id;
  volte_sub_info_s_type* volte_sub_info_ptr = cmph_get_volte_sub_info_ptr(asubs_id);

  CM_MSG_HIGH_1("cmsups_create_activate_cnf_internal, sub %d", asubs_id);

  volte_sub_info_ptr->ue_based_cw.is_enabled = TRUE;
  cmefs_fs_write(CMEFS_UE_BASED_CW,
                   (byte*)&(volte_sub_info_ptr->ue_based_cw),
                   sizeof(cm_ue_based_cw_s_type),
                   asubs_id);

  cm_rpt_ptr = cm_get_cmd_buf();
  cm_rpt_ptr->hdr.cmd                              = CM_ACTIVATE_SS_CONF;
  cm_rpt_ptr->cmd.activate_conf.as_id              = asubs_id;
  cm_rpt_ptr->cmd.activate_conf.success            = TRUE;
  cm_rpt_ptr->cmd.activate_conf.cm_ss_ref          = sups_cmd_info_ptr->ss_ref;
  cm_rpt_ptr->cmd.activate_conf.ss_info.info_type  = NO_INFO;

  cm_put_cmd(cm_rpt_ptr);
  return;
}
/*===========================================================================

FUNCTION cmsups_create_deactivate_cnf_internal

DESCRIPTION
deactivate Call waiting when UE based CW is enabled

RETURN VALUE

SIDE EFFECTS
none

===========================================================================*/
void cmsups_create_deactivate_cnf_internal(cm_sups_cmd_info_s_type *sups_cmd_info_ptr)
{
  cm_rpt_type *cm_rpt_ptr           = NULL;
  sys_modem_as_id_e_type asubs_id   = sups_cmd_info_ptr->cmd_subs_id;
  volte_sub_info_s_type* volte_sub_info_ptr = cmph_get_volte_sub_info_ptr(asubs_id);

  CM_MSG_HIGH_1("cmsups_create_deactivate_cnf_internal, sub %d", asubs_id);

  volte_sub_info_ptr->ue_based_cw.is_enabled = FALSE;
  cmefs_fs_write(CMEFS_UE_BASED_CW,
                   (byte*)&(volte_sub_info_ptr->ue_based_cw),
                   sizeof(cm_ue_based_cw_s_type),
                   asubs_id);

  cm_rpt_ptr = cm_get_cmd_buf ();
  cm_rpt_ptr->hdr.cmd                              = CM_DEACTIVATE_SS_CONF;
  cm_rpt_ptr->cmd.activate_conf.as_id              = asubs_id;
  cm_rpt_ptr->cmd.activate_conf.success            = TRUE;
  cm_rpt_ptr->cmd.activate_conf.cm_ss_ref          = sups_cmd_info_ptr->ss_ref;
  cm_rpt_ptr->cmd.activate_conf.ss_info.info_type  = NO_INFO;

  cm_put_cmd (cm_rpt_ptr);
  return;
}



/*===========================================================================

FUNCTION cmsups_get_reject_interrogate_clir_oir_pref

DESCRIPTION
  Returns the sups_reject_oir_clir_interrogation for a given asubs_id

DEPENDENCIES
  None

RETURN VALUE
  cmsups_is_reject_oir_clir_e_type

SIDE EFFECTS
  None

===========================================================================*/
cmsups_is_reject_oir_clir_e_type cmsups_get_reject_interrogate_clir_oir_pref(
  sys_modem_as_id_e_type sub_id
)
{
  cmsups_s_type *sups_ptr = cmsups_ptr();

  if(sub_id <= SYS_MODEM_AS_ID_NONE || sub_id >= MAX_SIMS)
  {
    CM_MSG_HIGH_1("cmsups_get_reject_interrogate_clir_oir_pref: invalid sub_id %d", sub_id);
    sub_id = SYS_MODEM_AS_ID_1;
  }
  CM_MSG_HIGH_2( "cmsups_get_reject_interrogate_clir_oir_pref: %d, sub_id: %d",
                 sups_ptr->sups_reject_oir_clir_interrogation[sub_id], sub_id );
  return sups_ptr->sups_reject_oir_clir_interrogation[sub_id];
} /* cmsups_get_reject_interrogate_clir_oir_pref () */


/*===========================================================================

FUNCTION cmsups_reject_oir_clir_interrogate_cnf_internal

DESCRIPTION
  Reject CLIR/ OIR impersonating lower layer by sending SS error Facility Not supported

RETURN VALUE
  NULL;
SIDE EFFECTS
none

===========================================================================*/
static void cmsups_reject_oir_clir_interrogate_cnf_internal(cm_sups_cmd_info_s_type *sups_cmd_info_ptr)
{
  cm_rpt_type               *cm_rpt_ptr          = NULL;
  cm_interrogate_ss_conf_T  *interrogate_ss_conf = NULL;
  sys_modem_as_id_e_type    asubs_id = sups_cmd_info_ptr->cmd_subs_id;

  cm_rpt_ptr = cm_get_cmd_buf ();

  CM_MSG_HIGH_1("cmsups_reject_oir_clir_interrogate_cnf_internal, sub %d", asubs_id);
  
  interrogate_ss_conf = &cm_rpt_ptr->cmd.interrogate_conf;

  memset( interrogate_ss_conf, 0x00, sizeof(cm_interrogate_ss_conf_T));

  cm_rpt_ptr->hdr.cmd                        = CM_INTERROGATE_SS_CONF;
  interrogate_ss_conf->as_id                 = asubs_id;
  interrogate_ss_conf->cm_ss_ref             = sups_cmd_info_ptr->ss_ref;
  interrogate_ss_conf->success               = TRUE;

  interrogate_ss_conf->res_type              = SS_ERROR_INFO;
  
  interrogate_ss_conf->InterrogateSS_Res.cm_ss_error.present = TRUE;
  interrogate_ss_conf->InterrogateSS_Res.cm_ss_error.error_code_tag = ERROR_CODE_TAG;
  interrogate_ss_conf->InterrogateSS_Res.cm_ss_error.error_code = facilityNotSupported;
  interrogate_ss_conf->InterrogateSS_Res.cm_ss_error.error_type = NO_ERR;

  cm_put_cmd (cm_rpt_ptr);

  return;
} /* cmsups_reject_oir_clir_interrogate_cnf_internal () */


/*===========================================================================

FUNCTION cmsups_is_reject_oir_clir_interrogate_cmd

DESCRIPTION
  This function checks if Interrogate OIR/ CLIR command should be rejected 
  for further processing or not. If rejected there should be No signalling
  to the NW

RETURN VALUE
  TRUE/ FALSE based on reject value;

SIDE EFFECTS
  none

===========================================================================*/
static boolean cmsups_is_reject_oir_clir_interrogate_cmd( 
  cm_ss_e_type           ss,
  cmsups_s_type         *sups_ptr,
  cm_sups_cmd_s_type    *sups_cmd_ptr
)
{
  boolean                  do_reject_oir_clir = FALSE;
  cm_sups_cmd_info_s_type *sups_cmd_info_ptr  = SUPS_CMD_INFO_PTR( sups_cmd_ptr );

  if (sups_cmd_info_ptr->ss_code == clir && sups_cmd_ptr->cmd == CM_SUPS_CMD_INTERROGATE)
  {
    cmsups_is_reject_oir_clir_e_type is_reject_oir_clir;
    is_reject_oir_clir = cmsups_get_reject_interrogate_clir_oir_pref(sups_cmd_info_ptr->cmd_subs_id);

    switch (cmsups_ptr()->sups_cmd_route)
    {
    case CMSUPS_CMD_ALLOW_IMS:
      if (is_reject_oir_clir == CMSUPS_SS_REJECT_OIR ||
        is_reject_oir_clir == CMSUPS_SS_REJECT_OIR_CLIR)
      {
        do_reject_oir_clir = TRUE;
      }
      break;
    case CMSUPS_CMD_ALLOW_CS:
      if (is_reject_oir_clir == CMSUPS_SS_REJECT_CLIR ||
        is_reject_oir_clir == CMSUPS_SS_REJECT_OIR_CLIR)
      {
        do_reject_oir_clir = TRUE;
      }
      break;
    default:
      break;
    }
    if (do_reject_oir_clir) 
    {
      cmsups_cmd_copy_fields( sups_ptr, sups_cmd_ptr );
      cmsups_event (ss, CM_SUPS_EVENT_INTERROGATE);
      cmsups_reject_oir_clir_interrogate_cnf_internal(sups_cmd_info_ptr);  
    }
  }
  return do_reject_oir_clir;
} /* cmsups_is_reject_oir_clir_interrogate_cmd () */


/*===========================================================================

FUNCTION cmwsups_sglte_determine_ss_for_sups_cmd

DESCRIPTION
  Determine the SS for sups command in SGLTE mode

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_sups_cmd_err_e_type cmwsups_sglte_determine_ss_for_sups_cmd(

   sys_modem_as_id_e_type     asubs_id,
       /* Command Subs ID */

   cm_sups_cmd_e_type         sups_cmd,
       /* Sups Command */

   cm_ss_e_type              *domain_ss
       /* SS to originate the Sups Command */
)
{
  cmsups_cmd_route_e_type sups_cmd_route;
  sd_ss_srv_domain_pref_e_type domain    = SD_SS_SRV_DOMAIN_PREF_NONE;

  #ifdef FEATURE_IP_CALL
  sups_cmd_route = cmipsups_route_check( sups_cmd, asubs_id );

  if((sups_cmd_route == CMSUPS_CMD_REJECT) && cmipsups_is_ussd_sups_cmd(sups_cmd))
  {
    sups_cmd_route = CMSUPS_CMD_ALLOW_CS;
  }

  #else
  sups_cmd_route = CMSUPS_CMD_ALLOW_CS;
  #endif

  CM_MSG_HIGH_1( "Sups Cmd Route = %d", sups_cmd_route );

  switch ( sups_cmd_route )
  {
    case CMSUPS_CMD_ALLOW_CS:
      domain = SD_SS_SRV_DOMAIN_PREF_CS_ONLY;
      break;

    case CMSUPS_CMD_ALLOW_IMS:
      domain = SD_SS_SRV_DOMAIN_PREF_PS_ONLY;
      break;

    default:
      break;
  }

  *domain_ss = cmmsc_sglte_get_ss_for_domain( asubs_id, domain );

  if( *domain_ss == CM_SS_NONE )
  {
    CM_ERR_1( "SGLTE Sups Cmd: Invalid srv domain %d", domain );
    return CM_SUPS_CMD_ERR_SRV_STATE_S;
  }
  else
  {
    return CM_SUPS_CMD_ERR_NONE;
  }
}

/*===========================================================================

FUNCTION cmsups_client_cmd_forward_ll

DESCRIPTION
  Forward a client sups commands to MN.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  TRUE if CM needs to wait for a replay from MN before continuing
  processing this command, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
static void  cmsups_client_cmd_forward_ll(

    cmsups_s_type  *sups_ptr,
            /* Pointer to the sups object */

    cm_sups_cmd_s_type    *sups_cmd_ptr
        /* Pointer to a CM command */
)
{

  cm_sups_cmd_info_s_type *sups_cmd_info_ptr;

  sys_modem_as_id_e_type   asubs_id = sups_cmd_ptr->cmd_info.cmd_subs_id;
  cm_ss_e_type             ss = cmph_map_subs_to_ss(asubs_id);

  cm_sups_event_e_type     sups_event_type = CM_SUPS_EVENT_NONE;
  volte_sub_info_s_type*   volte_sub_info_ptr = cmph_get_volte_sub_info_ptr(asubs_id);
  boolean                  is_ue_based_cw =
              volte_sub_info_ptr->ue_based_cw.use_ue_based_cw;

  #ifdef FEATURE_MMODE_DUAL_SIM
  sys_modem_as_id_e_type   other_as_id = SYS_MODEM_AS_ID_NONE;
  
  cm_ss_e_type                 data_ss = CM_SS_MAIN;
  cmph_s_type                 *ph_ptr  = cmph_ptr();
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( sups_cmd_ptr != NULL );
  CM_ASSERT( sups_ptr != NULL );
  CM_ASSERT( sups_cmd_ptr->cmd_type == CM_CMD_TYPE_SUPS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* sups command information pointer */
  sups_cmd_info_ptr = SUPS_CMD_INFO_PTR( sups_cmd_ptr );

  if ( cmph_is_subs_feature_mode_sglte( sups_cmd_info_ptr->cmd_subs_id ) )
  {
    cm_ss_e_type domain_ss = CM_SS_NONE;

    cmwsups_sglte_determine_ss_for_sups_cmd( sups_cmd_info_ptr->cmd_subs_id,
                                             sups_cmd_ptr->cmd,
                                             &domain_ss );
    ss = domain_ss;
  }

  /* If use UE based call waiting, it needs to be returned stored value.
  ** there is no signaling to the network 
  */
  CM_MSG_HIGH_3("sups_fw_ll,is_ue_based_cw=%d, enabled=%d, ss_code=%d ", is_ue_based_cw,
                  volte_sub_info_ptr->ue_based_cw.is_enabled,
                  sups_cmd_info_ptr->ss_code);
                     
  if(is_ue_based_cw && sups_cmd_info_ptr->ss_code == cw)
  {
    switch(sups_cmd_ptr->cmd)
    {
    case CM_SUPS_CMD_ACTIVATE:
      cmsups_cmd_copy_fields( sups_ptr, sups_cmd_ptr );
      cmsups_event (ss, CM_SUPS_EVENT_ACTIVATE);
      cmsups_create_activate_cnf_internal(sups_cmd_info_ptr);
      break;
    case CM_SUPS_CMD_DEACTIVATE:
      cmsups_cmd_copy_fields( sups_ptr, sups_cmd_ptr );
      cmsups_event (ss, CM_SUPS_EVENT_DEACTIVATE);
      cmsups_create_deactivate_cnf_internal(sups_cmd_info_ptr);
      break;
    case CM_SUPS_CMD_INTERROGATE:
      cmsups_cmd_copy_fields( sups_ptr, sups_cmd_ptr );
      cmsups_event (ss, CM_SUPS_EVENT_INTERROGATE);
      cmsups_create_interrogate_cnf_internal(sups_cmd_info_ptr);
      break;
    default:
      break;
    }
    return;
  }

  /* If OIR CLIR Interrogation reject NV is set to disallow Signaling then
  ** simulate NW reject from here and no need to forward comand to Lower layers
  */
  if (cmsups_is_reject_oir_clir_interrogate_cmd(ss, sups_ptr, sups_cmd_ptr) == TRUE)
  {
    CM_MSG_HIGH_1("Reject CLIR/ OIR Interrogate command sent on ss %d", ss);
    return;
  }

  #ifdef FEATURE_MMODE_DUAL_SIM

  if (cmph_is_dsds() || cmph_is_tsts())
  {

    other_as_id = ph_ptr->default_data_subs ;
    /* Get SS for given asubs_id */
    ss = cmph_map_subs_to_ss(asubs_id);
  
    if( ss == CM_SS_NONE )
    {
      CM_ERR_1("Invalid subs id %d",asubs_id);
    }
  
     /* Get SS for given asubs_id */
    data_ss = cmph_map_subs_to_ss(other_as_id);
    
    if ((ph_ptr->internal_standby_pref == SYS_MODEM_DS_PREF_SINGLE_STANDBY)||
         (ph_ptr->default_data_subs == asubs_id))
    {
      CM_MSG_HIGH_0("Not needed to suspend data");
    }
    else if((cmcall_is_active_call_other_than_ss(ss,CM_CALL_TYPE_VOICE)!= CM_CALL_ID_INVALID ) ||
            (cmcall_is_active_call_other_than_ss(ss,CM_CALL_TYPE_EMERGENCY) != CM_CALL_ID_INVALID)||
            (cmcall_is_active_call_other_than_ss(ss,CM_CALL_TYPE_CS_DATA) != CM_CALL_ID_INVALID) 
       )
    {
      CM_MSG_HIGH_1("Voice/VT call active on other subs %d", other_as_id);
    }
    else
    {
      /* CR:449546 : If any respnse cmd is recvd from Client , no need to suspend 
                              the data on DDS 
       */
      if(!(sups_cmd_ptr->cmd == CM_SUPS_CMD_GET_PASSWORD_RES ||\
  			   sups_cmd_ptr->cmd == CM_SUPS_CMD_USS_NOTIFY_RES ||\
  			   sups_cmd_ptr->cmd == CM_SUPS_CMD_USS_RES ||\
  			   sups_cmd_ptr->cmd == CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES ||\
  			   sups_cmd_ptr->cmd == CM_SUPS_CMD_LCS_MOLR_COMPLETED))
      {
        CM_MSG_HIGH_1("Suspending data on SubsId %d", other_as_id);
        if( cmcall_get_subs_id_with_call_type(CM_CALL_TYPE_PS_DATA)
                                                  !=SYS_MODEM_AS_ID_NONE)
        {
          #if defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)
          boolean is_1x_call_active = FALSE;
          if(data_ss == CM_SS_MAIN )
          {
        
            is_1x_call_active = cmcall_end_active_1x_datacalls(TRUE);
          }
          if ((data_ss == CM_SS_HYBR_2)||(is_1x_call_active == FALSE) 
          #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
           || (data_ss == CM_SS_HYBR_3)
          #endif
          )
          #endif
          {
            #ifdef FEATURE_UE_INITIATED_DORMANCY
            sm_cmd_type *sm_ptr = cm_sm_get_buf_else_err_fatal(SMREG_PS_SIGNALING_REL_REQ);
            sm_ptr->cmd.ps_signalling_rel_req.as_id = other_as_id;
            sm_ptr->cmd.ps_signalling_rel_req.cause = SMREG_DUAL_STANDBY_CS_MO_CALL;
            cm_sm_send_cmd( sm_ptr );
            CM_MSG_HIGH_1("SMREG_PS_SIGNALING_REL_REQ sent on SubsId %d", other_as_id);
            #endif
          }
        }
        
        cmss_ptr()->ue_init_ps_data_suspend = data_ss;
    
        #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
        if(data_ss == CM_SS_HYBR_3)
        {
          cmss_process_hybr_3_data_suspend_rpt(TRUE);
        }
        else
        #endif
        if(data_ss == CM_SS_HYBR_2)
        {
          cmss_process_hybr_2_data_suspend_rpt(TRUE);
        }
        else
        {
          cmss_process_data_suspend_rpt(TRUE);
        }
      }
    }
  }
  else
  {
    /* Get SS for given asubs_id */
    ss = cmph_map_subs_to_ss(asubs_id);

    if ( cmph_is_subs_feature_mode_sglte( sups_cmd_info_ptr->cmd_subs_id ) )
    {
      cmwsups_sglte_determine_ss_for_sups_cmd( asubs_id,
                                               sups_cmd_ptr->cmd, &ss );
    }
  
    if( ss == CM_SS_NONE )
    {
      CM_ERR_1("Invalid subs id %d",asubs_id);
    }
  }
  #endif /* FEATURE_MMODE_DUAL_SIM */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If PS supp domain preference is PS_PREF and ICS interface is available, route SS
  ** command to IMS
  */
  if(cmsups_ptr()->sups_cmd_route == CMSUPS_CMD_ALLOW_IMS)
  {
    #ifdef FEATURE_IP_CALL
    cmipsups_client_cmd_forward_ip( sups_ptr, sups_cmd_ptr );
    #endif
  }
  else if(cmsups_ptr()->sups_cmd_route == CMSUPS_CMD_ALLOW_CS)
  {
    /* Route to CS domain */

    /* save a copy in the sups object and let the clients know */
    cmsups_cmd_copy_fields( sups_ptr, sups_cmd_ptr );

    /* Route to CS domain */
    switch( sups_cmd_ptr->cmd )
    {
    case CM_SUPS_CMD_LCS_MOLR:
    case CM_SUPS_CMD_LCS_MOLR_COMPLETED:
    case CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES:
      cmwsups_client_lcs_cmd_forward_gw(sups_cmd_info_ptr, sups_cmd_ptr->cmd);
      break;
    default:
      cmwsups_client_cmd_forward_gw(asubs_id, sups_cmd_ptr->cmd);
      break;
    }

    sups_event_type = cmsups_cmd_to_event_map(sups_cmd_ptr->cmd);
    if(sups_event_type != CM_SUPS_EVENT_NONE)
    {
      cmsups_event (ss, sups_event_type);
    }

  } /* end switch(sups_cmd_ptr->cmd )... */

  return;
}
/*===========================================================================

FUNCTION cmwsups_send_release_ss_req

DESCRIPTION
  Forward a SS release request to NAS with release cause

DEPENDENCIES
  NONE

RETURN VALUE
  NONE

SIDE EFFECTS
  none

===========================================================================*/
static void cmwsups_send_release_ss_req(
  byte                     invoke_id,
  byte                     ss_operation,
  ie_cm_ss_error_T         cm_ss_error,

  sys_modem_as_id_e_type   asubs_id
)
{
  mn_cnm_cmd_type   *mn_cmd_ptr;
  /* allocate a buffer */
  mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_RELEASE_SS_REQ);

  /* Copy command information fields. */
  mn_cmd_ptr->cmd.cm_release_ss_req.cm_ss_ref = invoke_id;
  mn_cmd_ptr->cmd.cm_release_ss_req.ss_operation = ss_operation;
  mn_cmd_ptr->cmd.cm_release_ss_req.as_id  = asubs_id;
  mn_cmd_ptr->cmd.cm_release_ss_req.cm_ss_error = cm_ss_error;

  /* send the command to MN */
  cm_mn_send_cmd(mn_cmd_ptr);
} /* cmwsups_send_release_ss_req () */

/*===========================================================================

FUNCTION cmwsups_client_lcs_cmd_forward_gw

DESCRIPTION
  Forward a sups commands to MN. 
  This function will be called when route is calculated as CS
  This Function will also be called when IMS sends fallback to CS.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  NONE

SIDE EFFECTS
  none

===========================================================================*/
void cmwsups_client_lcs_cmd_forward_gw(
  cm_sups_cmd_info_s_type   *sups_cmd_info_ptr,
  cm_sups_cmd_e_type              sups_cmd
)
{
  mn_cnm_cmd_type           *mn_cmd_ptr; /* MN command pointer */

  switch( sups_cmd )
  {
#if (defined(FEATURE_CM_MMGPS) || defined(FEATURE_CM_LCS))
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------*/
  /*      lcs-MOLR request       */
  /*-----------------------------*/

  case CM_SUPS_CMD_LCS_MOLR:

  /* allocate a buffer */
  mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_LCS_MOLR_REQ);

  /* Copy command information fields. */

  /* ss reference */
  mn_cmd_ptr->cmd.cm_lcs_molr_req.cm_ss_ref =
    sups_cmd_info_ptr->ss_ref;

  mn_cmd_ptr->cmd.cm_lcs_molr_req.molr_type =
    sups_cmd_info_ptr->molr_type;

  mn_cmd_ptr->cmd.cm_lcs_molr_req.location_method =
    sups_cmd_info_ptr->location_method;

  mn_cmd_ptr->cmd.cm_lcs_molr_req.lcs_qos =
    sups_cmd_info_ptr->lcs_qos;

  mn_cmd_ptr->cmd.cm_lcs_molr_req.external_id =
    sups_cmd_info_ptr->external_id;

  mn_cmd_ptr->cmd.cm_lcs_molr_req.mlc_number =
    sups_cmd_info_ptr->mlc_number;

  mn_cmd_ptr->cmd.cm_lcs_molr_req.gps_assistance_data =
    sups_cmd_info_ptr->gps_assistance_data;

  mn_cmd_ptr->cmd.cm_lcs_molr_req.supported_gad_shapes =
    sups_cmd_info_ptr->supported_gad_shapes;

  mn_cmd_ptr->cmd.cm_lcs_molr_req.as_id = sups_cmd_info_ptr->cmd_subs_id;

  break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------*/
  /*      lcs-MOLR completed     */
  /*-----------------------------*/

  case CM_SUPS_CMD_LCS_MOLR_COMPLETED:

  /* allocate a buffer */
  mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_LCS_MOLR_COMPLETED);

  /* Copy command information fields. */

  /* ss reference */
  mn_cmd_ptr->cmd.cm_lcs_molr_completed.cm_ss_ref =
    sups_cmd_info_ptr->ss_ref;

  mn_cmd_ptr->cmd.cm_lcs_molr_completed.as_id = sups_cmd_info_ptr->cmd_subs_id;

  break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*--------------------------------------*/
  /*  LCS Location Notification response  */
  /*--------------------------------------*/

  case CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES:

  /* allocate a buffer */
  mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_LCS_LOCATION_NOTIFICATION_RES);

  mn_cmd_ptr->cmd.cm_lcs_location_notification_res.invoke_id =
    sups_cmd_info_ptr->invoke_id;

  mn_cmd_ptr->cmd.cm_lcs_location_notification_res.location_notification_res =
    sups_cmd_info_ptr->location_notification_res;

  mn_cmd_ptr->cmd.cm_lcs_location_notification_res.as_id = sups_cmd_info_ptr->cmd_subs_id;

  break;
#endif
  default:
  CM_MSG_HIGH_1("Unknown SUPS command %d received from client", sups_cmd);
  return;

  }

  /* send the command to MN */
  cm_mn_send_cmd(mn_cmd_ptr);

} /* cmwsups_client_lcs_cmd_forward_gw() */

/*===========================================================================

FUNCTION cmwsups_client_cmd_forward_gw

DESCRIPTION
  Forward a sups commands to MN. 
  This function will be called when route is calculated as CS
  This Function will also be called when IMS sends fallback to CS.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  NONE

SIDE EFFECTS
  none

===========================================================================*/
extern void cmwsups_client_cmd_forward_gw(
  sys_modem_as_id_e_type asubs_id,
  /* Subscription ID */
  cm_sups_cmd_e_type     cmd
    /* Pointer to a CM command type */
)
{
  mn_cnm_cmd_type   *mn_cmd_ptr;
  /* MN command pointer */
  uint8                      num_bytes = 0;
  /* Number of actual bytes required for USSD data */
  cm_process_unstructured_ss_req_T *uss_phase2_msg_ptr = cmsups_get_phase2_msg();
  cmsups_s_type                    *sups_ptr           = cmsups_ptr();
  
  cm_sups_info_s_type              *sups_info_ptr = NULL;
  cm_sups_uss_data_info_s_type     *uss_info_ptr       = cmsups_get_uss_info_ptr(asubs_id, cmd);
  cm_ss_e_type      ss;

  byte                            calculated_size;
  /* Points to storage for phase2 msg */

  if ( cmph_is_subs_feature_mode_sglte( asubs_id ) )
  {
    cmwsups_sglte_determine_ss_for_sups_cmd( asubs_id, cmd, &ss );
  }
  else
  {
    ss = cmph_map_subs_to_ss( asubs_id );
  }

  sups_info_ptr = cmsups_get_sups_info_ptr(ss);

  if (sups_info_ptr == NULL || uss_info_ptr == NULL)
  {
    CM_ERR_1( "sups_info_ptr is NULL ss: %d", ss );
    return;
  }

  switch( cmd )
  {
                       /*-------------------------*/
                       /*    RegisterSS request   */
                       /*-------------------------*/

    case CM_SUPS_CMD_REGISTER:

      /* allocate a buffer */
      mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_REGISTER_SS_REQ );

      /* ss reference */
    mn_cmd_ptr->cmd.cm_register_ss_req.cm_ss_ref = sups_info_ptr->ss_ref;

      /* ss code */
    mn_cmd_ptr->cmd.cm_register_ss_req.ss_code = sups_info_ptr->ss_code;

      /* Copy command information fields. */

    mn_cmd_ptr->cmd.cm_register_ss_req.basic_service = sups_info_ptr->basic_service;

      mn_cmd_ptr->cmd.cm_register_ss_req.forwarded_to_number =
                sups_info_ptr->forwarded_to_number;

      mn_cmd_ptr->cmd.cm_register_ss_req.forwarded_to_subaddress =
                             sups_info_ptr->forwarded_to_subaddress;

      mn_cmd_ptr->cmd.cm_register_ss_req.cm_no_reply_condition_timer =
                sups_info_ptr->nr_timer;

    mn_cmd_ptr->cmd.cm_register_ss_req.as_id  = sups_info_ptr->asubs_id;

     CM_MSG_HIGH_5("CM_SUPS_CMD_REGISTER: ss_code=%d,ss_ref=%d basic_service present=%d, code_type=%d, bs_code=%d",
                      sups_info_ptr->ss_code,\
                      sups_info_ptr->ss_ref , \
                        mn_cmd_ptr->cmd.cm_register_ss_req.basic_service.present, \
                        mn_cmd_ptr->cmd.cm_register_ss_req.basic_service.code_type,\
                        mn_cmd_ptr->cmd.cm_register_ss_req.basic_service.bs_code );

     CM_MSG_HIGH_3("fw_tonum present=%d,len=%d,data=%d ", \
                       mn_cmd_ptr->cmd.cm_register_ss_req.forwarded_to_number.present,\
                       mn_cmd_ptr->cmd.cm_register_ss_req.forwarded_to_number.length,\
                       mn_cmd_ptr->cmd.cm_register_ss_req.forwarded_to_number.data);


      break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                    /*-------------------------*/
                    /*    EraseSS request      */
                    /*-------------------------*/

    case CM_SUPS_CMD_ERASE:

      /* allocate a buffer */
      mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_ERASE_SS_REQ);

  mn_cmd_ptr->cmd.cm_erase_ss_req.cm_ss_ref = sups_info_ptr->ss_ref;

  mn_cmd_ptr->cmd.cm_erase_ss_req.ss_code = sups_info_ptr->ss_code;

      mn_cmd_ptr->cmd.cm_erase_ss_req.basic_service =
                        sups_info_ptr->basic_service;

  mn_cmd_ptr->cmd.cm_erase_ss_req.as_id  = sups_info_ptr->asubs_id;

      break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                  /*-------------------------*/
                  /*    ActivateSS request   */
                  /*-------------------------*/

    case CM_SUPS_CMD_ACTIVATE:

      /* allocate a buffer */
      mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_ACTIVATE_SS_REQ);

      /* ss reference */
  mn_cmd_ptr->cmd.cm_activate_ss_req.cm_ss_ref = sups_info_ptr->ss_ref;

      /* ss code */
  mn_cmd_ptr->cmd.cm_activate_ss_req.ss_code = sups_info_ptr->ss_code;

      /* Copy command information fields. */
      mn_cmd_ptr->cmd.cm_activate_ss_req.basic_service =
              sups_info_ptr->basic_service;

  mn_cmd_ptr->cmd.cm_activate_ss_req.as_id  = sups_info_ptr->asubs_id;

      break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                   /*-------------------------*/
                   /*    DeactivateSS request */
                   /*-------------------------*/

    case CM_SUPS_CMD_DEACTIVATE:

      /* allocate a buffer */
      mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_DEACTIVATE_SS_REQ);

      /* ss reference */
      mn_cmd_ptr->cmd.cm_deactivate_ss_req.cm_ss_ref =
               sups_info_ptr->ss_ref;

      /* ss code */
      mn_cmd_ptr->cmd.cm_deactivate_ss_req.ss_code =
               sups_info_ptr->ss_code;

      /* Copy command information fields. */

      mn_cmd_ptr->cmd.cm_deactivate_ss_req.basic_service =
              sups_info_ptr->basic_service;

  mn_cmd_ptr->cmd.cm_deactivate_ss_req.as_id  = sups_info_ptr->asubs_id;

      break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                   /*-------------------------*/
                   /*   InterrogateSS request */
                   /*-------------------------*/

    case CM_SUPS_CMD_INTERROGATE:

      /* allocate a buffer */
      mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_INTERROGATE_SS_REQ);

      /* Copy command information fields. */

      /* ss reference */
      mn_cmd_ptr->cmd.cm_interrogate_ss_req.cm_ss_ref =
                    sups_info_ptr->ss_ref;

      /* ss code */
      mn_cmd_ptr->cmd.cm_interrogate_ss_req.ss_code =
                    sups_info_ptr->ss_code;

      mn_cmd_ptr->cmd.cm_interrogate_ss_req.basic_service =
                    sups_info_ptr->basic_service;

  mn_cmd_ptr->cmd.cm_interrogate_ss_req.as_id  = sups_info_ptr->asubs_id;

      break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                   /*-----------------------------*/
                   /*  Register Password request  */
                   /*-----------------------------*/

    case CM_SUPS_CMD_REG_PASSWORD:

      /* allocate a buffer */
      mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_REGISTER_PASSWORD_REQ);

      /* Copy command information fields. */

      /* ss reference */
      mn_cmd_ptr->cmd.cm_register_password_req.cm_ss_ref =
        sups_info_ptr->ss_ref;

      mn_cmd_ptr->cmd.cm_register_password_req.ss_code =
        sups_info_ptr->code;

      mn_cmd_ptr->cmd.cm_register_password_req.ss_password =
        sups_info_ptr->ss_password;

      mn_cmd_ptr->cmd.cm_register_password_req.ss_new_password =
        sups_info_ptr->ss_new_password;

      mn_cmd_ptr->cmd.cm_register_password_req.ss_new_password_again =
        sups_info_ptr->ss_new_password_again;

  mn_cmd_ptr->cmd.cm_register_password_req.as_id  = sups_info_ptr->asubs_id;

      break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                   /*-----------------------------*/
                   /*    Process Unstructured SS  */
                   /*-----------------------------*/

    case CM_SUPS_CMD_PROCESS_USS:

      /* allocate a buffer */
      mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_PROCESS_UNSTRUCTURED_SS_REQ);

      /* Copy command information fields. */

      /* ss reference */
      mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.cm_ss_ref =
    uss_info_ptr->ss_ref;

      /* ss operation */
      mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.ss_operation =
                                          uss_info_ptr->ss_operation;

    CM_MSG_HIGH_2( "CM_SUPS_CMD_PROCESS_USS Size: %d bytes, DCC: %d",
               uss_info_ptr->uss_data.size,
               uss_info_ptr->uss_data.uss_data_coding_scheme );

        calculated_size = uss_info_ptr->uss_data.size;

      /*lint -save -e641 Converting enum to int */
  if(((uss_info_ptr->uss_data.uss_data_coding_scheme &
                             CM_USSD_ALPHABET_MASK) == CM_USSD_DCS_7_BIT)
         ||
     ((uss_info_ptr->uss_data.uss_data_coding_scheme &
                             CM_USSD_ALPHABET_MASK) == CM_USSD_DCS_UNSPECIFIED)
        )
      {
            /* MAX_USS_CHAR is what MN can support
       */
   if (uss_info_ptr->uss_data.size > MAX_USS_CHAR_ASCII)
       {
     uss_info_ptr->uss_data.size = MAX_USS_CHAR_ASCII;
         CM_MSG_HIGH_1 ("Truncating USSD msg to %d bytes", MAX_USS_CHAR);
       }

        /* convert to the default alphabet */
        num_bytes = cm_util_ascii_to_gsm_alphabet(
              mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.uss_data.ussData,
          uss_info_ptr->uss_data.ussData,
        uss_info_ptr->uss_data.size );

        calculated_size = num_bytes;

      }
      /* Usage of CM_SUPS_CMD_PROCESS_USS has been to encode ussd data in CM
      ** and then send. EXCEPTION is being made to UCS2 where data being
      ** received is in encoded form. It is a TEMPORARY fix and needs to be
      ** changed with a new field that says if encoding has already been done.
      */
  else if(((uss_info_ptr->uss_data.uss_data_coding_scheme &
                                  CM_USSD_ALPHABET_MASK) == CM_USSD_DCS_8_BIT)
               ||
          ((uss_info_ptr->uss_data.uss_data_coding_scheme &
                                   CM_USSD_ALPHABET_MASK) == CM_USSD_DCS_UCS2)
             )

      {
         if(calculated_size > MAX_USS_BYTES)
        {
          calculated_size = MAX_USS_BYTES;
          CM_MSG_HIGH_1 ("Truncating USSD msg to %d bytes", MAX_USS_BYTES);
        }

        memscpy(  mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.
                 uss_data.ussData,
                 sizeof(mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.
                 uss_data.ussData),
             uss_info_ptr->uss_data.ussData,
             calculated_size);
      }
      else
      {
        CM_ERR_0 (" USSD Data coding scheme error");
      }
      /*lint -restore 641 Converting enum to int */

      mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.uss_data.size =
                      calculated_size;

      mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.
                                   uss_data.uss_data_coding_scheme =
                  uss_info_ptr->uss_data.uss_data_coding_scheme;

      mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.uss_data.present = TRUE;
  uss_info_ptr->uss_data.present = TRUE;

      /* also save a copy of the USSD in case there is a phase
         compatibility problem */

        memset (uss_phase2_msg_ptr, 0, sizeof(cm_process_unstructured_ss_req_T));

       /* Copy ss ref */
       uss_phase2_msg_ptr->cm_ss_ref =
                    mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.cm_ss_ref;

       /* Copy ss operation */
       uss_phase2_msg_ptr->ss_operation =
                    mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.ss_operation;


       /* Copy ss data */
       memscpy ( &uss_phase2_msg_ptr->uss_data,
                  sizeof(uss_phase2_msg_ptr->uss_data),
                &mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.uss_data,
                sizeof(uss_data_T));

  mn_cmd_ptr->cmd.cm_process_unstructured_ss_req.as_id = uss_info_ptr->asubs_id;

      CM_MSG_HIGH_4 ("uss data sent to clients and NAS. present:%d coding scheme: %d size: %d uss data: %s",
                uss_info_ptr->uss_data.present,
                uss_info_ptr->uss_data.uss_data_coding_scheme,
                calculated_size,
                uss_info_ptr->uss_data.ussData);

      break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    /*-------------------------*/
                    /*    ReleaseSS request    */
                    /*-------------------------*/

    case CM_SUPS_CMD_RELEASE:

      /* allocate a buffer */
      mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_RELEASE_SS_REQ);

      /* Copy command information fields. */

      mn_cmd_ptr->cmd.cm_release_ss_req.cm_ss_ref =
        uss_info_ptr->invoke_id;

      mn_cmd_ptr->cmd.cm_release_ss_req.ss_operation =
        uss_info_ptr->ss_operation;

  mn_cmd_ptr->cmd.cm_release_ss_req.as_id  = uss_info_ptr->asubs_id;

      cmsups_reset_mt_uss_context_info( uss_info_ptr->asubs_id );

      break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                     /*-------------------------*/
                     /*   Get Password response */
                     /*-------------------------*/

    case CM_SUPS_CMD_GET_PASSWORD_RES:

      /* allocate a buffer */
      mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_GET_PASSWORD_RES);

      mn_cmd_ptr->cmd.cm_get_password_res.invoke_id =
                                           sups_info_ptr->invoke_id;

      mn_cmd_ptr->cmd.cm_get_password_res.ss_password =  sups_info_ptr->ss_password;

  mn_cmd_ptr->cmd.cm_get_password_res.as_id    = sups_info_ptr->asubs_id;

      break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                     /*----------------------------*/
                     /*  Unstructured SS response  */
                     /*----------------------------*/

    case CM_SUPS_CMD_USS_RES:

      /* allocate a buffer */
      mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_UNSTRUCTURED_SS_RES);

      /* Copy command information fields. */
      mn_cmd_ptr->cmd.cm_unstructured_ss_res.invoke_id =
                                            uss_info_ptr->invoke_id;

  if( uss_info_ptr->uss_res_type == CM_USS_RES_SS_ERROR )
      {
        mn_cmd_ptr->cmd.cm_unstructured_ss_res.uss_res_data.data.cm_ss_error =
            uss_info_ptr->ss_error;
            mn_cmd_ptr->cmd.cm_unstructured_ss_res.uss_res_data.data_type = SS_ERROR_INFO;
      }
  else if ( uss_info_ptr->uss_res_type == CM_USS_RES_SS_DATA )
      {
        mn_cmd_ptr->cmd.cm_unstructured_ss_res.uss_res_data.data.uss_data =
              uss_info_ptr->uss_data;

        mn_cmd_ptr->cmd.cm_unstructured_ss_res.uss_res_data.data_type = USS_DATA_INFO;
      }

  mn_cmd_ptr->cmd.cm_unstructured_ss_res.as_id   = uss_info_ptr->asubs_id;

      break;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                    /*-----------------------------------*/
                    /*  Unstructured SS Notify response  */
                    /*-----------------------------------*/

    case CM_SUPS_CMD_USS_NOTIFY_RES:

      /* allocate a buffer */
      mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_UNSTRUCTURED_SS_NOTIFY_RES);

      /* Copy command information fields. */
      mn_cmd_ptr->cmd.cm_unstructured_ss_notify_res.invoke_id =
                                            uss_info_ptr->invoke_id;


  if(uss_info_ptr->uss_res_type == CM_USS_RES_SS_ERROR)
      {
        mn_cmd_ptr->cmd.cm_unstructured_ss_notify_res.cm_ss_error =
              uss_info_ptr->ss_error;
      }

  mn_cmd_ptr->cmd.cm_unstructured_ss_notify_res.as_id  = uss_info_ptr->asubs_id;

      break;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                       /*-------------------------*/
                     /*    EraseCCEntry request   */
                     /*-------------------------*/

    case CM_SUPS_CMD_ERASE_CC_ENTRY:

      #if defined(FEATURE_CCBS)
      /* allocate a buffer */
      mn_cmd_ptr = cm_mn_get_buf_else_err_fatal(CM_ERASE_CC_ENTRY_REQ );

      /* ss reference */
      mn_cmd_ptr->cmd.cm_erase_cc_entry_req.cm_ss_ref
                                        = sups_info_ptr->ss_ref;

      /* ss code */
  mn_cmd_ptr->cmd.cm_erase_cc_entry_req.ss_code = sups_info_ptr->ss_code;

      /* Deactivate one or all ccbs indx requests
      ** CM_CCBS_INDX_INVALID - maps to deactivating all requests
      ** ccbs_index is valid from 1 to 5 (22.030 Table B.1) ,
      */
  if (sups_info_ptr->ccbs_indx == CM_CCBS_INDX_INVALID)
      {
        mn_cmd_ptr->cmd.cm_erase_cc_entry_req.ccbs_index.present
                                             = FALSE;

        CM_MSG_HIGH_0 ("ERASE_CC_ENTRY for all ccbs indexes");
      }
      else
      {
        mn_cmd_ptr->cmd.cm_erase_cc_entry_req.ccbs_index.present
                                             = TRUE;

        mn_cmd_ptr->cmd.cm_erase_cc_entry_req.ccbs_index.ccbs_Index
                                         = sups_info_ptr->ccbs_indx;

        CM_MSG_HIGH_1 ("ERASE_CC_ENTRY for ccbs index %d",
                                     sups_info_ptr->ccbs_indx);
      }

      CM_MSG_HIGH_1 ("is_erase_cc_conf_pend flag = %d",
                   sups_ptr->ccbs_erase_pend_indx.is_erase_cc_conf_pend);

      /* Save the ccbs indx requested for deactivation to clear
      ** phone object ccbs indx store on confirmation
      */
      if (!sups_ptr->ccbs_erase_pend_indx.is_erase_cc_conf_pend)
      {
        sups_ptr->ccbs_erase_pend_indx.is_erase_cc_conf_pend = TRUE;
    sups_ptr->ccbs_erase_pend_indx.cm_ss_ref = sups_info_ptr->ss_ref;
    sups_ptr->ccbs_erase_pend_indx.ccbs_indx = sups_info_ptr->ccbs_indx;
      }
      else
      {
        CM_MSG_HIGH_0 ("Simult ccbs deact requests arrived");
      }

  mn_cmd_ptr->cmd.cm_erase_cc_entry_req.as_id  = sups_info_ptr->asubs_id;

      #endif

      break;

    default:
  CM_MSG_HIGH_1( "Unknown SUPS command %d received from client", cmd );
      return;

    }

  /* send the command to MN */
  cm_mn_send_cmd(mn_cmd_ptr);

} /* cmwsups_client_cmd_forward_gw() */

/*===========================================================================

FUNCTION CMSUPS_CONF_COPY

DESCRIPTION
  Copy the confirmation from MN to the SUPS object.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmsups_conf_copy(

    cm_ss_e_type        ss,
      /* stack into which to copy */

    const ss_info_T     *ss_info
)
{
  cm_sups_info_s_type  *sups_conf_ptr;

  int                   i;
  int                   j;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ss_info           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  sups_conf_ptr = cmsups_get_sups_info_ptr(ss);

  /* save the confirmation type */
  sups_conf_ptr->conf_type = ss_info->info_type;

  if( sups_conf_ptr->conf_type == NO_INFO)
    /* confirmation with no parameter information*/
    /* no further processing is required */
    return;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (ss_info->info_type)
  {
    case FWD_INFO:
      sups_conf_ptr->forwarding_info = ss_info->info.forwarding_info;
      /* need to convert the BCD number to ASCII number */
      if( sups_conf_ptr->forwarding_info.forwarding_feature_list.
                 forwarding_feature_list_length != 0 )

      {
        for( i = 0;
             i < sups_conf_ptr->forwarding_info.forwarding_feature_list.
                      forwarding_feature_list_length;
             i++ )
        {
            for ( j = 0; j < MAXNO_FORWARD_TO_NUMBER; j++ )
            {
              if( sups_conf_ptr->forwarding_info.forwarding_feature_list.forwarding_feature[i].forwarded_to_number[j].present )
              {
                memscpy(
                  &sups_conf_ptr->forwarding_info.forwarding_feature_list.forwarding_feature[i].forwarded_to_number[j],
                  sizeof(sups_conf_ptr->forwarding_info.forwarding_feature_list.forwarding_feature[i].forwarded_to_number[j]),
                  &ss_info->info.forwarding_info.forwarding_feature_list.forwarding_feature[i].forwarded_to_number[j],
                  sizeof( ss_info->info.forwarding_info.forwarding_feature_list.forwarding_feature[i].forwarded_to_number[j]));
              }
            }
        }
      }
      break;

    case CALL_BARRING_INFO:
      sups_conf_ptr->call_barring_info = ss_info->info.call_barring_info;
      break;

    case CUG_INFO:
      sups_conf_ptr->cug_info = ss_info->info.cug_info;
      break;

    case SS_DATA_INFO:
      sups_conf_ptr->ss_data = ss_info->info.ss_data;
      break;

    case SS_ERROR_INFO:
      sups_conf_ptr->ss_error = ss_info->info.cm_ss_error;
      break;

    case CC_CAUSE_INFO:
      sups_conf_ptr->cc_cause = ss_info->info.cm_cc_cause;
      break;

    #if defined(FEATURE_CCBS)
    case CCBS_INFO:
      CM_MSG_HIGH_0 ("cmsups_conf_copy(): not expected for CCBS_INFO");
      break;
    #endif /* FEATURE_CCBS */

    default:
      CM_MSG_HIGH_1( "ERR: Unknown Confirmation type %d received", ss_info->info_type);
      sups_conf_ptr->conf_type = UNKNOWN_CONF_INFO;
      break;
  }
}

#ifdef FEATURE_SS_L3_DATA
/*===========================================================================

FUNCTION cmwsups_copy_suppsvc_to_sups_info

DESCRIPTION
  Copies unparsed L3 message from suppsvc_result (in cmd_ptr) to sups_info_ptr

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  none

SIDE EFFECTS
  sups_unparsed_l3_data field in SUPS Object will get populated if data is
  present.

===========================================================================*/

static void cmwsups_copy_suppsvc_to_sups_info
(
  cm_ss_e_type                      ss,
    /* stack wo thich the indo needs to be copied */

  const unparsed_suppsvc_l3_data_T  *suppsvc_ptr 

)
{

  uint16 l3_data_size = 0;

  cm_sups_info_s_type           *sups_info_ptr = NULL;
    /* Pointer to the sups information */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( suppsvc_ptr != NULL );

  sups_info_ptr = cmsups_get_sups_info_ptr(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Sanity check */

  if (!suppsvc_ptr->present)
  {
    CM_MSG_HIGH_0("Layer 3 data not present");
    sups_info_ptr->sups_unparsed_l3_data.present = FALSE;
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Range checks */

  l3_data_size = suppsvc_ptr->size;


  if ( l3_data_size == 0  )
  {
    CM_ERR_0("Layer 3 data size is 0. No copy required");
    return;
  }

  if (l3_data_size > ARR_SIZE(suppsvc_ptr->data) )
  {
    CM_ERR_0("Value of suppsvc_ptr->size greater than array size");
    l3_data_size = ARR_SIZE(suppsvc_ptr->data) ;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy data */

  CM_MSG_HIGH_0("Copying layer 3 data");

  sups_info_ptr->sups_unparsed_l3_data.present =
    suppsvc_ptr->present;

  sups_info_ptr->sups_unparsed_l3_data.size = l3_data_size;

  memscpy (sups_info_ptr->sups_unparsed_l3_data.data,
          sizeof(sups_info_ptr->sups_unparsed_l3_data.data),
          suppsvc_ptr->data,
          l3_data_size);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Print debug messages */

  CM_MSG_HIGH_2( "Present: %d, Size: %d",
               sups_info_ptr->sups_unparsed_l3_data.present,
               sups_info_ptr->sups_unparsed_l3_data.size);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

} /* cmwsups_copy_suppsvc_to_sups_info */
#endif /*   #ifdef FEATURE_SS_L3_DATA */

/*===========================================================================

FUNCTION cmwsups_check_redial_cc_cause

DESCRIPTION
  Check if given CC cause needs sups retry

DEPENDENCIES

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  none

===========================================================================*/
boolean cmwsups_check_redial_cc_cause(
  byte  cause_value
)
{

  switch(cause_value)
  {
    case AS_REJ_RRC_OPEN_SESSION_FAILURE:
	case AS_REJ_LOW_LEVEL_FAIL:
	case AS_REJ_RRC_REL_IND:
	case AS_REJ_RR_REL_IND:
	case AS_REJ_ABORT_RADIO_UNAVAILABLE:
	case AS_REJ_LOW_LEVEL_IMMED_RETRY:
	case AS_REJ_RR_RANDOM_ACCESS_FAILURE:
	case MM_REJ_NO_SERVICE:
	case MM_REJ_NO_CELL_AVAILABLE:
	case MM_REJ_WRONG_STATE:
	case MM_REJ_TIMER_T3230_EXP:
	  return TRUE;
	  
	default:
	  return FALSE;
  }
}

/*lint +esym(715,ss) */
/*===========================================================================

FUNCTION cmsups_rpt_proc

DESCRIPTION
  Process lower layer reports (i.e. notifications of sups events from MN)

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsups_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
)
/*lint -esym(529, sups_ptr) not referenced */
{

  cm_rpt_type   *cm_rpt_ptr = (cm_rpt_type *) rpt_ptr;
      /* Pointer to LL report */

  cmsups_s_type                 *sups_ptr = cmsups_ptr();
      /* Pointer to the sups object */

  boolean                       is_mem_free_req = TRUE;
  boolean                       is_retry                   = FALSE;

  cm_sups_info_s_type           *sups_info_ptr = NULL;
      /* Pointer to the sups information */

  InterrogateSS_Res_T           *interr_conf_ptr;
      /* Pointer to the interrogateSS response */

  #if defined(FEATURE_CCBS)
  ie_ccbs_indicator_T           *ccbs_indicator_ptr;
     /* Pointer to ccbs indicator */

  int                            indx_count = 0;
     /* Loops through ccbs indx list */

  #endif

  int                            i;
  int                            j;

  cm_ss_e_type                   ss = CM_SS_MAIN;
    /* stack on which the report is recieved */
  cm_sups_event_e_type     sups_event_type = CM_SUPS_EVENT_NONE;

  boolean                       is_break_in_mid = FALSE;

  #ifdef FEATURE_SUPS_RETRY
  dword                       uptime              = time_get_uptime_secs();
  cmwsups_info_list_s_type        *sups_list_info_ptr = NULL;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_rpt_ptr != NULL );

  CM_ASSERT( sups_ptr != NULL );

  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do command specific processing as follows:
  **   Update appropriate sups object fields.
  **   Notifying clients of event if necessary.
  */
  switch( cm_rpt_ptr->hdr.cmd )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    /*                              **
                    **  Register SS Confirmation    **
                    **                              */
    case CM_REGISTER_SS_CONF :

      CM_MSG_HIGH_0("RXD: CM_REGISTER_SS_CONF");
      
      /* Update the number of pending sups requests */
      if (sups_ptr->num_pending_sups_cmds != 0)
        sups_ptr->num_pending_sups_cmds-- ;  
      
      ss = cmph_map_subs_to_ss(cm_rpt_ptr->cmd.register_conf.as_id);

      cmsups_update_asubs_id(cm_rpt_ptr->cmd.register_conf.as_id,&sups_info_ptr);
                                              
      #ifdef FEATURE_SUPS_RETRY
      sups_list_info_ptr = cmwsups_info_list_find(ss, cm_rpt_ptr->cmd.register_conf.cm_ss_ref);
      #endif
      
      #ifdef FEATURE_SS_L3_DATA
      cmwsups_copy_suppsvc_to_sups_info( ss,
                                &cm_rpt_ptr->cmd.register_conf.suppsvc_result);
      #endif

      sups_info_ptr->ss_ref = cm_rpt_ptr->cmd.register_conf.cm_ss_ref;

      if ( cm_rpt_ptr->cmd.register_conf.success )
      {
        cmsups_conf_copy( ss, &cm_rpt_ptr->cmd.register_conf.ss_info );
        sups_info_ptr->ss_success = TRUE;
      }
      else
      {
        /* error in the message received from the network */
        sups_info_ptr->ss_success = FALSE;
      }

      /* Multi-sim: Resume ps data */
      cmsups_msim_resume_data(ss);

      #if defined(FEATURE_SUPS_RETRY)
      if(((cm_rpt_ptr->cmd.register_conf.ss_info.info_type == CC_CAUSE_INFO &&
	       (cmwsups_check_redial_cc_cause(cm_rpt_ptr->cmd.register_conf.ss_info.info.cm_cc_cause.cause_value) == TRUE)) ||
          (cm_rpt_ptr->cmd.register_conf.ss_info.info_type == SS_ERROR_INFO &&
          cm_rpt_ptr->cmd.register_conf.ss_info.info.cm_ss_error.error_code == AS_REJ_ABORT_RADIO_UNAVAILABLE)) &&
          sups_list_info_ptr != NULL)
      {
        /* If retry timer will expire before next retry...send the event now */
        if(sups_list_info_ptr->sr_start_uptime  > (uptime + SUPS_NEXT_RETRY_DURATION - SUPS_RETRY_MAX_DURATION))
        {
          /* Radio Unavailable....initiate retry */
          sups_list_info_ptr->next_orig_uptime = uptime + SUPS_NEXT_RETRY_DURATION;
          
          CM_MSG_HIGH_3("SUPS failed, Initiate Retry. ss_ref %d cause %d next uptime %d",
                 sups_info_ptr->ss_ref,
                 cm_rpt_ptr->cmd.register_conf.ss_info.info.cm_cc_cause.cause_value,
                 sups_list_info_ptr->next_orig_uptime);
          
          /* Set flag to not free the corresponding memory */
          is_retry= TRUE;
        }
      }
      #endif

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    /*                           **
                    **  Erase SS Confirmation    **
                    **                           */

    case CM_ERASE_SS_CONF:

      CM_MSG_HIGH_0("RXD: CM_ERASE_SS_CONF");
      
      /* Update the number of pending sups requests */
      if (sups_ptr->num_pending_sups_cmds != 0)
        sups_ptr->num_pending_sups_cmds-- ;  

      ss = cmph_map_subs_to_ss(cm_rpt_ptr->cmd.erase_conf.as_id);

      cmsups_update_asubs_id( cm_rpt_ptr->cmd.erase_conf.as_id, &sups_info_ptr);

      #ifdef FEATURE_SUPS_RETRY
      sups_list_info_ptr = cmwsups_info_list_find(ss, cm_rpt_ptr->cmd.erase_conf.cm_ss_ref);
      #endif
      
      #ifdef FEATURE_SS_L3_DATA
      cmwsups_copy_suppsvc_to_sups_info( ss,
                                   &cm_rpt_ptr->cmd.erase_conf.suppsvc_result);
      #endif

      sups_info_ptr->ss_ref = cm_rpt_ptr->cmd.erase_conf.cm_ss_ref;

      if ( cm_rpt_ptr->cmd.erase_conf.success )
      {
        cmsups_conf_copy( ss, &cm_rpt_ptr->cmd.erase_conf.ss_info );
        sups_info_ptr->ss_success = TRUE;
      }
      else
      {  
        /* error in the message received from the network */
        sups_info_ptr->ss_success = FALSE;
      }

      /* Multi-sim: Resume ps data */
      cmsups_msim_resume_data(ss);

      #if defined(FEATURE_SUPS_RETRY)
      if(((cm_rpt_ptr->cmd.erase_conf.ss_info.info_type == CC_CAUSE_INFO &&
	       (cmwsups_check_redial_cc_cause(cm_rpt_ptr->cmd.erase_conf.ss_info.info.cm_cc_cause.cause_value) == TRUE)) ||
           (cm_rpt_ptr->cmd.erase_conf.ss_info.info_type == SS_ERROR_INFO&&
          cm_rpt_ptr->cmd.erase_conf.ss_info.info.cm_ss_error.error_code == AS_REJ_ABORT_RADIO_UNAVAILABLE)) &&
          sups_list_info_ptr != NULL)
      {
        /* If retry timer will expire before next retry...send the event now */
        if(sups_list_info_ptr->sr_start_uptime  > (uptime + SUPS_NEXT_RETRY_DURATION - SUPS_RETRY_MAX_DURATION))
        {      
          /* Radio Unavailable....initiate retry */
          sups_list_info_ptr->next_orig_uptime = uptime + SUPS_NEXT_RETRY_DURATION;
          
          CM_MSG_HIGH_3("SUPS failed, Initiate Retry. ss_ref %d cause %d next uptime %d",
                 sups_info_ptr->ss_ref,
                 cm_rpt_ptr->cmd.erase_conf.ss_info.info.cm_cc_cause.cause_value,
                 sups_list_info_ptr->next_orig_uptime);
          
          /* Set flag to not free the corresponding memory */
          is_retry= TRUE;
        }
      }
      #endif

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    /*                           **
                    **  Activate SS Confirmation **
                    **                           */

    case CM_ACTIVATE_SS_CONF:

      CM_MSG_HIGH_0("RXD: CM_ACTIVATE_SS_CONF");


      /* Update the number of pending sups requests */
      if (sups_ptr->num_pending_sups_cmds != 0)
        sups_ptr->num_pending_sups_cmds-- ;  


      ss = cmph_map_subs_to_ss(cm_rpt_ptr->cmd.activate_conf.as_id);

      cmsups_update_asubs_id( cm_rpt_ptr->cmd.activate_conf.as_id, &sups_info_ptr);

      #ifdef FEATURE_SUPS_RETRY
      sups_list_info_ptr = cmwsups_info_list_find(ss, cm_rpt_ptr->cmd.activate_conf.cm_ss_ref);
      #endif
      
      #ifdef FEATURE_SS_L3_DATA
      cmwsups_copy_suppsvc_to_sups_info( ss,
                                &cm_rpt_ptr->cmd.activate_conf.suppsvc_result);
      #endif

      sups_info_ptr->ss_ref = cm_rpt_ptr->cmd.activate_conf.cm_ss_ref;

      if ( cm_rpt_ptr->cmd.activate_conf.success )
      {
        cmsups_conf_copy( ss, &cm_rpt_ptr->cmd.activate_conf.ss_info );
        sups_info_ptr->ss_success = TRUE;
      }
      else
      {  
        /* error in the message received from the network */
        sups_info_ptr->ss_success = FALSE;
      }
      
      /* Multi-sim: Resume ps data */
      cmsups_msim_resume_data(ss);

      #if defined(FEATURE_SUPS_RETRY)
      if(((cm_rpt_ptr->cmd.activate_conf.ss_info.info_type == CC_CAUSE_INFO &&
	       (cmwsups_check_redial_cc_cause(cm_rpt_ptr->cmd.activate_conf.ss_info.info.cm_cc_cause.cause_value) == TRUE)) ||
          (cm_rpt_ptr->cmd.activate_conf.ss_info.info_type == SS_ERROR_INFO&&
          cm_rpt_ptr->cmd.activate_conf.ss_info.info.cm_ss_error.error_code == AS_REJ_ABORT_RADIO_UNAVAILABLE)) &&
          sups_list_info_ptr != NULL)
      {
        /* If retry timer will expire before next retry...send the event now */
        if(sups_list_info_ptr->sr_start_uptime  > (uptime + SUPS_NEXT_RETRY_DURATION - SUPS_RETRY_MAX_DURATION))
        {
          /* Radio Unavailable....initiate retry */
          sups_list_info_ptr->next_orig_uptime = uptime + SUPS_NEXT_RETRY_DURATION;
          
          CM_MSG_HIGH_3("SUPS failed, Initiate Retry. ss_ref %d cause %d next uptime %d",
                 sups_info_ptr->ss_ref,
                 cm_rpt_ptr->cmd.activate_conf.ss_info.info.cm_cc_cause.cause_value,
                 sups_list_info_ptr->next_orig_uptime);
          
          /* Set flag to not free the corresponding memory */
          is_retry= TRUE;
        }
      }
      #endif

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    /*                            **
                    ** Deactivate SS Confirmation **
                    **                            */

    case CM_DEACTIVATE_SS_CONF:

      CM_MSG_HIGH_0("RXD: CM_DEACTIVATE_SS_CONF");


      /* Update the number of pending sups requests */
      if (sups_ptr->num_pending_sups_cmds != 0)
        sups_ptr->num_pending_sups_cmds-- ;  

      ss = cmph_map_subs_to_ss(cm_rpt_ptr->cmd.deactivate_conf.as_id);

      cmsups_update_asubs_id( cm_rpt_ptr->cmd.deactivate_conf.as_id, &sups_info_ptr);

      #ifdef FEATURE_SUPS_RETRY
      sups_list_info_ptr = cmwsups_info_list_find(ss, cm_rpt_ptr->cmd.deactivate_conf.cm_ss_ref);
      #endif
      
      #ifdef FEATURE_SS_L3_DATA
      cmwsups_copy_suppsvc_to_sups_info( ss,
                                &cm_rpt_ptr->cmd.deactivate_conf.suppsvc_result);
      #endif

      sups_info_ptr->ss_ref = cm_rpt_ptr->cmd.deactivate_conf.cm_ss_ref;
      if ( cm_rpt_ptr->cmd.deactivate_conf.success )
      {
        cmsups_conf_copy( ss, &cm_rpt_ptr->cmd.deactivate_conf.ss_info );
        sups_info_ptr->ss_success = TRUE;
      }
      else
      {  
        /* error in the message received from the network */
        sups_info_ptr->ss_success = FALSE;
      }

      /* Multi-sim: Resume ps data */
      cmsups_msim_resume_data(ss);

      #if defined(FEATURE_SUPS_RETRY)
      if(((cm_rpt_ptr->cmd.deactivate_conf.ss_info.info_type == CC_CAUSE_INFO &&
	       (cmwsups_check_redial_cc_cause(cm_rpt_ptr->cmd.deactivate_conf.ss_info.info.cm_cc_cause.cause_value) == TRUE)) ||
          (cm_rpt_ptr->cmd.deactivate_conf.ss_info.info_type == SS_ERROR_INFO &&
          cm_rpt_ptr->cmd.deactivate_conf.ss_info.info.cm_ss_error.error_code == AS_REJ_ABORT_RADIO_UNAVAILABLE)) &&
          sups_list_info_ptr != NULL)
      {
        /* If retry timer will expire before next retry...send the event now */
        if(sups_list_info_ptr->sr_start_uptime  > (uptime + SUPS_NEXT_RETRY_DURATION - SUPS_RETRY_MAX_DURATION))
        {
          /* Radio Unavailable....initiate retry */
          sups_list_info_ptr->next_orig_uptime = uptime + SUPS_NEXT_RETRY_DURATION;
          
          CM_MSG_HIGH_3("SUPS failed, Initiate Retry. ss_ref %d cause %d next uptime %d",
                 sups_info_ptr->ss_ref,
                 cm_rpt_ptr->cmd.deactivate_conf.ss_info.info.cm_cc_cause.cause_value,
                 sups_list_info_ptr->next_orig_uptime);
          
          /* Set flag to not free the corresponding memory */
          is_retry= TRUE;
        }
      }
      #endif
      
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    /*                             **
                    ** Interrogate SS Confirmation **
                    **                             */

    case CM_INTERROGATE_SS_CONF:

      CM_MSG_HIGH_0("RXD: CM_INTERROGATE_SS_CONF");


      /* Update the number of pending sups requests */
      if (sups_ptr->num_pending_sups_cmds != 0)
        sups_ptr->num_pending_sups_cmds-- ; 
        
      ss = cmph_map_subs_to_ss(cm_rpt_ptr->cmd.interrogate_conf.as_id);

      cmsups_update_asubs_id( cm_rpt_ptr->cmd.interrogate_conf.as_id, &sups_info_ptr);

      #ifdef FEATURE_SUPS_RETRY
      sups_list_info_ptr = cmwsups_info_list_find(ss, cm_rpt_ptr->cmd.interrogate_conf.cm_ss_ref);
      #endif
      
      #ifdef FEATURE_SS_L3_DATA
      cmwsups_copy_suppsvc_to_sups_info( ss,
                             &cm_rpt_ptr->cmd.interrogate_conf.suppsvc_result);
      #endif

      sups_info_ptr->ss_ref = cm_rpt_ptr->cmd.interrogate_conf.cm_ss_ref;

      if( cm_rpt_ptr->cmd.interrogate_conf.success )
      {
        sups_info_ptr->ss_success = TRUE;

        interr_conf_ptr = &cm_rpt_ptr->cmd.interrogate_conf.InterrogateSS_Res;

        /* confirmation type */
        sups_info_ptr->conf_type = cm_rpt_ptr->cmd.interrogate_conf.res_type;

        /* save the interrogate SS response */
        switch( sups_info_ptr->conf_type )
        {
          /* no information */
          case NO_INFO:
            break;
          /* ss status */
          case SS_STATUS_INFO:
            sups_info_ptr->ss_status = interr_conf_ptr->cm_ss_status;
            break;

          /* cause */
          case CC_CAUSE_INFO:
            sups_info_ptr->cc_cause = interr_conf_ptr->cm_cc_cause;
            break;

          /* ss error */
          case SS_ERROR_INFO:
            sups_info_ptr->ss_error = interr_conf_ptr->cm_ss_error;
            break;

          /* CLI restriction */
          case CLI_RESTRICTION_INFO:
            sups_info_ptr->cli_restriction = interr_conf_ptr->cli_restriction_info;
            break;

          /* Forwarding Feature List */
          case FWD_FEATURE_LIST_INFO:
            sups_info_ptr->fwd_feature_list =
                       interr_conf_ptr->forwarding_feature_list;
            /* if there is a forwarded to number, convert the number
               from BCD to ASCII */
            if( sups_info_ptr->fwd_feature_list.forwarding_feature_list_length != 0 )
            {
              for( i = 0; i < sups_info_ptr->fwd_feature_list.forwarding_feature_list_length; i++)
              {
                for ( j = 0; j < MAXNO_FORWARD_TO_NUMBER; j++ )
                {
                  if ( sups_info_ptr->fwd_feature_list.forwarding_feature[i].forwarded_to_number[j].present )
                  {
                    memscpy(
                      &sups_info_ptr->fwd_feature_list.forwarding_feature[i].forwarded_to_number[j],
                      sizeof(sups_info_ptr->fwd_feature_list.forwarding_feature[i].forwarded_to_number[j]),
                      &interr_conf_ptr->forwarding_feature_list.forwarding_feature[i].forwarded_to_number[j],
                      sizeof( interr_conf_ptr->forwarding_feature_list.forwarding_feature[i].forwarded_to_number[j]) );
                  }
                }
              }
            }
            break;

          #if defined(FEATURE_CCBS)

          /* CCBS information */
          case CCBS_INFO:
            /* Returns list of ccbs indexes stored at network
            */
            sups_info_ptr->ccbs_info       = interr_conf_ptr->ccbs_info;

            if (!sups_info_ptr->ccbs_info.present)
            {
              break;
            }

            /* clear all ccbs index entries from Ph object
            */
            cmph_clear_ccbs_entries();
  
            /* ccbs index store in ph object is replaced with list returned
            ** from interrogate conf
            */
            for (indx_count = 0;
                 indx_count < (int)ARR_SIZE(interr_conf_ptr->ccbs_info.ccbs_feature_list);
                 ++indx_count)
            {
              ccbs_indicator_ptr = &interr_conf_ptr->ccbs_info.ccbs_feature_list[indx_count];

              if ((ccbs_indicator_ptr->present == TRUE) &&
                  (ccbs_indicator_ptr->ccbsIndex.present == TRUE)
                 )
              {
                if(!cmph_add_ccbs_entry (ss, ccbs_indicator_ptr->ccbsIndex.ccbs_Index,
                                         ccbs_indicator_ptr->subscriberNumber[0])
                  )
                {
                  CM_ERR_0 ("Error in adding ccbs entry");
                }
              }
            }
            break;
            #endif /* FEATURE_CCBS */

          /* Basic Service Group List */
          case BSG_LIST_INFO:
            sups_info_ptr->bsg_list = interr_conf_ptr->basic_service_group_list;
            break;

          /* Forwarded to Number */
          case FWD_TO_NUM_INFO:
          case CALLED_PARTY_NUM_INFO:
            break;

          default:
            CM_MSG_HIGH_1( "ERR: Unknown Confirmation type %d received", sups_info_ptr->conf_type);
            /* required if lower layer does not set the unknown info type */
            sups_info_ptr->conf_type = UNKNOWN_CONF_INFO;
            break;  
        }
      }
      else
      {
        /* error in the message received from the network */
        sups_info_ptr->ss_success = FALSE;

      }

      /* Multi-sim: Resume ps data */
      cmsups_msim_resume_data(ss);

      #if defined(FEATURE_SUPS_RETRY)
      if(((cm_rpt_ptr->cmd.interrogate_conf.res_type == CC_CAUSE_INFO &&
           (cmwsups_check_redial_cc_cause(cm_rpt_ptr->cmd.interrogate_conf.InterrogateSS_Res.cm_cc_cause.cause_value) == TRUE)) ||
          (cm_rpt_ptr->cmd.interrogate_conf.res_type == SS_ERROR_INFO &&
          cm_rpt_ptr->cmd.interrogate_conf.InterrogateSS_Res.cm_ss_error.error_code == AS_REJ_ABORT_RADIO_UNAVAILABLE)) &&
          sups_list_info_ptr != NULL)
      {
        /* If retry timer will expire before next retry...send the event now */
        if(sups_list_info_ptr->sr_start_uptime  > (uptime + SUPS_NEXT_RETRY_DURATION - SUPS_RETRY_MAX_DURATION))
        {
          /* Radio Unavailable....initiate retry */
          sups_list_info_ptr->next_orig_uptime = uptime + SUPS_NEXT_RETRY_DURATION;
          
          CM_MSG_HIGH_3("SUPS failed, Initiate Retry. ss_ref %d cause %d next uptime %d",
                 sups_info_ptr->ss_ref,
                 cm_rpt_ptr->cmd.interrogate_conf.InterrogateSS_Res.cm_cc_cause.cause_value,
                 sups_list_info_ptr->next_orig_uptime);
          
          /* Set flag to not free the corresponding memory */
          is_retry= TRUE;
        }
      }
      #endif

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    /*                                **
                    ** Register Password Confirmation **
                    **                                */

    case CM_REGISTER_PASSWORD_CONF:

      CM_MSG_HIGH_0("RXD: CM_REGISTER_PASSWORD_CONF");


      /* Update the number of pending sups requests */
      if (sups_ptr->num_pending_sups_cmds != 0)
        sups_ptr->num_pending_sups_cmds-- ;  

      ss = cmph_map_subs_to_ss(cm_rpt_ptr->cmd.reg_password_conf.as_id);

      cmsups_update_asubs_id(cm_rpt_ptr->cmd.reg_password_conf.as_id, &sups_info_ptr);

      #ifdef FEATURE_SUPS_RETRY
      sups_list_info_ptr = cmwsups_info_list_find(ss, cm_rpt_ptr->cmd.reg_password_conf.cm_ss_ref);
      #endif
      
      #ifdef FEATURE_SS_L3_DATA
      cmwsups_copy_suppsvc_to_sups_info(ss,
                            &cm_rpt_ptr->cmd.reg_password_conf.suppsvc_result);
      #endif

      sups_info_ptr->ss_ref = cm_rpt_ptr->cmd.reg_password_conf.cm_ss_ref;

      if ( cm_rpt_ptr->cmd.reg_password_conf.success )
      {
        sups_info_ptr->ss_success = TRUE;

        /* confirmation type */
        sups_info_ptr->conf_type = cm_rpt_ptr->cmd.reg_password_conf.info.data_type;

        switch ( sups_info_ptr->conf_type )
        {
          /* no information */
          case NO_INFO:
            break;

          /* SS error */
          case SS_ERROR_INFO:
            sups_info_ptr->ss_error =
                    cm_rpt_ptr->cmd.reg_password_conf.info.data.cm_ss_error;

            break;

          /* New password */
          case NEW_PWD_INFO:
            sups_info_ptr->ss_password =
                cm_rpt_ptr->cmd.reg_password_conf.info.data.ss_new_password;
            break;

          /* Cause */
          case CC_CAUSE_INFO:
            sups_info_ptr->cc_cause =
                     cm_rpt_ptr->cmd.reg_password_conf.info.data.cm_cc_cause;
            break;

          default:
            CM_MSG_HIGH_1( "ERROR: Unknown Confirmation type %d received", 
                      sups_info_ptr->conf_type );
            /* required if lower layer does not set the unknown info type */
            sups_info_ptr->conf_type = UNKNOWN_CONF_INFO;
            break;
        }
      }
      else 
      {
        sups_info_ptr->ss_success = FALSE;
      }
      
      /* Multi-sim: Resume ps data */
      cmsups_msim_resume_data(ss);

      #if defined(FEATURE_SUPS_RETRY)
      if(((cm_rpt_ptr->cmd.reg_password_conf.info.data_type == CC_CAUSE_INFO &&
	       (cmwsups_check_redial_cc_cause(cm_rpt_ptr->cmd.reg_password_conf.info.data.cm_cc_cause.cause_value) == TRUE))||
          (cm_rpt_ptr->cmd.reg_password_conf.info.data_type == SS_ERROR_INFO &&
          cm_rpt_ptr->cmd.reg_password_conf.info.data.cm_ss_error.error_code == AS_REJ_ABORT_RADIO_UNAVAILABLE)) &&
          sups_list_info_ptr != NULL)
      {
        /* If retry timer will expire before next retry...send the event now */
        if(sups_list_info_ptr->sr_start_uptime  > (uptime + SUPS_NEXT_RETRY_DURATION - SUPS_RETRY_MAX_DURATION))
        {
          /* Radio Unavailable....initiate retry */
          sups_list_info_ptr->next_orig_uptime = uptime + SUPS_NEXT_RETRY_DURATION;
          
          CM_MSG_HIGH_3("SUPS failed, Initiate Retry. ss_ref %d cause %d next uptime %d",
                 sups_info_ptr->ss_ref,
                 cm_rpt_ptr->cmd.reg_password_conf.info.data.cm_cc_cause.cause_value,
                 sups_list_info_ptr->next_orig_uptime);
          
          /* Set flag to not free the corresponding memory */
          is_retry= TRUE;
        }
      }
      #endif
      
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    /*                             **
                    ** Process USS Confirmation    **
                    **                             */
    case CM_PROCESS_UNSTRUCTURED_SS_CONF:

      CM_MSG_HIGH_0("RXD: CM_PROCESS_UNSTRUCTURED_SS_CONF");


      /* Update the number of pending sups requests */
      if (sups_ptr->num_pending_sups_cmds != 0)
        sups_ptr->num_pending_sups_cmds-- ;  

      ss = cmph_map_subs_to_ss(cm_rpt_ptr->cmd.process_uss_conf.as_id);

      cmsups_update_asubs_id( cm_rpt_ptr->cmd.process_uss_conf.as_id, &sups_info_ptr);

      #ifdef FEATURE_SUPS_RETRY
      sups_list_info_ptr = cmwsups_info_list_find(ss, cm_rpt_ptr->cmd.process_uss_conf.cm_ss_ref);
      #endif
      
      #ifdef FEATURE_SS_L3_DATA
      cmwsups_copy_suppsvc_to_sups_info(ss,
                             &cm_rpt_ptr->cmd.process_uss_conf.suppsvc_result);
      #endif

      sups_info_ptr->ss_ref = cm_rpt_ptr->cmd.process_uss_conf.cm_ss_ref;

      if ( cm_rpt_ptr->cmd.process_uss_conf.success)
      {
        sups_info_ptr->ss_success = TRUE;

        /* confirmation type */
        sups_info_ptr->conf_type = cm_rpt_ptr->cmd.process_uss_conf.ss_data_info.data_type;

        if( ( ( cm_rpt_ptr->cmd.process_uss_conf.ss_data_info.data_type == SS_ERROR_INFO ) &&
              ( cm_rpt_ptr->cmd.process_uss_conf.ss_data_info.data.cm_ss_error.error_code_tag  ==
                     INVOKE_PROBLEM ) &&
              ( cm_rpt_ptr->cmd.process_uss_conf.ss_data_info.data.cm_ss_error.error_code ==
                   UNRECOGNISED_OPERATION ) )
           || ( ( cm_rpt_ptr->cmd.process_uss_conf.ss_data_info.data_type == CC_CAUSE_INFO ) &&
               ( cm_rpt_ptr->cmd.process_uss_conf.ss_data_info.data.cm_cc_cause.cause_value
                    == FACILITY_REJECTED )
                           )
                  )
        {
          /*
          ** the ussd request was not understood by the network,
          ** resend a phase1 message.
          */
          CM_MSG_HIGH_0( "Send PHASE 1 USS MESSAGE" );

          uss_phase1_msg = TRUE;

          cmsups_ussd_phase1_req(ss);

          /* Needs to maintain a new local instead of uss_phase1_msg
             because, real MS USSD events are atomic, but UT USSD 
             events are triggered, one after one before getting the
             previous transaction completed, we will just break in
             mid when we have received incorrect USS_CONF  */

          is_break_in_mid = TRUE;
          
          break;
        }

        sups_info_ptr->uss_data_type = CM_USS_PHASE2_TYPE;

        switch( sups_info_ptr->conf_type )
        {
          /* no information */
          case NO_INFO:
            break;

          /* USS data */
          case USS_DATA_INFO:
            sups_info_ptr->uss_data =
                cm_rpt_ptr->cmd.process_uss_conf.ss_data_info.data.uss_data;
            break;

          /* SS error */
          case SS_ERROR_INFO:
            sups_info_ptr->ss_error =
               cm_rpt_ptr->cmd.process_uss_conf.ss_data_info.data.cm_ss_error;
            break;

          /* Cause */
          case CC_CAUSE_INFO:
            sups_info_ptr->cc_cause =
                cm_rpt_ptr->cmd.process_uss_conf.ss_data_info.data.cm_cc_cause;
            break;

          default:
            CM_MSG_HIGH_1( "ERR: Unknown Confirmation type %d received",
                      sups_info_ptr->conf_type );
            /* required if lower layer does not set the unknown info type */
            sups_info_ptr->conf_type = UNKNOWN_CONF_INFO;
            break;
        }

        /* set the USS data type for the client processing */
        sups_info_ptr->uss_data_type = CM_USS_PHASE2_TYPE;

      }
      else
      {
        sups_info_ptr->ss_success = FALSE;
      }

      /* Multi-sim: Resume ps data */
      cmsups_msim_resume_data(ss);

      #if defined(FEATURE_SUPS_RETRY)
      if(((cm_rpt_ptr->cmd.process_uss_conf.ss_data_info.data_type == CC_CAUSE_INFO &&
          (cmwsups_check_redial_cc_cause(cm_rpt_ptr->cmd.process_uss_conf.ss_data_info.data.cm_cc_cause.cause_value) == TRUE)) ||
          (cm_rpt_ptr->cmd.process_uss_conf.ss_data_info.data_type == SS_ERROR_INFO &&
          cm_rpt_ptr->cmd.process_uss_conf.ss_data_info.data.cm_ss_error.error_code == AS_REJ_ABORT_RADIO_UNAVAILABLE)) &&
          sups_list_info_ptr != NULL)
      {
        /* If retry timer will expire before next retry...send the event now */
        if(sups_list_info_ptr->sr_start_uptime  > (uptime + SUPS_NEXT_RETRY_DURATION - SUPS_RETRY_MAX_DURATION))
        {
          /* Radio Unavailable....initiate retry */
          sups_list_info_ptr->next_orig_uptime = uptime + SUPS_NEXT_RETRY_DURATION;
          
          CM_MSG_HIGH_3("SUPS failed, Initiate Retry. ss_ref %d cause %d next uptime %d",
                 sups_info_ptr->ss_ref,
                 cm_rpt_ptr->cmd.process_uss_conf.ss_data_info.data.cm_cc_cause.cause_value,
                 sups_list_info_ptr->next_orig_uptime);
          
          /* Set flag to not free the corresponding memory */
          is_retry= TRUE;
        }
      }
      #endif
      
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    /*                             **
                    ** Process USSD Confirmation   **
                    **                             */
    case CM_PROCESS_UNSTRUCTURED_SS_DATA_CONF:
      
      CM_MSG_HIGH_0("RXD: CM_PROCESS_UNSTRUCTURED_SS_DATA_CONF");


      /* Update the number of pending sups requests */
      if (sups_ptr->num_pending_sups_cmds != 0)
        sups_ptr->num_pending_sups_cmds-- ;  

      ss = cmph_map_subs_to_ss(cm_rpt_ptr->cmd.process_uss_data_conf.as_id);

      cmsups_update_asubs_id( cm_rpt_ptr->cmd.process_uss_data_conf.as_id, &sups_info_ptr);

      #ifdef FEATURE_SUPS_RETRY
      sups_list_info_ptr = cmwsups_info_list_find(ss, cm_rpt_ptr->cmd.process_uss_data_conf.cm_ss_ref);
      #endif
      
      #ifdef FEATURE_SS_L3_DATA
      cmwsups_copy_suppsvc_to_sups_info( ss,
                        &cm_rpt_ptr->cmd.process_uss_data_conf.suppsvc_result);
      #endif

      if ( ( uss_phase1_msg ) &&
           ( cm_rpt_ptr->cmd.process_uss_data_conf.success ) )
      {
        uss_phase1_msg = FALSE;

        sups_info_ptr->ss_success = TRUE;

        sups_info_ptr->uss_data_type = CM_USS_PHASE1_TYPE;

        sups_info_ptr->ss_ref =
                    cm_rpt_ptr->cmd.process_uss_data_conf.cm_ss_ref;

        sups_info_ptr->conf_type =
                     cm_rpt_ptr->cmd.process_uss_data_conf.ss_UserData_info.data_type;
        /*
        1. copy only USS_SIZE (160 not 200)
        2. Indicate to UI that there is no need to convert to ASCII
            since this is a phase1 message
        */
        switch( sups_info_ptr->conf_type )
        {
          case NO_INFO:
            break;

          /* USS data */
          case USS_DATA_INFO:
            memscpy( sups_info_ptr->uss_data.ussData,
                    sizeof(sups_info_ptr->uss_data.ussData),
                    cm_rpt_ptr->cmd.process_uss_data_conf.ss_UserData_info.data.ss_User_Data.ss_UserData,
                    MAX_USS_BYTES  ); 
			/* use MAX_USS_BYTES here since, from n/w side string would always come in gsm-7
			and, max bytes supported for gsm-7 is 160 */
                    
            sups_info_ptr->uss_data.size = cm_rpt_ptr->cmd.process_uss_data_conf.ss_UserData_info.data.ss_User_Data.size;
            sups_info_ptr->uss_data.present = cm_rpt_ptr->cmd.process_uss_data_conf.ss_UserData_info.data.ss_User_Data.present;
            break;

          /* SS error */
          case SS_ERROR_INFO:
            sups_info_ptr->ss_error =
              cm_rpt_ptr->cmd.process_uss_data_conf.ss_UserData_info.data.cm_ss_error;
            break;

          /* Cause */
          case CC_CAUSE_INFO:
            sups_info_ptr->cc_cause =
              cm_rpt_ptr->cmd.process_uss_data_conf.ss_UserData_info.data.cm_cc_cause;
            break;

          default:
            CM_MSG_HIGH_1( "ERR: Unknown Confirmation type %d received",
                      sups_info_ptr->conf_type );
            /* required if lower layer does not set the unknown info type */
            sups_info_ptr->conf_type = UNKNOWN_CONF_INFO;
            break;

        }
      }
      else
      {  

        /* USSD phase 1 procedure has completed */
        CM_MSG_HIGH_0(" resetting uss_phase1_msg");
        uss_phase1_msg = FALSE;
        sups_info_ptr->ss_success = FALSE;
        sups_info_ptr->ss_ref =
                    cm_rpt_ptr->cmd.process_uss_data_conf.cm_ss_ref;
      }

      /* Multi-sim: Resume ps data */
      cmsups_msim_resume_data(ss);

      #if defined(FEATURE_SUPS_RETRY)
      if(((cm_rpt_ptr->cmd.process_uss_data_conf.ss_UserData_info.data_type == CC_CAUSE_INFO &&
           (cmwsups_check_redial_cc_cause(cm_rpt_ptr->cmd.process_uss_data_conf.ss_UserData_info.data.cm_cc_cause.cause_value) == TRUE))||
          (cm_rpt_ptr->cmd.process_uss_data_conf.ss_UserData_info.data_type == SS_ERROR_INFO &&
          cm_rpt_ptr->cmd.process_uss_data_conf.ss_UserData_info.data.cm_ss_error.error_code == AS_REJ_ABORT_RADIO_UNAVAILABLE ))&&
          sups_list_info_ptr != NULL)
      {
        /* If retry timer will expire before next retry...send the event now */
        if(sups_list_info_ptr->sr_start_uptime  > (uptime + SUPS_NEXT_RETRY_DURATION - SUPS_RETRY_MAX_DURATION))
        {
          /* Radio Unavailable....initiate retry */
          sups_list_info_ptr->next_orig_uptime = uptime + SUPS_NEXT_RETRY_DURATION;
          
          CM_MSG_HIGH_3("SUPS failed, Initiate Retry. ss_ref %d cause %d next uptime %d",
                 sups_info_ptr->ss_ref,
                 cm_rpt_ptr->cmd.process_uss_data_conf.ss_UserData_info.data.cm_cc_cause.cause_value,
                 sups_list_info_ptr->next_orig_uptime);
          
          /* Set flag to not free the corresponding memory */
          is_retry= TRUE;
        }
      }
      #endif

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    /*                             **
                    ** Forward Check SS Indication **
                    **                             */

    case CM_FORWARD_CHECK_SS_INDICATION_IND:

      CM_MSG_HIGH_0("RXD: CM_FWD_CHECK_SS_IND");

      /* Update the number of pending sups requests */
      if (sups_ptr->num_pending_sups_cmds != 0)
        sups_ptr->num_pending_sups_cmds-- ;  

      ss = cmph_map_subs_to_ss(cm_rpt_ptr->cmd.fwd_check_ind.as_id);

      /* Multi-sim: Resume ps data */
      cmsups_msim_resume_data(ss);

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    /*                             **
                    ** Unstructured SS Indication  **
                    **                             */
    case CM_UNSTRUCTURED_SS_IND:
      {

        cmsups_cmd_route_e_type sups_route;
        if ( cmsups_get_mt_uss_routing_context_info( cm_rpt_ptr->cmd.uss_notify_ind.as_id, &sups_route) == TRUE &&
             sups_route == CMSUPS_CMD_ALLOW_IMS )
        {
          ie_cm_ss_error_T         cm_ss_error;
          CM_MSG_HIGH_0("Reject cmd, MT USS on IMS domain is ongoing");
          cm_ss_error.present = TRUE;
          cm_ss_error.error_code_tag = 0x02;
          cm_ss_error.error_code = ussd_Busy;
          cmwsups_send_release_ss_req( cm_rpt_ptr->cmd.uss_ind.invoke_id,
                                       unstructuredSS_Request,
                                       cm_ss_error,
                                       cm_rpt_ptr->cmd.uss_ind.as_id );
          return;
        }

        CM_MSG_HIGH_0("RXD: CM_UNSTRUCTURED_SS_IND");
        cmsups_set_mt_uss_context_info(cm_rpt_ptr->cmd.uss_ind.as_id, CMSUPS_CMD_ALLOW_CS);
  
        /* Update the number of pending sups requests */
        if (sups_ptr->num_pending_sups_cmds != 0)
          sups_ptr->num_pending_sups_cmds-- ;  

        ss = cmph_map_subs_to_ss(cm_rpt_ptr->cmd.uss_ind.as_id);

        cmsups_update_asubs_id( cm_rpt_ptr->cmd.uss_ind.as_id, &sups_info_ptr);

	  

        /* USS invoke id */
        sups_info_ptr->invoke_id = cm_rpt_ptr->cmd.uss_ind.invoke_id;

        /* USS data */
        sups_info_ptr->uss_data = cm_rpt_ptr->cmd.uss_ind.uss_data;

#ifdef FEATURE_SUPS_RETRY
        {
          cmwsups_info_list_s_type* sups_list = NULL; 
  		
          #if defined FEATURE_MMODE_TRIPLE_SIM || defined FEATURE_MMODE_SXLTE_G
          if (ss == CM_SS_HYBR_3)
          {
            sups_list = sups_ptr->hybr_gw3_sups_info_list_head; 
          }
          else
          #endif
          #if defined FEATURE_MMODE_DUAL_SIM
          if (ss == CM_SS_HYBR_2)
          {
            sups_list = sups_ptr->hybr_gw_sups_info_list_head; 
          }
          else
          #endif
          {
            sups_list = sups_ptr->sups_info_list_head; 
          }
		
		
          while(sups_list != NULL)
          {
            if (sups_list->cmd == CM_SUPS_CMD_PROCESS_USS)
            {
              CM_MSG_HIGH_0("Free USS Sups list object due to MT USS from network");
  
              #if defined FEATURE_MMODE_TRIPLE_SIM || defined FEATURE_MMODE_SXLTE_G
              if (ss == CM_SS_HYBR_3) 
              { 
                if(sups_list == sups_ptr->hybr_gw3_sups_info_list) 
                { 
                  CM_MSG_HIGH_1("sups_ptr->hybr_gw3_sups_info_list:",sups_ptr->hybr_gw3_sups_info_list);
                  sups_ptr->hybr_gw3_sups_info_list = NULL; 
                } 
                cmwsups_mem_free_sups_list_info(sups_list, &(sups_ptr->hybr_gw3_sups_info_list_head)); 
                CM_MSG_HIGH_1("sups_ptr->hybr_gw3_sups_info_list_head:",sups_ptr->hybr_gw3_sups_info_list_head);
              } 
              else
              #endif
              #if defined FEATURE_MMODE_DUAL_SIM
              if (ss == CM_SS_HYBR_2) 
              { 
                if(sups_list == sups_ptr->hybr_gw_sups_info_list) 
                { 
                  CM_MSG_HIGH_1("sups_ptr->hybr_gw_sups_info_list: %d",sups_ptr->hybr_gw_sups_info_list);
                  sups_ptr->hybr_gw_sups_info_list = NULL; 
                } 
                cmwsups_mem_free_sups_list_info(sups_list, &(sups_ptr->hybr_gw_sups_info_list_head)); 
                CM_MSG_HIGH_1("sups_ptr->hybr_gw_sups_info_list_head: %d",sups_ptr->hybr_gw_sups_info_list_head);
              } 
              else 
              #endif 
              { 
                if(sups_list == sups_ptr->sups_info_list) 
                { 
                  CM_MSG_HIGH_1("sups_ptr->sups_info_list: %d",sups_ptr->sups_info_list);
                  sups_ptr->sups_info_list = NULL; 
                } 
                cmwsups_mem_free_sups_list_info(sups_list, &(sups_ptr->sups_info_list_head));
                CM_MSG_HIGH_1("sups_ptr->sups_info_list_head: %d",sups_ptr->sups_info_list_head);
                sups_list = NULL; // may not be mandatory as it is local, but will be better to understand that it has been freed.
              } 
					
              break;
            }
            sups_list = sups_list->next; /* Move to next record */
          }
        }
#endif

        /* Multi-sim: Resume ps data */
        cmsups_msim_resume_data(ss);
      }
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    /*                                   **
                    ** Unstructured SS Notify Indication **
                    **                                   */
    case CM_UNSTRUCTURED_SS_NOTIFY_IND:
      {
        cmsups_cmd_route_e_type sups_route;
        if ( cmsups_get_mt_uss_routing_context_info( cm_rpt_ptr->cmd.uss_notify_ind.as_id, &sups_route) == TRUE &&
             sups_route == CMSUPS_CMD_ALLOW_IMS )
        {
          ie_cm_ss_error_T         cm_ss_error;
          CM_MSG_HIGH_0("Reject cmd, MT USS on IMS domain is ongoing");
          cm_ss_error.present = TRUE;
          cm_ss_error.error_code_tag = 0x02;
          cm_ss_error.error_code = ussd_Busy;
          cmwsups_send_release_ss_req( cm_rpt_ptr->cmd.uss_ind.invoke_id,
                                       unstructuredSS_Notify,
                                       cm_ss_error,
                                       cm_rpt_ptr->cmd.uss_ind.as_id );
          
          return;
        }

        CM_MSG_HIGH_0("RXD: CM_UNSTRUCTURED_SS_NOTIFY_IND");

        cmsups_set_mt_uss_context_info(cm_rpt_ptr->cmd.uss_notify_ind.as_id, CMSUPS_CMD_ALLOW_CS);

        /* Update the number of pending sups requests */
        if (sups_ptr->num_pending_sups_cmds != 0)
          sups_ptr->num_pending_sups_cmds-- ;  

        ss = cmph_map_subs_to_ss(cm_rpt_ptr->cmd.uss_notify_ind.as_id);

        cmsups_update_asubs_id( cm_rpt_ptr->cmd.uss_notify_ind.as_id, &sups_info_ptr);

        /* USS invoke id */
        sups_info_ptr->invoke_id = cm_rpt_ptr->cmd.uss_notify_ind.invoke_id;

        /* USS data */
        sups_info_ptr->uss_data = cm_rpt_ptr->cmd.uss_notify_ind.uss_data;
        if(sups_info_ptr->uss_data.size > MAX_USS_BYTES)
        {
          sups_info_ptr->uss_data.size = MAX_USS_BYTES; 
          /* max length can be 160 only in case of GSM-7 bit encoding */
          memset(sups_info_ptr->uss_data.ussData + MAX_USS_BYTES , 0 , ((MAX_USS_CHAR_ASCII - MAX_USS_BYTES) + 1) );
          CM_MSG_HIGH_1 ("Truncating USSD msg from n/w to %d bytes", MAX_USS_BYTES);
        }

        /* Multi-sim: Resume ps data */
        cmsups_msim_resume_data(ss);
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    /*                             **
                    ** Get Password Indication     **
                    **                             */
    case CM_GET_PASSWORD_IND:

      CM_MSG_HIGH_0("RXD: CM_GET_PASSWORD_IND");


      /* Update the number of pending sups requests */
      if (sups_ptr->num_pending_sups_cmds != 0)
        sups_ptr->num_pending_sups_cmds-- ;  

      ss = cmph_map_subs_to_ss(cm_rpt_ptr->cmd.get_password_ind.as_id);

      cmsups_update_asubs_id( cm_rpt_ptr->cmd.get_password_ind.as_id, &sups_info_ptr);
      
      /* Invoke ID */
      sups_info_ptr->invoke_id = cm_rpt_ptr->cmd.get_password_ind.invoke_id;

      /* Guidance Info */
      sups_info_ptr->guidance_info = (ss_password_value) \
                                cm_rpt_ptr->cmd.get_password_ind.guidance_info;
      
      /* Multi-sim: Resume ps data */
      cmsups_msim_resume_data(ss);

      break;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                    /*                             **
                    ** Release USS Indication      **
                    **                             */

    case CM_RELEASE_UNSTRUCTURED_SS_IND:

      CM_MSG_HIGH_0("RXD: CM_RELEASE_USS_IND");


      /* Update the number of pending sups requests */
      if (sups_ptr->num_pending_sups_cmds != 0)
        sups_ptr->num_pending_sups_cmds-- ;  


      /* Note: If a cross phase compatibility is required, do not send the
               release uss indication event to the clients till the final
               release complete is received.
      */
      if ( !uss_phase1_msg )
      {
        ss = cmph_map_subs_to_ss(cm_rpt_ptr->cmd.rel_uss_ind.as_id);

        cmsups_update_asubs_id( cm_rpt_ptr->cmd.rel_uss_ind.as_id, &sups_info_ptr);

        /* clear the invocation ID */
        sups_info_ptr->invoke_id = 0;
        cmsups_reset_mt_uss_context_info( cm_rpt_ptr->cmd.rel_uss_ind.as_id );

      }
      else
      {
        /* Free the memory alloacted to sups_info_ptr, after sending the event */
        if (is_mem_free_req == TRUE && sups_ptr->num_pending_sups_cmds == 0)
        {  
          cmsups_mem_free_sups_info (sups_ptr);
          cmsups_free_uss_info_ptr(cm_rpt_ptr->cmd.rel_uss_ind.as_id);
        }

        return;
      }
      
      /* Multi-sim: Resume ps data */
      cmsups_msim_resume_data(ss);

      #ifdef FEATURE_SUPS_RETRY
      {
        cmwsups_info_list_s_type* sups_list = NULL; 
        #if defined FEATURE_MMODE_TRIPLE_SIM || defined FEATURE_MMODE_SXLTE_G
        if (ss == CM_SS_HYBR_3)
        {
          sups_list = sups_ptr->hybr_gw3_sups_info_list_head; 
        }
        else
        #endif
        #if defined FEATURE_MMODE_DUAL_SIM
        if (ss == CM_SS_HYBR_2)
        {
          sups_list = sups_ptr->hybr_gw_sups_info_list_head; 
        }
        else
        #endif
        {
          sups_list = sups_ptr->sups_info_list_head; 
        }
		
        if(sups_list != NULL)
        {
          while(sups_list != NULL)
          {
            if (sups_list->cmd == CM_SUPS_CMD_PROCESS_USS)
            {
              CM_MSG_HIGH_0("Free USS Sups list object due to rel ind recvd from LL");
              #if defined FEATURE_MMODE_TRIPLE_SIM || defined FEATURE_MMODE_SXLTE_G
              if (ss == CM_SS_HYBR_3) 
              { 
                if(sups_list == sups_ptr->hybr_gw3_sups_info_list) 
                { 
                  CM_MSG_HIGH_0("sups_ptr->hybr_gw3_sups_info_list");
                  sups_ptr->hybr_gw3_sups_info_list = NULL; 
                } 
                cmwsups_mem_free_sups_list_info(sups_list, &(sups_ptr->hybr_gw3_sups_info_list_head)); 
                CM_MSG_HIGH_1("sups_ptr->hybr_gw3_sups_info_list_head:",sups_ptr->hybr_gw3_sups_info_list_head);
              } 
              else
              #endif
              #if defined FEATURE_MMODE_DUAL_SIM 
              if (ss == CM_SS_HYBR_2) 
              { 
                if(sups_list == sups_ptr->hybr_gw_sups_info_list) 
                { 
                  CM_MSG_HIGH_0("sups_ptr->hybr_gw_sups_info_list");
                  sups_ptr->hybr_gw_sups_info_list = NULL; 
                } 
                cmwsups_mem_free_sups_list_info(sups_list, &(sups_ptr->hybr_gw_sups_info_list_head)); 
                CM_MSG_HIGH_1("sups_ptr->hybr_gw_sups_info_list_head:",sups_ptr->hybr_gw_sups_info_list_head);
              } 
              else 
              #endif 
              { 
                if(sups_list == sups_ptr->sups_info_list) 
                { 
                  CM_MSG_HIGH_0("sups_ptr->sups_info_list");
                  sups_ptr->sups_info_list = NULL; 
                } 
                cmwsups_mem_free_sups_list_info(sups_list, &(sups_ptr->sups_info_list_head));
                CM_MSG_HIGH_0("sups_ptr->sups_info_list_head");
                sups_list = NULL; // may not be mandatory as it is local, but will be better to understand that it has been freed.
              } 
							
              break;
            }
            sups_list = sups_list->next; /* Move to next record */
          }
		}
	  }
	  #endif

      break;

    #if (defined(FEATURE_CM_MMGPS) || defined(FEATURE_CM_LCS))
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                  /*                                      **
                  ** LCS Location Notification Indication **
                  **                                      */
    case CM_LCS_LOCATION_NOTIFICATION_IND:

      CM_MSG_HIGH_0("RXD: CM_LCS_LOCATION_NOTIFICATION_IND");

      /* Update the number of pending sups requests */
      if (sups_ptr->num_pending_sups_cmds != 0)
        sups_ptr->num_pending_sups_cmds-- ;  

      ss = cmph_map_subs_to_ss(cm_rpt_ptr->cmd.lcs_location_notification_ind.as_id); 

      cmsups_update_asubs_id( cm_rpt_ptr->cmd.lcs_location_notification_ind.as_id, &sups_info_ptr);

      /* LCS Location Notification invoke id */
      sups_info_ptr->invoke_id =
                cm_rpt_ptr->cmd.lcs_location_notification_ind.invoke_id;

      /* Location Notification Args */

      sups_info_ptr->location_notification =
              cm_rpt_ptr->cmd.lcs_location_notification_ind.location_notification;

      /* Multi-sim: Resume ps data */
      cmsups_msim_resume_data(ss);

      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                  /*                             **
                  ** LCS MOLR Confirmation       **
                  **                             */

    case CM_LCS_MOLR_CONF:

      CM_MSG_HIGH_0("RXD: CM_LCS_MOLR_CONF");

      /* Update the number of pending sups requests */
      if (sups_ptr->num_pending_sups_cmds != 0)
        sups_ptr->num_pending_sups_cmds-- ;  

      ss = cmph_map_subs_to_ss(cm_rpt_ptr->cmd.lcs_molr_conf.as_id);

      cmsups_update_asubs_id( cm_rpt_ptr->cmd.lcs_molr_conf.as_id, &sups_info_ptr);

      #ifdef FEATURE_SUPS_RETRY
      sups_list_info_ptr = cmwsups_info_list_find(ss, cm_rpt_ptr->cmd.lcs_molr_conf.cm_ss_ref);
      #endif
      
      #ifdef FEATURE_SS_L3_DATA
      cmwsups_copy_suppsvc_to_sups_info( ss,
                                &cm_rpt_ptr->cmd.lcs_molr_conf.suppsvc_result);
      #endif

      sups_info_ptr->ss_ref = cm_rpt_ptr->cmd.lcs_molr_conf.cm_ss_ref;
      sups_info_ptr->ss_success = cm_rpt_ptr->cmd.lcs_molr_conf.success;
      sups_info_ptr->lcs_molr_res = cm_rpt_ptr->cmd.lcs_molr_conf.lcs_molr_res;

      /* Multi-sim: Resume ps data */
      cmsups_msim_resume_data(ss);

      #if defined(FEATURE_SUPS_RETRY)
      if(((cm_rpt_ptr->cmd.lcs_molr_conf.lcs_molr_res.data_type == CC_CAUSE_INFO &&
	       (cmwsups_check_redial_cc_cause(cm_rpt_ptr->cmd.lcs_molr_conf.lcs_molr_res.data.cm_cc_cause.cause_value) == TRUE))||
          (cm_rpt_ptr->cmd.lcs_molr_conf.lcs_molr_res.data_type == SS_ERROR_INFO &&
          cm_rpt_ptr->cmd.lcs_molr_conf.lcs_molr_res.data.cm_ss_error.error_code == AS_REJ_ABORT_RADIO_UNAVAILABLE)) &&
          sups_list_info_ptr != NULL)
      {
        /* If retry timer will expire before next retry...send the event now */
        if(sups_list_info_ptr->sr_start_uptime  > (uptime + SUPS_NEXT_RETRY_DURATION - SUPS_RETRY_MAX_DURATION))
        {      
          /* Radio Unavailable....initiate retry */
          sups_list_info_ptr->next_orig_uptime = uptime + SUPS_NEXT_RETRY_DURATION;
          
          CM_MSG_HIGH_3("SUPS failed, Initiate Retry. ss_ref %d cause %d next uptime %d",
                 sups_info_ptr->ss_ref,
                 cm_rpt_ptr->cmd.lcs_molr_conf.lcs_molr_res.data.cm_cc_cause.cause_value,
                 sups_list_info_ptr->next_orig_uptime);

          /* Set flag to not free the corresponding memory */
          is_retry= TRUE;
        }
      }
      #endif
      
      break;

  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                  /*                             **
                  ** Erase CC ENTRY Confirmation **
                  **                             */

    case CM_ERASE_CC_ENTRY_CONF:

      #if defined(FEATURE_CCBS)

      CM_MSG_HIGH_0 ("RXD: CM_ERASE_CC_ENTRY_CONF");

      /* Update the number of pending sups requests */
      if (sups_ptr->num_pending_sups_cmds != 0)
        sups_ptr->num_pending_sups_cmds-- ;  

      ss = cmph_map_subs_to_ss(cm_rpt_ptr->cmd.erase_cc_entry_conf.as_id); 

      cmsups_update_asubs_id( cm_rpt_ptr->cmd.erase_cc_entry_conf.as_id, &sups_info_ptr);
                                              
      #ifdef FEATURE_SUPS_RETRY
      sups_list_info_ptr = cmwsups_info_list_find(ss, cm_rpt_ptr->cmd.erase_cc_entry_conf.cm_ss_ref);
      #endif
      
      sups_info_ptr->ss_ref = cm_rpt_ptr->cmd.erase_cc_entry_conf.cm_ss_ref;

      #if defined(FEATURE_SUPS_RETRY)
        if(((cm_rpt_ptr->cmd.erase_cc_entry_conf.ss_info.info_type == CC_CAUSE_INFO &&
		    (cmwsups_check_redial_cc_cause(cm_rpt_ptr->cmd.erase_cc_entry_conf.ss_info.info.cm_cc_cause.cause_value) == TRUE)) ||
             (cm_rpt_ptr->cmd.erase_cc_entry_conf.ss_info.info_type == SS_ERROR_INFO &&
          cm_rpt_ptr->cmd.erase_cc_entry_conf.ss_info.info.cm_ss_error.error_code == AS_REJ_ABORT_RADIO_UNAVAILABLE)) &&
          sups_list_info_ptr != NULL)
        {
          /* If retry timer will expire before next retry...send the event now */
          if(sups_list_info_ptr->sr_start_uptime  > (uptime + SUPS_NEXT_RETRY_DURATION - SUPS_RETRY_MAX_DURATION))
          {       
            /* Radio Unavailable....initiate retry */
            sups_list_info_ptr->next_orig_uptime = uptime + SUPS_NEXT_RETRY_DURATION;
            
            CM_MSG_HIGH_3("SUPS failed, Initiate Retry. ss_ref %d cause %d next uptime %d",
                   sups_info_ptr->ss_ref,
                   cm_rpt_ptr->cmd.erase_cc_entry_conf.ss_info.info.cm_cc_cause.cause_value,
                   sups_list_info_ptr->next_orig_uptime);
            
            /* Set flag to not free the corresponding memory */
            is_retry= TRUE;
          }
        }
      #endif
      
      if (!cm_rpt_ptr->cmd.erase_cc_entry_conf.success)
      {
        sups_info_ptr->ss_success = FALSE;
        break;
      }

      sups_info_ptr->ss_success = TRUE;

      cmsups_conf_copy( ss, &cm_rpt_ptr->cmd.erase_cc_entry_conf.ss_info );

      /* info_type is NO_INFO when CCBS deactivation is successfull
      ** info_type is SS_ERROR_INFO for all other cases
      */
      if (cm_rpt_ptr->cmd.erase_cc_entry_conf.ss_info.info_type == NO_INFO)
      {
        /* Get ccbs index from SS object
        */
         if ( !((sups_ptr->ccbs_erase_pend_indx.is_erase_cc_conf_pend) &&

                (sups_ptr->ccbs_erase_pend_indx.cm_ss_ref ==
                 cm_rpt_ptr->cmd.erase_cc_entry_conf.cm_ss_ref)
               )
            )
         {

           CM_ERR_0 (" Erase CC Conf is not matching our request ");
         }

         if (sups_ptr->ccbs_erase_pend_indx.ccbs_indx != CM_CCBS_INDX_INVALID)
         {
           if (!cmph_remove_ccbs_entry (CM_SS_MAIN, sups_ptr->ccbs_erase_pend_indx.ccbs_indx))
           {
             CM_ERR_0 (" ccbs entry could not be removed ");
           }
         }
         else
         {
           /* clear all ccbs index entries from Ph object
           */
           cmph_clear_ccbs_entries ();
         }

         sups_ptr->ccbs_erase_pend_indx.is_erase_cc_conf_pend = FALSE;

      }

      /*
      ** if ss_code is ccbs then deactivation gets converted into EraseCCEntry
      ** request (24.093 section 4.4).
      ** Since client sends a deactivation request, they expect a deactivate
      ** sups event
      */

      /* Multi-sim: Resume ps data */
      cmsups_msim_resume_data(ss);

      break;

      #else
        /* Free the memory alloacted to sups_info_ptr, after sending the event */
        if (is_mem_free_req == TRUE && sups_ptr->num_pending_sups_cmds == 0)
        {  
          cmsups_mem_free_sups_info (sups_ptr);
          cmsups_free_uss_info_ptr(cm_rpt_ptr->cmd.erase_cc_entry_conf.as_id);
        }
        return;
      #endif 

    default:
      is_mem_free_req = FALSE;

      /* Abort_cc_req may be sent due to subs chg on other sub due to which stack needs to be shut down  
                 If we have received the abort_cc_cnf for any asid which means that stack will be shutting down
                so we need to clear the sups context and send the failure event to the clients to avoid them 
                 from waiting forever
           */
      
      #ifdef FEATURE_SUPS_RETRY
      if(cm_rpt_ptr->hdr.cmd == CM_ABORT_CC_CONF)
      {
         /* Clear the pending Sups command since the stack will be shutting down after stop mode.
         */
         /* NAS sends ASID in case of DUAL SIM , Get SS for given asubs_id */
         #ifdef FEATURE_MMODE_DUAL_SIM
         ss = cmph_map_subs_to_ss(cm_rpt_ptr->cmd.cm_abort_cc_cnf.as_id);
         #else
         ss = CM_SS_MAIN;
         #endif
         CM_MSG_HIGH_2("Recvd abort_cc_cnf on ASID=%d,ss=%d",cm_rpt_ptr->cmd.cm_abort_cc_cnf.as_id,ss);
         cmwsups_list_clear(ss);
      }
      #endif
      return;
  }

  sups_event_type = cmsups_conf_to_event_map(cm_rpt_ptr->hdr.cmd);
  
  if((is_break_in_mid == FALSE) && (sups_event_type != CM_SUPS_EVENT_NONE))
  {
    #ifdef FEATURE_SUPS_RETRY
    if(is_retry == FALSE)
    #endif
    cmsups_event( ss, sups_event_type);

    #ifdef FEATURE_CM_UPDATE_EF_CFIS
    if((sups_info_ptr != NULL) && sups_info_ptr->ss_success)
    {
      cmsups_check_and_update_cfis(sups_info_ptr, sups_event_type);
    }
    #else
    CM_MSG_HIGH_0( "Feature Disabled. No need to update EF-CFIS" );
    #endif

  }

  /* Free the memory alloacted to sups_info_ptr, after sending the event */
  if (is_mem_free_req == TRUE && sups_ptr->num_pending_sups_cmds == 0)
  {  
    cmsups_mem_free_sups_info (sups_ptr);
    cmsups_free_uss_info_ptr(cmph_map_cm_ss_to_subs(ss));
  }

  #ifdef FEATURE_SUPS_RETRY
  if(is_retry == FALSE && sups_list_info_ptr != NULL)
  {
    if(ss == CM_SS_HYBR_3)
    {
      sups_ptr->hybr_gw3_sups_info_list = NULL;
      cmwsups_mem_free_sups_list_info(sups_list_info_ptr,&(sups_ptr->hybr_gw3_sups_info_list_head));
    }
    else if(ss == CM_SS_HYBR_2)
    {
      sups_ptr->hybr_gw_sups_info_list = NULL;
      cmwsups_mem_free_sups_list_info(sups_list_info_ptr,&(sups_ptr->hybr_gw_sups_info_list_head));
    }
    else
    {
      sups_ptr->sups_info_list= NULL;
      cmwsups_mem_free_sups_list_info(sups_list_info_ptr,&(sups_ptr->sups_info_list_head));
    }
  }
  #endif

} /* cmsups_rpt_proc */
/*lint +esym(529, sups_ptr) not referenced */


/*===========================================================================

FUNCTION cmsups_call_control_complete_cb

DESCRIPTION
  Callback for Number Classification to call when call control is complete.

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsups_call_control_complete_cb(

  cm_call_id_type                         call_id,
    /* call id */

  cm_cc_status_e_type                     status,
    /* status of command */

  cm_call_type_e_type                     call_type,
    /* call type */

  const cm_orig_call_control_s_type      *cdma_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on cdma */

  const cm_orig_call_control_s_type      *gw_call_control_info_ptr,
    /* pointer to an info struct - NULL if not allowed on gw */

  const void                             *user_data
    /* data passed from original call */

)
/*lint -esym(715, cdma_call_control_info_ptr)*/
/*lint -esym(715, status)*/
/*lint -esym(715, call_type)*/
/*lint -esym(715, call_id)*/
/*lint -esym(529, return_status) not referenced */
/*lint -esym(765, cmsups_call_control_complete_cb)
** Can't be static - callback
*/
{
  cm_sups_cmd_s_type                     *sups_cmd_ptr;
  boolean                           return_status = FALSE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  
  CM_ASSERT(user_data                   != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  sups_cmd_ptr = (cm_sups_cmd_s_type *) user_data;

  /* Call control processing is complete
  */
  sups_cmd_ptr->is_call_control_processing_complete = TRUE;
  
  if(gw_call_control_info_ptr == NULL)
  {
     /* SS command is not allowed by CC (Reason could be GW subscription not available)
     ** Requeue the command to CM.
     */
      sups_cmd_ptr->cmd_info.cmd_err = 
                                      CM_SUPS_CMD_ERR_CALL_CONTROL_REJECTED;   

      CM_MSG_HIGH_0( "SS CC -- Not Allowed (GW Subsc may not be avail)"); 	  	
      cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );

      return;
  }

  sups_cmd_ptr->is_sim_cc_performed = gw_call_control_info_ptr->is_sim_cc_performed;

  switch( gw_call_control_info_ptr->call_control_result )
  {
    case CM_CC_RESULT_ALLOWED_NO_MODIFICATIONS:
      /* No modifications to the SS command.  Requeue the command to CM.
      */
      CM_MSG_HIGH_0( "SS Call Control -- Allowed No Modifications");
      sups_cmd_ptr->is_parsing_required = FALSE;
      sups_cmd_ptr->cc_result_alpha = gw_call_control_info_ptr->alpha;
      cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );
      break;

      case CM_CC_RESULT_ALLOWED_BUT_MODIFIED:
      case CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_SS:
      case CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_USSD:      
      if(sups_cmd_ptr->is_modem_cc_api == FALSE)
      {
        /* Modified to another SS/USS type.
        */
        CM_MSG_HIGH_1( "SS Call Control -- Allowed/Modified to %d", \
                      gw_call_control_info_ptr->sups_cmd );
        switch( gw_call_control_info_ptr->sups_cmd )
        {
           case CM_SUPS_CMD_ERASE:
             return_status = cm_sups_cmd_erase_cc( sups_cmd_ptr->cmd_cb_func,
                                   sups_cmd_ptr->data_block_ptr,
                                   sups_cmd_ptr->client_id,
                                   &gw_call_control_info_ptr->sups_params.erase_ss_params,
                                   NULL,
                                   TRUE );
             break;
           case CM_SUPS_CMD_REGISTER:
             return_status = cm_sups_cmd_register_cc( sups_cmd_ptr->cmd_cb_func,
                                      sups_cmd_ptr->data_block_ptr,
                                      sups_cmd_ptr->client_id,
                                      &gw_call_control_info_ptr->sups_params.reg_ss_params,
                                      NULL,
                                      TRUE );
             break;
           case CM_SUPS_CMD_ACTIVATE:
             return_status = cm_sups_cmd_activate_cc( sups_cmd_ptr->cmd_cb_func,
                                      sups_cmd_ptr->data_block_ptr,
                                      sups_cmd_ptr->client_id,
                                      &gw_call_control_info_ptr->sups_params.activate_ss_params,
                                      NULL,
                                      TRUE );
             break;
           case CM_SUPS_CMD_DEACTIVATE:
             return_status = cm_sups_cmd_deactivate_cc( sups_cmd_ptr->cmd_cb_func,
                                        sups_cmd_ptr->data_block_ptr,
                                        sups_cmd_ptr->client_id,
                                        &gw_call_control_info_ptr->sups_params.deactivate_ss_params,
                                        NULL,
                                        TRUE );
             break;
           case CM_SUPS_CMD_INTERROGATE:
             return_status = cm_sups_cmd_interrogate_cc( sups_cmd_ptr->cmd_cb_func,
                                         sups_cmd_ptr->data_block_ptr,
                                         sups_cmd_ptr->client_id,
                                         &gw_call_control_info_ptr->sups_params.interrogate_ss_params,
                                         NULL,
                                         TRUE );
             break;
           case CM_SUPS_CMD_PROCESS_USS:
             return_status = cm_sups_cmd_process_uss_cc( sups_cmd_ptr->cmd_cb_func,
                                         sups_cmd_ptr->data_block_ptr,
                                         sups_cmd_ptr->client_id,
                                         &gw_call_control_info_ptr->sups_params.uss_params,
                                         NULL,
                                         TRUE );
             break;
           case CM_SUPS_CMD_REG_PASSWORD:
             return_status = cm_sups_cmd_reg_password_cc( sups_cmd_ptr->cmd_cb_func,
                                          sups_cmd_ptr->data_block_ptr,
                                          sups_cmd_ptr->client_id,
                                          &gw_call_control_info_ptr->sups_params.reg_pwd_params,
                                          NULL,
                                          TRUE );
             break;
           case CM_SUPS_CMD_RELEASE:
             return_status = cm_sups_cmd_release_cc( sups_cmd_ptr->cmd_cb_func,
                                     sups_cmd_ptr->data_block_ptr,
                                     sups_cmd_ptr->client_id,
                                     &gw_call_control_info_ptr->sups_params.release_ss_params,
                                     NULL,
                                     TRUE );
             break;
           default:
             CM_ERR_1( "unsupported mod to SS=%d", gw_call_control_info_ptr->sups_cmd);
        }
        if (!return_status)
        {
          CM_ERR_0( "Unable to start new SUPS operation");
        }

      }
      else
      {
        /* Modified to another SS type.
        */
        CM_MSG_HIGH_0( "SS Call Control -- Allowed,Modified");
        if(gw_call_control_info_ptr->call_control_result ==
                                  CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_USSD)
        {
          /* Modified to USSD type.
          */
  
          sups_cmd_ptr->is_parsing_required = FALSE;
          sups_cmd_ptr->cc_result_alpha = 
                                            gw_call_control_info_ptr->alpha;
          sups_cmd_ptr->dialed_digits = gw_call_control_info_ptr->num;
          sups_cmd_ptr->cmd_info.uss_data = gw_call_control_info_ptr->sups_params.uss_params.uss_data;
          sups_cmd_ptr->cc_result = CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_USSD;
          sups_cmd_ptr->cmd = CM_SUPS_CMD_PROCESS_USS;
          cm_cmd_queue((cm_cmd_type*)sups_cmd_ptr );
          CM_MSG_HIGH_1 ("SS CC -- Allowed,Modified to USSD. Posted CM_SUPS_CMD_PROCESS_USS. with cc result %d",
                        sups_cmd_ptr->cc_result );
        }
        else
        {
          /* Mark that string needs to be parsed and then
          ** corresponding processing of Sups cmd needs to be done
          */
          
          sups_cmd_ptr->is_parsing_required = TRUE;
          sups_cmd_ptr->cc_result_alpha = 
                                            gw_call_control_info_ptr->alpha;
          sups_cmd_ptr->dialed_digits = gw_call_control_info_ptr->num;
          sups_cmd_ptr->cc_result = CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_SS;
          cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );

          CM_MSG_HIGH_1 (" SS CC -- Allowed, Modified to SS. Posted sups cmd. with cc result %d ", 
                       sups_cmd_ptr->cc_result );
        }
      }
      break;

    case CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_VOICE:
      if( sups_cmd_ptr->cmd_cb_func != NULL )
      {
        sups_cmd_ptr->cmd_cb_func( sups_cmd_ptr->data_block_ptr,
                                       sups_cmd_ptr->cmd,
                                       CM_SUPS_CMD_ERR_NOERR );
      }
  
      sups_cmd_ptr->cc_result = CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_VOICE;
      sups_cmd_ptr->cc_result_alpha = gw_call_control_info_ptr->alpha;
      sups_cmd_ptr->dialed_digits = gw_call_control_info_ptr->num;
  
      cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );
	  
      CM_MSG_HIGH_1 ("updated voice call num. SS Call Control -- Allowed/Voice. Posted sups cmd. with cc result %d ", 
                       sups_cmd_ptr->cc_result );


      break;

    case CM_CC_RESULT_NOT_ALLOWED:
      /* SS command is not allowed by CC.  Requeue the command to CM.
      */
      sups_cmd_ptr->cmd_info.cmd_err = 
                                      CM_SUPS_CMD_ERR_CALL_CONTROL_REJECTED;
      sups_cmd_ptr->cmd_info.cmd_err_cause = 
       (cm_sups_cmd_err_cause_e_type)gw_call_control_info_ptr->error_cause;
      sups_cmd_ptr->cc_result_alpha = gw_call_control_info_ptr->alpha;
      sups_cmd_ptr->cc_result = CM_CC_RESULT_NOT_ALLOWED;

      CM_MSG_HIGH_0( "SS Call Control -- Not Allowed");

      cm_cmd_queue( (cm_cmd_type*)sups_cmd_ptr );
      break;

    default:
      CM_ERR_1( "Unexpected CC Result %d", \
                      (int)gw_call_control_info_ptr->call_control_result );
  }
}  /* cmsups_call_control_complete_cb */
/*lint +esym(715, cdma_call_control_info_ptr)*/
/*lint +esym(715, status)*/
/*lint +esym(715, call_type)*/
/*lint +esym(715, call_id)*/
/*lint +esym(529, return_status) not referenced */

/*===========================================================================

FUNCTION cmwsups_process_sups_to_voice

DESCRIPTION
  Handle to sups to voice processing

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmwsups_process_sups_to_voice (
	
    cm_ss_e_type     ss,
     /* ss type */
    const cm_sups_cmd_s_type	 *sups_cmd_ptr
     /* command pointer info */
)
/*lint -esym(715, ss)*/
/*lint -esym(529, ss)*/
{
  
  cm_gw_cs_orig_params_s_type      *gw_orig_params_ptr;
  cm_orig_alpha_s_type              cm_alpha;
  cm_call_id_type                   call_id;
  cm_sups_info_s_type              *sups_info_ptr = NULL;
  cmsups_s_type             *sups_ptr       = cmsups_ptr();

  /* Since cm_gw_cs_orig_params_s_type is so big, let's allocate memory
  ** dynamically and deallocate when we are done sending it to the origination.
  */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(sups_cmd_ptr != NULL);
  CM_ASSERT( sups_cmd_ptr->cmd_type == CM_CMD_TYPE_SUPS );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  gw_orig_params_ptr = (cm_gw_cs_orig_params_s_type*) cm_mem_malloc
                               ( sizeof (cm_gw_cs_orig_params_s_type ) );
  

  sups_info_ptr = cmsups_ensure_sups_info_ptr_asubs_id(cmph_map_cm_ss_to_subs(ss));
  
  cm_util_default_gw_orig_params_per_sub( gw_orig_params_ptr, cmph_map_cm_ss_to_subs(ss));
  gw_orig_params_ptr->call_control_completed = TRUE;
  
  cm_alpha.buf[0] = '\0';
  cm_alpha.len = 0;
  
  if (!cm_mm_call_cmd_orig_per_subs ( NULL,
                            sups_cmd_ptr->data_block_ptr,
                            sups_cmd_ptr->client_id,
                            CM_CALL_TYPE_VOICE,
                            CM_SRV_TYPE_AUTOMATIC,
                            sups_cmd_ptr->cmd_info.cmd_subs_id,
                            NULL,                               /* Calling Number */
                            &sups_cmd_ptr->dialed_digits,   /* Called Number */
                            &cm_alpha,                          /* alpha_buf */
                            NULL,                               /* cdma_orig_params_ptr */
                            gw_orig_params_ptr,                 /* gw_cs_orig_params_ptr */
                            NULL,                               /* gw_ps_orig_params_ptr */
                            &call_id                            /* return call id ptr */
                          ))
  {
    CM_ERR_0( "Unable to start Orig for SUPS call control!");
  }
  else
  {
    CM_MSG_HIGH_0 ("call_cmd_orig called...");
    sups_info_ptr->mod_ss_params.call_id = call_id;
    sups_info_ptr->mod_ss_params.call_control_result = 
                                           CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_VOICE;
    sups_info_ptr->ss_ref                = sups_cmd_ptr->cmd_info.ss_ref;
    CM_MSG_HIGH_2 ("CM_SUPS_EVENT_SS_MOD_TO_ORIG: call_id: %d, ss_ref: %d",
                   call_id, sups_info_ptr->ss_ref);   

    cmsups_event (ss, CM_SUPS_EVENT_SS_MOD_TO_ORIG);  
  }
  cm_mem_free( gw_orig_params_ptr );
  if(sups_ptr->num_pending_sups_cmds == 0)
  {
    /* Free sups info ptr as CM is not waiting for any CNF from NAS*/
    cmsups_mem_free_sups_info (sups_ptr);
    cmsups_free_uss_info_ptr(cmph_map_cm_ss_to_subs(ss));
  }
}
/*lint +esym(715, ss)*/
/*lint +esym(529, ss)*/

/*===========================================================================

FUNCTION cmsups_free_sups

DESCRIPTION
  Delete the recently added subs from the list

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsups_free_sups ( cm_ss_e_type ss )
{
  cmsups_s_type             *sups_ptr       = cmsups_ptr();

  CM_ASSERT( sups_ptr != NULL );

  /* free the memory */ 
   cmsups_mem_free_sups_info (sups_ptr);

   #ifdef FEATURE_SUPS_RETRY
   
   #if defined FEATURE_MMODE_TRIPLE_SIM || defined FEATURE_MMODE_SXLTE_G
   if(ss == CM_SS_HYBR_3)
   {
     if(sups_ptr->hybr_gw3_sups_info_list != NULL)
     {
       cmwsups_mem_free_sups_list_info(sups_ptr->hybr_gw3_sups_info_list,&(sups_ptr->hybr_gw3_sups_info_list_head));
     }
     sups_ptr->hybr_gw3_sups_info_list = NULL;
   }
   else
   #endif
   #ifdef FEATURE_MMODE_DUAL_SIM
   if(ss == CM_SS_HYBR_2)
   {
     if(sups_ptr->hybr_gw_sups_info_list != NULL)
     {
       cmwsups_mem_free_sups_list_info(sups_ptr->hybr_gw_sups_info_list,&(sups_ptr->hybr_gw_sups_info_list_head));
     }
     sups_ptr->hybr_gw_sups_info_list = NULL;
   }
   else
   #endif
   {
     if(sups_ptr->sups_info_list != NULL)
     {
       cmwsups_mem_free_sups_list_info(sups_ptr->sups_info_list,&(sups_ptr->sups_info_list_head));
     }
     sups_ptr->sups_info_list = NULL;
   }
  #endif
 }

/*===========================================================================

FUNCTION cmsups_client_cmd_proc

DESCRIPTION
  Process clients' sups commands

DEPENDENCIES
  SUPS object must have already been initialized with
  cmsups_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmsups_client_cmd_proc(

    cm_sups_cmd_s_type  *sups_cmd_ptr
        /* pointer to a CM command */
)
/*lint -esym(550, parse_result) */
{

  cmsups_s_type             *sups_ptr       = cmsups_ptr();
      /* Point at sups object */

  cm_sups_cmd_err_e_type     sups_cmd_err = CM_SUPS_CMD_ERR_NOERR;

  boolean                    is_starting_cc_proc = FALSE;

  cm_ss_e_type               ss                  = CM_SS_MAIN;

  dword                      uptime              = time_get_uptime_secs();

  sys_modem_as_id_e_type    *asubs_id            = &sups_cmd_ptr->cmd_info.cmd_subs_id;

  #ifdef FEATURE_IP_CALL
  cmsups_cmd_route_e_type sups_cmd_route;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 

  CM_ASSERT( sups_cmd_ptr != NULL );
  CM_ASSERT( sups_ptr != NULL );
  CM_ASSERT( sups_cmd_ptr->cmd_type == CM_CMD_TYPE_SUPS );

  CM_MSG_HIGH_2("oprt_mode_online: cmsups_client_cmd_proc(), cmutil_num_of_subs_avail %d oprt_mode_send_time %d",
                cmutil_num_of_subs_avail(), cmph_ptr()->oprt_mode_send_time );
  if (cmutil_num_of_subs_avail() && cmph_ptr()->oprt_mode_send_time != CMPH_MAX_UPTIME)
  {
    cmph_send_oprt_mode_online_to_mmoc();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_MMODE_DUAL_SIM                                        
  if(cmph_is_msim() 
      &&
      ((sups_cmd_ptr->cmd == CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES)
       ||(sups_cmd_ptr->cmd == CM_SUPS_CMD_LCS_MOLR_COMPLETED)
       ||(sups_cmd_ptr->cmd == CM_SUPS_CMD_LCS_MOLR))
      )
  {
    if( cmph_ptr()->internal_standby_pref != SYS_MODEM_DS_PREF_DUAL_STANDBY
         #if defined FEATURE_MMODE_TRIPLE_SIM
          &&  cmph_ptr()->internal_standby_pref != SYS_MODEM_DS_PREF_TRIPLE_STANDBY
         #endif
       )
    {
      #if defined FEATURE_MMODE_TRIPLE_SIM
      if (cmph_ptr()->active_subs & SYS_MODEM_AS_ID_3_MASK)
	  {
       sups_cmd_ptr->cmd_info.cmd_subs_id          = SYS_MODEM_AS_ID_3;
	  }
	  else
      #endif
	  #ifdef FEATURE_MMODE_DUAL_SIM
	  if(cmph_ptr()->active_subs & SYS_MODEM_AS_ID_2_MASK)
	  {
       sups_cmd_ptr->cmd_info.cmd_subs_id          = SYS_MODEM_AS_ID_2;
	  }
	  else
      #endif
	  {
       sups_cmd_ptr->cmd_info.cmd_subs_id          = SYS_MODEM_AS_ID_1;
	  }
    }
    else
    {
      sups_cmd_ptr->cmd_info.cmd_subs_id          = cmph_ptr()->default_data_subs;
    }
  }
  CM_MSG_HIGH_1( "cmd_subs_id updated: %d", sups_cmd_ptr->cmd_info.cmd_subs_id);
  #endif 
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (  ( SYS_MODEM_AS_ID_1 == sups_cmd_ptr->cmd_info.cmd_subs_id) && 
        (FALSE == cmph_ptr()->is_gwl_subscription_available) &&
        (TRUE == cmph_ptr()->is_cdma_subscription_available) &&
        cmph_is_msim())
  {
     cmsups_client_cmd_err( sups_cmd_ptr, CM_SUPS_CMD_ERR_SUBS_ID_P );
     CM_ERR_0 ("cmsups_client_cmd_proc | Invalid subscription \n");/*lint -e{527}*/
     return; /*lint -e{527}*/
  }

  #ifdef FEATURE_MMODE_DUAL_SIM                                     
  if(sups_cmd_ptr->cmd_info.cmd_subs_id == SYS_MODEM_AS_ID_NO_CHANGE &&
     cmph_is_msim())
  {
    if((cmph_ptr()->internal_standby_pref != SYS_MODEM_DS_PREF_DUAL_STANDBY
         #ifdef FEATURE_MMODE_TRIPLE_SIM
          && cmph_ptr()->internal_standby_pref != SYS_MODEM_DS_PREF_TRIPLE_STANDBY
         #endif
       )
		&&(cmph_ptr()->active_subs == SYS_MODEM_AS_ID_2_MASK))
    {
      sups_cmd_ptr->cmd_info.cmd_subs_id = SYS_MODEM_AS_ID_2;
    }
    else if((cmph_ptr()->internal_standby_pref != SYS_MODEM_DS_PREF_DUAL_STANDBY
              && (cmph_ptr()->internal_standby_pref != SYS_MODEM_DS_PREF_TRIPLE_STANDBY)
             )
		&&(cmph_ptr()->active_subs == SYS_MODEM_AS_ID_3_MASK))
    {
      sups_cmd_ptr->cmd_info.cmd_subs_id = SYS_MODEM_AS_ID_3;
    }
    else
    {
     /* default it to MAIN if no as_id is passed for dual standby  */
     sups_cmd_ptr->cmd_info.cmd_subs_id =  SYS_MODEM_AS_ID_1;
    }
  }
  #endif 
  
  /* Get SS for given asubs_id */
  ss = cmph_map_subs_to_ss( *asubs_id );

  if ( sups_cmd_ptr->cmd_info.cmd_subs_id == SYS_MODEM_AS_ID_NO_CHANGE )
  {
    sups_cmd_ptr->cmd_info.cmd_subs_id = SYS_MODEM_AS_ID_1;
  }

  if ( cmph_is_subs_feature_mode_sglte( *asubs_id ) )
  {
    cm_ss_e_type domain_ss = CM_SS_NONE;
    cm_sups_cmd_err_e_type cmd_err = CM_SUPS_CMD_ERR_NONE;

    cmd_err = cmwsups_sglte_determine_ss_for_sups_cmd( *asubs_id, sups_cmd_ptr->cmd,
                                                       &domain_ss );
    if( cmd_err != CM_SUPS_CMD_ERR_NONE )
    {
      cmsups_client_cmd_err( sups_cmd_ptr, cmd_err );
      return;
    }
    else
    {
      ss = domain_ss;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Clear info part of sups object 
  */
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if( ss == CM_SS_HYBR_3 )
  {

    /* Allocate memory for sups_info_ptr */
    if (sups_ptr->hybr_gw3_sups_info == NULL)
    {
      sups_ptr->hybr_gw3_sups_info = cmsups_info_alloc(); 
      cmsups_init_sups_info(sups_ptr->hybr_gw3_sups_info);
      CM_MSG_HIGH_0( "Memory Allocated for Hybr_gw3_sups_info");
    }
    sups_ptr->hybr_gw3_sups_info->asubs_id = cmph_ptr()->hybr_3_stack_info.asubs_id;
    
  }
  else
  #endif
  if( ss == CM_SS_HYBR_2 )
  {

    /* Allocate memory for sups_info_ptr */
    if (sups_ptr->hybr_gw_sups_info == NULL)
    {
      sups_ptr->hybr_gw_sups_info = cmsups_info_alloc(); 
      cmsups_init_sups_info(sups_ptr->hybr_gw_sups_info);
      CM_MSG_HIGH_0( "Memory Allocated for Hybr_gw_sups_info");
    }
    sups_ptr->hybr_gw_sups_info->asubs_id = cmph_ptr()->hybr_2_stack_info.asubs_id;
    
  }
  else
  {

    /* Allocate memory for sups_info_ptr */
    if (sups_ptr->sups_info == NULL)
    {
      sups_ptr->sups_info = cmsups_info_alloc(); 
      cmsups_init_sups_info(sups_ptr->sups_info);  
      CM_MSG_HIGH_0( "Memory Allocated for sups_info");
 
    }
    sups_ptr->sups_info->asubs_id = cmph_ptr()->main_stack_info.asubs_id;
  }

  (void) cmsups_get_uss_info_ptr( *asubs_id, sups_cmd_ptr->cmd);


  #ifdef FEATURE_SUPS_RETRY  /* SUPS RETRY */
  

  /*
  ** SUPS retry feature is currently NOT supported for IP SUPS commands
  */
  #ifdef FEATURE_IP_CALL
  sups_cmd_route = cmipsups_route_check(sups_cmd_ptr->cmd, *asubs_id);
  if ((sups_cmd_route != CMSUPS_CMD_ALLOW_IMS) &&
    !(cmipsups_is_ussd_sups_cmd(sups_cmd_ptr->cmd) && (sups_cmd_route == CMSUPS_CMD_REJECT)))
  #endif
  {
    if( ss == CM_SS_HYBR_3 )
    {
      if (sups_cmd_ptr->cmd == CM_SUPS_CMD_PROCESS_USS &&
      (cmwsups_is_uss_cmd_already_present(ss) != sups_cmd_ptr->cmd_info.ss_ref) &&
      (cmwsups_is_uss_cmd_already_present(ss) != 0))
      {
        CM_MSG_HIGH_0("One sups cmd already pending...rejecting next one");
        cmsups_client_cmd_err( sups_cmd_ptr, CM_SUPS_CMD_ERR_OTHER );	  
        return;
      }
      else
      {
        /* Allocate memory for sups_info_ptr */
        if(!((sups_cmd_ptr->cmd == CM_SUPS_CMD_GET_PASSWORD_RES) ||
              (sups_cmd_ptr->cmd == CM_SUPS_CMD_USS_NOTIFY_RES) ||
              (sups_cmd_ptr->cmd == CM_SUPS_CMD_USS_RES) ||
              (sups_cmd_ptr->cmd == CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES) ))
        {
          /* ss ref is to be used for MO sups command to fetch the corresponding record */
           sups_ptr->hybr_gw3_sups_info_list= cmwsups_info_list_find_alloc(ss, sups_cmd_ptr->cmd_info.ss_ref);
           if(sups_ptr->hybr_gw3_sups_info_list != NULL)
              sups_ptr->hybr_gw3_sups_info_list->sr_start_uptime = uptime;
        }
      }
    }
    else if( ss == CM_SS_HYBR_2 )
  {
    if (sups_cmd_ptr->cmd == CM_SUPS_CMD_PROCESS_USS &&
	   (cmwsups_is_uss_cmd_already_present(ss) != sups_cmd_ptr->cmd_info.ss_ref) &&
	   (cmwsups_is_uss_cmd_already_present(ss) != 0))
    {
      CM_MSG_HIGH_0("One sups cmd already pending...rejecting next one");
      cmsups_client_cmd_err( sups_cmd_ptr, CM_SUPS_CMD_ERR_OTHER );	  
	  return;
    }
	else
	{
    /* Allocate memory for sups_info_ptr */
    if(!((sups_cmd_ptr->cmd == CM_SUPS_CMD_GET_PASSWORD_RES) ||
          (sups_cmd_ptr->cmd == CM_SUPS_CMD_USS_NOTIFY_RES) ||
          (sups_cmd_ptr->cmd == CM_SUPS_CMD_USS_RES) ||
          (sups_cmd_ptr->cmd == CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES) ))
    {
      /* ss ref is to be used for MO sups command to fetch the corresponding record */
       sups_ptr->hybr_gw_sups_info_list= cmwsups_info_list_find_alloc(ss, sups_cmd_ptr->cmd_info.ss_ref);
       if(sups_ptr->hybr_gw_sups_info_list != NULL)
          sups_ptr->hybr_gw_sups_info_list->sr_start_uptime = uptime;
    }
  }
  }
  else
  {
    if (sups_cmd_ptr->cmd == CM_SUPS_CMD_PROCESS_USS &&
    (cmwsups_is_uss_cmd_already_present(ss) != sups_cmd_ptr->cmd_info.ss_ref) &&
    (cmwsups_is_uss_cmd_already_present(ss) != 0))
    {
      CM_MSG_HIGH_0("One sups cmd already pending...rejecting next one");  
      cmsups_client_cmd_err( sups_cmd_ptr, CM_SUPS_CMD_ERR_OTHER );
      return;	  
  }
  else
  {
    /* Allocate memory for sups_info_ptr */
    if(!((sups_cmd_ptr->cmd == CM_SUPS_CMD_GET_PASSWORD_RES) ||
          (sups_cmd_ptr->cmd == CM_SUPS_CMD_USS_NOTIFY_RES) ||
          (sups_cmd_ptr->cmd == CM_SUPS_CMD_USS_RES) ||
          (sups_cmd_ptr->cmd == CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES) ))
    {
       sups_ptr->sups_info_list= cmwsups_info_list_find_alloc(ss, sups_cmd_ptr->cmd_info.ss_ref);
       if(sups_ptr->sups_info_list != NULL)
          sups_ptr->sups_info_list->sr_start_uptime = uptime;
    }
  }
  }
  }
  #endif

  /* If call control is needed, pass it along to the number classification
  ** routines.
  */
  CM_MSG_HIGH_3 ("cc complete: %d dial digit len: %d cc_res: %d",
                sups_cmd_ptr->is_call_control_processing_complete,
                sups_cmd_ptr->dialed_digits.len, 
                sups_cmd_ptr->cc_result );


  /* Send call control result to clients */
  if(sups_cmd_ptr->is_call_control_processing_complete && 
     sups_cmd_ptr->is_sim_cc_performed)
  {
    /* save a copy in the sups object and let the clients know of call control result and alpha */
    cmsups_cmd_copy_fields( sups_ptr, sups_cmd_ptr );
    cmsups_event (ss, CM_SUPS_EVENT_CC_RESULT_INFO);
  }

  if (sups_cmd_ptr->is_call_control_processing_complete == FALSE &&
      sups_cmd_ptr->dialed_digits.len != 0
     )
  {
    cm_cc_generic_params_s_type   cc_generic_params;
    cc_generic_params.is_fdn_to_check = TRUE;
      
    is_starting_cc_proc = cm_nc_call_control_processing
    (
      CM_CALL_ID_UNASSIGNED,
      CM_CC_SUBSCRIPTION_SOURCE_NONE,       /* SUPS for GW only */
      cmph_get_gw_subscription_source ( ss ),
      CM_CALL_TYPE_SUPS,
      &sups_cmd_ptr->dialed_digits,
      (void*) sups_cmd_ptr,
      cmsups_call_control_complete_cb,
      NULL,
      &cc_generic_params
    );

    if (is_starting_cc_proc)
    {
      CM_MSG_HIGH_0("SUPS: Starting Call Control Processing .... ");
    }
    else
    {
      CM_MSG_HIGH_0 ("cm_nc_call_control_processing returned FALSE");
      /*lint -save -e527 Unreachable
      **     When CM_DEBUG is on, CM_ERR does exit(1)
      */
      sups_cmd_err = CM_SUPS_CMD_ERR_CALL_CONTROL_FAILURE;
      /* Since Number classification returned false, it's upto CM to free
      ** the command buffer that was sent to it.
      */
      sups_cmd_ptr->is_reused = FALSE;
      /*lint -restore */
    }

    /* If there was a call control error, the command is done so notify the client.
    ** Otherwise, don't notify the client.
    */
    if (sups_cmd_err != CM_SUPS_CMD_ERR_NOERR )
    {
      cmsups_client_cmd_err( sups_cmd_ptr, sups_cmd_err );
      cmsups_free_sups(ss);
      cmsups_free_uss_info_ptr(*asubs_id);
    }
    return;
  }
  else if (sups_cmd_ptr->is_call_control_processing_complete == TRUE &&
       sups_cmd_ptr->cc_result == CM_CC_RESULT_ALLOWED_BUT_MODIFIED_TO_VOICE)
  {
     /* Check if CC modified sups call to voice */
     /*
     ** Notify clients of command processing status.
     ** If error found, return.
     */
     cmsups_client_cmd_err( sups_cmd_ptr, sups_cmd_err );

     CM_MSG_HIGH_0 ("sups to voice processing");
     cmwsups_process_sups_to_voice(ss, sups_cmd_ptr); 
     return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Start by checking whether this is a valid command
  ** relative to the current state of the call/phone.
  */
  sups_cmd_err = cmsups_client_cmd_check( sups_cmd_ptr );

  /*
  ** Notify clients of command processing status.
  ** If error found, return.
  */
  cmsups_client_cmd_err( sups_cmd_ptr, sups_cmd_err );
  if( sups_cmd_err != CM_SUPS_CMD_ERR_NOERR )
  {
    cmsups_free_sups(ss);
    cmsups_free_uss_info_ptr(*asubs_id);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, no errors were found,
  ** parse string, if specified, and then
  ** forward the command to Lower Layer.
  */

  if(sups_cmd_ptr->is_parsing_required == TRUE)
  {
    /* If we reach here, sups cmd params are not to be
    ** considered, but dialled string needs to be parsed
    ** to find out the sups cmd and its parameters
    */
    cmutil_parse_sups_result_e_type parse_result =
                                              CMUTIL_PARSE_SUPS_RESULT_NONE;
        
    cmutil_sups_params_s_type sups_cmd_params;

    CM_MSG_HIGH_0("Invoking parse utility");
    /* invoke parsing utility */
    if(sups_cmd_ptr->dialed_digits.len > CM_MAX_NUMBER_CHARS)
    {
      /* Error in parsing length */
      CM_ERR_1(" Parsing length is out of boundary %d",
                     sups_cmd_ptr->dialed_digits.len);
      return;
    }

    parse_result = cmutil_parse_sups_string(
                       sups_cmd_ptr->dialed_digits,
                       sups_cmd_ptr->dialed_digits.len,
                     &sups_cmd_params);
    if( parse_result != CMUTIL_PARSE_SUPS_RESULT_SUCCESS )
    {
      /* Error in parsing */
      CM_ERR_1("Parsing error %d, sending client originated Supplementary service request",
                     parse_result);
      /* Send the original Sups command to LL */
    }
    else
    {
      sups_cmd_ptr->cmd = sups_cmd_params.sups_cmd;
      #ifdef FEATURE_CM_DEBUG_BUFFER
      cmdbg_add_to_dbg_buffer(CM_BUFF_SUPS_CMD_TYPE, sups_cmd_ptr->cmd, (void*)sups_cmd_ptr);
      #endif
      switch(sups_cmd_ptr->cmd)
      {
        case CM_SUPS_CMD_ACTIVATE:
          sups_cmd_ptr->cmd_info.ss_code =
                      sups_cmd_params.sups_params.activate_ss_params.ss_code;

          sups_cmd_ptr->cmd_info.basic_service =
                sups_cmd_params.sups_params.activate_ss_params.basic_service;

          sups_cmd_ptr->cmd_info.ss_password =
                       sups_cmd_params.sups_params.activate_ss_params.ss_pwd;

          sups_cmd_ptr->cmd_info.ss_operation = activateSS;                       

          break;

        case CM_SUPS_CMD_DEACTIVATE:
          sups_cmd_ptr->cmd_info.ss_code =
                    sups_cmd_params.sups_params.deactivate_ss_params.ss_code;

          sups_cmd_ptr->cmd_info.basic_service =
              sups_cmd_params.sups_params.deactivate_ss_params.basic_service;

          sups_cmd_ptr->cmd_info.ss_password =
                     sups_cmd_params.sups_params.deactivate_ss_params.ss_pwd;

          if (sups_cmd_ptr->cmd_info.ss_code == (byte)ccbs)
          {
            sups_cmd_ptr->cmd = CM_SUPS_CMD_ERASE_CC_ENTRY;
            sups_cmd_ptr->cmd_info.ccbs_indx  =
                  sups_cmd_params.sups_params.deactivate_ss_params.ccbs_indx;
          }
          sups_cmd_ptr->cmd_info.ss_operation = deactivateSS; 
          break;

          case CM_SUPS_CMD_INTERROGATE:
            sups_cmd_ptr->cmd_info.ss_code =
                   sups_cmd_params.sups_params.interrogate_ss_params.ss_code;

            sups_cmd_ptr->cmd_info.basic_service =
            sups_cmd_params.sups_params.interrogate_ss_params.basic_service;

            sups_cmd_ptr->cmd_info.ss_operation = interrogateSS;
            break;

          case CM_SUPS_CMD_ERASE:
            sups_cmd_ptr->cmd_info.ss_code =
                       sups_cmd_params.sups_params.erase_ss_params.ss_code;

            sups_cmd_ptr->cmd_info.basic_service =
                sups_cmd_params.sups_params.erase_ss_params.basic_service;

            sups_cmd_ptr->cmd_info.ss_operation = eraseSS;
            break;

          case CM_SUPS_CMD_REGISTER:
            sups_cmd_ptr->cmd_info.ss_code =
                       sups_cmd_params.sups_params.reg_ss_params.ss_code;

            sups_cmd_ptr->cmd_info.basic_service =
                sups_cmd_params.sups_params.reg_ss_params.basic_service;

            sups_cmd_ptr->cmd_info.forwarded_to_number =
                        sups_cmd_params.sups_params.reg_ss_params.fwd_to_num;

            sups_cmd_ptr->cmd_info.forwarded_to_subaddress =
                    sups_cmd_params.sups_params.reg_ss_params.fwd_to_subaddr;

            sups_cmd_ptr->cmd_info.nr_timer =
                          sups_cmd_params.sups_params.reg_ss_params.nr_timer;

            sups_cmd_ptr->cmd_info.ss_operation = registerSS;
            break;

          case CM_SUPS_CMD_REG_PASSWORD:
            sups_cmd_ptr->cmd_info.code =
                       sups_cmd_params.sups_params.reg_pwd_params.ss_code;

            sups_cmd_ptr->cmd_info.ss_password =
                       sups_cmd_params.sups_params.reg_pwd_params.ss_old_pwd;

            sups_cmd_ptr->cmd_info.ss_new_password =
                      sups_cmd_params.sups_params.reg_pwd_params.ss_new_pwd;

            sups_cmd_ptr->cmd_info.ss_new_password_again =
                 sups_cmd_params.sups_params.reg_pwd_params.ss_new_pwd_again;

            sups_cmd_ptr->cmd_info.ss_operation = registerPassword;
            break;

          case CM_SUPS_CMD_PROCESS_USS:
            sups_cmd_ptr->cmd_info.ss_operation =
                                               processUnstructuredSS_Request;

            *(&(sups_cmd_ptr->cmd_info.uss_data)) = *(&(sups_cmd_params.sups_params.uss_params.uss_data));
            break;

          default:
            /* Send the original Sups command to LL */
            break;
      }
    }

    /* Just to be safe, mark this field FALSE */
    sups_cmd_ptr->is_parsing_required = FALSE;

  }
  else /* Parsing is not required */
  {
    CM_MSG_HIGH_0 ("USS. Parsing not req. ss_op processUnstructuredSS_Request");
    if ( sups_cmd_ptr->cmd == CM_SUPS_CMD_PROCESS_USS)
    {
      sups_cmd_ptr->cmd_info.ss_operation = processUnstructuredSS_Request;
      /* Reset ss_code to 0 in case of USS */	  
      sups_cmd_ptr->cmd_info.ss_code      = 0;
      CM_MSG_HIGH_0 ("ss_op: processUnstructuredSS_Request ss code: 0");
    }
  }
 
  /* forward to lower layer */
  cmsups_client_cmd_forward_ll( sups_ptr, sups_cmd_ptr );

  /* Increase the pending sups count */
  sups_ptr->num_pending_sups_cmds++ ;


  /* Not freeing memory of sups_ptr here is intentional.
  ** It will be freed once CM gets conf. report from NAS.
  */

}
/*lint +esym(550, parse_result) */

#ifdef FEATURE_SUPS_RETRY
/*===========================================================================

FUNCTION cmwsups_send_sups_fail_event

DESCRIPTION
  Process timer events

DEPENDENCIES
  Sups object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmwsups_send_sups_fail_event(
cm_ss_e_type                  ss,
cmwsups_info_list_s_type* sups_list
)
{

  cm_sups_info_s_type *sups_info_ptr = NULL;
  cm_sups_event_e_type  sups_event = CM_SUPS_EVENT_NONE;
  
  /* If SUPS info ptr for given SS is NULL, alloc again now
  */
  sups_info_ptr = cmsups_ensure_sups_info_ptr_asubs_id(cmph_map_cm_ss_to_subs(ss));

  /* Update sups info with the cmd info if ss_ref does not match */
  if(sups_info_ptr->ss_ref != sups_list->sups_cmd_info.ss_ref)
  {
    cmsups_cmd_copy_fields_into_info(sups_info_ptr,&(sups_list->sups_cmd_info));
  }

  /* Set success as FALSE */
  sups_info_ptr->ss_success = FALSE;
  
  /* Send SUPS event now */
  
  switch(sups_list->cmd)
  {
    case CM_SUPS_CMD_REGISTER:
      sups_event = CM_SUPS_EVENT_REGISTER_CONF;
      break;
    
    case CM_SUPS_CMD_ERASE:
      sups_event = CM_SUPS_EVENT_ERASE_CONF;
      break;

    case CM_SUPS_CMD_ACTIVATE:
      sups_event = CM_SUPS_EVENT_ACTIVATE_CONF;
      break;

    case CM_SUPS_CMD_DEACTIVATE:
      sups_event = CM_SUPS_EVENT_DEACTIVATE_CONF;
      break;

    case CM_SUPS_CMD_INTERROGATE:
      sups_event = CM_SUPS_EVENT_INTERROGATE_CONF;
      break;

    case CM_SUPS_CMD_REG_PASSWORD:
      sups_event = CM_SUPS_EVENT_REG_PASSWORD_CONF;
      break;

    case CM_SUPS_CMD_PROCESS_USS:
      sups_event = CM_SUPS_EVENT_PROCESS_USS_CONF;
      break;

    case CM_SUPS_CMD_LCS_MOLR:
      sups_event = CM_SUPS_EVENT_LCS_MOLR_CONF;
      break;

    case CM_SUPS_CMD_ERASE_CC_ENTRY:
      sups_event = CM_SUPS_EVENT_DEACTIVATE_CONF;
      break;

    default:
      sups_event = CM_SUPS_EVENT_NONE;
  }

  if(sups_event != CM_SUPS_EVENT_NONE)
  {
    cmsups_event(ss, sups_event);
  }

  return;
}

/*===========================================================================

FUNCTION CMWSUPS_LIST_CLEAR

DESCRIPTION
  Clear all SUPS list for given SS

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated sups info struct.

SIDE EFFECTS
  none

===========================================================================*/
static void cmwsups_list_clear( 
    cm_ss_e_type ss
    /* SS for this record is required
    */
)
{
   cmwsups_info_list_s_type       *list_ptr = NULL;
   cmwsups_info_list_s_type       *temp_ptr = NULL;

   /* ss = CM_SS_HYBR_2 is only valid for multi-sim */
   if ((ss == CM_SS_HYBR_2 || ss == CM_SS_HYBR_3) \
         && cmph_is_ssim())
   {
     return;
   }

   if(ss == CM_SS_HYBR_3)
   {
     list_ptr = cmsups_ptr()->hybr_gw3_sups_info_list_head;
   }
   else if(ss == CM_SS_HYBR_2)
   {
     list_ptr = cmsups_ptr()->hybr_gw_sups_info_list_head;
   }
   else
   {
     list_ptr = cmsups_ptr()->sups_info_list_head;
   }

   while(list_ptr != NULL)
   {
     temp_ptr = list_ptr->next;
	 
	 /* Send failure event to clients */
     cmwsups_send_sups_fail_event(ss, list_ptr);
     CM_MSG_HIGH_1("Sending sups fail event for cmd=%d",list_ptr->cmd);
     cm_mem_free(list_ptr);
     list_ptr = temp_ptr;
   }

   if(ss == CM_SS_HYBR_3)
   {
     cmsups_ptr()->hybr_gw3_sups_info_list_head = NULL;
     cmsups_ptr()->hybr_gw3_sups_info_list = NULL;	
   }
   else if(ss == CM_SS_HYBR_2)
   {
     cmsups_ptr()->hybr_gw_sups_info_list_head = NULL;
     cmsups_ptr()->hybr_gw_sups_info_list = NULL;	
   }
   else
   {
     cmsups_ptr()->sups_info_list_head = NULL;	
     cmsups_ptr()->sups_info_list = NULL;
   }

   return;
}

/*===========================================================================

FUNCTION cmwsups_timer_proc

DESCRIPTION
  Process timer events

DEPENDENCIES
  Sups object must have already been initialized with
  cmcall_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmwsups_timer_proc(

  cm_timer_event_type    timer_event
    /* Indicate specific timer event.
    **
    ** Note that currently timer event is not being used
    */
)
{

  dword                       uptime              = time_get_uptime_secs();
  cmsups_s_type          *sups_ptr         = cmsups_ptr();
  cmwsups_info_list_s_type *sups_list   = NULL;
  cmwsups_info_list_s_type *temp_list   = NULL;
  cmph_s_type              *ph_ptr = cmph_ptr();
      /* Pointer to the phone object */

  sd_ss_mode_pref_e_type         comm_mode = CM_MODE_PREF_NONE;
  /* Mode capability supported by the current target */
  
  sd_ss_band_pref_e_type         comm_band = CM_BAND_PREF_NONE;
  /* Band capability supported by the current target */
  
#ifdef FEATURE_CM_LTE
  sys_lte_band_mask_e_type       comm_lte_band = SYS_LTE_BAND_MASK_CONST_NONE;
  /* LTE band capability supported by the current target */
#endif
  sd_ss_band_pref_e_type         comm_tds_band = CM_BAND_PREF_NONE;
  /* TD-SCDMA band capability supported by the current target */


  /* Check if further attempts are do be made or not 
  ** Otherwise clear the SUPS list 
  */
  
  if(sups_ptr->sups_info_list_head == NULL &&
     sups_ptr->hybr_gw_sups_info_list_head == NULL &&
     sups_ptr->hybr_gw3_sups_info_list_head == NULL )
  {
    return;
  }
  
  if(cmph_ptr()->oprt_mode != SYS_OPRT_MODE_ONLINE)
  {
    /* Phone must be online to originate the command.
    */
    CM_MSG_HIGH_1("SUPS timer proc, oprt_mode = %d",
                   cmph_ptr()->oprt_mode);

    cmwsups_list_clear(CM_SS_MAIN);
    cmwsups_list_clear(CM_SS_HYBR_2);
    cmwsups_list_clear(CM_SS_HYBR_3);
	return;
  }
  /* free the sups reference memeory  when the mode preference is LTE only and the domain is CS_PS     
  */
#ifdef FEATURE_CM_LTE

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined (FEATURE_MMODE_SXLTE_G)
  if((sups_ptr->hybr_gw3_sups_info_list_head != NULL)&& cmph_is_msim())
  {
      cmph_get_comm_mode_band_capab_with_pm(&comm_mode, &comm_band, &comm_lte_band, &comm_tds_band, cmph_map_cm_ss_to_subs(CM_SS_HYBR_3));
      if(((comm_mode & SD_SS_MODE_PREF_TDS_GSM_WCDMA ) == SD_SS_MODE_PREF_NONE )||
            cmcall_is_lte_only_mode( cmph_map_sd_mode_pref_to_cm_mode_pref(comm_mode),\
            cmph_map_sd_band_pref_to_cm_band_pref(comm_band),\
            comm_lte_band,\
            cmph_map_sd_tds_band_pref_to_cm_tds_band_pref(comm_tds_band)))
      {
        CM_MSG_HIGH_0("clear hybr_3");
        cmwsups_list_clear(CM_SS_HYBR_3);
        return;
      }
  }
  #endif

  if((sups_ptr->hybr_gw_sups_info_list_head != NULL)&& cmph_is_msim())
  {
      cmph_get_comm_mode_band_capab_with_pm(&comm_mode, &comm_band, &comm_lte_band, &comm_tds_band, cmph_map_cm_ss_to_subs(CM_SS_HYBR_2));
      if(((comm_mode & SD_SS_MODE_PREF_TDS_GSM_WCDMA ) == SD_SS_MODE_PREF_NONE )||
            cmcall_is_lte_only_mode( cmph_map_sd_mode_pref_to_cm_mode_pref(comm_mode),\
            cmph_map_sd_band_pref_to_cm_band_pref(comm_band),\
            comm_lte_band,\
            cmph_map_sd_tds_band_pref_to_cm_tds_band_pref(comm_tds_band)))
  {
    CM_MSG_HIGH_0("clear hybr_2");
    cmwsups_list_clear(CM_SS_HYBR_2);
    return;
  }
  }


  if((sups_ptr->sups_info_list_head != NULL))
  {
    cmph_get_comm_mode_band_capab_with_pm(&comm_mode, &comm_band, &comm_lte_band, &comm_tds_band, cmph_map_cm_ss_to_subs(CM_SS_MAIN));
    if(((comm_mode & SD_SS_MODE_PREF_TDS_GSM_WCDMA ) == SD_SS_MODE_PREF_NONE )||
       cmcall_is_lte_only_mode(cmph_map_sd_mode_pref_to_cm_mode_pref(comm_mode),\
                               cmph_map_sd_band_pref_to_cm_band_pref(comm_band),\
                               comm_lte_band,\
                               cmph_map_sd_tds_band_pref_to_cm_tds_band_pref(comm_tds_band)))
    {
          CM_MSG_HIGH_3("ph_ptr->main_stack_info.pref_info: mode_pref = %d, band_pref = %d, tds_band_pref = %d",
                   ph_ptr->main_stack_info.pref_info.mode_pref ,ph_ptr->main_stack_info.pref_info.band_pref,ph_ptr->main_stack_info.pref_info.tds_band_pref );

          CM_MSG_HIGH_0("ph_ptr->main_stack_info.pref_info.lte = ");
          cm_print_lte_band_mask(ph_ptr->main_stack_info.pref_info.lte_band_pref );
	  cmwsups_list_clear(CM_SS_MAIN);
	  return;
    }
  } 
  #endif

  #if defined FEATURE_MMODE_TRIPLE_SIM || defined FEATURE_MMODE_SXLTE_G
  /* Check pending SUPS for HYBR_2 */
  sups_list = sups_ptr->hybr_gw3_sups_info_list_head;

    while(sups_list != NULL)
    {
      temp_list = sups_list->next;

     /* If currently service mode is not GWL then  */
      
      /* If next orig uptime is 0 means origination is already sent. 
      ** No need to send again.
      ** If next orig uptime is non-zero and smaller than current uptime
      ** then time to send next retry for this sups command
      */
      if(sups_list->next_orig_uptime < uptime && sups_list->next_orig_uptime != 0) 
      {
        if(cmss_ptr()->info.gw3_srv_status != SYS_SRV_STATUS_SRV)
        {
           CM_MSG_HIGH_3("HYBR_3:OOS:no service as_id %d ss_ref %d gw3_srv_status %d",
           sups_list->sups_cmd_info.cmd_subs_id,
           sups_list->sups_cmd_info.ss_ref,
           cmss_ptr()->info.gw3_srv_status);
    }
        sups_list->next_orig_uptime = 0; /* Set to 0 as request being sent now */
        cmsups_client_cmd_resend(sups_list); /* If sups send fails, corresponding sups record is freed */

        CM_MSG_HIGH_3("Resending the Sups cmd: as_id %d ss_ref %d srv_status %d",
          sups_list->sups_cmd_info.cmd_subs_id,
          sups_list->sups_cmd_info.ss_ref,
          sups_list->next_orig_uptime);

  }
      else if(sups_list->sr_start_uptime + SUPS_RETRY_MAX_DURATION < uptime)
      {
        /* If a sups record exceeded max duration of attempts, free the memory */
        /* If next orig time is  0 mans we have sent the request and we are waiting for response from NAS
                  so we should nt clear the sups , it will be cleared once we get the response from LL
                  if next orig time is non zero means we have got the response , clear now */
           if(sups_list->next_orig_uptime != 0)
           {
             if(sups_list == sups_ptr->hybr_gw3_sups_info_list)
             {
               sups_ptr->hybr_gw3_sups_info_list = NULL;
             }
             /* Send failure event to clients */
             cmwsups_send_sups_fail_event(CM_SS_HYBR_3, sups_list);
             cmwsups_mem_free_sups_list_info(sups_list, &(sups_ptr->hybr_gw3_sups_info_list_head));
             CM_MSG_HIGH_3("SUPS MAX timer expired freeing the memory ss ref:%d,next_orig:%d,strt_time:%d",
                   sups_list->sups_cmd_info.ss_ref,
                   sups_list->next_orig_uptime,
                   sups_list->sr_start_uptime);
           }
           else
           {
              CM_MSG_HIGH_3("SUPS MAX timer expired, not free'g memory, wait for NAS resp, ss ref:%d,next_orig:%d,strt_time:%d",
                   sups_list->sups_cmd_info.ss_ref,
                   sups_list->next_orig_uptime,
                   sups_list->sr_start_uptime);
           }
      }
      else
      {
        CM_MSG_HIGH_3("SUPS hold for retry, as_id %d, ss_ref %d, next uptime %d",
          sups_list->sups_cmd_info.cmd_subs_id,
          sups_list->sups_cmd_info.ss_ref,
          sups_list->next_orig_uptime);
       }
      sups_list = temp_list; /* Move to next record */
  }
#endif
  #ifdef FEATURE_MMODE_DUAL_SIM
  /* Check pending SUPS for HYBR_2 */
  sups_list = sups_ptr->hybr_gw_sups_info_list_head;
  if(sups_list != NULL && cmph_is_msim())
  {
    while(sups_list != NULL)
    {
      temp_list = sups_list->next;

     /* If currently service mode is not GWL then  */
      
      /* If next orig uptime is 0 means origination is already sent. 
      ** No need to send again.
      ** If next orig uptime is non-zero and smaller than current uptime
      ** then time to send next retry for this sups command
      */
      if(sups_list->next_orig_uptime < uptime && sups_list->next_orig_uptime != 0) 
      {
        if(cmss_ptr()->info.gw_srv_status != SYS_SRV_STATUS_SRV)
        {
           CM_MSG_HIGH_3("HYBR_2:OOS:no service as_id %d ss_ref %d gw_srv_status %d",
           sups_list->sups_cmd_info.cmd_subs_id,
           sups_list->sups_cmd_info.ss_ref,
           cmss_ptr()->info.gw_srv_status);
        }
          sups_list->next_orig_uptime = 0; /* Set to 0 as request being sent now */
          cmsups_client_cmd_resend(sups_list); /* If sups send fails, corresponding sups record is freed */

        CM_MSG_HIGH_3("Resending the Sups cmd: as_id %d ss_ref %d srv_status %d",
          sups_list->sups_cmd_info.cmd_subs_id,
          sups_list->sups_cmd_info.ss_ref,
          sups_list->next_orig_uptime);

      }
      else if(sups_list->sr_start_uptime + SUPS_RETRY_MAX_DURATION < uptime)
      {
        /* If a sups record exceeded max duration of attempts, free the memory */
        /* If next orig time is  0 mans we have sent the request and we are waiting for response from NAS
                  so we should nt clear the sups , it will be cleared once we get the response from LL
                  if next orig time is non zero means we have got the response , clear now */
           if(sups_list->next_orig_uptime != 0)
           {
             if(sups_list == sups_ptr->hybr_gw_sups_info_list)
             {
               sups_ptr->hybr_gw_sups_info_list = NULL;
             }
             /* Send failure event to clients */
             cmwsups_send_sups_fail_event(CM_SS_HYBR_2, sups_list);
             cmwsups_mem_free_sups_list_info(sups_list, &(sups_ptr->hybr_gw_sups_info_list_head));
             CM_MSG_HIGH_3("SUPS MAX timer expired freeing the memory ss ref:%d,next_orig:%d,strt_time:%d",
                   sups_list->sups_cmd_info.ss_ref,
                   sups_list->next_orig_uptime,
                   sups_list->sr_start_uptime);
           }
           else
           {
              CM_MSG_HIGH_3("SUPS MAX timer expired, not free'g memory, wait for NAS resp, ss ref:%d,next_orig:%d,strt_time:%d",
                   sups_list->sups_cmd_info.ss_ref,
                   sups_list->next_orig_uptime,
                   sups_list->sr_start_uptime);
           }
      }
      else
      {
        CM_MSG_HIGH_3("SUPS hold for retry, as_id %d  ss_ref %d  next uptime %d",
          sups_list->sups_cmd_info.cmd_subs_id,
          sups_list->sups_cmd_info.ss_ref,
          sups_list->next_orig_uptime);
       }
      sups_list = temp_list; /* Move to next record */
    }
  }
  #endif
  /* Check for MAIN stack */
  sups_list = sups_ptr->sups_info_list_head;
  if(sups_list != NULL)
  {
    while(sups_list != NULL)
    {
      temp_list = sups_list->next;

      /* If next orig uptime is 0 means origination is already sent. 
      ** No need to send again.
      ** If next orig uptime is non-zero and smaller than current uptime
      ** then time to send next retry for this sups command
      */
	  if(sups_list->next_orig_uptime < uptime && sups_list->next_orig_uptime != 0)
      {
        if((cmss_ptr()->info.srv_status != SYS_SRV_STATUS_SRV) ||
           (cmss_ptr()->info.sys_mode != SYS_SYS_MODE_GSM &&
           cmss_ptr()->info.sys_mode != SYS_SYS_MODE_WCDMA &&
           cmss_ptr()->info.sys_mode != SYS_SYS_MODE_LTE && cmss_ptr()->info.sys_mode == SYS_SYS_MODE_TDS)
           )
        {
           CM_MSG_HIGH_3("MAIN:OOS:no service as_id %d sys_mode %d srv_status %d",
           sups_list->sups_cmd_info.cmd_subs_id,
           cmss_ptr()->info.sys_mode,
           cmss_ptr()->info.srv_status);
        }
        
          sups_list->next_orig_uptime = 0; /* Set to 0 as uptime is alreayd sent */
          cmsups_client_cmd_resend(sups_list); /* If sups send fails, corresponding sups record is freed */
  
          CM_MSG_HIGH_3("Resending the Sups cmd: as_id %d ss_ref %d srv_status %d",
            sups_list->sups_cmd_info.cmd_subs_id,
            sups_list->sups_cmd_info.ss_ref,
            sups_list->next_orig_uptime);

      }
      else if(sups_list->sr_start_uptime + SUPS_RETRY_MAX_DURATION < uptime)
      {
          /* If a sups record exceeded max duration of attempts, free the memory */
          /* If next orig time is  0 mans we have sent the request and we are waiting for response from NAS
                  so we should nt clear the sups , it will be cleared once we get the response from LL
                  if next orig time is non zero means we have got the response , clear now */
        if(sups_list->next_orig_uptime != 0)
        {
          if(sups_list == sups_ptr->sups_info_list)
          {
            sups_ptr->sups_info_list = NULL;
          }
          /* Send failure event to clients */
          cmwsups_send_sups_fail_event(CM_SS_MAIN, sups_list);
          cmwsups_mem_free_sups_list_info(sups_list, &(sups_ptr->sups_info_list_head));
             CM_MSG_HIGH_3("SUPS MAX timer expired freeing the memory ss ref:%d,next_orig:%d,strt_time:%d",
                   sups_list->sups_cmd_info.ss_ref,
                   sups_list->next_orig_uptime,
                   sups_list->sr_start_uptime);
         }
         else
         {
            CM_MSG_HIGH_3("SUPS MAX timer expired, not free'g memory, wait for NAS resp, ss ref:%d,next_orig:%d,strt_time:%d",
                   sups_list->sups_cmd_info.ss_ref,
                   sups_list->next_orig_uptime,
                   sups_list->sr_start_uptime);
         }
      }
      else
      {
          CM_MSG_HIGH_3("SUPS hold for retry, as_id %d  ss_ref %d  next uptime %d",
            sups_list->sups_cmd_info.cmd_subs_id,
            sups_list->sups_cmd_info.ss_ref,
            sups_list->next_orig_uptime);
       }
      sups_list = temp_list;
    }
  }

}
#endif

/*===========================================================================
FUNCTION cmsups_dem_is_apps_wakeup_from_suspend_event

DESCRIPTION
  Determines if the event should wakeup the processor from the suspend state

DEPENDENCIES
  None.

RETURN VALUE
  True : If the apps processor needs to be woken up
  False: Otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean  cmsups_dem_is_apps_wakeup_from_suspend_event(

    cm_sups_event_e_type   sups_event
        /* SUPS event */
)
/*lint -esym(715, sups_event) */
{
  #ifndef FEATURE_APPS_POWER_COLLAPSE
  /* If Apps power collapse is not defined, return ture
  */
  return TRUE;
  #else

  switch ( sups_event )
  {
    case CM_SUPS_EVENT_USS_NOTIFY_IND:
    case CM_SUPS_EVENT_USS_IND:
    case CM_SUPS_EVENT_LCS_LOCATION_NOTIFICATION_IND:
      return TRUE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    default:
      return FALSE;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #endif
} /* cmsups_dem_is_apps_wakeup_from_suspend_event */
/*lint +esym(715, sups_event) */

#else /* FEATURE_WCDMA || FEATURE_GSM */

/*===========================================================================
FUNCTION cmwsups_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmwsups_dummy( void )
/*lint -esym(714,cmwsups_dummy) */
/*lint -esym(765,cmwsups_dummy)
** Can't be static as it would result in compiler warnings
*/
{
}


#endif /* FEATURE_GSM || FEATURE_WCDMA */
