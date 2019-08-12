/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   C D M A    C S F B    S T A T E

GENERAL DESCRIPTION
  This module contains the functions that support LTE-1X CSFB Tunnel Operation.
  This file contains the lowest level state machine for the CDMA-CSFB operation.
  While being in CSFB state, Mobile shall operate in CSFB-IDLE,CSFB_ACCESS states.
  In CSFB-ACCESS state, CP shall tunnel GCSNA messages as appropriate.

EXTERNALIZED FUNCTIONS
  TO DO

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The Main Control task calls the mcc_subtask procedure,with entry reason of CSFB,
  after it has been CSFB activated by MMOC .

  Copyright (c) 2010 - 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mcccsfb.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/21/15   sjo     Support for FR-29020: Power Saving Mode w/o NW coordination
08/07/15   ppr     UHDM Call:Chgs to clear RXTX queue except UHDM msg
03/04/15   ppr     FR25498:1xCSFB specific call end reasons
02/03/15   ppr     FR#24681:1x-CP WLAN coexistence feature changes
11/11/14   ppr     Changes to handle n/w Err REDIR to HDR after MT CSFB ESR
10/29/14   ppr     Logging gcsna_option_inuse in CSFB init()
09/29/14   agh     Fix the GCSNA Option used for SRVCC
06/20/14   ppr     Added SRVCC_CALL_END cmd support
05/06/14   gga     1X_CP F3 redistribution
03/19/14   ssh     Fix to perform the Timer Based registration in SRLTE mode.
03/19/14   ppr     SRVCC Feature Changes
03/14/14   gga     Fixed the bugs which came due to R8 CSFB Call setup 
                   optimization
03/03/14   agh     Reset call_orig and is_emergency in csfb init, idle states
02/27/14   gga     CSFB: Changes to send RC support according to 1xA in the 
                   subsequent call
01/27/14   gga     Added support to move to CSFB_IDLE if no 1x Release Order 
                   is received for MT SO76 SMS.
01/08/14   agh     FR 15489: UIM Clients to Handle SIM BUSY Indications
10/09/13   pap     Removing warnings for LLVM.
07/29/13   eye     Switch to secure random number generation api.
06/20/13   ssh     Cleared call release reason before entering the CSFB mode.
05/27/13   ssh     Added support to abort MO SMS while C2K params timeout.
05/21/13   ppr     Avoid sending the c2k param request twice if MC is waiting 
                   for SIB8 info to do any CSFB registration.
05/06/13   cjb     Resolve compiler warnings
05/06/13   gga     Dynamic MSGR memory allocation changes 
01/07/13   gga     Correctly initialized the msg_seq from 0-63 instead of 
                   0-9.
11/19/12   ssh     Added SMS over CSFB support.
10/08/12   ssh     Resync Auth after quiting the Access state.
09/15/12   ppr     FR#2370 - 1x memory optimization changes
09/13/12   jtm     Compiler warning clean up.
08/31/12   ssh     Fixed compiler warning.
08/20/12   vks     Fix compiler warnings.
07/11/12   jj      CSFB: Changes to send tunneled Release Order when there is
                   call end before Hanodover is complete.
07/03/12   ssh     1xCSFB: Added support for doing a hard failure for GCSNA
                   rejects with cause 3,4,5.
06/29/12   ssh     CSFB: added check for the powerdown reg in the C2K resp
                   while doing the powerdown reg.
06/14/12   ssh     CSFB: added support for duplicate detection.
04/26/12   ssh     CSFB: retry mech optimization.
04/06/12   ssh     CSFB: enabled 1xA on CSFB
04/04/12   trc     Convert to m1x_time module
03/13/12   jj      CSFB: CSFB NV Items support.
03/12/12   ssh     Guarded the PWR DWN reg with a timer.
03/07/12   ssh     Corrected a typo in timer name.
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
12/21/11   ppr     Memory Optimization changes for migrating CP's cmd queue
                   from static to a dynamic memory
11/02/11   jj      CW cleanup.
09/13/11   jj      CSFB: Support for Hard/Soft failures for Reg.
08/26/11   jj      CSFB: Support for GCSNA negotiation.
07/22/11   jj      CSFB: Support for slotted wakeup in CSFB mode.
07/21/11   jj      CSFB: Added support for C2K Params receipt timeout.
08/02/11   jtm     Fixed compiler error.
06/28/11   jtm     Moved local variable declarations.
06/28/11   jj      CSFB: Skipping PSIST check for Ordered registrations.
06/21/11   jj      CSFB: Removed the condition to declare pre-reg failure if
                   there is context loss and forced power-up reg. or any reg
                   cannot be triggered immediately.
05/17/11   jj      CSFB: Functionality to support random GCSNA seq num.
05/16/11   jj      CSFB: Added changes to tunnel Reject Order if there are any
                   unsupported messages as per GCSNA Option.
05/16/11   jj      CSFB: Added support to send GCSNA ack for incoming 1x
                   CRKT SRV message.
05/16/11   jj      CSFB: Added support to drop rxed msgs incase previous
                   access is in progress.
05/09/11   jj      CSFB: Fix to not reset L3 message status if there was a
                   prior context loss.
05/05/11   jj      CSFB: Fix to not sent pre-reg failure repeatedly in case
                   of Pre-Registration Failure.
05/05/11   jj      CSFB: Changes to fail pre-registration if Forced Reg can
                   not be performed.
05/05/11   jj      CSFB: Fix for performing PSIST check before sending 1X
                   param request for Registration.
05/03/11   jj      CSFB: Support for deactivating lower layers if there is a
                   HO failure.
05/02/11   jj      CSFB: Added support for handling GCSNA ack for UL messages.
05/02/11   jj      CSFB: Fixed issue with checking c2k params status for
                   MO/MT calls.
04/29/11   jj      CSFB: Added support for error scenarios - phase2.
04/28/11   jj      CSFB: Error handling phase1.
04/21/11   jj      CSFB: Added changes to support SSD operation in CSFB mode.
04/05/11   jj      CSFB: Debug addition to print PSIST values.
03/24/11   jj      CSFB: Added changes to handle some of the error scenarios
                   gracefully.
03/17/11   jj      CSFB: Added gaurd check to clear prior call context (if any)
                   upon entering CSFB IDLE state.
03/17/11   jj      CSFB: PSIST check refinements for MO call.
03/16/11   jj      CSFB: Compile errors fix.
03/16/11   jj      CSFB: Changes to support PSIST operation in tunnel mode.
02/28/11   jj      CSFB: Support for forced registration in case of context loss.
12/22/10   ssh     Merged MSGR queue signal with CMD_Q_SIG. And reduced the
                   GCSNA txn buffer to be one.
02/24/11   jj      Fixed changes to avoid multiple registrations.
02/24/11   jj      Fixed the issue of performing reg with invalid registration
                   type.
02/15/11   jj      Fixed compile and lint warnings.
02/11/11   jj      CSFB: Added changes not to trigger power-up registration
                   for Wildclass bandclass.
01/27/11   jj      CSFB: Added changes to support power-up reg because of
                   band-class change when there is toggle between 1x -> LTE
                   modes.
01/12/11   jj      CSFB: Changes done to remove unused GCSNA sig.
01/12/11   jj      CSFB: Added changes to support sending of ORM in
                   ULHandoverPreparationTransfer.
12/23/10   jj      CSFB: Added a check to clear signal at MC for triggering
                   timer based reg when we enter Access State.
12/20/10   jj      CSFB: Added case to support new CSFB exit state.
12/15/10   jj      CSFB: Added support for power-down reg.
12/14/10   ssh     CSFB: Added MT support contd. (GPM processing and
                   sending PRM support.)
11/19/10   ssh     CSFB: corrected the malformed ORM.
11/17/10   jj      CSFB:Added support for handling MO from EUTRA
11/16/10   jj      CSFB: Added more debugs.
11/15/10   ssh     CSFB: added MO call Support.
11/11/10   jj      CSFB redesign
11/03/10   jj      CSFB: Updated to clear signal for periodic registrations.
10/19/10   jj      CSFB: Added support for IRAT switch in CSFB mode.
09/29/10   ssh     Corrected the sequence of clearing the signal and calling
                   the retry function.
09/24/10   ssh     CSFB: Added retry and retransmission support.
09/23/10   jj      CSFB:Added support for Reg Context update.
09/01/10   jj      Support to send RGM.
08/25/10   ssh     Renamed the SIB8 msg.
08/18/10   jj      CSFB: Added state level implementation.
08/04/10   jj      Added support to Registration checking in CSFB Idle state.
08/04/10   jj      Added MSGR Sig mask to support MCC substates
07/29/10   jj      Update state machine for CSFB operation.


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "msg.h"
#include "m1x_diag.h"
#include "err.h"
#include "rex.h"
#include "dog.h"
#include "queue.h"
#include "ran.h"
#include "mcccsfb.h"
#include "mcccsfbsup.h"
#include "mccreg_v.h"
#include "mccreg_i.h"
#include "mccgcsna.h"
#include "mccsyobj_v.h"
#include "db.h"
#include "snm_v.h"
#include "snm_i.h"
#include "mc_i.h"
#include "mc_v.h"
#include "modem_1x_defs.h"
#include "sd_v.h"

/* FR24681:WLAN Coexistence Feature, 
 * For Feature Header defn and Interface changes  */
#include "wcn_coex_mgr.h"

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* ------------------------------------------------------------------------ */
/*                    CSFB State Level Variables                            */
/* ------------------------------------------------------------------------ */

/* Variable to keep track of reason for entering MCC_CSFB_ACCESS state */
mcc_csfb_access_rsn_type mcc_csfb_access_reason = MCC_CSFB_ACCESS_NONE;

dword mcc_csfb_psist;

/* Variable to keep track of if PSIST check for RGM has been performed */
boolean mcc_csfb_is_reg_psist_chk_done = FALSE;

/* Variable to track if we are waiting on a DL GCSNA ack for the sent 1X
 * CRKT SRV MSG.
 */
boolean mcc_csfb_is_dl_gcsna_ack_pending = FALSE;

/************************************************************************/
/*                       Other variables                                */
/************************************************************************/

/************************************************************************/
/*            Function prototypes                */
/************************************************************************/
/*===========================================================================
FUNCTION MCC_CSFB_SEND_GCSNA_MSG
===========================================================================*/
void mcc_csfb_send_gcsna_msg(void);

/*========================================================================
FUNCTION CDMA_CSFB_STATE

DESCRIPTION
  This procedure performs CSFB sub State machine operations.
  The two main sub-states while being in  CSFB mode are,
  CSFB_IDLE and CSFB_ACCESS states.

  For any transmission over Tunnel, CSFB ACCESS state will be
  entered, otherwise CP will remain in CSFB_IDLE state.

DEPENDENCIES
  None

RETURN VALUE
  Returns the next state within the CDMA state machine to be processed.

SIDE EFFECTS
  None.
===========================================================================*/

