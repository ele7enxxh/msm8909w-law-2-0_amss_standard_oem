/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           C A L L   M A N A G E R   A D V I C E   O F   C H A R G E

GENERAL DESCRIPTION
  This module performs the Adivce of Charge functionality for the
  Call Manager.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001 - 2006, 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmwaoc.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/20/12   gm      Data synchronization for CM Call objects
01/06/12   gm      Data synchronization changes for freefloating platform
05/13/11   rn      Use Increase MMGSDI API for ACM files
04/21/11   sv      Correct Lint Errors
04/15/11   anm    Removing cmwaoc_init() as part of GSDI clean up
04/11/11   am      Cleaning stubs and runnning qtf SUTE
03/11/11   rm      GSDI clean-up
01/19/10   am      Removing gsdi_get_card_mode()
12/23/10   sv      Integrate DSDS feature
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
10/05/10   pm      Remove unneccesary clk.h & CLK_MS_PER_SEC
09/23/10   xs      Superlint critical/high error fixed
09/17/10   mp      Fix for the SIM Busy URC when at+cacm used back to back
09/17/10   chl     Add clk.h back for CLK_MS_PER_SEC
09/14/10   pm      Remove unneccesary clk.h
05/20/10   sv      Include clk.h
01/13/10   sg      When call get terminated, call prt (if any) should be 
                   removed from charging call list.
01/11/10   mj      CSIM - GW uses Session based MMGSDI interfaces  
10/30/09   pm      Fix KW error - possible memory leak
09/09/09   am      Initializing ph_ptr->main_stack_info.phone_charge.is_acm_max_read_success
                   and ph_ptr->main_stack_info.phone_charge.is_acm_read_success
09/01/09   sv      Removing un-necessary clk API header
04/07/09   am      fixing memset error.
03/24/09   sv      updated to cmtaski.h
02/27/09   rm      Adding header files of other MOBs
02/04/09   rm      Initialising ACM filename correctly in MMGSDI
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
10/01/08   sv      Remove unnecessary string.h include to remove lint errors.
12/12/07   rm      Adding RAT-balancing feature support
                   Removed cm_refresh_aoc_init
08/07/07   ic      Lint clean up
04/03/07   rp      Adding use_e7 parameter to cmwaoc_set_ccm_timeout and calling 
                   it from cm_aoc_update_aoc
12/28/06   ka      Removing conditional return of AOC functionality with 
                   FALSE.
08/16/06   ka/rp   Moving aoc_ready flag to PH object.
03/24/06   ka      Cleaning up elapsed time cnt and call insertion into call
                   charge list. Restarting PH ccm timer for concurrent call
                   after first call is ended.
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
10/13/05   ic      Replaced INTLOCK / INTFREE with rex_enter_crit_sect() /
                   rex_leave_crit_sect()  
12/06/04   ic      In cm_aoc_add_to_chargelist() renamed variable timer_ptr  
                   to timer_to_set_ptr to get rid of Lint error "declaration 
                   of symbol 'timer_ptr' hides symbol 'timer_ptr' (line 160, 
                   file ..\..\services\time\timer.h)"
08/04/04   ic      Removed unused argument from cm_aoc_save_acm().
                   CM Lint clean up - cmwaoc.c with Lint supression verified 
                   and free of Lint issues in all CUTE configurations. 
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
                   Added cmwaoc_dummy() to get rid of ADS 1.2 compiler warning
07/06/04   dk      Modified signature of cm_find_connection_id().
06/23/04   ic      Fixed ADS 120 compiler warnings (Saber build)
06/14/04   jqi     Made CM MMGSDI calls to be asynchronouse.
03/16/04   ka      Merged changes from 6250
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
01/19/04   ka      Changed cmaoc_send_call_end for Multiple PDP.
01/13/04   ka      Merged changes from MSM6200
05/30/03   tml     Added rec_num_accessed_pt to gsdi_sim_read synch call
05/01/03   jar     Added cm_refresh_aoc_init for REFRESH FCN Handling
04/28/03   AT      Lint cleanup.
04/11/03   prk     Removed references to FEATURE_AOC and FEATURE_MMGSDI.
02/25/03   AT      Fix 11.10 case to now drop calls on SIM error.
02/03/03   AT      Modified to make sure Max Charge hit is displayed when
                   ACM reaches ACMmax.  Set the end_status in the call object.
01/21/03   ws      Updated copyright information for 2003
11/22/02   ic      Removed unused variable from cm_aoc_save_acm()
11/14/02   vt      Merged changes from GW mainline.
10/30/02   AT      Fixed unnecessary error message when using Async function.
                   Added some F3 messages to help determine charges at warning
                   and at call being dropped.
10/25/02   AT      Utilize MMGSDI's Async function to help eliminate latency.
09/13/02   HQ      MM GSDI changes.
09/08/02   AT      Fixed problem AOC undercharging.
07/30/02   AT      Completed all GSM 11.10 tests.  Fixed minor charge issues.
06/21/02   AT      Fixed a problem where we were incorrectly rounding up CCM
04/26/02   AT      Fixed a problem where we double charge for the first segment.
04/24/02   AT      Fixed an error with multicall.
                   Added F3 messages for Charges for the call.
04/22/02   AT      Updated with changes for Advice of Charge.
04/16/02   AT      Merged Advice of Charge with GSDI and UI.
03/11/02   AT      Fixed the ending of the call when we are out of funds.
01/30/02   AT      Fixed comments from code reviews.
12/13/01   AT      Integrated this file with GSDI for both GSM SIM and USIM.
11/15/01   AT      Updated to allow for the response back to MN.
                   Also this file now lints cleanly.
11/01/01   AT      Added a function to reset the ACM.
09/01/01   AT      Initial release.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(766,customer.h) */
#include "mmcp_variation.h"
#include "customer.h"  /* Customer configuration file */

#ifdef CM_GW_SUPPORTED
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"       /* Internal interface to cm.c */
#include "comdef.h"    /* Definition for basic types and macros */

#include "task.h"
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "err.h"       /* Interface to error log services */
#include "msg.h"       /* Interface to F3 messages services */
#include "rex.h"       /* Interface to rex services */
#include "cmph.h"      /* Interface to CM phone object */
#include "cmcall.h"    /* Get the call structures. */
#include "cmwaoc.h"    /* Get the function prototypes necessary. */
#include "cmwcall.h"   /* cmwcall_get_nsapi function prototype */
#include "uim.h"
#include "uim_v.h"
#include "cmtaski.h"

#include "cmmmgsdi.h"


#ifdef CM_DEBUG
#error code not present
#endif


/* Head of the MAIN call list. */
/* in_CS for all tasks. */
static cmcall_s_type *call_list_head;

/* Head of the sub2 call list. */
/* in_CS for all tasks. */
static cmcall_s_type *hybr_gw_call_list_head;


#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
/* Head of the MAIN call list. */
static cmcall_s_type *hybr_gw3_call_list_head;
#endif


/* File name for ACM file. */
static uim_items_enum_type acm_filename = UIM_NO_SUCH_ITEM;



/*===========================================================================

FUNCTION cmwaoc_get_list_head

DESCRIPTION
  This function returns the head of the list of calls being charged.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
cmcall_s_type  *cmwaoc_get_list_head(

   sys_modem_as_id_e_type  asubs_id
    /* sub whose acm needs to be updated */

)
{
  SYS_ARG_NOT_USED(asubs_id);
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  /* return the call head based on the stack
  */
  #if defined(FEATURE_MMODE_SXLTE_G)
  if( asubs_id == SYS_MODEM_AS_ID_2 && cmph_is_sxlte())
  {
    return hybr_gw3_call_list_head;
  }
  else
  #endif
  if( asubs_id == SYS_MODEM_AS_ID_2)
  {
    return hybr_gw_call_list_head;
  }
  else
  #ifdef FEATURE_MMODE_TRIPLE_SIM
  /* return the call head based on the stack
  */
  if( asubs_id == SYS_MODEM_AS_ID_3 )
  {
    return hybr_gw3_call_list_head;
  }
  else
  #endif
  {  
    return (call_list_head);
  }
}


/*===========================================================================

FUNCTION cmwaoc_set_list_head

DESCRIPTION
  This function returns the head of the list of calls being charged.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
static  void            cmwaoc_set_list_head(

   sys_modem_as_id_e_type  asubs_id,
    /* sub whose acm needs to be updated */

        cmcall_s_type   *list_head
          /* stack whise list to retrieve */

)
{
  SYS_ARG_NOT_USED(asubs_id);
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  /* return the call head based on the stack
  */
  #ifdef FEATURE_MMODE_SXLTE_G
  if(cmph_is_sxlte() && asubs_id == SYS_MODEM_AS_ID_2)
  {
    hybr_gw3_call_list_head = list_head;
  }
  else
  #endif
  if(asubs_id == SYS_MODEM_AS_ID_2)
  {
    hybr_gw_call_list_head = list_head;
  }
  else
  #ifdef FEATURE_MMODE_TRIPLE_SIM
  /* return the call head based on the stack
  */
  if(asubs_id == SYS_MODEM_AS_ID_3)
  {
    hybr_gw3_call_list_head = list_head;
  }
  else
  #endif
  {  
    call_list_head = list_head;
  }
}



