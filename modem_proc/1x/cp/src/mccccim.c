/*===========================================================================

M A I N   C O N T R O L   C D M A   C A L L   C O N T R O L
              I N S T A N C E    M A N A G E R

DESCRIPTION
  This file contains  the processing of a call control instance manager.
  This is where all the message, state and user command processing associated
  with all call control instances take place.  This module communicates
  with MCCTC ( LL3) using well defines primitives.

  Copyright (c) 1999 - 2016 Qualcomm Technologies, Inc. 
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

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccccim.c_v   1.47   04 Oct 2002 14:58:46   varavamu  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccccim.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/30/16   ppr     Chgs to add RatMask payload to Incoming_page call rpts to CM
01/07/16   agh     DRVCC from WiFi to 1X/1XCSFB
12/23/15   ppr     Chgs to forcefully send codec_info to MMODE immediately 
                   after AWIM processing for MT call scenarios
03/19/15   agh     Changes to support Voice Adapter based Audio architecture
12/23/14   ppr     Changes to include 1x OTA msg type in Info recs to CM
12/02/14   agh     Support for new customer specific voice service option
07/17/14   ppr     Added CALL_END cmd support for MT call in WFO sub-state
06/20/14   ppr     Added SRVCC_CALL_END cmd support
05/06/14   gga     1X_CP F3 redistribution
04/08/14   ppr     Changes to handle CM_CSFB_CALL_END_F cmd in traffic state
                   for UHDM based calls
03/21/14   gga     Added support to send orig failure with different reason
                   for an emergency call.
02/05/14   gga     Added a debug to print the duration of DTMF in ms
10/03/13   pap     Mainlining FEATURE_SPECIFIC_CALL_END_STATS.
08/27/13   pap     Mainlining features FEATURE_MODEM_1X_INHIBIT_VOC_ENABLE.
08/20/13   dkb     Mainline FEATURE_PASSPORT_MODE
06/13/13   gga     Added NV support for FEATURE_SPECIFIC_CALL_END_STATS
04/26/13   ppr     CSFB:Defer sending orig cont msg for UHDM based CSFB MO calls
                   till we move to native 1X traffic
04/19/13   vks     Need to call rlp_establish_srvc_type() for MDR data calls
04/02/13   pap     Feature cleanup
01/28/13   gga     Klocwork Reviews Phase 2
01/03/13   nee     Replacement of memcpy() to memscpy()
12/28/12   gga     Klocwork Reviews
11/14/12   ppr     Phase2 - Changes for Specific call end reasons
11/08/12   ppr     Phase1 Changes for Specific call end reasons
10/19/12   vks     Use 1x internal feature (FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB)
                   for HDOn-EVRC feature (FEATURE_VOC_4GV_WB_ON_NB)
10/19/12   ppr     Added defensive checks
10/16/12   vks     Initial changes to support HDOn-EVRC feature
09/15/12   ppr     FR#2370 - 1x memory optimization changes
06/22/12   ppr     Changes in MC for not to do ERR_FATAL for Illegal DTMF digit
05/31/12   cjb     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
05/09/12   ppr     Feature Cleanup
03/13/12   jj      Changes to indicate Codec/Handoff events to upper layers.
03/13/12   jj      CSFB: Fix to defer sending CONNECT_F to CM till HANDOVER_CNF
                   is received.
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
02/15/12   srk     Feature Cleanup: Removed FEATURE_NO_13K_VOICE_SUPPORT and
                   FEATURE_SO_VOICE_SMV
02/04/12   ppr     Feature Cleanup
01/27/12   ppr     Feature Cleanup: Mainlined Always ON features
01/19/12   ppr     Reverting feature cleanup changes for
                   FEATURE_IS2000_RELEASE_A_LOGGING
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
08/01/11   jj      Support for SO33 in SVLTE scenarios.
08/23/11   jj      CSFB: Defer sending call failure indications to CM if in
                   CSFB mode.
07/01/11   ag      Fix to set SO73 COP mode to 4 in case of SO change.
04/29/11   jj      CSFB: Added support for error scenarios - phase2.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
03/03/11   ssh     Reverted the temporary featurization by T53.
03/02/11   ag      Fix to release MVS before MUX is commanded to release SO.
02/24/11   jtm     Fixed comiler error when FEATURE_T53 is disabled.
02/16/11   ssh     Mainlining JCDMA specific requirements.
01/12/11   ssh     Added the fix to deallocate the un-used CM buffer.
01/04/11   ag      Fixed compiler and lint warnings.
01/04/11   ag      Mainlined FEATURE_MVS_MIGRATE.
12/28/10   ag      Decoupling of SO73 from 1xAdv.
12/13/10   jj      Indentation update.
12/04/10   jj      CSFB: Added support for Rel8/Rel9 CSFB.
11/22/10   ag      Added support for sending connection reference to CM.
11/15/10   jtm     Initial SU API split for CP.
09/28/10   ssh     Corrected the compiler issue.
09/28/10   ssh     Fixed compiler warnings.
09/14/10   ag      Featurized voice support under FEATURE_MODEM_1X_VOICE_SUPPORT.
09/13/10   ag      Support to delay enabling vocoder for an MT call till call
                   is accepted.
08/25/10   ag      Changes to set default COP for SO73 to 4.
07/19/10   ag      Included modem_1x_defs.h.
07/19/10   ag      Replaced FEATURE_8K_STUFF_TO_BE_PURGED with
                   FEATURE_MODEM_1X_MVS_API_GEN2 for MVS API selection.
07/15/10   ssh     Fixed Compiler Warnings.
06/15/10   ag      Added support for enhanced loopback service option.
04/02/10   jtm     Changed rlp_api.h to mdrrlp_api.h.
11/06/09   ag      1X Advanced - Enhanced Markov Service Option Support
09/29/09   ag      1X Advanced Signaling Support - Phase 2
09/22/09   ss      Updated DS header include.
07/29/09   ag      Fix to call sd_ss_ind_misc_orig_success only for MO call.
06/23/09   ag      Added support to pass BS ack for connect order to CM under
                   FEATURE_JCDMA_2.
06/03/09   ag      Added support for displaying otasp session status.
                   Merged OTASP fixes for SPC validation failure.
05/03/09   jj      CMI Phase-II update.
03/20/09   jtm     Added txcmc_v.h include.
03/20/09   jtm     Added required include of mvs.h.
03/13/09   adw     Lint fixes.
12/02/08   adw     Commented out _v.h includes that no longer exist inside 1x.
10/21/08   ag      Fixed the issue of data call drop because of alert messages.
07/24/08   bb      Fix compilcation issues with MVS migration feature
06/19/08   bb      Changes for FEATURE_8K_STUFF_TO_BE_PURGED feature
05/15/08   bb      Changes for "MVS API Migration" featur
05/29/08   ag      Fixed null pointer access inside a funtion.
10/04/08   bb      Fixing the Klocwork errors
12/14/07   an      Added Passport Mode
09/11/07   pg      Corrected featurization for incoming call forwarding under
                   FEATURE_JCDMA.
06/11/07   pg      Lint Cleanup: Remove the first 2 arguments from
                   mccccim_is_clam_so_compatible().  They are not used.
05/11/07   pg      More Lint Cleanup.
03/21/07   pg      Rename FEATURE_JCDMA_2 to FEATURE_JCDMA.
03/15/06   pg      Send empty FWIM for incoming call fowarding regardless
                   p_rev_in_use under FEATURE_JCDMA.
11/13/06   pg      Removed dead code in mccccim_send_ext_flash_info_msg().
11/06/06   rsj     Added MCCCCIM_LOW_BATTERY end_reason handling for JCDMA
                   Low Battery event processing in function mccccim_rel.
06/12/06   fc      Added support for service status order.
06/02/06   fc      Cleanup umts interface.
04/26/06   an      Fixed Lint Errors
04/24/06   fc      Fixed Rel A featurization.
12/20/05   sb      Replaced cm_mc_rpt_free_q with cm_mc_rpt_get_buf_else_err_fatal().
08/17/05   fh      Added including of mdrrlp.h when feature FEATURE_IS95B_MDR is
                   defined. This including was originally in rxcmc.h, and now it
                   is removed during the header file clean up.
07/11/05   fh      Cleaned up header file including from MUX:
                     added including of rxcmc.h
                     added including of txcmc.h
05/03/05   fc      Merged the following from MSHSHARED:
04/27/05   an       Added support for 4GV service option
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
02/18/05   fc      Merged the following from MSMSHARED:
02/08/05   an       Mainlined IS95B features
02/03/05   fc      Fixed lint error.
01/07/05   an      Removed feature FEATURE_SMS and FEATURE_UASMS
12/10/04   sb      Fixed lint error.
07/21/04   sj      Added support for umts to 1x handover.
07/14/04   bkm     Undo voc rate reduction when voice SO is initialized -
                   corresponds to create_cc and SO change.
06/18/04   ank     Mainlined FEATURE_GPSONE_DBM.
06/16/04   fc      Release C addendum changes for Sync ID.
05/17/04   va      Merged following from MSMSHARED:
04/08/04   ph       When processing AHDM, return call_id of remaining call.
03/05/04   bkm      Lint cleanup.
03/01/04   va       Fixed the mccccim_is_con_in_rel_substate to not do
                    actual con_ref match of p_rev_in_use < 7
03/12/04   fc      Added support for Release C fast call setup.
02/09/04   yll     Initialize alert_sent_to_cm to FALSE in
                   mccccim_process_create_cc().
01/08/04   va      Changed the "==" to "=" in reset of buffer status
12/12/03   va      Fixed the reset of backup buffer status value
11/24/03   yll     Fixed a compile error under FEATURE_T53
10/22/03   va      Made the change to preserve pre-CCS sequence of incoming->
                   signal->connected seq to CM for calls that can be
                   auto answered like Markov & loopback.
10/17/03   ph      added support for new DIAG interface to retrieve MC state.
09/16/03   ph      added mccccim_is_any_cc_in_wfo() for checking before gps tuneaway.
09/11/03   ph      fixed typo.
09/09/03   bkm     In mccccim_rel(), POWER_DOWN now informs CM of call releases.
08/29/03   va      Fixed mccccim_process_ack to send bacl CM_L2ACK_CALL_HOLD_F
                   even if FWIM or EFWIM has no records.
09/03/03   sb      Reject an AHDM if in a data call; do not delete all call
                   control instances if AHDM is rejected.
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
07/23/03   bkm     Some more lint cleanup.
06/10/03   bkm     Lint cleanup.
05/21/03   az      Removed a bug in which we were sending CM_INCOMING_CALL to
                   CM in wait for order substate.
05/13/03   lh      Added call_id to call hold interface.
04/11/03   ph      Added F3 msg for displaying SO resolved from Voice Wildcard.
03/31/03   va      Added support for handling BS initiated CLAM w/o SO.
                   In this case, we now buffer incoming call rpt to CM
                   till we know what kind of call this is.
03/31/03   va      Fixed BS initiated 3-way calling, changed CM_FLASH_TYPE
                   _EMERGENCY to CM_CALL_SUPS_TYPE_EMERGENCY_CALL
11/18/02   va      Fixed a compiler warning.
10/30/02   bkm     Fixed data type mismatch compile issues under FEATURE_T53.
10/04/02   va      Added a function to get substate of a CC based on con_ref
10/02/02   ph      Added event.h
10/02/02   sb      Changed orig_fail layer to L3 (since we are post-L2 Ack).
10/01/02   az      Mainlined FEATURE_IS2000
09/27/02   va      Set bypass_alert_ans=0 if AWIM/EAWIM or Maintenance Order is
                   received, no matter what the ECAM says.
09/27/02   va      Fixed error where MC was not sending connect report to CM.
09/06/02   va      Clenaed lint errors.
08/13/02   HQ      Added FEATURE_CDSMS.
08/05/02   va      Code Review changes.
07/02/02   az      Added new voice service option FEATURE_SO_VOICE_SMV
06/24/02   ph      Fixes to compile errors when rel a is turned off
06/21/02   ph      changed cmmc.h to cmxll.h
06/20/02   az      Fixed compiler warnings
06/19/02   va      Fixed bug related to emergency calls for p_rev_in_use < 7
06/07/02   va      Added code to handle emer call if p_rev_in_use < 7
05/31/02   va      Added code to avoid sending emer flash in WFO substate.
05/07/02   ph      Fixed compile errors when REL A is turned off.
05/02/02   va      Added layer information to mcc_send_orig_fail call
04/16/02   va      Added some emergency flash enhancements.
04/12/02   lh      Added support for subaddress in Orig_C.
03/25/02   ph      Changed call to snm_is_eom_so_compatible() to allow
                   SO to be changed.
03/25/02   va      Consolidated the event reporting CC instantiated event.
03/18/02   va      Fixed con_ref payload in CC instantiated event.
03/15/02   va      Support for new events.
03/11/02   va      Featurized 13K voice service options.
02/25/02   jrp     Fixed errors in non-Rel A build.
02/13/02   va      Added support for CC related events
02/11/02   va      Added support for flashing emergency call
02/07/02   lh      Added checking for invalid cc_index and function
                   mccccim_call_id_is_in_rel_substate().
01/25/02   jrp     Changed rel_code to rel_codes to comform to CM name change.
01/24/02   ph      Fixed QOS parms length checking.
01/23/01   va      Changed end interface to CM.
01/23/02   ph      Added support for interfaces to SNM for deleted connections
                   and SCR updates.
01/11/01   va      Fixed cases where REL_A_SVD is not defined.
01/08/02   lh      Remove FEATURE_IS2000_REL_A_SVD from MC-CM interface.
12/13/01   va      Merged SVD Changes
  11/30/01 va      Support for RIF-HHO and HO to p_rev < 7
  11/21/01 lh      Changes for CM/UI integration.
  08/28/01 lh      Added support for Concurrent Service.
  08/24/01 ph      Added new func mccccim_con_is_in_rel_substate()
  08/23/01 ph      SVD-changes (initial cut)
10/17/01   jrp     Added call to flush reduced active set log on call completion.
10/15/01   kk      Fixed compiler errors when Release A is not defined.
10/04/01   kk      QoS info is stored in cc_info for Data Calls and conveyed
                   to CM for possible display on UI screen.
09/28/01   kk      Global Emergency Call in Flash added to TC msg processing.
08/09/01   ph      Merged in from 1.52 MSM mcctcsup.c
                   08/02/01   jrp     Added support for SO54, SO55.
08/09/01   lh      Sync up to CP_VU 87.
07/06/01   ph      Added support for Callback Timers
06/22/01   va      Bug fix in mccccim_process_ndss function.
                   Added rel_reason in substate timer expired indication to
                   LL3.
06/15/01   va      Misc. bug fixes.
06/14/01   va      Initial Revision of MC CDMA Call Control Instance Manager.

===========================================================================*/

/* Include Files */

#include "1x_variation.h"
#include "modem_1x_defs.h"
#include "cai_v.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "txcmc.h"
#include "txcmc_i.h"
#include "rxcmc_i.h"
#include "mccccim.h"
#include "cm.h"
#include "cmxll.h"
#include "msg.h"
#include "m1x_diag.h"
#include "err.h"
#include "mcctcsup.h"
#include "mcctci_i.h"
#include "rxtx_v.h"
#include "mdrrlp_api.h"
#include "snm_v.h"
#include "snm_i.h"
#include "mclog_v.h"
#include "mccccl.h"
#include "event.h"
#include "mccsrid_v.h"
#include "mccso.h"

#if  defined (FEATURE_IS95B_MDR)
#include "mdrrlp_api.h"
#endif

#ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
#include "mvssup.h"
#include "mvs.h"

#ifdef FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB
#error code not present
#endif /* FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB */

#endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/*lint -e818 */

#define MCCCCIM_MAX_DTMF_TIME 32767U

 /* Code for an invalid con_ref index */
const byte MCCCCIM_CON_REF_NOT_FOUND = 0xff;

 /* Code for an invalid call_id index */
const byte MCCCCIM_CALL_ID_NOT_FOUND = 0xff;

 /* Code for a NULL con_ref */
const byte MCCCCIM_NULL_CON_REF = 0;

/* CC Info table */
mccccim_cc_info_type mccccim_cc_info[MCCCCIM_NUM_MAX_CCS];

#ifdef FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB
#error code not present
#endif /* FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB */

/* Function Prototype needed by this module */
void mccccim_handle_state_transition
(
  mccccim_cc_info_type * cc_info,
  cdma_cc_state_type new_state
);

void mccccim_send_flash_info_msg
(
  mc_msg_type *msg_ptr,
  /* pointer to message received from other other tasks */
  mccccim_cc_info_type * cc_ptr
);

void mccccim_send_ext_flash_info_msg
(
  mc_msg_type *msg_ptr,
  /* pointer to message received from other other tasks */
  mccccim_cc_info_type * cc_ptr
);

/* Information about origination that was converted into flash. no need
   track flash directly */
static mccccim_emer_flash_info_type mccccim_emer_flash_info;

/* Emergency buffer to send msg to rxtx */
static rxtx_cmd_type rxtx_mc_backup_buf;

/* Takes care of un-initialized backup buffer */
static boolean backup_buf_uninitialized = TRUE;

/*===========================================================================

FUNCTION MCCCCIM_GET_CC_INDEX_FROM_CON_REF

DESCRIPTION
  This function searches the Call Control Instance data base and retrives
  the entry that matches the given con_ref

DEPENDENCIES
  None.

RETURN VALUE
  index of the corresponding CC in the array, if no such con_ref is
  found, MCCCCIM_CON_REF_NOT_FOUND is returned.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 mccccim_get_cc_index_from_con_ref
(
  byte con_ref
)
{
  uint8 i;

  for (i=0; i<MCCCCIM_NUM_MAX_CCS; i++)
  {
    if ( mccccim_cc_info[i].active)
    {
      if (con_ref != MCCCCIM_NULL_CON_REF)
      {
        /* If we are looking for a particular con_ref, search
           based on a con_ref match */
        if ( mccccim_cc_info[i].con_ref == con_ref)
        {
          return i;
        }
      }
      else
      {
        /* If we are looking for the NULL con_ref search based on that */
        if (mccccim_cc_info[i].is_null_cc)
        {
          return i;
        }
      }
    }
  }

  return MCCCCIM_CON_REF_NOT_FOUND;

} /* mccccim_get_cc_index_from_con_ref */

/*===========================================================================

FUNCTION MCCCCIM_GET_CC_INDEX_FROM_CALL_ID

DESCRIPTION
  This function searches the Call Control Instance data base and retrives
  the entry that matches the given call_id

DEPENDENCIES
  None.

RETURN VALUE
  index of the corresponding CC in the array, if no such call_id is
  found, MCCCCIM_CALL_ID_NOT_FOUND is returned.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 mccccim_get_cc_index_from_callid
(
  byte call_id
)
{
  uint8 i;

  for (i=0; i<MCCCCIM_NUM_MAX_CCS; i++)
  {
    if ( mccccim_cc_info[i].active)
    {
      /* Check if call_id matches*/
      if (mccccim_cc_info[i].call_id == call_id)
      {
        return i;
      }
    }
  }

  return MCCCCIM_CALL_ID_NOT_FOUND;

} /* mccccim_get_cc_index_from_callid */

/*===========================================================================

FUNCTION MCCCCIM_WFO

DESCRIPTION
  This function does all the processing needed when a CC enters the WFO
  substate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mccccim_wfo
(
  mccccim_cc_info_type * cc_info
)
{
  /* ---------------------------------------------------------------------
  ** Upon entering the substate, the mobile station shall set the substate
  ** timer for T52m seconds (IS-95 section 6.6.4.3.1)
  ** --------------------------------------------------------------------- */

  /* set substate timer */
  (void) mcc_set_callback_timer( &cc_info->substate_timer, cai_tmo.t52m, MCC_CC_SUBSTATE_TIMER_TYPE | cc_info->cc_index );

} /* mccccim_wfo */

/*===========================================================================

FUNCTION MCCCCIM_SEND_CONNECT_ORDER

DESCRIPTION
  This function sends the connect order to the BS for a particular
  CC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A RTC message is sent to the RXTX task.
===========================================================================*/
/*lint -esym(715, cc_ptr) */
void mccccim_send_connect_order
(
  mccccim_cc_info_type * cc_ptr
)
{
  rxtx_cmd_type *rx_ptr;
  boolean send_by_backup_buf = FALSE;
    /* If the msg to rxtx is through backup buffer */

  /* -------------------------------------------------------
  ** Call answered. Send connect order.
  ** ------------------------------------------------------- */
#ifdef FEATURE_1X_CP_MEM_OPT
  if((rx_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL )
#else /* !FEATURE_1X_CP_MEM_OPT */
  if((rx_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL )
#endif /* FEATURE_1X_CP_MEM_OPT */
  {
    /* -------------------------------------------------------
    ** buffer could not be allocated. We are going to use
    ** the backup static buffer reserved for this occasion.
    ** But first make sure that the static buffer has been
    ** initialized.
    ** ------------------------------------------------------- */
    if( backup_buf_uninitialized )
    {
      backup_buf_uninitialized = FALSE;
      rx_ptr = &rxtx_mc_backup_buf;
      send_by_backup_buf = TRUE;

      /* ------------------------------------------------------
      ** If the msg goes through the backup buffer, we don't even
      ** need to notify the MC after ack is received
      ** from the base station.
      ** ------------------------------------------------------ */
      rx_ptr->tc_msg.hdr.command = RXTX_TC_MSG_F;
#ifdef FEATURE_1X_CP_MEM_OPT
      rx_ptr->tc_msg.hdr.queue = RXTX_NO_Q;
#endif /* FEATURE_1X_CP_MEM_OPT */
      rx_ptr->tc_msg.hdr.cmd_hdr.task_ptr   = NULL;
      rx_ptr->tc_msg.hdr.cmd_hdr.sigs       = 0;
      rx_ptr->tc_msg.hdr.cmd_hdr.done_q_ptr = NULL;

      /* We do not need to set the next_ptr to NULL here    */
      /* This is because the call to q_link, before putting */
      /* this buffer on Queue will perform this anyway.     */
    }
    else if( rxtx_mc_backup_buf.hdr.status == RXTX_DONE_S )
    {
      rx_ptr = &rxtx_mc_backup_buf;
      send_by_backup_buf = TRUE;
    }
    else
    {
#ifdef FEATURE_1X_CP_MEM_OPT
      ERR_FATAL("No free RXTX buffer and backup buffer", 0, 0, 0 );
#else /* !FEATURE_1X_CP_MEM_OPT */
      ERR_FATAL( "No free buffers on rxtx_mc_free_q and backup buf",
            0, 0, 0 );
#endif /* FEATURE_1X_CP_MEM_OPT */
    }
  }

  if( rx_ptr != NULL ) /*lint !e774*/
  {
    rx_ptr->tc_msg.msg.tc_ord.gen.msg_type = CAI_TC_REV_ORD_MSG;
    rx_ptr->tc_msg.msg.tc_ord.gen.order = CAI_MS_CONNECT_ORD;
    #ifdef FEATURE_IS2000_REL_A
    if (cc_ptr->con_ref)
    {
      rx_ptr->tc_msg.msg.tc_ord.gen.con_ref_incl = TRUE;
      rx_ptr->tc_msg.msg.tc_ord.gen.con_ref = cc_ptr->con_ref;
    }
    else
    {
      rx_ptr->tc_msg.msg.tc_ord.gen.con_ref_incl = FALSE;
    }
    #endif

    rx_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    if( !send_by_backup_buf )
    {
      /* ------------------------------------------------------
      ** Fill in the command buffer only if we are not sending
      ** through the backup buffer. If we are sending by backup
      ** buffer, the header has been filled up when it is first
      ** used.
      ** ------------------------------------------------------ */
      mcctc_fill_rxtx_hdr_ack_q ( rx_ptr );
    }

    rxtx_cmd( rx_ptr );
  }

} /* mccccim_send_connect_order */
/*lint +esym(715, cc_ptr) */

/*===========================================================================

FUNCTION MCCCCIM_INCOMING_CALL

DESCRIPTION
  This function sends an Incoming Call notification to the User Interface
  and Data Services tasks.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccccim_incoming_call
(
  mccccim_cc_info_type * cc_ptr
)
{
  cm_mc_rpt_type *cmd_ptr;
    /* Pointer to UI command buffer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
  {
    cmd_ptr->incoming_call.hdr.cmd = CM_INCOMING_CALL_F;
    cmd_ptr->incoming_call.so = ((cc_ptr->so == CAI_SO_NULL) ?
                                    cc_ptr->temp_so : cc_ptr->so);
    cmd_ptr->incoming_call.alert_ans_bypass = cc_ptr->bypass_alert_ans;

    cmd_ptr->incoming_call.call_id = cc_ptr->call_id;

    cmd_ptr->incoming_call.con_ref = cc_ptr->con_ref;

    cmd_ptr->incoming_call.subs_capability = mcc_get_rat_mask();

    mcc_cm_mc_rpt( cmd_ptr );
  }
  else
  {
    ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
  }
  cc_ptr->alert_sent_to_cm = TRUE;
  cdma.is_page_in_sv_mode = FALSE;

} /* mccccim_incoming_call */

/*===========================================================================

FUNCTION MCCCCIM_STOP_ALERT

DESCRIPTION
  This function builds a Stop Alert message to be sent to the User Interface
  to stop ringing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccccim_stop_alert
(
  mccccim_cc_info_type * cc_ptr
)
{
  cm_mc_rpt_type *cmd_ptr;
    /* Pointer to UI command buffer */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry != MCC_CSFB)
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cmd_ptr->stop.hdr.cmd = CM_STOP_ALERT_F;

      /* For CCS, call_id is needed in this interface */
      cmd_ptr->stop.call_id = cc_ptr->call_id;

      if(cdma.specific_call_end_stats_supported)
      {
        cmd_ptr->stop.srv_opt = cdma.so_req;
        switch (cc_ptr->rel_del_reason)
        {
          case MCCCCIM_FADE :
          {
          #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
            if(cdma.entry_ptr->entry != MCC_CSFB)
          #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
            {
              cmd_ptr->stop.rpt_type = CM_CALL_FADE_F;
              /* Check for the call end reason */
              switch( cdma.call_end_reason )
              {
                case MC_FADE_RL_ACK_TIMEOUT:
                {
                  /* Fade the call Ack timeout due to 12 retransmission*/
                  cmd_ptr->stop.error_code.fade_error_code = CM_FADE_RL_ACK_TIMEOUT;
                  break;
                }
                case MC_FADE_BAD_FL:
                {
                  /* Fade the call Bad FWD link or T5M expiry*/
                  cmd_ptr->stop.error_code.fade_error_code = CM_FADE_BAD_FL;
                  break;
                }
                default:
                {
                  cmd_ptr->stop.error_code.fade_error_code = CM_FADE_GENERIC;
                  break;
                }
              } /* End of switch( cdma.call_end_reason ) */
            }
            break;
          }

          case MCCCCIM_LOCK:
          {
            /* No need to do anything for this case, MCCTC sends a lock command
               to CM which makes them drop all calls anyway and stay "locked" */
              break;
          }

          case MCCCCIM_BS_END:
          case MCCCCIM_REDIRECTION:
          case MCCCCIM_HHO_FAIL:
          case MCCCCIM_USER_POWERDOWN:
          {
          #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
            if(cdma.entry_ptr->entry != MCC_CSFB)
          #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
            {
              cmd_ptr->stop.rpt_type = CM_CALL_RELEASE_F;
              /* Always use normal reason for BS originated release */
              cmd_ptr->stop.error_code.rel_error_code = CM_CALL_REL_NORMAL;
            }
            break;
          }
          case MCCCCIM_SO_REJ:
          {
          #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
            if(cdma.entry_ptr->entry != MCC_CSFB)
          #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
            {
              cmd_ptr->stop.rpt_type = CM_CALL_RELEASE_F;
              cmd_ptr->stop.error_code.rel_error_code = CM_CALL_REL_SO_REJECT;
            }
            break;
          }
          case MCCCCIM_REL_OTASP_ERR:
          {
            cmd_ptr->stop.rpt_type = CM_CALL_RELEASE_F;
            cmd_ptr->stop.error_code.rel_error_code = CM_CALL_REL_OTASP_SPC_ERR;
            break;
          }

          default:
          {
            M1X_MSG( DCP, LEGACY_MED,
              "delete reason %d, Do send normal CM_STOP_ALERT_F rpt to CM",
              cc_ptr->rel_del_reason);
            break;
          }
        }

        if( cmd_ptr->stop.rpt_type ==  CM_CALL_FADE_F )
        {
          M1X_MSG( DCP, LEGACY_MED,
            "CM MC rpt:CM_STOP_ALERT_F, rpt_type:%d, SO:%d, error_code:%d",
            cmd_ptr->stop.rpt_type,
            cmd_ptr->stop.srv_opt,
            cmd_ptr->stop.error_code.fade_error_code);
        }
        else if( cmd_ptr->stop.rpt_type == CM_CALL_RELEASE_F )
        {
          M1X_MSG( DCP, LEGACY_MED,
            "CM MC rpt:CM_STOP_ALERT_F, rpt_type:%d, SO:%d, error_code:%d",
            cmd_ptr->stop.rpt_type,
            cmd_ptr->stop.srv_opt,
            cmd_ptr->stop.error_code.rel_error_code);
        }
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "CM MC rpt:CM_STOP_ALERT_F, rpt_type:%d, SO:%d",
            cmd_ptr->stop.rpt_type,
            cmd_ptr->stop.srv_opt);
        }
      } /* if(cdma.specific_call_end_stats_supported) */

      mcc_cm_mc_rpt( cmd_ptr );
    }
    else
    {
      ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
  }
  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "CSFB DEBUG: In CSFB mode, skipping Alert");
  }
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

} /* mccccim_stop_alert */

