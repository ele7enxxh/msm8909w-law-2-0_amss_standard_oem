/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              S E R V I C E    C O N F I G U R A T I O N    &

                          N E G O T I A T I O N


GENERAL DESCRIPTION
  This module supports Service Configuration and Negotiation as defined
  in the TSB for IS-95-A.

EXTERNALIZED FUNCTIONS
  srv_init - called after Traffic Channel Init to initialize the Service
    Configuration and Negotiation Services.

  srv_rpt - called to report events of interest to the Service Configuration
    and Negotiation Services.

  srv_eval_so - called to evaluate support for a specified service option and
    returns alternative if not supported.

  srv_set_def_cfg - called to build the default service configuration
    associated with a specified service option.

  srv_get_alt_so_list - called to evaluate a service option and return a list
    of alternative service options associated with the specified service option.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 1995 - 2014 Qualcomm Technologies, Inc. 
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

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/srv.c_v   1.28   04 Oct 2002 15:30:16   varavamu  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/srv.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/02/14   agh     Support for new customer specific voice service option
05/06/14   gga     1X_CP F3 redistribution
11/19/12   ssh     Added SMS over CSFB support.
09/15/12   ppr     FR#2370 - 1x memory optimization changes
05/24/12   ppr     Feature Cleanup
04/04/12   trc     Convert to m1x_time module
02/15/12   srk     Feature Cleanup: Removed FEATURE_NO_13K_VOICE_SUPPORT and
                   FEATURE_SO_VOICE_SMV
02/04/12   ppr     Feature Cleanup
01/27/12   ppr     Feature Cleanup: Mainlined Always ON features
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
02/09/11   ssh     Fixed the FPC mode value in the current config structure.
12/28/10   ag      Decoupling of SO73 from 1xAdv.
11/15/10   jtm     Initial SU API split for CP.
09/29/09   ag      1X Advanced Signaling Support - Phase 2
08/26/09   adw     Removed deprecated ts.h header.
05/14/09   jj      CMI Phase-II update.
04/17/09   jtm     Added includes to fix compiler warnings.
10/18/07   ag      Fixed compiler warning.
05/11/07   pg      More Lint Cleanup.
08/04/06   fh      AKA/AES: cleaned up the usage of AES capability functions
07/13/06   fh      AKA/AES: changed an AES function call according to AES changes.
06/07/06   fh      Added Voice Service AES encryption/decryption under the features
05/01/06   an      Fixed Lint Errors
04/24/06   fc      Fixed Rel A featurization.
08/20/05   va      Made call tc_inform_change_init_service for HDM case
                   This is needed if test SOs are changed as part of HO
08/18/05   pg      Fixed wrong reject order msg type for FTC_SRV_RSP_MSG in
                   srv_norm_subf() and srv_wf_req_subf().
07/25/05   fc      Changes for Rel D addendum f-csh messages.
07/05/05   sb      Added MEID support.
06/23/05   an      Fixed lint errors.
06/20/05   an      Update the mux option in the service conf parameter.
05/03/05   fc      Merged the following from MSMSHARED:
04/27/05   an       Added support for 4GV service option
03/28/05   fc      Added support for DCA.
                   Made FCS also available to Rel D.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
02/18/05   fc      Merged the following from MSMSHARED:
02/08/05   an       Mainlined IS95B features
02/03/05   fc      Fixed lint error
11/05/04   sb      Added initialization if chm_switching_parms_incl is 0.
10/07/04   fc      Removed redundant condition check.
10/01/04   fc      Fixed featurization.
09/28/04   fc      R-DV nnscr initialization changes.
09/27/04   va      Merged from REL B/C archive:
07/08/04   fc        Added debug messages.
09/16/04   fc      Added support for reverse DV.
08/18/04   fc      Service negotiation changes for Release C and D.
06/28/04   bkm     Merged the following from IS2000_REL_B_C:
  06/24/04   fc      Separate the event processing for sunfunction timer
                     expiration and SCR transaction completion.
06/21/04   va      HHO clean up changes
06/16/04   fc      Release C addendum changes for Sync ID.
06/08/04   bkm     Fixed typo s in feature names within comments.
06/04/04   bkm     Fixed compile issue and lint cleanup.
06/03/04   bkm     Merged the following from IS2000_REL_B_C:
  05/20/04   fc      Lint clean up.
  04/20/04   fc      Fixed the issue of rejecting SCM if P_REV < 6.
04/20/04   bkm     Misc updates so that num_soft/softer_switching_frames_chm
                   fields are stored in SRV NNSCR rather than MCCDV.
04/08/04   bkm     Channel mask updates.
03/26/04   bkm     Merged the following from IS2000_REL_B_C:
  03/12/04   fc      Added support for Release C fast call setup.
  12/12/03   fc      Added support for sync id and fast call setup.
01/13/05   bkm     Removed remaining references to CAI_CHIND* enums values.
01/05/04   bkm     Misc updates for DV F-PDCH and channel mask, plus merge
                   of Rel C fast calls setup changes. (Some of these changes
                   are not compiled for now - search for BKM.)
12/02/03   bkm     Update SNM ifaces to use separate fwd/rev channel masks
                   instead of OTA chind.  Also remove P2 featurization on
                   internal chind variable.
08/01/03   ph      Added chind to config eval
06/18/03   fc      Fixed the initialization of DCCH frame size.
06/02/03   bkm     Lint cleanup.
12/18/02   ph      Updated process_con_msg() to handle the case when we need to
                   update the pending cfg from a UHDM that has no SCR.
11/07/02   az      Mainlined FEATURE_IS95B
10/03/02   va      Set default fields of pending SCR before action time.
10/03/02   ph      Lint Cleanup
10/01/02   az      Mainlined FEATURE_IS2000
09/16/02   ph      Updated LPM processing sequence for SCM/U/GHDM.
07/26/02   ph      Correctly initialize LPM when we hand-down from PREVs>=6.
07/24/02   ph      For UHDM/GHDM, we'll update LPM irrespective of whether
                   nnscr is included. This will take care of ch_ind, scr and
                   actual LPM changes due to the message.
07/02/02   az      Added new voice service option FEATURE_SO_VOICE_SMV
06/28/02   ph      Included ch_ind from UHDM when setting up pending_cfg LPM table
                   as get_cur_chind() is not updated at this point.
06/05/02   ph      Added LPM initialization for pending cfg, and update of pending
                   cfg with complete LPM when a nnscr arrives in a SCM/UHDM/GHDM.
05/29/02   ph      Added support for LPM table
05/01/02   ph      Some cleanup of featurization.
04/17/02   ph      Added new checks in process_con_msg() and set a return value in
                   the event payload for caller to examine.
02/20/02   az      Featurized 13K voice service option
01/23/02   ph      Clean up legacy design
12/11/01   ph      merged from SVD branch
                   08/23/01   ph      Updates to tc_so_change() call for SVD.
                   08/23/01   ph      Updates for SVD.
10/15/01   kk      Fixed compiler errors when Release A is not defined.
10/02/01   jrp     Merged from MSM_CP.05.00.96
                   08/16/01   sj      Added the voc_config() in srv_cmd_cfg() when
                                      FEATURE_COMPLETE_SNM is not defined.
08/27/01   jrp     Reject configuration if use_old_srv_cfg is set.
07/06/01   ph      Added support for Callback Timers
06/27/01   kk      Fixed merge error.
06/26/01   ph      Merged in from MSM_CP.05.00.81
                   06/14/01   va      Made sure SRV is in wait for timer exp subfunction even
                                      when MS receives  G/U HDM with SCR.
                   05/15/01   fc      Corrected the printout the bogus error message in srv_init
                                      for E/CAM with assign_mode = 4 and grant_mode = 0.
06/14/01   va      Fixed a bug whereby the SRV wouldn't be in "Wait for action
                   time" subfunction for U/G HDM with SCR.
05/17/01   lh      Merged following changes from the common archieve.
  05/11/01   lcc     Corrected the problem not connecting the SCR when it's included
                     with GHDM.  This happens only to non-IS2000 targets.
  04/16/01   lcc     In srv_process_con_msg, not do validation of SCR and NN-SCR
                     if it's triggered by G/UHDM since the validation was already
                     done earlier.
  04/12/01   fc      Added support of events report: Service negotiation
                     completed
                     Renamed mcc_report_event_timer to mclog_report_event_timer.
  04/10/01   lcc     1. Added support for P2 mode.
                     2. Corrected misplaced code in srv_wf_con_subf.
05/04/01   va      Changed the prototype for srv_get_pending_cfg
03/13/01   ht      Moved a feature label to include comments.
03/06/01   fc      Added support of events report: Timer expired.
02/12/01   jq      Added reinitiation of srv req when MS rejects BS's srv
                   connect msg.
01/17/01   lcc     1. Moved some code to initialize SCR and NNCR into the functions
                      snm_is2000_scr_init and snm_is2000_nnscr_init from
                      srv_process_con_msg.
                   2. In srv_init, used snm_is2000_nnscr_init to initialize
                      NNSCR in current and pending cfg instead of explicitly
                      initializing each member in the structure.
12/17/00   ht      Add function srv_set_evrc_usage().  In srv_eval_so(),
                   add user setting check before enabling EVRC.
11/02/00   lcc     Added support for FEATURE_COMPLETE_SNM which obsoleted srv_eval_so,
                   srv_eval_cfg, srv_set_def_cfg, srv_alt_cfg, srv_so_is_enabled,
                   and srv_get_alt_so_list (when the feature is defined).
08/17/00   va      Initialized for_sch_cc_incl, rev_sch_cc_incl in srv_current_cfg
                   in srv_init.
08/03/00   va      Support for IS-733 SO in pref_RCs function. Used the cached
                   info for pref RC in srv also.
07/28/00   fc      Mainline FEATURE_CS_ACTION_TIME_TRANS.
06/12/00   lcc     1. Rename function srv_update_nn_scr_from_pcm to srv_update_
                      nn_scr_from_pcnm to correspond to actual abbreviation used
                      for PCNM.  Also changes in this function to conform to
                      unified action time processing scheme.
                   2. At service connect (at SRV_SIG_EC), removed call to
                      snm_update_configuration which is now called in the pre-
                      processor of SCR or HO transactions
06/02/00   lcc     Minor update to conform to names used in standard.
05/03/00   va      Support for fch_cc_incl to be optional
04/24/00   lcc     Separated snm_update_configuration into 2 functions: snm_update
                   _configuration and snm_continue_update_configuration.  Former
                   being the time critical part (need to be executed right away) while
                   the latter can wait.
04/17/00   va      Made srv_get_pref_RCs return nothing
04/10/00   va      Support for reading pref_RC from NV
04/07/00   va      Merged the following from PP (MSM3100_CP.03.00.17):
03/14/00   ry      Added OTAPA support
03/28/00   ry      Fixed compiler errors and warnings for Pink Panther build
03/15/00   ry      Added IS2000 featurization
           ry      Merged the following from PP (MSM3100_CP.03.00.10):
           fc      Added support for IS-733 service option number 0x11.
02/13/00    va     Addendum Changes.
01/27/00    va     Merged the following changes:
                   Change ERR_FATAL to ERR in svr_get_alt_so_list(), since
                   the mobile still needs to send Page Response Msg with the
                   invalid service option to the Base Station.
12/20/99    va     Support for updating non-neg info from info in the power
                   control message.
11/15/99    va     IS2000 Changes:
                   Support for new service configuration and non negotiable
                   service configuration.
                   Interface srv_get_pref_rcs to get prefereed RCs for a given SO.
                   Interface srv_get_pending_cfg to get the pending configuration.
                   Action times of SCR moved to lower layers.
11/03/99   lcc     Merged from GD4021:
           doj     Changed wait time for handoff message with implicit action
                   time from 20 ms to 0 ms, since the 20 ms is already done
                   in handoff processing.
           ry      Updated srv_norm_subf(), srv_wf_req_subf(),
                   srv_wf_rsp_subf(), srv_wf_con_subf(), and srv_wf_tim_subf()
                   to handle restoring the service configuration records upon
                   receipt of the SRV_RIF_HHO_EC event
           kmp     Added #ifdef FEATURE_IS95B || FEATURE_SPECIAL_MDR to
                   srv_process_con_msg() to fix compilation serious errors when
                   95B and MDR (any form of MDR) are not defined
           nmn     Added IS-96A vocoder capability checking for Alt SO
           lh      Merged following change:
           nmn     Added IS-96A vocoder capability checking
08/24/99   nmn     Updated Alt SO for 2-way SMS
08/06/99   nmn     Removed IS-96
07/30/99   lcc     Replaced calls to srv_eval_cfg and srv_alt_cfg with
                   snm_eval_config (which encapsulates srv_eval_cfg and
                   srv_alt_cfg) and some cleanup as a result of that.
