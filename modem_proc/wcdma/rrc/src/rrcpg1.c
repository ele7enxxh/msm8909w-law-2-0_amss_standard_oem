/*===========================================================================
                  R R C  P A G I N G  T Y P E 1

DESCRIPTION
  This file contains the implementation for the Paging type 1 procedure.

EXTERNALIZED FUNCTIONS
  rrcpg1_procedure_event_handler
    This function is the event handler for all events that are sent to the
    Paging Type 1 procedure.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  
All Rights Reserved.QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrcpg1.c_v   1.19   13 Jun 2002 13:24:22   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcpg1.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/04/16   gv      Changes to send true page indication to wl1 for UTRAN orginated pages 
05/09/16   sp      Made changes to fix QSH Active set metric, UL OTA parameters metrics 
06/10/15   bc      Changes to ignore Paging type 1 messages received when UE is not in camped state.
11/10/14   sp      Made changes to save procedure substates in crash debug info
09/25/14   sn      Changes to send true page indication to WCDMA L1
07/21/14   sn      f3 reduction changes
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
02/03/14   as      Made changes to backup internal/external cmds for crash debugging
08/23/13   sr      Made changes to send PAGE_IND to NAS if CN page is received in PCH
08/14/13   vg      Made changes to implement a new diag event EVENT_WCDMA_RRC_FAILURE
06/27/13   gv      Made changes to buffer the MM and RR commands for crash debugging
01/03/13   ad      Made changes to reduce the redundant F3s
10/23/12   pm      Made changes to assign pointer to NULL after de-allocation
08/21/12   gv      Mainlined the feature FEATURE_WCDMA_FALSE_ALARM_IND and also added code
                   to indicate false alarm to WL1 when rrc_do_not_forward_page_nv is TRUE
08/13/12   gv      Made changes to indicate WL1 with false alarm when a duplicate page
                   msg with bcch modification info is received
03/07/12   gv      Modified the function name to fix compilation issue with free floating
03/07/12   mn      Changes to ignore other paging records in a paging type 1 message
                   when the U-RNTI in a paging record is same as UE U-RNTI.
01/11/12   sn      Changes to send ETWS security info to upper layers 
12/16/11   gv      Replaced QSR_MSG_HIGH with MSG_HIGH
12/12/11   gv      Added code to support free floating of tasks
09/14/11   rl      Made changes to get the release cause from the correct list element.
08/10/11   gv      Made changes to correctly validate the presence of paging record list
07/18/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.102.00
07/05/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.101.00
06/29/11   su      Added code for NV support to enable/disable
                   the FEATURE_RRC_DO_NOT_FORWARD_PAGE.
06/29/11   su      Added code changes to send CPHY_FALSE_PAGE_ALARM_IND
                   primitive to L1 in case of False Page Alarm.
06/22/11   su      Added code changes to avoid memory leak while posting 
                   UPDATE_SIB_IND, and triggering OOS when DL_WK_IND is
                   received when UPDATE_SIB event is active.
04/06/11   kp      Made changes to rrcpg1_modify_sibs_for_sfn_update to 
                   return stats so that L1 can re try some time later.
04/06/11   sks     Made changes to update NAS with correct page record.
04/05/11   su      Made changes to use the correct macro RRC_MAX_PAGE1.
03/31/11   su      Made changes to handle multiple records
                   (CN ID page and UTRAN ID page) in connected 
                   mode for the same UE correctly.
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/17/11   su      Made changes to move handling of 
                   v860NonCriticalExtensions of PagingType1 
                   message under FEATURE_WCDMA_ETWS.
02/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.90.50
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/18/11   su      Fixed compilation warnings.
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
12/22/10   su      Made code changes to support FEATURE_WCDMA_ETWS.
11/06/10   sks     Made changes to take imsi digits as uint8 instead of uint32 when
                   checking IMSI in PG1.
11/03/10   rl      Fixed compiler warnings
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
09/01/10   rm      Fixed lint errors
06/29/10   rm      Added code to support LTE<->WCDMA Inter RAT
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
03/12/10   dm      Added new member cause for Update SIBs indication.
03/12/10   rm      Added code to support FEATURE_DUAL_SIM
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
01/22/10   rm      Modified the usage of qchatupk_wcdma.h with rrc_qchatupk.h
                   (at the request of QCHAT team)
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
11/27/09   gkg     RRC changes required to integrate QChat Porting Kit Optimizations 
                   into AMSS. The changes are under feature FEATURE_QCHAT.
10/07/09   gkg     Changes for REL8 under FEATURE_WCDMA_REL8
05/10/09   kp      Added support for demand paging RRC code using pragma.
05/08/09   ss      Updated Copyright Information
04/29/09   ss      Made changes to compiler warnings on gcc compilers.
03/28/09   rm      CMI Featurisation
10/24/08   rm      Added support for Rove out from GAN, IDLE DETACHED MODE
                   Changes are under feature flag FEATURE_GAN_3G_ROVE_OUT_ONLY
10/07/08   kp      Added support to print page stats independent of FEATURE_RRC_DO_NOT_FORWARD_PAGE
09/12/08   ps      Added code for REl7 under FEATURE_WCDMA_REL7
06/18/07   kp      Bug fix to take care of making handling of I.E "Release indicator" 
                   independent of I.E-"CN originated page to connected mode UE".
06/04/07   kp      If N-BCCH is Up then ignore this attempt of S-BCCH set up request 
                   from L1 as this function is called by L1 at a timer expiry so there
                   is a corner case where in L1 will trigger this request even 
                   though N-BCCH is up.
04/20/07   da      Made changes needed to update ASN.1 version to R6 sept R6 2006
04/11/07   da      Added API to return values rrcpg1_num_pages_matched 
                   rrcpg1_num_pages to be used for QXDM display purpose.
04/10/07   kp      Bug fix for logging issue in BCCH Mod timer feature implementation. 
03/28/07   kp      Implemented support for BCCH Mod Timer.This support will take care
                   of posting UPDATE_SIBs command to SIB module just before the SFN at
                   which SIB’s will change as indicated by I.E BCCH Mod Time in BCCH ModInfo.
                   The approach is to start a timer whose value is the difference of 
                   current SFN & the SFN at which new sib’s will be broadcasted.
                   At the expiration of this timer post UPDATE_SIBs command to SIB module.
09/25/06   kp      Added support for V-Tag comparison of stored MIB with the one
                   received in BCCH-mod info
09/05/06   vm      Populated the new field tx_to_discon_reason in the command
                   initiate_cell_sel_ind with a default value.
                   Also, the function call rrc_transition_to_disconnected_state() 
                   now takes an additional argument.            
08/29/06   sm/da   Made changes required for R6 ASN.1 migration.  Also merged
                   FEATURE_WCDMA_REL6 and FEATURE WCDMA HSUPA.
04/18/06   da      Modified code to support migration to Mar-06 Rel-5 ASN1 +
                   R6 Hacks
04/10/06   kp      Added L1 drift support in RRC PCH state
02/28/06  da/sm    Modified code to support migration to Dec-05 Rel-6 ASN1
12/05/05   kc      Fixed compilation warnings in REL-5 branch
11/30/05   kc      Fixed Arm-7 vs Arm-9 ASN1 Notation dependency compiler issues for group release
11/23/05   kc      Added header files to remove compilation warnings without rel-5 dependencies
11/09/05   kc      Changes to support Group Release functionality in Rel-5
11/18/05   da      Contents in FEATURE_HANDLE_CN_PAGE_IN_CONNECTEDSTATE is ignored 
                   when GCF flag is True.
11/09/05   kc      Changes to setup PCCPCH based on L1 request
11/06/05   sk      Made change to take UE to idle if UE is paged from CN
                   using imsi, tmsi, ptmsi in connected state.
08/30/05   da      Made change so to not treat BCCH mod as false alarm in F3 msgs.
08/24/05   vr      Check if BCCH Modification time is present in the PG1 message
                   for BCCH Modification Ind.If not,set the value to zero.
03/02/05  ttl      Support RRC ASN1 Release 5.
02/07/05   da      Added additional parameter to f3 msg to indicated whether current 
                   call is false alarm or not.  
08/30/04   sgk     removed enum value RRCPG1_FAILURE_MAJOR from rcpg1_status_e_type
                   to fix lint error 'not referenced'.
07/13/04   sk      Added few debug prints to print paging type
11/12/03   sk      Added support for UTRAN originated paging
                   Added an enum rrcpg1_paging_type_e_type
                   Initiate a cell update when UTRAN originated paging is received
                   in Cell_PCH/URA_PCH state.
04/30/03   vk      Support for EVENT_WCDMA_PAGE_RECEIVED.
02/05/03   sk      March2002Rel specific changes 
                     Substitured   rrc_PCCH_MessageType_extension_chosen with
                                      rrc_PCCH_MessageType_spare_chosen
11/04/02   rj      Added implementaion to ignore BCCH Modification Info IE for
                   a specified time if RRC already updating SIBs due to
                   BCCH Modification Info.
10/24/02   rj      Deleted tmc.h from include files.
06/13/02   rj      Updated code to use U_RNTI from the ORDERED_CONFIG instead
                   of from RRCDATA module. Also added F3 messages for total 
                   pages and false alarms.
06/02/02   rj      Update function rrcpg1_process_paging_type1_msg to include
                   procedure name while sending RRC_UPDATE_SIBS_IND command
                   to SIB module.
05/15/02   rj      Added code to get stats for false alarm pages.
04/24/02   rj      Added code to forward Page with paging cause
                   RRC_PAGE_CAUSE_UNKNOWN to MM.
03/25/02   rj      Modified function rrcpg1_validate_paging_type_msg to fix
                   memory leak.
03/05/02   rj      Renamed varible rrc_u_rnti to rrc_u_rnti_internal.
02/28/02   rj      Added code to initiate Cell Update with Page response cause
                   if Paging type 1 is received either in CELL_PCH or URA_PCH
                   state.
02/13/02   rj      Fixed a typo in function rrcpg1_validate_paging_type_msg.
12/21/01   rj      Fixed some logical corrections for bit mask manipulations.
11/21/01   rj      Updated code to be compatible with June release of 25.331 
                   RRC Protocol Specification.
10/24/01   rj      Updated function rrcpg1_validate_paging_type_msg to select
                   Initial UE Id type from the Paging Type 1 msg and perfrom 
                   TMSI Page matching.
10/05/01   rj/kc   Fixed command pointer mismatch in function 
                   rrcpg1_process_paging_type1_msg 
                   while enqueueing RRC_UPDATE_SIBS_IND internal command. Also
                   Also corrected check for BCCH Modification Info presence.
09/22/01   rj      Added check for Paging Record List presence in function
                   rrcpg1_validate_paging_type_msg.
09/14/01   rj      Fixed a bug in paging matching and added F3 message to  
                   declare extensions are not supported. 
09/03/01   rj      Removed include file rrcasnutil.h since it is not required.
08/30/01   rj      Added code to check if Paging extension are chosen and 
                   Updated function rrcpg1_process_paging_type1_msg for 
                   peeking into OTA message.
07/20/01   rj      Added processing of Paging Type 1 for race conditions.
05/18/01   ram     In rrcpg1_validate_paging_type_msg when reading the paging
                   record list, added a check to make sure the list pointer
                   was not NULL.
04/29/01   rj      Included rrcscmgr.h since rrc_get_state function is moved
                   to RRCSCMGR module from RRCTASK module.
04/12/01   rj      Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Variable Definitions
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "wcdma_variation.h"
#include "comdef.h"

#include "rrccmd_v.h"               /* RRC command definitions */
#include "msg.h"
#include "err.h"
#include "rrcasn1.h"
#include "mmtask.h"
#ifdef FEATURE_CMI
#include "mmtask_v.h"
#endif
#include "rrcscmgr.h"
#include "rrcllc.h"
#include "rrctmr.h"
#include "rrclogging.h"
#include "rrcrcr.h"
#include "rrcmisc.h"
#include "macrrcif_v.h"
#include "rrcsibproc.h"
#include "seq.h"
#include "rrcsibdb.h"
#include "rrcpg1.h"
#include "rrcnv.h"
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
#error code not present
#endif
#include "rrcccm.h"
#ifdef TEST_FRAMEWORK
#error code not present
#endif
#ifdef FEATURE_QCHAT
#include "rrc_qchatupk.h"
#endif /*FEATURE_QCHAT*/
#ifdef FEATURE_WCDMA_ETWS
#include "rrcbmc.h"
#endif


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#define RRCPG1_TIMER_TO_IGNORE_BCCH_MOD_INFO 200
#define GROUP_RELEASE_COPY_BYTE_1 1
#define GROUP_RELEASE_COPY_BYTE_2 2
#define GROUP_RELEASE_COPY_BYTE_3 3
#define GROUP_RELEASE_COPY_BYTE_4 4