word cdma_csfb_state( void )
{
  /* Boolean to store SRVCC Mode */
  boolean srvcc_mode = mcc_csfb_get_srvcc_mode();
  
  #ifdef FEATURE_COEX_RPT_RAT_L3_STATE
  cxm_tech_l3_state_s cxm_cdma;
    /* Str to inform CXM about MC operating mode and state */ 
  #endif /* FEATURE_COEX_RPT_RAT_L3_STATE */
  
  /* ---------------------------------------------------------------------*/
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG : Enter CSFB state machine, current state:%d, Srvcc Mode:%d",
    cdma.curr_state, srvcc_mode );

  /* This is the state machine of CSFB. We will enter into CSFB state upon
   * CM activating us in CSFB mode . As soon as we enter into CSFB mode, we
   * operate in CDMA_CSFB_IDLE state.
   *
   * While being in CSFB mode, we monitor only CMDQ/MSGR. Upon getting valid
   * triggers form CMD Q/ MSGR we move to Access state to perform tunnel
   * access procedures.
   *
   * We exit CSFB mode if CM deactivates us/IRAT switch triggers(Redir/Resel)
   * /Call/Page related triggers which would make us move to legacy mode.
   */
  /* ---------------------------------------------------------------------*/

  /* Enter CSFB mode,do init Procedures */
  mcc_csfb_state_init();

  /* Fetch the C2K Params inorder to perform power-up reg because of band change
   * when we move across native 1x -> LTE mode.
   * Perform power-up registration only when there is no other reg readily available
   * to be done.
   * If there is a band class change from previous stored value to the one currently obtained,
   * we will perform power-up reg because of bandclass change.
   * Make sure that we are not in call ORM/PRM state before performing this check.
   * This is a possiblity when we come from CDMA traffic / idle state because of call
   * failure.
   */
  if( (cdma.curr_state != CDMA_CSFB_EXIT) &&
      (mcc_csfb_sib8_msg.reg_params.powerUpReg == TRUE) &&
      (mccreg_vars.reg_enabled) &&
      (mcc_csfb_cntx_loss_ind == MCC_CSFB_CNTX_LOSS_NONE) &&
      ((cdma.reg_type = mcc_csfb_reg_idle_check()) == MCCREG_NO_REG_REQUIRED) &&
      /* Avoid sending C2K params request msg if we wait for SIB8 
       * info to do any CSFB registration. Any way we will send the 
       * C2K params request msg once we receive the SIB8 info */
      (mcc_csfb_wait_for_sib8_before_doing_reg == FALSE)
#ifdef FEATURE_MODEM_1X_SRVCC     
      /* Avoid sending C2K params request msg if MC activated in 
       * SRVCC mode, no power-up registration required in SRVCC mode */
      && (srvcc_mode == FALSE)
#endif /* FEATURE_MODEM_1X_SRVCC */
    )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: Updating C2K params to check for power-up reg");
    /* Forcebly change the state to REG Substate, and check if we
     * have to perform any registration.
     */
    mcc_csfb_access_reason = MCC_CSFB_ACCESS_REG;
    cdma.curr_state = CDMA_CSFB_ACCESS;

    /* Set a flag to indicate that PSIST check has not been done */
    mcc_csfb_is_reg_psist_chk_done = FALSE;
  }

  /* During SRLTE mode, we might have received the SIB8 which resulted into 
   * CSFB mode activation. As SIB8 neighbors are not updated to SD and 
   * similarly the reg variable can't be updated while native 1X is 
   * operational. Process all the defered processing of the SIB8 now. */
  if(cdma.is_sib8_proc_deferred) 
  {
    /* Variable to report SIB8 neighbors to SD */
    sd_cdma_sib8_nbr_cell_info_list_s mcc_sib8_nbr_list;


    /* Process the registration related parts received in SIB8
    ** Irrespective of whether CSFB availble or unavailable, we shall always update
    ** registration context as per latest SIB8/C2K Param received.
    */
    mcc_csfb_update_reg_var();


    /* Update neighbors received in SIB8 to SD, irrespective of
     * presence of pre-reg params
     */
    mcc_csfb_fill_sib8_nbrs(&mcc_sib8_nbr_list);
    sd_ss_ind_update_cdma_sib8_nbrs(&mcc_sib8_nbr_list);
  
    /* Update SID & NID */
    mccsyobj_update_epzid((mcc_csfb_sib8_msg.sib8_present && mcc_csfb_sib8_msg.reg_params_present), mcc_csfb_sib8_msg.reg_params.sid,
                           (mcc_csfb_sib8_msg.sib8_present && mcc_csfb_sib8_msg.reg_params_present), mcc_csfb_sib8_msg.reg_params.nid,
                           FALSE, 0,
                           CDMA_CSFB);

    cdma.is_sib8_proc_deferred = FALSE;
  }
  /* Enter into CSFB state loop */
  while(MAIN_STATE(cdma.curr_state) == CDMA_CSFB)
  {
    switch(cdma.curr_state)
    {
      case CDMA_CSFB_IDLE:
      {
        M1X_MSG( DCP, LEGACY_HIGH,
           "CSFB DEBUG: Entering CDMA_CSFB_IDLE state, srvcc_mode:%d",
                 srvcc_mode );
        
        #ifdef FEATURE_COEX_RPT_RAT_L3_STATE
        /* Inform MC State change to MCS CXM for WLAN coexistence */
        cxm_cdma.mode = CXM_TECH_ACTIVE;
        cxm_cdma.state = CXM_TECH_L3_CSFB_IDLE;
        (void) cxm_set_tech_l3_state(CXM_TECH_ONEX, &cxm_cdma);
        M1X_MSG( DCP, LEGACY_HIGH,
        "CSFB state chg to MCS CXM,tech:%d,mode:%d,state:%d,cdma.cur_state:%d",
                 CXM_TECH_ONEX, 
                 cxm_cdma.mode, 
                 cxm_cdma.state,
                 cdma.curr_state );
        #endif /* FEATURE_COEX_RPT_RAT_L3_STATE */ 
        
        cdma_csfb_idle_state();    /* Process the idle state */
        break;
      } /* CDMA_CSFB_IDLE */

      case CDMA_CSFB_ACCESS:
      {
        M1X_MSG( DCP, LEGACY_HIGH,
           "CSFB DEBUG: Entering CDMA_CSFB_ACCESS state, srvcc_mode:%d",
                 srvcc_mode );
                 
        #ifdef FEATURE_COEX_RPT_RAT_L3_STATE
        /* Inform MC State change to MCS CXM for WLAN coexistence */
        cxm_cdma.mode = CXM_TECH_ACTIVE;
        cxm_cdma.state = CXM_TECH_L3_CSFB_ACCESS;
        (void) cxm_set_tech_l3_state(CXM_TECH_ONEX, &cxm_cdma);
        M1X_MSG( DCP, LEGACY_HIGH,
        "CSFB state chg to MCS CXM,tech:%d,mode:%d,state:%d,cdma.cur_state:%d",
                 CXM_TECH_ONEX, 
                 cxm_cdma.mode, 
                 cxm_cdma.state,
                 cdma.curr_state );
        #endif /* FEATURE_COEX_RPT_RAT_L3_STATE */
        
        cdma_csfb_access_state();  /* Process the Access state */
        break;
      } /* CDMA_CSFB_ACCESS */

      case CDMA_CSFB_EXIT:
      {
        M1X_MSG( DCP, LEGACY_HIGH,
           "CSFB DEBUG: entering CDMA_CSFB_EXIT state, srvcc_mode:%d",
                 srvcc_mode );
                 
        #ifdef FEATURE_COEX_RPT_RAT_L3_STATE
        /* Inform MC State change to MCS CXM for WLAN coexistence */
        cxm_cdma.mode = CXM_TECH_INACTIVE;
        cxm_cdma.state = CXM_TECH_L3_INACTIVE;
        (void) cxm_set_tech_l3_state(CXM_TECH_ONEX, &cxm_cdma);
        M1X_MSG( DCP, LEGACY_HIGH,
        "CSFB state chg to MCS CXM,tech:%d,mode:%d,state:%d,cdma.cur_state:%d",
                 CXM_TECH_ONEX, 
                 cxm_cdma.mode, 
                 cxm_cdma.state,
                 cdma.curr_state );
        #endif /* FEATURE_COEX_RPT_RAT_L3_STATE */
        
        /* Take necessary action based on exit reason */
        mcc_csfb_exit_reason_processing();
        break;
      } /* CDMA_CSFB_EXIT */

      default:
      {
        ERR_FATAL("DEBUG: Unknown CSFB state, CSFB busted ",0,0,0);
        break;
      }

    } /* switch */
  } /* while ( state == CDMA_CSFB ) */

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Exiting CSFB mode, next CDMA state = %d",
    cdma.curr_state);

  /* Out of CSFB mode, execute the CSFB's exclusive exit procedures */
  mcc_csfb_state_exit();

  return cdma.curr_state;
} /* cdma_csfb_state */

/*===========================================================================

FUNCTION MCC_CSFB_STATE_EXIT

DESCRIPTION
  This procedure performs init operations before entering into CSFB sub-states.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_csfb_state_exit( void )
{
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG:Execuing CSFB exit procedures");
  /* clear all pending/ongoing GCSNA transmissions */
  mcc_csfb_gcsna_stop_txn();

  mccreg_vars.counter_enabled = FALSE;

  /* Reset the timer for C2K Params timeout */
  (void) mcc_clr_callback_timer(&mcc_csfb_c2k_param_tmo_timer,
                                MCC_CSFB_C2K_PARAM_TMO_TIMER_ID);

  /* Reset the slotted mode timer */
  (void) mcc_clr_callback_timer(&mcc_csfb_slotted_mode_timer,
                                MCC_CSFB_SLOTTED_MODE_TIMER_ID);

  /* Reset the Power down registration gueard timer */
  (void) mcc_clr_callback_timer(&mcc_csfb_pwr_dwn_reg_guard_timer,
                                MCC_CSFB_PWR_DWN_REG_GUARD_TIMER_ID);

  /* Reset the SO76 SMS access gueard timer */
  (void) mcc_clr_callback_timer(&mcc_csfb_so76_sms_access_guard_timer,
                                MCC_CSFB_SO76_SMS_ACCESS_GUARD_TIMER_ID);

  /* Reset the timer for SRVCC UHDM timeout */
  (void) mcc_clr_callback_timer(&mcc_srvcc_uhdm_tmo_timer,
                                MCC_SRVCC_UHDM_TMO_TIMER_ID);                               

  /* Reset the value of number of C2K Params retry */
  csfb_cdma.max_c2k_params_attempt = 0;

  /* We received CNF for the message , set the soft failure count to Zero */
  csfb_cdma.reg_retry_cnt = 0;

  /* Check if we are moving out CSFB as part of UHDM call 
   * If yes then clear RXTX queue except UHDM msg, 
   * for the scenarios where n/w wrongly sending some stale Ack/Rel ord 
   * and that stale msg in RxTx queue causing UHDM call failure 
   * in native 1x */
  if(cdma.curr_state == CDMA_TRAFFIC)
  {
    mccrxtx_cmd_type *rxtx_ptr;     
    /* Pointer to received message */
    mccrxtx_cmd_type mcc_rxtx_buf;     
    /* Temp buf to hold received message */
             
    rxtx_ptr = (mccrxtx_cmd_type*) q_check(&mcc_rxtx_q);
    
    while (rxtx_ptr != NULL)
    {
      if( (rxtx_ptr->hdr.command == MCCRXTX_MSG_F ) &&
          (rxtx_ptr->msg.msg.gen.msg_type != CAI_EXT_HO_DIR_MSG) &&
          (rxtx_ptr->msg.msg.gen.msg_type != CAI_GEN_HO_DIR_MSG) &&
          (rxtx_ptr->msg.msg.gen.msg_type != CAI_UNIV_HO_DIR_MSG) &&
          (rxtx_ptr->msg.msg.gen.msg_type != CAI_MEID_UNIV_HO_DIR_MSG)
        )
      {
              M1X_MSG( DCP, LEGACY_HIGH,
                       "DEBUG:Not UHDM msg so deleting it from RxTx queue" );
        /* Save this node first, later on this node will be used to continue
         * the search through the rxtx queue */
        mcc_rxtx_buf = *rxtx_ptr;
       
        /* Delete this node from the mcc_rxtx_q queue first */
        q_delete(&mcc_rxtx_q, &rxtx_ptr->hdr.cmd_hdr.link);

        /* Following function will set signal if requested */
        cmd_done( &rxtx_ptr->hdr.cmd_hdr );

        #ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( rxtx_ptr, MC_MCC_RXTX_Q );
        rxtx_ptr = NULL;
        #endif /* FEATURE_1X_CP_MEM_OPT */
        
        /* Continue the search, starting from the next node after this node */
        rxtx_ptr = (mccrxtx_cmd_type *)q_next(&mcc_rxtx_q,
                                            &mcc_rxtx_buf.hdr.cmd_hdr.link);
      } /* (Not UHDM msg) */
      else
      {
        /* Go to next queue item */
        rxtx_ptr = (mccrxtx_cmd_type *)q_next(&mcc_rxtx_q,
                                            &rxtx_ptr->hdr.cmd_hdr.link);
      } /* (UHDM msg) */
    } /* while (rxtx_ptr != NULL) */  
  
  } /* (curr_state == CDMA_TRAFFIC) */
  
} /* mcc_csfb_state_exit */