/*===========================================================================

FUNCTION cmwaoc_send_call_end

DESCRIPTION
  Command CM to end a call - in origination, incoming or conversation state.

  Note that the client supplied callback function, cmd_cb_func,
  will be called to notify the client of the command status.

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  TRUE - request sent, FALSE - No buffer to send request

SIDE EFFECTS
  If command is successful, CM clients will be notified of the
  corresponding CM event.

===========================================================================*/
static  void          cmwaoc_send_call_end(

        cmcall_s_type *call_ptr

)
{
  mn_cnm_cmd_type                 *mn_ptr = NULL;
      /* MN command pointer */
  sm_cmd_type                     *sm_ptr = NULL;
      /* SM command pointer */
  cm_call_id_type                 call_id;
      /* Call Id of call object */
  sys_modem_as_id_e_type   asubs_id;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr           != NULL);

  asubs_id = cmcall_get_asubs_id(call_ptr);
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  switch (call_ptr->call_type )
  {
    case CM_CALL_TYPE_PS_DATA:
      sm_ptr = cm_sm_get_buf_else_err_fatal(SMREG_PDP_DEACTIVATE_REQ);
      call_id = cmcall_call_id_get( call_ptr );

      if(cm_find_connection_id( call_id,
                             &sm_ptr->header.connection_id ))
      {
        sm_ptr->cmd.pdp_deactivate_req.nsapi = cmwcall_get_nsapi( call_id );
        sm_ptr->cmd.pdp_deactivate_req.tear_down_ind =
           call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.tear_down_ind;
        sm_ptr->cmd.pdp_deactivate_req.cause =
           call_ptr->cmcall_mode_info.info.gsm_wcdma_ps_info.cause.ext_cause;

        sm_ptr->cmd.pdp_deactivate_req.as_id = asubs_id;

        cm_sm_send_cmd( sm_ptr );
      }
      else
      {
        CM_ERR_1("Unknown connection ID for call %d",
            cmcall_call_id_get( call_ptr ));
      }
      break;

    case CM_CALL_TYPE_VOICE:
    case CM_CALL_TYPE_CS_DATA:
    default:
      call_ptr->end_status = CM_CALL_END_NO_FUNDS;
      mn_ptr = cm_mn_get_buf_else_err_fatal(CM_1_X_SEND_REQ);
      mn_ptr->cmd.cm_n_x_send_req.cause_value = ACM_GEQ_ACMMAX;

      if(cm_find_connection_id( cmcall_call_id_get( call_ptr ),
                                 &mn_ptr->cmd.cm_n_x_send_req.connection_id))
      {
        mn_ptr->cmd.cm_n_x_send_req.as_id = asubs_id;

        cm_mn_send_cmd( mn_ptr );
      }
      else
      {
        CM_ERR_1("Unknown connection ID for call %d",
            cmcall_call_id_get( call_ptr ));
      }
  }
}


/*===========================================================================

FUNCTION cmwaoc_drop_all_calls_now

DESCRIPTION
  When we get a SIM error, this function will drop all calls with charges
  associated with them.

DEPENDENCIES
  None.

RETURN VALUE
  Void

SIDE EFFECTS
  None
===========================================================================*/
void cmwaoc_drop_all_calls_now (

     sys_modem_as_id_e_type  asubs_id
     /* sub whose acm needs to be updated */

)
{
  cmcall_s_type *call_list;
  cm_wcall_aoc_data_type *tmp_charge_ptr;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  call_list = cmwaoc_get_list_head(asubs_id);

  while (call_list != NULL)
  {
    tmp_charge_ptr =
        &call_list->cmcall_mode_info.info.gsm_wcdma_info.wcall_charge_data;

    if(tmp_charge_ptr->e.e1 || tmp_charge_ptr->e.e4 || tmp_charge_ptr->e.e5)
    {
      CM_MSG_HIGH_0("AOC: Dropping call due to SIM error.");
      cmwaoc_send_call_end(call_list);
    }
    call_list = call_list->next_charged_call;
  }
}


/*===========================================================================

FUNCTION cmwaoc_mmgsdi_async_cb

DESCRIPTION
  This function will process the asynchronous callbacks from MMGSDI.

DEPENDENCIES
  None.

RETURN VALUE
  Void

SIDE EFFECTS
  None
===========================================================================*/
static void cmwaoc_mmgsdi_async_cb(

    mmgsdi_return_enum_type               status,
    mmgsdi_cnf_enum_type                  cnf,
    const mmgsdi_cnf_type                 *cnf_ptr
)
{
  sys_modem_as_id_e_type  asubs_id;
  cm_generic_rpt_s_type   *gen_rpt_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cnf_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(cnf_ptr->write_cnf.response_header.mmgsdi_status != MMGSDI_SUCCESS)
  {    
    asubs_id = cm_mmgsdi_ssn_tbl_get_subs_id_for_id(
                              cnf_ptr->write_cnf.response_header.session_id);
        
    if (SYS_MODEM_AS_ID_NONE == asubs_id)
    {
      CM_ERR_2("cmwaoc_mmgsdi_async_cb(), invalid asubs_id: %d, session id: %d", 
                    asubs_id, cnf_ptr->write_cnf.response_header.session_id);
      return;
    }
        
    gen_rpt_ptr = cm_generic_rpt_get_buf_else_err_fatal();

    gen_rpt_ptr->hdr.cmd = CM_DROP_CHARGED_CALLS;
    gen_rpt_ptr->generic_rpt.drop_calls_rpt.ss =
                                      cmph_map_subs_to_sd_ss(asubs_id);

    cm_generic_rpt(gen_rpt_ptr);
  }

  SYS_ARG_NOT_USED(cnf);
  SYS_ARG_NOT_USED(status);

}  /* cmwaoc_mmgsdi_async_cb() */