#define RRC_MAX_PAGE1 8
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */


typedef enum
{
  RRCPG1_SUCCESS,                            /* Successful Status */      
  RRCPG1_RELEASE,                            /* Release the call  */
  RRCPG1_LIST_ABSENT,                        /* Indicate that paging list is absent */
  RRCPG1_FAILURE_MINOR                       /* Minor Failure Status */
} rrcpg1_status_e_type;

typedef enum
{
  RRCPG1_CN_ORIGINATED_PAGE,                   /* CN originated paging in idle mode*/      
  RRCPG1_CN_ORIGINATED_PAGE_TO_CONNECTED_UE,   /* CN originated paging in connected mode*/
  RRCPG1_UTRAN_ORIGINATED_PAGE_TO_CONNECTED_UE /* UTRAN originated paging in connected mode*/
} rrcpg1_paging_type_e_type;


/* -----------------------------------------------------------------------
** Global Constant Data Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

rrc_cn_domain_identity_e_type   rrcpg1_cn_domain_id;
rrc_paging_cause_e_type         rrcpg1_paging_cause;

  /*flag to indicate the paging to upper layers */
LOCAL rrcpg1_paging_type_e_type  rrcpg1_paging_type;
/* The UE ID used for paging */
rrc_page_ue_id_choice_e_type    rrcpg1_page_ue_id;

LOCAL uint32 rrcpg1_num_pages;        /* Number of Paging Type 1 messages 
                                         received */
LOCAL uint32 rrcpg1_num_pages_matched; 
                                      /* Number of Pages that are 
                                         matched */
LOCAL boolean rrcpg1_bcch_mod_timer_status;
                                      /* Status of Timer to ignore
                                         BCCH Modifcation Info */
                                        
/*Saved Mib value tag & BCCH Mod Time from OTA message*/
LOCAL rrcpg1_paging_bcch_mod_info bcch_mod_info;

#ifdef FEATURE_HANDLE_CN_PAGE_IN_CONNECTED_STATE
  boolean rrcpg1_match_found_conn_mode_cn_id = FALSE;
#endif
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

FUNCTION rrcpg1_modify_sibs_for_sfn_update/rrcpg1_ff_modify_sibs_for_sfn_update

DESCRIPTION


DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
#ifndef FEATURE_WCDMA_FREE_FLOATING_TASK
boolean rrcpg1_modify_sibs_for_sfn_update
#else
void rrcpg1_ff_modify_sibs_for_sfn_update
#endif
(
  void
)
{
  rrc_cmd_type *rrc_cmd_ptr;            /* Pointer to the RRC Command */
  rrc_MasterInformationBlock * mib_ptr;
  rrc_state_e_type rrc_state;
  boolean sib_active, status=FALSE;
  boolean is_n_bcch_up = FALSE;

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
  l1_ext_cmd_type *l1_cmd_ptr;
#endif

  sib_active = rrcsib_is_sib_active();
  rrc_state = rrc_get_state();

  is_n_bcch_up = rrcllc_get_n_bcch_setup_status();

  MSG_4(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"sfn update: N-BCCH status %d, rrc %d, timer %d SIB change guard %d",
         is_n_bcch_up,
         rrc_state,
         rrcpg1_bcch_mod_timer_status,
         sib_change_safe_guard);

  /*If N-BCCH is Up then ignore this attempt of S-BCCH set up request from L1 
   as this function is called by L1 at a timer expiry so there is a corner case where
   in L1 will trigger this request even though N-BCCH is up*/

  if((( RRC_STATE_DISCONNECTED==rrc_state)||( RRC_STATE_CELL_PCH==rrc_state)
        ||( RRC_STATE_URA_PCH==rrc_state)) && 
        (FALSE == sib_active)&&
        (sib_change_safe_guard == FALSE)&&
        (is_n_bcch_up == FALSE)
    )
  {
    if ( rrcpg1_bcch_mod_timer_status == FALSE)
    {

      /* Build RRC_UPDATE_SIBS_IND command to post to SIB */
      /* Allocates the buffer to RRC Internal command */
      if( (rrc_cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
      {     
        /* Fill in the command parameters for RRC_UPDATE_SIBS_IND */
        rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_UPDATE_SIBS_IND;
        rrc_cmd_ptr->cmd.update_sibs_ind.cause = RRCSIB_SFN_UPDATE; 

        if((mib_ptr= (rrc_MasterInformationBlock *)  rrc_sibdb_return_sib_for_srv_cell
           (rrc_MIB)) != NULL)
        {
          rrc_cmd_ptr->cmd.update_sibs_ind.mib_value_tag = mib_ptr->mib_ValueTag;

          WRRC_MSG2_HIGH("mib-vtag is %d sfn is %d ***", rrc_cmd_ptr->cmd.update_sibs_ind.mib_value_tag, 
                   seq_get_sfn());

          rrc_cmd_ptr->cmd.update_sibs_ind.procedure = RRC_PROCEDURE_PG1;
          rrc_cmd_ptr->cmd.update_sibs_ind.real_sib_mod = FALSE;

          rrc_put_int_cmd(rrc_cmd_ptr);   
          status = TRUE;
        }
        else
        {
          rrc_free_cmd_buf(rrc_cmd_ptr);
          rrc_cmd_ptr = NULL;
        }
      } 
    }
  }
#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
  if(status == FALSE)
  {
    l1_cmd_ptr = l1_get_cmd_buf();
    
    /* Fill in the Command Header */
    l1_cmd_ptr->cmd_hdr.cmd_id = CPHY_NUDGE_FOR_SFN_MISS_MATCH_RSP;
    l1_cmd_ptr->cmd.rrc_nudge_status_update.status = status;
    rrc_send_l1_cmd(l1_cmd_ptr);
  }
#else
  return status;
#endif
}
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/

/*******************************************************************************************
*************************************NON-Demand paging section end*****************************
*******************************************************************************************/

/* =======================================================================
**                            Function Definitions
** ======================================================================= */
/*===========================================================================

FUNCTION rrcpg1_copy_group_bit_string_to_urnti  

DESCRIPTION
  This function copies the bit_string received in group-paging to local uint32

DEPENDENCIES
  None.

RETURN VALUE
  Returns an integer value of URNTI-Group.

===========================================================================*/
static uint32  rrcpg1_copy_group_bit_string_to_urnti
(
  ASN1BitStr32 group_release_string,
  uint32       copy_length
)
{
  uint32 i=0;
  uint32 local_urnti=0;
  uint32 oss_string_to_urnti_iterations = 4;

  //copy the 4 bytes of ossBitString that represent the U-RNTI group into
  //a 32 bit int value..  Depending on the copy length,  copy the contents
  //from the group_release_string,  else just shift left to make it a 32
  //bit number
  for(i=0; i < oss_string_to_urnti_iterations; i++)
  {
    if(i < copy_length)
    {
      local_urnti |=  group_release_string.data[i] ;
    }
    
    //left shift it to make it a uint32 value of urnti, do it only 3 times.. last iteration, no need to 
    //left shift it as last iteration needs to be in lsb itself
    if(i < 3)
    {
      local_urnti = local_urnti << 8;
    }

  }

  return local_urnti;
}
/*===========================================================================

FUNCTION rrc_compare_group_release_info  

DESCRIPTION
  This function compares GROUP-URNTI received, with the URNTI stored by the
  UE.  

DEPENDENCIES
  None.

RETURN VALUE
SUCCESS:  If the comparison matches
FAILURE:  If the comparison fails

===========================================================================*/
uecomdef_status_e_type rrc_compare_group_release_info
(
  rrc_GroupReleaseInformation * group_release_info
)
{
  rrc_int_u_rnti_type u_rnti_group = 0;     /* Local vriable to store U-RNTI gotten from GROUP*/
  rrc_int_u_rnti_type u_rnti_stored = 0;    /* Local vriable to store U-RNTI received from RRCLLC */

  
  uint32 compare_bit_mask_shift_right_length=0;

  //In the switch stmt below,  compare the URNTI-Group choice and convert the URNTI to a uint32 type
  switch(group_release_info->uRNTI_Group.t)
  {
  case T_rrc_U_RNTI_Group_all: 
    compare_bit_mask_shift_right_length = 0;
    WRRC_MSG0_HIGH("Group Match Success for Pg1.  ALL chosen");
    return SUCCESS;
    //break absent here to keep lint happy

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b1: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b1), 
                                                         GROUP_RELEASE_COPY_BYTE_4);
    compare_bit_mask_shift_right_length = 1;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b2: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b2), 
                                                         GROUP_RELEASE_COPY_BYTE_4);
    compare_bit_mask_shift_right_length = 2;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b3: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b3), 
                                                         GROUP_RELEASE_COPY_BYTE_4);
    compare_bit_mask_shift_right_length = 3;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b4: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b4), 
                                                         GROUP_RELEASE_COPY_BYTE_4);
    compare_bit_mask_shift_right_length = 4;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b5: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b5), 
                                                         GROUP_RELEASE_COPY_BYTE_4);
    compare_bit_mask_shift_right_length = 5;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b6: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b6), 
                                                         GROUP_RELEASE_COPY_BYTE_4);
    compare_bit_mask_shift_right_length = 6;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b7: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b7), 
                                                         GROUP_RELEASE_COPY_BYTE_4);
    compare_bit_mask_shift_right_length = 7;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b8: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b8), 
                                                         GROUP_RELEASE_COPY_BYTE_3);
    compare_bit_mask_shift_right_length = 8;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b9: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b9), 
                                                         GROUP_RELEASE_COPY_BYTE_3);
    compare_bit_mask_shift_right_length = 9;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b10: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b10), 
                                                         GROUP_RELEASE_COPY_BYTE_3);
    compare_bit_mask_shift_right_length = 10;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b11: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b11), 
                                                         GROUP_RELEASE_COPY_BYTE_3);
    compare_bit_mask_shift_right_length = 11;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b12: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b12), 
                                                         GROUP_RELEASE_COPY_BYTE_3);
    compare_bit_mask_shift_right_length = 12;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b13: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b13), 
                                                         GROUP_RELEASE_COPY_BYTE_3);
    compare_bit_mask_shift_right_length = 13;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b14: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b14), 
                                                         GROUP_RELEASE_COPY_BYTE_3);
    compare_bit_mask_shift_right_length = 14;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b15: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b15), 
                                                         GROUP_RELEASE_COPY_BYTE_3);
    compare_bit_mask_shift_right_length = 15;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b16: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b16), 
                                                         GROUP_RELEASE_COPY_BYTE_2);
    compare_bit_mask_shift_right_length = 16;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b17: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b17), 
                                                         GROUP_RELEASE_COPY_BYTE_2);
    compare_bit_mask_shift_right_length = 17;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b18: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b18), 
                                                         GROUP_RELEASE_COPY_BYTE_2);
    compare_bit_mask_shift_right_length = 18;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b19: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b19), 
                                                         GROUP_RELEASE_COPY_BYTE_2);
    compare_bit_mask_shift_right_length = 19;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b20: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b20), 
                                                         GROUP_RELEASE_COPY_BYTE_2);
    compare_bit_mask_shift_right_length = 20;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b21: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b21), 
                                                         GROUP_RELEASE_COPY_BYTE_2);
    compare_bit_mask_shift_right_length = 21;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b22: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b22), 
                                                         GROUP_RELEASE_COPY_BYTE_2);
    compare_bit_mask_shift_right_length = 22;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b23: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b23), 
                                                         GROUP_RELEASE_COPY_BYTE_2);
    compare_bit_mask_shift_right_length = 23;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b24: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b24), 
                                                         GROUP_RELEASE_COPY_BYTE_1);
    compare_bit_mask_shift_right_length = 24;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b25: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b25), 
                                                         GROUP_RELEASE_COPY_BYTE_1);
    compare_bit_mask_shift_right_length = 25;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b26: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b26), 
                                                         GROUP_RELEASE_COPY_BYTE_1);
    compare_bit_mask_shift_right_length = 26;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b27: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b27), 
                                                         GROUP_RELEASE_COPY_BYTE_1);
    compare_bit_mask_shift_right_length = 27;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b28: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b28), 
                                                         GROUP_RELEASE_COPY_BYTE_1);
    compare_bit_mask_shift_right_length = 28;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b29: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b29), 
                                                         GROUP_RELEASE_COPY_BYTE_1);
    compare_bit_mask_shift_right_length = 29;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b30: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b30), 
                                                         GROUP_RELEASE_COPY_BYTE_1);
    compare_bit_mask_shift_right_length = 30;
    break;

  case T_rrc_U_RNTI_Group_u_RNTI_BitMaskIndex_b31: 
    u_rnti_group = rrcpg1_copy_group_bit_string_to_urnti(*(group_release_info->uRNTI_Group.u.u_RNTI_BitMaskIndex_b31), 
                                                         GROUP_RELEASE_COPY_BYTE_1);
    compare_bit_mask_shift_right_length = 31;
    break;    

  default:
    WRRC_MSG0_ERROR("Unexpected Bit-Mask.  Paging-Group No-Match");
    return FAILURE;
    //break absent here to keep lint happy
  }

  if(U_RNTI_NOT_PRESENT == rrcllc_get_current_urnti(&u_rnti_stored) )
  {
    ERR_FATAL("URNTI is not present",0,0,0);
  }

  MSG_HIGH("Stored-URNTI 0x%x, Group-URNTI 0x%x, Group-Bit-Mask %d", u_rnti_stored, u_rnti_group, 
           compare_bit_mask_shift_right_length);
  
  //shift both URNTI's to match the bits' that we need to compare
  u_rnti_stored >>= compare_bit_mask_shift_right_length;
  u_rnti_group >>= compare_bit_mask_shift_right_length;

  WRRC_MSG2_HIGH("Shifted-Stored-URNTI 0x%x, Shifted-Group-URNTI 0x%x", u_rnti_stored, u_rnti_group);

  if(u_rnti_stored == u_rnti_group)
  {
    return SUCCESS;
  }
  else
  {
    return FAILURE;
  }
}
/*===========================================================================

FUNCTION rrcpg1_update_rrc_page_ind  

DESCRIPTION
  This function updates the parameters that are required for RRC_PAGE_IND
  command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

static void rrcpg1_update_rrc_page_ind
(
  rrc_PagingRecord *paging_record_ptr        /* Pointer to PCCH message*/
)
{
  /* Structure to log the event when paging occurs */
  rrc_event_paging_info_type    paging_event_info;

  switch(paging_record_ptr->t)
  {
    case T_rrc_PagingRecord_cn_Identity:
      switch(paging_record_ptr->u.cn_Identity->cn_DomainIdentity)
      { 
        case rrc_CN_DomainIdentity_cs_domain:
          rrcpg1_cn_domain_id = RRC_CS_DOMAIN_CN_ID;
          break;

        case rrc_CN_DomainIdentity_ps_domain:
          rrcpg1_cn_domain_id = RRC_PS_DOMAIN_CN_ID;
          break;

        default:
          WRRC_MSG1_ERROR("Unknown CN Domian Id: %d is received", paging_record_ptr->u.cn_Identity->
               cn_DomainIdentity);
          break;
      }

      switch(paging_record_ptr->u.cn_Identity->pagingCause)
      {
        case rrc_PagingCause_terminatingConversationalCall:
          rrcpg1_paging_cause = RRC_PAGE_CAUSE_TERM_CONV_CALL;
          break;

        case rrc_PagingCause_terminatingStreamingCall:
          rrcpg1_paging_cause = RRC_PAGE_CAUSE_TERM_STREAM_CALL;
          break;

        case rrc_PagingCause_terminatingInteractiveCall:
          rrcpg1_paging_cause = RRC_PAGE_CAUSE_TERM_INTERACT_CALL;
          break;

        case rrc_PagingCause_terminatingBackgroundCall:
          rrcpg1_paging_cause = RRC_PAGE_CAUSE_TERM_BACKGND_CALL;
          break;

        case rrc_PagingCause_terminatingHighPrioritySignalling:
          rrcpg1_paging_cause = RRC_PAGE_CAUSE_HIGH_PRI_SIGNALLING;
          break;

        case rrc_PagingCause_terminatingLowPrioritySignalling:
          rrcpg1_paging_cause = RRC_PAGE_CAUSE_LOW_PRI_SIGNALLING;
          break;

        case rrc_PagingCause_terminatingCauseUnknown:
          rrcpg1_paging_cause = RRC_PAGE_CAUSE_UNKNOWN;
          break;


        default:
          WRRC_MSG1_ERROR("Unknown Paging Cause: %d",paging_record_ptr->u.cn_Identity->pagingCause);
          break;
      }
      switch(paging_record_ptr->u.cn_Identity->cn_pagedUE_Identity.t)
      {
        case T_rrc_CN_PagedUE_Identity_imsi_GSM_MAP:
          rrcpg1_page_ue_id = RRC_PAGE_WITH_IMSI;
          break;
       
        case T_rrc_CN_PagedUE_Identity_tmsi_GSM_MAP:
          rrcpg1_page_ue_id = RRC_PAGE_WITH_TMSI;
          break;

        case T_rrc_CN_PagedUE_Identity_p_TMSI_GSM_MAP:
          rrcpg1_page_ue_id = RRC_PAGE_WITH_PTMSI;
          break;

        case T_rrc_CN_PagedUE_Identity_imsi_DS_41:
        case T_rrc_CN_PagedUE_Identity_tmsi_DS_41:
        default:
          WRRC_MSG1_ERROR("Unsupported UE Id %d is received",paging_record_ptr->u.cn_Identity->
               cn_pagedUE_Identity.t);
          break;
      }
       
      break;

    case T_rrc_PagingRecord_utran_Identity:
      switch( paging_record_ptr->u.utran_Identity->
              cn_OriginatedPage_connectedMode_UE.cn_DomainIdentity)
      {
        case rrc_CN_DomainIdentity_cs_domain:
          rrcpg1_cn_domain_id = RRC_CS_DOMAIN_CN_ID;
          break;

        case rrc_CN_DomainIdentity_ps_domain:
          rrcpg1_cn_domain_id = RRC_PS_DOMAIN_CN_ID;
          break;

        default:
          WRRC_MSG1_ERROR("Unknown CN Domian Id: %d is received",
              paging_record_ptr->u.utran_Identity->
              cn_OriginatedPage_connectedMode_UE.cn_DomainIdentity);
          break;
      }    
  
      switch(paging_record_ptr->u.utran_Identity->
             cn_OriginatedPage_connectedMode_UE.pagingCause)
      {
        case rrc_PagingCause_terminatingConversationalCall:
          rrcpg1_paging_cause = RRC_PAGE_CAUSE_TERM_CONV_CALL;
          break;
    
        case rrc_PagingCause_terminatingStreamingCall:
          rrcpg1_paging_cause = RRC_PAGE_CAUSE_TERM_STREAM_CALL;
          break;
    
        case rrc_PagingCause_terminatingInteractiveCall:
          rrcpg1_paging_cause = RRC_PAGE_CAUSE_TERM_INTERACT_CALL;
          break;
    
        case rrc_PagingCause_terminatingBackgroundCall:
          rrcpg1_paging_cause = RRC_PAGE_CAUSE_TERM_BACKGND_CALL;
          break;
      
        case rrc_PagingCause_terminatingHighPrioritySignalling:
          rrcpg1_paging_cause = RRC_PAGE_CAUSE_HIGH_PRI_SIGNALLING;
          break;
      
        case rrc_PagingCause_terminatingLowPrioritySignalling:
          rrcpg1_paging_cause = RRC_PAGE_CAUSE_LOW_PRI_SIGNALLING;
          break;
      
        case rrc_PagingCause_terminatingCauseUnknown:
          rrcpg1_paging_cause = RRC_PAGE_CAUSE_UNKNOWN;
          break;
      
        default:
          WRRC_MSG1_ERROR("Unknown Paging Cause: %d",paging_record_ptr->u.cn_Identity->pagingCause);
          break;
      }
      
      switch( paging_record_ptr->u.utran_Identity->
              cn_OriginatedPage_connectedMode_UE.pagingRecordTypeID)
      {
        case rrc_PagingRecordTypeID_imsi_GSM_MAP:
          rrcpg1_page_ue_id = RRC_PAGE_WITH_IMSI;
          break;
       
        case rrc_PagingRecordTypeID_tmsi_GSM_MAP_P_TMSI:
          rrcpg1_page_ue_id = RRC_PAGE_WITH_TMSI;
          break;
      
        case rrc_PagingRecordTypeID_imsi_DS_41:
        case rrc_PagingRecordTypeID_tmsi_DS_41:
        default:
          WRRC_MSG1_ERROR("Unsupported UE Id %d is received",
               paging_record_ptr->u.utran_Identity->cn_OriginatedPage_connectedMode_UE.
               pagingRecordTypeID);
        break;
      }
      break;

    default:
      WRRC_MSG1_HIGH("Unknown Paging Identity: %d ", paging_record_ptr->t);
      break;
  }

  /* Fill the Paging type */
  paging_event_info.paging_type = RRCLOG_PAGING_TYPE_1;
  /* Populate the paging cause field with the appro[riate cause*/
  paging_event_info.paging_cause = rrcpg1_paging_cause;
  /* Send the event */

  event_report_payload(EVENT_WCDMA_PAGE_RECEIVED, /* Paging event ID */
                       sizeof(rrc_event_paging_info_type), /* payload size */
                       &paging_event_info); /* paging cause and type */
  WRRC_MSG2_HIGH("Page1 type %d, cause %d ",paging_event_info.paging_type,paging_event_info.paging_cause);

} /* rrcpg1_update_rrc_page_ind */
/*===========================================================================

FUNCTION rrcpg1_validate_paging_type_msg  

DESCRIPTION
  This function looks into all Paging Records of the Paging Type 1 message
  until if finds a match for either IMSI, TMSI or PTMSI.

DEPENDENCIES
  None.

RETURN VALUE
  rrcpg1_status_e_type : Returns RRCPG1_SUCCESS if it finds match. Otherwise
  it returns RRCPG1_FAILURE_MINOR.

===========================================================================*/