/*===========================================================================

FUNCTION MCC_CSFB_STATE_INIT

DESCRIPTION
  This procedure performs init operations before entering into CSFB sub-states.
  The init operation include intializing of GCSNA variables / reg related variables.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_csfb_state_init( void )
{
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Doing CSFB state initialization..");

  /* ---------------------------------------------------------------------*/
  /* As part of CSFB initialization procedures, intialize all GCSNA timers,
   * CSFB exclusive variable initialization, reg related inits etc.
   * These intializations are done whenever CSFB mode is entered because of
   * CM activating us in CSFB mode or we re-enter into CSFB mode because of
   * failure in switching to 1X mode.
   */

  /* ------------------ GCSNA Intializations -----------------*/
  /* Define timer for the transmission buffers (gcsna_msg_sent). */
  rex_def_timer( &gcsna_msg_sent.txn_timer, MC_TCB_PTR,
    MCC_CSFB_TXN_TIMER_SIG);

  /* Define timer for the c2k parameter timeout */
  rex_def_timer_ex(&mcc_csfb_c2k_param_tmo_timer,
                   mcc_expired_timer_callback_func,
                   MCC_CSFB_C2K_PARAM_TMO_TIMER_ID);

  /* Define timer for slotted mode operation in CSFB mode */
  rex_def_timer_ex(&mcc_csfb_slotted_mode_timer,
                   mcc_expired_timer_callback_func,
                   MCC_CSFB_SLOTTED_MODE_TIMER_ID);

  /* Define timer for the Powerdown Registration Guard timer */
  rex_def_timer_ex(&mcc_csfb_pwr_dwn_reg_guard_timer,
                   mcc_expired_timer_callback_func,
                   MCC_CSFB_PWR_DWN_REG_GUARD_TIMER_ID);

  /* Define timer for the SO76 SMS access Guard timer */
  rex_def_timer_ex(&mcc_csfb_so76_sms_access_guard_timer,
                   mcc_expired_timer_callback_func,
                   MCC_CSFB_SO76_SMS_ACCESS_GUARD_TIMER_ID);

  /* Define timer for the SRVCC UHDM timeout */
  rex_def_timer_ex(&mcc_srvcc_uhdm_tmo_timer,
                   mcc_expired_timer_callback_func,
                   MCC_SRVCC_UHDM_TMO_TIMER_ID);

  /* Set the timer type as no timer for now. */
  gcsna_msg_sent.timer_type = MCC_GCSNA_NO_TIMER;

  /* ------------------ Registartion Intialization -----------------*/
  /* Initialize CSFB State related variables, before entering into
  ** CSFB State m/c
  */
  mccreg_csfb_reg_init();

  /* ------------------ CSFB generic variable Reset -----------------*/
  /* Reset the unprocessed command buffer */
  memset(&csfb_unprocessed_cmd, 0, sizeof(csfb_unprocessed_cmd));

  /* Reset the call status */
  csfb_cdma.call_status = CSFB_CALL_PENDING_NONE;
  cdma.call_orig = FALSE;
  cdma.is_emergency = FALSE;

  /* Reset the psm_ready_ind_pending for Power Saving Mode */
  cdma.psm_ready_ind_pending = FALSE;

  /* Reset the access reason */
  mcc_csfb_access_reason = MCC_CSFB_ACCESS_NONE;

  /* Reset the call end reason. */
  cdma.end_reason = CAI_REL_NORMAL;

  /* Reset retain_mt_page_cxt flag */
  csfb_cdma.retain_mt_page_cxt = FALSE;

  #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
  /* Reset error reason returned by auth task */
  mc_set_auth_err_status(FALSE);
  #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */

  /* Reset 1x state transition reason */
 csfb_cdma.csfb_1x_transition = FALSE;

 #ifdef FEATURE_IS2000_1X_ADV
 cur_bs_ptr->csp.sp.rc_bitmap_capability_ext_allowed = cdma.onexadv_status.status;

 cdma.onexadv_status.rc_bitmap_capability_ext_allowed = cdma.onexadv_status.status;

M1X_MSG( DCP, LEGACY_MED,
   "RC bitmap and capability ext allowed %d",
   cdma.onexadv_status.rc_bitmap_capability_ext_allowed);
 #endif /* FEATURE_IS2000_1X_ADV */



  /* Reset 1x reg status to None if the previous Reg status was REG_STOP */
  /* If it is REG_STOP, we stopped Registration process till we received
   * CSFB deact, so its status is not retained CSFB deacts.Same is not true with
   * REG_FORCE, because we have to retain its status across CSFB deacts.
   */
  if(csfb_cdma.reg_status == MCC_CSFB_REG_STOP)
  {
    csfb_cdma.reg_status = MCC_CSFB_REG_NONE;
  }

#ifdef FEATURE_MODEM_1X_SRVCC
  /* SRVCC Mode */
  if(mcc_csfb_get_srvcc_mode() == TRUE)
  {
    /* For SRVCC Mode, we need to use GCSNA_OPTION_8 */
    gcsna_option_inuse = GCSNA_OPTION_8;
  } /* SRVCC Mode */
  else 
#endif /* FEATURE_MODEM_1X_SRVCC */
  { /* CSFB Mode */
    /* To start with ,initialize GCSNA Option to GCSNA_OPTION_1 */
    gcsna_option_inuse = GCSNA_OPTION_1;
  } /* CSFB Mode */

  /* Initialize random value for GCSNA seq number */
  gcsna_msg_seq = (mc_sec_ran_next())%64;

  csfb_cdma.using_rand_msg_seq = TRUE;

  M1X_MSG( DCP, LEGACY_MED,
           "DEBUG: Initializing gcsna_msg_seq:%d, gcsna_option_inuse:%d",
           gcsna_msg_seq,
           gcsna_option_inuse );

  /* The invalid values of the Global cell ID which can be received in the 
   * Redirection indication from LTE. This value is used to validate and 
   * differentiate whether the call is a redirection based CSFB call. It is
   * used while optimizing call setup time for Rel 8 calls */
  mc_redir_req_msg.global_cell_id = INVALID_GLOBAL_CELL_ID;

  /* The invalid number of the MNC digits which is received in PLMN ID through
   * Redirection indication from LTE.  This value is used to validate and 
   * differentiate whether the call is a redirection based CSFB call. It is
   * used while optimizing call setup time for Rel 8 calls */
  mc_redir_req_msg.redir_plmn.num_mnc_digits = INVALID_NUM_MNC_DIGITS;

} /* void mcc_csfb_state_init( void ) */

/*===========================================================================

FUNCTION CDMA_CSFB_IDLE

DESCRIPTION
  This procedure performs CSFB IDLE State operations and
  triggers to perform CSFB Tunnel Access Procedures upon
  hitting access conditions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void cdma_csfb_idle_state(void)
{

  /* Wait mask and sigs */
   dword wait_mask,sigs;

  /* Command from the MC_CMD_Q queue */
  static mc_msg_type *cmd_ptr;

  /* Command from MSGR_Q queuw */
  static mc_msgr_rcv_msg_type *msgr_cmd_ptr;

  /* Initialize wait mask and sigs */
  wait_mask = sigs = 0;

  /* Initialize CSFB exit reason */
  mcc_csfb_exit.exit = MCC_CSFB_EXIT_NONE;
  mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_NONE;

  /* Gaurd check to clear any call contexts / access reasons before
   * entering IDLE state because of any state transitions.
   */

  /* Reset the call status */
  csfb_cdma.call_status = CSFB_CALL_PENDING_NONE;
  cdma.call_orig = FALSE;
  cdma.is_emergency = FALSE;

  /* Reset the access reason */
  mcc_csfb_access_reason = MCC_CSFB_ACCESS_NONE;

  /* Reset PSIST check flag */
  mcc_csfb_is_reg_psist_chk_done = FALSE;

  /* Reset GCSNA flags */
  mcc_csfb_is_dl_gcsna_ack_pending = FALSE;

  #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
  /* Reset error reason returned by auth task */
  mc_set_auth_err_status(FALSE);
  #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */

  /* Inform CM that 1X is ready to enter Power Saving Mode by sending the 
   * indication "CM_MC_PSM_READY_IND_F" and reset the flag. */
  if(cdma.psm_ready_ind_pending == TRUE)
  {
    cm_mc_rpt_type* cm_ptr;
    if( (cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cm_ptr->psm_ind_info.hdr.cmd = CM_MC_PSM_READY_IND_F;
      mcc_cm_mc_rpt( cm_ptr );
      cdma.psm_ready_ind_pending = FALSE;
      M1X_MSG( DCP, LEGACY_HIGH,
        "PSM CSFB DEBUG: Sent CM_MC_PSM_READY_IND_F");
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
         "No free buffers on cm_mc_rpt_free_q" );
    }
  }
  do
  {
    sigs = mcc_wait(wait_mask);

    if ((sigs & MCC_CMD_Q_SIG) != 0)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Idle state - Rxed sig on CMD Queue");

      if ((cmd_ptr = (mc_msg_type*) q_get( &mc_cmd_q)) != NULL)
      {
        /* process the command and determine what needs to be done next */
        mcc_csfb_proc_hs_cmd(cmd_ptr);

        /* clean up command processing */
       mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
      }
    } /* end else if ((sigs & MCC_CMD_Q_SIG) != 0) */

    else if((sigs & MCC_MSGR_Q_SIG) != 0)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Idle state - rxed sig on MSGR Queue");

      /* Do MSGR Queue check */
      if ( (msgr_cmd_ptr =
           (mc_msgr_rcv_msg_type*) q_get( &mc_msgr_rcv.msg_q )) != NULL)
      {
        /* process the command and determine what needs to be done next */
        mcc_csfb_proc_msgr_rx(msgr_cmd_ptr);

        /* is_l3_msg is used to keep track if a valid layer3 message has been
        * received ver tunnel. We will process the layer3 part of the message
        * only when its set to MCC_CSFB_L3_MSG_RXED. Once the message processing
        * is completed, this is reset to MCC_CSFB_L3_MSG_NOT_RXED.
        */

        /* Because IRAT switch procedures can make us move out of CSFB state,
         * we dont process any tunneled L3 message while IRAT switch is in progress.
         */

        if(( (mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg == MCC_CSFB_L3_MSG_RXED) ||
             (mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg == MCC_CSFB_DUP_L3_MSG_RXED)
           ) &&
           (mc_irat_switch == FALSE)
          )
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: Rxed layer3 messsage over tunnel");
          /* Do Layer 3 processing */
          mcc_csfb_process_layer3_msg();
          /* reset the indication for l3 message to MCC_CSFB_L3_MSG_NOT_RXED */
          mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg = MCC_CSFB_L3_MSG_NOT_RXED;
        }
        else if(mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg == MCC_CSFB_L3_MSG_GCSNA_OPT_REJ)
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: GCSNA opt mismatch, Exit");
          mcc_csfb_setup_gcsna_rej_msg(GCSNA_OPT_MISMATCH);
          mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
          mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_GCSNA_REJ;
          cdma.curr_state = CDMA_CSFB_EXIT;
        }
        else if(mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg == MCC_CSFB_L3_MSG_GCSNA_PREV_REJ)
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: GCSNA 1x prev mismatch, Exit");
          mcc_csfb_setup_gcsna_rej_msg(GCSNA_PREV_MISMATCH);
          mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
          mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_GCSNA_REJ;
          cdma.curr_state = CDMA_CSFB_EXIT;
        }
        /* clean up command processing */
        msgr_rex_free_msg_buf(&msgr_cmd_ptr->msg_hdr.qlink);
      }
    } /* end else if ((sigs & MCC_MSGR_Q_SIG) != 0) */

    else if ( ( (sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              ( mcc_timer_on_callback_q(MCC_CSFB_C2K_PARAM_TMO_TIMER_ID) )
            )
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Ignoring the C2K Params timer expiry in idle");
      /* Reset c2k params expiration timer and timer signal */
      (void) mcc_clr_callback_timer(&mcc_csfb_c2k_param_tmo_timer,
                                    MCC_CSFB_C2K_PARAM_TMO_TIMER_ID);
      /* Since the timer has expired in Idle State, ignore the timeout */
    } /* if (sigs & MCC_CALLBACK_TIMER_SIG) != 0) */

    else if ( ( (sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              ( mcc_timer_on_callback_q(MCC_CSFB_SO76_SMS_ACCESS_GUARD_TIMER_ID) )
            )
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Ignoring the SO76 SMS access guard timer expiry in idle");
      /* Reset SO76 SMS access guard expiration timer and timer signal */
      (void) mcc_clr_callback_timer(&mcc_csfb_so76_sms_access_guard_timer,
                                    MCC_CSFB_SO76_SMS_ACCESS_GUARD_TIMER_ID);
      /* Since the timer has expired in Idle State, ignore the timeout */
    } /* if (sigs & MCC_CALLBACK_TIMER_SIG) != 0) */

    else if ( ( (sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              ( mcc_timer_on_callback_q(MCC_CSFB_SLOTTED_MODE_TIMER_ID) )
            )
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Slotted wake up in CSFB mode");
      /* Reset slotted mode expiration timer and timer signal */
      (void) mcc_clr_callback_timer(&mcc_csfb_slotted_mode_timer,
                                    MCC_CSFB_SLOTTED_MODE_TIMER_ID);
      /* This is an intentional wake up to check if we need to perform
       * any registration. The prospective registrations possible are
       * timer based registration / zone based registration. With this
       * wake up, we will perform regular reg check (below) and determine
       * if registration is needed.
      */
    } /* if (sigs & MCC_CALLBACK_TIMER_SIG) != 0) */
    else if ( ( (sigs & MCC_CALLBACK_TIMER_SIG) != 0 ) &&
              ( mcc_timer_on_callback_q(MCC_CSFB_PWR_DWN_REG_GUARD_TIMER_ID) )
            )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: Power down Guard timer expired, Exiting now!!");
      /* Reset slotted mode expiration timer and timer signal */
      (void) mcc_clr_callback_timer(&mcc_csfb_pwr_dwn_reg_guard_timer,
                                    MCC_CSFB_PWR_DWN_REG_GUARD_TIMER_ID);

      mcc_csfb_access_reason = MCC_CSFB_ACCESS_NONE;

      cdma.curr_state = CDMA_EXIT;
      csfb_cdma.csfb_mode = FALSE;
      csfb_cdma.csfb_1x_transition = FALSE;

    } /* if (sigs & MCC_CALLBACK_TIMER_SIG) != 0) */
#ifdef FEATURE_MODEM_1X_SRVCC
    else if( ( (sigs & MCC_CALLBACK_TIMER_SIG) != 0 ) &&
             ( mcc_timer_on_callback_q(MCC_SRVCC_UHDM_TMO_TIMER_ID) )
           )
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Ignoring SRVCC UHDM timer expiry in Idle");

      /* Reset SRVCC UHDM expiration timer and timer signal */
      (void) mcc_clr_callback_timer(&mcc_srvcc_uhdm_tmo_timer,
                                    MCC_SRVCC_UHDM_TMO_TIMER_ID);
    } /* MCC_SRVCC_UHDM_TMO_TIMER_ID */   
#endif /* FEATURE_MODEM_1X_SRVCC */

    if( (cdma.curr_state == CDMA_CSFB_IDLE) && (mc_irat_switch == FALSE) )
    {
      /* Perform registration only when we are Idle state and not in middle of
       * any IRAT switch procedures.
      */
      cdma.reg_type = mcc_csfb_reg_idle_check();

      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Reg check performed, reg_type = %d",
        cdma.reg_type);

      if (cdma.reg_type != MCCREG_NO_REG_REQUIRED)
      {
        /* Need to perform Registration, move to access state */
        cdma.curr_state = CDMA_CSFB_ACCESS;
        mcc_csfb_access_reason = MCC_CSFB_ACCESS_REG;
      }
      else
      {
        /* No registration is needed, start the slotted timer */
        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG: Starting Slotted mode timer");

        /* Set the timer for CSFB slotted mode wakeup - customized wake up
         * time as 1.28 sec.
         */
        (void) mcc_set_callback_timer(&mcc_csfb_slotted_mode_timer,
                                      1280,
                                      MCC_CSFB_SLOTTED_MODE_TIMER_ID);
      }
    } /* if(cdma.curr_state == CDMA_CSFB_IDLE) */

    /* While being in Idle state, wait for triggers from CMDQ/MSGR/Timer based reg trigger
     */

    /* set the wait Mask */
    wait_mask = MCC_CMD_Q_SIG | MCC_MSGR_Q_SIG | MCC_CALLBACK_TIMER_SIG;
  } while (cdma.curr_state == CDMA_CSFB_IDLE) ;

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Exiting CSFB Idle state, next state = %d",
    cdma.curr_state);

  /* Stop the timer started for CSFB Slotted mode operation */
  (void) mcc_clr_callback_timer(&mcc_csfb_slotted_mode_timer,
                                MCC_CSFB_SLOTTED_MODE_TIMER_ID);
} /* cdma_csfb_idle_state */