#if !(defined FEATURE_MMGSDI_SESSION_LIB)
/*===========================================================================

FUNCTION cmwaoc_gsdi_async_cb

DESCRIPTION
  This function will process the callback from GSDI asynchronous calls.
  For now, that will be limited to writes and increases.

DEPENDENCIES
  None.

RETURN VALUE
  Void

SIDE EFFECTS
  None
===========================================================================*/
void cmwaoc_gsdi_async_cb (

  gsdi_cnf_T* gsdi_cnf

)
/*lint -esym(818, gsdi_cnf) */
/*lint -esym(765, cmwaoc_gsdi_async_cb)
** Can't be const / static - callback
*/
{
  cm_ss_e_type            ss = CM_SS_MAIN;
  cm_generic_rpt_s_type   *gen_rpt_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(gsdi_cnf != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(gsdi_cnf->message_header.gsdi_status != GSDI_SUCCESS)
  {
    
    CM_MSG_HIGH_2("Invalid response %x from async command %d",
                                    gsdi_cnf->message_header.resp_type,
                                    gsdi_cnf->message_header.gsdi_status);

    gen_rpt_ptr = cm_generic_rpt_get_buf_else_err_fatal();
    
    gen_rpt_ptr->hdr.cmd = CM_DROP_CHARGED_CALLS;
    gen_rpt_ptr->generic_rpt.drop_calls_rpt.ss =
                                      cmph_map_cm_ss_type_to_sd_ss_type(ss);
    cm_generic_rpt(gen_rpt_ptr);
  }
}

#endif


/*===========================================================================

FUNCTION cmwaoc_init_using_mmgsdi

DESCRIPTION
  This function initializes the data structures necessary for Advice of
  Charge. This includes initializing the values of ACM and ACMmax from
  the SIM, as well as setting the SIM type variables.

DEPENDENCIES
  The initialization of the SIM must be complete, with the PIN verified.

RETURN VALUE
  Void

SIDE EFFECTS
  None
===========================================================================*/

void cmwaoc_init_using_mmgsdi(

      cm_ss_e_type ss
   /* stack whose acm needs to be reset */

)
{ 
  mmgsdi_refresh_file_list_type      file_info;
    /* File info */

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_session_type_enum_type   session_type;
  #endif

  mmgsdi_file_enum_type file_list[4] = {
                                 MMGSDI_GSM_ACM,
                                 MMGSDI_GSM_ACM_MAX,
                                 MMGSDI_USIM_ACM,
                                 MMGSDI_USIM_ACM_MAX
                                 };

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  #ifdef FEATURE_MMGSDI_SESSION_LIB
  if(ss == CM_SS_HYBR_2)
  {
    session_type = MMGSDI_GW_PROV_SEC_SESSION;
  }
  else
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if(ss == CM_SS_HYBR_3)
  {
    if (cmph_is_sxlte())
    {
      session_type = MMGSDI_GW_PROV_SEC_SESSION;
    }
    else
    {
      session_type = MMGSDI_GW_PROV_TER_SESSION;
    }
  }
  else
  #endif
  {
    session_type = MMGSDI_GW_PROV_PRI_SESSION;
  }

  switch(cm_mmgsdi_ssn_tbl_get_app_type_for_type(session_type))
  {
    case MMGSDI_APP_SIM:
      acm_filename = UIM_GSM_ACM;
      break;
	  
    case MMGSDI_APP_USIM:
      acm_filename = UIM_USIM_ACM;
      break;
	  
    default:
      CM_MSG_HIGH_0("cmwaoc_init_using_mmgsdi: Invalid card mode"); 
  }
  #endif /* FEATURE_MMGSDI_SESSION_LIB */

  /* Clear file info */
  memset (&file_info, 0, sizeof(file_info));

  /* - - - - - - - - - - - - - -- - - - - - - - - - - - - -- - - - - - - -*/


  file_info.num_files = ARR_SIZE (file_list);

  file_info.file_list_ptr = file_list;

  cmmmgsdi_refresh_files_read_proc(ss, &file_info, CM_CLIENT_ID_ANONYMOUS);

}  /* cmwaoc_init_using_mmgsdi() */



/*===========================================================================

FUNCTION cmwaoc_save_acm

DESCRIPTION
  This function saves the ACM to the SIM.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void cmwaoc_save_acm(

     sys_modem_as_id_e_type  asubs_id,
     /* sub whose acm needs to be updated */

     dword increase_by
      /* amount by which the acm needs to be increased */

)
{
  uint8 temp_returned[CM_AOC_ACM_SIZE];
  #ifndef FEATURE_MMGSDI_SESSION_LIB
  byte len;
  #endif
  cmph_s_type      *ph_ptr = cmph_ptr(); 

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_increase_data_type    increase_data;
  dword value;
  #endif

  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  CM_MSG_MED_2 ("cmwaoc_save_acm: asubs_id %d, acm_file_name - %d",
                          asubs_id, acm_filename);

  if(CMPH_SS_PHONE_CHARGE(asubs_id,ph_ptr).aoc_ready)
  {
    #ifdef FEATURE_MMGSDI_SESSION_LIB
    value = increase_by;
    temp_returned[0] = (uint8)(value>>16);
    temp_returned[1] = (uint8)(value>>8);
    temp_returned[2] = (uint8)value;
    
    increase_data.data_len = CM_AOC_ACM_SIZE;
    increase_data.data_ptr = temp_returned;
	
    if(cmmmgsdi_increase(asubs_id,
                        (acm_filename == UIM_USIM_ACM)?MMGSDI_USIM_ACM:MMGSDI_GSM_ACM,
                         CM_MMGSDI_SUBS_TYPE_GW,
                         increase_data
    ) != TRUE)
    #else
    if(gsdi_sim_increase(acm_filename,
                          increase_by,
                          temp_returned,
                          &len,
                          0, /* client ref */
                          cmwaoc_gsdi_async_cb /* callback */
                         ) != GSDI_SUCCESS
                         )
    #endif
    {
      CM_ERR_0("Could not update ACM");
      cmwaoc_drop_all_calls_now (asubs_id);

      if(asubs_id == SYS_MODEM_AS_ID_2)
      {
        #if defined(FEATURE_MMODE_SXLTE_G) || defined(FEATURE_MMODE_TRIPLE_SIM)
        if (cmph_is_sxlte())
        {
          ph_ptr->hybr_3_stack_info.phone_charge.aoc_ready = FALSE;
        }
        else
        #endif
        {
          ph_ptr->hybr_2_stack_info.phone_charge.aoc_ready = FALSE;
        }
      }
      else
      #ifdef FEATURE_MMODE_TRIPLE_SIM
      if(asubs_id == SYS_MODEM_AS_ID_3)
      {
        ph_ptr->hybr_3_stack_info.phone_charge.aoc_ready = FALSE;
      }
      else
      #endif
      {
        ph_ptr->main_stack_info.phone_charge.aoc_ready = FALSE;
      }
    }
    else
    {
      CM_MSG_HIGH_1 ("gsdi_sim_increase success for acm_file - %d",acm_filename);
    }
  }
}


/*===========================================================================

FUNCTION cmwaoc_set_ccm_timeout

DESCRIPTION
  This function sets ccm_timeout and use_ccm_timeout depending on e7, e2 
  values.
  e7 will be used to set the timer if use_e7 is TRUE and e7 > 0, otherwise 
  e2 is checked.

DEPENDENCIES
  The call charge pointer must be initialized.

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/

static void  cmwaoc_set_ccm_timeout(

  cm_wcall_aoc_data_type *call_charge_ptr,

  boolean                 use_e7

) 
{   
  if (call_charge_ptr->e.e7 > 0  && use_e7  )
  {
    /* E1 will be applied after E7 times out. */
    call_charge_ptr->ccm_timeout = (unsigned long) (call_charge_ptr->e.e7) *
      MS_PER_TIMEUNIT;

    call_charge_ptr->use_ccm_timeout = TRUE;
  }
  else if (call_charge_ptr->e.e2 > 0)   
  {
    /* Charge for the first time segment now. */
    call_charge_ptr->ccm_timeout = (unsigned long) (call_charge_ptr->e.e2) *
      MS_PER_TIMEUNIT;
    
    call_charge_ptr->use_ccm_timeout = TRUE;
  }
  else
  {
    call_charge_ptr->use_ccm_timeout = FALSE;
  }
  
}

/*===========================================================================

FUNCTION cmwaoc_setup_aoc_for_call

DESCRIPTION
  This function initializes the AOC data structure within the given.

DEPENDENCIES
  The call structure pointed to within call_ptr must be initialized.

RETURN VALUE
  Boolean flag as to whether we can support charges for this call.

SIDE EFFECTS
  None
===========================================================================*/

boolean cmwaoc_setup_aoc_for_call(

  cmcall_s_type           *call_ptr, 

  const cm_e_values_type  *charge_info

)
{
  cm_wcall_aoc_data_type *call_charge_ptr;
    /* The following is a pointer to the charge portion of the message.  */

  cmph_s_type            *ph_ptr = cmph_ptr();
    /* Get the phone object. */

  sys_modem_as_id_e_type  asubs_id;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr    != NULL); 
  CM_ASSERT(charge_info != NULL); 
  CM_ASSERT(ph_ptr      != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  asubs_id = cmcall_get_asubs_id(call_ptr);
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  if(!CMPH_SS_PHONE_CHARGE(asubs_id, ph_ptr).aoc_ready)
  {
    return FALSE;
  }

  call_charge_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info.wcall_charge_data;

  CM_MSG_HIGH_6("AOC Charges e1-e6 %d %d %d %d %d %d", 
  	  charge_info->e1, charge_info->e2, charge_info->e3, charge_info->e4, charge_info->e5, charge_info->e6);
 
  CM_MSG_HIGH_1("AOC Charges e7 %d", charge_info->e7);

  /* Do some sanity checking on the parameters */
  if ( (charge_info->e3 == 0) &&
       ((charge_info->e1 > 0) || (charge_info->e4 > 0) || (charge_info->e5 > 0))
     )
  {
    CM_ERR_0 ("e3 is 0, e1 or e4 or e5 is non-zero");
  }

  if(!call_charge_ptr->aoc_initialized || !call_charge_ptr->use_ccm_timeout )
  {
    if(CMPH_SS_PHONE_CHARGE(asubs_id, ph_ptr).drop_calls &&
        (charge_info->e1 > 0 || charge_info->e4 > 0 || charge_info->e6 > 0)
       )
    {
      
      CM_MSG_HIGH_2("AOC: ropping call due to max charge being reached. ACM %d ACMMAX %d", cmwaoc_get_acm(asubs_id),
                                           cmwaoc_get_acmmax(asubs_id));
      cmwaoc_send_call_end(call_ptr);
      return TRUE;
    }

    /* We will initialize it now. */
    call_charge_ptr->aoc_initialized = TRUE;
    call_charge_ptr->new_e_values_charged = TRUE;

    /* Initialize the AOC values. */
    call_charge_ptr->AOC = call_charge_ptr->last_AOC_to_update_CCM = 0;

    /* Copy the E values into the structure. */
    call_charge_ptr->e.e1 = charge_info->e1;
    call_charge_ptr->e.e2 = charge_info->e2;
    call_charge_ptr->e.e3 = charge_info->e3;
    call_charge_ptr->e.e4 = charge_info->e4;
    call_charge_ptr->e.e5 = charge_info->e5;
    call_charge_ptr->e.e6 = charge_info->e6;
    call_charge_ptr->e.e7 = charge_info->e7;

    /* Charge the initial E4*E3 to AOC for this call; */
    call_charge_ptr->AOC = call_charge_ptr->e.e4 * call_charge_ptr->e.e3;

    /* Set ccm_timeout with e7 (use_e7 = TRUE). If e7 is 0
    ** use e2 to set the timer.
    */
    cmwaoc_set_ccm_timeout(call_charge_ptr, TRUE);
    CM_MSG_HIGH_3 ("call %d ccm_timeout %d and use_ccm_timeout %d", 
                   call_ptr->call_id, call_charge_ptr->ccm_timeout, 
                   call_charge_ptr->use_ccm_timeout) ;


    cmwaoc_add_to_chargelist(call_ptr);

    if(call_charge_ptr->AOC > 0)
    {
      cmwaoc_update_ccm(call_ptr);
      cmph_update_acm(asubs_id);
    }
  }
  else
  {
    /* Charge the new E4*E3 to AOC for this call; */
    if(charge_info->e4 * charge_info->e3 > 0)
    {
      CM_MSG_HIGH_1("AOC: Add initial %d", charge_info->e4 * charge_info->e3);

      call_charge_ptr->AOC += charge_info->e4 * charge_info->e3;
      cmwaoc_update_ccm(call_ptr);
    }

    /* Copy the E values into the structure. */
    call_charge_ptr->new_e.e1 = charge_info->e1;
    call_charge_ptr->new_e.e2 = charge_info->e2;
    call_charge_ptr->new_e.e3 = charge_info->e3;
    call_charge_ptr->new_e.e4 = charge_info->e4;
    call_charge_ptr->new_e.e5 = charge_info->e5;
    call_charge_ptr->new_e.e6 = charge_info->e6;
    call_charge_ptr->new_e.e7 = charge_info->e7;

    /* Flag that there are new E values */
    call_charge_ptr->new_e_values_charged = FALSE;
  }
  return TRUE;
}