/*===========================================================================

FUNCTION MCCCCIM_AUTO_ANSWER

DESCRIPTION
  This function auto answers a call for a CC in WFA substate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A connect order is sent to the BS and this CC enters the CONV
  substate.

===========================================================================*/
void mccccim_auto_answer
(
  mccccim_cc_info_type * cc_info
)
{
  /* Send TC connect Order */
  mccccim_send_connect_order( cc_info );

  mccccim_handle_state_transition( cc_info, CDMA_CC_CONV);

} /* mccccim_auto_answer */

/*===========================================================================

FUNCTION MCCCCIM_WFA_SO

DESCRIPTION
  This function evalutes the connected SO ( if any) when the CC enters
  the WFA substate and decides if the call has to be auto answered

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A connect order may be sent to the BS and this CC may enter the CONV
  substate.

===========================================================================*/
void mccccim_wfa_so
(
  mccccim_cc_info_type * cc_info
)
{
  if ((cc_info->so == CAI_SO_LOOPBACK) ||
     (cc_info->so == CAI_SO_LOOPBACK_13K) ||
     (cc_info->so == CAI_SO_RS1_MARKOV) ||
     (cc_info->so == CAI_SO_RS2_MARKOV)
     || (cc_info->so == CAI_SO_MARKOV_SO54)
     #ifdef FEATURE_IS2000_1X_ADV
     || (cc_info->so == CAI_SO_MARKOV_SO74)
     #endif /* FEATURE_IS2000_1X_ADV */
     || (cc_info->so == CAI_SO_LOOPBACK_SO55)
     #ifdef FEATURE_IS2000_1X_ADV
     || (cc_info->so == CAI_SO_LOOPBACK_SO75)
     #endif /* FEATURE_IS2000_1X_ADV */

     // Data Pattern Test service options for TDSO
     || (cc_info->so == CAI_SO_TDSO)
     || (cc_info->so == CAI_SO_FULL_TDSO)
    )
  {
    /* For one of the above SOs, CCIM will auto -answer the call */
    mccccim_auto_answer( cc_info );
  }

} /* mccccim_wfa_so */

/*===========================================================================

FUNCTION MCCCCIM_WFA

DESCRIPTION
  This function does all the processing needed when a CC enters the WFA
  substate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mccccim_wfa
(
  mccccim_cc_info_type * cc_info
)
{
  /* -----------------------------------------------------------------
  ** Upon entering the Waiting for Mobile Station Answer Substate, the
  ** mobile station shall set the substate timer for T53m seconds
  ** ----------------------------------------------------------------- */

  /* set substate timer */
  (void) mcc_set_callback_timer( &cc_info->substate_timer, cai_tmo.t53m, MCC_CC_SUBSTATE_TIMER_TYPE | cc_info->cc_index );

  mccccim_wfa_so( cc_info );

} /* mccccim_wfa */

/*===========================================================================

FUNCTION MCCCCIM_CONV

DESCRIPTION
  This function does all the processing needed when a CC enters the CONV
  substate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mccccim_conv
(
  mccccim_cc_info_type * cc_info
)
{
  cm_mc_rpt_type * cm_ptr;
  /* Perform actions related to transition to CONV substate */

  /* Check for remaining digits if mobile orig call and send
     origination Continuation Message */
  #ifdef FEATURE_IS2000_REL_A
    size_t copy_size;
    /* Return value of memscpy function */
  #endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  /* Defer sending origination continuation msg for UHDM based CSFB MO calls
   * till we move to native 1X traffic state */
  if(cdma.entry_ptr->entry != MCC_CSFB)
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    if (cc_info->mob_orig && cc_info->orig_c_required)
    {
      send_orig_c_msg();

      cc_info->orig_c_required = FALSE;
    }
  } /* if(cdma.entry_ptr->entry != MCC_CSFB) */

  if (cc_info->so != CAI_SO_NULL)
  {
    /* Tell TXC we are in the conv sub state
       so that it can connect SO to vocoder */
    mcc_tx_buf.conn_so.hdr.command = TXC_CONNECT_SO_F;

    #ifdef FEATURE_IS2000_REL_A_SVD
    /* Specify the sr_id */
    mcc_tx_buf.conn_so.sr_id = cc_info->sr_id;
    #endif

    mcc_txc_cmd( &mcc_tx_buf );

    /* -------------------------------------------
    ** Send indicator to CM that call is connected
    ** ------------------------------------------- */
    /* For CSFB calls, we want to defer sending of CM_CALL_CONNECTED_F
     * till we have received Handover Complete Confirmation from CM
     */

    #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    if(cdma.entry_ptr->entry != MCC_CSFB)
    #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
    {
      if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
      {
        ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
      }
      else
      {
        cm_ptr->connect.hdr.cmd = CM_CALL_CONNECTED_F;
        cm_ptr->connect.so      = cc_info->so;
        /* For CCS, add call_id and con_ref too */
        cm_ptr->connect.call_id = cc_info->call_id;
        cm_ptr->connect.con_ref = cc_info->con_ref;

        #ifdef FEATURE_IS2000_REL_A
        /* CM needs to send QoS information to UI, in case any user priority
           is displayed */
        if (cc_info->qos_parms_incl)
        {
          cm_ptr->connect.qos_parms_incl = cc_info->qos_parms_incl;
          cm_ptr->connect.qos_parms_len = cc_info->qos_parms_len;
          if( cc_info->qos_parms_len <= CAI_ORIG_QOS_LEN_MAX )
          {
             copy_size = memscpy( cm_ptr->connect.qos_parms,
                                  CM_ORIG_QOS_LEN_MAX*sizeof(byte),
                cc_info->qos_parms,
                cc_info->qos_parms_len
              );

             if( copy_size != cc_info->qos_parms_len )
             {
               M1X_MSG( DCP, LEGACY_ERROR,
                 "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                 copy_size,
                 cc_info->qos_parms_len,
                 CM_ORIG_QOS_LEN_MAX*sizeof(byte));
             }

          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Unexpected cc_info->qos_parms_len:%d",
              cc_info->qos_parms_len);
            cm_ptr->connect.qos_parms_incl = 0 ;
          }
        }
        else
        {
          cm_ptr->connect.qos_parms_incl = 0 ;
        }
        #endif /* FEATURE_IS2000_REL_A */

        mcc_cm_mc_rpt( cm_ptr );

        /* Inform system determination of MO call connected */
        if(cc_info->mob_orig)
        {
          (void) sd_ss_ind_misc_orig_success(NULL);
        }
        
        #ifdef FEATURE_MODEM_1X_DRVCC
        /* For DRVCC MO call, send Handover Complete indication to CM */
        if (drvcc_cdma.handover_in_progress )
        {
          if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
          {
            cm_ptr->hdr.cmd = CM_1XDRVCC_HO_COMPLETE_IND_F;
        
            /* Send command to CM */
            M1X_MSG( DCP, LEGACY_HIGH,
              "Sent DRVCC Handover Complete indication to CM");
            mcc_cm_mc_rpt(cm_ptr);
          }
          drvcc_cdma.handover_in_progress = FALSE;
          /* Mark that 1X_CP is waiting for Audio Rel Indication from IMS */
          drvcc_cdma.waiting_for_audio_rel = TRUE;
        }
        #endif /* FEATURE_MODEM_1X_DRVCC */          
      }
    }
  } /* if(cdma.entry_ptr->entry != MCC_CSFB) */
} /* mccccim_conv */

/*===========================================================================

FUNCTION MCCCCIM_REL

DESCRIPTION
  This function does all the processing needed when a CC enters the Release
  substate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Release reports are sent to CM if it is a BS initiated release, if it is a
  MS initiated release corresponding primitives are sent to MCCTC.
===========================================================================*/
void mccccim_rel
(
  mccccim_cc_info_type * cc_info
)
{
  mccccim_tc_event_type mcctc_evt;
  cm_mc_rpt_type * cm_ptr;

  #ifdef FEATURE_IS2000_REL_A
  // Flush the reduced active set log.
  mccccl_submit_reduced_active_set_log();
  #endif // FEATURE_IS2000_REL_A

  /* Log the final set of frame while releasing a TDSO call */
  if ((cc_info->so == CAI_SO_TDSO) ||
      (cc_info->so == CAI_SO_FULL_TDSO))
  {
    tdso_end_call();
  }

  /* Tell TXC & RXC that we are in the rel sub state, send this command mcc_txc_cmd()
     and mcc_rxc_cmd() only for user-generated release reasons in the switch below */

  #ifdef FEATURE_IS2000_REL_A_SVD
  mcc_tx_buf.release_so.hdr.command = TXC_RELEASE_SO_F;
  /* Specify the sr_id */
  mcc_tx_buf.release_so.sr_id = cc_info->sr_id;

  mcc_rx_buf.release_so.hdr.command = RXC_RELEASE_SO_F;
  /* Specify the sr_id */
  mcc_rx_buf.release_so.sr_id = cc_info->sr_id;
  #endif

  switch (cc_info->rel_del_reason)
  {
    case MCCCCIM_BS_END:
    case MCCCCIM_REDIRECTION:
    case MCCCCIM_USER_POWERDOWN:
    {
      #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      if(cdma.entry_ptr->entry != MCC_CSFB)
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
      {
        /* -----------------------------------------------------------
        ** Send message to CM indicating BS has indicated call release
        ** ----------------------------------------------------------- */
        if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
        {
          ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
        }
        else
        {
          cm_ptr->release.hdr.cmd = CM_CALL_RELEASE_F;

          /* Always use normal reason for BS originated release */
          cm_ptr->release.rel_codes[0] = CM_CALL_REL_NORMAL;

          /* Include call_id that is being released */
          cm_ptr->release.num_call_ids = 1;
          cm_ptr->release.call_ids[0] = cc_info->call_id;

          if(cdma.specific_call_end_stats_supported)
          {
            cm_ptr->release.srv_opt[0] = cdma.so_req;
          } /* if(cdma.specific_call_end_stats_supported) */

          mcc_cm_mc_rpt( cm_ptr );
        }
      }
      #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "CSFB DEBUG: In CSFB mode, skipping Sending Call Release");
      }
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

      break;
    }

    case MCCCCIM_REL_OTASP_ERR:
    {
      /* -----------------------------------------------------------
      ** Send message to CM indicating OTASP error. Treat it as
      ** BS initiated release.
      ** ----------------------------------------------------------- */
      if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
      {
        ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
      }
      else
      {
        cm_ptr->release.hdr.cmd = CM_CALL_RELEASE_F;

        /* Always use normal reason for BS originated release */
        cm_ptr->release.rel_codes[0] = CM_CALL_REL_OTASP_SPC_ERR;

        /* Include call_id that is being released */
        cm_ptr->release.num_call_ids = 1;
        cm_ptr->release.call_ids[0] = cc_info->call_id;

       if(cdma.specific_call_end_stats_supported)
       {
         cm_ptr->release.srv_opt[0] = cdma.so_req;
       } /* if(cdma.specific_call_end_stats_supported) */

        mcc_cm_mc_rpt( cm_ptr );
      }
      break;
    }

    case MCCCCIM_SO_REJ:
    {
      #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      if(cdma.entry_ptr->entry != MCC_CSFB)
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
      {
        /* -----------------------------------------------------------
        ** Send message to CM indicating BS has indicated call release
        ** ----------------------------------------------------------- */
        if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
        {
          ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
        }
        else
        {
          cm_ptr->release.hdr.cmd = CM_CALL_RELEASE_F;

          /* Always use normal reason for BS originated release */
          cm_ptr->release.rel_codes[0] = CM_CALL_REL_SO_REJECT;

          /* Include call_id that is being released */
          cm_ptr->release.num_call_ids = 1;
          cm_ptr->release.call_ids[0] = cc_info->call_id;

          if(cdma.specific_call_end_stats_supported)
          {
            cm_ptr->release.srv_opt[0] = cdma.so_req;
          } /* if(cdma.specific_call_end_stats_supported) */

          mcc_cm_mc_rpt( cm_ptr );
        }
      }
      #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "CSFB DEBUG: In CSFB mode, sending Call Release");
      }
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
      break;
    }

    case MCCCCIM_USER_END:
    case MCCCCIM_USER_SRV_INACTIVE:
    {
      #ifdef FEATURE_IS2000_REL_A_SVD
      /* Release MVS so that MVS does not attempt to retrieve frames from MUX */
      if ( CAI_IS_SO_VOICE(cc_info->so) )
      {
        mccso_release_mvs();
      }

      /* Inform TXC and RXC about the release */
      mcc_txc_cmd( &mcc_tx_buf );
      mcc_rxc_cmd( &mcc_rx_buf );
      #endif

      /* Send notification to MCCTC */
      mcctc_evt.call_end.evt_code = MCCTC_CALL_END;
      mcctc_evt.call_end.con_ref = cc_info->con_ref ;
      mcctc_evt.call_end.rel_reason= cc_info->rel_del_reason ;
      tc_handle_ccim_event( &mcctc_evt);
      break;
    }

    case MCCCCIM_STATE_TIMER_EXP:
    {
      #ifdef FEATURE_IS2000_REL_A_SVD
      /* Release MVS so that MVS does not attempt to retrieve frames from MUX */
      if ( CAI_IS_SO_VOICE(cc_info->so) )
      {
        mccso_release_mvs();
      }

      /* Inform TXC and RXC about the release */
      mcc_txc_cmd( &mcc_tx_buf );
      mcc_rxc_cmd( &mcc_rx_buf );
      #endif

      #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      if(cdma.entry_ptr->entry != MCC_CSFB)
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
      {
        /* Need to inform CM about the release of this call */
        if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
        {
          cm_ptr->hdr.cmd = CM_CALL_FADE_F;
          cm_ptr->fade.num_call_ids = 1;
          cm_ptr->fade.call_ids[0] = cc_info->call_id;

          if(cdma.specific_call_end_stats_supported)
          {
            cm_ptr->fade.srv_opt[0] = cdma.so_req;
          } /* if(cdma.specific_call_end_stats_supported) */

          mcc_cm_mc_rpt( cm_ptr );
        }
        else
        {
          ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
        }
      }
      #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "CSFB DEBUG: In CSFB mode, skipping Call FADE");
      }
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
      break;
    }
    /* If JCDMA is not defined, treat this case statement was not present and
     * slip into default */
    case  MCCCCIM_LOW_BATTERY:
      if(mcc_passport_mode_is_jcdma())
      {
        #ifdef FEATURE_IS2000_REL_A_SVD
        /* Release MVS so that MVS does not attempt to retrieve frames from MUX */
        if ( CAI_IS_SO_VOICE(cc_info->so) )
        {
          mccso_release_mvs();
        }

        /* Inform TXC and RXC about the release */
        mcc_txc_cmd( &mcc_tx_buf );
        mcc_rxc_cmd( &mcc_rx_buf );
        #endif

        /* Send notification to MCCTC */
        mcctc_evt.call_end.evt_code = MCCTC_CALL_END;
        mcctc_evt.call_end.con_ref = cc_info->con_ref ;
        mcctc_evt.call_end.rel_reason= cc_info->rel_del_reason ;
        tc_handle_ccim_event( &mcctc_evt);
        break;
      }

    case MCCCCIM_STATE_TIMER_EXP_NO_ANS:
    default:
    {
      /* Do Nothing */
      break;
    }
  }

  if (mccccim_emer_flash_info.waiting_for_ack &&
      (cc_info->cc_index == mccccim_emer_flash_info.cc_index))
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Call Released while waiting for Emer flash ack");
    mcc_send_orig_failure(CM_CALL_ORIG_ERR_ABORT,
                          mccccim_emer_flash_info.emer_call_id,
                          CM_CALL_ORIG_ERR_LAYER_L3
                          );
    mccccim_emer_flash_info.waiting_for_ack = FALSE;
  }

} /* mccccim_rel */

/*===========================================================================

FUNCTION MCCCCIM_HANDLE_STATE_TRANSITION

DESCRIPTION
  This function handles state transitions of a CC between various
  substates.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mccccim_handle_state_transition
(
  mccccim_cc_info_type * cc_info,
  cdma_cc_state_type new_state
)
{
  if (((cc_info->substate == CDMA_CC_WFO) || (cc_info->substate == CDMA_CC_WFA)) &&
       new_state == CDMA_CC_RELEASE)
  {
    /* In these cases, we need to stop alerting first */
    /* Call_id is contained in cc_info */
    mccccim_stop_alert( cc_info );
  }
  /* Report event : CC State Change event for Release A and Call
     procesing state change event for non-RelA targets  */
  mclog_report_event_cc_state( cc_info->substate,
                               new_state, cc_info->con_ref);
  cc_info->substate = new_state;
  switch (cc_info->substate)
  {
    case CDMA_CC_WFO:
    {
      mccccim_wfo( cc_info );
    }
    break;

    case CDMA_CC_WFA:
    {
      mccccim_wfa( cc_info );
    }
    break;

    case CDMA_CC_CONV:
    {
      mccccim_conv( cc_info );
    }
    break;

    case CDMA_CC_RELEASE:
    {
      mccccim_rel( cc_info );
    }
    break;
  }  //lint !e744 no default in switch

  /* Send any log packets here, if necessary */

} /* mccccim_handle_state_transition */

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_SO_CHANGE

DESCRIPTION
  This function handles processing related to a SO change for a CC.
  For most SOs, a CM_CALL_CONNECTED_F report is sent to CM if the CC
  is in the CONV substate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mccccim_process_so_change
(
  mccccim_cc_info_type * cc_info
)
{
  cm_mc_rpt_type * cm_ptr;
  boolean send_cm_rpt = TRUE;
  size_t copy_size;
  /* Return value of memscpy function */

  /* -------------------------------------------------------
  ** Set default Rate Set option, will change below if 13K
  ** -----------------------------------------------------*/
  #if defined(FEATURE_IS95B_MDR)
  rlp_rate_set_enum_type rlp_rate_set = RLP_RATE_SET_1;
    /* Rate set in use to pass to rlp */
  #else
  rlp_mux_option_type rlp_mux = RLP_MUX_OPTION1;
    /* Holds current mux option */
  #endif

  word so = cc_info->so;  

  /* SO is probably valid now, send alert to CM if we have not sent it yet */
  if ( (!cc_info->alert_sent_to_cm) && (cc_info->so != CAI_SO_NULL) &&
       (cc_info->substate != CDMA_CC_WFO))
  {
    mccccim_incoming_call(cc_info);
  }

  #ifdef FEATURE_1X_CUST_VOICE_SO
  /* This workaround has been added to handle NV dependent non-constant
   * SO while ensuring the existing switch construct is not disrupted */
  if (so == mcc_get_cust_so() && so != CAI_SO_NULL)
  {
    so = CAI_GEN_EVRC_CUST_SO;
  } 
  #endif /* FEATURE_1X_CUST_VOICE_SO */

  switch (so)
  {
    #ifdef FEATURE_CDSMS
    case CAI_SO_SMS:
    case CAI_SO_RS2_SMS:
    #endif /* FEATURE_CDSMS */


    case CAI_SO_RS1_OTAPA:
    case CAI_SO_RS2_OTAPA:


    #ifdef FEATURE_GPSONE
    case CAI_SO_RS1_PD:
    case CAI_SO_RS2_PD:
    #endif /* FEATURE_GPSONE */

    case CAI_SO_VOICE_IS96A:
    case CAI_SO_VOICE_13K_IS733:
    case CAI_SO_VOICE_13K:
    case CAI_SO_VOICE_EVRC:
    case CAI_SO_VOICE_4GV_NB:
    case CAI_SO_VOICE_4GV_WB:
    case CAI_SO_VOICE_EVRC_NW:
    #ifdef FEATURE_1X_CUST_VOICE_SO
    case CAI_GEN_EVRC_CUST_SO:
    #endif /* FEATURE_1X_CUST_VOICE_SO */
    {
      /* For all these SOs send Rpt to CM only if we are already in CONV
      substate */
      if (cc_info->substate != CDMA_CC_CONV)
      {
        send_cm_rpt = FALSE;
      }
      break;
    }

    /* For data call establish RLP rate sets correctly */

    /* DS_CASE_DATA_RS2 is a macro containing several cases.
       It is defined in ds.h. */

    DS_CASE_DATA_RS2
      /*------------------------------------------
        Set Rate Set indicator to show Rate Set 2
      ------------------------------------------*/
      #if defined(FEATURE_IS95B_MDR)
      rlp_rate_set = RLP_RATE_SET_2;
      #else
      rlp_mux = RLP_MUX_OPTION2;
      #endif

      /* fall thru to process */
      /*lint -fallthrough */

    /* DS_CASE_DATA_RS1 is a macro containing several cases.
       It is defined in ds.h. */

    DS_CASE_DATA_RS1
      /* -------------------------------------------
      ** Data mode requested, Establish RLP Rate Set
      ** -----------------------------------------*/
      #if defined(FEATURE_IS95B_MDR)
      /* Take care of initialization of RLP 1 and 2 here.  For RLP type 3, connection
         initializer is used so no need to do it here.*/
      if( DS_ISDATA_RLP_TYPE2(cc_info->so) )
      {
        /* This is a service option that uses RLP Type 2. Note that the traffic type
           is hard coded to primary, for now */
        rlp_establish_srvc_type(RLP_TRAFFIC_TYPE_PRI, rlp_rate_set, RLP_TYPE_II);
      }
      else
#ifdef FEATURE_IS2000_SCH
      if ( !DS_ISDATA_RLP_TYPE3(cc_info->so) )
#endif // FEATURE_IS2000
      {
        /* This is a service option that uses RLP Type 1. Note that the traffic type
           is hard coded to primary, for now */
        rlp_establish_srvc_type(RLP_TRAFFIC_TYPE_PRI, rlp_rate_set, RLP_TYPE_I);
      }
      #else
      rlp_establish_rate_set( rlp_mux);  /* Old RLP interface */
      #endif /* if defined(FEATURE_IS95B_MDR) */

      M1X_MSG( DCP, LEGACY_MED,
        "Data call requested" );
      /* For all these SOs send Rpt to CM only if we are already in CONV
      substate */
      if (cc_info->substate != CDMA_CC_CONV)
      {
        send_cm_rpt = FALSE;
      }

      break;

    case CAI_SO_NULL:
    {
      send_cm_rpt = FALSE;
      break;
    }

  } //lint !e744, No default needed

  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry == MCC_CSFB)
	 {
    /* If we are in CSFB mode, we will send reports to CM
     * only when we have received HANDOVER CNF. Hence
     * defer sending any report to CM until then.
     */
    send_cm_rpt = FALSE;
  }
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

  if ( send_cm_rpt )
  {
    /* -------------------------------------------
    ** Send indicator to CM that call is connected
    ** ------------------------------------------- */
    if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
    {
      ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
    else
    {
      cm_ptr->connect.hdr.cmd = CM_CALL_CONNECTED_F;
      cm_ptr->connect.so      = cc_info->so;
      cm_ptr->connect.call_id = cc_info->call_id;
      cm_ptr->connect.con_ref = cc_info->con_ref;

      #ifdef FEATURE_IS2000_REL_A
      /* CM needs to send QoS information to UI, in case any user priority
         is displayed */
      if (cc_info->qos_parms_incl)
      {
        cm_ptr->connect.qos_parms_incl = cc_info->qos_parms_incl;
        cm_ptr->connect.qos_parms_len = cc_info->qos_parms_len;
        if( cc_info->qos_parms_len <= CAI_ORIG_QOS_LEN_MAX )
        {
          copy_size =memscpy
                     ( cm_ptr->connect.qos_parms, CM_ORIG_QOS_LEN_MAX*sizeof(byte),
                       cc_info->qos_parms, cc_info->qos_parms_len
            );
          if( copy_size != cc_info->qos_parms_len )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
              copy_size,
              cc_info->qos_parms_len,
              CM_ORIG_QOS_LEN_MAX*sizeof(byte));
          }
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unexpected qos_parms_len:%d",
            cc_info->qos_parms_len);
          cm_ptr->connect.qos_parms_incl = 0 ;
        }
      }
      else
      {
        cm_ptr->connect.qos_parms_incl = 0 ;
      }
      #endif /* FEATURE_IS2000_REL_A */

      mcc_cm_mc_rpt( cm_ptr );

      /* Inform system determination of MO call connected */
      if(cc_info->mob_orig)
      {
        (void) sd_ss_ind_misc_orig_success(NULL);
      }
      
      #ifdef FEATURE_MODEM_1X_DRVCC
      /* For DRVCC MO call, send Handover Complete indication to CM */
      if (drvcc_cdma.handover_in_progress )
      {
        if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
        {
          cm_ptr->hdr.cmd = CM_1XDRVCC_HO_COMPLETE_IND_F;
      
          /* Send command to CM */
          M1X_MSG( DCP, LEGACY_HIGH,
                   "Sent DRVCC Handover Complete indication to CM");
          mcc_cm_mc_rpt(cm_ptr);
        }     
        drvcc_cdma.handover_in_progress = FALSE;
        /* Mark that 1X_CP is waiting for Audio Rel Indication from IMS */
        drvcc_cdma.waiting_for_audio_rel = TRUE;     
      }
      #endif /* FEATURE_MODEM_1X_DRVCC */          
    }
  }

} /* mccccim_process_so_change */