07/19/99   nmn     * Removed DS operations from srv_eval_so, srv_eval_cfg, and
                     srv_set_def_cfg, replaced them with ANALOG_FAX, because
                     the rest of the DS SOs are in SNM, not needed here.
                   * Added 13k checking to some service options in srv_eval_so.
                   * Added #ifdef FEATURE_ANALOG_FAX in srv_eval_so.
                   * Added srv_get_alt_so_list and helper function
                     srv_so_is_enabled (both under #ifdef FEATURE_IS95B or
                     FEATURE_SPECIAL_MDR).
06/16/99   doj     Added support for service configuration via handoff
                   message.  Put Lint exception at end of srv_rpt_subs, since
                   we know we do not use the variable ec yet.  Cast time
                   calculations to words in srv_process_con_msg to fix Lint
                   errors.  Cast several functions to void to stop Lint
                   complaint about ignoring return values.
06/08/99   kmp     Merge the IS95B changes below. Updated srv_eval_so() since
                   using the p_rev_in_use instead of P_REV.
           kmp     Removed JSTD008 in srv_eval_so() and replaced with P_REV
                   checking.
           jq      Modified srv_init() to support default_config parameter
                   required by the unified CAM/ECAM implementation
06/07/99   lcc     Fixed reopened CR7630.  Phone should not accept 1/4 rates frames
                   for RLP RS1 service options (this reverses CR8456).
05/14/99   lcc     In srv_process_con_msg, added call to snm_eval_config directly
                   (which is also called in srv_eval_cfg) but with suggest_alt set
                   to false.  This makes snm_eval_config to return successful as
                   long as the configuration is acceptable (no attempt to suggest
                   a better configuration).
05/02/99   lcc     When srv_eval_cfg is called to evaluate a configuration,
                   the first connection is checked for CAI_SO_NULL (instead of
                   CAI_SO_REJ) to determine if srv_alt_cfg needs to be called
                   to generate a alternative configuration.  Also corrected a
                   problem in srv_alt_cfg where CAI_SO_NULL should have been
                   CAI_SO_REJ, which is returned by srv_eval_so to indicate
                   failed SO.
02/25/99   pms     Fixed CR8456.  Mobile does not reject Service Connect Msgs
                   where 1/4 rate frames are included for RLP.
02/11/99   lh      Fixed CR8268. Fixed fatal error on unknow SO.
01/27/99   yll     Fixed CR7630. Now phone handles quarter rate correctly
                   in compliance with IS707.4.
11/30/98   ck      Fixed the missing braces in the srv_eval_cfg function.

02/12/99   lcc     Added featurization with FEATURE_SPECIAL_MDR and changed some
                   names to conform to standard.
01/15/99   lcc     Obsoleted function srv_alt_so and merged following 3 items
                   from MDR branch:
01/16/98   nmn     Fixed some SNM-DS conflicts
12/03/98   lcc     Added some comments.
11/30/98   lcc     Added support for service negotiation manager.
11/17/98   ck      The evaluation of service configuration for SOs 6 and 14
                   was modified to be TSB-79 compliant.
10/27/98   ck      Added the evaluation of service option as part of
                   evaluating the service configuration.
10/20/98   ldg     Added new SO's for IS-707A analog end to end fax.
10/02/98   ck      Neared variables to save ROM space.
08/25/98   ck      Added support for CAI_SO_RS2_SMS and negotiation of Rate
                   Set 2 SMS down to Rate Set 1 SMS.
08/20/98   ck      Using new voc_capability_supported interface instead of
                   db_13k_voc_available.
08/11/98   ck      Changed Service Negotitation for EVRC
01/27/98   ldg     Commented usages of the unusual DS_CASE macro.
01/26/98   fkm     FEATURE_xxx Updates (via Automatic Scripts)
11/03/97   ldg     Replaced references to specific data service options with
                   a macro which represents the same information.
                   Added support for additional sets of data service options;
                   IS-707.0, Qualcomm proprietary, and IS-707 are now
                   supported.
06/30/97   rdh     Added T_Q support.
06/12/97   ck      Modified Service Negotiation for EVRC.
06/10/97   ck      Added copyright notice.
06/10/97   ck      Added support to negotiate EVRC
11/14/96   jjw     Added Rate Set 2 data service capabilities
10/21/95   gb      Removed dependancy of all 13k SOs on 13k vocoder.
08/10/95   gb      Added New Markov Service Options.
07/14/95   rdb     Minor cleanup prior to relase. Changed MSG_ERROR calls
                   to MSG_MED calls.
07/13/95   rdb     Did lots of cleaning up. Added better processing for
                   Service Connect Message action times. Added logic to
                   propose alternative service configurations.
07/12/95   rdb     Comments, clean up, careful going over.
07/11/95   rdb     Version for intermediate tests (granted mode = 0, 1).
07/10/95   rdb     First version to support simple (streamlined) tests.
06/08/95   rdb     First cut of module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "queue.h"
#include "rex.h"
#include "task.h"
#include "err.h"
#include "msg.h"
#include "m1x_diag.h"
#include "mc_v.h"
#include "db.h"
#include "qw.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "cai_v.h"
#include "caii_v.h"
#include "rxtx_v.h"
#include "srv.h"
#include "snm_v.h"
#include "snm_i.h"
#include "mccccl.h"
#include "mclog_v.h"
#include "event.h"
#include "mcctcsup.h"
#if defined(FEATURE_IS2000_REL_C)
#include "mccsyobj_v.h"
#include "mccsyncid.h"
#endif /* FEATURE_IS2000_REL_C */

#ifdef FEATURE_IS2000_REL_A_AES
#include "mccap_v.h"
/* CMI Effort - Temporarily commenting out
#include "dsrlp_v.h" */

#endif /* FEATURE_IS2000_REL_A_AES */

/*lint -e818 -e655 */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
                  Service Subfunction Codes

These codes are used to identify the Service Subfunctions.
-------------------------------------------------------------------------*/

typedef enum
{
  SRV_NORM_SUBF,     /* Normal service subfunction */
  SRV_WF_REQ_SUBF,   /* Waiting for service request message subfunction */
  SRV_WF_RSP_SUBF,   /* Waiting for service response message subfunction */
  SRV_WF_CON_SUBF,   /* Waiting for service connect message subfunction */
  SRV_WF_TIM_SUBF    /* Waiting for service action time subfunction */
} srv_subf_type;

/*-------------------------------------------------------------------------
                            Variables
-------------------------------------------------------------------------*/

srv_subf_type srv_subf;
  /* Keeps track of the current service negotiation subfunction. */

caii_complete_srv_cfg_type  srv_current_cfg;
  /* Holds current active service configuration */

srv_pending_cfg_type  srv_pending_cfg;
  /* Holds pending service configuration */

caii_srv_cfg_type  srv_proposed_cfg;
  /* For building a proposed service configuration to send. */

byte  srv_req_seq;
  /* Keeps track of which request sequence number to use next. Used only
     with service negotiation. */

byte  srv_saved_con_seq;
  /* For remembering fields from the service connect message */

word  srv_saved_so;
  /* Keeps track of which service option the mobile specified in the
     origination message or page response message. */

byte  srv_saved_req_seq;
  /* Keeps track of the request sequence number used in the most
     recently sent service request message. The mobile station ignores
     any service response message which does not echo back this same
     request sequence number. */

boolean  srv_neg_needed;
  /* Indicates whether the mobile station needs to initiate service
     negotiation upon entering the conversation substate during a
     mobile originated call */

/*-------------------------------------------------------------------------
      REGIONAL FUNCTIONS used in this module
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION SRV_SEND_REJ_ORD

DESCRIPTION
  This function sends a reject order containing a specified ordq value
  and a specified rejected message type.

DEPENDENCIES

RETURN VALUE
  SRV_DONE_S - Order succesfully queued to layer 2 task.
  SRV_ERR_S  - Unable to get a buffer to queue up order.

SIDE EFFECTS

===========================================================================*/

srv_status_type  srv_send_rej_ord
(
  byte ordq,    /* Ordq value to put in reject order */
  byte rej_type /* Type of message being rejected */
)
{
  srv_status_type  status; /* For returning status to caller */
  rxtx_cmd_type *cmd_ptr;  /* Pointer to command to send to layer 2 task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get buffer from rxtx_mc_free_q to send message in */
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );

    status = SRV_ERR_S;
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );

    status = SRV_ERR_S;
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    /* Fill in fields of order */
    cmd_ptr->tc_msg.msg.tc_ord.rej.fix.msg_type     = CAI_TC_REV_ORD_MSG;
    cmd_ptr->tc_msg.msg.tc_ord.rej.fix.order        = CAI_MS_REJECT_ORD;
    cmd_ptr->tc_msg.msg.tc_ord.rej.fix.ordq         = ordq;
    cmd_ptr->tc_msg.msg.tc_ord.rej.fix.rej_msg_type = rej_type;

    /* Fill in fields of command */

    cmd_ptr->tc_msg.ack_req = FALSE;

    cmd_ptr->tc_msg.hdr.command            = RXTX_TC_MSG_F;
    cmd_ptr->tc_msg.hdr.cmd_hdr.task_ptr   = NULL;
    cmd_ptr->tc_msg.hdr.cmd_hdr.sigs       = 0;
#ifndef FEATURE_1X_CP_MEM_OPT
    cmd_ptr->tc_msg.hdr.cmd_hdr.done_q_ptr = &rxtx_mc_free_q;
#endif /* !FEATURE_1X_CP_MEM_OPT */

    rxtx_cmd( cmd_ptr );  /* Issue command to rxtx task */

    status = SRV_DONE_S;
  }

  return ( status );

} /* srv_send_rej_ord */

/*===========================================================================

FUNCTION SRV_SEND_REQ_MSG

DESCRIPTION
  This function sends a Service Request Message containing a specified
  purpose code and a specified service configuration.

DEPENDENCIES

RETURN VALUE
  SRV_DONE_S - Message succesfully queued to layer 2 task.
  SRV_ERR_S  - Unable to get a buffer to queue up message.

SIDE EFFECTS
  Modifies the srv_req_seq and srv_saved_req_seq variables.

===========================================================================*/

srv_status_type  srv_send_req_msg
(
  byte  purp,
    /* The purpose code to put in the service request message. Valid
       codes: CAI_SRV_ACC_PURP, CAI_SRC_REJ_PURP, CAI_SRV_PRO_PURP. */

  caii_srv_cfg_type  *cfg_ptr
    /* Ptr to the service configuration to put in the service
       request message. Only significant if purp = CAI_SRV_PRO_PURP. */
)
{
  srv_status_type  status; /* For returning status to caller */
  rxtx_cmd_type *cmd_ptr;  /* Pointer to command to send to layer 2 task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );

    status = SRV_ERR_S;
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );

    status = SRV_ERR_S;
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    /* Fill in message fields */
    cmd_ptr->tc_msg.msg.srv_req.msg_type = CAI_RTC_SRV_REQ_MSG;

    cmd_ptr->tc_msg.msg.srv_req.req_seq  = srv_req_seq;
    srv_saved_req_seq = srv_req_seq;
    srv_req_seq = (srv_req_seq + 1) % 8;

    cmd_ptr->tc_msg.msg.srv_req.purp = purp;

    if ( purp == CAI_SRV_PRO_PURP )
    {
      cmd_ptr->tc_msg.msg.srv_req.cfg = *cfg_ptr;
    }

    /* Fill in fields of command */

    cmd_ptr->tc_msg.ack_req = TRUE;

    cmd_ptr->tc_msg.hdr.command            = RXTX_TC_MSG_F;
    cmd_ptr->tc_msg.hdr.cmd_hdr.task_ptr   = NULL;
    cmd_ptr->tc_msg.hdr.cmd_hdr.sigs       = 0;
#ifndef FEATURE_1X_CP_MEM_OPT
    cmd_ptr->tc_msg.hdr.cmd_hdr.done_q_ptr = &rxtx_mc_free_q;
#endif /* !FEATURE_1X_CP_MEM_OPT */

    /* Send message to RXTX to be transmitted to the base station */
    rxtx_cmd( cmd_ptr );

    status = SRV_DONE_S;
  }

  return ( status );

} /* srv_send_req_msg */

/*===========================================================================

FUNCTION SRV_SEND_RSP_MSG

DESCRIPTION
  This function sends a Service Response Message containing a specified
  request sequence number, purpose code, and service configuration.

DEPENDENCIES

RETURN VALUE
  SRV_DONE_S - Message succesfully queued to layer 2 task.
  SRV_ERR_S  - Unable to get a buffer to queue up message.

SIDE EFFECTS

===========================================================================*/

