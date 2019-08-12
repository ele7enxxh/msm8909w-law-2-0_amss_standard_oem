/*===========================================================================
         R R C   R A D I O   B E A R E R   C O M M O N   M O D U L E 

DESCRIPTION

  This module performs common functions related to the Radio Bearer
  procedures.
  
EXTERNALIZED FUNCTIONS

  rrcrb_translate_gsm_map_rab_id    Translates a RAB Id from ASN format to 
                                    internal format.


INITIALIZATION AND SEQUENCING REQUIREMENTS

    
Copyright (c) 2000, 2001, 2003-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcrbcommon.c_v   1.3   03 Sep 2001 15:57:16   rjeenaga  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcrbcommon.c#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------  
05/25/16   sp      Made changes for Mav debug snapshot
05/09/16   nr      Made changes to reduce the F3s printed in get_am_index
10/16/15   as      Added code to support LTA durting activation time
06/12/15   sp      Fixed KW errors
02/04/15   aa      Made changes to F3 for fach to DCH scenario in directed cell validation
08/04/14   sp      Made changes to fix compilation errors
07/24/14   sp      Made changes to update re-establihment redesign F3s.
07/21/14   sg      Made changes to reduce F3s
06/17/14   sp      Made changes to mainline DCH-FACH transition optimization changes
06/17/14   sp      Made changes to mainline RLC-restablihment redesign changes
06/04/14   as      Made changes for DCH-FACH transition time optimization
06/02/14   sp      Made changes for FR19997 - UL rlc re-establishment redesign
05/16/14   vi      Made changes to support MOB framework
05/06/14   geg     Use safer versions of memcpy() and memmove()
04/10/14   db      Made changes to detect UL size change during FACH to FACH redirection
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
03/28/14   vi      Made changes to set drx to 320ms if rrc_constant_320ms_drx_nv is set
03/14/14   db      Made changes to set the RB action to RECONFIG_RB in rrc_fill_rb_id_chan_req_from_est_rabs()
03/14/14   sp      Made changes to avoid unnecessary re-establishment while coming out of pch state
03/13/14   db      Made changes to check for UL size change if reest entity is not set for both DL and UL
03/08/14   vg      Made changes to reduce the stack size usage of some RRC functions which has the usage more than 500 bytes.
01/22/14   ad      Made changes to reestablish DL and UL both even if one-sided reestablishment
                   flag is FALSE
01/09/14   sp      Removed code under FEATURE_CS_VOICE_OVER_HSPA feature
11/26/13   vg      Made changes to fix side effect of 571464 on Dime3.0
11/22/13   vg      Made changes to ensure CC does not have RLC size from OTA when transitioning from DCH to FACH and "same_as_before" not used for AM RBs.
09/24/13   ad      Made changes to correct the state check during OTA processing to set 
                   hsrach action. Made changes to change the RLC cfg mask so that any changes in RLC UL 
                   size get reflected because of EUL mapping changes
11/13/13   sa      Mainlined the FEATURE_MVS_UNDER_GHDI
07/24/13   ad      Added code to forcefully reconfigure RLC for Fixed to flexible LI 7
07/08/13   ad      Made changes to remove redundant F3s
06/27/13   gv      Made changes to buffer the MM and RR commands for crash debugging
06/27/13   sa      Added changes to support call type mask for AMR with PS0x0 config
05/21/13   db      Made changes to check for flexible size in OC for DCH to FACH scenario
04/19/13   sa      Made changes to update the call type mask for inter system handover from G to W with default config id present 
03/14/13   md      Made changes to provide active subscription ID to NAS APIs for DSDA
03/08/13   mp      Corrected DB used to detect flexible to fixed rlc size change for DCH to FACH transition.
02/04/13   ad      Corrected indentation
02/01/13   ad      Made changes to check for DL size change only if rrcllc_get_am_dl_rlc_size_otae returns 
                   INVALID size except for dch to fach transition
01/03/13   ad      Made changes to reduce the redundant F3s
11/22/12   sa      Made changes to update the call type mask for CS data 
10/22/12   md      Made changes to avoid RLC re-establishment for fixed to flexible Li 7 size change.
10/04/12   md      Made changes to detect RLC size change during state transitions via CELL PCH state.
10/04/12   pm      Removed unnecessary comments
10/04/12   pm      Made sure SRB's are present in Chan_config_req during FACH to DCH transition
08/17/12   sa      Made the changes to use the toc pointer instead of OC while checking the onesidedReest
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/31/12   md      Merged MAC_I fixes.
05/10/12   md      Added code changes to support FEATURE_MAC_I
05/05/12   md      Made changes to re-establish rlc entity only if rlc sze changes.
04/12/12   as      Added code to support feature NB/WB handover support
03/12/12   rl      Made changes to fix the FEATURE_WCDMA_NEW_L2_INTERFACE porting issue.
02/03/12   mn      Added APIs to provide UL AMR and AMR-WB mode
12/21/11   ad      Made changes to bring the mvs code outside of qchat feature
12/13/11   sks     Made changes to support LTOW SRVCC.
07/08/11   gv      Fixed KW errors
06/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.99.50
05/26/11   ad      Fixed compiler warnings
05/24/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.98.50
05/18/11   ad      Added changes to include UM RBs also in channel config request 
                   from established rabs
05/03/11   rl      Fixed the lint error
04/27/11   sks     Fixed compilation error when FEATURE_VOC_AMR_WB is defined.
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/13/11   ad      Changes to address the rate change issue for voice call
                   Qchat media concurrency scenarios
03/28/11   rl      Merged the HS-FACH/PCH changes from mainline
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   sks     Added support for LTOW PSHO.
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/18/11   rm      Added definiton of rrcrb_proc_waiting_for_l2_ack
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/29/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.85.50
11/19/10   ssg     Added changes for new WCDMA-L2 interface.
11/19/10   kp      Added code to avoid bufferoverflow of chan_config_req->rb
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
09/17/10   su      Featurizing MVS related code
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
08/21/10   ss      Fixed compilation warnings.
06/30/10   gkg     Fixed Compiler Warning.
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
05/10/10   gkg     Fixed Compile Warnings.
05/07/10   prk     Fixed compiler warnings.
04/26/10   ss      Added code changes for Fast Dormancy feature under feature flag
                   FEATURE_3GPP_FAST_DORMANCY
03/17/10   gkg     To comply with CMI, featurize Callback function pointer 
                   rrccu_rlc_ul_data_ind with feature FEATURE_DSM_WM_CB
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
02/11/10   gkg     Made changes for Opt Timer for CSoHS.
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
12/21/09   gkg     Made changes for CSoHSPA under feature FEATURE_CS_VOICE_OVER_HSPA.
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
10/28/09   gkg     Fixed KW Errors.
10/13/09   gkg     Made the changes such that Codec info is set to RRC_CODEC_NONE
                   if CS DATA TYPE is mentioned in RRC_SYNC_IND.
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
04/08/09   gkg     Made changes so that SCUDIF is handled based on derived
                   configuration instead of relying on MM.
09/11/09   ps      Made changes for reducing the internal RRC command size 
                   under FEATURE_RRC_REDUCE_CMD_SIZE
08/21/09   ss      Fixed correct format specifiers in MSG_HIGH
08/17/09   ss      Added code to provide ciphering ON/OFF indication to UI 
                   under feature flag FEATURE_UMTS_VOICE_CIPHERING_IND.
05/11/09   ps      Feature to dynamically allocate OC, CC and TOC ptr 
05/10/09   kp      Added support for demand paging RRC code using pragma.
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
04/09/09   ps      Made changes to send reconfig ind in CU/or go to idle
                   if RL failure is triggered while l2 ack for reconfig complete.
04/03/09   kp      Fixed compilation issue for RRC perl UTF under feature flag
                   FEATURE_RRC_UNIT_TEST.
03/25/09   gkg     Featurized code changes for High Mobility Indication. If 
                   FEATURE_WCDMA_REL7_SPECRS is not defined, no need to send
                   High Mobility Indication to L1.
03/24/09   ps      Made changes that once OTA provides RLC size, then it
                   can be changed only through OTA, only in case of R3 OTA
                   RLC size will be calculated from the TF of the Trch
01/19/08   ps      Made changes for ASn1 migration 25.331v7.9  
                   i.e.May 2008 version  (2008-05) 
12/16/08   gkg     To ensure that variable amr_cb_set has always valid values, made
                   changes to make it global variable.
11/28/08   gkg     Made Changes for new MVS-RRC Interface to (de)configure vocoder
                   for AMR Voice Calls.
10/01/08   gkg     Fixed the Klockwork errors.
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
06/13/08   gkg     Made changes so that if RLC PDU size changes for RBs through
                   Transport Channel IEs of OTA messages, those RBs are reconfigured
                   and therefore their RLC entity is notified for new RLC PDU size.
04/17/08   ps      Fixed compiler error when FEATURE_SCUDIF_ENHANCEMENT and 
                   FEATURE_VOC_AMR_WB are enabled 
04/14/08   gkg     Made changes for configuration of AMR Call so that no. of AMR
                   classes in UL to be registered with MVS is 3 whenever no. of RBs in
                   voice RAB is 3. This is done irrespective of amr Rate.
01/08/08   ps/gkg  Made changes for SCUDIF under feature flag 
                   FEATURE_SCUDIF_ENHANCEMENT
11/14/07   rm      Made change to not configure the vocoder when TFCS info is
                   not changed in the OTA message
09/24/07   ps      Fixed compilation error when feature
                   FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING is not defined
09/10/07   ps      Added support for Frequency redirection received in 
                   FACH->FACH and FACH->PCH cases under feature flag
                   FEATURE_RRC_FREQ_REDIRECTION_THRU_RB_AND_CU
05/22/07   sm      Corrected feature definition requirement for function
                   rrcllc_validate_rb_info_reconfig_list_r6()
03/02/06   ps      Added support for Wb-AMR under the feature flag
                   FEATURE_VOC_AMR_WB.
02/07/07   ps      Made changes to handle Standalone Rb-Mappings
02/02/07   da      Removed code that use to handle DCH->FACH activation time
                   and 300ms delay.  functionality has been moved to LLC. 
11/15/06   sm      Added EUL HHO and reverting back to old config support.
                   Added function rrcllc_validate_rb_info_reconfig_list_r6().
09/12/06   da       Fix for lint warnings
09/11/06   da      Added support for FEATURE RLC ONE SIDE RE ESTABLISHMENT
09/05/06   vm      Populated the new field tx_to_discon_reason in the command
                   initiate_cell_sel_ind with a default value. 
                   Also fixed existing lint error.
09/05/06   da      Added function rrcrb_get_new_crnti_valid()
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
08/28/06   da      Added support for OOS area handling during transition from DCH.
03/10/06   da      Added Initial support for PDCP under FEATURE_UMTS_PDCP
03/01/06   tkk     Featurized the "mvssup_wcdma_set_ul_channel" fourth parameter
                   to avoid compilation error for raven.
02/23/06   ss/tkk  Added support for possible AMR rate change though reconfig messages.
12/02/05   da      Added macro GET_ACT_TIME to be used to determine DCH-FACH transition delay.
12/01/05   da      Added new function rrcrb_get_dch_fach_trans_delay()
07/29/05   da      Added 2 new functions rrc_query_mac_rab_status() which will return the 
                   value of mac_rab_status and rrcrb_update_mac_rab_status which will
                   update the value of mac_rab_status.
05/19/05   kc      Added changes to use local function to check whether a voice call is
                   up or not.  Also if updating amr-mapping-info fails, we will not configure
                   AMR information to L1.
05/04/05   da      Added additinal check to see if it's a cs domain rb id in func
                   rrcrb_update_amr_cctrch_id_for_reconfig.  only then do we need to send
                   info to voice services.
10/21/04   da      Removed FEATURE_CELLPCH_URAPCH.  CELLPCH and URAPCH states should always
                   be handled.  Deleted code that handles case when CELLPCH and URAPCH is not
                   defined.
10/19/04   da      Removed FEATURE SRNS RELOCATION.  SRNS_RELOCATION should always be
                   handled.  Deleted code that handles case when SRNS_RELOCATION is not
                   defined.
09/24/04   da      Added a check to see if srb4 exits in rrcrb_register_with_rlc_for_up_data_indication
09/22/04   sk      Do not register/unregister for ul data indication for RBs belonging 
                   to CS domain RAB.
08/30/04   sgk     Included headers "rrcccm.h", "rrclcm.h", "rrcrbe.h", 
                   "rrcrbr.h", "rrcrbreconfig.h", "rrctcreconfig.h" and 
                   "rrcpcreconfig.h" to fix lint errors function call not made
                   in presence of a prototype for functions 
                   'rrc_ccm_get_curr_camped_freq', 'rrc_ccm_get_curr_camped_psc', 
                   'rrclcm_get_ul_rlc_lc_id', 'rrcrbe_get_current_sub_state', 
                   'rrcrbr_get_current_sub_state', 'rrcrbrc_get_current_sub_state',
                   'rrctcrc_get_current_sub_state', 'rrcpcrc_get_current_sub_state'. 
08/20/04   jh(kc)  Added function rrcrb_update_amr_cctrch_id_for_reconfig().
08/05/04   bu      Filled lthe proc id in rrcrb_send_initiate_cell_selection_req().
07/14/04   sk      Added function rrcrb_get_proc_sub_state to get the 
                   current substate.
05/06/04   sk      fixed an issue in directed cell info check function.
05/04/04   sk      Changed rrcrb_send_rnti_update_req() input parms.
03/04/04   ttl     Added FEATURE_CELLPCH_URAPCH for code of Cell_PCH/URA_PCH.
02/04/04   sk      Moved common function from all rb procedures to
                   common file.
                   Added function to send DRX req in common file
11/14/03   kc      Added funtion to update RNTI info in a common file and also added 
                   function to re-establish RB's for SRNS relocation.
09/03/01   rj      Removed include file rrcasnutil.h since it is no longer 
                   required after moving send_rrc_status_message to SEND CHAIN.
05/08/01   ram     The translate function was not working correctly for
                   bit strings less than 24 bits in length. Modified the
                   algorithm to use left shifts as necessary to only
                   copy the used bits in the string.
05/02/01   ram     Updated rrcrb_translate_gsm_map_rab_id to use the union
                   of int and char array to convert the ossbitstring type.
04/06/01   ram     Created file.


===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "err.h"
#include "msg.h"
#include "rrcasn1.h"
#include "rrccmd_v.h"
#include "rrcdata_v.h"
#include "rrcintif.h"
#include "rrcmmif.h"
#include "rrcsmc.h"
#include "rrcccm.h"
#include "rrclcm.h"
#include "rrcrbe.h"
#include "rrcrbr.h"
#include "rrcrbreconfig.h"
#include "rrctcreconfig.h"
#include "rrcpcreconfig.h"
#include "rrcrbcommon.h"
#include "rrcscmgr.h"
#include "rrcllc.h"
#include "rrcumi.h"
#include "rrccu.h"
#include "rrcllcoc.h"

#include "rrcmnif.h"
#include "rrclbt.h"
#include "rrccu.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif



/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
                                                                   
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

extern mac_rab_status_e_type mac_rab_status;
boolean rrcrb_cs_data_call;
rrc_rb_info_for_amr_type amr_rb_info;


#ifdef FEATURE_WCDMA_REL7_SPECRS
/*This variable stores throughout the RRC Conn, whether Chnage of UE Capability
    is supported by Serving RNC or not.*/
boolean rrc_change_of_ue_cap_response = FALSE;
#endif

#ifdef FEATURE_UMTS_PDCP
extern boolean rrc_pdcp_enabled;
#endif /* FEATURE_UMTS_PDCP*/

/* stores if the l2 ack is received by reconfig procedures or not for DCH*/
rrc_proc_e_type rrc_wait_for_l2ack_for_reconfig = MAX_NUM_OF_PROCEDURES;

extern boolean rrcrbe_send_initiate_cu_flag;
extern boolean rrcrbrc_send_initiate_cu_flag;
extern boolean rrcrbr_send_initiate_cu_flag;
extern boolean rrcpcrc_send_initiate_cu_flag;
extern boolean rrctcrc_send_initiate_cu_flag;
/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/*===========================================================================
** -----------------------------------------------------------------------------------
**-- ----------NON-Demand paging section Srart--------------------------------------------
** -----------------------------------------------------------------------------------
  This section will have code section that will not be demand paged. Function which should be in this section are
  -RRC functions that are called by L1/L2 which are higher priority then RRC
  -RRC functions that are called in ISR context or RTOS timer call back
  -RRC functions that indirectly gets called by L1/L2 /ISR/timer call back   
  For example: Fun1() gets called by L1. Fun2() is called by Fun1(). Now both  Fun1() & Fun2() should be NON demand paged, 
  so both should be added in this section  
  Usage of pragma 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void foo(void)
  {
    function body here
  }
        __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
  If you have another function to be added in the same section then again it needs to be wrapped with these pragma 
  for example:- function bar() is just below function foo() then function bar() needs to be written like this …. 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void bar(void)
  {
    function body here
  }
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
** ----------------------------------------------------------------------- */