/*===========================================================================

FUNCTION cmwaoc_update_ccm

DESCRIPTION
  This function is called when its time to update the current call
  meter in the phone pointer.  This occurs whenever a call updates
  its own AOC counters.

DEPENDENCIES
  The call structure pointed to within call_ptr must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void cmwaoc_update_ccm(

     cmcall_s_type *call_ptr

)
{
  cm_ph_charge_data_type *ph_charge_ptr    = NULL;
  cm_wcall_aoc_data_type *call_charge_ptr  = NULL;
  cmph_s_type  *ph_ptr                     = cmph_ptr();
  sys_modem_as_id_e_type  asubs_id;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr           != NULL); 
  CM_ASSERT(ph_ptr        != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  asubs_id = cmcall_get_asubs_id(call_ptr);
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  if(!CMPH_SS_PHONE_CHARGE(asubs_id,ph_ptr).aoc_ready)
  {
    return;
  }

  /* Set pointers to the charge structures for convenience and readability */

  if(asubs_id == SYS_MODEM_AS_ID_2)
  {
    #if defined(FEATURE_MMODE_SXLTE_G) || defined(FEATURE_MMODE_TRIPLE_SIM)
    if (cmph_is_sxlte())
    {
      ph_charge_ptr = &ph_ptr->hybr_3_stack_info.phone_charge;
    }
    else
    #endif
    {
      ph_charge_ptr = &ph_ptr->hybr_2_stack_info.phone_charge;
    }
  }
  else
  #ifdef FEATURE_MMODE_TRIPLE_SIM
  if(asubs_id == SYS_MODEM_AS_ID_3)
  {
    ph_charge_ptr = &ph_ptr->hybr_3_stack_info.phone_charge;
  }
  else
  #endif
  {
    ph_charge_ptr = &ph_ptr->main_stack_info.phone_charge;
  }

  call_charge_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info.wcall_charge_data;

  /* Update the CCM to include the new AOC charges for this call. */
  CM_MSG_HIGH_1("AOC: CCM being increased by %d",
      call_charge_ptr->AOC - call_charge_ptr->last_AOC_to_update_CCM);

  ph_charge_ptr->CCM += call_charge_ptr->AOC -
                        call_charge_ptr->last_AOC_to_update_CCM;

  /* Indicate that this charge has been made. */
  call_charge_ptr->last_AOC_to_update_CCM = call_charge_ptr->AOC;

  /* Now update the ACM if its not too soon. */
  if(ph_charge_ptr->acm_timer_running == FALSE)
  {
    cmph_update_acm( asubs_id );
  }
}

/*===========================================================================

FUNCTION cm_ph_update_aoc

DESCRIPTION
  This function updates the AOC value for the given call.

DEPENDENCIES
  The call structure pointed to within call_ptr must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void  cmwaoc_update_aoc(

      cmcall_s_type *call_ptr

)
{
  unsigned long           additional_segments;
  unsigned long           additional_e5s;
  cm_wcall_aoc_data_type  *call_charge_ptr = NULL;
  cmph_s_type             *ph_ptr          = cmph_ptr();
  sys_modem_as_id_e_type  asubs_id;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr           != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  asubs_id = cmcall_get_asubs_id(call_ptr);
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  if(!CMPH_SS_PHONE_CHARGE(asubs_id,ph_ptr).aoc_ready)
  {
    return;
  }

  /* Set pointers to the charge structures for convenience and readability */
  call_charge_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info.wcall_charge_data;

  /* If we are dropping calls, its time to drop this one. */
  if( (CMPH_SS_PHONE_CHARGE(asubs_id,ph_ptr).drop_calls) &&
       (call_charge_ptr->use_ccm_timeout ) &&
       (call_charge_ptr->ccm_timeout == 0) )
  {
    CM_MSG_HIGH_0("AOC: Dropping call due to max charge being reached.");
    cmwaoc_send_call_end(call_ptr);
  }
  else
  {
    /* Figure out how many data segments were accumulated. */
    additional_segments = call_charge_ptr->segment_count -
              call_charge_ptr->segment_count_in_AOC;

    /* Apply the charges for the data portion of the call. */
    if(call_charge_ptr->e.e5 && (additional_segments >= call_charge_ptr->e.e5))
    {
      additional_e5s = additional_segments/call_charge_ptr->e.e5;
      call_charge_ptr->AOC += (call_charge_ptr->e.e6 * additional_e5s) *
                  call_charge_ptr->e.e3;
      call_charge_ptr->segment_count_in_AOC +=
        call_charge_ptr->e.e5 * additional_e5s;
    }

    /* Now charge the timed portion of the charges for the call. */
    if( call_charge_ptr->use_ccm_timeout && call_charge_ptr->ccm_timeout == 0 )
    {
      CM_MSG_HIGH_2("AOC: Incrementing AOC for call %d by %d", 
                  cmcall_call_id_get( call_ptr ),
      call_charge_ptr->e.e1 * call_charge_ptr->e.e3 );

      call_charge_ptr->AOC += call_charge_ptr->e.e1 * call_charge_ptr->e.e3;
      
      /*
      ** Set ccm_timeout with e2. (use_e7 = FALSE). if e2 is 0 
      ** dont start the timer
      */
      cmwaoc_set_ccm_timeout(call_charge_ptr, FALSE);
      
    }

    /* If the new E values have not yet been charged, charge them now. */
    if(call_charge_ptr->new_e_values_charged == FALSE)
    {
      /* The new e_values are now gonna be taken care of. */
      call_charge_ptr->new_e_values_charged = TRUE;

      /* copy new_e_values to the e_values */
      call_charge_ptr->e.e1 = call_charge_ptr->new_e.e1;
      call_charge_ptr->e.e2 = call_charge_ptr->new_e.e2;
      call_charge_ptr->e.e3 = call_charge_ptr->new_e.e3;
      call_charge_ptr->e.e4 = call_charge_ptr->new_e.e4;
      call_charge_ptr->e.e5 = call_charge_ptr->new_e.e5;
      call_charge_ptr->e.e6 = call_charge_ptr->new_e.e6;
      call_charge_ptr->e.e7 = call_charge_ptr->new_e.e7;


      /* Set ccm_timeout with e7 (use_e7 = TRUE). If e7 is 0
      ** use e2 to set the timer.
      */
      cmwaoc_set_ccm_timeout(call_charge_ptr, TRUE);

    }

    /* Put this call back in the chargelist. */
    cmwaoc_add_to_chargelist(call_ptr);

    /* With the new charges for this call, update the CCM. */
    cmwaoc_update_ccm(call_ptr);
  }
} /* cmwaoc_update_aoc */