srv_status_type  srv_send_rsp_msg
(
  byte  req_seq,
    /* The service request sequence number to put in the service response
       message */

  byte  purp,
    /* The purpose code to put in the service response message. Valid
       codes: CAI_SRV_ACC_PURP, CAI_SRV_REJ_PURP, CAI_SRV_PRO_PURP. */

  caii_srv_cfg_type  *cfg_ptr
    /* Ptr to the service configuration to put in the service
       response message. Only significant if purp = CAI_SRV_PRO_PURP. */
)
{
  srv_status_type  status; /* For returning status to caller */
  rxtx_cmd_type *cmd_ptr;  /* Pointer to command to send to layer 2 task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );

    status = SRV_ERR_S;
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );

    status = SRV_ERR_S;
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    /* Fill in fields of message */

    cmd_ptr->tc_msg.msg.srv_rsp.msg_type = CAI_RTC_SRV_RSP_MSG;

    cmd_ptr->tc_msg.msg.srv_rsp.req_seq  = req_seq;
    cmd_ptr->tc_msg.msg.srv_rsp.purp = purp;

    if ( purp == CAI_SRV_PRO_PURP )
    {
      cmd_ptr->tc_msg.msg.srv_rsp.cfg = *cfg_ptr;
    }

    /* Fill in fields of command */

    cmd_ptr->tc_msg.ack_req = TRUE;

    cmd_ptr->tc_msg.hdr.command            = RXTX_TC_MSG_F;
    cmd_ptr->tc_msg.hdr.cmd_hdr.task_ptr   = NULL;
    cmd_ptr->tc_msg.hdr.cmd_hdr.sigs       = 0;
#ifndef FEATURE_1X_CP_MEM_OPT
    cmd_ptr->tc_msg.hdr.cmd_hdr.done_q_ptr = &rxtx_mc_free_q;
#endif /* !FEATURE_1X_CP_MEM_OPT */

    /* Send message to RXTX to be transmitted to the base station */
    rxtx_cmd( cmd_ptr );

    status = SRV_DONE_S;
  }

  return ( status );

} /* srv_send_rsp_msg */

/*===========================================================================

FUNCTION SRV_SEND_CMP_MSG

DESCRIPTION
  This function sends a service connect completion message containing a
  specified service connect sequence number.

DEPENDENCIES

RETURN VALUE
  SRV_DONE_S - Message succesfully queued to layer 2 task.
  SRV_ERR_S  - Unable to get a buffer to queue up message.

SIDE EFFECTS

===========================================================================*/

srv_status_type  srv_send_cmp_msg
(
  byte  con_seq /* The service connect sequence number to put in message. */
)
{
  srv_status_type  status; /* For returning status to caller */
  rxtx_cmd_type *cmd_ptr;  /* Pointer to command to send to layer 2 task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );

    status = SRV_ERR_S;
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );

    status = SRV_ERR_S;
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    /* Fill in fields of message */

    cmd_ptr->tc_msg.msg.srv_cmp.msg_type = CAI_SRV_CMP_MSG;
    cmd_ptr->tc_msg.msg.srv_cmp.con_seq  = con_seq;

    /* Fill in fields of command */

    cmd_ptr->tc_msg.ack_req = TRUE;

    cmd_ptr->tc_msg.hdr.command            = RXTX_TC_MSG_F;
    cmd_ptr->tc_msg.hdr.cmd_hdr.task_ptr   = NULL;
    cmd_ptr->tc_msg.hdr.cmd_hdr.sigs       = 0;
#ifndef FEATURE_1X_CP_MEM_OPT
    cmd_ptr->tc_msg.hdr.cmd_hdr.done_q_ptr = &rxtx_mc_free_q;
#endif /* !FEATURE_1X_CP_MEM_OPT */

    /* Send message to RXTX to be transmitted to the base station */
    rxtx_cmd( cmd_ptr );

    status = SRV_DONE_S;

    #ifdef FEATURE_IS2000_REL_A_AES
    /* when AES feature is defined, for specific voice encryption/decryption use,
     * we save the sr_id of the voice service. If no voice service, the saved
     * value will be MCCAES_INVALID_VOICE_SR_ID
     *
     * from the same function call, it will also get the ui_encrypt_mode
     * for the voice service. If voice service does not exist, the ui_encrypt_mode
     * does not mean anything here.
     */
    if ( mcc_ms_supports_ui_aes() )
    {
      cdma.aes_status.voice_so_sr_id = snm_get_voice_so_sr_id(&cdma.aes_status.voice_so_ui_encrypt_mode);
    }
    #endif /* FEATURE_IS2000_REL_A_AES */
  }

  return ( status );

} /* srv_send_cmp_msg */

/*===========================================================================

FUNCTION SRV_CMD_CFG

DESCRIPTION
      This function commands the multiplex sublayer and service option
      applications to begin using a specified service configuration at
      a specified action time.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void  srv_cmd_cfg
(
  caii_srv_cfg_type  *cfg_ptr
)
{
  tc_so_change ( cfg_ptr );

} /* srv_cmd_cfg */

/*===========================================================================

FUNCTION SRV_RPT_SUBS

DESCRIPTION
      This function reports substate change events to the multiplex
      sublayer and service option applications. This is necessary
      because some service option applications are substate sensitive.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void  srv_rpt_subs
(
  srv_event_code_type ec
    /* The event report which tells what traffic channel substate event
       has occurred. */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*!!! More to come */

} /* srv_rpt_subs */ //lint !e715 We know we do not reference ec (yet).

/*===========================================================================

FUNCTION SRV_PROCESS_CON_MSG

DESCRIPTION
      This function performs common processing for service connect
      messages received.  It also handles a General Handoff Direction Message
      if it contains service configuration information.

DEPENDENCIES

RETURN VALUE
  Which service subfunction to invoke next (SRV_NORM_SUBF or
  SRV_WF_TIM_SUBF).

SIDE EFFECTS
  Also sets the status field in the event report (e_ptr->status).

===========================================================================*/

srv_subf_type srv_process_con_msg
(
  srv_event_type  *e_ptr
    /* Pointer to event block describing event. */
)
{
  srv_subf_type  subf= SRV_NORM_SUBF;
    /* Used for returning the service subfunction to invoke next time. */
    /* Initailzed so that Lint does not complain */

  byte action_time = 0;
    /* For use in time calculations. */

  byte con_seq = srv_saved_con_seq;
    /* Connection sequence */

  boolean use_time = 0;
    /* Indicates whether we should use ACTION_TIME */

  byte msg_type = 0;
    /* Message type for reject message */

  caii_srv_cfg_type *cfg_ptr = NULL;
    /* Negotiable configuration pointer */

  caii_non_neg_srv_cfg_type *nn_cfg_ptr = NULL;
    /* Non-negotiable configuration pointer */

  byte sr_id_list[1]; /* For use in call to initialize def LPM */

  boolean cfg_okay = FALSE, continue_cfg_eval = TRUE;

  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

  boolean restore_ind = FALSE;
    /* Indicates if stored config is to be restored */

#if defined(FEATURE_IS2000_REL_C)
  uint8 rej_code;
#endif /* FEATURE_IS2000_REL_C */

  mcc_fwd_chan_mask_type fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask();
  mcc_rev_chan_mask_type rev_chan_mask = mccccl_code_channel_list.get_rev_chan_mask();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize to reject message */
  rej_parms.rej_code = CAI_REJ_CFG;

  if (e_ptr->ec == SRV_CON_MSG_EC)
  {
    /* Need to clear the substate timer whether we accept this config or not */
    (void) mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID);

    e_ptr->ep.con_msg.scm_validation_success = FALSE;

    /* Make assignments from Service Connect Message structure. */
    cfg_ptr    = &e_ptr->ep.con_msg.msg_ptr->cfg;
    con_seq     = e_ptr->ep.con_msg.msg_ptr->con_seq;
    msg_type    = e_ptr->ep.con_msg.msg_ptr->msg_type;
    use_time    = e_ptr->ep.con_msg.msg_ptr->use_time;
    action_time = e_ptr->ep.con_msg.msg_ptr->action_time;
    nn_cfg_ptr = &e_ptr->ep.con_msg.msg_ptr->non_neg_cfg;

#ifdef FEATURE_IS2000_REL_A
    if (P_REV_IS_7_OR_GREATER && e_ptr->ep.con_msg.msg_ptr->use_old_srv_cfg
#if defined(FEATURE_IS2000_REL_C)
        && !P_REV_IS_9_OR_GREATER
#endif /* FEATURE_IS2000_REL_C */
       )
    {
      // Reject SCM if use_old_srv_cfg is set.
      continue_cfg_eval = FALSE;
      M1X_MSG( DCP, LEGACY_HIGH,
        "Rej SCM - use_old_srv_cfg %d not supported",
        e_ptr->ep.con_msg.msg_ptr->use_old_srv_cfg);
    }
    else
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_A_SVD
    if (P_REV_IS_7_OR_GREATER && e_ptr->ep.con_msg.msg_ptr->cc.cc_info_incl &&
        (pre_process_cc_info(&(e_ptr->ep.con_msg.msg_ptr->cc), &rej_parms) != 0))
    {
      // Reject SCM if the CC info is not acceptable
      continue_cfg_eval = FALSE;
      M1X_MSG( DCP, LEGACY_FATAL,
        "CC Info in SCM not acceptable");
    }
#endif /* FEATURE_IS2000_REL_A_SVD */

#if defined(FEATURE_IS2000_REL_C)
    /* Check SCM for stored service configuration specific */
    else if ((rej_code = mccsyncid_validate_scm(e_ptr->ep.con_msg.msg_ptr)) != 0)
    {
      continue_cfg_eval = FALSE;
      rej_parms.rej_code = rej_code;
    }
    else
#endif /* FEATURE_IS2000_REL_C */

    if (continue_cfg_eval) /*lint !e774 */
    {
#if defined(FEATURE_IS2000_REL_C)
      if (P_REV_IS_9_OR_GREATER && e_ptr->ep.con_msg.msg_ptr->use_old_srv_cfg)
      {
        const restore_srv_cfg_info_type *restore_srv_cfg_info;

        restore_srv_cfg_info = mccsyncid_get_restore_config();

        /* Populate service configuration to be validated */
        *cfg_ptr = restore_srv_cfg_info->cfg.neg_scr;
        *nn_cfg_ptr = restore_srv_cfg_info->cfg.non_neg_scr;

        /* Don't do any update based on current config during SCR evaluation */
        restore_ind = TRUE;
      }
#endif /* FEATURE_IS2000_REL_C */

      /* Make sure the configuration passed in is acceptable.  Note NULL is used
         as the alt cfg pointer which is not used in this case. */
      if (snm_eval_config(cfg_ptr, NULL, restore_ind, FALSE,
          fwd_chan_mask, rev_chan_mask) == SNM_ACCEPT_CFG )
      {
        if (!P_REV_IS_6_OR_GREATER)
        {
          cfg_okay = TRUE;
        }
        else
        {
          /* Need to update the nnscr with right LPM values before it is
             evaluated and copied in to pending_cfg. Note that we specify
             the new nnscr as the target nnscr in this call, this results
             in updating the LPM if the LPM_IND is DEFAULT or LPM_USE_LAST */
          snm_update_lpm_table(nn_cfg_ptr, nn_cfg_ptr, cfg_ptr,
            fwd_chan_mask, rev_chan_mask);

          if (snm_eval_non_neg_scr(nn_cfg_ptr, cfg_ptr, fwd_chan_mask,
              rev_chan_mask) != SNM_SUCCESS)
          {
            M1X_MSG( DCP, LEGACY_HIGH,
              "NNSCR not acceptable, rejecting SCM");
          }
          else
          {
            cfg_okay = TRUE;
          }
        }

        if (cfg_okay)
        {
          e_ptr->ep.con_msg.scm_validation_success = TRUE;

#if defined(FEATURE_IS2000_REL_C)
          /* Perform initialization for service configuration restoration */
          mccsyncid_scm_init(e_ptr->ep.con_msg.msg_ptr);
#endif /* FEATURE_IS2000_REL_C */
        }
      }
    } /* end if (continue_cfg_eval) */
  }
  else if (e_ptr->ec == SRV_HO_DIR_EC)
  {

    /* Make assignments from Generic Handoff Message structure.  This should
       work correctly for any handoff message (GHDM or the future UHDM). */

    if (e_ptr->ep.ho_msg.msg_ptr->extra.scr_included)
    {
      /* Need to clear the substate timer whether we accept this config or not */
      (void) mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID);

      con_seq = e_ptr->ep.ho_msg.msg_ptr->extra.serv_con_seq;
    }
    else /* Note: If P_REV is less than 6, this function will always be called
            with 'scr_included' equal to TRUE; But, we will be here if we need
            to handle an U/GHDM with an NNSCR, but with no SCR */
    {
      con_seq = srv_saved_con_seq;
    }

    /* We don't have to inspect e_ptr->ep.ho_msg.msg_ptr->extra.nnscr_included
       or e_ptr->ep.ho_msg.msg_ptr->extra.scr_included
       here because e_ptr->ep.ho_msg.msg_ptr->extra.non_neg_cfg and
       e_ptr->ep.ho_msg.msg_ptr->extra.cfg are guaranteed
       to be updated (validate_handoff_msg) with the right scr/nnscr irrespective
       of whether one is included in the message or not */
    cfg_ptr = &e_ptr->ep.ho_msg.msg_ptr->extra.cfg;
    nn_cfg_ptr = &e_ptr->ep.ho_msg.msg_ptr->extra.non_neg_cfg;
    msg_type = e_ptr->ep.ho_msg.msg_ptr->msg_type;
    use_time = 0;
    action_time = 0;
    /* For the current design, USE_TIME should always be FALSE, and/or
       ACTION_TIME should be zero.  Since this function is called at the
       action time of the handoff message, we want the service config
       change to happen immediately. */

    /* For G/UHDM, the configuration was already validated earlier, no need
       to repeat it here.*/
    cfg_okay = TRUE;
  }