/*===========================================================================

FUNCTION          RRCRB_QUERY_MAC_STATUS


DESCRIPTION

  This function returns the value of mac_rab_status indicating to MAC what kind
  of RABS are currently in est_rab + reconfig msg.

DEPENDENCIES

  None

RETURN VALUE

  Return value is of rrcrb_mac_rab_status_e_type type

SIDE EFFECTS

  None

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
mac_rab_status_e_type rrc_query_mac_rab_status
(
  void
)
{
  return mac_rab_status;
}
/*===========================================================================

FUNCTION          rrc_query_cs_data_status


DESCRIPTION

  This function returns the value of cs_data_status indicating to L1 what kind
  of RABS are currently in est_rab + reconfig msg.

DEPENDENCIES

  None

RETURN VALUE

  Return value is of rrcrb_mac_rab_status_e_type type

SIDE EFFECTS

  None

===========================================================================*/
boolean  rrc_query_cs_data_status
(
  void
)
{
  return rrcrb_cs_data_call;
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/


/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/*============================================================================
FUNCTION: rrcllc_validate_rb_info_reconfig_list_r6()

DESCRIPTION:
  This function validates the IE rrc_RB_InformationReconfigList_r6 and return
  SUCCESS or FAILURE accordingly
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_validate_rb_info_reconfig_list_r6
(
  rrc_RB_InformationReconfigList_r6 *rrc_rb_information_reconfig_list
)
{
  boolean msg_not_valid = FALSE;
#ifdef FEATURE_UMTS_PDCP
  struct rrc_HeaderCompressionInfo_r4 *pdcpHeaderCompressionInfo = NULL;  
#endif /* FEATURE_UMTS_PDCP*/
  rlc_lc_id_type                          lc_id;
  uecomdef_logch_mode_e_type              lc_mode;  
  uint32 rb_idx=0;
  rrc_RB_InformationReconfig_r6 * rb_info_reconfig_ptr;
  	
  do
  { 
   rb_info_reconfig_ptr = &rrc_rb_information_reconfig_list->elem[rb_idx];
    /* Check whether rb id in rb reconfig list is in the established rabs list.  If not set msg_not_valid variable
       to true to resend cell update msg with failure cause  */
    if (RRC_RB_NOT_PRESENT == validate_rb_id_in_reconfig_list(
        rb_info_reconfig_ptr->rb_Identity))
    {
      /* RB Message was invalid. */
      WRRC_MSG1_HIGH("Invalid RB id:%d in rb_InformationReconfigList",
        rb_info_reconfig_ptr->rb_Identity );
      msg_not_valid = TRUE;
      break;
    }

    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr, 
        rrc_RB_InformationReconfig_r6,rb_StopContinue))
    {
      if ((uint16)(rb_info_reconfig_ptr->rb_Identity) <= 2)
      {
        /* not sure why this check is present */
        WRRC_MSG1_ERROR("indicating stop/continue for RB %d <= 2",
          (uint16)(rb_info_reconfig_ptr->rb_Identity));
        msg_not_valid = TRUE;
        break;
      }
      
      lc_id = rrclcm_get_ul_rlc_lc_id(
              rb_info_reconfig_ptr->rb_Identity);
      
      if (lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND)
      {
        msg_not_valid = TRUE;
        WRRC_MSG1_ERROR("No corresponding LC id for the RB %d",
          (uint16)(rb_info_reconfig_ptr->rb_Identity));
        break;
      }
      else if (rrclcm_get_ul_rlc_mode(lc_id, &lc_mode) == RRCLCM_FAILURE)
      {
        msg_not_valid = TRUE;
        WRRC_MSG1_ERROR("Invalid lc_mode for RB %d",
          rb_info_reconfig_ptr->rb_Identity);
        break;
      }
      else if (lc_mode == UE_MODE_TRANSPARENT)
      {
        msg_not_valid = TRUE;
        WRRC_MSG2_ERROR("Transparent mode RB %d lc_id %d can't be stopped/contd",
          (uint16)(rb_info_reconfig_ptr->rb_Identity),lc_id);
        break;
      }
    }

#ifdef FEATURE_UMTS_PDCP
    if (rrc_pdcp_enabled)  
    { 
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,
          rrc_RB_InformationReconfig_r6,pdcp_SN_Info)) 
      {
        msg_not_valid = TRUE;
        WRRC_MSG0_HIGH("Invalid: PDCP SN info not supported CU CNF");
        break;
      }
      
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,
          rrc_RB_InformationReconfig_r6,pdcp_Info))
      {
        /* Validate PDCP info */
        if (rrc_get_rab_type_for_given_rb_id(
            rb_info_reconfig_ptr->rb_Identity) != RRCRB_PS_DATA_RAB)
        {
          WRRC_MSG1_HIGH("NW incld PDCP info for non-ps rb id %d: Invalid",
                   rb_info_reconfig_ptr->rb_Identity);
          msg_not_valid = TRUE;
          break;
        }
        
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_info_reconfig_ptr,
            rrc_RB_InformationReconfig_r6,pdcp_Info))
        {
          /* check whether lossless SRNS relocation flag is set or   
           * and header compression algo is present in the message 
           */
          if ((RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_ptr->pdcp_Info.pdcp_Info,
               rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) && 
              (RRC_CHECK_COMMON_MSG_TYPE(rb_info_reconfig_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport,
               rrc_LosslessSRNS_RelocSupport_supported)))
          {
            if (rb_info_reconfig_ptr->pdcp_Info.
                pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_absent)
            {
              WRRC_MSG0_HIGH("PDCP pdu header is absent.  Invalid config");
              msg_not_valid = TRUE;
              break;
            }
         
          } /* if rrc_PDCP_Info_r4_losslessSRNS_RelocSupport_present */
        }

        if (rb_info_reconfig_ptr->pdcp_Info.pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) 
        {
          if ((RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_ptr->pdcp_Info.pdcp_Info,
               rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) && 
              (RRC_CHECK_COMMON_MSG_TYPE(rb_info_reconfig_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport,
               rrc_LosslessSRNS_RelocSupport_notSupported)))
          {
            if (!(RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_ptr->pdcp_Info.pdcp_Info,
				rrc_PDCP_Info_r4,headerCompressionInfoList)) )
            {
              WRRC_MSG0_HIGH("PDCP header compress info list not present");
              msg_not_valid = TRUE;
              break;
            }
          }
        }
        
        if (RRC_MSG_COMMON_BITMASK_IE(rb_info_reconfig_ptr->pdcp_Info.pdcp_Info,
			rrc_PDCP_Info_r4,headerCompressionInfoList)) 
        {
         uint32 local_idx=0;
          while (rb_info_reconfig_ptr->pdcp_Info.pdcp_Info.headerCompressionInfoList.n >local_idx) 
          {
            pdcpHeaderCompressionInfo = &rb_info_reconfig_ptr->pdcp_Info.pdcp_Info.headerCompressionInfoList.elem[local_idx];
            if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.t == 
                T_rrc_AlgorithmSpecificInfo_r4_rfc2507_Info)
            {
              /* if the UE capability "Maximum header compression context space", is exceeded with this configuration send
               * failure with cause "Invalid Config"
               */
              if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER > IPHC_MAX_HC_CONTEXT_SPACE)
              {
                WRRC_MSG1_HIGH("Max header comprss context space exceeded : %d",
                         pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER);
                msg_not_valid = TRUE;
                break;
              }
            }
            else if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.t == 
                     T_rrc_AlgorithmSpecificInfo_r4_rfc3095_Info)
            {
              /* 	the chosen MAX_CID shall not be greater than the value "Maximum number of ROHC context sessions" as 
               *  indicated in the IE "PDCP Capability"
               */
              if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(
			  	pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info,ul_RFC3095))
              {
                if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->
					ul_RFC3095.max_CID > ROHC_MAX_CONTEXT_SESSIONS)
                {
                  WRRC_MSG1_HIGH("Invalid config.  MAX_CID %d is invalid",
                           pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->ul_RFC3095.max_CID);
                  msg_not_valid = TRUE;
                  break;
                }
              }
              
              if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(
			  	pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info,
			  	dl_RFC3095))
              {
                if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->dl_RFC3095.max_CID > ROHC_MAX_CONTEXT_SESSIONS)
                {
                  WRRC_MSG1_HIGH("Invalid config.  MAX_CID %d is invalid",
                           pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->dl_RFC3095.max_CID);
                  msg_not_valid = TRUE;
                  break;
                }
              }

            }/* else if (pdcpHeaderCompressionInfoList->value.algorithmSpecificInfo.t == rrc_rfc3095_Info)*/

            local_idx++;
          } /*  while (pdcpHeaderCompressionInfoList) */
        } /*  if (rb_list_ptr->value.pdcp_Info.pdcp_Info.bit_mask & rrc_PDCP_Info_r4_headerCompressionInfoList_present) */         
      }
    }
#endif /* FEATURE_UMTS_PDCP */

    /* Advance the Linked List pointer */
    rb_idx++;
  } while(rrc_rb_information_reconfig_list->n > rb_idx) ;

  if (TRUE == msg_not_valid)
  {
    return(FAILURE);
  }
  else
  {
    return(SUCCESS);
  }
}


/*===========================================================================

FUNCTION   RRCRB_SEND_RB_PROC_COMPL_IND

DESCRIPTION

  This function is called by all RB procedures and it sends RRC_RB_PROC_COMPL_IND to 
  CELL UPDATE procedure.
  
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None.
 
SIDE EFFECTS

  None.

===========================================================================*/
void rrcrb_send_rb_proc_compl_ind(void)
{
  
  /* Get a RRC internal command buffer */
  rrc_cmd_type *cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command id  */
  cmd_ptr->cmd_hdr.cmd_id = RRC_RB_PROC_COMPL_IND;

  /* No need to indicate the proc ID etc... This can be expanded in future to 
     include the proc id etc... for now, only CU is using this 
   */
  
  /* Put the command in the internal queue */
  rrc_put_int_cmd( cmd_ptr );
  WRRC_MSG0_HIGH("RRC_RB_PROC_COMPL_IND sent to CU");

  return;
} /* end of rrcrb_send_rb_proc_compl_ind */

/*===========================================================================

FUNCTION   RRCRB_GET_NEW_CRNTI_VALID()

DESCRIPTION
 This function returns if reconfig msg has valid CRNTI


        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcrb_get_new_crnti_valid
(
  void
)
{
  return (rrcrbe_get_new_crnti_valid() ||
          rrcrbr_get_new_crnti_valid() ||
          rrcrbrc_get_new_crnti_valid() ||
          rrcpcrc_get_new_crnti_valid() ||
          rrctcrc_get_new_crnti_valid() );
}
/*===========================================================================

FUNCTION   RRCRB_IS_SRNS_IN_PROGRESS()

DESCRIPTION
 This function returns if SRNS is in progress with current reconfig procedure


        
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcrb_is_srns_in_progress
(
  void
)
{
  boolean status;
  status = (rrcrbe_is_srns_in_progress() || rrcrbrc_is_srns_in_progress() ||
          rrcpcrc_is_srns_in_progress() || rrctcrc_is_srns_in_progress() ||
          rrcrbr_is_srns_in_progress() || rrccu_is_srns_in_progress() ||
          rrcumi_is_srns_in_progress()
         );
  if(status == TRUE)
  {
    WRRC_MSG1_HIGH("rrcrb_is_srns_in_progress %d", status);
  }
  return status;
}
/*===========================================================================

FUNCTION   rrcrb_is_cu_required()

DESCRIPTION
 This function returns TRUE if cell update is required after
 the reconfig msg.  Else returns FALSE
      
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE or FALSE

SIDE EFFECTS

  None.

===========================================================================*/
boolean rrcrb_is_cu_required
(
  void
)
{
  boolean status;
  status = (   rrcrbe_send_initiate_cu_flag 
          || rrcrbrc_send_initiate_cu_flag 
          || rrcrbr_send_initiate_cu_flag
          || rrcpcrc_send_initiate_cu_flag
          || rrctcrc_send_initiate_cu_flag );
  
  if(status == TRUE)
  {
    WRRC_MSG1_HIGH("rrcrb_is_cu_required %d", status);
  }
  return status;
}

/*===========================================================================
FUNCTION   RRCRB_UPDATE_MAC_RAB_STATUS

DESCRIPTION

  This function updates variable mac_rab_status. Parameters to this func 
  are proc id, which identifies the calling procedure, cn_domain, used by RBE
  to identify which domain RABS are being setup for, RB_InformationReleaseList,
  used by RBE and CU procedures, and boolean go_back_to_old_status, which
  indicates to update mac_rab_status from scratch by looking into est_rabs only.
  
DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcrb_update_mac_rab_status
(
  rrc_proc_e_type proc_id,
  rrc_cn_domain_identity_e_type cn_domain,
  rrc_RB_InformationReleaseList  *rb_info_rel_list_ptr,
  boolean go_back_to_old_status
)
{
  rrc_established_rabs_type *est_rabs_copy_local_ptr = (rrc_established_rabs_type *)rrc_malloc(sizeof(rrc_established_rabs_type));
  
  uint32 count;  /* Local Counter */
  uint32 count1; /* Local Counter */
  uint32 count2; /* Local Counter */
  rrcrb_cs_data_call = FALSE;
  /* In case of a channel config req fails, need to roll back mac rab status *
   * to old value.  update the variable by going thru established rabs list. */
  if (go_back_to_old_status) 
  {
    mac_rab_status = NO_RABS;
    for (count = 0; count < MAX_RAB_TO_SETUP; count ++) 
    {
      if( (rrc_est_rabs.rabs[count].rab_id != RRC_INVALID_RAB_ID) &&
          (rrc_est_rabs.rabs[count].num_rbs_for_rab > 0))
      {
        if (rrc_est_rabs.rabs[count].cn_domain == RRC_CS_DOMAIN_CN_ID) 
        {
          if (mac_rab_status == NO_RABS) 
          {
            mac_rab_status = CS_RABS;
            if (RRCRB_CS_DATA_RAB == rrc_est_rabs.rabs[count].rab_type)
            {
               rrcrb_cs_data_call = TRUE;
            }
          }
          else if (mac_rab_status == PS_RABS)
          {
            mac_rab_status = CS_PLUS_PS_RABS;
            /* once we have CS+PS, no need to go thru entire est_rabs list */
            break;
          }
        }
        else if (rrc_est_rabs.rabs[count].cn_domain == RRC_PS_DOMAIN_CN_ID) 
        {
          if (mac_rab_status == NO_RABS) 
          {
            mac_rab_status = PS_RABS;
          }
          else if (mac_rab_status == CS_RABS) 
          {
            mac_rab_status = CS_PLUS_PS_RABS;
            /* once we have CS+PS, no need to go thru entire est_rabs list */
            break;
          }
        }
      }    
    } /* for (count = 0; count < MAX_RAB_TO_SETUP; count ++) */
  } /* (go_back_to_old_status)  */ 

  /* If proc is IHO, blindly set mac_rab_status to CS_RABS since IHO can only
   * happen for CS domain 
   */
  else if (proc_id == RRC_PROCEDURE_IHO)
  {
#ifdef FEATURE_LTE_TO_WCDMA
    if (cn_domain == RRC_CS_DOMAIN_CN_ID)
    {
      mac_rab_status = CS_RABS;
      if (TRUE == rrciho_is_cs_data_rab_present())
      {
        rrcrb_cs_data_call = TRUE;
      }	  
    }
    else if (cn_domain == RRC_PS_DOMAIN_CN_ID)
    {
      if(mac_rab_status == NO_RABS)
      {
      mac_rab_status = PS_RABS;
    }
      else if(mac_rab_status == CS_RABS)
      {
        mac_rab_status = CS_PLUS_PS_RABS;
      }
    }
#else
    mac_rab_status = CS_RABS;
    if (TRUE == rrciho_is_cs_data_rab_present())
    {
      rrcrb_cs_data_call = TRUE;
    }
#endif /* FEATURE_LTE_TO_WCDMA */
  }
  else if (proc_id == RRC_PROCEDURE_RBE)
  {
    if (mac_rab_status == NO_RABS)
    {
      if (cn_domain == RRC_CS_DOMAIN_CN_ID)
      {
        mac_rab_status = CS_RABS;
        if (TRUE == rrcrbe_is_cs_data_rab_present())	
        {
           rrcrb_cs_data_call = TRUE;
        }
      }
      else if (cn_domain == RRC_PS_DOMAIN_CN_ID)
      {
        mac_rab_status = PS_RABS;
      }
    }
    else if (mac_rab_status == CS_RABS)
    {
      if (cn_domain == RRC_PS_DOMAIN_CN_ID)
      {
        mac_rab_status = CS_PLUS_PS_RABS;
      }
    }
    else if (mac_rab_status == PS_RABS)
    {
      if (cn_domain == RRC_CS_DOMAIN_CN_ID)
      {
        mac_rab_status = CS_PLUS_PS_RABS;
      }
    }
  } // if (proc_id == RRC_PROCEDURE_RBE
  else if ((rb_info_rel_list_ptr != NULL) && (rb_info_rel_list_ptr->n != 0) && 
           (proc_id == RRC_PROCEDURE_RBR || proc_id == RRC_PROCEDURE_CU))
  {
    uint32 rb_idx=0;
    mac_rab_status = NO_RABS;
    WCDMA_MEMCPY(est_rabs_copy_local_ptr, 
                 sizeof(rrc_established_rabs_type),
                 &rrc_est_rabs, 
                 sizeof(rrc_established_rabs_type));

    do
    {
      for( count1 =0; count1 < MAX_RAB_TO_SETUP; count1++ )
      {
        /* If the RAB is valid and it has atleast one RB
        mapped to it, we should try to find a match for this RB */
        if( (est_rabs_copy_local_ptr->rabs[count1].rab_id != RRC_INVALID_RAB_ID) &&
            (est_rabs_copy_local_ptr->rabs[count1].num_rbs_for_rab > 0)
            )
        {
          /* This RAB is valid. Look at each RB within the RAB */
          for( count2=0; count2<MAX_RB_PER_RAB; count2++ )
          {
            if( est_rabs_copy_local_ptr->rabs[count1].rb_for_rab[count2].rb_id == rb_info_rel_list_ptr->elem[rb_idx])
            {
              /* Match is found. Remove this RB and decrement
              the number of RBs for this RAB */
              MSG_HIGH("MAC_RAB_STATUS: RB removed rbid:%d, rabid:%d, domain:%d",
                   rb_info_rel_list_ptr->elem[rb_idx], //rbr.rbs_tobe_released.rb_id[count],
                   est_rabs_copy_local_ptr->rabs[count1].rab_id,
                   est_rabs_copy_local_ptr->rabs[count1].cn_domain
                 );
              est_rabs_copy_local_ptr->rabs[count1].rb_for_rab[count2].rb_id = RRC_INVALID_RB_ID;
              est_rabs_copy_local_ptr->rabs[count1].num_rbs_for_rab--;
              if( est_rabs_copy_local_ptr->rabs[count1].num_rbs_for_rab == 0 )
              {
                est_rabs_copy_local_ptr->rabs[count1].rab_id = RRC_INVALID_RAB_ID;
              }
            }
          }
        }
      } 
      rb_idx++;
    } while (rb_info_rel_list_ptr->n > rb_idx);
    /* Now travers thru est_rabs_copy to assign the right value to mac_rab_status */

    for (count=0; count < MAX_RAB_TO_SETUP; count++)
    {
      if(est_rabs_copy_local_ptr->rabs[count].rab_id != RRC_INVALID_RAB_ID)
      {
        if (mac_rab_status == NO_RABS)
        {
          if (est_rabs_copy_local_ptr->rabs[count].cn_domain == RRC_CS_DOMAIN_CN_ID )
          {
            mac_rab_status = CS_RABS;
            if (RRCRB_CS_DATA_RAB == est_rabs_copy_local_ptr->rabs[count].rab_type)
            {
              rrcrb_cs_data_call = TRUE;
            }
          }
          else if (est_rabs_copy_local_ptr->rabs[count].cn_domain == RRC_PS_DOMAIN_CN_ID)
          {
            mac_rab_status = PS_RABS;
          }
        }
        else if (mac_rab_status == CS_RABS)
        {
          if (est_rabs_copy_local_ptr->rabs[count].cn_domain == RRC_PS_DOMAIN_CN_ID)
          {
            mac_rab_status = CS_PLUS_PS_RABS;
            break;
          }
        }
        else if (mac_rab_status == PS_RABS)
        {
          if (est_rabs_copy_local_ptr->rabs[count].cn_domain == RRC_CS_DOMAIN_CN_ID)
          {
            mac_rab_status = CS_PLUS_PS_RABS;
            break;
          }
        }
      }
    }
  } /* else if RBR */
  WRRC_MSG1_HIGH("MAC_RAB_STATUS %d", mac_rab_status);
  rrc_free(est_rabs_copy_local_ptr);
}
/*===========================================================================

FUNCTION          RRCRB_TRANSLATE_GSM_MAP_RAB_ID


DESCRIPTION       

  This function translates a GSM MAP RAB Id type defined by ASN.1 into
  the internal type used by RRC. 

DEPENDENCIES

  None
  
RETURN VALUE
                  
  The GSM MAP RAB Id converted to the internal type rrc_rab_id_type

SIDE EFFECTS
                  
  None

===========================================================================*/
rrc_rab_id_type rrcrb_translate_gsm_map_rab_id
(
  ASN1BitStr32  *ext_rab_id         /* Pointer to external RAB ID */
)
{
  uint32 count;                    /* Local counter       */
  uint32 num_bytes;                /* number of full bytes in string */
  uint32 num_shift;                /* number of bits to shift the result.
                                   This is based on the number of signigicant
                                   bits in the string */
  rrc_rab_id_type rab_id=0;        /* To save the RAB id  */

  /* ossBitString represents an item by indicating the
  number of significant bits in the "length" and putting
  the value starting with the MSB 8 bits, in a unsigned
  char array. The length may not be octet alligned. For
  example if the length is 20 bits, value[1] will contain
  8 MSB bits, value[2] will contain the 2nd 8 MSB bits
  and value[3] will have the 2 lsb bits in the ms and 2
  ms bit positions. */

  /* First find the number of bits that don't
  contain any data. Since the length may not be
  octet alligned, there may be some bits that
  don't have any data and need to be shifted out. */

  num_shift = (8 - ((ext_rab_id->numbits)%8));
  if( num_shift == 8 )
  {
    /* Bits are octet alligned, no shifting needed */ 
    num_shift = 0;
    num_bytes = ((ext_rab_id->numbits)/8);
  }
  else
  {
    num_bytes = ((ext_rab_id->numbits)/8) + 1;
  }

  if( num_bytes > 4)
  {
    WRRC_MSG0_ERROR("rab_id longer than 32 bits!");
    return( rab_id );
  }

  /* Now loop through and copy all the given bits.
  NOTE: Little Endian is assumed here. If Big Endian
  is used, we may need to change this. */
  for(count=0; count < num_bytes; count++)
  {
    /* Left shift each byte from the char pointer
    to the uint32 item. */
    rab_id = rab_id << 8;
    rab_id = (rab_id | (ext_rab_id->data[count]));
  }

  /* Finally right shift to remove the bits with no data */
  rab_id = (rab_id >> num_shift);

  return( rab_id );
}