/*===========================================================================

FUNCTION MCCCCIM_DELETE_CC

DESCRIPTION
  This function does processing associated with the deletion of a CC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  All the CC specific timers are cleared.
===========================================================================*/
void mccccim_delete_cc
(
  mccccim_cc_info_type * cc_info
)
{
  cm_mc_rpt_type * cm_ptr;

  if ((cc_info->substate == CDMA_CC_WFO) || (cc_info->substate == CDMA_CC_WFA))
  {
    /* Stop alerting, if ringing is still going on */
    /* For CCS, call_id is needed as a parameter */
    mccccim_stop_alert( cc_info );
  }

  /* Do processing associated with deletion of a CC */
  cc_info->active = FALSE;

  /* Clear Substate timer associated with this CC instance */
  (void )mcc_clr_callback_timer( &cc_info->substate_timer, MCC_CC_SUBSTATE_TIMER_TYPE | cc_info->cc_index);

  /* Clear dtmf timer associated with this CC instance */
  (void )mcc_clr_callback_timer( &cc_info->dtmf_timer, MCC_CC_DTMF_TIMER_TYPE | cc_info->cc_index);

  M1X_MSG( DCP, LEGACY_MED,
    "deleted CC con_ref : %d due to %d",
    cc_info->con_ref,
    cc_info->rel_del_reason);

  #ifdef FEATURE_IS2000_REL_A_SVD
  /* Free the SR_ID occupied by this CC */
  /* This call is only effective if the SR_ID is owned by MC. The call to SR_ID
  ** manager after this for SO update takes care of the DS, or NULL ownership.
  */
  mccsrid_free_srid(cc_info->sr_id, SRID_OWNER_MC);
  #endif /* FEATURE_IS2000_REL_A_SVD */

  if (cc_info->substate != CDMA_CC_RELEASE)
  {
    /* This CC was not in a REL substate, Depeneidng on the reason
    for deletion, we may have to inform CM about this deletion,
    Note: If the CC is already in Release substate then notification would
    already have been sent to CM */

    switch (cc_info->rel_del_reason)
    {
      case MCCCCIM_FADE :
      {
        #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
        if(cdma.entry_ptr->entry != MCC_CSFB)
        #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
        {
          /* Need to inform CM about the release of this call */
          if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
          {
            cm_ptr->hdr.cmd = CM_CALL_FADE_F;
            cm_ptr->fade.num_call_ids = 1;
            cm_ptr->fade.call_ids[0] = cc_info->call_id;

            if(cdma.specific_call_end_stats_supported)
            {
              cm_ptr->fade.srv_opt[0] = cdma.so_req;

              /* Check for the call end reason */
              switch( cdma.call_end_reason )
              {
                case MC_FADE_RL_ACK_TIMEOUT:
                {
                  /* Fade the call Ack timeout due to 12 retransmission*/
                  M1X_MSG( DCP, LEGACY_MED,
                    "CM MC rpt:CM_CALL_FADE_F, call_end_reason = CM_FADE_RL_ACK_TIMEOUT, SO:%d",
                    cm_ptr->fade.srv_opt[0]);
                  cm_ptr->fade.call_end_fade_reason[0] = CM_FADE_RL_ACK_TIMEOUT;
                  break;
                }

                case MC_FADE_BAD_FL:
                {
                  /* Fade the call Bad FWD link or T5M expiry*/
                  M1X_MSG( DCP, LEGACY_MED,
                    "CM MC rpt:CM_CALL_FADE_F with call_end_reason = CM_FADE_BAD_FL, SO:%d",
                    cm_ptr->fade.srv_opt[0]);
                  cm_ptr->fade.call_end_fade_reason[0] = CM_FADE_BAD_FL;
                  break;
                }

                default:
                {
                  cm_ptr->fade.call_end_fade_reason[0] = CM_FADE_GENERIC;
                  break;
                }
              } /* End of switch( cdma.call_end_reason ) */

              /* Reset cdma.call_end_reason global variable to default value */
              cdma.call_end_reason = MC_CALL_ORIG_ERR_MAX;
            } /* if(cdma.specific_call_end_stats_supported) */

            mcc_cm_mc_rpt( cm_ptr );
          }
          else
          {
            ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
          }
        }
        break;
      }

      case MCCCCIM_LOCK:
      {
        /* No need to do anything for this case, MCCTC sends a lock command
        to CM which makes them drop all calls anyway and stay "locked" */
        break;
      }

      case MCCCCIM_BS_END:
      case MCCCCIM_REDIRECTION:
      case MCCCCIM_HHO_FAIL:
      {
        #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
        if(cdma.entry_ptr->entry != MCC_CSFB)
        #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
        {
          /* -----------------------------------------------------------
          ** Send message to CM indicating BS has indicated call release
          ** ----------------------------------------------------------- */
          if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
          {
            ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
          }
          else
          {
            cm_ptr->release.hdr.cmd = CM_CALL_RELEASE_F;

            /* Always use normal reason for BS originated release */
            cm_ptr->release.rel_codes[0] = CM_CALL_REL_NORMAL;

            /* Include call_id that is being released */
            cm_ptr->release.num_call_ids = 1;
            cm_ptr->release.call_ids[0] = cc_info->call_id;

            if(cdma.specific_call_end_stats_supported)
            {
              cm_ptr->release.srv_opt[0] = cdma.so_req;
            } /* if(cdma.specific_call_end_stats_supported) */

            mcc_cm_mc_rpt( cm_ptr );
          }
        }
        #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "CSFB DEBUG: In CSFB mode, skipping sending Release");
        }
        #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
        break;
      }

      default:
      {
        M1X_MSG( DCP, LEGACY_MED,
          "delete reason %d, Do nothing",
          cc_info->rel_del_reason);
        break;
      }
    }

  }
  if (mccccim_emer_flash_info.waiting_for_ack &&
      (cc_info->cc_index == mccccim_emer_flash_info.cc_index))
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Call terminated while waiting for Emer flash ack");
    mcc_send_orig_failure(CM_CALL_ORIG_ERR_ABORT,
                          mccccim_emer_flash_info.emer_call_id,
                          CM_CALL_ORIG_ERR_LAYER_L3
                          );
    mccccim_emer_flash_info.waiting_for_ack = FALSE;
  }

  // Inform SNM about the deletion of the Call control instance so that
  // it can reset the state of that connection in its database.
  snm_update_for_deleted_connection(cc_info->sr_id);

  #ifdef FEATURE_IS2000_REL_A
  event_report_payload(EVENT_CALL_CONTROL_TERMINATED,
    sizeof(event_call_control_terminated_type),
    (void *) &(cc_info->con_ref));
  #endif

} /* mccccim_delete_cc */

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_RESET_WFO

DESCRIPTION
  This function handles the "reset WFO" primitive coming from MCCTC.
  All CCs which are in WFO substate get their timers reset. Refer to section
  2.6.10 in IS2000A-5 (Rel A signaling std)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mccccim_process_reset_wfo
(
  mccccim_event_type * event_ptr
)
{
  /* All CC instances in the WFO substate have to reset their
     substate timers */
  uint8 i;

  for (i=0; i < MCCCCIM_NUM_MAX_CCS; i++)
  {
    if (  mccccim_cc_info[i].active &&
         (mccccim_cc_info[i].substate == CDMA_CC_WFO)
       )
    {
      /* Reset the substate timer */
      (void) mcc_set_callback_timer( &mccccim_cc_info[i].substate_timer, cai_tmo.t52m, MCC_CC_SUBSTATE_TIMER_TYPE | i );
    }
  } /* for */

} /* mccccim_process_reset_wfo */ /*lint !e715 event_ptr not referenced is okay */

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_CREATE_CC

DESCRIPTION
  This function handles the "instantiation of a CC" primitive
  coming from MCCTC. The CC eneters the corresponding substate based on
  the "bypass_alert_ans" .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mccccim_process_create_cc
(
  mccccim_event_type * event_ptr
)
{
  uint8 i;

  /* First find an empty entry in the CC table */
  for (i=0; i < MCCCCIM_NUM_MAX_CCS; i++)
  {
    if (!mccccim_cc_info[i].active)
    {
      break;
    }
  }

  if (i == MCCCCIM_NUM_MAX_CCS)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No BUffers available for a new CC");
    event_ptr->create_cc.status = MCCCCIM_ERR;
    return;
  }

  M1X_MSG( DCP, LEGACY_HIGH,
    "Creating new CC instance at index %d",
    i);

  /* If we are here, it means we found an entry for this new CC */
  mccccim_cc_info[i].active = TRUE;
  mccccim_cc_info[i].call_id = event_ptr->create_cc.call_id;
  mccccim_cc_info[i].con_ref = event_ptr->create_cc.con_ref;
  mccccim_cc_info[i].sr_id = event_ptr->create_cc.sr_id;
  mccccim_cc_info[i].bypass_alert_ans = event_ptr->create_cc.bypass_alert_ans;
  mccccim_cc_info[i].mob_orig = event_ptr->create_cc.mob_orig;
  mccccim_cc_info[i].temp_so = event_ptr->create_cc.temp_so;
  mccccim_cc_info[i].is_null_cc = event_ptr->create_cc.is_null_cc;
  mccccim_cc_info[i].orig_c_required = event_ptr->create_cc.orig_c_required;
  mccccim_cc_info[i].so = CAI_SO_NULL;

  /* Initialize DTMF related variables */
  mccccim_cc_info[i].dtmf_stop = FALSE;
  mccccim_cc_info[i].dtmf_ack_wait = FALSE;
  mccccim_cc_info[i].cont_dtmf = 0xff;

  if (CAI_IS_SO_VOICE(mccccim_cc_info[i].temp_so))
  {
    /* Set default capacity operating point to 0 */
    byte rate = 0;

    /* Set default capacity operating point for SO73 to 4 */
    if (mccccim_cc_info[i].temp_so == CAI_SO_VOICE_EVRC_NW)
    {
      rate = 4;
    }

    #ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
    /* Reset any previously active rate reduction on Voice SO init */
    /* To be compatible with the old rate reduction parameters in
       the driver we need to multiply the new factor by 2 for MVS */
    (void) mccso_set_avg_encoder_rate(rate);

    #endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

   (void) rate; /* To satisfy the compiler warnings. */
  }

  if (mccccim_cc_info[i].mob_orig)
  {
    mccccim_cc_info[i].substate = CDMA_CC_CONV;
    mccccim_cc_info[i].alert_sent_to_cm = TRUE;
  }
  else
  {
    if (mccccim_cc_info[i].bypass_alert_ans)
    {
      mccccim_cc_info[i].substate = CDMA_CC_CONV;
      M1X_MSG( DCP, LEGACY_HIGH,
        "Skipping Alert and Answer States");
      if ((mccccim_cc_info[i].temp_so == CAI_SO_NULL) &&
          (mccccim_cc_info[i].so == CAI_SO_NULL)
         )
      {
        /* If we have no idea which type of call this is, CM or
           its clients have no way of knowing who shd pick up
           this call, so just postpone this until later */
        /* This can only happen due to a BS initiated CLAM
           with so_incl set to 0 */

        mccccim_cc_info[i].alert_sent_to_cm = FALSE;
      }
      else
      {
        mccccim_incoming_call( &mccccim_cc_info[i]);
        /* The above fn would mark alert_sent_to_cm as 1 */
      }
    }
    else
    {
      mccccim_cc_info[i].substate = CDMA_CC_WFO;
      mccccim_cc_info[i].alert_sent_to_cm = FALSE;
    }
  }

  /* Report CC created event */
  mclog_report_event_cc_create( event_ptr->create_cc.con_ref,
                                mccccim_cc_info[i].substate);

  /* Handle transition into new state */
  mccccim_handle_state_transition( &mccccim_cc_info[i], mccccim_cc_info[i].substate );

  event_ptr->create_cc.status = MCCCCIM_DONE;

} /* mccccim_process_create_cc */

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_UPDATE_CONREC

DESCRIPTION
  This function processes the connection records of a SCR and updates the
  SO and sr_id of the corresponding CC instance.  It also deletes CC not found
  in connection records anymore.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Some CCs not in con_recs may be deleted.
===========================================================================*/
void mccccim_process_update_conrec
(
  mccccim_event_type * event_ptr
)
{
  uint8 i,j, cc_index;
  size_t copy_size;
  /* Return value of memscpy function */

  #ifdef FEATURE_IS2000_REL_A
  mclog_event_cc_conref_change_type cc_event;
  #endif

  const caii_srv_cfg_type * new_cfg_ptr = event_ptr->update_conrec.new_cfg_ptr;

  for (i=0; i < MCCCCIM_NUM_MAX_CCS; i++)
  {
    if (!mccccim_cc_info[i].active)
    {
      continue;
    }
    /* Check if we have a CC with con_ref=0, if that is the case, this is the first
    service connection and hence we have to replace con_ref=0 with the con_ref
    in the first connection record */
    if (mccccim_cc_info[i].con_ref == MCCCCIM_NULL_CON_REF )
    {
      if (new_cfg_ptr->num_con)
      {
        #ifdef FEATURE_IS2000_REL_A
        cc_event.old_con_ref = 0;
        cc_event.new_con_ref =  new_cfg_ptr->con[0].con_ref; //lint !e713, loss of precision
        /* Report CC conref changed event */
        event_report_payload(EVENT_CALL_CONTROL_CONREF_CHANGE,
                             sizeof(mclog_event_cc_conref_change_type),
                             (void *) &cc_event);
        #endif

        mccccim_cc_info[i].con_ref = new_cfg_ptr->con[0].con_ref;
        break;
      }
    }

    /* this index "i" is active, if p_rev_in_use is less than or equal
    to 7, then there is a possibility the con_ref could change even though
    it still refers to the same call, so just cheat here and assign the
    new con_ref to the existing CC so that we do not treat the "old call"
    as being gone. For p_rev_in_use < 7, SNM guarantees there is only
    one connection record */
    /*lint -e506 */
    if (
        #ifdef FEATURE_IS2000_REL_A
        !P_REV_IS_7_OR_GREATER &&
        #endif /* FEATURE_IS2000_REL_A */
        ( mccccim_cc_info[i].con_ref != new_cfg_ptr->con[0].con_ref)
       )
    /*lint +e506 */
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "con_ref changed for existing CC old/new con_ref: %d %d",
        mccccim_cc_info[i].con_ref,
        new_cfg_ptr->con[0].con_ref);

      #ifdef FEATURE_IS2000_REL_A
      cc_event.old_con_ref =  mccccim_cc_info[i].con_ref; //lint !e713, loss of precision
      cc_event.new_con_ref =  new_cfg_ptr->con[0].con_ref; //lint !e713, loss of precision
      /* Report CC conref changed event */
      event_report_payload(EVENT_CALL_CONTROL_CONREF_CHANGE,
                       sizeof(mclog_event_cc_conref_change_type),
                       (void *) &cc_event);
      #endif

      mccccim_cc_info[i].con_ref = new_cfg_ptr->con[0].con_ref;
    }
  }

  /* Update SO for all active control instances based on con_rec */
  for (i=0; i < new_cfg_ptr->num_con; i++)
  {
    cc_index = mccccim_get_cc_index_from_con_ref( new_cfg_ptr->con[i].con_ref);
    /* Check the cc_index range check to fix the Klocwork error */
    if ((cc_index != MCCCCIM_CON_REF_NOT_FOUND) && (cc_index < MCCCCIM_NUM_MAX_CCS))
    {
      if (mccccim_cc_info[cc_index].so != new_cfg_ptr->con[i].so)
      {
        mccccim_cc_info[cc_index].so = new_cfg_ptr->con[i].so;
        if (CAI_IS_SO_VOICE(mccccim_cc_info[cc_index].so))
        {
          /* Set default capacity operating point to 0 */
          byte rate = 0;

          /* Set default capacity operating point for SO73 to 4 */
          if (mccccim_cc_info[cc_index].so == CAI_SO_VOICE_EVRC_NW)
          {
            rate = 4;
          }

          #ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
          /* Reset rate reduction if changing Voice SOs */
          /* To be compatible with the old rate reduction parameters in
             the driver we need to multiply the new factor by 2 for MVS */
          (void) mccso_set_avg_encoder_rate(rate);
          if ( (mccccim_cc_info[cc_index].so == CAI_SO_VOICE_EVRC_NW) ||
               (mccccim_cc_info[cc_index].so == CAI_SO_VOICE_4GV_WB) )
          {
            if(rate == 0)
            {
              cdma.codec_rate = 16000;
            }
            else
            {
              cdma.codec_rate = 8000;
            }
          }
          else
          {
            cdma.codec_rate = 8000;
          }

          /* update codec information to upper layers */
          mcc_cm_mc_rpt2(SYS_SPEECH_CODEC_INFO);
          #endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

          (void) rate; /* To satisfy the compiler warnings. */
        }
      }

      if (i==0)
      {
        /* The first connection record will be identified as NULL */
         mccccim_cc_info[cc_index].is_null_cc = TRUE;
         M1X_MSG( DCP, LEGACY_MED,
           "Null CC is SO %d",
           mccccim_cc_info[cc_index].so);
      }
      else
      {
        mccccim_cc_info[cc_index].is_null_cc = FALSE;
      }

      if (mccccim_cc_info[cc_index].sr_id != new_cfg_ptr->con[i].sr_id)
      {

        #ifdef FEATURE_IS2000_REL_A_SVD
        /* If SR_ID is the initial value 0, there is no need to free it */
        if( mccccim_cc_info[cc_index].sr_id !=0 )
        {
          /* For CCS, try to deallocate the old sr_id, Note if the owner is not
          MC the sr_id will not  become "AVAILABLE" but that is okay */
          mccsrid_free_srid(mccccim_cc_info[cc_index].sr_id, SRID_OWNER_MC);
        }
        #endif /* FEATURE_IS2000_REL_A_SVD */

        mccccim_cc_info[cc_index].sr_id =  new_cfg_ptr->con[i].sr_id;
      }

      #ifdef FEATURE_IS2000_REL_A
      /* For every connection QoS may have been included in SCR
         If so, save it to convey to CM for user display of
         User Priority, etc.  */
      if ((new_cfg_ptr->con[i].qos_parms_incl) &&
          ( (int)new_cfg_ptr->con[i].qos_parms_len <= CAI_ORIG_QOS_LEN_MAX))
      {
        mccccim_cc_info[cc_index].qos_parms_incl = 1;
        mccccim_cc_info[cc_index].qos_parms_len = new_cfg_ptr->con[i].qos_parms_len;
        copy_size = memscpy
          ( mccccim_cc_info[cc_index].qos_parms,
                      CAI_ORIG_QOS_LEN_MAX*sizeof(byte),
            new_cfg_ptr->con[i].qos_parms,
            new_cfg_ptr->con[i].qos_parms_len
          );

        if( copy_size !=  new_cfg_ptr->con[i].qos_parms_len )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
            copy_size,
            new_cfg_ptr->con[i].qos_parms_len,
            CAI_ORIG_QOS_LEN_MAX*sizeof(byte));
        }
      }
      else
      {
        mccccim_cc_info[cc_index].qos_parms_incl = 0 ;
      }
      #endif /* FEATURE_IS2000_REL_A */

      M1X_MSG( DCP, LEGACY_MED,
        "SO %d connected for conref %d ",
        mccccim_cc_info[cc_index].so,
        mccccim_cc_info[cc_index].con_ref);
      mccccim_process_so_change( &mccccim_cc_info[cc_index] );
    }
  }

  /* Delete CCs which are not part of con_rec any more */

  for (i=0 ; i < MCCCCIM_NUM_MAX_CCS ; i++)
  {
    if (!mccccim_cc_info[i].active)
    {
      continue;
    }
    for (j=0; (j < new_cfg_ptr->num_con) && (j < CAII_SRV_CON_MAX); j++)
    {
      if (mccccim_cc_info[i].active &&
         ( mccccim_cc_info[i].con_ref == new_cfg_ptr->con[j].con_ref))
      {
        break;
      }
    }
    if ( j == new_cfg_ptr->num_con)
    {
      M1X_MSG( DCP, LEGACY_MED,
        " con_ref %d not found, Deleting this CC",
        mccccim_cc_info[i].con_ref);
      mccccim_cc_info[i].rel_del_reason = MCCCCIM_BS_END;
      mccccim_delete_cc( &mccccim_cc_info[i]);
    }
  }


  if( (new_cfg_ptr->num_con) < CAII_SRV_CON_MAX )
  {
  #ifdef FEATURE_IS2000_REL_A_SVD
    /* Update SR_ID manager with the new SCR */
     mccsrid_update_srid(new_cfg_ptr);
  #endif /* FEATURE_IS2000_REL_A_SVD */

    snm_update_with_new_config( (caii_srv_cfg_type *) new_cfg_ptr);

    event_ptr->update_conrec.status = MCCCCIM_DONE;
  }
} /* mccccim_process_update_conrec */

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_UPDATE_SO

DESCRIPTION
  This function processes the change of SO of a CC due to service
  Option Negotiation.

DEPENDENCIES
  There should only be one CC left at this point.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mccccim_process_update_so
(
  mccccim_event_type * event_ptr
)
{
  uint8 j; /* Loop variables */
  /* This is received when the BS used Serv Option Negotiation
     to connect a new SO, In this case, just connect that with the
     only SO that is active */

  for ( j=0; j < MCCCCIM_NUM_MAX_CCS ; j++)
  {
    if (mccccim_cc_info[j].active)
    {
      mccccim_cc_info[j].so =  event_ptr->update_so.so;
      /* Any CC existing by SO negotiation can only have con_ref =1 and
      must be identifed by NULL, so force these on this CC */
      mccccim_cc_info[j].con_ref = 1;
      mccccim_cc_info[j].is_null_cc = 1;

      if (CAI_IS_SO_VOICE(mccccim_cc_info[j].so))
      {
        /* Set default capacity operating point to 0 */
        byte rate = 0;

        /* Set default capacity operating point for SO73 to 4 */
        if (mccccim_cc_info[j].so == CAI_SO_VOICE_EVRC_NW)
        {
          rate = 4;
        }

        #ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
        /* Reset rate reduction if changing Voice SOs */
        /* To be compatible with the old rate reduction parameters in
           the driver we need to multiply the new factor by 2 for MVS */
        (void) mccso_set_avg_encoder_rate(rate);
        #endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

        (void) rate; /* To satisfy the compiler warnings. */

        if ( (mccccim_cc_info[j].so == CAI_SO_VOICE_EVRC_NW) || (mccccim_cc_info[j].so == CAI_SO_VOICE_4GV_WB) )
        {
          if(rate == 0)
          {
            cdma.codec_rate = 16000;
          }
          else
          {
            cdma.codec_rate = 8000;
          }
        }
        else
        {
          cdma.codec_rate = 8000;
        }
        /* update codec information to upper layers */
        mcc_cm_mc_rpt2(SYS_SPEECH_CODEC_INFO);

      }

      M1X_MSG( DCP, LEGACY_MED,
        "SO %d connected for CC index %d ",
        mccccim_cc_info[j].so,
        j);
      mccccim_process_so_change( &mccccim_cc_info[j] );
      break;
    }
  }

  event_ptr->update_so.status = MCCCCIM_DONE;

} /* mccccim_process_update_so */

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_DELETE_ALL_CC

DESCRIPTION
  This function deletes all active CCs and sends appropriate
  notifications, if any, based on the delete reason.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mccccim_process_delete_all_cc
(
  mccccim_event_type * event_ptr
)
{
  uint8 i;

  /* Delete all active CC instances */
  for (i=0; i < MCCCCIM_NUM_MAX_CCS ; i++)
  {
    if (mccccim_cc_info[i].active)
    {
      /* Remember the reason for deletion */
      mccccim_cc_info[i].rel_del_reason = event_ptr->del_all_cc.del_reason;
      mccccim_delete_cc( &mccccim_cc_info[i]);
    }
  }

  event_ptr->del_all_cc.status = MCCCCIM_DONE;

} /* mccccim_process_delete_all_cc */

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_DELETE_CC