static rrcpg1_status_e_type rrcpg1_validate_paging_type_msg
(
  rrc_PCCH_Message *pcch_msg_ptr        /* Pointer to PCCH message*/
)
{
  rrcpg1_status_e_type status = RRCPG1_FAILURE_MINOR;
                                        /* status is initilaized to failure */
  boolean match_found = FALSE;          /* This is local varibale to 
                                           indicates Paging Record conatins
                                           UE initial Identity or U-RNTI */

#ifdef FEATURE_HANDLE_CN_PAGE_IN_CONNECTED_STATE
  boolean match_found_conn_mode_cn_id = FALSE;   /* This is local varibale to 
                                                                    indicate Paging Record conatins
                                                                    CN paged UE identity in connected mode */
#endif
  rrc_imsi_gsm_map_type imsi;           /* Local variable to store the 
                                           IMSI value */
  rrc_int_u_rnti_type u_rnti;           /* Local vriable to store received 
                                           U-RNTI in Paging Type 1 */
  rrc_int_u_rnti_type u_rnti_local;     /* Local vriable to store U-RNTI
                                           received from RRCLLC */
  uint32 length_in_bits;                /* Temporary variable to store length
                                           of TMSI or PTMSI in bits*/
  uint32 length_in_bytes;               /* Temporary variable to store length
                                           of TMSI or PTMSI in bytes*/
  uint32 idx =0;

#ifdef FEATURE_HANDLE_CN_PAGE_IN_CONNECTED_STATE
  rrc_cmd_type *int_cmd_ptr;            /* Pointer RRC Command */
#endif //FEATURE_HANDLE_CN_PAGE_IN_CONNECTED_STATE

 /* Check whether externsion chosen */
  if(RRC_CHECK_COMMON_MSG_TYPE(pcch_msg_ptr->message,rrc_PCCH_MessageType_spare))
  {
    WRRC_MSG0_ERROR("Extensions are not supported");
    return (status);
  }

  /* Check whether Paging Record List is present */  
  if(!(RRC_MSG_COMMON_BITMASK_IE_TYPE2(pcch_msg_ptr->message.u.pagingType1,
        pagingRecordList)))
  {
    MSG_MED("Paging Record List is not present",0,0,0);
    return (status);
  }

  while(!((match_found) ||( idx >= RRC_MAX_PAGE1))) 
  {
    switch(pcch_msg_ptr->message.u.pagingType1.pagingRecordList.elem[idx].t)
    {
      case T_rrc_PagingRecord_cn_Identity:
        /* we should ignore this page if UE is in connected state */
        if( (rrc_get_state() == RRC_STATE_CONNECTING) || 
            (rrc_get_state() == RRC_STATE_DISCONNECTED) ) 
        {
          rrcpg1_paging_type = RRCPG1_CN_ORIGINATED_PAGE;
          switch (pcch_msg_ptr->message.u.pagingType1.pagingRecordList.elem[idx].u.
                  cn_Identity->cn_pagedUE_Identity.t)
          {
            case T_rrc_CN_PagedUE_Identity_imsi_GSM_MAP:
              /* Check whether IMSI recieved from NAS is valid */
              if(rrc_imsi_valid == TRUE)
              {
                /* Translate IMSI to internal format */
                rrc_translate_gsm_map_imsi(pcch_msg_ptr->message.u.pagingType1.
                                           pagingRecordList.elem[idx].u.cn_Identity->
                                           cn_pagedUE_Identity.u.imsi_GSM_MAP,
                                           &imsi);
                /* Search for the IMSI match in the Paging Record */
                if( (imsi.num_digits == rrc_imsi.num_digits) &&
                    (!memcmp((const char *) imsi.value,(const char *) rrc_imsi.value,
                    (imsi.num_digits * sizeof (uint8) ) ) )
                  )
                {
                  rrcpg1_update_rrc_page_ind(&pcch_msg_ptr->message.u.pagingType1.pagingRecordList.elem[idx]);

                  match_found = TRUE;

                  status = RRCPG1_SUCCESS;
                }
              }
              else /* RRC does not have a valid IMSI. So do not look for one in
                      the Paging Record */
              {
                WRRC_MSG1_HIGH("Page Recieved with IMSI when internal IMSI is invalid",
                          0);
              }
              break;

            case T_rrc_CN_PagedUE_Identity_tmsi_GSM_MAP:
              /* Check whether TMSI recieved from NAS is valid */
              if(rrc_tmsi_valid == TRUE)
              {

                length_in_bits = RRC_GET_BITS_STRING_LEN_PTR(pcch_msg_ptr->message.u.pagingType1.
                   pagingRecordList.elem[idx].u.cn_Identity->cn_pagedUE_Identity.u.
                   tmsi_GSM_MAP);
                /* Convert the received length in bits to the length in bytes */
                length_in_bytes = length_in_bits / 8;

                length_in_bytes += (length_in_bits % 8) ? 1: 0;

                /* Search for the TMSI match in the Paging Record */
                if( (length_in_bytes == RRC_MAX_GSMMAP_TMSI_LENGTH) &&
                    (!memcmp((const char *)(RRC_GET_BITS_PTR_STRING_PTR(pcch_msg_ptr->message.u.pagingType1.
                     pagingRecordList.elem[idx].u.cn_Identity->cn_pagedUE_Identity.u.
                     tmsi_GSM_MAP)),
                     (const char *) rrc_tmsi.value,RRC_MAX_GSMMAP_TMSI_LENGTH) ) 
                  )
                {
                  rrcpg1_update_rrc_page_ind(&pcch_msg_ptr->message.u.pagingType1.pagingRecordList.elem[idx]);

                  match_found = TRUE;

                  status = RRCPG1_SUCCESS;
                }
              }
              else /* RRC does not have a valid TMSI. So do not look for one in
                      the Paging Record */
              {
                WRRC_MSG1_HIGH("Page Recieved with TMSI when internal TMSI is invalid",
                          0);
              }
              break;

            case T_rrc_CN_PagedUE_Identity_p_TMSI_GSM_MAP:
              /* Check whether TMSI recieved from NAS is valid */
              if(rrc_ptmsi_valid == TRUE)
              {
                length_in_bits = RRC_GET_BITS_STRING_LEN_PTR(pcch_msg_ptr->message.u.pagingType1.
                   pagingRecordList.elem[idx].u.cn_Identity->cn_pagedUE_Identity.u.
                   p_TMSI_GSM_MAP);
                /* Convert the received length in bits to the length in bytes */
                length_in_bytes = length_in_bits / 8;

                length_in_bytes += (length_in_bits % 8) ? 1: 0;

                /* Search for the PTMSI match in the Paging Record */
                if( (length_in_bytes == RRC_MAX_GSMMAP_PTMSI_LENGTH ) &&
                    (!memcmp((const char *) (RRC_GET_BITS_PTR_STRING_PTR(pcch_msg_ptr->message.u.pagingType1.
                     pagingRecordList.elem[idx].u.cn_Identity->cn_pagedUE_Identity.u.
                     p_TMSI_GSM_MAP)),
                     (const char *) rrc_ptmsi.value,RRC_MAX_GSMMAP_PTMSI_LENGTH) )
                  ) 
                {
                  rrcpg1_update_rrc_page_ind(&pcch_msg_ptr->message.u.pagingType1.pagingRecordList.elem[idx]);

                  match_found = TRUE;

                  status = RRCPG1_SUCCESS;
                }
              }
              else /* RRC does not have a valid PTMSI. So do not look for one in
                      the Paging Record */
              {
                WRRC_MSG1_HIGH("Page Recieved with PTMSI when internal PTMSI is invalid",
                          0);
              }
              break;

            case T_rrc_CN_PagedUE_Identity_imsi_DS_41:
            case T_rrc_CN_PagedUE_Identity_tmsi_DS_41:
            default:
              WRRC_MSG1_HIGH("Unsupported CN Paged UE Indentity: %d", 
                        pcch_msg_ptr->message.u.pagingType1.pagingRecordList.elem[idx].
                        u.cn_Identity->cn_pagedUE_Identity.t);
              break;
          }

        }
        else
        {
          WRRC_MSG1_ERROR("Wrong CN orig page in State %d",rrc_get_state());

#ifdef FEATURE_HANDLE_CN_PAGE_IN_CONNECTED_STATE
          if (!rrc_get_gcf_flag_status()) 
          {     
            rrcpg1_paging_type = RRCPG1_CN_ORIGINATED_PAGE;
            switch (pcch_msg_ptr->message.u.pagingType1.pagingRecordList.elem[idx].u.
                    cn_Identity->cn_pagedUE_Identity.t)
            {
              case T_rrc_CN_PagedUE_Identity_imsi_GSM_MAP:
                /* Check whether IMSI recieved from NAS is valid */
                if(rrc_imsi_valid == TRUE)
                {
                  /* Translate IMSI to internal format */
                  rrc_translate_gsm_map_imsi(pcch_msg_ptr->message.u.pagingType1.
                                             pagingRecordList.elem[idx].u.cn_Identity->
                                             cn_pagedUE_Identity.u.imsi_GSM_MAP,
                                             &imsi);
                  /* Search for the IMSI match in the Paging Record */
                  if( (imsi.num_digits == rrc_imsi.num_digits) &&
                      (!memcmp((const char *) imsi.value,(const char *) rrc_imsi.value,
                               (imsi.num_digits * sizeof (uint8) ) ) )
                      )
                  {
                    match_found_conn_mode_cn_id = TRUE;
                    rrcpg1_update_rrc_page_ind(&pcch_msg_ptr->message.u.pagingType1.pagingRecordList.elem[idx]);
                  }
                  else
                  {
                    WRRC_MSG0_HIGH("IMSI mismatch");
                  }
                }
                else /* RRC does not have a valid IMSI. So do not look for one in
                      the Paging Record */
                {
                  WRRC_MSG1_HIGH("Page Recieved with IMSI when internal IMSI is invalid",
                           0);
                }
                break;
                
              case T_rrc_CN_PagedUE_Identity_tmsi_GSM_MAP:
                /* Check whether TMSI recieved from NAS is valid */
                if(rrc_tmsi_valid == TRUE)
                {
                  
                  length_in_bits = RRC_GET_BITS_STRING_LEN_PTR(pcch_msg_ptr->message.u.pagingType1.
                    pagingRecordList.elem[idx].u.cn_Identity->cn_pagedUE_Identity.u.
                    tmsi_GSM_MAP);
                  /* Convert the received length in bits to the length in bytes */
                  length_in_bytes = length_in_bits / 8;
                  
                  length_in_bytes += (length_in_bits % 8) ? 1: 0;
                  
                  /* Search for the TMSI match in the Paging Record */
                  if( (length_in_bytes == RRC_MAX_GSMMAP_TMSI_LENGTH) &&
                      (!memcmp((const char *)(RRC_GET_BITS_PTR_STRING_PTR(pcch_msg_ptr->message.u.pagingType1.
                               pagingRecordList.elem[idx].u.cn_Identity->cn_pagedUE_Identity.u.
                               tmsi_GSM_MAP)),
                               (const char *) rrc_tmsi.value,RRC_MAX_GSMMAP_TMSI_LENGTH) ) 
                      )
                  {
                    match_found_conn_mode_cn_id = TRUE;
                    rrcpg1_update_rrc_page_ind(&pcch_msg_ptr->message.u.pagingType1.pagingRecordList.elem[idx]);
                  }
                  else
                  {
                    WRRC_MSG0_HIGH("TMSI mismatch");
                  }
                }
                else /* RRC does not have a valid TMSI. So do not look for one in
                      * the Paging Record */
                {
                  WRRC_MSG1_HIGH("Page Recieved with TMSI when internal TMSI is invalid",
                           0);
                }
                break;
                
              case T_rrc_CN_PagedUE_Identity_p_TMSI_GSM_MAP:
                /* Check whether TMSI recieved from NAS is valid */
                if(rrc_ptmsi_valid == TRUE)
                {
                  length_in_bits = RRC_GET_BITS_STRING_LEN_PTR(pcch_msg_ptr->message.u.pagingType1.
                    pagingRecordList.elem[idx].u.cn_Identity->cn_pagedUE_Identity.u.
                    p_TMSI_GSM_MAP);
                  /* Convert the received length in bits to the length in bytes */
                  length_in_bytes = length_in_bits / 8;
                  
                  length_in_bytes += (length_in_bits % 8) ? 1: 0;

                  /* Search for the PTMSI match in the Paging Record */
                  if( (length_in_bytes == RRC_MAX_GSMMAP_PTMSI_LENGTH ) &&
                      (!memcmp((const char *)(RRC_GET_BITS_PTR_STRING_PTR(pcch_msg_ptr->message.u.pagingType1.
                               pagingRecordList.elem[idx].u.cn_Identity->cn_pagedUE_Identity.u.
                               p_TMSI_GSM_MAP)),
                               (const char *) rrc_ptmsi.value,RRC_MAX_GSMMAP_PTMSI_LENGTH) )
                      ) 
                  {
                    match_found_conn_mode_cn_id = TRUE;
                    rrcpg1_update_rrc_page_ind(&pcch_msg_ptr->message.u.pagingType1.pagingRecordList.elem[idx]);
                  }
                  else
                  {
                    WRRC_MSG0_HIGH("PTMSI mismatch");
                  }
                }
                else /* RRC does not have a valid PTMSI. So do not look for one in
                        the Paging Record */
                {
                  WRRC_MSG1_HIGH("Page Recieved with PTMSI when internal PTMSI is invalid",
                           0);
                }
                break;

              case T_rrc_CN_PagedUE_Identity_imsi_DS_41:
              case T_rrc_CN_PagedUE_Identity_tmsi_DS_41:
              default:
                WRRC_MSG1_HIGH("Unsupported CN Paged UE Indentity: %d", 
                         pcch_msg_ptr->message.u.pagingType1.pagingRecordList.elem[idx].
                         u.cn_Identity->cn_pagedUE_Identity.t);
                break;
            }
          }
#endif /*FEATURE_HANDLE_CN_PAGE_IN_CONNECTED_STATE */

        }
        break;

      
      case T_rrc_PagingRecord_utran_Identity:
        if( (rrc_get_state() == RRC_STATE_CONNECTING) || 
            (rrc_get_state() == RRC_STATE_DISCONNECTED) ) 
        {
          /*lint -e666 */
          WRRC_MSG1_HIGH("Wrong Paging Origination in State %d",rrc_get_state());
          /*lint +e666 */
        }
        else
        {
          rrc_translate_urnti(&pcch_msg_ptr->message.u.pagingType1.
                              pagingRecordList.elem[idx].
                              u.utran_Identity->u_RNTI, &u_rnti);

          if(U_RNTI_NOT_PRESENT == rrcllc_get_current_urnti(&u_rnti_local) )
          {
            ERR_FATAL("URNTI is not present",0,0,0);
          }

          if(u_rnti == u_rnti_local)
          {
            if(pcch_msg_ptr->message.u.pagingType1.pagingRecordList.elem[idx].
               u.utran_Identity->m.cn_OriginatedPage_connectedMode_UEPresent)
            {
              rrcpg1_update_rrc_page_ind(&pcch_msg_ptr->message.u.pagingType1.pagingRecordList.elem[idx]);

              rrcpg1_paging_type = RRCPG1_CN_ORIGINATED_PAGE_TO_CONNECTED_UE;

              match_found = TRUE;

              status = RRCPG1_SUCCESS;
            }    
            /* if it is not CN originated page then do not inform upper layers about 
            paging but UE has to initiate cell update so return success here */
            else
            {
              match_found = TRUE;

              rrcpg1_paging_type = RRCPG1_UTRAN_ORIGINATED_PAGE_TO_CONNECTED_UE;

              status = RRCPG1_SUCCESS;
            }
          }
        }
        break;
     
     default:
       WRRC_MSG1_HIGH("Unknown Paging Identity: %d ", 
                 pcch_msg_ptr->message.u.pagingType1.pagingRecordList.elem[idx].t);
       break;
    }
    idx++;

    if( pcch_msg_ptr->message.u.pagingType1.pagingRecordList.n <= idx )
    {
      MSG_MED("Paging record list ended", 0, 0, 0);
      break; /* Break out of the while loop */
    }
  }
#ifdef FEATURE_HANDLE_CN_PAGE_IN_CONNECTED_STATE
  /* If we receive CN Identity Page in connected mode and no Utran Page Identity
    is received for us in this Paging record List, we will transition to Idle in order to 
    maintain sync with Network*/
  if((match_found == FALSE) && (match_found_conn_mode_cn_id == TRUE))
  {
    WRRC_MSG2_HIGH("PG1: match_found for UTRAN ID Page is %d, and for for CN ID Page is %d ",
              match_found,match_found_conn_mode_cn_id);
    rrcpg1_match_found_conn_mode_cn_id = match_found_conn_mode_cn_id;

    /* Allocates the buffer to RRC Internal command */
    if( (int_cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
    { 
      int_cmd_ptr->cmd_hdr.cmd_id = RRC_INITIATE_CELL_SELECTION_IND;
      int_cmd_ptr->cmd.initiate_cell_sel_ind.cause = RRC_TRANSITION_TO_DISCONNECTED;
      /* pretend as if it is coming from CU so that CSP will process it properly*/
      int_cmd_ptr->cmd.initiate_cell_sel_ind.proc = RRC_PROCEDURE_CU;
      int_cmd_ptr->cmd.initiate_cell_sel_ind.dest_freq_present = FALSE;
      int_cmd_ptr->cmd.initiate_cell_sel_ind.dest_psc_present  = FALSE;
      int_cmd_ptr->cmd.initiate_cell_sel_ind.dest_state = RRC_STATE_DISCONNECTED;
      int_cmd_ptr->cmd.initiate_cell_sel_ind.tx_to_discon_reason = RRC_TX_TO_DISCON_OTHER;
      rrc_put_int_cmd(int_cmd_ptr); 
    }
  }
#endif
  return(status);
} /*rrcpg1_validate_paging_type_msg */

/*===========================================================================

FUNCTION rrcpg1_validate_paging_type_msg_r5 

DESCRIPTION
  This function looks into all Paging Records of the Paging Type 1 message
  until if finds a match for either IMSI, TMSI or PTMSI.

DEPENDENCIES
  None.

RETURN VALUE
  rrcpg1_status_e_type : Returns RRCPG1_SUCCESS if it finds match. Otherwise
  it returns RRCPG1_FAILURE_MINOR.

===========================================================================*/

static rrcpg1_status_e_type rrcpg1_validate_paging_type_msg_r5
(
  rrc_PCCH_Message *pcch_msg_ptr        /* Pointer to PCCH message*/
)
{
  rrcpg1_status_e_type status = RRCPG1_FAILURE_MINOR;
                                        /* status is initilaized to failure */
  rrc_int_u_rnti_type u_rnti;           /* Local vriable to store received 
                                           U-RNTI in Paging Type 1 */
  rrc_int_u_rnti_type u_rnti_local;     /* Local vriable to store U-RNTI
                                           received from RRCLLC */

  rrc_PagingRecord2List_r5 *local_paging_record_List;
                                       /* Local Pointer to Paging Record List*/

  rrc_state_e_type  rrc_state;
  rrc_PagingRecord2_r5_utran_GroupIdentity  *local_group_release_list;
  uint32 idx =0,idy=0;
 /* Check whether externsion chosen */
  if( RRC_CHECK_COMMON_MSG_TYPE(pcch_msg_ptr->message,rrc_PCCH_MessageType_spare))
  {
    WRRC_MSG0_ERROR("Extensions are not supported");
    return (status);
  }

#ifdef FEATURE_WCDMA_ETWS
  if(pcch_msg_ptr->message.u.pagingType1.laterNonCriticalExtensions.v590NonCriticalExtensions.
     m.v860NonCriticalExtensionsPresent)
  {
    if(pcch_msg_ptr->message.u.pagingType1.laterNonCriticalExtensions.v590NonCriticalExtensions.v860NonCriticalExtensions.
       pagingType1_v860ext.m.etws_InformationPresent)
    {
      WRRC_MSG0_HIGH("RRCPG1:ETWS info present");
      rrcbmc_send_etws_information_ind_message(pcch_msg_ptr->message.u.pagingType1.laterNonCriticalExtensions.v590NonCriticalExtensions.
                               v860NonCriticalExtensions.pagingType1_v860ext.etws_Information,NULL);
    }
    else
    {
      WRRC_MSG0_HIGH("REL8: v860NonCriticalExtensions not supported");
      status = RRCPG1_LIST_ABSENT;
      return (status);
    } 
  }
#endif

  if(!(pcch_msg_ptr->message.u.pagingType1.laterNonCriticalExtensions.v590NonCriticalExtensions.
      pagingType1_v590ext.m.pagingRecord2ListPresent))
  {
    WRRC_MSG0_HIGH("Paging Record List not Present");
    status = RRCPG1_LIST_ABSENT;
    return (status);
  }

  //for rel-5 only Utran Originated Paging is supported by ASN1
  rrc_state = rrc_get_state();
  if( (RRC_STATE_CONNECTING == rrc_state) || 
      (RRC_STATE_DISCONNECTED == rrc_state) ) 
  {
    /*lint -e666 */
    WRRC_MSG1_HIGH("Wrong Paging Origination in State %d",rrc_get_state());
    return (status);
    /*lint +e666 */
  }

  /* Update the head of Linked list */
  local_paging_record_List =  &pcch_msg_ptr->message.u.pagingType1.laterNonCriticalExtensions.
    v590NonCriticalExtensions.pagingType1_v590ext.pagingRecord2List;
  idx =0;
  while(local_paging_record_List->n > idx) 
  {
    if(local_paging_record_List->elem[idx].t == T_rrc_PagingRecord2_r5_utran_SingleUE_Identity)
    {
      rrc_translate_urnti(&local_paging_record_List->elem[idx].u.utran_SingleUE_Identity->
                          u_RNTI, &u_rnti);

      if(U_RNTI_NOT_PRESENT == rrcllc_get_current_urnti(&u_rnti_local) )
      {
        ERR_FATAL("URNTI is not present",0,0,0);
      }

      if(u_rnti == u_rnti_local)
      {
        //if network is indicating a call release, then update the release cause and return release enum
        /*According to 25.331 spec section 8.1.2.3 I.E "Release indicator" should be handled independently 
         *of I.E- "CN originated page to connected mode UE" which is optional I.E in Paging record.
         */
        if(local_paging_record_List->elem[idx].u.utran_SingleUE_Identity->rrc_ConnectionReleaseInformation.t == 
            T_rrc_RRC_ConnectionReleaseInformation_release)
        {
          if (rrc_ReleaseCause_unspecified == local_paging_record_List->elem[idx].u.utran_SingleUE_Identity->rrc_ConnectionReleaseInformation.u.release->releaseCause)
          {
            rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                    (uint8) RRC_PROCEDURE_PG1,
                                    (uint8) RRCLOG_releaseCauseUnspecified,
                                    (uint8) RRCLOG_PROTOCOL_ERROR_NA
                                  );  
          }        
          //update the release cause and return release enum
          rrc_release_cause = rrcrcr_update_release_cause(local_paging_record_List->elem[idx].u.
                                                          utran_SingleUE_Identity->
                                                          rrc_ConnectionReleaseInformation.u.
                                                          release->releaseCause);
          return RRCPG1_RELEASE;
        }
     
        if(local_paging_record_List->elem[idx].u.utran_SingleUE_Identity->m.cn_OriginatedPage_connectedMode_UEPresent)
        {
          switch( local_paging_record_List->elem[idx].u.utran_SingleUE_Identity->
                  cn_OriginatedPage_connectedMode_UE.cn_DomainIdentity)         
          { 
            case rrc_CN_DomainIdentity_cs_domain:
              rrcpg1_cn_domain_id = RRC_CS_DOMAIN_CN_ID;
              break;
    
            case rrc_CN_DomainIdentity_ps_domain:
              rrcpg1_cn_domain_id = RRC_PS_DOMAIN_CN_ID;
              break;
    
            default:
              WRRC_MSG1_ERROR("Unknown CN Domian Id: %d is received",
                  local_paging_record_List->elem[idx].u.utran_SingleUE_Identity->
                  cn_OriginatedPage_connectedMode_UE.cn_DomainIdentity);
              break;
          }    

          switch( local_paging_record_List->elem[idx].u.utran_SingleUE_Identity->
                  cn_OriginatedPage_connectedMode_UE.pagingCause)
          {
          case rrc_PagingCause_terminatingConversationalCall:
            rrcpg1_paging_cause = RRC_PAGE_CAUSE_TERM_CONV_CALL;
            break;
    
          case rrc_PagingCause_terminatingStreamingCall:
            rrcpg1_paging_cause = RRC_PAGE_CAUSE_TERM_STREAM_CALL;
            break;
    
          case rrc_PagingCause_terminatingInteractiveCall:
            rrcpg1_paging_cause = RRC_PAGE_CAUSE_TERM_INTERACT_CALL;
            break;
    
          case rrc_PagingCause_terminatingBackgroundCall:
            rrcpg1_paging_cause = RRC_PAGE_CAUSE_TERM_BACKGND_CALL;
            break;
    
          case rrc_PagingCause_terminatingHighPrioritySignalling:
            rrcpg1_paging_cause = RRC_PAGE_CAUSE_HIGH_PRI_SIGNALLING;
            break;
    
          case rrc_PagingCause_terminatingLowPrioritySignalling:
            rrcpg1_paging_cause = RRC_PAGE_CAUSE_LOW_PRI_SIGNALLING;
            break;
    
          case rrc_PagingCause_terminatingCauseUnknown:
            rrcpg1_paging_cause = RRC_PAGE_CAUSE_UNKNOWN;
            break;
    
          default:
            WRRC_MSG1_ERROR("Unknown Paging Cause: %d",local_paging_record_List->elem[idx].u.utran_SingleUE_Identity->
               cn_OriginatedPage_connectedMode_UE.pagingCause);
            break;
          }
          
          switch( local_paging_record_List->elem[idx].u.utran_SingleUE_Identity->
              cn_OriginatedPage_connectedMode_UE.pagingRecordTypeID)
          {
          case rrc_PagingRecordTypeID_imsi_GSM_MAP:
            rrcpg1_page_ue_id = RRC_PAGE_WITH_IMSI;
            break;
    
          case rrc_PagingRecordTypeID_tmsi_GSM_MAP_P_TMSI:
            rrcpg1_page_ue_id = RRC_PAGE_WITH_TMSI;
            break;
    
          case rrc_PagingRecordTypeID_imsi_DS_41:
          case rrc_PagingRecordTypeID_tmsi_DS_41:
          default:
            WRRC_MSG1_ERROR("Unsupported UE Id %d is received",
                local_paging_record_List->elem[idx].u.utran_SingleUE_Identity->
                cn_OriginatedPage_connectedMode_UE.pagingRecordTypeID);
            break;
          }

          rrcpg1_paging_type = RRCPG1_CN_ORIGINATED_PAGE_TO_CONNECTED_UE;

          return RRCPG1_SUCCESS;
        }    
        /* if it is not CN originated page then do not inform upper layers about 
        paging but UE has to initiate cell update so return success here */
        else
        {
          rrcpg1_paging_type = RRCPG1_UTRAN_ORIGINATED_PAGE_TO_CONNECTED_UE;

          return RRCPG1_SUCCESS;
        }
      }
    }
    else if(local_paging_record_List->elem[idx].t == T_rrc_PagingRecord2_r5_utran_GroupIdentity)
    {
      local_group_release_list = local_paging_record_List->elem[idx].u.utran_GroupIdentity;
      idy =0;
      while(local_group_release_list->n > idy)
      {
        if(SUCCESS == rrc_compare_group_release_info(&local_group_release_list->
                                                     elem[idy].groupReleaseInformation))
        {
          if(local_group_release_list->elem[idy].rrc_ConnectionReleaseInformation.t == 
             T_rrc_RRC_ConnectionReleaseInformation_release)  
          {
            if (rrc_ReleaseCause_unspecified == local_group_release_list->elem[idy].
                                                rrc_ConnectionReleaseInformation.u.
                                                release->releaseCause)
            {
              rrc_log_protocol_error( (uint8) (rrc_get_state()),
                                      (uint8) RRC_PROCEDURE_PG1,
                                      (uint8) RRCLOG_releaseCauseUnspecified,
                                      (uint8) RRCLOG_PROTOCOL_ERROR_NA
                                    );  
            }
            //update the release cause and return release enum
            rrc_release_cause = rrcrcr_update_release_cause(local_group_release_list->elem[idy].
                                                            rrc_ConnectionReleaseInformation.u.
                                                            release->releaseCause);
            return RRCPG1_RELEASE;
          }
          else if(local_group_release_list->elem[idy].rrc_ConnectionReleaseInformation.t == 
                  T_rrc_RRC_ConnectionReleaseInformation_noRelease) 
          {
            // URNTI matched but release info is set to "No Release".Do a cell update
            rrcpg1_paging_type = RRCPG1_UTRAN_ORIGINATED_PAGE_TO_CONNECTED_UE;
            return RRCPG1_SUCCESS;
          }
        }
 
        idy++;
      }
      
      

    }
    idx++;
    /* Check if the pointer is NULL */
    if(local_paging_record_List  == NULL )
    {
      break; /* Break out of the while loop */
    }
  }

  return(status);

} /*rrcpg1_validate_paging_type_msg_r5 */

/*===========================================================================

FUNCTION rrcpg1_send_page_ind  

DESCRIPTION
  This function allocates memory and sends RRC_PAGE_IND to NAS when a page match is found

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/

void rrcpg1_send_page_ind(void)
{
  mm_cmd_type *mm_cmd_ptr;  /* Pointer to the MM Command */

  /* Allocate memory for RRC_PAGE_IND command */
  if( (mm_cmd_ptr =  mm_rrc_get_cmd_buf(RRC_PAGE_IND)) != NULL)
  {
    /* Fill in parameters fro RRC_CLOASE_SESSION_IND command */
    mm_cmd_ptr->cmd.rrc_page_ind.cn_domain_id = rrcpg1_cn_domain_id;
  
    mm_cmd_ptr->cmd.rrc_page_ind.paging_cause = rrcpg1_paging_cause;
  
    mm_cmd_ptr->cmd.rrc_page_ind.page_ue_id = rrcpg1_page_ue_id;
  
#ifdef FEATURE_DUAL_SIM
    mm_cmd_ptr->cmd.rrc_page_ind.as_id = rrc_get_as_id();
#endif

    WRRC_MSG0_HIGH(" Dispatching out MM cmd / Sent RRC_PAGE_IND to the MM");
    rrc_send_rrc_message_to_mm(mm_cmd_ptr);               /* Sends the command to MM */
  }
}