/*===========================================================================

FUNCTION MCC_CSFB_IS_PSIST_TEST_PASS

DESCRIPTION
  E-UTRAN provides PSIST values in the SIB8 [TS36.331]. When the PSIST values
  are set, the cdma2000 upper layer in the UE shall perform a persistence test
  only one time using a random value locally generated by the UE when the UE
  makes a voice call origination, SMS origination or registration.
  The persistence test procedure shall be based on C.S0003.

  The cdma2000 upper layers in the UE shall generate a random number RP,
  0 < RP < 1, using the technique described in  the "Pseudorandom Number
  Generator" C.S0005. The persistence test passes when RP is less than the value
  of P (P is PSIST which is obtained from SIB8).

  If the persistence test passes, the cdma2000 upper layer may trigger a voice call
  origination, SMS origination or registration. If the persistence test fails when
  the UE makes a voice call origination or SMS origination, the UE follows
  requirements configured by the operator.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_csfb_is_psist_test_pass
(
  void
)
{
  /* Random value to compare PSIST with */
  dword ran;
  /* Current System Access substate */
  word curr_state;

  /* set the curr_state based on the access reason we want to perform access for */
  if(mcc_csfb_access_reason == MCC_CSFB_ACCESS_REG)
  {
    curr_state = CDMA_REG;
    /* We are checking PSIST value for RGM, set the PSIST check flag */
    mcc_csfb_is_reg_psist_chk_done = TRUE;
    /* Return true, without PSIST check if it is a Ordered Registration,
     * because we shouldnt perform PSIST check for Responses.
     */
    if(cdma.reg_type == CAI_ORDERED_REG)
    {
      return TRUE;
    }
  }
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
  else if(mcc_csfb_access_reason == MCC_CSFB_ACCESS_MSG)
  {
    curr_state = CDMA_MSG;
  }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
  else if(mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORD_RES)
  {
    curr_state = CDMA_ORD_RES;
  }
  else
  {
    curr_state = CDMA_ORIG;
  }

  /* Calculate PSIST */
  if(mccsa_calc_psist(CAI_ACH,&(mcc_csfb_sib8_msg),curr_state) == NOT_ABORT)
  {
    ran = mc_sec_ran_next();

    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG: ran generated = %d, mcc_csfb_psist = %d",
      ran,
      mcc_csfb_psist);

    if(ran < mcc_csfb_psist)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG:Access PSIST check passed, allow access");
      return TRUE;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG:Access PSIST check passed, fail access");
      return FALSE;
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG:PSIST calculation failed, fail access");
    return FALSE;
  }
} /* mcc_csfb_is_psist_test_pass */

/*===========================================================================

FUNCTION MCC_CSFB_SEND_REGISTRATION_MSG

DESCRIPTION
  This function build a Registration Message and send it to LTE for transmission.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the access attempt was aborted, else FALSE.

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_send_registration_msg (void)
{
  /* We need not perform PSIST check here because beform getting 1x params
   * we performed PSIST check.
   */
  /* Form the GCSNA message - in this case CDMA_REG
   */
  mcc_csfb_build_rgm();

  /* Send the message over the Message Router */
  mcc_gcsna_msg_tx(GCSNA_1X_CRKT_SRV_MSG,TRUE,GCSNA_USE_UL_INFO_TX_MSG_TYPE);

  /* We have sent RGM,reset PSIST flag */
  mcc_csfb_is_reg_psist_chk_done = FALSE;
} /* mcc_csfb_send_registration_msg */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
/*===========================================================================

FUNCTION MCC_CSFB_SEND_DBM

DESCRIPTION
  This function builds a Data Burst Message and send it to LTE for
  transmission.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_send_dbm (void)
{
  /* Form the GCSNA message */
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Forming DBM");
  mcc_csfb_build_dbm();

  /* Send the message over the Message Router */
  mcc_gcsna_msg_tx(GCSNA_1X_CRKT_SRV_MSG,FALSE,GCSNA_USE_UL_INFO_TX_MSG_TYPE);
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: sent DBM");
} /* mcc_csfb_send_tunneled_orm */

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
/*===========================================================================

FUNCTION MCC_CSFB_SEND_TUNNELED_ORM

DESCRIPTION
  This function builds an Origination Message and send it to LTE for transmission.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_send_tunneled_orm (void)
{
  /* We need not perform PSIST check here because beform giving CM call
   * confirmation we performed PSIST check.
   */
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Forming ORM");
  /* Form the GCSNA message */
  mcc_csfb_build_orm();

  /* Send the message over the Message Router */
  mcc_gcsna_msg_tx(GCSNA_1X_CRKT_SRV_MSG,FALSE,GCSNA_USE_UL_HO_PREP_MSG_TYPE);
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: sent ORM");

  /* Set an indication to track current state of call */
  csfb_cdma.call_status = CSFB_CALL_TUNNELED;
} /* mcc_csfb_send_tunneled_orm */

/*===========================================================================

FUNCTION MCC_CSFB_SEND_PAGE_RSP_MSG

DESCRIPTION
  This function builds a Page response Message and send it to LTE for
  transmission.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_send_page_rsp_msg(void)
{
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Forming PRM");
  /* Form the GCSNA message */
  mcc_csfb_build_prm();

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
  /* Send the message over the Message Router */
  if( (csfb_cdma.call_status == CSFB_MT_CALL_PENDING) &&
      (cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS) )
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Sending PRM for SO76 call");
    mcc_gcsna_msg_tx(GCSNA_1X_CRKT_SRV_MSG,FALSE,GCSNA_USE_UL_INFO_TX_MSG_TYPE);
  }
  else
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
  {
  mcc_gcsna_msg_tx(GCSNA_1X_CRKT_SRV_MSG,FALSE,GCSNA_USE_UL_HO_PREP_MSG_TYPE);
  }
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: sent PRM");

  /* Set an indication to track current state of call */
  csfb_cdma.call_status = CSFB_CALL_TUNNELED;
} /* mcc_csfb_send_page_rsp_msg */

/*===========================================================================

FUNCTION MCC_CSFB_SEND_ORD_RES

DESCRIPTION
  This function builds a Message for Orders/ any UL message and send it to LTE for
  transmission.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_send_ord_res(void)
{
  word curr_state;

  curr_state = cdma.curr_state;

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Forming UL Message in curr_state=%d",
    curr_state);

  /* Form the GCSNA message */
  mcc_csfb_build_msg();

  /* Send the message over the Message Router */
  mcc_gcsna_msg_tx(GCSNA_1X_CRKT_SRV_MSG,TRUE,GCSNA_USE_UL_INFO_TX_MSG_TYPE);
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: sent MSG type =%d",
    csfb_cdma.sa_resp_msg);

  /* Set an indication to track current state of call */
  csfb_cdma.call_status = CSFB_CALL_PENDING_NONE;

} /* mcc_csfb_send_ord_res */