/*===========================================================================

FUNCTION rrcrb_validate_directed_cell_info

DESCRIPTION
  This function validates the directed cell info and returns a boolean
  that indicates whether a cellupdate is required or not. 

DEPENDENCIES
  None.

RETURN VALUE
  Boolean TRUE  : Cell update is required.
          FALSE : Cell update is not required required.

===========================================================================*/
boolean  rrcrb_validate_directed_cell_info
(
  rrcrb_directed_cell_info_type  directed_cell_info
)
{
  rrc_freq_type rrc_freq;
  boolean       directed_cell_match = FALSE;

  if (!(directed_cell_info.freq_info_present || directed_cell_info.pscr_present))
  {
     directed_cell_match = TRUE;
  }

  else if (directed_cell_info.freq_info_present)
  {
    rrc_freq = rrc_ccm_get_curr_camped_freq();
	MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"This F3 is not valid for FACH to DCH transition:Camped freq: %d directed freq: %d Camped_psc: %d, directed_psc: %d",
		rrc_freq, directed_cell_info.uarfcn_dl, rrc_ccm_get_curr_camped_psc(),directed_cell_info.pscr);
    if(rrc_freq == directed_cell_info.uarfcn_dl) 
                  
    {
      /*check whether primary scrambling code is also present */
      if (directed_cell_info.pscr_present)
      {
        if (rrc_ccm_get_curr_camped_psc() == directed_cell_info.pscr)
        {
          /* both scr and freq match no need to do cellupdate */
          directed_cell_match = TRUE;
        }
        else
        {   
          /* both PSC will not match need to do cell update */
          directed_cell_match = FALSE;
        }
      }
      else
      {
        /*no PSC in the message. Since only frequency info is present
        need to do Cell update */
        directed_cell_match = FALSE;
      }
    }
    else
    {
      /* control will not come here as this check is already made in set_ordered_config
      and it returns failure if the frequencies are different. */
      directed_cell_match = FALSE;
    }
  } /* if frequency info is present */
  else
  {
    /* frequency info is not present. Treat the current frequency as the destination
    frequency. check for the scrambling code */
    /*check whether primary scrambling code is also present */
    if (directed_cell_info.pscr_present)
    {
      WRRC_MSG2_HIGH(" Current_psc: %d, directed_psc: %d", rrc_ccm_get_curr_camped_psc(),directed_cell_info.pscr);
      if (rrc_ccm_get_curr_camped_psc() == directed_cell_info.pscr)
      {
        /* both scr  match no need to do cellupdate */
        directed_cell_match = TRUE;
      }
      else
      {
        /* both scr and freq match no need to do cellupdate */
        directed_cell_match = FALSE;
      }
    } /* if PSC is present */

  } /* no frequency info*/
  return directed_cell_match;
}  /* rrcrb_validate_directed_cell_info */

/*===========================================================================

FUNCTION rrcrb_send_rnti_update_req

DESCRIPTION
  This function sends the RNTI Update request to LLC to update
  with RNTIs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void  rrcrb_send_rnti_update_req
(
  rrcrb_rnti_update_info_type  rnti_update_info
)
{

  /* Allocates the buffer to RRC Internal command */
  rrc_cmd_type  *cmd_ptr = rrc_get_int_cmd_buf();

  cmd_ptr->cmd_hdr.cmd_id = RRC_RNTI_UPDATE_REQ;

  /* Fill parameters for RRC_RNTI_UPDATE_REQ command */
  cmd_ptr->cmd.rnti_update_req.procedure = rnti_update_info.procedure;

  /*check whether c-rnti needs tobe updated */
  if (rnti_update_info.crnti_action == RRC_RNTI_UPDATE)
  {
    /* Indicate that C-RNTI needs to be updated */
    cmd_ptr->cmd.rnti_update_req.crnti_action = RRC_RNTI_UPDATE;
    cmd_ptr->cmd.rnti_update_req.crnti = rnti_update_info.crnti;
  }
  else
  {
    /* It could be either no change or
    delete request. Copy the action */
    cmd_ptr->cmd.rnti_update_req.crnti_action = rnti_update_info.crnti_action;
  }

  /*check whether U-rnti needs tobe updated */
  if (rnti_update_info.urnti_action == RRC_RNTI_UPDATE)
  {
    /* Indicate that U-RNTI needs to be updated */
    cmd_ptr->cmd.rnti_update_req.urnti_action = RRC_RNTI_UPDATE;
    cmd_ptr->cmd.rnti_update_req.urnti = rnti_update_info.urnti;
  }

  else
  {
    /* It could be either no change or
    delete request. Copy the action */
    cmd_ptr->cmd.rnti_update_req.urnti_action = rnti_update_info.urnti_action;
  }
  
  cmd_ptr->cmd.rnti_update_req.rlc_re_establish_srb = rnti_update_info.rlc_re_establish_srb;
  cmd_ptr->cmd.rnti_update_req.rlc_re_establish_rab = rnti_update_info.rlc_re_establish_rab;

  /* Confirmation from LLC is not needed */
  cmd_ptr->cmd.rnti_update_req.cnf_required = rnti_update_info.cnf_reqd;
  
  rrc_put_int_cmd(cmd_ptr);       /* sends the command to RRC  LLC. */

  MSG_HIGH("RNTI_UPDATE_REQ CRNTI:%d, URNTI:%d rrc_proc: %d",
            rnti_update_info.crnti_action, 
           rnti_update_info.urnti_action,
           rnti_update_info.procedure);
}  /* rrcrbrc_send_rnti_update_req */

/*===========================================================================

FUNCTION rrcrbrc_send_rlc_re_est_req_to_smc

DESCRIPTION
  This function sends the rlc_reestablishment request to 
  SMC procedure when reconfig message has SRNS relocation info
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcrb_send_rlc_re_est_req_to_smc
(
  rrc_proc_e_type rrc_proc,
  rrc_rlc_re_est_for_srns_rb_e_type rb_type,
  boolean cnf_reqd
)
{

  /* Get the command buffer */
  rrc_cmd_type *int_cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command id  */
  int_cmd_ptr->cmd_hdr.cmd_id = RRC_RE_EST_RLC_FOR_SRNS_REQ;
  int_cmd_ptr->cmd.rrc_re_est_srns_req.rrc_proc = rrc_proc;
  int_cmd_ptr->cmd.rrc_re_est_srns_req.rb_type  = rb_type;
  int_cmd_ptr->cmd.rrc_re_est_srns_req.cnf_reqd = cnf_reqd;
  WRRC_MSG2_HIGH("RLC Reestablish req for %d from proc rrc_proc_e_type_value%d", 
           rb_type, rrc_proc); 
  rrc_put_int_cmd(int_cmd_ptr);             

}/*rrcrbrc_send_rlc_re_est_req_to_smc*/
 /*===========================================================================

FUNCTION          RRCRB_SEND_L1_DRX_REQ

DESCRIPTION       This function is responsible for sending CPHY_DRX_REQ
                  to L1.

DEPENDENCIES
                  

RETURN VALUE
                  Void
SIDE EFFECTS      None.
===========================================================================*/
void rrcrb_send_l1_drx_req(rrc_proc_e_type rrc_proc)
{

  uint32 drx_len;
  /* Send DRX command to L1, no confirm to wait for */
  rrc_cmd_type *cmd_ptr = rrc_get_int_cmd_buf();

  l1_req_cmd * rrc_cmd_l1_req = NULL;
  RRC_GET_POINTER_L1_REQ_VALUE(cmd_ptr->cmd.rrc_llc_req.l1_req,rrc_cmd_l1_req);

  /* Initialize the LLC command header first */
  cmd_ptr->cmd_hdr.cmd_id = RRC_LLC_CMD_REQ;
  cmd_ptr->cmd.rrc_llc_req.procedure = rrc_proc;
  cmd_ptr->cmd.rrc_llc_req.cmd_dest = LAYER1_PHY;

  /* Now fill the L1 cmd header */
  cmd_ptr->cmd.rrc_llc_req.l1_cmd.cmd_id = CPHY_DRX_REQ;
  cmd_ptr->cmd.rrc_llc_req.l1_cmd.act_time_type = L1_ACTIVATION_TIME_NONE;

  /* Populate the cmd data */
  drx_len = rrcllc_get_drx_info();

  rrc_cmd_l1_req->drx.drx_cycle_len =  rrc_csp_set_drx_length(drx_len);
  
  /* Send the Internal RRC command - LLC should directly handle it */
  rrc_put_int_cmd(cmd_ptr);
  WRRC_MSG2_HIGH("DRX_REQ to L1 cycle:%d proc:rrc_proc_e_type_value%d",
           rrc_cmd_l1_req->drx.drx_cycle_len, rrc_proc);

} /*rrrb_send_l1_drx_req */

 /*===========================================================================

FUNCTION          rrcrb_send_upd_ota_parms_ind

DESCRIPTION       This function is responsible for sending CPHY_UPD_OTA_PARMS_IND
                  to L1.

DEPENDENCIES
                  

RETURN VALUE
                  Void
SIDE EFFECTS      None.
===========================================================================*/
void rrcrb_send_upd_ota_parms_ind(rrc_proc_e_type rrc_proc, uint8 bit_mask_ind)
{
#ifdef FEATURE_WCDMA_REL7_SPECRS   
  rrc_cmd_type *cmd_ptr = rrc_get_int_cmd_buf();

  l1_req_cmd * rrc_cmd_l1_req = NULL;
  RRC_GET_POINTER_L1_REQ_VALUE(cmd_ptr->cmd.rrc_llc_req.l1_req,rrc_cmd_l1_req);


  /* Initialize the LLC command header first */
  cmd_ptr->cmd_hdr.cmd_id = RRC_LLC_CMD_REQ;
  cmd_ptr->cmd.rrc_llc_req.procedure = rrc_proc;
  cmd_ptr->cmd.rrc_llc_req.cmd_dest = LAYER1_PHY;

  /* Now fill the L1 cmd header */
  cmd_ptr->cmd.rrc_llc_req.l1_cmd.cmd_id = CPHY_UPD_OTA_PARMS_IND;
  cmd_ptr->cmd.rrc_llc_req.l1_cmd.act_time_type = L1_ACTIVATION_TIME_NONE;

  /* Populate the cmd data */
  rrc_cmd_l1_req->upd_ota_parms.bit_mask = bit_mask_ind;
  rrc_cmd_l1_req->upd_ota_parms.high_mobility_ind = TRUE;
  
  /* Send the Internal RRC command - LLC should directly handle it */
  rrc_put_int_cmd(cmd_ptr);
  WRRC_MSG2_HIGH("CPHY_UPD_OTA_PARMS_IND to L1. Bit mask:%d proc:rrc_proc_e_type_value%d",
           rrc_cmd_l1_req->upd_ota_parms.bit_mask, rrc_proc);

#else /*FEATURE_WCDMA_REL7_SPECRS*/
  WRRC_MSG0_ERROR("High Mobility Ind to L1 not supported.");
#endif /*FEATURE_WCDMA_REL7_SPECRS*/
}

/*===========================================================================

FUNCTION rrcrb_send_cell_update_initiate_req

DESCRIPTION
  This function sends the cell update initiate request 

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcrb_send_cell_update_initiate_req
(
rrc_proc_e_type              procedure,
rrc_cell_update_cause_e_type cause,
boolean                      cnf_reqd
)
{
  rrc_cmd_type *int_cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command parameters for RRC_CELL_UPDATE_INITIATE_REQ */
  int_cmd_ptr->cmd_hdr.cmd_id                                    = RRC_CELL_UPDATE_INITIATE_REQ;
  int_cmd_ptr->cmd.initiate_cell_update.cause                    = cause;
  int_cmd_ptr->cmd.initiate_cell_update.cu_complete_cnf_required = cnf_reqd;
  int_cmd_ptr->cmd.initiate_cell_update.procedure                = procedure;

  MSG_HIGH("CELL_UPDATE_INITIATE Proc:%d, Cause:%d, cnf_reqd:%d", 
            procedure, cause, cnf_reqd);
  rrc_put_int_cmd(int_cmd_ptr);             

}/*rrcrb_send_cell_update_initiate_req*/