DESCRIPTION
  This function deletes an active CCs based on the primitive from
  LL3 (MCCTC) and sends appropriate notifications, if any, based
  on the delete reason.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mccccim_process_delete_cc
(
  mccccim_event_type * event_ptr
)
{
  uint8 cc_index;

  /* Search for CC identified by this con_ref and delete it */
  cc_index = mccccim_get_cc_index_from_con_ref( event_ptr->del_cc.con_ref);

  /* Check the cc_index range check to fix the Klocwork error */
  if ((cc_index != MCCCCIM_CON_REF_NOT_FOUND) && (cc_index < MCCCCIM_NUM_MAX_CCS))
  {
    mccccim_cc_info[cc_index].rel_del_reason = event_ptr->del_cc.del_reason;
    mccccim_delete_cc( &mccccim_cc_info[cc_index]);

  }

  event_ptr->del_cc.status = MCCCCIM_DONE;

} /* mccccim_process_delete_cc */

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_RELEASE_ALL_CC

DESCRIPTION
  This function puts all active CCs in Release substate, and sends
  appropriate notifications, if any, based on the release reason.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mccccim_process_release_all_cc
(
  mccccim_event_type * event_ptr
)
{
  uint8 i;

  for (i=0; i < MCCCCIM_NUM_MAX_CCS ; i++)
  {
    /* Loop through and release all CCs which are not in release
    substate already */
    if (mccccim_cc_info[i].active && (mccccim_cc_info[i].substate != CDMA_CC_RELEASE))
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Releasing CC index: %d, due to %d",
        i,
        event_ptr->rel_all_cc.rel_reason);
      mccccim_cc_info[i].rel_del_reason = event_ptr->rel_all_cc.rel_reason;
      mccccim_handle_state_transition(&mccccim_cc_info[i], CDMA_CC_RELEASE);
    }
  }

  event_ptr->rel_all_cc.status = MCCCCIM_DONE;

} /* mccccim_process_release_all_cc */

/*===========================================================================

FUNCTION MCCCCIM_CHANGE_TO_SO_NEG

DESCRIPTION
  This function does processing related to change in negotiation
  type from service negotoation to "Service Option" negotiation,
  When this happens, according to IS2000A-5, L3 has to delete all
  CCs except the one with "primary" traffic.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Some CCs may be deleted.
===========================================================================*/
void mccccim_change_to_so_neg
(
  mccccim_event_type * event_ptr
)
{
  uint8 i,j;
  const caii_srv_cfg_type * cfg_ptr;

  cfg_ptr = snm_get_current_config();

  /* If we move to SO negotiation, MS is required to terminate all
     CC instances which has non-primary traffic and identify the CC with primary
     traffic as a null con_ref CC, Reference 2.6.4.1.2.2.1 of Release A std */
  for (i = 0; i < MCCCCIM_NUM_MAX_CCS; i++)
  {
    if (!mccccim_cc_info[i].active)
    {
      continue;
    }
    for (j = 0; j < cfg_ptr->num_con ; j++)
    {
      if (cfg_ptr->con[j].con_ref == mccccim_cc_info[i].con_ref)
      {
        if (!(cfg_ptr->con[j].fwd_traf == CAI_PRI_TRAF))  /*lint !e641 */
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "Deleting con_ref %d which does not use pri traffic",
            mccccim_cc_info[i].con_ref);
          mccccim_cc_info[i].rel_del_reason = MCCCCIM_BS_END;
          mccccim_delete_cc( &mccccim_cc_info[i]);
        }
        else
        {
          /* Identify this CC by null now */
          mccccim_cc_info[i].is_null_cc = 1;
          mccccim_cc_info[i].con_ref = 1;
        }
        break;
      }
    }

    if (j == cfg_ptr->num_con)
    {
      /* This shd never happen, this is a sanity check */
      M1X_MSG( DCP, LEGACY_ERROR,
        "con_ref in CCIM not in SCR %d",
        cfg_ptr->con[j].con_ref);
      mccccim_cc_info[i].rel_del_reason = MCCCCIM_BS_END;
      mccccim_delete_cc( &mccccim_cc_info[i]);
    }
  }

  event_ptr->gen.status = MCCCCIM_DONE;

} /* mccccim_change_to_so_neg */

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_MAINT_ORDER

DESCRIPTION
  This function processes the "Maintenance Order" received from the
  BS.

DEPENDENCIES
  None.

RETURN VALUE
  Status to be returned to MCCTC.

SIDE EFFECTS
  The CC will enter the WFA substate if it is already not in
  "Release substate"
===========================================================================*/
mccccim_event_status_type mccccim_process_maint_order
(
  mccccim_cc_info_type * cc_ptr,
  caii_rx_msg_type * msg_ptr
)
{
  mccccim_event_status_type status = MCCCCIM_DONE;
  switch (cc_ptr->substate)
  {
    case CDMA_CC_WFO:
    {
      mccccim_incoming_call( cc_ptr );
      mccccim_handle_state_transition( cc_ptr, CDMA_CC_WFA );
      break;
    }

    case CDMA_CC_CONV:
    {
     /* -------------------------------------------------------------------
     ** If it is a data call, then substate should not be changed to WFA
     ** -----------------------------------------------------------------*/
      if(!CAI_IS_SO_DATA(cc_ptr->so))
      {
        cc_ptr->bypass_alert_ans = 0;
        mccccim_incoming_call( cc_ptr );
        mccccim_handle_state_transition( cc_ptr, CDMA_CC_WFA );
      }
      break;
    }

    case CDMA_CC_WFA:
    {
      /* Just reset the substate timer */
      (void) mcc_set_callback_timer( &cc_ptr->substate_timer, cai_tmo.t53m, MCC_CC_SUBSTATE_TIMER_TYPE | cc_ptr->cc_index );
      break;
    }

    case CDMA_CC_RELEASE:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Maint Order not allowed in this state");
      status = MCCCCIM_MSG_REJECT_STATE;
      break;
    }
  }  //lint !e744, No default needed

  return status;

} /* mccccim_process_maint_order */ /*lint !e715 msg_ptr nor referenced */

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_ALERT_MSG

DESCRIPTION
  This function processes the "Alert with Info"  or "Ext
  Alert with Info" msg received from the BS.

DEPENDENCIES
  None.

RETURN VALUE
  Status to be returned to MCCTC.

SIDE EFFECTS
  The CC will enter the WFA substate . Notification is sent to CM.
===========================================================================*/
mccccim_event_status_type mccccim_process_alert_msg
(
  mccccim_cc_info_type * cc_ptr,
  caii_rx_msg_type * msg_ptr
)
{
  /* Struct for sending primitive to MCCTC */
  mccccim_tc_event_type mcctc_evt;
  mccccim_event_status_type status = MCCCCIM_DONE;
  byte msg_type = msg_ptr->gen.msg_type;

  switch( cc_ptr->substate )
  {
    case CDMA_CC_WFO:
    {
      mccccim_incoming_call( cc_ptr);

      if (msg_type == CAI_ALERT_MSG)
      {
        /* For CCS, call_id also has to be passed in this interface */
        (void)mcc_process_info_records(
                                 cc_ptr->call_id,
                                 msg_ptr->alert.msg_type,
                                 msg_ptr->alert.num_recs,
                                 (byte *)msg_ptr->alert.recs );
      }
      #ifdef FEATURE_IS2000_REL_A
      else
      {
        /* This is Ext. Alert with Info message */

        /* For CCS, call_id also has to be passed in this interface */
        (void)mcc_process_info_records(
                                  cc_ptr->call_id,
                                  msg_ptr->tceawim.msg_type,
                                  msg_ptr->tceawim.num_recs,
                                  (byte *)msg_ptr->tceawim.recs );
      }
      #endif

      mccccim_handle_state_transition( cc_ptr, CDMA_CC_WFA);
      
      /* For MT voice call case send codec_info report again after sending 
       * incoming_call to CM because CM will ingore codec_info which we sent 
       * after processing Service Connect because of no incoming_call 
       * call object initialized from CM side.
       * This fix is required where customer might be using 3rd party 
       * audio solution */
      if(CAI_IS_SO_VOICE(cc_ptr->so) == TRUE)
      {
        mcc_cm_mc_rpt2(SYS_SPEECH_CODEC_INFO);
      }
       
      break;
    }

    case CDMA_CC_WFA:
    {
      if (msg_type == CAI_ALERT_MSG)
      {
        /* For CCS, call_id also has to be passed in this interface */
        (void)mcc_process_info_records(
                                 cc_ptr->call_id,
                                 msg_ptr->alert.msg_type,
                                 msg_ptr->alert.num_recs,
                                 (byte *)msg_ptr->alert.recs );
      }
      #ifdef FEATURE_IS2000_REL_A
      else
      {
        /* This is Ext. Alert with Info message */

        /* For CCS, call_id also has to be passed in this interface */
        (void)mcc_process_info_records(
                                  cc_ptr->call_id,
                                  msg_ptr->tceawim.msg_type,
                                  msg_ptr->tceawim.num_recs,
                                  (byte *)msg_ptr->tceawim.recs );
      }
      #endif

      /* reset substate timer */
      (void) mcc_set_callback_timer( &cc_ptr->substate_timer, cai_tmo.t53m, MCC_CC_SUBSTATE_TIMER_TYPE | cc_ptr->cc_index );
      break;
    }

    case CDMA_CC_CONV:
    {
      if (msg_type == CAI_ALERT_MSG)
      {
        /* -------------------------------------------------------------------
        ** If it is a data call, then substate should not be changed to WFA
        ** -----------------------------------------------------------------*/
        if ( (!CAI_IS_SO_DATA(cc_ptr->so)) && (mcc_check_alert_signal_info( msg_ptr->alert.msg_type,
                                       msg_ptr->alert.num_recs,
                                       (byte *)msg_ptr->alert.recs )) )
        {
          /* -------------------------------------------------------------
          ** If the message  contains a Signal information record with the
          ** SIGNAL_TYPE field set to '01' or '10', or if the message does
          ** not contain a Signal information record, the mobile station
          ** shall enter the Waiting For Mobile Station Answer Substate.
          ** ------------------------------------------------------------- */
          cc_ptr->bypass_alert_ans = 0;
          mccccim_incoming_call( cc_ptr);
          /* For CCS, call_id also has to be passed in this interface */
          (void)mcc_process_info_records(
                                 cc_ptr->call_id,
                                 msg_ptr->alert.msg_type,
                                 msg_ptr->alert.num_recs,
                                 (byte *)msg_ptr->alert.recs );
          mccccim_handle_state_transition( cc_ptr, CDMA_CC_WFA);
        }
        else
        {
          /* Process the records even if CC did not change substate */
          /* For CCS, call_id also has to be passed in this interface */
          (void)mcc_process_info_records(
                                 cc_ptr->call_id,
                                 msg_ptr->alert.msg_type,
                                 msg_ptr->alert.num_recs,
                                 (byte *)msg_ptr->alert.recs );
        }
      }
      #ifdef FEATURE_IS2000_REL_A
      else
      {
        /* This is an Ext alert with Info msg */

        /* -------------------------------------------------------------------
        ** If it is a data call, then substate should not be changed to WFA
        ** -----------------------------------------------------------------*/
        if ( (!CAI_IS_SO_DATA(cc_ptr->so))  && (mcc_check_alert_signal_info( msg_ptr->tceawim.msg_type,
                                         msg_ptr->tceawim.num_recs,
                                         (byte *)msg_ptr->tceawim.recs )) )
        {
          /* -------------------------------------------------------------
          ** If the message  contains a Signal information record with the
          ** SIGNAL_TYPE field set to '01' or '10', or if the message does
          ** not contain a Signal information record, the mobile station
          ** shall enter the Waiting For Mobile Station Answer Substate.
          ** ------------------------------------------------------------- */
          cc_ptr->bypass_alert_ans = 0;
          mccccim_incoming_call( cc_ptr);
          /* For CCS, call_id also has to be passed in this interface */
         (void)mcc_process_info_records(
                                  cc_ptr->call_id,
                                  msg_ptr->tceawim.msg_type,
                                  msg_ptr->tceawim.num_recs,
                                  (byte *)msg_ptr->tceawim.recs );
          mccccim_handle_state_transition( cc_ptr, CDMA_CC_WFA);
        }
        else
        {
          /* Process the records even if CC did not change substate */
          /* For CCS, call_id also has to be passed in this interface */
         (void)mcc_process_info_records(
                                  cc_ptr->call_id,
                                  msg_ptr->tceawim.msg_type,
                                  msg_ptr->tceawim.num_recs,
                                  (byte *)msg_ptr->tceawim.recs );
        }
      }
      #endif

      break;
    }

    case CDMA_CC_RELEASE:
    {
      cc_ptr->call_id = cm_call_id_allocate();

      cc_ptr->bypass_alert_ans = 0;
      /* Let CM know about the incoming call */
      mccccim_incoming_call( cc_ptr);
      if (msg_type == CAI_ALERT_MSG)
      {
        /* For CCS, call_id also has to be passed in this interface */
        (void)mcc_process_info_records(
                                 cc_ptr->call_id,
                                 msg_ptr->alert.msg_type,
                                 msg_ptr->alert.num_recs,
                                 (byte *)msg_ptr->alert.recs );
      }
      #ifdef FEATURE_IS2000_REL_A
      else
      {
        /* This is Ext. Alert with Info message */

        /* For CCS, call_id also has to be passed in this interface */
        (void)mcc_process_info_records(
                                  cc_ptr->call_id,
                                  msg_ptr->tceawim.msg_type,
                                  msg_ptr->tceawim.num_recs,
                                  (byte *)msg_ptr->tceawim.recs );
      }
      #endif

      mccccim_handle_state_transition( cc_ptr, CDMA_CC_WFA);
      /* Notify MCCTC */
      mcctc_evt.gen.evt_code = MCCTC_ENTER_TC_STATE;
      tc_handle_ccim_event( &mcctc_evt );
      break;
    }

  } /*lint !e744 no default needed */

  return status;

} /* mccccim_process_alert_msg */

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_DTMF_ORDER

DESCRIPTION
  This function processes the "DTMF Order" msg received from the BS.

DEPENDENCIES
  None.

RETURN VALUE
  Status to be returned to MCCTC.

SIDE EFFECTS
  None.
===========================================================================*/
mccccim_event_status_type mccccim_process_dtmf_order
(
  mccccim_cc_info_type * cc_ptr,
  caii_rx_msg_type * msg_ptr
)
{
  cm_mc_rpt_type * cm_ptr;
  mccccim_event_status_type status = MCCCCIM_DONE;

  switch (cc_ptr->substate)
  {
    case CDMA_CC_CONV:
    {
      if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
      {
        if (msg_ptr->tc_ord.ordq.ordq == CAI_STOP_CONT_DTMF)
        {
          cm_ptr->dtmf_cont_stop.hdr.cmd = CM_DTMF_CONT_STOP_F;
          cm_ptr->dtmf_cont_stop.call_id =  cc_ptr->call_id;

        }
        else
        {
          cm_ptr->dtmf_cont_start.hdr.cmd = CM_DTMF_CONT_START_F;
          cm_ptr->dtmf_cont_start.dtmf_code = msg_ptr->tc_ord.ordq.ordq;
          cm_ptr->dtmf_cont_start.call_id =  cc_ptr->call_id;
        }

        mcc_cm_mc_rpt( cm_ptr );
       }
      else
      {
        ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
      }

      break;
    }

    default:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DTMF Cont Order not allowed in this state");
      status = MCCCCIM_MSG_REJECT_STATE;
      break;
    }
  }

  return status;

} /* mccccim_process_dtmf_order */

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_CC_MSG

DESCRIPTION
  This function processes Call Control Specific messages received
  from LL3 ( MCCTC).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mccccim_process_cc_msg( mccccim_event_type * event_ptr )
{
  uint8 cc_index, con_ref;

  uint8 i;
  cm_mc_rpt_type * cm_ptr;
  caii_rx_msg_type * msg_ptr = event_ptr->cc_msg.msg_ptr;

  event_ptr->cc_msg.status = MCCCCIM_DONE;

  M1X_MSG( DCP, LEGACY_MED,
      "Rxed message type: %d", msg_ptr->gen.msg_type );

  switch (msg_ptr->gen.msg_type)
  {
    case CAI_TC_FWD_ORD_MSG:
    {
#ifdef FEATURE_IS2000_REL_A
      /* First extract the con_ref of the CC we are searching for */
      if (P_REV_IS_7_OR_GREATER && msg_ptr->tc_ord.ordq.con_ref_incl)
      {
        con_ref = msg_ptr->tc_ord.ordq.con_ref;
      }
      else
      {
        con_ref = MCCCCIM_NULL_CON_REF;
      }
#else
      con_ref = MCCCCIM_NULL_CON_REF;
#endif /* FEATURE_IS2000_REL_A */

      /* Now get the cc_index for this con_ref */
      cc_index = mccccim_get_cc_index_from_con_ref(con_ref);

      /* check the cc_index valid range value to fix the Klocwork error */
      if ((cc_index == MCCCCIM_CON_REF_NOT_FOUND) || (cc_index >= MCCCCIM_NUM_MAX_CCS))
      {
        /* CC is not found return an error status to MCCTC */
        M1X_MSG( DCP, LEGACY_ERROR,
          " CC not found , con_ref : %d ",
          con_ref);
        event_ptr->cc_msg.status = MCCCCIM_CC_NOT_FOUND;
        event_ptr->cc_msg.con_ref = con_ref;
        break;
      }

      M1X_MSG( DCP, LEGACY_MED,
            "Rxed Order message: %d", msg_ptr->tc_ord.gen.order );

      switch (msg_ptr->tc_ord.gen.order)
      {
        case CAI_MAINT_ORD:
          M1X_MSG( DCP, LEGACY_LOW,
            "Rxed Maintenance Order" );

          event_ptr->cc_msg.status =
            mccccim_process_maint_order( &mccccim_cc_info[cc_index], msg_ptr);
          event_ptr->cc_msg.con_ref = con_ref;
          break;

        case CAI_DTMF_ORD:
          /* -----------
          ** Continuous DTMF Order
          ** ----------- */
          M1X_MSG( DCP, LEGACY_LOW,
            "Rxed Continuous DTMF Order" );
          event_ptr->cc_msg.status =
            mccccim_process_dtmf_order( &mccccim_cc_info[cc_index], msg_ptr);
          event_ptr->cc_msg.con_ref = con_ref;

          break;

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unknown Order type %d fwded to  MCCCCIM",
            msg_ptr->tc_ord.gen.order);
          event_ptr->cc_msg.status = MCCCCIM_ERR;
          break;

      }

      break;
    }

    case CAI_ALERT_MSG:
    {
      /* ------------------------------
      ** Alert With Information Message
      ** ------------------------------ */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Alert w/Info Msg" );
      con_ref = MCCCCIM_NULL_CON_REF;

      /* Now get the cc_index for this con_ref */
      cc_index = mccccim_get_cc_index_from_con_ref(con_ref);

      /* check the cc_index valid range value to fix the Klocwork error */
      if ((cc_index == MCCCCIM_CON_REF_NOT_FOUND) || (cc_index >= MCCCCIM_NUM_MAX_CCS))
      {
        /* CC is not found return an error status to MCCTC */
        M1X_MSG( DCP, LEGACY_ERROR,
          " CC not found , con_ref : %d ",
          con_ref);
        event_ptr->cc_msg.status = MCCCCIM_CC_NOT_FOUND;
        event_ptr->cc_msg.con_ref = con_ref;
      }
      else
      {
        event_ptr->gen.status =
           mccccim_process_alert_msg( &mccccim_cc_info[cc_index], msg_ptr);
        event_ptr->cc_msg.con_ref = con_ref;
      }

      break;
    }

#ifdef FEATURE_IS2000_REL_A
    case CAI_EXT_ALERT_W_INFO_MSG:
    {
      /* ------------------------------
      ** Ext.Alert With Information Message
      ** ------------------------------ */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Ext Alert w/Info Msg" );
      /* First extract the con_ref of the CC we are searching for */
      if (P_REV_IS_7_OR_GREATER && msg_ptr->tceawim.con_ref_incl)
      {
        con_ref = msg_ptr->tceawim.con_ref;
      }
      else
      {
        con_ref = MCCCCIM_NULL_CON_REF;
      }
      /* Now get the cc_index for this con_ref */
      cc_index = mccccim_get_cc_index_from_con_ref(con_ref);

     /* check the cc_index valid range value to fix the Klocwork error */
      if ((cc_index == MCCCCIM_CON_REF_NOT_FOUND) || (cc_index >= MCCCCIM_NUM_MAX_CCS))
      {
        /* CC is not found return an error status to MCCTC */
        M1X_MSG( DCP, LEGACY_ERROR,
          " CC not found , con_ref : %d ",
          con_ref);
        event_ptr->cc_msg.status = MCCCCIM_CC_NOT_FOUND;
        event_ptr->cc_msg.con_ref = con_ref;
      }
      else
      {
        event_ptr->gen.status =
           mccccim_process_alert_msg( &mccccim_cc_info[cc_index], msg_ptr);
        event_ptr->cc_msg.con_ref = con_ref;
      }
      break;
    }
#endif

    case CAI_SEND_BURST_MSG :
    {
            /* -----------
      ** Send Burst DTMF Order
      ** ----------- */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed Send Burst DTMF Order" );


#ifdef FEATURE_IS2000_REL_A
      /* First extract the con_ref of the CC we are searching for */
      if (P_REV_IS_7_OR_GREATER && msg_ptr->sbdtmf.con_ref_incl)
      {
        con_ref = msg_ptr->sbdtmf.con_ref;
      }
      else
      {
        con_ref = MCCCCIM_NULL_CON_REF;
      }
#else
      con_ref = MCCCCIM_NULL_CON_REF;
#endif /* FEATURE_IS2000_REL_A */

      /* Now get the cc_index for this con_ref */
      cc_index = mccccim_get_cc_index_from_con_ref(con_ref);

      /* check the cc_index valid range value to fix the Klocwork error */
      if ((cc_index == MCCCCIM_CON_REF_NOT_FOUND) || (cc_index >= MCCCCIM_NUM_MAX_CCS))
      {
        /* CC is not found return an error status to MCCTC */
        M1X_MSG( DCP, LEGACY_ERROR,
          " CC not found , con_ref : %d ",
          con_ref);
        event_ptr->cc_msg.status = MCCCCIM_CC_NOT_FOUND;
        event_ptr->cc_msg.con_ref = con_ref;
        break;
      }

      if (mccccim_cc_info[cc_index].substate != CDMA_CC_CONV)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Msg type %d cannot be handled by MCCCCIM in %d ",
          msg_ptr->gen.msg_type,
          mccccim_cc_info[cc_index].substate);
        event_ptr->cc_msg.status = MCCCCIM_MSG_REJECT_STATE;
        event_ptr->cc_msg.con_ref = con_ref;
        break;
      }

      if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
      {
        cm_ptr->dtmf.hdr.cmd    = CM_DTMF_BURST_F;
        /* Add call_id to this interface when we support CCS */
        cm_ptr->dtmf.call_id    = mccccim_cc_info[cc_index].call_id;
        cm_ptr->dtmf.dtmf_on    = msg_ptr->sbdtmf.dtmf_on_length;
        cm_ptr->dtmf.dtmf_off   = msg_ptr->sbdtmf.dtmf_off_length;

        if (msg_ptr->sbdtmf.num_digits > CM_MAX_DATA_DIGITS)
        {
          cm_ptr->dtmf.cnt = CM_MAX_DATA_DIGITS;
        }
        else
        {
          cm_ptr->dtmf.cnt = msg_ptr->sbdtmf.num_digits;
        }

        for (i=0; i < cm_ptr->dtmf.cnt; i++)
        {
          cm_ptr->dtmf.digits[i] = msg_ptr->sbdtmf.digit[i];
        }

        mcc_cm_mc_rpt( cm_ptr );
      }
      else
      {
        ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
      }

      break;
    }

    case CAI_FWD_FLASH_MSG:
    {
      /* ------------------------------
      ** Flash with Information Message
      ** ------------------------------ */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Flash w/Info Msg" );

      /* Flash with Info does not have con_ref, so this has to be processed
         by the NULL con_ref CC */
      con_ref = MCCCCIM_NULL_CON_REF;

      /* Now get the cc_index for this con_ref */
      cc_index = mccccim_get_cc_index_from_con_ref(con_ref);

      /* check the cc_index valid range value to fix the Klocwork error */
      if ((cc_index == MCCCCIM_CON_REF_NOT_FOUND) || (cc_index >= MCCCCIM_NUM_MAX_CCS))
      {
        /* CC is not found return an error status to MCCTC */
        M1X_MSG( DCP, LEGACY_ERROR,
          " CC not found , con_ref : %d ",
          con_ref);
        event_ptr->cc_msg.status = MCCCCIM_CC_NOT_FOUND;
        event_ptr->cc_msg.con_ref = con_ref;
        return;
      }

      if (mccccim_cc_info[cc_index].substate != CDMA_CC_CONV)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Msg type %d cannot be handled by MCCCCIM in %d ",
          msg_ptr->gen.msg_type,
          mccccim_cc_info[cc_index].substate);
        event_ptr->cc_msg.status = MCCCCIM_MSG_REJECT_STATE;
        event_ptr->cc_msg.con_ref = con_ref;
        return;
      }

      /* --------------------------------------------------------
      ** The Flash with Information Message must contain at least
      ** one information record.
      ** -------------------------------------------------------- */
      if (msg_ptr->alert.num_recs > 0)
      {
        /* For CCS, call_id also has to be passed in this interface */
        (void) mcc_process_info_records(
                                         mccccim_cc_info[cc_index].call_id,
                                         msg_ptr->alert.msg_type,
                                         msg_ptr->alert.num_recs,
                                         (byte *) msg_ptr->alert.recs );
      }
      else
      {
        event_ptr->cc_msg.status = MCCCCIM_MSG_REJECT_STRUCT;
        event_ptr->cc_msg.con_ref = con_ref;
        return;
      }
      break;

    }