#if defined(FEATURE_IS2000_REL_C)
  else if (e_ptr->ec == SRV_CLAM_MSG_EC)
  {
    const restore_srv_cfg_info_type *restore_srv_cfg_info;

    /* Need to clear the substate timer whether we accept this config or not */
    (void) mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID);

    e_ptr->ep.clam_msg.clam_validation_success = TRUE;

    msg_type    = e_ptr->ep.clam_msg.msg_ptr->msg_type;
    use_time    = 0;
    action_time = 0;
    con_seq = srv_saved_con_seq;

    restore_srv_cfg_info = mccsyncid_get_restore_config();
    cfg_ptr = (caii_srv_cfg_type *) &restore_srv_cfg_info->cfg.neg_scr;
    nn_cfg_ptr = (caii_non_neg_srv_cfg_type *) &restore_srv_cfg_info->cfg.non_neg_scr;

    /* Perform initialization for service configuration restoration */
    mccsyncid_clam_init(e_ptr->ep.clam_msg.msg_ptr);

    /* The configuration was already validated eariler */
    cfg_okay = TRUE;
  }
#endif /* FEATURE_IS2000_REL_C */

  else
  {
    /* Should never happen */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid EC %d",
      e_ptr->ec);
    return ( subf );
  }

  if ( cfg_okay )
  {
    srv_saved_con_seq = con_seq;

    srv_pending_cfg.cfg.neg_scr  = *cfg_ptr; /*lint !e613 initialize properly if cfg_okay */
    srv_pending_cfg.cfg_msg_type = msg_type;

    /* Initialize fields that didn't arrive over the air for
       which we need to use current config */
    snm_set_default_fields_in_scr(&srv_pending_cfg.cfg.neg_scr);

    // Update Non_Neg SCR
    if (P_REV_IS_6_OR_GREATER)
    {
      srv_pending_cfg.cfg.non_neg_scr = *nn_cfg_ptr;/*lint !e613 initialize properly if cfg_okay */
      /* Initialize fields that didn't arrive over the air for
      which we need to use current config */
      snm_set_default_fields_in_nnscr( &srv_pending_cfg.cfg.non_neg_scr);
    }
    else
    {
      //p_rev is not 6, so no special fields of IS2000 present
      // We better fill in those fields correctly
      snm_is2000_scr_init( &srv_pending_cfg.cfg.neg_scr );
      /* Now initialize some of the non neg parameters */
      snm_is2000_nnscr_init( &srv_pending_cfg.cfg.non_neg_scr );
      /* Initialize the LPM to default values */
      sr_id_list[0]=1;
      snm_is2000_nnscr_lpm_default(&srv_pending_cfg.cfg.non_neg_scr, 1,
        sr_id_list,
        MCC_F_FCH,  /* Forward */
        MCC_R_FCH,  /* Reverse, R-PICH is not relevant */
        FALSE);
    }

    // For IS2000, service is connection in G/UHDM do_successful_handoff_processing
    if ((e_ptr->ec == SRV_CON_MSG_EC)
#if defined(FEATURE_IS2000_REL_C)
        || (e_ptr->ec == SRV_CLAM_MSG_EC)
#endif /* FEATURE_IS2000_REL_C */
       )
    {
      /* No need to do the following for HDM, these things are handled in mcctcho module*/
      if (!use_time)
      /* use implicit action time */
      {
        action_time = 0xff;
      }

      /* Set subfunction timer and activate Waiting for Service Action
       Time Subfunction */
      tc_inform_scr_change(srv_pending_cfg.cfg.neg_scr.con[0].so, action_time, TRUE);

    }

    if ((e_ptr->ec == SRV_HO_DIR_EC) &&
        (!e_ptr->ep.ho_msg.msg_ptr->extra.scr_included))
    {
      if (subf == SRV_WF_TIM_SUBF)  /*lint !e774 */
      {
        /* Note this means we already have a pending SCR/NNSCR but this is going to
           get voided because we are now changing the pending cfg anyway, so change the
           subf back to normal */
        subf = SRV_NORM_SUBF;
        M1X_MSG( DCP, LEGACY_HIGH,
          "Changing SUBF becos SCR trans getting voided");
      }
      else
      {
        subf = srv_subf; /* leave subf unchanged if there is no SCR in the g/uhdm */
      }
    }
    else
    {
      subf = SRV_WF_TIM_SUBF;
    }

    e_ptr->status = SRV_DONE_S;  /* Set event report status */

  }
  else
  {
    rej_parms.rej_msg_type = msg_type;
    e_ptr->status = SRV_DONE_S;

    /* Send a reject order to reject the configuration */
    send_rej_order(&rej_parms);

    (void)mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID );
    subf = SRV_NORM_SUBF;  /* Return to normal subfunction */
  }

  return ( subf );

} /* srv_process_con_msg */

/*===========================================================================

FUNCTION SRV_PROCESS_CTL_MSG

DESCRIPTION
      This function performs common processing for service option control
      messages received.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  Also sets the status field in the event report (e_ptr->status).
===========================================================================*/

void  srv_process_ctl_msg
(
  srv_event_type  *e_ptr
    /* Pointer to event block describing event. */
)
{
  /*!!! More to come */
  /*!!! The idea here is to pass the service option control message */
  /*!!! to the appropriate service option application */

  e_ptr->status = SRV_DONE_S;

} /* srv_process_ctl_msg */

/*===========================================================================

FUNCTION SRV_NORM_SUBF

DESCRIPTION
      This function is part of the service negotiation state machine. It
      corresponds to the Normal Service Subfunction.

DEPENDENCIES
  None.

RETURN VALUE
  Value specifying the next subfunction to invoke.

SIDE EFFECTS
  None.
===========================================================================*/

srv_subf_type  srv_norm_subf
(
  srv_event_type  *e_ptr
    /* Pointer to event block describing event. */
)
{
  srv_subf_type  subf = SRV_NORM_SUBF;
    /* Used for returning the service subfunction to invoke next time.
       Initialized to the current subfunction. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( e_ptr->ec )
  {
    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_SIG_EC:
      /* Shouldn't happen, but harmless. */

      /* Clear timer and associated signal */
      (void)mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID );

      e_ptr->status = SRV_DONE_S;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#if defined(FEATURE_IS2000_REL_C)
    case SRV_SCR_CMP_EC:
      /* Action time has arrived */

      /* Time to update configuration
         Do only the non-time critical portion of configuration update
         For IS2000 targets, the time critical portion has already been done earlier. */
      snm_continue_update_config();

      /* Event report: Service negotiation completed */
      event_report(EVENT_SERVICE_NEGOTIATION_COMPLETED);

      e_ptr->status = SRV_DONE_S;
      break;
#endif /* FEATURE_IS2000_REL_C */

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_UI_EC:
      /* Evaluate user request */
      // Note : don't change order of condition below
      if (snm_so_is_enabled(e_ptr->ep.ui.so) &&
          snm_gen_config(e_ptr->ep.ui.so, &srv_proposed_cfg, TRUE,
          SNM_MAXIMUM_MUX, 0, mccccl_code_channel_list.get_fwd_chan_mask(),
          mccccl_code_channel_list.get_rev_chan_mask()) == SNM_SUCCESS)
      {
        e_ptr->status = SRV_DONE_S;

        if ( e_ptr->status == SRV_DONE_S )
        {
          e_ptr->status = srv_send_req_msg ( CAI_SRV_PRO_PURP,
                                             &srv_proposed_cfg );

          (void)mcc_set_callback_timer(&mcc_srv_timer, CAI_T68M, MCC_SRV_TIMER_ID);
          subf = SRV_WF_RSP_SUBF;
        }
      }

      else
      {
        /* Not able to support requested service option. */
        e_ptr->status = SRV_NOTSUP_S;
      } /* else */

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_REQ_MSG_EC:

      if ( e_ptr->ep.req_msg.msg_ptr->purp == CAI_SRV_REJ_PURP )
      {
        e_ptr->status = srv_send_rej_ord ( CAI_REJ_STATE, CAI_FTC_SRV_REQ_MSG );
      }

      else if ( e_ptr->ep.req_msg.msg_ptr->purp == CAI_SRV_PRO_PURP )
      {
        // Evaluate the configuration, requesting alternate configuration to
        // be generated if necessary, and then process appropriately
        switch ( snm_eval_config( &e_ptr->ep.req_msg.msg_ptr->cfg,
                                  &srv_proposed_cfg, FALSE, TRUE,
                                  mccccl_code_channel_list.get_fwd_chan_mask(),
                                  mccccl_code_channel_list.get_rev_chan_mask()
                                ) )
        {
          case SNM_ACCEPT_CFG : // Accept the configuration
            e_ptr->status =
              srv_send_rsp_msg ( e_ptr->ep.req_msg.msg_ptr->req_seq,
                                 CAI_SRV_ACC_PURP,
                                 NULL );
            /* Set subfunction timer and activate the Waiting for Service
               Connect Message Subfunction */
            (void)mcc_set_callback_timer(&mcc_srv_timer, CAI_T65M, MCC_SRV_TIMER_ID);
            subf = SRV_WF_CON_SUBF;
            break;

          case SNM_ALT_CFG :   // Propose an alternative configuration
            e_ptr->status =
              srv_send_rsp_msg ( e_ptr->ep.req_msg.msg_ptr->req_seq,
                                 CAI_SRV_PRO_PURP,
                                 &srv_proposed_cfg );
            /* Set subfunction timer and activate the Waiting for Service
               Request Message Subfunction */

            (void)mcc_set_callback_timer(&mcc_srv_timer, CAI_T68M, MCC_SRV_TIMER_ID);
            subf = SRV_WF_REQ_SUBF;
            break;

          case SNM_REJECT_CFG : // Reject the configuration
            e_ptr->status =
              srv_send_rsp_msg ( e_ptr->ep.req_msg.msg_ptr->req_seq,
                                 CAI_SRV_REJ_PURP,
                                 NULL );
            break;
        } //lint !e744, default not needed since all values are evaluated
      }

      else
      {
        e_ptr->status = srv_send_rej_ord ( CAI_REJ_FIELD, CAI_FTC_SRV_REQ_MSG );
      }

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_RSP_MSG_EC:

      e_ptr->status = srv_send_rej_ord ( CAI_REJ_STATE, CAI_FTC_SRV_RSP_MSG );

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_CON_MSG_EC:
    case SRV_HO_DIR_EC:
#if defined(FEATURE_IS2000_REL_C)
    case SRV_CLAM_MSG_EC:
#endif /* FEATURE_IS2000_REL_C */

      /* Yes, these cases fall through, since the Service Connect Message
         Call Assignment Message and Handoff Direction Message use the same
         function for a service connection. */

      subf = srv_process_con_msg ( e_ptr );
        /* Perform common processing for service connect or handoff message
           event (e_ptr->status will be set on return) */

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_CTL_MSG_EC:

      srv_process_ctl_msg ( e_ptr );

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_TC_WFO_EC:
    case SRV_TC_WFA_EC:
    case SRV_TC_REL_EC:
      srv_rpt_subs ( e_ptr->ec );
      e_ptr->status = SRV_DONE_S;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_TC_CON_EC:
      /* Mobile station just entered conversation substate */

      if ( srv_neg_needed )
      {
        /* Mobile is required to send a service request message */

        srv_neg_needed = FALSE;  /* Mark flag as processed */

        // Generate the first configuration for an origination.  Max mux options
        // will be used.  The generated configuration will be placed in
        // *negotiation_info.proposed_cfg (pointing to srv_proposed_cfg).
        // To support gradual migration to SNM, the old method is used if this
        // fails.
        if ( snm_first_orig_config( srv_saved_so ) == SNM_SUCCESS )
        {
          e_ptr->status = SRV_DONE_S;
        }
        else
        {
          // Above must succeed if all SOs are managed by SNM.  If it fails, it's
          // an error.  Note that a default config will still be built in that case.
          e_ptr->status = SRV_ERR_S;
        }

        if ( e_ptr->status == SRV_DONE_S )
        {
          e_ptr->status = srv_send_req_msg ( CAI_SRV_PRO_PURP,
                                             &srv_proposed_cfg );
            /* Send service request message */

          /* Set up subfunction timer and activate the Waiting for
             Service Response Message Subfunction */

          (void)mcc_set_callback_timer(&mcc_srv_timer, CAI_T68M, MCC_SRV_TIMER_ID);
          subf = SRV_WF_RSP_SUBF;
        }

        else
        {
          e_ptr->status = SRV_ERR_S;
          M1X_MSG( DCP, LEGACY_ERROR,
            "Cfg not built - so: %x",
            srv_saved_so );
        }

      } /* if */

      else /* srv_neg_needed == FALSE */
      {
        e_ptr->status = SRV_DONE_S;
      }

        /* Report substate change (i.e., entry into the Conversation
           Substate) to multiplex sublayer and service option applications */
      srv_rpt_subs ( e_ptr->ec );

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_RIF_HHO_EC:
      M1X_MSG( DCP, LEGACY_HIGH,
        "Rxed SRV_RIF_HHO_EC in normal subf " );
      e_ptr->status = SRV_DONE_S;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Unknown event - ec: %x",
        e_ptr->ec );
      e_ptr->status = SRV_BADPARM_S;
      break;

  } /* switch */ /*lint !e788 */

  return ( subf );

} /* srv_norm_subf */