/*===========================================================================

FUNCTION rrcrbrc_send_ura_update_initiate_req

DESCRIPTION
  This function sends the ura update initiate request 

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcrb_send_ura_update_initiate_req
(
rrc_proc_e_type              procedure,
rrc_ura_update_cause_e_type  cause,
boolean                      cnf_reqd
)
{
  rrc_cmd_type *int_cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command parameters for RRC_CELL_UPDATE_INITIATE_REQ */
  int_cmd_ptr->cmd_hdr.cmd_id                                   = RRC_URA_UPDATE_INITIATE_REQ;
  int_cmd_ptr->cmd.initiate_ura_update.cause                    = cause;
  int_cmd_ptr->cmd.initiate_ura_update.uu_complete_cnf_required = cnf_reqd;
  int_cmd_ptr->cmd.initiate_ura_update.procedure                = procedure;
  MSG_HIGH("URA_UPDATE_INITIATE Proc:rrc_proc_e_type_value%d, Cause:%d, cnf_reqd:%d", 
            procedure, cause, cnf_reqd);
  rrc_put_int_cmd(int_cmd_ptr);             

}/*rrcrb_send_ura_update_initiate_req*/

/*===========================================================================

FUNCTION rrcrb_send_initiate_cell_selection_req

DESCRIPTION
  This function send the cell selection initiate request.
DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcrb_send_initiate_cell_selection_req
(
   rrc_proc_e_type              procedure,
   rrc_cell_sel_cause_e_type    cause,
   rrc_state_e_type             dest_state
)
{
  rrc_cmd_type *int_cmd_ptr = rrc_get_int_cmd_buf();

  /* Fill in the command parameters for RRC_INITIATE_CELL_SELECTION */
  int_cmd_ptr->cmd_hdr.cmd_id = RRC_INITIATE_CELL_SELECTION_IND;
  int_cmd_ptr->cmd.initiate_cell_sel_ind.cause = cause;
  int_cmd_ptr->cmd.initiate_cell_sel_ind.proc  = procedure;
  int_cmd_ptr->cmd.initiate_cell_sel_ind.dest_state = dest_state;

  if(rrcllc_return_freq_for_fach_pch(&int_cmd_ptr->cmd.initiate_cell_sel_ind.dest_freq)
     == SUCCESS)
  {
    int_cmd_ptr->cmd.initiate_cell_sel_ind.dest_freq_present = TRUE;
  }
  else
  {
    int_cmd_ptr->cmd.initiate_cell_sel_ind.dest_freq_present = FALSE;
  }

  if(rrcllc_return_psc_for_fach_pch(&int_cmd_ptr->cmd.initiate_cell_sel_ind.dest_psc)
     == SUCCESS)
  {
    int_cmd_ptr->cmd.initiate_cell_sel_ind.dest_psc_present = TRUE;
  }
  else
  {
    int_cmd_ptr->cmd.initiate_cell_sel_ind.dest_psc_present = FALSE;
  }

  int_cmd_ptr->cmd.initiate_cell_sel_ind.tx_to_discon_reason = RRC_TX_TO_DISCON_OTHER;


  MSG_HIGH("INITIATE_CELL_SELECTION Proc:rrc_proc_e_type_value%d, Cause:%d, Dest_st:%d", 
            procedure, cause, dest_state);
  rrc_put_int_cmd(int_cmd_ptr); 

}/*rrcrb_send_initiate_cell_selection_req*/

/*===========================================================================

FUNCTION   RRCRB_REGISTER_WITH_RLC_FOR_UL_DATA_IND

DESCRIPTION

  This function sends a command to RLC to register for uplink data indication
  when UE moves to CELL_PCH/URA_PCH state. It always indicates that registration
  is required for signalling RBs. To include user plane RBs it looks 
  at established_rabs variable 
        
DEPENDENCIES

  None.
 
RETURN VALUE

  None.

SIDE EFFECTS

  None.

===========================================================================*/
void rrcrb_register_with_rlc_for_ul_data_ind
(
  rrc_proc_e_type              procedure
)
{
  rrc_cmd_type    *llc_cmd_ptr = rrc_get_int_cmd_buf();
  uint32           count      = 0;
  uint32           rab_count  = 0;
  uint32           rb_count   = 0;
  rlc_lc_id_type ul_rlc_lc_id = RRCLCM_RLC_LC_ID_NOT_FOUND;
  /*send CRLC DATA IND REQ to RLC via rrcllc */

  l2_ul_req_cmd * rrc_cmd_l2_req = NULL;
  RRC_GET_POINTER_L2_REQ_VALUE(llc_cmd_ptr->cmd.rrc_llc_req.l2_ul_req,rrc_cmd_l2_req);

  /* Initialize the LLC command header first */
  llc_cmd_ptr->cmd_hdr.cmd_id= RRC_LLC_CMD_REQ;
  llc_cmd_ptr->cmd.rrc_llc_req.procedure = procedure;
  llc_cmd_ptr->cmd.rrc_llc_req.cmd_dest = LAYER2_RLC_UL;

  /* Now fill the L2 cmd header */
   llc_cmd_ptr->cmd.rrc_llc_req.l2_ul_cmd.cmd_id = CRLC_UL_REG_FOR_DATA_IND_REQ;
   rrc_cmd_l2_req->ul_reg_data_ind.block_status_timer=TRUE;

   /*Include the lc_ids for signalling RBs with RLC AM mode. Eventhough RRC can monitor
   the UL data in the uplink on signalling RBs this is required to suspend the periodic status
   timer at RLC */
   rrc_cmd_l2_req->ul_reg_data_ind.lc_id[count++] =
     rrclcm_get_ul_rlc_lc_id(DCCH_AM_RADIO_BEARER_ID);
   rrc_cmd_l2_req->ul_reg_data_ind.lc_id[count++] =
     rrclcm_get_ul_rlc_lc_id(DCCH_DT_HIGH_PRI_RADIO_BEARER_ID); 
   ul_rlc_lc_id = rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH, 
                               DCCH_DT_LOW_PRI_RADIO_BEARER_ID,
                               UE_MODE_ACKNOWLEDGED_DATA);
   
   if(ul_rlc_lc_id != RRCLCM_RLC_LC_ID_NOT_FOUND)
   {
     rrc_cmd_l2_req->ul_reg_data_ind.lc_id[count++] =
       rrclcm_get_ul_rlc_lc_id(DCCH_DT_LOW_PRI_RADIO_BEARER_ID);
   }

   /* get the lc_ids for user plane RBs */
   for (rab_count=0; rab_count<MAX_RAB_TO_SETUP; rab_count++)
   {
     if ((rrc_est_rabs.rabs[rab_count].cn_domain == RRC_CS_DOMAIN_CN_ID) 
         &&
         (rrc_est_rabs.rabs[rab_count].num_rbs_for_rab > 0)
        )
     {
       WRRC_MSG2_ERROR("No reg for CS domain,rabid:%d,rbs:%d",
            rrc_est_rabs.rabs[rab_count].rab_id,
            rrc_est_rabs.rabs[rab_count].num_rbs_for_rab
            );

     }
     else if ((rrc_est_rabs.rabs[rab_count].cn_domain == RRC_PS_DOMAIN_CN_ID)
             &&
             (rrc_est_rabs.rabs[rab_count].num_rbs_for_rab > 0)
             )
     {
       /*it is a valid established rab, now compare each rb-id in this established rab with 
       the rb-id received in the message */
       for (rb_count=0; rb_count<rrc_est_rabs.rabs[rab_count].num_rbs_for_rab; rb_count++)
       {
         rrc_cmd_l2_req->ul_reg_data_ind.lc_id[count++] =
            rrclcm_get_ul_rlc_lc_id(rrc_est_rabs.rabs[rab_count].rb_for_rab[rb_count].rb_id);
       }
     } /*end of if*/
   }/* rab_count loop */
   rrc_cmd_l2_req->ul_reg_data_ind.num_lc = (uint16)count;
   WRRC_MSG2_HIGH("registering with RLC for %d RBs Proc:rrc_proc_e_type_value%d", count, procedure);
   rrc_put_int_cmd(llc_cmd_ptr); 

}    /* rrcrb_register_with_rlc_for_ul_data_ind */

/*===========================================================================

FUNCTION rrcrb_get_proc_sub_state

DESCRIPTION
  This function returns the respective procedure substate.
  
DEPENDENCIES
  None.

RETURN VALUE
  rrcrb_substate_e_type proc_sub_state.
===========================================================================*/
rrcrb_substate_e_type rrcrb_get_proc_sub_state
(
  rrc_proc_e_type proc_id
)
{
     rrcrb_substate_e_type proc_sub_st = RRCRB_MAX_SUBSTATES;
     switch (proc_id)
     {
       case  RRC_PROCEDURE_RBE:
         proc_sub_st = rrcrbe_get_current_sub_state();
         break;
       case  RRC_PROCEDURE_RBR:
         proc_sub_st = rrcrbr_get_current_sub_state();
         break;
       case  RRC_PROCEDURE_RBRC:
         proc_sub_st = rrcrbrc_get_current_sub_state();
         break;
       case  RRC_PROCEDURE_TCR:
         proc_sub_st = rrctcrc_get_current_sub_state();
         break;
       case  RRC_PROCEDURE_PCR:
         proc_sub_st = rrcpcrc_get_current_sub_state();
         break;
       default:
         break;
     }
     return proc_sub_st;
}/*rrcrbrc_send_rlc_re_est_req_to_smc*/

/*===========================================================================
FUNCTION rrcrb_update_amr_cctrch_id_for_reconfig
DESCRIPTION
DEPENDENCIES
  None.
RETURN VALUE
  rrcrb_substate_e_type proc_sub_state.
===========================================================================*/
#ifdef FEATURE_VOC_AMR_WB
void rrcrb_update_amr_cctrch_id_for_reconfig
( 
  mvs_mode_type codec_type 
)
#else/*FEATURE_VOC_AMR_WB*/
void rrcrb_update_amr_cctrch_id_for_reconfig
( 
  void 
)
#endif/*FEATURE_VOC_AMR_WB*/
{
#ifdef FEATURE_MVS
  rrcllc_amr_mapping_info_type amr_info; /* To store AMR mode
                                            info incase a voice RAB has been setup */
  mvs_amr_mode_type previous_amr_mode = MVS_AMR_MODE_MAX;
  mvs_amr_mode_type prev_dl_amr_mode = MVS_AMR_MODE_MAX;
  mvs_amr_mode_type new_dl_amr_mode = MVS_AMR_MODE_MAX;
  boolean dl_mode_changed = FALSE;
  uint32 i, j, voice_rab_idx = 0;
  uint32 num_of_RBs = 0; /*To store no. of RBs in Voice RAB */
#ifdef FEATURE_VOC_AMR_WB
  mvs_amr_mode_type previous_amr_wb_mode= MVS_AMR_MODE_MAX;
#endif /*FEATURE_VOC_AMR_WB*/

  /* Channel config was succesful. Check if any
     RABs were setup, or only Signalling RBs were setup */
  if(!rrcllc_check_if_new_tfcs_present())
  {
     WRRC_MSG0_MED("Skipping Configuring Vocoder as TFC's are not changed");
    return;
  }
  if(TRUE == rrc_is_voice_rab_present())
  {
    WRRC_MSG0_HIGH("AMR-Update for voice RAB");
    /* We need to indicate to the higher layers that
      a voice RAB has been setup. First call LLC to
      get the necessary information to send to voice
      services. The structure amr_info will be filled
      by this function. */
#ifdef FEATURE_VOC_AMR_WB
     
    if( codec_type != rrc_active_codec)
    {
      rrc_set_current_codec_mode(codec_type);
   
      memset(&amr_rb_info, 0, sizeof(rrc_rb_info_for_amr_type));
        /* Initialize RAB type to unknown as Circuit Switched Voice RAB maps to value 0 */
      amr_rb_info.rab_type = RRCRB_UNKNOWN_RAB;
    
      for(i=0; i<MAX_RAB_TO_SETUP; i++)
      {
        if(rrc_est_rabs.rabs[i].rab_type == RRCRB_CS_VOICE_RAB)
        {
          amr_rb_info.rab_type = RRCRB_CS_VOICE_RAB;
          amr_rb_info.num_rbs_for_rab= rrc_est_rabs.rabs[i].num_rbs_for_rab;
   
          for(j=0; j< amr_rb_info.num_rbs_for_rab; j++)
          {
            amr_rb_info.rb_id[j] = rrc_est_rabs.rabs[i].rb_for_rab[j].rb_id;
          }
          break; /* Since a voice RAB has been found; no need to iterate */
        }
      }
        
      rrc_change_codec_configuration(i);
        
      /* This is just to silence lint */
      if(i == MAX_RAB_TO_SETUP)
      {
        return;
      }
      return;  
    }

    rrc_set_current_codec_mode(codec_type);
    if( codec_type == MVS_MODE_AMR_WB)
    {
      amr_info.mode_type = MVS_MODE_AMR_WB;
      if(FAILURE == rrcllc_get_amr_wb_mapping(&(amr_info)))
      {
        WRRC_MSG0_ERROR("No AMR Mapping");
        return;
      } 
    }
    else
    {
      amr_info.mode_type = MVS_MODE_AMR;
#endif /*FEATURE_VOC_AMR_WB*/
      if(FAILURE == rrcllc_get_amr_mapping(&(amr_info)))
      {
        WRRC_MSG0_ERROR("No AMR Mapping");
        return;
      }
#ifdef FEATURE_VOC_AMR_WB
    }
#endif /*FEATURE_VOC_AMR_WB*/
    /* The following piece of code ensures that
       "amr_rb_info" is correctly populated with right
       RB Ids corresponding to voice RAB. This is 
       further useful to calculate DL rate 
       per RB Id. Ofcourse, the effective AMR rate
       is the sum of all of them put together */
    memset(&amr_rb_info, 0, sizeof(rrc_rb_info_for_amr_type));
    /* Initialize RAB type to unknown as Circuit Switched Voice RAB maps to value 0 */
    amr_rb_info.rab_type = RRCRB_UNKNOWN_RAB;

    for(i=0; i<MAX_RAB_TO_SETUP; i++)
    {
      if(rrc_est_rabs.rabs[i].rab_type == RRCRB_CS_VOICE_RAB)
      {
        amr_rb_info.rab_type = RRCRB_CS_VOICE_RAB;
        amr_rb_info.num_rbs_for_rab= rrc_est_rabs.rabs[i].num_rbs_for_rab;
        voice_rab_idx = i;
        for(j=0; j< amr_rb_info.num_rbs_for_rab; j++)
        {
          amr_rb_info.rb_id[j] = rrc_est_rabs.rabs[i].rb_for_rab[j].rb_id;
        }
        break; /* Since a voice RAB has been found; no need to iterate */
      }
    }
    
    /* This is just to silence lint */
    if(i == MAX_RAB_TO_SETUP)
    {
      return;
    }

    num_of_RBs = rrc_est_rabs.rabs[voice_rab_idx].num_rbs_for_rab;

    /*If no. of RBs in Voica RAB is 3. All the AMR Classes need to be registered in UL.*/	
    if(num_of_RBs == 3)
    {
      amr_info.ul_amr_c_id = rrclcm_get_ul_rlc_lc_id((rrc_RB_Identity)rrc_est_rabs.
                                          rabs[voice_rab_idx].rb_for_rab[2].rb_id);
      WRRC_MSG1_HIGH("LC id for AMR Class C = %d",amr_info.ul_amr_c_id);			  
    }

#ifdef FEATURE_VOC_AMR_WB		
        amr_info.mode_type = codec_type;
#endif /*FEATURE_VOC_AMR_WB*/

   /*****************************/
   /* Uplink AMR rate processing logic*/
   /*****************************/
#ifdef FEATURE_VOC_AMR_WB
    if(codec_type == MVS_MODE_AMR_WB)
    {
      previous_amr_wb_mode = ghdi_mvs_get_ul_awb_mode();
    }
    else
#endif /*FEATURE_VOC_AMR_WB*/
    {
      previous_amr_mode = ghdi_mvs_get_ul_amr_mode();
    }
#ifdef FEATURE_VOC_AMR_WB
    if( codec_type == MVS_MODE_AMR)
#endif /*FEATURE_VOC_AMR_WB*/
    {
      /* Verify the new mode is different from the old mode */
      if(amr_info.amr_mode != previous_amr_mode)
      {
        rrc_register_ul_mvs_through_ghdi(&amr_info, num_of_RBs);
      }
    }
#ifdef FEATURE_VOC_AMR_WB
      else if(codec_type == MVS_MODE_AMR_WB)
      {
        if(amr_info.amr_mode != previous_amr_wb_mode)
        {
          WRRC_MSG2_HIGH("WB_AMR config changed previs %d present %d ",previous_amr_wb_mode,amr_info.amr_mode);
          ghdi_mvs_amr_set_awb_mode(amr_info.amr_mode);
          ghdi_mvs_amr_set_scr_mode(amr_info.scr_mode);
        }    
      }
#endif /*FEATURE_VOC_AMR_WB*/


/********************************/
/* Downlink AMR rate processing logic */
/********************************/
#ifdef FEATURE_VOC_AMR_WB
    if(codec_type == MVS_MODE_AMR)
#endif /*FEATURE_VOC_AMR_WB*/
    {
      dl_mode_changed = rrc_get_dl_amr_mode_given_rbs(&prev_dl_amr_mode, &new_dl_amr_mode, &amr_rb_info);    
      WRRC_MSG2_HIGH("DL -> prev mode=%d, new mode=%d", prev_dl_amr_mode, new_dl_amr_mode);
      /* Register/Deregister DL MVS only when there is a rate change that is noticed
           between current config & ordered config information */
      if(dl_mode_changed == TRUE)
      {
        amr_info.dl_amr_mode = new_dl_amr_mode;

        if (new_dl_amr_mode >= MVS_AMR_MODE_1020)
        { 
          if(rrc_est_rabs.rabs[i].num_rbs_for_rab == 3)
          { 
            if(rrc_get_amr_mode_bitrate(prev_dl_amr_mode) == RRC_AMR_MODE_LOW_BITRATE)
            {
              rrc_register_dl_mvs_through_ghdi(&amr_info);
            }
          }
          else
          {
            WRRC_MSG0_ERROR("Invalid TB size or num of RBs");
            return;
          } 
        }
        else
        {
          if(rrc_get_amr_mode_bitrate(prev_dl_amr_mode) == RRC_AMR_MODE_HIGH_BITRATE)
          {
            rrc_register_dl_mvs_through_ghdi(&amr_info);
          }
        }
      }
    }

    
  }
#endif
}