#ifdef FEATURE_IS2000_REL_A
    case CAI_EXT_FLASH_W_INFO_MSG:
    {
            /* ------------------------------
      ** Flash with Information Message
      ** ------------------------------ */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Ext. Flash w/Info Msg" );

      /* First extract the con_ref of the CC we are searching for */
      if (P_REV_IS_7_OR_GREATER && msg_ptr->tceawim.con_ref_incl)
      {
        con_ref = msg_ptr->tceawim.con_ref;
      }
      else
      {
        con_ref = MCCCCIM_NULL_CON_REF;
      }
      /* Now get the cc_index for this con_ref */
      cc_index = mccccim_get_cc_index_from_con_ref(con_ref);

      /* check the cc_index valid range value to fix the Klocwork error */
      if ((cc_index == MCCCCIM_CON_REF_NOT_FOUND) || (cc_index >= MCCCCIM_NUM_MAX_CCS))
      {
        /* CC is not found return an error status to MCCTC */
        M1X_MSG( DCP, LEGACY_ERROR,
          " CC not found , con_ref : %d ",
          con_ref);
        event_ptr->cc_msg.status = MCCCCIM_CC_NOT_FOUND;
        event_ptr->cc_msg.con_ref = con_ref;
        return;
      }

      if (mccccim_cc_info[cc_index].substate != CDMA_CC_CONV)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Msg type %d cannot be handled by MCCCCIM in %d ",
          msg_ptr->gen.msg_type,
          mccccim_cc_info[cc_index].substate);
        event_ptr->cc_msg.status = MCCCCIM_MSG_REJECT_STATE;
        event_ptr->cc_msg.con_ref = con_ref;
        return;
      }

      /* --------------------------------------------------------
      ** The Flash with Information Message must contain at least
      ** one information record.
      ** -------------------------------------------------------- */
      if (msg_ptr->tceawim.num_recs > 0)
      {
        /* For CCS, call_id also has to be passed in this interface */
        (void) mcc_process_info_records(
                                         mccccim_cc_info[cc_index].call_id,
                                         msg_ptr->tceawim.msg_type,
                                         msg_ptr->tceawim.num_recs,
                                         (byte *)msg_ptr->tceawim.recs );
      }
      else
      {
        event_ptr->cc_msg.status = MCCCCIM_MSG_REJECT_STRUCT;
        event_ptr->cc_msg.con_ref = con_ref;
        return;
      }

      break;
    }
#endif

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Msg type %d cannot be handled by MCCCCIM ",
        msg_ptr->gen.msg_type);
      event_ptr->cc_msg.status = MCCCCIM_ERR;
      break;

  }

  return;

}

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_AHDM

DESCRIPTION
  This function does the processing needed for a Analog Handoff
  Direction Message.  The AHDM may be rejected if the con_ref is
  invalid, or if the call is a data call.  If the AHDM is accepted,
  all CC instances except the one mentioned in AHDM are deleted.
  (Note the one mentiones in AHDM is also freed up from MCCCCIM
  but that is not equivalent to "deleting" a call.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Some CCs could be deleted.
===========================================================================*/
void mccccim_process_ahdm
(
  mccccim_event_type * event_ptr
)
{
  byte con_ref,i;

  #ifdef FEATURE_IS2000_REL_A
  caii_fm_ho_type * ahdm_ptr = event_ptr->ahdm_msg.msg_ptr;
  #endif /* FEATURE_IS2000_REL_A */

  boolean delete_cc;
  uint8 cc_index;

  #ifdef FEATURE_IS2000_REL_A
  if (P_REV_IS_7_OR_GREATER && ahdm_ptr->con_ref_incl)
  {
    con_ref = ahdm_ptr->con_ref;
  }
  else
  #endif
  {
    /* If con_ref is not present in AHDM, we prserve the NULL CC */
    con_ref = MCCCCIM_NULL_CON_REF;
  }

  cc_index = mccccim_get_cc_index_from_con_ref(con_ref);

  /* check the cc_index range check to fix the Klocwork error */
  if ((cc_index == MCCCCIM_CON_REF_NOT_FOUND) || (cc_index >= MCCCCIM_NUM_MAX_CCS))
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "AHDM received for non-existent call");
    event_ptr->ahdm_msg.status = MCCCCIM_ERR;
  }
  else if (DS_ISDATA(mccccim_cc_info[cc_index].so))
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "AHDM received for data call");
    event_ptr->ahdm_msg.status = MCCCCIM_ERR;
  }
  else
  {
    /* Delete all CCs except the one specified by the AHDM */
    for (i=0; i<MCCCCIM_NUM_MAX_CCS; i++)
    {
      if (!mccccim_cc_info[i].active)
      {
        continue;
      }

      delete_cc = FALSE;

      if (i == cc_index)
      {
        event_ptr->ahdm_msg.substate = mccccim_cc_info[i].substate;
        event_ptr->ahdm_msg.call_id = mccccim_cc_info[i].call_id;
        /* This CC is not gone but for our CDMA state machine purpose, we need
           to free up this entry, so do so */
        mccccim_cc_info[i].active = FALSE;
        /* Clear CC specific state timers and DTMF timers here for CCS*/

        /* Clear Substate timer associated with this CC instance */
        (void )mcc_clr_callback_timer( &mccccim_cc_info[i].substate_timer, MCC_CC_SUBSTATE_TIMER_TYPE | mccccim_cc_info[i].cc_index);

        /* Clear dtmf timer associated with this CC instance */
        (void )mcc_clr_callback_timer( &mccccim_cc_info[i].dtmf_timer, MCC_CC_DTMF_TIMER_TYPE | mccccim_cc_info[i].cc_index);
      }
      else
      {
        delete_cc = TRUE;
      }

      if (delete_cc)
      {
        mccccim_cc_info[i].rel_del_reason = MCCCCIM_BS_END;
        mccccim_delete_cc( &mccccim_cc_info[i] );
      }
    }

    event_ptr->ahdm_msg.status = MCCCCIM_DONE;
  }

} /* mccccim_process_ahdm */

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_EMER_CALL

DESCRIPTION
  This function checks if an emergency call origination received
  from CM can be flashed on top of any CC instance.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mccccim_process_emer_call
(
  mccccim_event_type * event_ptr
)
{
  uint8 i,j;
  mc_flash_type flash;
  word so_to_judge_call;

  event_ptr->emer.status = MCCCCIM_CC_NOT_FOUND;

  for (i=0; i<MCCCCIM_NUM_MAX_CCS; i++)
  {
    if (mccccim_cc_info[i].active && (mccccim_cc_info[i].substate != CDMA_CC_RELEASE))
    {
      /* Check to see if the SO or temp_so is a Voice SO,
      Note, we are assuming here that if temp_so is a voice SO
      then the eventual call will be a voice call, should be a valid assumption
      */
      if (mccccim_cc_info[i].so == CAI_SO_NULL)
      {
        so_to_judge_call = mccccim_cc_info[i].temp_so;
      }
      else
      {
        so_to_judge_call = mccccim_cc_info[i].so;
      }
      if (so_to_judge_call != CAI_SO_NULL)
      {
        if (CAI_IS_SO_VOICE(so_to_judge_call))
        {
          event_ptr->emer.status = MCCCCIM_DONE;
          if (mccccim_emer_flash_info.waiting_for_ack ||
              (mccccim_cc_info[i].substate == CDMA_CC_WFO) ||
              ((mccccim_cc_info[i].substate == CDMA_CC_WFA)
               #ifdef FEATURE_IS2000_REL_A
               && !P_REV_IS_7_OR_GREATER  //lint !e506
               #endif /* FEATURE_IS2000_REL_A */
              )
             )
          {
            /* Note the std does allow us to send this flash in WFO substate but
            if we do that and we get L2 ack from BS for this while we are still in
            WFO substate, CM cannot notify UI that the call has been flashed, as UI
            has no idea about this call yet, so to simplify things just send back an
            ABORT error code to CM, CM will retry */
            M1X_MSG( DCP, LEGACY_HIGH,
              "Already waiting for Ack on an emergency flash or in WFO substate");
            mcc_send_orig_failure(CM_CALL_ORIG_ERR_CALL_COLLISION,
                                  event_ptr->emer.cm_msg_ptr->call_id,
                                  CM_CALL_ORIG_ERR_LAYER_L3
                                  );
            return;
          }
          /* Populate mc_flash_type data struct here */
          /* This data struct is used just so that we can re-use the
          mccccim_send_flash_info_msg function   */
          flash.hdr.cmd = MC_FLASH_F;

          #ifdef FEATURE_IS2000_REL_A
          flash.flash_type = CM_CALL_SUPS_TYPE_EMERGENCY_CALL;
          flash.digit_mode = event_ptr->emer.cm_msg_ptr->digit_mode;
          flash.number_type = event_ptr->emer.cm_msg_ptr->number_type;
          flash.number_plan = event_ptr->emer.cm_msg_ptr->number_plan;
          #endif // FEATURE_IS2000_REL_A

          flash.cnt = event_ptr->emer.cm_msg_ptr->cnt;

          for (j=0; j<flash.cnt; j++)
          {
            flash.called_address[j] = event_ptr->emer.cm_msg_ptr->called_address[j];
          }

          if (mccccim_cc_info[i].is_null_cc)
          {
            mccccim_send_flash_info_msg( (mc_msg_type * )&flash, &mccccim_cc_info[i] );  /*lint !e826*/
          }
          else
          {
            #ifdef FEATURE_IS2000_REL_A_SVD
            mccccim_send_ext_flash_info_msg( (mc_msg_type *)&flash, &mccccim_cc_info[i] );  /*lint !e826*/
            #endif
          }

          M1X_MSG( DCP, LEGACY_HIGH,
            "Emergency call flashed on top of Call Id %d, SO: %d",
            mccccim_cc_info[i].call_id,
            so_to_judge_call);

          /* Do some book keeping now */
          mccccim_emer_flash_info.emer_call_id = event_ptr->emer.cm_msg_ptr->call_id;
          mccccim_emer_flash_info.waiting_for_ack = TRUE;
          mccccim_emer_flash_info.cc_index = i;
        } /* if (CAI_IS_SO_VOICE(so_to_judge_call)) */
        else
        {
          event_ptr->emer.status = MCCCCIM_DONE;
        
          /* If the current MT call is not a Voice Call then
           * Reject E911 Orig request with CM_CALL_ORIG_ERR_CALL_COLLISION
           * So that CM will end the current MT call once we send 
           * "CM_INCOMING_CALL_F" to CM and then CM will retry E911 orig */
          M1X_MSG( DCP, LEGACY_HIGH,
            "Rej E911 orig with CALL_COLLISION err, MT Non-Voice SO:%d",
             so_to_judge_call);
          mcc_send_orig_failure( CM_CALL_ORIG_ERR_CALL_COLLISION,
                                 event_ptr->emer.cm_msg_ptr->call_id,
                                 CM_CALL_ORIG_ERR_LAYER_L3
                               );
          return;
        } /* Not Voice Call */
      } /* if(so_to_judge_call != CAI_SO_NULL) */
    }
  } //for loop

  return;

} /* mccccim_process_emer_call */

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_NDSS

DESCRIPTION
  This function  figures out which is the call that has to be
  re-originated for NDSS. Usually it is the call that was "last"
  originated.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The call to be re-originated is deleted from CCIM data base
  after a notification has been sent to CM.
===========================================================================*/
void mccccim_process_ndss
(
  mccccim_event_type * event_ptr
)
{
  /* If CCIM get this event, it means the BS is trying to redirect
  one of the existing calls , i.e the last MS originated call, so
  delete that call and send a "REDIRECTION_F" notification to CM */

  /* It is not common for the BS to redirect an existing call especially
  if there are multiple calls, For simplicity, just pick the MS orig
  call with the highest CC index as the call to be re-originated */

  uint8 i;
  cm_mc_rpt_type * cm_ptr;
  uint8 cc_index = MCCCCIM_CON_REF_NOT_FOUND;

  for (i=0; i < MCCCCIM_NUM_MAX_CCS; i++)
  {
    if (mccccim_cc_info[i].active && mccccim_cc_info[i].mob_orig)
    {
      cc_index = i;
    }
  }

  if ((cc_index != MCCCCIM_CON_REF_NOT_FOUND) &&
      (mccccim_cc_info[cc_index].substate != CDMA_CC_RELEASE) )
  {
    /* Tell CM to re-originate this call */
    /* Send NDSS Redirection command to UI */
    if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cm_ptr->hdr.cmd = CM_NDSS_REDIRECTION_F;
      /* With CCS, also send call_id to CM */
      cm_ptr->ndss_redirection.call_id = mccccim_cc_info[cc_index].call_id;
      
      #ifdef CM_API_OTA_CNAM_INFO
      /* As of now MCCCCIM_NDSS event is set only while processing
       * CAI_FTC_SRV_REDIR_MSG so filling corresponding CM OTA msg Type */
      cm_ptr->ndss_redirection.hdr.msg_type = CM_MSG_TYPE_FTSRDM;
      M1X_MSG( DCP, LEGACY_MED,
               "Info_Rec_Type:%d, MC OTA Msg Type:0x%x, CM OTA Msg Type:%d",
               cm_ptr->hdr.cmd,
               CAI_FTC_SRV_REDIR_MSG, 
               cm_ptr->ndss_redirection.hdr.msg_type );
      #endif /* CM_API_OTA_CNAM_INFO */
      
      mcc_cm_mc_rpt( cm_ptr );
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No free buffers on ui_cmd_free_q" );
    }

    /* Now inactivate or delete this CC */
    mccccim_cc_info[cc_index].rel_del_reason = MCCCCIM_USER_END;

    /* Note : The call is just being redirected not really "gone", so we
    just do clean up here w/o explicitly calling delete_cc function */

    mccccim_cc_info[cc_index].active = FALSE;

      /* Clear Substate timer associated with this CC instance */
    (void )mcc_clr_callback_timer( &mccccim_cc_info[cc_index].substate_timer, MCC_CC_SUBSTATE_TIMER_TYPE | cc_index);

    /* Clear dtmf timer associated with this CC instance */
    (void )mcc_clr_callback_timer( &mccccim_cc_info[cc_index].dtmf_timer, MCC_CC_DTMF_TIMER_TYPE | cc_index);

  }

} /* mccccim_process_ndss */ /*lint !e715 event_ptr not refernced is okay */

/*===========================================================================

FUNCTION MCCCCIM_INITIALIZE_TIMERS

DESCRIPTION
  This function initializes timers that are call-control specific.
  This should only be called once and is invoked from mcc_powerup_init()
  in mccdma.c

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccccim_initialize_timers
(
  void
)
{
  byte i;

    /* Initialize Call Control Instance Timers */
  for (i=0; i < MCCCCIM_NUM_MAX_CCS; i++)
  {
    rex_def_timer_ex( &(mccccim_cc_info[i].dtmf_timer),
                      mcc_expired_timer_callback_func,
                      MCC_CC_DTMF_TIMER_TYPE | i       /* OR the Index to TYPE for identifying timer */
                    );
    rex_def_timer_ex( &(mccccim_cc_info[i].substate_timer),
                      mcc_expired_timer_callback_func,
                      MCC_CC_SUBSTATE_TIMER_TYPE | i   /* OR the Index to TYPE for identifying timer */
                    );
  }

} /* mccccim_initiaize_timers */


/*===========================================================================

FUNCTION MCCCCIM_INITIALIZE

DESCRIPTION
  This function resets the CCIM database. Good to call this as soon
  MCCTC enters TC init.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mccccim_initialize
(
  void
)
{
  uint8 i;
  for (i=0; i < MCCCCIM_NUM_MAX_CCS; i++)
  {
    mccccim_cc_info[i].active = FALSE;
    mccccim_cc_info[i].cc_index = i;
  }

  mccccim_emer_flash_info.waiting_for_ack = FALSE;

} /* mccccim_initialize */

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_EVENT

DESCRIPTION
  This function processes primitives received from LL3 (MCCTC).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mccccim_process_event
(
  mccccim_event_type * event_ptr
)
{
  switch (event_ptr->gen.event_code)
  {
    case MCCCCIM_RESET_WFO:
    {
      mccccim_process_reset_wfo( event_ptr );
      break;
    }

    case MCCCCIM_CREATE_CC:
    {
      mccccim_process_create_cc( event_ptr);
      break;
    }

    case MCCCCIM_UPDATE_CON_RECS:
    {
      mccccim_process_update_conrec( event_ptr);
      break;
    }

    case MCCCCIM_UPDATE_SO:
    {
      mccccim_process_update_so( event_ptr );
      break;
    }

    case MCCCCIM_DELETE_ALL_CC:
    {
      mccccim_process_delete_all_cc( event_ptr );
      break;
    }

    case MCCCCIM_DELETE_CC:
    {
      mccccim_process_delete_cc( event_ptr );
      break;
    }

    case MCCCCIM_RELEASE_ALL_CC:
    {
      mccccim_process_release_all_cc( event_ptr );
      break;
    }

    case MCCCCIM_CC_MSG:
    {
      mccccim_process_cc_msg ( event_ptr );
      break;
    }

    case MCCCCIM_ANALOG_HO:
    {
      mccccim_process_ahdm( event_ptr );
      break;
    }

    case MCCCCIM_HANDLE_EMER_CALL:
    {
      mccccim_process_emer_call( event_ptr );
      break;
    }

    case MCCCCIM_NDSS:
    {
      mccccim_process_ndss( event_ptr );
      break;
    }

    case MCCCCIM_SO_NEG:
    {
      mccccim_change_to_so_neg( event_ptr );
      break;
    }

    default:
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Unknown event received by MCCCCIM %d",
        event_ptr->gen.event_code);
      event_ptr->gen.status = MCCCCIM_ERR;
      break;
    }
  }

} /* mccccim_process_event */

/*===========================================================================

FUNCTION MCCCCIM_SEND_FLASH_INFO_MSG

DESCRIPTION
  This function sends the flash with information message during traffic
  channel processing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccccim_send_flash_info_msg
(
  mc_msg_type *msg_ptr,
  /* pointer to message received from other other tasks */
  mccccim_cc_info_type * cc_ptr
)
{
  byte * rec_ptr; /* ptr to the information record of the msg */
  rxtx_cmd_type *rxtx_cmd_ptr;
  int i;  /* counter */

  #ifdef FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB
  #error code not present
#endif /* FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB */

  /* Send Flash With Information Message */
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((rxtx_cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX_MC_Q" );
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((rxtx_cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    rxtx_cmd_ptr->tc_msg.msg.flash.msg_type = CAI_REV_FLASH_MSG;
    rec_ptr = (byte *) &rxtx_cmd_ptr->tc_msg.msg.flash.recs[0];

    #ifdef FEATURE_IS2000_REL_A
    if (msg_ptr->flash.flash_type == CM_CALL_SUPS_TYPE_EMERGENCY_CALL)
    {
      if (P_REV_IS_7_OR_GREATER)
      {
        rxtx_cmd_ptr->tc_msg.msg.flash.num_recs = 1;

        ((caii_global_emerg_type *) rec_ptr)->hdr.record_type =
           CAI_GLOB_EM_CALL_REC;

        /* MS won't initiate a Position Location Determination Session */
        ((caii_global_emerg_type *) rec_ptr)->ms_orig_pos_loc_ind = 0;

        if (msg_ptr->flash.cnt)
        {
          ((caii_global_emerg_type *) rec_ptr)->num_char = msg_ptr->flash.cnt;
          ((caii_global_emerg_type *) rec_ptr)->num_incl = 1;
          ((caii_global_emerg_type *) rec_ptr)->digit_mode = msg_ptr->flash.digit_mode;
          ((caii_global_emerg_type *) rec_ptr)->number_type = msg_ptr->flash.number_type;
          ((caii_global_emerg_type *) rec_ptr)->number_plan = msg_ptr->flash.number_plan;

          for (i=0; i < msg_ptr->flash.cnt; i++)
          {
            ((caii_global_emerg_type *) rec_ptr)->chari[i] =
                 msg_ptr->flash.called_address[i];
          }
        }
        else
        {
          ((caii_global_emerg_type *) rec_ptr)->num_incl = 0;
        }
      }
      else
      {
        /* -------------------------------------
        ** P_REV_IN_USE is less than 7,
        ** Include a Keypad Facility info record instead
        ** ------------------------------------- */
        rxtx_cmd_ptr->tc_msg.msg.flash.num_recs = 1;

        ((caii_key_rec_type *) rec_ptr)->fix.hdr.record_type =
          CAI_KEYPAD_REC;
        ((caii_key_rec_type *) rec_ptr)->fix.num_char = msg_ptr->flash.cnt;

        for (i=0; i < msg_ptr->flash.cnt; i++)
        {
          ((caii_key_rec_type *) rec_ptr)->var[i] =
            msg_ptr->flash.called_address[i];
        }
      }
      rxtx_cmd_ptr->tc_msg.ack_req = TRUE;

      /* Fill in the command header */
      mcctc_fill_rxtx_hdr_ack_q ( rxtx_cmd_ptr );

      rxtx_cmd( rxtx_cmd_ptr );

      return;
    }
    #endif //FEATURE_IS2000_REL_A

    /* we know what flash record to send based on the current state */
    if (
        (!(mcc_passport_mode_is_jcdma() || mcc_is_jcdma_supported()))&&
        ((cc_ptr->substate == CDMA_CC_WFA) && !P_REV_IS_6_OR_GREATER)

        )
    {
      /* in WFA, we need to send out flash with feature indicator */
      rxtx_cmd_ptr->tc_msg.msg.flash.num_recs = 1;
      ((caii_feat_rec_type *)rec_ptr)->hdr.record_type = CAI_FEAT_REC;
      ((caii_feat_rec_type *)rec_ptr)->feature = CAI_INCOMING_CALL_FWD;
    }
    else
    if (((cc_ptr->substate == CDMA_CC_WFA)
         && (mcc_passport_mode_is_jcdma() || mcc_is_jcdma_supported() ||
             P_REV_IS_6_OR_GREATER)
         ) ||
         (cc_ptr->substate == CDMA_CC_CONV))
    {

      if (msg_ptr->flash.cnt == 0)
      {
        rxtx_cmd_ptr->tc_msg.msg.flash.num_recs = 0;
      }
      else
      {
        /* -------------------------------------
        ** Include a Keypad Facility info record
        ** ------------------------------------- */
        rxtx_cmd_ptr->tc_msg.msg.flash.num_recs = 1;

        ((caii_key_rec_type *) rec_ptr)->fix.hdr.record_type =
        CAI_KEYPAD_REC;
        ((caii_key_rec_type *) rec_ptr)->fix.num_char = msg_ptr->flash.cnt;

        for (i=0; i < msg_ptr->flash.cnt; i++)
        {
          ((caii_key_rec_type *) rec_ptr)->var[i] =
            msg_ptr->flash.called_address[i];
        }
      }
    }
    else
    {
      /* wrong state */
      M1X_MSG( DCP, LEGACY_ERROR,
        "Received FLASH mesg in wrong state");
    }

    rxtx_cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( rxtx_cmd_ptr );

    rxtx_cmd( rxtx_cmd_ptr );
  }

  return;

} /* mccccim_send_flash_info_msg */

#ifdef FEATURE_IS2000_REL_A_SVD
/*===========================================================================

FUNCTION MCCCCIM_SEND_EXT_FLASH_INFO_MSG

DESCRIPTION
  This function sends the flash with information message during traffic
  channel processing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccccim_send_ext_flash_info_msg
(
  mc_msg_type *msg_ptr,
  /* pointer to message received from other other tasks */
  mccccim_cc_info_type * cc_ptr
)
{
  byte * rec_ptr; /* ptr to the information record of the msg */
  rxtx_cmd_type *rxtx_cmd_ptr;
  int i;  /* counter */

  #ifdef FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB
  #error code not present
#endif /* FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB */

  /* Send Flash With Information Message */
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((rxtx_cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX_MC_Q" );
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((rxtx_cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    rxtx_cmd_ptr->tc_msg.msg.efwim.msg_type = CAI_EFWIM_MSG;
    rec_ptr = (byte *) &rxtx_cmd_ptr->tc_msg.msg.efwim.recs[0];


    if (cc_ptr->is_null_cc)
    {
      rxtx_cmd_ptr->tc_msg.msg.efwim.con_ref_incl = FALSE;

    }
    else
    {
      rxtx_cmd_ptr->tc_msg.msg.efwim.con_ref_incl = TRUE;
      rxtx_cmd_ptr->tc_msg.msg.efwim.con_ref = cc_ptr->con_ref;
    }

    if (msg_ptr->flash.flash_type == CM_CALL_SUPS_TYPE_EMERGENCY_CALL)
    {
      rxtx_cmd_ptr->tc_msg.msg.efwim.num_rec = 1;

      ((caii_global_emerg_type *) rec_ptr)->hdr.record_type =
         CAI_GLOB_EM_CALL_REC;

      /* MS won't initiate a Position Location Determination Session */
      ((caii_global_emerg_type *) rec_ptr)->ms_orig_pos_loc_ind = 0;

      if (msg_ptr->flash.cnt)
      {
        ((caii_global_emerg_type *) rec_ptr)->num_char = msg_ptr->flash.cnt;
        ((caii_global_emerg_type *) rec_ptr)->num_incl = 1;
        ((caii_global_emerg_type *) rec_ptr)->digit_mode = msg_ptr->flash.digit_mode;
        ((caii_global_emerg_type *) rec_ptr)->number_type = msg_ptr->flash.number_type;
        ((caii_global_emerg_type *) rec_ptr)->number_plan = msg_ptr->flash.number_plan;

        for (i=0; i < msg_ptr->flash.cnt; i++)
        {
          ((caii_global_emerg_type *) rec_ptr)->chari[i] =
               msg_ptr->flash.called_address[i];
        }
      }
      else
      {
        ((caii_global_emerg_type *) rec_ptr)->num_incl = 0;
      }
      rxtx_cmd_ptr->tc_msg.ack_req = TRUE;

      /* Fill in the command header, we need to know when this was acked */
      mcctc_fill_rxtx_hdr_ack_q ( rxtx_cmd_ptr );

      rxtx_cmd( rxtx_cmd_ptr );

      return;
    }

    if (((cc_ptr->substate == CDMA_CC_WFA) && ( P_REV_IS_6_OR_GREATER)) ||
         (cc_ptr->substate == CDMA_CC_CONV))
    {
      if (msg_ptr->flash.cnt == 0)
      {
        rxtx_cmd_ptr->tc_msg.msg.efwim.num_rec = 0;
      }
      else
      {
        /* -------------------------------------
        ** Include a Keypad Facility info record
        ** ------------------------------------- */
        rxtx_cmd_ptr->tc_msg.msg.efwim.num_rec = 1;

        ((caii_key_rec_type *) rec_ptr)->fix.hdr.record_type =
        CAI_KEYPAD_REC;
        ((caii_key_rec_type *) rec_ptr)->fix.num_char = msg_ptr->flash.cnt;

        for (i=0; i < msg_ptr->flash.cnt; i++)
        {
          ((caii_key_rec_type *) rec_ptr)->var[i] =
            msg_ptr->flash.called_address[i];
        }
      }
    }
    else
    {
      /* wrong state */
      M1X_MSG( DCP, LEGACY_ERROR,
        "Received FLASH mesg in wrong state");
    }

    rxtx_cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( rxtx_cmd_ptr );

    rxtx_cmd( rxtx_cmd_ptr );
  }

  return;

} /* mccccim_send_ext_flash_info_msg */
#endif /* FEATURE_IS2000_REL_A_SVD */

/*===========================================================================

FUNCTION MCCCCIM_SEND_CONT_DTMF

DESCRIPTION
  This function sends a Continous dtmf order message to layer 2 for the
  input digit.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccccim_send_cont_dtmf
(
  mccccim_cc_info_type * cc_ptr,
    /* Pointer to CC for which CONT DTMF has to be sent */
  byte digit
    /* digit to place into message */
)
{
  rxtx_cmd_type *msg_ptr;
   /* Pointer to command to send to RXTX task */

  byte trans_digit;
    /* Translated DTMF digit (ASCII format to BCD format) to be sent in message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  M1X_MSG( DCP, LEGACY_MED,
    "send_cont_dtmf %d",
    digit );
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((msg_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX_MC_Q" );
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((msg_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    msg_ptr->tc_msg.msg.tc_ord.ordq.msg_type = CAI_TC_REV_ORD_MSG;
    msg_ptr->tc_msg.msg.tc_ord.ordq.order = CAI_DTMF_ORD;
    trans_digit = mcc_encode_dtmf(digit);

    if( trans_digit != MCC_DIAL_ILLEGAL )
    {
      msg_ptr->tc_msg.msg.tc_ord.ordq.ordq = (trans_digit & 0x0F );

      #ifdef FEATURE_IS2000_REL_A
      if (P_REV_IS_7_OR_GREATER)
      {
        if (cc_ptr->is_null_cc)
        {
          msg_ptr->tc_msg.msg.tc_ord.ordq.con_ref_incl = FALSE;
        }
        else
        {
          msg_ptr->tc_msg.msg.tc_ord.ordq.con_ref_incl = TRUE;
          msg_ptr->tc_msg.msg.tc_ord.ordq.con_ref = cc_ptr->con_ref;
        }
      }
      #endif

      msg_ptr->tc_msg.ack_req = TRUE;
       /* We need to know when this message is acked for timing purposes */
      mcctc_fill_rxtx_hdr_ack_q ( msg_ptr );

      rxtx_cmd( msg_ptr );

      cc_ptr->dtmf_ack_wait = TRUE;
       /* we are now waiting for an ack to a Start Continous DTMF msg */
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Don't send_cont_dtmf for Illegal digit: %d",
        digit );
    }
  }

} /* mccccim_send_cont_dtmf */


/*===========================================================================

FUNCTION MCCCCIM_START_CONT_DTMF

DESCRIPTION
  This function processes a request from ui to send a continous dtmf order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccccim_start_cont_dtmf
(
  mccccim_cc_info_type * cc_ptr,
    /* Pointer to CC for which this has to be sent */
  mc_msg_type *msg_ptr
    /* Pointer to MC_START_CONT_DTMF_F command from UI */
)
{
  cdma_dtmf_type *dtmf_ptr;
    /* Pointer to START_DTMF command to queue for later */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check the CC instance specific DTMF timer */
  if( (rex_get_timer(&(cc_ptr->dtmf_timer)) != 0 ) || (cc_ptr->dtmf_ack_wait) )
  {
    /* Can't send command if we are waiting for an ack to a start or stop
       or waiting for time to expire between sending start and stop */
#ifdef FEATURE_1X_CP_MEM_OPT
    if( (dtmf_ptr = (cdma_dtmf_type*) mc_alloc_queue_buf( MCC_DTMF_Q )) == NULL)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Memory allocation failed for MCC_DTMF_Q" );
    }
#else /* !FEATURE_1X_CP_MEM_OPT */
    if( (dtmf_ptr = (cdma_dtmf_type*) q_get(&mcc_dtmf_free_q)) == NULL )
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No buffers on mcc_dtmf_free_q" );
    }