/*===========================================================================
FUNCTION MCC_CSFB_BUILD_GCSNA_REJ

DESCRIPTION
  This function forms GCSNA Reject message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcc_csfb_build_gcsna_rej(void)
{
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Building GCSNA Reject seq = %d",
    mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_seq);
  mcc_gcsna_int_msg.gcsna_rej.msg_id = GCSNA_SRV_REJ_MSG;
  mcc_gcsna_int_msg.gcsna_rej.rej_seq = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_seq;
  mcc_gcsna_int_msg.gcsna_rej.cause = csfb_cdma.gcsna_rej_cause;

  if( (mcc_gcsna_int_msg.gcsna_rej.cause == GCSNA_OPT_MISMATCH) ||
      (mcc_gcsna_int_msg.gcsna_rej.cause == GCSNA_OPT_PREV_MISMATCH)
    )
  {
    mcc_gcsna_int_msg.gcsna_rej.num_fields = 1;
#ifdef FEATURE_MODEM_1X_SRVCC
    /* SRVCC Mode */
    if(mcc_csfb_get_srvcc_mode() == TRUE)
    {
      /* For SRVCC Scenarios, we need to use GCSNA_OPTION_8 */
      mcc_gcsna_int_msg.gcsna_rej.gcsna_opt = GCSNA_OPTION_8;      
    } /* SRVCC Mode */
    else
#endif /* FEATURE_MODEM_1X_SRVCC */
    { /* CSFB Mode */
      mcc_gcsna_int_msg.gcsna_rej.gcsna_opt = GCSNA_OPTION_1;
    } /* CSFB Mode */
  }
  if( (mcc_gcsna_int_msg.gcsna_rej.cause == GCSNA_PREV_MISMATCH )||
      (mcc_gcsna_int_msg.gcsna_rej.cause == GCSNA_OPT_PREV_MISMATCH)
    )
  {
    mcc_gcsna_int_msg.gcsna_rej.prev_1x = cdma.mob_cai_rev;
  }

  if(mcc_gcsna_int_msg.gcsna_rej.cause == GCSNA_INV_MSG_ID)
  {
    mcc_gcsna_int_msg.gcsna_rej.msg_id_inv = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_id;
  }

} /* mcc_csfb_build_gcsna_rej */

/*===========================================================================
FUNCTION MCC_CSFB_SEND_GCSNA_ACK

DESCRIPTION
  This function builds a GCSNA ack

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_send_gcsna_ack(byte gcsna_ack_seq)
{

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Forming GCSNA ack seq = %d",
    gcsna_ack_seq);

  /* Form the GCSNA L2 ack*/
  mcc_csfb_build_gcsna_ack(gcsna_ack_seq);

  /* Send the message over the Message Router */
  mcc_gcsna_msg_tx(GCSNA_L2_ACK,FALSE,GCSNA_USE_UL_INFO_TX_MSG_TYPE);

} /* mcc_csfb_send_gcsna_ack */

/*===========================================================================
FUNCTION MCC_CSFB_SEND_GCSNA_REJECT

DESCRIPTION
  This function builds GCSNA Reject and transmits it.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_send_gcsna_reject(void)
{
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Forming GCSNA Reject");

  /* Form the GCSNA Reject */
  mcc_csfb_build_gcsna_rej();

  /* Send the message over the Message Router */
  mcc_gcsna_msg_tx(GCSNA_SRV_REJ_MSG,FALSE,GCSNA_USE_UL_INFO_TX_MSG_TYPE);

} /* mcc_csfb_send_gcsna_reject */

/*===========================================================================
FUNCTION MCC_CSFB_SEND_GCSNA_MSG

DESCRIPTION
  This function builds a GCSNA messages comprising of GCSNA ack/ GCSNA SRV REJ

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcc_csfb_send_gcsna_msg(void)
{
  if(csfb_cdma.gcsna_resp_msg == GCSNA_SRV_REJ_MSG)
  {
    mcc_csfb_send_gcsna_reject();
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "DEBUG: Unknown GCSNA msg is requested for transmission");
  }
} /* mcc_csfb_send_gcsna_msg */

/*===========================================================================
FUNCTION MCC_CSFB_INIT_ACCESS_STATE

DESCRIPTION
  Do access attempt specific intializations.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void mcc_csfb_init_access_state(void)
{
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Initializing access state");

  switch(mcc_csfb_access_reason)
  {
    case MCC_CSFB_ACCESS_REG:
    case MCC_CSFB_ACCESS_MSG:
    case MCC_CSFB_ACCESS_ORD_RES:
    {
      /* For REG/MSG, the access starts with requesting c2K parameters
       * Hence set the c2k params state to MCC_1X_PARAM_REQ_PRGS.
       */
      c2k_parms_msg.params_state = MCC_1X_PARAM_REQ_INIT;
      /* Reset the value of number of C2K Params retry */
      csfb_cdma.max_c2k_params_attempt = 0;
      break;
    } /* MCC_CSFB_ACCESS_REG || MCC_CSFB_ACCESS_MSG */
    case MCC_CSFB_ACCESS_ORIG:
    case MCC_CSFB_ACCESS_PRM:
    {
      /* check the call state, for fresh ORM or PRM attempt it should be
       * any thing but CSFB_MO_CALL_PENDING / CSFB_MT_CALL_PENDING
       */
      if( (csfb_cdma.call_status == CSFB_MO_CALL_PENDING) ||
          (csfb_cdma.call_status == CSFB_MT_CALL_PENDING)
        )
      {
        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG: Call initialize");
        /* For ORG/PRM, the access starts waiting for Handover from EUTRA
         * which carries 1x params.
         * Hence set the c2k params state to MCC_1X_PARAM_REQ_PRGS and wait for
         * Handover from EUTRA.
         */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
        if( (csfb_cdma.call_status == CSFB_MT_CALL_PENDING) &&
            (cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS) )
        {
          /* For SO76 call, the access starts with requesting c2K parameters
           * Hence set the c2k params state to MCC_1X_PARAM_REQ_INIT.
           */
          c2k_parms_msg.params_state = MCC_1X_PARAM_REQ_INIT;

          /* Reset the value of number of C2K Params retry */
          csfb_cdma.max_c2k_params_attempt = 0;
        }
        else
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
#ifdef FEATURE_MODEM_1X_SRVCC
        /* SRVCC Mode */
        if(mcc_csfb_get_srvcc_mode() == TRUE)
        {
          if(c2k_parms_msg.params_state == MCC_1X_SRVCC_PARAM_RSP_RXED)
          {
            /* For SRVCC case, first MC will get the HO_EUTRA and then
             * based on MC HO_EUTRA ind to CM, CM sends the SRVCC ORIG Req
             * So mark params_state as Rsp rxed to tunnel the ORM immediately */
            c2k_parms_msg.params_state = MCC_1X_PARAM_RSP_RXED;
          } /* (params_state == MCC_1X_SRVCC_PARAM_RSP_RXED) */
          else
          {
            /* Unexpected scenario, in SRVCC case we first need to get
             * HO_EUTRA and then followed by SRVCC ORM */
            M1X_MSG( DCP, LEGACY_ERROR,
                     "SRVCC: Unexpected call_status:%d, params_state:%d",
                     csfb_cdma.call_status, 
                     c2k_parms_msg.params_state );
          } /* (params_state != MCC_1X_SRVCC_PARAM_RSP_RXED) */
        } /* SRVCC Mode */
        else
#endif /* FEATURE_MODEM_1X_SRVCC */
        { /* CSFB Mode */
          c2k_parms_msg.params_state = MCC_1X_PARAM_REQ_PRGS;
        } /* CSFB Mode */
      } /* (call_status ==  MO_CALL_PENDING || MT_CALL_PENDING) */
      else
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: Call status: %d is not Init, continue in same state",
          csfb_cdma.call_status);
        c2k_parms_msg.params_state = MCC_1X_PARAM_REQ_DONE;
      }
      break;
    } /* MCC_CSFB_ACCESS_ORIG || MCC_CSFB_ACCESS_PRM */
    case MCC_CSFB_ACCESS_GCSNA_MSG:
    {
      /* Since this is a GCSNA message, we donr need to transmit 1X Params Request */
      c2k_parms_msg.params_state = MCC_1X_PARAM_REQ_DONE;
      break;
    } /* MCC_CSFB_ACCESS_GCSNA_MSG */
    default:
    {
      /* For any access attempt other than REG/ORM/PRM , set the
       * c2k_parms_msg.params_state to MCC_1X_PARAM_REQ_DONE, even if
       * they are received or not, since to do the access attempt 1x param
       * is not requireed.
       */
      c2k_parms_msg.params_state = MCC_1X_PARAM_REQ_DONE;
      break;
    }
  } /* switch (mcc_csfb_access_reason) */

  /* Stop any ongoing gcsna access attempt */
  mcc_csfb_gcsna_stop_txn();

} /* void mcc_csfb_init_access_state(void) */