/*===========================================================================

FUNCTION cmwaoc_end_call

DESCRIPTION
  This function is called when a call ends.  It accounts for any
  partial charges and makes sure the CCM and ACM are up to date.

DEPENDENCIES
  The call structure pointed to within call_ptr must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void cmwaoc_end_call(

     cmcall_s_type *call_ptr
      /* call to end */

)
{
  unsigned long additional_segments;
  unsigned long additional_e5s;
  cm_wcall_aoc_data_type *call_charge_ptr = NULL;
  cmph_s_type  *ph_ptr                    = cmph_ptr();
  sys_modem_as_id_e_type  asubs_id;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr           != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  asubs_id = cmcall_get_asubs_id(call_ptr);
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  if(asubs_id == SYS_MODEM_AS_ID_2)
  {
    if (cmph_is_sxlte())
    {
      #ifdef FEATURE_MMODE_SXLTE_G
      if(hybr_gw3_call_list_head != NULL)
      {
        cmwaoc_remove_call_from_list(call_ptr);
      }
      #endif
    }
    else
    {
      if(hybr_gw_call_list_head != NULL)
      {
        cmwaoc_remove_call_from_list(call_ptr);
      }
    }
    
  }
  else
  #ifdef FEATURE_MMODE_TRIPLE_SIM
  if(asubs_id == SYS_MODEM_AS_ID_3)
  {
    if(hybr_gw3_call_list_head != NULL)
    {
      cmwaoc_remove_call_from_list(call_ptr);
    }
  }
  else
  #endif
  {
    if(call_list_head != NULL)
    {
      cmwaoc_remove_call_from_list(call_ptr);
    }
  }

  if(!CMPH_SS_PHONE_CHARGE(asubs_id,ph_ptr).aoc_ready) 
  {
    return;
  }

  call_charge_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info.wcall_charge_data;

  additional_segments = call_charge_ptr->segment_count -
                        call_charge_ptr->segment_count_in_AOC;

  if(additional_segments > 0)
  {
    additional_e5s = (additional_segments + call_charge_ptr->e.e5 - 1) /
                     call_charge_ptr->e.e5;

    call_charge_ptr->AOC += (call_charge_ptr->e.e6 * additional_e5s) *
                            call_charge_ptr->e.e3;
  }

  cmwaoc_update_ccm(call_ptr);
  cmph_update_acm(asubs_id);
}

/*===========================================================================

FUNCTION cmwaoc_ccm_in_30

DESCRIPTION
  This function will figure out the value that the current call meter will be
  in 30 seconds at the given rates being charged.  This rate represents a
  best guess at the time being called.  For each call with associated charges,
  any time related charges accrued in 30 seconds will be added.  Additionally,
  for data calls, any data related charges will also be accrued.  This
  algorithm first accounts for any parts of a segment that may already have
  been transmitted.  It then computes the number of additional segments which
  will accrue at the maximum rate for the next 30 seconds.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
unsigned long cmwaoc_ccm_in_30(

   sys_modem_as_id_e_type  asubs_id
    /* sub whose acm needs to be updated */

)
{
  cmcall_s_type           *call;          
    /* Call currently being examined. */
  cm_wcall_aoc_data_type *call_charge_ptr;
  unsigned long           additional_aoc = 0; 
    /* Additional AOC to add to each call. */
  long                    time_left;               
    /* Used in the time computation. */
  unsigned long           increments;     
    /* Number of time increments in current call */
  cmph_s_type             *ph_ptr = cmph_ptr(); 
    /* Get the phone object. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT(ph_ptr != NULL);
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call = cmwaoc_get_list_head(asubs_id);

  if(call != NULL && !CMPH_SS_PHONE_CHARGE(asubs_id,ph_ptr).aoc_ready)
  {
    return 0;
  }

  /* For each call in the charge list, compute the charge in 30 seconds. */
  while (call != NULL)
  {
    /* Get the pointer to the charge structure. */
    call_charge_ptr =
                &call->cmcall_mode_info.info.gsm_wcdma_info.wcall_charge_data;

    /* Now check if there will be time related charges. */
    if(call_charge_ptr->e.e1 > 0 && call_charge_ptr->e.e2 > 0)
    {
      /* Check how much time is left for this call. */
      time_left = (long) ((30 * 1000) - call_charge_ptr->ccm_timeout);
      if(time_left > 0)
      {
        increments = (unsigned long) (1 + time_left/
                         (long) (call_charge_ptr->e.e2 * MS_PER_TIMEUNIT));
        additional_aoc += increments *
                        call_charge_ptr->e.e1 * call_charge_ptr->e.e3;
      }
    }

    /* If there are data charges associated with the call, account for those
    ** now. 
    */ 
    if(call_charge_ptr->segments_per_sec > 0 &&
        call_charge_ptr->e.e5 > 0 && call_charge_ptr->e.e6 > 0)
    {
      additional_aoc += (call_charge_ptr->e.e3 * call_charge_ptr->e.e6) *
                  (unsigned long) (call_charge_ptr->segment_count -
                   (call_charge_ptr->segment_count_in_AOC +
                   (30 * call_charge_ptr->segments_per_sec)/call_charge_ptr->e.e5));
    }
    call = call->next_charged_call;
  }

  return(CMPH_SS_PHONE_CHARGE(asubs_id,ph_ptr).CCM + additional_aoc);
}