#endif /* FEATURE_1X_CP_MEM_OPT */
    else
    {
      dtmf_ptr->dtmf_cmd = MC_START_CONT_DTMF_F;
      dtmf_ptr->parm.digit = msg_ptr->start_cont_dtmf.digit;
      q_put( &mcc_dtmf_q, &dtmf_ptr->link );
    }
  }
  else
  {
    mccccim_send_cont_dtmf( cc_ptr, msg_ptr->start_cont_dtmf.digit );
  }

} /* mccccim_start_cont_dtmf */

/*===========================================================================

FUNCTION MCCCCIM_SEND_STOP_DTMF

DESCRIPTION
  This function sends a stop continous dtmf order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccccim_send_stop_dtmf
(
  mccccim_cc_info_type * cc_ptr
    /* Pointer to CC for which this has to be sent */
)
{
  rxtx_cmd_type *msg_ptr;
   /* Pointer to command to send to layer 2 task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((msg_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX_MC_Q" );
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((msg_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    msg_ptr->tc_msg.msg.tc_ord.ordq.msg_type = CAI_TC_REV_ORD_MSG;
    msg_ptr->tc_msg.msg.tc_ord.ordq.order = CAI_DTMF_ORD;
    msg_ptr->tc_msg.msg.tc_ord.ordq.ordq = 0xFF;

    msg_ptr->tc_msg.ack_req = TRUE;

    #ifdef FEATURE_IS2000_REL_A
    if (!P_REV_IS_7_OR_GREATER || cc_ptr->is_null_cc)
    {
      msg_ptr->tc_msg.msg.tc_ord.ordq.con_ref_incl = FALSE;
    }
    else
    {
      msg_ptr->tc_msg.msg.tc_ord.ordq.con_ref_incl = TRUE;
      msg_ptr->tc_msg.msg.tc_ord.ordq.con_ref = cc_ptr->con_ref;
    }
    #endif

    /* We need to know when this message is acked for sending next start */
    mcctc_fill_rxtx_hdr_ack_q ( msg_ptr );

    rxtx_cmd( msg_ptr );

    cc_ptr->dtmf_ack_wait = TRUE;
      /* we are now waiting for an ack to a Start Continous DTMF msg */
  }

} /* mccccim_send_stop_dtmf */

/*===========================================================================

FUNCTION MCCCCIM_STOP_CONT_DTMF

DESCRIPTION
  This function processes requests to Stop continous dtmfs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccccim_stop_cont_dtmf
(
  mccccim_cc_info_type * cc_ptr,
    /* Pointer to CC for which this has to be sent */
  mc_msg_type *msg_ptr
    /* Pointer to MC_STOP_CONT_DTMF_F command from UI */
)
{
  word time_since_start;
     /* Amount of time elapsed since ack was for START_CONT_DTMF order */
  int time_until_stop;
     /* Amount of time left before STOP_DTMF command issued */
  cdma_dtmf_type *dtmf_ptr;
    /* Pointer to STOP_DTMF command to be placed on mcc_dtmf_q */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((q_cnt( &mcc_dtmf_q ) != 0) || (cc_ptr->dtmf_ack_wait))
  {
    /* ------------------------------------------------------------------
    ** If an item is on the queue assume that it must be the START_DTMF
    ** before this STOP_DTMF so don't process the STOP yet; or if waiting
    ** for an ack to the START_DTMF then can't process the STOP_DTMF yet
    ** ------------------------------------------------------------------ */
#ifdef FEATURE_1X_CP_MEM_OPT
    if( (dtmf_ptr = (cdma_dtmf_type*) mc_alloc_queue_buf( MCC_DTMF_Q )) != NULL)
    {
      dtmf_ptr->dtmf_cmd = MC_STOP_CONT_DTMF_F;
      dtmf_ptr->parm.ms = msg_ptr->stop_cont_dtmf.ms;
      q_put(&mcc_dtmf_q, &dtmf_ptr->link );
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Memory allocation failed for MCC_DTMF_Q" );
    }
#else /* !FEATURE_1X_CP_MEM_OPT */
    if ((dtmf_ptr = (cdma_dtmf_type*) q_get( &mcc_dtmf_free_q )) != NULL)
    {
      dtmf_ptr->dtmf_cmd = MC_STOP_CONT_DTMF_F;
      dtmf_ptr->parm.ms = msg_ptr->stop_cont_dtmf.ms;
      q_put(&mcc_dtmf_q, &dtmf_ptr->link );
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No buffers on mcc_dtmf_free_q" );
    }
#endif /* FEATURE_1X_CP_MEM_OPT */
  }
  else
  {
    /* ------------------------------------------------------------------
    ** Evaluate whether enough time has elapsed between ack of START_DTMF
    ** order and receipt of this STOP_DTMF command
    ** ----------------------------------------------------------------- */
    time_since_start = (word)((dword)MCCCCIM_MAX_DTMF_TIME -
                              rex_get_timer( &(cc_ptr->dtmf_timer) ));
    time_until_stop = (int) (msg_ptr->stop_cont_dtmf.ms - time_since_start);

    if (time_until_stop > 10 )
    {
      /* Set timer for remaining time */
      (void) mcc_set_callback_timer( &(cc_ptr->dtmf_timer), (word) time_until_stop, MCC_CC_DTMF_TIMER_TYPE | cc_ptr->cc_index );

      /* Indicate that when timer expires a Stop DTMF order must be txed */
      cc_ptr->dtmf_stop = TRUE;
    }
    else
    {
      /* Don't bother to set timer if less than 10 ms of time remaining */
      (void) mcc_clr_callback_timer( &(cc_ptr->dtmf_timer), MCC_CC_DTMF_TIMER_TYPE | cc_ptr->cc_index );

      mccccim_send_stop_dtmf( cc_ptr );
    }
  }

} /* mccccim_stop_cont_dtmf() */

/*===========================================================================

FUNCTION MCCCCIM_SEND_BURST_DTMF

DESCRIPTION
  This function builds and sends a send burst dtmf order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccccim_send_burst_dtmf
(
  mccccim_cc_info_type * cc_ptr,
    /* Pointer to CC for which this has to be sent */
  mc_burst_dtmf_type *cmd_ptr
    /* Pointer to MC_BURST_DTMF_F command from UI */
)
{
  word i;
    /* Index through digits */
  rxtx_cmd_type *msg_ptr;
    /* Pointer to command to send to Layer 2 task */
  boolean illegal_dtmf_digit = FALSE;
    /* boolean variable which inidicates illegal dtmf digit or not */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  M1X_MSG( DCP, LEGACY_MED,
    "send_burst dtmf %d %d %d",
    cmd_ptr->digits[0],
    cmd_ptr->digits[1],
    cmd_ptr->digits[2] );

  #ifdef FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB
  #error code not present
#endif /* FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB */

#ifdef FEATURE_1X_CP_MEM_OPT
  if ((msg_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX_MC_Q" );
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((msg_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    msg_ptr->tc_msg.msg.sbdtmf.msg_type = CAI_SEND_DTMF_MSG;
    msg_ptr->tc_msg.msg.sbdtmf.dtmf_on_length = cmd_ptr->on_length;
    msg_ptr->tc_msg.msg.sbdtmf.dtmf_off_length = cmd_ptr->off_length;
    msg_ptr->tc_msg.msg.sbdtmf.num_digits = cmd_ptr->cnt;

    M1X_MSG( DCP, LEGACY_LOW,
      "Send Burst dtmf %d digits",
      msg_ptr->tc_msg.msg.sbdtmf.num_digits );
    #ifdef FEATURE_IS2000_REL_A
    if (!P_REV_IS_7_OR_GREATER || cc_ptr->is_null_cc)
    {
      msg_ptr->tc_msg.msg.sbdtmf.con_ref_incl = FALSE;
    }
    else
    {
      msg_ptr->tc_msg.msg.sbdtmf.con_ref_incl = TRUE;
      msg_ptr->tc_msg.msg.sbdtmf.con_ref = cc_ptr->con_ref;
    }
    #endif

    /* Translate DTMFs into values required for message */
    for( i=0; i< cmd_ptr->cnt; i++ )
    {
      if ( ( msg_ptr->tc_msg.msg.sbdtmf.digit[i] =
             mcc_encode_dtmf( cmd_ptr->digits[i]) ) != MCC_DIAL_ILLEGAL )
      {
        continue;
      }
      else
      {
        illegal_dtmf_digit = TRUE;
        break;
      }
    }/* for */

    if( !illegal_dtmf_digit )
    {
      msg_ptr->tc_msg.ack_req = TRUE;

      /* We need to know when this message is acked */
      mcctc_fill_rxtx_hdr_ack_q ( msg_ptr );

      rxtx_cmd( msg_ptr );

      /* We are now waiting for an ack to a Send Burst DTMF Message */
      cc_ptr->dtmf_ack_wait = TRUE;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Don't send_burst_dtmf for Illegal digit: %d",
        cmd_ptr->digits[i] );
    }
  }

} /* mccccim_send_burst_dtmf */

/*===========================================================================

FUNCTION MCCCCIM_BURST_DTMF

DESCRIPTION
  This function processes MC_BURST_DTMF_F commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccccim_burst_dtmf
(
  mccccim_cc_info_type * cc_ptr,
    /* Pointer to CC for which this has to be sent */
  mc_burst_dtmf_type *cmd_ptr
    /* Pointer to MC_BURST_DTMF_F command from UI */
)
{
  cdma_dtmf_type *dtmf_ptr;
    /* Pointer to MC_BURST_DTMF command to be placed on mcc_dtmf_q */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For CCS this has to be CC instance specific DTMF timer */
  if ((rex_get_timer( &(cc_ptr->dtmf_timer) ) != 0) || (cc_ptr->dtmf_ack_wait))
  {
    /* -----------------------------------------------------------------
    ** Can't send command if we are waiting for an ack to a DTMF message
    ** tx in progress or waiting for time to expire between sending a
    ** Start and Stop
    ** ----------------------------------------------------------------- */

    /* Note for the time being we have just one global mcc_dtmf_free_q,
       if we get to the point where we have multiple services supporting
       DTMF, we may need one queue per CC, For CCS. Keep it simple till that
       happens
    */
#ifdef FEATURE_1X_CP_MEM_OPT
    if( (dtmf_ptr = (cdma_dtmf_type*) mc_alloc_queue_buf( MCC_DTMF_Q )) == NULL)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Memory allocation failed for MCC_DTMF_Q" );
    }
#else /* !FEATURE_1X_CP_MEM_OPT */
    if ((dtmf_ptr = (cdma_dtmf_type*) q_get( &mcc_dtmf_free_q )) == NULL)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No buffers on mcc_dtmf_free_q" );
    }
#endif /* FEATURE_1X_CP_MEM_OPT */
    else
    {
      dtmf_ptr->dtmf_cmd = MC_BURST_DTMF_F;
      dtmf_ptr->parm.burst = *cmd_ptr;
      q_put( &mcc_dtmf_q, &dtmf_ptr->link );
    }
  }
  else
  {
    mccccim_send_burst_dtmf( cc_ptr, cmd_ptr );
  }

} /* mccccim_burst_dtmf() */

/*===========================================================================

FUNCTION MCCCCIM_PROC_CM_CALL_HOLD

DESCRIPTION
  This function sends the flash with information message during traffic
  channel processing.  This function only process the command in Call Hold
  context

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccccim_proc_cm_call_hold
(
  mccccim_cc_info_type * cc_ptr,
   /* Pointer to cc instance */
  mc_msg_type *msg_ptr
  /* pointer to message received from other other tasks */
)
{
  byte * rec_ptr; /* ptr to the information record of the msg */
  rxtx_cmd_type *rxtx_cmd_ptr;
  int i;  /* counter */

  /* Send Flash With Information Message */
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((rxtx_cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX_MC_Q" );
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((rxtx_cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    if (!P_REV_IS_7_OR_GREATER || cc_ptr->is_null_cc)
    {
      rxtx_cmd_ptr->tc_msg.msg.flash.msg_type = CAI_REV_FLASH_MSG;
      rec_ptr = (byte *) &rxtx_cmd_ptr->tc_msg.msg.flash.recs[0];

        if (msg_ptr->flash.cnt == 0)
        {
          rxtx_cmd_ptr->tc_msg.msg.flash.num_recs = 0;
        }
        else
        {
        /* -------------------------------------
        ** Include a Keypad Facility info record
        ** ------------------------------------- */
          rxtx_cmd_ptr->tc_msg.msg.flash.num_recs = 1;

          ((caii_key_rec_type *) rec_ptr)->fix.hdr.record_type =
            CAI_KEYPAD_REC;
          ((caii_key_rec_type *) rec_ptr)->fix.num_char = msg_ptr->flash.cnt;

          for (i=0; i < msg_ptr->flash.cnt; i++)
          {
            ((caii_key_rec_type *) rec_ptr)->var[i] =
              msg_ptr->flash.called_address[i];
          }
        }
    }
    else
    {
      /* Send Ext flash with Info mesg here for CCS */
      #ifdef FEATURE_IS2000_REL_A_SVD
      /* Send Ext flash with Info mesg here for CCS */
      rxtx_cmd_ptr->tc_msg.msg.efwim.msg_type = CAI_EFWIM_MSG;
      rec_ptr = (byte *) &rxtx_cmd_ptr->tc_msg.msg.efwim.recs[0];

      if (cc_ptr->is_null_cc)
      {
        rxtx_cmd_ptr->tc_msg.msg.efwim.con_ref_incl = FALSE;
      }
      else
      {
        rxtx_cmd_ptr->tc_msg.msg.efwim.con_ref_incl = TRUE;
        rxtx_cmd_ptr->tc_msg.msg.efwim.con_ref = cc_ptr->con_ref;

      }

      if (msg_ptr->flash.cnt == 0)
      {
        rxtx_cmd_ptr->tc_msg.msg.efwim.num_rec = 0;
      }
      else
      {
        /* -------------------------------------
        ** Include a Keypad Facility info record
        ** ------------------------------------- */
        rxtx_cmd_ptr->tc_msg.msg.efwim.num_rec = 1;

        ((caii_key_rec_type *) rec_ptr)->fix.hdr.record_type =
            CAI_KEYPAD_REC;
        ((caii_key_rec_type *) rec_ptr)->fix.num_char = msg_ptr->flash.cnt;

        for (i=0; i < msg_ptr->flash.cnt; i++)
        {
          ((caii_key_rec_type *) rec_ptr)->var[i] =
              msg_ptr->flash.called_address[i];
        }
      }
      #endif /* FEATURE_IS2000_REL_A_SVD */

    }

    rxtx_cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_ack_q ( rxtx_cmd_ptr );

    rxtx_cmd( rxtx_cmd_ptr );
  }

  return;

} /* mccccim_proc_cm_call_hold */

/*===========================================================================
FUNCTION MCCCCIM_PROCESS_HS_CMDS

DESCRIPTION
  This function processes commands received from CM. The CC this
  commands is intended for is derived from the call_id in the
  command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mccccim_process_hs_cmds
(
  mc_msg_type *msg_ptr
    /* Pointer to message received from the handset*/
)
{
  byte cc_index;
  byte i;       /* loop variable */
  mc_status_type status;  /* return status */


  switch (msg_ptr->hdr.cmd)
  {
    case MC_END_F:
    {
      status = MC_DONE_S;

      for (i = 0; i<msg_ptr->end.num_call_ids; i++)
      {
        /* Try to get CCI index using call_id */
        if ((cc_index = mccccim_get_cc_index_from_callid( msg_ptr->end.call_ids[i]))
            != MCCCCIM_CALL_ID_NOT_FOUND)
        {
          /* Check the valid range value for cc_index to fix the Klocwork error */
          if (cc_index >= MCCCCIM_NUM_MAX_CCS)
          {
            status = MC_BADPARM_S;
            break;
          }

          if (msg_ptr->end.end_reasons[i] == CAI_REL_SRV_INACT)
          {
            mccccim_cc_info[cc_index].rel_del_reason = MCCCCIM_USER_SRV_INACTIVE;
          }
          else if(mcc_passport_mode_is_jcdma() && msg_ptr->end.end_reasons[i] == CAI_REL_PWDN)
          {
            mccccim_cc_info[cc_index].rel_del_reason = MCCCCIM_LOW_BATTERY;
          }
          else
          {
            mccccim_cc_info[cc_index].rel_del_reason = MCCCCIM_USER_END;
          }
          mccccim_handle_state_transition( &mccccim_cc_info[cc_index],CDMA_CC_RELEASE);
        } /* if(cc_index != MCCCCIM_CALL_ID_NOT_FOUND) */
        else
        {
          /* One invalid CCI call_id causes bad return value */
          status = MC_BADPARM_S;
        }
      } /* End of for() */

      msg_ptr->hdr.status = status;

      break;
    }

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    case MC_CSFB_CALL_END_F:
#ifdef FEATURE_MODEM_1X_SRVCC
    case MC_SRVCC_CALL_END_F:
#endif /* FEATURE_MODEM_1X_SRVCC */
#ifdef FEATURE_MODEM_1X_DRVCC
    case MC_DRVCC_END_F:
#endif /* FEATURE_MODEM_1X_DRVCC */
    {
      cm_call_id_type   call_id;  /* CSFB / SRVCC call_id */
      byte              end_reason; /* reason for ending the  call */

      boolean srvcc_mode = mcc_csfb_get_srvcc_mode();
      
      if(srvcc_mode == TRUE)
      {
#ifdef FEATURE_MODEM_1X_SRVCC
        if (msg_ptr->hdr.cmd == MC_SRVCC_CALL_END_F)
        {
          call_id = msg_ptr->srvcc_call_end.call_id;
          end_reason = msg_ptr->srvcc_call_end.end_reason;
          M1X_MSG( DCP, LEGACY_MED,
                 "Rxed MC_SRVCC_CALL_END_F in TC native 1x mode, call id = %d",
                 call_id );
        }
        else
#endif /* FEATURE_MODEM_1X_SRVCC */
        {
          M1X_MSG( DCP, LEGACY_ERROR,
                   "Unexpected CM cmd:%d in mode:%d",
                   msg_ptr->hdr.cmd, srvcc_mode);

          msg_ptr->hdr.status = MC_DONE_S;
          break;
        } /* Error Case */
      }
      else if (msg_ptr->hdr.cmd == MC_CSFB_CALL_END_F)
      {
        call_id = msg_ptr->csfb_call_end.call_id;
        end_reason = msg_ptr->csfb_call_end.end_reason;
        M1X_MSG( DCP, LEGACY_MED,
                 "CSFB:Proc CSFB_CALL_END in TC native 1x mode, call id = %d",
                 call_id );
      }
#ifdef FEATURE_MODEM_1X_DRVCC
      else if (msg_ptr->hdr.cmd == MC_DRVCC_END_F)
      {
        call_id = msg_ptr->drvcc_call_end.call_id;
        end_reason = msg_ptr->drvcc_call_end.end_reason;
        M1X_MSG( DCP, LEGACY_MED,
               "Rxed MC_SRVCC_CALL_END_F in TC native 1x mode, call id = %d",
               call_id );
      }
#endif /* FEATURE_MODEM_1X_DRVCC */
      else /* Error Case */
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "Unexpected CM cmd:%d in mode:%d",
                  msg_ptr->hdr.cmd, srvcc_mode);

        msg_ptr->hdr.status = MC_DONE_S;
        break;
      } /* Error Case */

      status = MC_DONE_S;

      /* Try to get CCI index using call_id */
      if( (cc_index = 
            mccccim_get_cc_index_from_callid(call_id))
          != MCCCCIM_CALL_ID_NOT_FOUND 
        )
      {
        /* Check the valid range value for cc_index to fix the Klocwork error */
        if(cc_index >= MCCCCIM_NUM_MAX_CCS)
        {
          status = MC_BADPARM_S;
          break;
        }

        if(mccccim_cc_info[cc_index].substate == CDMA_CC_WFO)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Cmd %d received in wrong CC state %d",
            msg_ptr->hdr.cmd,
            mccccim_cc_info[cc_index].substate);
          status = MC_BADPARM_S;
        } /* (substate == CDMA_CC_WFO) */
        else
        {
          if(end_reason == CAI_REL_SRV_INACT)
          {
            mccccim_cc_info[cc_index].rel_del_reason = 
                                          MCCCCIM_USER_SRV_INACTIVE;
          }
          #ifdef FEATURE_PASSPORT_MODE
          else if( mcc_passport_mode_is_jcdma() && 
                   end_reason == CAI_REL_PWDN
                 )
          {
            mccccim_cc_info[cc_index].rel_del_reason = MCCCCIM_LOW_BATTERY;
          }
          #endif /* FEATURE_PASSPORT_MODE */
          else
          {
            mccccim_cc_info[cc_index].rel_del_reason = MCCCCIM_USER_END;
          }
          mccccim_handle_state_transition( &mccccim_cc_info[cc_index], 
                                           CDMA_CC_RELEASE );
        } /* (substate != CDMA_CC_WFO) */
      } /* (cc_index != MCCCCIM_CALL_ID_NOT_FOUND) */
      else
      {
        /* One invalid CCI call_id causes bad return value */
        status = MC_BADPARM_S;
      }

      msg_ptr->hdr.status = status;
      break;
    } /* MC_CSFB_CALL_END_F */
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
              
    case MC_START_CONT_DTMF_F:
    {
      /* Get the cc_index corresponding to this call */
      if ((cc_index =
           mccccim_get_cc_index_from_callid( msg_ptr->start_cont_dtmf.call_id))
           != MCCCCIM_CALL_ID_NOT_FOUND)
      {
        /* Check the valid range value for cc_index to fix the Klocwork error */
        if (cc_index >= MCCCCIM_NUM_MAX_CCS)
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "CC entry is not found");
          msg_ptr->hdr.status = MC_BADPARM_S;
          return;
        }
        if (mccccim_cc_info[cc_index].substate == CDMA_CC_WFO)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Cmd %d received in wrong CC state %d",
            msg_ptr->hdr.cmd,
            mccccim_cc_info[cc_index].substate);
          msg_ptr->hdr.status = MC_BADPARM_S;
          return;
        }

        mccccim_start_cont_dtmf( &mccccim_cc_info[cc_index], msg_ptr);
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Invalid Call_ID %d",
          msg_ptr->start_cont_dtmf.call_id);
      }

      msg_ptr->hdr.status = MC_DONE_S;
      break;
    }

    case MC_STOP_CONT_DTMF_F:
    {
      /* Get the cc_index corresponding to this call */
      if ((cc_index =
           mccccim_get_cc_index_from_callid( msg_ptr->stop_cont_dtmf.call_id))
           != MCCCCIM_CALL_ID_NOT_FOUND)
      {
        /* Check the valid range value for cc_index to fix the Klocwork error */
        if (cc_index >= MCCCCIM_NUM_MAX_CCS)
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "Invalid CC Index value %d",
            cc_index);
          msg_ptr->hdr.status = MC_BADPARM_S;
          return;
        }

        if (mccccim_cc_info[cc_index].substate == CDMA_CC_WFO)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Cmd %d received in wrong CC state %d",
            msg_ptr->hdr.cmd,
            mccccim_cc_info[cc_index].substate);
          msg_ptr->hdr.status = MC_BADPARM_S;
          return;
        }

        mccccim_stop_cont_dtmf(&mccccim_cc_info[cc_index], msg_ptr );
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Invalid Call_ID %d",
          msg_ptr->stop_cont_dtmf.call_id);
      }

      msg_ptr->hdr.status = MC_DONE_S;
      break;
    }

    case MC_BURST_DTMF_F:
    {
      /* Get the cc_index corresponding to this call */
      if ((cc_index =
           mccccim_get_cc_index_from_callid( msg_ptr->burst_dtmf.call_id))
           != MCCCCIM_CALL_ID_NOT_FOUND)
      {
        /* Check the valid range value for cc_index to fix the Klocwork error */
        if (cc_index >= MCCCCIM_NUM_MAX_CCS)
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "Invalid CC Index value %d",
            cc_index);
          msg_ptr->hdr.status = MC_BADPARM_S;
          return;
        }

        if (mccccim_cc_info[cc_index].substate == CDMA_CC_WFO)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Cmd %d received in wrong CC state %d",
            msg_ptr->hdr.cmd,
            mccccim_cc_info[cc_index].substate);
          msg_ptr->hdr.status = MC_BADPARM_S;
          return;
        }

        mccccim_burst_dtmf( &mccccim_cc_info[cc_index],&msg_ptr->burst_dtmf );
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Invalid Call_ID %d",
          msg_ptr->burst_dtmf.call_id);
      }

      msg_ptr->hdr.status = MC_DONE_S;
      break;
    }

    case MC_FLASH_F:
    {
      /* Get the cc_index corresponding to this call */
      if ((cc_index = mccccim_get_cc_index_from_callid( msg_ptr->flash.call_id))
           != MCCCCIM_CALL_ID_NOT_FOUND)
      {
        /* Check the valid range value for cc_index to fix the Klocwork error */
        if (cc_index >= MCCCCIM_NUM_MAX_CCS)
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "Invalid CC Index value %d",
            cc_index);
          msg_ptr->hdr.status = MC_BADPARM_S;
          return;
        }

        if (mccccim_cc_info[cc_index].substate == CDMA_CC_WFO ||
           mccccim_cc_info[cc_index].substate == CDMA_CC_RELEASE )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Cmd %d received in wrong CC state %d",
            msg_ptr->hdr.cmd,
            mccccim_cc_info[cc_index].substate);
          msg_ptr->hdr.status = MC_BADPARM_S;
          return;
        }

        /* Send Flash With Information Message */
        #if defined(FEATURE_IS2000_REL_A)
        if (mccccim_cc_info[cc_index].is_null_cc || !P_REV_IS_7_OR_GREATER
        /* For JCDMA mode, send the flash with info mesg regardless */
        || mcc_passport_mode_is_jcdma()
        )  //lint !e506 !e774
        #endif /* FEATURE_IS2000_REL_A */
        {
          mccccim_send_flash_info_msg(msg_ptr, &mccccim_cc_info[cc_index]);
        }
        #if defined(FEATURE_IS2000_REL_A)
        else
        {
          #ifdef FEATURE_IS2000_REL_A_SVD
          /* For CCS need a new function for sending Ext flash with info */
          mccccim_send_ext_flash_info_msg(msg_ptr, &mccccim_cc_info[cc_index]);
          #else
          M1X_MSG( DCP, LEGACY_ERROR,
            "Non NULL CC present in a non-SVD case");
          #endif
        }
        #endif /* FEATURE_IS2000_REL_A */
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Invalid Call_ID %d",
          msg_ptr->flash.call_id);
      }

      msg_ptr->hdr.status = MC_DONE_S;
      break;
    }

    case MC_HOLD_F:
    {
      if(mcc_passport_mode_is_jcdma() || mcc_is_jcdma_supported())
      {

        /* Get the cc_index corresponding to this call */
        if ((cc_index = mccccim_get_cc_index_from_callid( msg_ptr->flash.call_id))
             != MCCCCIM_CALL_ID_NOT_FOUND)
        {
          /* Check the valid range value for cc_index to fix the Klocwork error */
          if (cc_index >= MCCCCIM_NUM_MAX_CCS)
          {
            M1X_MSG( DCP, LEGACY_HIGH,
              "Invalid CC Index value %d",
              cc_index);
            msg_ptr->hdr.status = MC_BADPARM_S;
            return;
          }

          if (mccccim_cc_info[cc_index].substate == CDMA_CC_WFO)
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Cmd %d received in wrong CC state %d",
              msg_ptr->hdr.cmd,
              mccccim_cc_info[cc_index].substate);
            msg_ptr->hdr.status = MC_BADPARM_S;
            return;
          }

          mccccim_proc_cm_call_hold( &mccccim_cc_info[cc_index],msg_ptr);
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Call_ID %d",
            msg_ptr->flash.call_id);
        }

        msg_ptr->hdr.status = MC_DONE_S;

        break;
      }
      else
      {
          M1X_MSG( DCP, LEGACY_ERROR,
            "MC_HOLD_F unexpected!");
          break;
      }
    }

    case MC_ANSWER_F:
    {
      /* Get the cc_index corresponding to this call */
      if((cc_index = mccccim_get_cc_index_from_callid( msg_ptr->answer.call_id))
          != MCCCCIM_CALL_ID_NOT_FOUND)
      {
        /* Check the valid range value for cc_index to fix the Klocwork error */
        if (cc_index >= MCCCCIM_NUM_MAX_CCS)
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "Invalid CC Index value %d",
            cc_index);
          msg_ptr->hdr.status = MC_BADPARM_S;
          return;
        }

        if (mccccim_cc_info[cc_index].substate != CDMA_CC_WFA)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Cmd %d received in wrong CC state %d",
            msg_ptr->hdr.cmd,
            mccccim_cc_info[cc_index].substate);
          msg_ptr->hdr.status = MC_BADPARM_S;
          return;
        }

        /* In case of MT call, MUX was inhibited to enable vocoder at TC init.
         * Now that user has accepted the call, so command MUX to enable vocoder. */
        mcc_rx_buf.hdr.command = RXC_VOC_ENABLE_F;
        M1X_MSG( DCP, LEGACY_MED,
          "Send RXC_VOC_ENABLE_F");
        mcc_rxc_cmd( &mcc_rx_buf);

        mccccim_send_connect_order( &mccccim_cc_info[cc_index]);
        M1X_MSG( DCP, LEGACY_MED,
          "Send Connect Order to BS");
        mccccim_handle_state_transition( &mccccim_cc_info[cc_index], CDMA_CC_CONV);

      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Invalid Call_ID %d",
          msg_ptr->answer.call_id);
      }

      msg_ptr->hdr.status = MC_DONE_S;

      break;
    }
    default:
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "%d : Invalid Command Received.",
        msg_ptr->hdr.cmd);
      msg_ptr->hdr.status = MC_DONE_S;
    }
  }  //lint !e787 Some enums not needed here

} /* mccccim_process_hs_cmds */

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_SUBSTATE_TIMER_EXP