/*===========================================================================
FUNCTION MCC_CSFB_ACCESS_STATE_PROCESS

DESCRIPTION
  This is the tunnel access state, in which DL messages are tunneld from LTE.
  At a broder level, access state comprises of Registration/Origination/
  Page Response/Message substates, which are identified by mcc_csfb_access_reason.

  As per the message that needs to be tunneled, mcc_csfb_access_reason will get
  filled.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void mcc_csfb_access_state_process(void)
{

  db_items_value_type item;
   /* storage for db items */


  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Entered Tunnel access state, access reason = %d",
    mcc_csfb_access_reason);

  /* Access states at a higher level are identified with mcc_csfb_access_reason,
   * but the actual transmisson is triggered by c2k_parms_msg.params_state.
   * we transmit messages, as per the state of c2k_parms_msg.params_state
   * Following are the states of the variable - c2k_parms_msg.params_state:
   *   --> MCC_1X_PARAM_INIT        -> As soon as access state is entered.
   *   --> MCC_1X_PARAM_REQ_IN_PRGS -> This indicates that C2K Param req is
   *                                   in prgs.we perform no access related
   *                                   action in this state.
   *   --> MCC_1X_PARAM_RXED        -> This indicates receipt of 1X Params.
   *                                   This state is set when 1X Params are
   *                                   received.
   *   --> MCC_1X_PARAM_REQ_DONE    -> This indicates C2K Param is used for
   *                                   an access attempt, and its state is
   *                                   still serene.
   *                                   Based on this state, we might do trigger
   *                                   some access attempts, without having to
   *                                   explicitly request for 1X Param.
   *   --> MCC_1X_PARAM_REQ_INVALID -> Incase we have received an invalid
   *                                   1X Param rsp. This state is set when we
   *                                   parse and realize that 1X Params have gone
   *                                   bad.
   *
  */

  switch (mcc_csfb_access_reason)
  {
    case MCC_CSFB_ACCESS_REG:
    case MCC_CSFB_ACCESS_MSG:
    case MCC_CSFB_ACCESS_ORD_RES:
    {
      /* --------------------------- DETAILS - --------------------------- */
      /* Sending of Registration/ Msg/ ORD RES is a two step process.First
       * step is to rqst for 1X Param message and wait for response.
       * After rxing 1X Param Rsp, form RGM and tunnel it.
       *
       * We transmit 1X Param only when params_state = MCC_1X_PARAM_INIT.
       * We form and transmit RGM only when params_state = MCC_1X_PARAM_RXED.
       * We mark the status to MCC_1X_PARAM_REQ_DONE, once RGM is tunneled.
       * Other values are no-op.
       *
       * We get out of registration sub-state if we receive GCSNA ack/any
       * failure triggers.
      */
      /* For RGM, before we send C2K params req , make sure that PSIST is
       * passed. If it is a failure, terminate RGM process and return to CSFB
       * Idle state.
       */
      /* ------------------------------------------------------------------ */

      /* Switch on params_state to do the correct access */
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: c2k_parms_msg.params_state = %d",
        c2k_parms_msg.params_state);

      switch(c2k_parms_msg.params_state)
      {
        case MCC_1X_PARAM_REQ_INIT:
        {
          /* Canditate for sending 1X Param Request */
          /* The first step for RGM is to perform PSIST checks
           * as per C.S0002 standard.
           * Input for the PSIST algorithm is access barring received in SIB8.
           * Also, make sure that PSIST check is performed only when we have
           * determined that we have a reg_type, else we could be here to check
           * if power-up reg for band class chnage needs to be performed.
           */
          if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_REG) &&
              (cdma.reg_type != MCCREG_NO_REG_REQUIRED) &&
              (mcc_csfb_is_reg_psist_chk_done == FALSE)
            )
          {
            M1X_MSG( DCP, LEGACY_MED,
              "DEBUG: PSIST checking for RGM");
            if(mcc_csfb_is_psist_test_pass() == FALSE)
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "DEBUG: Rxed PSIST for RGM failed, returning");

              if(cdma.reg_type == CAI_POWERDOWN_REG)
              {
                /* Check if it happened for power-down registration, if yes
                 * directly report DEACT CNF intead of reporting pre-reg failure
                 */
                cdma.curr_state = CDMA_EXIT;
                csfb_cdma.csfb_mode = FALSE;
              }
              else
              {
                /* Inform CM of the error happened with Registration */
                cm_mc_rpt_type *cm_ptr;
                cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
                cm_ptr->cdma_csfb_status.hdr.cmd = CM_1XCSFB_STATUS_F;
                cm_ptr->cdma_csfb_status.lte_1xcsfb_result = CM_1XCSFB_PREREG_FAIL;
                mcc_cm_mc_rpt( cm_ptr );
                /* Mark so that no registration check is performed any further */
                csfb_cdma.reg_status = MCC_CSFB_REG_STOP;
                /* Exit to CSFB Idle eventually */
                mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
                mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_PSIST_CHK_FAIL;
                cdma.curr_state = CDMA_CSFB_EXIT;
              }
              M1X_MSG( DCP, LEGACY_MED,
                "DEBUG: Done access state processing, curr_state = %d",
                cdma.curr_state);
              return;
            } /* end of mcc_csfb_is_psist_test_pass() == FALSE */
          }
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
          else if(mcc_csfb_access_reason == MCC_CSFB_ACCESS_MSG)
          {
            M1X_MSG( DCP, LEGACY_MED,
              "DEBUG: PSIST checking for MSG TXN");
            if(mcc_csfb_is_psist_test_pass() == FALSE)
            {
              M1X_MSG( DCP, LEGACY_HIGH,
                "PSIST check for MSG TXN failed");
              /* Abort the ongoing SMS. */
              mcc_check_mo_dbm_status((cm_dbm_mo_status_e_type)CM_SMS_MO_L2_ACK_FAILURE);


              /* Exit to CSFB Idle eventually */
              mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
              mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_PSIST_CHK_FAIL;
              cdma.curr_state = CDMA_CSFB_EXIT;

              M1X_MSG( DCP, LEGACY_HIGH,
                "DEBUG: Done access state processing, curr_state = %d",
                cdma.curr_state);
              return;
            }
          }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
          /* Send 1X Param Request */
          mcc_gcsna_msg_tx(GCSNA_1X_PARAM_REQ,FALSE,GCSNA_USE_NONE_TYPE);

          /* Set the params_state to MCC_1X_PARAM_REQ_IN_PRGS */
          c2k_parms_msg.params_state = MCC_1X_PARAM_REQ_PRGS;
          break;
        } /* MCC_1X_PARAM_REQ_INIT */

        case MCC_1X_PARAM_RSP_RXED:
        {
          /* Rxed C2K Params, for RGM with C2K params recived and tunnel it */
          /* Just before tunneling the RGM make sure that we perform a highest ordered
           * registration.
           */
          /* Note: There needs to be a special case handling here for power_down reg.
           * Incase there is a power-down in progress becauase of which we want to
           * do power-down reg, we dont want to perform highest ordered reg check.
           * similar casing is needed for Ordered Reg, because n/w has commanded us
           * to perform registration and we dont want to determine highest order reg
           * in this case.
           */

          /* In case of access done for transmitting MSG, return */
          if(mcc_csfb_access_reason == MCC_CSFB_ACCESS_REG)
          {
            if( (cdma.reg_type != CAI_POWERDOWN_REG) &&
                (cdma.reg_type != CAI_ORDERED_REG)
              )
            {
              M1X_MSG( DCP, LEGACY_HIGH,
                "DEBUG: C2K params rxed, Performing highest order reg check");
              cdma.reg_type = mcc_csfb_reg_idle_check();
            }

            /* If powerdown reg is disabled in the received C2K param message, we
             * need to skip it. */

            if((cdma.reg_type == CAI_POWERDOWN_REG) &&
               (mcc_csfb_sib8_msg.reg_params.powerDownReg == FALSE))
            {
              M1X_MSG( DCP, LEGACY_HIGH,
                "Skipping PwrDwn Reg since pwr dwn reg is not needed");
              cdma.reg_type = MCCREG_NO_REG_REQUIRED;

              cdma.curr_state = CDMA_EXIT;
              csfb_cdma.csfb_mode = FALSE;
              /* Set the params_state to MCC_1X_PARAM_REQ_DONE */
              c2k_parms_msg.params_state = MCC_1X_PARAM_REQ_DONE;
              return;
            }

            M1X_MSG( DCP, LEGACY_HIGH,
              "DEBUG: Reg Determined = %d",
              cdma.reg_type);
            if(cdma.reg_type != MCCREG_NO_REG_REQUIRED)
            {
              /* perform PSIST check here for the cases where we determined
               * need for power-up reg based on band class change, in which case
               * we have requested 1x params without prior PSIST check.
               */
              if(mcc_csfb_is_reg_psist_chk_done == FALSE)
              {
                /* Perform PSIST check now */
                if(mcc_csfb_is_psist_test_pass() == FALSE)
                {
                  M1X_MSG( DCP, LEGACY_ERROR,
                    "DEBUG: Rxed PSIST for RGM failed, returning");
                  if(cdma.reg_type == CAI_POWERDOWN_REG)
                  {
                    /* Check if it happened for power-down registration, if yes
                     * directly report DEACT CNF intead of reporting pre-reg failure
                     */
                    cdma.curr_state = CDMA_EXIT;
                    csfb_cdma.csfb_mode = FALSE;
                  }
                  else
                  {
                    /* Inform CM of the error happened with Registration */
                    cm_mc_rpt_type *cm_ptr;
                    cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
                    cm_ptr->cdma_csfb_status.hdr.cmd = CM_1XCSFB_STATUS_F;
                    cm_ptr->cdma_csfb_status.lte_1xcsfb_result = CM_1XCSFB_PREREG_FAIL;
                    mcc_cm_mc_rpt( cm_ptr );
                    /* Mark so that no registration check is performed any further */
                    csfb_cdma.reg_status = MCC_CSFB_REG_STOP;
                    /* Exit to CSFB Idle eventually */
                    mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
                    mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_PSIST_CHK_FAIL;
                    cdma.curr_state = CDMA_CSFB_EXIT;
                  }
                  M1X_MSG( DCP, LEGACY_MED,
                    "DEBUG:Done access state processing, curr_state = %d",
                    cdma.curr_state);
                  return;
                } /* end of (mcc_csfb_is_psist_test_pass() == FALSE) */
              } /* mcc_csfb_is_reg_psist_chk_done == FALSE */

              /* PSIST check passed, send RGM */
              mcc_csfb_send_registration_msg();
            }
            else
            {
              /* No reg check is required, return to the idle state */
              M1X_MSG( DCP, LEGACY_MED,
                "DEBUG: No Reg required,returning = %d",
                cdma.reg_type);
              cdma.curr_state = CDMA_CSFB_IDLE;
              mcc_csfb_access_reason = MCC_CSFB_ACCESS_NONE;
            }
          }
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
          else if(mcc_csfb_access_reason == MCC_CSFB_ACCESS_MSG)
          {
            /* Send the DBM now. */
            mcc_csfb_send_dbm();
          }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
          else
          {
            mcc_csfb_send_ord_res();
          }
          /* Set the params_state to MCC_1X_PARAM_REQ_DONE */
          c2k_parms_msg.params_state = MCC_1X_PARAM_REQ_DONE;
          break;
        } /* MCC_1X_PARAM_RSP_RXED */

        case MCC_1X_PARAM_RSP_INVALID:
        {
          /* If invalid 1X params are received for REG attempt, report
           * error to CM.
           */
          if(mcc_csfb_access_reason == MCC_CSFB_ACCESS_REG)
          {
            cm_mc_rpt_type *cm_ptr;
            M1X_MSG( DCP, LEGACY_HIGH,
              "DEBUG: Rxed MCC_1X_PARAM_RSP_INVALID, aborting access");
            cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
            cm_ptr->cdma_csfb_status.hdr.cmd = CM_1XCSFB_STATUS_F;
            cm_ptr->cdma_csfb_status.lte_1xcsfb_result = CM_1XCSFB_PREREG_FAIL;
            mcc_cm_mc_rpt( cm_ptr );
            /* Mark so that no registration check is performed any further */
            csfb_cdma.reg_status = MCC_CSFB_REG_STOP;
          }
          else if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORD_RES) &&
                   (csfb_cdma.sa_resp_ord.ordq.order == CAI_RELEASE_ORD)
                 )
          {
            /* Inform CM of the call end  */
            cm_mc_rpt_type *cm_ptr;
            cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
            cm_ptr->csfb_call_end_cnf.hdr.cmd = CM_1XCSFB_CALL_END_CNF_F;
            cm_ptr->csfb_call_end_cnf.call_id = cdma.call_id;
            /* Send Call End Conf to CM. */
            mcc_cm_mc_rpt( cm_ptr );
          }
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
          else if(mcc_csfb_access_reason == MCC_CSFB_ACCESS_MSG)
          {
            /* Abort the ongoing SMS. */
            mcc_check_mo_dbm_status((cm_dbm_mo_status_e_type)CM_SMS_MO_L2_ACK_FAILURE);
          }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
          mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
          mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_INVALID_1X_PARAMS;
          cdma.curr_state = CDMA_CSFB_EXIT;
          break;
        } /* MCC_1X_PARAM_RSP_INVALID */

        default:
        {
          /* for default cases,actually do nothing */
          break;
        }
      }
      break;
    } /* case MCC_CSFB_ACCESS_REG || MCC_CSFB_ACCESS_MSG */

   case MCC_CSFB_ACCESS_ORIG:
   {
     /* Send origination only when we receive HO from EUTRA and the state of
      * C2K params is MCC_1X_PARAM_RXED.Till then keep waiting for HO from
      * EUTRA.
      */
     if(c2k_parms_msg.params_state == MCC_1X_PARAM_RSP_RXED)
     {

       /* UI would set the SO to CAI_SO_VOICE_WILDCARD if a 'voice' call is
          originated by user else would set the SO to the option requested by
         DIAG. If the SO is a wildcard, then the preferred voice options
          stored in NV are used to determine the service option to originate in */

       if (cdma.so_req == CAI_SO_VOICE_WILDCARD)
       {
         /* Get roam status of phone from db */
         db_get(DB_ROAM, &item);

         /* check to see if this origination is going to be put through in the
            home system */
         if (item.roam == DB_ROAM_OFF)
         {
           /* use the preferred service option for originations in home systems */
           cdma.so_req = mcc_pref_voice_so.home_orig_voice_so;
         }
         else
         {
           /* use the preferred SO for originations in roaming systems */
           cdma.so_req = mcc_pref_voice_so.roam_orig_voice_so;
         }

         cdma.so_req = snm_eval_so(cdma.so_req, NULL);
       }


       /* Tunnel 1X ORM */
       mcc_csfb_send_tunneled_orm();

       /* Set the params_state to MCC_1X_PARAM_REQ_IN_PRGS */
       c2k_parms_msg.params_state = MCC_1X_PARAM_REQ_DONE;
     }
     else if(c2k_parms_msg.params_state == MCC_1X_PARAM_RSP_INVALID)
     {
#ifdef FEATURE_MODEM_1X_SRVCC
       if(mcc_csfb_get_srvcc_mode() == FALSE)
#endif /* FEATURE_MODEM_1X_SRVCC */
       { /* CSFB Mode */
         onex_cp_lte_1xcsfb_ho_fail_rsp_t ho_fail_rsp;

         /* We received invalid params. Indicate the call failure to CM
          * via RRC->NAS->CM. After indicating the error, expect CM to
          * send Call Clear Indication. When we receive CALL_CLEAR ind from
          * CM, we will delete the call instance and return to CSFB IDle state.
          */
         M1X_MSG( DCP, LEGACY_HIGH,
           "DEBUG: Deactivating the lower layers");
         mcc_csfb_deactivate_lower_layers();
         /* Now send the failure to upper layers 
          * Fill in the cause as per the error type */
         ho_fail_rsp.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD;
         /* Send the message over MSGR to LTE_RRC */
         mc_msgr_send_msg( &ho_fail_rsp.hdr,
                          ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP,
                          sizeof(ho_fail_rsp));

         M1X_MSG( DCP, LEGACY_MED,
           "DEBUG: Sent Failure ind. waiting for call clear");
       } /* CSFB Mode */
     } /* (params_state == MCC_1X_PARAM_RSP_INVALID) */
     break;
   } /* MCC_CSFB_ACCESS_ORIG */

    case MCC_CSFB_ACCESS_PRM:
    {
      /* Send page response only when we receive HO from EUTRA and the state of
       * C2K params is MCC_1X_PARAM_RXED.Till then keep waiting.
       */

      if(c2k_parms_msg.params_state == MCC_1X_PARAM_RSP_RXED)
      {
        /* Tunnel 1X PRM */
        mcc_csfb_send_page_rsp_msg();

        /* Reset params_state as we are done with the tunnel acess. */
        c2k_parms_msg.params_state = MCC_1X_PARAM_REQ_DONE;
      }
      else if(c2k_parms_msg.params_state == MCC_1X_PARAM_RSP_INVALID)
      {

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
        if(cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Invalid C2K params rxed for SO76 call, send call resp to CM");
          mc_cm_send_call_resp(LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD);
        }
        else
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
        {
          onex_cp_lte_1xcsfb_ho_fail_rsp_t ho_fail_rsp;
  
          /* We received invalid params. Indicate the call failure to CM
           * via RRC->NAS->CM. After indicating the error, expect CM to
           * send Call Clear Indication. When we receive CALL_CLEAR ind from
           * CM, we will delete the call instance and return to CSFB IDle state.
           */
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: Deactivating the lower layers");
          mcc_csfb_deactivate_lower_layers();
          /* Now send the failure to upper layers */
          /* Fill in the cause as per the error type */
          ho_fail_rsp.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD;
          /* Send the message over MSGR to LTE_RRC */
          mc_msgr_send_msg( &ho_fail_rsp.hdr,
                          ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP,
                          sizeof(ho_fail_rsp));
        }

        M1X_MSG( DCP, LEGACY_MED,
            "DEBUG: Sent Failure ind. waiting for call clear");
      } /* (params_state == MCC_1X_PARAM_RSP_INVALID) */
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
      else if((c2k_parms_msg.params_state == MCC_1X_PARAM_REQ_INIT) &&
              (cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS)
             )
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Requesting for C2K Params for SO 76 call");
        /* Send 1X Param Request */
        mcc_gcsna_msg_tx(GCSNA_1X_PARAM_REQ,FALSE,GCSNA_USE_NONE_TYPE);

        /* Set the params_state to MCC_1X_PARAM_REQ_IN_PRGS */
        c2k_parms_msg.params_state = MCC_1X_PARAM_REQ_PRGS;
      }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
      break;
    } /* MCC_CSFB_ACCESS_PRM */

    case MCC_CSFB_ACCESS_GCSNA_MSG:
    {
      /* Send GCSNA messages, GCSNA_L2_ACK/GCSNA_SRV_REJ_MSG.
       * For these messages we dont have to seek C2K Params, hence start with
       * msg transmission directly.
       */
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: Sending GCSNA msg = %d",
        csfb_cdma.gcsna_resp_msg);
      mcc_csfb_send_gcsna_msg();
      break;
    } /* MCC_CSFB_ACCESS_GCSNA_MSG */

    default:
      break;
  }
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG:Done access state processing, curr_state = %d",
    cdma.curr_state);
} /* mcc_csfb_access_state_process */