/*===========================================================================

FUNCTION cmwaoc_update_ccm_timeout_in_calls

DESCRIPTION
  Updates ccm_timeout field in calls by getting the expired time value from 
  ccm_timer in phone

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Update ccm_timeout field in calls
===========================================================================*/
static void cmwaoc_update_ccm_timeout_in_calls(

   sys_modem_as_id_e_type  asubs_id
    /* sub whose acm needs to be updated */

)
{
  rex_timer_type *timer_to_set_ptr;   /* pointer to timer to set */
  rex_timer_cnt_type timer_cnt;       /* value to set the timer   */
  rex_timer_cnt_type elapsed_cnt;     /* value to set the timer   */

  cmcall_s_type *call_ptr = NULL;
  cmph_s_type   *ph_ptr = cmph_ptr();
  cm_wcall_aoc_data_type *call_charge_ptr = NULL;

  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  call_ptr = cmwaoc_get_list_head(asubs_id);
  call_charge_ptr =
        &call_ptr->cmcall_mode_info.info.gsm_wcdma_info.wcall_charge_data;

  /* ccm_timer in phone is initialized with ccm_timeout of first call in
  ** call_list_head. Phone's ccm_timer counts down and expires when it  
  ** reaches 0.
  */
  if(asubs_id == SYS_MODEM_AS_ID_2)
  {
    #if defined(FEATURE_MMODE_SXLTE_G) || defined(FEATURE_MMODE_TRIPLE_SIM)
    if (cmph_is_sxlte())
    {
      timer_to_set_ptr = &ph_ptr->hybr_3_stack_info.phone_charge.ccm_timer;
    }
    else
    #endif
    {
      timer_to_set_ptr = &ph_ptr->hybr_2_stack_info.phone_charge.ccm_timer;
    }
  }
  else
  #ifdef FEATURE_MMODE_TRIPLE_SIM
  if(asubs_id == SYS_MODEM_AS_ID_3)
  {
    timer_to_set_ptr = &ph_ptr->hybr_3_stack_info.phone_charge.ccm_timer;
  }
  else
  #endif
  {
    timer_to_set_ptr = &ph_ptr->main_stack_info.phone_charge.ccm_timer;
  }

  timer_cnt = rex_get_timer(timer_to_set_ptr);
  elapsed_cnt = call_charge_ptr->ccm_timeout - timer_cnt;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  while (call_ptr != NULL)
  {
    if(call_charge_ptr->use_ccm_timeout)
    {
      if(call_charge_ptr->ccm_timeout > elapsed_cnt)
      {
        call_charge_ptr->ccm_timeout -= elapsed_cnt;
      }
      else
      {
        call_charge_ptr->ccm_timeout = 0;
      }
    }
    else
    {
      break;
    }

    call_ptr = call_ptr->next_charged_call;

    if(call_ptr)
    {
      call_charge_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info.wcall_charge_data;
    }

  } /* while */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* cmwaoc_update_ccm_timeout_in_calls () */


/*===========================================================================

FUNCTION cmwaoc_insert_to_chargelist  

DESCRIPTION
  Inserts given call object in charge list

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Updates call charge list

===========================================================================*/
static void cmwaoc_insert_to_chargelist (

  cmcall_s_type *call_ptr
    /* call object to be inserted */

)
{
  cm_wcall_aoc_data_type *call_charge_ptr = NULL;
  cmcall_s_type *list_call_ptr = NULL;
  cm_wcall_aoc_data_type *list_call_charge_ptr = NULL;
  sys_modem_as_id_e_type   asubs_id = cmcall_get_asubs_id(call_ptr);
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  list_call_ptr = cmwaoc_get_list_head(asubs_id);
  list_call_charge_ptr =
      &list_call_ptr->cmcall_mode_info.info.gsm_wcdma_info.wcall_charge_data;
  call_charge_ptr =
           &call_ptr->cmcall_mode_info.info.gsm_wcdma_info.wcall_charge_data;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!list_call_charge_ptr->use_ccm_timeout ||
        list_call_charge_ptr->ccm_timeout >= call_charge_ptr->ccm_timeout)
  {
     call_ptr->next_charged_call = cmwaoc_get_list_head(asubs_id);
     cmwaoc_set_list_head(asubs_id, call_ptr);
  }
  else
  {    
    while (list_call_ptr->next_charged_call != NULL)
    {
      /* list_call_charge_ptr keeps track of the next call's charges */
      list_call_charge_ptr =
          &list_call_ptr->next_charged_call->cmcall_mode_info.info.gsm_wcdma_info.wcall_charge_data;

      /* If the next call is after the one being inserted, break out of the
      ** while loop. 
      */ 
      if(!list_call_charge_ptr->use_ccm_timeout ||
          list_call_charge_ptr->ccm_timeout >= call_charge_ptr->ccm_timeout)
      {
        break;
      }

      /* Otherwise, the next call is after the one being inserted, so move
      ** to it and see what the one after it looks like.
       */
      list_call_ptr = list_call_ptr->next_charged_call;
    }

    /* At this point, we know that call_ptr should go after list_call_ptr and
    ** before list_call_ptr->next_charged_call.  Therefore, make call_ptr's
    ** next_charged_call be list_call_ptr's next_charged_call, and then make
    ** list_call_ptr's next charged call equal to the one being inserted. 
    */ 
    call_ptr->next_charged_call = list_call_ptr->next_charged_call;
    list_call_ptr->next_charged_call = call_ptr;

  } /* else */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* cmwaoc_insert_to_chargelist () */


/*===========================================================================

FUNCTION cmwaoc_remove_call_from_list

DESCRIPTION
  This function will remove the given call from the list of charging calls.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void cmwaoc_remove_call_from_list(

  const cmcall_s_type *call_ptr

)
{
  cmph_s_type    *ph_ptr       = cmph_ptr();
  cmcall_s_type   *tmp_call_ptr = NULL;
  boolean         ccm_running   = FALSE;
  rex_timer_type  *ccm_timer    = NULL;
  sys_modem_as_id_e_type  asubs_id;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr           != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  asubs_id = cmcall_get_asubs_id(call_ptr);
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  ccm_running   = CMPH_SS_PHONE_CHARGE(asubs_id,ph_ptr).ccm_timer_running;

  if(asubs_id == SYS_MODEM_AS_ID_2)
  {
    #if defined(FEATURE_MMODE_SXLTE_G) || defined(FEATURE_MMODE_TRIPLE_SIM)
    if (cmph_is_sxlte())
    {
      ccm_timer = &ph_ptr->hybr_3_stack_info.phone_charge.ccm_timer;
    }
    else
    #endif
    {
      ccm_timer = &ph_ptr->hybr_2_stack_info.phone_charge.ccm_timer;
    }
  }
  else
  #ifdef FEATURE_MMODE_TRIPLE_SIM
  if(asubs_id == SYS_MODEM_AS_ID_3)
  {
    ccm_timer = &ph_ptr->hybr_3_stack_info.phone_charge.ccm_timer;
  }
  else
  #endif
  {
    ccm_timer = &ph_ptr->main_stack_info.phone_charge.ccm_timer;
  }

  tmp_call_ptr  = cmwaoc_get_list_head(asubs_id);

  if(!CMPH_SS_PHONE_CHARGE(asubs_id,ph_ptr).aoc_ready || tmp_call_ptr == NULL)
  {
    return;
  }

  if(tmp_call_ptr == call_ptr)
  {
    cmwaoc_set_list_head(asubs_id,tmp_call_ptr->next_charged_call);
  }
  else
  {
    while ((tmp_call_ptr != NULL) && (tmp_call_ptr->next_charged_call != call_ptr))
    {
      tmp_call_ptr = tmp_call_ptr->next_charged_call;
    }

    if(tmp_call_ptr != NULL)
    {
      tmp_call_ptr->next_charged_call = call_ptr->next_charged_call;
    }
  }

  if((cmwaoc_get_list_head(asubs_id) == NULL) && ccm_running)
  {
    (void) rex_clr_timer(ccm_timer);
  }
}


/*===========================================================================

FUNCTION cmwaoc_manage_ph_ccm_timer

DESCRIPTION
  Checks if phone ccm_timer has to be started.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Writes to Ph Obj
===========================================================================*/
static void cmwaoc_manage_ph_ccm_timer (

   sys_modem_as_id_e_type  asubs_id
    /* sub whose acm needs to be updated */
)
{

  cm_wcall_aoc_data_type *list_call_charge_ptr = NULL;

  cmph_s_type            *ph_ptr               = cmph_ptr ();

  cm_ph_charge_data_type *phone_charge         = NULL;

  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If there are no calls in CCM list, timer is not started
  */
  if(!cmwaoc_get_list_head(asubs_id))
  {
    return;
  }

  list_call_charge_ptr =
      &cmwaoc_get_list_head(asubs_id)->cmcall_mode_info.info.gsm_wcdma_info.wcall_charge_data;

  if(asubs_id == SYS_MODEM_AS_ID_2)
  {
    #if defined(FEATURE_MMODE_SXLTE_G) || defined(FEATURE_MMODE_TRIPLE_SIM)
    if (cmph_is_sxlte())
    {
      phone_charge = &ph_ptr->hybr_3_stack_info.phone_charge;
    }
    else
    #endif
    {
      phone_charge = &ph_ptr->hybr_2_stack_info.phone_charge;
    }
  }
  else
  #ifdef FEATURE_MMODE_TRIPLE_SIM
  if(asubs_id == SYS_MODEM_AS_ID_3)
  {
    phone_charge = &ph_ptr->hybr_3_stack_info.phone_charge;
  }
  else
  #endif
  {
    phone_charge = &ph_ptr->main_stack_info.phone_charge;
  }

  if(list_call_charge_ptr->ccm_timeout > 0)
  {
    (void)rex_set_timer(&(phone_charge->ccm_timer),
                    list_call_charge_ptr->ccm_timeout);

    phone_charge->ccm_timer_running = TRUE;
  }
  else
  {
    phone_charge->ccm_timer_running = FALSE;
  }

}


/*===========================================================================

FUNCTION cmwaoc_add_to_chargelist

DESCRIPTION
  This function takes a call and adds it to the current list of calls
  with associated charges.  To do this, it first stops the current timer
  and subtracts any elapsed time.  It then places the item into the list
  and restarts the timer accordingly.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void cmwaoc_add_to_chargelist(

  cmcall_s_type *call_ptr

)
{
  cmph_s_type            *ph_ptr          = cmph_ptr();  
  boolean                 ccm_running     = FALSE;  
  cm_wcall_aoc_data_type *call_charge_ptr = NULL;
  sys_modem_as_id_e_type  asubs_id;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr           != NULL); 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  asubs_id = cmcall_get_asubs_id(call_ptr);
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  call_charge_ptr = &call_ptr->cmcall_mode_info.info.gsm_wcdma_info.wcall_charge_data;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If either aoc is not ready or charge meter timeout for call is 0  
  ** return.
  */
  if ( !(CMPH_SS_PHONE_CHARGE(asubs_id,ph_ptr).aoc_ready) ||
       (call_charge_ptr->ccm_timeout == 0)
     )
  {
    return;
  }
   
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/     

  rex_enter_crit_sect(cmtask_crit_sect_ptr());
  /* Serialize data access hybr_gw/call_list_head */

  if(cmwaoc_get_list_head(asubs_id) == NULL)
  {
    cmwaoc_set_list_head(asubs_id, call_ptr);
  }
  else
  {
    ccm_running = CMPH_SS_PHONE_CHARGE(asubs_id, ph_ptr).ccm_timer_running;

    if(ccm_running)
    {
      /* Updates time used up by all calls in aoc list */
      cmwaoc_update_ccm_timeout_in_calls (asubs_id);
    }

    /* Inserts call to charge list */
    cmwaoc_insert_to_chargelist (call_ptr);    
  }

  /* Restarts phone ccm timer if there are calls in AOC list */
  cmwaoc_manage_ph_ccm_timer (asubs_id);

  rex_leave_crit_sect(cmtask_crit_sect_ptr());

}