/*===========================================================================

FUNCTION SRV_WF_REQ_SUBF

DESCRIPTION
      This function is part of the service negotiation state machine. It
      corresponds to the Waiting for Service Request Message Subfunction.

DEPENDENCIES
  None.

RETURN VALUE
  Value specifying the next subfunction to invoke.

SIDE EFFECTS
  None.
===========================================================================*/

srv_subf_type  srv_wf_req_subf
(
  srv_event_type  *e_ptr
    /* Pointer to event block describing event. */
)
{
  srv_subf_type  subf = SRV_WF_REQ_SUBF;
    /* Used for returning the service subfunction to invoke next time.
       Initialized to the current subfunction. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( e_ptr->ec )
  {
    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_SIG_EC:
      /* Must be that substate timer expired. */

      (void)mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID );

      subf = SRV_NORM_SUBF;

      e_ptr->status = SRV_DONE_S;

      /* Event report: Timer T68m expired */
      mclog_report_event_timer(CAI_TIMER_T68M);

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#if defined(FEATURE_IS2000_REL_C)
    case SRV_SCR_CMP_EC:
      /* Action time has arrived */

      M1X_MSG( DCP, LEGACY_MED,
        "SCR AT arrives in SRV_WF_REQ");

      /* Time to update configuration
         Do only the non-time critical portion of configuration update
         For IS2000 targets, the time critical portion has already been done earlier. */
      snm_continue_update_config();

      /* Event report: Service negotiation completed */
      event_report(EVENT_SERVICE_NEGOTIATION_COMPLETED);

      e_ptr->status = SRV_DONE_S;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
#endif /* FEATURE_IS2000_REL_C */

    case SRV_UI_EC:

      /* New user requests not allowed right now */
      e_ptr->status = SRV_NOTNOW_S;

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_REQ_MSG_EC:
      if ( e_ptr->ep.req_msg.msg_ptr->purp == CAI_SRV_REJ_PURP )
      {
        (void)mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID );
        subf = SRV_NORM_SUBF;
        e_ptr->status = SRV_DONE_S;
      }

      else if ( e_ptr->ep.req_msg.msg_ptr->purp == CAI_SRV_PRO_PURP )
      {
        // Evaluate the configuration, requesting alternate configuration to
        // be generated if necessary, and then process appropriately
        switch ( snm_eval_config( &e_ptr->ep.req_msg.msg_ptr->cfg,
                                  &srv_proposed_cfg, FALSE, TRUE,
                                  mccccl_code_channel_list.get_fwd_chan_mask(),
                                  mccccl_code_channel_list.get_rev_chan_mask()
                                ) )
        {
          case SNM_ACCEPT_CFG : // Accept the configuration
            e_ptr->status =
              srv_send_rsp_msg ( e_ptr->ep.req_msg.msg_ptr->req_seq,
                                 CAI_SRV_ACC_PURP,
                                 NULL );
            (void)mcc_set_callback_timer(&mcc_srv_timer, CAI_T65M, MCC_SRV_TIMER_ID);
            subf = SRV_WF_CON_SUBF;
            break;

          case SNM_ALT_CFG :   // Propose an alternative configuration
            e_ptr->status =
              srv_send_rsp_msg ( e_ptr->ep.req_msg.msg_ptr->req_seq,
                                 CAI_SRV_PRO_PURP,
                                 &srv_proposed_cfg );

            /* Reset subfunction timer and remain in this subfunction. */
            (void)mcc_set_callback_timer(&mcc_srv_timer, CAI_T68M, MCC_SRV_TIMER_ID);
            break;

          case SNM_REJECT_CFG : // Reject the configuration
            e_ptr->status =
              srv_send_rsp_msg ( e_ptr->ep.req_msg.msg_ptr->req_seq,
                                 CAI_SRV_REJ_PURP,
                                 NULL );
            (void)mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID );
            subf = SRV_NORM_SUBF;
            break;
        } //lint !e744, default not needed since all values are evaluated
      }
      else
      {
        e_ptr->status = srv_send_rej_ord ( CAI_REJ_FIELD, CAI_FTC_SRV_REQ_MSG );
      }

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_RSP_MSG_EC:

      e_ptr->status = srv_send_rej_ord ( CAI_REJ_STATE, CAI_FTC_SRV_RSP_MSG );
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_CON_MSG_EC:
    case SRV_HO_DIR_EC:

      /* Yes, these two cases fall through, since the Service Connect Message
         and a Handoff Direction Message use the same function for
         a service connection. */

      subf = srv_process_con_msg ( e_ptr );
        /* Perform common processing for service connect or handoff message
           event (e_ptr->status will be set on return) */

      break;

#if defined(FEATURE_IS2000_REL_C)
    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_CLAM_MSG_EC:

      /* Service negotiation in progress, reject CLAM */
      e_ptr->ep.clam_msg.clam_validation_success = FALSE;
      e_ptr->status = SRV_DONE_S;
      break;
#endif /* FEATURE_IS2000_REL_C */

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_CTL_MSG_EC:

      srv_process_ctl_msg ( e_ptr );

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_TC_WFO_EC:
    case SRV_TC_WFA_EC:
    case SRV_TC_CON_EC:
      srv_rpt_subs ( e_ptr->ec );
      e_ptr->status = SRV_DONE_S;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_TC_REL_EC:
      srv_rpt_subs ( e_ptr->ec );

      (void)mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID );
      subf = SRV_NORM_SUBF;
      e_ptr->status = SRV_DONE_S;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_RIF_HHO_EC:
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed SRV_RIF_HHO_EC in Wait for Req subf " );

      e_ptr->status = SRV_DONE_S;
      subf = SRV_NORM_SUBF;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Unknown event - ec: %x",
        e_ptr->ec );
      e_ptr->status = SRV_BADPARM_S;
      break;

  } /* switch */ /*lint !e788 */

  return ( subf );

} /* srv_wf_req_subf */

/*===========================================================================

FUNCTION SRV_WF_RSP_SUBF

DESCRIPTION
      This function is part of the service negotiation state machine. It
      corresponds to the Waiting for Service Response Message Subfunction.

DEPENDENCIES
  None.

RETURN VALUE
  Value specifying the next subfunction to invoke.

SIDE EFFECTS
  None.
===========================================================================*/

srv_subf_type  srv_wf_rsp_subf
(
  srv_event_type  *e_ptr
    /* Pointer to event block describing event. */
)
{
  srv_subf_type subf = SRV_WF_RSP_SUBF;
    /* Used for returning the service subfunction to invoke next time.
       Initialized to the current subfunction. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( e_ptr->ec )
  {
    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_SIG_EC:
      /* Substate timer expired */
      (void)mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID );

      subf = SRV_NORM_SUBF;
      e_ptr->status = SRV_DONE_S;

      /* Event report: Timer T68m expired */
      mclog_report_event_timer(CAI_TIMER_T68M);

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#if defined(FEATURE_IS2000_REL_C)

    case SRV_SCR_CMP_EC:
      /* Action time has arrived */

      M1X_MSG( DCP, LEGACY_ERROR,
        "SCR AT arrives in SRV_WF_RSP");

      /* Time to update configuration
         Do only the non-time critical portion of configuration update
         For IS2000 targets, the time critical portion has already been done earlier. */
      snm_continue_update_config();

      /* Event report: Service negotiation completed */
      event_report(EVENT_SERVICE_NEGOTIATION_COMPLETED);

      e_ptr->status = SRV_DONE_S;
      break;
#endif /* FEATURE_IS2000_REL_C */

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_UI_EC:

      /* New user requests not allowed now. */
      e_ptr->status = SRV_NOTNOW_S;

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_REQ_MSG_EC:

      if ( e_ptr->ep.req_msg.msg_ptr->purp == CAI_SRV_REJ_PURP )
      {
        e_ptr->status = srv_send_rej_ord ( CAI_REJ_STATE, CAI_FTC_SRV_REQ_MSG );
      }

      else if ( e_ptr->ep.req_msg.msg_ptr->purp == CAI_SRV_PRO_PURP )
      {
        // Evaluate the configuration, requesting alternate configuration to
        // be generated if necessary, and then process appropriately
        switch ( snm_eval_config( &e_ptr->ep.req_msg.msg_ptr->cfg,
                                  &srv_proposed_cfg, FALSE, TRUE,
                                  mccccl_code_channel_list.get_fwd_chan_mask(),
                                  mccccl_code_channel_list.get_rev_chan_mask()
                                ) )
        {
          case SNM_ACCEPT_CFG : // Accept the configuration
            e_ptr->status =
              srv_send_rsp_msg ( e_ptr->ep.req_msg.msg_ptr->req_seq,
                                 CAI_SRV_ACC_PURP,
                                 NULL );

            /* Set subfunction timer and activate the Waiting for Service
               Connect Message Subfunction */
            (void)mcc_set_callback_timer(&mcc_srv_timer, CAI_T65M, MCC_SRV_TIMER_ID);
            subf = SRV_WF_CON_SUBF;
            break;

          case SNM_ALT_CFG :   // Propose an alternative configuration
            e_ptr->status =
              srv_send_rsp_msg ( e_ptr->ep.req_msg.msg_ptr->req_seq,
                                 CAI_SRV_PRO_PURP,
                                 &srv_proposed_cfg );

            /* Set subfunction timer and activate the Waiting for Service
               Request Message Subfunction */
            (void)mcc_set_callback_timer(&mcc_srv_timer, CAI_T68M, MCC_SRV_TIMER_ID);
            subf = SRV_WF_REQ_SUBF;
            break;

          case SNM_REJECT_CFG : // Reject the configuration
            e_ptr->status =
              srv_send_rsp_msg ( e_ptr->ep.req_msg.msg_ptr->req_seq,
                                 CAI_SRV_REJ_PURP,
                                 NULL );
            (void)mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID );
            subf = SRV_NORM_SUBF;
            break;
        } //lint !e744, default not needed since all values are evaluated
      }

      else
      {
        e_ptr->status = srv_send_rej_ord ( CAI_REJ_FIELD, CAI_FTC_SRV_REQ_MSG );
      }

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_RSP_MSG_EC:

      if ( e_ptr->ep.rsp_msg.msg_ptr->req_seq == srv_saved_req_seq )
      {
        if ( e_ptr->ep.rsp_msg.msg_ptr->purp == CAI_SRV_REJ_PURP )
        {
          (void)mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID );
          subf = SRV_NORM_SUBF;

          e_ptr->status = SRV_DONE_S;
        }

        else if ( e_ptr->ep.rsp_msg.msg_ptr->purp == CAI_SRV_PRO_PURP )
        {
          // Evaluate the configuration, requesting alternate configuration to
          // be generated if necessary, and then process appropriately
          switch ( snm_eval_config( &e_ptr->ep.rsp_msg.msg_ptr->cfg,
                                    &srv_proposed_cfg, FALSE, TRUE,
                                    mccccl_code_channel_list.get_fwd_chan_mask(),
                                    mccccl_code_channel_list.get_rev_chan_mask()
                                  ) )
          {
            case SNM_ACCEPT_CFG : // Accept the configuration
              e_ptr->status = srv_send_req_msg ( CAI_SRV_ACC_PURP, NULL );
              (void)mcc_set_callback_timer(&mcc_srv_timer, CAI_T65M, MCC_SRV_TIMER_ID);
              subf = SRV_WF_CON_SUBF;
              break;

            case SNM_ALT_CFG :   // Propose an alternative configuration
              e_ptr->status = srv_send_req_msg ( CAI_SRV_PRO_PURP,
                                                 &srv_proposed_cfg );
              /* Reset subfunction timer and stay in this subfunction */
              (void)mcc_set_callback_timer(&mcc_srv_timer, CAI_T68M, MCC_SRV_TIMER_ID);
              break;

            case SNM_REJECT_CFG : // Reject the configuration
              e_ptr->status = srv_send_req_msg ( CAI_SRV_REJ_PURP, NULL );
              (void)mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID );
              subf = SRV_NORM_SUBF;
              break;
          } //lint !e744, default not needed since all values are evaluated
        }

        else
        {
          /* illegal purpose code. */
          e_ptr->status = srv_send_rej_ord ( CAI_REJ_FIELD, CAI_FTC_SRV_RSP_MSG );
        }
      }

      else /* Request seq number does not match. Ignore layer 3
              fields of message. */
      {
        e_ptr->status = SRV_DONE_S;
      }

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_CON_MSG_EC:
    case SRV_HO_DIR_EC:

      /* Yes, these two cases fall through, since the Service Connect Message
         and a Handoff Direction Message use the same function for
         a service connection. */

      subf = srv_process_con_msg ( e_ptr );
        /* Perform common processing for service connect or handoff message
           event (e_ptr->status will be set on return) */

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#if defined(FEATURE_IS2000_REL_C)
    case SRV_CLAM_MSG_EC:

      /* Service negotiation in progress, reject CLAM */
      e_ptr->ep.clam_msg.clam_validation_success = FALSE;
      e_ptr->status = SRV_DONE_S;
      break;