/*====================================================================
FUNCTION       rrcrb_get_mac_d_hfn()

DESCRIPTION    This function extract mac_d_hfn field from mac_d_hfn_intial_value_ptr
               if "mac_d_hfn_intial_value_present" indicates TRUE.

REUSABILITY: R5/R6
DEPENDENCIES   None

RETURN VALUE   value of mac_d_hfn if present else, invalid value

====================================================================*/

uecomdef_status_e_type  rrcrb_get_mac_d_hfn
(
  OSS_UINT32 mac_d_hfn_intial_value_present,
  ASN1BitStr32 *mac_d_hfn_intial_value_ptr,
  uint32 *mac_d_val
)
{

  uint32 mac_d_hfn_l = RRCSMC_INVALID_HFN_VALUE;
  uecomdef_status_e_type ret_val=SUCCESS;
    
  if (mac_d_hfn_intial_value_present)
  {
    if(MAC_D_HFN_LENGTH != mac_d_hfn_intial_value_ptr->numbits) 
    {
      WRRC_MSG1_ERROR("Invalid MAC-D-HFN length: 0x%x",
          mac_d_hfn_intial_value_ptr->numbits);
      ret_val = FAILURE;
    } 
    else 
    {
      uint32 idx = 0;
      mac_d_hfn_l = 0;	  
      for(; idx < MAC_D_HFN_LENGTH / 8; idx++) 
      {
        mac_d_hfn_l |= (uint8)mac_d_hfn_intial_value_ptr->data[idx];
        mac_d_hfn_l <<= 8;
      }
      mac_d_hfn_l = (mac_d_hfn_l >> 8);
    }
  }
  *mac_d_val = mac_d_hfn_l;
  return (ret_val);
}

/*===========================================================================
FUNCTION rrc_fill_rb_id_chan_req_from_rb_list_mapped_to_dch()

DESCRIPTION
  This function fills chan_config_req with RBs in rrc_rb_list_mapped_to_dch 
  if they are not already present in chan_config_req.
 
DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrc_fill_rb_id_chan_req_from_rb_list_mapped_to_dch
(
  uint32 *rb_count,
  rrc_channel_config_req_type *chan_config_req
)
{
  uint8 rb_idx = 0;
#ifndef FEATURE_MAC_I	
  uint8 am_entity_idx = 0;
#endif /* FEATURE_MAC_I */
  
  chan_config_req->num_rb_to_config = (int) *rb_count;
  
  /*Loop thru all the RBs that are present in rrc_rb_list_mapped_to_dch*/
  for(rb_idx = 0;((rb_idx < rrc_rb_list_mapped_to_dch.num_of_RBs)&&(*rb_count < MAX_RB));rb_idx++)
  {
    /*Check whether the RB is already part of chan_config_req*/
    if(MAX_RB == rrcllc_check_if_rbid_present_in_chan_config(chan_config_req,
        rrc_rb_list_mapped_to_dch.rrc_rb_mapped_to_dch[rb_idx]))
    {
      WRRC_MSG1_HIGH("Fill RB %d from rrc_rb_list_mapped_to_dch", rrc_rb_list_mapped_to_dch.
            rrc_rb_mapped_to_dch[rb_idx]);
      chan_config_req->rb[(*rb_count)].rb_id = rrc_rb_list_mapped_to_dch.rrc_rb_mapped_to_dch[rb_idx];
#ifndef FEATURE_MAC_I	
      /*Reestab_entity has been set while adding rbs to rrc_rb_list_mapped_to_dch*/
      if((am_entity_idx =get_am_index_wo_f3(rrc_rb_list_mapped_to_dch.rrc_rb_mapped_to_dch[rb_idx])) 
          < UE_MAX_AM_ENTITY)
      {
        WRRC_MSG2_HIGH("Re-establish RB-ID %d at AM index %d in OC",rrc_rb_list_mapped_to_dch.
              rrc_rb_mapped_to_dch[rb_idx],am_entity_idx);
        ordered_config_ptr->am_config[am_entity_idx].reestab_entity = RLC_RE_ESTABLISH_DL_UL;
      }
#endif /* FEATURE_MAC_I */
      chan_config_req->rb[(*rb_count)].rb_config = RECONFIG_RB_OTA;
      chan_config_req->rb[(*rb_count)].ul_cfg_mask = chan_config_req->rb[(*rb_count)].
            dl_cfg_mask = MODIFY_RB_CFG;
      (*rb_count)++;		
    }
  }
  chan_config_req->num_rb_to_config = (int) *rb_count;
  /*Since we have used all the RBs in rrc_rb_list_mapped_to_dch, we can init it.*/
  rrcllc_init_rb_list_mapped_to_dch();
}
/*===========================================================================
FUNCTION rrc_fill_rb_id_chan_req_from_est_rabs()

DESCRIPTION
  This function fills existing AM PS RBs in chan_config_req if they are 
  already not part of chan_config_req.
 
DEPENDENCIES
  None.

RETURN VALUE
 None

===========================================================================*/
void rrc_fill_rb_id_chan_req_from_est_rabs
(
  uint32 *rb_count,
  rrc_channel_config_req_type *chan_config_req
)
{
  uint32 chan_idx;
  uint8 rab_idx = 0, rb_idx = 0;
  uint8 am_entity_idx = 0;
  uint8 um_ul_entity_idx = UE_MAX_UL_UM_CHANNEL;
  uint8 um_dl_entity_idx = UE_MAX_DL_UM_CHANNEL;

  rrc_state_e_type dest_state;
  ordered_config_type *config_ptr = ordered_config_ptr;

#ifdef FEATURE_MAC_I 
  boolean oc_updated_with_hsrach_params = FALSE;
#endif
  uint8 rb_id;/* RB identity */
  
  chan_config_req->num_rb_to_config = (int) *rb_count;

  /*Get TOC ptr for DCH->FACH, as the RLC params are taken from TOC in this case
                  for DCH->FACH transition */
  if(transition_config.toc_usage == TOC_FOR_DCH_AND_FACH ||
     transition_config.toc_usage == TOC_FOR_FACH_AND_PCH ||
     transition_config.toc_usage == TOC_FOR_FACH_AND_PCH_WITH_DCH_INFO)
  {
    dest_state = RRC_STATE_CELL_FACH;
    config_ptr = transition_config.toc_ptr;


#ifdef FEATURE_WCDMA_HS_RACH
    /* Updating OC with HSRACH params here because function rrcllc_check_edch_flexible_ul_size_configured_est_rab, called below
       uses OC to know the new UL size.
       However HSRACH params in OC should be cleared eventually at the end of this function. 
       Clearing is needed because in DCH to FACH scenario, later after OC and CC swap, Copy CC to OC happens.
       So this causes CC also to have new size which is not correct.
    */
    if(rrchsrach_camped_cell_supports_hsrach() == TRUE)
    {
      rrcllc_update_hs_rach_params(ordered_config_ptr);
      oc_updated_with_hsrach_params = TRUE;
    }
#endif
  }  
  else if((rrc_get_state() == RRC_STATE_CELL_FACH) || (rrc_get_state() == RRC_STATE_CELL_DCH))
  {
    dest_state = RRC_STATE_CELL_DCH;
  }
  else
  {
    WRRC_MSG1_ERROR("didnt expect this function to be called in %d state",rrc_get_state());
    return;
  }

  for(rab_idx = 0; ((rab_idx < MAX_RAB_TO_SETUP) && (*rb_count < MAX_RB)); rab_idx++)
  {
    /*Check for only PS RBs*/
    if((rrc_est_rabs.rabs[rab_idx].cn_domain != RRC_PS_DOMAIN_CN_ID) || 
       (rrc_est_rabs.rabs[rab_idx].num_rbs_for_rab <= 0))
      continue;

    for(rb_idx = 0; (rb_idx < rrc_est_rabs.rabs[rab_idx].num_rbs_for_rab) && (*rb_count < MAX_RB); rb_idx++)
    {

      rb_id = rrc_est_rabs.rabs[rab_idx].rb_for_rab[rb_idx].rb_id;
    
      if((rrcllc_check_if_rlc_mode_is_am(rb_id)) == TRUE)
      {
        chan_idx = rrcllc_check_if_rbid_present_in_chan_config(chan_config_req,rb_id);
       /*Check whether the RB is already part of chan_config_req*/
        if( MAX_RB == chan_idx )
        {
          chan_idx = *rb_count;
          chan_config_req->rb[(*rb_count)].rb_id = rb_id;
          chan_config_req->rb[(*rb_count)].rb_config = RECONFIG_RB_OTA;
          chan_config_req->rb[(*rb_count)].ul_cfg_mask = MODIFY_RB_CFG;
          chan_config_req->rb[(*rb_count)].dl_cfg_mask = MODIFY_RB_CFG;
          (*rb_count)++; 
        }
        if(chan_idx < MAX_RB)
        {
          WRRC_MSG2_HIGH("Check Chan req has rb-id %d action %d",rb_id,chan_config_req->rb[chan_idx].rb_config);
  
          /*Skip re-establishment checks if action in not reconfig*/
          if(chan_config_req->rb[chan_idx].rb_config != RECONFIG_RB && 
             chan_config_req->rb[chan_idx].rb_config != RECONFIG_RB_OTA)
            continue;
  
          /*Change the RLC cfg mask to RECONFIG_RB*/
          WRRC_MSG1_MED("Changing the cfg mask from %d to RECONFIG_RB",chan_config_req->rb[chan_idx].rb_config);
          chan_config_req->rb[chan_idx].rb_config = RECONFIG_RB;
        }
        if((am_entity_idx = get_am_index_in_config_ptr(rb_id,config_ptr)) >= UE_MAX_AM_ENTITY)
        {
          WRRC_MSG1_ERROR("Didnt get AM index for rb-id %d in config_ptr",rb_id);
          break;
        }

        WRRC_MSG2_MED("Restablish entitiy for RB-ID %d is %d [0=DL/UL,1=DL,2=UL,3=None]",rb_id,
                          config_ptr->rlc_am_parms.am_info[am_entity_idx]->reestab_entity);

         /*We need to make sure if re-est entity is UL and there is no uplink size change detected by 
           rrc_fill_rb_id_chan_req_from_est_rabs we will need to reset it to NONE. The reset logic will be 
           tracked by a new CR*/
        if(config_ptr->rlc_am_parms.am_info[am_entity_idx]->reestab_entity != RLC_RE_ESTABLISH_DL_UL)
        {

          /* Downlink Size change Logic*/
          if(config_ptr->rlc_am_parms.am_info[am_entity_idx]->reestab_entity == RLC_RE_ESTABLISH_NONE )
          {
            /*Check for DL size change only if the PDU size received in OTA is INVALID except for dch to fach transition*/
            if ((rrcllc_get_am_dl_rlc_size_ota(rb_id) == RRCLLC_INVALID_RLC_SIZE) &&
                (  (transition_config.toc_usage == TOC_FOR_DCH_AND_FACH)  ||
                   (config_ptr->rlc_am_parms.am_info[am_entity_idx]->rlc_dl_am_config.dl_rlc_size != 
                      rrcllc_get_dl_ded_rlc_size(dest_state,rb_id))
                ))
            {
#ifdef FEATURE_MAC_I
              if (config_ptr->rlc_one_sided_reest[am_entity_idx] ) // here always needs to check with TOC pointer but not OC, since OC is bening cleared already
              {
                config_ptr->am_config[am_entity_idx].reestab_entity = RLC_RE_ESTABLISH_DL;
              }
              else
#endif
              {
                config_ptr->am_config[am_entity_idx].reestab_entity = RLC_RE_ESTABLISH_DL_UL;
              }
              MSG_HIGH("DL RB %d,size changed from %d to %d",rb_id,
               config_ptr->rlc_am_parms.am_info[am_entity_idx]->rlc_dl_am_config.dl_rlc_size,
               rrcllc_get_dl_ded_rlc_size(dest_state,rb_id));
            }
          }
        }                 
      }
      else if ((rrcllc_check_if_rlc_mode_is_um(rb_id)) == TRUE)				
      {
         chan_idx = rrcllc_check_if_rbid_present_in_chan_config(chan_config_req,rb_id);
       /*Check whether the RB is already part of chan_config_req*/      
        if( MAX_RB == chan_idx )
        {
          WRRC_MSG1_HIGH("Fill PS UM RB %d from rrc_est_rabs", rb_id);
          chan_config_req->rb[(*rb_count)].rb_id = rb_id;
          /*Get TOC ptr for DCh->FCAH, as the RLC params are taken from TOC in this case
                  for DCH->FACH transition */
          if(transition_config.toc_usage == TOC_FOR_DCH_AND_FACH)
          {
            if((um_ul_entity_idx = get_um_ul_index(rb_id, transition_config.toc_ptr)) 
              >= UE_MAX_UL_UM_CHANNEL)
            {
              WRRC_MSG1_ERROR("Didnt get UL UM index for rb-id %d in TOC",rb_id);
            }
          }
          else if((rrc_get_state() == RRC_STATE_CELL_FACH) || (rrc_get_state() == RRC_STATE_CELL_DCH))
          {
            if((um_ul_entity_idx = get_ul_um_index(rb_id)) 
                        >= UE_MAX_UL_UM_CHANNEL)
            {
               WRRC_MSG1_ERROR("Didnt get UL UM index for rb-id %d in OC",rb_id);
            }
          }
          else
          {
            WRRC_MSG1_ERROR("didnt expect this function to be called in %d state",rrc_get_state());
          }
					
          if(transition_config.toc_usage == TOC_FOR_DCH_AND_FACH)
          {
            if((um_dl_entity_idx =  get_dl_um_index(rb_id,transition_config.toc_ptr)) 
              >= UE_MAX_UL_UM_CHANNEL)
            {
              WRRC_MSG1_ERROR("Didnt get UM index for rb-id %d in TOC",rb_id);
            }
          }
          else if((rrc_get_state() == RRC_STATE_CELL_FACH) || (rrc_get_state() == RRC_STATE_CELL_DCH))
          {
            if((um_dl_entity_idx = get_dl_um_index(rb_id,ordered_config_ptr)) 
                        >= UE_MAX_DL_UM_CHANNEL)
            {
               WRRC_MSG1_ERROR("Didnt get UM index for rb-id %d in OC",rb_id);
            }
          }
          else
          {
            WRRC_MSG1_ERROR("didnt expect this function to be called in rrc_state_e_type_value%d state",rrc_get_state());
          }

          if((um_ul_entity_idx < UE_MAX_UL_UM_CHANNEL) && (um_dl_entity_idx < UE_MAX_DL_UM_CHANNEL))
          {
              chan_config_req->rb[(*rb_count)].rb_config = RECONFIG_RB_OTA;
              chan_config_req->rb[(*rb_count)].ul_cfg_mask = chan_config_req->rb[(*rb_count)].dl_cfg_mask = MODIFY_RB_CFG;
              (*rb_count)++;		
          }
          else if (um_dl_entity_idx < UE_MAX_DL_UM_CHANNEL)
          {
              chan_config_req->rb[(*rb_count)].rb_config = RECONFIG_RB_OTA;
              chan_config_req->rb[(*rb_count)].dl_cfg_mask = MODIFY_RB_CFG;
              chan_config_req->rb[(*rb_count)].ul_cfg_mask = 0;												
              (*rb_count)++;		
          }
          else if (um_ul_entity_idx < UE_MAX_UL_UM_CHANNEL)
          {
              chan_config_req->rb[(*rb_count)].rb_config = RECONFIG_RB_OTA;
              chan_config_req->rb[(*rb_count)].ul_cfg_mask = MODIFY_RB_CFG;
              chan_config_req->rb[(*rb_count)].dl_cfg_mask = 0;	
              (*rb_count)++;		
          }						
        }
        else
        {
          WRRC_MSG1_HIGH("UM Mode RB ID %d already present in chan_config_req",rb_id);
        }
      }			
    }
  }
  chan_config_req->num_rb_to_config = (int) *rb_count;
  
  if (oc_updated_with_hsrach_params == TRUE)
  {  
    rrcllc_init_mac_hsrach_mapping_info(&ordered_config_ptr->mac_hs_rach_config);
    WRRC_MSG2_HIGH(" Initing ordered_config_ptr %d, intact current_config_ptr %d ",ordered_config_ptr,current_config_ptr);
  }
}