/*===========================================================================

FUNCTION cmwaoc_handle_ccm_timeout();

DESCRIPTION
  This function handles the updates when the CCM timer times out.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void cmwaoc_handle_ccm_timeout(

   sys_modem_as_id_e_type  asubs_id
    /* sub whose acm needs to be updated */

)
{
  cmcall_s_type          *tmp_call_ptr;
  cm_wcall_aoc_data_type *tmp_charge_ptr;  
  cmph_s_type            *ph_ptr = cmph_ptr();

  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  if((cmwaoc_get_list_head(asubs_id) == NULL) || 
     !CMPH_SS_PHONE_CHARGE(asubs_id,ph_ptr).aoc_ready)
  {
    return;
  }

  rex_enter_crit_sect(cmtask_crit_sect_ptr());
  /* Serialize data access hybr_gw/call_list_head */

  if(asubs_id == SYS_MODEM_AS_ID_2)
  {
    #if defined(FEATURE_MMODE_SXLTE_G) || defined(FEATURE_MMODE_TRIPLE_SIM)
    if (cmph_is_sxlte())
    {
      ph_ptr->hybr_3_stack_info.phone_charge.ccm_timer_running = FALSE;
    }
    else
    #endif
    {
      ph_ptr->hybr_2_stack_info.phone_charge.ccm_timer_running = FALSE;
    }
  }
  else
  #ifdef FEATURE_MMODE_TRIPLE_SIM
  if(asubs_id == SYS_MODEM_AS_ID_3)
  {
    ph_ptr->hybr_3_stack_info.phone_charge.ccm_timer_running = FALSE;
  }
  else
  #endif
  {
    ph_ptr->main_stack_info.phone_charge.ccm_timer_running = FALSE;
  }

  cmwaoc_update_ccm_timeout_in_calls (asubs_id);
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  while (cmwaoc_get_list_head(asubs_id) != NULL)
  {
    tmp_call_ptr = cmwaoc_get_list_head(asubs_id);
    tmp_charge_ptr =
        &tmp_call_ptr->cmcall_mode_info.info.gsm_wcdma_info.wcall_charge_data;

    if(tmp_charge_ptr->use_ccm_timeout && tmp_charge_ptr->ccm_timeout == 0)
    {
      cmwaoc_remove_call_from_list(tmp_call_ptr);
      cmwaoc_update_aoc(tmp_call_ptr);
    }
    else
    {
      break;
    }
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Restarts ccm timer if there are further calls in AOC call list
  */
  cmwaoc_manage_ph_ccm_timer (asubs_id);

  rex_leave_crit_sect(cmtask_crit_sect_ptr());
}

/*===========================================================================

FUNCTION cmwaoc_reset

DESCRIPTION
  This function resets Advice of charge parameters.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void cmwaoc_reset (void)
{
  cmph_s_type *ph_ptr = cmph_ptr(); /* Get the phone object. */
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do not memset phone_charge to 0. It clears rex timers (ccm_timer, 
  ** acm_timer) that have to be set again.
  */
  ph_ptr->main_stack_info.phone_charge.drop_calls       = FALSE;
  ph_ptr->main_stack_info.phone_charge.sent_warning       = FALSE;
  ph_ptr->main_stack_info.phone_charge.ccm_timer_running    = FALSE;
  ph_ptr->main_stack_info.phone_charge.acm_timer_running    = FALSE;
  ph_ptr->main_stack_info.phone_charge.last_CCM_to_update_ACM = CM_DEFAULT_VALUE;
  ph_ptr->main_stack_info.phone_charge.CCM            = CM_DEFAULT_VALUE;
  ph_ptr->main_stack_info.phone_charge.aoc_ready        = FALSE;
  ph_ptr->main_stack_info.phone_charge.ACM            = CM_DEFAULT_VALUE;
  ph_ptr->main_stack_info.phone_charge.ACMmax         = CM_DEFAULT_VALUE;
  call_list_head = NULL;

  hybr_gw_call_list_head = NULL;
  if (cmph_is_msim())
  {
    ph_ptr->hybr_2_stack_info.phone_charge.drop_calls       = FALSE;
    ph_ptr->hybr_2_stack_info.phone_charge.sent_warning     = FALSE;
    ph_ptr->hybr_2_stack_info.phone_charge.ccm_timer_running    = FALSE;
    ph_ptr->hybr_2_stack_info.phone_charge.acm_timer_running    = FALSE;
    ph_ptr->hybr_2_stack_info.phone_charge.last_CCM_to_update_ACM = CM_DEFAULT_VALUE;
    ph_ptr->hybr_2_stack_info.phone_charge.CCM          = CM_DEFAULT_VALUE;
    ph_ptr->hybr_2_stack_info.phone_charge.aoc_ready        = FALSE;
    ph_ptr->hybr_2_stack_info.phone_charge.ACM          = CM_DEFAULT_VALUE;
    ph_ptr->hybr_2_stack_info.phone_charge.ACMmax         = CM_DEFAULT_VALUE;


  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  hybr_gw3_call_list_head = NULL;
  ph_ptr->hybr_3_stack_info.phone_charge.drop_calls       = FALSE;
  ph_ptr->hybr_3_stack_info.phone_charge.sent_warning     = FALSE;
  ph_ptr->hybr_3_stack_info.phone_charge.ccm_timer_running    = FALSE;
  ph_ptr->hybr_3_stack_info.phone_charge.acm_timer_running    = FALSE;
  ph_ptr->hybr_3_stack_info.phone_charge.last_CCM_to_update_ACM = CM_DEFAULT_VALUE;
  ph_ptr->hybr_3_stack_info.phone_charge.CCM          = CM_DEFAULT_VALUE;
  ph_ptr->hybr_3_stack_info.phone_charge.aoc_ready        = FALSE;
  ph_ptr->hybr_3_stack_info.phone_charge.ACM          = CM_DEFAULT_VALUE;
  ph_ptr->hybr_3_stack_info.phone_charge.ACMmax         = CM_DEFAULT_VALUE;
  #endif
  }
} /* cmwaoc_reset */


/*===========================================================================


FUNCTION cmwaoc_get_ccm

DESCRIPTION
  This function returns the current value of the Current Call Meter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
unsigned long cmwaoc_get_ccm(

  cm_ss_e_type   ss
    /* stack whise ccm to retrieve */

)
{
  cmph_s_type  *ph_ptr = cmph_ptr(); 
  sys_modem_as_id_e_type  asubs_id = cmph_map_cm_ss_to_subs(ss);

  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  return CMPH_SS_PHONE_CHARGE(asubs_id, ph_ptr).CCM;

}


/*===========================================================================

FUNCTION cmwaoc_reset_acm_mmgsdi

DESCRIPTION
  This function sets the value of the ACM to 0.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void cmwaoc_reset_acm_mmgsdi(

     cm_ss_e_type   ss
      /* stack whose acm needs to be reset */

)
{
  cmph_s_type                  *ph_ptr = cmph_ptr();
   /* Get the phone object. */
  uint8                        temp_value[CM_AOC_ACM_SIZE];
  mmgsdi_file_enum_type        mmgsdi_acm_filename = MMGSDI_NO_FILE_ENUM;
  mmgsdi_write_data_type       write_data;
  mmgsdi_return_enum_type      status = MMGSDI_ERROR;
  cm_ph_charge_data_type       *phone_charge;

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_access_type           file_access;
  mmgsdi_session_id_type       cm_mmgsdi_session_id;
  cm_mmgsdi_error_e_type       cm_mmgsdi_status = CM_MMGSDI_NOTFOUND;
  #endif /* FEATURE_MMGSDI_SESSION_LIB */
  mmgsdi_session_type_enum_type session_type = MMGSDI_GW_PROV_PRI_SESSION;
  mmgsdi_app_enum_type          app_type;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #if defined(FEATURE_MMODE_TRIPLE_SIM)
  if(ss == CM_SS_HYBR_3)
  {  
    phone_charge = &(ph_ptr->hybr_3_stack_info.phone_charge);
    if (cmph_is_sxlte())
    {
      session_type = MMGSDI_GW_PROV_SEC_SESSION;
    }
    else
    {
      session_type = MMGSDI_GW_PROV_TER_SESSION;
    }
  }
  else
  #endif
  if(ss == CM_SS_HYBR_2)
  {  
    phone_charge = &(ph_ptr->hybr_2_stack_info.phone_charge);
    session_type = MMGSDI_GW_PROV_SEC_SESSION;
  }
  else
  {  
    phone_charge = &(ph_ptr->main_stack_info.phone_charge);
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  phone_charge->ACM = 0;
  phone_charge->sent_warning = FALSE;
  phone_charge->drop_calls = FALSE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(!phone_charge->aoc_ready)
  {  
    CM_MSG_HIGH_0 ("AOC not ready"); 
    return;
  }

  temp_value [ 0 ] = 0;
  temp_value [ 1 ] = 0;
  temp_value [ 2 ] = 0;

  /*lint -e{718}*/
  /*lint -esym(628, cm_mmgsdi_ssn_tbl_get_app_type_for_type) */
  app_type = cm_mmgsdi_ssn_tbl_get_app_type_for_type(session_type);
  
  
  switch (app_type)
  {
    case MMGSDI_APP_USIM:
      mmgsdi_acm_filename = MMGSDI_USIM_ACM;
      break;

    case MMGSDI_APP_SIM:
      mmgsdi_acm_filename = MMGSDI_GSM_ACM;
      break;

    default:
      CM_ERR_1("Invalid App type: %x", app_type);
  }

  write_data.data_len = CM_AOC_ACM_SIZE;
  write_data.data_ptr = temp_value;

  #ifdef FEATURE_MMGSDI_SESSION_LIB

  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = mmgsdi_acm_filename;

  cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_get_id_for_type(session_type, 
                                                  &cm_mmgsdi_session_id);
  if (CM_MMGSDI_SUCCESS != cm_mmgsdi_status)
  {
     CM_ERR_2("Unable to get session id; session type: %d, err: %d", 
                  session_type, cm_mmgsdi_status); /*lint !e641 */
  }
  else
  {
    file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
    file_access.file.file_enum = mmgsdi_acm_filename;

    status = mmgsdi_session_write_record ( cm_mmgsdi_session_id,
                                           file_access,
                                           MMGSDI_CYCLIC_FILE,
                                           1,
                                           write_data,
                                           cmwaoc_mmgsdi_async_cb,
                                           NULL
                                         );
  }  
  #else 
  status = mmgsdi_write_record ( cm_mmgsdi_get_client_id(),
                                 MMGSDI_SLOT_1,
                                 mmgsdi_acm_filename,
                                 1,
                                 write_data,
                                 cmwaoc_mmgsdi_async_cb,
                                 NULL
                               );
  #endif /* FEATURE_MMGSDI_SESSION_LIB */

  if (MMGSDI_SUCCESS != status)
  {
    phone_charge->aoc_ready = FALSE;
    CM_ERR_0("Could not write to SIM.");
  }

}  /* cmwaoc_reset_acm_mmgsdi() */



/*===========================================================================

FUNCTION cmwaoc_get_acm

DESCRIPTION
  This function returns the current value of the Accumulated Call Meter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
unsigned long cmwaoc_get_acm(

  sys_modem_as_id_e_type  asubs_id
    /* sub whose max acm is requested */

)
{
  cmph_s_type  *ph_ptr = cmph_ptr(); 
      /* Get the phone object. */
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  return CMPH_SS_PHONE_CHARGE(asubs_id, ph_ptr).ACM;
}