/*===========================================================================
FUNCTION MCC_CSFB_EXIT_ACCESS_STATE

DESCRIPTION
  Do access attempt exit specific intializations.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void mcc_csfb_exit_access_state(void)
{
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Exiting access state");

  /* While exiting the access state , stop all GCSNA attempts and set the
   * access reason to NONE..
  */
  mcc_csfb_access_reason = MCC_CSFB_ACCESS_NONE;

  /* Stop any ongoing gcsna access attempt */
  mcc_csfb_gcsna_stop_txn();

  /* -----------------------------------------------------------------
  ** We may be in the middle of doing some authentication calculation,
  ** like an auth signature, which sends a report when it completes.
  ** if this is the case, we wait for that report now to ensure it is
  ** not improperly interpreted later, and to ensure the report buffer
  ** is returned to the right queue.
  ** ----------------------------------------------------------------- */
  mcc_resync_auth();
} /* mcc_csfb_exit_access_state */

/*===========================================================================
FUNCTION CDMA_CSFB_ACCESS

DESCRIPTION
  This procedure performs CSFB ACCESS State operations and
  tunnels GCSNA messages to 1X n/w through LTE n/w.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the next state within the CSFB state machine to be processed.

SIDE EFFECTS
  None.
===========================================================================*/

void cdma_csfb_access_state( void )
{
  /* Wait mask and sigs */
  dword wait_mask,sigs;

  /* Current access reason to keep track of */
  mcc_csfb_access_rsn_type curr_access_reason;

  /* Command from the MC_CMD_Q queue */
  static mc_msg_type *cmd_ptr;

  /* Command from MSGR_Q queuw */
  static mc_msgr_rcv_msg_type *msgr_cmd_ptr;

  /* Initialize wait mask and sigs */
  wait_mask = sigs = 0;

  /* Initialize curr access reason to none */
  curr_access_reason = MCC_CSFB_ACCESS_NONE;

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: CSFB access state, access_rsn = %d",
    mcc_csfb_access_reason);

  /* ------------------------------------------------------------------------*/
  /* we enter into CSFB access whenever we have to perform access procedures
   * over LTE network. We send message to LTE-NAS to transmit over LTE n/w.
   *
   * We  enter into CSFB access state to perform Registration/Origination/
   * Page Response/any Message transmission.
   *
   * Any single access attempt could comprise of Retry and Retransmission.
   * A message is retired if NAS reports  BAD_STATE, a message is retransmitted
   * if GCSNA ack is not received for the transmitted message.
   *
   * At any given point of time,there will be only one GCSNA message in transmission.
   * i.e GCSNA shall support stop and go mechanism. If there is a need to supercede
   * an ongoing transmission , we should stop the ongoing transmission and start
   * a new attempt.
  */
  /* ------------------------------------------------------------------------*/

  do
  {
    sigs = mcc_wait(wait_mask);

    if ((sigs & MCC_CMD_Q_SIG) != 0)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Access state -rxed sig on CMD Queue");

      if ((cmd_ptr = (mc_msg_type*) q_get( &mc_cmd_q)) != NULL)
      {
        mcc_csfb_proc_hs_cmd(cmd_ptr);

        /* clean up command processing */
        mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
      }
    } /* end else if ((sigs & MCC_CMD_Q_SIG) != 0) */

    else if((sigs & MCC_MSGR_Q_SIG) != 0)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Access State - rxed sig on MSGR");

      /* Do MSGR Queue check */
      if ( (msgr_cmd_ptr =
           (mc_msgr_rcv_msg_type*) q_get( &mc_msgr_rcv.msg_q )) != NULL)
      {
        /* Before processing any message set the l3_msg rxed indication to
         *  not received */
         mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg = MCC_CSFB_L3_MSG_NOT_RXED;

        /*  Process the MSGR commands */
        mcc_csfb_proc_msgr_rx(msgr_cmd_ptr);

        /* clean up command processing */
        msgr_rex_free_msg_buf(&msgr_cmd_ptr->msg_hdr.qlink);

        /* check if we are required to do IRAT switch */
        if(mc_irat_switch == TRUE)
        {
          /* we have begun IRAT switch procedures, halt access procedures
           * in progress and dont act on the l3 messages received.
          */
          mcc_csfb_gcsna_stop_txn();
          mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg = MCC_CSFB_L3_MSG_NOT_RXED;
        }

        /* is_l3_msg is used to keep track if a valid layer3 message has been
          * received on tunnel.
          * We will process the layer3 part of the message only when it is set to
          * MCC_CSFB_L3_MSG_RXED or MCC_CSFB_L3_MSG_RXED_INCORRECT.
          * Once the L3 message processing is completed, this is reset to
          * MCC_CSFB_L3_MSG_NOT_RXED.
          */

        /* Check if the message parsing was a success, else exit from here */
        if(mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg == MCC_CSFB_L3_MSG_RXED_INCORRECT)
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: Received malformed msg, exiting");
          /* We further can not do anything with this message, so set the exit reason to Release
            *  Question - how do we deal about this?? like we migh have received a malformed
            *  release order in b/w a call.. we dont want to terminate the call based on it. commenting
            *  the code for now.
           */
          /* mcc_csfb_exit.exit = MCC_CSFB_EXIT_RELEASE;
               mcc_csfb_exit.exit_reason= MCC_CSFB_EXIT_MALFORMED_MSG;
               cdma.curr_state = CDMA_CSFB_EXIT;
          */
        }
        else if((mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg == MCC_CSFB_L3_MSG_RXED) ||
                (mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg == MCC_CSFB_DUP_L3_MSG_RXED)
               )
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: Rxed layer3 messsage over tunnel with status = %d",
            mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg);

          /* Do Layer 3 message processing */
          mcc_csfb_process_layer3_msg();

          /* Reset the indication for l3 message to MCC_CSFB_L3_MSG_NOT_RXED */
          mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg = MCC_CSFB_L3_MSG_NOT_RXED;
        }
        else if(mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg == MCC_CSFB_L3_MSG_GCSNA_OPT_REJ)
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: GCSNA opt mismatch, Exit, acc_rsn:%d, call_status:%d",
                   mcc_csfb_access_reason,
                   csfb_cdma.call_status);
          if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORIG) ||
              (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM)
            )
          {

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
            if(cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS)
            {
              M1X_MSG( DCP, LEGACY_MED,
                "GCSNA opt mismatch for SO76 call, send call resp to CM");
              mc_cm_send_call_resp(LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD);
            }
            else
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
            {
#ifdef FEATURE_MODEM_1X_SRVCC
              /* SRVCC Mode */
              if(mcc_csfb_get_srvcc_mode() == TRUE)
              {
                /* For SRVCC scenarios, send HO_FAIL rpt to LTE-RRC only
                 * after we receive MO_EUTRA */
                if(csfb_cdma.call_status == CSFB_CALL_HO_RXED)
                {
                  mcc_send_lterrc_srvcc_ho_fail_rsp(
                                LTE_IRAT_1XSRVCC_HO_FAILURE_MSG_INVAILD);
                }
                 
                /* Send SRVCC HO_FAIL Indication to CM and 
                 * Wait for "CALL_CLEAR_F" cmd from CM */
                mcc_send_cm_srvcc_ho_fail_ind();
              } /* SRVCC Mode */
              else 
#endif /* FEATURE_MODEM_1X_SRVCC */
              { /* CSFB Mode */
                /* Send Failure to upper layers */
                onex_cp_lte_1xcsfb_ho_fail_rsp_t ho_fail_rsp;
                ho_fail_rsp.ho_fail_cause = 
                            LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD;
                /* Send the message over MSGR to LTE_RRC */
                mc_msgr_send_msg(&ho_fail_rsp.hdr,
                                 ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP,
                                 sizeof(ho_fail_rsp)
                                 );
              } /* CSFB Mode */
            }
            M1X_MSG( DCP, LEGACY_MED,
              "Call failure ind sent to upper layers, wait for call clear now");
          } /* (mcc_csfb_access_reason == ORM || PRM) */
          else
          {
            mcc_csfb_setup_gcsna_rej_msg(GCSNA_OPT_MISMATCH);
            mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
            mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_GCSNA_REJ;
            cdma.curr_state = CDMA_CSFB_EXIT;
          }
        }
        else if(mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg == MCC_CSFB_L3_MSG_GCSNA_PREV_REJ)
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: GCSNA 1x prev mismatch, Exit, acc_rsn:%d, call_status:%d",
                   mcc_csfb_access_reason,
                   csfb_cdma.call_status);
          if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORIG) ||
              (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM)
            )
          {
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
            if(cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS)
            {
              M1X_MSG( DCP, LEGACY_MED,
                "Gcsna 1x prev mismatch for SO76 call, send call resp to CM");
              mc_cm_send_call_resp(LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD);
            }
            else
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
            {
#ifdef FEATURE_MODEM_1X_SRVCC
              /* SRVCC Mode */
              if(mcc_csfb_get_srvcc_mode() == TRUE)
              {
                /* For SRVCC scenarios, send HO_FAIL rpt to LTE-RRC only
                 * after we receive MO_EUTRA */
                if(csfb_cdma.call_status == CSFB_CALL_HO_RXED)
                {
                  mcc_send_lterrc_srvcc_ho_fail_rsp(
                                LTE_IRAT_1XSRVCC_HO_FAILURE_MSG_INVAILD);
                }
                 
                /* Send SRVCC HO_FAIL Indication to CM and 
                 * Wait for "CALL_CLEAR_F" cmd from CM */
                mcc_send_cm_srvcc_ho_fail_ind();
              } /* SRVCC Mode */
              else
#endif /* FEATURE_MODEM_1X_SRVCC */
              { /* CSFB Mode */
                /* Send Failure to upper layers */
                onex_cp_lte_1xcsfb_ho_fail_rsp_t ho_fail_rsp;
                ho_fail_rsp.ho_fail_cause = 
                           LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD;
                /* Send the message over MSGR to LTE_RRC */
                mc_msgr_send_msg(&ho_fail_rsp.hdr,
                                 ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP,
                                 sizeof(ho_fail_rsp)
                                 );
              } /* CSFB Mode */
            }
            M1X_MSG( DCP, LEGACY_MED,
             "Call failure ind sent to upper layers, wait for call clear now");
          } /* (mcc_csfb_access_reason == ORM || PRM) */
          else
          {
            mcc_csfb_setup_gcsna_rej_msg(GCSNA_PREV_MISMATCH);
            mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
            mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_GCSNA_REJ;
            cdma.curr_state = CDMA_CSFB_EXIT;
          }
        }
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "DEBUG:Dropping the L3 message rxed on tunnel");
        }
      }
    } /* end else if ((sigs & MCC_MSGR_Q_SIG) != 0) */

    else if((sigs & MCC_CSFB_TXN_TIMER_SIG) != 0)
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG:MCC_CSFB_TXN_TIMER_SIG expiry");
      /* clear the signal and set the exp_timer_index */
      (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CSFB_TXN_TIMER_SIG );

      /* Check if the retransmission or the retry timed out. */
      if(gcsna_msg_sent.timer_type == MCC_GCSNA_RETRY_BACKOFF_TIMER)
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG:MCC_GCSNA_RETRY_BACKOFF_TIMER expiry");
        mcc_gcsna_retry();
      }
      else if(gcsna_msg_sent.timer_type == MCC_GCSNA_RETXN_TIMER)
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG:MCC_GCSNA_RETXN_TIMER expiry");

        /* Initialize the retry related parameters for the retransmission. */
        gcsna_msg_sent.num_remaining_retries = 1;

        mcc_gcsna_transmit();
      }
      else
      {
        /* Ideally we shouldn't be here!! */
        M1X_MSG( DCP, LEGACY_HIGH,
          "Unknown Timer expired");
        mcc_csfb_gcsna_stop_txn();
      }
    } /* else if((sigs & timer_mask) != 0) */

    else if ( ( (sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              (mcc_timer_on_callback_q(MCC_CSFB_C2K_PARAM_TMO_TIMER_ID) )
            )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: TMO for C2K Params in Access rsn = %d",
        mcc_csfb_access_reason);
      /* Reset c2k params expiration timer and timer signal */
      (void) mcc_clr_callback_timer(&mcc_csfb_c2k_param_tmo_timer,
                                    MCC_CSFB_C2K_PARAM_TMO_TIMER_ID);
      /* Check if the timeout happened for Registration case
       * If yes, declare reg failure.
       */
      if(mcc_csfb_access_reason == MCC_CSFB_ACCESS_REG)
      {
        /* Check if we have exceeded max number of 1X Params retransmissions */
        if(csfb_cdma.max_c2k_params_attempt > MCC_CSFB_MAX_C2K_PARAMS_ATTEMPTS)
        {
          /* Inform CM of the error happened with Registration */
          cm_mc_rpt_type *cm_ptr;
          cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
          cm_ptr->cdma_csfb_status.hdr.cmd = CM_1XCSFB_STATUS_F;
          cm_ptr->cdma_csfb_status.lte_1xcsfb_result = CM_1XCSFB_PREREG_FAIL;
          mcc_cm_mc_rpt( cm_ptr );
          /* Mark so that no registration check is performed any further */
          csfb_cdma.reg_status = MCC_CSFB_REG_STOP;
          /* Exit to CSFB Idle eventually */
          mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
          mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_C2K_PARAM_TMO;
          cdma.curr_state = CDMA_CSFB_EXIT;
        }
        else
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: SOFT failure for C2K Param TMO, retrying");
          csfb_cdma.max_c2k_params_attempt++ ;
          c2k_parms_msg.params_state = MCC_1X_PARAM_REQ_INIT;
        }
      }
      else if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORD_RES)
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
               || (mcc_csfb_access_reason == MCC_CSFB_ACCESS_MSG)
               || ( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM)
                 && (cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS) )
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
             )
      {
        if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORD_RES) &&
            (csfb_cdma.sa_resp_ord.ordq.order == CAI_RELEASE_ORD))
        {
          /* Inform CM of the call end  */
          cm_mc_rpt_type *cm_ptr;
          cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
          cm_ptr->csfb_call_end_cnf.hdr.cmd = CM_1XCSFB_CALL_END_CNF_F;
          cm_ptr->csfb_call_end_cnf.call_id = cdma.call_id;
          /* Send Call End Conf to CM. */
          mcc_cm_mc_rpt( cm_ptr );
        }
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
        else if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM) &&
                 (cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS) )
        {
          onex_cp_lte_1xcsfb_ho_fail_rsp_t ho_fail_rsp;
          M1X_MSG( DCP, LEGACY_ERROR,
            "Aborting the SO 76 MT call due to C2K param TMO");

          /* Confirm this behavior from CM. Since ESR is not made, call id deallocation itself could work. */
          /* Deactivate the lower layers first and then indicate failure to RRC.
           * this is necessary to ensure that lower layers are deactivated properly
           * before Failure is indicated to upper layers
           */
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: Deactivating the lower layers");
          mcc_csfb_deactivate_lower_layers();
          /* Now send the failure to upper layers */

          /* Inform CM of call failure through RRC->NAS */
          #ifdef FEATURE_1X_CSFB_CALL_END_RSNS
          ho_fail_rsp.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_RETRY_EXHAUST;
          #else
          ho_fail_rsp.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD;
          #endif /* FEATURE_1X_CSFB_CALL_END_RSNS */
          
          
          /* Send the message over MSGR to LTE_RRC */
          mc_msgr_send_msg( &ho_fail_rsp.hdr,
                            ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP,
                            sizeof(ho_fail_rsp) );
        }
        else if(mcc_csfb_access_reason == MCC_CSFB_ACCESS_MSG)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Aborting MO SMS due to C2K param TMO");
          mcc_check_mo_dbm_status((cm_dbm_mo_status_e_type)CM_SMS_MO_L2_ACK_FAILURE);
        }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
        /* It happened for msg transmission. Abort the ongoing msg transmission
         * and go to CSFB Idle State
         */
        mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
        mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_C2K_PARAM_TMO;
        cdma.curr_state = CDMA_CSFB_EXIT;
      }
      else
      {
        /* Timer expired for ORM or PRM, which is gaured by T3417 in NAS.
         * hence ignore the timeout.
         */
        M1X_MSG( DCP, LEGACY_MED,
          "CSFB DEDUG: Ignoring Timeout");
      }
    } /* if (sigs & MCC_CALLBACK_TIMER_SIG) != 0) */

    else if ( ( (sigs & MCC_CALLBACK_TIMER_SIG) != 0 ) &&
              ( mcc_timer_on_callback_q(MCC_CSFB_SO76_SMS_ACCESS_GUARD_TIMER_ID) )
            )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: SO76 SMS access guard timer expired, move to CSFB_IDLE");
      /* Reset SO76 SMS access guard expiration timer and timer signal */
      (void) mcc_clr_callback_timer(&mcc_csfb_so76_sms_access_guard_timer,
                                    MCC_CSFB_SO76_SMS_ACCESS_GUARD_TIMER_ID);
      /* We are in access state and didn't receive the required Release order
       * from the network after sending the Page response message. 
       * Hence we abort the transmission and move to IDLE.*/

      mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
      mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_MT_SMS_SO76_TMO;
      cdma.curr_state = CDMA_CSFB_EXIT;
    } /* if (sigs & MCC_CALLBACK_TIMER_SIG) != 0) */

    else if ( ( (sigs & MCC_CALLBACK_TIMER_SIG) != 0 ) &&
              ( mcc_timer_on_callback_q(MCC_CSFB_SLOTTED_MODE_TIMER_ID) )
            )
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Slotted wake up in CSFB access state, ignoring");
      /* Reset slotted mode expiration timer and timer signal */
      (void) mcc_clr_callback_timer(&mcc_csfb_slotted_mode_timer,
                                    MCC_CSFB_SLOTTED_MODE_TIMER_ID);
      /* We are already in CSFB access state and hence this wake up is unexpecetd.
       * This is a no-op in access state.
       */
    } /* if (sigs & MCC_CALLBACK_TIMER_SIG) != 0) */
    else if ( ( (sigs & MCC_CALLBACK_TIMER_SIG) != 0 ) &&
              ( mcc_timer_on_callback_q(MCC_CSFB_PWR_DWN_REG_GUARD_TIMER_ID) )
            )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: Power down Guard timer expired, EXITing now!");
      /* Reset slotted mode expiration timer and timer signal */
      (void) mcc_clr_callback_timer(&mcc_csfb_pwr_dwn_reg_guard_timer,
                                    MCC_CSFB_PWR_DWN_REG_GUARD_TIMER_ID);

      mcc_csfb_access_reason = MCC_CSFB_ACCESS_NONE;

      cdma.curr_state = CDMA_EXIT;
      csfb_cdma.csfb_mode = FALSE;
      csfb_cdma.csfb_1x_transition = FALSE;

    } /* if (sigs & MCC_CALLBACK_TIMER_SIG) != 0) */