/*===========================================================================

FUNCTION rrc_fill_rb_id_chan_req_from_est_rabs()

DESCRIPTION
   This function fills SRBs in chan_config_req.
 
DEPENDENCIES
  None.

RETURN VALUE
 None

===========================================================================*/
void rrc_fill_srb_in_chan_config_req
(
  uint32 *rb_count,
  rrc_channel_config_req_type *chan_config_req
)
{
  uint8 num_rb = *rb_count ;
  rlc_lc_id_type ul_rlc_lc_id = RRCLCM_RLC_LC_ID_NOT_FOUND;
  
  if(rrcllc_check_if_rbid_present_in_chan_config(chan_config_req,DCCH_UM_RADIO_BEARER_ID) == MAX_RB)
  {
    chan_config_req->rb[num_rb].rb_id = DCCH_UM_RADIO_BEARER_ID;
    chan_config_req->rb[num_rb++].rb_config = RECONFIG_RB;
  }
  if(rrcllc_check_if_rbid_present_in_chan_config(chan_config_req,DCCH_AM_RADIO_BEARER_ID) == MAX_RB)
  {
    chan_config_req->rb[num_rb].rb_id = DCCH_AM_RADIO_BEARER_ID;
    chan_config_req->rb[num_rb++].rb_config = RECONFIG_RB;
  }
  if(rrcllc_check_if_rbid_present_in_chan_config(chan_config_req,DCCH_DT_HIGH_PRI_RADIO_BEARER_ID) == MAX_RB)
  {
    chan_config_req->rb[num_rb].rb_id = DCCH_DT_HIGH_PRI_RADIO_BEARER_ID;
    chan_config_req->rb[num_rb++].rb_config = RECONFIG_RB;
  }
  
  /* check whether RB4 was present or not */
  ul_rlc_lc_id = rrclcm_check_ul_rlc_lc_id(UE_LOGCHAN_DCCH, 
                                           DCCH_DT_LOW_PRI_RADIO_BEARER_ID,
                                           UE_MODE_ACKNOWLEDGED_DATA);
  if(ul_rlc_lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND)
  {
     WRRC_MSG0_HIGH("LC_ID not found for SRB4");
  }
  else if(rrcllc_check_if_rbid_present_in_chan_config(chan_config_req,DCCH_DT_LOW_PRI_RADIO_BEARER_ID) == MAX_RB)
  {
    chan_config_req->rb[num_rb].rb_id = DCCH_DT_LOW_PRI_RADIO_BEARER_ID;
    chan_config_req->rb[num_rb++].rb_config = RECONFIG_RB;
  }
  *rb_count = num_rb;
  chan_config_req->num_rb_to_config = (int) *rb_count;
}

/*===========================================================================

FUNCTION   rrc_fill_rb_id_chan_req_from_standalone_db

DESCRIPTION
  Function to Extract the RB affected list based on Current and Ordered Standalone database

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rrc_fill_rb_id_chan_req_from_standalone_db
(
uint32  *rb_count , 
rrc_channel_config_req_type *chan_config_req
)
{
  uint8 i;
  boolean fill_rb_info;

  chan_config_req->num_rb_to_config = (int) *rb_count;
  for(i=0;((i<MAX_HANGING_RB_MAPPING_INFO) && (*rb_count < MAX_RB));i++)
  {
    if(rrc_current_hanging_rb_mapping[i].in_use == TRUE)
    {
      fill_rb_info = FALSE;
      if(rrc_ordered_hanging_rb_mapping[i].in_use == TRUE)
      {
        /* The Transport Channel got added for the Standalone RBs resulting in
         *  modification of Standalone Database.
         */
        if(FALSE == rrcllc_check_ordered_current_mapping_same(i))
        {
          /* Skipping the case when the RB-ID was already present in the OTA message. */
          if( MAX_RB == rrcllc_check_if_rbid_present_in_chan_config(
                 chan_config_req, rrc_current_hanging_rb_mapping[i].rb_id ))
          {
            fill_rb_info = TRUE;
            WRRC_MSG1_HIGH(" fill rb-id %d in chan req", rrc_current_hanging_rb_mapping[i].rb_id);
          }
        }
      }
      else
      { 
        /* The Transport Channel got added for the Standalone RBs resulting in deleting of
        * of the RB-ID from the Standalone database.
        */
        /* Skipping the case when the RB-ID was already present in the OTA message. */
        if( MAX_RB == rrcllc_check_if_rbid_present_in_chan_config(
                     chan_config_req, rrc_current_hanging_rb_mapping[i].rb_id ))
        {
          fill_rb_info = TRUE;
          WRRC_MSG1_HIGH(" Rb not in ordered, fill rb-id %d in  chan req", rrc_current_hanging_rb_mapping[i].rb_id);
        }
      }
      if( fill_rb_info == TRUE)
      {
        chan_config_req->rb[(*rb_count)].rb_id = rrc_current_hanging_rb_mapping[i].rb_id;
        
        if( (RRCLCM_RLC_LC_ID_NOT_FOUND == rrclcm_get_dl_rlc_lc_id
                (   rrc_current_hanging_rb_mapping[i].rb_id )) &&
                (RRCLCM_RLC_LC_ID_NOT_FOUND == rrclcm_get_ul_rlc_lc_id
                (   rrc_current_hanging_rb_mapping[i].rb_id )))
        {
          chan_config_req->rb[(*rb_count)++].rb_config  = ESTABLISH_RB;
        }
        else
        {
          chan_config_req->rb[(*rb_count)++].rb_config  = RECONFIG_RB;
        }
		WRRC_MSG1_HIGH("Adding RB-ID %d from Standalone Database in Chan req ",rrc_current_hanging_rb_mapping[i].rb_id);
      }
    }
  }
    /* Extracting the RB affected list based o Current and Ordered Standalone database. */
  chan_config_req->num_rb_to_config = (int) *rb_count;
}
/*===========================================================================

FUNCTION rrcrb_check_if_proc_waiting_for_new_cell_ind


DESCRIPTION
 
DEPENDENCIES
  None.

RETURN VALUE
 None

===========================================================================*/

boolean rrcrb_check_if_proc_waiting_for_new_cell_ind
(
  rrc_proc_e_type *                proc_id        
)
{

    
  rrcllc_oc_process_state_e_type  process_state;  

  (void) rrcllc_get_ordered_config_state_and_proc(proc_id, &process_state);
  if (((*proc_id == RRC_PROCEDURE_RBE || *proc_id == RRC_PROCEDURE_RBRC || 
                * proc_id == RRC_PROCEDURE_TCR || *proc_id == RRC_PROCEDURE_PCR ||
                 *proc_id == RRC_PROCEDURE_RBR) &&
      ( rrcrb_get_proc_sub_state(*proc_id) == RRCRB_WAIT_FOR_CELL_SELECTION_CNF ))||
      ((*proc_id == RRC_PROCEDURE_CU) && (rrccu_return_cu_substate() == RRCCU_WAIT_FOR_UE_TO_CAMP_ON)&&
        rrccu_check_if_cell_selection_in_progress() == TRUE)
      )
  {
    WRRC_MSG1_HIGH("Proc waiting for new cell id rrc_proc_e_type_value%d",*proc_id);
    return TRUE;
  }
  return FALSE;
}

#ifdef FEATURE_SCUDIF_ENHANCEMENT
#error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/


/*============================================================================
FUNCTION: rrcllc_validate_rb_info_reconfig_list_r7()

DESCRIPTION:
  This function validates the IE rrc_RB_InformationReconfigList_r7 and return
  SUCCESS or FAILURE accordingly
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_validate_rb_info_reconfig_list_r7
(
  rrc_RB_InformationReconfigList_r7 *rrc_rb_information_reconfig_list
)
{
  boolean msg_not_valid = FALSE;
#ifdef FEATURE_UMTS_PDCP
  rrc_HeaderCompressionInfo_r4 *pdcpHeaderCompressionInfo = NULL;  
#endif /* FEATURE_UMTS_PDCP*/
  rlc_lc_id_type                          lc_id;
  uecomdef_logch_mode_e_type              lc_mode;  
  uint32 rb_idx=0;
  rrc_RB_InformationReconfig_r7 * rb_information_reconfig_ptr;
  do
  { 
   rb_information_reconfig_ptr = &rrc_rb_information_reconfig_list->elem[rb_idx];
    /* Check whether rb id in rb reconfig list is in the established rabs list.  If not set msg_not_valid variable
       to true to resend cell update msg with failure cause  */
    if (RRC_RB_NOT_PRESENT == validate_rb_id_in_reconfig_list(
        rb_information_reconfig_ptr->rb_Identity))
    {
      /* RB Message was invalid. */
      WRRC_MSG1_HIGH("Invalid RB id:%d in rb_InformationReconfigList",
        rb_information_reconfig_ptr->rb_Identity );
      msg_not_valid = TRUE;
      break;
    }

    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_information_reconfig_ptr,
        rrc_RB_InformationReconfig_r7,rb_StopContinue))
    {
      if ((rb_information_reconfig_ptr->rb_Identity) <= 2)
      {
        /* not sure why this check is present */
        WRRC_MSG1_ERROR("indicating stop/continue for RB %d <= 2",
          (uint16)(rb_information_reconfig_ptr->rb_Identity));
        msg_not_valid = TRUE;
        break;
      }
      
      lc_id = rrclcm_get_ul_rlc_lc_id(
              rb_information_reconfig_ptr->rb_Identity);
      
      if (lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND)
      {
        msg_not_valid = TRUE;
        WRRC_MSG1_ERROR("No corresponding LC id for the RB %d",
          (uint16)(rb_information_reconfig_ptr->rb_Identity));
        break;
      }
      else if (rrclcm_get_ul_rlc_mode(lc_id, &lc_mode) == RRCLCM_FAILURE)
      {
        msg_not_valid = TRUE;
        WRRC_MSG1_ERROR("Invalid lc_mode for RB %d",
          rb_information_reconfig_ptr->rb_Identity);
        break;
      }
      else if (lc_mode == UE_MODE_TRANSPARENT)
      {
        msg_not_valid = TRUE;
        WRRC_MSG2_ERROR("Transparent mode RB %d lc_id %d can't be stopped/contd",
          (uint16)(rb_information_reconfig_ptr->rb_Identity),lc_id);
        break;
      }
    }

#ifdef FEATURE_UMTS_PDCP
    if (rrc_pdcp_enabled)  
    { 
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_information_reconfig_ptr,
          rrc_RB_InformationReconfig_r7,pdcp_SN_Info)) 
      {
        msg_not_valid = TRUE;
        WRRC_MSG0_HIGH("Invalid: PDCP SN info not supported CU CNF");
        break;
      }
      
      if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_information_reconfig_ptr,
          rrc_RB_InformationReconfig_r7,pdcp_Info))
      {
        /* Validate PDCP info */
        if (rrc_get_rab_type_for_given_rb_id(
            rb_information_reconfig_ptr->rb_Identity) != RRCRB_PS_DATA_RAB)
        {
          WRRC_MSG1_HIGH("NW incld PDCP info for non-ps rb id %d: Invalid",
                   rb_information_reconfig_ptr->rb_Identity);
          msg_not_valid = TRUE;
          break;
        }
        if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_information_reconfig_ptr,
            rrc_RB_InformationReconfig_r7,pdcp_Info))
        {
          /* check whether lossless SRNS relocation flag is set or   
           * and header compression algo is present in the message 
           */
          if ((RRC_MSG_COMMON_BITMASK_IE(rb_information_reconfig_ptr->pdcp_Info.pdcp_Info,
               rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) && 
              (RRC_CHECK_COMMON_MSG_TYPE(rb_information_reconfig_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport,
               rrc_LosslessSRNS_RelocSupport_supported)))
          {
            if (rb_information_reconfig_ptr->pdcp_Info.
                pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_absent)
            {
              WRRC_MSG0_HIGH("PDCP pdu header is absent.  Invalid config");
              msg_not_valid = TRUE;
              break;
            }
         
          } /* if rrc_PDCP_Info_r4_losslessSRNS_RelocSupport_present */
        }

        if (rb_information_reconfig_ptr->pdcp_Info.pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) 
        {
          if ((RRC_MSG_COMMON_BITMASK_IE(rb_information_reconfig_ptr->pdcp_Info.pdcp_Info,
               rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) && 
              (RRC_CHECK_COMMON_MSG_TYPE(rb_information_reconfig_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport, 
               rrc_LosslessSRNS_RelocSupport_notSupported)))
          {
            if (!(RRC_MSG_COMMON_BITMASK_IE(rb_information_reconfig_ptr->pdcp_Info.pdcp_Info,
				rrc_PDCP_Info_r4,headerCompressionInfoList)) )
            {
              WRRC_MSG0_HIGH("PDCP header compress info list not present");
              msg_not_valid = TRUE;
              break;
            }
          }
        }
        
        if (RRC_MSG_COMMON_BITMASK_IE(rb_information_reconfig_ptr->pdcp_Info.pdcp_Info,
			rrc_PDCP_Info_r4,headerCompressionInfoList)) 
        {
          uint32 local_idx =0;
          while (rb_information_reconfig_ptr->pdcp_Info.pdcp_Info.headerCompressionInfoList.n > local_idx) 
          {
            pdcpHeaderCompressionInfo = &rb_information_reconfig_ptr->pdcp_Info.pdcp_Info.headerCompressionInfoList.elem[local_idx];
            if (RRC_CHECK_COMMON_MSG_TYPE(pdcpHeaderCompressionInfo->algorithmSpecificInfo,
                rrc_AlgorithmSpecificInfo_r4_rfc2507_Info))
            {
              /* if the UE capability "Maximum header compression context space", is exceeded with this configuration send
               * failure with cause "Invalid Config"
               */
              if (
                  (pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER > IPHC_MAX_HC_CONTEXT_SPACE))
              {
                WRRC_MSG1_HIGH("Max header comprss context space exceeded : %d",
                         pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER);
                msg_not_valid = TRUE;
                break;
              }
            }
            else if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.t == 
                     T_rrc_AlgorithmSpecificInfo_r4_rfc3095_Info)
            {
              /* 	the chosen MAX_CID shall not be greater than the value "Maximum number of ROHC context sessions" as 
               *  indicated in the IE "PDCP Capability"
               */
              if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.
			  	rfc3095_Info,ul_RFC3095))
              {
                if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.
					rfc3095_Info->ul_RFC3095.max_CID > ROHC_MAX_CONTEXT_SESSIONS)
                {
                  WRRC_MSG1_HIGH("Invalid config.  MAX_CID %d is invalid",
                           pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->ul_RFC3095.max_CID);
                  msg_not_valid = TRUE;
                  break;
                }
              }
              
              if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info,
			  	dl_RFC3095))
              {
                if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.
					u.rfc3095_Info->dl_RFC3095.max_CID > ROHC_MAX_CONTEXT_SESSIONS)
                {
                  WRRC_MSG1_HIGH("Invalid config.  MAX_CID %d is invalid",
                           pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->dl_RFC3095.max_CID);
                  msg_not_valid = TRUE;
                  break;
                }
              }

            }/* else if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.t == rrc_rfc3095_Info)*/

            local_idx++;
          } /*  while (pdcpHeaderCompressionInfoList) */
        } /*  if (rb_list_ptr->value.pdcp_Info.pdcp_Info.bit_mask & rrc_PDCP_Info_r4_headerCompressionInfoList_present) */         
      }
    }
#endif /* FEATURE_UMTS_PDCP */

    /* Advance the Linked List pointer */
    rb_idx++;
  } while(rrc_rb_information_reconfig_list->n > rb_idx) ;

  if (TRUE == msg_not_valid)
  {
    return(FAILURE);
  }
  else
  {
    return(SUCCESS);
  }
}

#ifdef FEATURE_VOC_AMR_WB
/*===========================================================================

FUNCTION   rrcrbrc_update_nas_sync_ind_r7

DESCRIPTION Updates the NAS Syncronisation Indicator field received in the RB Reconfig message.
            In case the NAS Syncronisation Indicator IE is not present , then a default value of 
            current active  codec is used.

DEPENDENCIES
  None.

RETURN VALUE
 None

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL uecomdef_status_e_type rrc_update_nas_sync_ind_r7
(
   uint32 rab_index,
   rrcrb_rab_info_per_msg_type   * rabs_info,
   rrc_NAS_Synchronisation_Indicator *nas_Synchronisation_Indicator 
)
{
  rabs_info->rabs_in_msg[rab_index].nas_Synchronisation_Indicator = 
  rrc_translate_nas_sync_indicator(nas_Synchronisation_Indicator); 
   
  if( rabs_info->rabs_in_msg[rab_index].nas_Synchronisation_Indicator == MVS_MODE_NONE)
  {

    WRRC_MSG1_HIGH("Unsupported  configuration .. codec not supported %d",rabs_info->rabs_in_msg[rab_index].nas_Synchronisation_Indicator );
    return FAILURE;
  }
#ifdef WCDMA_RRC_INDICATE_CODEC_TO_CM
  rrc_set_ordered_codec_status(TRUE);
#endif
  rrc_set_ordered_codec_mode(rabs_info->rabs_in_msg[rab_index].nas_Synchronisation_Indicator);
  return SUCCESS;
}
#endif /*FEATURE_VOC_AMR_WB*/