#endif /* FEATURE_IS2000_REL_C */

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_CTL_MSG_EC:

      srv_process_ctl_msg ( e_ptr );

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_TC_WFO_EC:
    case SRV_TC_WFA_EC:
    case SRV_TC_CON_EC:
      srv_rpt_subs ( e_ptr->ec );
      e_ptr->status = SRV_DONE_S;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_TC_REL_EC:
      srv_rpt_subs ( e_ptr->ec );

      (void)mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID );
      subf = SRV_NORM_SUBF;
      e_ptr->status = SRV_DONE_S;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_RIF_HHO_EC:
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed SRV_RIF_HHO_EC in Wait for Rsp subf " );
      e_ptr->status = SRV_DONE_S;
      subf = SRV_NORM_SUBF;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Unknown event - ec: %x",
        e_ptr->ec );
      e_ptr->status = SRV_BADPARM_S;
      break;

  } /* switch */ /*lint !e788 */

  return ( subf );

} /* srv_wf_rsp_subf */

/*===========================================================================

FUNCTION SRV_WF_CON_SUBF

DESCRIPTION
      This function is part of the service negotiation state machine. It
      corresponds to the Waiting for Service Connect Message Subfunction.

DEPENDENCIES
  None.

RETURN VALUE
  Value specifying the next subfunction to invoke.

SIDE EFFECTS
  None.
===========================================================================*/

srv_subf_type  srv_wf_con_subf
(
  srv_event_type  *e_ptr
    /* Pointer to event descriptor. */
)
{
  srv_subf_type  subf = SRV_WF_CON_SUBF;
    /* Used for returning the service subfunction to invoke next time.
       Initialized to the current subfunction. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( e_ptr->ec )
  {
    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_SIG_EC:  /* Subfunction timer has expired */

      (void)mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID );  /* Clean up timer and signal */

      subf = SRV_NORM_SUBF;  /* Activate normal service subfunction */

      e_ptr->status = SRV_DONE_S;  /* Set return status */

      /* Event report: Timer T65m expired */
      mclog_report_event_timer(CAI_TIMER_T65M);

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#if defined(FEATURE_IS2000_REL_C)
    case SRV_SCR_CMP_EC:
      /* Action time has arrived */

      M1X_MSG( DCP, LEGACY_MED,
        "SCR AT arrives in SRV_WF_CON");

      /* Time to update configuration
         Do only the non-time critical portion of configuration update
         For IS2000 targets, the time critical portion has already been done earlier. */
      snm_continue_update_config();

      /* Event report: Service negotiation completed */
      event_report(EVENT_SERVICE_NEGOTIATION_COMPLETED);

      e_ptr->status = SRV_DONE_S;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
#endif /* FEATURE_IS2000_REL_C */

    case SRV_UI_EC:  /* Request from user */

      /* User requests not accepted during this subfunction */
      e_ptr->status = SRV_NOTNOW_S;

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_REQ_MSG_EC:

      if ( e_ptr->ep.req_msg.msg_ptr->purp == CAI_SRV_REJ_PURP )
      {
        e_ptr->status = srv_send_rej_ord ( CAI_REJ_STATE, CAI_FTC_SRV_REQ_MSG );
      }

      else if ( e_ptr->ep.req_msg.msg_ptr->purp == CAI_SRV_PRO_PURP )
      {
        // Evaluate the configuration, requesting alternate configuration to
        // be generated if necessary, and then process appropriately
        switch ( snm_eval_config( &e_ptr->ep.req_msg.msg_ptr->cfg,
                                  &srv_proposed_cfg, FALSE, TRUE,
                                   mccccl_code_channel_list.get_fwd_chan_mask(),
                                   mccccl_code_channel_list.get_rev_chan_mask()
                                ) )
        {
          case SNM_ACCEPT_CFG : // Accept the configuration
            e_ptr->status =
              srv_send_rsp_msg ( e_ptr->ep.req_msg.msg_ptr->req_seq,
                                 CAI_SRV_ACC_PURP,
                                 NULL );

            /* Set subfunction timer and activate the Waiting for Service
               Connect Message Subfunction */
            (void)mcc_set_callback_timer(&mcc_srv_timer, CAI_T65M, MCC_SRV_TIMER_ID);
            subf = SRV_WF_CON_SUBF;
            break;

          case SNM_ALT_CFG :   // Propose an alternative configuration
            e_ptr->status =
              srv_send_rsp_msg ( e_ptr->ep.req_msg.msg_ptr->req_seq,
                                 CAI_SRV_PRO_PURP,
                                 &srv_proposed_cfg );

            /* Set subfunction timer and activate the Waiting for Service
               Request Message Subfunction */
            (void)mcc_set_callback_timer(&mcc_srv_timer, CAI_T68M, MCC_SRV_TIMER_ID);
            subf = SRV_WF_REQ_SUBF;
            break;

          case SNM_REJECT_CFG : // Reject the configuration
            e_ptr->status =
              srv_send_rsp_msg ( e_ptr->ep.req_msg.msg_ptr->req_seq,
                                 CAI_SRV_REJ_PURP,
                                 NULL );
            (void)mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID );
            subf = SRV_NORM_SUBF;
            break;
        } //lint !e744, default not needed since all values are evaluated
      }

      else
      {
        e_ptr->status = srv_send_rej_ord ( CAI_REJ_FIELD, CAI_FTC_SRV_REQ_MSG );
      }

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_RSP_MSG_EC:

        /* Service response message not allowed in this subfunction */
      e_ptr->status = srv_send_rej_ord ( CAI_REJ_STATE, CAI_FTC_SRV_RSP_MSG );


      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_CON_MSG_EC:
    case SRV_HO_DIR_EC:

      /* Yes, these two cases fall through, since the Service Connect Message
         and a Handoff Direction Message use the same function for
         a service connection. */

      subf = srv_process_con_msg ( e_ptr );
        /* Perform common processing for service connect or handoff message
           event (e_ptr->status will be set on return) */

      /* subf is set to SRV_NORM_SUBF after the con msg is rejected.
         we can repropose at this point since either the previous service neg
         was done incorrectly or the BS didn't start service neg before the
         service connect msg. This procedure is not required by the Standard,
         but a optimization to shorten connect time and prevent dead lock in
         case some BS doesn't know why the con msg was rejected and waits for
         the mobile to send cmd */
      if ((subf == SRV_NORM_SUBF) && cdma.call_orig)
      {
        // we only support one SO at this time
        if (snm_so_is_enabled (e_ptr->ep.con_msg.msg_ptr->cfg.con[0].so) &&
            (snm_first_orig_config(e_ptr->ep.con_msg.msg_ptr->cfg.con[0].so) == SNM_SUCCESS))
        {
          e_ptr->status = srv_send_req_msg ( CAI_SRV_PRO_PURP,
                                             &srv_proposed_cfg );
          (void)mcc_set_callback_timer(&mcc_srv_timer, CAI_T68M, MCC_SRV_TIMER_ID);
          subf = SRV_WF_RSP_SUBF;
        }
      }

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#if defined(FEATURE_IS2000_REL_C)
    case SRV_CLAM_MSG_EC:

      /* Service negotiation in progress, reject CLAM */
      e_ptr->ep.clam_msg.clam_validation_success = FALSE;
      e_ptr->status = SRV_DONE_S;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
#endif /* FEATURE_IS2000_REL_C */

    case SRV_CTL_MSG_EC:

      srv_process_ctl_msg ( e_ptr );
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_TC_WFO_EC:
    case SRV_TC_WFA_EC:
    case SRV_TC_CON_EC:
      srv_rpt_subs ( e_ptr->ec );
      e_ptr->status = SRV_DONE_S;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_TC_REL_EC:
      srv_rpt_subs ( e_ptr->ec );

      (void)mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID );
      subf = SRV_NORM_SUBF;
      e_ptr->status = SRV_DONE_S;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_RIF_HHO_EC:
      e_ptr->status = SRV_DONE_S;
      subf = SRV_NORM_SUBF;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Unknown event - ec: %x",
        e_ptr->ec );
      e_ptr->status = SRV_BADPARM_S;
      break;

  } /* switch */ /*lint !e788 */

  return ( subf );

} /* srv_wf_con_subf */

/*===========================================================================

FUNCTION SRV_WF_TIM_SUBF

DESCRIPTION
      This function is part of the service negotiation state machine. It
      corresponds to the Waiting for Service Action Time Subfunction.

DEPENDENCIES
  None.

RETURN VALUE
  Value specifying the next subfunction to invoke.

SIDE EFFECTS
  None.
===========================================================================*/

srv_subf_type  srv_wf_tim_subf
(
  srv_event_type  *e_ptr
    /* Pointer to event block describing event. */
)
{
  srv_subf_type  subf = SRV_WF_TIM_SUBF;
    /* Used for returning the service subfunction to invoke next time.
       Initialized to the current subfunction. */
  int i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( e_ptr->ec )
  {
    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_SIG_EC:

      /* Shouldn't happen, but harmless. */

      /* Clear timer and associated signal */
      (void)mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID );

      e_ptr->status = SRV_DONE_S;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_SCR_CMP_EC:
      /* Action time has arrived */

      /* Time to update configuration
         Do only the non-time critical portion of configuration update
         For IS2000 targets, the time critical portion has already been done earlier. */
      snm_continue_update_config();

      if (srv_pending_cfg.cfg_msg_type == CAI_SRV_CON_MSG)
      {
        e_ptr->status = srv_send_cmp_msg ( srv_saved_con_seq );

        /* If cfg_msg_type is not a Service Connect Message (i.e., it is a
           handoff message), we send a handoff completion message instead
           of a service connect completion message.  This is handled
           elsewhere as part of handoff processing. */
      }
      else
      {
        /* This is for HDM, Make sure Markov/loopback services if any are
           properly initialized */
        /* For SCM this is done when the trans is sent */
        for (i=0; i < srv_current_cfg.neg_scr.num_con ; i++)
        {
          tc_inform_scr_change_init_service(srv_current_cfg.neg_scr.con[i].so);
        }
      }

      /* Event report: Service negotiation completed */
      event_report(EVENT_SERVICE_NEGOTIATION_COMPLETED);

      subf = SRV_NORM_SUBF;  /* Return to normal subfunction */

      e_ptr->status = SRV_DONE_S;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_UI_EC:

      /* User requests not accepted during this subfunction */
      e_ptr->status = SRV_NOTNOW_S;

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_REQ_MSG_EC:

      e_ptr->status = srv_send_rej_ord ( CAI_REJ_STATE, CAI_FTC_SRV_REQ_MSG );

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_RSP_MSG_EC:

      e_ptr->status = srv_send_rej_ord ( CAI_REJ_STATE, CAI_FTC_SRV_RSP_MSG );

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_CON_MSG_EC:

      e_ptr->status = srv_send_rej_ord ( CAI_REJ_STATE, CAI_SRV_CON_MSG );

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

#if defined(FEATURE_IS2000_REL_C)
    case SRV_CLAM_MSG_EC:

      /* Service negotiation in progress, reject CLAM */
      e_ptr->ep.clam_msg.clam_validation_success = FALSE;
      e_ptr->status = SRV_DONE_S;
      break;
#endif /* FEATURE_IS2000_REL_C */

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_CTL_MSG_EC:

      srv_process_ctl_msg ( e_ptr );

      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_HO_DIR_EC:

      /* Use msg_type instead of a CAI_... value, so this will work for
         future handoff messages (like the UHDM). */
      /* New SCR along with HDM is allowed in this sub function */
      subf = srv_process_con_msg ( e_ptr );
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_TC_WFO_EC:
    case SRV_TC_WFA_EC:
    case SRV_TC_CON_EC:
      srv_rpt_subs ( e_ptr->ec );
      e_ptr->status = SRV_DONE_S;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_TC_REL_EC:
      srv_rpt_subs ( e_ptr->ec );

      (void)mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID );
      subf = SRV_NORM_SUBF;
      e_ptr->status = SRV_DONE_S;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    case SRV_RIF_HHO_EC:
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed SRV_RIF_HHO_EC. Going back to normal subf " );
      e_ptr->status = SRV_DONE_S;
      subf = SRV_NORM_SUBF;
      break;

    /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Unknown event - ec: %x",
        e_ptr->ec );
      e_ptr->status = SRV_BADPARM_S;
      break;

  } /* switch */ /*lint !e788 */

  return ( subf );

} /* srv_wf_tim_subf */