#ifdef FEATURE_MODEM_1X_SRVCC
    else if ( ( (sigs & MCC_CALLBACK_TIMER_SIG) != 0 ) &&
              ( mcc_timer_on_callback_q(MCC_SRVCC_UHDM_TMO_TIMER_ID) )
            )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
               "SRVCC UHDM timer expiry in Access State" );
        
      /* Reset SRVCC UHDM expiration timer and timer signal */
      (void) mcc_clr_callback_timer(&mcc_srvcc_uhdm_tmo_timer,
                                    MCC_SRVCC_UHDM_TMO_TIMER_ID);

      /* Inform CM about SRVCC HO Failure and wait for CALL_CLEAR_F cmd */
      mcc_send_cm_srvcc_ho_fail_ind();    
    } /* if (sigs & MCC_CALLBACK_TIMER_SIG) != 0) */
#endif /* FEATURE_MODEM_1X_SRVCC */

    if(mcc_csfb_access_reason != MCC_CSFB_ACCESS_NONE)
    {
      /* Do the state level initialization at the start of any new access attempt
      */
      if( mcc_csfb_access_reason != curr_access_reason)
      {
        /* State initialization should be done at the time of start of a new
         * access attempt.
         */
        mcc_csfb_init_access_state();
        curr_access_reason = mcc_csfb_access_reason;
      }

      /* Do the state level processing for the tunnel access. */
      mcc_csfb_access_state_process();
    }

    /* set the wait mask */
    wait_mask = MCC_CMD_Q_SIG | MCC_MSGR_Q_SIG | MCC_CSFB_TXN_TIMER_SIG | MCC_CALLBACK_TIMER_SIG;

  }while(cdma.curr_state == CDMA_CSFB_ACCESS);

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG:Exit access state, cdma state = %d",
    cdma.curr_state);
  mcc_csfb_exit_access_state();

} /* cdma_csfb_access_state */

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