/*===========================================================================

FUNCTION rrc_update_rab_info_reconfig_param

DESCRIPTION


  
DEPENDENCIES


RETURN VALUE
  Success/Failure
 

SIDE EFFECTS

  None

===========================================================================*/
rrcrb_msg_status_e_type 
rrc_update_rab_info_reconfig_param
(
  rrc_RAB_InformationReconfigList *rab_list_ptr,
  rrcrb_rab_info_per_msg_type   * rabs_info
#ifdef FEATURE_VOC_AMR_WB
  ,mvs_mode_type * nas_sync_ind
#endif/*FEATURE_VOC_AMR_WB*/
)
{
  uint32 rab_index=0; 
  rrc_rab_id_type rab_id;           /* To store RAB id in internal RRC format */
  rrcmn_rab_status_T rab_type; 
  rrc_rab_search_e_type rab_status; 
  MSG_LOW("RAB Info Reconfig in R7 Reconfig OTA.", 0, 0, 0);

  for(rab_index = 0; (rab_list_ptr->n > rab_index) && ( rab_index < RRCRB_MAX_RAB_PER_MSG); rab_index++)
  {
    /* Check if the RAB id is GSM_MAP type. We don't support any other type */
    if (rab_list_ptr->elem[rab_index].rab_Identity.t != T_rrc_RAB_Identity_gsm_MAP_RAB_Identity)
    {
      WRRC_MSG1_HIGH("RABid t%d is not GSM_MAP!", rab_list_ptr->elem[rab_index].rab_Identity.t);
      return RRCRB_MSG_UNSUPPORTED_CONFIG;
    }

    /* GSM_MAP RAB Id. Check if it's valid by calling the appropriate NAS entity. 
     First convert it to internal format. */
    rab_id = rrcrb_translate_gsm_map_rab_id((rab_list_ptr->elem[rab_index].rab_Identity.u.gsm_MAP_RAB_Identity));

    /* Check if the RAB id is already in ESTABLISHED_RABS */
    rab_status = rrc_find_rab_in_established_rabs(rab_id);
    if ( rab_status != RRC_RAB_FOUND )
    {
      WRRC_MSG1_HIGH("RAB ID 0X%x not in ESTABLISHED_RABS", rab_id);
      return RRCRB_MSG_INVALID_CONFIG;
    }

    /* Store this info locally so that ESTABLISHED_RABS can
     be updated later, when these RABs are established */
    rabs_info->rabs_in_msg[rab_index].rab_id_is_gsm_map = TRUE;
    rabs_info->rabs_in_msg[rab_index].rab_id = rab_id;

    /* Check if this RAB is CS Domain or PS Domain */
    if ( rab_list_ptr->elem[rab_index].cn_DomainIdentity  == rrc_CN_DomainIdentity_cs_domain )
    {
      /* Store the domain identity. This will be later added to ESTABLISHED_RABS */
      rabs_info->rabs_in_msg[rab_index].cn_domain = RRC_CS_DOMAIN_CN_ID;

      /* Check if it is in loopback test */
      if ( rrclbt_is_lb_on() == TRUE )
      {
        /* Set Rab type to Data */
        MSG_LOW("Loopback Test", 0, 0, 0);
        rabs_info->rabs_in_msg[rab_index].rab_type = RRCRB_CS_DATA_RAB;
      }
      else
      {
        /* For CS RABs, Call MN layer to verify this RAB */
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
        rab_type = rrcmn_verify_rab_subs( (radio_access_bearer_T)rab_id ,rrc_get_as_id());
#else
        rab_type = rrcmn_verify_rab( (radio_access_bearer_T)rab_id );
#endif

        WRRC_MSG2_HIGH("For RAB-Id %d, RAB type returned from NAS = %d",rab_id, rab_type);

        switch ( rab_type )
        {
          case RRCMN_CS_DATA_CALL:
            rabs_info->rabs_in_msg[rab_index].rab_type = RRCRB_CS_DATA_RAB;
            break;
  
          case RRCMN_CS_VOICE_CALL:
            rabs_info->rabs_in_msg[rab_index].rab_type = RRCRB_CS_VOICE_RAB;
  
#ifdef FEATURE_SCUDIF_ENHANCEMENT
            #error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/
            break;
  
          default:
            WRRC_MSG0_ERROR("CS RAB validation failed!");
            return RRCRB_MSG_UNSUPPORTED_CONFIG;
        } /* end switch */

#ifdef FEATURE_VOC_AMR_WB
        /*As a general approach we should use/update the NAS Sync Indicator even for CS DATA*/
        if((rabs_info->rabs_in_msg[rab_index].rab_type == RRCRB_CS_VOICE_RAB)
#ifdef FEATURE_SCUDIF_ENHANCEMENT
#error code not present
#endif/*FEATURE_SCUDIF_ENHANCEMENT*/
           )
        {
          if(rrc_update_nas_sync_ind_r7(rab_index, rabs_info, 
              &(rab_list_ptr->elem[rab_index].nas_Synchronisation_Indicator)) == FAILURE)
          {
            return RRCRB_MSG_UNSUPPORTED_CONFIG;    
          }
          *nas_sync_ind = rabs_info->rabs_in_msg[rab_index].nas_Synchronisation_Indicator;
        }
#endif /*FEATURE_VOC_AMR_WB*/
      }
    }
    /* Else check for PS Domain */
    else if ( rab_list_ptr->elem[rab_index].cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain )
    {
      /* Store the domain identity. This will be later added to ESTABLISHED_RABS */
      rabs_info->rabs_in_msg[rab_index].cn_domain = RRC_PS_DOMAIN_CN_ID;
      WRRC_MSG1_HIGH("PS RAB-Id %d to be Reconfig", rab_id);
      rabs_info->rabs_in_msg[rab_index].rab_type = RRCRB_PS_DATA_RAB;
    }
  } // for each RAB info

  rabs_info->num_rabs_in_msg = rab_index;
  return RRCRB_MSG_VALID;
}

/*===========================================================================

FUNCTION rrc_update_reconfig_waiting_for_l2ack_dch

DESCRIPTION
Updates the procedure waiting for L2 Ack for the reconfig complete msg sent  in DCH  state

  
DEPENDENCIES


RETURN VALUE
  None
 

SIDE EFFECTS

  None

===========================================================================*/
void rrc_update_reconfig_waiting_for_l2ack_dch
(
    rrc_proc_e_type proc_id
)
{
  rrc_wait_for_l2ack_for_reconfig = proc_id;
  WRRC_MSG1_MED("Updated wait for proc id rrc_proc_e_type_value%d",proc_id);
}

/*===========================================================================

FUNCTION rrc_check_any_proc_waiting_for_l2_ack

DESCRIPTION
Determines, if any procedure is waiting for the L2 ACk msg for the
reconfig  complete msg sent in DCH

  
DEPENDENCIES


RETURN VALUE
  TRUE:  Some prcocedure is waiting for l2 ack
  else FALSE
 

SIDE EFFECTS

  None

===========================================================================*/
boolean rrc_check_any_proc_waiting_for_l2_ack
(
  void
)
{
  boolean status = FALSE;
  switch(rrc_wait_for_l2ack_for_reconfig)
  {
    case RRC_PROCEDURE_RCE:
    case RRC_PROCEDURE_RBE:
    case RRC_PROCEDURE_RBRC:
    case RRC_PROCEDURE_RBR:
    case  RRC_PROCEDURE_TCR:
    case  RRC_PROCEDURE_PCR:
    case RRC_PROCEDURE_CU:
      status= TRUE;
    break;
    
    default:
      status= FALSE;
    break;
  }
  WRRC_MSG1_HIGH("Proc waiting for l2 ack %d",status);
  return status;
}

/*===========================================================================
FUNCTION rrc_update_rab_info_reconfig_param_r8

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE
  Success/Failure

SIDE EFFECTS
  None
===========================================================================*/
rrcrb_msg_status_e_type rrc_update_rab_info_reconfig_param_r8
(
  rrc_RAB_InformationReconfigList_r8 *rab_list_ptr,
  rrcrb_rab_info_per_msg_type   * rabs_info
#ifdef FEATURE_VOC_AMR_WB
  ,mvs_mode_type * nas_sync_ind
#endif/*FEATURE_VOC_AMR_WB*/
)
{
  uint32 rab_index=0; 
  rrc_rab_id_type rab_id;           /* To store RAB id in internal RRC format */

  for (rab_index = 0; (rab_list_ptr->n > rab_index) && (rab_index < RRCRB_MAX_RAB_PER_MSG); rab_index++)
  {
    /* Check if the RAB id is GSM_MAP type. We don't support any other type */
    if (rab_list_ptr->elem[rab_index].rab_Identity.t != T_rrc_RAB_Identity_gsm_MAP_RAB_Identity)
    {
      WRRC_MSG1_ERROR("REL8: RAB-Id t %d is not GSM_MAP!", rab_list_ptr->elem[rab_index].rab_Identity.t);
      return RRCRB_MSG_UNSUPPORTED_CONFIG;
    }

    /* GSM_MAP RAB Id. Check if it's valid by calling the appropriate NAS entity. First convert it to internal format. */
    rab_id = rrcrb_translate_gsm_map_rab_id((rab_list_ptr->elem[rab_index].rab_Identity.u.gsm_MAP_RAB_Identity));

    /* Check if the RAB id is already in ESTABLISHED_RABS */
    if (rrc_find_rab_in_established_rabs(rab_id) != RRC_RAB_FOUND)
    {
      WRRC_MSG1_ERROR("REL8: RAB-Id 0X%x not in EST_RABS!", rab_id);
      return RRCRB_MSG_INVALID_CONFIG;
    }

    /* Store this info locally so that ESTABLISHED_RABS can be updated later, when these RABs are established */
    rabs_info->rabs_in_msg[rab_index].rab_id_is_gsm_map = TRUE;
    rabs_info->rabs_in_msg[rab_index].rab_id = rab_id;

    /* Check if this RAB is CS Domain or PS Domain */
    if ( rab_list_ptr->elem[rab_index].cn_DomainIdentity  == rrc_CN_DomainIdentity_cs_domain )
    {
      /* Store the domain identity. This will be later added to ESTABLISHED_RABS */
      rabs_info->rabs_in_msg[rab_index].cn_domain = RRC_CS_DOMAIN_CN_ID;

      /* Check if it is in loopback test */
      if(rrclbt_is_lb_on() == TRUE)
      {
        /* Set Rab type to Data */
        MSG_LOW("Loopback Test", 0, 0, 0);
        rabs_info->rabs_in_msg[rab_index].rab_type = RRCRB_CS_DATA_RAB;
      }
      else
      {
        /* For CS RABs, Call MN layer to verify this RAB */
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
        switch (rrcmn_verify_rab_subs( (radio_access_bearer_T)rab_id ,rrc_get_as_id()))
#else
        switch (rrcmn_verify_rab( (radio_access_bearer_T)rab_id ))
#endif
        {
          case RRCMN_CS_DATA_CALL:
            rabs_info->rabs_in_msg[rab_index].rab_type = RRCRB_CS_DATA_RAB;
            break;

          case RRCMN_CS_VOICE_CALL:
            rabs_info->rabs_in_msg[rab_index].rab_type = RRCRB_CS_VOICE_RAB;

#ifdef FEATURE_SCUDIF_ENHANCEMENT
            #error code not present
#endif /*FEATURE_SCUDIF_ENHANCEMENT*/
            break;

          default:
            WRRC_MSG0_ERROR("CS RAB validation failed!");
            return RRCRB_MSG_UNSUPPORTED_CONFIG;
        } /* end switch */

#ifdef FEATURE_VOC_AMR_WB
        /*As a general approach we should use/update the NAS Sync Indicator even for CS DATA*/
        if((rabs_info->rabs_in_msg[rab_index].rab_type == RRCRB_CS_VOICE_RAB)
#ifdef FEATURE_SCUDIF_ENHANCEMENT
#error code not present
#endif/*FEATURE_SCUDIF_ENHANCEMENT*/
           )
        {
          if(rrc_update_nas_sync_ind_r7(rab_index, rabs_info, 
              &(rab_list_ptr->elem[rab_index].nas_Synchronisation_Indicator)) == FAILURE)
          {
            return RRCRB_MSG_UNSUPPORTED_CONFIG;    
          }
          *nas_sync_ind = rabs_info->rabs_in_msg[rab_index].nas_Synchronisation_Indicator;
        }
#endif /*FEATURE_VOC_AMR_WB*/
      }
    }
    /* Else check for PS Domain */
    else if ( rab_list_ptr->elem[rab_index].cn_DomainIdentity == rrc_CN_DomainIdentity_ps_domain )
    {
      /* Store the domain identity. This will be later added to ESTABLISHED_RABS */
      rabs_info->rabs_in_msg[rab_index].cn_domain = RRC_PS_DOMAIN_CN_ID;
      WRRC_MSG1_HIGH("REL8: PS RAB-Id %d to be reconfigured.", rab_id);
      rabs_info->rabs_in_msg[rab_index].rab_type = RRCRB_PS_DATA_RAB;
    }

  } // for each RAB info

  rabs_info->num_rabs_in_msg = rab_index;
  return RRCRB_MSG_VALID;
}

/*============================================================================
FUNCTION: rrcllc_validate_rb_info_reconfig_list_r8()

DESCRIPTION:
  This function validates the IE rrc_RB_InformationReconfigList_r8 and return
  SUCCESS or FAILURE accordingly
      
DEPENDENCIES:
  None.

RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
=============================================================================*/
uecomdef_status_e_type rrcllc_validate_rb_info_reconfig_list_r8
(
  rrc_RB_InformationReconfigList_r8 *rrc_rb_information_reconfig_list
)
{
  boolean msg_not_valid = FALSE;
  rlc_lc_id_type                           lc_id;
  uecomdef_logch_mode_e_type   lc_mode;  
  rrc_RB_InformationReconfig_r8 *rb_information_reconfig_ptr =NULL;
  uint32 rb_index=0;
  
  do
  { 
    rb_information_reconfig_ptr = &rrc_rb_information_reconfig_list->elem[rb_index];
    /* Check whether rb id in rb reconfig list is in the established rabs list.  If not set msg_not_valid variable
       to true to resend cell update msg with failure cause  */
    if (RRC_RB_NOT_PRESENT == validate_rb_id_in_reconfig_list(
        rb_information_reconfig_ptr->rb_Identity))
    {
      /* RB Message was invalid. */
      WRRC_MSG1_HIGH("Invalid RB id:%d in rb_InformationReconfigList",
        rb_information_reconfig_ptr->rb_Identity );
      msg_not_valid = TRUE;
      break;
    }

    if (RRC_MSG_COMMON_BITMASK_IE_PTR(rb_information_reconfig_ptr,
      rrc_RB_InformationReconfig_r8,rb_StopContinue))
    {
      if ((uint16)(rb_information_reconfig_ptr->rb_Identity) <= 2)
      {
        /* not sure why this check is present */
        WRRC_MSG1_ERROR("indicating stop/continue for RB %d <= 2",
          (uint16)(rb_information_reconfig_ptr->rb_Identity));
        msg_not_valid = TRUE;
        break;
      }
      
      lc_id = rrclcm_get_ul_rlc_lc_id(
              rb_information_reconfig_ptr->rb_Identity);
      
      if (lc_id == RRCLCM_RLC_LC_ID_NOT_FOUND)
      {
        msg_not_valid = TRUE;
        WRRC_MSG1_ERROR("No corresponding LC id for the RB %d",
          (uint16)(rb_information_reconfig_ptr->rb_Identity));
        break;
      }
      else if (rrclcm_get_ul_rlc_mode(lc_id, &lc_mode) == RRCLCM_FAILURE)
      {
        msg_not_valid = TRUE;
        WRRC_MSG1_ERROR("Invalid lc_mode for RB %d",
          rb_information_reconfig_ptr->rb_Identity);
        break;
      }
      else if (lc_mode == UE_MODE_TRANSPARENT)
      {
        msg_not_valid = TRUE;
        WRRC_MSG2_ERROR("Transparent mode RB %d lc_id %d can't be stopped/contd",
          (uint16)(rb_information_reconfig_ptr->rb_Identity),lc_id);
        break;
      }
    }

#ifdef FEATURE_UMTS_PDCP
    if (rrc_pdcp_enabled)  
    {
      /*Validate PDCP related info for this RB-Id*/
      if(rrcllc_validate_pdcp_info_in_rb_info_reconfig_list_r8
        (rb_information_reconfig_ptr) == FALSE)
      {
        msg_not_valid = TRUE;
        break;
      }
    }
#endif /* FEATURE_UMTS_PDCP */

    /* Advance the Linked List pointer */
   rb_index++;
  } while(rrc_rb_information_reconfig_list->n > rb_index) ;

  if (TRUE == msg_not_valid)
  {
    return(FAILURE);
  }
  else
  {
    return(SUCCESS);
  }
}