DESCRIPTION
  This function does processing associated with substate
  timer expiring.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  This CC may enter the "Release" substate and a notification
  may be sent to MCCTC.

===========================================================================*/
void mccccim_process_substate_timer_exp
(
  mccccim_cc_info_type * cc_ptr
)
{
  /* No need to do anything if CC is in CONV or Rel substate */
  boolean send_tc_evt = FALSE;
  mccccim_tc_event_type mcctc_evt;

  switch (cc_ptr->substate)
  {
    case CDMA_CC_WFO:
    {
      /* Event report: Timer T52m expired */
      M1X_MSG( DCP, LEGACY_HIGH,
        "WFO Substate timer expired for con_ref %d",
        cc_ptr->con_ref);
      mclog_report_event_timer(CAI_TIMER_T52M);
      cc_ptr->rel_del_reason = MCCCCIM_STATE_TIMER_EXP;
      send_tc_evt = TRUE;
      break;
    }

    case CDMA_CC_WFA:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "WFA Substate timer expired for con_ref %d",
        cc_ptr->con_ref);
      /* Event report: Timer T52m expired */
      mclog_report_event_timer(CAI_TIMER_T53M);
      cc_ptr->rel_del_reason = MCCCCIM_STATE_TIMER_EXP_NO_ANS;
      send_tc_evt = TRUE;
      break;
    }

    /* No Op for any other substate */
    default:
    {
      break;
    }

  } /*lint !e744 No default needed */

  if (send_tc_evt)
  {
    mccccim_handle_state_transition( cc_ptr, CDMA_CC_RELEASE );
    /* Send Notification to MCCTC */
    mcctc_evt.timer_exp.evt_code = MCCTC_SUBSTATE_TIMER_EXP;
    mcctc_evt.timer_exp.con_ref = cc_ptr->con_ref ;
    mcctc_evt.timer_exp.rel_reason = cc_ptr->rel_del_reason;
    tc_handle_ccim_event( &mcctc_evt);
  }

} /* mccccim_process_substate_timer_exp */


/*===========================================================================

FUNCTION MCCCCIM_PROCESS_DTMF_TIMER_EXP

DESCRIPTION
  This function does processing associated with DTMF
  timer expiring for a particular CC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccccim_process_dtmf_timer_exp
(
  mccccim_cc_info_type * cc_ptr
)
{
  /* Time to send Stop DTMF message */
  if (cc_ptr->dtmf_stop)
  {
    cc_ptr->dtmf_stop = FALSE;
    mccccim_send_stop_dtmf( cc_ptr );
  }
  else
  {
    /* Seems like we should make an ERR call here */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Stop DTMF cmd not received for con_ref ",
      cc_ptr->con_ref  );
  }

} /* mccccim_process_dtmf_timer_exp */

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_TIMER_EXP

DESCRIPTION
  This function does processing associated with a Call Control
  Instance Timer expiration.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccccim_process_timer_exp
(
  unsigned long timer_id
)
{
  uint8 cc_index = (uint8) (timer_id & MCC_TIMER_SEQUENCE_MASK);

  if ( (cc_index >= MCCCCIM_NUM_MAX_CCS) ||
       (!mccccim_cc_info[cc_index].active)
     )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "CC Timer: Bad Call Control Instance %d",
      timer_id);
    return;
  }

  switch (timer_id & MCC_TIMER_TYPE_MASK)  /*lint !e737 */
  {
    case  MCC_CC_SUBSTATE_TIMER_TYPE:
    {
      (void)mcc_clr_callback_timer(&mccccim_cc_info[cc_index].substate_timer, timer_id);

      mccccim_process_substate_timer_exp( &mccccim_cc_info[cc_index]);
      break;
    }

    case  MCC_CC_DTMF_TIMER_TYPE:
    {
      (void)mcc_clr_callback_timer(&mccccim_cc_info[cc_index].dtmf_timer, timer_id);

      mccccim_process_dtmf_timer_exp( &mccccim_cc_info[cc_index]);
      break;
    }

    default:
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "CC Timer: Bad Type %d",
        timer_id);
      break;
    }
  } /* end switch */

} /* mccccim_process_timer_exp */

/*===========================================================================

FUNCTION MCCCCIM_GET_NUM_CCS_IN_NON_REL_STATE

DESCRIPTION
  This function returns the number of Call Control Instances
  which are active but not in "Release" substate. Used by MCCTC
  to determine if MCCTC can go to the "Release" substate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 mccccim_get_num_ccs_in_non_rel_state
(
  void
)
{
  uint8 i;
  uint8 num_active = 0;

  for (i=0; i<MCCCCIM_NUM_MAX_CCS; i++)
  {
    if (mccccim_cc_info[i].active && (mccccim_cc_info[i].substate!=CDMA_CC_RELEASE))
    {
      num_active++;
    }
  }

  return num_active;

} /* mccccim_get_num_ccs_in_non_rel_state */

/*===========================================================================

FUNCTION MCCCCIM_CAN_SO_BE_CONNECTED

DESCRIPTION
  This function determines if the TXC can "connect" this SO.

DEPENDENCIES
  None.

RETURN VALUE
  "True" if it can be connected, "false" otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccccim_can_so_be_connected
(
  byte con_ref,
  byte con_ref_index,
  word so
)
{
  uint8 i;
  boolean can_connect = FALSE;

  switch (so)
  {
    case CAI_SO_LOOPBACK_13K:
    case CAI_SO_LOOPBACK:
    case CAI_SO_RS1_MARKOV:
    case CAI_SO_RS2_MARKOV:
    case CAI_SO_MARKOV:
    case CAI_SO_MARKOV_13K:
    case CAI_SO_MARKOV_SO54:
    #ifdef FEATURE_IS2000_1X_ADV
    case CAI_SO_MARKOV_SO74:
    #endif /* FEATURE_IS2000_1X_ADV */
    case CAI_SO_LOOPBACK_SO55:
    #ifdef FEATURE_IS2000_1X_ADV
    case CAI_SO_LOOPBACK_SO75:
    #endif /* FEATURE_IS2000_1X_ADV */

    case CAI_SO_TDSO:
    case CAI_SO_FULL_TDSO:
    case CAI_SO_NULL:
    {
      /* These SOs can always be connected safely */
      can_connect = TRUE;
      return can_connect;
    }

  } /*lint !e744 no default necessary */

  /* If it is not one of the above SOs, they can only be connected
     if the call is in CONV substate, so check for that */
  for (i=0; i<MCCCCIM_NUM_MAX_CCS; i++)
  {
    /* Check if this con_ref is active and is in the right state for connection*/
    if (mccccim_cc_info[i].active && (mccccim_cc_info[i].con_ref == con_ref) )
    {
      if (mccccim_cc_info[i].substate == CDMA_CC_CONV)
      {
        can_connect = TRUE;
      }
      break;
    }

    /* If this is the very first SCM, then con_ref may not match, as the existing
    CC will have con_ref = 0 and will be replaced by MC after the action time with
    the con_ref of the first entry in the connection record, so check for that */
    if (mccccim_cc_info[i].active && !mccccim_cc_info[i].con_ref && !con_ref_index)
    {
      if (mccccim_cc_info[i].substate == CDMA_CC_CONV)
      {
        can_connect = TRUE;
      }
      break;
    }
  }

  return can_connect;

} /* mccccim_can_so_be_connected */

/*===========================================================================

FUNCTION MCCCCIM_CON_IS_IN_REL_SUBSTATE

DESCRIPTION
  This function determines if a connection is in release substate and
  is used when the SCR action packet is prepared.

DEPENDENCIES
  None.

RETURN VALUE
  "True" if con_ref is not present or in release substate,
  "false" otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccccim_con_is_in_rel_substate
(
  byte con_ref
)
{
  boolean is_in_rel = TRUE;
  byte i;

  /* Go through the list of connections */
  for (i=0; i<MCCCCIM_NUM_MAX_CCS; i++)
  {
    /* Check if this con_ref is active and is in the right state for connection*/
    /* con_ref may not match if the con_ref of this CC is 0, which means it is a
    very first call and first SCM has not been received, so account for that too */
    if (mccccim_cc_info[i].active)
    {
      #ifdef FEATURE_IS2000_REL_A
      if (P_REV_IS_7_OR_GREATER)
      {
        if ((mccccim_cc_info[i].con_ref == con_ref) ||
            (mccccim_cc_info[i].con_ref == 0))
        {
          if (mccccim_cc_info[i].substate != CDMA_CC_RELEASE)
          {
            is_in_rel = FALSE;
          }
          // can leave the loop, having found the con_ref
          break;
        }
      }
      else
      #endif /* FEATURE_IS2000_REL_A */

      {
        /* No need to check for con_ref in p_rev <= 6 */
        if (mccccim_cc_info[i].substate != CDMA_CC_RELEASE)
        {
          is_in_rel = FALSE;
        }
        /* Only one entry can be active for p_rev_in_use < 7, so
           no need to loop any further */
        break;
      }
    } /* if mccccim_cc_info[i].active */

  }

  return is_in_rel;

} /* mccccim_con_is_in_rel_substate */

/*===========================================================================

FUNCTION MCCCCIM_TC_DTMF_ACK

DESCRIPTION
  This function processes acknowledgements to Burst DTMF messages and
  Stop Continuous DTMF orders.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A Start Continous DTMF order may be sent or a Burst DTMF message may be
  sent.

===========================================================================*/