/*===========================================================================

FUNCTION cmwaoc_get_acmmax

DESCRIPTION
  This function returns the current value of the maximum value the
  Accumulated Call Meter can achieve before calls are killed..

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
unsigned long cmwaoc_get_acmmax(

  sys_modem_as_id_e_type  asubs_id
    /* sub whose max acm is requested */
)
{
  cmph_s_type  *ph_ptr = cmph_ptr(); 
      /* Get the phone object. */
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  return CMPH_SS_PHONE_CHARGE(asubs_id, ph_ptr).ACMmax;
}



/*===========================================================================

FUNCTION cmwaoc_set_acmmax_mmgsdi

DESCRIPTION
  This function sets the maximum value Accumulated Call Meterof. The maximum 
  value of the Accumulated Call Meter can achieved before calls are killed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void cmwaoc_set_acmmax_mmgsdi(

  cm_ss_e_type            ss,

  unsigned long           new_max

)
{
  uint8                        temp_value[CM_AOC_ACM_SIZE];
  cmph_s_type                  *ph_ptr = cmph_ptr(); 
       /* Get the phone object. */
  mmgsdi_file_enum_type        mmgsdi_acmmax_filename = MMGSDI_NO_FILE_ENUM;
  mmgsdi_write_data_type       write_data;
  cm_ph_charge_data_type       *phone_charge;
  mmgsdi_return_enum_type      status = MMGSDI_ERROR;

  #ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_session_id_type           cm_mmgsdi_session_id;
  cm_mmgsdi_error_e_type           cm_mmgsdi_status = CM_MMGSDI_NOTFOUND;
  mmgsdi_access_type               file_access;
  #endif /* FEATURE_MMGSDI_SESSION_LIB */
  mmgsdi_app_enum_type           app_type;
  mmgsdi_session_type_enum_type    cm_mmgsdi_session_type = MMGSDI_GW_PROV_PRI_SESSION;
  sys_modem_as_id_e_type              asubs_id = cmph_map_cm_ss_to_subs(ss);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(asubs_id == SYS_MODEM_AS_ID_2)
  {
    #if defined(FEATURE_MMODE_SXLTE_G) || defined(FEATURE_MMODE_TRIPLE_SIM)
    if (cmph_is_sxlte())
    {
      phone_charge = &(ph_ptr->hybr_3_stack_info.phone_charge);
    }
    else
    #endif
    {
      phone_charge = &(ph_ptr->hybr_2_stack_info.phone_charge);
    }
    cm_mmgsdi_session_type = MMGSDI_GW_PROV_SEC_SESSION;
  }
  else
  #ifdef FEATURE_MMODE_TRIPLE_SIM
  if(asubs_id == SYS_MODEM_AS_ID_3)
  {
    phone_charge = &(ph_ptr->hybr_3_stack_info.phone_charge);
    cm_mmgsdi_session_type = MMGSDI_GW_PROV_TER_SESSION;
  }
  else
  #endif
  {  
    phone_charge = &(ph_ptr->main_stack_info.phone_charge);
  }
     
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!phone_charge->aoc_ready)
  {
    CM_MSG_HIGH_0 ("AOC not ready"); 
    return;
  }

  phone_charge->ACMmax = new_max;

  /* If they are adding more funds, allow calls and reset the warning */
  if((new_max == 0) || (new_max > phone_charge->ACM))
  {
    phone_charge->drop_calls = FALSE;
    phone_charge->sent_warning = FALSE;
  }
  else
  {
    /* They are not allowing enough funds, make sure we don't allow calls. */
    cmph_aoc_no_funds(asubs_id);
  }

  temp_value [ 0 ] = (uint8) ((new_max >> 16) & 0xff);
  temp_value [ 1 ] = (uint8) ((new_max >> 8 ) & 0xff);
  temp_value [ 2 ] = (uint8) ((new_max      ) & 0xff);

  app_type = (mmgsdi_app_enum_type)
              cm_mmgsdi_ssn_tbl_get_app_type_for_type(cm_mmgsdi_session_type);

  switch (app_type)
    {
    case MMGSDI_APP_USIM:
        mmgsdi_acmmax_filename = MMGSDI_USIM_ACM_MAX;
        break;

    case MMGSDI_APP_SIM:
        mmgsdi_acmmax_filename = MMGSDI_GSM_ACM_MAX;
        break;

      default:
      CM_ERR_1("Invalid App type: %x", app_type);
  }
 
  write_data.data_len = CM_AOC_ACM_SIZE;
  write_data.data_ptr = temp_value;

  #ifdef FEATURE_MMGSDI_SESSION_LIB

  cm_mmgsdi_status = cm_mmgsdi_ssn_tbl_get_id_for_type(cm_mmgsdi_session_type, 
                                                  &cm_mmgsdi_session_id);
  if (CM_MMGSDI_SUCCESS != cm_mmgsdi_status)
  {
    CM_ERR_2("Unable to get session id; session type: %d, err: %d", 
                 cm_mmgsdi_session_type, cm_mmgsdi_status ); /*lint !e641 */
  }
  else
  { 
    file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
    file_access.file.file_enum = mmgsdi_acmmax_filename;

    status = mmgsdi_session_write_transparent ( cm_mmgsdi_session_id,
                                                file_access,
                                                0,
                                                write_data,
                                                cmwaoc_mmgsdi_async_cb,
                                                NULL
                                              );
  }  
  #else 
  status = mmgsdi_write_transparent ( cm_mmgsdi_get_client_id(),
                                      MMGSDI_SLOT_1,
                                      mmgsdi_acmmax_filename,
                                      0,
                                      write_data,
                                      cmwaoc_mmgsdi_async_cb,
                                      NULL
                                    );
  #endif /* FEATURE_MMGSDI_SESSION_LIB */

  if (MMGSDI_SUCCESS != status)
  {
    phone_charge->aoc_ready = FALSE;
    CM_ERR_0("Could not write to SIM.");
  }
} /* cmwaoc_set_acmmax_mmgsdi() */


/*===========================================================================

FUNCTION cmwaoc_get_puct

DESCRIPTION
  Once implemented, this function will return the Price/Unit Currency
  Value.  This value can be applied to ACM or CCM to get the cost in
  a currency you care about.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
unsigned long cmwaoc_get_puct(void)
{

  return 1;
}


#else /* FEATURE_WCDMA || FEATURE_GSM */

/*===========================================================================
FUNCTION cmwaoc_dummy

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
void cmwaoc_dummy( void )
/*lint -esym(714,cmwaoc_dummy) */
/*lint -esym(765,cmwaoc_dummy)
** Can't be static as it would result in compiler warnings 
*/ 
{
}


#endif /* FEATURE_WCDMA || FEATURE_GSM */