#ifdef FEATURE_UMTS_PDCP
/*============================================================================
FUNCTION: rrcllc_validate_pdcp_info_in_rb_info_reconfig_list_r8()

DESCRIPTION:
  This function validates the IE rrc_RB_InformationReconfig_r8 for PDCP Info return
  TRUE or FALSE accordingly
      
DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE/FALSE

SIDE EFFECTS:
=============================================================================*/
boolean rrcllc_validate_pdcp_info_in_rb_info_reconfig_list_r8
(
  rrc_RB_InformationReconfig_r8 * rb_reconfig_ptr
)
{ 
#ifdef FEATURE_UMTS_PDCP
  rrc_HeaderCompressionInfo_r4 *pdcpHeaderCompressionInfo = NULL;  
#endif /* FEATURE_UMTS_PDCP*/

  if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_reconfig_ptr,
  	rrc_RB_InformationReconfig_r8,pdcp_SN_Info)) 
  {
    WRRC_MSG0_ERROR("REL8: PDCP SN info not supported.");
    return FALSE;
  }

  /*RRC_GKG: Find a reference what type of PDCP validation is done by QCOM.*/
  if(RRC_MSG_COMMON_BITMASK_IE_PTR(rb_reconfig_ptr,
  	rrc_RB_InformationReconfig_r8,pdcp_Info))
  {
    /*RRC_GKG: Later a check will be added for CSoHSPA. Beacuse CS RAB can have PDCP Info.*/
    if(rrc_get_rab_type_for_given_rb_id(rb_reconfig_ptr->rb_Identity) != RRCRB_PS_DATA_RAB)
    {
      WRRC_MSG1_ERROR("REL8: NW incld PDCP info for non-PS rb id %d", rb_reconfig_ptr->rb_Identity);
      return FALSE;
    }
    
    /*Check whether lossless SRNS relocation flag is set and PDCP header is absent in the message */
    if((RRC_MSG_COMMON_BITMASK_IE(rb_reconfig_ptr->pdcp_Info.pdcp_Info,rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) && 
        (RRC_CHECK_COMMON_MSG_TYPE(rb_reconfig_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport,
         rrc_LosslessSRNS_RelocSupport_supported)))
    {
      if(rb_reconfig_ptr->pdcp_Info.pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_absent)
      {
        WRRC_MSG0_ERROR("REL8: PDCP pdu header is absent for Lossless SRNS Relocation");
        return FALSE;
      }
    }

    /*Check whether PDCP header is present and Lossless SRNS relocation flag isn't set and Header compression 
    Lits is absent.*/
    if(rb_reconfig_ptr->pdcp_Info.pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present) 
    {
      if ((RRC_MSG_COMMON_BITMASK_IE(rb_reconfig_ptr->pdcp_Info.pdcp_Info,
	  	rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) && 
        (RRC_CHECK_COMMON_MSG_TYPE(rb_reconfig_ptr->pdcp_Info.pdcp_Info.losslessSRNS_RelocSupport,
         rrc_LosslessSRNS_RelocSupport_notSupported)))
      {
        if(!(RRC_MSG_COMMON_BITMASK_IE(rb_reconfig_ptr->pdcp_Info.pdcp_Info,
			rrc_PDCP_Info_r4,headerCompressionInfoList)) )
        {
          WRRC_MSG0_ERROR("REL8: PDCP header compression info list not present");
          return FALSE;
        }
      }
    }
    
    if (RRC_MSG_COMMON_BITMASK_IE(rb_reconfig_ptr->pdcp_Info.pdcp_Info,
		rrc_PDCP_Info_r4,headerCompressionInfoList)) 
    {
      uint32 local_idx=0;
      while (rb_reconfig_ptr->pdcp_Info.pdcp_Info.headerCompressionInfoList.n > local_idx) 
      {
         pdcpHeaderCompressionInfo = &rb_reconfig_ptr->pdcp_Info.pdcp_Info.headerCompressionInfoList.elem[local_idx];
        if (RRC_CHECK_COMMON_MSG_TYPE(pdcpHeaderCompressionInfo->algorithmSpecificInfo,
            rrc_AlgorithmSpecificInfo_r4_rfc2507_Info))
        {
          /* if the UE capability "Maximum header compression context space", is exceeded with this configuration send
           * failure with cause "Invalid Config" */
          if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER > IPHC_MAX_HC_CONTEXT_SPACE)
          {
            WRRC_MSG1_HIGH("REL8: Max header comprss context space exceeded : %d",
                     pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER);
            return FALSE;
          }
        }
        else if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.t == 
                 T_rrc_AlgorithmSpecificInfo_r4_rfc3095_Info)
        {
          /*The chosen MAX_CID shall not be greater than the value "Maximum number of ROHC context sessions" 
            as indicated in the IE "PDCP Capability" */
          if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info,
		  	    ul_RFC3095))
          {
            if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->ul_RFC3095.max_CID > ROHC_MAX_CONTEXT_SESSIONS)
            {
              WRRC_MSG1_HIGH("REL8: Invalid config.  MAX_CID %d is invalid",
                       pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->ul_RFC3095.max_CID);
              return FALSE;
            }
          }
          
          if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info,
		  	   dl_RFC3095))
          {
            if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->dl_RFC3095.max_CID > ROHC_MAX_CONTEXT_SESSIONS)
            {
              WRRC_MSG1_HIGH("REL8: Invalid config.  MAX_CID %d is invalid",
                       pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->dl_RFC3095.max_CID);
              return FALSE;
            }
          }

        }/* else if (rrc_rfc3095_Info)*/

        local_idx++;
      } /*  while (pdcpHeaderCompressionInfoList) */
    } /*  if (rrc_PDCP_Info_r4_headerCompressionInfoList_present) */         
  }/*if (rrc_RB_InformationReconfig_r8_pdcp_Info_present)*/

  return TRUE;
}

/*============================================================================
FUNCTION: rrcllc_validate_pdcp_info_in_rb_info_setup_list_r8()

DESCRIPTION:
  This function validates the IE rrc_RB_InformationSetupList_r8 for PDCP Info return
  TRUE or FALSE accordingly
      
DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE/FALSE

SIDE EFFECTS:
=============================================================================*/
boolean rrcllc_validate_pdcp_info_in_rb_info_setup_list_r8
(
  rrc_RB_InformationSetup_r8 * rb_setup_ptr
)
{
#ifdef FEATURE_UMTS_PDCP
  rrc_HeaderCompressionInfo_r4 *pdcpHeaderCompressionInfo = NULL;
#endif /* FEATURE_UMTS_PDCP*/

    /*Check whether lossless SRNS relocation flag is set and PDCP header is absent in the message */
  if((RRC_MSG_COMMON_BITMASK_IE(rb_setup_ptr->pdcp_Info,rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
    (RRC_CHECK_COMMON_MSG_TYPE(rb_setup_ptr->pdcp_Info.losslessSRNS_RelocSupport,
    rrc_LosslessSRNS_RelocSupport_supported)))
  {
    if (rb_setup_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_absent)
    {
      WRRC_MSG0_ERROR("REL8: PDCP pdu header is absent for Lossless SRNS Relocation");
      return FALSE;
    }
  } 

  if(rb_setup_ptr->pdcp_Info.pdcp_PDU_Header == rrc_PDCP_PDU_Header_present)
  {
    if((RRC_MSG_COMMON_BITMASK_IE(rb_setup_ptr->pdcp_Info,rrc_PDCP_Info_r4,losslessSRNS_RelocSupport)) &&
      (RRC_CHECK_COMMON_MSG_TYPE(rb_setup_ptr->pdcp_Info.losslessSRNS_RelocSupport,
      rrc_LosslessSRNS_RelocSupport_notSupported)))
    {
      if(!(RRC_MSG_COMMON_BITMASK_IE(rb_setup_ptr->pdcp_Info,
	  	rrc_PDCP_Info_r4,headerCompressionInfoList)))
      {
        WRRC_MSG0_ERROR("REL8: PDCP header compression info list not present");
        return FALSE;
      }
    }
  }

  if (RRC_MSG_COMMON_BITMASK_IE(rb_setup_ptr->pdcp_Info,rrc_PDCP_Info_r4,headerCompressionInfoList))
  {
  uint32 local_idx=0;

    while(rb_setup_ptr->pdcp_Info.headerCompressionInfoList.n > local_idx)
    {
        pdcpHeaderCompressionInfo = &rb_setup_ptr->pdcp_Info.headerCompressionInfoList.elem[local_idx];
      if (RRC_CHECK_COMMON_MSG_TYPE(pdcpHeaderCompressionInfo->algorithmSpecificInfo,
          rrc_AlgorithmSpecificInfo_r4_rfc2507_Info))
      {
        /* if the UE capability "Maximum header compression context space", is exceeded with this configuration send
         * failure with cause "Invalid Config"  */
        if(
          (pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER > IPHC_MAX_HC_CONTEXT_SPACE))
        {
          WRRC_MSG1_HIGH("REL8: Max header comprss context space exceeded",
                   pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc2507_Info->max_HEADER);
          return FALSE;
        }
      }
      else if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.t == T_rrc_AlgorithmSpecificInfo_r4_rfc3095_Info)
      {
        /* 	the chosen MAX_CID shall not be greater than the value "Maximum number of ROHC context sessions" as 
         *  indicated in the IE "PDCP Capability" */
        if (RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(
			pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info,ul_RFC3095))
        {
          if (pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->ul_RFC3095.max_CID > ROHC_MAX_CONTEXT_SESSIONS)
          {
            WRRC_MSG1_HIGH("REL8: Invalid config.  MAX_CID %d is invalid",
                     pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->ul_RFC3095.max_CID);
            return FALSE;
          }
        }

        if(RRC_MSG_COMMON_BITMASK_IE_TYPE2_PTR(
			pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info,dl_RFC3095))
        {
          if(pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->dl_RFC3095.max_CID > ROHC_MAX_CONTEXT_SESSIONS)
          {
            WRRC_MSG1_HIGH("REL8: Invalid config.  MAX_CID %d is invalid",
                     pdcpHeaderCompressionInfo->algorithmSpecificInfo.u.rfc3095_Info->dl_RFC3095.max_CID);
            return FALSE;
          }
        }
      }  /*rrc_rfc3095_Info*/

      local_idx++;

    } /*  while (pdcpHeaderCompressionInfoList) */
  } /* rrc_PDCP_Info_r4_headerCompressionInfoList_present */
  return TRUE;
}
#endif /*FEATURE_UMTS_PDCP*/


/*============================================================================
FUNCTION: rrcrb_proc_waiting_for_l2_ack()

DESCRIPTION:
  This function checks is the passed procedure is in waiting for L2 ACK substate
  This function will check for RB procedures only
DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if passed procedure is in waiting for L2 ACK substate
  FALSE otherwise

SIDE EFFECTS:
=============================================================================*/
boolean rrcrb_proc_waiting_for_l2_ack
(
  rrc_proc_e_type procedure
)
{
  boolean proc_waiting_for_l2ack = FALSE;
  switch (procedure)
  {
    case RRC_PROCEDURE_RBE:
    case RRC_PROCEDURE_RBR:
    case RRC_PROCEDURE_RBRC:
    case  RRC_PROCEDURE_TCR:
    case  RRC_PROCEDURE_PCR:
      if(RRCRB_WAIT_FOR_L2_ACK == rrcrb_get_proc_sub_state(procedure))
      {
        proc_waiting_for_l2ack = TRUE;
      }
      break;
    case RRC_PROCEDURE_CU:
      break;
    default :
      break;
  }
  
  WRRC_MSG2_HIGH("Procedure rrc_proc_e_type%d waiting for L2 ACK ? :%d",procedure, proc_waiting_for_l2ack);
  return proc_waiting_for_l2ack;
}

/*============================================================================
FUNCTION: rrcrb_is_cu_going_to_be_triggered()

DESCRIPTION:
  This function checks if CU is going to be triggered or not
DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if CU is going to be triggeed
  FALSE otherwise

SIDE EFFECTS:
=============================================================================*/
boolean rrcrb_is_cu_going_to_be_triggered
(
  rrc_proc_e_type procedure
)
{

  boolean cu_status = FALSE;
  
  switch (procedure)
  {
    case RRC_PROCEDURE_RBE:
      cu_status = rrcrbe_send_initiate_cu_flag;
      break;
    case RRC_PROCEDURE_RBR:
       cu_status = rrcrbr_send_initiate_cu_flag;
      break;
    case RRC_PROCEDURE_RBRC:
      cu_status = rrcrbrc_send_initiate_cu_flag;
      break;
    case  RRC_PROCEDURE_TCR:
      cu_status = rrctcrc_send_initiate_cu_flag;
      break;
    case  RRC_PROCEDURE_PCR:
      cu_status = rrcpcrc_send_initiate_cu_flag;
      break;

    default :
      WRRC_MSG1_HIGH("Invalid Procedure : %d",procedure);
      break;
  }

  if((cu_status) || (rrccu_return_cu_substate() == RRCCU_WAIT_FOR_RB_PROC_COMPL_IND) )
  {
    return TRUE;
  }
  return FALSE;
}
#ifdef FEATURE_QSH_DUMP
/*===========================================================================
FUNCTION wrrc_qsh_dump_rb_proc_debug

DESCRIPTION
  This function dumps rb proc globals

DEPENDENCIES
  None


RETURN VALUE
None


SIDE EFFECTS
  None

===========================================================================*/
  __attribute__((section(".uncompressible.text")))

size_t wrrc_qsh_dump_rb_proc_debug(uint8* dump_addr, size_t size)
{
  wrrc_qsh_dump_tag_rb_proc rb_proc_globals;
  rrc_established_rabs_for_a_domain_type rab_info;
   qsh_client_dump_tag_hdr_init((qsh_dump_tag_hdr_s *)dump_addr,WRRC_QSH_DUMP_TAG_RB_PROC_DEBUG,
                                   (sizeof(wrrc_qsh_dump_tag_rb_proc) - sizeof(qsh_dump_tag_hdr_s)));
 WCDMA_MEMCPY(&rb_proc_globals,sizeof(wrrc_qsh_dump_tag_rb_proc),
  (wrrc_qsh_dump_tag_rb_proc*)dump_addr, sizeof(wrrc_qsh_dump_tag_rb_proc));
  rab_info = rrc_get_rabs_in_established_rabs_for_given_domain(RRC_PS_DOMAIN_CN_ID);;
  rb_proc_globals.rrc_rbe_substate = rrcrbe_get_current_sub_state();
  rb_proc_globals.rrc_rbrc_substate = rrcrbrc_get_current_sub_state();
  rb_proc_globals.rrc_pcr_substate = rrcpcrc_get_current_sub_state();
  rb_proc_globals.rrc_tcr_substate = rrctcrc_get_current_sub_state();
  rb_proc_globals.rrc_rbr_substate = rrcrbr_get_current_sub_state();
  rb_proc_globals.num_of_ps_rabs = rab_info.num_rabs;
  rab_info = rrc_get_rabs_in_established_rabs_for_given_domain(RRC_CS_DOMAIN_CN_ID);
  rb_proc_globals.num_of_cs_rabs = rab_info.num_rabs;
  rb_proc_globals.rrcrbe_send_initiate_cu_flag = rrcrbe_send_initiate_cu_flag;
  rb_proc_globals.rrcrbrc_send_initiate_cu_flag = rrcrbrc_send_initiate_cu_flag;
  rb_proc_globals.rrcpcrc_send_initiate_cu_flag = rrcpcrc_send_initiate_cu_flag;
  rb_proc_globals.rrctcrc_send_initiate_cu_flag = rrctcrc_send_initiate_cu_flag;
  rb_proc_globals.rrcrbr_send_initiate_cu_flag = rrcrbr_send_initiate_cu_flag;

  if(sizeof(rb_proc_globals) >= size)
  {
    WCDMA_MEMCPY((wrrc_qsh_dump_tag_rb_proc*)dump_addr, size,&rb_proc_globals, size);
    return size;
  }
  else
  {
    WCDMA_MEMCPY((wrrc_qsh_dump_tag_rb_proc*)dump_addr, sizeof(rb_proc_globals),&rb_proc_globals, sizeof(rb_proc_globals));
    return ((size_t)sizeof(rb_proc_globals));
  }
}
#endif
#ifdef FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME
/*============================================================================
FUNCTION: rrcrbcommon_set_failure_cause()

DESCRIPTION:
  This function sets failure cause if procedure is failed because of CU gettign triggered
DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
=============================================================================*/
void rrcrbcommon_set_failure_cause(void)
{
  if(RRCRB_WAIT_FOR_CELL_UPDATE_CNF == rrcrb_get_proc_sub_state(rrcdata_misc_info.procedure))
  {
      
    switch(rrcdata_misc_info.procedure)
    {
      case RRC_PROCEDURE_RBE:
        rrcrbe_set_failure_cause(RRCRB_PHY_CHAN_FAILURE);
        break;
      
      case RRC_PROCEDURE_RBRC:
        rrcrbrc_set_failure_cause(RRCRB_PHY_CHAN_FAILURE);
        break;
      
      case RRC_PROCEDURE_TCR:
        rrctcrc_set_failure_cause(RRCRB_PHY_CHAN_FAILURE);
        break;
    
      case RRC_PROCEDURE_PCR:
        rrcpcrc_set_failure_cause(RRCRB_PHY_CHAN_FAILURE);
        break;
      
      case RRC_PROCEDURE_RBR:
        rrcrbr_set_failure_cause(RRCRB_PHY_CHAN_FAILURE);
        break;
      case RRC_PROCEDURE_ASU:
        /*ASU is anyway ignored as CU is in progress. no need to set failure status*/
        break;

      default:
        WRRC_MSG1_ERROR("Invalid procedure id passed. something wrong %d", rrcdata_misc_info.procedure);
      break;
    }
      
  }


}
#endif  /*FEATURE_WCDMA_ALLOW_LTA_DURING_ACT_TIME*/