void mccccim_dtmf_ack
(
  mccccim_cc_info_type * cc_ptr
)
{
  cdma_dtmf_type *dtmf_ptr;
    /* Pointer to DTMF command to queue for later */
  byte save_digit;
    /* Digit to be saved from Start Continous DTMF msg */
  static mc_burst_dtmf_type msg;
    /* Message to be sent to send_burst_dtmf() procedure */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize number of digits to 0 */
  msg.cnt = 0;

  /* Set default values for DTMF on and off lengths */
  msg.on_length  = CAI_DTMF_ON_350;
  msg.off_length = CAI_DTMF_OFF_200;

  while ((dtmf_ptr = (cdma_dtmf_type*) q_get( &mcc_dtmf_q )) != NULL)
  {
    if (dtmf_ptr->dtmf_cmd == MC_START_CONT_DTMF_F)
    {
      /* Got a Start - need to decide on a Continuous or Burst DTMF msg */
      save_digit = dtmf_ptr->parm.digit;
#ifdef FEATURE_1X_CP_MEM_OPT
      mc_free_queue_buf( dtmf_ptr, MCC_DTMF_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
      q_put( &mcc_dtmf_free_q, &dtmf_ptr->link );
#endif /* FEATURE_1X_CP_MEM_OPT */

      if ((dtmf_ptr = (cdma_dtmf_type*) q_check( &mcc_dtmf_q )) != NULL)
      {
        /* ---------------------------------------------------------------
        ** Check to see if the time is short enough between start and stop
        ** to use a Burst DTMF message
        ** --------------------------------------------------------------- */
        if ((dtmf_ptr->dtmf_cmd == MC_START_CONT_DTMF_F) ||
            (dtmf_ptr->dtmf_cmd == MC_BURST_DTMF_F))
        {
          /* DTMF commands out of sync */
          M1X_MSG( DCP, LEGACY_ERROR,
            "DTMF Sync Error" );
        }
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
                  "Number of ms key depressed (MC_STOP_CONT_DTMF):%d",
                   dtmf_ptr->parm.ms );
          if (dtmf_ptr->parm.ms <= 350)
          {
            /* Can send a Burst DTMF message */
            if ((dtmf_ptr = (cdma_dtmf_type*) q_get( &mcc_dtmf_q )) == NULL)
            {
#ifdef FEATURE_1X_CP_MEM_OPT
              M1X_MSG( DCP, LEGACY_ERROR,
                "No buffers on mcc_dtmf_q" );
#else /* !FEATURE_1X_CP_MEM_OPT */
              M1X_MSG( DCP, LEGACY_ERROR,
                "No buffers on mcc_dtmf_free_q" );
#endif /* FEATURE_1X_CP_MEM_OPT */
            }
            else
            {
              msg.digits[msg.cnt] = save_digit;
              msg.cnt++;

              /* Put item back on free queue */
#ifdef FEATURE_1X_CP_MEM_OPT
              mc_free_queue_buf( dtmf_ptr, MCC_DTMF_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
              q_put( &mcc_dtmf_free_q, &dtmf_ptr->link );
#endif /* FEATURE_1X_CP_MEM_OPT */
            }
          }
          else
          {
            /* DTMF has been pushed too long - have to use Continuous DTMF */
            if (msg.cnt == 0)
            {
              /* no burst dtmf needs to be sent */
              mccccim_send_cont_dtmf( cc_ptr, save_digit);
            }
            else
            {
              /* Wait until after the Burst DTMF is sent */
              cc_ptr->cont_dtmf = save_digit;
            }
            break;
          }
        }
      }
      else
      {
        /* Stop is not on the queue yet - use Continuous DTMF */
        if (msg.cnt == 0)
        {
          /* no burst dtmf needs to be sent */
          mccccim_send_cont_dtmf( cc_ptr, save_digit);
        }
        else
        {
          /* wait until after burst dtmf is sent */
          cc_ptr->cont_dtmf = save_digit;
        }
      }
    }
    else if (dtmf_ptr->dtmf_cmd == MC_BURST_DTMF_F)
    {
      mccccim_send_burst_dtmf( cc_ptr, &dtmf_ptr->parm.burst );

      /* Put item back on free queue */
#ifdef FEATURE_1X_CP_MEM_OPT
      mc_free_queue_buf( dtmf_ptr, MCC_DTMF_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
      q_put( &mcc_dtmf_free_q, &dtmf_ptr->link );
#endif /* FEATURE_1X_CP_MEM_OPT */
      break;
    }
    else
    {
      /* DTMF commands out of sync */
      M1X_MSG( DCP, LEGACY_ERROR,
        "DTMF Sync Error" );

      /* Put item back on free queue */
#ifdef FEATURE_1X_CP_MEM_OPT
      mc_free_queue_buf( dtmf_ptr, MCC_DTMF_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
      q_put( &mcc_dtmf_free_q, &dtmf_ptr->link );
#endif /* FEATURE_1X_CP_MEM_OPT */
    }
  } /* while */

  if (msg.cnt > 0)
  {
    mccccim_send_burst_dtmf( cc_ptr, &msg );
  }

} /* mccccim_dtmf_ack */

/*===========================================================================

FUNCTION MCCCCIM_PROCESS_ACK

DESCRIPTION
  This function does processing related to receiving L2 acks for
  specific CC messages.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccccim_process_ack
(
  rxtx_cmd_type *cmd_ptr
  /* Pointer to rxtx command taken from mc_rxtx_ack_q */
)
{
  cdma_dtmf_type *dtmf_ptr;
    /* Pointer to dtmf command to queue for later */

  cm_mc_rpt_type *cm_rpt_cmd_ptr;
    /* Pointer to command to send to CM task */
  byte * rec_ptr;

  byte cc_index, con_ref;

  switch (cmd_ptr->tc_msg.msg.gen.msg_type)
  {
    case CAI_TC_REV_ORD_MSG:
    {
      /* Order message - check for Start or Stop Continous DTMF Order */
      if( cmd_ptr->tc_msg.msg.tc_ord.gen.order == CAI_DTMF_ORD )
      {
        #ifdef FEATURE_IS2000_REL_A
        if (!P_REV_IS_7_OR_GREATER || !cmd_ptr->tc_msg.msg.tc_ord.ordq.con_ref_incl)
        {
          con_ref = 0;
        }
        else
        {
          con_ref = cmd_ptr->tc_msg.msg.tc_ord.ordq.con_ref;
        }
        #else
        con_ref = 0;
        #endif /* FEATURE_IS2000_REL_A */

        cc_index = mccccim_get_cc_index_from_con_ref( con_ref );

        /* check the cc_index valid range value to fix the Klocwork error */
        if ((cc_index == MCCCCIM_CON_REF_NOT_FOUND) || (cc_index >= MCCCCIM_NUM_MAX_CCS))
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Ack rcvd for a CC mesg and CC is not found anymore");
          return;
        }

        /* No longer waiting for ack */
        mccccim_cc_info[cc_index].dtmf_ack_wait = FALSE;

        if (cmd_ptr->tc_msg.msg.tc_ord.ordq.ordq != CAI_STOP_CONT_DTMF)
        {
          /* Check for Stop DTMF command on queue */
          if ((dtmf_ptr = (cdma_dtmf_type*) q_get( &mcc_dtmf_q )) != NULL)
          {
            if (dtmf_ptr->dtmf_cmd == MC_STOP_CONT_DTMF_F)
            {
              /* Set timer for the exact time the DTMF key was depressed */
              (void) mcc_set_callback_timer( &(mccccim_cc_info[cc_index].dtmf_timer), dtmf_ptr->parm.ms, MCC_CC_DTMF_TIMER_TYPE | cc_index );

              /* -------------------------------------------------------
              ** Indicate that when timer expires a Stop Continuous DTMF
              ** Order must be tranmitted
              ** ------------------------------------------------------- */
              mccccim_cc_info[cc_index].dtmf_stop = TRUE;
            }
            else
            {
              /* DTMF commands out of sync */
              M1X_MSG( DCP, LEGACY_ERROR,
                "DTMF Sync Error" );
            }

            /* Put item back on free queue */
#ifdef FEATURE_1X_CP_MEM_OPT
            mc_free_queue_buf( dtmf_ptr, MCC_DTMF_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
            q_put( &mcc_dtmf_free_q, &dtmf_ptr->link );
#endif /* FEATURE_1X_CP_MEM_OPT */
          }
          else
          {
            /* Start timer and wait for stop to come in */
            (void) mcc_set_callback_timer( &(mccccim_cc_info[cc_index].dtmf_timer), MCCCCIM_MAX_DTMF_TIME, MCC_CC_DTMF_TIMER_TYPE | cc_index );
          }
        }
        else
        {
          /* A Stop Continous DTMF Order just got acked */
          mccccim_dtmf_ack( &mccccim_cc_info[cc_index]);
        }
      }
      #ifdef FEATURE_JCDMA
      else if (cmd_ptr->tc_msg.msg.tc_ord.gen.order == CAI_MS_CONNECT_ORD)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Received BS ack to the Connect Order");
        con_ref = 0;
        cc_index = mccccim_get_cc_index_from_con_ref( con_ref );

        if (cc_index == MCCCCIM_CON_REF_NOT_FOUND)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Ack rcvd for a CC mesg and CC is not found anymore");
          return;
        }

        /* This is the L2 Ack. for the Connect Order */
        if ((cm_rpt_cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "No buffers on cm_mc_rpt_free_q");
        }
        else
        {
          /* Notify CM of the Connect Order Ack. Message */
          cm_rpt_cmd_ptr->cdma_connect_order_ack.hdr.cmd = CM_CDMA_CONNECT_ORDER_ACK_F;
          cm_rpt_cmd_ptr->cdma_connect_order_ack.call_id = mccccim_cc_info[cc_index].call_id;

          /* Send command to CM */
          mcc_cm_mc_rpt(cm_rpt_cmd_ptr);
          M1X_MSG( DCP, LEGACY_MED,
            "Notify CM of the Connect Order Ack. Msg.");
        }
      }
      #endif /* FEATURE_JCDMA */
      break;
    }

    case CAI_SEND_DTMF_MSG:
    {
      #ifdef FEATURE_IS2000_REL_A
      if (!P_REV_IS_7_OR_GREATER || !cmd_ptr->tc_msg.msg.sbdtmf.con_ref_incl)
      {
        con_ref = 0;
      }
      else
      {
        con_ref = cmd_ptr->tc_msg.msg.sbdtmf.con_ref;
      }
      #else
      con_ref = 0;
      #endif /* FEATURE_IS2000_REL_A */

      cc_index = mccccim_get_cc_index_from_con_ref( con_ref );

      /* check the cc_index valid range values to fix the Klocwork error */
      if ((cc_index == MCCCCIM_CON_REF_NOT_FOUND) || (cc_index >= MCCCCIM_NUM_MAX_CCS))
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Ack rcvd for a CC mesg and CC is not found anymore");
        return;
      }
      /* No longer waiting for ack */
      mccccim_cc_info[cc_index].dtmf_ack_wait = FALSE;

      if(mcc_passport_mode_is_jcdma())
      {
        if ((cm_rpt_cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "No buffers on cm_mc_rpt_free_q" );
        }
        else
        {
          /* Notify CM of the Send Burst DTMF Ack. message */
          cm_rpt_cmd_ptr->snd_brst_dtmf_ack.hdr.cmd = CM_SND_BRST_DTMF_ACK_F;
          cm_rpt_cmd_ptr->snd_brst_dtmf_ack.call_id = mccccim_cc_info[cc_index].call_id;

          /* Send command to CM */
          mcc_cm_mc_rpt( cm_rpt_cmd_ptr );
        }
      }

      if (mccccim_cc_info[cc_index].cont_dtmf != 0xFF)
      {
        /* A Continous dtmf message is pending */
        mccccim_send_cont_dtmf( &mccccim_cc_info[cc_index],
                        mccccim_cc_info[cc_index].cont_dtmf );

        /* Reset pending flag */
        mccccim_cc_info[cc_index].cont_dtmf = 0xFF;
      }
      else
      {
        mccccim_dtmf_ack(&mccccim_cc_info[cc_index]);
      }
      break;
    }

    case CAI_REV_FLASH_MSG:
    {
      #ifndef FEATURE_IS2000_REL_A
      if(mcc_passport_mode_is_jcdma() || mcc_is_jcdma_supported())
      #endif
      {
        con_ref = 0;
        cc_index = mccccim_get_cc_index_from_con_ref( con_ref );

        if (cc_index == MCCCCIM_CON_REF_NOT_FOUND)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Ack rcvd for a CC mesg and CC is not found anymore");
          return;
        }

        if (cmd_ptr->tc_msg.msg.flash.num_recs != 0)
        {
          rec_ptr = (byte *) (&cmd_ptr->tc_msg.msg.flash.recs[0]);
          switch (((caii_rec_hdr_type *) rec_ptr)->record_type)
          {
            case CAI_KEYPAD_REC:
            {
              /* This is the L2 Ack. for the Call Hold message or emergency call */
              if ((cm_rpt_cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                  "No buffers on cm_mc_rpt_free_q" );
              }
              else
              {
                if (mccccim_emer_flash_info.waiting_for_ack &&
                  (mccccim_emer_flash_info.cc_index == cc_index))
                {
                  cm_rpt_cmd_ptr->hdr.cmd = CM_CALL_EMERGENCY_FLASHED_F;
                  cm_rpt_cmd_ptr->call_emergency_flashed.original_call_id =
                    mccccim_emer_flash_info.emer_call_id;
                  cm_rpt_cmd_ptr->call_emergency_flashed.emergency_flash_call_id =
                  mccccim_cc_info[cc_index].call_id;
        
                  #ifdef CM_API_OTA_CNAM_INFO
                  /* Fill CM OTA Msg Type as Other Message */
                  cm_rpt_cmd_ptr->call_emergency_flashed.hdr.msg_type = 
                                                        CM_MSG_TYPE_AOMT;
                  M1X_MSG( DCP, LEGACY_MED,
                  "Info_Rec_Type:%d, MC OTA Msg Type:0x%x, CM OTA Msg Type:%d",
                           cm_rpt_cmd_ptr->hdr.cmd,
                           cmd_ptr->tc_msg.msg.gen.msg_type, 
                           cm_rpt_cmd_ptr->call_emergency_flashed.hdr.msg_type );
                  #endif /* CM_API_OTA_CNAM_INFO */
        
                  mcc_cm_mc_rpt( cm_rpt_cmd_ptr );
                  mccccim_emer_flash_info.waiting_for_ack = FALSE;
                }
                else if(mcc_passport_mode_is_jcdma() || mcc_is_jcdma_supported())
                {
                  /* Notify CM of the Send Burst DTMF Ack. message */
                  cm_rpt_cmd_ptr->call_hold_l2ack.hdr.cmd = CM_CALL_HOLD_L2ACK_F;

                  /* For CCS, include call_id also in this report */
                  cm_rpt_cmd_ptr->call_hold_l2ack.call_id = mccccim_cc_info[cc_index].call_id;

                  /* Send command to CM */
                  mcc_cm_mc_rpt( cm_rpt_cmd_ptr );
                }
                else
                {
                  /* Return the buffer since we dont need to send any report to CM. */
                  M1X_MSG( DCP, LEGACY_ERROR,
                    "Deallocating CM buf: wait_for_ack=%d, cc_index=%d, emer_cc_index = %d",
                    mccccim_emer_flash_info.waiting_for_ack,
                    cc_index,
                    mccccim_emer_flash_info.cc_index );
                  cm_mc_rpt_free_buf(cm_rpt_cmd_ptr);
                  cm_rpt_cmd_ptr = NULL;
                }
              }
              break;
            }

            #ifdef FEATURE_IS2000_REL_A
            case CAI_GLOB_EM_CALL_REC:
            if (mccccim_emer_flash_info.waiting_for_ack &&
              (mccccim_emer_flash_info.cc_index == cc_index))
            {
              if ((cm_rpt_cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
              {
                cm_rpt_cmd_ptr->hdr.cmd = CM_CALL_EMERGENCY_FLASHED_F;
                cm_rpt_cmd_ptr->call_emergency_flashed.original_call_id =
                  mccccim_emer_flash_info.emer_call_id;
                cm_rpt_cmd_ptr->call_emergency_flashed.emergency_flash_call_id =
                  mccccim_cc_info[cc_index].call_id;
                  
                #ifdef CM_API_OTA_CNAM_INFO
                /* Fill CM OTA Msg Type as Other Message */
                cm_rpt_cmd_ptr->call_emergency_flashed.hdr.msg_type = 
                                                      CM_MSG_TYPE_AOMT;
                M1X_MSG( DCP, LEGACY_MED,
                "Info_Rec_Type:%d, MC OTA Msg Type:0x%x, CM OTA Msg Type:%d",
                         cm_rpt_cmd_ptr->hdr.cmd,
                         cmd_ptr->tc_msg.msg.gen.msg_type, 
                         cm_rpt_cmd_ptr->call_emergency_flashed.hdr.msg_type );
                #endif /* CM_API_OTA_CNAM_INFO */
                  
                mcc_cm_mc_rpt( cm_rpt_cmd_ptr );
              }
              else
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                  "No free buffers on ui_cmd_free_q" );
              }
              mccccim_emer_flash_info.waiting_for_ack = FALSE;
            }
            break;
            #endif

          } /*lint !e744 no default in switch */
        }

        else if(mcc_passport_mode_is_jcdma() || mcc_is_jcdma_supported())
        {
          /* This is the L2 Ack. for the deactivate Call Hold message */
          if ((cm_rpt_cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "No buffers on cm_mc_rpt_free_q" );
          }
          else
          {
            /* Notify CM of the Send Burst DTMF Ack. message */
            cm_rpt_cmd_ptr->call_hold_l2ack.hdr.cmd = CM_CALL_HOLD_L2ACK_F;

            /* For CCS, include call_id also in this report */
            cm_rpt_cmd_ptr->call_hold_l2ack.call_id = mccccim_cc_info[cc_index].call_id;

           /* Send command to CM */
            mcc_cm_mc_rpt( cm_rpt_cmd_ptr );
         }
        }

        break;
      }
    }

    #ifdef FEATURE_IS2000_REL_A
    case CAI_EFWIM_MSG:
    {
      /* Once emergency flash record is supported, this case can no longer
         assume it is an ack for call hold record */
      if (cmd_ptr->tc_msg.msg.efwim.con_ref_incl)
      {
        con_ref = cmd_ptr->tc_msg.msg.efwim.con_ref;
      }
      else
      {
        con_ref = 0;
      }

      cc_index = mccccim_get_cc_index_from_con_ref( con_ref );

      if (cc_index == MCCCCIM_CON_REF_NOT_FOUND)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Ack rcvd for a CC mesg and CC is not found anymore");
        return;
      }

      if (cmd_ptr->tc_msg.msg.efwim.num_rec != 0)
      {
        rec_ptr = (byte *) &cmd_ptr->tc_msg.msg.efwim.recs[0];

        switch (((caii_rec_hdr_type *)rec_ptr)->record_type)
        {

          case CAI_KEYPAD_REC:
          if(mcc_passport_mode_is_jcdma() || mcc_is_jcdma_supported())
          {

            /* This is the L2 Ack. for the Call Hold message */
            if ((cm_rpt_cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "No buffers on cm_mc_rpt_free_q" );
            }
            else
            {
              /* Notify CM of the Send Burst DTMF Ack. message */
              cm_rpt_cmd_ptr->call_hold_l2ack.hdr.cmd = CM_CALL_HOLD_L2ACK_F;
              /* For CCS, include call_id also in this report */
              cm_rpt_cmd_ptr->call_hold_l2ack.call_id = mccccim_cc_info[cc_index].call_id;

              /* Send command to CM */
              mcc_cm_mc_rpt( cm_rpt_cmd_ptr );
            }
          }
          break;

          case CAI_GLOB_EM_CALL_REC:
            if (mccccim_emer_flash_info.waiting_for_ack)
            {
              if ((cm_rpt_cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
              {
                cm_rpt_cmd_ptr->hdr.cmd = CM_CALL_EMERGENCY_FLASHED_F;
                cm_rpt_cmd_ptr->call_emergency_flashed.original_call_id =
                  mccccim_emer_flash_info.emer_call_id;
                cm_rpt_cmd_ptr->call_emergency_flashed.emergency_flash_call_id =
                  mccccim_cc_info[cc_index].call_id;
                  
                #ifdef CM_API_OTA_CNAM_INFO
                /* Fill CM OTA Msg Type as Other Message */
                cm_rpt_cmd_ptr->call_emergency_flashed.hdr.msg_type = 
                                                      CM_MSG_TYPE_AOMT;
                M1X_MSG( DCP, LEGACY_MED,
                "Info_Rec_Type:%d, MC OTA Msg Type:0x%x, CM OTA Msg Type:%d",
                         cm_rpt_cmd_ptr->hdr.cmd,
                         cmd_ptr->tc_msg.msg.gen.msg_type, 
                         cm_rpt_cmd_ptr->call_emergency_flashed.hdr.msg_type );
                #endif /* CM_API_OTA_CNAM_INFO */
                
                mcc_cm_mc_rpt( cm_rpt_cmd_ptr );
              }
              else
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                  "No free buffers on ui_cmd_free_q" );
              }
              mccccim_emer_flash_info.waiting_for_ack = FALSE;
            }
            break;
        } /*lint !e744 no default in switch */
      }

      else if(mcc_passport_mode_is_jcdma() || mcc_is_jcdma_supported())
      {
        /* This is the L2 Ack. for the deactivate Call Hold message */
        if ((cm_rpt_cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "No buffers on cm_mc_rpt_free_q" );
        }
        else
        {
          /* Notify CM of the Send Burst DTMF Ack. message */
          cm_rpt_cmd_ptr->call_hold_l2ack.hdr.cmd = CM_CALL_HOLD_L2ACK_F;

          /* For CCS, include call_id also in this report */
          cm_rpt_cmd_ptr->call_hold_l2ack.call_id = mccccim_cc_info[cc_index].call_id;

          /* Send command to CM */
          mcc_cm_mc_rpt( cm_rpt_cmd_ptr );
        }
      }

      break;
    }
    #endif /* FEATURE_IS2000_REL_A */

  } /*lint !e744 no default in switch */

} /* mccccim_process_ack */

/*===========================================================================

FUNCTION MCCCCIM_TC_EXIT

DESCRIPTION
  This function does the necessary clean up when the MS exits the
  TC state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccccim_tc_exit
(
  void
)
{
  /* This is a clean up function to set everything right */
  uint8 i;

  for (i=0; i < MCCCCIM_NUM_MAX_CCS; i++)
  {
    if (mccccim_cc_info[i].active)
    {
       mccccim_cc_info[i].active = FALSE;

       /* clear substate and DTMF timers and their queue entries */
       (void)mcc_clr_callback_timer( & (mccccim_cc_info[i].dtmf_timer), MCC_CC_DTMF_TIMER_TYPE | i );

       (void)mcc_clr_callback_timer( & (mccccim_cc_info[i].substate_timer), MCC_CC_SUBSTATE_TIMER_TYPE | i );

       M1X_MSG( DCP, LEGACY_ERROR,
         "Leaving Traffic with active CC Instance %d con_ref %d call_id %d",
         i,
         mccccim_cc_info[i].con_ref,
         mccccim_cc_info[i].call_id);
    } // if

  } // for

  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry != MCC_CSFB)
  /* Defer reporting any failures to CM if we are in CSFB mode. In CSFB mode,
   * we are not the active stack hence CM wouldnt be expecting any reports
   * from us
   */
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    if (mccccim_emer_flash_info.waiting_for_ack)
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "waiting for Ack on an emergency flash");
      mcc_send_orig_failure(CM_CALL_ORIG_ERR_ABORT,
                            mccccim_emer_flash_info.emer_call_id,
                            CM_CALL_ORIG_ERR_LAYER_L3
                            );
      mccccim_emer_flash_info.waiting_for_ack = FALSE;
    }
  }
  /* Just in case we used the back up buffer for "Connect Order" reset it now */
  rxtx_mc_backup_buf.hdr.status = RXTX_DONE_S;

} /* mccccim_tc_exit */

#ifdef FEATURE_IS2000_REL_A_SVD
/*===========================================================================

FUNCTION MCCCCIM_IS_EOM_SO_COMPATIBLE

DESCRIPTION
  This function checks if an SO is compatible with the existing ones. It is
  called to see whether an EOM can be sent.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if compatible.
  FALSE if the proposed SO does not, and has no alternative to be compatible.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccccim_is_eom_so_compatible
(
  word *so
)
{

  word temp_so_req;
    /* Temporary storage to store the service option */
  db_items_value_type item;
    /* storage for db items */
  temp_so_req = *so; /* store the service option in a temp variable */

  /*
  ** UI would set the SO to CAI_SO_VOICE_WILDCARD if a 'voice' call is
  ** originated by user else would set the SO to the option requested by DIAG. If
  ** the SO is a wildcard, then the preferred voice options stored in NV are used
  ** to determine the service option to originate in
  */

  if (temp_so_req == CAI_SO_VOICE_WILDCARD)
  {
    /* Get roam status of phone from db */
    db_get (DB_ROAM, &item);

    /* check to see if this origination is going to be put through in the
       home system */
    if (item.roam == DB_ROAM_OFF )
    {
      /* use the preferred service option for originations in home systems */
      *so = mcc_pref_voice_so.home_orig_voice_so;
    }
    else
    {
      /* use the preferred SO for originations in roaming systems */
      *so = mcc_pref_voice_so.roam_orig_voice_so;
    }

    *so = snm_eval_so(*so, NULL);
    M1X_MSG( DCP, LEGACY_HIGH,
      "Voice Wildcard resolved to SO %d",
      *so);
  }


  // check with SNM to see if we are compatible. This could potentially
  // change the so.
  return snm_is_eom_so_compatible_cur_cfg(so);

}  /* mccccim_is_eom_so_compatible */

/*===========================================================================

FUNCTION MCCCCIM_IS_CLAM_SO_COMPATIBLE

DESCRIPTION
  This function checks if an SO from CLAM is compatible with the existing ones,
  or if it is compatible with the one that MS proposed in EOM.

DEPENDENCIES
  This function can only be called when SO is included in CLAM.

RETURN VALUE
  TRUE if compatible.
  FALSE if the proposed SO does not, and has no alternative to be compatible.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccccim_is_clam_so_compatible
(
  word so_clam
    /* SO supplied in CLAM */
)
{

  return snm_so_is_enabled(so_clam);

} /* mccccim_is_clam_so_compatible */

/*===========================================================================

FUNCTION MCCCCIM_UPDATE_TEMP_SO

DESCRIPTION
  This function updates the temp_so field in CCI.

DEPENDENCIES
  This function shall only be called if CCI identified by CON_REF exists.

RETURN VALUE
  TRUE if successful.
  FALSE if not.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccccim_update_temp_so
(
  byte con_ref,
    /* CON_REF to identify CCI */
  word temp_so
    /* Temp SO */
)
{
  byte i;

  if((i = mccccim_get_cc_index_from_con_ref(con_ref)) !=
     MCCCCIM_CON_REF_NOT_FOUND)
  {
    mccccim_cc_info[i].temp_so = temp_so;
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* mccccim_update_temp_so */

/*===========================================================================

FUNCTION MCCCCIM_IS_ABLE_TO_CREATE_CC

DESCRIPTION
  This function check to see if a CC can be created. This function does not
  actually create a CC.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if a CC can be created.
  FALSE if not.

SIDE EFFECTS
  None.
===========================================================================*/
boolean mccccim_is_able_to_create_cc
(
  void
)
{
  uint8 i;

  /* First find an empty entry in the CC table */
  for (i=0; i < MCCCCIM_NUM_MAX_CCS; i++)
  {
    if (!mccccim_cc_info[i].active)
    {
      break;
    }
  }

  if (i == MCCCCIM_NUM_MAX_CCS)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "No BUffers available for a new CC");
    return FALSE;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "CC can be created");
    return TRUE;
  }

} /* mccccim_is_able_to_create_cc */

/*===========================================================================

FUNCTION MCCCCIM_IS_CON_REF_EXIST

DESCRIPTION
  This function check to see if a CC has the specified con_ref.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if a CC with matching con_ref is found.
  FALSE if not.

SIDE EFFECTS
  None.
===========================================================================*/
boolean mccccim_is_con_ref_exist
(
  byte con_ref
)
{
  return (mccccim_get_cc_index_from_con_ref(con_ref) !=
          MCCCCIM_CON_REF_NOT_FOUND);

} /* mccccim_is_con_ref_exist */

/*===========================================================================

FUNCTION MCCCCIM_NUM_AVAILABLE_CC

DESCRIPTION
  This function reports the maximum number of CCIs that can be initiated.

DEPENDENCIES
  None.

RETURN VALUE
  Number of CCIs that can be initiated.

SIDE EFFECTS
  None.
===========================================================================*/
byte mccccim_num_available_cc
(
  void
)
{
  byte i;                   /* loop variable */
  byte num_inactive = 0;    /* available CCI */

  /* First find an empty entry in the CC table */
  for (i=0; i < MCCCCIM_NUM_MAX_CCS; i++)
  {
    if (!mccccim_cc_info[i].active)
    {
      num_inactive++;
    }
  }

  return num_inactive;

} /* mccccim_num_available_cc */

/*===========================================================================

FUNCTION MCCCCIM_IS_ORIG

DESCRIPTION
  This function check if any call is MS originated.

DEPENDENCIES
  None.

RETURN VALUE
  True if any call is MS originated.
  False if no call is MS originated.

SIDE EFFECTS
  None.
===========================================================================*/
boolean mccccim_is_orig
(
  void
)
{
  byte i;                   /* loop variable */

  /* First find an empty entry in the CC table */
  for (i=0; i < MCCCCIM_NUM_MAX_CCS; i++)
  {
    if (!mccccim_cc_info[i].active)
    {
      if (mccccim_cc_info[i].mob_orig)
      {
        return TRUE;
      }
    }
  }

  return FALSE;

} /* mccccim_is_orig */

/*===========================================================================

FUNCTION MCCCCIM_CALL_ID_IS_IN_REL_SUBSTATE

DESCRIPTION
  This function determines if a call_id is in release substate.

DEPENDENCIES
  None.

RETURN VALUE
  "True" if call_id is present and in release substate, "false" otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccccim_call_id_is_in_rel_substate
(
  byte call_id
)
{
  boolean is_in_rel = FALSE;
  byte i;

  /* Go through the list of connections */
  for (i=0; i<MCCCCIM_NUM_MAX_CCS; i++)
  {
    /* Check if this con_ref is active and is in the right state for connection*/
    if ( (mccccim_cc_info[i].active)                     &&
         (mccccim_cc_info[i].call_id == call_id)
       )
    {
      if (mccccim_cc_info[i].substate == CDMA_CC_RELEASE)
      {
        is_in_rel = TRUE;
      }
      // can leave the loop, having found the con_ref
      break;
    }

  }

  return is_in_rel;

} /* mccccim_call_id_is_in_rel_substate */
#endif /* FEATURE_IS2000_REL_A_SVD */

/*===========================================================================

FUNCTION MCCCCIM_GET_CC_SUBSTATE

DESCRIPTION
  This function returns the substate of a CC instance.

DEPENDENCIES
  None.

RETURN VALUE
  substate of the corresponding CC in the array, if no such con_ref is
  found, CDMA_CC_RELEASE is returned.

SIDE EFFECTS
  None.
===========================================================================*/
cdma_cc_state_type mccccim_get_cc_substate
(
  byte con_ref
)
{
  byte cc_index = mccccim_get_cc_index_from_con_ref(con_ref);

  /* Check the cc_index range check to fix the Klocwork error */
  if ((cc_index != MCCCCIM_CON_REF_NOT_FOUND) && (cc_index < MCCCCIM_NUM_MAX_CCS))
  {
    return mccccim_cc_info[cc_index].substate;
  }
  else
  {
    /* This con_ref is not found, just say release */
    return CDMA_CC_RELEASE;
  }

} /* mccccim_get_cc_substate */

/*===========================================================================

FUNCTION mccccim_is_any_cc_in_wfo

DESCRIPTION
  This function checks if any Call Control instance is in WFO.

DEPENDENCIES
  None.

RETURN VALUE
  "True" if any active CC is in WFO, "false" otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccccim_is_any_cc_in_wfo
(
  void
)
{
  byte i;

  /* Go through the list of connections */
  for (i=0; i<MCCCCIM_NUM_MAX_CCS; i++)
  {
    /* Check if this CC is active and in WFO substate */
    if ( mccccim_cc_info[i].active && (mccccim_cc_info[i].substate == CDMA_CC_WFO))
    {
      return TRUE;
    }
  } /* for */

  /* there are no active CCs in WFO */
  return FALSE;

} /* mccccim_is_any_cc_in_wfo */

/*===========================================================================

FUNCTION MCCCCIM_DIAG_GET_STATE

DESCRIPTION
  This function returns state info for all CCs active.

DEPENDENCIES
  This function assumes that space has been allotted for mclog_state_ptr.

RETURN VALUE
  The argument ptr is filled with CC info and returned to caller.

SIDE EFFECTS
  None.
===========================================================================*/
void mccccim_diag_get_state
(
  mclog_diag_state_query_rsp_type *mclog_state_ptr
)
{
  int i;

  /* Sanity check */
  if (mclog_state_ptr == NULL)
  {
    ERR_FATAL("Internal error",0,0,0);
    return;
    /* return; */
  }

  /* Go through the list of connections */
  for ( mclog_state_ptr->num_cc=0,i=0;
        (i<MCCCCIM_NUM_MAX_CCS) && (mclog_state_ptr->num_cc < MCLOG_MAX_NUM_CONNECTIONS);
        i++
      )
  {
    if (mccccim_cc_info[i].active)
    {
      mclog_state_ptr->cc[mclog_state_ptr->num_cc].call_id = (uint8) mccccim_cc_info[i].call_id;
      mclog_state_ptr->cc[mclog_state_ptr->num_cc].con_ref = (uint8) mccccim_cc_info[i].con_ref;
      mclog_state_ptr->cc[mclog_state_ptr->num_cc].so = (uint16) mccccim_cc_info[i].so;
      mclog_state_ptr->cc[mclog_state_ptr->num_cc].sr_id = (uint8) mccccim_cc_info[i].sr_id;
      mclog_state_ptr->cc[mclog_state_ptr->num_cc].substate = (uint8) mccccim_cc_info[i].substate;
      mclog_state_ptr->num_cc++;
    }
  }

} /* mccccim_diag_get_state */

/*lint +e818 */