/*===========================================================================

FUNCTION rrcpg1_process_paging_type1_msg  

DESCRIPTION
  This function checks whether RRC_DOWNLINK_SDU_IND contains Paging Type 1
  message.If it contains Paging Type 1 mesage,it will processes the same and
  send RRC_PAGE_IND to the MM. If there BCCH modification Info is present, it
  also sends RRC_UPDATE_SIBS_IND to the SIB procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/

static void rrcpg1_process_paging_type1_msg
(
  rrc_cmd_type *cmd_ptr                 /* Pointer to the RRC Command */
)
{
  rrc_PCCH_Message *pcch_msg_ptr;       /* Pointer to PCCH message*/

  rrc_cmd_type *rrc_cmd_ptr;            /* Pointer to the RRC Command */

  boolean false_paging_alarm = TRUE;   /* boolean showing false alarm status */

  rrcpg1_status_e_type page_status = RRCPG1_FAILURE_MINOR;

#ifdef FEATURE_QSH_EVENT_METRIC
  rrc_log_pg_ota page_info;
  memset(&page_info,0,sizeof(rrc_log_pg_ota));
#endif

  /* Assert the receieved message is of type rrc_DL_PCCH_Message */

  if((RRC_DOWNLINK_SDU_IND == cmd_ptr->cmd_hdr.cmd_id) && 
     (cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type == rrc_PCCH_Message_PDU) )
  {
    if(cmd_ptr->cmd.downlink_sdu_ind.decoded_msg == NULL)
    {
      /* Ignore the message.  ASN1 decoding failed */ 
      MSG_LOW("ASN1 decoding  Failed", 0,0,0);
     
    }
    else /* ASN1 decoding is successful */
    {
      /* Increment the counter for number of Paging Type 1 messages */
      rrcpg1_num_pages++;

       /* Get the OTA message pointer */
      pcch_msg_ptr = (rrc_PCCH_Message *) 
                     cmd_ptr->cmd.downlink_sdu_ind.decoded_msg;  
      

      if (rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL5)
      {  
        if(RRC_MSG_COMMON_BITMASK_IE(pcch_msg_ptr->message.u.pagingType1,
           rrc_PagingType1,laterNonCriticalExtensions))
        {
          page_status = rrcpg1_validate_paging_type_msg_r5(pcch_msg_ptr);
  
          if(RRCPG1_RELEASE == page_status)
          {
            //send cmd to transition to idle
            (void)rrc_transition_to_disconnected_state(RRC_PROCEDURE_PG1, RRC_TX_TO_DISCON_OTHER);
  
            WRRC_MSG0_HIGH("UE connection being released by UTRAN");
          }
          else if(RRCPG1_SUCCESS == page_status)
          {
            /* Increment the no. of page matches */
            rrcpg1_num_pages_matched++;
            /* This page is not a false alarm */
            false_paging_alarm = FALSE;
            MSG_HIGH("Paging type1, type:%d, cause:%d, id:%d",
                     rrcpg1_paging_type,
                     rrcpg1_paging_cause,
                     rrcpg1_page_ue_id
                     );
          }
        }
      }

      //if in Rel-5 Extensions we don't have a successful match,  i.e.  SUCCESS/RELEASE
      //try to look into the Rel-99 message
      if((RRCPG1_SUCCESS != page_status) && (RRCPG1_RELEASE != page_status))
      {        
        page_status = rrcpg1_validate_paging_type_msg(pcch_msg_ptr);

        /* Validate the Paging Type 1 message */
        if(RRCPG1_SUCCESS ==  page_status)
        {
          /* Increment the no. of page matches */
          rrcpg1_num_pages_matched++;
          /* This page is not a false alarm */
          false_paging_alarm = FALSE;
          MSG_HIGH("Paging type1, type:%d, cause:%d, id:%d",
                   rrcpg1_paging_type,
                   rrcpg1_paging_cause,
                   rrcpg1_page_ue_id
                   );
#ifdef FEATURE_QCHAT
          if(qchatupk_wcdma_rdw_enabled())
          {
            WRRC_MSG0_HIGH("QCHAT is also processing PG1 OTA");
            qchatupk_rrcpg1_process_paging_type1_msg(pcch_msg_ptr);
          }
#endif /*FEATURE_QCHAT*/  
        }
      }
        
      if(RRCPG1_SUCCESS == page_status)
      {
        if (rrcpg1_paging_type != RRCPG1_UTRAN_ORIGINATED_PAGE_TO_CONNECTED_UE)
        {
          if((rrc_do_not_forward_page_nv == FALSE)
#ifdef FEATURE_GAN_3G_ROVE_OUT_ONLY
             #error code not present
#endif
             )
          {
            rrcpg1_send_page_ind();
          }
          else
          {
            false_paging_alarm = TRUE;
          }
        }
        /* UTRAN originated page. No need to inform upper layers. Initiate cell
          update procedure with cause "paging response" */
        else
        {
          if (RRC_STATE_CELL_PCH == rrc_get_state() || 
              RRC_STATE_URA_PCH == rrc_get_state() )
          {
            /* Build RRC_CELL_UPDATE_INITIATE_REQ command to post to 
               Cell Update */

            /* Allocates the buffer to RRC Internal command */
            if( (rrc_cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
            {     
              /* Fill in the command parameters for RRC_CELL_UPDATE_INITIATE_REQ */
              rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_CELL_UPDATE_INITIATE_REQ;

              rrc_cmd_ptr->cmd.initiate_cell_update.cause = RRC_PAGING_RESPONSE;
              rrc_cmd_ptr ->cmd.initiate_cell_update.cu_complete_cnf_required = FALSE;

              rrc_cmd_ptr->cmd.initiate_cell_update.procedure = RRC_PROCEDURE_PG1;

              WRRC_MSG0_HIGH("Initiate cell update for UTRAN originated page");
              rrc_put_int_cmd(rrc_cmd_ptr);   
            } 
          }
          else
          {
            WRRC_MSG2_ERROR("Invalid paging type %d in rrc state %d", 
                      rrcpg1_paging_type, rrc_get_state());
          }
        }
      }
      /* Check whether BCCH Modification Info is present */
      if(RRC_MSG_COMMON_BITMASK_IE_TYPE2(pcch_msg_ptr->message.u.pagingType1,
        bcch_ModificationInfo))
      {
        /* Increment the no. of page matches */
        rrcpg1_num_pages_matched++;
        /* This page is not a false alarm */
        false_paging_alarm = FALSE;
        if ( rrcpg1_bcch_mod_timer_status == FALSE)
        {
          /*Compare the value of IE "MIB value tag" in the IE "BCCH modification info" 
           with the value tag stored for the master information block in SIB data base 
           for serving cell.If both Differ only then process PG1 BCCH Mod I.E other 
           wise UE has current system info so Ignore this Page1 message*/
          if(((rrc_get_serving_cell_mib_value_tag()) != (uint32)
              (pcch_msg_ptr->message.u.pagingType1.bcch_ModificationInfo.mib_ValueTag))&&
             (RRC_NOT_CAMPED_ON != rrc_ccm_get_curr_camping_status(RRC_PROCEDURE_CSP))
            )
          {
            /* Set the timer status to TRUE */
            rrcpg1_bcch_mod_timer_status = TRUE;
#ifdef FEATURE_QSH_EVENT_METRIC 
            page_info.sys_info_mod = TRUE;
#endif
            /* Check if the modification time is present.*/
            if(RRC_MSG_COMMON_BITMASK_IE_TYPE2(pcch_msg_ptr->message.u.pagingType1.bcch_ModificationInfo,
                bcch_ModificationTime))
            {
              rrcpg1_process_bcch_mod_info_msg_ie(
                                              (uint8)pcch_msg_ptr->message.u.pagingType1.bcch_ModificationInfo.mib_ValueTag,
                                              (uint16)pcch_msg_ptr->message.u.pagingType1.bcch_ModificationInfo.bcch_ModificationTime,
                                              TRUE
                                             );
              WRRC_MSG2_HIGH("BCCH modification ind in page, time:%d, value_tag:%d",
                                                                              pcch_msg_ptr->message.u.pagingType1.bcch_ModificationInfo.bcch_ModificationTime,
                                                                              pcch_msg_ptr->message.u.pagingType1.bcch_ModificationInfo.mib_ValueTag);
            }
            else
            {
              rrcpg1_process_bcch_mod_info_msg_ie(
                                              (uint8)pcch_msg_ptr->message.u.pagingType1.bcch_ModificationInfo.mib_ValueTag,
                                              RRCPG1_INVALID_BCCH_MOD_TIMER_VAL,
                                              FALSE
                                             );
              WRRC_MSG1_HIGH("BCCH modification ind in page, Mib value_tag:%d",
                                                                         pcch_msg_ptr->message.u.pagingType1.bcch_ModificationInfo.mib_ValueTag);
            }
            /* Start the timer */
            rrctmr_start_timer(RRCTMR_IGNORE_BCCH_MOD_TIMER, 
                               RRCPG1_TIMER_TO_IGNORE_BCCH_MOD_INFO);
          }
          else
          {
            WRRC_MSG2_HIGH("Ignoring PG1 BCCH mod IE since Srv-cell's MIB tage=%d & rexd MIB tage in PG1 = %d",
                                 rrc_get_serving_cell_mib_value_tag(),
                                 pcch_msg_ptr->message.u.pagingType1.bcch_ModificationInfo.mib_ValueTag);
          }
        }
        else
        {
          WRRC_MSG0_HIGH("BCCH Mod Info is Ignored. Timer ON");
          false_paging_alarm = TRUE;
        }
      }
      MSG_HIGH("Total Pages: %d, False Alarms: %d, Curr Pg Status: %d",rrcpg1_num_pages,
                 (rrcpg1_num_pages - rrcpg1_num_pages_matched), false_paging_alarm);

      if(false_paging_alarm == TRUE)
      {
        wl1_drx_page_ind(FALSE);
      }
      else
      {
        wl1_drx_page_ind(TRUE);
      }

    }
    
  }
  else if (cmd_ptr->cmd_hdr.cmd_id == 
            RRC_IGNORE_BCCH_MOD_TIMER_EXPIRED_IND)
  {
    /* Reset the timer status */
    rrcpg1_bcch_mod_timer_status = FALSE;
  }
  else if(cmd_ptr->cmd_hdr.cmd_id == 
            RRC_BCCH_MODIFICATION_TIMER_EXPIRED_IND)
  {
    /*Post UPDATE_SIBS_CMD to SIB */
    rrcpg1_post_update_sibs_cmd();
  }
  else
  {
    WRRC_MSG2_HIGH("Invalid Event: %d or SDU type: %d is forwarded to PG1 Procedure",
              cmd_ptr->cmd_hdr.cmd_id,cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type);
  }
  
#ifdef FEATURE_QSH_EVENT_METRIC
  page_info.page_record_match = page_status;
  page_info.cn_id = rrcpg1_cn_domain_id;
  page_info.paging_cause = rrcpg1_paging_cause; 
  rrc_qsh_log_paging_params(RRC_PROCEDURE_PG1,&page_info);
#endif
} /* rrcpg1_process_paging_type1_msg */
/*===========================================================================

FUNCTION rrcpg1_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the Paging Type 1 procedure.

  The following events are handled by this procedure:

    RRC_DOWNLINK_SDU_IND Primitive (with PAGING TYPE 1 Message)


DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcpg1_procedure_event_handler
(
  rrc_cmd_type *cmd_ptr                       /* Pointer to RRC command */
)
{
 
  rrc_state_e_type rrc_state = rrc_get_state();
  rrc_crash_debug_save_proc_substates(RRC_PROCEDURE_PG1,RRC_INVALID_PROC_SUBSTATE,cmd_ptr->cmd_hdr.cmd_id);

  WRRC_MSG1_HIGH("function rrcpg1_procedure_event_handler is called in rrc_state = rrc_state_e_type_value%d",rrc_state);

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
  if(RRC_CPHY_NUDGE_FOR_SFN_MISS_MATCH_REQ == cmd_ptr->cmd_hdr.cmd_id)
  {
    rrcpg1_ff_modify_sibs_for_sfn_update();
    return;
  }
#endif

  switch (rrc_state)
  {
    case RRC_STATE_DISCONNECTED: 
    case RRC_STATE_CELL_PCH:
    case RRC_STATE_URA_PCH:
    case RRC_STATE_CONNECTING:
      rrcpg1_process_paging_type1_msg(cmd_ptr); 
      break;
 
    case RRC_STATE_CELL_FACH:    
    case RRC_STATE_CELL_DCH:  
      if(RRC_DOWNLINK_SDU_IND == cmd_ptr->cmd_hdr.cmd_id)
      {
        /* These two states are invalid to receive Paging Type 1 message. 
         * But due to race conditions, RRC can process the Paging
           Type 1 after transitioning into CELL_FACH or CELL_DCH states. */
        WRRC_MSG1_HIGH("Paging Type 1 message is received in invalid state %d",
                  rrc_state); 
      }
      rrcpg1_process_paging_type1_msg(cmd_ptr); 
      break;

    default: 
      /* This should never happen since all the states are covered above */
      WRRC_MSG1_HIGH("Event is forwarded to PG1 in an invalid State: %d",
                rrc_state);
      break;
   }  
} /* rrcpg1_procedure_event_handler */
/*===========================================================================

FUNCTION rrcpg1_init_procedure

DESCRIPTION
  This function initializes everything that the Paging Type 1 procedure
  needs to have initialized.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcpg1_init_procedure(void)
{
  /* Set the TMSI value invalid */
  rrc_tmsi_valid = FALSE;
  /* Set the PTMSI value invalid */
  rrc_ptmsi_valid = FALSE;
  /* Set the IMSI value invalid */
  rrc_imsi_valid = FALSE;
  /* Reset the no of pages count */
  rrcpg1_num_pages = 0; 
  /* Reset the count for no of pages that are matched */       
  rrcpg1_num_pages_matched = 0; 
  /* Set the timer status to FALSE */
  rrcpg1_bcch_mod_timer_status = FALSE;
  /*Init BCCH Mod info db here*/
  bcch_mod_info.bcch_mod_info_db_valid = FALSE;
  bcch_mod_info.bcch_mod_mib_v_tag = RRCPG1_INVALID_MIB_V_TAG;
  bcch_mod_info.bcch_mod_timer_value = RRCPG1_INVALID_BCCH_MOD_TIMER_VAL;
} /* rrcpg1_init_procedure */

