/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          S E R V I C E    N E G O T I A T I O N   M A N A G E R
         S U P P O R T   F O R   M E D I U M   D A T A   R A T E

GENERAL DESCRIPTION
  This module contains supporting functions related to service negotiation
  manager (SNM).  These functions are created for support of MDR Release 1.0 and
  it is taken into consideration that a service options will only be gradually
  transitioned into SNM.  Initially, only MDR test and DS service options will
  be managed by SNM.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 1999 - 2015 Qualcomm Technologies, Inc. 
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/snm.c_v   1.50   04 Oct 2002 15:29:26   varavamu  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/snm.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/16   sjo     Reject UHDM when the msg is rxed with a voice SO while 
                   a data call is in progress.
03/08/16   ppr     Logging proper sr_id
03/19/15   agh     Changes to support Voice Adapter based Audio architecture
04/18/14   agh     Fixed Klocwork errors
09/27/13   ppr     Reject ESCAM if Service Connect Msg not received.
08/05/13   gga     Feature cleanup
07/18/13   bb      Changes to reject BS proposed DATA SO if origination
                   is on signalling SO
07/08/13   gga     Fixed compilation errors when masking build-time
                   messages off
06/25/13   gga     Fixed Klocwork errors
12/28/12   gga     Klocwork Reviews
12/27/12   ppr     Changes to reject BS proposed DATA SO if
                   Origination is with VOICE SO
08/10/12   ppr     Added snm_update_pending_with_current_config() API
05/24/12   ppr     Feature Cleanup
05/09/12   ppr     Feature Cleanup
03/12/12   jj      Corrected logging format as accepted by diag.
02/21/12   ppr     Feature Cleanup
02/04/12   ppr     Feature Cleanup
01/27/12   ppr     Feature Cleanup: Mainlined Always ON features
01/19/12   ppr     Reverting feature cleanup changes for
                   FEATURE_IS2000_REL_A
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
07/12/11   ab      KW Fixes
06/14/11   ag      Changes to keep FPC_MODE of RCPM and NNSCR in sync.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
04/19/11   jj      CSFB: Code merge from rel/1H11 to main/latest.
03/29/11   ssh     Fixed a compiler issue.
02/18/11   jtm     Removed unneeded DS header includes.
02/14/11   ag      Used REX_ISR_LOCK and REX_ISR_UNLOCK for protecting shared
                   data from simultaneous access by multiple tasks/ISRs.
01/27/11   azh     Lint and KW fixes.
02/08/11   ag      Support for FPC_MODE field of RCPM or RCP record of GEM.
01/04/11   ag      Mainlined FEATURE_MVS_MIGRATE.
12/28/10   ag      Decoupling of SO73 from 1xAdv.
11/15/10   jtm     Initial SU API split for CP.
09/15/10   azh     Fixed KlocWork warnings.
09/14/10   ag      Featurized voice support under FEATURE_MODEM_1X_VOICE_SUPPORT.
07/15/10   ssh     Fixed Compiler Warnings.
07/12/10   jj      Replaced CP's QDSP6 feature with an umbrella feature.
06/10/10   jtm     Eliminated dependency on log_dmss.h by moving CP
                   specific log types within the CP module.
05/26/10   ag      Replaced PACKED with PACK() to support QDSP6.
05/19/10   ss      Added support to remove the redundant mvs_acquire() calls.
04/23/10   ag      Support for 1x Modem statistical report.
03/29/10   ag      Added support for RCs 8/11 for supplemental channel.
02/01/10   vks     Remove 9K revisit featurization around mvs calls.
01/21/10   ag      Added check to make sure RCs (11,8) are sent together as
                   preferred RCs in origination and page response messages.
01/09/10   vks     Temporarily featurize out mvs release calls for MDM9K.
10/15/09   ssh     Added null pointer check as part of KW error fix.
09/22/09   ss      Updated DS header include.
07/14/09   jj      CMI changes corresponding to making CAI_AC_ALT_SO_MAX
                   have a single value.
23/06/09   jj      Modified the check for fields of Service Connect Message to
                   evalute the SCH coding.
06/03/09   jj      Updaed few CMI Phase2 change
05/25/09   pk      Added the check to see if an SO is enabled in case of
                   evaluating a SO.
05/03/09   jj      CMI Phase-II update.
04/29/09   jj      Replaced assert.h with amssassert.h
04/17/09   jtm     Added includes to fix compiler warnings.
03/03/09   jj      Featurized Q6 changes.
12/10/08   jj      QDSP6 migration porting effort, as a part of which replaced
                   INTLOCK and INTFREE with REX_ENABLE_PREMPTION and
                   REX_DISABLE_PREMPTION.
06/11/08   ag      Fixed more Klocwork errors.
06/11/08   ag      Fixed lint critical errors.
05/15/08   bb      Changes for "MVS API Migration" feature
04/08/08   pg      Featurization and general code cleanup for memory reduction
                   effort.
01/01/08   bb      Fixing Klocwork errors
10/01/07   an      Reject secondary traffic when P_REV < 7
05/11/07   pg      More Lint Cleanup.
03/19/07   pg      Lint Cleanup.
03/15/07   an      Changed reference_buf pointer to an array
03/08/07   an      Fixed Critical Lint Errors
02/02/07   sb      Added critical section to snm_add_so.
11/21/06   sb      Added support for VOIP-1X handoffs.
09/29/06   fh      AKA/AES: removed the AES testing code, and eliminated the
                            AES testing featurization of
                            FEATURE_IS2000_REL_A_AES_BS_SUPPORTS
09/02/06   fh      AKA/AES: Miscellaneous code review changes
07/26/06   fh      AKA/AES: added the code to reject the service configuration
                   if UI_ENCRYPT_MODE is invalid in the record.
07/25/06   fh      AKA/AES: put part of lab testing that is dependent on BS AES
                            support under a temporary feature of
                            FEATURE_IS2000_REL_A_AES_BS_SUPPORTS
07/24/06   fh      AKA/AES: cleaned up process of service negotiation of
                   ui_encrypt_mode fields in service configuration records.
06/07/06   fh      Added Voice Service AES encryption/decryption under the features
05/17/06   fh      Merged in new development of AKA/AES features
05/08/06   fc      Corrected the setting of fch_cc_incl snm_gen_config.
05/01/06   an      Fixed Lint Errors
04/27/06   fc      Fixed lint errors.
04/24/06   fc      Fixed Rel A featurization.
04/21/06   fh      AES change: Removed the code under the feature
                   FEATURE_IS2000_REL_A_AES_BS_SUPPORTS outside the feature
                   FEATURE_IS2000_REL_A_AES.
03/15/06   fc      Fixed the issue of generating invalid MUX option for
                   RC 1 and 2.
03/14/06   fh      Added AES support.
10/18/05   va      Changed Pref CH_IND for unsupported SO from 0 to FCH. 0
                   ch_ind has special meaning in REL C.
08/03/05   fc      Fixed lint errors.
06/30/05   an      Include max_alt_so SO's in alt list in snm_get_alt_so_list
06/23/05   fc      Updated snm_gen_config.
05/03/05   fc      Merged the followning from MSMSHARED:
05/03/05   an       Checked if snm_so_is_enabled for alternate so's in snm_eval_so
04/27/05   an       Added support for 4GV service option
04/19/05   fc      Merged the following from MSMSHARED:
04/12/05   az       Fix for CR 60463. SDB_OMIT info is being passed through
                    callback to DS for assembling SDB header.
03/28/05   fc      Added support for DCA.
03/23/05   fc      Merged the following from MSMSHARED:
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
02/18/05   fc      Merged the following from MSMSHARED:
02/09/05   az       Fix for CR 41052. Rejecting config RS2, 18X.
02/10/05   va      Merged from msmshared Tip:
02/03/05   fc       Fixed lint error.
02/01/05   sb       Changed log_alloc_ex to log_alloc (log_alloc_ex is obsolete).
01/04/05   sb       Log code uses FPOS instead of sizeof, to account for padding.
12/10/05   sb       Fixed lint errors.
02/09/05   az      Added check in snm_validate_rev_sch_cfg()
                   as well.
02/09/05   az      Rejecting config RS2 with 32X
01/17/05   sb      Fixed incorrect condition check.
01/14/05   sb      Release D interface updates.
01/10/05   va      Merged from REL_B_C archive:
12/10/04   sb        Fixed lint errors.
12/22/04   fc      Code review comment changes.
11/12/04   fc      Fixed issue with V2.
11/11/04   fc      Corrected featurization for Rel D DV.
11/12/04   fc      Removed redundant featurization.
                   Fixed P_REV checks for DV.
10/07/04   fc      Added more comments.
10/01/04   fc      Added some more featurization.
09/28/04   fc      Changed log record format.
09/27/04   va      Merged following from REL B/C archive
07/29/04   bkm      Set bit for requested SO as well as alt SOs for same SOG.
06/30/04   fc       Fixed featurization.
09/16/04   fc      Added support for reverse DV.
09/01/04   fc      Fixed lint error.
08/18/04   fc      Added P_REV check.
08/16/04   fc      Fixed compilation issue for Rel B/C undefined.
08/09/04   fc      Service negotiation changes for Release C and D.
07/09/04   bkm     Fix compilation issue for Rel B undefined.
06/29/04   bkm     Merge of file history comments:
  05/21/04   fc      Lint clean up.
  05/20/04   fc      For fast call setup, if there is no restored SO
                     connection supports SCH, don't invalidate SCR.
  05/19/04   fc      Fixed an index reference error.
06/21/04   va      Changed update NNSCR (from pending to current) format a little bit
06/16/04   fc      Support for Release C addendum SYNC ID changes.
06/08/04   bkm     Fixed typos in feature names within comments.
05/28/04   bkm     Merged the following from IS2000_REL_B_C:
  05/10/04   fc      Removed an erronous else if condition.
04/08/04   bkm     Channel mask updates.
04/01/04   bkm     Fixed compilation issue after merge.
03/26/04   bkm     Merged the following from IS2000_REL_B_C:
  03/18/04   fc      Code review comments.
  03/17/04   va      Update DCCH OLPC params properly if it comes in NNSCR
  03/13/04   fc      Added comments.
  03/12/04   fc      Added support for Release C fast call setup.
  02/10/04   fc      Put back reject for SCR with asymmetric rate for the
                     scenario of zero number of connections.
  02/05/04   fc      Fixed merge error.
  01/20/04   fc      Corrected fundicated RCs validation for P3 to P1 transition.
01/13/05   bkm     Use REL_C_DV featurization around PDCH functions.
01/05/04   bkm     Misc updates to support DV F-PDCH.
12/04/03   bkm     Added snm_so_supports_pdch().
12/02/03   bkm     Update SNM ifaces to use separate fwd/rev channel masks
                   instead of OTA chind.  Also removed P2 featurization on
                   internal chind variable.
11/12/03   fc      Changed snm_eval_config to have MUX option carried over
                   from negotiation_infoif applicable.
08/14/03   va      Removed FEATURE_JCDMA_MAX_RATE feature for snm_set_sch_mux_list
08/25/03   fc      Fixed RC mismatch condition check in snm_eval_config.
08/01/03   ph      Added chind to config eval.
07/30/03   ph      Check suggest_alt before suggesting alternative for
                     - non-zero connection records while evaluating configurations.
                     - incompatible fch/dcch RCs.
08/01/03   yll     Added support for Dynamic Feature.
07/23/03   bkm     Some more lint cleanup.
07/16/03   va      Handle the case where we are evaluating EOM SO before SCM is
                   rxed for the SO to be connected.
07/16/03   fc      Added P_REV check in snm_is_so_rc_compatible_with_mux.
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
06/18/03   fc      Added support for decoupling FCH and DCCH MUX.
06/02/03   bkm     Lint cleanup.
06/05/03   va      Put FEATURE_IS2000_TDSO around TDSO SO refernces.
06/04/03   va      Added interface to get RLP BLOB for CAP IR :
                   snm_get_rlp_cap_blob
04/23/03   bkm     Added snm_set_sch_mux_list to support Max Rate feature.
                   Mux options for SO33 must be adjusted when the Max Rate
                   is changed.
04/11/03   ph      Added F3 to clarify alternate SO search.
03/28/03   va      Release B support.
03/14/03   ph      Fix not to reject TDSO/STDSO when they don't have a fundicated
                   channel mapped to them in the LPM.
03/05/03   rs      Added a couple of F3 messages in snm_eval_non_neg_scr().
01/27/03   jqi     Added support for band class 6.
11/11/02   ph      Fixed typo.
11/07/02   az      Removed FEATURE_IS2000_REV0_PREADDENDUM
11/07/02   az      Removed FEATURE_3GDATA_PROTOTYPE
11/07/02   az      Mainlined FEATURE_IS95B
11/07/02   va      Checked num_for/rev_sch in addition to for/rev_sch_cc_incl
                   in SCR before rejecting SCM for targets that do not support
                   SCH.
10/03/02   va      Added fn to set default fields of a SCR from current cfg.
10/03/02   ph      Lint Cleanup
10/02/02   az      Mainlined FEATURE_IS2000
09/16/02   ph      Changes to LPM processing.
09/06/02   va      Moved SCR logging into the function that is executed in MC
                   context.
08/23/02   ph      Fixed error in verifying LPM entries.
08/12/02   yll     Changes in converting the alternate SOs to SO_BITMAP, if
                   the primary SO does not have a group number (for example,
                   0x8000), we will try to match the alternate SO's group
                   number and use that in SO_BITMAP.
08/08/02   ph      Clean up snm_verify_phy_chan_allowed() (code review)
08/08/02   ph      Check prev for SCH validation.
08/02/02   ph      Fix for rel A turned off.
07/24/02   ph      While verifying downloaded LPM, verifying traffic channel entries
                   in the forward direction as well, if applicable.
07/15/02   ph      Changed validation of LPM to be comprehensive. We now reject
                   cases only if there is no way for us to proceed.
07/02/02   ph      Removed some rejection cases for LPM temporarily for testing purposes.
                   (will be revamped later with comprehensive validation).
06/28/02   ph      Included ch_ind for snm_update_lpm_table(). Added P2 featurization
                   to snm_is2000_nnscr_lpm_default().
06/26/02   ph      Initialization of sr_id for PREV < 6
06/18/02   ph      Added new function snm_verify_phy_chan_allowed and boundary checking
                   when initializing the default LPM table.
06/07/02   ph      Fix compile errors and some cleanup
06/05/02   ph      Updated snm_update_lpm_table() to handle the case we may be
                   updating the pending cfg.
05/31/02   va      changed snm_verify_chind to check if supported channels is part
                   of current chind ( as opposed to being equal to)
05/29/02   ph      Added support for LPM table
05/02/02   ph      Added QOS support and some cleanup.
05/01/02   ph      Some cleanup of featurization.
04/18/02   ph      setting so to CAI_SO_NULL for the case when num_con = 0.
04/17/02   lh      Added calling voc_release() if SCR does not contains voice SO.
03/21/02   ph      Initialized rlp_blob_generator in snm_add_so().
03/19/02   ph      When an EOM SO is not compatible, added support to suggest
                   alternatives (reorganized snm_is_eom_so_compatible_cur_cfg()).
03/12/02   snn     Modified the feature around the function
                   snm_reg_rlp_blob_generator from FEATURE_IS2000_SCH to
                   FEATURE_IS2000 because, RLP BLOB is needed for SCH and FCH.
02/13/02   va      Merged from MSM archive
12/07/01   jqi     In snm_p_rev_verified, corrected evaluating band class to
                   CAI_BAND_PCS instead of 1 so that US PCS, Korean PCS and Band
                   Class 5 are supported.
02/11/02   va      Added support for not sending EOM for a SO both pri and sec
                   traffic types are taken and this is not a signaling SO.
01/31/02   va      Took care of syncing up mux and RC in snm_gen_config
01/24/02   ph      Fixed PREV 6 compliance issue with max_rate change in
                   an accepted config in snm_eval_config().
01/23/02   ph      Added SNM support for proposing SVD configurations.
                   Also, support for EOM handling, architectural changes,
                   cleanup of legacy design.
12/11/01   ph      merged in SVD branch
                   11/30/01   va      Added a new funtion snm_get_so_grp_num
                   09/04/01   ph      added error message for sch_cc verification
                   09/04/01   ph      Included support to allow sch_cc verification for SVD
                   08/29/01   jrp     Removed redundant check for ltu_info_incl, use_var_rate and
                                      use_flex_num_bits.
                   08/28/01   lh      Added P_REV checking for MuxPDU 5.
                   08/27/01   jrp     Reject configuration if ltu_info_incl, use_var_rate or
                                      use_flex_num_bits is on.
                   08/29/01   lh      Disable MuxPDU 5 for P_REV_IN_USE < 7.
                   08/23/01   ph      SVD changes (initial cut)
12/07/01   ph      Merged in from MSM_CP.05.00.108
                   11/20/01   lcc     In snm_gen_config, corrected setting of rlp_info_incl to 1 when
                                      info. is included instead just having a non-zero value.  Parser
                                      expects this to be 1 instead of non-zero.
10/17/01   jrp     Added support for new service configuration log packet.
10/15/01   kk      Fixed compiler errors when Release A is not defined.
08/28/01   lh      Added P_REV checking for MuxPDU 5.
08/29/01   jrp     Removed redundant check for ltu_info_incl, use_var_rate and
                   use_flex_num_bits.
08/27/01   jrp     Reject configuration if ltu_info_incl, use_var_rate or
                   use_flex_num_bits is on.
08/16/01   kk      Merged in from MSM_CP.05.00.89.
                   08/07/01   ph      In snm_check_con_and_alternate_cfg(),
                   removed restrictive verification of sr_id for P2.
                   Also, cleaned up initialization of beginning_so in snm_eval_paged_so()
                   for the case when the paged SO is not enabled.
08/09/01   ph      Merged in from MSM_CP.05.00.87
                   08/06/01   jrp     Added ifdef to solve compilation issue with non FEATURE_IS2000
                                      builds.
                   08/02/01   jrp     Added support for CAI_MUX_WILDCARD.  This code was added
                                      to allow for mux independent SO54, SO55.  However, it is
                                      general, and can be used for future SOs.
                   07/26/01   ph      Added Copying of DCCH info in snm_update_configuration()
06/27/01   va      Removed check of sch-mux/RC inconsistency for 0xf20 mux option.
06/26/01   ph      Merged in from MSM_CP.05.00.81
                   06/14/01   ht      Enclosed num_good, s_index, j, inside feature to
                                      fix a compiler warning.
                   06/08/01   ph/kk   Allowing Mixed Rate Sets (removal of some checks and addition
                                      of other checks between SCH RC's and SCH Mux)
                   06/01/01   fc      Changed snm_update_non_neg_configuration to accept constant
                                      pointer as argument.
                   05/29/01   fc      Changed snm_update_non_neg_configuration function prototype.
                   05/22/01   lcc     Corrected the problem of using uninitialized data when
                                      responding with CAI_SO_REJ in page response.
05/25/01   va      Changed argument to snm_update_non_neg_cfg to accept a pointer as
                   an argument.
05/17/01   ph      Merged from MSM_CP.05.00.76
                   05/03/01   lcc     1. Corrected service negotiation of traffic type such that
                                        NULL traffic type is not accepted conditionally.
                                      2. Allow any traffic type to be used when service option
                                         uses NULL traffic.
                   04/10/01   lcc     1. Added service negotiation support for DCCH.
                                      2. Changed prototypes of snm_get_current_config and
                                         snm_get_current_nnscr to return the pointers directly.
                                      3. Added function snm_update_fpc_pri_chan.
                   04/09/01   ych     Removed the code that used to incorrectly check for the
                                       Data service options under FEATURE_JCDMA_DS
                   03/31/01   fc      Added function snm_update_non_neg_configuration.

03/12/01   lcc     In validation of SR_ID, allows SR_ID of 0 for service options that
                   uses signalling messages (e.g. SMS).  This is in anticipation of a
                   proposed change in the Standard.
03/01/01   va      Merged the following from the common archive:
           va      Corected the RATE_SET check macro which was using FWD RC macro
                   to check REV RC.
           ych     Merged T-53/JCDMA features for SK.
           lcc     1. In snm_gen_config, when P_REV is less than 6, the IS2000 fields
                      in SCR are now initialized.  If not, it may cause problem when
                      service option negotiation is used.
                   2. Added functions snm_is2000_scr_init and snm_is2000_nnscr_init.
           jq      Made some naming changes to variables and functions. Added
                   FEAUTURE ifdefs to make it compilable when SCH is undefed
           lcc     1. Added FEATURE_IS2000 in snm_gen_config where it was left out.
           lcc     1. Corrected hardcoded value of coding type in snm_populate_f_sch_cfg
                      and snm_populate_r_sch_cfg from 1 to mccap_get_preferred_sch_coding.
                   2. In snm_gen_config, when connection is not needed, initialize
                      all the channel configuration field to not included since
                      they are quite meaningless before a connection is established.
           lcc     1. Corrected a problem in snm_gen_config which may use a
                      uninitialized so_index.
                   2. Added initialization of rate set to RS 1 in snm_get_rate_set
                      when SO is not in SNM.  This allows a default rate set to use
                      on TC if SO is CAI_SO_REJ.
           va      Removed check for "config_valid" flag in snm_get_current_nnscr()
                   function. This is always valid because of the defaults we init with.
           lcc/hrk Added support to check coding depending on MSM capability.
02/27/01   kk      New function snm_convert_alt_so_bitmap added to form the
                   SO bitmap and fill the group information.
02/27/01   va      Added negotiation support for new rel A parameters.
11/02/00   lcc     1. Support for migrating all SOs to SNM.
                   2. In service configuration, generate new fields only when
                      p_rev_in_use is 6 or higher.
                   3. Added function snm_p_rev_verified, which maps BC 1 P_REV 1
                      to equivalent of BC 0 P_REV 3.
09/11/00   yll     Changes to allow the NULL traffic type in service
                   configuration for both forward and reverse traffics.
08/23/00   lcc     Fixed lint warnings.
08/17/00   va      For and rev SCH CC in SCR are partial updates. Added
                   support to keep old config, if new one does not have these CCs.
06/22/00   va      Support for NV pref RC for data SOs also. Also  added
                   support to cache the NV item.
06/15/00   va      Incorporated code review changes; Selecting right sch_id
                   and its cfg if the BS proposes a list of multiple SCH cfgs
06/12/00   va      Added RC-MUX negotaion for all data SOs
05/05/00   lcc     Corrected placement of FEATURE_IS2000 around prototype of
                   snm_initialize_connection.  Also added function to get current
                   nnscr.
05/04/00   va      Support for fch_cc_incl to be optional in SCR
04/24/00   lcc     Added rlp_blob related stuff.  Separated snm_update_configuration
                   into 2 functions: snm_update_configuration and snm_continue_update_configuration
                   Some minor cleanup as well.
04/20/00   lad     Made minor renaming changes to service config log.
04/17/00   va      Changed snm_get_pref_RCs funtion to return void.
04/10/00   lad     Added Service Configuration Logging.
04/05/00   va      Support for rc list
03/15/00   ry      Added IS2000 featurization
02/16/00   lcc     Added minimal support for RC list.
02/13/00   va      Addendum Changes.
12/20/00   va      Added support for RC checks, pref RC FOR DATA SOs also.a
                   This acts in backward compatible mode only.
11/15/99   va      IS2000 Voice Changes:
                   Support for non negotiable service configuration.
                   New function snm_get_pref_rcs() for getting preferred RCs
                   given a SO.
07/30/99   lcc     1) Implemented code review comments.  2) Encapsulated
                   srv_eval_cfg and srv_alt_cfg into snm_eval_config and
                   changed its function return to be more specific.  Now,
                   snm_eval_config can be used to evaluated any configuration
                   which helps to clean up several functions in srv.c.
                   3) Encapsulated srv_eval_so in snm_eval_paged_so such that
                   snm_eval_paged_so can be used to evulated any SO.
07/19/99   nmn     Added definition of snm_get_alt_so_list, surrounded by
                   #ifdef of FEATURE_IS95B or FEATURE_SPECIAL_MDR.  Also
                   fixed a lint switch which was formatted incorrectly.
06/15/99   doj     Moved service config externs to srv.h to support service
                   config via handoff message.  Fixed Lint errors
                   ignoring return value of functions by casting them to void.
05/14/99   lcc     In snm_eval_config, performs function returns appropriate when
                   suggest_alt is set to false.  Also corrected some problems which cause
                   negotiation_info.max_mux_proposed to be set to an incorrect value.
05/02/99   lcc     In snm_eval_config, changed value returned in first connection
                   from CAI_SO_REJ to CAI_SO_NULL when the SO is not managed
                   by SNM.
04/06/99   lcc     Changed some message levels.  Also corrected the counter-
                   proposal if base station propose an invalid traffic type.
03/26/99   lcc     Corrected the mux options to propose in snm_eval_config if
                   the base station proposes a mux option that belongs to the
                   wrong rate set.
02/16/99   nmn     Cleaned, linted, and tested SOCM support
02/12/99   lcc     Added several function to get information from SNM as well as
                   changed some names to conform to standard.
01/06/99   lcc     Corrected a minor problem when FEATURE_IS95B_MDR is not
                   defined.
12/09/98   lcc     Changed snm_initialize_connection to return a status.
12/08/98   lcc     Added some messages.
12/03/98   lcc     Added checking for a null seed in snm_initialize_connection.
11/25/98   lcc     Initial release.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "amssassert.h"
#include "target.h"
#include "customer.h"
#include "modem_1x_defs.h"
#include "modem_1x_utils.h"
#include "err.h"
#include "msg.h"
#include "m1x_diag.h"
#include "mc_v.h"
#include "mc_i.h"
#include "mccdma_i.h"
#include "cai_v.h"
#include "cai_i.h"
#include "caii_v.h"
#include "caii_i.h"
#include "srv.h"
#include "snm_v.h"
#include "snm_i.h"
#include "mccap_v.h"
#include "log.h"
#include "mccccl.h"
#include "mccccim.h"

#if defined(FEATURE_IS2000_REL_C)
#include "mccsyncid.h"
#endif /* FEATURE_IS2000_REL_C */

#include "dsrlp_api.h"

#ifdef FEATURE_IS2000_REL_A_AES
#include "mccaes.h"
#endif /* FEATURE_IS2000_REL_A_AES */

#ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
#include "mvs.h"
#endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

/*lint -e818 -e655 */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
                            Types
-------------------------------------------------------------------------*/

#ifdef FEATURE_IS2000_REL_A

/*lint -e754 Lint complains that the reserved fields are not used, but they are
  needed so that the total number of bits is 8. */

/* Please note that words should always be aligned to an even address.  Add reserved
   bytes to ensure this (and the reserved bytes must also be in the tools interface). */

typedef PACK (union)
{
  PACK (struct)
  {
    uint8 fch      :1;  /* FCH is active */   /*lint !e46 uint8 intended */
    uint8 dcch     :1;  /* DCCH is active */  /*lint !e46 uint8 intended */
    uint8 f_sch    :1;  /* F-SCH is active */ /*lint !e46 uint8 intended */
    uint8 r_sch    :1;  /* R-SCH is active */ /*lint !e46 uint8 intended */
#ifdef FEATURE_IS2000_REL_C
    uint8 f_pdch   :1; /* F-PDCH is active */ /*lint !e46 uint8 intended */
    uint8 r_pdch   :1; /* R-PDCH is active */ /*lint !e46 uint8 intended */
    uint8 reserved :2;                        /*lint !e46 uint8 intended */
#else
    uint8 reserved :4;                        /*lint !e46 uint8 intended */
#endif /* FEATURE_IS2000_REL_C */
  } individual_channels;

  uint8  mask;  /* Use to init entire mask. */

} ch_info_mask;

typedef struct
{
  uint8  fwd_rc; /* Radio configuration for the forward link */
  uint8  rev_rc; /* Radio configuration for the reverse link */
} ch_info_type;

typedef struct
{
  uint8  frame_40_used :1;  /* 40ms frames are used */ //lint !e46 uint8 intended
  uint8  frame_80_used :1;  /* 80ms frames are used */ //lint !e46 uint8 intended
  uint8  reserved      :6;                             //lint !e46 uint8 intended
} frame_size_type;

typedef struct
{
  uint8  sch_mux_hi;   /* High byte of SCH mux option */
  uint8  sch_mux_lo;   /* Low byte of SCH mux option */
  uint8  sch_rc;       /* Radio configuration for SCH */
  uint8  coding;       /* Convolutional/Turbo coding indicator.
                          0 indicates convolutional only.
                          1 indicates coding depends on bit rate. */
  frame_size_type frame_size;
  uint8  max_rate;     /* Maximum negotiated data rate */
  uint8  frame_offset; /* Frame offset for the SCH */
} sch_info_type;

typedef struct
{
  uint8  con_ref;            /* Connection reference */
  uint8  service_option_hi;  /* High byte of service option */
  uint8  service_option_lo;  /* Low byte of service option */
  uint8  for_traffic;        /* Traffic type used for forward link */

  uint8  rev_traffic;        /* Traffic type used for reverse link */
  uint8  ui_encrypt_mode;    /* Encryption mode indicator for user privacy */
  uint8  sr_id;              /* Service Reference Identifer */

  uint8  rlp_blob_len;       /* RLP information block of bits length */
  uint8  qos_params_len;     /* Length of the block of QoS parameters */

  uint8  reference_buf[46];

  /*
   *  Fill the Reference Buffer with the following:
   *
   *  - RLP_BLOB_LEN bytes corresponding to the RLP_BLOB
   *
   *  - QOS_PARAMS_LEN bytes corresponding to the QOS parameters.
   */

} con_rec_info_type;

typedef union
{
  struct
  {
    uint8  forward  :1;  /* Forward mapping indicator */ //lint !e46 uint8 intended
    uint8  reverse  :1;  /* Reverse mapping indicator */ //lint !e46 uint8 intended
    uint8  reserved :6;                                  //lint !e46 uint8 intended
  } individual_direction;

  uint8  mask;  /* Use to init entire mask. */

} direction_type;

typedef struct
{
  uint8           sr_id;                /* Service reference identifier */
  uint8           logical_resource;     /* Logical resource identifier */
  uint8           physical_resource;    /* Physical resource identifier */
  direction_type  forward_reverse_flag;
  uint8           priority;             /* Multiplexing priority */
} lpm_info_type;

typedef struct
{
  uint8 nbits_idx;  /* SCH number of information bits index */
  uint8 ltu_len_hi; /* High LTU length - number of LTUs per physical layer SCH frame */
  uint8 ltu_len_lo; /* Low LTU length - number of LTUs per physical layer SCH frame */
} ltu_row_type;

typedef struct
{
  uint8 ltu_table_id; /* LTU table identifier */
  uint8 num_rows;     /* Number of LTU rows */

  uint8 reference_buf[1];

  /*
   *  Fill the Reference Buffer with NUM_ROWS instances of LTU_ROW_TYPE
   */

} ltu_info_type;

typedef struct
{
  uint8 fsch0_ltu_tab_id;
  /* This field shall be set to 0 if the F-SCH is to use default LTU sizes or if F-SCH is not part of the SCR. */
  uint8 rsch0_ltu_tab_id;
  /* This field shall be set to 0 if the R-SCH is to use default LTU sizes or if R-SCH is not part of the SCR. */

} ltu_tab_id_type;

#ifdef FEATURE_IS2000_REL_B
typedef struct
{
  uint8 for_dcch_mux_option_hi;
    /* High byte of forward link DCCH multiplex option */
  uint8 for_dcch_mux_option_lo;
    /* Low byte of forward link DCCH multiplex option */
  uint8 rev_dcch_mux_option_hi;
    /* High byte of reverse link DCCH multiplex option */
  uint8 rev_dcch_mux_option_lo;
    /* Low byte of reverse link DCCH multiplex option */
} snm_log_mux_info_type;
#endif /* FEATURE_IS2000_REL_B */

/* The maximum size that the buffer in LOG_SRV_CONFIG_VER_3_C or
   LOG_SRV_CONFIG_VER_2_C will require */

#ifdef FEATURE_IS2000_REL_B
  #define LOG_SRV_CONFIG_BUFFER_REQ_FOR_REL_B sizeof(snm_log_mux_info_type)
#else /* FEATURE_IS2000_REL_B */
  #define LOG_SRV_CONFIG_BUFFER_REQ_FOR_REL_B 0
#endif /* FEATURE_IS2000_REL_B */

#ifdef FEATURE_IS2000_REL_C

    #define LOG_SRV_CONFIG_BUFFER_REQ_FOR_REL_C (1 + CAI_SYNC_ID_LEN_MAX)

#else /* FEATURE_IS2000_REL_C */
    #define LOG_SRV_CONFIG_BUFFER_REQ_FOR_REL_C 0
#endif /* FEATURE_IS2000_REL_C */

  #define LOG_SRV_CONFIG_BUFFER_REQ_FOR_REL_C_DV 0

  #define LOG_SRV_CONFIG_BUFFER_REQ_FOR_REL_D_DV 0

#define LOG_SRV_CONFIG_VER_2_OR_3_BUFFER_SIZE \
    ( sizeof(ch_info_type) \
     + sizeof(ch_info_type) \
     + sizeof(sch_info_type) \
     + sizeof(sch_info_type) \
     + ( CAII_SRV_CON_MAX *  \
        (9 /* Position of Ref_Buf*/ \
        + 15 /*rlp_blob_len*/ \
        + 31 /*qos_params_len*/) ) \
     + ( CAI_MAX_NUM_LPM_ENTRIES * sizeof(lpm_info_type)) \
     /* LTU tables length is not added because they are not supported. */ \
     + LOG_SRV_CONFIG_BUFFER_REQ_FOR_REL_B \
     + LOG_SRV_CONFIG_BUFFER_REQ_FOR_REL_C \
     + LOG_SRV_CONFIG_BUFFER_REQ_FOR_REL_C_DV \
     + LOG_SRV_CONFIG_BUFFER_REQ_FOR_REL_D_DV \
    )


/* FC: Tools not ready
#ifdef FEATURE_IS2000_REL_C
LOG_RECORD_DEFINE(LOG_SRV_CONFIG_VER_4_C)
#elif defined(FEATURE_IS2000_REL_B)
*/
#ifdef FEATURE_IS2000_REL_B
LOG_RECORD_DEFINE(LOG_SRV_CONFIG_VER_3_C)
#else
LOG_RECORD_DEFINE(LOG_SRV_CONFIG_VER_2_C)
#endif /* FEATURE_IS2000_REL_B */

  uint16 fwd_mux_option; /* Forward link multiplex option */
  uint16 rev_mux_option; /* Reverse link multiplex option */

  /************************************/

  uint8 fwd_num_bits; /* Forward link transmission rates */
  uint8 rev_num_bits; /* Reverse link transmission rates */

  ch_info_mask channels;
  uint8        num_con_rec; /* Number of service option connections */

  /************************************/

  uint8 num_lpm_entries; /* Number of LPM entries */
  uint8 num_ltu_tables;  /* Number of LTU tables
                            This field shall be set to 0 if the SCR does not include SCH configuration information
                            It shall also be set to 0 if the MS is to use the default LTU sizes in both F-SCH and
                            R-SCH */

  uint8 reference_buf[LOG_SRV_CONFIG_VER_2_OR_3_BUFFER_SIZE];

  /*
   *  Fill the Reference Buffer with the following:
   *
   *  - If FCH is active, include 1 instance of CH_INFO_TYPE for the FCH information.
   *
   *  - If DCCH is active, include 1 instance of CH_INFO_TYPE for the DCCH information.
   *
   *  - If F-SCH is active, include 1 instance of SCH_INFO_TYPE for the F-SCH information.
   *
   *  - If R-SCH is active, include 1 instance of SCH_INFO_TYPE for the R-SCH information.
   *
   *  - Include NUM_CON_REC instances of CON_REC_INFO_TYPE.
   *
   *  - Include NUM_LPM_ENTRIES instances of LPM_INFO_TYPE.
   *
   *  - Include NUM_LTU_TABLES instances of LTU_INFO_TYPE.
   *
   *  - If NUM_LTU_TABLES is > 0, include 1 instance of LTU_TAB_ID_TYPE.
   */

LOG_RECORD_END
#endif /* FEATURE_IS2000_REL_A */

/*---------------------------------------------------------------------------
  Service Configuration Log Type
---------------------------------------------------------------------------*/
typedef PACK(struct) {
  PACK(struct) {
    byte sch_id   : 3; /* SCH ID             */
    byte fwd_rev  : 1; /* 0 == fwd, 1 == rev */
    byte reserved : 4;

  } identifier;

  int16 mux_opt;     /* SCH Mulitplex option */
  byte  coding_type; /* SCH Coding type      */

} mc_log_svc_config_sch_info_type;

typedef PACK(struct) {
  uint16 f_fch_rc  :4; /* Forward Fundamental Channel Radio Config       */
  uint16 f_sch0_rc :4; /* Forward Supplemental Channel 0 Radio Config    */
  uint16 f_sch1_rc :4; /* Forward Supplemental Channel 1 Radio Config    */
  uint16 f_dcch_rc :4; /* Forward Dedicated Control Channel Radio Config */

} mc_log_fwd_ch_rc_type;

typedef PACK(struct) {
  uint16 r_fch_rc  :4; /* Reverse Fundamental Channel Radio Config       */
  uint16 r_sch0_rc :4; /* Reverse Supplemental Channel 0 Radio Config    */
  uint16 r_sch1_rc :4; /* Reverse Supplemental Channel 1 Radio Config    */
  uint16 r_dcch_rc :4; /* Reverse Dedicated Control Channel Radio Config */

} mc_log_rev_ch_rc_type;

/* LOG_SVC_CONFIG_C */
typedef PACK(struct)
{
  log_hdr_type hdr;

  uint16 so;         /* Service Option */

  /* Radio Configurations */
  mc_log_fwd_ch_rc_type fwd_ch_rc;
                     /* Forward channel Radio Config Info */
  mc_log_rev_ch_rc_type rev_ch_rc;
                     /* Reverse channel Radio Config Info */

  byte fch_frame_size;   /* FCH frame size  */
  byte dcch_frame_size;  /* DCCH frame size */

  byte fwd_traffic_type; /* Forward link traffic type */
  byte rev_traffic_type; /* Reverse link traffic type */

  int16 fwd_mux_opt;     /* Forward Link Multiplex option */
  int16 rev_mux_opt;     /* Reverse Link Multiplex option */

  byte fwd_rate;         /* Forward transmission rate */
  byte rev_rate;         /* Reverse transmission rate */

  byte num_for_sch;      /* Number of forward supplemental channels */
  byte num_rev_sch;      /* Number of reverse supplemental channels */

  /* The following entries will be ordered in order of active channels:
  ** F-SCH0, F-SCH1, R-SCH0, R-SCH1.  Inactive channels will not be
  ** included in the record.
  */
  mc_log_svc_config_sch_info_type sch_info[4];

} mc_log_svc_config_type;

/*-------------------------------------------------------------------------
                            Variables
-------------------------------------------------------------------------*/

/* !!!Caution!!!
   negotiation_info holds pointers to the following data structure.  Since the
   following structures are legacy code, they are still defined as what they
   were before.  They should eventually be moved into negotiation_info and be
   hidden.

   The structures of interest are srv_current_cfg, srv_pending_cfg, and
   srv_proposed_cfg, and they are declared extern in srv.h.

   This data structure keeps track of the progress of negotiation.  It must be
   initialized before service negotiation on traffic channel begins.  It is
   initialized in srv_init for originations and snm_eval_paged_so for
   terminations.  Note the ???_config members are already maintained in
   srv.c.  Their pointer are kept here to maintain the cohesion.  When time
   permits, these instances can be moved here instead of just the pointers.
   Note that since this data structure maintains negotiation in progress, the
   information captured here may not represent the active */
static negotiation_info_type negotiation_info;

/* Service Negotiation Parameters (SNP), keeps all service option descriptors */
static snp_type snp;

/* Table to map rate values into 1X, 2X, ... */
const uint8 snm_rs1_rate_table[] = { 1, 2, 4, 8, 16, 32, 64 };
const uint8 snm_rs2_rate_table[] = { 1, 2, 4, 8, 16, 18, 32, 36, 72 };

/* Table to map max conv/turbo block size values into 1X, 2X, ...
   This is the same for RS1 and RS2 in the std, so use just 1 table */
const uint8 snm_conv_turbo_rate_table[] = { 1, 2, 4, 8, 16, 32, 64 };


/* This is a variable used by snm_eval_config(). The only reason it is
   not defined in the stack is that it is too big and can cause stack
   overflows. Hence */
static caii_srv_cfg_type local_srv_cfg;

/*-------------------------------------------------------------------------
                           Functions
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION snm_add_so

DESCRIPTION
  This function adds a service option descriptor to SNP.

DEPENDENCIES
  SNP must be initialized (snm_initialize_snm does it).

RETURN VALUE
  SNM_SUCCESS if the SO is added successfully;otherwise if there is no more
  room in SNP, or if a same SO already exists, it returns SNM_FAILURE.

SIDE EFFECTS
  None.

===========================================================================*/

snm_status_type snm_add_so
(
  word                   so_number,
  snm_neg_par_type       *neg_par,
  const snm_config_type  *default_config
)
{
  snm_status_type status = SNM_FAILURE;

  if ((neg_par->fwd_rc.num_of_rc == 0) ||
      (neg_par->rev_rc.num_of_rc == 0))
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Num RCs is 0 for SO %d",
      so_number);
    return SNM_FAILURE;
  }

  /* Note: the following is a critical section.  This function is used
     by both Data and TDSO. */
  REX_ISR_LOCK(&mc_crit_sect);

  if (snp.total_so < MAX_SUPPORTED_SO)
  {
    word  n;

    /* If there is room, check if SO is already in list */
    for (n = 0; n < snp.total_so; n++)
    {
      /* If SO already exists, stop and return SNM_FAILURE */
      if (snp.so[n].service_option_num == so_number)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "SO added twice: %d",
          so_number);
        break;
      }
    }

    if (n == snp.total_so)  /* This means SO is not found in array */
    {
      /* Populate descriptor with function parameters... */
      snp.so[n].service_option_num = so_number;
      snp.so[n].negotiation_pars = *neg_par;
      snp.so[n].default_config = *default_config;

      /* ... and initialize some to defaults */
      snp.so[n].socm_handler = NULL;
      snp.so[n].status = SNM_ENABLED;
      snp.so[n].page_response_so = so_number;
      snp.so[n].connection_initializer = NULL;
      snp.so[n].rlp_blob_generator = NULL;

#ifdef FEATURE_IS2000_REL_A
      snp.so[n].qos_blob_generator = NULL;
      snp.so[n].qos_blob_eval = NULL;
      snp.so[n].sdb_so_omit_status_handler = NULL;
#endif /* FEATURE_IS2000_REL_A */

      /* Update total of SOs */
      snp.total_so++;
      status = SNM_SUCCESS;
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Too many SOs. MAX: %d, SO: %d",
      MAX_SUPPORTED_SO,
      so_number);
  }

  REX_ISR_UNLOCK(&mc_crit_sect);

  /* status should have been set correctly, return it */
  return status;

} /* snm_add so */

/*===========================================================================

FUNCTION snm_find_so

DESCRIPTION
  This function locates a SO in SNP.  It returns failure is the SO is not
  in SNP.

DEPENDENCIES
  None.

RETURN VALUE
  SNM_SUCCESS if SO found.  SNM_FAILURE if SNP is empty or not initialized.

SIDE EFFECTS
  None.

===========================================================================*/

snm_status_type snm_find_so
(
  word so,    /* Service option number */
  word *index /* Variable to hold index to SO found */
)
{
  word n;

  /* If SNP has more than MAX_SUPPORTED_SO, it must not have been initialized
     or something really bad happened. Log the error */
  if (snp.total_so > MAX_SUPPORTED_SO)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "SNM not initialized properly: %d",
      so);
    return SNM_FAILURE;
  }

  /* Or if there is nothing, return SNM_FAILURE */
  if (snp.total_so == 0)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "SNM is empty: %d",
      so);
    return SNM_FAILURE;
  }

  /* Now locate the service option in SNP */
  for (n = 0; n < snp.total_so; n++)
  {
    /* If the service option number matches, that's it. */
    if (snp.so[n].service_option_num == so)
    {
      *index = n;
      return SNM_SUCCESS;
    }
  }

  /* If it reaches here, SO is not found */
  return SNM_FAILURE;

} /* snm_find_so */

/*===========================================================================

FUNCTION snm_enable_so

DESCRIPTION
  This function enables a service option.

DEPENDENCIES
  None.

RETURN VALUE
  SNM_SUCCESS if SO successfully enabled; otherwise if the SO is not found in SNP,
  SNM_FAILURE is returned.

SIDE EFFECTS
  None.

===========================================================================*/

snm_status_type snm_enable_so
(
  word so_number
)
{
  word so_index;

  /* Is it in SNP? */
  if (snm_find_so(so_number, &so_index) == SNM_SUCCESS)
  {
    /* Yes, then change status of SO */
    snp.so[so_index].status = SNM_ENABLED;
    return SNM_SUCCESS;
  }
  else
  {
    /* else failed */
    return SNM_FAILURE;
  }

} /* snm_enable_so */

/*===========================================================================

FUNCTION snm_initialize_snm

DESCRIPTION
  This function initializes the Service Negotiation Manager (SNM).  For the time
  being, all it does is to initialize SNP to contain no service options and to
  initialize some members in negotiation_info that needs initialized only once.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void snm_initialize_snm
(
  void
)
{
  /* Clear service options in SNP */
  snp.total_so = 0;

  /* Initialize part of negotiation_info */
  negotiation_info.proposed_config = &srv_proposed_cfg;
  negotiation_info.current_config = &srv_current_cfg;
  negotiation_info.pending_config = &srv_pending_cfg.cfg;
  negotiation_info.pref_rc_valid = FALSE;

} /* snm_initialize_snm */

/*===========================================================================

FUNCTION snm_disable_so

DESCRIPTION
  This function disables a service option.

DEPENDENCIES
  None.

RETURN VALUE
  SNM_SUCCESS if SO successfully disabled; otherwise if the SO is not found in SNP,
  SNM_FAILURE is returned.

SIDE EFFECTS
  None.

===========================================================================*/

snm_status_type snm_disable_so
(
  word so_number
)
{
  word so_index;

  /* Is it in SNP? */
  if (snm_find_so(so_number, &so_index) == SNM_SUCCESS)
  {
    /* Yes, then change status of SO */
    snp.so[so_index].status = SNM_DISABLED;
    return SNM_SUCCESS;
  }
  else
  {
    /* else failed */
    return SNM_FAILURE;
  }

} /* snm_disable_so */

/*===========================================================================

FUNCTION snm_p_rev_verified

DESCRIPTION
  Verifies if the min_p_rev passed into the function is supported by the current
  p_rev_in_use.  We are making this a function because P_REV 1 has the equivalent
  functionality as P_REV 3 so when p_rev_in_use is 1, we consider it the same as
  3 for verifying service options.  As an example, if min_p_rev is 3 for a service
  option, it is allowed when p_rev_in_use is 1 (which is equivalent to 3), but is
  not allow when p_rev_in_use is 2 or 0.

DEPENDENCIES
  None

RETURN VALUE
  True if min_p_rev passes the current p_rev_in_use.

SIDE EFFECTS
  None
===========================================================================*/

static boolean snm_p_rev_verified
(
  uint8 so_min_p_rev
)
{
  uint8 effective_p_rev = mc_get_p_rev_in_use();

  /* Adjust the effective p_rev to 3 if p_rev_in_use is 1 and band class is
  ** either US PCS or Korean PCS or Band Class 5 or Band Class 6.
  */

  if (effective_p_rev == 1 &&
      (mcc_is_band_class_in_group(MCC_PCS_GROUP, cdma.band_class))
     )
  {
    effective_p_rev = 3;
  }

  if ( so_min_p_rev <= effective_p_rev )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* snm_p_rev_verified */

/*===========================================================================

FUNCTION snm_so_is_enabled

DESCRIPTION
  This function evaluates if a SO can be used under the current operating
  conditions.  For SNM managed SOs, SNP will be consulted to see if
  the SO is enabled.  For other SOs, srv_eval_so is used, which support
  smooth transition of SOs to SNM.  Eventually, when all SOs are captured
  in SNP, the call to srv_eval_so will be obsolete.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if SO is supported, FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/

boolean snm_so_is_enabled
(
  word so /* The service option number to be evaluated */
)
{
  boolean status = FALSE;
  word so_index;

  /* First look to see if SO in SNP.  This checking will not be necessary once
     all SOs are migrated to SNP. */
  if (snm_find_so(so, &so_index) == SNM_SUCCESS)
  {
    if ((snp.so[so_index].status == SNM_ENABLED) &&
        snm_p_rev_verified(snp.so[so_index].negotiation_pars.min_p_rev))
    {
      status = TRUE;
    }
  }

  return status;

} /* snm_so_is_enabled */

/*===========================================================================

FUNCTION snm_get_total_SOs

DESCRIPTION
  This function returns the total number of service options maintained by SNM.

DEPENDENCIES
  SNM must be initialized.

RETURN VALUE
  Number of SOs in SNM.

SIDE EFFECTS
  None
===========================================================================*/

word snm_get_total_SOs
(
  void
)
{
  return snp.total_so;

} /* snm_get_total_SOs */

/*===========================================================================

FUNCTION snm_get_SO_number

DESCRIPTION
  This function returns service option number of the nth SO in SNM.

DEPENDENCIES
  SNM must be initialized.

RETURN VALUE
  Service option number of quried SO.  If the entry does not exist, CAI_SO_REJ
  is returned.

SIDE EFFECTS
  None
===========================================================================*/

word snm_get_SO_number
(
  word index
)
{
  /* Don't allow indexing pass end of array */
  if ((index >= snp.total_so) || (index >= MAX_SUPPORTED_SO))
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "SO index does not exist: %d",
      index);
    return CAI_SO_REJ;
  }

  /* Now return service option number */
  return snp.so[index].service_option_num;

} /* snm_get_SO_number */

/*===========================================================================

FUNCTION snm_set_page_response_so

DESCRIPTION
  This function changes the page_response_so member of a SO descriptor.

DEPENDENCIES
  None.

RETURN VALUE
  SNM_SUCCESS if page response changed successfully; otherwise if the SO is not
  found in SNP, it returns SNM_FAILURE.

SIDE EFFECTS
  None.

===========================================================================*/

snm_status_type snm_set_page_response_so
(
  word so_number,  /* Service option number */
  word response_so
)
{
  word so_index;

  /* Is it in SNP ? */
  if (snm_find_so(so_number, &so_index) == SNM_SUCCESS)
  {
    /* Yes, then change page_response_so */
    snp.so[so_index].page_response_so = response_so;
    return SNM_SUCCESS;
  }
  else
  {
    return SNM_FAILURE;
  }

} /* snm_set_page_response */

/*===========================================================================

FUNCTION snm_get_page_response_so

DESCRIPTION
  This function returns the page_response_so member of the SO descriptor of
  the service option passed in as parameter.  If the SO is not found in SNP,
  the same service option is returned.

DEPENDENCIES
  None.

RETURN VALUE
  See description.

SIDE EFFECTS
  None.

===========================================================================*/

word snm_get_page_response_so
(
  word so_number /* Service option number */
)
{
  word so_index;

  /* Is it in SNP ? */
  if (snm_find_so( so_number, &so_index) == SNM_SUCCESS )
  {
    /* Yes, then return page_response_so */
    return snp.so[so_index].page_response_so;
  }
  else
  {
    /* else just return same SO */
    return so_number;
  }

} /* snm_get_page_response_so */

/*===========================================================================

FUNCTION snm_set_user_pref_fundicated_rc

DESCRIPTION
  This function caches the preferred fundicated RC value that is read from NV
  by MC.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void snm_set_user_pref_fundicated_rc
(
  cai_radio_config_type pref_fundicated_rc
)
{
  negotiation_info.pref_fundicated_rc = pref_fundicated_rc;
  negotiation_info.pref_rc_valid = TRUE;

  #ifndef FEATURE_IS2000_1X_ADV
  if (pref_fundicated_rc == CAI_RC_11)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid user preferred RC %d",
      pref_fundicated_rc);
  }
  #endif /* FEATURE_IS2000_1X_ADV */

} /* snm_set_user_pref_fundicated_rc */


#ifdef FEATURE_IS2000_REL_A_AES
/*===========================================================================

FUNCTION snm_set_ui_encrypt_mode

DESCRIPTION
  This function sets ui_encrypt_mode field in SCR. For now it is only being
  called by AES

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void snm_set_ui_encrypt_mode
(
  uint8 sr_id,                /* the record with this sr_id will be updated */
  uint8 ui_encrypt_mode       /* the new ui_encrypt_mode */
)
{
  uint8 i = 0;
  while ( i < negotiation_info.current_config->neg_scr.num_con )
  {
    if ( negotiation_info.current_config->neg_scr.con[i].sr_id == sr_id )
    {
      negotiation_info.current_config->neg_scr.con[i].ui_encrypt_mode = ui_encrypt_mode;
      break;
    }
    else
    {
      i++;
    }
  }
} /* snm_set_ui_encrypt_mode */


/*===========================================================================

FUNCTION snm_get_voice_so_sr_id

DESCRIPTION
  This function returns the sr_id for the voice service if it exists. For now
  this function is only called by AES.

DEPENDENCIES

RETURN VALUE
  sr_id of the voice service. If no voice service in current configuration, it
  returns MCCAES_INVALID_VOICE_SR_ID

  At the same time, it set the ui_encrypt_mode for the voice service. If no
  voice service, the value set through the pointer does not mean anything.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 snm_get_voice_so_sr_id
(
  uint8 *ui_encrypt_mode
)
{
  uint8 i = 0;

  if ( ui_encrypt_mode != NULL )
  {
    for ( i = 0; i < negotiation_info.current_config->neg_scr.num_con; i++ )
    {
      if ( mccaes_is_voice_so( negotiation_info.current_config->neg_scr.con[i].so ) )
      {
        *ui_encrypt_mode = negotiation_info.current_config->neg_scr.con[i].ui_encrypt_mode;
        M1X_MSG( DCP, LEGACY_MED,
          "Voice Service SR_ID %d, SO %d, ui_encrypt_mode %d",
          negotiation_info.current_config->neg_scr.con[i].sr_id,
          negotiation_info.current_config->neg_scr.con[i].so,
          negotiation_info.current_config->neg_scr.con[i].ui_encrypt_mode );
        return negotiation_info.current_config->neg_scr.con[i].sr_id;
      }
    }
  }

  return MCCAES_INVALID_VOICE_SR_ID;
} /* snm_get_voice_so_sr_id */

#endif /* FEATURE_IS2000_REL_A_AES */

/*===========================================================================

FUNCTION snm_get_user_pref_fundicated_rc

DESCRIPTION
  This function returns the user perferred fundicated RC as stored in NV
  that is cached by SNM.
  We need an interface for this because this value is needed by SRV also.
  Once it's all SOs are migrated to SNM, this value will be local to SNM.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if there is a user preferred RC, FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/

boolean snm_get_user_pref_fundicated_rc
(
  cai_radio_config_type *user_pref_fundicated_rc
)
{
  if (negotiation_info.pref_rc_valid)
  {
    *user_pref_fundicated_rc = negotiation_info.pref_fundicated_rc;
  }

  return negotiation_info.pref_rc_valid;

} /* snm_get_user_pref_fundicated_rc */

/*===========================================================================

FUNCTION snm_reg_connection_initializer

DESCRIPTION
  This function binds a connection initializer to a service option descriptor.
  If an old initializer already exists, it overwrites it.

DEPENDENCIES
  None

RETURN VALUE
  SNM_SUCCESS if initializer registered; SNM_FAILURE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

snm_status_type snm_reg_connection_initializer
(
  word so,
  void ( *initializer )( const caii_srv_cfg_type*, word )
)
{
  word so_index;

  if ( snm_find_so( so, &so_index ) == SNM_FAILURE )
  {
    return SNM_FAILURE;
  }

  snp.so[so_index].connection_initializer = initializer;

  return SNM_SUCCESS;

} /* snm_reg_connection_initializer */

/*===========================================================================

FUNCTION snm_reg_rlp_blob_generator

DESCRIPTION
  This function binds a RLP blob generator to a service option descriptor.
  If an old blob generator already exists, it overwrites it.

DEPENDENCIES
  None

RETURN VALUE
  SNM_SUCCESS if generator registered; SNM_FAILURE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

snm_status_type snm_reg_rlp_blob_generator
(
  word so,
  uint8 (*rlp_blob_generator)(byte* blob)
)
{
  word so_index;

  if (snm_find_so(so, &so_index) == SNM_FAILURE)
  {
    return SNM_FAILURE;
  }

  snp.so[so_index].rlp_blob_generator = rlp_blob_generator;

  return SNM_SUCCESS;

} /* snm_reg_rlp_blob_generator */

/*===========================================================================

FUNCTION snm_reg_qos_functions

DESCRIPTION
  This function allows clients (DS, for now) to register functions to support
  QOS negotiation and generation.

    qos_blob_generator: This function should put the QOS blob being proposed
                        for the SO in qos_blob and return the length of the blob.
                        The size of the array qos_blob is equal to the maximum
                        size of the QOS blob possible as specified in the standard.

    qos_blob_eval: This function should evaluate the blob given by the BS
                   and return alternatives if possible.

        bs_blob_len:     Length of the QOS blob given by BS
        bs_blob:         The QOS blob given by the BS
        can_suggest_alt: Whether the function can suggest alternatives
                         if the BS-given blob is not acceptable.

                         The next two are return arguments and apply only if
                         can_suggest_alt is TRUE and the function wants to
                         propose an alternate blob.

        ret_alt_blob_len: This is used by the function as a return parameter to
                          provide caller with the length of the alternate blob being
                          proposed.
        ret_alt_blob:     This is used by the function as a return parameter to
                          provide caller with the alternate blob being proposed.
                          The size of the array ret_alt_blob is equal to the maximum
                          size of the QOS blob possible as specified in the standard.

        RETURN VALUE: SNM_ACCEPT_CFG to accept the BS provided blob
                      SNM_REJECT_CFG to reject the BS provided blob
                      SNM_ALT_CFG to suggest an alternative
                        (only possible if can_suggest_alt is TRUE and function must put
                         alternative blob info in ret_alt_blob_len and ret_alt_blob.)

DEPENDENCIES
  None.

RETURN VALUE
  SNM_SUCCESS if successfully registered, SNM_FAILURE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

snm_status_type snm_reg_qos_functions
(
  word so,
  byte (*qos_blob_generator) (byte * qos_blob),
  snm_eval_status_type (*qos_blob_eval) (byte    bs_blob_len,
                                         byte  * bs_blob,
                                         boolean can_suggest_alt,
                                         byte  * ret_alt_blob_len,
                                         byte  * ret_alt_blob
                                        )
)
{
  #ifdef FEATURE_IS2000_REL_A
  word so_index;

  if (snm_find_so( so, &so_index) == SNM_FAILURE)
  {
    return SNM_FAILURE;
  }

  snp.so[so_index].qos_blob_generator = qos_blob_generator;
  snp.so[so_index].qos_blob_eval = qos_blob_eval;

  return SNM_SUCCESS;
  #else
  MODEM_1X_UNUSED(so);
  MODEM_1X_UNUSED(qos_blob_generator);
  MODEM_1X_UNUSED(qos_blob_eval);
  return SNM_FAILURE;
  #endif /* FEATURE_IS2000_REL_A */

} /* snm_reg_qos_functions */

/*===========================================================================

FUNCTION snm_reg_sdb_so_omit_status_handler

DESCRIPTION
  This registers a call back function to report back the SDB_SO_OMIT bit.

DEPENDENCIES
  None

RETURN VALUE
  SNM_SUCCESS if status handler registered; SNM_FAILURE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

snm_status_type snm_reg_sdb_so_omit_status_handler
(
  word so,
  void (*sdb_so_omit_status_handler) (boolean sdb_so_omit_status)
)
{
  #ifdef FEATURE_IS2000_REL_A
  word so_index;
  if (snm_find_so(so, &so_index) == SNM_FAILURE)
  {
    return SNM_FAILURE;
  }
  snp.so[so_index].sdb_so_omit_status_handler = sdb_so_omit_status_handler;
  return SNM_SUCCESS;
  #else
  MODEM_1X_UNUSED(so);
  MODEM_1X_UNUSED(sdb_so_omit_status_handler);
  return SNM_FAILURE;
  #endif /* FEATURE_IS2000_REL_A */
}

/*===========================================================================

FUNCTION snm_set_max_for_sups

DESCRIPTION
  This function changes the maximum number of forward supplementals to be used
  with a service option.

DEPENDENCIES
  None.

RETURN VALUE
  SNM_SUCCESS if serivce option exists and number of supplementals are valid;
  SNM_FAILURE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

snm_status_type snm_set_max_for_sups
(
  word  service_option,
  byte  num_sup
)
{
  word so_index;

  /* First make sure service option is in SNM */
  if (snm_find_so( service_option, &so_index ) == SNM_FAILURE)
  {
    /* If not, just return failure */
    return SNM_FAILURE;
  }

  /* Now do a boundary check on the number of supplementals */
  if ( num_sup > CAI_MAX_NUM_SUP )
  {
    return SNM_FAILURE;
  }

  /* Now change the forward mux option to reflect to number of sups. */
  snp.so[so_index].negotiation_pars.max_mux_to_use.forward =
    (num_sup*2) + (word)snp.so[so_index].default_config.forward.rate_set + 1;

  return SNM_SUCCESS;

} /* snm_set_max_mux_options */

/*===========================================================================

FUNCTION snm_set_sch_mux_list

DESCRIPTION
  This function sets the SCH mux list for the given service option number.

DEPENDENCIES
  SNP must be initialized (snm_initialize_snm does it).

RETURN VALUE
  SUCCESS if the mux list was added successfully; otherwise if
  the SO doesn't exists, it returns FAILURE.

SIDE EFFECTS
  None.

===========================================================================*/

snm_status_type snm_set_sch_mux_list
(
  word                 so_number, /* Service option number */
  snm_fwd_rev_type     fwd_rev,   /* Direction of SCH */
  snm_sch_mo_list_type *mo_list   /* Mux options to set */
)
{
   word so_index;

   /* Validate the array bound */
   if (mo_list->num_mos > CAI_MAX_MULT_OPT_SCH)
   {
     return SNM_FAILURE;
   }

   /* Find the entry for the SO */
   if (snm_find_so(so_number, &so_index) == SNM_SUCCESS)
   {
      /* Yes, then set the mux option list */
      REX_ISR_LOCK(&mc_crit_sect);

      if (fwd_rev == SNM_FORWARD)
      {
        snp.so[so_index].negotiation_pars.fwd_sch_mo_list = (*mo_list);
      }
      else /* fwd_rev == SNM_REVERSE */
      {
        snp.so[so_index].negotiation_pars.rev_sch_mo_list = (*mo_list);
      }

      REX_ISR_UNLOCK(&mc_crit_sect);

      return SNM_SUCCESS;
   }
   else
   {
      /* else, precond or find failed */
      return SNM_FAILURE;
   }
} /* snm_set_sch_mux_list */

/*===========================================================================

FUNCTION snm_get_rate_set

DESCRIPTION
  This function is used to obtain the rate set used in forward and reverse
  links for a given service option.

DEPENDENCIES
  None.

RETURN VALUE
  SNM_SUCCESS if SO is located in SNP; otherwise SNM_FAILURE is returned.

SIDE EFFECTS
  None.

===========================================================================*/

snm_status_type snm_get_rate_set
(
  word so_number,
  snm_rate_set_pair_type* rate_set_pair
)
{
  word so_index;

  /* Is it in SNP? */
  if (snm_find_so(so_number, &so_index) == SNM_SUCCESS)
  {
    /* Yes, then retrieve rate sets */
    rate_set_pair->forward = snp.so[so_index].default_config.forward.rate_set;
    rate_set_pair->reverse = snp.so[so_index].default_config.reverse.rate_set;
    return SNM_SUCCESS;
  }
  else
  {
    /* else default to rate set 1 and return failure */
    rate_set_pair->forward = CAI_RATE_9600;
    rate_set_pair->reverse = CAI_RATE_9600;
    return SNM_FAILURE;
  }

} /* snm_get_rate_set */

/*===========================================================================

FUNCTION snm_get_rlp_cap_blob

DESCRIPTION
  Retrieve RLP_BLOB to be used for Capability Info Record.

DEPENDENCIES
  SNP has to be initialized.

RETURN VALUE
  Length of the RLP_CAP_BLOB

SIDE EFFECTS
  none
===========================================================================*/

uint8 snm_get_rlp_cap_blob
(
  uint8 *rlp_blob
)
{
  word so_index;

  /* For now, associate RLP_BLOB with SO=33, this logic may have
     to change a bit once multiple SOs support RLP BLOB type info */
  if (snm_find_so(CAI_SO_PPP_PKT_DATA_3G, &so_index) == SNM_SUCCESS)
  {
    /* The std changed the RLP_CAP_BLOB definition used in CAP IR
       from REL 0 to REL A. From REL A onwards the MS is required to
       send the whole RLP_BLOB in the CAP IR, REL 0 requires only a
       small subset of it , so need to pack this info based on
       p_rev_in_use */
    #ifdef FEATURE_IS2000_REL_A
    if (P_REV_IS_7_OR_GREATER) //lint !e506 !e774
    {
      if (snp.so[so_index].rlp_blob_generator)
      {
        return (snp.so[so_index].rlp_blob_generator(rlp_blob));
      }
      else
      {
        return 0;
      }
    }
    else
    #endif /* FEATURE_IS2000_REL_A */
    {
      /* This is locally generated as defined in REL 0 A2.
         RLP_CAP_BLOB contains 3 bits max_ms_nak_rounds_fwd and 3 bits
         max_ms_nak_rounds_rev. */
      rlp_blob[0] = (dsrlp_get_max_ms_nak_rounds_fwd() << 5) |
                    (dsrlp_get_max_ms_nak_rounds_rev() << 2);//lint !e734
      return 1;
    }
  }
  else
  {
    /* No SO=33, No RLP_BLOB */
    return 0;
  }

} /* snm_get_rlp_cap_blob */

/*===========================================================================

FUNCTION snm_eval_paged_so

DESCRIPTION
  This function evaluates the SO paged by the base station (after translating
  with snm_get_page_response_so).  It does it by looking into SNP and see
  if the SO exists.  If it does, negotiation_info is updated with this SO as
  the seed for negotiation.  If this SO is enabled, the same SO is returned;
  otherwise, the list of alternative SOs will be traversed until the first
  enabled SO is found.  If an alternative does not exist or if they are all
  disabled, CAI_SO_REJ will be returned.

  If the SO is not in SNP, the list of compatible SOs (compatible_service_options)
  of each SO will be scanned.  The first enabled SO containing the evaluated SO
  in its compatible SO list will be returned, which will also be used as the seed.
  If this fails also, the old negotiation mechanism will be used (srv_eval_so
  is called to evaluated the SO).  If this fails also, CAI_SO_REJ is returned.

  This function also initializes negotiation_info to prepare for negotiation
  on the traffic channel.

DEPENDENCIES
  None.

RETURN VALUE
  If SO is supported, the same SO is returned; otherwise an alternate SO is
  returned.  If no alternate SO exists, CAI_SO_REJ is returned.

SIDE EFFECTS
  None.
===========================================================================*/

word snm_eval_paged_so
(
  word paged_so /* The service option number to be evaluated */
)
{
  word wn;
  word so_index, *wlist, alt_so_index = 0;
  word so_to_return = CAI_SO_REJ; /* Initilaize to SO not supported */
  byte con_index;

  /*
     This is the rule here: If the SO is in SNP and it is not disabled, then we
     agree on the SO, and we return the same SO and will use it as the seed.
     If this SO is diabled, we traverse alt_so_list for an alternative to
     suggest.  If we exhausted the list, we have to reject.

     If the SO is not found in SNP, look in compatible_so of each
     SO.  If enabled compatible SO is found, it is returned and will be used
     as the seed.
  */

  /* First do some default initialization */

  for (con_index=0; con_index < MAX_SNM_CON; con_index++)
  {
    negotiation_info.con_list[con_index].max_mux_proposed = FALSE;
    negotiation_info.con_list[con_index].neg_seed =
      negotiation_info.con_list[con_index].beginning_so = CAI_SO_NULL;

    /* Zero out pointer to service option descriptor */
    negotiation_info.con_list[con_index].connection_sod = 0;

    /* Reset con_ref */
    negotiation_info.con_list[con_index].sr_id_valid = FALSE;
    negotiation_info.con_list[con_index].cur_fwd_fch_rc_index = 0;

    negotiation_info.con_list[con_index].cur_fwd_dcch_rc_index = 0;

    negotiation_info.con_list[con_index].fundicated_rc_proposed_chan_mask = SNM_INIT_CHAN_MASK;
    negotiation_info.con_list[con_index].cur_fwd_sch_mux_index =
    negotiation_info.con_list[con_index].cur_rev_sch_mux_index = 0;

#ifdef FEATURE_IS2000_REL_A_SVD
    negotiation_info.con_list[con_index].so_can_be_changed = FALSE;
#endif /* FEATURE_IS2000_REL_A_SVD */
  }

  /* Current config is not valid */
  negotiation_info.current_config_valid = FALSE;

  /* Check if SO is in SNP */
  if (snm_find_so(paged_so, &so_index) == SNM_SUCCESS)
  {
    /* Yes, then used this as the seed. Can use the first in con_list
       as we know there is the beginning of the the Serv Negot. process */
    negotiation_info.con_list[0].neg_seed = paged_so;
    negotiation_info.con_list[0].alt_so_list =
      snp.so[so_index].negotiation_pars.alternate_so;

    /* Check to see whether SO is supported and alternate SO has to used instead. */
    so_to_return = snm_eval_so(paged_so, &alt_so_index);

    /* Paged SO is enabled and support under current P_REV? */
    if (so_to_return == paged_so)
    {
      /* Yes, complete initialization of negotiation_info and return */
      negotiation_info.con_list[0].next_so_index = 0;
      negotiation_info.con_list[0].beginning_so =
      negotiation_info.con_list[0].prev_so      = so_to_return;
    }
    /* Paged SO is not supported but found alternate SO */
    else if (so_to_return != CAI_SO_REJ)
    {
      /* Found an alternative that's enabled, update negotiation_info
         and return it. */
      negotiation_info.con_list[0].next_so_index = alt_so_index + 1;
      negotiation_info.con_list[0].beginning_so =
      negotiation_info.con_list[0].prev_so      = so_to_return;
    }
    else
    {
      /* If SO is not Enabled or can't find one in alt_so_list either,
         initialize beginning_so properly so that if we ever get
         to traffic channel, we know we don't have a SO yet */
      negotiation_info.con_list[0].beginning_so = so_to_return;
    }
  }
  else /* SO is not in SNP, now need to search in compatible_so */
  {
    for (so_index = 0; so_index < snp.total_so; so_index++)
    {
      for (wn = 0, wlist = snp.so[so_index].negotiation_pars.compatible_so;
           wn < MAX_COMP_SO && wlist[wn] != CAI_SO_NULL; wn++ )
      {
        if (((wlist[wn] == paged_so) && (snp.so[so_index].status == SNM_ENABLED)) &&
             snm_p_rev_verified(snp.so[so_index].negotiation_pars.min_p_rev))
        {
          /* Found compatible SO to respond with.  Update negotiation_info
             and return SO. */
          negotiation_info.con_list[0].neg_seed =
          negotiation_info.con_list[0].beginning_so =
            snp.so[so_index].service_option_num;
          negotiation_info.con_list[0].next_so_index = 0;
          negotiation_info.con_list[0].alt_so_list =
            snp.so[so_index].negotiation_pars.alternate_so;
          so_to_return = snp.so[so_index].service_option_num;
          negotiation_info.con_list[0].prev_so = so_to_return;
          break;
        }
      } /* end for Wn... */

      /* Before repeating outer loop, see if compatible SO found */
      if (so_to_return != CAI_SO_REJ)
      {
        break; /* If yes, then break */
      }
    } /* end for so_index... */

    if (so_to_return == CAI_SO_REJ)
    {
      /* If SO is not supported, initialize beginning_so properly so that if we ever get
         to traffic channel, we know we don't have a SO yet */
      negotiation_info.con_list[0].beginning_so = CAI_SO_REJ;
    }
  }

  M1X_MSG( DCP, LEGACY_MED,
    "Eval, return: %d, %d",
    paged_so,
    so_to_return);

  return so_to_return;

} /* snm_eval_paged_so */

/*===========================================================================

FUNCTION snm_eval_so

DESCRIPTION
  This function evaluates the SO specified in the function parameter and returns
  the same SO if it is supported, otherwise a alternative SO is returned.  If there
  is no alternative, CAI_SO_REJ is returned.  Unlike snm_eval_paged_so, the function
  does not affect the internal state of SNM and thus can be called at any time.
  Note 1: If the SO is not defined in SNM, no attempt is made to look for its
  occurrence in the compatible SO lists.
  Note 2: If SO is proposed from alternate_so_list and if valid pointer to the index
  of alternate SO list is passed as input pointer, the funcation will update the
  content of pointer.

DEPENDENCIES
  None.

RETURN VALUE
  See description.

SIDE EFFECTS
  None.
===========================================================================*/

uint16 snm_eval_so
(
  uint16 so,           /* The service option number to be evaluated */
  uint16 *alt_so_index /* Pointer to the index of alternate SO list */
)
{
  uint16 so_index, *so_list, i, return_so = CAI_SO_REJ;

  if ( snm_find_so( so, &so_index) == SNM_FAILURE )
  {
    return CAI_SO_REJ;
  }

  /* If we are here, we found the SO in SNM. */
  if ( snm_so_is_enabled(so) )
  {
    return so;
  }

  /* Service option is disabled, look for an enabled SO in the alt_so_list */
  for (i = 0, so_list = snp.so[so_index].negotiation_pars.alternate_so;
       i < cai_ac_alt_so_max && so_list[i] != CAI_SO_NULL; ++i)
  {
    if ( snm_so_is_enabled(so_list[i]) )
    {
      /* Found an alternative that's enabled, we are done */
      return_so = so_list[i];

      /* Update the index of alternate so list if pointer to it is specified */
      if (alt_so_index != NULL)
      {
         *alt_so_index = i;
      }
      break;
    }
  }

  return return_so;

} /* snm_eval_so */

/*===========================================================================

FUNCTION snm_get_alt_so_list

DESCRIPTION
  This function evaluates a given service option, returning the appropriate
  list of alternative service options.

DEPENDENCIES
  alt_so points to an array of bytes large enough to handle the maximum
  number of alternative service options allowed (max_alt_so).  This
  maximum is the lesser of the two values: max_num_alt_so received from
  ESPM, or the internal array size (defined in cai.h as CAI_AC_ALT_SO_MAX).

  The so_num must have already been checked to be a valid service option.

RETURN VALUE
  Returns the number of alternative service options in the alt_so list.

SIDE EFFECTS
  The values that alt_so point to will be changed to the alternative
  service options.

===========================================================================*/

byte snm_get_alt_so_list
(
  word so_num,
  byte max_alt_so,
  word alt_so[]
)
{
  byte num_alt_so = 0; /* number of elements of the alt_so list */
  word so_index;       /* index into the SNP */
  byte index;          /* index into the alt so list */
  word so_to_include;  /* so number (to reduce number of SNP lookups) */


  /* make sure we do the lesser between given max_alt_so and the number
     of alternative service options we store.  This should never
     happen, it's a sanity check. */
  if (max_alt_so > cai_ac_alt_so_max)
  {
    max_alt_so = cai_ac_alt_so_max;
  }

  if (snm_find_so(so_num, &so_index) == SNM_SUCCESS) /* if SO is in SNP */
  {
    /* go through the alt so list */
    for (index = 0; (index < cai_ac_alt_so_max) && (num_alt_so < max_alt_so); index++)
    {
      /* get the next element of the alt_so list */
      so_to_include = snp.so[so_index].negotiation_pars.alternate_so[index];

      /* if the list is shorter than the maximum length, it ends with NULL */
      if (so_to_include == CAI_SO_NULL)
      {
        break;
      } /* end if (so_to_include) */
      else /*this element is valid */
      {
        /* if it's enabled, include this SO in the list */
        if (snm_so_is_enabled(so_to_include))
        {
          alt_so[num_alt_so++] = so_to_include;

        } /* end if (snm_so_is_enabled) */
      }
    } /* end for (index = 0;) */
  }

  return num_alt_so;

} /* snm_get_alt_so_list */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION snm_convert_alt_so_bitmap

DESCRIPTION
  This function fills the Service Option Bitmap information based on the
  Alternative Service Options.
  Inputs:
    so_num          Primary Service Option number
    num_alt_so      Number of alternative SO's to be translated into bitmap
    alt_so[]        Alternative SO's to be translated into bitmap

  Outputs:
    so_bitmap_ind   Indicates if a bit_map is included, if so the length
    so_group_num    Group number for the primary SO, or if the primary SO
                    does not have a group number (ex, 0x8000), then the SO
                    group of the alternate Service Option is matched.
    so_bitmap       Bitmask of alternative Service Options that are in the
                    same group as so_group_num. See above.
    num_alt_so      Total number of alternative SO's that can not be placed
                    in the SO bitmap
    alt_so[]        Alternative SO's that can not be placed in the SO bitmap

DEPENDENCIES
  This function assumes that number of alternative SO's and the alt_so array
  is properly populated.

RETURN VALUE
  None

SIDE EFFECTS
  Any Alternative Service Option in alt_so_list which either have no Service
  Option group assigned or do not belong to the same Service Option group
  whose bitmap is being included, will be returned in alt_so_list. Total
  number of such alternative SO's will be returned as num_alt_so.

===========================================================================*/

void snm_convert_alt_so_bitmap
(
  word so_num,
  byte *num_alt_so,
  word alt_so[],
  byte *so_bitmap_ind,
  byte *so_group_num,
  uint16 *so_bitmap
)
{
  byte new_num_alt_so = 0;    // number of elements of the alt_so array after
                              // processing the whole list
  byte index;                 // counter index into the alt so
  byte temp_group;
  word so_index;              // index corresponding to the SO

  word main_so_index;         // index for the SO for which info is requested
  byte main_so_group;         // assigned grp # for SO for which info is requested

  *so_bitmap = 0;
  *so_bitmap_ind = SO_BITMAP_SIZE_0;

  if (snm_find_so(so_num, &so_index) == SNM_SUCCESS)
  {
    *so_group_num = snp.so[so_index].negotiation_pars.so_group;

    // Save index & actual group# of the SO for which we want the bitmap
    main_so_index = so_index;
    main_so_group = *so_group_num;

    for (index = 0 ; (index < *num_alt_so) &&
        (index < CAI_AC_ALT_SO_MAX) &&
        (new_num_alt_so < CAI_AC_ALT_SO_MAX) ; index++ )
    {
      /* Get the so index in snp for the alternative so. */
      if (snm_find_so(alt_so[index], &so_index) == SNM_SUCCESS)
      {
        /* Group number for the alternative SO */
        temp_group = snp.so[so_index].negotiation_pars.so_group;

        /* If the primary SO doesn't have a group number,  */
        /* try the group number in the alternate SO list.  */
        if (*so_group_num == CAI_SO_GROUP_UNASSIGNED)
        {
          *so_group_num = temp_group;
        }

        /* See if this alternative SO has a group assigned,
           and if so, see if it belongs to the same SO group as the main SO. */
        if ((temp_group != CAI_SO_GROUP_UNASSIGNED) &&
            (temp_group == *so_group_num))
        {
          /* Now place this alternative SO to the bitmap since we know
             that it has the same group info with the main SO. */
          *so_bitmap = *so_bitmap | snp.so[so_index].negotiation_pars.bitmap_pos;
        }
        else /* that particular alternative SO is not in the same group */
             /* with the main SO or it does not have any group assigned. */
        {
          alt_so[ new_num_alt_so ] = alt_so [ index ] ;
          new_num_alt_so++;
        } /* else */
      } /* alt so in snp. */
    } /* for */

    // Include the original SO in the SOG bitmap if it has an assigned group.
    // In the loop above, if the original SO does not have an assigned group,
    // the function uses the group of the first SO in the alt_so list that does
    // have a group and ones without assigned groups go in the alt_so list to
    // be returned. But if the original SO does not have an assigned group,
    // presumably it should not be listed in alt_so since it is "the" SO.
    if (main_so_group != CAI_SO_GROUP_UNASSIGNED)
    {
      *so_bitmap = *so_bitmap | snp.so[main_so_index].negotiation_pars.bitmap_pos;
    }
  } /* main SO in snp. */

  /* We've placed the alt SO's that are not in the same group and
     total number of alt so needs to be updated. */
  *num_alt_so =  new_num_alt_so;

  /* At this point alt so's that share the same group info
     are placed, so now from so_bitmap deduce the so_bitmap_ind */
  if (((*so_bitmap) & 0xF0) != 0)
  {
    *so_bitmap_ind = SO_BITMAP_SIZE_8 ;
  }
  else if (((*so_bitmap) & 0x0F) != 0)
  {
    *so_bitmap_ind = SO_BITMAP_SIZE_4 ;
  }
  else /* no alt so placed in the bitmap */
  {
    *so_bitmap_ind = SO_BITMAP_SIZE_0 ;
  }

  /* so_bitmap_ind is filled */
  /* so_bitmap is set */
  /* so_group is set at top. */

} /* snm_convert_alt_so_bitmap */

/*===========================================================================

FUNCTION snm_get_so_grp_num

DESCRIPTION
  This function returns the group number of a given SO.

DEPENDENCIES
  SO must be registered with SNM.

RETURN VALUE
  Group number for the SO if SO is found, else 0.

SIDE EFFECTS
  None
===========================================================================*/

uint8 snm_get_so_grp_num
(
  word so
)
{
  word so_index;

  if (snm_find_so(so, &so_index) == SNM_SUCCESS)
  {
    return snp.so[so_index].negotiation_pars.so_group;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_FATAL,
      "Unknown SO");
  }

  return CAI_SO_GROUP_UNASSIGNED;

} /* snm_get_so_grp_num */
#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================

FUNCTION snm_get_pref_fundicated_RCs

DESCRIPTION
  This function returns the preferred RC for the forward and reverse fundicated
  channel. This preference is sent back in the page resp or the origination
  message. The SNM has a RC list stored for each SO and it returns the first
  in the list.  For SOs not managed by SNM, it calls a corresponding function
  in SRV.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void snm_get_pref_fundicated_RCs
(
  word                  so,          /* The SO for which pref RCs are needed */
  cai_radio_config_type *fwd_rc_ptr, /* Storage for returned fwd RC */
  cai_radio_config_type *rev_rc_ptr  /* Storage for return rev RC */
)
{
  word so_index;
  uint8 i, j;
  boolean found = FALSE;
  cai_radio_config_type temp_rc;

  if (snm_find_so(so, &so_index) == SNM_SUCCESS)
  {
    /* Let's first try to see if we can get the cached pref RC from NV. If
       that makes sense for this SO, let's use that */
    if (negotiation_info.pref_rc_valid)
    {
      for (i = 0; i < snp.so[so_index].negotiation_pars.fwd_rc.num_of_rc; i++)
      {
        if (snp.so[so_index].negotiation_pars.fwd_rc.list[i] ==
            negotiation_info.pref_fundicated_rc)
        {
          *fwd_rc_ptr = negotiation_info.pref_fundicated_rc;

          /* Update the index so that Serv negotiation begins from this entry
             We can use [0] here as this is for the first and only call for now */
          negotiation_info.con_list[0].cur_fwd_fch_rc_index  = i;
          negotiation_info.con_list[0].cur_fwd_dcch_rc_index = i;

          for (j = 0; j < snp.so[so_index].negotiation_pars.rev_rc.num_of_rc; j++)
          {
            temp_rc = snp.so[so_index].negotiation_pars.rev_rc.list[j];

            if ((RC_CLASS(*fwd_rc_ptr) == RC_CLASS(temp_rc)) &&
                (RATE_SET_OF_FWD_RC(*fwd_rc_ptr) == RATE_SET_OF_REV_RC(temp_rc)) &&
                (FWD_RC_SUPPORTS_1X_ADV(*fwd_rc_ptr) == REV_RC_SUPPORTS_1X_ADV(temp_rc)))
            {
              *rev_rc_ptr = temp_rc;
              found = TRUE;
              break;
            }
          }
          break;
        }
      }
    }

    if (!found)
    {
      /* Take the pref RCs from the SO table. The first in the list is
         considered the most preferred. */
      *fwd_rc_ptr = snp.so[so_index].negotiation_pars.fwd_rc.list[0];
      *rev_rc_ptr = snp.so[so_index].negotiation_pars.rev_rc.list[0];
      negotiation_info.con_list[0].cur_fwd_fch_rc_index  = 0;

      negotiation_info.con_list[0].cur_fwd_dcch_rc_index = 0;
    }
  }
  else
  {
    /* If SO is not in SNM try it in SRV in the good old way */
    srv_get_pref_fundicated_RCs(so, fwd_rc_ptr, rev_rc_ptr);
  }

} /* snm_get_pref_fundicated_RCs */

/*===========================================================================

FUNCTION snm_get_pref_chan_ind_for_so

DESCRIPTION
  This function gets the preferred CH_IND and EXT_CH_IND for the specified SO.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715, bs_rev_pdch_supported) */
void snm_get_pref_chan_ind_for_so
(
  uint16 so,                      /* Service option for which CH_IND and
                                     EXT_CH_IND to be set */
  uint8 *ch_ind_ptr               /* Pointer to CH_IND */
)
{
  uint16 so_index;
  snm_chan_mask_type so_chan_mask;
  *ch_ind_ptr = CAI_CHIND_RESERVED_1;  /*lint !e641 */

  if (snm_find_so(so, &so_index) == SNM_FAILURE)
  {
    /* SO not supported, be conversative use FCH */
    M1X_MSG( DCP, LEGACY_MED,
      "Service Option %d not found in SNM",
      so);
    *ch_ind_ptr = (uint8) CAI_CHIND_FCH_ONLY;
    return;
  }

  /* Figure out channels supported by SO */
  so_chan_mask = snm_gen_so_chan_mask(so);

  /* DCCH is supported, use DCCH */
  if (P_REV_IS_6_OR_GREATER &&  mccap_item_is_supported(MCCAP_DF_DCCH) &&
      (so_chan_mask & SNM_DCCH))
  {
    *ch_ind_ptr = (uint8) CAI_CHIND_DCCH_ONLY;
    return;
  }

  /* Use FCH */
  *ch_ind_ptr = (uint8) CAI_CHIND_FCH_ONLY;

} /* snm_get_pref_chan_ind_for_so */

/*===========================================================================

FUNCTION snm_verify_chan_mask_for_so

DESCRIPTION
  This function verifies if the channel mask specified is supported by the
  specified service option.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if supported; FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/

boolean snm_verify_chan_mask_for_so
(
  mcc_fwd_chan_mask_type fwd_chan_mask,
  mcc_rev_chan_mask_type rev_chan_mask,
  uint16                 so
)
{
  uint16 so_index;
  snm_chan_mask_type chan_mask_not_verified = SNM_INIT_CHAN_MASK;
  snm_chan_mask_type so_chan_mask = SNM_INIT_CHAN_MASK;

  /* Figure out which channels to be verified */
  if ((fwd_chan_mask & MCC_F_FCH) || (rev_chan_mask & MCC_R_FCH))
  {
    chan_mask_not_verified |= SNM_FCH;
  }

  if ((fwd_chan_mask & MCC_F_DCCH) || (rev_chan_mask & MCC_R_DCCH))
  {
    chan_mask_not_verified |= SNM_DCCH;
  }

  if ((fwd_chan_mask & MCC_F_PDCH) || (rev_chan_mask & MCC_R_PDCH))
  {
    chan_mask_not_verified |= SNM_PDCH;
  }

  if (chan_mask_not_verified == SNM_INIT_CHAN_MASK)
  {
    /* None of legal channel specified, should never happen */
    return FALSE;
  }

  /* Check if MS capability supports DCCH */
  if ((chan_mask_not_verified & SNM_DCCH) &&
      (!P_REV_IS_6_OR_GREATER || !mccap_item_is_supported(MCCAP_DF_DCCH)))
  {
    /* DCCH not supported */
    return FALSE;
  }

#ifdef FEATURE_IS2000_REL_C
  /* Check if MS capability supports F-PDCH */
  if ((chan_mask_not_verified & SNM_PDCH) &&
      (!P_REV_IS_9_OR_GREATER || !mccap_is_for_pdch_supported()))
  {
    /* F-PDCH not supported */
    return FALSE;
  }
#endif /* FEATURE_IS2000_REL_C */

  if (snm_find_so(so, &so_index) == SNM_FAILURE)
  {
    /* If SO is not in SNM (possibly CAI_SO_REJ), not much we can do other than
       accept it. */
    return TRUE;
  }
  else
  {
    /* Figure out allowed physical channels for the SO */
    so_chan_mask = snm_gen_so_chan_mask(so);

    /* Check if at least one of channels assigned is acceptable */
    if (chan_mask_not_verified & so_chan_mask)
    {
      return TRUE;
    }
  }

  /* If we reached here, none of channels assigned are supported */
  return FALSE;

} /* snm_verify_chan_mask_for_so */

/*===========================================================================

FUNCTION snm_orig_init

DESCRIPTION
  Performs some SNM initialization required for originations.

DEPENDENCIES
  None

RETURN VALUE
  SNM_SUCCESS if successfully initialized; SNM_FAILURE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

snm_status_type snm_orig_init
(
  word originating_so
)
{
  word so_index;
  byte con_index;

  for (con_index=0; con_index < MAX_SNM_CON; con_index++)
  {
    negotiation_info.con_list[con_index].max_mux_proposed = FALSE;
    negotiation_info.con_list[con_index].neg_seed =
      negotiation_info.con_list[con_index].beginning_so = CAI_SO_NULL;
    /* Zero out pointer to service option descriptor */
    negotiation_info.con_list[con_index].connection_sod = 0;
    /* Reset con_ref */
    negotiation_info.con_list[con_index].sr_id_valid = FALSE;

    /* For the first connection, we handle it below */
    if (con_index != 0)
    {
      negotiation_info.con_list[con_index].cur_fwd_fch_rc_index = 0;
      negotiation_info.con_list[con_index].cur_fwd_dcch_rc_index = 0;
    }
    negotiation_info.con_list[con_index].fundicated_rc_proposed_chan_mask = SNM_INIT_CHAN_MASK;
    negotiation_info.con_list[con_index].cur_fwd_sch_mux_index =
    negotiation_info.con_list[con_index].cur_rev_sch_mux_index = 0;

#ifdef FEATURE_IS2000_REL_A_SVD
    negotiation_info.con_list[con_index].so_can_be_changed = FALSE;
#endif /* FEATURE_IS2000_REL_A_SVD */
  }


  /* If SO not in SNP, return failure */
  if (snm_find_so(originating_so, &so_index) == SNM_FAILURE)
  {
    /* CAI_SO_NULL in seed indicates SO not supported by SNM */
    negotiation_info.con_list[0].neg_seed = CAI_SO_NULL;
    negotiation_info.con_list[0].connection_sod = 0;
    return SNM_FAILURE;
  }

  /* Initialize negotiation info.  For an origination, seed and beginning SO
     are always the same as the originating SO.
     Note: it is assumed here that the SO is enabled (verified earlier in
     access channel). */
  negotiation_info.con_list[0].neg_seed     =
  negotiation_info.con_list[0].beginning_so = originating_so;
  negotiation_info.con_list[0].next_so_index = 0;
  negotiation_info.con_list[0].alt_so_list =
    snp.so[so_index].negotiation_pars.alternate_so;

  /* Zero out pointer to service option descriptor */
  negotiation_info.con_list[0].connection_sod = 0;

  /* proposed_config, curent_config, and pending_config are initialized in
    snm_initialize_snm and don't change afterwards. */

  /* Since we have not proposed anything, max mux has not been proposed */
  negotiation_info.con_list[0].max_mux_proposed = FALSE;

  /* Current config is not valid */
  negotiation_info.current_config_valid = FALSE;

  if (!P_REV_IS_6_OR_GREATER)
  {
    /* If p_rev_in_use is 6 or greater this value would be
       initialized accoridng to the pref RC in NV */
    negotiation_info.con_list[0].cur_fwd_fch_rc_index = 0;
    negotiation_info.con_list[0].cur_fwd_dcch_rc_index = 0;
  }
  negotiation_info.con_list[0].fundicated_rc_proposed_chan_mask = SNM_INIT_CHAN_MASK;
  negotiation_info.con_list[0].cur_fwd_sch_mux_index =
  negotiation_info.con_list[0].cur_rev_sch_mux_index = 0;
  negotiation_info.con_list[0].prev_so = originating_so;

  /* handle sr_id initialization */
  if (P_REV_IS_6_OR_GREATER)
  {
#ifdef FEATURE_IS2000_REL_A
    negotiation_info.con_list[0].sr_id = cdma.sr_id;
#else
    negotiation_info.con_list[0].sr_id = CAI_SR_ID_1;
#endif /* FEATURE_IS2000_REL_A */

    negotiation_info.con_list[0].sr_id_valid = TRUE;
  }

  return SNM_SUCCESS;

} /* snm_orig_init */

/*===========================================================================

FUNCTION snm_get_neg_index_for_sr_id

DESCRIPTION
  To return the stored negotiation info index for a given connection.

  If the sr_id is not to be used(prev < 6), it returns the first valid entry in
  the database. If the sr_id is to be matched, do the following:

  (a) Loop through the valid Negotiation_info array entries:
      If the entry(m)'s sr_id matches the given argument, return TRUE and m.

  (b) If we didn't find a match, but there is a valid entry (n) without a
      sr_id in the database, return TRUE and n.

      [ (b) happens when the MS does not initiate Serv Neg and waits for
            a SRQM or SCM or G/UHDM. Subsequent passes of Service Negotiation
            should cause this function to succeed in (a). ]

DEPENDENCIES
  In the negotiation_info's connection array, invalid entries should
  have beginning_so initialized to CAI_SO_NULL.

RETURN VALUE
  TRUE if connection found and 'index' corresponds to the array index
  FALSE if connection not found

SIDE EFFECTS
  None.
===========================================================================*/

static boolean snm_get_neg_index_for_sr_id
(
  boolean use_sr_id,
  byte query_sr_id,
  byte *ret_index
)
{
  boolean found_entry_with_no_sr_id = FALSE;
  byte    i;

  M1X_MSG( DCP, LEGACY_MED,
    "Searching neg info based on use_sr_id:%d, query_sr_id:%d",
    use_sr_id, query_sr_id );

  for (i=0; i< MAX_SNM_CON; i++)
  {
    if (negotiation_info.con_list[i].beginning_so == CAI_SO_NULL)
    {
      continue;
    }

    /* This is a valid entry */

    /* If we don't have to query based on sr_id, we should just return now. */
    if (!use_sr_id)
    {
      *ret_index = i;
      return TRUE;
    }

    if (negotiation_info.con_list[i].sr_id_valid)
    {
      if (negotiation_info.con_list[i].sr_id == query_sr_id)
      {
        /* Connection found in negotiation_info, return */
        *ret_index = i;
        return TRUE;
      }
    }
    else
    {
      /* No sr_id in entry, this must be the case where Negotiation
         starts for the first connection after a page. */
      found_entry_with_no_sr_id = TRUE;
      *ret_index = i;
    }

  } /* end for */

  /* We didn't find a matching sr_id.
     Handle the case when we have a valid entry with no sr_id. This
     reflects the situation where Negotiation begins for the first connection
     after a page. */
  if (found_entry_with_no_sr_id)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Did not find a matching sr_id.. starting a fresh neg on new sr_id");
    /* Store the con_ref (in *ret_index) in our database and return TRUE */
    negotiation_info.con_list[*ret_index].sr_id = query_sr_id;
    negotiation_info.con_list[*ret_index].sr_id_valid = TRUE;
    return TRUE;
  }

  /* No entries with matching sr_id or "no sr_id" */
  return FALSE;

} /* snm_get_neg_index_for_sr_id */

/*===========================================================================

FUNCTION snm_get_free_neg_index

DESCRIPTION
  Returns the index of a free entry in the negotiation_info connection array.

DEPENDENCIES
  In the negotiation_info's connection array, invalid entries should
  have beginning_so initialized to CAI_SO_NULL.

RETURN VALUE
  TRUE if free entry present, and the index value
  FALSE if there are no more free entries.

SIDE EFFECTS
  None.
===========================================================================*/

static boolean snm_get_free_neg_index
(
  byte *ret_index
)
{
  byte    i;

  for (i=0; i< MAX_SNM_CON; i++)
  {
    if (negotiation_info.con_list[i].beginning_so == CAI_SO_NULL)
    {
      /* Free entry found, initialize and return */
      negotiation_info.con_list[i].beginning_so = CAI_SO_REJ;
      *ret_index = i;
      return TRUE;
    }
  }

  /* No entry found */
  return FALSE;

} /* snm_get_free_neg_index */

/*===========================================================================

FUNCTION snm_is_rc_class2_required

DESCRIPTION
  This function evalutes the specified forward and reverse SCR mask to
  see if RC class 2 is required.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if RC class 2 is required. Otherwise, FALSE.

SIDE EFFECTS
  None

===========================================================================*/

static boolean snm_is_rc_class2_required
(
  snm_chan_mask_type fwd_scr_chan_mask,
  snm_chan_mask_type rev_scr_chan_mask
)
{
  boolean rc_class2_required = FALSE;

  /* DCCH assignment, must be RC class 2 */
  if ((fwd_scr_chan_mask & SNM_DCCH) || (rev_scr_chan_mask & SNM_DCCH))
  {
    rc_class2_required = TRUE;
  }

  return rc_class2_required;

} /* snm_is_rc_class2_required */

/*===========================================================================

FUNCTION snm_gen_fundicated_rc

DESCRIPTION
  Proposes the fundicated RC of a SO connection in a configuration.

DEPENDENCIES
  If only reverse link RC is to be generated, forward RC must already be set
  to a valid value.


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static boolean snm_gen_fundicated_rc
(
  snm_so_descriptor_type *so_info,
    /* The SO descriptor to evaluate against */
  byte snm_con_index,
    /* Connection index in negotiation_info.con_list[] */
  snm_fwd_rev_type chan_link_mask,
    /* Forward and/or reverse link */
  snm_chan_mask_type rc_chan_mask,
    /* Indicates the channel's RC to be generated */
  snm_chan_mask_type fwd_scr_chan_mask,
    /* The fwd SCR mask that indicates the channel considered for evaluation */
  snm_chan_mask_type rev_scr_chan_mask,
    /* The rev SCR mask that indicates the channel considered for evaluation */
  uint8 rate_set,
    /* Indicates the rate set of RC to be generated */
  uint8 *fwd_rc,
     /* Proposed forward RC to be stored */
  uint8 *rev_rc
     /* Proposed reverse RC to be stored */
)
{
  uint8 i, index, *rc_index_ptr;
  int num_iterations;
  boolean rc_found = FALSE;
  boolean rc_class2_required;

  /* Figure out if RC Class 2 is required */
  rc_class2_required = snm_is_rc_class2_required(fwd_scr_chan_mask, rev_scr_chan_mask);

  /* *******************
  ** Forward RC proposal
  ** ******************* */
  if (chan_link_mask & SNM_FORWARD)
  {
    /* Specified forward channel not assigned, use 0 for RC value */
    if (!(fwd_scr_chan_mask & rc_chan_mask))
    {
      *fwd_rc = 0;
      rc_found = TRUE;
    }
    /* Specified forward channel assigned, propose RC value from RC list */
    else
    {
      rc_found = FALSE;
      num_iterations = 0;

      /* Propose entry from RC list */
      for (i=0; !rc_found &&
                (num_iterations < so_info->negotiation_pars.fwd_rc.num_of_rc) &&
                (i < so_info->negotiation_pars.fwd_rc.num_of_rc); num_iterations++, i++)
      {
        /* Use DCCH RC index */
        if (rc_chan_mask == SNM_DCCH)
        {
          rc_index_ptr =
            &negotiation_info.con_list[snm_con_index].cur_fwd_dcch_rc_index;
        }
        else
        /* Use FCH RC index */
        {
          rc_index_ptr =
            &negotiation_info.con_list[snm_con_index].cur_fwd_fch_rc_index;
        }

        index = *rc_index_ptr;

        /* Check RC class (and rate set) criteria */
        if ((!rc_class2_required ||
             (RC_CLASS(so_info->negotiation_pars.fwd_rc.list[index]) > 1)) &&
            ((rate_set == SNM_RESERVED)  /* Any rate set is fine */ ||
             (rate_set == RATE_SET_OF_FWD_RC(so_info->negotiation_pars.fwd_rc.list[index]))))
        {
          /* Found compatible RC */
          *fwd_rc = so_info->negotiation_pars.fwd_rc.list[index];  /*lint !e641 */
          /* Incremant index to next RC */
          *rc_index_ptr = (++index) % so_info->negotiation_pars.fwd_rc.num_of_rc;
          rc_found = TRUE;
        }
        else
        {
          /* Skip to the next RC list entry */
          index = (index + 1) % so_info->negotiation_pars.fwd_rc.num_of_rc;
        }
      }

      if (!rc_found)
      {
        /* Did not find a compatible forward RC, exit */
        return rc_found;
      }
    }
  }

  /* *******************
  ** Reverse RC proposal
  ** ******************* */
  if (chan_link_mask & SNM_REVERSE)
  {
    uint8 temp_rc = *fwd_rc;

    /* Specified reverse channel not assigned, use 0 for RC value */
    if (!(rev_scr_chan_mask & rc_chan_mask))
    {
      *rev_rc = 0;
      rc_found = TRUE;
    }
    /* Propose RC value from RC list */
    else
    {
      /* Specified forward channel not assigned. Find a compatible forward RC
         as reference for reverse RC proposal */
      if (!(fwd_scr_chan_mask & rc_chan_mask))
      {
        rc_found = FALSE;
        num_iterations = 0;

        for (i=0; !rc_found &&
                  (num_iterations < so_info->negotiation_pars.fwd_rc.num_of_rc) &&
                  (i < so_info->negotiation_pars.fwd_rc.num_of_rc); num_iterations++, i++)
        {

          /* Use DCCH RC index */
          if (rc_chan_mask == SNM_DCCH)
          {
            rc_index_ptr = &negotiation_info.con_list[snm_con_index].cur_fwd_dcch_rc_index;
          }
          else

          /* Use FCH RC index */
          {
            rc_index_ptr = &negotiation_info.con_list[snm_con_index].cur_fwd_fch_rc_index;
          }

          index = *rc_index_ptr;

          /* Check RC class (and rate set) criteria */
          if ((!rc_class2_required ||
               (RC_CLASS(so_info->negotiation_pars.fwd_rc.list[index]) > 1)) &&
              ((rate_set == SNM_RESERVED)  /* Any rate set is fine */ ||
               (rate_set == RATE_SET_OF_FWD_RC(so_info->negotiation_pars.fwd_rc.list[index]))))
          {
            /* Found compatible forward RC */
            temp_rc = so_info->negotiation_pars.fwd_rc.list[index];  /*lint !e641 */
            /* Incremant index to next RC */
            *rc_index_ptr = (++index) % so_info->negotiation_pars.fwd_rc.num_of_rc;
            rc_found = TRUE;
          }
          else
          {
            /* Skip to the next RC list entry */
            index = (index + 1) % so_info->negotiation_pars.fwd_rc.num_of_rc;
          }
        }

        if (!rc_found)
        {
          /* Did not find a compatible forward RC, exit */
          return rc_found;
        }
      }

      rc_found = FALSE;

      /* Now pick a compatible RC from the reverse RC list */
      for (i = 0; !rc_found &&
                  (i < so_info->negotiation_pars.rev_rc.num_of_rc); i++)
      {
        /* Check RC class and rateset criteria */
        if ((RC_CLASS(so_info->negotiation_pars.rev_rc.list[i]) ==
             RC_CLASS(temp_rc)) && /*lint !e641 */
            (RATE_SET_OF_REV_RC(so_info->negotiation_pars.rev_rc.list[i]) ==
             RATE_SET_OF_FWD_RC(temp_rc)))
        {
          /* Found compatible RC */
          *rev_rc = so_info->negotiation_pars.rev_rc.list[i];  /*lint !e641 */
          rc_found = TRUE;
        }
      }
    }
  }

  return rc_found;

} /* snm_gen_fundicated_rc */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION snm_get_max_sch_rate_from_mux

DESCRIPTION
  This fucntion computes the max SCH rate corresponding to a given
  SCH mux option. This function works only for MUX PDU Type 3.

DEPENDENCIES
  None

RETURN VALUE
  Max SCH rate supported by the MS for this SCH mux option.

SIDE EFFECTS
  None
===========================================================================*/

static byte snm_get_max_sch_rate_from_mux
(
  word sch_mux /* SCH mux option */
)
{
  uint8 rate,i;

  if (sch_mux == 0xf20)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Wrong function used for rate calculation for 0xf20 mux");
    return 0;
  }

  /* First extract rate in ??X from sch_mux */
  /* First handle the special case of MUX = 3 or 4, i.e. 1X SCH */
  if (sch_mux <= 0x4)  // Note:Mux 0 - 2 are invalid and won't even get here
  {
    rate = 1; /* 1X for mux 3 or 4 */
  }
  else
  {
    /* Compute the rate in 2 stages */
    rate = sch_mux >> 2;                     //lint !e734 loss of precision intended
    /* Now upper 2 bits of rate is single/double-size PDU, and lower 6 bits is number of
       data blocks. */
    rate = ((rate >> 6) + 1)*(rate & 0x3f); /* Now rate in in ??X */
  }

  for (i=0; i<CAI_SCH_RATE_RS1_INVALID_RS2_72X; i++)  /*lint !e641 */
  {
    if ((sch_mux & 1) && (i<CAI_MAX_SCH_RATE_RS1)) /*lint !e641 */ /* Rate set 1 */
    {
      if (snm_rs1_rate_table[i] == rate)
      {
        return i;
      }
    }
    else /* Rate set 2 */
    {
      if (snm_rs2_rate_table[i] == rate)
      {
        return i;
      }
    }
  }

  /* We should never be here, just return 1X */
  return 0;

}
#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================

FUNCTION snm_populate_fwd_sch_cfg

DESCRIPTION
  Based on the SO descriptor and the negotiation_info state, populates the
  fields of the F-SCH channel

DEPENDENCIES
  This function should be called only if P_REV_IN_USE_IS_6_OR_GREATER and
  we know for sure SCHs are supported by DS and the MSM.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void snm_populate_fwd_sch_cfg
(
  caii_srv_cfg_type *cfg_ptr,
    /* The config ptr to be written to */
  snm_so_descriptor_type *so_info,
    /* The SO descriptor to evaluate against */
  byte con_index,
    /* index in to negotiation_info.con_list[] */
  snm_chan_mask_type fwd_scr_chan_mask,
    /* The fwd SCR mask that indicates the channel to be used for evaluation */
  snm_chan_mask_type  rev_scr_chan_mask /*lint -esym(715, rev_scr_chan_mask)*/
    /* The rev SCR mask that indicates the channel to be used for evaluation */
)
{
  boolean found_mux;
  uint8 i; /* Loop Counter */
  caii_for_sch_spec_fields_type for_sch_fields;
  uint8 index = negotiation_info.con_list[con_index].cur_fwd_sch_mux_index;

  /* This check should have been made before but check anyway */
  if (!so_info->negotiation_pars.fwd_sch_mo_list.num_mos)
  {
    cfg_ptr->for_sch_cc_incl = FALSE;
    /* Nothing more to do here, Just return */
    return;
  }

  cfg_ptr->for_sch_cc_incl = TRUE;

  /* The following would change if we support 2 SCHs */
  cfg_ptr->num_for_sch = 1;
  cfg_ptr->for_sch[0].for_sch_id = 0;
  cfg_ptr->for_sch[0].for_sch_mux =
    so_info->negotiation_pars.fwd_sch_mo_list.list[index];

  if (fwd_scr_chan_mask & SNM_FCH)
  {
    cfg_ptr->for_sch[0].sch_chn_cfg.sch_rc = cfg_ptr->for_fch_rc;
  }
  else
  if (fwd_scr_chan_mask & SNM_DCCH)
  {
    cfg_ptr->for_sch[0].sch_chn_cfg.sch_rc = cfg_ptr->for_dcch_rc;
  }
  else
  {
    cfg_ptr->for_sch[0].sch_chn_cfg.sch_rc = CAI_RC_3; /*lint !e641 */
  }

  (void)mccap_get_ch_specific_fields(MCCAP_FWD_SCH, &for_sch_fields);

  cfg_ptr->for_sch[0].sch_chn_cfg.coding = (uint8)mccap_get_preferred_sch_coding();

#ifdef FEATURE_IS2000_REL_A
  if (cfg_ptr->for_sch[0].for_sch_mux == 0xf20)
  {
    if (cfg_ptr->for_sch[0].sch_chn_cfg.sch_rc == CAI_RC_3)  /*lint !e641 */
    {
       cfg_ptr->for_sch[0].sch_chn_cfg.sch_rc = CAI_RC_4;    /*lint !e641 */
    }
  }
#endif /* FEATURE_IS2000_REL_A */

  /* Check for RC, mux consistency but do this ONLY if mux is not above 10 or
    MUX option is tied to a particular rate set, Eg. 0xf20 is not tied to
    any rate set */

  if (((cfg_ptr->for_sch[0].for_sch_mux <= CAI_MUX16) ||
       (cfg_ptr->for_sch[0].for_sch_mux & CAI_MO_RATE_SET_MASK ))
      && (RATE_SET_OF_FWD_RC(cfg_ptr->for_sch[0].sch_chn_cfg.sch_rc) !=
          (MUX_TO_RATE_SET(cfg_ptr->for_sch[0].for_sch_mux) & 1))
     )
  {
    /* The SCH RC we proposed is not matching the SCH mux option, so go thro'
       the mux list and propose one that matches */
    found_mux = FALSE;
    for (i=index+1; (i<so_info->negotiation_pars.fwd_sch_mo_list.num_mos)
          &&(i<CAI_MAX_MULT_OPT_SCH); i++)
    {
      if ((so_info->negotiation_pars.fwd_sch_mo_list.list[i] & CAI_MO_RATE_SET_MASK )
          && (RATE_SET_OF_FWD_RC(cfg_ptr->for_sch[0].sch_chn_cfg.sch_rc) !=
              (MUX_TO_RATE_SET(so_info->negotiation_pars.fwd_sch_mo_list.list[i]) & 1)))
      {
        continue;
      }

      cfg_ptr->for_sch[0].for_sch_mux = so_info->negotiation_pars.fwd_sch_mo_list.list[i];
      found_mux = TRUE;
      index = i;
      break;
    }

    if (!found_mux)
    {
      /* Okay we couldn't find RCs and mux consistent with each other, do
         not propose SCH */
      cfg_ptr->for_sch_cc_incl = FALSE;
      M1X_MSG( DCP, LEGACY_ERROR,
        "F-SCH not proposed because of RC/MUX inconsistency");
      return;
    }
  }

#ifdef FEATURE_IS2000_REL_A
  if (P_REV_IS_7_OR_GREATER)
  {
    cfg_ptr->for_sch[0].sch_chn_cfg.frame_40_used =
      for_sch_fields.for_sch_recs[0].for_frame_40_supported;
    cfg_ptr->for_sch[0].sch_chn_cfg.frame_80_used =
      for_sch_fields.for_sch_recs[0].for_frame_80_supported;

    if (cfg_ptr->for_sch[0].for_sch_mux == 0xf20)
    {
      cfg_ptr->for_sch[0].sch_chn_cfg.max_rate =
        for_sch_fields.for_sch_recs[0].for_max_rate;
    }
    else
    {
      cfg_ptr->for_sch[0].sch_chn_cfg.max_rate =
        snm_get_max_sch_rate_from_mux(cfg_ptr->for_sch[0].for_sch_mux);
    }
  }
#endif /* FEATURE_IS2000_REL_A */

  /* If we are here, it means for_sch_cc_incl is 1 */
  negotiation_info.con_list[con_index].cur_fwd_sch_mux_index = (index+1) %
    so_info->negotiation_pars.fwd_sch_mo_list.num_mos;

} /* snm_populate_fwd_sch_cfg */

/*===========================================================================

FUNCTION snm_populate_rev_sch_cfg

DESCRIPTION
  Based on the SO descriptor and the negotiation_info state, populates the
  fields of the R-SCH channel

DEPENDENCIES
  This function should be called only if P_REV_IN_USE_IS_6_OR_GREATER and
  we know for sure SCHs are supported by DS and the MSM.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void snm_populate_rev_sch_cfg
(
  caii_srv_cfg_type *cfg_ptr,
    /* The config ptr to be written to */
  snm_so_descriptor_type* so_info,
    /* The SO descriptor to evaluate against */
  byte con_index,
    /* index in to negotiation_info.con_list[] */
  snm_chan_mask_type fwd_scr_chan_mask, /*lint -esym(715, fwd_scr_chan_mask)*/
    /* The fwd SCR mask that indicates the channel to be used for evaluation */
  snm_chan_mask_type rev_scr_chan_mask
    /* The rev SCR mask that indicates the channel to be used for evaluation */
)
{
  boolean found_mux; /* For mux search in a loop */
  uint8 i;           /* Loop Counter */
  caii_rev_sch_spec_fields_type rev_sch_fields;
  uint8 index = negotiation_info.con_list[con_index].cur_rev_sch_mux_index;

  /* This check should have been made before but check anyway */
  if (!so_info->negotiation_pars.rev_sch_mo_list.num_mos)
  {
    cfg_ptr->rev_sch_cc_incl = FALSE;
    /* Nothing more to do here, Just return */
    return;
  }

  cfg_ptr->rev_sch_cc_incl = TRUE;

  /* The following would change if we support 2 SCHs */
  cfg_ptr->num_rev_sch = 1;
  cfg_ptr->rev_sch[0].rev_sch_id = 0;
  cfg_ptr->rev_sch[0].rev_sch_mux =
    so_info->negotiation_pars.rev_sch_mo_list.list[index];

  if (rev_scr_chan_mask & SNM_FCH)
  {
    cfg_ptr->rev_sch[0].sch_chn_cfg.sch_rc = cfg_ptr->rev_fch_rc;
  }
  else if (rev_scr_chan_mask & SNM_DCCH)
  {
    cfg_ptr->rev_sch[0].sch_chn_cfg.sch_rc = cfg_ptr->rev_dcch_rc;
  }
  else
  {
    cfg_ptr->rev_sch[0].sch_chn_cfg.sch_rc = CAI_RC_3;   /*lint !e641 */
  }

  (void)mccap_get_ch_specific_fields(MCCAP_REV_SCH, &rev_sch_fields);

  cfg_ptr->rev_sch[0].sch_chn_cfg.coding = (uint8)mccap_get_preferred_sch_coding();

  /* Check SCH mux against RC only if MUX option implies a rate set,
     for eg. 0xf20 can work with any rate set */
  if (((cfg_ptr->rev_sch[0].rev_sch_mux <= CAI_MUX16) ||
       (cfg_ptr->rev_sch[0].rev_sch_mux & CAI_MO_RATE_SET_MASK))
      && (RATE_SET_OF_REV_RC(cfg_ptr->rev_sch[0].sch_chn_cfg.sch_rc) !=
          (MUX_TO_RATE_SET(cfg_ptr->rev_sch[0].rev_sch_mux) & 1)))
  {
    /* The SCH RC we proposed is not matching the SCH
        mux option, so go thro' the mux list and propose one that matches */
     found_mux = FALSE;
     for (i=index+1; (i<so_info->negotiation_pars.rev_sch_mo_list.num_mos) &&
           (i < CAI_MAX_MULT_OPT_SCH); i++)
     {
       if ((so_info->negotiation_pars.rev_sch_mo_list.list[i] & CAI_MO_RATE_SET_MASK)
           && (RATE_SET_OF_REV_RC(cfg_ptr->rev_sch[0].sch_chn_cfg.sch_rc) !=
               (MUX_TO_RATE_SET(so_info->negotiation_pars.rev_sch_mo_list.list[i]) & 1)))
       {
         continue;
       }
       cfg_ptr->rev_sch[0].rev_sch_mux = so_info->negotiation_pars.rev_sch_mo_list.list[i];
       found_mux = TRUE;
       index = i;
       break;
     }

     if (!found_mux)
     {
       /* Okay we couldn't find RCs and mux consistent with each other, do
          not propose SCH */
       cfg_ptr->rev_sch_cc_incl = FALSE;
       M1X_MSG( DCP, LEGACY_ERROR,
         "R-SCH not proposed because of RC/MUX inconsistency");
       return;
     }
  }

#ifdef FEATURE_IS2000_REL_A
  if (P_REV_IS_7_OR_GREATER)
  {
    cfg_ptr->rev_sch[0].sch_chn_cfg.frame_40_used =
      rev_sch_fields.rev_sch_recs[0].rev_frame_40_supported;
    cfg_ptr->rev_sch[0].sch_chn_cfg.frame_80_used =
      rev_sch_fields.rev_sch_recs[0].rev_frame_80_supported;

    if (cfg_ptr->rev_sch[0].rev_sch_mux == 0xf20)
    {
      cfg_ptr->rev_sch[0].sch_chn_cfg.max_rate =
        rev_sch_fields.rev_sch_recs[0].rev_max_rate;
    }
    else
    {
      cfg_ptr->rev_sch[0].sch_chn_cfg.max_rate =
        snm_get_max_sch_rate_from_mux(cfg_ptr->rev_sch[0].rev_sch_mux);
    }
  }
#endif /* FEATURE_IS2000_REL_A */

  /* If we are here, it means for_sch_cc_incl is 1 */
  negotiation_info.con_list[con_index].cur_rev_sch_mux_index = (index+1) %
    so_info->negotiation_pars.rev_sch_mo_list.num_mos;

} /* snm_populate_rev_sch_cfg */

/*===========================================================================

FUNCTION snm_gen_scr_chan_mask

DESCRIPTION
  This function generates the SCR channel masks that indicates the active
  channel configurations.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

/*lint -esym(715, use_chan_mask) */
static void snm_gen_scr_chan_mask
(
  caii_srv_cfg_type *cfg_ptr,
    /* Pointer to the service configuration */
  mcc_fwd_chan_mask_type fwd_chan_mask,
    /* Forward channel mask */
  mcc_rev_chan_mask_type rev_chan_mask,
    /* Reverse channel mask */
  boolean use_chan_mask,
    /* Indicates if channel masks shall be considered as input along with SCR */
  snm_chan_mask_type *fwd_scr_chan_mask_ptr,
    /* Pointer to the forward channel mask for service configuration */
  snm_chan_mask_type *rev_scr_chan_mask_ptr
    /* Pointer to the reverse channel mask for service configuration */
)
{
  /* Initialize forward and reverse SCR mask to 0 */
  *fwd_scr_chan_mask_ptr =
  *rev_scr_chan_mask_ptr = SNM_INIT_CHAN_MASK;

  if (!P_REV_IS_6_OR_GREATER)
  {
    /* For P_REV < 6, only FCH config must be active */
    *fwd_scr_chan_mask_ptr |= SNM_FCH;
    *rev_scr_chan_mask_ptr |= SNM_FCH;
  }
  /* Only channel masks are available, generate SCR masks based on
     specified channel masks */
  else if (cfg_ptr == NULL)
  {
    /* Generate forward SCR mask */
    if (fwd_chan_mask & MCC_F_FCH)
    {
      *fwd_scr_chan_mask_ptr |= SNM_FCH;
    }

    if (fwd_chan_mask & MCC_F_DCCH)
    {
      *fwd_scr_chan_mask_ptr |= SNM_DCCH;
    }

    /* Generate reverse SCR mask */
    if (rev_chan_mask & MCC_R_FCH)
    {
      *rev_scr_chan_mask_ptr |= SNM_FCH;
    }

    if (rev_chan_mask & MCC_R_DCCH)
    {
      *rev_scr_chan_mask_ptr |= SNM_DCCH;
    }

  }
  /* Service configuration is available, generate SCR masks based on specified SCR
     (and channel masks if indicated) */
  else
  {
    /* Generate SCR channel mask based on specified service configuration */
    if (cfg_ptr->fch_cc_incl)
    {
      *fwd_scr_chan_mask_ptr |= SNM_FCH;

      /* If R-FCH is assigned or RC != 0, then R-FCH config is active */
      *rev_scr_chan_mask_ptr |= SNM_FCH;

    }

    if (cfg_ptr->dcch_cc_incl)
    {
      *fwd_scr_chan_mask_ptr |= SNM_DCCH;

      /* If R-DCCH is assigned or P_REV < 11 or RC != 0, then R-DCCH config is active */

      /* R-DCCH config is active */
      *rev_scr_chan_mask_ptr |= SNM_DCCH;

    }
  }

} /* snm_gen_scr_chan_mask */

/*===========================================================================

FUNCTION snm_gen_config

DESCRIPTION
  This function builds a service configuration with default or max. mux.
  for a specified service option based on information stored in SNP.  When
  set_connection is FALSE, fch_cc_incl, dcch_cc_incl, for_sch_cc_incl,
  rev_sch_cc_incl are all set to false (used only when first get to TC before
  service negotiation is finished).

DEPENDENCIES
  None.

RETURN VALUE
  SNM_SUCCESS - Service configuration successfully built.
  SNM_FAILURE - service option not in SNP, default configuration generated.

SIDE EFFECTS
  None.

===========================================================================*/

snm_status_type snm_gen_config
(
  word so,
    /* The service option for which a default service configuration
       is to be constructed */
  caii_srv_cfg_type *cfg_ptr,
    /* Pointer to place to construct the default configuration. */
  boolean set_connection,
    /* Specifies whether or not to include a service option connection
       in the default service configuration. */
  snm_mux_config_type mux,
    /* Specifies whether default or maximum mux. options to use */
  byte con_index,
    /* Specifies index in to array of connections in the negotiation_info
       struct. */
  mcc_fwd_chan_mask_type  fwd_chan_mask,
    /* Indicates the forward channel assignment */
  mcc_rev_chan_mask_type  rev_chan_mask
    /* Indicates the reverse channel assignment */
)
{
  word so_index; /* Index into the SO table */
  snm_chan_mask_type fwd_scr_chan_mask = SNM_INIT_CHAN_MASK;
  snm_chan_mask_type rev_scr_chan_mask = SNM_INIT_CHAN_MASK;

  if (con_index >= MAX_SNM_CON)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Illegal index in to negotiation_info.con_list[] %d",
      con_index );
    return SNM_FAILURE;
  }

  if (snm_find_so(so, &so_index) == SNM_FAILURE)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "SO %d not found",
      so);
    return SNM_FAILURE;
  }

  /* Figure out forward and reverse SCR mask to simplfy channel assigned
     comparsion later on */
  snm_gen_scr_chan_mask(NULL, fwd_chan_mask, rev_chan_mask, TRUE,
    &fwd_scr_chan_mask, &rev_scr_chan_mask);

  /* Initialize as if fundicated channel is not specified */
  cfg_ptr->fwd_mux   =
  cfg_ptr->rev_mux   =
  cfg_ptr->fwd_rates =
  cfg_ptr->rev_rates = 0;

  if (mux == SNM_DEFAULT_MUX)
  {
    /* F-FCH and/or F-DCCH is active, set forward fundicated mux */
    if (fwd_scr_chan_mask & (SNM_FCH | SNM_DCCH))
    {
      cfg_ptr->fwd_mux = snp.so[so_index].default_config.forward.mux;
    }

    /* R-FCH and/or R-DCCH is active, set reverse fundicated mux */
    if (rev_scr_chan_mask & (SNM_FCH | SNM_DCCH))
    {
      cfg_ptr->rev_mux = snp.so[so_index].default_config.reverse.mux;
    }

  }
  else /* mux == MAXIMUM */
  {
    /* F-FCH and/or F-DCCH is active, set forward fundicated mux */
    if (fwd_scr_chan_mask & (SNM_FCH | SNM_DCCH))
    {
      cfg_ptr->fwd_mux = snp.so[so_index].negotiation_pars.max_mux_to_use.forward;
    }

    /* R-FCH and/or R-DCCH is active, set reverse fundicated mux */
    if (rev_scr_chan_mask & (SNM_FCH | SNM_DCCH))
    {
      cfg_ptr->rev_mux = snp.so[so_index].negotiation_pars.max_mux_to_use.reverse;
    }
    /* If max_mux is set to CAI_MUX_WILDCARD, then mux values will be adjusted
       at the end of this function */
  }

  /* F-FCH is active, set forward rate */
  if (fwd_scr_chan_mask & SNM_FCH)
  {
    cfg_ptr->fwd_rates = snp.so[so_index].default_config.forward.tx_rates;
  }

  /* R-FCH is active, set reverse rate */
  if (rev_scr_chan_mask & SNM_FCH)
  {
    cfg_ptr->rev_rates = snp.so[so_index].default_config.reverse.tx_rates;
  }

  /* Default # of connections and channel configurations */
  cfg_ptr->num_con = 0;
  cfg_ptr->fch_cc_incl     =
  cfg_ptr->dcch_cc_incl    =
  cfg_ptr->for_sch_cc_incl =
  cfg_ptr->rev_sch_cc_incl = FALSE;

#ifdef FEATURE_IS2000_REL_C
  cfg_ptr->for_pdch_cc_incl = FALSE;
#endif /* FEATURE_IS2000_REL_C */

#ifdef FEATURE_IS2000_REL_B
  cfg_ptr->fch_dcch_mux_option_ind = CAI_MUX_OPTION_SAME_FOR_FCH_AND_DCCH;  /*lint !e641 */
#endif /* FEATURE_IS2000_REL_B */

  if (!set_connection)
  {
    return SNM_SUCCESS;
  }

  /* If instructed to establish a connection */
  else
  {
    /* Only 1 connection is supported at this point. */
    cfg_ptr->num_con = 1;
    cfg_ptr->con[0].con_ref = 0;
    cfg_ptr->con[0].so = so;

    /* CAI_PRI_OR_SEC_TRAF is internally used to indicate if a particular
       service option allows either type of the traffic. If the allowed
       traffic type in the default config is CAI_PRI_OR_SEC_TRAF, we set
       it to CAI_PRI_TRAF. Otherwise, use whatever is in the default config. */

    if (snp.so[so_index].default_config.forward.allowed_traffic_type ==
        CAI_PRI_OR_SEC_TRAF)  /*lint !e641 */
    {
      cfg_ptr->con[0].fwd_traf = CAI_PRI_TRAF; /*lint !e641 */
    }
    else
    {
      cfg_ptr->con[0].fwd_traf =
        snp.so[so_index].default_config.forward.allowed_traffic_type;  /*lint !e641 */
    }

    if (snp.so[so_index].default_config.reverse.allowed_traffic_type ==
        CAI_PRI_OR_SEC_TRAF) /*lint !e641 */
    {
      cfg_ptr->con[0].rev_traf = CAI_PRI_TRAF; /*lint !e641 */
    }
    else
    {
      cfg_ptr->con[0].rev_traf =
        snp.so[so_index].default_config.reverse.allowed_traffic_type; /*lint !e641 */
    }

    if (P_REV_IS_6_OR_GREATER)
    {
      cfg_ptr->con[0].ui_encrypt_mode = 0;

#ifdef FEATURE_IS2000_REL_A
      if (cdma.call_orig)
      {
        cfg_ptr->con[0].sr_id = cdma.sr_id;
      }
      else
#endif /* FEATURE_IS2000_REL_A */

      {
        /* sr_id may take a different value for DS, but for MSM5000, since we only support
           1 service option connection, we are okay to just set this to 1 */
        cfg_ptr->con[0].sr_id = CAI_SR_ID_1;  /*lint !e641 */
      }

      /* Also store the sr_id in our negotiation_info database */
      negotiation_info.con_list[con_index].sr_id = cfg_ptr->con[0].sr_id;
      negotiation_info.con_list[con_index].sr_id_valid = TRUE;

      /* See if RLP blob has to be included */
      cfg_ptr->con[0].rlp_info_incl = FALSE;

      if (snp.so[so_index].rlp_blob_generator)
      {
        /* If so, get blob by calling the generator function. */
        if ((cfg_ptr->con[0].rlp_blob_len =
             snp.so[so_index].rlp_blob_generator(cfg_ptr->con[0].rlp_blob)) > 0)
        {
          cfg_ptr->con[0].rlp_info_incl = TRUE;
        }
      }

#ifdef FEATURE_IS2000_REL_A
      /* See if QoS blob has to be included */
      cfg_ptr->con[0].qos_parms_incl = FALSE;

      if (P_REV_IS_7_OR_GREATER)
      {
        if (snp.so[so_index].qos_blob_generator)
        {
          if ((cfg_ptr->con[0].qos_parms_len =
               snp.so[so_index].qos_blob_generator(cfg_ptr->con[0].qos_parms)) > 0)
          {
            if (cfg_ptr->con[0].qos_parms_len >
                sizeof(cfg_ptr->con[0].qos_parms))
            {
              ERR_FATAL("Data corruption by QOS generator, len %d",
                cfg_ptr->con[0].qos_parms_len, 0, 0);
            }
            else
            {
              cfg_ptr->con[0].qos_parms_incl = TRUE;
            }
          }
        } /* if (snp.so[so_index].qos_blob_generator) */
      }
#endif /* FEATURE_IS2000_REL_A */

    }
  }

  if (so != negotiation_info.con_list[con_index].prev_so)
  {
    /* SO has changed since the last time we evaluated/genrated the config,
       so reset RC and mux indices */
    negotiation_info.con_list[con_index].cur_fwd_fch_rc_index =

    negotiation_info.con_list[con_index].cur_fwd_dcch_rc_index =

    negotiation_info.con_list[con_index].fundicated_rc_proposed_chan_mask = 0;
    negotiation_info.con_list[con_index].cur_fwd_sch_mux_index =
    negotiation_info.con_list[con_index].cur_rev_sch_mux_index = 0;
    negotiation_info.con_list[con_index].prev_so = so;
  }

  if (!P_REV_IS_6_OR_GREATER)
  {
    /* For P_REV lower than 6, we need to initialize the FCH RC properly because
       we'll use them later in the SCR transaction. */
    snm_is2000_scr_init(cfg_ptr);
  }
  else
  {
    /* Include FCH/DCCH/PDCH channel config info based on the forward and reverse
       channel mask. */


    /* DCCH included */
    if ((fwd_scr_chan_mask & SNM_DCCH) || (rev_scr_chan_mask & SNM_DCCH))
    {
      cfg_ptr->dcch_cc_incl = TRUE;
      cfg_ptr->dcch_frame_size = CAI_DCCH_20MS_FRAME;  /*lint !e641 */

      /* Generate DCCH RCs */
      if (!snm_gen_fundicated_rc(&snp.so[so_index], con_index,
          (SNM_FORWARD | SNM_REVERSE), SNM_DCCH, fwd_scr_chan_mask, rev_scr_chan_mask,
          SNM_RESERVED, &cfg_ptr->for_dcch_rc, &cfg_ptr->rev_dcch_rc))
      {
        /* Something is wrong here, we cannot find a good matching rev_rc
           Just return FAILURE, Should never happen */
        return SNM_FAILURE;
      }
    }

    /* FCH included */
    if ((fwd_scr_chan_mask & SNM_FCH) || (rev_scr_chan_mask & SNM_FCH))
    {
      cfg_ptr->fch_cc_incl = TRUE;
      cfg_ptr->fch_frame_size = 0;  /*lint !e641 */

      /* Generate FCH RCs */
      if (!snm_gen_fundicated_rc(&snp.so[so_index], con_index,
          (SNM_FORWARD | SNM_REVERSE), SNM_FCH, fwd_scr_chan_mask, rev_scr_chan_mask,
          SNM_RESERVED, &cfg_ptr->for_fch_rc, &cfg_ptr->rev_fch_rc))
      {
        /* Something is wrong here, we cannot find a good matching rev_rc
           Just return FAILURE, Should never happen */
        return SNM_FAILURE;
      }
    }

      /* Now include SCH CC info if necessary.
      **
      ** Notice: Currently for asymmetic fundicated channel assignment,
      ** we don't propose SCH CC info.
      **
      */
      if (snp.so[so_index].negotiation_pars.fwd_sch_mo_list.num_mos &&
          (((fwd_scr_chan_mask & rev_scr_chan_mask & SNM_FCH) &&
            (RC_CLASS(cfg_ptr->for_fch_rc) > 1))
          || (fwd_scr_chan_mask & rev_scr_chan_mask & SNM_DCCH)
         ))
      {
        if (mccap_num_f_sch_supported())
        {
          snm_populate_fwd_sch_cfg(cfg_ptr, &snp.so[so_index], con_index,
            fwd_scr_chan_mask, rev_scr_chan_mask);
        }
      }

      /* Check if we need to propose any rev-SCH */
      if (snp.so[so_index].negotiation_pars.rev_sch_mo_list.num_mos &&
          (((fwd_scr_chan_mask & rev_scr_chan_mask & SNM_FCH) &&
            (RC_CLASS(cfg_ptr->for_fch_rc) > 1))
            || (fwd_scr_chan_mask & rev_scr_chan_mask & SNM_DCCH)
         ))
      {
        if (mccap_num_r_sch_supported())
        {
          snm_populate_rev_sch_cfg(cfg_ptr, &snp.so[so_index], con_index,
            fwd_scr_chan_mask, rev_scr_chan_mask);
        }
      }
  }

  /* Adjust mux to be in sync with RC, Let's not do that for SOs using SCCH,
     those must already be correct at this point */
  if ((fwd_scr_chan_mask & SNM_FCH) || (fwd_scr_chan_mask & SNM_DCCH))
  {
    if (cfg_ptr->fwd_mux <= CAI_MUX2)
    {
      if (fwd_scr_chan_mask & SNM_FCH)
      {
        cfg_ptr->fwd_mux = RATE_SET_TO_MUX(RATE_SET_OF_FWD_RC(cfg_ptr->for_fch_rc));
      }
      else
      {
        cfg_ptr->fwd_mux = RATE_SET_TO_MUX(RATE_SET_OF_FWD_RC(cfg_ptr->for_dcch_rc));
      }

      if ((rev_scr_chan_mask & SNM_FCH) || (rev_scr_chan_mask & SNM_DCCH))
      {
        cfg_ptr->rev_mux = cfg_ptr->fwd_mux;
      }
    }
  }
  else if ((rev_scr_chan_mask & SNM_FCH) || (rev_scr_chan_mask & SNM_DCCH))
  {
    if (cfg_ptr->rev_mux <= CAI_MUX2)
    {
      if (rev_scr_chan_mask & SNM_FCH)
      {
        cfg_ptr->rev_mux = RATE_SET_TO_MUX(RATE_SET_OF_FWD_RC(cfg_ptr->rev_fch_rc));
      }
      else
      {
        cfg_ptr->rev_mux = RATE_SET_TO_MUX(RATE_SET_OF_FWD_RC(cfg_ptr->rev_dcch_rc));
      }
    }
  }

  return SNM_SUCCESS;

} /* snm_gen_config */

/*===========================================================================

FUNCTION SNM_COMPOSE_CONFIG

DESCRIPTION
  This function performs incremental update of new service configuration
  with old service configuration. It also updates the forward and reverse
  SCR channel mask.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void snm_compose_config
(
  caii_srv_cfg_type *new_cfg_ptr,
    /* New service configuration */
  caii_srv_cfg_type *old_cfg_ptr,
    /* Old service configuration */
  mcc_fwd_chan_mask_type fwd_chan_mask,
    /* Forward channel mask */
  mcc_rev_chan_mask_type rev_chan_mask,
    /* Reverse channel mask */
  snm_chan_mask_type *fwd_scr_chan_mask_ptr,
    /* Pointer to SCR forward channel mask */
  snm_chan_mask_type *rev_scr_chan_mask_ptr
    /* Pointer to SCR reverse channel mask */
)
{
  /* Compose FCH config */
  if (!new_cfg_ptr->fch_cc_incl)
  {
    /* If fch_cc_incl is not included, it means the BS is just telling us to
       use the existing configuration, Make sure are still good with the SO
       we are evaluating */
    if (old_cfg_ptr->fch_cc_incl &&
        ((fwd_chan_mask & MCC_F_FCH) || (rev_chan_mask & MCC_R_FCH)))
    {
      new_cfg_ptr->fch_frame_size = old_cfg_ptr->fch_frame_size;
      new_cfg_ptr->for_fch_rc = old_cfg_ptr->for_fch_rc;
      new_cfg_ptr->rev_fch_rc = old_cfg_ptr->rev_fch_rc;

      /* Update SCR masks for existing configurations to be validated */

      if (new_cfg_ptr->for_fch_rc != 0)
      {
        *fwd_scr_chan_mask_ptr |= SNM_FCH;
      }

      if (new_cfg_ptr->rev_fch_rc != 0)
      {
        *rev_scr_chan_mask_ptr |= SNM_FCH;
      }
    }
  }

  /* Compose the DCCH config */
  if (!new_cfg_ptr->dcch_cc_incl)
  {
    /* If dcch_cc_incl is not included, it means the BS is just telling us to
       use the existing configuration, Make sure are still good with the SO
       we are evaluating */
    if (old_cfg_ptr->dcch_cc_incl &&
        ((fwd_chan_mask & MCC_F_DCCH) || (rev_chan_mask & MCC_R_DCCH)))
    {
      new_cfg_ptr->dcch_frame_size = old_cfg_ptr->dcch_frame_size;
      new_cfg_ptr->for_dcch_rc = old_cfg_ptr->for_dcch_rc;
      new_cfg_ptr->rev_dcch_rc = old_cfg_ptr->rev_dcch_rc;

      /* Update SCR masks for existing configurations to be validated */

      if (new_cfg_ptr->for_dcch_rc != 0)
      {
        *fwd_scr_chan_mask_ptr |= SNM_DCCH;
      }

      if (new_cfg_ptr->rev_dcch_rc != 0)
      {
        *rev_scr_chan_mask_ptr |= SNM_DCCH;
      }
    }
  }

} /* snm_compose_config */

/*===========================================================================

FUNCTION snm_gen_so_chan_mask

DESCRIPTION
  This function generates the channel mask that indicates physical channel
  allowed for the specified SO.

DEPENDENCIES
  None

RETURN VALUE
  Channel mask indicates physical channel supported by SO.

SIDE EFFECTS
  None.
===========================================================================*/

snm_chan_mask_type snm_gen_so_chan_mask
(
  uint16 so
)
{
  snm_chan_mask_type so_chan_mask = SNM_INIT_CHAN_MASK;
  uint16 i, so_index;

  if (snm_find_so(so, &so_index) == SNM_SUCCESS)
  {
    /* Loop through channel value in SO's chan_list to generate the mask */
    for (i = 0; i < MAX_CHN; i++)
    {
      if (snp.so[so_index].negotiation_pars.chan_list[i] == SNM_RESERVED)
      {
        /* We reached the end of the list, so exit */
        break;
      }
      else
      {
        /* Add SO supported channels to the mask */
        so_chan_mask |= snp.so[so_index].negotiation_pars.chan_list[i];
      }
    }

    /* Exclude channels not supported by MS capability */
    if (!P_REV_IS_6_OR_GREATER || !mccap_item_is_supported(MCCAP_DF_DCCH))
    {
      so_chan_mask ^= SNM_DCCH;
    }

#ifdef FEATURE_IS2000_REL_C
    if (!P_REV_IS_9_OR_GREATER || !mccap_is_for_pdch_supported())
    {
      so_chan_mask ^= SNM_PDCH;
    }
#endif /* FEATURE_IS2000_REL_C */

    /* Figure out whether F-SCH is supported by SO */
    if (P_REV_IS_6_OR_GREATER && (mccap_num_f_sch_supported() > 0) &&
        (snp.so[so_index].negotiation_pars.fwd_sch_mo_list.num_mos > 0))
    {
      so_chan_mask |= SNM_F_SCH;
    }

    /* Figure out whether R-SCH is supported by SO */
    if (P_REV_IS_6_OR_GREATER && (mccap_num_r_sch_supported() > 0) &&
        (snp.so[so_index].negotiation_pars.rev_sch_mo_list.num_mos > 0))
    {
      so_chan_mask |= SNM_R_SCH;
    }
  }

  return so_chan_mask;

} /* snm_gen_so_chan_mask */

/*===========================================================================

FUNCTION snm_eval_config_vs_ms_cap

DESCRIPTION
  Evaluates the service configuration to see if specified channels are
  supported by MS capability. Proposes alternate channel config if alt
  cfg is needed and specified channels are not supported by MS.

DEPENDENCIES
  None

RETURN VALUE
  SNM_ACCEPT_CFG: If cfg is accepted.
  SNM_REJECT_CFG: If cfg is rejected,
  SNM_ALT_CFG:    If alt cfg is proposed

SIDE EFFECTS
  None

===========================================================================*/

static snm_eval_status_type snm_eval_config_vs_ms_cap
(
  caii_srv_cfg_type *cfg_ptr,
    /* The service configuration to be evaluated and modified if alt cfg
       is needed */
  snm_chan_mask_type fwd_scr_chan_mask,
    /* Mask indicates active forward channel configuartions */
  snm_chan_mask_type rev_scr_chan_mask,
    /* Mask indicates active forward channel configuartions */
  boolean suggest_alt,
    /* Is an alternate cfg needed */
  snm_eval_status_type curr_status
)
{
  snm_eval_status_type status = curr_status;

  /* ************************************************
  ** Check whether DCCH is supported by MS capability
  ** ************************************************ */
  if ((fwd_scr_chan_mask & SNM_DCCH) || (rev_scr_chan_mask & SNM_DCCH))
  {
    /* Check specified DCCH can be supported by capability */
    if (!P_REV_IS_6_OR_GREATER || !mccap_item_is_supported(MCCAP_DF_DCCH))
    {
      if (!suggest_alt)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "MS does not support DCCH, reject SCR");
        return SNM_REJECT_CFG;
      }
      else
      {
        cfg_ptr->dcch_cc_incl = FALSE;
        status = SNM_ALT_CFG;
      }
    }
  }

#ifdef FEATURE_IS2000_REL_C
  /* **************************************************
  ** Check whether F-PDCH is supported by MS capability
  ** ************************************************** */
  if (fwd_scr_chan_mask & SNM_PDCH)
  {
    if (!P_REV_IS_9_OR_GREATER || !mccap_is_for_pdch_supported())
    {
      if (!suggest_alt)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "MS does not support F-PDCH, reject SCR");
        return SNM_REJECT_CFG;
      }
      else
      {
        cfg_ptr->for_pdch_cc_incl = FALSE;
        status = SNM_ALT_CFG;
      }
    }
  }
#endif /* FEATURE_IS2000_REL_C */

  /* *************************************************
  ** Check whether F-SCH is supported by MS capability
  ** ************************************************* */
  if (cfg_ptr->for_sch_cc_incl && cfg_ptr->num_for_sch &&
      (!P_REV_IS_6_OR_GREATER || !mccap_item_is_supported(MCCAP_DF_FOR_SCH)))
  {
    if (!suggest_alt)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "MS does not support fwd SCH, reject SCR");
      return SNM_REJECT_CFG;
    }
    else
    {
      cfg_ptr->for_sch_cc_incl = FALSE;
      status = SNM_ALT_CFG;
    }
  }

  /* *************************************************
  ** Check whether R-SCH is supported by MS capability
  ** ************************************************* */
  if (cfg_ptr->rev_sch_cc_incl && cfg_ptr->num_rev_sch &&
      (!P_REV_IS_6_OR_GREATER || !mccap_item_is_supported(MCCAP_DF_REV_SCH) ))
  {
    if (!suggest_alt)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "MS does not support rev SCH, reject SCR");
      return SNM_REJECT_CFG;
    }
    else
    {
      cfg_ptr->rev_sch_cc_incl = FALSE;
      status = SNM_ALT_CFG;
    }
  }

  return status;

} /* snm_eval_config_vs_ms_cap */

/*===========================================================================

FUNCTION snm_is_fundicated_rc_okay

DESCRIPTION
  Validates the fundicated RC of a SO connection in a configuration to see
  if it is acceptable. The SO number of the connection is assumed to be valid.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if RC is valid. Otherwise, FALSE.

SIDE EFFECTS
  None

===========================================================================*/

static boolean snm_is_fundicated_rc_okay
(
  snm_so_descriptor_type *so_info,
    /* The SO descriptor to evaluate against */
  snm_fwd_rev_type link_mask,
    /* Mask indicates whether forward or reverse or both RCs to be verified  */
  snm_chan_mask_type rc_chan_mask,
    /* Mask indicates which channel's RCS to be validated */
  snm_chan_mask_type fwd_scr_chan_mask,
    /* Mask indicates active forward channel configuartions */
  snm_chan_mask_type rev_scr_chan_mask,
    /* Mask indicates active reverse channel configuartions */
  cai_radio_config_type fwd_rc,
    /* Forward RC */
  cai_radio_config_type rev_rc
    /* Reverse RC */
)
{
  boolean rc_class2_required;
    /* Indicates if RC class 2 is needed */
  boolean rc_ok = FALSE;
  int     i;

  /* Figure out whether RC class 2 is required */
  rc_class2_required = snm_is_rc_class2_required(fwd_scr_chan_mask, rev_scr_chan_mask);

  /* Check forward RC */
  if (link_mask & SNM_FORWARD)
  {
    rc_ok = FALSE;

    /* Forward fundicated channel not assigned, RC value shall be 0 */
    if (!(fwd_scr_chan_mask & rc_chan_mask))
    {
      if (fwd_rc == CAI_INIT_RC)
      {
        rc_ok = TRUE;
      }
    }
    /* Forward fundicated channel assigned, validate against RC list */
    else
    {
      for (i = 0; i < so_info->negotiation_pars.fwd_rc.num_of_rc; i++)
      {
        if ((so_info->negotiation_pars.fwd_rc.list[i] == fwd_rc) &&
            (!rc_class2_required || (RC_CLASS(fwd_rc) > 1)))
        {
          rc_ok = TRUE;
          break;
        }
      }
    }

    if (!rc_ok)
    {
      return rc_ok;
    }
  }

  /* Check reverse RC */
  if (link_mask & SNM_REVERSE)
  {
    rc_ok = FALSE;

    /* Reverse fundicated channel not assigned, RC value shall be 0 */
    if (!(rev_scr_chan_mask & rc_chan_mask))
    {
      if (rev_rc == CAI_INIT_RC)
      {
        rc_ok = TRUE;
      }
    }
    else
    {
      /* Reverse fundicated channel assigned, validate against RC list */
      for (i = 0; i < so_info->negotiation_pars.rev_rc.num_of_rc; i++)
      {
        if (so_info->negotiation_pars.rev_rc.list[i] == rev_rc)
                {
          /* Rev RC should be in the same RC class as the fwd RC and forward rate
             set should be same as that of reverse */
          if ((fwd_rc == CAI_INIT_RC) ||
              ((RC_CLASS(fwd_rc) == RC_CLASS(rev_rc)) &&
               (RATE_SET_OF_FWD_RC(fwd_rc) == RATE_SET_OF_REV_RC(rev_rc))))
          {
            rc_ok = TRUE;
          }
          break;
        }
      }
    }
  }

  return rc_ok;

} /* snm_is_fundicated_rc_okay */

/*===========================================================================

FUNCTION snm_is_fundicated_rc_match

DESCRIPTION
  Verifies the RCs' class and rate set among fundicated channels.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if RC's class and rate set match among fundicated channels.
  Otherwise, FALSE.

SIDE EFFECTS
  None

===========================================================================*/

static boolean snm_is_fundicated_rc_match
(
  snm_chan_mask_type fwd_scr_chan_mask,
    /* Mask indicates active forward channel configuartions */
  snm_chan_mask_type rev_scr_chan_mask, /*lint -esym(715, rev_scr_chan_mask)*/
    /* Mask indicates active reverse channel configuartions */
  caii_srv_cfg_type *cfg_ptr
    /* The service configuration to be evaluated and modified if alt cfg
       is needed */
)
{
  boolean fundicated_rc_match = TRUE;
  uint8 fch_rateset, dcch_rateset;
  uint8 fch_rc_class, dcch_rc_class;

  /* Only FCH or DCCH is active but not both, we don't need to do any check */
  if (((fwd_scr_chan_mask | rev_scr_chan_mask) & (SNM_FCH | SNM_DCCH)) ^
      (SNM_FCH | SNM_DCCH))
  {
     return TRUE;
  }

  /* Figure out FCH rateset */
  if (fwd_scr_chan_mask & SNM_FCH)
  {
    fch_rateset = RATE_SET_OF_FWD_RC(cfg_ptr->for_fch_rc);
    fch_rc_class = RC_CLASS(cfg_ptr->for_fch_rc);
  }
  else
  {
    fch_rateset = RATE_SET_OF_REV_RC(cfg_ptr->rev_fch_rc);
    fch_rc_class = RC_CLASS(cfg_ptr->rev_fch_rc);
  }

  /* Figure out DCCH rateset */
  if (fwd_scr_chan_mask & SNM_DCCH)
  {
    dcch_rateset = RATE_SET_OF_FWD_RC(cfg_ptr->for_dcch_rc);
    dcch_rc_class = RC_CLASS(cfg_ptr->for_dcch_rc);
  }
  else
  {
    dcch_rateset = RATE_SET_OF_REV_RC(cfg_ptr->rev_dcch_rc);
    dcch_rc_class = RC_CLASS(cfg_ptr->rev_dcch_rc);
  }

  /* All fundicated channels shall be in the same RC class. For P_REV < 8,
     all fundicated channels shall be in the sane rate set */
  if ((fch_rc_class != dcch_rc_class) ||
      ((fch_rateset != dcch_rateset)
#ifdef FEATURE_IS2000_REL_B
        && !P_REV_IS_8_OR_GREATER
#endif /* FEATURE_IS2000_REL_B */
     ))
  {
    fundicated_rc_match = FALSE;
  }

  return (fundicated_rc_match);

} /* snm_is_fundicated_rc_match */

/*===========================================================================

FUNCTION snm_eval_fundicated_frame_size

DESCRIPTION
  Validates the fundicated frame size to see if it is acceptable.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if fundicated frame size is valid. Otherwise, FALSE.

SIDE EFFECTS
  None

===========================================================================*/

static snm_eval_status_type snm_eval_fundicated_frame_size
(
  snm_chan_mask_type frame_size_chan_mask,
    /* Mask indicates which channel's frame size to be validated */
  caii_srv_cfg_type *cfg_ptr,
    /* The service configuration to be evaluated and modified if alt cfg
       is needed */
  boolean suggest_alt,
    /* Is an alternate cfg needed */
  snm_eval_status_type curr_status
)
{
  snm_eval_status_type status = curr_status;

  /* *********************
  ** Check DCCH frame size
  ** ********************* */
  if (frame_size_chan_mask == SNM_DCCH)
  {
    if (cfg_ptr->dcch_frame_size != CAI_DCCH_20MS_FRAME)  /*lint !e641 */
    {
      if (!suggest_alt)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "BS proposed 5ms DCCH frame size, reject SCR");
        return SNM_REJECT_CFG;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "BS proposed 5ms DCCH frame size, propose SCR");
        cfg_ptr->dcch_cc_incl = TRUE;
        cfg_ptr->dcch_frame_size = CAI_DCCH_20MS_FRAME; /*lint !e641 */
        status = SNM_ALT_CFG;
      }
    }
  }
  else

  /* *********************
  ** Check FCH frame size
  ** ********************* */
  if (frame_size_chan_mask == SNM_FCH)
  {
    /* ********************
    ** Check FCH frame size
    ** ******************** */
    if (cfg_ptr->fch_frame_size)
    {
      if (!suggest_alt)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "BS proposed 5ms FCH frame size, reject SCR");
        return SNM_REJECT_CFG;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "BS proposed 5ms FCH frame size, propose SCR");
        cfg_ptr->fch_cc_incl = TRUE;
        cfg_ptr->fch_frame_size = 0;
        status = SNM_ALT_CFG;
      }
    }
  }

  return status;

} /* snm_eval_fundicated_frame_size */

/*===========================================================================

FUNCTION snm_is_fundicated_mux_okay

DESCRIPTION
  Validates the fundicated MUX to see if it is acceptable. If the validation
  is tied to SO, the SO number of the connection is assumed to be valid.

DEPENDENCIES
  RCs are valid.

RETURN VALUE
  TRUE if MUX is valid. Otherwise, FALSE.

SIDE EFFECTS
  None

===========================================================================*/

static boolean snm_is_fundicated_mux_okay
(
  snm_so_descriptor_type *so_info,
    /* The SO descriptor to evaluate against */
  snm_chan_mask_type fwd_scr_chan_mask,
    /* Mask indicates active forward channel configuartions */
  snm_chan_mask_type rev_scr_chan_mask,
    /* Mask indicates active reverse channel configuartions */
  cai_radio_config_type fwd_rc,
    /* Forward RC */
  cai_radio_config_type rev_rc,
    /* Reverse RC */
  word fwd_mux,
    /* Forward MUX */
  word rev_mux
    /* Reverse MUX */
)
{
  boolean mux_ok = TRUE;

  /* RC is 0 or >= 3 */
  if (P_REV_IS_6_OR_GREATER &&
      ((fwd_rc == CAI_INIT_RC) || (rev_rc == CAI_INIT_RC) || (RC_CLASS(fwd_rc) > 1) || (RC_CLASS(rev_rc) > 1)))
  {
    if (fwd_rc != CAI_INIT_RC)
    {
      /* Forward fundicated channel is assigned, MUX cannot be zero */
      if (fwd_mux == 0)
      {
        mux_ok = FALSE;
      }
      /* Make sure rate set of mux and RC matches.*/
      else if (RATE_SET_OF_FWD_RC(fwd_rc) != MUX_TO_RATE_SET(fwd_mux))
      {
        mux_ok = FALSE;
      }
    }
    else
    {
      /* No forward fundicated channel is assigned, MUX must be zero */
      if ((fwd_mux != 0) && !(fwd_scr_chan_mask & (SNM_FCH | SNM_DCCH)))
      {
        mux_ok = FALSE;
      }
    }

    if (rev_rc != CAI_INIT_RC)
    {
      /* Reverse fundicated channel is assigned, MUX cannot be zero */
      if (rev_mux == 0)
      {
        mux_ok = FALSE;
      }
      /* Make sure rate set of mux and RC matches.*/
      else if (RATE_SET_OF_REV_RC(rev_rc) != MUX_TO_RATE_SET(rev_mux))
      {
        mux_ok = FALSE;
      }
    }
    /* No reverse fundicated channel is assigned, MUX must be zero */
    else if ((rev_mux != 0) && !(rev_scr_chan_mask & (SNM_FCH | SNM_DCCH)))
    {
      mux_ok = FALSE;
    }
  }
  /* RC is 1 or 2 */
  else
  {
    /* Validation is done against SO */
    if (so_info != NULL)
    {
      /* Cfg contains an older RC, make sure the mux option is consistent
         with this RC */
      snm_mux_pair_type max_mux = so_info->negotiation_pars.max_mux_to_use;

      /* Forward MUX */
      if (((fwd_mux ^ max_mux.forward) & 1) &&   /* Not matching */
          (max_mux.forward != CAI_MUX_WILDCARD)) /* Forward mux is not wildcarded */
      {
        mux_ok = FALSE;
      }
      /* Reverse mux. */
      else if (((rev_mux ^ max_mux.reverse) & 1) &&  /* Not matching */
               (max_mux.reverse != CAI_MUX_WILDCARD)) /* Reverse mux is not wildcarded */
      {
        mux_ok = FALSE;
      }
      /* Make sure not exceed max MUX */
      else
      {
        /* Propose minimum of base station proposed mux. and our max. mux.
           Ignore max_mux if wildcarded */

        if (((fwd_mux > max_mux.forward) && (max_mux.forward != CAI_MUX_WILDCARD)) ||
            ((rev_mux > max_mux.reverse) && (max_mux.reverse != CAI_MUX_WILDCARD)))
        {
          mux_ok = FALSE;
        }
      }
    }
    /* Not validated against SO, do basic value validation. */
    else
    {
      if (((fwd_scr_chan_mask & (SNM_FCH | SNM_DCCH)) &&
           (fwd_mux != CAI_MUX1) && (fwd_mux != CAI_MUX2)) ||
          ((rev_scr_chan_mask & (SNM_FCH | SNM_DCCH)) &&
           (rev_mux != CAI_MUX1) && (rev_mux != CAI_MUX2)))
      {
        mux_ok = FALSE;
      }
    }
  } /* end if (RC_CLASS(fwd_rc) > 1) */

  return mux_ok;

} /* snm_is_fundicated_mux_okay */

/*===========================================================================

FUNCTION snm_gen_fundicated_mux

DESCRIPTION
  Proposes the fundicated MUX of a SO connection in a configuration.

DEPENDENCIES
  RCs are valid.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void snm_gen_fundicated_mux
(
  snm_so_descriptor_type *so_info,
    /* The SO descriptor to evaluate against */
  snm_chan_mask_type fwd_scr_chan_mask,
    /* Mask indicates active forward channel configuartions */
  snm_chan_mask_type rev_scr_chan_mask,
    /* Mask indicates active reverse channel configuartions */
  cai_radio_config_type fwd_rc,
    /* Forward RC */
  cai_radio_config_type rev_rc,
    /* Reverse RC */
  word *fwd_mux,
    /* Proposed forward MUX to be stored */
  word *rev_mux
    /* Proposed reverse MUX to be stored */
)
{
  /* RC is 0 or >= 3 */
  if (P_REV_IS_6_OR_GREATER &&
      ((fwd_rc == CAI_INIT_RC) || (rev_rc == CAI_INIT_RC) || (RC_CLASS(fwd_rc) > 1) || (RC_CLASS(rev_rc) > 1)))
  {
    if (fwd_rc == CAI_INIT_RC)
    {
      /* No forward fundicated channel is using the forward MUX value,
         forward MUX value must be 0. */
      if (!(fwd_scr_chan_mask & (SNM_FCH | SNM_DCCH)))
      {
        *fwd_mux = 0;
      }
      /* Otherwise, don't change forward MUX value since another fundicated channel is
         using it */
    }
    else
    /* Forward fundicated channel assigned, deduce MUX value from RC value */
    {
      *fwd_mux = RATE_SET_TO_MUX(RATE_SET_OF_FWD_RC(fwd_rc));
    }

    if (rev_rc == CAI_INIT_RC)
    {
      /* No reverse fundicated channel is using the reverse MUX value,
         reverse MUX value must be 0 */
      if (!(rev_scr_chan_mask & (SNM_FCH | SNM_DCCH)))
      {
        *rev_mux = 0;
      }
      /* Otherwise, don't change reverse MUX value since another fundicated channel is
         using it */
    }
    else
    /* Reverse fundicated channel assigned, deduce MUX value from RC value */
    {
      *rev_mux = RATE_SET_TO_MUX(RATE_SET_OF_REV_RC(rev_rc));
    }
  }
  /* RC is 1 or 2. Always symmetric channel assignment */
  /* Added the null pointer check */
  else if (so_info != NULL)
  {
    snm_mux_pair_type max_mux = so_info->negotiation_pars.max_mux_to_use;

    /* Propose fwd MUX option if needed */
    if ( (max_mux.forward != CAI_MUX_WILDCARD) &&
         ( ((*fwd_mux ^ max_mux.forward) & 1) || (*fwd_mux > max_mux.forward) )
       )
    {
      /* Add 1 for rate set 1 and subtract 1 for rate set 2 */
      if (*fwd_mux & 1)
      {
        *fwd_mux = *fwd_mux + 1;
      }
      else
      {
        *fwd_mux = *fwd_mux - 1;
      }

      if ( (*fwd_mux > max_mux.forward) )
      {
        *fwd_mux = MIN(*fwd_mux, max_mux.forward);
      }
    }

    /* Propose rev MUX option if needed */
    if ( (max_mux.reverse != CAI_MUX_WILDCARD) &&
         ( ((*rev_mux ^ max_mux.reverse) & 1) || (*rev_mux > max_mux.reverse) )
       )
    {
      /* Add 1 for rate set 1 and subtract 1 for rate set 2 */
      if (*rev_mux & 1 )
      {
        *rev_mux = *rev_mux + 1;
      }
      else
      {
        *rev_mux = *rev_mux - 1;
      }

      if (*rev_mux > max_mux.reverse)
      {
        *rev_mux = MIN(*rev_mux, max_mux.reverse);
      }
    }
  } /* end if (RC_CLASS(fwd_rc) > 1) */

} /* snm_gen_fundicated_mux */

/*===========================================================================

FUNCTION snm_eval_fundicated_mux

DESCRIPTION
  Evaluates the fundicated MUXs of a SO connection in a configuration to see
  if it is acceptable. Proposes alterate fundicated MUXs if alt cfg is needed
  and cfg is not acceptable. The SO number of the connection is assumed to be
  valid.

DEPENDENCIES
  RCs shall be valid.

RETURN VALUE
  SNM_ACCEPT_CFG: If Cfg is accepted.
  SNM_REJECT_CFG: If it is rejected,
  SNM_ALT_CFG:    If alt cfg is proposed

SIDE EFFECTS
  None

===========================================================================*/

static snm_eval_status_type snm_eval_fundicated_mux
(
  snm_chan_mask_type mux_chan_mask,
    /* Mask indicates which channel's MUX to be validated */
  snm_chan_mask_type fwd_scr_chan_mask,
    /* Mask indicates active forward channel configuartions */
  snm_chan_mask_type rev_scr_chan_mask,
    /* Mask indicates active reverse channel configuartions */
  caii_srv_cfg_type *cfg_ptr,
    /* The service configuration to be evaluated and modified if alt cfg
       is needed */
  snm_so_descriptor_type *so_info,
    /* The SO descriptor to evaluate against */
  boolean suggest_alt,
    /* Is an alternate cfg needed */
  snm_eval_status_type curr_status
)
{
  snm_eval_status_type status = curr_status;
  word *fwd_mux_ptr, *rev_mux_ptr;
  cai_radio_config_type fwd_rc, rev_rc;

#ifdef FEATURE_IS2000_REL_B
  /* **************************
  ** Check MUX option indicator
  ** ************************** */

  if (P_REV_IS_8_OR_GREATER)
  {
    /* Both FCH and DCCH assigned */
    if (((fwd_scr_chan_mask | rev_scr_chan_mask) & SNM_FCH) &&
        ((fwd_scr_chan_mask | rev_scr_chan_mask) & SNM_DCCH))
    {
      uint8 fch_rateset, dcch_rateset;

      /* Figure out rate set of FCH */
      if (fwd_scr_chan_mask & SNM_FCH)
      {
        fch_rateset = RATE_SET_OF_FWD_RC(cfg_ptr->for_fch_rc);
      }
      else
      {
        fch_rateset = RATE_SET_OF_REV_RC(cfg_ptr->rev_fch_rc);
      }

      /* Figure out rate set of DCCH */
      if (fwd_scr_chan_mask & SNM_DCCH)
      {
        dcch_rateset = RATE_SET_OF_FWD_RC(cfg_ptr->for_dcch_rc);
      }
      else
      {
        dcch_rateset = RATE_SET_OF_REV_RC(cfg_ptr->rev_dcch_rc);
      }

      /* MUX option indicator species only FCH or DCCH */
      if ((cfg_ptr->fch_dcch_mux_option_ind == CAI_MUX_OPTION_ONLY_FOR_FCH) || /*lint !e641 */
          (cfg_ptr->fch_dcch_mux_option_ind == CAI_MUX_OPTION_ONLY_FOR_DCCH))  /*lint !e641 */
      {
        /* Invalid MUX Option Ind specified */
        if (!suggest_alt)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Bad Mux Opt Ind %d, reject SCR",
            cfg_ptr->fch_dcch_mux_option_ind);
          return SNM_REJECT_CFG;
        }
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Bad Mux Opt Ind %d, propose SCR",
            cfg_ptr->fch_dcch_mux_option_ind);
          /* FCH and DCCH are in the same rate set */
          if (fch_rateset == dcch_rateset)
          {
            cfg_ptr->fch_dcch_mux_option_ind = CAI_MUX_OPTION_SAME_FOR_FCH_AND_DCCH; /*lint !e641 */
          }
          /* FCH and DCCH are in different rate set */
          else
          {
            cfg_ptr->fch_dcch_mux_option_ind = CAI_MUX_OPTION_DIFF_FOR_FCH_AND_DCCH; /*lint !e641 */
          }

          status = SNM_ALT_CFG;
        }
      }

      /* Same MUX option used for both FCH and DCCH. But the rate set of FCH and DCCH are different. */
      else if ((cfg_ptr->fch_dcch_mux_option_ind == CAI_MUX_OPTION_SAME_FOR_FCH_AND_DCCH) && /*lint !e641 */
               (fch_rateset != dcch_rateset))
      {
        /* Invalid MUX Option Ind specified */
        if (!suggest_alt)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Bad Mux Opt Ind %d, reject SCR",
            cfg_ptr->fch_dcch_mux_option_ind);
          return SNM_REJECT_CFG;
        }
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Bad Mux Opt Ind %d, propose SCR",
            cfg_ptr->fch_dcch_mux_option_ind);
          cfg_ptr->fch_dcch_mux_option_ind = CAI_MUX_OPTION_DIFF_FOR_FCH_AND_DCCH;  /*lint !e641 */
          status = SNM_ALT_CFG;
        }
      }
    }

    /* Only DCCH assignment */
    else if ((fwd_scr_chan_mask | rev_scr_chan_mask) & SNM_DCCH)
    {
      /* MUX option indicator species only FCH */
      if (cfg_ptr->fch_dcch_mux_option_ind == CAI_MUX_OPTION_ONLY_FOR_FCH)  /*lint !e641 */
      {
        /* Invalid MUX Option Ind specified */
        if (!suggest_alt)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Bad Mux Opt Ind %d, reject SCR",
            cfg_ptr->fch_dcch_mux_option_ind);
          return SNM_REJECT_CFG;
        }
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Bad Mux Opt Ind %d, propose SCR",
            cfg_ptr->fch_dcch_mux_option_ind);
          cfg_ptr->fch_dcch_mux_option_ind = CAI_MUX_OPTION_ONLY_FOR_DCCH;  /*lint !e641 */
          status = SNM_ALT_CFG;
        }
      }
    }
    else

    /* Only FCH assignment */
    if ((fwd_scr_chan_mask | rev_scr_chan_mask) & SNM_FCH)
    {
      /* MUX option indicator species only DCCH */
      if (cfg_ptr->fch_dcch_mux_option_ind == CAI_MUX_OPTION_ONLY_FOR_DCCH) /*lint !e641 */
      {
        /* Invalid MUX Option Ind specified */
        if (!suggest_alt)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Bad Mux Opt Ind %d, reject SCR",
            cfg_ptr->fch_dcch_mux_option_ind);
          return SNM_REJECT_CFG;
        }
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Bad Mux Opt Ind %d, propose SCR",
            cfg_ptr->fch_dcch_mux_option_ind);
          cfg_ptr->fch_dcch_mux_option_ind = CAI_MUX_OPTION_ONLY_FOR_FCH; /*lint !e641 */
          status = SNM_ALT_CFG;
        }
      }
    }
  }
#endif /* FEATURE_IS2000_REL_B */

  /* **********
  ** Check MUX
  ** ********** */

  /* Verify DCCH MUX */
  if (mux_chan_mask & SNM_DCCH)
  {
    snm_chan_mask_type temp_fwd_scr_chan_mask = fwd_scr_chan_mask;
    snm_chan_mask_type temp_rev_scr_chan_mask = rev_scr_chan_mask;

#ifdef FEATURE_IS2000_REL_B
    /* DCCH MUXs is separately specified */
    if (P_REV_IS_8_OR_GREATER && (cfg_ptr->fch_dcch_mux_option_ind == CAI_MUX_OPTION_DIFF_FOR_FCH_AND_DCCH))  /*lint !e641 */
    {
      /* Since MUXs are specific to DCCH, so no need to worry on FCH assignment during DCCH MUX validation */
      temp_fwd_scr_chan_mask ^= SNM_FCH;
      temp_rev_scr_chan_mask ^= SNM_FCH;

      fwd_mux_ptr = &cfg_ptr->for_dcch_mux;
      rev_mux_ptr = &cfg_ptr->rev_dcch_mux;
    }
    else
#endif /* FEATURE_IS2000_REL_B */
    /* Specified MUXs common to fundicated channels */
    {
      fwd_mux_ptr = &cfg_ptr->fwd_mux;
      rev_mux_ptr = &cfg_ptr->rev_mux;
    }

    fwd_rc = (cai_radio_config_type) cfg_ptr->for_dcch_rc;
    rev_rc = (cai_radio_config_type) cfg_ptr->rev_dcch_rc;

    if (!snm_is_fundicated_mux_okay(so_info, temp_fwd_scr_chan_mask, temp_rev_scr_chan_mask,
          fwd_rc, rev_rc, *fwd_mux_ptr, *rev_mux_ptr))
    {
      /* Bad MUXs */
      if (!suggest_alt)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad DCCH Mux: fwd %d rev %d, reject SCR",
          *fwd_mux_ptr,
          *rev_mux_ptr);
        return SNM_REJECT_CFG;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad DCCH Mux: fwd %d rev %d, propose SCR",
          *fwd_mux_ptr,
          *rev_mux_ptr);
        cfg_ptr->dcch_cc_incl = TRUE;
        snm_gen_fundicated_mux(so_info, temp_fwd_scr_chan_mask, temp_rev_scr_chan_mask, fwd_rc,
          rev_rc, fwd_mux_ptr, rev_mux_ptr);
        status = SNM_ALT_CFG;
      }
    }
  }

  /* FCH MUX validation */
  if (mux_chan_mask & SNM_FCH)
  {
    snm_chan_mask_type temp_fwd_scr_chan_mask = fwd_scr_chan_mask;
    snm_chan_mask_type temp_rev_scr_chan_mask = rev_scr_chan_mask;

#ifdef FEATURE_IS2000_REL_B
    /* DCCH MUXs is separately specified */
    if (P_REV_IS_8_OR_GREATER && (cfg_ptr->fch_dcch_mux_option_ind == CAI_MUX_OPTION_DIFF_FOR_FCH_AND_DCCH)) /*lint !e641 */
    {
      /* Since MUXs are specific to FCH, so no need to worry on DCCH assignment during FCH MUX validation */
      temp_fwd_scr_chan_mask ^= SNM_DCCH;
      temp_rev_scr_chan_mask ^= SNM_DCCH;
    }
#endif /* FEATURE_IS2000_REL_B */

    fwd_mux_ptr = &cfg_ptr->fwd_mux;
    rev_mux_ptr = &cfg_ptr->rev_mux;
    fwd_rc = (cai_radio_config_type) cfg_ptr->for_fch_rc;
    rev_rc = (cai_radio_config_type) cfg_ptr->rev_fch_rc;

    if (!snm_is_fundicated_mux_okay(so_info, temp_fwd_scr_chan_mask, temp_rev_scr_chan_mask,
          fwd_rc,rev_rc, *fwd_mux_ptr, *rev_mux_ptr))
    {
      /* Bad MUXs */
      if (!suggest_alt)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad FCH Mux: fwd %d rev %d, reject SCR",
          *fwd_mux_ptr,
          *rev_mux_ptr);
        return SNM_REJECT_CFG;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad FCH Mux: fwd %d rev %d, propose SCR",
          *fwd_mux_ptr,
          *rev_mux_ptr);
        cfg_ptr->fch_cc_incl = TRUE;
        snm_gen_fundicated_mux(so_info, temp_fwd_scr_chan_mask, temp_rev_scr_chan_mask,
          fwd_rc, rev_rc, fwd_mux_ptr, rev_mux_ptr);
        status = SNM_ALT_CFG;
      }
    }
  }

  return status;

} /* snm_eval_fundicated_mux */

/*===========================================================================

FUNCTION snm_eval_fch_rates

DESCRIPTION
  Evaluates the transmission rates of a configuration to see if it is
  acceptable. Proposes alternate rates if alt cfg is needed and cfg is not
  acceptable. The SO number of the connection is assumed to be valid.

DEPENDENCIES
  None

RETURN VALUE
  SNM_ACCEPT_CFG: If cfg is accepted.
  SNM_REJECT_CFG: If cfg is rejected,
  SNM_ALT_CFG:    If alt cfg is proposed

SIDE EFFECTS
  None

===========================================================================*/

static snm_eval_status_type snm_eval_fch_rates
(
  snm_chan_mask_type fwd_scr_chan_mask,
    /* Mask indicates active forward channel configuartions */
  snm_chan_mask_type rev_scr_chan_mask,
    /* Mask indicates active reverse channel configuartions */
  caii_srv_cfg_type *cfg_ptr,
    /* The service configuration to be evaluated and modified if alt cfg
       is needed */
  snm_so_descriptor_type *so_info,
    /* The SO descriptor to evaluate against */
  boolean suggest_alt,
    /* Is an alternate cfg needed */
  snm_eval_status_type curr_status
)
{
  snm_eval_status_type status = curr_status;

  /* Validate against SO */
  if (so_info != NULL)
  {
    /* Check Fwd Tx rates */
    if ((fwd_scr_chan_mask & SNM_FCH) && (cfg_ptr->fwd_rates != so_info->default_config.forward.tx_rates))
    {
      if (!suggest_alt)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Fwd rate %d is not good, reject SCR",
          cfg_ptr->fwd_rates);
        return SNM_REJECT_CFG;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Fwd rate %d is not good, propose SCR",
          cfg_ptr->fwd_rates);
        cfg_ptr->fwd_rates = so_info->default_config.forward.tx_rates;
        status = SNM_ALT_CFG;
      }
    }

    /* Now Check Rev Tx rates */
    if ((rev_scr_chan_mask & SNM_FCH) && (cfg_ptr->rev_rates != so_info->default_config.reverse.tx_rates))
    {
      if (!suggest_alt)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Rev rate %d is not good, reject SCR",
          cfg_ptr->rev_rates);
        return SNM_REJECT_CFG;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Rev rate %d is not good, propose SCR",
          cfg_ptr->rev_rates);
        cfg_ptr->rev_rates = so_info->default_config.reverse.tx_rates;
        status = SNM_ALT_CFG;
      }
    }
  }

  /* Do basic validation */
  else
  {
    /* Check Fwd Tx rates */
    if ((fwd_scr_chan_mask & SNM_FCH) && (cfg_ptr->fwd_rates != CAI_RS1_ALL) &&
        (cfg_ptr->fwd_rates != CAI_RS2_ALL))
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Fwd Rate %d is not good, reject SCR",
        cfg_ptr->fwd_rates);
      return SNM_REJECT_CFG;
    }

    /* Now Check Rev Tx rates */
    if ((rev_scr_chan_mask & SNM_FCH) && (cfg_ptr->rev_rates != CAI_RS1_ALL) &&
        (cfg_ptr->rev_rates != CAI_RS2_ALL))
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Rev Rate %d is not good, reject SCR",
        cfg_ptr->rev_rates);
      return SNM_REJECT_CFG;
    }
  }

  return status;

} /* snm_eval_fch_rates */

/*===========================================================================

FUNCTION snm_eval_fundicated_config

DESCRIPTION
  Evaluates the fundicated config of a SO connection in a configuration to see
  if it is acceptable. Proposes alternate fundicated config if alt cfg is needed
  and cfg is not acceptable. The SO number of the connection is assumed to be
  valid.

DEPENDENCIES
  None

RETURN VALUE
  SNM_ACCEPT_CFG: If cfg is accepted.
  SNM_REJECT_CFG: If cfg is rejected,
  SNM_ALT_CFG:    If alt cfg is proposed

SIDE EFFECTS
  None

===========================================================================*/

static snm_eval_status_type snm_eval_fundicated_config
(
  snm_chan_mask_type so_chan_mask,
    /* Mask indicates channels supported by SO */
  snm_chan_mask_type fwd_scr_chan_mask,
    /* Mask indicates active forward channel configuartions */
  snm_chan_mask_type rev_scr_chan_mask,
    /* Mask indicates active reverse channel configuartions */
  caii_srv_cfg_type *cfg_ptr,
    /* The service configuration to be evaluated and modified if alt cfg
       is needed */
  snm_so_descriptor_type *so_info,
    /* The SO descriptor to evaluate against */
  byte snm_con_index,
    /* connection index in negotiation_info.con_list[] */
  boolean suggest_alt,
    /* Is an alternate cfg needed */
  snm_eval_status_type curr_status
)
{
  snm_eval_status_type status = curr_status;
  snm_chan_mask_type compatible_chan_mask = SNM_INIT_CHAN_MASK;
    /* Indicates which fundicated channel(s)' config is compatible with SO */
  snm_fwd_rev_type fch_alt_rc = SNM_FORWARD | SNM_REVERSE;
    /* Indicates whether alternate reverse (and forward) FCH RC has to be
       proposed if needed */
  snm_fwd_rev_type dcch_alt_rc = SNM_FORWARD | SNM_REVERSE;
    /* Indicates whether alternate reverse (and forward) DCCH RC has to be
       proposed if needed */

  snm_chan_mask_type so_preferred_fundicated_chan_mask;
    /* Indicates which fundicated channel(s) is needed for SO */

  if (CAI_IS_SO_SIGNALING(so_info->service_option_num))
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Signalling SO %d don't care fundicated config",
      so_info->service_option_num);
    return status;
  }

  /* ************
  ** Validate RCs
  ** ************ */

  /* Validate DCCH RC if SO supports DCCH */
  if ((fwd_scr_chan_mask | rev_scr_chan_mask) & so_chan_mask & SNM_DCCH)
  {
    if (snm_is_fundicated_rc_okay(so_info, SNM_FORWARD, SNM_DCCH,
        fwd_scr_chan_mask, rev_scr_chan_mask,
                                  (cai_radio_config_type) cfg_ptr->for_dcch_rc,
                                  (cai_radio_config_type) cfg_ptr->rev_dcch_rc))
    {
      /* Okay the fwd RC was good, check the rev RC now */
      if (snm_is_fundicated_rc_okay(so_info, SNM_REVERSE, SNM_DCCH,
          fwd_scr_chan_mask, rev_scr_chan_mask,
                                    (cai_radio_config_type) cfg_ptr->for_dcch_rc,
                                    (cai_radio_config_type) cfg_ptr->rev_dcch_rc))
      {
        /* Both fwd and rev RCs are good */
        compatible_chan_mask |= SNM_DCCH;
      }
      else
      {
        /* Mark only alternate reverse DCCH RC has to be proposed */
        dcch_alt_rc = SNM_REVERSE;
      }
    }
  }

  /* Validate FCH RC if SO supports FCH */
  if ((fwd_scr_chan_mask | rev_scr_chan_mask) & so_chan_mask & SNM_FCH)
  {
    if (snm_is_fundicated_rc_okay(so_info, SNM_FORWARD, SNM_FCH,
        fwd_scr_chan_mask, rev_scr_chan_mask,
                                  (cai_radio_config_type) cfg_ptr->for_fch_rc,
                                  (cai_radio_config_type) cfg_ptr->rev_fch_rc))
    {
      /* Okay the fwd RC was good, check the rev RC now */
      if (snm_is_fundicated_rc_okay(so_info, SNM_REVERSE, SNM_FCH,
          fwd_scr_chan_mask, rev_scr_chan_mask,
                                    (cai_radio_config_type) cfg_ptr->for_fch_rc,
                                    (cai_radio_config_type) cfg_ptr->rev_fch_rc))
      {
        /* Both fwd and rev RCs are good */
        compatible_chan_mask |= SNM_FCH;
      }
      else
      {
        /* Mark only alternate reverse FCH RC has to be proposed */
        fch_alt_rc = SNM_REVERSE;
      }
    }
  }

  /* Check the final case of mismatched RC between FCH and DCCH for the case of
     at least one of the fundicated channel config is compatible with SO */
  if ((compatible_chan_mask != SNM_INIT_CHAN_MASK) &&
      ((fwd_scr_chan_mask | rev_scr_chan_mask) & SNM_DCCH) &&
      ((fwd_scr_chan_mask | rev_scr_chan_mask) & SNM_FCH))
  {
    /* Check the RC class and rate set */
    if (!snm_is_fundicated_rc_match(fwd_scr_chan_mask, rev_scr_chan_mask, cfg_ptr))
    {
      if (!suggest_alt)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Mismatch RC class or rate set, reject SCR");
        return SNM_REJECT_CFG;
      }
      else
      {
        boolean fwd_has_fch = (fwd_scr_chan_mask & SNM_FCH) ? TRUE : FALSE;
        boolean rev_has_fch = (rev_scr_chan_mask & SNM_FCH) ? TRUE : FALSE;
        boolean fwd_has_dcch = (fwd_scr_chan_mask & SNM_DCCH) ? TRUE : FALSE;
        boolean rev_has_dcch = (rev_scr_chan_mask & SNM_DCCH) ? TRUE : FALSE;

        /* DCCH config is compatible with SO */
        if (compatible_chan_mask & SNM_DCCH)
        {
          cfg_ptr->fch_cc_incl = TRUE;

          /* DCCH assignment is symmetric to FCH assignment, simply set RCs of FCH same as DCCH */
          if ((fwd_has_fch == fwd_has_dcch) &&
              (rev_has_fch == rev_has_dcch))
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Mismatch RC class or rate set, propose SCR");
            cfg_ptr->for_fch_rc = cfg_ptr->for_dcch_rc;
            cfg_ptr->rev_fch_rc = cfg_ptr->rev_dcch_rc;
          }

          /* DCCH assignment is asymmetric to FCH assignment, pick FCH RCs from RC lists */
          else
          {
            uint8 rate_set;

#ifdef FEATURE_IS2000_REL_B
            /* P_REV >= 8 supports mixed rate set on fundicated channels */
            if (P_REV_IS_8_OR_GREATER)
            {
              rate_set = SNM_RESERVED;  /*lint !e641 */
            }
            /* P_REV < 8 all fundicated channels must have the same rate set */
            else
#endif /* FEATURE_IS2000_REL_B */
            {
              rate_set = fwd_has_dcch ? RATE_SET_OF_FWD_RC(cfg_ptr->for_dcch_rc) :
                RATE_SET_OF_REV_RC(cfg_ptr->rev_dcch_rc);
            }

            if (!snm_gen_fundicated_rc(so_info, snm_con_index,
                (SNM_FORWARD | SNM_REVERSE), SNM_FCH, fwd_scr_chan_mask, rev_scr_chan_mask,
                rate_set, &cfg_ptr->for_fch_rc, &cfg_ptr->rev_fch_rc))
            {
              M1X_MSG( DCP, LEGACY_MED,
                "SO %d: None of FCH RC can be proposed, reject SCR",
                so_info->service_option_num);
              return SNM_REJECT_CFG;
            }
          }

          /* Both fwd and rev FCH RCs are good now */
          compatible_chan_mask |= SNM_FCH;
          status = SNM_ALT_CFG;
        }
        /* FCH config is compatible with SO */
        else
        {
          cfg_ptr->dcch_cc_incl = TRUE;

          /* FCH assignment is symmetric to DCCH assignment, simply set RCs of FCH same as DCCH */
          if ((fwd_has_fch == fwd_has_dcch) &&
              (rev_has_fch == rev_has_dcch))
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Mismatch RC class or rate set, propose SCR");
            cfg_ptr->for_dcch_rc = cfg_ptr->for_fch_rc;
            cfg_ptr->rev_dcch_rc = cfg_ptr->rev_fch_rc;
          }
          /* FCH assignment is asymmetric to DCCH assignment, pick FCH RCs from RC lists */
          else
          {
            uint8 rate_set;

#ifdef FEATURE_IS2000_REL_B
            /* P_REV >= 8 supports mixed rate set on fundicated channels */
            if (P_REV_IS_8_OR_GREATER)
            {
              rate_set = SNM_RESERVED; /*lint !e641 */
            }
            /* P_REV < 8 all fundicated channels must have the same rate set */
            else
#endif /* FEATURE_IS2000_REL_B */
            {
              rate_set = fwd_has_fch ? RATE_SET_OF_FWD_RC(cfg_ptr->for_fch_rc) :
                RATE_SET_OF_REV_RC(cfg_ptr->rev_fch_rc);
            }

            if (!snm_gen_fundicated_rc(so_info, snm_con_index,
                (SNM_FORWARD | SNM_REVERSE), SNM_DCCH, fwd_scr_chan_mask,
                rev_scr_chan_mask, rate_set, &cfg_ptr->for_dcch_rc,
                &cfg_ptr->rev_dcch_rc))
            {
              M1X_MSG( DCP, LEGACY_MED,
                "SO %d: None of DCCH RC can be proposed, reject SCR",
                so_info->service_option_num);
              return SNM_REJECT_CFG;
            }
          }

          /* Both fwd and rev DCCH RCs are good now */
          compatible_chan_mask |= SNM_DCCH;

          status = SNM_ALT_CFG;
        }
      }
    }
  }


  /* *****************************************************************************
  ** Figure out whether there is compatible forward and reverse TC channel for SO.
  ** ***************************************************************************** */

  /* Default fundicated channel is not needed for the SO */
  so_preferred_fundicated_chan_mask = SNM_INIT_CHAN_MASK;

  /* Fundicated channel is needed as both forward and reverse TC channel for SO */
  so_preferred_fundicated_chan_mask = (fwd_scr_chan_mask & rev_scr_chan_mask) &
      so_chan_mask & (SNM_FCH | SNM_DCCH);

  /* SO requires fundicated channel support but none of the fundicated RC is
     compatible with SO */
  if ((so_preferred_fundicated_chan_mask != SNM_INIT_CHAN_MASK) &&
      !(compatible_chan_mask & so_preferred_fundicated_chan_mask))
  {
    if (!suggest_alt)
    {
      if ((so_preferred_fundicated_chan_mask & SNM_DCCH) &&
          !(compatible_chan_mask & SNM_DCCH))
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad DCCH RCs: fwd %d, rev %d Alt: %d, reject SCR",
          cfg_ptr->for_dcch_rc,
          cfg_ptr->rev_dcch_rc,
          suggest_alt);
      }

      if ((so_preferred_fundicated_chan_mask & SNM_FCH) &&
          !(compatible_chan_mask & SNM_FCH))
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad FCH RCs: fwd %d, rev %d Alt: %d, reject SCR",
          cfg_ptr->for_fch_rc,
          cfg_ptr->rev_fch_rc,
          suggest_alt);
      }

      return SNM_REJECT_CFG;
    }

    /* Let's propose RC */
    else
    {
      /* Decide which fundicated channel's RC to be proposed */
      snm_chan_mask_type rc_proposal_chan_mask = so_preferred_fundicated_chan_mask;

      /* If SO supports both FCH and DCCH and SCR has both configs, pick the
         fundicated channel that MS did not try for RC proposal last time */
      if ((so_preferred_fundicated_chan_mask & SNM_FCH) &&
          (so_preferred_fundicated_chan_mask & SNM_DCCH))
      {
        /* Last time we did not propose DCCH RCs for this SO. */
        if (negotiation_info.con_list[snm_con_index].fundicated_rc_proposed_chan_mask ==
            SNM_DCCH)
        {
          /* Let's propose FCH RCs for this time */
          rc_proposal_chan_mask = SNM_FCH;
        }
        else
        {
          /* Let's propose DCCH RCs for this time */
          rc_proposal_chan_mask = SNM_DCCH;
        }
      }

      /* Record the channel that RC has been proposed */
      negotiation_info.con_list[snm_con_index].fundicated_rc_proposed_chan_mask =
        rc_proposal_chan_mask;

      /* Propose DCCH RCs */
      if (rc_proposal_chan_mask == SNM_DCCH)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad DCCH RCs: fwd %d, rev %d, propose SCR",
          cfg_ptr->for_dcch_rc,
          cfg_ptr->rev_dcch_rc);

        if (snm_gen_fundicated_rc(so_info, snm_con_index,
            dcch_alt_rc, SNM_DCCH, fwd_scr_chan_mask, rev_scr_chan_mask,
            SNM_RESERVED, &cfg_ptr->for_dcch_rc, &cfg_ptr->rev_dcch_rc))
        {
          cfg_ptr->dcch_cc_incl = TRUE;
          status = SNM_ALT_CFG;
        }
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "SO %d: None of DCCH RC can be proposed, reject SCR",
            so_info->service_option_num);
          return SNM_REJECT_CFG;
        }

        /* Both fwd and rev DCCH RCs are good now */
        compatible_chan_mask |= SNM_DCCH;
      }
      /* Propose FCH RCs */
      if (rc_proposal_chan_mask == SNM_FCH)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad FCH RCs: fwd %d, rev %d, propose SCR",
          cfg_ptr->for_fch_rc,
          cfg_ptr->rev_fch_rc);

        if (snm_gen_fundicated_rc(so_info, snm_con_index,
            fch_alt_rc, SNM_FCH, fwd_scr_chan_mask, rev_scr_chan_mask,
            SNM_RESERVED, &cfg_ptr->for_fch_rc, &cfg_ptr->rev_fch_rc))
        {
          cfg_ptr->fch_cc_incl = TRUE;
          status = SNM_ALT_CFG;
        }
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "SO %d: None of FCH RC can be proposed, reject SCR",
            so_info->service_option_num);
          return SNM_REJECT_CFG;
        }

        /* Both fwd and rev FCH RCs are good now */
        compatible_chan_mask |= SNM_FCH;
      }

      /* Check the final case of mismatch RC between FCH and DCCH */
      if (((fwd_scr_chan_mask | rev_scr_chan_mask) & SNM_FCH) &&
          ((fwd_scr_chan_mask | rev_scr_chan_mask) & SNM_DCCH))
      {
        /* Check the RC class */
        if (!snm_is_fundicated_rc_match(fwd_scr_chan_mask, rev_scr_chan_mask,
             cfg_ptr))
        {
          boolean fwd_has_fch = (fwd_scr_chan_mask & SNM_FCH) ? TRUE : FALSE;
          boolean rev_has_fch = (rev_scr_chan_mask & SNM_FCH) ? TRUE : FALSE;
          boolean fwd_has_dcch = (fwd_scr_chan_mask & SNM_DCCH) ? TRUE : FALSE;
          boolean rev_has_dcch = (rev_scr_chan_mask & SNM_DCCH) ? TRUE : FALSE;

          /* DCCH config is compatible with SO */
          if (compatible_chan_mask & SNM_DCCH)
          {
            cfg_ptr->fch_cc_incl = TRUE;

            /* DCCH assignment is symmetric to FCH assignment, simply set RCs of FCH same as DCCH */
            if ((fwd_has_fch == fwd_has_dcch) &&
                (rev_has_fch == rev_has_dcch))
            {
              cfg_ptr->for_fch_rc = cfg_ptr->for_dcch_rc;
              cfg_ptr->rev_fch_rc = cfg_ptr->rev_dcch_rc;
            }
            /* DCCH assignment is asymmetric to FCH assignment, pick FCH RCs from RC lists */
            else
            {
              uint8 rate_set;

#ifdef FEATURE_IS2000_REL_B
              /* P_REV >= 8 supports mixed rate set on fundicated channels */
              if (P_REV_IS_8_OR_GREATER)
              {
                rate_set = SNM_RESERVED;  /*lint !e641 */
              }
              /* P_REV < 8 all fundicated channels must have the same rate set */
              else
#endif /* FEATURE_IS2000_REL_B */
              {
                rate_set = fwd_has_dcch ? RATE_SET_OF_FWD_RC(cfg_ptr->for_dcch_rc) :
                  RATE_SET_OF_REV_RC(cfg_ptr->rev_dcch_rc);
              }

              if (!snm_gen_fundicated_rc(so_info, snm_con_index,
                  (SNM_FORWARD | SNM_REVERSE), SNM_FCH, fwd_scr_chan_mask,
                  rev_scr_chan_mask, rate_set, &cfg_ptr->for_fch_rc, &cfg_ptr->rev_fch_rc))
              {
                M1X_MSG( DCP, LEGACY_MED,
                  "SO %d: None of FCH RC can be proposed, reject SCR",
                  so_info->service_option_num);
                return SNM_REJECT_CFG;
              }
            }

            /* Both fwd and rev FCH RCs are good now */
            compatible_chan_mask |= SNM_FCH;

            status = SNM_ALT_CFG;
          }

          /* FCH config is compatible with SO */
          else
          {
            cfg_ptr->dcch_cc_incl = TRUE;

            /* FCH assignment is symmetric to DCCH assignment, simply set RCs of FCH same as DCCH */
            if ((fwd_has_fch == fwd_has_dcch) &&
                (rev_has_fch == rev_has_dcch))
            {
              cfg_ptr->for_dcch_rc = cfg_ptr->for_fch_rc;
              cfg_ptr->rev_dcch_rc = cfg_ptr->rev_fch_rc;
            }
            /* FCH assignment is asymmetric to DCCH assignment, pick DCCH RCs from RC lists */
            else
            {
              uint8 rate_set;

#ifdef FEATURE_IS2000_REL_B
              /* P_REV >= 8 supports mixed rate set on fundicated channels */
              if (P_REV_IS_8_OR_GREATER)
              {
                rate_set = SNM_RESERVED; /*lint !e641 */
              }
              /* P_REV < 8 all fundicated channels must have the same rate set */
              else
#endif /* FEATURE_IS2000_REL_B */
              {
                rate_set = fwd_has_fch ? RATE_SET_OF_FWD_RC(cfg_ptr->for_fch_rc) :
                  RATE_SET_OF_REV_RC(cfg_ptr->rev_fch_rc);
              }

              if (!snm_gen_fundicated_rc(so_info, snm_con_index,
                  (SNM_FORWARD | SNM_REVERSE), SNM_DCCH, fwd_scr_chan_mask,
                  rev_scr_chan_mask, rate_set, &cfg_ptr->for_dcch_rc, &cfg_ptr->rev_dcch_rc))
              {
                M1X_MSG( DCP, LEGACY_MED,
                  "SO %d: None of DCCH RC can be proposed, reject SCR",
                  so_info->service_option_num);
                return SNM_REJECT_CFG;
              }
            }

            /* Both fwd and rev FCH RCs are good now */
            compatible_chan_mask |= SNM_DCCH;

            status = SNM_ALT_CFG;
          }
        }
      }

    } /* end if (!suggest_alt) */
  } /* endif (!fch_rc_ompatible && !dcch_rc_compatible) */

  /* Continue the validation of DCCH config if RCs are ok */
  if (compatible_chan_mask & SNM_DCCH)
  {
    /* *********************
    ** Check DCCH frame size
    ** ********************* */
    if ((status = snm_eval_fundicated_frame_size(SNM_DCCH, cfg_ptr, suggest_alt,
        status)) == SNM_REJECT_CFG)
    {
      /* Bad frame size */
      return SNM_REJECT_CFG;
    }

    /* ***************
    ** Check DCCH MUX
    ** ************** */
    if ((status = snm_eval_fundicated_mux(SNM_DCCH, fwd_scr_chan_mask,
        rev_scr_chan_mask, cfg_ptr, so_info, suggest_alt, status)) == SNM_REJECT_CFG)
    {
      /* Bad MUXs */
      return SNM_REJECT_CFG;
    }

    /* Mark DCCH config verified */
    negotiation_info.dcch_cc_verified = TRUE;
  }

  /* Continue the validation of FCH config if RCs are ok */
  if (compatible_chan_mask & SNM_FCH)
  {
    /* ********************
    ** Check FCH frame size
    ** ******************** */
    if ((status = snm_eval_fundicated_frame_size(SNM_FCH, cfg_ptr, suggest_alt,
        status)) == SNM_REJECT_CFG)
    {
      /* Bad frame size */
      return SNM_REJECT_CFG;
    }

    /* *************
    ** Check FCH MUX
    ** ************* */
    if ((status = snm_eval_fundicated_mux(SNM_FCH, fwd_scr_chan_mask,
        rev_scr_chan_mask, cfg_ptr, so_info, suggest_alt, status)) == SNM_REJECT_CFG)
    {
      /* Bad MUXs */
      return SNM_REJECT_CFG;
    }

    /* ****************************
    ** Check FCH transmission rates
    ** **************************** */
    if ((status = snm_eval_fch_rates(fwd_scr_chan_mask, rev_scr_chan_mask, cfg_ptr,
        so_info, suggest_alt, status)) == SNM_REJECT_CFG)
    {
      /* Bad rates */
      return SNM_REJECT_CFG;
    }

    /* Mark FCH config verified */
    negotiation_info.fch_cc_verified = TRUE;
  }

  /* Good enough to move on since we do final SO RC check with LPM in
     NNSCR validation. */
  return status;


} /* snm_eval_fundicated_config */

/*===========================================================================

FUNCTION snm_eval_traffic_type

DESCRIPTION
  Evaluates the traffic types of a SO connection in a configuration to see
  if it is acceptable. Proposes alterate traffic types if alt cfg is needed
  and cfg is not acceptable. The SO number of the connection is assumed to be
  valid.

DEPENDENCIES
  None

RETURN VALUE
  SNM_ACCEPT_CFG: If Cfg is accepted.
  SNM_REJECT_CFG: If it is rejected,
  SNM_ALT_CFG:    If alt cfg is proposed

SIDE EFFECTS
  None

===========================================================================*/

static snm_eval_status_type snm_eval_traffic_type
(
  caii_srv_cfg_type *cfg_ptr,
    /* The service configuration to be evaluated and modified if alt cfg
       is needed */
  byte con_index,
    /* Index to the SO connection to be evaluated */
  snm_so_descriptor_type *so_info,
    /* The SO descriptor to evaluate against */
  boolean suggest_alt,
    /* Is an alternate cfg needed */
  snm_eval_status_type curr_status
)
{
  snm_eval_status_type status = curr_status;
  boolean bad_cfg = FALSE;

  if (
      #ifdef FEATURE_IS2000_REL_A
      !P_REV_IS_7_OR_GREATER &&
      #endif
      ((cfg_ptr->con[con_index].fwd_traf >= CAI_SEC_TRAF) ||  /*lint !e641 */
       (cfg_ptr->con[con_index].rev_traf >= CAI_SEC_TRAF)))   /*lint !e641 */
  {
    bad_cfg = TRUE;
  }

#ifdef FEATURE_IS2000_REL_C
  /* P_REV >= 9, don't support traffic that does not classify as NULL,
     primary,secondary or signaling traffic. */
  if (P_REV_IS_9_OR_GREATER &&
      ((cfg_ptr->con[con_index].fwd_traf >= CAI_UNCLASSIFIED_TRAF) ||  /*lint !e641 */
       (cfg_ptr->con[con_index].rev_traf >= CAI_UNCLASSIFIED_TRAF)))   /*lint !e641 */
  {
    bad_cfg = TRUE;
  }
#endif /* FEATURE_IS2000_REL_C */

  /* P_REV < 9, don't support traffic that does not classify as NULL,
     primary or secondary traffic. */
  if (
#ifdef FEATURE_IS2000_REL_C
      !P_REV_IS_9_OR_GREATER &&
#endif /* !FEATURE_IS2000_REL_C */
      ((cfg_ptr->con[con_index].fwd_traf >= CAI_SIG_TRAF) ||  /*lint !e641 */
       (cfg_ptr->con[con_index].rev_traf >= CAI_SIG_TRAF))    /*lint !e641 */
     )
  {
    bad_cfg = TRUE;
  }

  /* If we expect traffic but BS propose the wrong traffic type or NULL, reject
     Note: we allow the case when we expect NULL but BS proposes traffic
     because it's harmless */
  if (so_info->default_config.forward.allowed_traffic_type &&
      !(cfg_ptr->con[con_index].fwd_traf &
        so_info->default_config.forward.allowed_traffic_type)) /*lint !e641 */
  {
    bad_cfg = TRUE;
  }

  if (so_info->default_config.reverse.allowed_traffic_type &&
      !(cfg_ptr->con[con_index].rev_traf &
        so_info->default_config.reverse.allowed_traffic_type))  /*lint !e641 */
  {
    bad_cfg = TRUE;
  }

#ifdef FEATURE_IS2000_REL_A_SVD
  /* Check for possible clash of traffic types */
  if (cfg_ptr->num_con > 1)
  {
    if (
        /* fwd_traf type already present in this configuration in another
           con_index */
        ((cfg_ptr->con[con_index].fwd_traf &
          negotiation_info.fwd_traf.traffic_type_eval_so_far) &&
         (con_index != ((cfg_ptr->con[con_index].fwd_traf == CAI_PRI_TRAF) ? /*lint !e641 */
                        negotiation_info.fwd_traf.cfg_pri_con_index :
                        negotiation_info.fwd_traf.cfg_sec_con_index)))
        ||
        /* rev_traf already present in this configuration in another
           con_index */
        ((cfg_ptr->con[con_index].rev_traf &
          negotiation_info.rev_traf.traffic_type_eval_so_far) &&
         (con_index != ((cfg_ptr->con[con_index].rev_traf == CAI_PRI_TRAF) ?  /*lint !e641 */
                        negotiation_info.rev_traf.cfg_pri_con_index :
                        negotiation_info.rev_traf.cfg_sec_con_index))))
    {
      bad_cfg = TRUE;
    }
  }
#endif /* FEATURE_IS2000_REL_A_SVD */

  if (bad_cfg)
  {
    if (!suggest_alt)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Bad traffic type Fwd:%d Rev:%d, reject SCR",
        cfg_ptr->con[con_index].fwd_traf,
        cfg_ptr->con[con_index].rev_traf);
      return SNM_REJECT_CFG;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Bad traffic type Fwd:%d Rev:%d, propose SCR",
        cfg_ptr->con[con_index].fwd_traf,
        cfg_ptr->con[con_index].rev_traf);

      /* Fwd traf type */
      if (so_info->default_config.forward.allowed_traffic_type ==
          CAI_PRI_OR_SEC_TRAF)  /*lint !e641 */
      {
#ifdef FEATURE_IS2000_REL_A_SVD
        if ((cfg_ptr->num_con > 1) &&
             (!((negotiation_info.fwd_traf.traffic_type_eval_so_far &
                 CAI_SEC_TRAF) &&  /*lint !e641 */
                (con_index != negotiation_info.fwd_traf.cfg_sec_con_index)
               )
             )
           )
        {
          /* If we have a choice and the SEC is not taken, we'll take it */
          cfg_ptr->con[con_index].fwd_traf = CAI_SEC_TRAF;  /*lint !e641 */
        }
        else
#endif/* FEATURE_IS2000_REL_A_SVD */
        {
          cfg_ptr->con[con_index].fwd_traf = CAI_PRI_TRAF;  /*lint !e641 */
        }
      }
      else
      {
        cfg_ptr->con[con_index].fwd_traf =
          so_info->default_config.forward.allowed_traffic_type;  /*lint !e641 */
      }

      /* Rev traf type */
      if (so_info->default_config.reverse.allowed_traffic_type ==
          CAI_PRI_OR_SEC_TRAF) /*lint !e641 */
      {
#ifdef FEATURE_IS2000_REL_A_SVD
        if ((cfg_ptr->num_con > 1) &&
             (!((negotiation_info.rev_traf.traffic_type_eval_so_far &
                 CAI_SEC_TRAF) && /*lint !e641 */
                (con_index != negotiation_info.rev_traf.cfg_sec_con_index)
               )
             )
           )
        {
          /* If we have a choice and the SEC is not taken, we'll take it */
          cfg_ptr->con[con_index].rev_traf = CAI_SEC_TRAF; /*lint !e641 */
        }
        else
#endif /* FEATURE_IS2000_REL_A_SVD */
        {
          cfg_ptr->con[con_index].rev_traf = CAI_PRI_TRAF; /*lint !e641 */
        }
      }
      else
      {
        cfg_ptr->con[con_index].rev_traf =
          so_info->default_config.reverse.allowed_traffic_type;  /*lint !e641 */
      }

      status = SNM_ALT_CFG;
    }
  } /* end if (bad_cfg) */

#ifdef FEATURE_IS2000_REL_A_SVD
  /* Mark and remember traffic types being used */
  if (cfg_ptr->num_con > 1)
  {
    /* fwd traffic type */
    negotiation_info.fwd_traf.traffic_type_eval_so_far |=
      cfg_ptr->con[con_index].fwd_traf;

    if (cfg_ptr->con[con_index].fwd_traf == CAI_PRI_TRAF)  /*lint !e641 */
    {
      negotiation_info.fwd_traf.cfg_pri_con_index = con_index;
    }
    else if (cfg_ptr->con[con_index].fwd_traf == CAI_SEC_TRAF)  /*lint !e641 */
    {
      negotiation_info.fwd_traf.cfg_sec_con_index = con_index;
    }

    /* rev traffic type */
    negotiation_info.rev_traf.traffic_type_eval_so_far |=
      cfg_ptr->con[con_index].rev_traf;

    if (cfg_ptr->con[con_index].rev_traf == CAI_PRI_TRAF)  /*lint !e641 */
    {
      negotiation_info.rev_traf.cfg_pri_con_index = con_index;
    }
    else if (cfg_ptr->con[con_index].rev_traf == CAI_SEC_TRAF)  /*lint !e641 */
    {
      negotiation_info.rev_traf.cfg_sec_con_index = con_index;
    }
  }
#endif /* FEATURE_IS2000_REL_A_SVD */

  return status;

} /* snm_eval_traffic_type */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION snm_eval_qos

DESCRIPTION
  Evaluates the QoS of a SO connection in a configuration to see
  if it is acceptable. Proposes alterate traffic types if alt cfg is needed
  and cfg is not acceptable. The SO number of the connection is assumed to be
  valid.

DEPENDENCIES
  None

RETURN VALUE
  SNM_ACCEPT_CFG: If Cfg is accepted.
  SNM_REJECT_CFG: If it is rejected,
  SNM_ALT_CFG:    If alt cfg is proposed

SIDE EFFECTS
  None

===========================================================================*/

static snm_eval_status_type snm_eval_qos
(
  caii_srv_cfg_type *cfg_ptr,
    /* The service configuration to be evaluated and modified if alt cfg
       is needed */
  byte con_index,
    /* Index to the SO connection to be evaluated */
  snm_so_descriptor_type *so_info,
    /* The SO descriptor to evaluate against */
  boolean suggest_alt,
    /* Is an alternate cfg needed */
  snm_eval_status_type curr_status
)
{
  snm_eval_status_type status = curr_status;
  snm_eval_status_type qos_eval_status;

  if (cfg_ptr->con[con_index].qos_parms_incl &&
      /* QOS proposed by BS */
      so_info->qos_blob_eval)
      /* we have function to evaluate it */
  {
    if ((qos_eval_status = so_info->qos_blob_eval(
         cfg_ptr->con[con_index].qos_parms_len,
         cfg_ptr->con[con_index].qos_parms,
         suggest_alt, &(cfg_ptr->con[con_index].qos_parms_len),
         cfg_ptr->con[con_index].qos_parms)) == SNM_REJECT_CFG)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "QoS blob not acceptable, reject SCR");
      return SNM_REJECT_CFG;
    }
    else
    {

      /* Sanity Check */
      if (!suggest_alt && (qos_eval_status == SNM_ALT_CFG))
      {
        /* This is possible corruption of the configuration */
        ERR_FATAL("Configuration may have been corrupted", 0, 0, 0);
      }

      if (qos_eval_status == SNM_ALT_CFG)
      {
        /* Sanity Check */
        if (cfg_ptr->con[con_index].qos_parms_len >
            sizeof(cfg_ptr->con[con_index].qos_parms))
        {
          ERR_FATAL("Data corruption by QOS generator, len %d",
            cfg_ptr->con[con_index].qos_parms_len, 0, 0);
        }

        M1X_MSG( DCP, LEGACY_MED,
          "QoS blob not acceptable, propose SCR");

        if (cfg_ptr->con[con_index].qos_parms_len == 0)
        {
          /* In this case, we need to set qos_parms_incl to false */
          cfg_ptr->con[con_index].qos_parms_incl = FALSE;
        }

        /* We'll need to suggest the alternative QOS state given by Data
           Services */
        status = SNM_ALT_CFG;
      }
    }
  } /* end if (qos_parms_incl && qos_blob_eval */

  /* If this is a SREQ or SRSP and we support QOS for this SO and it has not
     been included by the BS, we should propose it here.
     This will occur if the BS ignored the QOS we send in OM/EOM in s
     ubsequent SREQ/SRSP messages */
  if (!cfg_ptr->con[con_index].qos_parms_incl &&
      so_info->qos_blob_generator && suggest_alt)
  {
    /* Generate QOS blob */
    cfg_ptr->con[con_index].qos_parms_len =
      so_info->qos_blob_generator(cfg_ptr->con[con_index].qos_parms);

    if (cfg_ptr->con[con_index].qos_parms_len >
        sizeof(cfg_ptr->con[con_index].qos_parms))
    {
      ERR_FATAL("Data corruption by QOS generator, len %d",
        cfg_ptr->con[con_index].qos_parms_len, 0, 0);
    }

    /* There is a QOS blob */
    if (cfg_ptr->con[con_index].qos_parms_len > 0)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "MS propose QoS");
      cfg_ptr->con[con_index].qos_parms_incl = TRUE;
      status = SNM_ALT_CFG;
    }
  }

  return status;

} /* snm_eval_qos */
#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================

FUNCTION snm_eval_sr_id

DESCRIPTION
  Evaluates the SR_ID of a SO connection in a configuration to see
  if it is acceptable. Proposes alterate traffic types if alt cfg is needed
  and cfg is not acceptable. The SO number of the connection is assumed to be
  valid.

DEPENDENCIES
  None

RETURN VALUE
  SNM_ACCEPT_CFG: If Cfg is accepted.
  SNM_REJECT_CFG: If it is rejected,
  SNM_ALT_CFG:    If alt cfg is proposed

SIDE EFFECTS
  None

===========================================================================*/

static snm_eval_status_type snm_eval_sr_id
(
  caii_srv_cfg_type *cfg_ptr,
    /* The service configuration to be evaluated and modified if alt cfg
       is needed */
  byte con_index,
    /* Index to the SO connection to be evaluated */
  byte snm_con_index,
    /* connection index in negotiation_info.con_list[] */
  boolean suggest_alt,
    /* Is an alternate cfg needed */
  snm_eval_status_type curr_status
)
{
  snm_eval_status_type status = curr_status;

  /* SR_ID is defined from 1 to 6 only.  Other values are invalid.
     However the standards group is proposing a change such that signalling
     service options will use SR_ID of 0.  Before we have a perminent fix
     for this, let's just use an interim solution of checking for all
     signalling service options here now. */
  if ((!CAI_IS_SO_SIGNALING(cfg_ptr->con[con_index].so) &&
       (cfg_ptr->con[con_index].sr_id == 0)) ||
      (cfg_ptr->con[con_index].sr_id > MAX_SR_ID_VALUE))
  {
    if (!suggest_alt
#ifdef FEATURE_IS2000_REL_A_SVD
        || (cfg_ptr->num_con > 1)
           /* Right now, we can't correct sr_ids for SVD */
#endif /* FEATURE_IS2000_REL_A_SVD */
       )
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Invalid SR ID %d, reject SCR",
        cfg_ptr->con[con_index].sr_id);
      return SNM_REJECT_CFG;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Invalid SR ID %d, propose SCR",
        cfg_ptr->con[con_index].sr_id);

#ifdef FEATURE_IS2000_REL_A
      if (cdma.call_orig)
      {
        cfg_ptr->con[con_index].sr_id = cdma.sr_id;
      }
      else
#endif /* FEATURE_IS2000_REL_A */
      {
        cfg_ptr->con[con_index].sr_id = CAI_SR_ID_1;  /*lint !e641 */
      }

      /* Also store the sr_id in our negotiation_info database */
      negotiation_info.con_list[snm_con_index].sr_id =
        cfg_ptr->con[con_index].sr_id;
      negotiation_info.con_list[snm_con_index].sr_id_valid = TRUE;

      status = SNM_ALT_CFG;
    }
  }

  return status;

} /* snm_eval_sr_id */

/*===========================================================================

FUNCTION snm_eval_sch_rc

DESCRIPTION
  Evaluates the RC of SCH config to see if it is acceptable. Proposes
  alterate SCH RC if alt cfg is needed and cfg is not acceptable.

DEPENDENCIES
  None

RETURN VALUE
  SNM_ACCEPT_CFG: If Cfg is accepted.
  SNM_REJECT_CFG: If it is rejected,
  SNM_ALT_CFG:    If alt cfg is proposed

SIDE EFFECTS
  None

===========================================================================*/

static snm_eval_status_type snm_eval_sch_rc
(
  snm_fwd_rev_type link_mask,
    /* Mask indicates whether forward or reverse SCH's RC to be verified */
  snm_chan_mask_type fwd_scr_chan_mask,
    /* Mask indicates active forward channel configuartions */
  snm_chan_mask_type rev_scr_chan_mask,
    /* Mask indicates active reverse channel configuartions */
  caii_srv_cfg_type *cfg_ptr,
    /* The service configuration to be evaluated and modified if alt cfg
       is needed */
  uint8 sch_index,
    /* Index to SCH config */
  snm_so_descriptor_type *so_info,
    /* The SO descriptor to evaluate against */
  boolean suggest_alt,
    /* Is an alternate cfg needed */
  snm_eval_status_type curr_status
)
{
  snm_eval_status_type status = curr_status;
  uint8 i;
  boolean bad_cfg = TRUE;

  if ((link_mask != SNM_FORWARD) && (link_mask != SNM_REVERSE))
  {
    /* Shall never happen */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid channel link specified %d",
      link_mask);
    return SNM_REJECT_CFG;
  }

  /* Check the valid range for sch_index value to fix the Klocwork error */
  if (sch_index >=  CAI_MAX_NUM_FOR_SCH)
  {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Invalid SCH Index value %d",
        sch_index);
      return SNM_REJECT_CFG;
  }

  /* Check forward SCH RC */
  if (link_mask == SNM_FORWARD)
  {
    if ((cfg_ptr->for_sch[sch_index].sch_chn_cfg.sch_rc <= (byte)CAI_RC_11) &&
        (RC_CLASS(cfg_ptr->for_sch[sch_index].sch_chn_cfg.sch_rc) > 1))
        /* RC class of SCH cannot be less than 1 */
    {
      /* We allow for Mixed Rate Sets. Check to see if the SCH RC is on
         the SO list of RCs */
      for (i=0; i < so_info->negotiation_pars.fwd_rc.num_of_rc; i++)
      {
        if (cfg_ptr->for_sch[sch_index].sch_chn_cfg.sch_rc ==
            so_info->negotiation_pars.fwd_rc.list[i])  /*lint !e641 */
        {
          bad_cfg = FALSE;
          break;
        }
      } /* end for */
    }

    if (bad_cfg)
    {
      if (!suggest_alt)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad F-SCH RC %d, reject SCR",
          cfg_ptr->for_sch[sch_index].sch_chn_cfg.sch_rc);
        return SNM_REJECT_CFG;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad F-SCH RC %d, propose SCR",
          cfg_ptr->for_sch[sch_index].sch_chn_cfg.sch_rc);

        if ((fwd_scr_chan_mask & SNM_FCH) && negotiation_info.fch_cc_verified)
        {
          cfg_ptr->for_sch[sch_index].sch_chn_cfg.sch_rc = cfg_ptr->for_fch_rc;
        }

        else if ((fwd_scr_chan_mask & SNM_DCCH) && negotiation_info.dcch_cc_verified)
        {
          cfg_ptr->for_sch[sch_index].sch_chn_cfg.sch_rc = cfg_ptr->for_dcch_rc;
        }

        else
        {
          cfg_ptr->for_sch[sch_index].sch_chn_cfg.sch_rc = CAI_RC_3;  /*lint !e641 */
        }

        status = SNM_ALT_CFG;
      }
    }
  }
  /* Check reverse SCH RC */
  else
  {
    if ((cfg_ptr->rev_sch[sch_index].sch_chn_cfg.sch_rc <= (byte)CAI_RC_8) &&
        (RC_CLASS(cfg_ptr->rev_sch[sch_index].sch_chn_cfg.sch_rc) > 1))
        /* RC class of SCH cannot be less than 1 */
    {
      /* We allow mixed rate sets. Check to see if the SCH RC is on the SO
         list of RCs */
      for (i = 0; i < so_info->negotiation_pars.rev_rc.num_of_rc; i++)
      {
        if (cfg_ptr->rev_sch[sch_index].sch_chn_cfg.sch_rc ==
            so_info->negotiation_pars.rev_rc.list[i])  /*lint !e641 */
        {
          bad_cfg = FALSE;
          break;
        }
      } /* end for */
    }

    if (bad_cfg)
    {
      if (!suggest_alt)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad R-SCH RC %d, reject SCR",
          cfg_ptr->rev_sch[sch_index].sch_chn_cfg.sch_rc);
        return SNM_REJECT_CFG;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad R-SCH RC %d, propose SCR",
          cfg_ptr->rev_sch[sch_index].sch_chn_cfg.sch_rc);

        if ((rev_scr_chan_mask & SNM_FCH) && negotiation_info.fch_cc_verified)
        {
          cfg_ptr->rev_sch[sch_index].sch_chn_cfg.sch_rc = cfg_ptr->for_fch_rc;
        }

        else if ((rev_scr_chan_mask & SNM_DCCH) && negotiation_info.dcch_cc_verified)
        {
          cfg_ptr->rev_sch[sch_index].sch_chn_cfg.sch_rc = cfg_ptr->for_dcch_rc;
        }

        else
        {
          cfg_ptr->rev_sch[sch_index].sch_chn_cfg.sch_rc = CAI_RC_3;  /*lint !e641 */
        }

        status = SNM_ALT_CFG;
      }
    }
  }

  return status;

} /* snm_eval_sch_rc */

/*===========================================================================

FUNCTION snm_eval_sch_mux

DESCRIPTION
  Evaluates the MUX of SCH config to see if it is acceptable. Proposes
  alterate SCH MUX if alt cfg is needed and cfg is not acceptable.

DEPENDENCIES
  None

RETURN VALUE
  SNM_ACCEPT_CFG: If Cfg is accepted.
  SNM_REJECT_CFG: If it is rejected,
  SNM_ALT_CFG:    If alt cfg is proposed

SIDE EFFECTS
  None

===========================================================================*/

static snm_eval_status_type snm_eval_sch_mux
(
  snm_fwd_rev_type link_mask,
    /* Mask indicates whether forward or reverse SCH's MUX to be verified */
  caii_srv_cfg_type *cfg_ptr,
    /* The service configuration to be evaluated and modified if alt cfg
       is needed */
  uint8 sch_index,
    /* Index to SCH config */
  snm_so_descriptor_type *so_info,
    /* The SO descriptor to evaluate against */
  byte snm_con_index,
    /* connection index in negotiation_info.con_list[] */
  boolean suggest_alt,
    /* Is an alternate cfg needed */
  snm_eval_status_type curr_status
)
{
  snm_eval_status_type status = curr_status;
  uint8 i;
  boolean bad_cfg = TRUE;

  if ((link_mask != SNM_FORWARD) && (link_mask != SNM_REVERSE))
  {
    /* Shall never happen */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid channel link specified %d",
      link_mask);
    return SNM_REJECT_CFG;
  }

  /* Check the valid range for sch_index value to fix the Klocwork error */
  if (sch_index >= CAI_MAX_NUM_FOR_SCH)
  {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Invalid SCH Index value %d",
        sch_index);
      return SNM_REJECT_CFG;
  }

  /* Check forward SCH MUX */
  if (link_mask == SNM_FORWARD)
  {
    for (i = 0; i < so_info->negotiation_pars.fwd_sch_mo_list.num_mos ; i++)
    {
      if (so_info->negotiation_pars.fwd_sch_mo_list.list[i] ==
          cfg_ptr->for_sch[sch_index].for_sch_mux)
      {
        bad_cfg = FALSE;
        break;
      }
    }

    if (((cfg_ptr->for_sch[sch_index].for_sch_mux <= CAI_MUX16) ||
         (cfg_ptr->for_sch[sch_index].for_sch_mux & CAI_MO_RATE_SET_MASK)) &&
           /* Not a mux pdu type 5 */
        (RATE_SET_OF_FWD_RC(cfg_ptr->for_sch[sch_index].sch_chn_cfg.sch_rc) !=
         (MUX_TO_RATE_SET(cfg_ptr->for_sch[sch_index].for_sch_mux) & 1)))
           /* Check if the SCH mux option and SCH RCs are in the same rate set */
    {
      bad_cfg = TRUE;
    }

#ifdef FEATURE_IS2000_REL_A
    if (!P_REV_IS_7_OR_GREATER &&
        (cfg_ptr->for_sch[sch_index].for_sch_mux > CAI_MUX16) &&
        (((cfg_ptr->for_sch[sch_index].for_sch_mux & CAI_MO_MUX_PDU_TYPE_MASK) >> 10) ==
         CAI_MUX_PDU_TYPE_5))  /*lint !e641 */
    {
      /* Mux Option indicates MuxPDU 5. Not legal for P_REV < 7 */
      bad_cfg = TRUE;
    }
#endif /* FEATURE_IS2000_REL_A */

    if (bad_cfg)
    {
      if (!suggest_alt)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad F-SCH mux 0x%x, reject SCR",
          cfg_ptr->for_sch[sch_index].for_sch_mux);
        return SNM_REJECT_CFG;
      }
      /* Propose alt MUX agsinst supported SO */
      else
      {
        boolean found = FALSE;
        /* Init to 0 to handle cases with invalid value of snm_con_index 
         * so that mux_index is not used uninitialized */
        uint8 mux_index = 0;

        M1X_MSG( DCP, LEGACY_MED,
          "Bad F-SCH mux 0x%x, propose SCR",
          cfg_ptr->for_sch[sch_index].for_sch_mux);

        /* Choose the next mux for our table */
        if (snm_con_index == SNM_RESERVED) /*lint !e641 */
        {
          /* Not validate against SO connection, start from first MUX entry */
          mux_index = 0;
        }
        else if (snm_con_index < MAX_SNM_CON)
        {
          /* Validate against SO connection, start from MUX entry not referenced by last time */
          mux_index = negotiation_info.con_list[snm_con_index].cur_fwd_sch_mux_index;
        }

        for (i = 0; i < so_info->negotiation_pars.fwd_sch_mo_list.num_mos ; i++)
        {
          if ((so_info->negotiation_pars.fwd_sch_mo_list.list[mux_index] &
               CAI_MO_RATE_SET_MASK) &&
              (RATE_SET_OF_FWD_RC(cfg_ptr->for_sch[sch_index].sch_chn_cfg.sch_rc) !=
               (MUX_TO_RATE_SET(so_info->negotiation_pars.fwd_sch_mo_list.list[mux_index]) & 1)))
          {
            mux_index = (mux_index + 1) %
              so_info->negotiation_pars.fwd_sch_mo_list.num_mos;
            continue;
          }

          cfg_ptr->for_sch[sch_index].for_sch_mux =
            so_info->negotiation_pars.fwd_sch_mo_list.list[mux_index];

          if ((snm_con_index != SNM_RESERVED) && 
              (snm_con_index < MAX_SNM_CON))  /*lint !e641 */
          {
            /* Validate against SO connection, record MUX entry last referenced */
            negotiation_info.con_list[snm_con_index].cur_fwd_sch_mux_index =
              (++mux_index) % so_info->negotiation_pars.fwd_sch_mo_list.num_mos;
          }

          status = SNM_ALT_CFG;
          found = TRUE;
          break;
        }

        if (!found)
        {
          /* Cannot find proper mux option, so don't propose any SCH */
          cfg_ptr->for_sch_cc_incl = FALSE;
          status = SNM_ALT_CFG;
        }
      }
    }
  }
  /* Check reverse SCH MUX */
  else
  {
    for (i = 0; i < so_info->negotiation_pars.rev_sch_mo_list.num_mos ; i++)
    {
      if (so_info->negotiation_pars.rev_sch_mo_list.list[i] ==
          cfg_ptr->rev_sch[sch_index].rev_sch_mux)
      {
        bad_cfg = FALSE;
        break;
      }
    }

    if (((cfg_ptr->rev_sch[sch_index].rev_sch_mux <= CAI_MUX16) ||
         (cfg_ptr->rev_sch[sch_index].rev_sch_mux & CAI_MO_RATE_SET_MASK)) &&
           /* Not a mux pdu type 5 */
        (RATE_SET_OF_REV_RC(cfg_ptr->rev_sch[sch_index].sch_chn_cfg.sch_rc) !=
         (MUX_TO_RATE_SET(cfg_ptr->rev_sch[sch_index].rev_sch_mux) & 1)))
           /* Check if the SCH mux option and SCH RCs are in the same rate set */
    {
      bad_cfg = TRUE;
    }

#ifdef FEATURE_IS2000_REL_A
    if (!P_REV_IS_7_OR_GREATER &&
        (cfg_ptr->rev_sch[sch_index].rev_sch_mux > CAI_MUX16) &&
        (((cfg_ptr->rev_sch[sch_index].rev_sch_mux & CAI_MO_MUX_PDU_TYPE_MASK) >> 10) ==
         CAI_MUX_PDU_TYPE_5))  /*lint !e641 */
    {
      /* Mux Option indicates MuxPDU 5. Not legal for P_REV < 7 */
      bad_cfg = TRUE;
    }
#endif /* FEATURE_IS2000_REL_A */

    if (bad_cfg)
    {
      if (!suggest_alt)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad R-SCH mux 0x%x, reject SCR",
          cfg_ptr->rev_sch[sch_index].rev_sch_mux);
        return SNM_REJECT_CFG;
      }
      /* Propose alt MUX agsinst supported SO */
      else
      {
        boolean found = FALSE;
        /* Init to 0 to handle cases with invalid value of snm_con_index 
         * so that mux_index is not used uninitialized */
        uint8 mux_index = 0;

        M1X_MSG( DCP, LEGACY_MED,
          "Bad R-SCH mux 0x%x, propose SCR",
          cfg_ptr->rev_sch[sch_index].rev_sch_mux);

        /* Choose the next mux for our table */
        if (snm_con_index == SNM_RESERVED)  /*lint !e641 */
        {
          /* Not validate against SO connection, start from first MUX entry */
          mux_index = 0;
        }
        else if (snm_con_index < MAX_SNM_CON)
        {
          /* Validate against SO connection, start from MUX entry not referenced by last time */
          mux_index = negotiation_info.con_list[snm_con_index].cur_rev_sch_mux_index;
        }

        for (i = 0; i < so_info->negotiation_pars.rev_sch_mo_list.num_mos ; i++)
        {
          if ((so_info->negotiation_pars.rev_sch_mo_list.list[mux_index] &
               CAI_MO_RATE_SET_MASK) &&
              (RATE_SET_OF_REV_RC(cfg_ptr->rev_sch[sch_index].sch_chn_cfg.sch_rc) !=
               (MUX_TO_RATE_SET(so_info->negotiation_pars.rev_sch_mo_list.list[mux_index]) & 1)))
          {
            mux_index = (mux_index + 1) %
              so_info->negotiation_pars.rev_sch_mo_list.num_mos;
            continue;
          }

          cfg_ptr->rev_sch[sch_index].rev_sch_mux =
            so_info->negotiation_pars.rev_sch_mo_list.list[mux_index];

          if ((snm_con_index != SNM_RESERVED) && 
              (snm_con_index < MAX_SNM_CON))  /*lint !e641 */
          {
            /* Validate against SO connection, mark MUX entry not referenced */
            negotiation_info.con_list[snm_con_index].cur_rev_sch_mux_index =
              (++mux_index) % so_info->negotiation_pars.rev_sch_mo_list.num_mos;
          }

          status = SNM_ALT_CFG;
          found = TRUE;
          break;
        }

        if (!found)
        {
          /* Cannot find proper mux option, so don't propose any SCH */
          cfg_ptr->rev_sch_cc_incl = FALSE;
          status = SNM_ALT_CFG;
        }
      }
    }
  }

  return status;

} /* snm_eval_sch_mux */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION snm_eval_sch_max_rate

DESCRIPTION
  Evaluates the max rate of SCH config to see if it is acceptable. Proposes
  alterate SCH max rate if alt cfg is needed and cfg is not acceptable.

DEPENDENCIES
  None

RETURN VALUE
  SNM_ACCEPT_CFG: If Cfg is accepted.
  SNM_REJECT_CFG: If it is rejected,
  SNM_ALT_CFG:    If alt cfg is proposed

SIDE EFFECTS
  None

===========================================================================*/

static snm_eval_status_type snm_eval_sch_max_rate
(
  snm_fwd_rev_type link_mask,
    /* Mask indicates whether forward or reverse SCH's MUX to be verified */
  caii_srv_cfg_type *cfg_ptr,
    /* The service configuration to be evaluated and modified if alt cfg
       is needed */
  uint8 sch_index,
    /* Index to SCH config */
  boolean suggest_alt,
    /* Is an alternate cfg needed */
  snm_eval_status_type curr_status
)
{
  snm_eval_status_type status = curr_status;
  caii_for_sch_spec_fields_type fwd_sch_fields;
    /* For SCH fields from MCCAP */
  caii_rev_sch_spec_fields_type rev_sch_fields;
    /* Rev SCH fields from MCCAP */
  uint8 max_rate, *sch_max_rate_ptr;
  uint16 sch_mux;

  if ((link_mask != SNM_FORWARD) && (link_mask != SNM_REVERSE))
  {
    /* Shall never happen */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid channel link specified %d",
      link_mask);
    return SNM_REJECT_CFG;
  }

  if (link_mask == SNM_FORWARD)
  {
    sch_max_rate_ptr = &cfg_ptr->for_sch[sch_index].sch_chn_cfg.max_rate;
    sch_mux = cfg_ptr->for_sch[sch_index].for_sch_mux;

    /* MUX PDU 5 */
    if (sch_mux == 0xf20)
    {
      /* Retrieve MS capability details for SCH */
      (void) mccap_get_ch_specific_fields(MCCAP_FWD_SCH, (void *) &fwd_sch_fields);
      max_rate = fwd_sch_fields.for_sch_recs[0].for_max_rate;
    }
    /* Not MUX PDU 5 */
    else
    {
      max_rate = snm_get_max_sch_rate_from_mux(sch_mux);
    }
  }
  else
  {
    sch_max_rate_ptr = &cfg_ptr->rev_sch[sch_index].sch_chn_cfg.max_rate;
    sch_mux = cfg_ptr->rev_sch[sch_index].rev_sch_mux;

    /* MUX PDU 5 */
    if (sch_mux == 0xf20)
    {
      /* Retrieve MS capability details for SCH */
      (void) mccap_get_ch_specific_fields(MCCAP_REV_SCH, (void *) &rev_sch_fields);
      max_rate = rev_sch_fields.rev_sch_recs[0].rev_max_rate;
    }
    /* MUX PDU 3 */
    else
    {
      max_rate = snm_get_max_sch_rate_from_mux(sch_mux);
    }
  }

  /* Check SCH rate */
  if (((sch_mux == 0xf20) && (*sch_max_rate_ptr > max_rate))
      /* For mux pdu type 5 make sure the max rate is set to something
         we can handle */
      || ((sch_mux != 0xf20) && (*sch_max_rate_ptr != max_rate)))
          /* For mux pdu type 3 max rate is implied in the mux option itself
             just make sure the value given in the max rate is consistent */
  {
    if (suggest_alt)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Bad %d max_rate %d, propose SCR",
        link_mask,
        *sch_max_rate_ptr);
      status = SNM_ALT_CFG;
      /* Set max rate to be what we computed above */
      *sch_max_rate_ptr = max_rate;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Bad %d max_rate %d, reject SCR",
        link_mask,
        *sch_max_rate_ptr);
      return SNM_REJECT_CFG;
    }
  }

  return status;

} /* snm_eval_sch_max_rate */

/*===========================================================================

FUNCTION snm_eval_sch_frame_size

DESCRIPTION
  Evaluates the frame size of SCH config to see if it is acceptable. Proposes
  alterate SCH frame size if alt cfg is needed and cfg is not acceptable.

DEPENDENCIES
  None

RETURN VALUE
  SNM_ACCEPT_CFG: If Cfg is accepted.
  SNM_REJECT_CFG: If it is rejected,
  SNM_ALT_CFG:    If alt cfg is proposed

SIDE EFFECTS
  None

===========================================================================*/

static snm_eval_status_type snm_eval_sch_frame_size
(
  snm_fwd_rev_type link_mask,
    /* Mask indicates whether forward or reverse SCH's MUX to be verified */
  caii_srv_cfg_type *cfg_ptr,
    /* The service configuration to be evaluated and modified if alt cfg
       is needed */
  uint8 sch_index,
    /* Index to SCH config */
  boolean suggest_alt,
    /* Is an alternate cfg needed */
  snm_eval_status_type curr_status
)
{
  snm_eval_status_type status = curr_status;
  caii_for_sch_spec_fields_type fwd_sch_fields;
    /* For SCH fields from MCCAP */
  caii_rev_sch_spec_fields_type rev_sch_fields;
    /* Rev SCH fields from MCCAP */
  boolean frame_40_supported, frame_80_supported;
  boolean *frame_40_used_ptr, *frame_80_used_ptr;

  if ((link_mask != SNM_FORWARD) && (link_mask != SNM_REVERSE))
  {
    /* Shall never happen */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid channel link specified %d",
      link_mask);
    return SNM_REJECT_CFG;
  }

  if (link_mask == SNM_FORWARD)
  {
    frame_40_used_ptr = &cfg_ptr->for_sch[sch_index].sch_chn_cfg.frame_40_used;
    frame_80_used_ptr = &cfg_ptr->for_sch[sch_index].sch_chn_cfg.frame_80_used;

    /* Retrieve MS capability details for SCH */
    (void) mccap_get_ch_specific_fields(MCCAP_FWD_SCH, (void *) &fwd_sch_fields);
    frame_40_supported = fwd_sch_fields.for_sch_recs[0].for_frame_40_supported;
    frame_80_supported = fwd_sch_fields.for_sch_recs[0].for_frame_80_supported;
  }
  else
  {
    frame_40_used_ptr = &cfg_ptr->rev_sch[sch_index].sch_chn_cfg.frame_40_used;
    frame_80_used_ptr = &cfg_ptr->rev_sch[sch_index].sch_chn_cfg.frame_80_used;

    /* Retrieve MS capability details for SCH */
    (void) mccap_get_ch_specific_fields(MCCAP_REV_SCH, (void *) &rev_sch_fields);
    frame_40_supported = rev_sch_fields.rev_sch_recs[0].rev_frame_40_supported;
    frame_80_supported = rev_sch_fields.rev_sch_recs[0].rev_frame_80_supported;
  }

  /* Check SCH frame */
  if (*frame_40_used_ptr && !frame_40_supported)
  {
    if (suggest_alt)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "%d Frame 40 support mismatch, propose SCR",
        link_mask);
      status = SNM_ALT_CFG;
      *frame_40_used_ptr = FALSE;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_MED,
        "%d Frame 40 support mismatch, reject SCR",
        link_mask);
      return SNM_REJECT_CFG;
    }
  }

  if (*frame_80_used_ptr && !frame_80_supported)
  {
    if (suggest_alt)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "%d Frame 80 support mismatch, propose SCR",
        link_mask);
      status = SNM_ALT_CFG;
      *frame_80_used_ptr = FALSE;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_MED,
        "%d Frame 80 support mismatch, reject SCR",
        link_mask);
      return SNM_REJECT_CFG;
    }
  }

  return status;

} /* snm_eval_sch_frame_size */
#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================

FUNCTION snm_eval_sch_coding

DESCRIPTION
  Evaluates the coding of SCH config to see if it is acceptable. Proposes
  alterate SCH coding if alt cfg is needed and cfg is not acceptable.

DEPENDENCIES
  None

RETURN VALUE
  SNM_ACCEPT_CFG: If Cfg is accepted.
  SNM_REJECT_CFG: If it is rejected,
  SNM_ALT_CFG:    If alt cfg is proposed

SIDE EFFECTS
  None

===========================================================================*/

static snm_eval_status_type snm_eval_sch_coding
(
  snm_fwd_rev_type link_mask,
    /* Mask indicates whether forward or reverse SCH's MUX to be verified */
  caii_srv_cfg_type *cfg_ptr,
    /* The service configuration to be evaluated and modified if alt cfg
       is needed */
  uint8 sch_index,
    /* Index to SCH config */
  boolean suggest_alt,
    /* Is an alternate cfg needed */
  snm_eval_status_type curr_status
)
{
  snm_eval_status_type status = curr_status;
  boolean coding_type, *sch_coding_ptr;

#ifdef FEATURE_IS2000_REL_A
  boolean bad_cfg = FALSE;
  uint8 sch_rc, max_rate, max_conv_block_size, max_turbo_block_size;
  caii_for_sch_spec_fields_type fwd_sch_fields;
    /* For SCH fields from MCCAP */
  caii_rev_sch_spec_fields_type rev_sch_fields;
    /* Rev SCH fields from MCCAP */
#endif /* FEATURE_IS2000_REL_A*/

  if ((link_mask != SNM_FORWARD) && (link_mask != SNM_REVERSE))
  {
    /* Shall never happen */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid channel link specified %d",
      link_mask);
    return SNM_REJECT_CFG;
  }

  /* Check the valid range for sch_index value to fix the Klocwork error */
  if (sch_index >= CAI_MAX_NUM_FOR_SCH)
  {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Invalid SCH Index value %d",
        sch_index);
      return SNM_REJECT_CFG;
  }


#ifdef FEATURE_IS2000_REL_A
  if (cdma.p_rev_in_use >= P_REV_IS2000_REL_A)
  {
    /* There are some fileds in SCM which needs to be dereferenced only
     * when the p_rev_in_use is REV A or more */

    /* Retrieve MS capability details for SCH */
    if (link_mask == SNM_FORWARD)
    {
      (void) mccap_get_ch_specific_fields(MCCAP_FWD_SCH, (void *) &fwd_sch_fields);
      max_conv_block_size = fwd_sch_fields.for_sch_recs[0].for_max_conv_block_size;
      max_turbo_block_size = fwd_sch_fields.for_sch_recs[0].for_max_turbo_block_size;

      sch_coding_ptr = &cfg_ptr->for_sch[sch_index].sch_chn_cfg.coding;
      sch_rc = cfg_ptr->for_sch[sch_index].sch_chn_cfg.sch_rc;
      max_rate = cfg_ptr->for_sch[sch_index].sch_chn_cfg.max_rate;
      coding_type = cfg_ptr->for_sch[sch_index].sch_chn_cfg.coding;
    }
    else
    {
      (void) mccap_get_ch_specific_fields(MCCAP_REV_SCH, (void *) &rev_sch_fields);
      max_conv_block_size = rev_sch_fields.rev_sch_recs[0].rev_max_conv_block_size;
      max_turbo_block_size = rev_sch_fields.rev_sch_recs[0].rev_max_turbo_block_size;

      sch_coding_ptr = &cfg_ptr->rev_sch[sch_index].sch_chn_cfg.coding;
      sch_rc = cfg_ptr->rev_sch[sch_index].sch_chn_cfg.sch_rc;
      max_rate = cfg_ptr->rev_sch[sch_index].sch_chn_cfg.max_rate;
      coding_type = cfg_ptr->rev_sch[sch_index].sch_chn_cfg.coding;
    }

    /* Check SCH coding */
    switch (*sch_coding_ptr)
    {
      /* Need to make sure the "max rate" can be achieved with the given
         coding type, Note since we already verified that max rate is okay
         it means it is achieveable with one of the coding types for sure,
         so it is not possible with conv it means it is possible with turbo
         and vice-versa */
      case CAI_CONVOLUTIONAL_CODING:
        if (RATE_SET_OF_FWD_RC(sch_rc) == 0)
        {
          /* This is a Rate set 1 based RC, compare with RS1 table for max rate */
          if (snm_rs1_rate_table[max_rate] > snm_conv_turbo_rate_table[max_conv_block_size])
          {
            /* Cannot do this rate with conv then it must be possible with
               turbo since we already verified max rate */
            coding_type = CAI_TURBO_CODING;  /*lint !e641 */
            bad_cfg = TRUE;
          }
        }
        else
        {
          /* This is a Rate set 2 based RC, compare with RS2 table for max rate */
          if (snm_rs2_rate_table[max_rate] > snm_conv_turbo_rate_table[max_conv_block_size])
          {
            /* Cannot do this rate with conv then it must be possible with
               turbo since we already verified max rate */
            coding_type = CAI_TURBO_CODING;  /*lint !e641 */
            bad_cfg = TRUE;
          }
        }
        break;

      case CAI_TURBO_CODING:
        if (RATE_SET_OF_FWD_RC(sch_rc) == 0)
        {
          /* This is a Rate set 1 based RC, compare with RS1 table for max rate */
          if (snm_rs1_rate_table[max_rate] > snm_conv_turbo_rate_table[max_turbo_block_size])
          {
            /* Cannot do this rate with turbo then it must be possible with
               conv since we already verified max rate */
            coding_type = CAI_CONVOLUTIONAL_CODING;  /*lint !e641 */
            bad_cfg = TRUE;
          }
        }
        else
        {
          /* This is a Rate set 2 based RC, compare with RS2 table for max rate */
          if (snm_rs2_rate_table[max_rate] > snm_conv_turbo_rate_table[max_turbo_block_size])
          {
            /* Cannot do this rate with turbo then it must be possible with
               conv since we already verified max rate */
            coding_type = CAI_CONVOLUTIONAL_CODING;  /*lint !e641 */
            bad_cfg = TRUE;
          }
        }
        break;

      default:
        /* Coding type is not invalid, go with the coding type which supports
           the maximum SCH rate */
        coding_type = CAI_CONVOLUTIONAL_CODING;  /*lint !e641 */
        bad_cfg = TRUE;
        break;
    } /* end switch */

    if (bad_cfg)
    {
      /* In the above step we determined we did not like the coding type
         proposed by the BS and found an alternate coding type, now if we
         can propose an alternate propose it else reject the config */
      if (suggest_alt)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad %d SCH coding %d, propose SCR",
          link_mask,
          *sch_coding_ptr);
        *sch_coding_ptr = coding_type;
        status = SNM_ALT_CFG;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad %d SCH coding %d, reject SCR",
          link_mask,
          *sch_coding_ptr);
        return SNM_REJECT_CFG;
      }
    }
  }
  else
#endif /* FEATURE_IS2000_REL_A*/
  {
    coding_type = (uint8) mccap_get_preferred_sch_coding();

    if (link_mask == SNM_FORWARD)
    {
      sch_coding_ptr = &cfg_ptr->for_sch[sch_index].sch_chn_cfg.coding;
    }
    else
    {
      sch_coding_ptr = &cfg_ptr->rev_sch[sch_index].sch_chn_cfg.coding;
    }

    if (*sch_coding_ptr > coding_type)
    {
      if (suggest_alt)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad %d SCH coding %d, propose SCR",
          link_mask,
          *sch_coding_ptr);
        *sch_coding_ptr = coding_type;
        status = SNM_ALT_CFG;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad %d SCH coding %d, reject SCR",
          link_mask,
          *sch_coding_ptr);
        return SNM_REJECT_CFG;
      }
    }
  }

  return status;

} /* snm_eval_sch_coding */

/*===========================================================================

FUNCTION snm_eval_sch_config

DESCRIPTION
  Evaluates the SCH config of a SO connection in a configuration to see
  if it is acceptable. Proposes alterate SCH config if alt cfg is needed
  and cfg is not acceptable. The SO number of the connection is assumed to be
  valid.

DEPENDENCIES
  None

RETURN VALUE
  SNM_ACCEPT_CFG: If Cfg is accepted.
  SNM_REJECT_CFG: If it is rejected,
  SNM_ALT_CFG:    If alt cfg is proposed

SIDE EFFECTS
  None

===========================================================================*/

static snm_eval_status_type snm_eval_sch_config
(
  snm_chan_mask_type fwd_scr_chan_mask,
    /* Mask indicates active forward channel configuartions */
  snm_chan_mask_type rev_scr_chan_mask,
    /* Mask indicates active reverse channel configuartions */
  caii_srv_cfg_type *cfg_ptr,
    /* The service configuration to be evaluated and modified if alt cfg
       is needed */
  snm_so_descriptor_type *so_info,
    /* The SO descriptor to evaluate against */
  byte snm_con_index,
    /* connection index in negotiation_info.con_list[] */
  boolean suggest_alt,
    /* Is an alternate cfg needed */
  snm_eval_status_type curr_status
)
{
  snm_eval_status_type status = curr_status;

  uint8 i;
  uint8 max_sch  = 0;
  uint8 num_good = 0;
    /* Num of valid SCH cfg we found within SCH list in the BS cfg */
  uint8 sch_index = 0;

#ifdef FEATURE_IS2000_REL_A
  caii_for_sch_spec_fields_type fwd_sch_fields;
    /* For SCH fields from MCCAP */
  caii_rev_sch_spec_fields_type rev_sch_fields;
    /* Rev SCH fields from MCCAP */
  uint8 max_rate = 0;
    /* Max rate for a given mux option */
#endif /* FEATURE_IS2000_REL_A */

  if ((cfg_ptr->for_sch_cc_incl && cfg_ptr->num_for_sch) ||
      (cfg_ptr->rev_sch_cc_incl && cfg_ptr->num_rev_sch)
     )
  {

#ifdef FEATURE_IS2000_REL_C
    /* ********************************************************
    ** Check whether simultaneously F-PDCH and SCH is supported
    ** ******************************************************** */
    if (fwd_scr_chan_mask & SNM_PDCH)
    {
      if (!mccap_is_simult_for_pdch_sch_supported())
      {
        if (suggest_alt)
        {
          cfg_ptr->for_sch_cc_incl = FALSE;
          cfg_ptr->rev_sch_cc_incl = FALSE;
          status = SNM_ALT_CFG;
        }
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "MS does not support simultaneously F-PDCH and SCH, reject SCR");
          return SNM_REJECT_CFG;
        }
      }
    }
#endif /* FEATURE_IS2000_REL_C */

    /* *********************************************************
    ** Check RC class of FCH to see whether SCH can be supported
    ** ********************************************************* */
    if (((fwd_scr_chan_mask & SNM_FCH) && (RC_CLASS(cfg_ptr->for_fch_rc) < 2)) ||
        ((rev_scr_chan_mask & SNM_FCH) && (RC_CLASS(cfg_ptr->rev_fch_rc) < 2)))

    {
      if (suggest_alt)
      {
        cfg_ptr->for_sch_cc_incl = FALSE;
        cfg_ptr->rev_sch_cc_incl = FALSE;
        status = SNM_ALT_CFG;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Cannot support SCH on FCH RC class 1, reject SCR");
        return SNM_REJECT_CFG;
      }
    }

    /* ***************************************************
    ** Validate Fwd SCH parameters against SO supports SCH
    ** *************************************************** */
    if (cfg_ptr->for_sch_cc_incl && cfg_ptr->num_for_sch &&
        (so_info->negotiation_pars.fwd_sch_mo_list.num_mos > 0))
    {
      /* Num cannot be greater than the max we support */
      max_sch = mccap_num_f_sch_supported();
      negotiation_info.fwd_sch_cc_verified = TRUE;

      /* If num of SCH is greater than the num we can support and we
         do not have to suggest an alt cfg, just reject right away */
      if (cfg_ptr->num_for_sch > max_sch)
      {
        if (!suggest_alt)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "%d F-SCHs not supported, reject SCR",
            cfg_ptr->num_for_sch);
          return SNM_REJECT_CFG;
        }
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "%d F-SCHs not supported, propose SCR",
            cfg_ptr->num_for_sch);
        }
      }

      /* For each SCH check the SCH ID, RC, MUX, and coding type */
      num_good = 0;
      sch_index = 0;

      for (i = 0; (i < cfg_ptr->num_for_sch && (i < CAI_MAX_NUM_FOR_SCH)); i++)
      {
        /* ************
        ** Check SCH ID
        ** ************ */
        if (cfg_ptr->for_sch[i].for_sch_id >= max_sch)
        {
          if (!suggest_alt)
          {
            M1X_MSG( DCP, LEGACY_MED,
              "F-SCH %d not allowed, reject SCR",
              cfg_ptr->for_sch[i].for_sch_id);
            return SNM_REJECT_CFG;
          }

          M1X_MSG( DCP, LEGACY_MED,
            "F-SCH %d not allowed, propose SCR",
            cfg_ptr->for_sch[i].for_sch_id);
          continue;
        }

        /* If we are here it means sch_id was within range */
        if (!suggest_alt)
        {
          sch_index = i;
        }
        else
        {
          if (i != num_good)
          {
            cfg_ptr->for_sch[num_good] = cfg_ptr->for_sch[i];
            cfg_ptr->for_sch[num_good].for_sch_id = num_good;
          }
          sch_index = num_good++;
        }

        /* ********
        ** Check RC
        ** ******** */
        if ((status = snm_eval_sch_rc(SNM_FORWARD, fwd_scr_chan_mask, rev_scr_chan_mask,
             cfg_ptr, sch_index, so_info, suggest_alt, status)) == SNM_REJECT_CFG)
        {
          /* Bad RC */
          return SNM_REJECT_CFG;
        }

        /* ****************
        ** Check mux option
        ** **************** */
        if ((status = snm_eval_sch_mux(SNM_FORWARD, cfg_ptr, sch_index,
             so_info, snm_con_index, suggest_alt, status)) == SNM_REJECT_CFG)
        {
          /* Bad MUX */
          return SNM_REJECT_CFG;
        }

#ifdef FEATURE_IS2000_REL_A
        if (P_REV_IS_7_OR_GREATER)
        {
          /* **************
          ** Check max rate
          ** ************** */
          if ((status = snm_eval_sch_max_rate(SNM_FORWARD, cfg_ptr, sch_index,
               suggest_alt, status)) == SNM_REJECT_CFG)
          {
            /* Bad rate */
            return SNM_REJECT_CFG;
          }

          /* Max rate of 32X cannot be done in RC 3 */
          if ((cfg_ptr->for_sch[sch_index].sch_chn_cfg.max_rate >=
               CAI_SCH_RATE_RS1_32X_RS2_18X) &&  /*lint !e641 */
              (cfg_ptr->for_sch[sch_index].sch_chn_cfg.sch_rc == CAI_RC_3))  /*lint !e641 */
          {
            if (suggest_alt)
            {
              M1X_MSG( DCP, LEGACY_MED,
                "Error 32X with RC 3 !!, propose SCR");
              /* Set max rate to be what we computed above */
              cfg_ptr->for_sch[sch_index].sch_chn_cfg.sch_rc = CAI_RC_4;  /*lint !e641 */
              status = SNM_ALT_CFG;
            }
            else
            {
              M1X_MSG( DCP, LEGACY_MED,
                "Error 32X with RC 3 !!, reject SCR");
              return SNM_REJECT_CFG;
            }
          }

          /* ****************
          ** Check frame size
          ** **************** */
          if ((status = snm_eval_sch_frame_size(SNM_FORWARD, cfg_ptr, sch_index,
               suggest_alt, status)) == SNM_REJECT_CFG)
          {
            /* Bad frame size */
            return SNM_REJECT_CFG;
          }
        }
        else
        {
          /* If we are here, it means the p_rev_in_use is 6, default the
             max rate and other new fields correctly */
          if (cfg_ptr->for_sch[sch_index].for_sch_mux == 0xf20)
          {
            (void) mccap_get_ch_specific_fields(MCCAP_FWD_SCH,
                     (void *) &fwd_sch_fields);
            max_rate = fwd_sch_fields.for_sch_recs[0].for_max_rate;
          }
          else
          {
            max_rate = snm_get_max_sch_rate_from_mux(
                       cfg_ptr->for_sch[sch_index].for_sch_mux);
          }
          cfg_ptr->for_sch[sch_index].sch_chn_cfg.max_rate = max_rate;
          cfg_ptr->for_sch[sch_index].sch_chn_cfg.frame_40_used = FALSE;
          cfg_ptr->for_sch[sch_index].sch_chn_cfg.frame_80_used = FALSE;
        }
#endif /* FEATURE_IS2000_REL_A */

        /* *****************
        ** Check coding type
        ** ***************** */
        if ((status = snm_eval_sch_coding(SNM_FORWARD, cfg_ptr, sch_index,
             suggest_alt, status)) == SNM_REJECT_CFG)
        {
          /* Bad coding type */
          return SNM_REJECT_CFG;
        }

        if (num_good >= max_sch)
        {
          /* Okay we have extracted all the info in the BS SCH array into our
             cfg_ptr, Cannot accomodate anymore, so we are done here */
          break; /* Break from the loop */
        }
      } /* end for */

      if (suggest_alt)
      {
        if (cfg_ptr->num_for_sch != num_good)
        {
          cfg_ptr->num_for_sch = num_good;
          status = SNM_ALT_CFG;
          /* If as a result of all this, there are no SCHs included in alt cfg
             just make sure for_sch_cc_incl = 0 */
          if (cfg_ptr->num_for_sch == 0)
          {
            cfg_ptr->for_sch_cc_incl = FALSE;
          }
        }

        /* If the BS proposed some SCH cfg and we can support SCH but
           none of the SCH id matched, just propose SCH cfg based
           on negotiation_info */
        if ((cfg_ptr->num_for_sch != 0) && (num_good == 0) && (max_sch !=0))
        {
          snm_chan_mask_type filter_mask = SNM_FCH;

          filter_mask |= SNM_DCCH;

          snm_populate_fwd_sch_cfg(cfg_ptr, so_info, snm_con_index,
            (fwd_scr_chan_mask & filter_mask), (rev_scr_chan_mask & filter_mask));
          /* If as a result of all this, there are no SCHs included in alt cfg
             just make sure for_sch_cc_incl = FALSE */
          if (cfg_ptr->num_for_sch == 0)
          {
            cfg_ptr->for_sch_cc_incl = FALSE;
          }
          status = SNM_ALT_CFG;
        }
      }
    }

    /* ***************************************************
    ** Validate Rev SCH parameters against SO supports SCH
    ** *************************************************** */
    if (cfg_ptr->rev_sch_cc_incl && cfg_ptr->num_rev_sch &&
        (so_info->negotiation_pars.rev_sch_mo_list.num_mos > 0))
        /* SO supports SCH */
    {
      /* Num of r-SCH cannot be greater than the max we support */
      max_sch = mccap_num_r_sch_supported();
      negotiation_info.rev_sch_cc_verified = TRUE;

      /* If num of SCH is greater than the num we can support and we
         do not have to suggest an alt cfg, just reject right away */
      if (cfg_ptr->num_rev_sch > max_sch)
      {
        if (!suggest_alt)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "%d R-SCHs not supported, reject SCR",
            cfg_ptr->num_rev_sch);
          return SNM_REJECT_CFG;
        }
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "%d R-SCHs not supported, propose SCR",
            cfg_ptr->num_rev_sch);
        }
      }

      num_good = 0;
      for (i = 0; (i < cfg_ptr->num_rev_sch && (i < CAI_MAX_NUM_REV_SCH)); i++)
      {
        if (cfg_ptr->rev_sch[i].rev_sch_id >= max_sch)
        {
          if (!suggest_alt)
          {
            return SNM_REJECT_CFG;
          }
          continue;
        }

        /* If we are here it means sch_id was within range */
        if (!suggest_alt)
        {
          sch_index = i;
        }
        else
        {
          if (i != num_good)
          {
            cfg_ptr->rev_sch[num_good] = cfg_ptr->rev_sch[i];
            cfg_ptr->rev_sch[num_good].rev_sch_id = num_good;
            status = SNM_ALT_CFG;
          }
          sch_index = num_good++;
        }

        /* ********
        ** Check RC
        ** ******** */
        if ((status = snm_eval_sch_rc(SNM_REVERSE, fwd_scr_chan_mask, rev_scr_chan_mask,
             cfg_ptr, sch_index, so_info, suggest_alt, status)) == SNM_REJECT_CFG)
        {
          /* Bad RC */
          return SNM_REJECT_CFG;
        }

        /* ****************
        ** Check mux option
        ** **************** */
        if ((status = snm_eval_sch_mux(SNM_REVERSE, cfg_ptr, sch_index,
             so_info, snm_con_index, suggest_alt, status)) == SNM_REJECT_CFG)
        {
          /* Bad MUX */
          return SNM_REJECT_CFG;
        }

#ifdef FEATURE_IS2000_REL_A
        if (P_REV_IS_7_OR_GREATER)
        {
          /* **************
          ** Check max rate
          ** ************** */
          if ((status = snm_eval_sch_max_rate(SNM_REVERSE, cfg_ptr, sch_index,
               suggest_alt, status)) == SNM_REJECT_CFG)
          {
            /* Bad rate */
            return SNM_REJECT_CFG;
          }

          /* Max rate of 18X cannot be done in RC 4 */
          if ( (cfg_ptr->rev_sch[sch_index].sch_chn_cfg.max_rate >=
                      CAI_SCH_RATE_RS1_32X_RS2_18X) &&  /*lint !e641 */
                (/* this macro returns 1 for RC4 Rate set 2 */
                 RATE_SET_OF_REV_RC(cfg_ptr->rev_sch[sch_index].sch_chn_cfg.sch_rc) == 1)
             )
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Error 18X with RC 4 !!, reject SCR");
            return SNM_REJECT_CFG;
          }
          /* ****************
          ** Check frame size
          ** **************** */
          if ((status = snm_eval_sch_frame_size(SNM_REVERSE, cfg_ptr, sch_index,
               suggest_alt, status)) == SNM_REJECT_CFG)
          {
            /* Bad frame size */
            return SNM_REJECT_CFG;
          }
        }
        else
        {
          /* If we are here, it means the p_rev_in_use is 6, default the
             max rate  and other new fields correctly */
          if (cfg_ptr->rev_sch[sch_index].rev_sch_mux == 0xf20)
          {
            (void) mccap_get_ch_specific_fields(MCCAP_REV_SCH,
                     (void *) &rev_sch_fields);
            max_rate = rev_sch_fields.rev_sch_recs[0].rev_max_rate;
          }
          else
          {
            max_rate = snm_get_max_sch_rate_from_mux(
                       cfg_ptr->rev_sch[sch_index].rev_sch_mux);
          }

          cfg_ptr->rev_sch[sch_index].sch_chn_cfg.max_rate = max_rate;
          cfg_ptr->rev_sch[sch_index].sch_chn_cfg.frame_40_used = 0;
          cfg_ptr->rev_sch[sch_index].sch_chn_cfg.frame_80_used = 0;
        }
#endif /* FEATURE_IS2000_REL_A */

        if ((status = snm_eval_sch_coding(SNM_REVERSE, cfg_ptr, sch_index,
             suggest_alt, status)) ==  SNM_REJECT_CFG)
        {
          /* Bad coding type */
          return SNM_REJECT_CFG;
        }
      } /* For loop */

      if (suggest_alt)
      {
        if (cfg_ptr->num_rev_sch != num_good)
        {
          cfg_ptr->num_rev_sch = num_good;

          /* If as a result of all this, there are no SCHs included in alt cfg
           just make sure rev_sch_cc_incl = 0 */
          if (cfg_ptr->num_rev_sch == 0)
          {
            cfg_ptr->rev_sch_cc_incl = FALSE;
          }
          status = SNM_ALT_CFG;
        }

        /* If the BS proposed some SCH cfg and we can support SCH but
           none of the SCH id matched, just propose some SCH values based
           on negotiation_info */
        if ((cfg_ptr->num_rev_sch != 0) && (num_good == 0) && (max_sch != 0))
        {
          snm_populate_rev_sch_cfg(cfg_ptr, so_info, snm_con_index,
            fwd_scr_chan_mask, rev_scr_chan_mask);

          /* If as a result of all this, there are no SCHs included in alt cfg
             just make sure rev_sch_cc_incl = 0 */
          if (cfg_ptr->num_rev_sch == 0)
          {
            cfg_ptr->rev_sch_cc_incl = FALSE;
          }

          status = SNM_ALT_CFG;
        }
      }
    } /* end if (rev_sch_cc_incl) */

  }

  return status;

} /* snm_eval_sch_config */

/*===========================================================================

FUNCTION snm_check_con_and_generate_alt

DESCRIPTION
  Evaluates a certain SO connection in a configuration to see if it is
  acceptable. The SO number of the connection is assumed to be valid.

DEPENDENCIES
  None

RETURN VALUE
  SNM_ACCEPT_CFG: If Cfg is accepted, SNM_REJECT_CFG: If it is rejected,
  SNM_ALT_CFG: If alt cfg is proposed

SIDE EFFECTS
  None
===========================================================================*/

static snm_eval_status_type snm_check_con_and_generate_alt
(
  caii_srv_cfg_type *cfg_ptr,
    /* The service configuration to be evaluated */
  boolean restore_ind,
    /* Indicates if config is to be restored */
  byte con_index,
    /* Index to the SO connection to be evaluated */
  snm_so_descriptor_type *so_info,
    /* The SO descriptor to evaluate against */
  boolean suggest_alt,
    /* Is an alternate cfg needed */
  caii_srv_cfg_type *alt_cfg_ptr,
    /* Pointer to the alternate cfg */
  byte snm_con_index,
    /* connection index in negotiation_info.con_list[] */
  mcc_fwd_chan_mask_type fwd_chan_mask,
    /* Mask indicates active forward channels */
  mcc_rev_chan_mask_type rev_chan_mask
    /* Mask indicates active reverse channels */
)
{
  snm_eval_status_type status = SNM_ACCEPT_CFG;
  caii_srv_cfg_type *temp_cfg_ptr;
  snm_chan_mask_type fwd_scr_chan_mask = SNM_INIT_CHAN_MASK;
    /* Indicates forward channel configuration to be validated */
  snm_chan_mask_type rev_scr_chan_mask = SNM_INIT_CHAN_MASK;
    /* Indicates reverse channel configuration to be validated */
  snm_chan_mask_type so_chan_mask = SNM_INIT_CHAN_MASK;
    /* Indicates physical channel supported by SO */

  temp_cfg_ptr = (suggest_alt) ? alt_cfg_ptr : cfg_ptr;

  if (cfg_ptr == NULL)
  {
    /* Should never happen */
    ERR_FATAL("Null cfg ptr", 0, 0, 0);
  }

  if ((suggest_alt) && (cfg_ptr != NULL))
  {
    if (alt_cfg_ptr == NULL)
    {
      /* Should never happen */
      ERR_FATAL("Null alt cfg ptr", 0, 0, 0);
    }
    else
    {
      /* Start with the config the BS proposed and modify unacceptable
         parameters */
      *alt_cfg_ptr = *cfg_ptr;
    }
  }

  if (so_info->service_option_num !=
      negotiation_info.con_list[snm_con_index].prev_so)
  {
    /* SO has changed since the last time we evaluated or generated the config,
       so reset RC and mux indices */
    negotiation_info.con_list[snm_con_index].cur_fwd_fch_rc_index = 0;
    negotiation_info.con_list[snm_con_index].cur_fwd_dcch_rc_index = 0;
    negotiation_info.con_list[snm_con_index].fundicated_rc_proposed_chan_mask = SNM_INIT_CHAN_MASK;
    negotiation_info.con_list[snm_con_index].cur_fwd_sch_mux_index =
    negotiation_info.con_list[snm_con_index].cur_rev_sch_mux_index = 0;
    negotiation_info.con_list[snm_con_index].prev_so =
      so_info->service_option_num;
  }

  /* **********************************************************************
  ** Based on new configuration or channel masks, generate SCR masks that
  ** indicates active channel configurations to be validated.
  ** For SRQM, new configurations is used to generate SCR masks.
  ** For SCM and UHDM, channel masks are used to generate SCR masks.
  ** ********************************************************************** */
  snm_gen_scr_chan_mask(temp_cfg_ptr, fwd_chan_mask, rev_chan_mask, (suggest_alt ? FALSE : TRUE),
    &fwd_scr_chan_mask, &rev_scr_chan_mask);

  /* **************************************************************************
  ** Compose the service configuration to be validated and update the SCR masks
  ** accordingly.
  ** ************************************************************************** */
  if (P_REV_IS_6_OR_GREATER && !restore_ind)
  {
    /* ************************************************************************
    **  If fch_cc_incl is not included, it means the BS is just telling us to
    **  use the existing configuration, Make sure are still good with the SO
    **  we are evaluating.
    **
    **  If dcch_cc_incl is not included, it means the BS is just telling us to
    **  use the existing configuration, Make sure are still good with the SO
    **  we are evaluating.
    **
    **  If for_pdch_cc_incl is not included, it means the BS is just telling us
    **  to use the existing configuration, Make sure are still good with the SO
    **  we are evaluating.
    **
    **  If alt_cfg_ptr is 0, this modifies cfg_ptr. This should be okay
    **  because dcch_cc_incl = 0, no one better be accessing these values
    **  anyway.
    ** ************************************************************************ */

    mcc_fwd_chan_mask_type temp_fwd_chan_mask = fwd_chan_mask;
    mcc_rev_chan_mask_type temp_rev_chan_mask = rev_chan_mask;

    if (suggest_alt)
    {
      /* EXT_CH_IND or CH_IND is not final yet. Consider all existing channel
         configurations for incremental update */
      temp_fwd_chan_mask |= (MCC_F_FCH | MCC_F_DCCH);
      temp_rev_chan_mask |= (MCC_R_FCH | MCC_R_DCCH);

    }

    snm_compose_config(temp_cfg_ptr, &negotiation_info.current_config->neg_scr,
      temp_fwd_chan_mask, temp_rev_chan_mask, &fwd_scr_chan_mask, &rev_scr_chan_mask);
  }

  /* ********************
  ** Check MS capability
  ** ******************* */
  if ((status = snm_eval_config_vs_ms_cap(temp_cfg_ptr, fwd_scr_chan_mask, rev_scr_chan_mask,
      suggest_alt, status)) == SNM_REJECT_CFG)
  {
    /* Channel not supported by MS */
    return SNM_REJECT_CFG;
  }

  if (!P_REV_IS_6_OR_GREATER)
  {
    /* **************
    ** Check FCH MUX
    ** ************* */
    if ((status = snm_eval_fundicated_mux(SNM_FCH, fwd_scr_chan_mask,
        rev_scr_chan_mask, temp_cfg_ptr, so_info, suggest_alt, status)) == SNM_REJECT_CFG)
    {
      /* Bad MUXs */
      return SNM_REJECT_CFG;
    }

    /* ************************
    ** Check transmission rates
    ** ************************ */
    if ((status = snm_eval_fch_rates(fwd_scr_chan_mask, rev_scr_chan_mask, temp_cfg_ptr,
        so_info, suggest_alt, status)) == SNM_REJECT_CFG)
    {
      /* Bad rates */
      return SNM_REJECT_CFG;
    }
  }
  else
  {
    /* *****************************************
    ** Figure out allowed TC channels for the SO
    ** ***************************************** */
    so_chan_mask = snm_gen_so_chan_mask(so_info->service_option_num);

    /* *****************************
    ** Validate FCH and DCCH config.
    ** ***************************** */
    if ((fwd_scr_chan_mask | rev_scr_chan_mask) & (SNM_FCH | SNM_DCCH))
    {
       if ((status = snm_eval_fundicated_config(so_chan_mask,
            fwd_scr_chan_mask, rev_scr_chan_mask, temp_cfg_ptr, so_info, snm_con_index,
            suggest_alt, status)) == SNM_REJECT_CFG)
       {
         return SNM_REJECT_CFG;
       }
    }
  }

  /* *************************
  ** Now validate traffic type
  ** ************************* */
  if ((status = snm_eval_traffic_type(temp_cfg_ptr, con_index, so_info,
       suggest_alt, status)) == SNM_REJECT_CFG)
  {
    /* Traffic types rejected */
    return SNM_REJECT_CFG;
  }

  if (P_REV_IS_6_OR_GREATER)
  {
    /* Validate SCH configs */
    if ((status = snm_eval_sch_config(fwd_scr_chan_mask, rev_scr_chan_mask, temp_cfg_ptr,
         so_info, snm_con_index, suggest_alt, status)) == SNM_REJECT_CFG)
    {
      /* SCH config rejected */
      return SNM_REJECT_CFG;
    }

#ifdef FEATURE_IS2000_REL_A
    if (P_REV_IS_7_OR_GREATER)
    {
      /* *********************************
      ** Evaluate the QOS blob, if present
      ** ********************************* */
      if ((status = snm_eval_qos(temp_cfg_ptr, con_index, so_info,
           suggest_alt, status)) == SNM_REJECT_CFG)
      {
        /* Bad QoS */
        return SNM_REJECT_CFG;
      }
    } /* end if (P_REV_IS_7_OR_GREATER) */
#endif /* FEATURE_IS2000_REL_A */

    /* **************
    ** Validate SR_ID
    ** ************** */
    if ((status = snm_eval_sr_id(temp_cfg_ptr, con_index, snm_con_index,
        suggest_alt, status)) == SNM_REJECT_CFG)
    {
      /* Bad SR_ID */
      return SNM_REJECT_CFG;
    }
  } /* If P_REV_IS_6_OR_GREATER */

  /* **********************************
  ** Validate the number of connections
  ** ********************************** */
#ifdef FEATURE_IS2000_REL_A_SVD
  if (!P_REV_IS_7_OR_GREATER)
#endif /* FEATURE_IS2000_REL_A_SVD */
  {
    if (con_index > 0) //lint !e539
    {
      if (!suggest_alt)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "con_index can't be non-zero %d",
          con_index);
        return SNM_REJECT_CFG;
      }
      else
      {
        status = SNM_ALT_CFG;
      }
    }
  }

  /* validate ui_encrypt_mode */
  #ifdef FEATURE_IS2000_REL_A_AES
  /* BS support notice:
   * if BS does not support service negotiation of the ui_encrypt_mode, the following check
   * would better not to be performed.
   */
  if ( P_REV_IS_7_OR_GREATER )
  {
    /* check ui_encrypt_mode of the connection against the current BS capability
     *
     * note that for UHDM, the ui_encrypt_mode should have already validated against
     * the capability of the new BS defined in UHDM. If anything wrong with ui_encrypt_mode
     * in UHDM, the message should have already been rejected.
     * so here the check only applies to Service Request Message and Service Connect Message
     *
     */
    if ( (! mccaes_validate_ui_aes_mode(temp_cfg_ptr->con[con_index].ui_encrypt_mode)) )
    {
      if (!suggest_alt)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "ui_encrypt_mode %d in connection id %d invalid",
          temp_cfg_ptr->con[con_index].ui_encrypt_mode,
          con_index );
        return SNM_REJECT_CFG;
      }
      else
      {
        temp_cfg_ptr->con[con_index].ui_encrypt_mode = CAI_ENC_MODE_DISABLED;
        status = SNM_ALT_CFG;
      }
    }
  }

  #endif /* FEATURE_IS2000_REL_A_AES */

  if (status == SNM_ALT_CFG)
  {
    if (P_REV_IS_6_OR_GREATER)
    {
      #ifdef FEATURE_IS2000_REL_A_AES
      /* only when AES feature is not defined, or P_REV is not 7 or greater,
       * we hard code the ui_encrypt_mode to be 0 for alternative configuration.
       * Otherwise, we keep it as is because it is validated or alternatively
       * proposed accordign to AES procedure.
       */
      if ( ! P_REV_IS_7_OR_GREATER )
      {
        /* for P_REV 6, we don't have any UI encryption/decryption support */
        temp_cfg_ptr->con[con_index].ui_encrypt_mode = CAI_ENC_MODE_DISABLED;
      }
      #else
      temp_cfg_ptr->con[con_index].ui_encrypt_mode = CAI_ENC_MODE_DISABLED;
      #endif /* FEATURE_IS2000_REL_A_AES */

      /* See if RLP blob has to be included */
      temp_cfg_ptr->con[con_index].rlp_info_incl = FALSE;
      if (so_info->rlp_blob_generator)
      {
        /* If so, get blob by calling the generator function. */
        if ((temp_cfg_ptr->con[con_index].rlp_blob_len =
            so_info->rlp_blob_generator(temp_cfg_ptr->con[con_index].rlp_blob)) != FALSE )
        {
          temp_cfg_ptr->con[con_index].rlp_info_incl = TRUE;
        }
      }
    } /* end if (P_REV_IS_6_OR_GREATER) */
  }

  return status;

} /* snm_check_con_and_generate_alt */

/*===========================================================================

FUNCTION snm_verify_fundicated_config_vs_ms_cap

DESCRIPTION
  Evaluates the fundicated config that has not been considered to be
  compatible with any SO connection against MS capability.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : If cfg is accepted.
  FALSE: If cfg is rejected.

SIDE EFFECTS
  None

===========================================================================*/

static snm_eval_status_type snm_verify_fundicated_config_vs_ms_cap
(
  mcc_fwd_chan_mask_type fwd_chan_mask,
    /* Mask indicates active forward channels */
  mcc_rev_chan_mask_type rev_chan_mask,
    /* Mask indicates active reverse channels */
  caii_srv_cfg_type *cfg_ptr,
    /* Pointer to service configuration to be evaluated and altered if
       alternative configuration is to be generated */
  boolean suggest_alt,
    /* Indicates if a alternative configuration is to be generated */
  snm_eval_status_type curr_status
)
{
  snm_eval_status_type status = curr_status;
  snm_chan_mask_type fwd_scr_chan_mask = SNM_INIT_CHAN_MASK;
    /* Indicates forward channel configuration to be validated */
  snm_chan_mask_type rev_scr_chan_mask = SNM_INIT_CHAN_MASK;
    /* Indicates reverse channel configuration to be validated */
  boolean fwd_has_fch, rev_has_fch, fwd_has_dcch, rev_has_dcch;

  /* ***************************************************************************
  ** Generate config channel masks that indicates active channels'
  ** configurations. For SRQM, channel masks are not used in deciding SCR masks.
  ** *************************************************************************** */
  snm_gen_scr_chan_mask(cfg_ptr, fwd_chan_mask, rev_chan_mask, (suggest_alt ? FALSE : TRUE),
    &fwd_scr_chan_mask, &rev_scr_chan_mask);

  fwd_has_fch = (fwd_scr_chan_mask & SNM_FCH) ? TRUE : FALSE;
  rev_has_fch = (rev_scr_chan_mask & SNM_FCH) ? TRUE : FALSE;
  fwd_has_dcch = (fwd_scr_chan_mask & SNM_DCCH) ? TRUE : FALSE;
  rev_has_dcch = (rev_scr_chan_mask & SNM_DCCH) ? TRUE : FALSE;

  /* DCCH config has not been verified */
  if (!negotiation_info.dcch_cc_verified && ((fwd_scr_chan_mask | rev_scr_chan_mask) & SNM_DCCH))
  {
    /* ***************************************
    ** Validate DCCH RCs against MS capability
    ** *************************************** */
    if (!mccap_check_rc_for_chan(
          (fwd_has_dcch ? MCC_F_DCCH : 0),
          (rev_has_dcch ? MCC_R_DCCH : 0),
          (cai_radio_config_type) cfg_ptr->for_dcch_rc,
          (cai_radio_config_type) cfg_ptr->rev_dcch_rc) ||
          /* RCs not supported */
        (fwd_has_dcch && rev_has_dcch &&
         ((RATE_SET_OF_FWD_RC(cfg_ptr->for_dcch_rc) != RATE_SET_OF_REV_RC(cfg_ptr->rev_dcch_rc)) ||
            /* Asymmetric rateset */
          (RC_CLASS(cfg_ptr->for_dcch_rc) != RC_CLASS(cfg_ptr->rev_dcch_rc))))
            /* Asymmetric RC class */
       )
    {
      if (!suggest_alt)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad DCCH RCs: fwd %d, rev %d Alt: %d, reject SCR",
          cfg_ptr->for_dcch_rc,
          cfg_ptr->rev_dcch_rc,
          suggest_alt);
        return SNM_REJECT_CFG;
      }
      /* FCH assignment is asymmetric to DCCH assignment, simply set RCs of DCCH same as FCH */
      else if (negotiation_info.fch_cc_verified &&
               (fwd_has_fch == fwd_has_dcch) &&
               (rev_has_fch == rev_has_dcch))
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad DCCH RCs: fwd %d, rev %d, propose SCR",
          cfg_ptr->for_dcch_rc,
          cfg_ptr->rev_dcch_rc);
        cfg_ptr->dcch_cc_incl = TRUE;
        cfg_ptr->for_dcch_rc = cfg_ptr->for_fch_rc;
        cfg_ptr->rev_dcch_rc = cfg_ptr->rev_fch_rc;
        status = SNM_ALT_CFG;
      }
      else
      {
        /* Let snm_eval_config() handles the generation of alternate DCCH config */
        return SNM_REJECT_CFG;
      }
    }

    /* *********************
    ** Check DCCH frame size
    ** ********************* */
    if ((status = snm_eval_fundicated_frame_size(SNM_DCCH, cfg_ptr, suggest_alt,
        status)) == SNM_REJECT_CFG)
    {
      /* Bad frame size */
      return SNM_REJECT_CFG;
    }

    /* ***************
    ** Check DCCH MUX
    ** ************** */
    if ((status = snm_eval_fundicated_mux(SNM_DCCH, fwd_scr_chan_mask,
        rev_scr_chan_mask, cfg_ptr, NULL, suggest_alt, status)) == SNM_REJECT_CFG)
    {
      /* Bad MUXs */
      return SNM_REJECT_CFG;
    }

    /* ********************************************************
    ** Check the case of RC class mismatch between FCH and DCCH
    ** ******************************************************** */
    if ((fwd_has_fch || rev_has_fch) &&
        (fwd_has_dcch || rev_has_dcch))
    {
      /* Check the RC class */
      if (!snm_is_fundicated_rc_match(fwd_scr_chan_mask, rev_scr_chan_mask, cfg_ptr))
      {
        if (!suggest_alt)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "FCH and DCCH RC rateset or class not match, reject SCR");
          status = SNM_REJECT_CFG;
        }
        else
        {
          /* DCCH assignment is symmetric to FCH assignment, simply set RCs of FCH same as DCCH */
          if ((fwd_has_fch == fwd_has_dcch) &&
              (rev_has_fch == rev_has_dcch))
          {
            M1X_MSG( DCP, LEGACY_MED,
              "FCH and DCCH RC rateset or class not match, propose SCR");
            cfg_ptr->fch_cc_incl = TRUE;
            cfg_ptr->for_fch_rc = cfg_ptr->for_dcch_rc;
            cfg_ptr->rev_fch_rc = cfg_ptr->rev_dcch_rc;
            status = SNM_ALT_CFG;
            /* Mark FCH config verified */
            negotiation_info.fch_cc_verified = TRUE;
          }
          else
          {
            /* Let snm_eval_config() handles the generation of alternate DCCH config */
            status = SNM_REJECT_CFG;
          }
        }
      }
    }

    /* Mark DCCH config verified */
    negotiation_info.dcch_cc_verified = TRUE;
  }

  /* FCH config has not been verified */
  if (!negotiation_info.fch_cc_verified && ((fwd_scr_chan_mask | rev_scr_chan_mask) & SNM_FCH))
  {
    /* **************************************
    ** Validate FCH RCs against MS capability
    ** ************************************** */
    if (!mccap_check_rc_for_chan(
          (fwd_has_fch ? MCC_F_FCH : 0),
          (rev_has_fch ? MCC_R_FCH : 0),
          (cai_radio_config_type) cfg_ptr->for_fch_rc,
          (cai_radio_config_type) cfg_ptr->rev_fch_rc) ||
          /* RCs not supported */
        (fwd_has_fch && rev_has_fch &&
         ((RATE_SET_OF_FWD_RC(cfg_ptr->for_fch_rc) != RATE_SET_OF_REV_RC(cfg_ptr->rev_fch_rc)) ||
            /* Asymmetric rateset */
          (RC_CLASS(cfg_ptr->for_fch_rc) != RC_CLASS(cfg_ptr->rev_fch_rc))))
            /* Asymmetric RC class */
       )
    {
      if (!suggest_alt)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad FCH RCs: fwd %d, rev %d Alt: %d, reject SCR",
          cfg_ptr->for_fch_rc,
          cfg_ptr->rev_fch_rc,
          suggest_alt);
       return SNM_REJECT_CFG;
      }

      /* DCCH assignment is symmetric to FCH assignment, simply set RCs of FCH same as DCCH */
      else if (negotiation_info.dcch_cc_verified &&
               (fwd_has_fch == fwd_has_dcch) &&
               (rev_has_fch == rev_has_dcch))
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Bad FCH RCs: fwd %d, rev %d, propose SCR",
          cfg_ptr->for_fch_rc,
          cfg_ptr->rev_fch_rc);
        cfg_ptr->fch_cc_incl = TRUE;
        cfg_ptr->for_fch_rc = cfg_ptr->for_dcch_rc;
        cfg_ptr->rev_fch_rc = cfg_ptr->rev_dcch_rc;
        status = SNM_ALT_CFG;
      }

      else
      {
        /* Let snm_eval_config() handles the generation of alternate FCH config */
        return SNM_REJECT_CFG;
      }
    }

    /* ********************
    ** Check FCH frame size
    ** ******************** */
    if ((status = snm_eval_fundicated_frame_size(SNM_FCH, cfg_ptr, suggest_alt,
        status)) == SNM_REJECT_CFG)
    {
      /* Bad frame size */
      return SNM_REJECT_CFG;
    }

    /* **************
    ** Check FCH MUX
    ** ************* */
    if ((status = snm_eval_fundicated_mux(SNM_FCH, fwd_scr_chan_mask,
        rev_scr_chan_mask, cfg_ptr, NULL, suggest_alt, status)) == SNM_REJECT_CFG)
    {
      /* Bad MUXs */
      return SNM_REJECT_CFG;
    }

    /* ****************************
    ** Check FCH transmission rates
    ** **************************** */
    if ((status = snm_eval_fch_rates(fwd_scr_chan_mask, rev_scr_chan_mask, cfg_ptr,
        NULL, suggest_alt, status)) == SNM_REJECT_CFG)
    {
      /* Bad rates */
      return SNM_REJECT_CFG;
    }

    /* Mark FCH config verified */
    negotiation_info.fch_cc_verified = TRUE;
  }

  return status;

} /* snm_verify_fundicated_config_vs_ms_cap */

/*===========================================================================

FUNCTION snm_verify_sch_config_vs_ms_cap

DESCRIPTION
  Evaluates the SCH config against MS capability.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : If cfg is accepted.
  FALSE: If cfg is rejected.

SIDE EFFECTS
  None

===========================================================================*/

static snm_eval_status_type snm_verify_sch_config_vs_ms_cap
(
  mcc_fwd_chan_mask_type  fwd_chan_mask,
    /* Mask indicates active forward channels */
  mcc_rev_chan_mask_type rev_chan_mask,
    /* Mask indicates active reverse channels */
  caii_srv_cfg_type *cfg_ptr,
    /* Pointer to service configuration to be evaluated and altered if
       alternative configuration is to be generated */
  boolean suggest_alt,
    /* Indicates if a alternative configuration is to be generated */
  snm_eval_status_type curr_status
)
{
  snm_eval_status_type status = curr_status;
  snm_chan_mask_type fwd_scr_chan_mask = SNM_INIT_CHAN_MASK;
    /* Indicates forward channel configuration to be validated */
  snm_chan_mask_type rev_scr_chan_mask = SNM_INIT_CHAN_MASK;
    /* Indicates reverse channel configuration to be validated */
  uint8 i;
  uint8 max_sch = 0;
    /* Num of valid SCH cfg we found within SCH list in the BS cfg */
  word so_index;

#ifdef FEATURE_IS2000_REL_A
  caii_for_sch_spec_fields_type fwd_sch_fields;
    /* For SCH fields from MCCAP */
  caii_rev_sch_spec_fields_type rev_sch_fields;
    /* Rev SCH fields from MCCAP */
  uint8 max_rate = 0;
    /* Max rate for a given mux option */
#endif /* FEATURE_IS2000_REL_A */

  /* **********************************************************************
  ** Based on new configuration or channel masks, generate SCR masks that
  ** indicates active channel configurations to be validated.
  ** For SRQM, new configurations is used to generate SCR masks.
  ** For SCM and UHDM, channel masks are used to generate SCR masks.
  ** ********************************************************************** */
  snm_gen_scr_chan_mask(cfg_ptr, fwd_chan_mask, rev_chan_mask, (suggest_alt ? FALSE : TRUE),
    &fwd_scr_chan_mask, &rev_scr_chan_mask);

  if (cfg_ptr->for_sch_cc_incl && cfg_ptr->num_for_sch &&
      !negotiation_info.fwd_sch_cc_verified)
  {
    /* Since none of SO connections support SCH. So use SO 33 negotation
       parameters as reference for subsequent SCH config validation */
    if(snm_find_so(CAI_SO_PPP_PKT_DATA_3G, &so_index) == SNM_FAILURE)
    {
      return SNM_REJECT_CFG;
    }

    /* ***************************
    ** Validate Fwd SCH parameters
    ** *************************** */
    if (cfg_ptr->for_sch_cc_incl && cfg_ptr->num_for_sch &&
        !negotiation_info.fwd_sch_cc_verified)
    {
      /* Num cannot be greater than the max we support */
      max_sch = mccap_num_f_sch_supported();
      negotiation_info.fwd_sch_cc_verified = TRUE;

      /* If num of SCH is greater than the num we can support and we
         do not have to suggest an alt cfg, just reject right away */
      if (cfg_ptr->num_for_sch > max_sch)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "%d F-SCHs not supported",
          cfg_ptr->num_for_sch);
        return SNM_REJECT_CFG;
      }


      for (i = 0; ((i < cfg_ptr->num_for_sch) && (i < CAI_MAX_NUM_FOR_SCH)); i++)
      {
        /* ************
        ** Check SCH ID
        ** ************ */
        if (cfg_ptr->for_sch[i].for_sch_id >= max_sch)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "F-SCH ID %d not supported",
            cfg_ptr->for_sch[i].for_sch_id);
          return SNM_REJECT_CFG;
        }

        /* ********
        ** Check RC
        ** ******** */
        if ((status = snm_eval_sch_rc(SNM_FORWARD, fwd_scr_chan_mask, rev_scr_chan_mask,
             cfg_ptr, i, &snp.so[so_index], suggest_alt, status)) == SNM_REJECT_CFG)
        {
          /* Bad RC */
          return SNM_REJECT_CFG;
        }

        /* ****************
        ** Check mux option
        ** **************** */
        if ((status = snm_eval_sch_mux(SNM_FORWARD, cfg_ptr, i,
             &snp.so[so_index], SNM_RESERVED, suggest_alt, status)) == SNM_REJECT_CFG)
        {
          /* Bad MUX */
          return SNM_REJECT_CFG;
        }

#ifdef FEATURE_IS2000_REL_A
        if (P_REV_IS_7_OR_GREATER)
        {
          /* **************
          ** Check max rate
          ** ************** */
          if ((status = snm_eval_sch_max_rate(SNM_FORWARD, cfg_ptr, i, suggest_alt,
               status)) == SNM_REJECT_CFG)
          {
            /* Bad rate */
            return SNM_REJECT_CFG;
          }

          /* Max rate of 32X cannot be done in RC 3 */
          if ((cfg_ptr->for_sch[i].sch_chn_cfg.max_rate >=
               CAI_SCH_RATE_RS1_32X_RS2_18X) && /*lint !e641 */
              (cfg_ptr->for_sch[i].sch_chn_cfg.sch_rc == CAI_RC_3)) /*lint !e641 */
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Error : 32X with RC3 !!");
            return SNM_REJECT_CFG;
          }

          /* ****************
          ** Check frame size
          ** **************** */
          if ((status = snm_eval_sch_frame_size(SNM_FORWARD, cfg_ptr, i, suggest_alt,
               status)) == SNM_REJECT_CFG)
          {
            /* Bad frame size */
            return SNM_REJECT_CFG;
          }
        }
        else
        {
          /* If we are here, it means the p_rev_in_use is 6, default the
             max rate and other new fields correctly */
          if (cfg_ptr->for_sch[i].for_sch_mux == 0xf20)
          {
            (void) mccap_get_ch_specific_fields(MCCAP_FWD_SCH,
                     (void *) &fwd_sch_fields);
            max_rate = fwd_sch_fields.for_sch_recs[0].for_max_rate;
          }
          else
          {
            max_rate = snm_get_max_sch_rate_from_mux(cfg_ptr->for_sch[i].for_sch_mux);
          }
          cfg_ptr->for_sch[i].sch_chn_cfg.max_rate = max_rate;
          cfg_ptr->for_sch[i].sch_chn_cfg.frame_40_used = FALSE;
          cfg_ptr->for_sch[i].sch_chn_cfg.frame_80_used = FALSE;
        }
#endif /* FEATURE_IS2000_REL_A */

        /* *****************
        ** Check coding type
        ** ***************** */
        if ((status = snm_eval_sch_coding(SNM_FORWARD, cfg_ptr, i,
             suggest_alt, status)) == SNM_REJECT_CFG)
        {
          /* Bad coding type */
          return SNM_REJECT_CFG;
        }
      } /* end for */
    }

    /* ***************************
    ** Validate Rev SCH parameters
    ** *************************** */
    if (cfg_ptr->rev_sch_cc_incl && cfg_ptr->num_rev_sch &&
        !negotiation_info.rev_sch_cc_verified)
    {
      /* Since none of SO connections support SCH. So use SO 33 negotation
         parameters as reference for subsequent SCH config validation */
      if(snm_find_so(CAI_SO_PPP_PKT_DATA_3G, &so_index) == SNM_FAILURE)
      {
        return SNM_REJECT_CFG;
      }

      /* Num of r-SCH cannot be greater than the max we support */
      max_sch = mccap_num_r_sch_supported();
      negotiation_info.rev_sch_cc_verified = TRUE;

      /* If num of SCH is greater than the num we can support and we
         do not have to suggest an alt cfg, just reject right away */
      if (cfg_ptr->num_rev_sch > max_sch)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "%d R-SCHs not supported",
          cfg_ptr->num_rev_sch);
        return SNM_REJECT_CFG;
      }

      for (i = 0; ((i < cfg_ptr->num_rev_sch) && (i < CAI_MAX_NUM_REV_SCH)); i++)
      {
        if (cfg_ptr->rev_sch[i].rev_sch_id >= max_sch)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "R-SCH ID %d not supported",
            cfg_ptr->for_sch[i].for_sch_id);
          return SNM_REJECT_CFG;
        }

        /* ********
        ** Check RC
        ** ******** */
        if ((status = snm_eval_sch_rc(SNM_REVERSE, fwd_scr_chan_mask, rev_scr_chan_mask,
             cfg_ptr, i, &snp.so[so_index], suggest_alt, status)) == SNM_REJECT_CFG)
        {
          /* Bad RC */
          return SNM_REJECT_CFG;
        }

        /* ****************
        ** Check mux option
        ** **************** */
        if ((status = snm_eval_sch_mux(SNM_REVERSE, cfg_ptr, i,
             &snp.so[so_index], SNM_RESERVED, suggest_alt, status)) == SNM_REJECT_CFG) /*lint !e641 */
        {
          /* Bad MUX */
          return SNM_REJECT_CFG;
        }

#ifdef FEATURE_IS2000_REL_A
        if (P_REV_IS_7_OR_GREATER)
        {
          /* **************
          ** Check max rate
          ** ************** */
          if ((status = snm_eval_sch_max_rate(SNM_REVERSE, cfg_ptr, i, suggest_alt,
               status)) == SNM_REJECT_CFG)
          {
            /* Bad rate */
            return SNM_REJECT_CFG;
          }

          /* Max rate of 18X cannot be done in RC 4 */
          if ((cfg_ptr->rev_sch[i].sch_chn_cfg.max_rate >=
               CAI_SCH_RATE_RS1_32X_RS2_18X) && /*lint !e641 */
              (/* this macro returns 1 for RC4 Rate set 2 */
                 RATE_SET_OF_REV_RC(cfg_ptr->rev_sch[i].sch_chn_cfg.sch_rc) == 1)
             )
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Error : 18X with RC4 !!");
            return SNM_REJECT_CFG;
          }
          /* ****************
          ** Check frame size
          ** **************** */
          if ((status = snm_eval_sch_frame_size(SNM_REVERSE, cfg_ptr, i, suggest_alt,
               status)) == SNM_REJECT_CFG)
          {
            /* Bad frame size */
            return SNM_REJECT_CFG;
          }
        }
        else
        {
          /* If we are here, it means the p_rev_in_use is 6, default the
             max rate  and other new fields correctly */
          if (cfg_ptr->rev_sch[i].rev_sch_mux == 0xf20)
          {
            (void) mccap_get_ch_specific_fields(MCCAP_REV_SCH, &rev_sch_fields);
            max_rate = rev_sch_fields.rev_sch_recs[0].rev_max_rate;
          }
          else
          {
            max_rate = snm_get_max_sch_rate_from_mux(
                       cfg_ptr->rev_sch[i].rev_sch_mux);
          }

          cfg_ptr->rev_sch[i].sch_chn_cfg.max_rate = max_rate;
          cfg_ptr->rev_sch[i].sch_chn_cfg.frame_40_used = 0;
          cfg_ptr->rev_sch[i].sch_chn_cfg.frame_80_used = 0;
        }
#endif /* FEATURE_IS2000_REL_A */

        /* *****************
        ** Check coding type
        ** ***************** */
        if ((status = snm_eval_sch_coding(SNM_REVERSE, cfg_ptr, i, suggest_alt,
             status)) == SNM_REJECT_CFG)
        {
          /* Bad coding type */
          return SNM_REJECT_CFG;
        }
      } /* For loop */
    } /* end if (rev_sch_cc_incl) */

  }

  return status;

} /* snm_verify_sch_config_vs_ms_cap */

/*===========================================================================

FUNCTION snm_eval_config

DESCRIPTION
  This function evaluates a specified service configuration to see if it is
  supported by the mobile station.

  It the negotiation is started by a SNM managed SO, then SNM style
  negotiation is used.  It returns SNM_ACCEPT_CFG if the configuration is
  considered optimal, or if suggest_alt is FALSE, as long as the
  configuration can be supported by the mobile.

  There are several scenarios that the proposed configuration is not accepted:
  1. Config. contains more than 1 connection
  2. Service option not supported or enabled
  3. Mux. option (and other configuration variables) not acceptable

  For (1), the list of SO connections is scanned through to see if one of them
  can be supported.  If so, a configuration will be generated as a counter-
  proposal; otherwise, it will be handled like (2).

  For (2), starting from the first SO in the alt_so_list, an alternative for
  that SO will be sought.  If
  such an SO is located, a configuration based on that will be proposed (which
  results in SNM_ALT_CFG being returned).  If not found, or if the list of
  alternate SOs is exhausted, SNM_REJECT_CFG is returned.

  For (3), a configuration will be generated by taking the min. of the base
  station's mux.  options and the mobile's max. mux. options (unless max. mux.
  options have not been proposed before and if so, max. mux. will be proposed).  The other
  variables in the configuration (e.g. traffic type) will be corrected if
  necessary.

  To support the old form of negotiation (SOs not managed by SNM yet), if the
  evaluated SO is not in SNM, this function will return failure with the first
  connection set to CAI_SO_NULL.  This information can be used by the caller to
  determine if it should proceed with the old negotiation mechanism should this
  function returns failure.

  Note that suggest_alt controls whether an alternative configuration is to be
  generated.  When suggest_alt is false, alt_cfg_ptr is not used.

DEPENDENCIES
  negotiation_info must have been initialized.

RETURN VALUE
  SNM_ACCEPT_CFG if configuration is accepted as is,
  SNM_ALT_CFG if an alternative configuration is to be used (in alt_cfg_ptr),
  SNM_REJECT_CFG if configuration is not supported and there is no alternative
  to propose.

SIDE EFFECTS
  negotiation_info is updated if necessary.
  alt_cfg_ptr normally points to srv_proposed_cfg which is also the
  proposed_config member of negotiation_info.

===========================================================================*/

snm_eval_status_type snm_eval_config
(
  caii_srv_cfg_type *cfg_ptr,
    /* The service configuration to be evaluated */
  caii_srv_cfg_type *alt_cfg_ptr,
    /* Location to put alternate service configuration if desired */
  boolean restore_ind,
    /* Indicates if config is to be restored */
  boolean suggest_alt,
    /* Indicates if a alternative configuration is to be generated */
  mcc_fwd_chan_mask_type  fwd_chan_mask,
    /* The fwd channel mask to use for evaluation when P_REV is >=6 */
  mcc_rev_chan_mask_type  rev_chan_mask
    /* The rev channel mask to use for evaluation when P_REV is >=6 */
)
{
  word    wn, *wlist;
  byte    so_con;
  word    so_index=0;
  byte    con_index = 0, index;
  boolean use_sr_id;
  word    iterations_so_far;
  snm_eval_status_type status = SNM_ACCEPT_CFG;
  snm_eval_status_type status_list[MAX_CON];
  boolean data_so_valid = FALSE;
  boolean voice_so_valid = FALSE;

  if(suggest_alt)
  {
    MCC_CHK_NULL_PTR(alt_cfg_ptr)
  }

  for (index = 0; index < MAX_CON; index++)
  {
    status_list[index] = SNM_REJECT_CFG;
  }

  M1X_MSG( DCP, LEGACY_MED,
    "Eval cfg:Num Connections %d, fwd_mux %d, rev_mux %d",
    cfg_ptr->num_con,
    cfg_ptr->fwd_mux,
    cfg_ptr->rev_mux);

  M1X_MSG( DCP, LEGACY_MED,
    "SO of first connection record- as rxed in OTA %d",
    cfg_ptr->con[0].so);

  /* First of all, if the configuration contains no connections, then do a simple
     evaluation (note: SNM is a non-issue since no service option involved).
     In this case, we accept the configuration as long as it does not use
     assymmetric rate sets and all frame rates are used. */
  if (cfg_ptr->num_con == 0)
  {
    boolean good_mux_rate = FALSE;

    if ((cfg_ptr->fwd_mux == cfg_ptr->rev_mux) &&
        (cfg_ptr->fwd_rates == cfg_ptr->rev_rates) &&
        (((cfg_ptr->fwd_mux == CAI_MUX1) &&
          (cfg_ptr->fwd_rates == CAI_RS1_ALL)) ||
         ((cfg_ptr->fwd_mux == CAI_MUX2) &&
          (cfg_ptr->fwd_rates == CAI_RS2_ALL))))
    {
      good_mux_rate = TRUE;
    }

#ifdef FEATURE_IS2000_REL_B
    if (P_REV_IS_8_OR_GREATER)
    {
      if ((cfg_ptr->fch_dcch_mux_option_ind != CAI_MUX_OPTION_DIFF_FOR_FCH_AND_DCCH) || /*lint !e641 */
             /* Same MUXs used for both FCH and DCCH */
          ((cfg_ptr->fch_dcch_mux_option_ind == CAI_MUX_OPTION_DIFF_FOR_FCH_AND_DCCH) && /*lint !e641 */
           (cfg_ptr->for_dcch_mux == cfg_ptr->rev_dcch_mux) &&
           ((cfg_ptr->for_dcch_mux == CAI_MUX1) || (cfg_ptr->for_dcch_mux == CAI_MUX2))))
             /* Different MUXs for DCCH. Make sure DCCH MUXs are good */
      {
        good_mux_rate = TRUE;
      }
      else
      {
        good_mux_rate = FALSE;
      }
    }
#endif /* FEATURE_IS2000_REL_B */


    if (good_mux_rate)
    {
      return SNM_ACCEPT_CFG;
    }
    else
    {
      /* Return REJECT if the above validation fails */
      return SNM_REJECT_CFG;
    }
  }

  /* Reject BS proposed Voice SO if Origination is with DATA SO */
  
  if ( (cdma.call_orig) &&
       ( negotiation_info.con_list[0].sr_id_valid == TRUE ) &&
       ( CAI_IS_SO_DATA( negotiation_info.con_list[0].prev_so) ||
         ( negotiation_info.con_list[0].prev_so == CAI_SO_TDSO ) ||
         ( negotiation_info.con_list[0].prev_so == CAI_SO_FULL_TDSO) 
       )
     )
  {
    for (so_con = 0; so_con < cfg_ptr->num_con; so_con++)
    {
      if ( CAI_IS_SO_VOICE(cfg_ptr->con[so_con].so) ||
           CAI_IS_SO_SIGNALING ( cfg_ptr->con[so_con].so )
         )
      {
        voice_so_valid = TRUE;
        M1X_MSG( DCP, LEGACY_HIGH,
          "Proposed data SO %d - orig so %d",
          cfg_ptr->con[so_con].so,
          negotiation_info.con_list[0].prev_so);
      }
  
      if (negotiation_info.con_list[0].sr_id == cfg_ptr->con[so_con].sr_id)
      {
        break;
      }
    } /*End for (so_con = 0; so_con < cfg_ptr->num_con;so_con++) */
  
    if ( voice_so_valid == TRUE )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "SO mismatch - reject config, so_valid:%d",
        voice_so_valid);
      return SNM_REJECT_CFG;
    }
  } /* if ( ( negotiation_info.con_list[0].sr_id_valid == TRUE ) && */


  /* Reject BS proposed DATA SO if Origination is with VOICE SO */
  if ( (cdma.call_orig) &&
       (negotiation_info.con_list[0].sr_id_valid == TRUE) &&
       ( CAI_IS_SO_VOICE ( negotiation_info.con_list[0].prev_so) ||
         CAI_IS_SO_SIGNALING ( negotiation_info.con_list[0].prev_so) )
     )
  {
    for (so_con = 0; so_con < cfg_ptr->num_con; so_con++)
    {
      if ( CAI_IS_SO_DATA(cfg_ptr->con[so_con].so) ||
          (cfg_ptr->con[so_con].so == CAI_SO_TDSO ) ||
          (cfg_ptr->con[so_con].so == CAI_SO_FULL_TDSO)
         )
      {
        data_so_valid = TRUE;
        M1X_MSG( DCP, LEGACY_HIGH,
          "Proposed data SO %d - orig so %d",
          cfg_ptr->con[so_con].so,
          negotiation_info.con_list[0].prev_so);
      }

      if (negotiation_info.con_list[0].sr_id == cfg_ptr->con[so_con].sr_id)
      {
        break;
      }
    } /*End for (so_con = 0; so_con < cfg_ptr->num_con;so_con++) */

    if ( data_so_valid == TRUE )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "SO mismatch - reject config, data_so_valid:%d",
        data_so_valid);
      return SNM_REJECT_CFG;
    }
  } /* if ( ( negotiation_info.con_list[0].sr_id_valid == TRUE ) && */

  /* Now, we can proceed with the normal evaluation. */

  /* Mark fundicated channel configurations have not been verified */
  negotiation_info.fch_cc_verified = FALSE;
  negotiation_info.dcch_cc_verified = FALSE;

  /* Mark that sch_cc info has not been verified across the entire SCR */
  negotiation_info.fwd_sch_cc_verified = FALSE;
  negotiation_info.rev_sch_cc_verified = FALSE;

#ifdef FEATURE_IS2000_REL_A_SVD
  negotiation_info.fwd_traf.traffic_type_eval_so_far =
  negotiation_info.rev_traf.traffic_type_eval_so_far = FALSE;
  negotiation_info.fwd_traf.cfg_pri_con_index =
  negotiation_info.fwd_traf.cfg_sec_con_index =
  negotiation_info.rev_traf.cfg_pri_con_index =
  negotiation_info.rev_traf.cfg_sec_con_index = 0;
#endif /* FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_IS2000_REL_B
  /* Initialize FCH/DCCH MUX option indicator */
  if (!P_REV_IS_8_OR_GREATER)
  {
    cfg_ptr->fch_dcch_mux_option_ind = CAI_MUX_OPTION_SAME_FOR_FCH_AND_DCCH; /*lint !e641 */
  }
#endif /* FEATURE_IS2000_REL_B */

  local_srv_cfg = *cfg_ptr;

  /* We will come here only if the service negotiation is kicked off with
     a SNM managed SO. */

#ifdef FEATURE_IS2000_REL_A_SVD
  if (!P_REV_IS_7_OR_GREATER && (cfg_ptr->num_con > 1) && !suggest_alt )
  {
    M1X_MSG( DCP, LEGACY_MED,
      "SVD supported only in PREV >= 7");
    return SNM_REJECT_CFG;
  }
#endif /* FEATURE_IS2000_REL_A_SVD */

  if ((cfg_ptr->num_con > MAX_CON) && !suggest_alt)
  {
    /* More connections than we can support.  If we don't have to suggest any
       alternative, just exit with SNM_FAILURE */
    M1X_MSG( DCP, LEGACY_MED,
      "Too many connections");
    return SNM_REJECT_CFG;
  }

  /* See if we can use the sr_id in negotiation functions */
  use_sr_id = (P_REV_IS_6_OR_GREATER) ? TRUE : FALSE;

  /* Only evaluate configuration if the following conditions satisfy.
     If they don't, then we'll go ahead and suggest a fresh configuration
     using snm_gen_config() below (after this if-for). */

  if ((cfg_ptr->num_con <= MAX_CON)
#ifdef FEATURE_IS2000_REL_A_SVD
      && (P_REV_IS_7_OR_GREATER || (cfg_ptr->num_con == 1))
#endif /* FEATURE_IS2000_REL_A_SVD */
     )
  {
    for (so_con = 0, iterations_so_far = 0; so_con < cfg_ptr->num_con;)
    {
      /* Check to see if there is already a proposal for the connection.*/
      if (!snm_get_neg_index_for_sr_id(use_sr_id, cfg_ptr->con[so_con].sr_id,
          &con_index))
      {
        /* BS proposed a new connection by BS, store info. */
        if (!snm_get_free_neg_index(&con_index))
        {
          /* No room to hold new connection info.
             Normally, this should not happen but it can if BS went through
             multiple iterations of service negotation and used new CON_REF
             values each time. */

          M1X_MSG( DCP, LEGACY_MED,
            "No room for evaluating new connection");
          return SNM_REJECT_CFG;
        }

#ifdef FEATURE_IS2000_REL_A_SVD
        /* New proposal from the BS, mark the SO which can be arbitrarily
           changed during negotiation by the BS. */
        negotiation_info.con_list[con_index].so_can_be_changed = TRUE;
#endif /* FEATURE_IS2000_REL_A_SVD */

        if (P_REV_IS_6_OR_GREATER)
        {
          negotiation_info.con_list[con_index].sr_id =
            cfg_ptr->con[so_con].sr_id;
          negotiation_info.con_list[con_index].sr_id_valid = TRUE;
        }
      }

      M1X_MSG( DCP, LEGACY_MED,
        "con_index calculated = %d",
        con_index);
      if (negotiation_info.con_list[con_index].beginning_so == CAI_SO_REJ)
      {
        /* We don't have an SO to begin with, let's see if we can find one to
           use in SCR */
        if (snm_find_so(cfg_ptr->con[so_con].so, &so_index) != SNM_SUCCESS )
        {
          /* Can't find the SO, move on to evaluate the next one. */
          M1X_MSG( DCP, LEGACY_MED,
            "SO not found %d",
            cfg_ptr->con[so_con].so);
          goto LOOP_CONTROL;  /*lint !e801 */
        }
        M1X_MSG( DCP, LEGACY_MED,
          "Initializing neg_info with con_index=%d with the rxed SO",
          con_index);

        negotiation_info.con_list[con_index].neg_seed     =
        negotiation_info.con_list[con_index].beginning_so =
        negotiation_info.con_list[con_index].prev_so      =
          cfg_ptr->con[so_con].so;
        negotiation_info.con_list[con_index].next_so_index = 0;
        negotiation_info.con_list[con_index].alt_so_list =
          snp.so[so_index].negotiation_pars.alternate_so;
      }

      /* Now, evaluate the configurations. There are two cases. */

      M1X_MSG( DCP, LEGACY_MED,
        "SO of beginning record in neg_info %d",
        negotiation_info.con_list[con_index].beginning_so);

      /* Case 1: SO same as BEGINNING_SO */
      if (cfg_ptr->con[so_con].so ==
          negotiation_info.con_list[con_index].beginning_so)
      {
        /* Locate SO in SNP. BEGINNING_SO comes from SNP, no need to check for
           error here. */
        (void) snm_find_so(negotiation_info.con_list[con_index].beginning_so,
                 &so_index);

        M1X_MSG( DCP, LEGACY_MED,
          "Beginning and Rxed SO are the same .. SO status = %d",
          snp.so[so_index].status);
        if ((snp.so[so_index].status == SNM_ENABLED) &&
              snm_p_rev_verified(snp.so[so_index].negotiation_pars.min_p_rev))
        {
          /* Check to see if the configuration is acceptable */
          if ((status_list[so_con] =
               snm_check_con_and_generate_alt(&local_srv_cfg, restore_ind, so_con,
                                              &snp.so[so_index],suggest_alt,
                                              alt_cfg_ptr, con_index, fwd_chan_mask,
                                              rev_chan_mask)
              ) == SNM_REJECT_CFG)
          {
            return status_list[so_con];
          }

          if ((status_list[so_con] == SNM_ALT_CFG) && alt_cfg_ptr)
          {
            /* Update reference configuration to be used in next configuration
               evaluation */
            local_srv_cfg = *alt_cfg_ptr;
            status = SNM_ALT_CFG;
          }
          goto LOOP_CONTROL; /*lint !e801 */
        }
      }

      /* Case 2: SO different from BEGINNING_SO. See if it is in alternate
         list */
      for (wn = 0, wlist = negotiation_info.con_list[con_index].alt_so_list;
           wn < cai_ac_alt_so_max && wlist[wn] != CAI_SO_NULL; wn++)
      {
        if (wlist[wn] == cfg_ptr->con[so_con].so)
        {
          /* SO is in alternate list, need to make sure it is enabled */
          if (snm_find_so(wlist[wn], &so_index) != SNM_SUCCESS)
          {
            /* Should never happen. */
            M1X_MSG( DCP, LEGACY_ERROR,
              "Alt.SO not in SNM.  Seed: %d, alt.SO: %d",
              negotiation_info.con_list[con_index].neg_seed,
              wlist[wn]);
            break;
          }
          M1X_MSG( DCP, LEGACY_MED,
            "SO status = %d",
            snp.so[so_index].status);
          if ((snp.so[so_index].status == SNM_ENABLED) &&
              snm_p_rev_verified(snp.so[so_index].negotiation_pars.min_p_rev))
          {
            /* SO found in alternate SO list and is enabled and SO is supported
               under the current P_REV. */

            /* Check to see if the configuration is acceptable */
            if ((status_list[so_con] =
                 snm_check_con_and_generate_alt(&local_srv_cfg, restore_ind,
                                                so_con, &snp.so[so_index],
                                                suggest_alt, alt_cfg_ptr,
                                                con_index, fwd_chan_mask,
                                                rev_chan_mask)
                ) == SNM_REJECT_CFG)
            {
              M1X_MSG( DCP, LEGACY_MED,
                "status_list[so_con] =%d , rejecting..",
                status_list[so_con]);
              return status_list[so_con];
            }
            M1X_MSG( DCP, LEGACY_MED,
              "status_list[so_con] =%d",
              status_list[so_con]);

            if ((status_list[so_con] == SNM_ALT_CFG) && alt_cfg_ptr)
            {
              /* Update reference configuration to be used in next configuration
                 evaluation */
              local_srv_cfg = *alt_cfg_ptr;
              status = SNM_ALT_CFG;
            }
          }

          /* If SO is found in alt_so_list, regardless of whether the SO is
             usable or not, we are done with this connection, so we can break
             here. */
          break;
        }
      }

#ifdef FEATURE_IS2000_REL_A_SVD
      /* This block essentially allows the BS to arbitrarily alter the SOs for
         a certain connection (sr_id) during the service negotiation process.
         For instance, the BS may send a SREQ msg with {srid 1, SO1} and then
         later send a SREQ msg with {srid 1, SO2} where SO1 and SO2 are
         different families of SOs.

         Of course, this flexibility for the BS is not allowed for connections
         from a orig or page or eom. It is only allowed for connections which
         BS originated while we are in a call. */

      if (P_REV_IS_7_OR_GREATER &&
          /* SO not in alternate list */
          ((wn == cai_ac_alt_so_max) || (wlist[wn] == CAI_SO_NULL)) &&
          /* BS proposed connection */
          negotiation_info.con_list[con_index].so_can_be_changed)
      {
        /* Get the SO index */
        if (snm_find_so(cfg_ptr->con[so_con].so, &so_index) != SNM_SUCCESS)
        {
          /* Can't find the SO, move on to next connection */
          M1X_MSG( DCP, LEGACY_MED,
            "SO not found %d",
            cfg_ptr->con[so_con].so);
          goto LOOP_CONTROL;  /*lint !e801 */
        }

        M1X_MSG( DCP, LEGACY_MED,
          "Radical change of SO, old seed %d, old beginning so %d, new so %d",
          negotiation_info.con_list[con_index].neg_seed,
          negotiation_info.con_list[con_index].beginning_so,
          cfg_ptr->con[so_con].so);

        negotiation_info.con_list[con_index].neg_seed     =
        negotiation_info.con_list[con_index].beginning_so =
        negotiation_info.con_list[con_index].prev_so      =
          cfg_ptr->con[so_con].so;
        negotiation_info.con_list[con_index].next_so_index = 0;
        negotiation_info.con_list[con_index].alt_so_list =
          snp.so[so_index].negotiation_pars.alternate_so;

        if ((status_list[so_con] =
             snm_check_con_and_generate_alt(&local_srv_cfg, restore_ind, so_con,
                                            &snp.so[so_index],suggest_alt,
                                            alt_cfg_ptr, con_index, fwd_chan_mask,
                                            rev_chan_mask)
             ) == SNM_REJECT_CFG)
        {
          return status_list[so_con];
        }

        if ((status_list[so_con] == SNM_ALT_CFG) && alt_cfg_ptr)
        {
          /* Update reference configuration to be used in next configuration
             evaluation */
          local_srv_cfg = *alt_cfg_ptr;
          status = SNM_ALT_CFG;
        }
      } // if ( ((wn == cai_ac_alt_so_max) ..
#endif /* FEATURE_IS2000_REL_A_SVD */

   LOOP_CONTROL:
      if (status_list[so_con] != SNM_ALT_CFG)
      {
        /* Finished evaluation of this connection and the configuration has not
         * been changed. So, move on to the next connection. */
        so_con++;
      }
      else if (iterations_so_far++ < (cfg_ptr->num_con*2))
      {
        /* Alternate configuration has been suggested for this connection and
           and we have not exceeded the limit for number of iterations.
           We will restart evaluation from the first connection. */
        so_con = 0;

        /* Mark fundicated channel configurations have not been verified */
        negotiation_info.fch_cc_verified = FALSE;

        negotiation_info.dcch_cc_verified = FALSE;

      }
      else
      {
        /* Too many iterations of finding alternative for this SVD config,
           break out. */
        M1X_MSG( DCP, LEGACY_MED,
          "SNM exceeded limit of iterations, rej cfg");
        return SNM_REJECT_CFG;
      }
    } /* end for so_con... */
  } /* end if (cfg_ptr->num_con <= MAX_CON) */

  /* Summarize the evaluation status of all connections */
  for (index=0; index < cfg_ptr->num_con && index < MAX_CON; index++)
  {
    if (status_list[index] == SNM_REJECT_CFG)
    {
      /* Either (a) too many connections or
                (b) SO not found or not in alternate list */
      status = status_list[index];
      M1X_MSG( DCP, LEGACY_MED,
        "SO not found or not in alternate list");
      break;
    }
  }

  if (P_REV_IS_6_OR_GREATER)
  {
    /* Make sure all fundicated config got validated */
    if ((!negotiation_info.fch_cc_verified && local_srv_cfg.fch_cc_incl)
         || (!negotiation_info.dcch_cc_verified && local_srv_cfg.dcch_cc_incl)
       )
    {
      if ((status = snm_verify_fundicated_config_vs_ms_cap(fwd_chan_mask,
           rev_chan_mask, &local_srv_cfg, suggest_alt, status)) == SNM_ALT_CFG)
      {
        if(NULL != alt_cfg_ptr)
        {
          /* This copying over for the 'alternate' case is to capture the change
             made in snm_verify_...vs_ms_cap() */
           *alt_cfg_ptr = local_srv_cfg;
        }
        else
        {
           M1X_MSG( DCP, LEGACY_ERROR,
             "alt cfg pointer is NULL");
        }
      }
    }

    /* Make sure F-SCH config got validated */
    if ((status != SNM_REJECT_CFG) &&
        !negotiation_info.fwd_sch_cc_verified &&
        local_srv_cfg.for_sch_cc_incl && (local_srv_cfg.num_for_sch > 0))
    {
      if ((status = snm_verify_sch_config_vs_ms_cap(fwd_chan_mask, rev_chan_mask,
           &local_srv_cfg, suggest_alt, status)) == SNM_ALT_CFG)
      {
        if(NULL != alt_cfg_ptr)
        {
          /* This copying over for the 'alternate' case is to capture the change
             made in snm_verify_...vs_ms_cap() */
           *alt_cfg_ptr = local_srv_cfg;
        }
        else
        {
           M1X_MSG( DCP, LEGACY_ERROR,
             "alt cfg pointer is NULL");
        }
      }
    }

    /* Make sure R-SCH config got validated */
    if ((status != SNM_REJECT_CFG) &&
        !negotiation_info.rev_sch_cc_verified &&
        local_srv_cfg.rev_sch_cc_incl && (local_srv_cfg.num_rev_sch > 0))
    {
      if ((status = snm_verify_sch_config_vs_ms_cap(fwd_chan_mask, rev_chan_mask,
           &local_srv_cfg, suggest_alt, status)) == SNM_ALT_CFG)
      {
        if(NULL != alt_cfg_ptr)
        {
          /* This copying over for the 'alternate' case is to capture the change
             made in snm_verify_...vs_ms_cap() */
           *alt_cfg_ptr = local_srv_cfg;
        }
        else
        {
           M1X_MSG( DCP, LEGACY_ERROR,
             "alt cfg pointer is NULL");
        }

      }
    }

  }

  if (status == SNM_ACCEPT_CFG)
  {
    /* This copying over for the 'accept' case is to capture the change
       to MAX_RATE that is made for PREV 6 within
       check_con_and_generate_alt(). */
    *cfg_ptr = local_srv_cfg;
  }

  if ((status == SNM_ACCEPT_CFG) || (status == SNM_ALT_CFG))
  {
    /* We are done as after iterating through all the connections that
       were proposed, we are either accepting or proposing an alt.
       Return. */
    return status;
  }

  /* If we are here, it means either because there were too many connections or
     we couldn't find the proposed SO (for any connection) in SNM or it was not
     in our alternate list. If we can suggest alternate, we will propose a
     configuration below with a single connection. */

  if (!suggest_alt)
  {
    return SNM_REJECT_CFG;
  }

#ifdef FEATURE_IS2000_REL_A_SVD
  if (P_REV_IS_7_OR_GREATER &&
      /* BS proposed multiple connections */
      (cfg_ptr->num_con > 1) &&
      (negotiation_info.current_config_valid) &&
      /* SVD configurations */
      (negotiation_info.current_config->neg_scr.num_con > 1))
  {
    /* We'll just propose what we currently have, it is better than proposing
       just the one connection below. */
    *alt_cfg_ptr = negotiation_info.current_config->neg_scr;
    M1X_MSG( DCP, LEGACY_MED,
      "Proposing the current configuration");
    return SNM_ALT_CFG;
  }
#endif /* FEATURE_IS2000_REL_A_SVD */

  /* We should retrieve the first valid entry from negotiation_info
     database and use it here. This will be the connection we set up
     service negotiation for (whether it is a MO or MT call.) */
  if (!snm_get_neg_index_for_sr_id(FALSE, 0, &con_index))
  {
    /* Bad !!! should not happen */
    M1X_MSG( DCP, LEGACY_MED,
      "Failed to find entry in negotiation database");
    return SNM_REJECT_CFG;
  }

  if ((negotiation_info.con_list[con_index].beginning_so != CAI_SO_NULL) &&
      (negotiation_info.con_list[con_index].beginning_so != CAI_SO_REJ))
  {
    for (wn = negotiation_info.con_list[con_index].next_so_index,
         wlist = negotiation_info.con_list[con_index].alt_so_list;
         wn < cai_ac_alt_so_max && wlist[wn] != CAI_SO_NULL ; wn++)
    {
      /* Go through unproposed SOs in alt_so_list, locate it in SNP and check
         if it is enabled */

      if (snm_find_so(wlist[wn], &so_index) != SNM_SUCCESS)
      {
        /* Should never happen. */
        M1X_MSG( DCP, LEGACY_ERROR,
          "Alt.SO not in SNM.  Seed: %d, alt.SO: %d",
          negotiation_info.con_list[0].neg_seed,
          wlist[wn] );
        /* If this does happen, just skip and go to the next one. */
        continue;
      }

      if ((snp.so[so_index].status == SNM_ENABLED) &&
          snm_p_rev_verified( snp.so[so_index].negotiation_pars.min_p_rev))
      {
        /* Found an alternative that's enabled, update negotiation_info
           and return it. */
        negotiation_info.con_list[con_index].next_so_index = wn + 1;
        status = SNM_ALT_CFG;
        break;
      }
    } /* end for wn... */
  } /* end if (negoti.. */

  if (status == SNM_ALT_CFG) /* We found an SO_INDEX in above loop */
  {
    /* We have to propose an alternative SO.  Always use maximum MUX options. */
    if (snm_gen_config(snp.so[so_index].service_option_num, alt_cfg_ptr, TRUE,
        SNM_MAXIMUM_MUX, con_index, fwd_chan_mask, rev_chan_mask) == SNM_FAILURE)
    {
      /* Only happen if alternative SO does not support assigned channels. */
      M1X_MSG( DCP, LEGACY_MED,
        "Failed to generate cfg with alt SO %d",
        snp.so[so_index].service_option_num);
      status = SNM_REJECT_CFG;
    }
    else
    {
      if (P_REV_IS_6_OR_GREATER
#ifdef FEATURE_IS2000_REL_A_SVD
          /* If BS proposed multiple connections, we can't pick the sr_id from
             an arbitrary connection, so we'll not change it here, just accept
             the one from snm_gen_config() */
          && (cfg_ptr->num_con == 1)
#endif /* FEATURE_IS2000_REL_A_SVD */
         )
      {
        alt_cfg_ptr->con[0].sr_id = cfg_ptr->con[0].sr_id;

        negotiation_info.con_list[con_index].sr_id = alt_cfg_ptr->con[0].sr_id;
        negotiation_info.con_list[con_index].sr_id_valid = TRUE;
      }
      negotiation_info.con_list[con_index].max_mux_proposed = TRUE;
      M1X_MSG( DCP, LEGACY_MED,
        "Alt. cfg:SO %d, fwd_mux %d, rev_mux %d",
        alt_cfg_ptr->con[0].so,
        alt_cfg_ptr->fwd_mux,
        alt_cfg_ptr->rev_mux);

    } /* end else */
  } /* end if (status == SNM_ALT_CFG).. */

  return status;

} /* snm_eval_config */

/*===========================================================================

FUNCTION snm_verify_fundicated_rc_allowed

DESCRIPTION
  This function verifies if the fundicated RC specified can be used for the
  specified service option.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if supported; FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/

static boolean snm_verify_fundicated_rc_allowed
(
  snm_chan_mask_type channel,
    /* Channel */
  uint16 so,
    /* Service option */
  snm_chan_mask_type fwd_scr_chan_mask,
    /* Forward channel mask for service configuration */
  snm_chan_mask_type rev_scr_chan_mask,
    /* Reverse channel mask for service configuration */
  caii_srv_cfg_type *cfg_ptr
    /* The service configuration to be evaluated */
)
{
  uint16 so_index;
  cai_radio_config_type fwd_rc = CAI_INIT_RC, rev_rc = CAI_INIT_RC;

  if ((channel != SNM_FCH)
      && (channel != SNM_DCCH)
     )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid channel %d",
      channel);
    return FALSE;
  }

  if (snm_find_so(so, &so_index) == SNM_FAILURE)
  {
    /* If SO is not in SNM (possibly CAI_SO_REJ), return false */
    return FALSE;
  }

  /* Figure out RC values to check */
  if (channel == SNM_DCCH)
  {
    if ((fwd_scr_chan_mask | rev_scr_chan_mask) & SNM_DCCH)
    {
      fwd_rc = (cai_radio_config_type) cfg_ptr->for_dcch_rc;
      rev_rc = (cai_radio_config_type) cfg_ptr->rev_dcch_rc;
    }
    else
    {
      return FALSE;
    }
  }
  else
  if ((fwd_scr_chan_mask | rev_scr_chan_mask) & SNM_FCH)

  {
    if ((fwd_scr_chan_mask | rev_scr_chan_mask) & SNM_FCH)
    {
      fwd_rc = (cai_radio_config_type) cfg_ptr->for_fch_rc;
      rev_rc = (cai_radio_config_type) cfg_ptr->rev_fch_rc;
    }
    else
    {
      return FALSE;
    }
  }

  if (!snm_is_fundicated_rc_okay(&snp.so[so_index], (SNM_FORWARD | SNM_REVERSE),
      channel, fwd_scr_chan_mask, rev_scr_chan_mask, fwd_rc, rev_rc))
  {
    return FALSE;
  }

  return TRUE;

} /* snm_verify_fundicated_rc_allowed */

/*===========================================================================

FUNCTION snm_eval_non_neg_scr

DESCRIPTION
  This function evaluates the non negotiable service configuration that
  is passed into it.  This makes sure we can accept all the parameters that
  the BS is giving us. This record needs to be evaluated along with the SCR
  to make sure if we have valid FPC parameters if we operating in new RCs etc.
  Non negotiable SCRs unlike SCRs are cumulative i.e new record does not completely
  wipe out the old record. We modify what is specified in this record and leave
  the rest as is. It is important to reset this record in current config properly.
    :w

DEPENDENCIES

RETURN VALUE
  SNM_SUCCESS if configuration is acceptable, SNM_FAILURE otherwise.

SIDE EFFECTS
===========================================================================*/

snm_status_type snm_eval_non_neg_scr
(
  caii_non_neg_srv_cfg_type *nn_cfg_ptr,
    /* The Non-Neg service configuration record to be evaluated */
  caii_srv_cfg_type *neg_cfg_ptr,
    /*  The SCR that goes with the Non-neg part */
  mcc_fwd_chan_mask_type  fwd_chan_mask,
    /* Mask indicates active forward channels */
  mcc_rev_chan_mask_type  rev_chan_mask
    /* Mask indicates active reverse channels */
)
{
  uint8 i;
  boolean lpm_specified_rev_sig_chan = FALSE;
  boolean lpm_specified_fwd_sig_chan = FALSE;
  boolean rev_dtch_for_sr_id[MAX_SR_ID_VALUE+1];
  boolean fwd_dtch_for_sr_id[MAX_SR_ID_VALUE+1];
  uint16 so_for_sr_id[MAX_SR_ID_VALUE+1];
  snm_chan_mask_type fwd_scr_chan_mask = SNM_INIT_CHAN_MASK, rev_scr_chan_mask = SNM_INIT_CHAN_MASK;

  /***************************************************************/
  /***************************************************************/
  /* Validate LPM   (BEGIN)                                      */
  /***************************************************************/
  /***************************************************************/

  const caii_non_neg_srv_cfg_type *nnscr_for_lpm_eval = nn_cfg_ptr;

  /* ************************************************************************
  ** Based on channel masks, generate SCR masks that indicates active channel
  ** configurations to be validated.
  ** ********************************************************************** */
  snm_gen_scr_chan_mask(neg_cfg_ptr, fwd_chan_mask, rev_chan_mask, TRUE,
    &fwd_scr_chan_mask, &rev_scr_chan_mask);

  /* Initialize helper arrays */
  for (i = 0; i <= MAX_SR_ID_VALUE; i++)
  {
    rev_dtch_for_sr_id[i]=FALSE;
    fwd_dtch_for_sr_id[i]=FALSE;
    so_for_sr_id[i] = CAI_SO_NULL;
  }

  /* map SO's to sr_id, will be useful below while evaluating LPM */
  for (i = 0; i < neg_cfg_ptr->num_con; i++)
  {
    if (neg_cfg_ptr->con[i].sr_id <= MAX_SR_ID_VALUE)
    {
      so_for_sr_id[neg_cfg_ptr->con[i].sr_id] = neg_cfg_ptr->con[i].so;
    }
  }

  /* check lpm_ind */
  if (nnscr_for_lpm_eval->lpm_ind > CAI_LPM_LAST)  /*lint !e641 */
  {
    /* reject the configuration */
    M1X_MSG( DCP, LEGACY_MED,
      "Invalid lpm_ind %d",
      nnscr_for_lpm_eval->lpm_ind);
    return SNM_FAILURE;
  }

  /* Go through the LPM table and perform checks */
  for (i = 0; i < nnscr_for_lpm_eval->num_lpm_entries; i++)
  {
    /* check for validity of entries */
    if ((nnscr_for_lpm_eval->lpm_entries[i].logical_resource > CAI_LPM_DSCH) || /*lint !e641 */
        ((nnscr_for_lpm_eval->lpm_entries[i].physical_resource > CAI_LPM_SCH1) /*lint !e641 */
#ifdef FEATURE_IS2000_REL_C
         && !P_REV_IS_9_OR_GREATER
#endif /* FEATURE_IS2000_REL_C */
        ) ||
#ifdef FEATURE_IS2000_REL_C
        (P_REV_IS_9_OR_GREATER &&
         (nnscr_for_lpm_eval->lpm_entries[i].physical_resource > CAI_LPM_PDCH)) || /*lint !e641 */
#endif /* FEATURE_IS2000_REL_C */
        (nnscr_for_lpm_eval->lpm_entries[i].sr_id > MAX_SR_ID_VALUE))
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Invalid sr_id or logical/physical resource");
      return SNM_FAILURE;
    }

    /* Check if DCCH, if present, is okay */
    if ((nnscr_for_lpm_eval->lpm_entries[i].physical_resource == CAI_LPM_DCCH) /*lint !e641 */
        && !((fwd_chan_mask | rev_chan_mask) & (MCC_F_DCCH | MCC_R_DCCH))
       )
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DCCH not assigned or not supported, but included in LPM");
      continue;
    }

    /* check if FCH, if present, is okay */
    if ((nnscr_for_lpm_eval->lpm_entries[i].physical_resource == CAI_LPM_FCH) && /*lint !e641 */
        !((fwd_chan_mask | rev_chan_mask) & (MCC_F_FCH | MCC_R_FCH)))
    {
      M1X_MSG( DCP, LEGACY_MED,
        "FCH not assigned, but included in LPM");
      continue;
    }

#ifdef FEATURE_IS2000_REL_C
    /* check if PDCH, if present, is okay */
    if ((nnscr_for_lpm_eval->lpm_entries[i].physical_resource == CAI_LPM_PDCH) /*lint !e641 */
       )
    {
      M1X_MSG( DCP, LEGACY_MED,
        "PDCH not assigned or supported, but included in LPM");
      continue;
    }
#endif /* FEATURE_IS2000_REL_C */

    /* check if this is entry for signaling */
    if ((nnscr_for_lpm_eval->lpm_entries[i].logical_resource == CAI_LPM_DSCH) && /*lint !e641 */
        ((nnscr_for_lpm_eval->lpm_entries[i].physical_resource == CAI_LPM_FCH) || /*lint !e641 */
         (nnscr_for_lpm_eval->lpm_entries[i].physical_resource == CAI_LPM_DCCH) /*lint !e641 */
       ))
    {
      lpm_specified_fwd_sig_chan =
        (nnscr_for_lpm_eval->lpm_entries[i].forward_flag) ? TRUE : lpm_specified_fwd_sig_chan;
      lpm_specified_rev_sig_chan =
        (nnscr_for_lpm_eval->lpm_entries[i].reverse_flag) ? TRUE : lpm_specified_rev_sig_chan;
      continue;
    }

    /* check if this is a traffic entry in rev */
    if (nnscr_for_lpm_eval->lpm_entries[i].logical_resource == CAI_LPM_DTCH) /*lint !e641 */
    {
      word so = so_for_sr_id[nnscr_for_lpm_eval->lpm_entries[i].sr_id];
      uint8 sr_id = nnscr_for_lpm_eval->lpm_entries[i].sr_id;
      snm_chan_mask_type so_chan_mask = snm_gen_so_chan_mask(so);

      switch (nnscr_for_lpm_eval->lpm_entries[i].physical_resource)
      {
        case CAI_LPM_FCH:
          if (so_chan_mask & SNM_FCH)
          {
            /* Make sure the physical channel RC are allowed by SO. */
            if (snm_verify_fundicated_rc_allowed(SNM_FCH, so,
                fwd_scr_chan_mask, rev_scr_chan_mask, neg_cfg_ptr))
            {
              if (nnscr_for_lpm_eval->lpm_entries[i].reverse_flag)
              {
                rev_dtch_for_sr_id[sr_id] = TRUE;
              }
              if (nnscr_for_lpm_eval->lpm_entries[i].forward_flag)
              {
                fwd_dtch_for_sr_id[sr_id] = TRUE;
              }
            }
            else
            {
              M1X_MSG( DCP, LEGACY_MED,
                "LPM: SRID %d SO %d not compatible with FCH RC, reject NNSCR",
                sr_id,
                so);
              return SNM_FAILURE;
            }
          }
          break;

        case CAI_LPM_DCCH:
          if (so_chan_mask & SNM_DCCH)
          {
            /* Make sure the physical channel RC are allowed by SO. */
            if (snm_verify_fundicated_rc_allowed(SNM_DCCH, so,
                fwd_scr_chan_mask, rev_scr_chan_mask, neg_cfg_ptr))
            {
              if (nnscr_for_lpm_eval->lpm_entries[i].reverse_flag)
              {
                rev_dtch_for_sr_id[sr_id] = TRUE;
              }
              if (nnscr_for_lpm_eval->lpm_entries[i].forward_flag)
              {
                fwd_dtch_for_sr_id[sr_id] = TRUE;
              }
            }
            else
            {
              M1X_MSG( DCP, LEGACY_MED,
                "LPM: SRID %d SO %d not compatible with DCCH RC, reject NNSCR",
                sr_id,
                so);
              return SNM_FAILURE;
            }
          }
          break;

        /* This section below is a temporary fix for handling the situation
           that TDSO/STDSO services do not need a fundicated channel mapped
       to them. The final solution should ideally be incorporated in the
       SNM database negotiation parameters (possibly expanding the scope
       of the existing ch_ind field or otherwise) so as to avoid the
           SO-specific check being used below.*/
        case CAI_LPM_SCH0:
          if ((so_for_sr_id[sr_id] == CAI_SO_FULL_TDSO) ||
              (so_for_sr_id[sr_id] == CAI_SO_TDSO))
          {
            if (nnscr_for_lpm_eval->lpm_entries[i].reverse_flag)
            {
              rev_dtch_for_sr_id[sr_id] = TRUE;
            }
            if (nnscr_for_lpm_eval->lpm_entries[i].forward_flag)
            {
              fwd_dtch_for_sr_id[sr_id] = TRUE;
            }
          }
          break;

        default:
          /* No action needs to be taken for other channels */
          break;

      } /* switch */
    } /* if */

  } /* for i<nnscr_for_lpm_eval->num_lpm_entries */

  if (!lpm_specified_rev_sig_chan)
  {
    /* reject */
    M1X_MSG( DCP, LEGACY_MED,
      "No legal reverse signaling channel provided in LPM, reject NNSCR");
    return SNM_FAILURE;
  }

  if (!lpm_specified_fwd_sig_chan)
  {
    /* reject */
    M1X_MSG( DCP, LEGACY_MED,
      "No legal forward signaling channel provided in LPM, reject NNSCR");
    return SNM_FAILURE;
  }

  /* Now, let's make sure that every non-signaling, non-null traffic type service in the SCR
     has at least an entry in the LPM for reverse traffic */
  for (i=0; i<neg_cfg_ptr->num_con; i++)
  {
    if ( !CAI_IS_SO_SIGNALING(neg_cfg_ptr->con[i].so) &&
         (((neg_cfg_ptr->con[i].rev_traf != CAI_NUL_TRAF) && /*lint !e641 */
           !rev_dtch_for_sr_id[neg_cfg_ptr->con[i].sr_id]) ||
          ((neg_cfg_ptr->con[i].fwd_traf != CAI_NUL_TRAF) && /*lint !e641 */
           !fwd_dtch_for_sr_id[neg_cfg_ptr->con[i].sr_id])))
    {
      M1X_MSG( DCP, LEGACY_MED,
        "LPM did not specify fwd/rev entry for sr_id %d, reject NNSCR",
        neg_cfg_ptr->con[i].sr_id);
      return SNM_FAILURE;
    }
  } /* end for */

  /***************************************************************/
  /***************************************************************/
  /* Validate LPM   (END)                                        */
  /***************************************************************/
  /***************************************************************/

  /* Check FPC_PRI_CHAN */
  if (nn_cfg_ptr->fpc_incl)
  {
    /* Not OTA msg value, perform sanity check */
    if (nn_cfg_ptr->fpc_pri_chan == CAI_FPC_PRI_CHAN_CPCCH) /*lint !e641 */
    {
      if (((fwd_chan_mask & MCC_F_CPCCH) == 0) || /*lint !e641 */
          ((fwd_chan_mask & (MCC_F_FCH | MCC_F_DCCH)) != 0)) /*lint !e641 */
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Unacceptable FPC channel %d, reject NNSCR",
          nn_cfg_ptr->fpc_pri_chan);
        return SNM_FAILURE;
      }
    }
    else
    {
      if (((1 << nn_cfg_ptr->fpc_pri_chan) & fwd_chan_mask) == 0)  /*lint !e641 */
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Unacceptable FPC channel %d, reject NNSCR",
          nn_cfg_ptr->fpc_pri_chan);
        return SNM_FAILURE;
      }
    }
  }

#ifdef FEATURE_IS2000_REL_A
  if (P_REV_IS_7_OR_GREATER)
  {
    /* Check flex rate and LTU */
    if ((nn_cfg_ptr->use_flex_num_bits || nn_cfg_ptr->ltu_info_incl) &&
        !mccap_is_flex_rates_supported())
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No flex rates support, reject NNSCR");
      return SNM_FAILURE;
    }
    if (nn_cfg_ptr->use_var_rate && !mccap_is_sch_var_rates_supported())
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No var rates support, reject NNSCR");
      return SNM_FAILURE;
    }
  }
#endif /* FEATURE_IS2000_REL_A */

  return SNM_SUCCESS;

} /* snm_eval_non_neg_scr */

/*===========================================================================

FUNCTION snm_set_default_fields_in_scr

DESCRIPTION
 This function defaults certain fields of the neg. service configuration from the
 current configuration. Need to do this since only pending configuration is used
 for HH-RIF cases.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void snm_set_default_fields_in_scr
(
  caii_srv_cfg_type *neg_scr_ptr
)
{
  /* Changes to Channel Configuration in the SCR are incremental, so if
     pending cfg does not have a FCH, DCCH,SCH cc_incl fields , it just means
     we use the existing values */
  if (!neg_scr_ptr->fch_cc_incl &&
      negotiation_info.current_config->neg_scr.fch_cc_incl)
  {
    neg_scr_ptr->fch_cc_incl = TRUE;
    neg_scr_ptr->for_fch_rc =
      negotiation_info.current_config->neg_scr.for_fch_rc;
    neg_scr_ptr->rev_fch_rc =
      negotiation_info.current_config->neg_scr.rev_fch_rc;
    neg_scr_ptr->fch_frame_size =
      negotiation_info.current_config->neg_scr.fch_frame_size ;
  }

  if (!neg_scr_ptr->dcch_cc_incl &&
      negotiation_info.current_config->neg_scr.dcch_cc_incl)
  {
    neg_scr_ptr->dcch_cc_incl = TRUE;
    neg_scr_ptr->for_dcch_rc =
      negotiation_info.current_config->neg_scr.for_dcch_rc;
    neg_scr_ptr->rev_dcch_rc =
      negotiation_info.current_config->neg_scr.rev_dcch_rc;
    neg_scr_ptr->dcch_frame_size =
      negotiation_info.current_config->neg_scr.dcch_frame_size ;
  }

  if (!neg_scr_ptr->for_sch_cc_incl &&
      negotiation_info.current_config->neg_scr.for_sch_cc_incl)
  {
    uint8 i;

    neg_scr_ptr->for_sch_cc_incl = TRUE;
    neg_scr_ptr->num_for_sch =
      negotiation_info.current_config->neg_scr.num_for_sch;
    for (i=0; i<neg_scr_ptr->num_for_sch;i++)
    {
      neg_scr_ptr->for_sch[i] =
        negotiation_info.current_config->neg_scr.for_sch[i];
    }
  }

  if (!neg_scr_ptr->rev_sch_cc_incl &&
      negotiation_info.current_config->neg_scr.rev_sch_cc_incl)
  {
    uint8 i;

    neg_scr_ptr->rev_sch_cc_incl = TRUE;
    neg_scr_ptr->num_rev_sch =
      negotiation_info.current_config->neg_scr.num_rev_sch;
    for (i=0; i<neg_scr_ptr->num_rev_sch;i++)
    {
      neg_scr_ptr->rev_sch[i] =
        negotiation_info.current_config->neg_scr.rev_sch[i];
    }
  }

  /* The following is intended primarily for non-REL A builds to
     help the old MUX SCR interface (mcctxns.c) deal correctly with the
     case when num_con = 0 */
  if (neg_scr_ptr->num_con == 0)
  {
    /* Set the SO of first con to NULL */
    M1X_MSG( DCP, LEGACY_MED,
      "Received SCR with zero con rec");
    neg_scr_ptr->con[0].so = CAI_SO_NULL;
  }

} /* snm_set_default_fields_in_scr */

/*===========================================================================

FUNCTION snm_set_default_fields_in_nnscr

DESCRIPTION
 This function defaults certain fields of the non-neg service configuration from the
 current configuration. Need to do this since only pending configuration is used
 for action packet generation

DEPENDENCIES
  Does not populate LPM table in NNSCR; That should be called separately

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void snm_set_default_fields_in_nnscr
(
  caii_non_neg_srv_cfg_type * nnscr
)
{

  /*********************************************************************
   It is necessary to have the LPM table in nnscr pointer updated before
   this
  **********************************************************************/

  if (!nnscr->fpc_incl)
  {
    nnscr->fpc_incl = negotiation_info.current_config->non_neg_scr.fpc_incl;
    negotiation_info.current_config->non_neg_scr.fpc_pri_chan =
      nnscr->fpc_pri_chan =
    negotiation_info.current_config->non_neg_scr.fpc_pri_chan;
    nnscr->fpc_mode=
    negotiation_info.current_config->non_neg_scr.fpc_mode;
    nnscr->fpc_olpc_fch_incl =
    negotiation_info.current_config->non_neg_scr.fpc_olpc_fch_incl ;
    if (nnscr->fpc_olpc_fch_incl)
    {
      nnscr->fpc_fch_fer =
        negotiation_info.current_config->non_neg_scr.fpc_fch_fer;
      nnscr->fpc_fch_min_setpt =
        negotiation_info.current_config->non_neg_scr.fpc_fch_min_setpt;
      nnscr->fpc_fch_max_setpt =
        negotiation_info.current_config->non_neg_scr.fpc_fch_max_setpt;
    }

    nnscr->fpc_olpc_dcch_incl =
    negotiation_info.current_config->non_neg_scr.fpc_olpc_dcch_incl ;
    if (nnscr->fpc_olpc_dcch_incl)
    {
      nnscr->fpc_dcch_fer =
        negotiation_info.current_config->non_neg_scr.fpc_dcch_fer;
      nnscr->fpc_dcch_min_setpt =
        negotiation_info.current_config->non_neg_scr.fpc_dcch_min_setpt;
      nnscr->fpc_dcch_max_setpt =
        negotiation_info.current_config->non_neg_scr.fpc_dcch_max_setpt;
    }
  } /* end if (nnscr->fpc_incl) */
  else
  {
    if (!nnscr->fpc_olpc_fch_incl &&
        negotiation_info.current_config->non_neg_scr.fpc_olpc_fch_incl)
    {
      nnscr->fpc_olpc_fch_incl = TRUE;
      nnscr->fpc_fch_fer =
        negotiation_info.current_config->non_neg_scr.fpc_fch_fer;
      nnscr->fpc_fch_min_setpt =
        negotiation_info.current_config->non_neg_scr.fpc_fch_min_setpt;
      nnscr->fpc_fch_max_setpt =
        negotiation_info.current_config->non_neg_scr.fpc_fch_max_setpt;
    }

    if (!nnscr->fpc_olpc_dcch_incl &&
        negotiation_info.current_config->non_neg_scr.fpc_olpc_dcch_incl)
    {
      nnscr->fpc_olpc_dcch_incl = TRUE;
      nnscr->fpc_dcch_fer =
        negotiation_info.current_config->non_neg_scr.fpc_dcch_fer;
      nnscr->fpc_dcch_min_setpt =
        negotiation_info.current_config->non_neg_scr.fpc_dcch_min_setpt;
      nnscr->fpc_dcch_max_setpt =
        negotiation_info.current_config->non_neg_scr.fpc_dcch_max_setpt;
    }

  }

  if (!nnscr->gating_rate_incl &&
      negotiation_info.current_config->non_neg_scr.gating_rate_incl)
  {
    nnscr->gating_rate_incl = TRUE;
    nnscr->pilot_gate_rate =
      negotiation_info.current_config->non_neg_scr.pilot_gate_rate;
  }

} /* snm_set_default_fields_in_nnscr */

/*===========================================================================

FUNCTION snm_is2000_scr_init

DESCRIPTION
  Initialize the IS2000 fields in a SCR.  Useful when dealing with P_REV lower
  than 6 service configurations.

DEPENDENCIES
  Input configuration must have valid non-IS2000 fields filled.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void snm_is2000_scr_init
(
  caii_srv_cfg_type  *cfg_ptr
    /* Pointer to configuration to be initialized */
)
{
  /* For P_REV lower than 6, only FCH is included. */
  cfg_ptr->fch_cc_incl = TRUE;
  cfg_ptr->fch_frame_size = 0; /* 20ms frame only */

  /* derive RC from multiplex option */
  if (cfg_ptr->fwd_mux % 2)
  {
    cfg_ptr->for_fch_rc = CAI_RC_1; /*lint !e641 */
    cfg_ptr->rev_fch_rc = CAI_RC_1; /*lint !e641 */
  }
  else
  {
    cfg_ptr->for_fch_rc = CAI_RC_2; /*lint !e641 */
    cfg_ptr->rev_fch_rc = CAI_RC_2; /*lint !e641 */
  }

  cfg_ptr->dcch_cc_incl = FALSE;
  cfg_ptr->for_sch_cc_incl = FALSE;
  cfg_ptr->rev_sch_cc_incl = FALSE;

#ifdef FEATURE_IS2000_REL_C
  cfg_ptr->for_pdch_cc_incl = FALSE;
#endif /* FEATURE_IS2000_REL_C */

  /* sr_id */
#ifdef FEATURE_IS2000_REL_A
  if (cdma.call_orig)
  {
    cfg_ptr->con[0].sr_id = cdma.sr_id;
  }
  else
#endif /* FEATURE_IS2000_REL_A */
  {
    cfg_ptr->con[0].sr_id = CAI_SR_ID_1; /*lint !e641 */
  }

#ifdef FEATURE_IS2000_REL_B
  cfg_ptr->fch_dcch_mux_option_ind = CAI_MUX_OPTION_SAME_FOR_FCH_AND_DCCH; /*lint !e641 */
#endif /* FEATURE_IS2000_REL_B */

} /* snm_is2000_scr_init */

/*===========================================================================

FUNCTION snm_is2000_nnscr_init

DESCRIPTION
  Initialize the fields in a NNSCR.  Useful when dealing with P_REV lower
  than 6 service configurations.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void snm_is2000_nnscr_init
(
  caii_non_neg_srv_cfg_type *nnscr /* Pointer to nnscr to be initialized */
)
{

  /* Just mark everything as not included */
  nnscr->fpc_incl = FALSE;
  nnscr->fpc_olpc_fch_incl = FALSE;
  nnscr->fpc_olpc_fch_incl = FALSE;
  nnscr->fpc_olpc_dcch_incl = FALSE;
  nnscr->fpc_pri_chan = 0;

#ifdef FEATURE_IS2000_1X_ADV
  /* fpc_mode is received or initialized in RCP data while processing ECAM.
     So initialize it from RCP data. Check if fpc_mode is not initialized, just
     for worst-case error scenario. */
  if (mcc_rcp_data.rx_tx_params.fpc_mode.incl)
  {
    nnscr->fpc_mode = mcc_rcp_data.rx_tx_params.fpc_mode.value;
  }
  else
#endif /* FEATURE_IS2000_1X_ADV */
  {
    nnscr->fpc_mode = 0;
  }

  M1X_MSG( DCP, LEGACY_MED,
    "NNSCR FPC_MODE initialized to %d",
    nnscr->fpc_mode);

  nnscr->gating_rate_incl = FALSE;
  nnscr->lpm_ind = 0;

} /* snm_is2000_nnscr_init */

/*===========================================================================

FUNCTION snm_is2000_nnscr_lpm_default

DESCRIPTION
  Initialize the LPM fields in a NNSCR to default values.
  sr_id_list[] specifies the sr_id values to use in the default table,
  fwd_chan_mask and rev_chan_mask specifies the channel resources in effect
  and rc_class_2 specifies the current RC class.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void snm_is2000_nnscr_lpm_default
(
  caii_non_neg_srv_cfg_type *nnscr,
    /* Pointer to nnscr to be initialized */
  byte sr_id_count,
  byte sr_id_list[],
  mcc_fwd_chan_mask_type fwd_chan_mask,
  mcc_rev_chan_mask_type rev_chan_mask,
  boolean rc_class_2
)
{
  byte index;

#define LPM_BOUND_CHECK(num_lpm_entries)                                      \
   if (num_lpm_entries >= CAI_MAX_NUM_LPM_ENTRIES)                            \
   {                                                                          \
     /* This should never happen given the phy channels we support now and */ \
     /* the number of calls we support now. This is a sanity check         */ \
     /* If this happens, an internal error has occurred                    */ \
     ERR_FATAL("Default LPM exceeding storage limit %d", num_lpm_entries,0,0);\
   }

  nnscr->lpm_ind = CAI_LPM_DEFAULT; /*lint !e641 */
  nnscr->num_lpm_entries  = 0;

  /* Internal error check */
  if ((sr_id_count > 1)
      #ifdef FEATURE_IS2000_REL_A
      && !P_REV_IS_7_OR_GREATER //lint !e506 !e774
      #endif /* FEATURE_IS2000_REL_A */
     )
  {
    /* Should never happen, if it does, some internal error has occurred */
    ERR_FATAL("For prev <= 6, default LPM can't have more than one service", 0,0,0);
  }

  if ((fwd_chan_mask & MCC_F_FCH) || (rev_chan_mask & MCC_R_FCH))
  {
    LPM_BOUND_CHECK( nnscr->num_lpm_entries );
    nnscr->lpm_entries[nnscr->num_lpm_entries].sr_id = 0;
    nnscr->lpm_entries[nnscr->num_lpm_entries].logical_resource = CAI_LPM_DSCH; /*lint !e641 */
    nnscr->lpm_entries[nnscr->num_lpm_entries].physical_resource = CAI_LPM_FCH; /*lint !e641 */
    nnscr->lpm_entries[nnscr->num_lpm_entries].forward_flag =
      (fwd_chan_mask & MCC_F_FCH) ? TRUE : FALSE;
    nnscr->lpm_entries[nnscr->num_lpm_entries].reverse_flag =
      (rev_chan_mask & MCC_R_FCH) ? TRUE : FALSE;
    nnscr->num_lpm_entries++;

    for (index=0; index < sr_id_count; index++)
    {
      LPM_BOUND_CHECK( nnscr->num_lpm_entries );
      nnscr->lpm_entries[nnscr->num_lpm_entries].sr_id = sr_id_list[index];
      nnscr->lpm_entries[nnscr->num_lpm_entries].logical_resource = CAI_LPM_DTCH; /*lint !e641 */
      nnscr->lpm_entries[nnscr->num_lpm_entries].physical_resource = CAI_LPM_FCH; /*lint !e641 */
      nnscr->lpm_entries[nnscr->num_lpm_entries].forward_flag =
        (fwd_chan_mask & MCC_F_FCH) ? TRUE : FALSE;
      nnscr->lpm_entries[nnscr->num_lpm_entries].reverse_flag =
        (rev_chan_mask & MCC_R_FCH) ? TRUE : FALSE;
      nnscr->num_lpm_entries++;
    }
    //lint +e539
  } /* FCH */

  if ((fwd_chan_mask & MCC_F_DCCH) || (rev_chan_mask & MCC_R_DCCH))
  {
    if (
         #ifdef FEATURE_IS2000_REL_A
         P_REV_IS_7_OR_GREATER ||
         #endif /* FEATURE_IS2000_REL_A */
         !(fwd_chan_mask & MCC_F_FCH)
       )
    {
      LPM_BOUND_CHECK( nnscr->num_lpm_entries );
      nnscr->lpm_entries[nnscr->num_lpm_entries].sr_id = 0;
      nnscr->lpm_entries[nnscr->num_lpm_entries].logical_resource = CAI_LPM_DSCH; /*lint !e641 */
      nnscr->lpm_entries[nnscr->num_lpm_entries].physical_resource = CAI_LPM_DCCH; /*lint !e641 */
      nnscr->lpm_entries[nnscr->num_lpm_entries].forward_flag =
        (fwd_chan_mask & MCC_F_DCCH) ? TRUE : FALSE;
      nnscr->lpm_entries[nnscr->num_lpm_entries].reverse_flag =
        (rev_chan_mask & MCC_R_DCCH) ? TRUE : FALSE;
      nnscr->num_lpm_entries++;
    }

    for (index=0; index < sr_id_count; index++)
    {
      LPM_BOUND_CHECK( nnscr->num_lpm_entries );
      nnscr->lpm_entries[nnscr->num_lpm_entries].sr_id = sr_id_list[index];
      nnscr->lpm_entries[nnscr->num_lpm_entries].logical_resource = CAI_LPM_DTCH; /*lint !e641 */
      nnscr->lpm_entries[nnscr->num_lpm_entries].physical_resource = CAI_LPM_DCCH; /*lint !e641 */
      nnscr->lpm_entries[nnscr->num_lpm_entries].forward_flag =
        (fwd_chan_mask & MCC_F_DCCH) ? TRUE : FALSE;
      nnscr->lpm_entries[nnscr->num_lpm_entries].reverse_flag =
        (rev_chan_mask & MCC_R_DCCH) ? TRUE : FALSE;
      nnscr->num_lpm_entries++;
    }
    //lint -e539
  } /* DCCH */

  /* SCH0 */
  /*lint -e506 -e774 */
  #ifdef FEATURE_IS2000_REL_A
  if (!P_REV_IS_7_OR_GREATER || rc_class_2)
  #endif /* FEATURE_IS2000_REL_A */
  /*lint +e506 +e774 */
  {
    for (index=0; index < sr_id_count; index++)
    {
      LPM_BOUND_CHECK( nnscr->num_lpm_entries );
      nnscr->lpm_entries[nnscr->num_lpm_entries].sr_id = sr_id_list[index];
      nnscr->lpm_entries[nnscr->num_lpm_entries].logical_resource = CAI_LPM_DTCH; /*lint !e641 */
      nnscr->lpm_entries[nnscr->num_lpm_entries].physical_resource = CAI_LPM_SCH0; /*lint !e641 */
      nnscr->lpm_entries[nnscr->num_lpm_entries].forward_flag =
      nnscr->lpm_entries[nnscr->num_lpm_entries].reverse_flag = TRUE;
      nnscr->num_lpm_entries++;
    }
  }

#undef LPM_BOUND_CHECK

} /* snm_is2000_nnscr_lpm_default */

/*===========================================================================

FUNCTION snm_update_lpm_table

DESCRIPTION
  This function is called to update the lpm table parameters from a given
  non_neg_config to a target non_neg_cfg.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void snm_update_lpm_table
(
  caii_non_neg_srv_cfg_type       *target_nnscr,
  const caii_non_neg_srv_cfg_type *new_lpm_nnscr,
  const caii_srv_cfg_type         *new_scr,
  mcc_fwd_chan_mask_type          fwd_chan_mask,
  mcc_rev_chan_mask_type          rev_chan_mask
)
{
  byte sr_id_list[CAI_MAX_NUM_CONNECTIONS];
  byte sr_id_count;
  byte index;

  target_nnscr->lpm_ind = new_lpm_nnscr->lpm_ind;

  switch (new_lpm_nnscr->lpm_ind)
  {
    case CAI_LPM_DEFAULT:

      #ifdef FEATURE_IS2000_REL_A
      /* SR_IDs to go in the default LPM table (prev dependent) */
      if (P_REV_IS_7_OR_GREATER) //lint !e506 !e774
      {
        for (index=0; index < new_scr->num_con && 
             index < CAI_MAX_NUM_CONNECTIONS; index++)
        {
          sr_id_list[index] = new_scr->con[index].sr_id;
        }
        sr_id_count = index;
      }
      else
      #endif /* FEATURE_IS2000_REL_A */
      { /* Standard asks us to use '1', but later in release A this was fixed
           to use the sr_id from the configuration. There seems to be no harm
           in picking up this clarification for PREV 6 also */
        sr_id_list[0] = new_scr->con[0].sr_id;
        sr_id_count = 1;
      }
      /* Range check added for sr_id_count to silence Klocwork */
      if (sr_id_count <= CAI_MAX_NUM_CONNECTIONS)
      {
      snm_is2000_nnscr_lpm_default(target_nnscr, sr_id_count, sr_id_list,
                                   fwd_chan_mask, rev_chan_mask,
                                   new_scr->fch_cc_incl &&
                                   (new_scr->for_fch_rc >= CAI_RC_3) /*lint !e641 */
                                  ); //lint !e730 boolean argument above is okay
      }
      break;

    case CAI_LPM_SUPPLIED:
      /* Copy over to target config, RHS has been validated in snm_eval_non_neg_scr().
         When this function is called from srv_process_con_msg (pending cfg), the source and
         dest below are the same */
      target_nnscr->num_lpm_entries = new_lpm_nnscr->num_lpm_entries;

      for (index=0; index<new_lpm_nnscr->num_lpm_entries; index++)
      {
        target_nnscr->lpm_entries[index] = new_lpm_nnscr->lpm_entries[index];
      }
      break;

    case CAI_LPM_LAST:
      {
        const caii_non_neg_srv_cfg_type* cur_nnscr = snm_get_current_nnscr();

        /* When this func is called from snm_update_non_neg_configuration()
           (current cfg), the source and dest below are the same
           Copy over from current non neg cfg. */
        target_nnscr->num_lpm_entries = cur_nnscr->num_lpm_entries;

        for (index=0; index< cur_nnscr->num_lpm_entries; index++)
        {
          target_nnscr->lpm_entries[index] = cur_nnscr->lpm_entries[index];
        }
      }
      break;
  } /* switch */ //lint !e744 No default - this is later caught in snm_eval_non_neg..

} /* snm_update_lpm_table */

/*===========================================================================

FUNCTION snm_first_orig_config

DESCRIPTION
  Generates the first configuration to be proposed for an origination.  Max.
  mux. options will be used.  The generated configuration will be placed in
  *negotiation_info.proposed_cfg (pointing to srv_proposed_cfg).
  negotiation_info.max_mux_proposed will be set to TRUE.

DEPENDENCIES
  None

RETURN VALUE
  SNM_SUCCESS if configuration successfully generated; SNM_FAILURE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

snm_status_type snm_first_orig_config
(
  word so
)
{
  /* It is okay to use '0' here as this is the first config being proposed
     and we initialized negotiation_info.con_list[0] before */
  if (snm_gen_config(so, negotiation_info.proposed_config, TRUE, SNM_MAXIMUM_MUX, 0,
                     mccccl_code_channel_list.get_fwd_chan_mask(),
                     mccccl_code_channel_list.get_rev_chan_mask()) == SNM_FAILURE)
  {
    return SNM_FAILURE;
  }

  /* Since we are proposing max. mux. options, remember it. */
  negotiation_info.con_list[0].max_mux_proposed = TRUE;

  return SNM_SUCCESS;

} /* snm_first_orig_config */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION snm_update_sdb_so_omit_status

DESCRIPTION
  This calls the DS registered callback to report the SDB_SO_OMIT bit.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void snm_update_sdb_so_omit_status
(
  word so,
  boolean sdb_so_omit
)
{
  word so_index;
  if (snm_find_so(so, &so_index) == SNM_FAILURE)
  {
    return;
  }

  if (snp.so[so_index].sdb_so_omit_status_handler != NULL)
  {
    snp.so[so_index].sdb_so_omit_status_handler(sdb_so_omit);
  }

} /* snm_update_sdb_so_omit_status */
#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================

FUNCTION snm_update_non_neg_configuration

DESCRIPTION
  This function is called precisely at the action time of Service Connect
  Message or General Handoff Direction Message or Universal Handoff Direction
  Message to update the non-negotiatiable current configuration to what was
  given in the message.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void snm_update_non_neg_configuration
(
  void
)
{
  /* Copy pending_cfg to current_cfg */
  negotiation_info.current_config->non_neg_scr =
    negotiation_info.pending_config->non_neg_scr;

#ifdef FEATURE_IS2000_REL_A
  if (P_REV_IS_7_OR_GREATER)
  {
    int i, j;

    /* To do Klocwork error cleanup */
    if (negotiation_info.current_config->non_neg_scr.numrec > CAI_NNSCR_MAX_NUM_SR_REC)
    {
        M1X_MSG( DCP, LEGACY_MED,
          "numrec %d exceeds the maximum possible values %d",
          negotiation_info.current_config->non_neg_scr.numrec,
          CAI_NNSCR_MAX_NUM_SR_REC);
    }

    if (negotiation_info.current_config->neg_scr.num_con > CAII_SRV_CON_MAX)
    {
        M1X_MSG( DCP, LEGACY_MED,
          "num_con %d exceeds the maximum possible value %d",
          negotiation_info.current_config->neg_scr.num_con,
          CAII_SRV_CON_MAX);
    }

    /* Update the sdb_so_omit flag for registered call backs. */
    for (i = 0; i < negotiation_info.current_config->non_neg_scr.numrec &&
          i < CAI_NNSCR_MAX_NUM_SR_REC; i++)
    {
      for (j = 0; j < negotiation_info.current_config->neg_scr.num_con &&
            j < CAII_SRV_CON_MAX; j++)
      {
        if (negotiation_info.current_config->non_neg_scr.rec[i].sr_id ==
            negotiation_info.current_config->neg_scr.con[j].sr_id)
        {
          snm_update_sdb_so_omit_status(
            negotiation_info.current_config->neg_scr.con[j].so,
            negotiation_info.current_config->non_neg_scr.rec[i].sdb_so_omit);
        }
      } /* end for j */
    } /* end for i */
  }
#endif /* FEATURE_IS2000_REL_A */

} /* snm_update_non_neg_configuration */

/*===========================================================================

FUNCTION snm_update_connections

DESCRIPTION
  This function updates the connection_sod member of negotiation_info
  to point to the corresponding service option descriptors in SNP according to
  what's contained in current configuration.

DEPENDENCIES
  negotiation_info.current_cfg must point to the current configuration.

RETURN VALUE
  SNM_SUCCESS if successfully updated; SNM_FAILURE (say, connected SO not in SNP) otherwise.

SIDE EFFECTS
  None
===========================================================================*/

static snm_status_type snm_update_connections
(
  void
)
{
  word so_index;
  byte i, con_index;
  snm_status_type ret_value = SNM_SUCCESS;
  boolean use_sr_id;

  /* Check range just in case.  Defensive programming. */
  if (negotiation_info.current_config->neg_scr.num_con > MAX_CON)
  {
    return SNM_FAILURE;
  }

  /* We need to see if we can use the sr_id in negotiation functions */
  use_sr_id = (P_REV_IS_6_OR_GREATER) ? TRUE : FALSE;

  /* Establish indexes to service option descriptors of connections to
     eliminate looking them up later. */

  for (i=0; i < negotiation_info.current_config->neg_scr.num_con; i++)
  {
    if ((snm_find_so(negotiation_info.current_config->neg_scr.con[i].so, &so_index) == SNM_SUCCESS)
        &&
        snm_get_neg_index_for_sr_id(use_sr_id, negotiation_info.current_config->neg_scr.con[i].sr_id, &con_index))
    {
      negotiation_info.con_list[con_index].connection_sod = &snp.so[so_index];
    }
    else
    {
      ret_value = SNM_FAILURE;
    }
  }

  return ret_value;

} /* snm_update_connections */

/*===========================================================================

FUNCTION snm_initialize_connections

DESCRIPTION
  Calls the initializer function (if defined) of a particular connection of
  the current configuration.

DEPENDENCIES
  None

RETURN VALUE
  SNM_SUCCESS if the initializer was called; SNM_FAILURE if initializer was not called
  for any reason (e.g. not registered).

SIDE EFFECTS
  None
===========================================================================*/

static snm_status_type snm_initialize_connections (void)
{
  snm_so_descriptor_type* sod;
  snm_status_type ret_status = SNM_SUCCESS;
  byte i, con_index;
  boolean use_sr_id;
  boolean voice_so_included = FALSE;

  // we need to see if we can use the sr_id in negotiation functions
  use_sr_id = (P_REV_IS_6_OR_GREATER) ? TRUE : FALSE;

  for (i=0; i < negotiation_info.current_config->neg_scr.num_con; i++)
  {
    if (snm_get_neg_index_for_sr_id(use_sr_id, negotiation_info.current_config->neg_scr.con[i].sr_id, &con_index)
        &&
        (negotiation_info.con_list[con_index].neg_seed != CAI_SO_NULL)
        &&
        ((sod = negotiation_info.con_list[con_index].connection_sod) != NULL)
        &&
        (sod->connection_initializer != NULL))
    {
      /* Call the initializer */
      sod->connection_initializer(&negotiation_info.current_config->neg_scr, i);
    }
    else
    {
      ret_status = SNM_FAILURE;
    }

    /* Check to see whether there is any voice SO in this SCR. */
    if (!voice_so_included && CAI_IS_SO_VOICE(negotiation_info.current_config->neg_scr.con[i].so))
    {
      voice_so_included = TRUE;
    }
  }

  /* Release vocoder if there is no voice SO */
  if (!voice_so_included)
  {
#ifdef FEATURE_MODEM_1X_VOICE_SUPPORT

    #ifdef FEATURE_VOICE_AGENT_ARCHITECTURE
    onex_voc_cmd_open_t voice_adapter_params;  
    voice_adapter_params = mcc_get_vadapter_params();
    voice_adapter_params.event_cb( voice_adapter_params.session_context,
                                   ONEX_VOC_EVENT_RQST_STOP,
                                   NULL, /* STOP Event has no parameters */
                                   0
                                 );    
    M1X_MSG( DCP, LEGACY_MED,
             "Sent STOP request to Audio");
    #else
    mvs_release(MVS_CLIENT_CDMA);
    #endif /* FEATURE_VOICE_AGENT_ARCHITECTURE */
    
    mcc_is_mvs_acquired = FALSE;
#endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */
  }

  return ret_status;

} /* snm_initialize_connections() */

/*===========================================================================

FUNCTION snm_update_configuration

DESCRIPTION
  This function is called precisely at the action time of Service Connect Message
  to update the current configuration to what was given in the message.  Note that
  this function can be called from device driver's context (e.g. RXC) so it must
  only perform the absolute minimum.  This function perform the portion of SCR
  processing that's time critical (which includes calling the connection initializer).
  The function snm_activate_config does the other part of SCR processing that's not
  time critical.

DEPENDENCIES
  negotiation_info.pending_cfg must point to the pending configuration.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void snm_update_configuration
(
  void
)
{
  /* Copy pending_cfg to current_cfg */
  negotiation_info.current_config->neg_scr =
    negotiation_info.pending_config->neg_scr;

  /* Update non-neg SCR selectively i.e only values given in this new rec
     changes. Rest of the values remain the same. non-neg SCR is incremental. */
  if (P_REV_IS_6_OR_GREATER)
  {
    snm_update_non_neg_configuration();
  }

  /* Then update the connection indexes to point to the correct service option
     descriptors.  We can ignore the return since it's harmless (allows seamless
     migration of SOs to SNM since non-SNM SOs will result in error). */
  (void) snm_update_connections();

  /* Validate current configuration */
  negotiation_info.current_config_valid = TRUE;

  /* Now call the connection initializer */
  (void) snm_initialize_connections();

} /* snm_update_configuration */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION snm_submit_service_configuration_log

DESCRIPTION
  Fills a Service Configuration log buffer based on negotiated service
  contained in negotiation_info.  Submits the log to the logging thread.

DEPENDENCIES
  Requires negotiation_info data structure is populated.
  Requires that negotiation_info is not modified during execution of this
  function.

RETURN VALUE
  None.

SIDE EFFECTS
  Adds a log packet to the logging thread's buffer.
===========================================================================*/

static void snm_submit_service_configuration_log
(
  void
)
{
/* FC: Tools not ready
#ifdef FEATURE_IS2000_REL_C
  LOG_SRV_CONFIG_VER_4_C_type *scr_ptr;
#elif defined(FEATURE_IS2000_REL_B)
*/
#ifdef FEATURE_IS2000_REL_B
  LOG_SRV_CONFIG_VER_3_C_type *scr_ptr;
#else
  LOG_SRV_CONFIG_VER_2_C_type *scr_ptr;
#endif /* FEATURE_IS2000_REL_B */
    /* Pointer to buffer containing log packet */
  unsigned int buffer_size;
    /* Size of allocated buffer */
  unsigned int provisional_size;
    /* Actual size of log packet.  This must be <= buffer_size */
  unsigned int offset;
    /* Byte offset within variable length portion of log packet */
  uint16 sub_record_offset;
    /* Byte offset into rlp_blob */
  uint16 con_index;
    /* Index into negotiation_info.current_config->neg_scr.con[] */
  uint16 lpm_index;
    /* Index into negotiation_info.current_config->non_neg_scr.lpm_entries[] */
  uint16 rlp_blob_index;
    /* Index into negotiation_info.current_config->neg_scr.con[...].rlp_blob[]; */

  ch_info_type       *ch_info_ptr;
  sch_info_type      *sch_info_ptr;
  con_rec_info_type  *con_rec_info_ptr;
  lpm_info_type      *lpm_table_ptr;

#ifdef FEATURE_IS2000_REL_B
  snm_log_mux_info_type *mux_info_ptr;
#endif /* FEATURE_IS2000_REL_B */

  /*
   * Preallocate memory for log buffer.
   * This requires a calculation of the memory that will
   * be required.
   *
   * Note: This assumes that the values of the factors on the left do not
   *       change between the time they are calculated here and the time the
   *       fields are filled in later in this function.  If it is ever
   *       determined that there could be a race, the maximum size must be
   *       allocated here.
   */

  /* -1 below accounts for reference_buf */
/* FC: Tools not ready
#ifdef FEATURE_IS2000_REL_C
  buffer_size = sizeof(LOG_SRV_CONFIG_VER_4_C_type) - 1
#elif defined(FEATURE_IS2000_REL_B)
*/
#ifdef FEATURE_IS2000_REL_B
  buffer_size = FPOS(LOG_SRV_CONFIG_VER_3_C_type, reference_buf)
#else
  buffer_size = FPOS(LOG_SRV_CONFIG_VER_2_C_type, reference_buf)
#endif /* FEATURE_IS2000_REL_B */
    + (negotiation_info.current_config->neg_scr.fch_cc_incl)
      * sizeof(ch_info_type)
    + (negotiation_info.current_config->neg_scr.dcch_cc_incl)
      * sizeof(ch_info_type)
    + (negotiation_info.current_config->neg_scr.for_sch_cc_incl)
      * sizeof(sch_info_type)
    + (negotiation_info.current_config->neg_scr.rev_sch_cc_incl)
      * sizeof(sch_info_type)
    + (negotiation_info.current_config->neg_scr.num_con)
      * (FPOS(con_rec_info_type, reference_buf) + 15)
    /* 14 above accounts for reference_buf (-1) size plus max size
       of rlp_blob_len (15) */
    + (negotiation_info.current_config->non_neg_scr.num_lpm_entries)
      * sizeof(lpm_info_type)
    /* LTU tables length is not added because they are not supported. */

#ifdef FEATURE_IS2000_REL_B
    + (negotiation_info.current_config->neg_scr.dcch_cc_incl) *
      sizeof(snm_log_mux_info_type)
#endif /* FEATURE_IS2000_REL_B */

#ifdef FEATURE_IS2000_REL_C
    + 1
      /* Include Sync ID length value */

    + mccsyncid_get_active_sync_id_len()
      /* Include Sync ID value */

#endif /* FEATURE_IS2000_REL_C */

    ;

  /* Allocate memory for the log buffer scr. */
/* FC: Tools not ready
#ifdef FEATURE_IS2000_REL_C
  scr_ptr = (LOG_SRV_CONFIG_VER_4_C_type *) log_alloc
  (
    LOG_SRV_CONFIG_VER_4_C,
    buffer_size
  );
#elif defined(FEATURE_IS2000_REL_B)
*/
#ifdef FEATURE_IS2000_REL_B
  scr_ptr = (LOG_SRV_CONFIG_VER_3_C_type *) log_alloc
  (
    LOG_SRV_CONFIG_VER_3_C,
    buffer_size
  );
#else
  scr_ptr = (LOG_SRV_CONFIG_VER_2_C_type *) log_alloc
  (
    LOG_SRV_CONFIG_VER_2_C,
    buffer_size
  );
#endif /* FEATURE_IS2000_REL_B */

  /* If we are unable to allocate, then return without writing to
     the pointer. */
  if (!scr_ptr)
  {
     M1X_MSG( DCP, LEGACY_MED,
       "Unable to allocate log buffer" );
     return;
  }

  scr_ptr->fwd_mux_option =
    (uint16) negotiation_info.current_config->neg_scr.fwd_mux;
  scr_ptr->rev_mux_option =
    (uint16) negotiation_info.current_config->neg_scr.rev_mux;

  scr_ptr->fwd_num_bits = negotiation_info.current_config->neg_scr.fwd_rates;
  scr_ptr->rev_num_bits = negotiation_info.current_config->neg_scr.rev_rates;

  scr_ptr->channels.mask = 0;
  scr_ptr->channels.individual_channels.fch =
    negotiation_info.current_config->neg_scr.fch_cc_incl;
  scr_ptr->channels.individual_channels.dcch =
    negotiation_info.current_config->neg_scr.dcch_cc_incl;
  scr_ptr->channels.individual_channels.f_sch =
    negotiation_info.current_config->neg_scr.for_sch_cc_incl;
  scr_ptr->channels.individual_channels.r_sch =
    negotiation_info.current_config->neg_scr.rev_sch_cc_incl;

  scr_ptr->num_con_rec = negotiation_info.current_config->neg_scr.num_con;
  scr_ptr->num_lpm_entries =
    negotiation_info.current_config->non_neg_scr.num_lpm_entries;
  scr_ptr->num_ltu_tables = 0; // LTU tables are not supported currently

  /*********************************/
  /*  Fill reference buffer        */
  /*********************************/

  /*  Initialize offset  */
  offset = 0;

#ifdef FEATURE_IS2000_REL_B
  provisional_size = FPOS(LOG_SRV_CONFIG_VER_3_C_type, reference_buf);
#else
  provisional_size = FPOS(LOG_SRV_CONFIG_VER_2_C_type, reference_buf);
#endif /* FEATURE_IS2000_REL_B */
    /* Minus 1 for reference buffer */

  /* Check if FCH is active */
  if (scr_ptr->channels.individual_channels.fch)
  {
    /* Initialize ch_info_ptr pointer to fill FCH channel information */
    ch_info_ptr = (ch_info_type *) &scr_ptr->reference_buf[offset];

    ch_info_ptr->fwd_rc =
      negotiation_info.current_config->neg_scr.for_fch_rc;
    ch_info_ptr->rev_rc =
      negotiation_info.current_config->neg_scr.rev_fch_rc;

    offset += sizeof(ch_info_type);
  }

  /* Check if DCCH is active */
  if (scr_ptr->channels.individual_channels.dcch)
  {
    /*  Initialize ch_info_ptr pointer to fill DCCH channel information  */
    ch_info_ptr = (ch_info_type *) &scr_ptr->reference_buf[offset];

    ch_info_ptr->fwd_rc = negotiation_info.current_config->neg_scr.for_dcch_rc;
    ch_info_ptr->rev_rc = negotiation_info.current_config->neg_scr.rev_dcch_rc;

    offset += sizeof(ch_info_type);
  }

  /* Check if forward SCH is active */
  if (scr_ptr->channels.individual_channels.f_sch)
  {
    /*  Initialize sch_info_ptr pointer to fill SCH channel information  */
    sch_info_ptr = (sch_info_type *) &scr_ptr->reference_buf[offset];

    /* Only element 0 of for_sch is used, since only one sch is
       supported currently. */

    sch_info_ptr->sch_mux_hi =
      (uint8) ((negotiation_info.current_config->neg_scr.for_sch[0].for_sch_mux) >> 8);
    sch_info_ptr->sch_mux_lo =
      (uint8) ((negotiation_info.current_config->neg_scr.for_sch[0].for_sch_mux) & 0x00FF);
    sch_info_ptr->sch_rc =
      negotiation_info.current_config->neg_scr.for_sch[0].sch_chn_cfg.sch_rc;
    sch_info_ptr->coding =
      negotiation_info.current_config->neg_scr.for_sch[0].sch_chn_cfg.coding;

    sch_info_ptr->frame_size.frame_40_used =
      negotiation_info.current_config->neg_scr.for_sch[0].sch_chn_cfg.frame_40_used;
    sch_info_ptr->frame_size.frame_80_used =
      negotiation_info.current_config->neg_scr.for_sch[0].sch_chn_cfg.frame_80_used;
    sch_info_ptr->max_rate =
      negotiation_info.current_config->neg_scr.for_sch[0].sch_chn_cfg.max_rate;
    sch_info_ptr->frame_offset =
      negotiation_info.current_config->non_neg_scr.for_sch[0].frame_offset;

    offset += sizeof(sch_info_type);
  }

  /* Check if reverse SCH is active */
  if (scr_ptr->channels.individual_channels.r_sch)
  {
    /*  Initialize sch_info_ptr pointer to fill SCH channel information  */
    sch_info_ptr = (sch_info_type *) &scr_ptr->reference_buf[offset];

    sch_info_ptr->sch_mux_hi =
      (uint8) ((negotiation_info.current_config->neg_scr.rev_sch[0].rev_sch_mux) >> 8);
    sch_info_ptr->sch_mux_lo =
      (uint8) ((negotiation_info.current_config->neg_scr.rev_sch[0].rev_sch_mux) & 0x00FF);
    sch_info_ptr->sch_rc =
      negotiation_info.current_config->neg_scr.rev_sch[0].sch_chn_cfg.sch_rc;
    sch_info_ptr->coding =
      negotiation_info.current_config->neg_scr.rev_sch[0].sch_chn_cfg.coding;

    sch_info_ptr->frame_size.frame_40_used =
      negotiation_info.current_config->neg_scr.rev_sch[0].sch_chn_cfg.frame_40_used; /*lint -e661*/
    sch_info_ptr->frame_size.frame_80_used =
      negotiation_info.current_config->neg_scr.rev_sch[0].sch_chn_cfg.frame_80_used;
    sch_info_ptr->max_rate =
      negotiation_info.current_config->neg_scr.rev_sch[0].sch_chn_cfg.max_rate;

    sch_info_ptr->frame_offset = negotiation_info.current_config->non_neg_scr.rev_sch[0].frame_offset;

    offset += sizeof(sch_info_type);
  }

  /*  Fill connection records  */
  for (con_index=0; con_index < scr_ptr->num_con_rec; con_index++)
  {
    /*  Intialize con_rec_info_ptr pointer to fill connection record information  */
    con_rec_info_ptr =
      (con_rec_info_type *) &scr_ptr->reference_buf[offset];

    con_rec_info_ptr->con_ref =
      negotiation_info.current_config->neg_scr.con[con_index].con_ref;

    con_rec_info_ptr->service_option_hi =
      (uint8) ((negotiation_info.current_config->neg_scr.con[con_index].so) >> 8);
    con_rec_info_ptr->service_option_lo =
      (uint8) ((negotiation_info.current_config->neg_scr.con[con_index].so) & 0x00FF);
    con_rec_info_ptr->for_traffic =
      negotiation_info.current_config->neg_scr.con[con_index].fwd_traf;
    con_rec_info_ptr->rev_traffic =
      negotiation_info.current_config->neg_scr.con[con_index].rev_traf;
    con_rec_info_ptr->ui_encrypt_mode =
      negotiation_info.current_config->neg_scr.con[con_index].ui_encrypt_mode;
    con_rec_info_ptr->sr_id =
      negotiation_info.current_config->neg_scr.con[con_index].sr_id;

    con_rec_info_ptr->rlp_blob_len =
      negotiation_info.current_config->neg_scr.con[con_index].rlp_blob_len;
    con_rec_info_ptr->qos_params_len = 0;  /* QOS is currently not supported */

    offset += FPOS(con_rec_info_type, reference_buf);

    /* Initialize offset to fill buffer with rlp blob and QOS parameters */
    sub_record_offset = 0;

    for (rlp_blob_index = 0; (rlp_blob_index < con_rec_info_ptr->rlp_blob_len) &&
        ( sub_record_offset < 46) &&
        (rlp_blob_index < 15) &&
        (con_index < CAII_SRV_CON_MAX);rlp_blob_index++)
    {
      /*  Store rlp_blob stored in the MSM  */
      con_rec_info_ptr->reference_buf[sub_record_offset] =
        negotiation_info.current_config->neg_scr.con[con_index].rlp_blob[rlp_blob_index];
      sub_record_offset++;
    }

    // QOS is currently not supported
    // for (j=0; j < con_rec_info_ptr->qos_params_len; j++)
    // {
    //   Store QOS params stored in the MSM  */
    //   con_rec_info_ptr->reference_buf[sub_record_offset ++] = 0;  //FOR EXAMPLE
    // }

    offset += sub_record_offset;
  }

  /*  Fill LPM tables  */
  for (lpm_index = 0; lpm_index < scr_ptr->num_lpm_entries; lpm_index++)
  {
    /*  Intialize lpm_table_ptr pointer  */
    lpm_table_ptr =
      (lpm_info_type *) &scr_ptr->reference_buf[offset];

    lpm_table_ptr->sr_id =
      negotiation_info.current_config->non_neg_scr.lpm_entries[lpm_index].sr_id;
    lpm_table_ptr->logical_resource =
      negotiation_info.current_config->non_neg_scr.lpm_entries[lpm_index].logical_resource;
    lpm_table_ptr->physical_resource =
      negotiation_info.current_config->non_neg_scr.lpm_entries[lpm_index].physical_resource;

    lpm_table_ptr->forward_reverse_flag.mask = 0;
    lpm_table_ptr->forward_reverse_flag.individual_direction.forward =
      negotiation_info.current_config->non_neg_scr.lpm_entries[lpm_index].forward_flag;
    lpm_table_ptr->forward_reverse_flag.individual_direction.reverse =
      negotiation_info.current_config->non_neg_scr.lpm_entries[lpm_index].reverse_flag;

    lpm_table_ptr->priority =
      negotiation_info.current_config->non_neg_scr.lpm_entries[lpm_index].priority; /*lint -e796*/

    offset += sizeof(lpm_info_type);
  }

  /*  Fill LTU tables  */

  // *** LTU tables are not supported currently ***

//  ltu_info_type    *ltu_info;
//  ltu_row_type    *ltu_row;
//  ltu_tab_id_type  *ltu_tab_id;
//  for (i=0; i < scr_ptr->num_ltu_tables; i++)
//  {
//    //  Intialize ltu_info pointer
//    ltu_info = (ltu_info_type *) &scr_ptr->reference_buf[offset];
//
//    ltu_info->ltu_table_id = i;  //FOR EXAMPLE
//    ltu_info->num_rows = 2;      //FOR EXAMPLE
//
//    offset += 2;
//
//    // Initialize offset to fill buffer with LTU rows
//    sub_record_offset = 0;
//
//    for (j=0; j < ltu_info->num_rows; j++)
//    {
//      // Initialize ltu_row pointer
//      ltu_row = (ltu_row_type *) &ltu_info->reference_buf[sub_record_offset];
//
//      ltu_row->nbits_idx = 1;  //FOR EXAMPLE
//
//#define  LTU_LEN  368
//
//      ltu_row->ltu_len_hi = (uint8) (LTU_LEN >> 8);
//      ltu_row->ltu_len_lo = (uint8) (LTU_LEN & 0x00FF);
//
//      sub_record_offset += 3;
//    }
//
//    offset += sub_record_offset;
//  }
//
//  // Fill fsch0_ltu_tab_id
//  if (scr_ptr->num_ltu_tables)
//  {
//    // Initialize ltu_tab_id_type pointer
//    ltu_tab_id = (ltu_tab_id_type *) &scr_ptr->reference_buf[offset];
//
//    ltu_tab_id->fsch0_ltu_tab_id = 0;  //FOR EXAMPLE
//    ltu_tab_id->rsch0_ltu_tab_id = 1;  //FOR EXAMPLE
//
//    //  update offset
//    offset += 2;
//  }

#ifdef FEATURE_IS2000_REL_B
  if (negotiation_info.current_config->neg_scr.dcch_cc_incl)
  {
    /* Initialize mux_info_ptr pointer to fill mux information  */
    mux_info_ptr = (snm_log_mux_info_type *) &scr_ptr->reference_buf[offset]; /*lint !e797 out-of-bounds */

    if (negotiation_info.current_config->neg_scr.fch_dcch_mux_option_ind ==
        CAI_MUX_OPTION_DIFF_FOR_FCH_AND_DCCH) /*lint !e641 */
    {
      mux_info_ptr->for_dcch_mux_option_hi =
        (uint8) ((negotiation_info.current_config->neg_scr.for_dcch_mux) >> 8);
      mux_info_ptr->for_dcch_mux_option_lo =
        (uint8) ((negotiation_info.current_config->neg_scr.for_dcch_mux) & 0x00FF);
      mux_info_ptr->rev_dcch_mux_option_hi =
        (uint8) ((negotiation_info.current_config->neg_scr.rev_dcch_mux) >> 8);
      mux_info_ptr->rev_dcch_mux_option_lo =
        (uint8) ((negotiation_info.current_config->neg_scr.rev_dcch_mux) & 0x00FF);
    }
    else
    {
      mux_info_ptr->for_dcch_mux_option_hi =
        (uint8) ((negotiation_info.current_config->neg_scr.fwd_mux) >> 8);
      mux_info_ptr->for_dcch_mux_option_lo =
        (uint8) ((negotiation_info.current_config->neg_scr.fwd_mux) & 0x00FF);
      mux_info_ptr->rev_dcch_mux_option_hi =
        (uint8) ((negotiation_info.current_config->neg_scr.rev_mux) >> 8);
      mux_info_ptr->rev_dcch_mux_option_lo =
        (uint8) ((negotiation_info.current_config->neg_scr.rev_mux) & 0x00FF);
    }

    offset += sizeof(snm_log_mux_info_type);
  }
#endif /* FEATURE_IS2000_REL_B */

#if defined(FEATURE_IS2000_REL_C)
  /* Fill Sync ID information */
  scr_ptr->reference_buf[offset] = mccsyncid_get_active_sync_id_len(); /*lint !e797 !e796 */
  if (scr_ptr->reference_buf[offset] > 0) /*lint !e797 !e796 */
  {
    mccsyncid_get_active_sync_id((uint8 *) &scr_ptr->reference_buf[offset+1]); /*lint !e797 !e796 */
  }
  offset += scr_ptr->reference_buf[offset] + 1; /*lint !e797 !e796 */

#endif /* FEATURE_IS2000_REL_C */

  /*  Update provisional_size with offset  */
  provisional_size += offset;

  /* Verify that we didn't overrun the buffer. */
  if (provisional_size > buffer_size)
  {
    ERR_FATAL("Log exceeds buffer", 0, 0, 0);
  }
  else if (provisional_size < buffer_size)
  {
    log_shorten(scr_ptr, provisional_size);
    log_set_length(scr_ptr, provisional_size);
  }

  log_commit(scr_ptr);

} /* snm_submit_service_configuration_log */
#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================

FUNCTION snm_continue_update_config

DESCRIPTION
  This function is called at the action time of Service Connect Message to
  perform any processing related to the SCR that's not time critical and is
  called from MC's context.

DEPENDENCIES
  snm_update_configuration must have be called prior to this so that the
  current configuration contains what to be used.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void snm_continue_update_config
(
  void
)
{

#ifndef FEATURE_IS2000_REL_A
  mc_log_svc_config_type *log_ptr = NULL;
  uint16 log_len;
#endif /* !FEATURE_IS2000_REL_A */

  /* Command multiplex sublayer and service options to begin using new
     configuration */
  srv_cmd_cfg(&negotiation_info.current_config->neg_scr);

  /* Update modem statistics */
  /* This function is called whenever BS sends an SCR */
  cdma_stats.state_info.active_so = negotiation_info.current_config->neg_scr.con[0].so;
  cdma_stats.changed_bitmask |= SYS_CDMA_STATE_INFO_BIT_MASK;
  cdma_stats.channel_config_info.fsch0_mux = negotiation_info.current_config->neg_scr.for_sch[0].for_sch_mux;
  cdma_stats.channel_config_info.fsch0_rc = negotiation_info.current_config->neg_scr.for_sch[0].sch_chn_cfg.sch_rc;
  cdma_stats.channel_config_info.rsch0_mux = negotiation_info.current_config->neg_scr.rev_sch[0].rev_sch_mux;
  cdma_stats.channel_config_info.rsch0_rc = negotiation_info.current_config->neg_scr.rev_sch[0].sch_chn_cfg.sch_rc;
  cdma_stats.changed_bitmask |= SYS_CDMA_CHANNEL_CONFIG_INFO_BIT_MASK;

#ifndef FEATURE_IS2000_REL_A
  /* Perform logging here. */

  /* Calculate the length of this log packet. */
  log_len = FPOS(mc_log_svc_config_type, sch_info) +
              FSIZ(mc_log_svc_config_type, sch_info[0]) *
              (negotiation_info.current_config->neg_scr.num_for_sch +
               negotiation_info.current_config->neg_scr.num_rev_sch); //lint !e734

  log_ptr = (mc_log_svc_config_type *) log_alloc(LOG_SVC_CONFIG_C, log_len); //lint !e713

  /* If log_ptr is NULL, the log was dropped or not enabled. */
  if (log_ptr)
  {
    byte i, idx;  /* Index for array of SCH values */

    log_ptr->so = negotiation_info.current_config->neg_scr.con[0].so; //lint !e713

    /* FCH values */
    if (negotiation_info.current_config->neg_scr.fch_cc_incl)
    {
      log_ptr->fwd_ch_rc.f_fch_rc =
        negotiation_info.current_config->neg_scr.for_fch_rc;

      log_ptr->rev_ch_rc.r_fch_rc =
        negotiation_info.current_config->neg_scr.rev_fch_rc;

      log_ptr->fch_frame_size =
        negotiation_info.current_config->neg_scr.fch_frame_size;
    }
    else
    {
      log_ptr->fwd_ch_rc.f_fch_rc  = 0;
      log_ptr->rev_ch_rc.r_fch_rc  = 0;
      log_ptr->fch_frame_size = 0;
    }

    /* SCH values, if enabled */
    log_ptr->fwd_ch_rc.f_sch0_rc = 0;
    log_ptr->fwd_ch_rc.f_sch1_rc = 0;
    log_ptr->num_for_sch = 0;

    if (negotiation_info.current_config->neg_scr.for_sch_cc_incl)
    {
      log_ptr->num_for_sch = negotiation_info.current_config->neg_scr.num_for_sch;

      if (negotiation_info.current_config->neg_scr.num_for_sch > 0)
      {
        log_ptr->fwd_ch_rc.f_sch0_rc =
          negotiation_info.current_config->neg_scr.for_sch[0].sch_chn_cfg.sch_rc;

        if (negotiation_info.current_config->neg_scr.num_for_sch > 1)
        {
          log_ptr->fwd_ch_rc.f_sch1_rc =
            negotiation_info.current_config->neg_scr.for_sch[1].sch_chn_cfg.sch_rc;
        }
      }
    }

    log_ptr->rev_ch_rc.r_sch0_rc = 0;
    log_ptr->rev_ch_rc.r_sch1_rc = 0;
    log_ptr->num_rev_sch = 0;

    if (negotiation_info.current_config->neg_scr.rev_sch_cc_incl)
    {
      log_ptr->num_rev_sch = negotiation_info.current_config->neg_scr.num_rev_sch;

      if (negotiation_info.current_config->neg_scr.num_rev_sch > 0)
      {

        log_ptr->rev_ch_rc.r_sch0_rc =
          negotiation_info.current_config->neg_scr.rev_sch[0].sch_chn_cfg.sch_rc;

        if (negotiation_info.current_config->neg_scr.num_rev_sch > 1)
        {
          log_ptr->rev_ch_rc.r_sch1_rc =
            negotiation_info.current_config->neg_scr.rev_sch[1].sch_chn_cfg.sch_rc;
        }
      }
    }

    idx = 0; /* Init index to SCH array */

    /* Loop through all F-SCH channels */
    for (i = 0; i < log_ptr->num_for_sch; i++)
    {
      log_ptr->sch_info[idx].identifier.fwd_rev = 0;

      log_ptr->sch_info[idx].identifier.sch_id =
        negotiation_info.current_config->neg_scr.for_sch[i].for_sch_id;

      log_ptr->sch_info[idx].mux_opt =
        (int16)negotiation_info.current_config->neg_scr.for_sch[i].for_sch_mux;

      log_ptr->sch_info[idx].coding_type =
        (int16)negotiation_info.current_config->neg_scr.for_sch[i].sch_chn_cfg.coding;

      idx++;
    }

    /* Loop through all R-SCH channels */
    for (i = 0; i < log_ptr->num_rev_sch; i++)
    {
      log_ptr->sch_info[idx].identifier.fwd_rev = 1;

      log_ptr->sch_info[idx].identifier.sch_id =
        negotiation_info.current_config->neg_scr.rev_sch[i].rev_sch_id;

      log_ptr->sch_info[idx].mux_opt =
        (int16)negotiation_info.current_config->neg_scr.rev_sch[i].rev_sch_mux;

      log_ptr->sch_info[idx].coding_type =
        negotiation_info.current_config->neg_scr.rev_sch[i].sch_chn_cfg.coding;

      idx++;
    }

    /* DCCH values, if enabled */
    if (negotiation_info.current_config->neg_scr.dcch_cc_incl)
    {
      log_ptr->fwd_ch_rc.f_dcch_rc =
        negotiation_info.current_config->neg_scr.for_dcch_rc;

      log_ptr->rev_ch_rc.r_dcch_rc =
        negotiation_info.current_config->neg_scr.rev_dcch_rc;

      log_ptr->dcch_frame_size =
        negotiation_info.current_config->neg_scr.dcch_frame_size;
    }
    else
    {
      log_ptr->fwd_ch_rc.f_dcch_rc = 0;
      log_ptr->rev_ch_rc.r_dcch_rc = 0;
      log_ptr->dcch_frame_size = CAI_DCCH_20MS_FRAME;
    }

    /* Values not tied to specific channels */
    if (negotiation_info.current_config->neg_scr.num_con > 0)
    {
      log_ptr->fwd_traffic_type =
        negotiation_info.current_config->neg_scr.con[0].fwd_traf;

      log_ptr->rev_traffic_type =
        negotiation_info.current_config->neg_scr.con[0].rev_traf;
    }
    else
    {
      log_ptr->fwd_traffic_type = 0;
      log_ptr->rev_traffic_type = 0;
    }

    log_ptr->fwd_mux_opt = (int16)negotiation_info.current_config->neg_scr.fwd_mux;

    log_ptr->rev_mux_opt = (int16)negotiation_info.current_config->neg_scr.rev_mux;


    log_ptr->fwd_rate = negotiation_info.current_config->neg_scr.fwd_rates;

    log_ptr->rev_rate = negotiation_info.current_config->neg_scr.rev_rates;


    log_commit(log_ptr);
  } /* If log_ptr is valid */

#else /* !FEATURE_IS2000_REL_A */
  /*
   * Log release A structure
   * Only attempt to log if logging is turned on for this code.
   */
/* FC: Tools not ready
#ifdef FEATURE_IS2000_REL_C
  if (log_status(LOG_SRV_CONFIG_VER_4_C))
#elif defined(FEATURE_IS2000_REL_B)
*/
#ifdef FEATURE_IS2000_REL_B
  if (log_status(LOG_SRV_CONFIG_VER_3_C))
#else
  if (log_status(LOG_SRV_CONFIG_VER_2_C))
#endif /* FEATURE_IS2000_REL_B */
  {
    snm_submit_service_configuration_log();
  }
#endif /* !FEATURE_IS2000_REL_A */

} /* snm_continue_update_config */

#ifdef FEATURE_IS2000_REL_A_SVD
/*===========================================================================

FUNCTION snm_eom_init

DESCRIPTION
  init snm's negotiation_info database with the eom SO and sr_id

DEPENDENCIES
  none.

RETURN VALUE
  TRUE if initialized, FALSE otherwise

SIDE EFFECTS
  none
===========================================================================*/

boolean snm_eom_init
(
  word so,
  byte sr_id
)
{
  word so_index;
  byte con_index;

  /* Get so index */
  if (snm_find_so( so, &so_index ) == SNM_FAILURE)
  {
    /* This should never happen as the SO has already been verified before */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Can't find SO %d",
      so);
    return FALSE;
  }

  /* Get index in to neg database */
  if (!snm_get_free_neg_index(&con_index))
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Failed to get free entry in SNM negotiation_info database");
    return FALSE;
  }

  /* SO */
  negotiation_info.con_list[con_index].neg_seed     =
  negotiation_info.con_list[con_index].beginning_so =
  negotiation_info.con_list[con_index].prev_so      = so;
  negotiation_info.con_list[con_index].connection_sod = 0;

  /* alt so list */
  negotiation_info.con_list[con_index].next_so_index = 0;
  negotiation_info.con_list[con_index].alt_so_list =
    snp.so[so_index].negotiation_pars.alternate_so;

  /* sr_id */
  negotiation_info.con_list[con_index].sr_id_valid = TRUE;
  negotiation_info.con_list[con_index].sr_id       = sr_id;

  /* rc and mux */
  negotiation_info.con_list[con_index].cur_fwd_fch_rc_index = 0;

  negotiation_info.con_list[con_index].cur_fwd_dcch_rc_index = 0;

  negotiation_info.con_list[con_index].fundicated_rc_proposed_chan_mask = SNM_INIT_CHAN_MASK;
  negotiation_info.con_list[con_index].cur_fwd_sch_mux_index =
  negotiation_info.con_list[con_index].cur_rev_sch_mux_index = 0;
  negotiation_info.con_list[con_index].max_mux_proposed = FALSE;
  negotiation_info.con_list[con_index].so_can_be_changed = FALSE;

  return TRUE;

} /* snm_eom_init */

/*===========================================================================

FUNCTION snm_is_so_rc_compatible_with_mux

DESCRIPTION
  see if given so is RC compatible with given (existing configuration) mux values.
  This function is a helper to snm_is_eom_so_compatible()

DEPENDENCIES
  none

RETURN VALUE
  TRUE, if compatible, FALSE otherwise.

SIDE EFFECTS
  none
===========================================================================*/

static boolean snm_is_so_rc_compatible_with_mux
(
  word so_to_compare,
  word given_fwd_mux,
  word given_rev_mux
)
{
  word i, so_index_to_compare;
  boolean found_fwd_rc_match = FALSE, found_rev_rc_match = FALSE;

  if (!snm_so_is_enabled(so_to_compare))
  {
    M1X_MSG( DCP, LEGACY_MED,
      "SO %d not enabled",
      so_to_compare);
    return FALSE;
  }

  /* For P_REV < 8, rate set of fundicated channels shall be the same */
#ifdef FEATURE_IS2000_REL_B
  if (!P_REV_IS_8_OR_GREATER)
#endif /* !FEATURE_IS2000_REL_B */
  {
    if(snm_find_so(so_to_compare, &so_index_to_compare) == SNM_FAILURE)
    {
      return FALSE;
    }
    /* can't fail as we've verified it above */

    /* Fwd RC */
    for (i=0; i<snp.so[so_index_to_compare].negotiation_pars.fwd_rc.num_of_rc; i++)
    {
      if (RATE_SET_OF_FWD_RC(snp.so[so_index_to_compare].negotiation_pars.fwd_rc.list[i]) ==
          MUX_TO_RATE_SET(given_fwd_mux))
      {
        /* Found a supported fwd RC that matches given mux */
        found_fwd_rc_match = TRUE;
        break;
      }
    }

    /* Rev RC */
    for (i=0; i<snp.so[so_index_to_compare].negotiation_pars.rev_rc.num_of_rc; i++)
    {
      if (RATE_SET_OF_REV_RC(snp.so[so_index_to_compare].negotiation_pars.rev_rc.list[i]) ==
          MUX_TO_RATE_SET(given_rev_mux))
      {
        /* Found a supported rev RC that matches given mux */
        found_rev_rc_match = TRUE;
        break;
      }
    }

    if (!found_fwd_rc_match || !found_rev_rc_match)
    {
      /* Rate sets don't match */
      M1X_MSG( DCP, LEGACY_MED,
        "SO %d not rate set-compatible with cur cfg",
        so_to_compare);
      return FALSE;
    }
  }

  return TRUE;

} /* snm_is_so_rc_compatible_with_mux */

/*===========================================================================

FUNCTION snm_is_so_traffic_type_compatible_with_cfg

DESCRIPTION
  see if given so is traffic type compatible with given configuration.
  This function is a helper to snm_is_eom_so_compatible()

DEPENDENCIES
  none

RETURN VALUE
  TRUE, if compatible, FALSE otherwise.

SIDE EFFECTS
  none
===========================================================================*/

static boolean snm_is_so_traffic_type_compatible_with_cfg
(
  word               so_to_compare,
  caii_srv_cfg_type *cfg_ptr
)
{
  word cfg_so_index, so_index_to_compare, i;
  byte traffic_type_present = 0;

  if (!snm_so_is_enabled(so_to_compare))
  {
    M1X_MSG( DCP, LEGACY_MED,
      "SO not enabled %d",
      so_to_compare);
    return FALSE;
  }

  if(snm_find_so(so_to_compare, &so_index_to_compare) == SNM_FAILURE)
  {
    return FALSE;
  }
  /* can't fail as we've verified it above */

  /* Go through SOs in current config and see if their traffic types are
     compatible with the EOM SO. */
  for (i = 0; i < cfg_ptr->num_con; i++)
  {
    if (snm_find_so(cfg_ptr->con[i].so, &cfg_so_index) != SNM_SUCCESS)
    {
      /* This must never happen! */
      M1X_MSG( DCP, LEGACY_MED,
        "SO of cur cfg not found! %d",
        cfg_ptr->con[i].so);
      return FALSE;
    }

    if (mccccim_con_is_in_rel_substate(cfg_ptr->con[i].con_ref))
    {
      /* This connection is not even an active CC instance, so
      don't worry about this */
      continue;
    }

    if (CAI_IS_SO_SIGNALING(cfg_ptr->con[i].so))
    {
      /* This connection is signaling SO, so no need to worry about
      traffic type evaluation */
      continue;
    }

    /* Forward traffic type */
    if (((snp.so[cfg_so_index].default_config.forward.allowed_traffic_type &
          snp.so[so_index_to_compare].default_config.forward.allowed_traffic_type) != 0)  /*lint !e641 */
        /* There is an overlap of traffic type */
        &&
        ((snp.so[cfg_so_index].default_config.forward.allowed_traffic_type |
          snp.so[so_index_to_compare].default_config.forward.allowed_traffic_type) != CAI_PRI_OR_SEC_TRAF) /*lint !e641 */
        /* Invalid traffic type */
       )
    {                                                                        // support both types
      /* This means we can't propose the SO. */
      M1X_MSG( DCP, LEGACY_MED,
        "SO %d traffic type not compatible with cur cfg SO %d",
        snp.so[so_index_to_compare].service_option_num,
        cfg_ptr->con[i].so);
      return FALSE;
    }

    /* Keep track of all "taken" traffic types, only care primary and secondary traffic types */
    if (cfg_ptr->con[i].fwd_traf & CAI_PRI_OR_SEC_TRAF)
    {
      traffic_type_present |= cfg_ptr->con[i].fwd_traf;
    }

    /* Reverse traffic type */
    if (((snp.so[cfg_so_index].default_config.reverse.allowed_traffic_type &
          snp.so[so_index_to_compare].default_config.reverse.allowed_traffic_type) != 0)  /*lint !e641 */
        /* There is an overlap of traffic type */
        &&
        ((snp.so[cfg_so_index].default_config.reverse.allowed_traffic_type |
         snp.so[so_index_to_compare].default_config.reverse.allowed_traffic_type) != CAI_PRI_OR_SEC_TRAF) /*lint !e641 */
        /* Invalid traffic type */
       )
    {                                                                        // support both types
      /* This means we can't propose the SO. */
      M1X_MSG( DCP, LEGACY_MED,
        "SO %d traffic type not compatible with cur cfg SO %d",
        snp.so[so_index_to_compare].service_option_num,
        cfg_ptr->con[i].so);
      return FALSE;
    }
  } /* end for */

  if (cfg_ptr->num_con == 0)
  {
    /* This probably means we have not rxed the first SCM, so validate the
       new SO against the cdma.so_req for traffic type, that is the best we
       can do */

    if (CAI_IS_SO_SIGNALING(cdma.so_req))
    {
      /* This SO to be connected is signaling SO, so no need to worry about
      traffic type evaluation */
      return TRUE;
    }

    if (snm_find_so(cdma.so_req, &cfg_so_index) != SNM_SUCCESS)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "SO of cdma.so_req not found! %d",
        cdma.so_req);
      return TRUE;
    }

    /* Forward traffic type */
    if (((snp.so[cfg_so_index].default_config.forward.allowed_traffic_type &
          snp.so[so_index_to_compare].default_config.forward.allowed_traffic_type) != 0)  /*lint !e641 */
        /* There is an overlap of traffic type */
        &&
        ((snp.so[cfg_so_index].default_config.forward.allowed_traffic_type |
          snp.so[so_index_to_compare].default_config.forward.allowed_traffic_type) != CAI_PRI_OR_SEC_TRAF) /*lint !e641 */
        /* Invalid traffic type */
       )
    {                                                                        // support both types
      // This means we can't propose the SO.
      M1X_MSG( DCP, LEGACY_MED,
        "SO %d traffic type not compatible with cur cfg SO %d",
        snp.so[so_index_to_compare].service_option_num,
        cfg_ptr->con[i].so);
      return FALSE;
    }

    /* Reverse traffic type */
    if (((snp.so[cfg_so_index].default_config.reverse.allowed_traffic_type &
          snp.so[so_index_to_compare].default_config.reverse.allowed_traffic_type) != 0)  /*lint !e641 */
        /* There is an overlap of traffic type */
        &&
        ((snp.so[cfg_so_index].default_config.reverse.allowed_traffic_type |
          snp.so[so_index_to_compare].default_config.reverse.allowed_traffic_type) != CAI_PRI_OR_SEC_TRAF) /*lint !e641 */
        /* Invalid traffic type */
       )
    {
      /* This means we can't propose the SO. */
      M1X_MSG( DCP, LEGACY_MED,
        "SO %d traffic type not compatible with cur cfg SO %d",
        snp.so[so_index_to_compare].service_option_num,
        cdma.so_req);
      return FALSE;
    }

  }

  if (traffic_type_present == CAI_PRI_OR_SEC_TRAF)
  {
    /* Okay Both pri and sec traffic types have been taken by
       existing active calls and we already know this is not a signaling call,
       so return FALSE */
    M1X_MSG( DCP, LEGACY_MED,
      "Have Pri & Sec, can't originate SO %d",
      so_to_compare);
    return FALSE;
  }

  return TRUE;

} /* snm_is_so_traffic_type_compatible_with_cfg */

/*===========================================================================

FUNCTION snm_is_eom_so_compatible_cur_cfg

DESCRIPTION
  see if so is compatible with existing configuration when sending an EOM.
  Right now, we check to see if the existing rate set is compatible
  with what the SO can support.
  Also, we check if the pri/sec traffic types are compatible.
  If it is not, this function will try and find an alternate that is
  compatible and set it in 'so'

DEPENDENCIES
  none.

RETURN VALUE
  TRUE, if compatible or if alternate SO being used, FALSE otherwise.

SIDE EFFECTS
  The so can be potentially changed by this function to an alternate
  that is compatible with the current configuration.
===========================================================================*/

boolean snm_is_eom_so_compatible_cur_cfg
(
  word *so
)
{
  caii_srv_cfg_type *cur_cfg = &(negotiation_info.current_config->neg_scr);
  word so_index, i;
  word alt_so;

  if (!snm_so_is_enabled(*so))
  {
    M1X_MSG( DCP, LEGACY_MED,
      "EOM SO not enabled %d",
      *so);
    return FALSE;
  }

  if (snm_is_so_rc_compatible_with_mux(*so, cur_cfg->fwd_mux, cur_cfg->rev_mux)
      && (CAI_IS_SO_SIGNALING(*so) ||
          snm_is_so_traffic_type_compatible_with_cfg(*so, cur_cfg)))
  {
    return TRUE;
  }

  /* If we are here, then SO was not compatible, we'll try its alternates */
  if(snm_find_so(*so, &so_index) == SNM_FAILURE)
  {
    return FALSE;
  }
  /* can't fail as we've verified it above */
  M1X_MSG( DCP, LEGACY_MED,
    "Trying alternates for SO %d",
    *so);

  for (i = 0;
       (i < cai_ac_alt_so_max) && (snp.so[so_index].negotiation_pars.alternate_so[i] != CAI_SO_NULL);
       i++)
  {
    alt_so = snp.so[so_index].negotiation_pars.alternate_so[i];

    if (snm_is_so_rc_compatible_with_mux(alt_so, cur_cfg->fwd_mux, cur_cfg->rev_mux)
        && (CAI_IS_SO_SIGNALING(alt_so) ||
            snm_is_so_traffic_type_compatible_with_cfg(alt_so, cur_cfg)))
    {
      /* We have found an alternate, return with it */
      *so = alt_so;
      M1X_MSG( DCP, LEGACY_MED,
        "Found alternate SO %d for Origination",
        *so);
      return TRUE;
    }
  } /* end for */

  M1X_MSG( DCP, LEGACY_MED,
    "Failed to get alternate for SO %d",
    *so);

  return FALSE;

} /* snm_is_eom_so_compatible_cur_cfg */
#endif /* FEATURE_IS2000_REL_A_SVD */

/*===========================================================================

FUNCTION snm_update_for_deleted_connection

DESCRIPTION
  Clean up negotiation_info.con_list[] for the entry corresponding to
  "deleted_sr_id". This function should be called when a call control
  instance is deleted or when an EOM fails.

DEPENDENCIES
  none.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void snm_update_for_deleted_connection
(
  byte deleted_sr_id
)
{
  byte con_index = 0;
  boolean use_sr_id;

  /* We need to see if we can use the sr_id in negotiation functions */
  use_sr_id = (P_REV_IS_6_OR_GREATER) ? TRUE : FALSE;

  if (!snm_get_neg_index_for_sr_id(use_sr_id, deleted_sr_id, &con_index))
  {
    /* This should not happen */
    M1X_MSG( DCP, LEGACY_MED,
      "Entry for deleted connection not found in snm");
    if (use_sr_id)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "sr_id for missing entry is %d",
        deleted_sr_id);
    }

    return;
  }

  negotiation_info.con_list[con_index].max_mux_proposed = FALSE;
  negotiation_info.con_list[con_index].neg_seed     =
  negotiation_info.con_list[con_index].beginning_so = CAI_SO_NULL;
  /* Zero out pointer to service option descriptor */
  negotiation_info.con_list[con_index].connection_sod = 0;
  /* Reset con_ref */
  negotiation_info.con_list[con_index].sr_id_valid = FALSE;

  negotiation_info.con_list[con_index].prev_so = CAI_SO_NULL;
  negotiation_info.con_list[con_index].cur_fwd_fch_rc_index = 0;
  negotiation_info.con_list[con_index].cur_fwd_dcch_rc_index = 0;
  negotiation_info.con_list[con_index].fundicated_rc_proposed_chan_mask = SNM_INIT_CHAN_MASK;
  negotiation_info.con_list[con_index].cur_fwd_sch_mux_index =
  negotiation_info.con_list[con_index].cur_rev_sch_mux_index = 0;
#ifdef FEATURE_IS2000_REL_A_SVD
  negotiation_info.con_list[con_index].so_can_be_changed = FALSE;
#endif /* FEATURE_IS2000_REL_A_SVD */

} /* snm_update_for_deleted_connection */

/*===========================================================================

FUNCTION snm_update_with_new_config

DESCRIPTION
  Clean up negotiation_info.con_list[] for entries not in the current cfg;
  This is called by CCIM upon receiving a SCM or U/GHDM.

DEPENDENCIES
  An assumption being made here is that sr_id only has values ranging
  from 0 to 6 (MAX_SR_ID_VALUE). This is currently being enforced in SNM.
  If MAX_SR_ID_VALUE is changed in future, changes need to be made to this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void snm_update_with_new_config
(
  caii_srv_cfg_type *new_cfg_ptr
)
{
  byte sr_id_mask = 0;
  byte i;

  /* If PREV is less than SIX, we only use the first entry in negotiation_info,
     so nothing to do, just return */
  if (!P_REV_IS_6_OR_GREATER)
  {
    return;
  }

  /* Collect all SR_ID values assigned in the current configuration */
  for (i=0; (i < new_cfg_ptr->num_con) && (i < CAII_SRV_CON_MAX); i++)
  {
    sr_id_mask |= ( ((byte)1) << new_cfg_ptr->con[i].sr_id); /*lint !e734 */
  }

  for (i=0; i< MAX_SNM_CON; i++)
  {
    if (negotiation_info.con_list[i].beginning_so == CAI_SO_NULL)
    {
      continue;
    }

    /* This is a valid entry */
    if (negotiation_info.con_list[i].sr_id_valid &&
        !(sr_id_mask & (((byte)1) << negotiation_info.con_list[i].sr_id)))
    {
      /* The sr_id in negotiation_info is not present in the current
         configuration */
      snm_update_for_deleted_connection(negotiation_info.con_list[i].sr_id);
    }
  }

} /* snm_update_with_new_config */

/*===========================================================================

FUNCTION snm_update_fpc_pri_chan

DESCRIPTION
  Updates the value of FPC_PRI_CHAN in non-negotiable SCR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void snm_update_fpc_pri_chan
(
  uint8 fpc_pri_chan /* New value of FPC_PRI_CHAN */
)
{
  /* Make sure the FPC_MODE are in sync with FPC_PRI_CHAN */
  if (fpc_pri_chan == CAI_FPC_PRI_CHAN_CPCCH) /*lint !e641 */
  {
    negotiation_info.current_config->non_neg_scr.fpc_mode = CAI_FPC_MODE_RESERVED; /*lint !e641 */
  }
  else if (negotiation_info.current_config->non_neg_scr.fpc_mode == CAI_FPC_MODE_RESERVED) /*lint !e641 */
  {
    negotiation_info.current_config->non_neg_scr.fpc_mode = CAI_FPC_MODE_800_PRI; /*lint !e641 */
  }

  negotiation_info.current_config->non_neg_scr.fpc_pri_chan = fpc_pri_chan;

} /* snm_update_fpc_pri_chan */

/*===========================================================================

FUNCTION snm_update_pilot_gating_rate

DESCRIPTION
  Updates the value of pilot_GATING_RATE in non-negotiable SCR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void snm_update_pilot_gating_rate
(
  cai_pilot_gating_rate_type gating_rate /* The pilot gating rate to be set to */
)
{
  if (gating_rate == CAI_GATING_RATE_NONE)
  {
    negotiation_info.current_config->non_neg_scr.gating_rate_incl = 0;
  }
  else if (gating_rate <= CAI_GATING_RATE_QUARTER)
  {
    negotiation_info.current_config->non_neg_scr.gating_rate_incl = 1;
    negotiation_info.current_config->non_neg_scr.pilot_gate_rate = gating_rate; //lint !e734 !e641
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid gating rate %d",
      gating_rate );
  }

} /* snm_update_pilot_gating_rate */

/*===========================================================================

FUNCTION snm_get_current_config

DESCRIPTION
  This function returns the current negotiable part of service configuration.

DEPENDENCIES
  None.

RETURN VALUE
  SNM_SUCCESS if valid configuration has been negotiated.  SNM_FAILURE otherwise.
  If successful, pointer to current config is returned in function parameter.

SIDE EFFECTS
  None
===========================================================================*/

const caii_srv_cfg_type* snm_get_current_config
(
  void
)
{
  return &negotiation_info.current_config->neg_scr;

} /* snm_get_current_config */

/*===========================================================================

FUNCTION snm_get_current_nnscr

DESCRIPTION
  This function returns the current non-negotiable part of service configuration.

DEPENDENCIES
  None.

RETURN VALUE
  SNM_SUCCESS if valid configuration has been negotiated.  SNM_FAILURE otherwise.
  If successful, pointer to current config is returned in function parameter.

SIDE EFFECTS
  None
===========================================================================*/

const caii_non_neg_srv_cfg_type*  snm_get_current_nnscr
(
  void
)
{
  /* For IS2000 the config is always initialized to good defaults in srv_init
     So no need to check for config being valid */

  return &negotiation_info.current_config->non_neg_scr;

} /* snm_get_current_nnscr */

/*===========================================================================

FUNCTION snm_get_complete_pending_config

DESCRIPTION
  This function returns a pointer to the complete pending service configuration.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

const caii_complete_srv_cfg_type* snm_get_complete_pending_config
(
   void
)
{
  return (const caii_complete_srv_cfg_type*)negotiation_info.pending_config;

} /* snm_get_complete_pending_config */

/*===========================================================================

FUNCTION snm_gen_son_config

DESCRIPTION
  Generates the configuration established with Service Option Negotiation.

DEPENDENCIES
  None

RETURN VALUE
  SNM_SUCCESS if successfully initialized; SNM_FAILURE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

snm_status_type snm_gen_son_config
(
  word  so
)
{
  byte con_index;

  /* Since SO Negotiation can occur not just at the beginning of a call, but
     also after a HO, we must determine the negotiation_info.con index
     and use it in the call to snm_gen_config(). */
  if (!snm_get_neg_index_for_sr_id(FALSE, 0, &con_index))
  {
    return SNM_FAILURE;
  }

  /* Generate default configuration... */
  if (snm_gen_config(so, &(negotiation_info.pending_config->neg_scr), TRUE,
                     SNM_DEFAULT_MUX, con_index,
                     mccccl_code_channel_list.get_fwd_chan_mask(),
                     mccccl_code_channel_list.get_rev_chan_mask()) != SNM_SUCCESS)
  {
    return SNM_FAILURE;
  }

  return SNM_SUCCESS;

} /* snm_gen_son_config */

/*===========================================================================

FUNCTION snm_set_cont_msg_handler

DESCRIPTION
  This function sets the Service Option Control Message handler function
  pointer.

DEPENDENCIES
  snp.total_so must be less than MAX_WORD value.

RETURN VALUE
  SNM_SUCCESS if serivce option exists,
  SNM_FAILURE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

snm_status_type snm_set_cont_msg_handler
(
  word so,
  snm_socm_return_type (*handler_ptr)(word length, byte* field, word serv_opt)
)
{
  word so_index;
  if (snm_find_so( so, &so_index ) == SNM_FAILURE )
  {
    return SNM_FAILURE;
  }

  snp.so[so_index].socm_handler = handler_ptr;

  return SNM_SUCCESS;

} /* snm_set_cont_msg_handler */

/*===========================================================================

FUNCTION snm_process_srv_opt_ctl_msg

DESCRIPTION
  This function returns the Service Option Control Message handler.

DEPENDENCIES
  None.

RETURN VALUE
  Return value from the call to the socm_handler, VALID if no handler
  exists, UNSPEC if the SO does not exist.

SIDE EFFECTS
  None
===========================================================================*/

snm_socm_return_type snm_process_srv_opt_ctl_msg
(
  caii_rx_msg_type* msg_ptr
)
{
  word so = negotiation_info.current_config->neg_scr.con[0].so;
  word so_index;

  if ( snm_find_so(so, &so_index) == SNM_FAILURE )
  {
    return SNM_UNSPEC;
  }

  if (snp.so[so_index].socm_handler == NULL)
  {
    return SNM_VALID;
  }

  return ((*snp.so[so_index].socm_handler)
     (msg_ptr->srv_ctl.tsf_len, msg_ptr->srv_ctl.tsf, so));

} /* snm_get_cont_msg_handler */

#ifdef FEATURE_IS2000_REL_B
/*===========================================================================

FUNCTION snm_validate_for_sch_cfg

DESCRIPTION
  Performs validation of For-SCH config received in ESCAM

DEPENDENCIES
  None

RETURN VALUE
  SNM_SUCCESS if successfully validated; SNM_FAILURE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
snm_status_type snm_validate_for_sch_cfg
(
  int num_for_sch,
  caii_for_sch_type *for_sch_cfg
)
{
  boolean bad_cfg;
  int i,j ;
  caii_for_sch_spec_fields_type for_sch_fields;
  word so_index = 0;
  const caii_srv_cfg_type * config = snm_get_current_config();

  (void)mccap_get_ch_specific_fields(MCCAP_FWD_SCH, &for_sch_fields);

  for (i=0 ; i < config->num_con ; i++)
  {
    (void) snm_find_so(config->con[i].so, &so_index);
    if (snp.so[so_index].negotiation_pars.fwd_sch_mo_list.num_mos > 0)
    {
      break;
    }
  }

  if (num_for_sch > for_sch_fields.for_sch_num)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "num_for_sch %d is incorrect ",
      num_for_sch);
    return SNM_FAILURE;
  }

  for (i=0; i < num_for_sch ; i++)
  {
    if (for_sch_cfg[i].for_sch_id >= num_for_sch)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "%d for SCH Id out of range",
        for_sch_cfg[i].for_sch_id);
    }

    bad_cfg = TRUE;

    for (j=0 ; j < snp.so[so_index].negotiation_pars.fwd_rc.num_of_rc; j++)
    {
      if (for_sch_cfg[i].sch_chn_cfg.sch_rc ==
          snp.so[so_index].negotiation_pars.fwd_rc.list[j]) /*lint !e641 */
      {
        bad_cfg = FALSE;
        break;
      }
    }

    if (bad_cfg)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Bad RC %d",
        for_sch_cfg[i].sch_chn_cfg.sch_rc);
      return SNM_FAILURE;
    }

    /* Now check mux option */
    bad_cfg = TRUE;
    for (j=0; j< snp.so[so_index].negotiation_pars.fwd_sch_mo_list.num_mos ; j++) //revisit
    {
      if (snp.so[so_index].negotiation_pars.fwd_sch_mo_list.list[j] ==
          for_sch_cfg[i].for_sch_mux)
      {
        bad_cfg = FALSE;
        break;
      }
    }

    if (((for_sch_cfg[i].for_sch_mux <= 0x10) ||
         (for_sch_cfg[i].for_sch_mux & CAI_MO_RATE_SET_MASK))
        &&
        (RATE_SET_OF_FWD_RC(for_sch_cfg[i].sch_chn_cfg.sch_rc) !=
         (MUX_TO_RATE_SET(for_sch_cfg[i].for_sch_mux) & 1)))
    {
      bad_cfg = TRUE;
    }

    if (bad_cfg)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Bad mux 0x%x",
        for_sch_cfg[i].for_sch_mux);
      return SNM_FAILURE;
    }

    if (for_sch_cfg[i].for_sch_mux == 0xf20)
    {
      if (for_sch_cfg[i].sch_chn_cfg.max_rate >
          for_sch_fields.for_sch_recs[i].for_max_rate)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Bad max rate %d",
          for_sch_cfg[i].sch_chn_cfg.max_rate);
        return SNM_FAILURE;
      }
    }
    else
    {
      if (snm_get_max_sch_rate_from_mux(for_sch_cfg[i].for_sch_mux) !=
          for_sch_cfg[i].sch_chn_cfg.max_rate)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Bad max rate %d",
          for_sch_cfg[i].sch_chn_cfg.max_rate);
        return SNM_FAILURE;
      }
    }

    if ((for_sch_cfg[i].sch_chn_cfg.max_rate >= CAI_SCH_RATE_RS1_32X_RS2_18X) && /*lint !e641 */
        (for_sch_cfg[i].sch_chn_cfg.sch_rc == CAI_RC_3) /*lint !e641 */
       )
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Bad max rate RC combo");
      return SNM_FAILURE;
    }

    if (for_sch_cfg[i].sch_chn_cfg.frame_40_used &&
        !for_sch_fields.for_sch_recs[i].for_frame_40_supported)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "For SCH frame 40 mismatch");
      return SNM_FAILURE;
    }

    if (for_sch_cfg[i].sch_chn_cfg.frame_80_used &&
        !for_sch_fields.for_sch_recs[i].for_frame_80_supported)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "For SCH frame 80 mismatch");
      return SNM_FAILURE;
    }

    /* Now validate coding type */
    switch (for_sch_cfg[i].sch_chn_cfg.coding)
    {
      case CAI_CONVOLUTIONAL_CODING:
        if (RATE_SET_OF_FWD_RC(for_sch_cfg[i].sch_chn_cfg.sch_rc)
            == 0)
        {
          /* This is a Rate set 1 based RC, compare with RS1 table for max rate */
          if (snm_rs1_rate_table[for_sch_cfg[i].sch_chn_cfg.max_rate] >
              snm_conv_turbo_rate_table[for_sch_fields.for_sch_recs[0].for_max_conv_block_size])
          {
            /* Cannot do this rate with conv then it must be possible with turbo
               since we already verified max rate */
            M1X_MSG( DCP, LEGACY_ERROR,
              "Conv is too small");
            bad_cfg = TRUE;
          }
        }
        else
        {
          /* This is a Rate set 2 based RC, compare with RS2 table for max rate */
          if (snm_rs2_rate_table[for_sch_cfg[i].sch_chn_cfg.max_rate] >
              snm_conv_turbo_rate_table[for_sch_fields.for_sch_recs[0].for_max_conv_block_size])
          {
            /* Cannot do this rate with conv then it must be possible with turbo
            since we already verified max rate */
            M1X_MSG( DCP, LEGACY_ERROR,
              "Conv is too small");
            bad_cfg = TRUE;
          }
        }
        break;

      case CAI_TURBO_CODING:
        if (RATE_SET_OF_FWD_RC(for_sch_cfg[i].sch_chn_cfg.sch_rc) == 0)
        {
          /* This is a Rate set 1 based RC, compare with RS1 table for max rate */
          if (snm_rs1_rate_table[for_sch_cfg[i].sch_chn_cfg.max_rate] >
              snm_conv_turbo_rate_table[for_sch_fields.for_sch_recs[0].for_max_turbo_block_size])
          {
            /* Cannot do this rate with conv then it must be possible with turbo
               since we already verified max rate */
            M1X_MSG( DCP, LEGACY_ERROR,
              "Turbo is too small");
            bad_cfg = TRUE;
          }
        }
        else
        {
          /* This is a Rate set 2 based RC, compare with RS2 table for max rate */
          if (snm_rs2_rate_table[for_sch_cfg[i].sch_chn_cfg.max_rate] >
              snm_conv_turbo_rate_table[for_sch_fields.for_sch_recs[0].for_max_turbo_block_size])
          {
            /* Cannot do this rate with conv then it must be possible with turbo
            since we already verified max rate */
            M1X_MSG( DCP, LEGACY_ERROR,
              "Turbo is too small");
            bad_cfg = TRUE;
          }
        }
        break;

      default:
        bad_cfg = TRUE;
        break;
    }

    if (bad_cfg)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Bad Coding type %d",
        for_sch_cfg[i].sch_chn_cfg.coding);
      return SNM_FAILURE;
    }

  }

  return SNM_SUCCESS;

} /* snm_validate_for_sch_cfg */

/*===========================================================================

FUNCTION snm_validate_rev_sch_cfg

DESCRIPTION
  Performs validation of rev-SCH config received in ESCAM

DEPENDENCIES
  None

RETURN VALUE
  SNM_SUCCESS if successfully validated; SNM_FAILURE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

snm_status_type snm_validate_rev_sch_cfg
(
  int num_rev_sch,
  caii_rev_sch_type *rev_sch_cfg
)
{
  boolean bad_cfg;
  int i,j ;
  caii_rev_sch_spec_fields_type rev_sch_fields;
  word so_index = 0;
  const caii_srv_cfg_type * config = snm_get_current_config();

  (void)mccap_get_ch_specific_fields(MCCAP_REV_SCH, &rev_sch_fields);

  for (i=0 ; i < config->num_con ; i++)
  {
    (void) snm_find_so(config->con[i].so, &so_index);
    if (snp.so[so_index].negotiation_pars.rev_sch_mo_list.num_mos > 0)
    {
      break;
    }
  }

  if (num_rev_sch > rev_sch_fields.rev_sch_num)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "num_rev_sch %d is incorrect ",
      num_rev_sch);
    return SNM_FAILURE;
  }

  for (i=0; i < num_rev_sch ; i++)
  {
    if (rev_sch_cfg[i].rev_sch_id >= num_rev_sch)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "%d rev SCH Id out of range",
        rev_sch_cfg[i].rev_sch_id);
    }

    bad_cfg = TRUE;

    for (j=0 ; j < snp.so[so_index].negotiation_pars.rev_rc.num_of_rc; j++)
    {
      if (rev_sch_cfg[i].sch_chn_cfg.sch_rc ==
          snp.so[so_index].negotiation_pars.rev_rc.list[j]) /*lint !e641 */
      {
        bad_cfg = FALSE;
        break;
      }
    }

    if (bad_cfg)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Bad RC %d",
        rev_sch_cfg[i].sch_chn_cfg.sch_rc);
      return SNM_FAILURE;
    }

    /* Now check mux option */
    bad_cfg = TRUE;
    for (j=0; j< snp.so[so_index].negotiation_pars.rev_sch_mo_list.num_mos ; j++) //revisit
    {
      if (snp.so[so_index].negotiation_pars.rev_sch_mo_list.list[j] ==
          rev_sch_cfg[i].rev_sch_mux)
      {
        bad_cfg = FALSE;
        break;
      }
    }

    if (((rev_sch_cfg[i].rev_sch_mux <= 0x10) ||
         (rev_sch_cfg[i].rev_sch_mux & CAI_MO_RATE_SET_MASK))
        &&
        (RATE_SET_OF_REV_RC(rev_sch_cfg[i].sch_chn_cfg.sch_rc) !=
         (MUX_TO_RATE_SET(rev_sch_cfg[i].rev_sch_mux) & 1)))
    {
      bad_cfg = TRUE;
    }

    if (bad_cfg)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Bad rev SCH mux 0x%x",
        rev_sch_cfg[i].rev_sch_mux);
      return SNM_FAILURE;
    }

    if (rev_sch_cfg[i].rev_sch_mux == 0xf20)
    {
      if (rev_sch_cfg[i].sch_chn_cfg.max_rate >
          rev_sch_fields.rev_sch_recs[i].rev_max_rate)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Bad max rate %d",
          rev_sch_cfg[i].sch_chn_cfg.max_rate);
        return SNM_FAILURE;
      }
    }
    else
    {
      if (snm_get_max_sch_rate_from_mux(rev_sch_cfg[i].rev_sch_mux) !=
          rev_sch_cfg[i].sch_chn_cfg.max_rate)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Bad max rate %d",
          rev_sch_cfg[i].sch_chn_cfg.max_rate);
        return SNM_FAILURE;
      }
    }


    /* Max rate of 18X cannot be done in RC 4 */
    if ( (rev_sch_cfg[i].sch_chn_cfg.max_rate >=
                              CAI_SCH_RATE_RS1_32X_RS2_18X) && /*lint !e641 */
          (/* this macro returns 1 for RC4 Rate set 2 */
           RATE_SET_OF_REV_RC(rev_sch_cfg[i].sch_chn_cfg.sch_rc) == 1)
       )
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Error 18X with RC 4 !!, reject SCR");
      return SNM_FAILURE;
    }


    if (rev_sch_cfg[i].sch_chn_cfg.frame_40_used &&
        !rev_sch_fields.rev_sch_recs[i].rev_frame_40_supported)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Rev SCH frame 40 mismatch");
      return SNM_FAILURE;
    }
    if (rev_sch_cfg[i].sch_chn_cfg.frame_80_used &&
        !rev_sch_fields.rev_sch_recs[i].rev_frame_80_supported)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "rev SCH frame 80 mismatch");
      return SNM_FAILURE;
    }

    /* Now validate coding type */
    switch (rev_sch_cfg[i].sch_chn_cfg.coding)
    {
      case CAI_CONVOLUTIONAL_CODING:
        if (RATE_SET_OF_FWD_RC(rev_sch_cfg[i].sch_chn_cfg.sch_rc) == 0)
        {
          /* This is a Rate set 1 based RC, compare with RS1 table rev max rate */
          if (snm_rs1_rate_table[rev_sch_cfg[i].sch_chn_cfg.max_rate] >
              snm_conv_turbo_rate_table[rev_sch_fields.rev_sch_recs[0].rev_max_conv_block_size])
          {
            /* Cannot do this rate with conv then it must be possible with turbo
               since we already verified max rate */
            M1X_MSG( DCP, LEGACY_ERROR,
              "Conv is too small");
            bad_cfg = TRUE;
          }
        }
        else
        {
          /* This is a Rate set 2 based RC, compare with RS2 table rev max rate */
          if (snm_rs2_rate_table[rev_sch_cfg[i].sch_chn_cfg.max_rate] >
              snm_conv_turbo_rate_table[rev_sch_fields.rev_sch_recs[0].rev_max_conv_block_size])
          {
            /* Cannot do this rate with conv then it must be possible with turbo
               since we already verified max rate */
            M1X_MSG( DCP, LEGACY_ERROR,
              "Conv is too small");
            bad_cfg = TRUE;
          }
        }
        break;

      case CAI_TURBO_CODING:
        if (RATE_SET_OF_FWD_RC(rev_sch_cfg[i].sch_chn_cfg.sch_rc) == 0)
        {
          /* This is a Rate set 1 based RC, compare with RS1 table rev max rate */
          if (snm_rs1_rate_table[rev_sch_cfg[i].sch_chn_cfg.max_rate] >
              snm_conv_turbo_rate_table[rev_sch_fields.rev_sch_recs[0].rev_max_turbo_block_size])
          {
            /* Cannot do this rate with conv then it must be possible with turbo
               since we already verified max rate */
            M1X_MSG( DCP, LEGACY_ERROR,
              "Turbo is too small");
            bad_cfg = TRUE;
          }
        }
        else
        {
          /* This is a Rate set 2 based RC, compare with RS2 table rev max rate */
          if (snm_rs2_rate_table[rev_sch_cfg[i].sch_chn_cfg.max_rate] >
              snm_conv_turbo_rate_table[rev_sch_fields.rev_sch_recs[0].rev_max_turbo_block_size])
          {
            /* Cannot do this rate with conv then it must be possible with turbo
               since we already verified max rate */
            M1X_MSG( DCP, LEGACY_ERROR,
              "Turbo is too small");
            bad_cfg = TRUE;
          }
        }
        break;

      default:
        bad_cfg = TRUE;
        break;
    }

    if (bad_cfg)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Bad Coding type %d",
        rev_sch_cfg[i].sch_chn_cfg.coding);
      return SNM_FAILURE;
    }
  }

  return SNM_SUCCESS;

} /* snm_validate_rev_sch_cfg */

/*===========================================================================

FUNCTION snm_update_for_sch_cfg

DESCRIPTION
  Updates for-SCH config received in ESCAM

DEPENDENCIES
  validate_for_sch_cfg shoudl be called before this to validate the config

RETURN VALUE
  None

SIDE EFFECTS
  SCR is updated
===========================================================================*/

void snm_update_for_sch_cfg
(
  uint8 num_for_sch,
  caii_for_sch_type *for_sch_cfg
)
{
  int i;
  caii_srv_cfg_type *config = &negotiation_info.current_config->neg_scr;

  config->for_sch_cc_incl = TRUE;
  config->num_for_sch = num_for_sch;

  for (i=0 ; i < num_for_sch; i++)
  {
    config->for_sch[i] = for_sch_cfg[i];
  }

  /* SCH cfg update because of ESCAM */
  cdma_stats.channel_config_info.fsch0_mux = negotiation_info.current_config->neg_scr.for_sch[0].for_sch_mux;
  cdma_stats.channel_config_info.fsch0_rc = negotiation_info.current_config->neg_scr.for_sch[0].sch_chn_cfg.sch_rc;
  cdma_stats.changed_bitmask |= SYS_CDMA_CHANNEL_CONFIG_INFO_BIT_MASK;

} /* snm_update_for_sch_cfg */

/*===========================================================================

FUNCTION snm_update_rev_sch_cfg

DESCRIPTION
  Updates rev-SCH config received in ESCAM

DEPENDENCIES
  validate_rev_sch_cfg shoudl be called before this to validate the config

RETURN VALUE
  None

SIDE EFFECTS
  SCR is updated
===========================================================================*/

void snm_update_rev_sch_cfg
(
  uint8 num_rev_sch,
  caii_rev_sch_type *rev_sch_cfg
)
{
  int i;
  caii_srv_cfg_type * config = &negotiation_info.current_config->neg_scr;

  config->rev_sch_cc_incl = TRUE;
  config->num_rev_sch = num_rev_sch;

  for (i=0 ; (i < num_rev_sch && i < CAI_MAX_NUM_REV_SCH); i++)
  {
    config->rev_sch[i] = rev_sch_cfg[i];
  }

  /* SCH cfg update because of ESCAM */
  cdma_stats.channel_config_info.rsch0_mux = negotiation_info.current_config->neg_scr.rev_sch[0].rev_sch_mux;
  cdma_stats.channel_config_info.rsch0_rc = negotiation_info.current_config->neg_scr.rev_sch[0].sch_chn_cfg.sch_rc;
  cdma_stats.changed_bitmask |= SYS_CDMA_CHANNEL_CONFIG_INFO_BIT_MASK;

} /* snm_update_rev_sch_cfg */
#endif /*FEATURE_IS2000_REL_B */

#if defined(FEATURE_IS2000_REL_C)
/*===========================================================================

FUNCTION SNM_POPULATE_RESTORE_CONFIG

DESCRIPTION
  This function populates the service configuration to be restored to
  pending_cfg.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void snm_populate_restore_config
(
  void
)
{
  const restore_srv_cfg_info_type *restore_srv_cfg_info;

  restore_srv_cfg_info = mccsyncid_get_restore_config();

  /* Copy servie configuration to be restore to pending_config. */
  *negotiation_info.pending_config = restore_srv_cfg_info->cfg;

} /* snm_populate_restore_cfg */
#endif /* FEATURE_IS2000_REL_C */

/*===========================================================================

FUNCTION SNM_UPDATE_NN_SCR_FROM_PCNM

DESCRIPTION
  This function updates the Non neg SCR based on the info from the
  Power Control Message.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  Also sets the status field in the event report (e_ptr->status).
===========================================================================*/
void snm_update_nn_scr_from_pcnm
(
  const caii_pwr_ctrl_type* pcnm
)
{
  /* If no FPC info. included in PCNM, nnscr does not need to be updated */
  if (!pcnm->fpc_incl)
  {
    return;
  }

  negotiation_info.current_config->non_neg_scr.fpc_incl = TRUE;

  /* FPC_MODE and FPC_PRI_CHAN are always included */
  negotiation_info.current_config->non_neg_scr.fpc_mode = pcnm->fpc_mode;
  negotiation_info.current_config->non_neg_scr.fpc_pri_chan = pcnm->fpc_pri_chan;

  /* Now FCH OLPC parameters */
  if (pcnm->fpc_olpc_fch_incl)
  {
    negotiation_info.current_config->non_neg_scr.fpc_olpc_fch_incl = TRUE;
    negotiation_info.current_config->non_neg_scr.fpc_fch_max_setpt =
      pcnm->fpc_fch_max_setpt;
    negotiation_info.current_config->non_neg_scr.fpc_fch_min_setpt =
      pcnm->fpc_fch_min_setpt;
    negotiation_info.current_config->non_neg_scr.fpc_fch_fer =
      pcnm->fpc_fch_fer;
  }

  /* Then DCCH OLPC parameters */
  if (pcnm->fpc_olpc_dcch_incl)
  {
    negotiation_info.current_config->non_neg_scr.fpc_olpc_dcch_incl = TRUE;
    negotiation_info.current_config->non_neg_scr.fpc_dcch_max_setpt =
      pcnm->fpc_dcch_max_setpt;
    negotiation_info.current_config->non_neg_scr.fpc_dcch_min_setpt =
      pcnm->fpc_dcch_min_setpt;
    negotiation_info.current_config->non_neg_scr.fpc_dcch_fer =
      pcnm->fpc_dcch_fer;
  }

} /* snm_update_nn_scr_from_pcnm */

#ifdef FEATURE_IS2000_1X_ADV
/*===========================================================================

FUNCTION SNM_UPDATE_NN_SCR_FROM_RCP_INFO

DESCRIPTION
  This function updates the FPC_MODE in Non neg SCR based on the info from
  the Radio Configuration Parameters Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void snm_update_nn_scr_from_rcp_info
(
  void
)
{
  mccdma_rcp_data_type *rcp = &mcc_rcp_data;

  /* Ideally this will not happen. If no FPC info included in RCP info,
     NNSCR does not need to be updated */
  if (!rcp->rx_tx_params.fpc_mode.incl)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "FPC MODE not included in RCP info");
    return;
  }

  /* Update FPC_MODE */
  negotiation_info.current_config->non_neg_scr.fpc_mode =
              rcp->rx_tx_params.fpc_mode.value;

  M1X_MSG( DCP, LEGACY_MED,
    "NNSCR FPC_MODE updated with RCP fpc_mode %d",
    negotiation_info.current_config->non_neg_scr.fpc_mode);

}
#endif /* FEATURE_IS2000_1X_ADV */
/*lint +e818 +e655 */