/*===========================================================================

FUNCTION SRV_RPT_EVENT

DESCRIPTION
      This function is called to notify the Service Configuration &
      Negotiation Services of pertinent events.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/

void  srv_rpt_event
(
  srv_event_type  *e_ptr
    /* Pointer to descriptor block for reported event */
)
{
  M1X_MSG( DCP, LEGACY_MED,
    "Event ( in) - subf: %x  ec: %x",
    srv_subf,
    e_ptr->ec );

  switch ( srv_subf )  /* Invoke current service subfunction */
  {
    case SRV_NORM_SUBF:
      srv_subf = srv_norm_subf( e_ptr );
      break;

    case SRV_WF_RSP_SUBF:
      srv_subf = srv_wf_rsp_subf( e_ptr );
      break;

    case SRV_WF_REQ_SUBF:
      srv_subf = srv_wf_req_subf( e_ptr );
      break;

    case SRV_WF_CON_SUBF:
      srv_subf = srv_wf_con_subf( e_ptr );
      break;

    case SRV_WF_TIM_SUBF:
      srv_subf = srv_wf_tim_subf( e_ptr );
      break;

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Unknown subf - subf: %x",
        srv_subf );
      break;

  } /* switch */


  M1X_MSG( DCP, LEGACY_MED,
    "Event (out) - subf: %x  status: %x",
    srv_subf,
    e_ptr->status );

}  /* srv_rpt_event */

/*===========================================================================

FUNCTION SRV_GET_CUR_CFG

DESCRIPTION
  This function returns the current service configuration.

DEPENDENCIES
  Only valid during Traffic Channel Operation.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void  srv_get_cur_cfg
(
  caii_srv_cfg_type  *cfg_ptr
    /* Pointer to record in which to return the current service
       configuration. */
)
{
  *cfg_ptr = srv_current_cfg.neg_scr;

} /* srv_get_cur_cfg */

/*===========================================================================

FUNCTION SRV_GET_CUR_NON_NEG_CFG

DESCRIPTION
  This function returns the current non-neg service configuration.

DEPENDENCIES
  Only valid during Traffic Channel Operation.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void  srv_get_cur_non_neg_cfg
(
  caii_non_neg_srv_cfg_type  *cfg_ptr
    /* Pointer to record in which to return the current service
       configuration. */
)
{
  *cfg_ptr = srv_current_cfg.non_neg_scr;

} /* srv_get_cur_non_neg_cfg */

/*===========================================================================

FUNCTION SRV_GET_PENDING_CFG

DESCRIPTION
  This function returns the current service configuration.

DEPENDENCIES
  Only valid during Traffic Channel Operation.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

const caii_complete_srv_cfg_type * srv_get_pending_cfg
(
  void
)
{
  return &srv_pending_cfg.cfg;

} /* srv_get_pending_cfg */

/*===========================================================================

FUNCTION srv_get_pref_fundicated_RCs

DESCRIPTION
  This function returns the preferred RC for the forward and reverse
  fundicated channel. This preference is sent back in the page resp or the
  origination message.
  This function is called by SNM if the SO is not found there.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void srv_get_pref_fundicated_RCs
(
  word so, /* The SO for which pref RCs are needed */
  cai_radio_config_type * fwd_rc_ptr, /* Storage for returned fwd RC */
  cai_radio_config_type * rev_rc_ptr  /* Storage for return  rev RC */
)
{
  /* Do not call this function directly, call it through the
     corresponding function in SNM. This will be obsolete once all SOs
     migrate to SNM
     Comes here only if SO is not captured in SNM */

  /* Initialize variables so lint does not complain */
  cai_radio_config_type user_pref_fundicated_rc = CAI_RC_1;
  boolean user_pref_rc_valid = FALSE;
  word srv_opt;

  if (P_REV_IS_6_OR_GREATER)
  {
    user_pref_rc_valid = snm_get_user_pref_fundicated_rc(&user_pref_fundicated_rc);
  }

  srv_opt = so;

  #ifdef FEATURE_1X_CUST_VOICE_SO
  /* This workaround has been added to handle NV dependent non-constant
   * SO while ensuring the existing switch construct is not disrupted */        
  if (srv_opt == mcc_get_cust_so() && so != CAI_SO_NULL)
  {
    srv_opt = CAI_GEN_EVRC_CUST_SO;
  }   
  #endif /* FEATURE_1X_CUST_VOICE_SO */

  switch ( srv_opt)
  {
    case CAI_SO_VOICE_IS96A:
    case CAI_SO_LOOPBACK:
    case CAI_SO_SMS:
    case CAI_SO_MARKOV:
    case CAI_SO_RS1_MARKOV:
    case CAI_SO_VOICE_EVRC:
    #ifdef FEATURE_1X_CUST_VOICE_SO
    case CAI_GEN_EVRC_CUST_SO:
    #endif /* FEATURE_1X_CUST_VOICE_SO */
    case CAI_SO_VOICE_4GV_NB:
    case CAI_SO_VOICE_4GV_WB:
    case CAI_SO_VOICE_EVRC_NW:
    case CAI_SO_RS1_OTAPA:
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
    case CAI_SO_CSFB_TUNNELED_SMS:
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */

      /* All of these service options use Multiplex Option 1 and
         Rate Set 1. */
      if (P_REV_IS_6_OR_GREATER)
      {
        if (user_pref_rc_valid)
        {
          #ifdef FEATURE_IS2000_1X_ADV
          if ((so == CAI_SO_VOICE_EVRC_NW) &&
              (user_pref_fundicated_rc == CAI_RC_11))
          {
            *fwd_rc_ptr = user_pref_fundicated_rc;
            *rev_rc_ptr = CAI_RC_8;
          }
          else
          #endif /* FEATURE_IS2000_1X_ADV */
          if ((user_pref_fundicated_rc == CAI_RC_3) ||
              (user_pref_fundicated_rc == CAI_RC_4) ||
              (user_pref_fundicated_rc == CAI_RC_1))
          {
            *fwd_rc_ptr = user_pref_fundicated_rc;
            if (RC_CLASS(*fwd_rc_ptr) == 1)
            {
              *rev_rc_ptr = CAI_RC_1;
            }
            else
            {
              *rev_rc_ptr = CAI_RC_3;
            }
          }
          else
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Ignoring NV Pref RC %d",
              user_pref_fundicated_rc);
            *fwd_rc_ptr = CAI_RC_3;
            *rev_rc_ptr = CAI_RC_3;
          }
        }
        else
        {
          *fwd_rc_ptr = CAI_RC_3;
          *rev_rc_ptr = CAI_RC_3;
        }
      }
      else
      {
        *fwd_rc_ptr = CAI_RC_1;
        *rev_rc_ptr = CAI_RC_1;
      }
      break;

    case CAI_SO_LOOPBACK_13K:
    case CAI_SO_VOICE_13K_IS733:
    case CAI_SO_VOICE_13K:
    case CAI_SO_MARKOV_13K:
    case CAI_SO_RS2_MARKOV:
    case CAI_SO_RS2_SMS:

    case CAI_SO_RS2_OTAPA:


      /* All of these service options use Multiplex Option 2 and
         Rate Set 2. */

      if (P_REV_IS_6_OR_GREATER)
      {
        if (user_pref_rc_valid)
        {
          if ((user_pref_fundicated_rc == CAI_RC_2) ||
              (user_pref_fundicated_rc == CAI_RC_5))
          {
            *fwd_rc_ptr = user_pref_fundicated_rc;
            if (RC_CLASS(*fwd_rc_ptr) == 1)
            {
              *rev_rc_ptr = CAI_RC_2;
            }
            else
            {
              *rev_rc_ptr = CAI_RC_4;
            }
          }
          else
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Ignoring NV Pref %d",
              user_pref_fundicated_rc);
            *fwd_rc_ptr = CAI_RC_5;
            *rev_rc_ptr = CAI_RC_4;
          }
        }
        else
        {
          *fwd_rc_ptr = CAI_RC_5;
          *rev_rc_ptr = CAI_RC_4;
        }
      }
      else
      {
        *fwd_rc_ptr = CAI_RC_2;
        *rev_rc_ptr = CAI_RC_2;
      }
      break;

    default:
      /* In this case, the service option is not supported, just return some defaults */
      /* This error check must be done before getting here */
      *fwd_rc_ptr = CAI_RC_1;
      *rev_rc_ptr = CAI_RC_1;
      break;

  } /* switch */

} /* srv_get_pref_fundicated_RCs */

/*===========================================================================

FUNCTION SRV_INIT

DESCRIPTION
  This function intializes the Service Configuration and Negotiation
  Services. It should be called when the mobile station successfully
  completes the processing of the Traffic Channel Init substate.

DEPENDENCIES
  Assumes that srv_timer (a rex timer) has been defined and initialized.

RETURN VALUE
  SRV_DONE_S    - Initialization successful.
  SRV_BADPARM_S - Invalid initialization parameter(s) specified.
  SRV_NOTSUP_S  - Unsupported service option specified.

SIDE EFFECTS

===========================================================================*/

srv_status_type srv_init
(
  boolean mob_orig,
    /* Specifies whether the call is mobile originated (TRUE) or
       mobile terminated (FALSE). */

  word so,
    /* Specifies which service option number was included in either the
       origination message or the page response message. */

  mccdma_chnasn_type *chn_assign
    /* Details about the channel assignment */
)
{
  srv_status_type  status = SRV_DONE_S;
    /* For returning status to caller. */

  word tmp_so = so;

  snm_mux_config_type tmp_mux = SNM_DEFAULT_MUX;

  byte sr_id_list[1]; /* For use in call to initialize def LPM */

  mcc_fwd_chan_mask_type fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask();
  mcc_rev_chan_mask_type rev_chan_mask = mccccl_code_channel_list.get_rev_chan_mask();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure specified service option is supported */
  if (!snm_so_is_enabled(so))
  {
    /* Unsupported service option specified */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Unsupported or non-functioning so - so: %x",
      so );
    return SRV_NOTSUP_S;
  }

  if (((chn_assign->msg_type == CAI_CHN_ASN_MSG) &&
       (chn_assign->assign_mode == CAI_EXT_TRAF_CHAN_ASSIGN)) ||
      ((chn_assign->msg_type == CAI_EXT_CHN_ASN_MSG) &&
       ((chn_assign->assign_mode == CAI_TRAFFIC_CHAN_ASSIGN) ||
        (chn_assign->assign_mode == CAI_EXT_TRAF_CHAN_ASSIGN)
      ))

      || (chn_assign->msg_type == CAI_MEID_EXT_CHN_ASN_MSG)

     )
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Init - mob_orig: %x so: %x gm: %x",
      mob_orig,
      so,
      chn_assign->mode.am0.granted_mode);

    /* Save so from origination message or page response message. */
    srv_saved_so = so;

    /* Initialize all remaining variables, timers, etc. */

    srv_req_seq = srv_saved_req_seq = 0;

    srv_saved_con_seq = 0;

    /* Make sure subfunction timer is not running */
    (void) mcc_clr_callback_timer(&mcc_srv_timer, MCC_SRV_TIMER_ID);

#if defined(FEATURE_IS2000_REL_C)
    if (P_REV_IS_9_OR_GREATER &&
        (chn_assign->msg_type == CAI_EXT_CHN_ASN_MSG) &&
        ((chn_assign->assign_mode == CAI_EXT_TRAF_CHAN_ASSIGN) ||
         (chn_assign->assign_mode == CAI_PACKET_DATA_TRAF_CHAN_ASSIGN)) &&
        (chn_assign->mode.am0.granted_mode == CAI_GRANT_SRID_RESTORE))
    {
      srv_neg_needed = FALSE;

      /* Populate service configuration to be restored */
      snm_populate_restore_config();

      srv_pending_cfg.cfg_msg_type = chn_assign->msg_type;
      srv_current_cfg = srv_pending_cfg.cfg;

      /* Update active Sync ID info */
      mccsyncid_update_restore_info();
      mccsyncid_update_active_sync_id();

      /* Activate normal service subfunction */
      srv_subf = SRV_NORM_SUBF;
    }
    else