/*===========================================================================

FUNCTION rrcpg1_post_update_sibs_cmd

DESCRIPTION
  This function is used to post UPDATE_SIBS_CMD
  to SIB procedure

DEPENDENCIES
  This function takes MIB Value tag from BCCH ModInfo DB.
  So it should have valid values before calling this function.
  rrcpg1_set_bcch_mod_info_db() should be called once 
  before calling this function.

RETURN VALUE
  None.

===========================================================================*/
void rrcpg1_post_update_sibs_cmd
(
  void
)
{
  rrc_cmd_type *rrc_cmd_ptr;
  if((bcch_mod_info.bcch_mod_info_db_valid == TRUE)&&
     (rrc_get_state() != RRC_STATE_CELL_DCH)&&
     (rrcllc_get_ordered_config_status() != OC_SET_FOR_CELL_DCH)&&
     ((rrc_get_serving_cell_mib_value_tag()) != (uint32)
              bcch_mod_info.bcch_mod_mib_v_tag)
    )
  {
    /* Build RRC_UPDATE_SIBS_IND command to post to SIB */
    /* Allocates the buffer to RRC Internal command */
    if( (rrc_cmd_ptr =  rrc_get_int_cmd_buf()) != NULL)
    {     
      /* Fill in the command parameters for RRC_UPDATE_SIBS_IND */
      rrc_cmd_ptr->cmd_hdr.cmd_id = RRC_UPDATE_SIBS_IND;
       
      rrc_cmd_ptr->cmd.update_sibs_ind.mib_value_tag = 
      bcch_mod_info.bcch_mod_mib_v_tag;
      
      rrc_cmd_ptr->cmd.update_sibs_ind.procedure = RRC_PROCEDURE_PG1;
      rrc_cmd_ptr->cmd.update_sibs_ind.cause = RRCSIB_REAL_SIB_MOD;
      rrc_cmd_ptr->cmd.update_sibs_ind.real_sib_mod = TRUE;
      rrc_put_int_cmd(rrc_cmd_ptr);
    } 
  }
  /*As we are posting Update_SIBs_Ind to SIB, 
   so invalidate the BCCH Mod info db.*/
  rrcpg1_check_and_abort_bcch_mod_info_processing();
}
/*===========================================================================
FUNCTION rrcpg1_check_for_duplicate_bcch_mod_info

DESCRIPTION
This function checks for duplicate received BCCH Mod Info I.E.
If there is BCCH Mod Info I.E in process & the newly received 
I.E has the same parameter as the one under process then this 
function will Indicate duplicate. Other wise it will return appropriate 
status..

DEPENDENCIES
  None.

RETURN VALUE
  RRCPG1_BCCH_MOD_INFO_NOT_IN_PROCESS = Currently BCCH ModInfo i.e is not processed. 
  RRCPG1_BCCH_MOD_INFO_DUPLICATE =  Currently BCCH ModInfo i.e is being processed & received I.E
                                    is duplicate of the one in progress.
  RRCPG1_BCCH_MOD_INFO_IN_PROCESS =  Currently BCCH ModInfo i.e is being processed.
===========================================================================*/
rrcpg1_bcch_mod_info_status_e_type rrcpg1_check_for_duplicate_bcch_mod_info
(
  uint8 mib_v_tag,  /* MIB value tag in rexd OTA message*/
  uint16 bcch_mod_timer_val  /* BCCH modification timer value*/
)
{
  rrcpg1_bcch_mod_info_status_e_type status = RRCPG1_BCCH_MOD_INFO_NOT_IN_PROCESS;
  /*Check to see if we already in process of handling one BCCH Mod info I.E.
    If no then return then we need to return status as FALSE*/
  if(bcch_mod_info.bcch_mod_info_db_valid  ==   FALSE)
  {
    status = RRCPG1_BCCH_MOD_INFO_NOT_IN_PROCESS;
  }
  else
  {
    /*We are processing one BCCH Mod Info, So lets check whether
       the newly received BCCH Mod Info I.E is duplicate*/
    if((bcch_mod_info.bcch_mod_mib_v_tag == mib_v_tag)&&
       (bcch_mod_info.bcch_mod_timer_value == bcch_mod_timer_val)
      )
    {
      WRRC_MSG2_HIGH("Duplicate BCCHModInfo rexd,Stored MIBVtag=%d,TimerVal=%d",
                                                                          bcch_mod_info.bcch_mod_mib_v_tag,
                                                                          bcch_mod_info.bcch_mod_timer_value);
      status = RRCPG1_BCCH_MOD_INFO_DUPLICATE;
    }
    else if(rrctmr_get_remaining_time(RRC_BCCH_MODIFICATION_TIMER) != (uint64)0)
    {
      status = RRCPG1_BCCH_MOD_INFO_IN_PROCESS;
    }
    else
    {
      status = RRCPG1_BCCH_MOD_INFO_NOT_IN_PROCESS;
      bcch_mod_info.bcch_mod_info_db_valid = FALSE;
    }
  }
  return status;
}
/*===========================================================================
FUNCTION rrcpg1_process_bcch_mod_info()

DESCRIPTION
This function will process BCCH ModInfo I.E.
It will take care of starting timer based on current
SFN & the SFN where BCCH modifiction will happen.

This will also update BCCH ModInfo DB with the 
MIB value tag & BCCH Mod timer received in OTA
message.

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcpg1_process_bcch_mod_info
(
  uint8 mib_v_tag,  /* MIB value tag in rexd OTA message*/
  uint16 bcch_mod_timer  /* BCCH modification timer value*/
)
{
  /*BCCH Modification timer value*/
  uint32 bcch_mod_timer_val;
  /*BCCH Modification time interms of SFN value*/
  uint32 bcch_mod_sfn;
  /*Current SFN value*/
  uint32 current_sfn;
  /*BCCH Modification time present, Save this command 
    & start BCCH Modification timer */

  if(rrcpg1_set_bcch_mod_info_db(
                     mib_v_tag,
                     bcch_mod_timer) == TRUE)
  {
  
    /* Actual value BCCH-ModificationTime = IE value * 8 */
    bcch_mod_sfn = ((uint32)bcch_mod_info.bcch_mod_timer_value) << 3;
    current_sfn = (uint32)seq_get_sfn();
    bcch_mod_timer_val = (bcch_mod_sfn > current_sfn)?
                         (bcch_mod_sfn - current_sfn):((4095 -current_sfn)+bcch_mod_sfn);
  
    MSG_HIGH("Current SFN=%d,bcch_mod_sfn=%d,bcch timer val=%d",
                                                                current_sfn,
                                                                bcch_mod_sfn,
                                                                bcch_mod_timer_val);
    if(bcch_mod_timer_val > RRCPG1_MAX_TIME_TO_SET_UP_PCCPCH)
    {
      bcch_mod_timer_val -= RRCPG1_MAX_TIME_TO_SET_UP_PCCPCH;
      /*Since SFN is in terms of 10msec, so multiply bcch_mod_timer_val by 10*/
      bcch_mod_timer_val *= RRCPG1_VALUE_OF_ONE_FRAME;
      rrctmr_start_timer(RRC_BCCH_MODIFICATION_TIMER, 
                                                      bcch_mod_timer_val);
    }
    else
    {
      /*Post UPDATE_SIBS_CMD to SIB */
      rrcpg1_post_update_sibs_cmd();
    }
  }
}
/*===========================================================================
FUNCTION rrcpg1_check_and_abort_bcch_mod_info_processing()

DESCRIPTION
This function will check & abort processing of BCCH
ModInfo & also it takes care of stopping this timer 
RRC_BCCH_MODIFICATION_TIMER if its running. 

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcpg1_check_and_abort_bcch_mod_info_processing
(
void
)
{
  boolean abort_status = FALSE;
  /*Check & clean up bcch mod info db*/
  if(bcch_mod_info.bcch_mod_info_db_valid != FALSE)
  {
    bcch_mod_info.bcch_mod_info_db_valid = FALSE;
    bcch_mod_info.bcch_mod_mib_v_tag = RRCPG1_INVALID_MIB_V_TAG;
    bcch_mod_info.bcch_mod_timer_value = RRCPG1_INVALID_BCCH_MOD_TIMER_VAL;
    abort_status = TRUE;
  }
  /*Check to see if BCCH Mod timer is running.
    If so then stop timer right here*/
  if(rrctmr_get_remaining_time(RRC_BCCH_MODIFICATION_TIMER) != (uint64)0)
  {
    rrctmr_stop_timer(RRC_BCCH_MODIFICATION_TIMER);
    abort_status = TRUE;
  }
  if(abort_status == TRUE)
  {
    WRRC_MSG0_HIGH("Clearing BCCH ModInfo which was in progress");
  }
}
/*===========================================================================
FUNCTION rrcpg1_set_bcch_mod_info_db()

DESCRIPTION
This function will validate the input parameters & then update
bcch ModInfo db accordingly.

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
boolean rrcpg1_set_bcch_mod_info_db
(
  uint8 mib_v_tag,  /* MIB value tag in rexd OTA message*/
  uint16 bcch_mod_timer  /* BCCH modification timer value*/
)
{
  boolean status = FALSE;

  /*Validate & then update MIBVtag into bcch ModInfo db.
    MIB Value tag ranges from 1 to 8 only*/
  if((mib_v_tag < RRCPG1_INVALID_MIB_V_TAG)&&
     (mib_v_tag != 0))
  {
    bcch_mod_info.bcch_mod_mib_v_tag  =  mib_v_tag;
    status = TRUE;
  }
  
  /*Validate & then update BCCH Mod Timer Value into bcch ModInfo db*/
  if(bcch_mod_timer < RRCPG1_INVALID_BCCH_MOD_TIMER_VAL)
  {
    bcch_mod_info.bcch_mod_timer_value = bcch_mod_timer;
    status = TRUE;
  }
  
  /*If either MIBVtag or BCCH Mod Timer Value is updated in 
    bcch ModInfo db.Only then set  bcch ModInfo db as valid.*/

  MSG_HIGH("BCCH ModInfo DB is status %d with MibVtag=%d,BcchTimer=%d",
                  status,
                  mib_v_tag,
                  bcch_mod_timer);
  if(status == TRUE)
  {
    bcch_mod_info.bcch_mod_info_db_valid = TRUE;
  }
  return status;
}
/*===========================================================================
FUNCTION rrcpg1_process_bcch_mod_info_msg_ie()

DESCRIPTION
This function will process BCCHModInfo I.E received in 
OTA message.

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcpg1_process_bcch_mod_info_msg_ie
(
  uint8 mib_v_tag,  /* MIB value tag in rexd OTA message*/
  uint16 bcch_mod_time,  /* BCCH modification timer value*/
  boolean bcch_mod_time_present  /*Boolean to indicate whether BCCH Time present in BCCHModInfo i.e*/
)
{
  if(bcch_mod_time_present == TRUE)
  {
    rrcpg1_bcch_mod_info_status_e_type bcch_mod_info_status;
    WRRC_MSG0_HIGH("BCCH ModInfo I.E has BCCHModTime value in it");
    bcch_mod_info_status = rrcpg1_check_for_duplicate_bcch_mod_info(
                                                                     mib_v_tag,
                                                                     bcch_mod_time
                                                                   );
    if(bcch_mod_info_status == RRCPG1_BCCH_MOD_INFO_NOT_IN_PROCESS)
    {
      rrcpg1_process_bcch_mod_info(
                                    mib_v_tag,
                                    bcch_mod_time
                                   );
    }
    else if(bcch_mod_info_status == RRCPG1_BCCH_MOD_INFO_IN_PROCESS)
    {
      rrcpg1_check_and_abort_bcch_mod_info_processing();
      rrcpg1_process_bcch_mod_info(
                                    mib_v_tag,
                                    bcch_mod_time
                                   );
    }
    else
    {
      /*This is a Duplicate BCCH ModInfo, so lets ignore it, no need 
        of a debug message here as we have one in function
        rrcpg1_check_for_duplicate_bcch_mod_info()*/
    }
  }
  else
  {
    /*No BCCH modification time present, 
      So post UPDATE_SIBS_CMD to SIB */
    boolean status =FALSE;
    /*Make sure that MIB Value Tag is populated properly
      as rrcpg1_post_update_sibs_cmd() picks Mib Vtag 
      from BCCHModInfo db*/
    status = rrcpg1_set_bcch_mod_info_db(
                                         mib_v_tag,
                                         bcch_mod_time
                                        );
    if(status == TRUE)
    {
      rrcpg1_post_update_sibs_cmd();
    }
  }
}
/*===========================================================================
FUNCTION rrcpg1_return_bcch_mod_info_status()

DESCRIPTION
This function will return Bcch mod info
status 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE = If BCCH ModIfo in progress
  FALSE = If BCCH ModIfo is not in progress
===========================================================================*/
boolean rrcpg1_return_bcch_mod_info_status
(
void
)
{
  boolean status = FALSE;
  /*Check & clean up bcch mod info db*/
  if(bcch_mod_info.bcch_mod_info_db_valid == TRUE)
  {
    status = TRUE;
  }
  return status;
}
/*===========================================================================

FUNCTION rrcpg1_get_num_pages

DESCRIPTION
  This function returns the value of rrcpg1_num_pages.  This indicates
  the total number of pages received [false alarm + valid pages].
  This function will be called to display counter in QXDM

DEPENDENCIES
  None.

RETURN VALUE
  uint32.

===========================================================================*/
uint32 rrcpg1_get_num_pages(void)
{
  return rrcpg1_num_pages;
}
/*===========================================================================

FUNCTION rrcpg1_get_num_pages_matched

DESCRIPTION
  This function returns the value of rrcpg1_num_pages_matched.  This indicates
  the total number of pages minus the false alarms.
  This function will be called to display counter in QXDM

DEPENDENCIES
  None.

RETURN VALUE
  uint32.

===========================================================================*/
uint32 rrcpg1_get_num_pages_matched(void)
{
  return rrcpg1_num_pages_matched;
}