#endif /* FEATURE_IS2000_REL_C */

    {
      switch (chn_assign->mode.am0.granted_mode)
      {
        case CAI_GRANT_USE_DC:  /* Use DEFAULT_CONFIG field */

          /* With this grant mode, the mobile station will need to send
             a service request message if the call is mobile originated */
          srv_neg_needed = mob_orig;

          /* Activate normal service subfunction */
          srv_subf = SRV_NORM_SUBF;

          if (chn_assign->mode.am0.default_config == CAI_DEF_CONFIG_FOR1_REV1)
          {
            /* Establish default configuration using MUX option 1 and Rate Set 1. */
            tmp_so = CAI_SO_VOICE_IS96A;
            tmp_mux = SNM_DEFAULT_MUX;
          }
          else if (chn_assign->mode.am0.default_config == CAI_DEF_CONFIG_FOR2_REV2 )
          {
            /* Establish default configuration using MUX option 2 and Rate Set 2. */
            tmp_so = CAI_SO_VOICE_13K;
            tmp_mux = SNM_DEFAULT_MUX;
          }
          /* Following new value for default_config is introduced for P_REV 6 */
          else if (P_REV_IS_6_OR_GREATER &&
                   (chn_assign->mode.am0.default_config == CAI_DEF_CONFIG_USE_ASSGN_RC))
          {
            /* We need to generate a default configuration based on the RC
               specified in the ECAM. To do that, we need to pick a SO with
               default rate set compatible with the RC. */
            tmp_so = (RATE_SET_OF_FWD_RC(chn_assign->mode.am0.for_rc ) == 0 ) ? /*lint !e641 */
                     CAI_SO_VOICE_IS96A : CAI_SO_VOICE_13K;
            tmp_mux = SNM_DEFAULT_MUX;
          }
          else
          {
            /* the HW doesn't support mixed forward and reverse rate set at
             * this moment */
            /* Unsupported service option specified */
            M1X_MSG( DCP, LEGACY_ERROR,
              "Unsupported so and default_config - so: %x, dc: %x",
              so,
              chn_assign->mode.am0.default_config);
            status = SRV_NOTSUP_S;
          }

          break;

        /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
        case CAI_GRANT_DEF:   /* Use default MUX and Rate Set for SO */

          /* With this grant mode, the mobile station will need to send
             a service request message if the call is mobile originated */
          srv_neg_needed = mob_orig;

          /* Establish default configuration based on service option. */
          tmp_so = so;

          tmp_mux = SNM_DEFAULT_MUX;

          /* Activate normal service subfunction */
          srv_subf = SRV_NORM_SUBF;

          break;

        /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
        case CAI_GRANT_STR:  /* Streamlined mode - no negotiation needed */

          srv_neg_needed = FALSE;

          /* Establish default configuration based on service option. */
          tmp_so = so;

          tmp_mux = SNM_DEFAULT_MUX;

          /* Set subfunction timer for the waiting for service connect
             message subfunction and activate that subfunction. */
          (void)mcc_set_callback_timer(&mcc_srv_timer, CAI_T65M, MCC_SRV_TIMER_ID);
          srv_subf = SRV_WF_CON_SUBF;

          break;

        /*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unknown grant mode - gm: %x",
            chn_assign->mode.am0.granted_mode  );
          status = SRV_BADPARM_S;

          break;

      } /* end switch */

      if (status == SRV_DONE_S)
      {
        boolean rc_class_2;

        /* Establish default config. Note that configuration does not include
           any service option connections. Note that for IS2000, RC part of the
           config is overwritten below with correct values */
        (void) snm_gen_config(tmp_so, &srv_current_cfg.neg_scr, FALSE,
                 tmp_mux, 0, fwd_chan_mask, rev_chan_mask);

        /* Store the values of current RC in the current config */
        /* Have to deal with Fwd and Rev separately for Rev D */
        if ((fwd_chan_mask & MCC_F_FCH) || (rev_chan_mask & MCC_R_FCH))
        {
          srv_current_cfg.neg_scr.fch_cc_incl = TRUE;
          srv_current_cfg.neg_scr.fch_frame_size = 0;
          srv_current_cfg.neg_scr.for_fch_rc = chn_assign->mode.am0.for_rc;  /*lint !e641 */
          srv_current_cfg.neg_scr.rev_fch_rc = chn_assign->mode.am0.rev_rc;  /*lint !e641 */
        }

        if ((fwd_chan_mask & MCC_F_DCCH) || (rev_chan_mask & MCC_R_DCCH))
        {
          srv_current_cfg.neg_scr.dcch_cc_incl = TRUE;
          /*lint -e641 */
          srv_current_cfg.neg_scr.dcch_frame_size = CAI_DCCH_20MS_FRAME;
          srv_current_cfg.neg_scr.for_dcch_rc = chn_assign->mode.am0.for_rc;
          srv_current_cfg.neg_scr.rev_dcch_rc = chn_assign->mode.am0.rev_rc;
          /*lint +e641 */
        }

        /* Adjust mux to be in sync with RC. Let's not do that for SOs using
           SCCH, those must already be correct at this point */
        if ((fwd_chan_mask & MCC_F_FCH) || (fwd_chan_mask & MCC_F_DCCH))
        {
          if (srv_current_cfg.neg_scr.fwd_mux <= CAI_MUX2)
          {
            if (fwd_chan_mask & MCC_F_FCH)
            {
              srv_current_cfg.neg_scr.fwd_mux = RATE_SET_TO_MUX(
                RATE_SET_OF_FWD_RC(srv_current_cfg.neg_scr.for_fch_rc));  /*lint !e641 */
            }
            else
            {
              srv_current_cfg.neg_scr.fwd_mux = RATE_SET_TO_MUX(
                RATE_SET_OF_FWD_RC(srv_current_cfg.neg_scr.for_dcch_rc));  /*lint !e641 */
            }

            if ((rev_chan_mask & MCC_R_FCH) || (rev_chan_mask & MCC_R_DCCH))
            {
              srv_current_cfg.neg_scr.rev_mux = srv_current_cfg.neg_scr.fwd_mux;
            }
          }
        }
        else if ((rev_chan_mask & MCC_R_FCH) || (rev_chan_mask & MCC_R_DCCH))
        {
          if (srv_current_cfg.neg_scr.rev_mux <= CAI_MUX2)
          {
            if (rev_chan_mask & MCC_R_FCH)
            {
              srv_current_cfg.neg_scr.rev_mux = RATE_SET_TO_MUX(
                RATE_SET_OF_REV_RC(srv_current_cfg.neg_scr.rev_fch_rc));  /*lint !e641 */
            }
            else
            {
              srv_current_cfg.neg_scr.rev_mux = RATE_SET_TO_MUX(
                RATE_SET_OF_REV_RC(srv_current_cfg.neg_scr.rev_dcch_rc));  /*lint !e641 */
            }
          }
        }

        // revisit  Reset Non Nego portion of the pending config
        //This is important because when we send params to the lower
        // layers these fields are accessed and if we are in old P_REV mode
        // or this BS decided to do just SO negotiation we need some good
        // defaults here
        snm_is2000_nnscr_init(&srv_pending_cfg.cfg.non_neg_scr);

        /* Set good defaults for non_neg current cfg also,
           In case these values never arrive in SCM, we need correct
           values here in case we need to back up cfg from this after a
           return from HHO */
        snm_is2000_nnscr_init(&srv_current_cfg.non_neg_scr);

        if (P_REV_IS_6_OR_GREATER &&
            ((chn_assign->msg_type == CAI_EXT_CHN_ASN_MSG)

             || (chn_assign->msg_type == CAI_MEID_EXT_CHN_ASN_MSG)

            ))
        {
          srv_current_cfg.non_neg_scr.fpc_incl = TRUE;


            /* Default FPC mode if forward fundicated channel(s) is present */
#ifdef FEATURE_IS2000_1X_ADV
            /* fpc_mode is received or initialized in RCP data while processing ECAM.
               So initialize it from RCP data. Check if fpc_mode is not initialized, just
               for worst-case error scenario. */
            if (mcc_rcp_data.rx_tx_params.fpc_mode.incl)
            {
              srv_current_cfg.non_neg_scr.fpc_mode = mcc_rcp_data.rx_tx_params.fpc_mode.value;
            }
            else
#endif /* FEATURE_IS2000_1X_ADV */
            {
              srv_current_cfg.non_neg_scr.fpc_mode = CAI_FPC_MODE_800_PRI; /*lint !e641 */
            }

          M1X_MSG( DCP, LEGACY_MED,
            "FPC mode value in the current config is defaulted to = %d.",
            srv_current_cfg.non_neg_scr.fpc_mode);

          /* FPC_PRI_CHAN is always set properly by ECAM or default initialization */
          srv_current_cfg.non_neg_scr.fpc_pri_chan = chn_assign->mode.am0.fpc_pri_chan;

          if (fwd_chan_mask & MCC_F_FCH)
          {
            srv_current_cfg.non_neg_scr.fpc_olpc_fch_incl = TRUE;
            srv_current_cfg.non_neg_scr.fpc_fch_fer =
              chn_assign->mode.am0.fpc_fch_fer;
            srv_current_cfg.non_neg_scr.fpc_fch_min_setpt =
              chn_assign->mode.am0.fpc_fch_min_setpt;
            srv_current_cfg.non_neg_scr.fpc_fch_max_setpt =
              chn_assign->mode.am0.fpc_fch_max_setpt;
          }

          if (fwd_chan_mask & MCC_F_DCCH)
          {
            srv_current_cfg.non_neg_scr.fpc_olpc_dcch_incl = TRUE;
            srv_current_cfg.non_neg_scr.fpc_dcch_fer =
              chn_assign->mode.am0.fpc_dcch_fer;
            srv_current_cfg.non_neg_scr.fpc_dcch_min_setpt =
              chn_assign->mode.am0.fpc_dcch_min_setpt;
            srv_current_cfg.non_neg_scr.fpc_dcch_max_setpt =
              chn_assign->mode.am0.fpc_dcch_max_setpt;
          }
        }

        /* Initialize LPM in the current configuration to meet the
           possibility that a BS may send a lpm_ind of '02' (use previous lpm table)
           in the very first service configuration */

        #ifdef FEATURE_IS2000_REL_A
        if (P_REV_IS_7_OR_GREATER) //lint !e506 !e774
        {
          /* The standard asks to use an sr_id value of 'NULL', set it to 0xff
             which is an invalid sr_id */
          sr_id_list[0] = 0xff;
        }
        else
        #endif /* FEATURE_IS2000_REL_A */
        {
          sr_id_list[0] = 1;
        }

        /* Figure out the RC class for use in generating default LPM */
          rc_class_2 = (RC_CLASS(chn_assign->mode.am0.for_rc) == 2) ? TRUE : FALSE;

        snm_is2000_nnscr_lpm_default(&srv_pending_cfg.cfg.non_neg_scr, 1, sr_id_list,
          fwd_chan_mask, rev_chan_mask, rc_class_2);

        snm_is2000_nnscr_lpm_default(&srv_current_cfg.non_neg_scr, 1, sr_id_list,
          fwd_chan_mask, rev_chan_mask, rc_class_2);

#ifdef FEATURE_IS2000_REL_C
        if ((fwd_chan_mask & MCC_F_PDCH) &&
            (chn_assign->mode.am0.for_pdch_parms_incl))
        {
          srv_current_cfg.non_neg_scr.switching_params_incl = TRUE;
          if (chn_assign->mode.am0.chm_switching_params_incl)
          {
            srv_current_cfg.non_neg_scr.num_soft_switching_frames_chm =
              chn_assign->mode.am0.num_soft_switching_frames_chm;
            srv_current_cfg.non_neg_scr.num_softer_switching_frames_chm =
              chn_assign->mode.am0.num_softer_switching_frames_chm;
          }
          else
          {
            /* Standards sections 2.6.3.3 and 2.6.3.5 say we are to use the
               non-Control Hold values if chm_switching_parms_incl is 0. */
            srv_current_cfg.non_neg_scr.num_soft_switching_frames_chm =
              chn_assign->mode.am0.num_soft_switching_frames;
            srv_current_cfg.non_neg_scr.num_softer_switching_frames_chm =
              chn_assign->mode.am0.num_softer_switching_frames;
          }
        }
#endif /* FEATURE_IS2000_REL_C */

      } /* end if (status == SRV_DONE_S) */
    }
  }
  else
  {
    /* SO negotiation, default nnscr fields */
    snm_is2000_nnscr_init(&srv_pending_cfg.cfg.non_neg_scr);
    snm_is2000_nnscr_init(&srv_current_cfg.non_neg_scr);

    /* Initialize LPM */
    sr_id_list[0] = 1;

    snm_is2000_nnscr_lpm_default(&srv_pending_cfg.cfg.non_neg_scr, 1, sr_id_list,
      fwd_chan_mask, rev_chan_mask, FALSE);

    snm_is2000_nnscr_lpm_default(&srv_current_cfg.non_neg_scr, 1, sr_id_list,
      fwd_chan_mask, rev_chan_mask, FALSE);
  }

  return status;

} /* srv_init */

/*===========================================================================

FUNCTION SRV_IS_IN_NORM_SUBF

DESCRIPTION
  This function returns TRUE if SRV is in the normal subfunction; FALSE otherwise.

DEPENDENCIES

RETURN VALUE
  As described above.

SIDE EFFECTS

===========================================================================*/

boolean srv_is_in_norm_subf
(
  void
)
{

  return ( (srv_subf == SRV_NORM_SUBF)? TRUE : FALSE );

} /* srv_is_in_norm_subf */
/*lint +e818 +e655 */
