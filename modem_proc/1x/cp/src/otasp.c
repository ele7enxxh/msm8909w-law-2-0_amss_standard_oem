/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       O V E R   T H E   A I R   S E R V I C E   P R O G R A M M I N G

GENERAL DESCRIPTION
  This module contains routines to process OTASP messages received on
  the Traffic Channel in a Data Burst Message.

EXTERNALIZED FUNCTIONS

  otasp_init          - Initialize OTASP
  otasp_process_msg   - Processes an OTASP message
  otasp_report_status - Report status of OTASP call to UI

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 1996 - 2014 Qualcomm Technologies, Inc.
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/otasp.c_v   1.11   02 Oct 2002 22:52:34   louiel  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/otasp.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/20/14   jh      Fixed compiler warnings for hexagon 6.4.02 
07/17/14   agh     Fixed KW errors related to usage to memcpy
06/19/14   agh     Update CDMA global vars with committed values during the
                   same ongoing OTASP call in NV mode
05/06/14   gga     1X_CP F3 redistribution
04/16/14   ppr     Fix for memory leak issue while processing MMGSDI Report
03/25/14   agh     Added handling for unsupported capability record type in
                   protocol capabilty request message
03/25/14   ppr     Corrected buffer length validation while updating
                   MDN based NAIs
02/11/14   gga     Fixed KW errors
01/08/14   agh     FR 15489: UIM Clients to Handle SIM BUSY Indications
12/11/13   ppr     Changes to use  mmgsdi_session_read_prl()- API to read
                   NV#257 - NV_ROAMING_LIST_683_I in card mode
11/14/13   gga     Fixing the compilation error due to an extra semi-colon.
09/16/13   ppr     Defensive checks to validate the PRL size rcvd in OTASP
                   SSPR download req
07/29/13   eye     Switch to secure random number generation api.
07/12/13   ppr     Free the memory which allocated for deep copy of MMGSDI rpt
03/07/13   abd     Fix MSVC compilor errors
03/05/13   srk     Replace existing watchdog implementation with the new
                   Dog Heartbeat Mechanism.
01/28/13   gga     Klocwork Reviews Phase 2
01/03/13   nee     Replacement of mrmcpy() to memscpy() Phase 2
12/28/12   gga     Klocwork Reviews
12/06/12   trc     Fix secure stringl.h include
10/19/12   ppr     Added defensive checks
09/25/12   ppr     Added defencive checks for NULL ptr access
09/15/12   ppr     FR#2370 - 1x memory optimization changes
09/04/12   jj      Customer feature to allow OTASP only when BIP on LTE/HDR is
                   not in progress.
07/16/12   ssh     Moved the A-KEY PREV check in the beginning of the MS key
                   req message processing.
06/22/12   ppr     FR-2454 NV Item support for OTASP_MAX_UNLOCK_ATTEMPTS
06/15/12   ppr     Feature Cleanup
02/21/12   ppr     Feature Cleanup
02/04/12   ppr     Feature Cleanup
01/23/12   srk     Feature Cleanup
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
01/13/12   srk     Feature Cleanup: Mainlined Always ON features
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
09/09/11   ppr     Replaced AEE APIs with the standard C string APIs.
07/12/11   ab      KW Fixes
04/18/11   ag      Support for power down and power up registration after
                   OTASP commit.
02/09/11   ag      Mainlined FEATURE_EHRPD.
01/18/11   ag      Fix to not send commit status to CM if commit fails.
01/07/11   ag      Support to send NAM downloaded indication to CM when CDMA
                   Analog NAM block is downloaded.
01/04/11   ag      Fixed compiler and lint warnings.
12/24/10   ag      Changes to remove setting of preferred mode to AUTOMATIC
                   after PRL is downloaded.
12/15/10   ag      SU level CMI clean up.
11/15/10   jtm     Initial SU API split for CP.
10/21/10   azh     Fixed KW and compiler warnings.
10/18/10   jtm     Lint Fixes.
09/19/10   azh     Fixed Compiler warnings
09/15/10   azh     Fixed KlocWork warnings.
09/17/10   ag      Added support for dynamic DOG.
09/16/10   ag      Removed usage of FEATURE_OTASP_STATUS.
08/11/10   jj      CSIM change: Added check to access MMGSDI data only when
                   data len returned is nor zero.
07/16/10   jj      Added protocol field to the Commit Command's header
                   sent to UIM.
07/15/10   ssh     Fixed Compiler Warnings.
06/24/10   ssh     Added support for EFS sync.
04/23/10   ag      Support for 1x Modem statistical report.
04/15/10   ssh     Merged the GOBI specific changes under the FEATURE_GOBI.
03/23/10   ag      Support to update MDN based NAIs after MDN commit.
03/19/10   ag      Fix to check SSPR_P_REV while processing SSPR Configuration
                   Request Message with block_id = 0.
02/20/10   jj      Lint error fix.
02/15/10   jj      CSIM P2: Fixed a merging issue.
02/01/10   jj      CSIM P2 fix to send correct set of params for key_gen_rqst.
01/19/10   jj      Fixed a merge issue, which was causing a compile issue
                   with FEATURE_RUIM enabled and FEATURE_CSIM disabled.
01/12/10   jtm     Lint Fixes.
01/10/10   jj      CSIM P2 fix- fixed a crash because of unintialized ptr.
01/10/10   jj      CSIM p-2 changes.
11/02/09   jtm     Fixed compiler warnings.
10/21/09   ag      Added support for eHRPD IMSI OTASP operation.
09/14/09   ag      Fix to prevent a crash because of extra mcc_wait() call.
08/30/09   jj      Added include of uim_v.h as part of CMI.
08/21/09   jtm     Lint Fixes.
08/02/09   jtm     Lint fixes.
07/29/09   ag      Fix to receive UIM reports irrespective of command status
06/03/09   ag      Added support for displaying otasp session status.
                   Merged OTASP fixes for SPC validation failure.
05/14/09   ag      CMI Phase-II update.
05/11/09   jj      Fixed the issue of handling NAM blk download only for PCS
                   bands.
03/23/09   jtm     Featurize _v.h includes from subsystems outside of 1x.
03/20/09   jtm     Added include to cm_v.h and changed cmph_otasp_update_mode()
                   to cm_otasp_update_mode(). This function was moved and
                   renamed.
03/16/09   jtm     Removed cmph.h include. No longer needed by this file.
03/03/09   jtm     Added prl_v.h include.
10/16/08   ag      Added a feature FEATURE_MORE_OTASP_MAX_ULK_ATT for increased max unlock attempts.
                   Added powerdown support for incorrect SPC attempts for thin UI targets
06/30/08   pk      Mainlined feature FEATURE_EXTENDED_PRL
21/03/08   bb      Fixed klocwork errors
08/16/07   pg      Re-initialized nam value for each PRL download.
03/19/07   pg      Lint Cleanup.
03/08/07   an      Fixed Critical Lint Errors
10/12/06   rp      Added support for BC15
06/22/06   jqi     Eliminate a copy of PRL in CP/OTASP module to reduce the
                   memory by the size of MAX PRL size and use SD PRL for
                   uploading.
06/20/06   jqi     Removed the error fatal and responsed with the max allowed
                   features upon the capability request message.
06/14/06   an      Fixed RUIM Build Lint Errors
05/30/06   fc      Fixed lint errors.
05/01/06   an      Fixed Lint Errors
04/07/06   an      Changed START_STOP to otapa_req_ptr->start_stop
12/20/05   sb      Replaced cm_mc_rpt_free_q with cm_mc_rpt_get_buf_else_err_fatal().
10/19/05   sb      Added support for Band Class 14, Band Subclass I, J, K.
09/29/05   sb      Removed DB_VOICE_PRIVACY.
07/04/05   ht      Added OTASP MEID support.
03/23/05   fc      Merged the following from MSMSHARED:
03/22/05   an      Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated
                   FEATURE_CDMA_800 and FEATURE_CDMA_1900
11/30/04   jqi     Added FEATURE_IS683C_OTASP to complete OTASP support for
                   IS683C PRL.
10/22/04   wli     Deleted features around function call otasp_get_prl_stats().
03/05/04   bkm     Lint cleanup.
09/12/03   wli     Fixed SPC and SPASM failure counters in RUIM mode
08/11/03   bkm     Lint cleanup.
07/08/03   jqi     Added Extended PRL support
                   -Removed FEATURE_IS683C/A_PRL
                   -Updated SD API prl_validate() with SSPR P REV parameter.
                   -Clean up prl_fixed_roaming_list_type for standeard OTASP.
07/01/03   RC/aaj  Added change to accept block ID 0 as well 1 in dload rsp msg
06/20/03   wli     Merged 683C feature from PVCS mainline.
02/03/03   sb      Pass nam and pref_mode into cmph_otasp_update_mode.
10/02/02   yll     Linted the code.
09/30/02   sb      Mainlined FEATURE_SD20.
08/30/02   ts      Added support for Run Time R-UIM Enable
06/21/02   ph      changed cmmc.h to cmxll.h
11/01/01   RC      Fixed incorrect /8 of PRL size in otasp_get_prl_stats()
                   (SD 2.0 only).
10/23/01   kk      Merge from main archive.
                   10/02/01   jrp     Merged from MSM_CP.05.00.96
09/24/01   fc      Fixed compiler warning caused by UIM code.
                   08/20/01   ck      Fixed the bug where an invalid PRL was committed when it was
                                      downloaded after a valid PRL.
08/16/01   kk      Merge from common archive.
                   08/13/01   wli     Commented out cdma.spc if RUIM feature is defined.
06/26/01   lh      Merged following from the common archive.
                   05/25/01   ts      Converted the otasp calls to the UIM from auth calls to the
                   new UIM server calls.
06/14/01   va      Changed references to CDMA_CONV to CDMA_TRAFFIC.
02/20/01   fc      Cleanup debug messages.
02/16/01   fc      Added support for system determination 2.0. All changes
                   are featurized under FEATURE_SD20.
01/28/01   cah     Fixed problem with Pref Mode not resetting after PRL dl.
12/22/00   ck      Fixed the problem where Re-Authentication Response message
                   was not being sent when VP or message encryption was on
                   during key exchange.
12/01/00   ck      Moved FEATURE_AUTH to include Re-Auth response messages
11/29/00   ck      Added support for R-UIM implementation of IS-683A.
10/02/00   yll     Fixed the handling of otasp_otapa_failed_spc_val_attempts
                   and otasp_otapa_failed_spasm_val_attempts flags for OTAPA
                   session.
05/30/00   ck      Added a null function to remove compiler warning
03/19/00   ry      Updated process_otapa_req() to process an OTAPA Request
                   Message even if FEATURE_AUTH is undefined by removing the
                   check for !defined(FEATURE_AUTH); instead, made changes to
                   reject the message if nam_locked_state is TRUE
           ry      Fixed a compiler warning in process_otapa_req() if
                   FEATURE_AUTH is defined but FEATURE_OTASP_OTAPA is not
03/14/00   cah     Fixed compile problem in process_validation_req() if
                   OTAPA not defined.
03/14/00   ry      Added OTAPA support
03/10/00   kmp     Modified process_config_req() to always use the imsi_m
                   parameters instead of the operation imsi parameters
                   for the CDMA NAM Block and the CDMA/Analog NAM Block.
                   Modified otasp_reload_nam_params() to load the operational
                   imsi when the min based imsi is changed if that is the
                   operational imsi type. Did the same if the true imsi is the
                   current operational imsi type.
03/31/00   ry      Removed otasp_report_status()
03/06/00   ry      Merged the following from PP (MSM3100_CP.03.00.10):
           fc      Changed process_dload_req to clear old SID NID pairs from
                   otasp commit buffer.
           ram     Merged in the following change from the main trunk revision
                   of otasp.c into the trimode branch -
12/08/99   kmp     Changed otasp_reload_nam_params() to also update the min
                   based imsi parameters not just the True IMSI and operational
                   imsi parameters.
10/08/99   ck      Item NV_PRL_ENABLED_I is not written to NV for a Trimode
                   target as a RL is must.
                   The preferred mode is reset to AUTOMATIC by writing to NV
                   if the RL is downloaded in that session.
07/28/99   lh      CM and two-way SMS support added.
06/30/99   kmp     When committing acp data, place a #ifdef FEATURE_ACP around
                   this code in otasp_reload_nam_params().
06/17/99   ck      Updated the acp structure with the newly downloaded acp data
                   upon commit.
                   Added checks to ensure sid nid list does not exceed the
                   OTASPI_SID_NID_MAX when uploading NAM blocks.
05/10/99   kmp     Merged in IS-95B changes. Changed cdma.min1 and cdma.min2
                   to cdma.imsi_s1 and cdma.imsi_s2 respectively in
                   otasp_reload_nam_params(). Changed cdma.min1 and cdma.min2
                   in process_reauth_req() and otasp_process_reauth_report()
                   to function calls mcc_get_auth_imsi_s1() and
                   mcc_get_auth_imsi_s2() respectively. These changes
                   correspond to changes made to the cdma_ms_type data structure
                   in mccdma.h for IS-95B True IMSI support.
04/28/99   ck      Featurised the Key Exchange capability in FEATURE_DH_EXP in
                   the protocol capability response.
04/23/99   ck      Fixed the problem where the CDMA/Analog NAM Block was being
                   accepted in Non-std OTASP.
04/16/99   ck      Added capability to upload and download more than one
                   sid-nid pair. Changed T_AUTH to FEATURE_AUTH.
                   Initialized the roaming list download buffer to zeroes.
04/08/99   ck      Fixed the problem where the key response msg was sent in
                   place of key gen rsp msg when AUTH is not defined.
03/31/99   abh     Removed the use of imsi_s.
03/18/99   ck      Removed the feature and code under OTASP_800_HACK
03/18/99   ck      Merged in the changes from SPSW tip.
02/25/99   ck      Fixed the problem where the MS was sending the prev sent data
                   burst msg again when both NAM params and A-Key are committed
                   and Commit request is recd from the BS.
                   Fixed the problem where the RL upload failed when the size of
                   the RL was greater than the requested segment size.
02/23/99   ck      Changed the commit response to send the result code based
                   on whether Non Std OTASP is defined or not.
02/12/99   ck      Used mcc_roaming_list to upload the roaming list in the
                   SSPR Config Req msg and removed FEATURE_CLASS_1_IMSI.
01/20/99   ck      Added support for AKey exchange and Re-Authentication and
                   also support for IS-683A.
08/04/98   ych     Merged SSPR, E911 and Acquisition improvements.
07/20/98   pms     Added otasp_aho_handoff() and otasp_set_aho_handoff()
                   to handle OTASP call handoff to AMPS.
01/28/98   wjn     Call mcsys_get_prl_version() to set a new NV PRL field -
                   prl_version - after mcsys_validate_roaming_list() but
                   before writting the PRL structure to NV.
07/29/97   dna     Call NV function to get RL max size instead of constant.
04/18/97   dna     Made FEATURE_PRL_DOWNLOAD separate from FEATURE_PREFERRED_
                   ROAMING for OTASP_800_HACK.  Support Cellular Indicator
                   Data.  Support OTASP_800_HACK.  Support Dual Band OTASP.
02/11/97   fkm     (per dna) followed change of prl_fixed_roaming_list_type
                   as per prl.h
11/13/96   dna     Test and mark validity of roaming list before commit.
09/13/96   dna     Set roaming List Dimensions correctly.  Keep track and
                   report if something is committed.
08/26/96   dna     Fixed rl dload block retransmission, set dimensions.
08/08/96   dna     OTASP phase 2; subsidy lock and pr list download.
07/02/96   dna     Set IMSI_ADDR_NUM to zero for class 0 IMSI.
06/26/96   dna     Added special processing for download during commit.
06/25/96   dna     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "customer.h"

#include "modem_1x_defs.h"
#include "modem_1x_utils.h"
#include "memory.h"
#include "bit.h"
#include "comdef.h"
#include "err.h"
#include "ran.h"
#include "mc_i.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mccap_v.h"
#include "mccccim.h"
#include "mcctcsup.h"
#include "nv.h"
#include "otaspe.h"
#include "otaspi_i.h"
#include "otaspx.h"
#include "queue.h"
#include "qw.h"
#include "cmxll.h"
#include "cm.h"
#include "prl.h"
#include "cm_v.h"
#include "prl_v.h"
#include "crc.h"
#include "dh_v.h"
#include "auth_v.h"
#include "auth_i.h"
#ifdef FEATURE_UIM_RUIM
#include "uim.h"
#include "uim_v.h"
#include "mci.h"
#include "mci_i.h"
#ifdef FEATURE_CSIM
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"
#endif
#endif /* FEATURE_UIM_RUIM */

#include "diag.h"
#include "fs_public.h"

#if (defined(FEATURE_MDN_BASED_NAI) && defined(FEATURE_OTASP_UPDATE_MDN_NAI))
#include "dsmip.h"
#include "stringl.h"
#endif /* FEATURE_MDN_BASED_NAI && FEATURE_OTASP_UPDATE_MDN_NAI */

#ifdef FEATURE_CSIM

#ifndef FEATURE_1X_CP_MEM_OPT
/*  MMGSDI  Variables */
mmgsdi_cnf_type otasp_mmgsdi_cnf_buf;
#endif /* !FEATURE_1X_CP_MEM_OPT */

/*  MMGSDI related functions */
void otasp_mmgsdi_response_cb
(
  mmgsdi_return_enum_type  mmgsdi_status,
                               mmgsdi_cnf_enum_type     cnf,
                               const mmgsdi_cnf_type    *cnf_ptr
                              );
#endif /* FEATURE_CSIM */
#include "gstk_exp_v.h"
/*lint -e818 */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.


===========================================================================*/

/* ------------------------------------------------------------------
** Two nv_otasp_commit_type buffers are needed in case a new download
** request is received before the commit operation completes.
** ------------------------------------------------------------------ */
LOCAL nv_otasp_commit_type commit;   /* Buffer to store info to commit */
LOCAL nv_otasp_commit_type nv_commit;/* Buffer for NV commit command */

LOCAL boolean commit_ehrpd_imsi;
LOCAL nv_ehrpd_imsi_type otasp_ehrpd_imsi;

LOCAL otaspi_rtc_msg_type otasp_int; /* Buffer to store response message */

#ifdef FEATURE_UIM_RUIM
otaspi_rtc_msg_type ruim_otasp_int;  /* Buffer to store response message */
#endif /* FEATURE_UIM_RUIM */

/* Flag if NV is processing commit cmd */
LOCAL boolean otasp_commit_in_progress;

/* Flag if anything is committed to NV */
LOCAL boolean otasp_successful_commit;

LOCAL nv_cmd_type otasp_nv_buf;      /* Buffer for command to NV task */
LOCAL nv_item_type otasp_nv_item;  /* Item to hold LIST_ENABLED */

LOCAL otasp_dh_data otasp_dh;        /* Buffer to store otasp_dh data  */


LOCAL otaspi_roam_siz_type otasp_roam_size; /* Dimensions of roaming list */

LOCAL byte otasp_failed_unlock_attempts = 0;   /* Number of times bad SPC rxed */

LOCAL boolean otasp_call_ho_from_cdma;
/* Flag if OTASP has been handed off to AMPS */


boolean  otasp_reauth_in_progress; /* flag if reauthentication is in progress */

LOCAL dword reauth_req_rand;
    /*Stores the rand from the reauthentication req message */


boolean otasp_akey_commit_result;
 /* flag to indicate the result of the a-key committ operation */

LOCAL boolean sp_locked_state;
    /* boolean flag to indicate if the MS is locked */

LOCAL boolean nam_locked_state;
    /* boolean flag to indicate if SPASM protects access to NAM parameters */

LOCAL boolean otasp_committed_both_flag = FALSE;
  /* Flag to indicate if NAM parameters and AKey have been committed
     before sending the response    */

LOCAL boolean otasp_commit_akey_pending;
  /* Flag to indicate if AKey is to be committed */

LOCAL boolean new_roaming_list = TRUE;
    /* To indicate if a new roaming list is to be downloaded and this is
    ** used to initialize the roaming list */


LOCAL byte otasp_otapa_failed_spc_val_attempts = 0;
   /* Number of times bad SPC validation rxed */
LOCAL byte otasp_otapa_failed_spasm_val_attempts = 0;
   /* Number of times bad SPASM validation rxed */
LOCAL boolean otasp_otapa_session_occurred;
    /* Flag if an OTAPA session has occurred */


LOCAL boolean  otasp_auth_otapa_in_progress;
   /* flag if auth_otapa calculation is in progress */
LOCAL dword otasp_auth_otapa;
    /* Store the auth_otapa from auth_signature */
LOCAL boolean otasp_auth_otapa_ready;
    /* If auth_otapa is not ready, then need to calculate it */


#if defined(FEATURE_UIM_RUIM)
LOCAL dword rand_otapa;
    /* Stores the rand from the processing otapa request message */
#endif /* FEATURE_UIM_RUIM */


#ifdef FEATURE_UIM_RUIM
/* Table for maintaining info on the request messages */
otasp_ruim_req_rsp_table_type otasp_ruim_table;

/* Flag to indicate if we are sending the last segment to the card */
/* This helps in figuring out if the PRL has been committed in the card */
boolean otasp_ruim_last_segment;
/* The byte position of SSPR Request Response block */
#define PRL_NUM_REC_INDEX  6
#endif /* FEATURE_UIM_RUIM */

/* Used when validate PRL.
** updated when processing SSPR download request.
** and cleared after processing commit message.
*/
LOCAL prl_sspr_p_rev_e_type sspr_p_rev = PRL_SSPR_P_REV_INVALID;

/*===========================================================================

FUNCTION otasp_get_prl_stats

DESCRIPTION
  Get information about the PRL for OTASP configuration request processing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void otasp_get_prl_stats
(
  otaspi_roam_siz_type *rl_size,   /* Structure to store Roaming List stats*/

  const prl_hdr_s_type *prl_hdr_ptr /* Point at NV PRL information structure*/
)
{
  rl_size->max_pr_list_size = nv_max_size_of_roaming_list() ;
  /* The function in NV returns the number of bytes  and
     the number of octets is sent over the air */

  rl_size->cur_pr_list_size = prl_hdr_ptr->pr_list_size;
  /* The size of the roaming list in bytes as the number of octets are
     sent over the air */

  rl_size->num_acq_recs     = prl_hdr_ptr->num_acq_recs;

  rl_size->num_sys_recs     = prl_hdr_ptr->num_sys_recs;

  rl_size->pr_list_id       = prl_hdr_ptr->pr_list_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  M1X_MSG( DCP, LEGACY_MED,
    "Size=%d, ACQ=%d, SYS=%d",
    rl_size->cur_pr_list_size,
    rl_size->num_acq_recs,
    rl_size->num_sys_recs);

  rl_size->cur_sspr_p_rev = prl_hdr_ptr->sspr_p_rev; /*lint !e641 */
  /* Protocol revision of current preferred roaming list. */

  if( prl_hdr_ptr->sspr_p_rev == PRL_SSPR_P_REV_3 )
  {
    rl_size->num_comm_subnet_recs = prl_hdr_ptr->num_subnet_recs;
    /* Number of records in the common subnet table.*/

    M1X_MSG( DCP, LEGACY_MED,
      "Num comm subnet=%d",
      rl_size->num_comm_subnet_recs);
  }

} /* otasp_get_prl_stats */

/*===========================================================================

FUNCTION OTASP_GET_RL_STATS

DESCRIPTION
  This function returns information about the Preferred Roaming List for
  OTASP configuration request processing.

DEPENDENCIES
  None.

RETURN VALUE
  Fills in structure with MAX_PR_LIST_SIZE, CUR_PR_LIST_SIZE, NUM_ACQ_RECS,
  and NUM_SYS_RECS.

SIDE EFFECTS
  None.

===========================================================================*/

void otasp_get_rl_stats
(
  otaspi_roam_siz_type *rl_siz
    /* Structure to store Roaming List stats */
)
{
  byte           nv_sspr_p_rev = (byte) PRL_SSPR_P_REV_INVALID;
  prl_hdr_s_type prl_hdr, * prl_hdr_ptr = &prl_hdr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get sspr p rev from NV. Based on sspr p rev info, return requested data.
  */
  otasp_nv_buf.cmd = NV_READ_F;
  otasp_nv_buf.data_ptr   = (nv_item_type*) &nv_sspr_p_rev; //lint !e789 !e826
    // lint 789 - assigning auto value to static value.  Lint is flagging
    // that when this function ends, nv_sspr_p_rev goes out of scope and
    // otasp_nv_buf.data_ptr is left pointing to who-knows-what. Since
    // otasp_nv_buf.data_ptr is also set right before it is used, this is
    // not an issue.
  otasp_nv_buf.item       = NV_SSPR_P_REV_I;
  (void) mcc_nv_cmd( &otasp_nv_buf);

  /* Extract header of PRL*/
  prl_hdr_extract( prl_hdr_ptr,
                   (byte *)nv_pr_list.roaming_list,
                   (prl_sspr_p_rev_e_type)nv_sspr_p_rev );

  /* Populate the PRL dimension information. */
  otasp_get_prl_stats( rl_siz, prl_hdr_ptr );

  otasp_nv_buf.data_ptr = NULL;

} /* otasp_get_rl_stats */

#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION SEND_CFG_RESP_MSG

DESCRIPTION
   This function sends the Configuration Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_cfg_resp_msg(void)
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the Configuration Response Message */

  ruim_otasp_int.config_rsp.msg_type = OTASP_CONFIG_RSP_MSG;
  status = otaspx_int_to_ext( &len, &ruim_otasp_int, chari );
  if (status == OTASPX_DONE_S) /*lint !e641 */
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
}  /* send_cfg_resp_msg */
#endif /* FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION PROCESS_CONFIG_REQ

DESCRIPTION
  This function processes an OTASP Configuration Request Message,
  formats the appropriate Configuration Response Message, then
  calls the routine to send it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_config_req
(
  otaspi_config_req_msg_type* config_req_ptr
    /* Pointer to Configuration Request Message */
)
{
  word x;
  /* Variable to store value of memscpy() */
  size_t copy_size;
  byte imsi_index = 0;
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE)
  byte i;
  otaspi_rtc_pcs_data_type *pcs; /* Shorthand for PCS data */
  otaspi_rtc_cell_data_type *cell; /* Shorthand for Cellular data */
  otaspi_imsi_t_data_type *imsi;      /* Shorthand for IMSI_T data */
  otaspi_ehrpd_imsi_data_type *ehrpd_imsi; /* Shorthand for eHRPD_IMSI data */
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
#if defined( FEATURE_UIM_RUIM )
#ifdef FEATURE_CSIM
  mmgsdi_data_type config_data;
  mmgsdi_return_enum_type mmgsdi_status;
#else
  uim_cmd_type *uim_cmd_ptr;   /* Command */
#endif
  byte rej_block_index = 0;    /* index for tracking rejected blocks */

#ifdef FEATURE_CSIM
  config_data.data_ptr = NULL;
  config_data.data_len = 0;
 #endif

#endif /* FEATURE_UIM_RUIM */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    /* Initialize the number of blocks req to and recd from the card */
    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_card = 0;

    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_rsp_card = 0;

    /* Check to see if a message buffer is available */
    if ((otasp_ruim_table.req_index + 1) % OTASP_RUIM_MAX_MSG_BUFS !=
                                   otasp_ruim_table.rsp_index)
    {
      /* Store the message type */
      otasp_ruim_table.msg[otasp_ruim_table.req_index].msg_type =
                          OTASP_CONFIG_REQ_MSG;

      /* Store the number of blocks requested in the message */
      otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_msg =
                config_req_ptr->num_blocks;

      /* send a command for each block to the card */
      for (x = 0; x < config_req_ptr->num_blocks; x++)
      {
        #ifdef FEATURE_CSIM
        M1X_MSG( DCP, LEGACY_HIGH,
          "MMGSDI - OTASP sending config req blk_id = %d",
          config_req_ptr->block_id[x]);

        #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS  
        /* Don't send MMGSDI request if Sim Busy event has been received */
        if (mc_get_sim_busy_status())
        {
          M1X_MSG( DCP, LEGACY_HIGH,
                   "SIM Busy, Otasp config block req failed for block no = %d",
                    x );
          /* Store the id of the block that is not sent to the card */
          otasp_ruim_table.msg[otasp_ruim_table.req_index].
          rej_block_id[rej_block_index++] = config_req_ptr->block_id[x];

        }
        else
        #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
        {

          mmgsdi_status = mmgsdi_session_otasp_block_req (
                                   cdma.session_id,
                                   MMGSDI_OTASP_CONFIG,
                                   config_req_ptr->block_id[x],
                                   config_data,
                                   /* Config Data should be NULL here */
                                   otasp_mmgsdi_response_cb,
                                   0 );
          if (mmgsdi_status != MMGSDI_SUCCESS)
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "MMGSDI- OTASP Otasp_block_req returned error = %d",
              mmgsdi_status );
            /* Store the id of the block that is not sent to the card */
            otasp_ruim_table.msg[otasp_ruim_table.req_index].
            rej_block_id[rej_block_index++] = config_req_ptr->block_id[x];
          }
          else
          {
            /* Increment the number of blocks sent to the card */
              ++(otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                            num_blocks_req_card);
          }
        }
        #else
        if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
        {
           uim_cmd_ptr->hdr.command = UIM_CONFIG_REQ_F;

           uim_cmd_ptr->configuration.block_id = config_req_ptr->block_id[x];

           uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

           mcc_uim_cmd( uim_cmd_ptr );

           if (uim_cmd_ptr->hdr.status != UIM_RECEIVED_S)
           {
               M1X_MSG( DCP, LEGACY_ERROR,
                 "Bad status from UIM %d",
                 uim_cmd_ptr->hdr.status);

               /* Store the id of the block that is not processed by the card */
               otasp_ruim_table.msg[otasp_ruim_table.req_index].
                   rej_block_id[rej_block_index++] = config_req_ptr->block_id[x];
           }
           else
           {
               /* Increment the number of blocks sent to the card */
               ++(otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                        num_blocks_req_card);
           }
        }
        else
        {
           M1X_MSG( DCP, LEGACY_ERROR,
             "No free buffers on uim_free_q" );

           /* Store the id of the block that is not sent to the card */
           otasp_ruim_table.msg[otasp_ruim_table.req_index].
               rej_block_id[rej_block_index++] = config_req_ptr->block_id[x];
        }
       #endif
      }   /* end of for loop */
    } /* check for message buffer availability */

    /* If any block has been sent to the card successfully */
    if (otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                        num_blocks_req_card > 0)
    {
        /* consider this message queued and increment the request index */
        otasp_ruim_table.req_index = (otasp_ruim_table.req_index + 1) %
                                    OTASP_RUIM_MAX_MSG_BUFS;
    }
    else
    {
        /* none of the blocks are sent to the card */
        ruim_otasp_int.config_rsp.num_blocks = config_req_ptr->num_blocks;

        /* send a reject response for all the blocks in this message */
        for (x = 0; x < config_req_ptr->num_blocks; x++)
        {
            ruim_otasp_int.config_rsp.block[x].block_id =
                                       config_req_ptr->block_id[x];

            ruim_otasp_int.config_rsp.result_code[x] = OTASP_BLK_REJ_UNKNOWN;
            ruim_otasp_int.config_rsp.block[x].block_len = 0;
        }

        /* send the configuration response message */
        send_cfg_resp_msg();
    }
  }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  {
    otasp_int.config_rsp.msg_type = OTASP_CONFIG_RSP_MSG;
    otasp_int.config_rsp.num_blocks = config_req_ptr->num_blocks;

    for (x = 0; x < config_req_ptr->num_blocks; x++)
    {
      otasp_int.config_rsp.block[x].block_id = config_req_ptr->block_id[x];

      /* --------------------------------------------------------------
      ** The block_len for each block is filled in later by the packing
      ** routines, and is set to zero if the block is not supported.

         If SP_LOCKED_STATE or NAM_LOCKED_STATE is TRUE, then we
         explicitly set the block len to zero to make sure that
         the blocks are not processed during translation.
      ** -------------------------------------------------------------- */
      if ((sp_locked_state) || (nam_locked_state))
      {
          otasp_int.config_rsp.result_code[x] = OTASP_BLK_REJ_MS_LOCKED;
          otasp_int.config_rsp.block[x].block_len = 0;
      }
      else if ((!sp_locked_state) && (!nam_locked_state))
      {

       switch (config_req_ptr->block_id[x])
       {

        case OTASP_MOB_DIR_NUM_BLK: /* Mobile Directory Number */

          M1X_MSG( DCP, LEGACY_MED,
            "Reporting mob_dir");
          if( cdma.mob_dir.n_digits <= OTASPI_N_DIGITS_MAX )
          {
            otasp_int.config_rsp.block[x].blk.mob_dir.n_digits =
              cdma.mob_dir.n_digits;

            copy_size = memscpy( otasp_int.config_rsp.block[x].blk.mob_dir.digitn,
                               OTASPI_N_DIGITS_MAX*sizeof(byte),
                               cdma.mob_dir.digitn,
                               cdma.mob_dir.n_digits
                             );
            if( copy_size != cdma.mob_dir.n_digits )
            {
                M1X_MSG( DCP, LEGACY_ERROR,
                  "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                  copy_size,
                  cdma.mob_dir.n_digits,
                  OTASPI_N_DIGITS_MAX*sizeof(byte));
            }
            /* this is not the actual length, but is set to 1 to make
            sure that this block gets processed during translation */
            otasp_int.config_rsp.block[x].block_len = 1;
            otasp_int.config_rsp.result_code[x] = OTASP_BLK_ACCEPT;
          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Invalid cdma.mob_dir.n_digits:%d compared to OTASPI_N_DIGITS_MAX",
              cdma.mob_dir.n_digits);

            otasp_int.config_rsp.block[x].block_len = 0;
            otasp_int.config_rsp.result_code[x] = OTASP_BLK_REJ_INVALID;
          }
        break;

        case OTASP_CDMA_ANALOG_NAM_BLK: /* CDMA/Analog NAM Download Data */
          /* -------------------------------------------------------------
          ** CDMA NAM information is still reported even if the phone does
          ** not support CDMA at 800 MHz e.g. a dual band phone.  The PCS
          ** band CDMA NAM information is reported instead.
          ** ------------------------------------------------------------- */
          M1X_MSG( DCP, LEGACY_MED,
            "Reporting CDMA Analog NAM data");
          cell = &otasp_int.config_rsp.block[x].blk.rtc_cell;

          /* Fill in home_sid, firstchp, and ex */

          /* This phone doesn't support analog. Set analog fields to zero */
          cell->home_sid = 0;
          cell->firstchp = 0;
          cell->ex = 0;

          cell->scm = cdma.scm;
          cell->mob_p_rev = cdma.mob_cai_rev;
          cell->imsi_class =
             (cdma.imsi_m_addr_num == IMSI_CLASS_0_ADDR_NUM)? 0 : 1;
          cell->imsi_addr_num = (cdma.imsi_m_addr_num == IMSI_CLASS_0_ADDR_NUM)?
                               0 : cdma.imsi_m_addr_num;
          cell->mcc = cdma.imsi_m_mcc;
          cell->imsi_11_12 = cdma.imsi_m_11_12;
          qw_equ( cell->imsi_s, cdma.imsi_m_s );
          cell->accolc = cdma.accolc;
          cell->local_control = 0;  /* We don't support any local control opt */
          cell->mob_term_home = cdma.mob_term_home;
          cell->mob_term_for_sid = cdma.mob_term_for_sid;
          cell->mob_term_for_nid = cdma.mob_term_for_nid;
          cell->max_sid_nid = OTASPI_SID_NID_MAX;

          /* Initialize the counter */
          i = 0;

          /* Upload the sid nid list until you encounter a sid = 0 */
          while ((i < OTASPI_SID_NID_MAX) && (cdma.pair[i].sid != 0))
          {
            cell->var[i].sid = cdma.pair[i].sid;
            cell->var[i].nid = cdma.pair[i].nid;
            i++;
          }
          cell->stored_sid_nid = i;  /*lint !e661 */

          otasp_int.config_rsp.result_code[x] = OTASP_BLK_ACCEPT;
          otasp_int.config_rsp.block[x].block_len = 1;

        break;

        case OTASP_CDMA_NAM_BLK: /* CDMA NAM Download Data */
          M1X_MSG( DCP, LEGACY_MED,
            "Reporting CDMA NAM data");
          pcs = &otasp_int.config_rsp.block[x].blk.rtc_pcs;

          pcs->slotted_mode = TRUE;
          pcs->mob_p_rev = cdma.mob_cai_rev;
          pcs->imsi_class = (cdma.imsi_m_addr_num == IMSI_CLASS_0_ADDR_NUM)?0:1;
          pcs->imsi_addr_num = (cdma.imsi_m_addr_num == IMSI_CLASS_0_ADDR_NUM)?
                                0 : cdma.imsi_m_addr_num;
          pcs->mcc = cdma.imsi_m_mcc;
          pcs->imsi_11_12 = cdma.imsi_m_11_12;
          qw_equ( pcs->imsi_s, cdma.imsi_m_s );
          pcs->accolc = cdma.accolc;
          pcs->local_control = 0;  /* We don't support any local control opt */
          pcs->mob_term_home = cdma.mob_term_home;
          pcs->mob_term_for_sid = cdma.mob_term_for_sid;
          pcs->mob_term_for_nid = cdma.mob_term_for_nid;
          pcs->max_sid_nid = OTASPI_SID_NID_MAX;

          i = 0;

          /* Upload the sid nid list until you encounter a sid = 0 */
          while ((i < OTASPI_SID_NID_MAX) && (cdma.pair[i].sid != 0))
          {
            pcs->var[i].sid = cdma.pair[i].sid;
            pcs->var[i].nid = cdma.pair[i].nid;
            i++;
          }
          pcs->stored_sid_nid = i; /* Number of sid nid entries */ /*lint !e661 */
          otasp_int.config_rsp.result_code[x] = OTASP_BLK_ACCEPT;
          otasp_int.config_rsp.block[x].block_len = 1;
        break;

        case OTASP_PREF_ROAM_LIST_SIZE_BLK: /* Roaming List Dimensions */
          otasp_int.config_rsp.result_code[x] = OTASP_BLK_REJ_BLOCK_ID;
          otasp_int.config_rsp.block[x].block_len = 0;

        break;

        case OTASP_PREF_ROAM_LIST_BLK: /* Roaming List */
        /* Currently uploading the Roaming List is not supported */
           otasp_int.config_rsp.result_code[x] = OTASP_BLK_REJ_BLOCK_ID;
           otasp_int.config_rsp.block[x].block_len = 0;
        break;

        case OTASP_IMSI_T_BLK:
           otasp_int.config_rsp.block[x].block_len = 1;

           M1X_MSG( DCP, LEGACY_MED,
             "Reporting IMSI data");
           imsi = &otasp_int.config_rsp.block[x].blk.imsi_t;
           imsi->imsi_t_class =
                 (cdma.imsi_t_addr_num == IMSI_CLASS_0_ADDR_NUM)? 0 : 1;
           imsi->imsi_t_addr_num =
                 (cdma.imsi_t_addr_num == IMSI_CLASS_0_ADDR_NUM)?
                               0 : cdma.imsi_t_addr_num;
           imsi->imsi_t_mcc =  cdma.imsi_t_mcc;
           imsi->imsi_t_11_12 =  cdma.imsi_t_11_12;
           qw_equ( imsi->imsi_t_s, cdma.imsi_t_s );
           otasp_int.config_rsp.result_code[x] = OTASP_BLK_ACCEPT;
        break;

        case OTASP_EHRPD_IMSI_BLK:
        {

          M1X_MSG( DCP, LEGACY_MED,
            "Reporting eHRPD IMSI data");
          ehrpd_imsi = &otasp_int.config_rsp.block[x].blk.ehrpd_imsi;
          /* Read from NV */
          otasp_nv_buf.cmd = NV_READ_F;
          otasp_nv_buf.data_ptr = &otasp_nv_item;
          otasp_nv_buf.item = NV_EHRPD_IMSI_I;
          if (mcc_nv_cmd( &otasp_nv_buf ) == NV_DONE_S)
          {
            if (otasp_nv_item.ehrpd_imsi.num_of_mnc_digits == 2)
            {
              ehrpd_imsi->ehrpd_mnc_num_digits = 0;
            }
            else if (otasp_nv_item.ehrpd_imsi.num_of_mnc_digits == 3)
            {
              ehrpd_imsi->ehrpd_mnc_num_digits = 1;
            }
            else
            {
              M1X_MSG( DCP, LEGACY_MED,
                "Invalid number of mnc digits %d",
                otasp_nv_item.ehrpd_imsi.num_of_mnc_digits);
              otasp_int.config_rsp.result_code[x] = OTASP_BLK_REJ_UNKNOWN;
              otasp_int.config_rsp.block[x].block_len = 0;
              break;
            }

            for(i = 0; i < otasp_nv_item.ehrpd_imsi.num_of_mcc_digits; i++)
            {
              ehrpd_imsi->ehrpd_imsi_digits[imsi_index] = otasp_nv_item.ehrpd_imsi.ehrpd_mcc[i];
              imsi_index++;
            }

            for(i = 0; i < otasp_nv_item.ehrpd_imsi.num_of_mnc_digits; i++)
            {
              ehrpd_imsi->ehrpd_imsi_digits[imsi_index] = otasp_nv_item.ehrpd_imsi.ehrpd_mnc[i];
              imsi_index++;
            }

            for(i = 0; i < otasp_nv_item.ehrpd_imsi.num_of_msin_digits; i++)
            {
              ehrpd_imsi->ehrpd_imsi_digits[imsi_index] = otasp_nv_item.ehrpd_imsi.ehrpd_msin[i];
              imsi_index++;
            }

            ehrpd_imsi->ehrpd_imsi_num_digits = imsi_index;

            otasp_int.config_rsp.result_code[x] = OTASP_BLK_ACCEPT;
            otasp_int.config_rsp.block[x].block_len = 1;
          }
          else
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Failed to read item eHRPD IMSI from NV");
            otasp_int.config_rsp.result_code[x] = OTASP_BLK_REJ_UNKNOWN;
            otasp_int.config_rsp.block[x].block_len = 0;
          }
        }
        break;

        default:
          /* Block ID Not Supported */
          otasp_int.config_rsp.result_code[x] = OTASP_BLK_REJ_BLOCK_ID;
          otasp_int.config_rsp.block[x].block_len = 0;

        break;

       } /* end switch */
      }
    } /* end for */

    /* Translate and send the Configuration Response Message */

    status = otaspx_int_to_ext( &len, &otasp_int, chari );

    if (status == OTASPX_DONE_S)  /*lint !e641 */
    {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
    }
    else
    {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
    }
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

} /* process_config_req */

#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION SEND_DLOAD_RESP_MSG

DESCRIPTION
   This function sends the Download Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_dload_resp_msg(void)
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the Download Response Message */
  ruim_otasp_int.dload_rsp.msg_type = OTASP_DOWNLOAD_RSP_MSG;
  status = otaspx_int_to_ext( &len, &ruim_otasp_int, chari );
  if (status == OTASPX_DONE_S)  /*lint !e641 */
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
}  /* send_dload_resp_msg */

#endif /* FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION PROCESS_DLOAD_REQ

DESCRIPTION
  This function processes an OTASP Download Request Message,
  formats the appropriate Download Response Message, then
  calls the routine to send it.

DEPENDENCIES
  None.

RETURN VALUE
  Next state to be processed.  CDMA_CONV or CDMA_EXIT for powerdown.

SIDE EFFECTS
  None.

===========================================================================*/

word process_dload_req
(
  otaspi_download_req_msg_type* dload_req_ptr
    /* Pointer to Download Request Message */
)
{
  word x;    /* loop counters */
  word next_state; /* Next state to be processed */

  size_t copy_size;
  /* Return value of memscpy function */

#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  word y;   /* loop counter */
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  otaspi_ftc_pcs_data_type *pcs; /* Shorthand for PCS NAM information */
  otaspi_ftc_cell_data_type *cell; /* Shorthand for cellular NAM information */
  otaspi_imsi_t_data_type *imsi_t; /* Pointer to imsi_t items */
  otaspi_ehrpd_imsi_data_type *ehrpd_imsi; /* Shorthand for eHRPD_IMSI data */
#endif /* |FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

#ifdef FEATURE_UIM_RUIM
  byte rej_block_index = 0;    /* index for tracking rejected blocks */
#endif
#if defined( FEATURE_UIM_RUIM ) && ( !defined(FEATURE_CSIM) )
  uim_cmd_type *uim_cmd_ptr; /* Command */
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_CSIM
  mmgsdi_data_type dload_data;
  mmgsdi_return_enum_type mmgsdi_status;
#endif /* FEATURE_CSIM */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ---------------------------------------------------------
  ** OTASP messages are only processed in CDMA_CONV substate.
  ** Currently next_state is only changed if too many attempts
  ** are made to unlock the phone, in which case we powerdown.
  ** --------------------------------------------------------- */
  next_state = CDMA_TRAFFIC; /*lint !e641 */

  /* Check for num_blocks validation */
  if( dload_req_ptr->num_blocks > OTASPI_PARAM_BLOCK_MAX )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Unexpected Download Request Message with num_blocks:%d > OTASPI_PARAM_BLOCK_MAX",
      dload_req_ptr->num_blocks );
    return next_state;
  }

  /* The next state is not changed as the attempts are made to unlock
     the phone using this message only for Non-std OTASP and R-UIm does
     not support NS OTASP */

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    /* Initialize the number of blocks req to and recd from the card */
    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_card = 0;

    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_rsp_card = 0;

    /* Check to see if a message buffer is available */
    if ((otasp_ruim_table.req_index + 1) % OTASP_RUIM_MAX_MSG_BUFS !=
                                   otasp_ruim_table.rsp_index)
    {
      /* Store the message type */
      otasp_ruim_table.msg[otasp_ruim_table.req_index].msg_type =
                          OTASP_DOWNLOAD_REQ_MSG;

      /* Store the number of blocks requested in the message */
      otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_msg =
                dload_req_ptr->num_blocks;

      /* send a command for each block to the card */
      for (x = 0; x < dload_req_ptr->num_blocks; x++)
      {
        #ifdef FEATURE_CSIM
        dload_data.data_len = dload_req_ptr->block[x].block_len;
        /* Copy the data that is downloaded via OTASP to the command buffer */
        dload_data.data_ptr = dload_req_ptr->block[x].blk.ruim_data;
                  M1X_MSG( DCP, LEGACY_HIGH,
                    "MMGSDI OTASP - DLOAD , blk_id = %d",
                    dload_req_ptr->block[x].block_id );

        #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS  
        /* Don't send MMGSDI request if Sim Busy event has been received */
        if ( (mc_get_sim_busy_status()) &&
             (rej_block_index < OTASP_RUIM_MAX_BLOCKS) )
        {
          M1X_MSG( DCP, LEGACY_HIGH, 
                 "SIM Busy, Otasp download block req failed for block no = %d",
                  x );
          /* Store the id of the block that is not sent to the card */
          otasp_ruim_table.msg[otasp_ruim_table.req_index].
             rej_block_id[rej_block_index++] = dload_req_ptr->block[x].block_id;


        }
        else
        #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
        {

          mmgsdi_status = mmgsdi_session_otasp_block_req ( cdma.session_id,
                                                    MMGSDI_OTASP_DOWNLOAD,
                                                    dload_req_ptr->block[x].block_id,
                                                    dload_data,
                                                    otasp_mmgsdi_response_cb,
                                                    0 );
          if ((mmgsdi_status != MMGSDI_SUCCESS ) &&
             (rej_block_index < OTASP_RUIM_MAX_BLOCKS))
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "MMGSDI queuing returned err = %d",
              mmgsdi_status );
            /* Store the id of the block that is not sent to the card */
            otasp_ruim_table.msg[otasp_ruim_table.req_index].
               rej_block_id[rej_block_index++] = dload_req_ptr->block[x].block_id;
          }
          else
          {
            /* Increment the number of blocks sent to the card */
            ++(otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                          num_blocks_req_card);
          }
        }
        #else
        if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
        {
          uim_cmd_ptr->hdr.command = UIM_DOWNLOAD_REQ_F;

          uim_cmd_ptr->download.block_id = dload_req_ptr->block[x].block_id;

          if( (dload_req_ptr->block[x].block_len <= UIM_MAX_CHARS) &&
              (dload_req_ptr->block[x].block_len <= sizeof(dload_req_ptr->block[x].blk.ruim_data)) )
          {
            uim_cmd_ptr->download.block_len = dload_req_ptr->block[x].block_len;
            /* Copy the data that is downloaded via OTASP to the command buffer */
            copy_size = memscpy(uim_cmd_ptr->download.data,
                                UIM_MAX_CHARS*sizeof(byte),
                                dload_req_ptr->block[x].blk.ruim_data,
                   dload_req_ptr->block[x].block_len);
            if( copy_size != dload_req_ptr->block[x].block_len )
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                copy_size,
                dload_req_ptr->block[x].block_len,
                UIM_MAX_CHARS*sizeof(byte));
            }
          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Unexpected Download Request Message with num_blocks:%d > OTASPI_PARAM_BLOCK_MAX",
              dload_req_ptr->num_blocks );
            uim_cmd_ptr->download.block_len = 0;
          }
          uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

          mcc_uim_cmd( uim_cmd_ptr );

          if (uim_cmd_ptr->hdr.status != UIM_RECEIVED_S)
          {
             M1X_MSG( DCP, LEGACY_ERROR,
               "Bad status from UIM %d",
               uim_cmd_ptr->hdr.status);

             /* Store the id of the block that is not processed by the card */
             otasp_ruim_table.msg[otasp_ruim_table.req_index].
                 rej_block_id[rej_block_index++] =
                                       dload_req_ptr->block[x].block_id;
          }
          else
          {
            /* Increment the number of blocks sent to the card */
            ++(otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                     num_blocks_req_card);
          }
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "No free buffers on uim_free_q" );

          /* Store the id of the block that is not sent to the card */
          otasp_ruim_table.msg[otasp_ruim_table.req_index].
             rej_block_id[rej_block_index++] = dload_req_ptr->block[x].block_id;
        }
        #endif
      } /* end of for loop */
    }/* message buffer availability */

    /* If any block has been sent to the card successfully */
    if (otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                        num_blocks_req_card > 0)
    {
        /* consider this message queued and increment the request index */
        otasp_ruim_table.req_index = (otasp_ruim_table.req_index + 1) %
                                    OTASP_RUIM_MAX_MSG_BUFS;
    }
    else
    {
        /* none of the blocks are sent to the card */
        ruim_otasp_int.dload_rsp.num_blocks = dload_req_ptr->num_blocks;

        /* send a reject response for all the blocks in this message */
        for (x = 0; x < dload_req_ptr->num_blocks; x++)
        {
            ruim_otasp_int.dload_rsp.block[x].block_id =
                                       dload_req_ptr->block[x].block_id;

            ruim_otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_UNKNOWN;
        }

        /* send the download response message */
        send_dload_resp_msg();
    }
  }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  {
    otasp_int.dload_rsp.msg_type = OTASP_DOWNLOAD_RSP_MSG;
    otasp_int.dload_rsp.num_blocks = dload_req_ptr->num_blocks;

    for (x = 0; x < dload_req_ptr->num_blocks; x++)
    {
      otasp_int.dload_rsp.block[x].block_id =
                                   dload_req_ptr->block[x].block_id;

      switch (dload_req_ptr->block[x].block_id)
      {
        case OTASP_CDMA_ANALOG_NAM_BLK: /* Cellular Indicator Download Data */
        {

        /* ---------------------------------------------------------------
        ** The cellular data is processed for the dual band phone due to
        ** the analog information it contains.  The CDMA NAM info is used
        ** as the PCS NAM.
        ** --------------------------------------------------------------- */

          /* If this block is bad it deletes any previously rxed block */

          /* Block Length is set to zero if parsing fails */
          if (dload_req_ptr->block[x].block_len == 0)
          {
            /* Data Size Mismatch */
            otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_DATA_SIZE;
            commit.commit_ftc_cdma = FALSE;
            commit.commit_ftc_amps = FALSE;
          }
          else if (dload_req_ptr->block[x].blk.ftc_cell.n_sid_nid == 0)
          {
            /* Invalid parameter */
            otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_INVALID;
            commit.commit_ftc_cdma = FALSE;
            commit.commit_ftc_amps = FALSE;
          }
          else if (dload_req_ptr->block[x].blk.ftc_cell.n_sid_nid >
                   OTASPI_SID_NID_MAX)
          {
            /* SID/NID length mismatch */
            otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_SID_NID;
            commit.commit_ftc_cdma = FALSE;
            commit.commit_ftc_amps = FALSE;
          }
          else
          {
            cell = &dload_req_ptr->block[x].blk.ftc_cell;

            /* Store off CDMA NAM data and flag as OK to commit */
            commit.ftc_cdma.imsi_addr_num = (cell->imsi_class == 0)?
                                           IMSI_CLASS_0_ADDR_NUM :
                                           cell->imsi_addr_num;
            commit.ftc_cdma.mcc = cell->mcc;
            commit.ftc_cdma.imsi_11_12 = cell->imsi_11_12;
            commit.ftc_cdma.imsi_s1 = (qw_lo(cell->imsi_s) & 0x00FFFFFFL);
            commit.ftc_cdma.imsi_s2 = (word) ((qw_lo(cell->imsi_s) >> 24) |
                                  ((qw_hi(cell->imsi_s) & 0x3L) << 8));

            commit.ftc_cdma.accolc = cell->accolc;
            commit.ftc_cdma.mob_term_home = cell->mob_term_home;
            commit.ftc_cdma.mob_term_for_sid = cell->mob_term_for_sid;
            commit.ftc_cdma.mob_term_for_nid = cell->mob_term_for_nid;

            for (y=0; y < OTASPI_SID_NID_MAX ; y++)
            {
              /* Copy SID NID pairs from OTASP download request message */
              if (y < cell->n_sid_nid)
              {
                commit.ftc_cdma.pair[y].sid = cell->var[y].sid;
                commit.ftc_cdma.pair[y].nid = cell->var[y].nid;
              }
              /* Clear old SID NID pairs */
              else if (commit.ftc_cdma.pair[y].sid != 0)
              {
                commit.ftc_cdma.pair[y].sid = 0;
                commit.ftc_cdma.pair[y].nid = 0;
              }
              /* No more old SID NID pairs need to be cleared */
              else
              {
                break;
              }
            } /* end for */

            commit.commit_ftc_cdma = TRUE;
            M1X_MSG( DCP, LEGACY_MED,
              "OK to commit CDMA NAM data from CID");

            /* Set CDMA_PREF_SERV = current system??? */

            otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_ACCEPT;

            /* Send OTASP status to CM */
            otasp_report_status_to_cm(CM_OTASP_STATUS_NAM_DOWNLOADED);
          }
          break;
        }

        case OTASP_MOB_DIR_NUM_BLK: /* Mobile Directory Number */
        {
          boolean valid_block = TRUE; /* Flag if we are to commit this block */

          /* Block Length is set to zero if parsing fails */
          if (dload_req_ptr->block[x].block_len == 0)
          {
            /* Data Size Mismatch */
            otasp_int.dload_rsp.block[x].result_code =
              OTASP_BLK_REJ_DATA_SIZE;

            /* If a good block was rxed earlier, this bad one deletes it */
            commit.commit_mob_dir = FALSE;
          }
          else
          {

            if( dload_req_ptr->block[x].blk.mob_dir.n_digits <= OTASPI_N_DIGITS_MAX )
            {

              /* Check all digits to make sure they are valid */
              for (y = 0; y < dload_req_ptr->block[x].blk.mob_dir.n_digits; y++)
              {
                /* Valid digits are 1-9, 0 (10) * (11) and # (12) */
                if ((dload_req_ptr->block[x].blk.mob_dir.digitn[y] == 0) ||
                    (dload_req_ptr->block[x].blk.mob_dir.digitn[y] > 12))
                {
                  /* Invalid parameter */
                  otasp_int.dload_rsp.block[x].result_code =
                    OTASP_BLK_REJ_INVALID;
                  valid_block = FALSE;
                  /* If a good block was rxed earlier, this bad one deletes it */
                  commit.commit_mob_dir = FALSE;
                  break;
                }
              }
            }/* if( dload_req_ptr->block[x].blk.mob_dir.n_digits <= OTASPI_N_DIGITS_MAX ) */
            else
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "Unexpected Download Request Message with mob_dir.n_digits:%d > OTASPI_N_DIGITS_MAX",
                dload_req_ptr->block[x].blk.mob_dir.n_digits );
              /* Invalid parameter */
              otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_INVALID;
              valid_block = FALSE;
              /* If a good block was rxed earlier, this bad one deletes it */
              commit.commit_mob_dir = FALSE;
              break;
            }

            if (valid_block)
            {
              /* store off directory number and flag as OK to commit */
              commit.mob_dir.nam = cdma.curr_nam;
              commit.mob_dir.n_digits =
                dload_req_ptr->block[x].blk.mob_dir.n_digits;
              copy_size = memscpy( (byte*) commit.mob_dir.digitn,
                                    sizeof(commit.mob_dir.digitn),
                            dload_req_ptr->block[x].blk.mob_dir.digitn,
                            dload_req_ptr->block[x].blk.mob_dir.n_digits );
              if( copy_size != dload_req_ptr->block[x].blk.mob_dir.n_digits )
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                  "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                  copy_size,
                  dload_req_ptr->block[x].blk.mob_dir.n_digits,
                  sizeof(commit.mob_dir.digitn));
              }
              commit.commit_mob_dir = TRUE;
              M1X_MSG( DCP, LEGACY_MED,
                "OK to commit mob_dir");

              /* Set CDMA_PREF_SERV = current system??? */

              otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_ACCEPT;

              /* Send OTASP status to CM */
              otasp_report_status_to_cm(CM_OTASP_STATUS_MDN_DOWNLOADED);
            }
          }
          break;
        }

        case OTASP_CDMA_NAM_BLK: /* PCS Indicator Download Data */
        {
          /* If this block is bad it deletes any previously rxed block */

          /* Block Length is set to zero if parsing fails */
          if (dload_req_ptr->block[x].block_len == 0)
          {
            /* Data Size Mismatch */
            otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_DATA_SIZE;
            commit.commit_ftc_cdma = FALSE;
          }
          else if (dload_req_ptr->block[x].blk.ftc_pcs.n_sid_nid == 0)
          {
            /* Invalid parameter */
            otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_INVALID;
            commit.commit_ftc_cdma = FALSE;
          }
          else if (dload_req_ptr->block[x].blk.ftc_pcs.n_sid_nid >
                   OTASPI_SID_NID_MAX)
          {
            /* SID/NID length mismatch */
            otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_SID_NID;
            commit.commit_ftc_cdma = FALSE;
          }
          else
          {
            pcs = &dload_req_ptr->block[x].blk.ftc_pcs;

            /* Store off CDMA NAM data and flag as OK to commit */
            commit.ftc_cdma.imsi_addr_num = (pcs->imsi_class == 0)?
                                             IMSI_CLASS_0_ADDR_NUM :
                                             pcs->imsi_addr_num;
            commit.ftc_cdma.mcc = pcs->mcc;
            commit.ftc_cdma.imsi_11_12 = pcs->imsi_11_12;
            commit.ftc_cdma.imsi_s1 = (qw_lo(pcs->imsi_s) & 0x00FFFFFFL);
            commit.ftc_cdma.imsi_s2 = (word) ((qw_lo(pcs->imsi_s) >> 24) |
                                  ((qw_hi(pcs->imsi_s) & 0x3L) << 8));

            commit.ftc_cdma.accolc = pcs->accolc;
            commit.ftc_cdma.mob_term_home = pcs->mob_term_home;
            commit.ftc_cdma.mob_term_for_sid = pcs->mob_term_for_sid;
            commit.ftc_cdma.mob_term_for_nid = pcs->mob_term_for_nid;

            for (y=0; y < OTASPI_SID_NID_MAX ; y++)
            {
              /* Copy SID NID pairs from OTASP download request message */
              if (y < pcs->n_sid_nid)
              {
                commit.ftc_cdma.pair[y].sid = pcs->var[y].sid;
                commit.ftc_cdma.pair[y].nid = pcs->var[y].nid;
              }
              /* Clear old SID NID pairs */
              else if (commit.ftc_cdma.pair[y].sid != 0)
              {
                commit.ftc_cdma.pair[y].sid = 0;
                commit.ftc_cdma.pair[y].nid = 0;
              }
              /* No more old SID NID pairs need to be cleared */
              else
              {
                break;
              }
            } /* end for */

            commit.commit_ftc_cdma = TRUE;
            M1X_MSG( DCP, LEGACY_MED,
              "OK to commit CDMA NAM data from PID");

            /* Set CDMA_PREF_SERV = current system??? */

            otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_ACCEPT;

            /* Send OTASP status to CM */
            otasp_report_status_to_cm(CM_OTASP_STATUS_NAM_DOWNLOADED);
          }
          break;
        }

        case OTASP_PREF_ROAM_LIST_BLK: /* Roaming List */
        {
        /* this is a proprietary block and if we are doing 683-A, then we
           reject the message  */
           otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_BLOCK_ID;
          break;
        }

        case OTASP_VERIFY_SPC_BLK: /* Programming Lock Code Verify */
        {
           /* If we receive this block when we are doing 683-A OTASP, we
              reject this block as not supported */
           otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_BLOCK_ID;
          break;
        }

        case OTASP_IMSI_T_BLK:
        {
           imsi_t = &dload_req_ptr->block[x].blk.imsi_t;
           commit.imsi_t.addr_num = (imsi_t->imsi_t_class == 0)?
                                           IMSI_CLASS_0_ADDR_NUM :
                                           imsi_t->imsi_t_addr_num;
           commit.imsi_t.mcc = imsi_t->imsi_t_mcc;
           commit.imsi_t.imsi_t_11_12 = imsi_t->imsi_t_11_12;
           commit.imsi_t.s1 =  (qw_lo(imsi_t->imsi_t_s) & 0x00FFFFFFL);
           commit.imsi_t.s2 = (word) ((qw_lo(imsi_t->imsi_t_s) >> 24) |
                                  ((qw_hi(imsi_t->imsi_t_s) & 0x3L) << 8));

           commit.commit_imsi_t = TRUE;
           otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_ACCEPT;

           /* Send OTASP status to CM */
           otasp_report_status_to_cm(CM_OTASP_STATUS_IMSI_DOWNLOADED);
           break;
        }

        case OTASP_EHRPD_IMSI_BLK:
        {
          byte imsi_index = 0;
          ehrpd_imsi = &dload_req_ptr->block[x].blk.ehrpd_imsi;
          /* Copy the MCC */
          otasp_ehrpd_imsi.num_of_mcc_digits = OTASPI_EHRPD_IMSI_MCC_DIGITS;
          for (y = 0; y < otasp_ehrpd_imsi.num_of_mcc_digits; y++)
          {
            otasp_ehrpd_imsi.ehrpd_mcc[y] = ehrpd_imsi->ehrpd_imsi_digits[imsi_index];
            imsi_index++;
          }

          /* Copy the MNC */
          otasp_ehrpd_imsi.num_of_mnc_digits = ((ehrpd_imsi->ehrpd_mnc_num_digits == 0)? 2 : 3);
          for (y = 0; (y < otasp_ehrpd_imsi.num_of_mnc_digits) &&
              (imsi_index < OTASPI_EHRPD_IMSI_DIGITS_MAX) &&
              (y < MAX_MSIN_DIGITS); y++)
          {
            otasp_ehrpd_imsi.ehrpd_mnc[y] = ehrpd_imsi->ehrpd_imsi_digits[imsi_index];
            imsi_index++;
          }

          /* Copy the MSIN */
          otasp_ehrpd_imsi.num_of_msin_digits =
                 ehrpd_imsi->ehrpd_imsi_num_digits - imsi_index;
          for (y = 0; (y < otasp_ehrpd_imsi.num_of_msin_digits) &&
              (imsi_index < OTASPI_EHRPD_IMSI_DIGITS_MAX) &&
              (y < MAX_MSIN_DIGITS); y++)
          {
            otasp_ehrpd_imsi.ehrpd_msin[y] = ehrpd_imsi->ehrpd_imsi_digits[imsi_index];
            imsi_index++;
          }

          commit_ehrpd_imsi = TRUE;
          otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_ACCEPT;
          break;
        }

        default:
          /* Block ID Not Supported */
          otasp_int.dload_rsp.block[x].result_code = OTASP_BLK_REJ_BLOCK_ID;
        break;
      } /* end switch */
    } /* end for */

    /* Translate and send the Download Response Message */

    status = otaspx_int_to_ext( &len, &otasp_int, chari );

    if (status == OTASPX_DONE_S) /*lint !e641 */
    {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
    }
    else
    {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
    }
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
  return (next_state);

} /* end process_dload_req */

/*===========================================================================

FUNCTION PROCESS_MS_KEY_REQ

DESCRIPTION
  This function processes an OTASP MS Key Request Message,
  formats the appropriate MS Key Response Message, then
  calls the routine to send it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void process_ms_key_req
(
  otaspi_ms_key_req_msg_type* ms_key_req_ptr
    /* Pointer to MS Key Request Message */
)
{
   size_t copy_size;
  /* Return value of memscpy function */
#if defined (FEATURE_UIM_RUIM)
   byte i;
#endif

#ifdef FEATURE_UIM_RUIM
   word pos;
   dword temp_rand;
#ifdef FEATURE_CSIM
   byte  randseed[UIM_RAND_SEED];
   byte  param_p[UIM_PARAM_P];    /* Param P */
   byte  param_g[UIM_PARAM_G];   /* This is still accessed from UIM */
   mmgsdi_data_type key_req_randseed,key_req_param_p,key_req_param_g;
   mmgsdi_return_enum_type mmgsdi_status;
#else
   uim_cmd_type *uim_cmd_ptr;
#endif
#endif /* FEATURE_UIM_RUIM */


   dh_cmd_type *dh_cmd_ptr;
    /* Pointer to dh command */
   byte * random_number;
   byte j, k, offset;  /* to calculate the offset into the array */
   dword temp;  /* temporary variable to store a dword */

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    // Moved this block to the beginning of the function and added
    // return to prevent multiple MS Key Response messages from being sent.
    if (OTASP_KEY_EXCHANGE_P_REV < ms_key_req_ptr->a_key_p_rev)
    {
      /* --------------------------------------------------------
      ** If the protocol revision level supported by the mobile
      ** statiom is less than the minimum protocol revision level
      ** supported by the base station, the mobile station shall
      ** indicate the error condition by rejecting the message
      ** with a protocol version mismatch indication.
      ** -------------------------------------------------------- */
      otasp_int.ms_key_rsp.result_code = OTASP_BLK_REJ_PROTOCOL;
      M1X_MSG( DCP, LEGACY_HIGH,
        "Min A-Key P_rev > mob_a_key_p_rev" );
      send_ms_key_resp_msg();
      return;
   }

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
   /* Determine if the R-UIM is available for this NAM */
   if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
   {
     #ifdef FEATURE_CSIM
     pos = 0;
     for (i = 0; i < 5; i++)
     {
       temp_rand = mc_sec_ran_next();
       b_packd(temp_rand,randseed,pos,sizeof(dword)*8);
       pos += (sizeof(dword) * 8);
     }

     key_req_randseed.data_len = 20;
     key_req_randseed.data_ptr = randseed;
     /* The contents of param p and g are in dword format and we would
        like to send data to the card in OTA format resulting in
        unpacking the dword */

     pos = 0;
     for (i = 0; i < (ms_key_req_ptr->param_g_len/sizeof(dword)); i++)
     {
       b_packd(ms_key_req_ptr->param_g[i],
               param_g, pos, sizeof(dword)*8);
       pos += (sizeof(dword) * 8);
     }
     key_req_param_g.data_len = ms_key_req_ptr->param_g_len;
     key_req_param_g.data_ptr = param_g;

     pos = 0;
     for (i = 0; i < (ms_key_req_ptr->param_p_len/sizeof(dword)); i++)
     {
       b_packd(ms_key_req_ptr->param_p[i],
               param_p, pos, sizeof(dword)*8);
       pos += (sizeof(dword) * 8);
     }
     key_req_param_p.data_len = ms_key_req_ptr->param_p_len;
     key_req_param_p.data_ptr = param_p;

     #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
     /* Don't send MMGSDI request if Sim Busy event has been received */  
     if (mc_get_sim_busy_status())
     {
       M1X_MSG( DCP, LEGACY_HIGH, "SIM Busy, Otasp MS key req failed" );
       otasp_int.ms_key_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
       send_ms_key_resp_msg();
     }
     else
     #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
     {
       mmgsdi_status = mmgsdi_session_otasp_ms_key_req( cdma.session_id,
                                                 key_req_randseed,
                                                 ms_key_req_ptr->a_key_p_rev,
                                                 key_req_param_p,
                                                 key_req_param_g,
                                                 otasp_mmgsdi_response_cb,
                                                 0 );
       if (mmgsdi_status != MMGSDI_SUCCESS)
       {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Bad status from MMGSDI queuing %d",
          mmgsdi_status);
        otasp_int.ms_key_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
        send_ms_key_resp_msg();
       }
     }
     #else

     /* The R-UIM card would perform the key exchange operation */
     if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
     {
       uim_cmd_ptr->hdr.command = UIM_MS_KEY_REQ_F;

       /* This random number that is passed to the card is only a seed
          and the quality restrictions on the random number specified by
          IS-683A is now upon the card */

       /* Below loop makes 5 calls to mc_sec_ran_next() to get a 20 byte random seed
          as each call to ran_next returns only a dword */
       pos = 0;
       for (i = 0; i < 5; i++)
       {
         temp_rand = mc_sec_ran_next();
         b_packd(temp_rand,uim_cmd_ptr->ms_key_req.randseed,pos,sizeof(dword)*8);
         pos += (sizeof(dword) * 8);
       }

       uim_cmd_ptr->ms_key_req.a_key_p_rev =  ms_key_req_ptr->a_key_p_rev;
       uim_cmd_ptr->ms_key_req.param_g_len =  ms_key_req_ptr->param_g_len;

       /* The contents of param p and g are in dword format and we would
          like to send data to the card in OTA format resulting in
          unpacking the dword */

       pos = 0;
       for (i = 0; i < (ms_key_req_ptr->param_g_len/sizeof(dword)); i++)
       {
         b_packd(ms_key_req_ptr->param_g[i],
                 uim_cmd_ptr->ms_key_req.param_g, pos, sizeof(dword)*8);
         pos += (sizeof(dword) * 8);
       }

       uim_cmd_ptr->ms_key_req.param_p_len =  ms_key_req_ptr->param_p_len;

       pos = 0;
       for (i = 0; i < (ms_key_req_ptr->param_p_len/sizeof(dword)); i++)
       {
         b_packd(ms_key_req_ptr->param_p[i],
                 uim_cmd_ptr->ms_key_req.param_p, pos, sizeof(dword)*8);
         pos += (sizeof(dword) * 8);
       }

       uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;
       mcc_uim_cmd( uim_cmd_ptr );
       if (uim_cmd_ptr->hdr.status != UIM_RECEIVED_S)
       {
         M1X_MSG( DCP, LEGACY_ERROR,
           "Bad status from UIM %d",
           uim_cmd_ptr->hdr.status);
         otasp_int.ms_key_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
         send_ms_key_resp_msg();
       }
     }
     else
     {
       M1X_MSG( DCP, LEGACY_ERROR,
         "No free buffers on uim_free_q" );
       otasp_int.ms_key_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
         send_ms_key_resp_msg();
     }
     #endif
   }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
   else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */

   {
      /* copy the content of PARAM_G into the otasp structure  */
      copy_size = memscpy(otasp_dh.param_g, sizeof(otasp_dh.param_g),
                          ms_key_req_ptr->param_g, OTASPI_PARAM_G_MAX);

      if( copy_size != OTASPI_PARAM_G_MAX )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          OTASPI_PARAM_G_MAX,
          sizeof(otasp_dh.param_g));
      }
      /* swapping the byte ordering(to Intel) */
      j =  (OTASPI_PARAM_G_MAX/4)  ;
      k = j - 1;
      for (i = 0; i < j/2 ; i++)
      {
         temp = otasp_dh.param_g[k];
         otasp_dh.param_g[k--] = otasp_dh.param_g[i];
         otasp_dh.param_g[i] = temp;
      }

      copy_size = memscpy(otasp_dh.param_p, sizeof(otasp_dh.param_p),
                          ms_key_req_ptr->param_p, OTASPI_PARAM_P);
      if( copy_size != OTASPI_PARAM_P )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          OTASPI_PARAM_P,
          sizeof(otasp_dh.param_p));
      }

        /* copy the content of PARAM_P into the otasp structure  */

      /* swapping the byte ordering(to Intel) */
      j =  (OTASPI_PARAM_P/4) ;
      k = j - 1;
      for (i = 0; i < j/2 ; i++)
      {
         temp = otasp_dh.param_p[k];
         otasp_dh.param_p[k--] = otasp_dh.param_p[i];
         otasp_dh.param_p[i] = temp;
      }

      {
#ifdef FEATURE_1X_CP_MEM_OPT
        if ((dh_cmd_ptr = (dh_cmd_type*) mc_alloc_queue_buf( MC_DH_CMD_Q )) != NULL)
#else /* !FEATURE_1X_CP_MEM_OPT */
        if ((dh_cmd_ptr = (dh_cmd_type*) q_get( &dh_cmd_free_q )) != NULL)
#endif /* FEATURE_1X_CP_MEM_OPT */
        {
          dh_cmd_ptr->hdr.command = DH_EXP_F;

          /* function call to get a random number from DH */
          random_number = get_rand_from_dh() ;
          offset = 0;

          /* copy into the otasp structure in dword format with lease
          ** significant dword in 0th element*/

          for (i = DH_RAND/4 ; i > 0; i--)
          {
             otasp_dh.private_key[i-1] = b_unpackd(random_number, offset, 32);
             offset+=32;
          }

          /* all the parameters need to be in the reverse byte ordering for
          ** the apwrx routine in DH  */

          /* copy the dword array of prime into the command */
          copy_size = memscpy(dh_cmd_ptr->exp.dh_prime, sizeof(dh_cmd_ptr->exp.dh_prime),
                              otasp_dh.param_p, OTASPI_PARAM_P);
          if( copy_size != OTASPI_PARAM_P )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
              copy_size,
              OTASPI_PARAM_P,
              sizeof(dh_cmd_ptr->exp.dh_prime));
          }

          /* Zero the 64 bytes of base in the command */
          memset(dh_cmd_ptr->exp.dh_base, 0, DH_BASE);

          copy_size = memscpy(dh_cmd_ptr->exp.dh_base, sizeof(dh_cmd_ptr->exp.dh_base),
                              otasp_dh.param_g, OTASPI_PARAM_G_MAX);
          if( copy_size != OTASPI_PARAM_G_MAX )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
              copy_size,
              OTASPI_PARAM_G_MAX,
              sizeof(dh_cmd_ptr->exp.dh_base));
          }

          /* Zero the 64 bytes of exponent in the command */
          memset(dh_cmd_ptr->exp.dh_exponent, 0, DH_EXPONENT);

          /* copy the content of private key in otasp_dh struct into the cmd */
          copy_size = memscpy(dh_cmd_ptr->exp.dh_exponent, sizeof(dh_cmd_ptr->exp.dh_exponent),
                              otasp_dh.private_key , DH_RAND);
          if( copy_size != DH_RAND )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
              copy_size,
              DH_RAND,
              sizeof(dh_cmd_ptr->exp.dh_exponent));
          }

          /* if we are waiting for a report from DH, which means that the prev
          ** exponentiation is not complete, we have to abort the current one
          ** and then send the new command to DH  */

          if (otasp_dh.state == OTASP_WAIT_FOR_MS_RESULT)
          {
             (void) rex_set_sigs( DH_TCB_PTR, DH_ABORT_EXP_SIG);
          }

          dh_cmd_ptr->exp.rpt_function = mcc_queue_dh_rpt;

          dh_cmd_ptr->exp.prime_bits = (OTASPI_PARAM_P * 8);

          mcc_dh_cmd( dh_cmd_ptr );

          /* change the state of the dh to waiting for MS_RESULT */
          otasp_dh.state = OTASP_WAIT_FOR_MS_RESULT ;
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "No free buffers on dh_free_q" );
          otasp_int.ms_key_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
          send_ms_key_resp_msg();
        }
      }
   }


}  /* end ms_key_req_message */ //lint !e715 ms_key_req_ptr not referenced

#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )

/*===========================================================================
FUNCTION OTASP_PROCESS_DH_REPORT

DESCRIPTION
   This function processes the report received from DH task and send the
   response message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void otasp_process_dh_report
(
   dh_rpt_type*  otasp_dh_rpt
   /* Pointer to Dh report type */
)
{
   size_t copy_size;
  /* Return value of memscpy function */
   auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */

   byte i;
    /* index into the array */
   byte j;

   switch(otasp_dh_rpt->rpt_type) {

      case DH_EXP_R:

         M1X_MSG( DCP, LEGACY_MED,
           " Received a DH Exponentiation report ");
         /*check for the state and send the appropriate response message */
         if  (otasp_dh.state == OTASP_WAIT_FOR_MS_RESULT)
         {
            M1X_MSG( DCP, LEGACY_MED,
              " Received a DH Exp report for MS_RESULT");

            otasp_dh.state = OTASP_WAIT_FOR_KEY_GEN_REQ;
            /* change the state to indicate that you are waiting for
            ** Key Generation Request Message */

            /* The result returned is always the size of the prime and
            ** the MS_RESULT is stored in the otasp data structure in
               the reverse order as this data is sent over the air */

            j = (OTASPI_PARAM_P/sizeof(dword)) - 1;
            for (i = 0; i < OTASPI_PARAM_P/sizeof(dword) ; i++)
            {
                otasp_dh.ms_result[i] = otasp_dh_rpt->rpt.exp_result[j--];
            }

            otasp_int.ms_key_rsp.result_code = OTASP_BLK_ACCEPT ;
            send_ms_key_resp_msg();
         }

         else if (otasp_dh.state == OTASP_WAIT_FOR_A_KEY)
         {
            M1X_MSG( DCP, LEGACY_MED,
              " Received a DH Exp report for A_KEY");
            otasp_dh.state = OTASP_WAIT_FOR_DH;
            /* change the state to indicate that you are back to init state */

            qw_set(otasp_dh.a_key, otasp_dh_rpt->rpt.exp_result[1],
                        otasp_dh_rpt->rpt.exp_result[0]);


            /* send a command to auth to update the a_key_temp */
            /* do not wait for a report */
            if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
            {
              auth_cmd_ptr->hdr.command = AUTH_UPDATE_A_KEY_TEMP_F;

              qw_equ(auth_cmd_ptr->temp.a_key, otasp_dh.a_key);

              /* We do not expect a report back from AUTH Task  */
              auth_cmd_ptr->temp.rpt_function = NULL;

              mcc_auth_cmd( auth_cmd_ptr );

              if ( auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S )
              {
                 /* Auth has received the command and so
                 ** fill in the necessary fields and send the response */
                 otasp_int.key_gen_rsp.result_code = OTASP_BLK_ACCEPT ;
                 otasp_int.key_gen_rsp.ms_result_len =  OTASPI_PARAM_P;
                 copy_size = memscpy(otasp_int.key_gen_rsp.ms_result,
                                     sizeof(otasp_int.key_gen_rsp.ms_result),
                                     otasp_dh.ms_result, OTASPI_PARAM_P);

                 if( copy_size != OTASPI_PARAM_P )
                 {
                   M1X_MSG( DCP, LEGACY_ERROR,
                     "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                     copy_size,
                     OTASPI_PARAM_P,
                     sizeof(otasp_int.key_gen_rsp.ms_result));
                 }
                 send_key_gen_resp_msg();

                 /* Send OTASP status to CM */
                 otasp_report_status_to_cm(CM_OTASP_STATUS_AKEY_EXCHANGED);
              }
              else /* Status returned from command was an error condition */
              {
                 /* The protocol would fail if this operation fails, so
                 ** reject the message */
                 M1X_MSG( DCP, LEGACY_ERROR,
                   "Bad status from AUTH %d",
                   auth_cmd_ptr->hdr.status);
                 otasp_int.key_gen_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
                 send_key_gen_resp_msg();
              }
            }
            else
            {
              ERR_FATAL( "No free buffers on auth_free_q", 0, 0, 0 );
            }

         }
         else
         {
            /* we got the report in the wrong state, so ignore it*/
            M1X_MSG( DCP, LEGACY_ERROR,
              "Report %d received in wrong state from DH!",
              otasp_dh_rpt->rpt_type );
         }
         break;

      default :
         /* we got the wrong report */
         M1X_MSG( DCP, LEGACY_ERROR,
           "Wrong report %d from DH!",
           otasp_dh_rpt->rpt_type );
         break;
   }

} /* otasp_process_dh_report */

#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

/*===========================================================================

FUNCTION SEND_MS_KEY_RESP_MSG

DESCRIPTION
   This function send the MS Key Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_ms_key_resp_msg(void)

{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the MS Key Response Message */

  otasp_int.ms_key_rsp.msg_type = OTASP_MS_KEY_RSP_MSG;

  status = otaspx_int_to_ext( &len, &otasp_int, chari );
  if (status == OTASPX_DONE_S) /*lint !e641 */
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
} /* send_ms_key_resp_msg */

#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION SEND_COMMIT_RESP_MSG

DESCRIPTION
   This function send the Commit Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_commit_resp_msg(void)

{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the Commit Response Message */

  otasp_int.commit_rsp.msg_type = OTASP_COMMIT_RSP_MSG;

  status = otaspx_int_to_ext( &len, &otasp_int, chari );
  if (status == OTASPX_DONE_S) /*lint !e641 */
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
} /* send_commit_resp_msg */
#endif /* FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION PROCESS_KEY_GEN_REQ

DESCRIPTION
  This function processes an OTASP Key Generation Request Message,
  formats the appropriate Key Generation Response Message, then
  calls the routine to send it. The MS Key Generation Request
  message should have been received first in order to do the
  normal processing for this message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void process_key_gen_req
(
  otaspi_key_gen_req_msg_type* key_gen_req_ptr
    /* Pointer to MS Key Generation Request Message */
)
{
  size_t copy_size;
  /* Return value of memscpy function */
#if defined (FEATURE_UIM_RUIM)
  byte i;
#endif

#if defined (FEATURE_UIM_RUIM)
  word pos;
#endif

#if defined(FEATURE_CSIM)
  byte bs_result[UIM_PARAM_P];
  mmgsdi_data_type key_gen;
  mmgsdi_return_enum_type mmgsdi_status;
#elif defined (FEATURE_UIM_RUIM)
  uim_cmd_type *uim_cmd_ptr;
#endif /* FEATURE_CSIM */


   dh_cmd_type *dh_cmd_ptr;
    /* Pointer to dh command */
   byte j,k; /* index into the array */
   dword temp; /* temporary storage for dword */


 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
   /* Determine if the R-UIM is available for this NAM */
   if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
   {
     #ifdef FEATURE_CSIM
     (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_SIG );
     pos = 0;
     for (i = 0; i < (key_gen_req_ptr->bs_result_len/sizeof(dword)); i++)
     {
       b_packd(key_gen_req_ptr->bs_result[i],
               bs_result, pos, sizeof(dword)*8);
       pos += (sizeof(dword) * 8);
     }
     key_gen.data_len = key_gen_req_ptr->bs_result_len;
     key_gen.data_ptr = bs_result;

     #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS  
     /* Don't send MMGSDI request if Sim Busy event has been received */
     if (mc_get_sim_busy_status())
     {
       M1X_MSG( DCP, LEGACY_HIGH, "SIM Busy, Otasp key gen request failed" );
       otasp_int.key_gen_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
       send_key_gen_resp_msg();
     }
     else
     #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
     {

       mmgsdi_status = mmgsdi_session_otasp_key_gen_req( cdma.session_id,
                                                  key_gen,
                                                  otasp_mmgsdi_response_cb,
                                                  0 );
       if (mmgsdi_status != MMGSDI_SUCCESS)
       {
         M1X_MSG( DCP, LEGACY_ERROR,
           "No free buffers on uim_free_q" );
         otasp_int.key_gen_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
         send_key_gen_resp_msg();
       }
     }
     #else
      /* The R-UIM card would perform the key exchange operation */
      if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
      {
         uim_cmd_ptr->hdr.command = UIM_KEY_GEN_REQ_F;

         uim_cmd_ptr->key_gen.bs_result_len = key_gen_req_ptr->bs_result_len;

         /* The contents of bs result is in dword format and we would
            like to send data to the card in OTA format resulting in
            unpacking of the dword */

         pos = 0;
         for (i = 0; i < (key_gen_req_ptr->bs_result_len/sizeof(dword)); i++)
         {
           b_packd(key_gen_req_ptr->bs_result[i],
                    uim_cmd_ptr->key_gen.bs_result, pos, sizeof(dword)*8);
           pos += (sizeof(dword) * 8);
         }

         uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

         uim_cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;
         uim_cmd_ptr->hdr.cmd_hdr.sigs = NULL;
         uim_cmd_ptr->hdr.options = UIM_OPTION_ALWAYS_RPT;

         mcc_uim_cmd( uim_cmd_ptr );

         if (uim_cmd_ptr->hdr.status != UIM_RECEIVED_S)
         {
             M1X_MSG( DCP, LEGACY_ERROR,
               "Bad status from UIM %d",
               uim_cmd_ptr->hdr.status);
             otasp_int.key_gen_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
             send_key_gen_resp_msg();
         }
      }
      else
      {
         M1X_MSG( DCP, LEGACY_ERROR,
           "No free buffers on uim_free_q" );
         otasp_int.key_gen_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
         send_key_gen_resp_msg();
      }
      #endif
   }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
   else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */

   {
      /* this means that the ms key request was not successfully processed
         or that ms key request was not received */

      if (otasp_dh.state != OTASP_WAIT_FOR_KEY_GEN_REQ)
      {
         /* Message not expected in this mode  */
         M1X_MSG( DCP, LEGACY_ERROR,
           "Message not expected in this mode" );
         otasp_int.key_gen_rsp.result_code = OTASP_BLK_REJ_MODE ;
         send_key_gen_resp_msg();
      }
      else
      {
#ifdef FEATURE_1X_CP_MEM_OPT
         if ((dh_cmd_ptr = (dh_cmd_type*) mc_alloc_queue_buf( MC_DH_CMD_Q )) != NULL)
#else /* !FEATURE_1X_CP_MEM_OPT */
         if ((dh_cmd_ptr = (dh_cmd_type*) q_get( &dh_cmd_free_q )) != NULL)
#endif /* FEATURE_1X_CP_MEM_OPT */
         {
            dh_cmd_ptr->hdr.command = DH_EXP_F;

            copy_size = memscpy(dh_cmd_ptr->exp.dh_prime, sizeof(dh_cmd_ptr->exp.dh_prime),
                                otasp_dh.param_p, OTASPI_PARAM_P);

            if( copy_size != OTASPI_PARAM_P )
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                copy_size,
                OTASPI_PARAM_P,
                sizeof(dh_cmd_ptr->exp.dh_prime));
            }

            /* assumed that the result is 512 bits
            ** send the BS_RESULT as the Parameter G */
            copy_size = memscpy(dh_cmd_ptr->exp.dh_base, sizeof(dh_cmd_ptr->exp.dh_base),
                                key_gen_req_ptr->bs_result, DH_BASE);

            if( copy_size != DH_BASE )
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                copy_size,
                DH_BASE,
                sizeof(dh_cmd_ptr->exp.dh_base));
            }
            /* swapping the byte ordering(to Intel) */

            j =  (DH_BASE/4)  ;
            k = j - 1;
            for (i = 0; i < j/2 ; i++)
            {
               temp = dh_cmd_ptr->exp.dh_base[k];
               dh_cmd_ptr->exp.dh_base[k--] = dh_cmd_ptr->exp.dh_base[i];
               dh_cmd_ptr->exp.dh_base[i] = temp;
            }

            /* Zero the 64 bytes of exponent in the command */
            memset(dh_cmd_ptr->exp.dh_exponent, 0, DH_EXPONENT);

            copy_size = memscpy(dh_cmd_ptr->exp.dh_exponent, sizeof(dh_cmd_ptr->exp.dh_exponent),
                                otasp_dh.private_key, DH_RAND);
            if( copy_size != DH_RAND )
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                copy_size,
                DH_RAND,
                sizeof(dh_cmd_ptr->exp.dh_exponent));
            }

            dh_cmd_ptr->exp.rpt_function = mcc_queue_dh_rpt;

            dh_cmd_ptr->exp.prime_bits = (OTASPI_PARAM_P * 8);

            /* if we are waiting for a report from DH, which means that the prev
            ** exponentiation is not complete, we have to abort the current one
            ** and then send the new command to DH  */

            if (otasp_dh.state == OTASP_WAIT_FOR_A_KEY)
            {
               (void) rex_set_sigs( DH_TCB_PTR, DH_ABORT_EXP_SIG);
            }
            mcc_dh_cmd( dh_cmd_ptr );

            /* change the state of the dh to waiting for MS_RESULT */
            otasp_dh.state = OTASP_WAIT_FOR_A_KEY ;

         }
         else
         {
            M1X_MSG( DCP, LEGACY_ERROR,
              "No free buffers on dh_cmd_free_q" );
            otasp_int.key_gen_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
            send_key_gen_resp_msg();
         }
      }
   }


}  /* process_key_gen_req  */  //lint !e715 ms_key_req_ptr not referenced

/*===========================================================================

FUNCTION SEND_KEY_GEN_RESP_MSG

DESCRIPTION
   This function sends the Key Genration Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_key_gen_resp_msg(void)
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the Key Generation Response Message */

  otasp_int.key_gen_rsp.msg_type = OTASP_KEY_GEN_RSP_MSG;

  status = otaspx_int_to_ext( &len, &otasp_int, chari );
  if (status == OTASPX_DONE_S) /*lint !e641 */
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
}  /* send_key_gen_resp_msg */

/*===========================================================================

FUNCTION PROCESS_REAUTH_REQ

DESCRIPTION
  This function processes an OTASP Re-Authentication Request Message,
  formats the appropriate Re-authentication Response Message, then
  calls the routine to send it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void process_reauth_req
(
  otaspi_reauth_req_msg_type* reauth_req_ptr
    /* Pointer to Re-Authentication Request Message */
)
{


  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
#ifdef FEATURE_UIM_RUIM
  byte i;
    /* Index variable */
#endif /* FEATURE_UIM_RUIM */
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  otasp_reauth_in_progress = TRUE;

  reauth_req_rand = reauth_req_ptr->rand;

  if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
  {
#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    /* Determine if the R-UIM is available for this NAM */
    if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    {
      auth_cmd_ptr->hdr.command = AUTH_SIGNATURE_F;

      auth_cmd_ptr->sig.rand_type = RUIM_GLOBAL_RAND_CHAL;

      auth_cmd_ptr->sig.rand_chal = reauth_req_ptr->rand;


      if (cdma.call_orig

          && (cdma.otasp_call != NETWORK_INIT_SRV_PROV)

         )
      {
          /* user initiated call, send the dialed digits */
          /* number of bits in the digits */
          auth_cmd_ptr->sig.dig_len =
              (( cdma.dial.num_digits < (RUIM_RUN_CAVE_DIGITS * 2)) ?
                  cdma.dial.num_digits : (RUIM_RUN_CAVE_DIGITS * 2)) * 4 ;   /*lint !e734*/

          /* The digits in the cdma array is ASCII */

          auth_calculate_digits( cdma.dial.digits,
                         cdma.dial.num_digits, cdma.digit_mode,
                         auth_cmd_ptr->sig.digits);
      }
      else
      {
         /* network initiated call, send dialled digits as zero */
         auth_cmd_ptr->sig.dig_len = 0;
         for (i = 0; i < RUIM_RUN_CAVE_DIGITS ; i++)
         {
            auth_cmd_ptr->sig.digits[i] = 0;
         }
      }


      auth_cmd_ptr->sig.process_control = RUIM_DEFLT_PROC_CNTRL |
                                        RUIM_COMMIT_NEW_SSD | RUIM_SAVE_REGS_ON;
      auth_cmd_ptr->sig.esn = cdma.esn;
      auth_cmd_ptr->sig.rpt_function = mcc_queue_auth_rpt;

    }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
    {
      auth_cmd_ptr->hdr.command = AUTH_SIGNATURE_F;
      auth_cmd_ptr->sig.rand_chal = reauth_req_ptr->rand;

      /* for a mobile originated call use the saved auth_data in cdma structure
      ** else use the IMSI_S as the input for AUTHENTICATION signature */

      /* If during a mobile originated call, network initiates a OTAPA session
      ** then reauthentication will be treated as the mobile terminated call
      */
      if (cdma.call_orig

          && (cdma.otasp_call != NETWORK_INIT_SRV_PROV)

         )
      {
        auth_cmd_ptr->sig.auth_data = cdma.auth_data;
      }
      else
      {
        auth_cmd_ptr->sig.auth_data = mcc_get_auth_imsi_s1();
      }
      auth_cmd_ptr->sig.save_reg = TRUE;
      auth_cmd_ptr->sig.rpt_function = mcc_queue_auth_rpt;
      M1X_MSG( DCP, LEGACY_MED,
        "REAUTH: rand %lx auth_data:%lx, save_reg:%d",
        auth_cmd_ptr->sig.rand_chal,
        auth_cmd_ptr->sig.auth_data,
        auth_cmd_ptr->sig.save_reg);

    }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

    mcc_auth_cmd( auth_cmd_ptr );

    if (auth_cmd_ptr->hdr.status != AUTH_RECEIVED_S)
    {
      /* -----------------------------------------------
      ** We have a serious problem and can not calculate
      ** authr, so error fatal.
      ** ----------------------------------------------- */
      #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
      if (mc_get_auth_err_status())
      {
        /* Don't throw ERR_FATAL as SIM Busy is an expected event. */
        M1X_MSG( DCP, LEGACY_ERROR,
                "process_reauth_req - Bad status from AUTH %d", 
                (int) auth_cmd_ptr->hdr.status );
      }
      else
      #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */ 
      {
        ERR_FATAL( "Bad status from AUTH %d", (int) auth_cmd_ptr->hdr.status,0,0);
      }
      
    }
  }
  else
  {
    ERR_FATAL( "No free buffers on auth_free_q", 0, 0, 0 );
  }


}  /* end reauth_req_message */  //lint !e715 reauth_req_ptr not referenced.


/*===========================================================================
FUNCTION OTASP_PROCESS_AUTH_REPORT

DESCRIPTION
   This function processes the report received from Auth task while
   reauthentication is in progress or while auth_otapa is in progress

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void otasp_process_auth_report
(
   auth_rpt_type*  otasp_auth_rpt
   /* Pointer to auth report type */
)
{
   auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */

#ifdef FEATURE_UIM_RUIM
   boolean send_reauth_rsp = FALSE;
#endif /* FEATURE_UIM_RUIM */

   switch(otasp_auth_rpt->rpt_type)
   {
      case AUTH_SIGNATURE_R:

         /* recalculate the message encryption parameters and private long
         ** code mask during reauthentication operation, only if they were
         ** not active before the start of the reauthentication operation */

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
         /* Determine if the R-UIM is available for this NAM */
         if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
         {
           cdma.authr = 0;
           send_reauth_rsp = TRUE;

           if (otasp_auth_rpt->rpt_status == RUIM_PASS)
           {
               cdma.authr = otasp_auth_rpt->rpt.auth;

               if (!((cdma.encrypt_mode) || cdma.long_code.private_lcm_active))
               {
                 if ((auth_cmd_ptr = (auth_cmd_type*) q_get(&auth_free_q))!= NULL)
                 {
                   M1X_MSG( DCP, LEGACY_MED,
                     "Start key generation");
                   auth_cmd_ptr->hdr.command = AUTH_GENERATE_KEY_F;
                   auth_cmd_ptr->key.vpm_first_octet = RUIM_FIRST_OCTET_OF_VPM;
                   auth_cmd_ptr->key.vpm_last_octet =  RUIM_LAST_OCTET_OF_VPM;
                   auth_cmd_ptr->key.rpt_function = mcc_queue_auth_rpt;
                   mcc_auth_cmd( auth_cmd_ptr );

                   if (auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S)
                   {
                      send_reauth_rsp = FALSE;
                   }
                   else
                   {
                      M1X_MSG( DCP, LEGACY_ERROR,
                        "Bad status from AUTH %d",
                        auth_cmd_ptr->hdr.status);
                   }
                 }
                 else
                 {
                   M1X_MSG( DCP, LEGACY_ERROR,
                     "No free buffers on auth_free_q" );
                 }
               }
           }
           #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
           else
           {
               /* Set error reason for auth command failure to sim busy */
               if (mc_get_sim_busy_status())
               {
                 mc_set_auth_err_status(TRUE);
               }
           }
           #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */

           if (send_reauth_rsp)
           {
               /* We may be sending the response now bcos RUIM returned a bad
                  status */
               /* We do not have to generate the keys and we are done with the
                  process of reauthentication */

               otasp_reauth_in_progress = FALSE;

               /* Build the Re-Authentication Response Message */
               build_reauth_resp_msg();

               /* send the reauthentication response message */
               send_reauth_resp_msg();
           }
         }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
         else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
         {
           cdma.authr =  otasp_auth_rpt->rpt.auth;

           if (!((cdma.encrypt_mode) || cdma.long_code.private_lcm_active))
           {
              if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
              {
                 auth_cmd_ptr->hdr.command = AUTH_GENERATE_KEY_F;
                 /* We would always like to have voice privacy */
                 auth_cmd_ptr->key.do_vpm = TRUE;

                 auth_cmd_ptr->key.rpt_function = mcc_queue_auth_rpt;

                 mcc_auth_cmd( auth_cmd_ptr );

                 if (auth_cmd_ptr->hdr.status != AUTH_RECEIVED_S)
                 {
                    M1X_MSG( DCP, LEGACY_ERROR,
                      "Bad status from AUTH %d",
                      auth_cmd_ptr->hdr.status);
                 }
              }
              else
              {
                 M1X_MSG( DCP, LEGACY_ERROR,
                   "No free buffers on auth_free_q" );
              }
           }
           else
           {
              /* we do not have to generate the keys and we are done with the
              ** process of reauthentication */
              otasp_reauth_in_progress = FALSE;

              /* Build the Re-Authentication Response Message */
              build_reauth_resp_msg();

              /* send the reauthentication response message */
              send_reauth_resp_msg();

           }
         }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

         break;

      case AUTH_GENERATE_KEY_R:

         if (otasp_reauth_in_progress)
         {
            /* We do not check to see the status because, Whenever a transition is
               requested to PLC, we would reject it, since the plcm_available would
               have been set to FALSE in auth when R-UIM failed. */

            /* Build the Re-Authentication Response Message */
            build_reauth_resp_msg();

            /* new keys have been generated and reauthentication is  */
            /* complete, send the reauth response message          */
            send_reauth_resp_msg();

            otasp_reauth_in_progress = FALSE;
         }

         break;


      case AUTH_SIGNATURE_SPASM_R:
         if (otasp_auth_otapa_in_progress)
         {
           otasp_auth_otapa_in_progress = FALSE;
           otasp_auth_otapa = otasp_auth_rpt->rpt.auth;
           otasp_auth_otapa_ready = TRUE;
           M1X_MSG( DCP, LEGACY_MED,
             "Get auth_otapa: %lx",
             otasp_auth_otapa);
         }

         break;


      default:
         /* we got the wrong report */
         M1X_MSG( DCP, LEGACY_ERROR,
           "Wrong report %d from AUTH!",
           otasp_auth_rpt->rpt_type );
         break;
   } //lint !e788

} /* otasp_process_auth_report */

/*===========================================================================

FUNCTION BUILD_REAUTH_RESP_MSG

DESCRIPTION
   This function builds the Re-Authentication Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void build_reauth_resp_msg (void )
{

  otasp_int.reauth_rsp.authr = cdma.authr;
  otasp_int.reauth_rsp.randc =
               (byte) ((reauth_req_rand & 0xFF000000L) >> 24); /*lint !e737 */

  otasp_int.reauth_rsp.count =  auth_get_count();

  /* for a mobile originated call use the saved auth_data in cdma
  ** structure else use the IMSI_S as the input for AUTHENTICATION
  ** signature */

  /* If during a mobile originated call, network initiates a OTAPA session
  ** then reauthentication will be treated as the mobile terminated call
  */
  if ((cdma.call_orig)

       && (cdma.otasp_call != NETWORK_INIT_SRV_PROV)

     )
  {
     otasp_int.reauth_rsp.auth_data = cdma.auth_data;
  }
  else
  {
     otasp_int.reauth_rsp.auth_data = mcc_get_auth_imsi_s1();
  }
  M1X_MSG( DCP, LEGACY_MED,
    "REAUTH_R: rand:%lx,auth_sig:%lx,auth_data:%lx",
    otasp_int.reauth_rsp.randc,
    otasp_int.reauth_rsp.authr,
    otasp_int.reauth_rsp.auth_data);
} /* build_reauth_resp_msg */

/*===========================================================================

FUNCTION SEND_REAUTH_RESP_MSG

DESCRIPTION
   This function sends the Re-Authentication Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_reauth_resp_msg(void)
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 *//* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the Re-Authentication Response Message */

  otasp_int.reauth_rsp.msg_type = OTASP_REAUTH_RSP_MSG;

  status = otaspx_int_to_ext( &len, &otasp_int, chari );
  if (status == OTASPX_DONE_S) /*lint !e641 */
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
}  /* send_reauth_resp_msg */


#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
/*===========================================================================

FUNCTION OTASP_PROCESS_COMMIT

DESCRIPTION
  This function processes an OTASP Commit, after NV has finished processing
  the NV_OTASP_COMMIT_F command.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void otasp_process_commit (void)
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  nv_stat_enum_type     nam_commit_status;     /* Status of request */

   auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */

  boolean send_response = FALSE;
#if (defined(FEATURE_MDN_BASED_NAI) && defined(FEATURE_OTASP_UPDATE_MDN_NAI))
  boolean mdn_commit_status = FALSE;
#endif /* FEATURE_MDN_BASED_NAI && FEATURE_OTASP_UPDATE_MDN_NAI */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If NV signals commit complete when no commit is in progress, ignore it */
  if (!otasp_commit_in_progress)
  {
    return;
  }

  /* we might need to commit the A-Key also. Save the status of NV buffer
     as a result of commiting the NAM parameters if NAM parameters have
     been committed  */
  nam_commit_status = otasp_nv_buf.status;

  if (otasp_commit_akey_pending)
  {


     if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
     {

        /* Fill in necessary fields of command block */

        auth_cmd_ptr->hdr.command = AUTH_COMMIT_KEYS_F;
        auth_cmd_ptr->hdr.cmd_hdr.task_ptr = MC_TCB_PTR;
        auth_cmd_ptr->hdr.cmd_hdr.sigs = MCC_OTASP_COMMIT_SIG;

        auth_cmd_ptr->commit_keys.nam_index = cdma.curr_nam;
        auth_cmd_ptr->commit_keys.rpt_function = mcc_queue_auth_rpt;
        otasp_commit_in_progress = TRUE;
        otasp_committed_both_flag = TRUE;

        auth_cmd( auth_cmd_ptr );
        if (auth_cmd_ptr->hdr.status != AUTH_RECEIVED_S)
        {
          #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
          if (mc_get_auth_err_status())
          {
            /* Don't throw ERR_FATAL as SIM Busy is an expected event. */
            M1X_MSG( DCP, LEGACY_ERROR,
                     "otasp_process_commit - Bad status from AUTH %d",
                      (int) auth_cmd_ptr->hdr.status );
          }
          else
          #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
          {
            ERR_FATAL( "Bad status from AUTH %d", (int) auth_cmd_ptr->hdr.status,0,0);
          }
       }
     }
     else
     {
         ERR_FATAL( "No free buffers on auth_free_q", 0, 0, 0 );
     }

     otasp_commit_akey_pending = FALSE;
       /* as we have sent the command to commit */
  }
  else
  {
     otasp_int.commit_rsp.msg_type = OTASP_COMMIT_RSP_MSG;
     otasp_commit_in_progress = FALSE;
     send_response = TRUE;

#if (defined(FEATURE_MDN_BASED_NAI) && defined(FEATURE_OTASP_UPDATE_MDN_NAI))
     /* Update MDN based NAIs if MDN has been committed successfully */
     if ( (nam_commit_status == NV_DONE_S) && nv_commit.commit_mob_dir)
     {
       mdn_commit_status = TRUE;
     }
#endif /* FEATURE_MDN_BASED_NAI && FEATURE_OTASP_UPDATE_MDN_NAI */

     if (otasp_committed_both_flag)
     {
        if ((nam_commit_status == NV_DONE_S)

           && (otasp_akey_commit_result)

           )
        {
          otasp_reload_nam_params();
          otasp_int.commit_rsp.result_code = OTASP_BLK_ACCEPT;

          M1X_MSG( DCP, LEGACY_MED,
            "A-KEY committed");

          otasp_akey_commit_result = FALSE;
          a_key_temp_valid = FALSE;
          /* reset the flags */
          new_ssd_valid = FALSE;

        }
        else
        {
          otasp_int.commit_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
        }
        otasp_committed_both_flag = FALSE; /* reset the flag */
     }
     else
     {
        if (nv_commit.commit_mob_dir || nv_commit.commit_ftc_cdma ||
               nv_commit.commit_ftc_amps || nv_commit.commit_roam_list
            || nv_commit.commit_spc || nv_commit.commit_imsi_t

            || nv_commit.commit_nam_lock

            || commit_ehrpd_imsi
          )
        {
           if (nam_commit_status == NV_DONE_S)
           {
               otasp_reload_nam_params();
               commit_ehrpd_imsi = FALSE;
               otasp_int.commit_rsp.result_code = OTASP_BLK_ACCEPT;
           }
           else
           {

              otasp_int.commit_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
           }
        }

        else if (otasp_akey_commit_result)
        {
           M1X_MSG( DCP, LEGACY_MED,
             "A-KEY committed");
           otasp_successful_commit  = TRUE;
           otasp_akey_commit_result = FALSE;
           a_key_temp_valid = FALSE;
           otasp_int.commit_rsp.result_code = OTASP_BLK_ACCEPT;
           /* reset the flags */
           new_ssd_valid = FALSE;
        }

        else
        {
           otasp_int.commit_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
        }
     }
     M1X_MSG( DCP, LEGACY_MED,
       "OTASP commit complete");
  }

  if (send_response)
  {
      /* Send OTASP status to CM */
      if(otasp_int.commit_rsp.result_code == OTASP_BLK_ACCEPT)
      {
        otasp_report_status_to_cm(CM_OTASP_STATUS_COMMITTED);
        otasp_report_commit_status_to_cm(FALSE);
      }

      /* Translate and send the Commit Response Message */
      status = otaspx_int_to_ext( &len, &otasp_int, chari );

      if (status == OTASPX_DONE_S) /*lint !e641 */
      {
        send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
      }
      else
      {
        ERR_FATAL( "Failed OTASP message translation",0,0,0);
      }
  }
#if (defined(FEATURE_MDN_BASED_NAI) && defined(FEATURE_OTASP_UPDATE_MDN_NAI))
  /* Update MDN based NAIs if MDN has been committed successfully */
  if (mdn_commit_status)
  {
    otasp_update_mdn_based_nai();
    mdn_commit_status = FALSE;
  }
#endif /* FEATURE_MDN_BASED_NAI && FEATURE_OTASP_UPDATE_MDN_NAI */
} /* otasp_process_commit() */
#endif /* !FEATURE_UIM_RUIM FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
/*===========================================================================

FUNCTION CAP_RSP_RESET_FEATURE

DESCRIPTION
  This function reset the num feature field in the capability response
  message.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void  cap_rsp_reset_feature(

  otaspi_prot_cap_rsp_msg_type *cap_rsp_ptr
      /* Pointer to the cap rsp message
      */
)
{
  if( cap_rsp_ptr == NULL )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "CAP RSP MSG: NULL ptr" );

    return;
  }

  cap_rsp_ptr->num_features = 0;

  return;
}/* cap_rsp_reset_feature */

/*===========================================================================

FUNCTION CAP_RSP_ADD_FEATURE

DESCRIPTION
  This function adds the feature_id in the capability response message.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void  cap_rsp_add_feature(

  otaspi_prot_cap_rsp_msg_type *cap_rsp_ptr,
      /* Pointer to the cap rsp message for update
      */

  byte feature_id,
      /* Feature ID to be added
      */

  byte feature_p_rev
      /* Feature P Revision to be added.
      */
)
{

  /* Return FALSE if the pointer to the capability response message is NULL
  */
  if( cap_rsp_ptr == NULL )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      " CAP RSP MSG: NULL ptr" );

    return;
  }

  /* Return FALSE if the num features in the capability response message
  ** reaches the max number of features supported
  */
  if( cap_rsp_ptr->num_features >= OTASPI_FEATURE_MAX )
  {

    M1X_MSG( DCP, LEGACY_ERROR,
      " CAP RSP MSG: num_features=%d, max=%d",
      cap_rsp_ptr->num_features,
      OTASPI_FEATURE_MAX );

    cap_rsp_ptr->num_features = OTASPI_FEATURE_MAX;

    return;

  }


  /* Store the feature_id and feature_p_rev in capability response message
  */
  cap_rsp_ptr->var[cap_rsp_ptr->num_features].feature_id = feature_id;
  cap_rsp_ptr->var[cap_rsp_ptr->num_features].feature_p_rev = feature_p_rev;

  /* Increase the num of features in the capability response message
  */
  cap_rsp_ptr->num_features++;

  M1X_MSG( DCP, LEGACY_HIGH,
    " CAP RSP MSG: feature_id =%d, p_rev=%d, num_features=%d",
    feature_id,
    feature_p_rev,
    cap_rsp_ptr->num_features);

  return;

} /* cap_rsp_add_feature */
#endif /* !FEATURE_UIM_RUIM  && FEATURE_UIM_RUN_TIME_ENABLE */

/*===========================================================================

FUNCTION PROCESS_PROT_CAP_REQ

DESCRIPTION
  This function processes an OTASP Protocol Capability Request Message,
  formats the appropriate Protocol Capability Response Message, then
  calls the routine to send it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

/*lint -e831*/
void process_prot_cap_req
(
  otaspi_prot_cap_req_msg_type* prot_cap_req_ptr
    /* Pointer to Protocol Capability Request Message */
)
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  byte offset;  /* offset for additional bytes */
  byte num;  /* number of features  */


  word  j;   /* loop variable */
  byte recnum;     /* record number */
  boolean   duprec;   /* duplicated rec flag */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the msg type to OTASP_PROT_CAP_RSP_MSG */
  otasp_int.cap_rsp.msg_type = OTASP_PROT_CAP_RSP_MSG;


  if (prot_cap_req_ptr->otasp_ext_cap_rsp_required)
  {
    /* This is a new format prot_cap_req message
     * We need to format a Extended Protocol Capability Response Message
     */
    otasp_int.cap_rsp.otasp_mob_p_rev = OTASP_P_REV_3; /*lint !e641 */
          /* Support IS-683C */

    /* Set the msg type to OTASP_EXT_PROT_CAP_RSP_MSG */
    otasp_int.cap_rsp.msg_type = OTASP_EXT_PROT_CAP_RSP_MSG;
  }


  otasp_int.cap_rsp.mob_firm_rev = cdma.mob_firm_rev;
  otasp_int.cap_rsp.mob_model = cdma.mob_model;

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    /* Number of features supported by the card */
    otasp_int.cap_rsp.num_features = cdma.ruim_ota_features.num_features;

    if( otasp_int.cap_rsp.num_features > OTASPI_FEATURE_MAX )
    {

      M1X_MSG( DCP, LEGACY_ERROR,
        " CAP RSP MSG: num_features=%d, max=%d",
        otasp_int.cap_rsp.num_features,
        OTASPI_FEATURE_MAX );

      otasp_int.cap_rsp.num_features = OTASPI_FEATURE_MAX;
    }

    /* Fill the Feature-ID and Feature P_REV data */
    for (num = 0; num < otasp_int.cap_rsp.num_features ; num++)
    {
        otasp_int.cap_rsp.var[num].feature_id =
                          cdma.ruim_ota_features.feature_id[num];
        otasp_int.cap_rsp.var[num].feature_p_rev =
                          cdma.ruim_ota_features.feature_p_rev[num];
    }
  }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  {
    cap_rsp_reset_feature( &otasp_int.cap_rsp );

    cap_rsp_add_feature( &otasp_int.cap_rsp,
                         OTASP_NAM_DOWNLOAD,
                         OTASP_NAM_DOWNLOAD_P_REV );

    cap_rsp_add_feature( &otasp_int.cap_rsp,
                         OTASP_NAM_DOWNLOAD,
                         OTASP_NAM_DOWNLOAD_EHRPD_IMSI_P_REV );

    cap_rsp_add_feature( &otasp_int.cap_rsp,
                        OTASP_PROG_LOCK,
                        OTASP_SPL_P_REV );


    cap_rsp_add_feature( &otasp_int.cap_rsp,
                         OTASP_PREF_ROAM,
                         (byte) PRL_SSPR_P_REV_1 );


    cap_rsp_add_feature( &otasp_int.cap_rsp,
                         OTASP_PREF_ROAM,
                         OTASP_PREF_ROAM_P_REV );


    cap_rsp_add_feature( &otasp_int.cap_rsp,
                         OTASP_KEY_EXCHANGE,
                         OTASP_KEY_EXCHANGE_P_REV );



    cap_rsp_add_feature( &otasp_int.cap_rsp,
                         OTASP_OTAPA,
                         OTASP_OTAPA_P_REV );


#if defined(FEATURE_GOBI)
    /* VZW requires OTASP Protocol Capability Response to include ERI support
    ** with FEATURE_ID 196 0xC4 (As mentioned in spec C.S0016, this Feature id
    ** is available for manufacturer specific features.)*/
    #ifdef FEATURE_GOBI_VZW
    #define OTASP_ERI        0xC4
    #define OTASP_ERI_P_REV  0x01
    cap_rsp_add_feature( &otasp_int.cap_rsp,
                         OTASP_ERI,
                         OTASP_ERI_P_REV );
    #endif /* FEATURE_GOBI_VZW */
#endif /* defined(FEATURE_GOBI) */
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */


  if (otasp_int.cap_rsp.msg_type == OTASP_EXT_PROT_CAP_RSP_MSG)
  {
    /* initialize recnum */
    recnum = 0;

    for (num = 0; (num < prot_cap_req_ptr->num_cap_records)
          && (num < OTASPI_CAP_INFO_REC_TYPE_MAX)
          && (recnum < OTASPI_CAP_INFO_REC_TYPE_MAX); num++)
    {
      /* initialize the duplicated rec flag */
      duprec = FALSE;

      /* check for duplicates */
      for (j = 0; j < num; j++)
      {
        if (prot_cap_req_ptr->cap_record_type[j] ==
              prot_cap_req_ptr->cap_record_type[num])
        {
          /* already processed this type, ignore */
          duprec = TRUE;
          M1X_MSG( DCP, LEGACY_ERROR,
            "duprec num %d recnum %d",
            num,
            recnum);
          break;
        }
      }

      if (!duprec)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "proc rec num %d recnum %d",
          num,
          recnum);

        switch (prot_cap_req_ptr->cap_record_type[num])
        {
        case OTASPI_CAP_INFO_OPER_MODE:
          /* copy the cap rec type from the request */
          otasp_int.cap_rsp.cap_rec[recnum].cap_record_type =
              prot_cap_req_ptr->cap_record_type[num]; /*lint !e641 */

          /* Fill in OPER MODE */
          /* The length should be 1 byte */
          otasp_int.cap_rsp.cap_rec[recnum].cap_record_length = 1;

          otasp_int.cap_rsp.cap_rec[recnum].info.op_mode = 0;

          /* CDMA is supported, otherwise we won't be here */
          otasp_int.cap_rsp.cap_rec[recnum].info.op_mode |= OTASP_OP_MODE1;

          #ifdef FEATURE_HDR
          otasp_int.cap_rsp.cap_rec[recnum].info.op_mode |= OTASP_OP_MODE2;
          #endif

          /* increment the recnum */
          recnum++;

          break;

        case OTASPI_CAP_INFO_CDMA_BAND_CLASS:
          {
            /* allocate a temp buffer to get the band class info */
            caii_band_rec_type  band_class_rec;

            /* copy the cap rec type from the request */
            otasp_int.cap_rsp.cap_rec[recnum].cap_record_type =
                prot_cap_req_ptr->cap_record_type[num]; /*lint !e641 */

            /* The length should be 2 byte */
            otasp_int.cap_rsp.cap_rec[recnum].cap_record_length = 2;

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class = 0;

            /* get the band class info */
            mcc_info_band_class(&band_class_rec);

            /* set the appropriate bit in the cap_rec for each band class */
            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[0] ? OTASP_BAND_CLASS_0 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[1] ? OTASP_BAND_CLASS_1 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[2] ? OTASP_BAND_CLASS_2 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[3] ? OTASP_BAND_CLASS_3 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[4] ? OTASP_BAND_CLASS_4 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[5] ? OTASP_BAND_CLASS_5 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[6] ? OTASP_BAND_CLASS_6 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[7] ? OTASP_BAND_CLASS_7 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[8] ? OTASP_BAND_CLASS_8 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[9] ? OTASP_BAND_CLASS_9 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[10] ? OTASP_BAND_CLASS_10 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[11] ? OTASP_BAND_CLASS_11 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[12] ? OTASP_BAND_CLASS_12 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[13] ? OTASP_BAND_CLASS_13 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
              ( band_class_rec.band[14] ? OTASP_BAND_CLASS_14 : 0 );

            otasp_int.cap_rsp.cap_rec[recnum].info.band_class |=
               ( band_class_rec.band[15] ? OTASP_BAND_CLASS_15 : 0);
            /* increment the recnum */
            recnum++;
          }

          break;

        case OTASPI_CAP_INFO_MEID:
          /* We will process MEID Info record
           */
          if (mccap_is_meid_supported())
          {
            /* copy the cap rec type from the request */
            otasp_int.cap_rsp.cap_rec[recnum].cap_record_type =
                prot_cap_req_ptr->cap_record_type[num]; /*lint !e641 */

            /* we have the MEID
             * The length should be 7 bytes
             */
            otasp_int.cap_rsp.cap_rec[recnum].cap_record_length = 7;
            qw_equ(otasp_int.cap_rsp.cap_rec[num].info.meid, cdma.meid);

            /* increment the recnum */
            recnum++;
          }
          else
          {
            /* print an ERROR message and return */
            M1X_MSG( DCP, LEGACY_ERROR,
              "No VALID MEID for EXT CAP RSP MSG");
          }
          break;

        default:
          M1X_MSG( DCP, LEGACY_MED, 
                   "Unsupported CAP REC in Protocol Capability request msg %d",
                   prot_cap_req_ptr->cap_record_type[num] );
          /* copy the cap rec type from the request */
          otasp_int.cap_rsp.cap_rec[recnum].cap_record_type =
              prot_cap_req_ptr->cap_record_type[num]; /*lint !e641 */
          otasp_int.cap_rsp.cap_rec[recnum].cap_record_length = 0;
          recnum++;
          break;

        } /* switch (prot_cap_req_ptr->cap_record_type[num]) */
      } /* if (!duprec) */
    } /* for (num = 0; num < prot_cap_req_ptr->num_cap_records */

    /*  fill in the number of records */
    otasp_int.cap_rsp.num_cap_records = recnum;

  } /* if (otasp_int.cap_rsp.msg_type == OTASP_EXT_PROT_CAP_RSP_MSG) */
  else
  {
    offset = 0;
    otasp_int.cap_rsp.addnl_fields[offset] = 0;

    if (mcc_cellular_band_is_supported())
    {
      otasp_int.cap_rsp.addnl_fields[offset] |= 0x40;
    }
    if (mcc_pcs_band_is_supported())
    {
      otasp_int.cap_rsp.addnl_fields[offset] |= 0x20;
    }

    ++offset; /* offset plus one is the number of octets in addnl field  */
    otasp_int.cap_rsp.add_length = offset;
    /* number of octets in the additional fields */
  }

  /* Translate and send the Protocol Capability Response Message */

  status = otaspx_int_to_ext( &len, &otasp_int, chari );

  if (status == OTASPX_DONE_S) /*lint !e641 */
  {
    send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
    ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }

} /* process_prot_cap_req */  //lint !e715 Unreferenced pointer prot_cap_req_ptr
/*lint +e831*/

#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION SEND_SSPR_CFG_RESP_MSG

DESCRIPTION
   This function sends the SSPR Configuration Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_sspr_cfg_resp_msg(void)
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the SSPR Configuration Response Message */

  ruim_otasp_int.sspr_cfg_rsp.msg_type = OTASP_SSPR_CONFIG_RSP_MSG;
  status = otaspx_int_to_ext( &len, &ruim_otasp_int, chari );

  if (status == OTASPX_DONE_S) /*lint !e641*/
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
}  /* send_sspr_cfg_resp_msg */
#endif /* FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION SSPR_CFG_RSP_POPULATE_PRL_BLK

DESCRIPTION
  This function populate the PRL block upon OTASP SSPR Config Request Message,
  with block ID=1.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void sspr_cfg_rsp_populate_prl_blk
(
  otaspi_sspr_cfg_rsp_msg_type *sspr_cfg_rsp_ptr,
      /* Pointer to the sspr cfg rsp message */

  otaspi_sspr_cfg_req_msg_type *sspr_cfg_req_ptr
      /* Pointer to the sspr cfg req message */
)
{
  nv_roaming_list_type *roaming_list_ptr;

  roaming_list_ptr = &mcc_roaming_list;

  M1X_MSG( DCP, LEGACY_MED,
    "Reporting IS-683 PRL ");

  /* this is not the actual block len, the correct one will
     be computed before translation */
  sspr_cfg_rsp_ptr->block_len = 1;

  if (sspr_cfg_req_ptr->req_offset >= (roaming_list_ptr->size / 8))
  {
    sspr_cfg_rsp_ptr->result_code = OTASP_BLK_REJ_DATA_SIZE;
    sspr_cfg_rsp_ptr->block.blk.sspr_prl.last_segment = 0;
    sspr_cfg_rsp_ptr->block.blk.sspr_prl.segment_offset =
                         sspr_cfg_req_ptr->req_offset;
    sspr_cfg_rsp_ptr->block.blk.sspr_prl.segment_size  = 0;
    sspr_cfg_rsp_ptr->block.blk.sspr_prl.bits_ptr  = NULL;
  }
  else
  {
    sspr_cfg_rsp_ptr->result_code = OTASP_BLK_ACCEPT;

    sspr_cfg_rsp_ptr->block.blk.sspr_prl.bits_ptr =
         (byte*)roaming_list_ptr->roaming_list+sspr_cfg_req_ptr->req_offset;

    if ((uint32) (sspr_cfg_req_ptr->req_offset + sspr_cfg_req_ptr->req_max_size)
                                               >= (roaming_list_ptr->size/8))
    {
       sspr_cfg_rsp_ptr->block.blk.sspr_prl.segment_size =
           (byte)((roaming_list_ptr->size/8)-sspr_cfg_req_ptr->req_offset);
    }
    else
    {
       sspr_cfg_rsp_ptr->block.blk.sspr_prl.segment_size =
            sspr_cfg_req_ptr->req_max_size;
    }

    sspr_cfg_rsp_ptr->block.blk.sspr_prl.segment_offset =
         sspr_cfg_req_ptr->req_offset;

    if ((sspr_cfg_req_ptr->req_offset +
                      sspr_cfg_rsp_ptr->block.blk.sspr_prl.segment_size)
                      == (roaming_list_ptr->size /8))
    {
      sspr_cfg_rsp_ptr->block.blk.sspr_prl.last_segment = 1;
    }
    else
    {
      sspr_cfg_rsp_ptr->block.blk.sspr_prl.last_segment = 0;
    }
  }

} /* sspr_cfg_rsp_populate_prl_blk */

/*===========================================================================

FUNCTION PROCESS_SSPR_CONFIG_REQ

DESCRIPTION
  This function processes an OTASP SSPR Config Request Message,
  formats the appropriate Protocol Configuration Response Message, then
  calls the routine to send it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_sspr_config_req
(
  otaspi_sspr_cfg_req_msg_type* sspr_cfg_req_ptr
    /* Pointer to SSPR Configuration Request Message */
)
{
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

#if defined(FEATURE_CSIM)
  word req_offset;
  byte req_max_size;
  mmgsdi_return_enum_type mmgsdi_status;
#elif defined( FEATURE_UIM_RUIM )
  uim_cmd_type *uim_cmd_ptr;   /* UIM command pointer */
#endif /* FEATURE_CSIM */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    /* Initialize the number of blocks req to and recd from the card */
    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_card = 0;

    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_rsp_card = 0;

    /* Check to see if a message buffer is available */
    if ((otasp_ruim_table.req_index + 1) % OTASP_RUIM_MAX_MSG_BUFS !=
                                   otasp_ruim_table.rsp_index)
    {
       /* Store the message type */
       otasp_ruim_table.msg[otasp_ruim_table.req_index].msg_type =
                OTASP_SSPR_CONFIG_REQ_MSG;

       /* Store the number of blocks requested in the message as only one */
       otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_msg = 1;

       #ifdef FEATURE_CSIM
       if (sspr_cfg_req_ptr->block_id == OTASP_CFG_PREF_ROAM_LIST_BLK)
       {
         req_offset = sspr_cfg_req_ptr->req_offset;
         req_max_size= sspr_cfg_req_ptr->req_max_size;
       }
       else
       {
         req_offset = 0;
         req_max_size = 0;
       }

       #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
       /* Don't send MMGSDI request if Sim Busy event has been received */  
       if (mc_get_sim_busy_status())
       {
         M1X_MSG( DCP, LEGACY_HIGH, 
                  "SIM Busy, Otasp sspr config request failed" );
       }
       else
       #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
       {  
         mmgsdi_status = mmgsdi_session_otasp_sspr_config_req( cdma.session_id,
                                                        sspr_cfg_req_ptr->block_id,
                                                        req_offset,
                                                        req_max_size,
                                                        otasp_mmgsdi_response_cb,
                                                        0 );
         if (mmgsdi_status != MMGSDI_SUCCESS)
         {
           M1X_MSG( DCP, LEGACY_ERROR,
             "Bad Status returned from MMGSDI, mmgsdi_status=%d",
             mmgsdi_status );
         }
         else
         {
           /* Increment the number of blocks sent to the card */
           ++(otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                         num_blocks_req_card);
         }
       }
       #else

       if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
       {
          uim_cmd_ptr->hdr.command = UIM_SSPR_CONFIG_REQ_F;

          uim_cmd_ptr->sspr_config.block_id = sspr_cfg_req_ptr->block_id;

          /* If the Block is a PRL block, then set the offset and size */

          if (sspr_cfg_req_ptr->block_id == OTASP_CFG_PREF_ROAM_LIST_BLK)
          {
             uim_cmd_ptr->sspr_config.req_offset = sspr_cfg_req_ptr->req_offset;

             uim_cmd_ptr->sspr_config.req_max_size= sspr_cfg_req_ptr->req_max_size;
          }
          else
          {
             uim_cmd_ptr->sspr_config.req_offset = 0;

             uim_cmd_ptr->sspr_config.req_max_size= 0;
          }

          uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

          mcc_uim_cmd( uim_cmd_ptr );

          if (uim_cmd_ptr->hdr.status != UIM_RECEIVED_S)
          {
             M1X_MSG( DCP, LEGACY_ERROR,
               "Bad status from UIM %d",
               uim_cmd_ptr->hdr.status);
          }
          else
          {
             /* Increment the number of blocks sent to the card */
             ++(otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                    num_blocks_req_card);
          }
       }
       else
       {
          M1X_MSG( DCP, LEGACY_ERROR,
            "No free buffers on uim_free_q" );

          /* We do not store the id of the rejected block since there is only
             one block in this message */
       }
       #endif
    } /* check for message buffer availability */

    /* If any block has been sent to the card successfully */
    if (otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                        num_blocks_req_card > 0)
    {
        /* consider this message queued and increment the request index */
        otasp_ruim_table.req_index = (otasp_ruim_table.req_index + 1) %
                                    OTASP_RUIM_MAX_MSG_BUFS;
    }
    else
    {
        ruim_otasp_int.sspr_cfg_rsp.block_id = sspr_cfg_req_ptr->block_id;

        ruim_otasp_int.sspr_cfg_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;

        ruim_otasp_int.sspr_cfg_rsp.block_len = 0;

        /* send the SSPR configuration response message */
        send_sspr_cfg_resp_msg();
    }
  }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  {
    otasp_int.sspr_cfg_rsp.msg_type = OTASP_SSPR_CONFIG_RSP_MSG;

    otasp_int.sspr_cfg_rsp.block_id = sspr_cfg_req_ptr->block_id;

     /* --------------------------------------------------------------
     ** The block_len is filled in later by the packing routines, and
     ** is set to zero if the block is not supported.

        If SP_LOCKED_STATE or NAM_LOCKED_STATE is TRUE, then we
        explicitly set the block len to zero to make sure that
        the blocks are not processed during translation.
     ** -------------------------------------------------------------- */
    if ((sp_locked_state) || (nam_locked_state))
    {
      otasp_int.sspr_cfg_rsp.result_code = OTASP_BLK_REJ_MS_LOCKED;
      otasp_int.sspr_cfg_rsp.block_len = 0;
    }
    else if ((!sp_locked_state) && (!nam_locked_state))
    {
      switch (sspr_cfg_req_ptr->block_id)
      {
        case OTASP_CFG_PRL_DIMENSIONS_BLK :
          /* If the SSPR Configuration Request Message contains the request
           * for the Preferred Roaming List Dimension Parameter Block
           * (BLOCK_ID = '00000000') and if the mobile station supports
           * SSPR_P_REV greater than or equal to '00000011' and stores
           * preferred roaming list associated with SSPR_P_REV greater than
           * or equal to '00000011', then, the mobile station shall set the
           * RESULT_CODE in the SSPR Configuration Response Message to
           * '00100011', "Rejected - PRL format mismatch."
           */
          M1X_MSG( DCP, LEGACY_MED,
            "Reporting the PRL dimensions ");

          if (otasp_roam_size.cur_sspr_p_rev >= (uint8)PRL_SSPR_P_REV_3)
          {
            otasp_int.sspr_cfg_rsp.block_len = 0;
            otasp_int.sspr_cfg_rsp.result_code = OTASP_BLK_REJ_PRL_MISMATCH;
            M1X_MSG( DCP, LEGACY_HIGH,
              "PRL format mismatch");
          }
          else

          if (otasp_roam_size.cur_sspr_p_rev == (uint8)PRL_SSPR_P_REV_1)
          {
            /* Preferred Roaming List Dimensions */
            M1X_MSG( DCP, LEGACY_MED,
              "PRL Dimensions for IS-683A");

            /* this is not the actual block len, the correct one will
               be computed before translation */
            otasp_int.sspr_cfg_rsp.block_len = 1;

            otasp_int.sspr_cfg_rsp.result_code = OTASP_BLK_ACCEPT;
            otasp_int.sspr_cfg_rsp.block.blk.prl_size = otasp_roam_size;
          }
          else
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Invalid SSPR protocol revision");
            otasp_int.sspr_cfg_rsp.block_len = 0;
            otasp_int.sspr_cfg_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
          }
        break;

        case OTASP_CFG_PREF_ROAM_LIST_BLK:
          sspr_cfg_rsp_populate_prl_blk( &otasp_int.sspr_cfg_rsp,
                                         sspr_cfg_req_ptr );
        break;


        case OTASP_CFG_EXT_PRL_DIMENSIONS_BLK :

          M1X_MSG( DCP, LEGACY_MED,
            "Reporting the extended PRL dimensions ");
          /* this is not the actual block len, the correct one will
          be computed before translation */
          otasp_int.sspr_cfg_rsp.block_len = 1;

          /* Preferred Roaming List Dimensions */
          otasp_int.sspr_cfg_rsp.result_code = OTASP_BLK_ACCEPT;
          otasp_int.sspr_cfg_rsp.block.blk.prl_size = otasp_roam_size;
        break;

        default:
          /* invalid SSPR Parameter block */
          otasp_int.sspr_cfg_rsp.result_code = OTASP_BLK_REJ_BLOCK_ID;
          otasp_int.sspr_cfg_rsp.block_len = 0;
        break;
      }
    }

    /* Translate and send the Configuration Response Message */

    status = otaspx_int_to_ext( &len, &otasp_int, chari );

    if (status == OTASPX_DONE_S) /*lint !e641 */
    {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
    }
    else
    {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
    }
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

} /* process_sspr_config_req  */

/*===========================================================================

FUNCTION SSPR_APPEND_RL

DESCRIPTION
  This function takes a received part of a roaming list and puts
  it into the roaming list being accumulated in memory.

DEPENDENCIES
  The first of the received roaming list bits must be byte alligned.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void sspr_append_rl
(
  byte *bits_ptr,
    /* Index into the caii_rx_msg_type where the Roaming List piece begins */
  word segment_offset,
    /* Offset of the segment */
  word segment_size
    /* size of the segment  */
)
{
  static byte *index_ptr; /* Pointer to starting of the roaming list */
  size_t copy_size;
  /* Return value of memscpy function */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (new_roaming_list)
  {
    /* Initialize nv_pr_list and static variables */
    nv_pr_list.nam = cdma.curr_nam;
    nv_pr_list.size = 0;
    index_ptr = (byte*) nv_pr_list.roaming_list;
    (void) memset( index_ptr, 0, NV_ROAMING_LIST_MAX_SIZE );
    new_roaming_list = FALSE;
  }

  /* The offset is always byte aligned, we can do a simple memscpy */
  /*lint -e668*/
  copy_size = memscpy( (index_ptr + segment_offset),
                       (sizeof(nv_pr_list.roaming_list)- segment_offset),
                        bits_ptr, segment_size );
  if( copy_size != segment_size )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
      copy_size,
      segment_size,
      (sizeof(*index_ptr)- segment_offset));
  }
  /*lint +e668*/

} /* sspr_append_rl()  */

#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION SEND_SSPR_DLOAD_RESP_MSG

DESCRIPTION
   This function sends the SSPR Download Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_sspr_dload_resp_msg(void)
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the SSPR Download Response Message */
  ruim_otasp_int.sspr_dload_rsp.msg_type = OTASP_SSPR_DOWNLOAD_RSP_MSG;

  status = otaspx_int_to_ext( &len, &ruim_otasp_int, chari );

  if (status == OTASPX_DONE_S) /*lint !e641*/
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
}  /* send_sspr_dload_resp_msg */
#endif /* FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION PROCESS_SSPR_DLOAD_REQ

DESCRIPTION
  This function processes an OTASP SSPR Download Request Message,
  formats the appropriate Protocol Download Response Message, then
  calls the routine to send it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_sspr_dload_req
(
  otaspi_sspr_dload_req_msg_type* sspr_dload_req_ptr
    /* Pointer to SSPR Download Request Message */
)
{

#ifndef FEATURE_CSIM
  size_t copy_size;
  /* Return value of memscpy function */
#endif /* FEATURE_CSIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  otaspi_ftc_sspr_prl_type *sspr_roam;
  word offset;  /* offset of segment to be downloaded */
  byte size;    /* size of the segment */
  prl_validation_enum_type validn_result = PRL_VALIDATE_MAX_TYPE;
#ifdef EFS_HAVE_SYNC
  int res;
#endif /* EFS_HAVE_SYNC */

#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
#if defined( FEATURE_UIM_RUIM )
#ifdef FEATURE_CSIM
  mmgsdi_return_enum_type mmgsdi_status;
  mmgsdi_data_type sspr_download_data;
#else
  uim_cmd_type *uim_cmd_ptr; /* UIM command pointer */
#endif
  byte pos = 0;
#endif /* FEATURE_UIM_RUIM */

  boolean is_full_validation;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    /* Initialize the number of blocks req to and recd from the card */
    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_card = 0;

    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_rsp_card = 0;

    /* Check to see if a message buffer is available */
    if ((otasp_ruim_table.req_index + 1) % OTASP_RUIM_MAX_MSG_BUFS !=
                                   otasp_ruim_table.rsp_index)
    {
       /* Store the message type */
       otasp_ruim_table.msg[otasp_ruim_table.req_index].msg_type =
                OTASP_SSPR_DOWNLOAD_REQ_MSG;

       /* Store the number of blocks requested in the message as only one */
       otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_msg = 1;
       #ifdef FEATURE_CSIM
       /* Copy the contents of the PRL data buffer */
       sspr_download_data.data_len = sspr_dload_req_ptr->block_len;
       sspr_download_data.data_ptr = sspr_dload_req_ptr->block.blk.prl_data;

       /* move past the reserved bits */
       pos += FSIZ(otaspe_sspr_prl_fix_type, reserved);
       /* unpack the last segment indicator */
          otasp_ruim_last_segment =
              b_unpackb( sspr_dload_req_ptr->block.blk.prl_data, pos,
                         FSIZ(otaspe_sspr_prl_fix_type, last_segment) );

       #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
       /* Don't send MMGSDI request if Sim Busy event has been received */  
       if (mc_get_sim_busy_status())
       {
         M1X_MSG( DCP, LEGACY_HIGH,
                  "SIM Busy, Otasp sspr dload request failed" );
       }
       else
       #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
       { 
         mmgsdi_status = mmgsdi_session_otasp_block_req ( cdma.session_id,
                                                   MMGSDI_OTASP_SSPR_DOWNLOAD,
                                                   sspr_dload_req_ptr->block_id,
                                                   sspr_download_data,
                                                   otasp_mmgsdi_response_cb,
                                                   0 );
         if ( mmgsdi_status != MMGSDI_SUCCESS )
         {
           M1X_MSG( DCP, LEGACY_ERROR,
             "MMGSDI returned Err, status=%d",
             mmgsdi_status );
  
           /* We do not store the id of the rejected block since there is only
             one block in this message */
         }
         else
         {
           /* Increment the number of blocks sent to the card */
             ++( otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                          num_blocks_req_card );
         }
       }
       #else

       if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
       {
          uim_cmd_ptr->hdr.command = UIM_SSPR_DOWNLOAD_REQ_F;

          /* Fill in the block id and block length in the command */
          uim_cmd_ptr->sspr_download.block_id = sspr_dload_req_ptr->block_id;

          if( (sspr_dload_req_ptr->block_len <= UIM_MAX_DLOAD_REQ_BYTES) &&
              (sspr_dload_req_ptr->block_len <= sizeof(sspr_dload_req_ptr->block.blk.prl_data)) )
          {
            uim_cmd_ptr->sspr_download.block_len = sspr_dload_req_ptr->block_len;
            /* Copy the contents of the PRL data buffer */
            copy_size = memscpy(uim_cmd_ptr->sspr_download.data,
                                UIM_MAX_CHARS*sizeof(byte),
                   sspr_dload_req_ptr->block.blk.prl_data,
                   sspr_dload_req_ptr->block_len);
            if( copy_size != sspr_dload_req_ptr->block_len )
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                copy_size,
                sspr_dload_req_ptr->block_len,
                UIM_MAX_CHARS*sizeof(byte));
            }
          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Unexpected block_len:%d",
              sspr_dload_req_ptr->block_len);
            uim_cmd_ptr->sspr_download.block_len = 0;
          }

          /* move past the reserved bits */
          pos += FSIZ(otaspe_sspr_prl_fix_type, reserved);

          /* unpack the last segment indicator */
          otasp_ruim_last_segment =
              b_unpackb(sspr_dload_req_ptr->block.blk.prl_data, pos,
                        FSIZ(otaspe_sspr_prl_fix_type, last_segment));

          uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

          mcc_uim_cmd( uim_cmd_ptr );

          if (uim_cmd_ptr->hdr.status != UIM_RECEIVED_S)
          {
             M1X_MSG( DCP, LEGACY_ERROR,
               "Bad status from UIM %d",
               uim_cmd_ptr->hdr.status);
          }
          else
          {
             /* Increment the number of blocks sent to the card */
             ++(otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                    num_blocks_req_card);
          }
       }
       else
       {
          M1X_MSG( DCP, LEGACY_ERROR,
            "No free buffers on uim_free_q" );

          /* We do not store the id of the rejected block since there is only
             one block in this message */
       }
       #endif
    } /* check for message buffer availability */

    /* If any block has been sent to the card successfully */
    if (otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                        num_blocks_req_card > 0)
    {
        /* consider this message queued and increment the request index */
        otasp_ruim_table.req_index = (otasp_ruim_table.req_index + 1) %
                                    OTASP_RUIM_MAX_MSG_BUFS;
    }
    else
    {
        ruim_otasp_int.sspr_dload_rsp.block_id = sspr_dload_req_ptr->block_id;

        ruim_otasp_int.sspr_dload_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;

        /* send the SSPR Download response message */
        send_sspr_dload_resp_msg();
    }
  }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  {
    otasp_int.sspr_dload_rsp.msg_type = OTASP_SSPR_DOWNLOAD_RSP_MSG;

    otasp_int.sspr_dload_rsp.block_id = sspr_dload_req_ptr->block_id;

    switch (sspr_dload_req_ptr->block_id) {
      case OTASP_DLOAD_PREF_ROAM_LIST:
      case OTASP_DLOAD_EXT_PREF_ROAM_LIST:
    /*  Accept IS-683A (Block ID=0 as well as IS-683C (Block ID=1) blocks
     ** for PRL download response msg
     */

         M1X_MSG( DCP, LEGACY_MED,
           "Downloading IS-683A/IS-683C PRL (block id %d)",
           sspr_dload_req_ptr->block_id);

         /* Initialize this flag to false */
         /* Set this flag to TRUE only if the CRC passes, which also prevents an
            invalid PRL from being committed when an invalid PRL is downloaded
            after a valid PRL is downloaded and later committed */
         commit.commit_roam_list = FALSE;

         /* Block Length is set to zero if parsing fails */
         if (sspr_dload_req_ptr->block_len == 0)
         {
            /* Data Size Mismatch */
            otasp_int.sspr_dload_rsp.result_code = OTASP_BLK_REJ_DATA_SIZE;
            /* ---------------------------------------------------------------
            ** For other blocks we delete any received versions when an
            ** invalid block is received.  Since the Roaming List comes in
            ** multiple blocks, however, it isn't appropriate to do that here.
            ** -------------------------------------------------------------- */

            /* Fill the remaining fields of the response message */
            otasp_int.sspr_dload_rsp.segment_offset =
                        sspr_dload_req_ptr->block.blk.sspr_prl.segment_offset;
            otasp_int.sspr_dload_rsp.segment_size =
                        sspr_dload_req_ptr->block.blk.sspr_prl.segment_size;
         }
         else if (otasp_commit_in_progress && nv_commit.commit_roam_list)
         {
            /* ------------------------------------------------------------
            ** We are in the process of writing a Roaming List to NV, and
            ** rather than creating another huge buffer to handle this
            ** pathological case, we simply reject the block.
            ** ------------------------------------------------------------ */
            otasp_int.sspr_dload_rsp.result_code = OTASP_BLK_REJ_MODE;
         }
         else
         {
            offset = sspr_dload_req_ptr->block.blk.sspr_prl.segment_offset;
            size = sspr_dload_req_ptr->block.blk.sspr_prl.segment_size;
            otasp_int.sspr_dload_rsp.segment_offset = offset;
            otasp_int.sspr_dload_rsp.segment_size = size;

            if ((offset + size) > nv_max_size_of_roaming_list())
            {
               /* segment offset in conjunction with the segment size are
               ** inconsistent with the PRL storage capabilities of the MS,
               ** so reject the request with PRL length mismatch result code */
               otasp_int.sspr_dload_rsp.result_code = OTASP_BLK_REJ_PRL_LEN;
            }
            else
            {
               sspr_roam =  &sspr_dload_req_ptr->block.blk.sspr_prl;

               /* call the function to append the roaming list */
               sspr_append_rl(sspr_roam->bits_ptr, offset, size);

               if (sspr_dload_req_ptr->block.blk.sspr_prl.last_segment)
               {
                 /* this is the last segment of the roaming list */
                 /* this is the size of roaming list in octets */
                 nv_pr_list.size = b_unpackw( (byte*) nv_pr_list.roaming_list,
                                               0,
                                               PR_LIST_SIZE_LENGTH);
                 if(nv_pr_list.size <= NV_ROAMING_LIST_MAX_SIZE)
                 {
                   /* Convert the size in bytes to bits before storing in NV */
                   nv_pr_list.size = (word) 8 * nv_pr_list.size; /*lint !e734 */

                   /* Validate prl based on SSPR P REV value.
                   ** Based on IS 683 Table 4.5.3-1 SSPR Para block type
                   **
                   ** Parameter Block Type     Block ID
                   **
                   **         PRL                 0
                   **     Extended PRL            1
                   **
                   ** Validating senario:
                   **   If block ID = 0, there is a case there is extended
                   **   PRL is downloaded. Do a full validation.
                   **   If block ID = 1, validate ext PRL only.
                   **
                   **   Update sspr_p_rev nv item.
                   */
                   if( sspr_dload_req_ptr->block_id == 0 )
                   {
                     sspr_p_rev = PRL_SSPR_P_REV_1;
                     is_full_validation = TRUE;
                   }
                   else
                   {
                     sspr_p_rev = PRL_SSPR_P_REV_3;
                     is_full_validation = FALSE;
                   }

                   validn_result =
                       prl_validate( (byte*) nv_pr_list.roaming_list, NULL,
                                      &sspr_p_rev, is_full_validation);

                   if (validn_result == PRL_INVALID_CRC)
                   {
                     M1X_MSG( DCP, LEGACY_ERROR,
                       "Invalid CRC");
                     otasp_int.sspr_dload_rsp.result_code = OTASP_BLK_REJ_CRC;
                     /* how to discard the previous and current segments */
                     nv_pr_list.size = 0;
                   }
                   else if (validn_result == PRL_VALID)
                   {
                     /* Update sspr_p_rev
                      */
                     otasp_nv_buf.item = NV_SSPR_P_REV_I;
                     otasp_nv_buf.cmd = NV_WRITE_F;
                     otasp_nv_buf.data_ptr =(nv_item_type*)&sspr_p_rev; /*lint !e826 */

                     (void) mcc_nv_cmd( &otasp_nv_buf);

                     otasp_int.sspr_dload_rsp.result_code = OTASP_BLK_ACCEPT;
                     nv_pr_list.valid = TRUE;
                     /* Set the flag to TRUE */
                     commit.commit_roam_list = TRUE;
                     commit.roam_list_ptr = &nv_pr_list;

                     /* Send OTASP status to CM */
                     otasp_report_status_to_cm(CM_OTASP_STATUS_PRL_DOWNLOADED);
                   }
                   else
                   {
                     otasp_int.sspr_dload_rsp.result_code =
                                        OTASP_BLK_REJ_UNKNOWN;
                     /* how to discard the previous and current segments */
                     nv_pr_list.size = 0;
                     nv_pr_list.nam = commit.nam;
                   }
                   new_roaming_list = TRUE;
                 } /* if(nv_pr_list.size <= NV_ROAMING_LIST_MAX_SIZE) */
                 else
                 {
                   /* First two bytes of nv_pr_list.roaming_list will
                    * represent the length of the PRL and max allowed
                    * size is NV_ROAMING_LIST_MAX_SIZE
                    * so reject the req with PRL length mismatch
                    * result code */
                   otasp_int.sspr_dload_rsp.result_code =
                                                OTASP_BLK_REJ_PRL_LEN;
                   M1X_MSG( DCP, LEGACY_ERROR,
                     "Invalid PRL Length:%d",
                     nv_pr_list.size);
                 }
               } /* if (sspr_dload_req_ptr->block.blk.sspr_prl.last_segment) */
               else
               {
                  /* accept the segment of the roaming list */
                  otasp_int.sspr_dload_rsp.result_code = OTASP_BLK_ACCEPT;
               }
            }
         }
      break;

      default:
          /* invalid SSPR Parameter block */
          otasp_int.sspr_dload_rsp.result_code = OTASP_BLK_REJ_BLOCK_ID;
          otasp_int.sspr_dload_rsp.segment_offset = 0;
          otasp_int.sspr_dload_rsp.segment_size = 0;

      break;
    }

    /* Translate and send the SSPR Download Response Message */

    status = otaspx_int_to_ext( &len, &otasp_int, chari );

    if (status == OTASPX_DONE_S) /*lint !e641 */
    {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
    }
    else
    {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
    }

#ifdef EFS_HAVE_SYNC
    if (validn_result == PRL_VALID)
    {
      /* Kick the watchdog */
      mc_dog_report();

      M1X_MSG( DCP, LEGACY_MED,
        "Synchronizing the EFS");
      res = efs_sync("/");
      if (res != 0)
      {
        ERR_FATAL( "EFS sync failed",0,0,0);
      }
    }
    M1X_MSG( DCP, LEGACY_MED,
      "EFS sync after sspr dld req is successful.");
#endif /* EFS_HAVE_SYNC */

  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

} /* process_sspr_dload_req  */

#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION SEND_VALIDN_RESP_MSG

DESCRIPTION
   This function sends the Validation Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_validn_resp_msg(void)
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the Validation Response Message */
  ruim_otasp_int.validn_rsp.msg_type = OTASP_VALIDATION_RSP_MSG;
  status = otaspx_int_to_ext( &len, &ruim_otasp_int, chari );

  if (status == OTASPX_DONE_S) /*lint !e641*/
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
}  /* send_validn_resp_msg */
#endif /* FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION PROCESS_VALIDATION_REQ

DESCRIPTION
  This function processes an OTASP Validation Request Message,
  formats the appropriate Protocol Validation Response Message, then
  calls the routine to send it.

DEPENDENCIES
  None.

RETURN VALUE
  Next state to be processed.  CDMA_CONV or CDMA_EXIT for powerdown.

SIDE EFFECTS
  None.

===========================================================================*/

word process_validation_req
(
  otaspi_validation_req_msg_type* validn_req_ptr
    /* Pointer to Validation Request Message */
)
{
  word x;
  word pos1,pos2;
  otaspi_validation_req_msg_type validn_temp;
  word next_state; /* Next state to be processed */
  size_t copy_size;
  /* Return value of memscpy function */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  byte zeros[NV_SEC_CODE_SIZE];
  /* String to check against the SPC to see if it is zero */
  word y;
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
#if defined( FEATURE_UIM_RUIM )
#ifdef FEATURE_CSIM
  mmgsdi_data_type validate_data;
  mmgsdi_return_enum_type mmgsdi_status;
#else
  /* Pointer to UIM command */
  uim_cmd_type  *uim_cmd_ptr;
#endif
  /* index for tracking rejected blocks */
  byte rej_block_index = 0;
#endif /* FEATURE_UIM_RUIM */
  mccccim_event_type mccccim_evt;
    /* Event rpt struct for sending primitives to CCIM */
  uint8 otasp_max_unlock_attempts;  /* Otasp Max unlock attempts allowed */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  next_state = CDMA_TRAFFIC; /*lint !e641 */

  /* Get Otasp Max unlock attempts allowed */
  otasp_max_unlock_attempts = (uint8) mcc_otasp_max_unlock_attempts_allowed();

  /* The following lines of code help us to find out if CHANGE_SPC_BLOCK
  ** comes before the VERIFY_SPC_BLOCK. If both the blocks are present,
  ** then irrespective of the order, VERIFY_SPC_BLOCK has to be processed
  ** before CHANGE_SPC_BLOCK */
  pos1 = 0;
  pos2 = OTASPI_VALIDN_BLOCK_MAX - 1 ;
  for (x = 0;(( x < validn_req_ptr->num_blocks) &&
        (x < OTASPI_VALIDN_BLOCK_MAX)); x++)
  {
     if (validn_req_ptr->block[x].block_id == OTASP_VERIFY_SPC_BLOCK)
     {
        pos1 = x;
     }
     else if ( validn_req_ptr->block[x].block_id == OTASP_CHANGE_SPC_BLOCK)
     {
        pos2 = x;
     }
  }

  /* if CHANGE_SPC_BLOCK precedes VERIFY_SPC_BLOCK */
  if (pos2 < pos1)
  {
     /* We swap their order within the message */
     validn_temp.block[0] = validn_req_ptr->block[pos2];
     validn_req_ptr->block[pos2] = validn_req_ptr->block[pos1]; /*lint !e661 */
     validn_req_ptr->block[pos1] = validn_temp.block[0]; /*lint !e661 */
  }

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    /* Initialize the number of blocks req to and recd from the card */
    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_card = 0;

    otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_rsp_card = 0;

    /* Check to see if a message buffer is available */
    if ((otasp_ruim_table.req_index + 1) % OTASP_RUIM_MAX_MSG_BUFS !=
                                   otasp_ruim_table.rsp_index)
    {
      /* Store the message type */
      otasp_ruim_table.msg[otasp_ruim_table.req_index].msg_type =
                          OTASP_VALIDATION_REQ_MSG;

      /* Store the number of blocks requested in the message */
      otasp_ruim_table.msg[otasp_ruim_table.req_index].num_blocks_req_msg =
                validn_req_ptr->num_blocks;

      /* send a command for each block to the card */
      for (x = 0; x < validn_req_ptr->num_blocks; x++)
      {
      #ifdef FEATURE_CSIM
        validate_data.data_len = validn_req_ptr->block[x].block_len;
        validate_data.data_ptr = validn_req_ptr->block[x].blk.validn_data;

        #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
        /* Don't send MMGSDI request if Sim Busy event has been received */  
        if (mc_get_sim_busy_status())
        {
          M1X_MSG( DCP, LEGACY_HIGH, 
            "SIM Busy, Otasp validate block req failed for block no = %d", x );
          /* Store the id of the block that is not sent to the card */
          otasp_ruim_table.msg[otasp_ruim_table.req_index].
               rej_block_id[rej_block_index++] =
                             validn_req_ptr->block[x].block_id;
        }
        else
        #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
        {
          mmgsdi_status = mmgsdi_session_otasp_block_req ( cdma.session_id,
                                                    MMGSDI_OTASP_VALIDATE,
                                                    validn_req_ptr->block[x].block_id,
                                                    validate_data,
                                                    otasp_mmgsdi_response_cb,
                                                    0 );
          if (mmgsdi_status != MMGSDI_SUCCESS)
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "No free buffers on uim_free_q" );
            /* Store the id of the block that is not sent to the card */
            otasp_ruim_table.msg[otasp_ruim_table.req_index].
                 rej_block_id[rej_block_index++] =
                               validn_req_ptr->block[x].block_id;
          }
          else
          {
            /* Increment the number of blocks sent to the card */
            ++(otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                        num_blocks_req_card);
          }
        }
        #else
        if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
        {
           uim_cmd_ptr->hdr.command = UIM_VALIDATE_F;

           uim_cmd_ptr->validate.block_id = validn_req_ptr->block[x].block_id;

           if( (validn_req_ptr->block[x].block_len <= UIM_MAX_VALIDATION_REQ_BYTES) &&
               (validn_req_ptr->block[x].block_len <= sizeof(validn_req_ptr->block[x].blk.validn_data)) )
           {
             uim_cmd_ptr->validate.block_length = validn_req_ptr->block[x].block_len;
             copy_size = memscpy(uim_cmd_ptr->validate.data,
                                 UIM_MAX_CHARS*sizeof(byte),
                    validn_req_ptr->block[x].blk.validn_data,
                    validn_req_ptr->block[x].block_len);
             if( copy_size != validn_req_ptr->block[x].block_len )
             {
               M1X_MSG( DCP, LEGACY_ERROR,
                 "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                 copy_size,
                 validn_req_ptr->block[x].block_len,
                 UIM_MAX_CHARS*sizeof(byte));
             }
           }
           else
           {
             M1X_MSG( DCP, LEGACY_ERROR,
               "Unexpected block_len:%d",
               validn_req_ptr->block[x].block_len);
             uim_cmd_ptr->validate.block_length = 0;
           }

           uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

           mcc_uim_cmd( uim_cmd_ptr );

           if (uim_cmd_ptr->hdr.status != UIM_RECEIVED_S)
           {
               M1X_MSG( DCP, LEGACY_ERROR,
                 "Bad status from UIM %d",
                 uim_cmd_ptr->hdr.status);

               /* Store the id of the block that is not processed by the card */
               otasp_ruim_table.msg[otasp_ruim_table.req_index].
                   rej_block_id[rej_block_index++] =
                            validn_req_ptr->block[x].block_id;
           }
           else
           {
               /* Increment the number of blocks sent to the card */
               ++(otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                        num_blocks_req_card);
           }
        }
        else
        {
           M1X_MSG( DCP, LEGACY_ERROR,
             "No free buffers on uim_free_q" );

           /* Store the id of the block that is not sent to the card */
           otasp_ruim_table.msg[otasp_ruim_table.req_index].
               rej_block_id[rej_block_index++] =
                             validn_req_ptr->block[x].block_id;
        }
        #endif
      }   /* end of for loop */
    } /* check for message buffer availability */

    /* If any block has been sent to the card successfully */
    if (otasp_ruim_table.msg[otasp_ruim_table.req_index].
                                        num_blocks_req_card > 0)
    {
        /* consider this message queued and increment the request index */
        otasp_ruim_table.req_index = (otasp_ruim_table.req_index + 1) %
                                    OTASP_RUIM_MAX_MSG_BUFS;
    }
    else
    {
        /* none of the blocks are sent to the card */
        ruim_otasp_int.validn_rsp.num_blocks = validn_req_ptr->num_blocks;

        /* send a reject response for all the blocks in this message */
        for (x = 0; x < validn_req_ptr->num_blocks; x++)
        {
            ruim_otasp_int.validn_rsp.block[x].block_id =
                                       validn_req_ptr->block[x].block_id;

            ruim_otasp_int.validn_rsp.block[x].result_code= OTASP_BLK_REJ_UNKNOWN;
        }

        /* send the validation response message */
        send_validn_resp_msg();
    }
  }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  {
    /* do the normal processing */
    otasp_int.validn_rsp.msg_type = OTASP_VALIDATION_RSP_MSG;
    otasp_int.validn_rsp.num_blocks = validn_req_ptr->num_blocks;

    for (x = 0; x < validn_req_ptr->num_blocks; x++)
    {
      otasp_int.validn_rsp.block[x].block_id =
                  validn_req_ptr->block[x].block_id;

      switch (validn_req_ptr->block[x].block_id) {

        case OTASP_VERIFY_SPC_BLOCK:
        {
          boolean valid_block = TRUE; /* Flag if given SPC has valid digits */

          /* Block Length is set to zero if parsing fails */
          if (validn_req_ptr->block[x].block_len == 0)
          {
            /* Data Size Mismatch */
            otasp_int.validn_rsp.block[x].result_code = OTASP_BLK_REJ_DATA_SIZE;
          }
          else
          {
            if (validn_req_ptr->block[x].block_len <
                                 ((NV_SEC_CODE_SIZE + 1) / 2))
            {
               /* SPC Parameter Block does not have enough digits! */
               otasp_int.validn_rsp.block[x].result_code =
                      OTASP_BLK_REJ_DATA_SIZE;
            }
            else
            {
               /* Check all digits to make sure they are valid */
               for (y = 0; y < NV_SEC_CODE_SIZE; y++)
               {
                 /* Valid BCD digits are 0-9, but also count 10 as zero */
                 if (validn_req_ptr->block[x].blk.verify_spc.spc[y] > 10)
                 {
                   /* Invalid parameter */
                   otasp_int.validn_rsp.block[x].result_code =
                         OTASP_BLK_REJ_INVALID;
                   valid_block = FALSE;
                   break;
                 }
                 else
                 {
                   /* Convert BCD to ASCII for comparison with stored SPC */
                   if (validn_req_ptr->block[x].blk.verify_spc.spc[y] == 10)
                   {
                     /* If we got a 10, assume it is a zero */
                     validn_req_ptr->block[x].blk.verify_spc.spc[y] = 0;
                   }
                   validn_req_ptr->block[x].blk.verify_spc.spc[y] += '0';
                 }
               }

               if (valid_block)
               {
                  if (memcmp( validn_req_ptr->block[x].blk.verify_spc.spc,
                          cdma.spc, NV_SEC_CODE_SIZE ) != 0)
                  {
                     /* Service Programming Code sent doesn't match */
                     otasp_int.validn_rsp.block[x].result_code =
                                        OTASP_BLK_REJ_SPC;

                     if (cdma.otasp_call == USER_INIT_SRV_PROV)
                     {
                        if (++otasp_failed_unlock_attempts == otasp_max_unlock_attempts)
                        {
                          M1X_MSG( DCP, LEGACY_HIGH,
                            "%d SPC validation failures, terminate OTASP",
                            otasp_failed_unlock_attempts);
                          /* Send OTASP status to CM */
                          otasp_report_status_to_cm(CM_OTASP_STATUS_SPC_RETRIES_EXCEEDED);

                          /* Send Notification to MCCCCIM to release all CC instances */
                          mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
                          /* All CC instances should already be in release substate */
                          mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_REL_OTASP_ERR;
                          mccccim_process_event( &mccccim_evt );
                          /* No need to check return status in this case */

                          next_state = (word) CDMA_RELEASE;
                          cdma.rel_orig = FALSE;
                          cdma.otasp_call = NO_SRV_PROV;

                          /* Events report: Release */
                          /* Treat it as BS originated because upper layers are not aware of it */
                          mclog_report_release(MCLOG_RELEASE_BS_ORDER);
                        }
                     } /* USER_INIT_SRV_PROV */

                     else
                     {
                        /* OTAPA call, don't power down */
                        if (++otasp_otapa_failed_spc_val_attempts == otasp_max_unlock_attempts)
                        {
                           /* We don't want to powerdown if OTAPA session occurs
                           ** in the middle of a conversation. */
                           M1X_MSG( DCP, LEGACY_HIGH,
                             "%d SPC validation failures, terminate OTAPA",
                             otasp_otapa_failed_spc_val_attempts);

                           cdma.otasp_call = NO_SRV_PROV;
                        }
                     } /* OTAPA call */

                  }
                  else
                  {
                      /* We got a good SPC, so unlock the phone */
                      otasp_int.validn_rsp.block[x].result_code =
                                         OTASP_BLK_ACCEPT;
                      sp_locked_state = FALSE;

                      /* Send OTASP status to CM */
                      otasp_report_status_to_cm(CM_OTASP_STATUS_SPL_UNLOCKED);
                  }
               } /* end if (valid_block) */
            } /* end if (enough digits) */
          }
          break;
        }

        case OTASP_CHANGE_SPC_BLOCK:
        {
          boolean valid_block = TRUE; /* Flag if given SPC has valid digits */

          /* Block Length is set to zero if parsing fails */
          if (validn_req_ptr->block[x].block_len == 0)
          {
            /* Data Size Mismatch */
            otasp_int.validn_rsp.block[x].result_code =
                       OTASP_BLK_REJ_DATA_SIZE;
          }
          else if (validn_req_ptr->block[x].block_len <
                                 ((NV_SEC_CODE_SIZE + 1) / 2))
          {
             /* SPC Parameter Block does not have enough digits! */
             otasp_int.validn_rsp.block[x].result_code =
                             OTASP_BLK_REJ_DATA_SIZE;
          }
          else
          {
            /* Check all digits to make sure they are valid */
            for (y = 0; y < NV_SEC_CODE_SIZE; y++)
            {
               /* Valid BCD digits are 0-9, but also count 10 as zero */
               if (validn_req_ptr->block[x].blk.change_spc.spc[y] > 10)
               {
                  /* Invalid parameter */
                  otasp_int.validn_rsp.block[x].result_code =
                       OTASP_BLK_REJ_INVALID;
                  valid_block = FALSE;
                  break;
               }
               else
               {
                 /* Convert BCD to ASCII for storing the SPC */
                 if (validn_req_ptr->block[x].blk.change_spc.spc[y] == 10)
                 {
                    /* If we got a 10, assume it is a zero */
                    validn_req_ptr->block[x].blk.change_spc.spc[y] = 0;
                 }
                 validn_req_ptr->block[x].blk.change_spc.spc[y] += '0';
               }
            }
            if (valid_block)
            {
               if (sp_locked_state)
               {
                  otasp_int.validn_rsp.block[x].result_code =
                       OTASP_BLK_REJ_MS_LOCKED;
               }
               else /* spc lock state is zero */
               {
                if (memcmp( cdma.spc,
                   memset( zeros, '0', NV_SEC_CODE_SIZE ),
                   NV_SEC_CODE_SIZE ) == 0)
                {
                    if (cdma.spc_change_enabled)
                    {
                       otasp_int.validn_rsp.block[x].result_code =
                            OTASP_BLK_ACCEPT;

                       /* copy the SPC into local memory */
                       copy_size = memscpy( (byte*) commit.spc.digits,
                                             sizeof(commit.spc.digits),
                               validn_req_ptr->block[x].blk.change_spc.spc,
                               NV_SEC_CODE_SIZE );
                       if (copy_size != NV_SEC_CODE_SIZE )
                       {
                         M1X_MSG( DCP, LEGACY_ERROR,
                           "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                           copy_size,
                           NV_SEC_CODE_SIZE,
                           sizeof(commit.spc.digits));
                       }
                       commit.commit_spc = TRUE;
                    }
                    else
                    {
                       /* the user has disabled the spc change  */
                       otasp_int.validn_rsp.block[x].result_code =
                        OTASP_BLK_REJ_SPC_USER;
                    }
                 }
                 else
                 {
                    otasp_int.validn_rsp.block[x].result_code =
                            OTASP_BLK_ACCEPT;

                    /* copy the SPC into local memory */
                    copy_size = memscpy( (byte*) commit.spc.digits,
                                          sizeof(commit.spc.digits),
                            validn_req_ptr->block[x].blk.change_spc.spc,
                            NV_SEC_CODE_SIZE );
                    if( copy_size != NV_SEC_CODE_SIZE )
                    {
                      M1X_MSG( DCP, LEGACY_ERROR,
                        "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                        copy_size,
                        NV_SEC_CODE_SIZE,
                        sizeof(commit.spc.digits));
                    }
                    commit.commit_spc = TRUE;
                 }
               } /* sp lock state  */
            }  /* valid block */
          } /* else condition */

          break;
        }

        case VALIDATE_SPASM:
        {


          /* Not in a OTAPA call */
          if (cdma.otasp_call != NETWORK_INIT_SRV_PROV)
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Rej: not an OTAPA call");
            otasp_int.validn_rsp.block[x].result_code =
                                             OTASP_BLK_REJ_MODE_BLOCK_ID;
          }
          /* NAM is unlocked, save nam_lock and accept */
          else if (!nam_locked_state)
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Accept: NAM is unlocked");
            otasp_int.validn_rsp.block[x].result_code = OTASP_BLK_ACCEPT;

            /* We commit nam_lock only if nam_lock in NV is different from what
            ** base station sent to the mobile.
            */
            if (cdma.nam_lock != validn_req_ptr->block[x].blk.
                                                     validate_spasm.nam_lock)
            {
              commit.nam_lock.enabled = validn_req_ptr->block[x].blk.
                                                     validate_spasm.nam_lock;
              commit.commit_nam_lock = TRUE;
            }
          }
          /* auth_otapa is not included in the validation message but NAM is
             locked */
          else if (!validn_req_ptr->block[x].blk.validate_spasm.auth_otapa_incl)
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Rej:auth_otapa not included");
            otasp_int.validn_rsp.block[x].result_code = OTASP_BLK_REJ_SPASM;
          }
          /* auth_otapa has already calculated and ready - e.g. multiple SPASM
          ** validation attempts to unlock the phone.
          */
          else if (otasp_auth_otapa_ready)
          {
            M1X_MSG( DCP, LEGACY_MED,
              "auth_otapa %lx",
              otasp_auth_otapa);
            M1X_MSG( DCP, LEGACY_MED,
              "auth_otapa_r %lx",
              validn_req_ptr->block[x].blk.validate_spasm.auth_otapa);

            if (otasp_auth_otapa == validn_req_ptr->block[x].blk.validate_spasm.
                                                                     auth_otapa)
            {
              M1X_MSG( DCP, LEGACY_MED,
                "SPASM passed, unlock the mobile");
              /* auth_otapa match, SPASM validation passed. */
              otasp_int.validn_rsp.block[x].result_code = OTASP_BLK_ACCEPT;
              nam_locked_state = FALSE;

              /* We commit nam_lock only if nam_lock in NV is different from what
              ** base station sent to the mobile.
              */
              if (cdma.nam_lock !=
                          validn_req_ptr->block[x].blk.validate_spasm.nam_lock)
              {
                commit.nam_lock.enabled =
                          validn_req_ptr->block[x].blk.validate_spasm.nam_lock;
                commit.commit_nam_lock = TRUE;
              }
            }
            else
            {
              M1X_MSG( DCP, LEGACY_HIGH,
                "Rej: SPASM failed");
              /* Failed SPASM validation */
              otasp_int.validn_rsp.block[x].result_code = OTASP_BLK_REJ_SPASM;

              if (++otasp_otapa_failed_spasm_val_attempts == otasp_max_unlock_attempts)
              {
                 M1X_MSG( DCP, LEGACY_HIGH,
                   "%d SPASM validation failures, terminate OTAPA",
                   otasp_otapa_failed_spasm_val_attempts);
                 cdma.otasp_call = NO_SRV_PROV;
              }
            }
          }
          /* auth_otapa is not ready, send reject message. */
          else
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Rej: auth_otapa not ready");
            otasp_int.validn_rsp.block[x].result_code = OTASP_BLK_REJ_UNKNOWN;
          }


          break;
        }

        default:
          /* invalid Validation Parameter block */
          otasp_int.validn_rsp.block[x].result_code = OTASP_BLK_REJ_BLOCK_ID;

        break;

      }
    }

    /* Translate and send the Validation Response Message */

    status = otaspx_int_to_ext( &len, &otasp_int, chari );

    if (status == OTASPX_DONE_S)  /*lint !e641 */
    {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
    }
    else
    {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
    }
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
  return (next_state);

} /* process_validation_req  */

/*===========================================================================

FUNCTION OTASP_PROCESS_MSG

DESCRIPTION
  This function processes an OTASP message.

DEPENDENCIES
  None.

RETURN VALUE
  Next state to be processed.  CDMA_CONV or CDMA_EXIT for powerdown.

SIDE EFFECTS
  None.

===========================================================================*/

word otasp_process_msg
(
  otaspi_ftc_msg_type *otasp_msg_ptr
    /* OTASP message to be processed */
)
{
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
  word next_state; /* Next state to be processed */


   auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */


#ifdef FEATURE_CSIM
  mmgsdi_return_enum_type mmgsdi_status;
#endif

#if ( defined (FEATURE_UIM_RUIM) ) && ( !defined (FEATURE_CSIM) )
   uim_cmd_type *uim_cmd_ptr;
    /* Pointer to UIM command */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
   word prl_version;
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

#ifdef EFS_HAVE_SYNC
  int res;
#endif /* EFS_HAVE_SYNC */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ---------------------------------------------------------
  ** OTASP messages are only processed in CDMA_CONV substate.
  ** Currently next_state is only changed if too many attempts
  ** are made to unlock the phone, in which case we powerdown.
  ** --------------------------------------------------------- */
  next_state = CDMA_TRAFFIC; /*lint !e641 */

  M1X_MSG( DCP, LEGACY_MED,
    "Received OTASP message type %d",
    otasp_msg_ptr->gen.msg_type);

  switch (otasp_msg_ptr->gen.msg_type)
  {
    case OTASP_CONFIG_REQ_MSG:
      /* IS-683 OTASP Configuration Request Message */
      /* If a commit is in progress, we give the old configuration info */
      process_config_req (&otasp_msg_ptr->config_req);
    break;

    case OTASP_DOWNLOAD_REQ_MSG:
      /* IS-683 OTASP Download Request Message */
      next_state = process_dload_req (&otasp_msg_ptr->dload_req);
    break;

    case OTASP_MS_KEY_REQ_MSG:
      /* IS-683 OTASP MS Key Request Message */
      process_ms_key_req(&otasp_msg_ptr->ms_key_req);
    break;

    case OTASP_KEY_GEN_REQ_MSG:
      /* IS-683 OTASP Key Generation Request Message */
      process_key_gen_req(&otasp_msg_ptr->key_gen_req);
    break;

    case OTASP_REAUTH_REQ_MSG:
      /* IS-683 OTASP Re-authentication Request Message */
      process_reauth_req(&otasp_msg_ptr->reauth_req);
    break;

    case OTASP_COMMIT_REQ_MSG:
    {
#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
      /* Determine if the R-UIM is available for this NAM */
      if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
      {
        if (otasp_commit_in_progress)
        {
          /* If we are already committing, then drop the second commit */
        }
        else
        {
          #ifdef FEATURE_CSIM

          #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
          /* Don't send MMGSDI request if Sim Busy event has been received */  
          if (mc_get_sim_busy_status())
          {
            M1X_MSG( DCP, LEGACY_HIGH, 
                     "SIM Busy, Otasp commit request failed" );
            otasp_int.commit_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
            send_commit_resp_msg();
          }
          else
          #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
          { 
  
            mmgsdi_status = mmgsdi_session_otasp_commit_req( cdma.session_id,
                                                      otasp_mmgsdi_response_cb,
                                                      0 );
            if (mmgsdi_status != MMGSDI_SUCCESS)
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "No free buffers on uim_free_q" );
              otasp_int.commit_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
              send_commit_resp_msg();
            }
            else
            {
              /* Notify CM that otasp commit starts. */
              otasp_report_commit_status_to_cm(TRUE);
              otasp_commit_in_progress = TRUE;
            }
          }
          #else
          if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
          {
            uim_cmd_ptr->hdr.command = UIM_COMMIT_F;
            uim_cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;
            uim_cmd_ptr->hdr.cmd_hdr.sigs = NULL;
            uim_cmd_ptr->hdr.options = UIM_OPTION_ALWAYS_RPT;

            uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;
            uim_cmd_ptr->hdr.protocol = UIM_CDMA;
            uim_cmd( uim_cmd_ptr );

            /* Notify CM that otasp commit starts. */
            otasp_report_commit_status_to_cm(TRUE);

            otasp_commit_in_progress = TRUE;
          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "No free buffers on uim_free_q" );
            otasp_int.commit_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
            send_commit_resp_msg();
          }
          #endif
        }
      }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
      else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
      {
        boolean send_response = FALSE;
        otasp_committed_both_flag = FALSE;

        if (otasp_commit_in_progress)
        {
          /* If we are already committing, then drop the second commit */
        }
        else if ((sp_locked_state) || (nam_locked_state))
        {
          /* ----------------------------------------------------------
          ** The phone is locked and is not allowed to commit anything.
          ** We must send the Commit Response with result code saying
          ** the SPC is required first.
          ** For IS-683A we send the result code as MS locked.
          ** ---------------------------------------------------------- */
          send_response = TRUE;
          otasp_int.commit_rsp.result_code = OTASP_BLK_REJ_MS_LOCKED;
        }
        else if (commit.commit_mob_dir || commit.commit_ftc_cdma ||
                 commit.commit_ftc_amps || commit.commit_roam_list

                 || a_key_temp_valid

                 || commit.commit_spc || commit.commit_imsi_t

                 || commit.commit_nam_lock

                 || commit_ehrpd_imsi
          )
        {
          if (commit.commit_mob_dir || commit.commit_ftc_cdma ||
                 commit.commit_ftc_amps || commit.commit_roam_list
                 || commit.commit_spc || commit.commit_imsi_t

                 || commit.commit_nam_lock

                 || commit_ehrpd_imsi
             )
          {

            /* -----------------------------------------------------------
            ** If we are committing NAM info then commit.commit_ftc_cdma
            ** should be TRUE, since there is no case where we only commit
            ** analog NAM info.
            ** ----------------------------------------------------------- */
            if (!commit.commit_ftc_cdma && commit.commit_ftc_amps)
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "Committing only AMPS NAM info!");
            }

            if (commit.commit_roam_list)
            {
              M1X_MSG( DCP, LEGACY_HIGH,
                "Commiting PRL");

              /* We need to mark the list as valid or invalid before committing */
              nv_pr_list.valid = FALSE;

              prl_version = PRL_DEFAULT_VER;
              if (prl_validate((byte*) nv_pr_list.roaming_list, &prl_version,
                               &sspr_p_rev, FALSE) ==PRL_VALID)
              {
                sspr_p_rev = PRL_SSPR_P_REV_INVALID;
                nv_pr_list.valid = TRUE;
              }


              if (!mcc_pcs_band_is_supported()) {
              /* For 800 MHz CDMA phones, the list can be enabled or disabled  */
                otasp_nv_item.prl_enabled.nam = cdma.curr_nam;
                otasp_nv_item.prl_enabled.enabled = nv_pr_list.valid;

              /* Write to NV that the list should be enabled or disabled */
                otasp_nv_buf.item = NV_PRL_ENABLED_I;
                otasp_nv_buf.cmd = NV_WRITE_F;
                otasp_nv_buf.data_ptr = &otasp_nv_item;

                (void) mcc_nv_cmd( &otasp_nv_buf);
              }


              /* Set the PRL version before committing */
              nv_pr_list.prl_version = prl_version;
            }

            if (commit_ehrpd_imsi)
            {
              M1X_MSG( DCP, LEGACY_HIGH,
                "Commiting eHRPD IMSI");

              otasp_nv_item.ehrpd_imsi = otasp_ehrpd_imsi;

              /* Write the eHRPD IMSI to NV */
              otasp_nv_buf.item = NV_EHRPD_IMSI_I;
              otasp_nv_buf.cmd = NV_WRITE_F;
              otasp_nv_buf.data_ptr = &otasp_nv_item;

              (void) mcc_nv_cmd( &otasp_nv_buf);
            }

#ifdef EFS_HAVE_SYNC
            if((!mcc_pcs_band_is_supported()) ||
               (mcc_cellular_band_is_supported() && mcc_pcs_band_is_supported())
               || commit_ehrpd_imsi
              )
            {
              /* Kick the watchdog */
              mc_dog_report();

              M1X_MSG( DCP, LEGACY_MED,
                "Synchronizing the EFS");
              res = efs_sync("/");
              if (res != 0)
              {
                ERR_FATAL( "EFS sync failed",0,0,0);
              }
              M1X_MSG( DCP, LEGACY_MED,
                "EFS sync after otasp proc msg is successful.");
            }
#endif /* EFS_HAVE_SYNC */
            /* Copy information into buffer for NV commit command */
            nv_commit = commit;

          /* Clear the commit buffer for future download requests */
            commit.commit_mob_dir   = FALSE;
            commit.commit_ftc_cdma  = FALSE;
            commit.commit_ftc_amps  = FALSE;
            commit.commit_roam_list = FALSE;
            commit.commit_spc     = FALSE;
            commit.commit_imsi_t  = FALSE;

            commit.commit_nam_lock = FALSE;


            /* Commit OTASP information to NVRAM */
            otasp_nv_buf.cmd = NV_OTASP_COMMIT_F;
            otasp_nv_item.otasp_commit = &nv_commit;
            otasp_nv_buf.data_ptr = &otasp_nv_item;

            /* Fill in necessary fields of command block */
            otasp_nv_buf.done_q_ptr = NULL;
            otasp_nv_buf.tcb_ptr = MC_TCB_PTR;
            otasp_nv_buf.sigs = MCC_OTASP_COMMIT_SIG;
            nv_cmd( &otasp_nv_buf );

            otasp_commit_in_progress = TRUE;

            if (a_key_temp_valid)
            {
               otasp_commit_akey_pending = TRUE;
            }
            else
            {
               otasp_commit_akey_pending = FALSE;
            }

            M1X_MSG( DCP, LEGACY_MED,
              "OTASP commit in progress");
          }
          else
          {


            if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
            {

               /* Fill in necessary fields of command block */

               /* Copy information into buffer for NV commit command */
               nv_commit = commit;

               auth_cmd_ptr->hdr.command = AUTH_COMMIT_KEYS_F;
               auth_cmd_ptr->hdr.cmd_hdr.task_ptr = MC_TCB_PTR;
               auth_cmd_ptr->hdr.cmd_hdr.sigs = MCC_OTASP_COMMIT_SIG;

               auth_cmd_ptr->commit_keys.nam_index = cdma.curr_nam;
               auth_cmd_ptr->commit_keys.rpt_function = mcc_queue_auth_rpt;
               otasp_commit_in_progress = TRUE;

               otasp_commit_akey_pending = FALSE;

               auth_cmd( auth_cmd_ptr );
               if (auth_cmd_ptr->hdr.status != AUTH_RECEIVED_S)
               {
                 #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
                 if (mc_get_auth_err_status())
                 {
                   /* Don't throw ERR_FATAL as SIM Busy is an expected event. */
                   M1X_MSG( DCP, LEGACY_ERROR, 
                            "otasp_process_msg - Bad status from AUTH %d", 
                            (int) auth_cmd_ptr->hdr.status );
                 }
                 else
                 #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
                 {
                   ERR_FATAL( " Bad status from AUTH %d", (int) auth_cmd_ptr->hdr.status,0,0);
                 }
               }
             }
             else
             {
                ERR_FATAL( "No free buffers on auth_free_q", 0, 0, 0 );
             }

          /* We will send the response later, after the commit completes */
          }

          /* Notify CM that otasp commit starts. */
          otasp_report_commit_status_to_cm(TRUE);
        }
        else
        {
          /* --------------------------------------------------------------
          ** We have nothing to commit, but if our commit response got lost
          ** somewhere, a second commit may have been sent, so to be safe
          ** we immediately respond with result code ACCEPT.
          ** -------------------------------------------------------------- */
          send_response = TRUE;
          otasp_int.commit_rsp.result_code = OTASP_BLK_ACCEPT;
        }

        if (send_response)
        {
          otasp_int.commit_rsp.msg_type = OTASP_COMMIT_RSP_MSG;
          status = otaspx_int_to_ext( &len, &otasp_int, chari );

          if (status == OTASPX_DONE_S) /*lint !e641 */
          {
            send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
          }
          else
          {
            ERR_FATAL( "Failed OTASP message translation",0,0,0);
          }
        }
      }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
      break;
    }

    case OTASP_PROT_CAP_REQ_MSG:
      /* IS-683 OTASP Protocol Capability Request Message */
      process_prot_cap_req(&otasp_msg_ptr->prot_cap_req);
    break;

    case OTASP_SSPR_CONFIG_REQ_MSG:
      /* IS-683A OTASP SSPR Configuration Request Message  */
      process_sspr_config_req(&otasp_msg_ptr->sspr_cfg_req);
    break;

    case OTASP_SSPR_DOWNLOAD_REQ_MSG:
      /* IS-683A OTASP SSPR Download Request Message  */
      process_sspr_dload_req(&otasp_msg_ptr->sspr_dload_req);
    break;

    case OTASP_VALIDATION_REQ_MSG:
      /* IS-683A OTASP Validation Request Message  */
      next_state = process_validation_req(&otasp_msg_ptr->validn_req);
    break;

    case OTASP_OTAPA_REQ_MSG:
      /* IS-683A OTAPA Request Message */
      next_state = process_otapa_req(&otasp_msg_ptr->otapa_req);
    break;

    default:
    break;
  }

  return (next_state);

} /* otasp_process_msg */

/*===========================================================================

FUNCTION OTASP_COMMON_INIT

DESCRIPTION
  This function initializes the OTASP/OTAPA related common variables and
  flags. Should be called in otasp_init and in otasp_otapa_init only.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void otasp_common_init (void)
{
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  byte zeros[NV_SEC_CODE_SIZE];
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
   /* String to check against the SPC to see if it is zero */
  byte i;
   /* index variable */

/* Point at NV PRL information structure.
*/
const prl_hdr_s_type  *prl_hdr_ptr = prl_hdr_nv_get_ptr();

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  M1X_MSG( DCP, LEGACY_MED,
    "Initializing OTASP/OTAPA");
  otasp_commit_in_progress = FALSE;
  (void) rex_clr_sigs( MC_TCB_PTR, MCC_OTASP_COMMIT_SIG );

  /* Initilalize the sp_lock_state to TRUE */
  sp_locked_state = TRUE;


  /* Initialize the Key exchange related flags  */
  a_key_temp_valid = FALSE;
  new_ssd_valid = FALSE;


  /* Initialize the commit structure */
  commit.commit_mob_dir = FALSE;
  commit.commit_ftc_cdma = FALSE;
  commit.commit_ftc_amps = FALSE;
  commit.commit_roam_list = FALSE;
  commit.commit_spc = FALSE;
  commit.commit_imsi_t = FALSE;

  commit.commit_nam_lock = FALSE;

  commit_ehrpd_imsi = FALSE;

  /* Initialize the SID NID pairs */
  for (i = 0; i < OTASPI_SID_NID_MAX ; i++)
  {
    commit.ftc_cdma.pair[i].sid = 0;
    commit.ftc_cdma.pair[i].nid = 0;
  }

  #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (!nv_rtre_use_ruim_for_nam(cdma.curr_nam))
  #endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    /* Get the initial roaming list stats from System Determination */
    otasp_get_prl_stats( &otasp_roam_size, prl_hdr_ptr );
  }


  otasp_dh.state = OTASP_WAIT_FOR_DH;


  otasp_reauth_in_progress = FALSE;

#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (!nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    /* If SPC is all zeros, that means we can unlock the phone immediately */
    if (memcmp( cdma.spc,
                memset( zeros, '0', NV_SEC_CODE_SIZE ),
                NV_SEC_CODE_SIZE ) == 0)
    {
      sp_locked_state = FALSE;

      /* Send OTASP status to CM */
      otasp_report_status_to_cm(CM_OTASP_STATUS_SPL_UNLOCKED);
    }
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

  /* Maximum 6 attempts are allowed in same programming session to validate
   * SPC/SPASM to unlock the phone. Resetting count to 0 if new
   * programming session starts. This will also prevent resetting of
   * validation failure attempts by new programming session if max attempts
   * have already happened and MS did not power cycle. */
  if(otasp_failed_unlock_attempts < (uint8) mcc_otasp_max_unlock_attempts_allowed() )
  {
    otasp_failed_unlock_attempts = 0;
  }

  /* Tell NV which NAM to commit this to */
  commit.nam = cdma.curr_nam;

  /* Initialize AMPS HO Flag */
  otasp_call_ho_from_cdma = FALSE;



  /* Flag indicates whether mobile is calculating auth_otapa */
  otasp_auth_otapa_in_progress = FALSE;

  /* Flag indiates whether auth_otapa is ready */
  otasp_auth_otapa_ready = FALSE;




#ifdef FEATURE_UIM_RUIM

  /* Initialize the indices of the table */
  otasp_ruim_table.req_index = 0;
  otasp_ruim_table.rsp_index = 0;

  otasp_ruim_last_segment = FALSE;

#endif /* FEATURE_UIM_RUIM */

} /* otasp_common_init() */

/*===========================================================================

FUNCTION OTASP_INIT

DESCRIPTION
  This function initializes the OTASP function. Should be called when
  entering CDMA_CONV substate.

  It initializes the following flags as otasp_otapa_init does not:
    otasp_successful_commit
    otasp_otapa_session_occurred

  It sets nam_locked_state to be FALSE.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void otasp_init (void)
{
  /* otasp_common_init initializes OTASP/OTAPA common variables and flags */
  otasp_common_init();

  otasp_successful_commit  = FALSE;


  /* Flag indicates whether an OTAPA session has occurred */
  otasp_otapa_session_occurred = FALSE;

  /* NAM lock status for the active NAM */
  /* If not in OTAPA call, it shall be set FALSE */
  nam_locked_state = FALSE;



} /* otasp_init() */



/*===========================================================================

FUNCTION OTASP_OTAPA_INIT

DESCRIPTION
  This function initializes the Network-initiated OTAPA function.
  Should be called when an OTAPA session starts.

  It does not reset the following flags as otasp_init does:
    otasp_successful_commit
    otasp_otapa_session_occurred

  It sets nam_locked_state to be cdma.nam_lock

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void otasp_otapa_init (void)
{
  /* otasp_common_init initializes OTASP/OTAPA common variables and flags */
  otasp_common_init();

  M1X_MSG( DCP, LEGACY_MED,
    "otapa init");
  /* NAM lock status for the active NAM. */
  nam_locked_state = cdma.nam_lock;

  /* Maximum 6 attempts are allowed in same programming session to validate
   * SPC/SPASM to unlock the phone. Resetting counters to 0 if new
   * programming session starts. This will also prevent resetting of
   * validation failure attempts by new programming session if max attempts
   * have already happened. This is a defensive check. This function
   * is really not called if max attempts have happened and MS did
   * not power cycle. */
  if(otasp_otapa_failed_spc_val_attempts < (uint8) mcc_otasp_max_unlock_attempts_allowed() )
  {
    otasp_otapa_failed_spc_val_attempts = 0;
  }

  if(otasp_otapa_failed_spasm_val_attempts < (uint8) mcc_otasp_max_unlock_attempts_allowed() )
  {
    otasp_otapa_failed_spasm_val_attempts = 0;
  }

} /* otasp_otapa_init() */


/*===========================================================================

FUNCTION OTASP_AHO_HANDOFF

DESCRIPTION
  This function returns whether or not this OTASP call has been handed off
  to an AMPS system.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if current OTASP call has been handed off to an AMPS system,
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean otasp_aho_handoff (void)
{
  return otasp_call_ho_from_cdma;
}

/*===========================================================================

FUNCTION OTASP_SET_AHO_HANDOFF

DESCRIPTION
  This function allows write access to the OTASP AMPS handoff flag.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if current OTASP call has been handed off to an AMPS system,
  FALSE otherwise.

SIDE EFFECTS
  Sets the value of otasp_call_ho_from_cdma.

===========================================================================*/

void otasp_set_aho_handoff (boolean value)
{
  otasp_call_ho_from_cdma = value;
}

#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
/*===========================================================================
FUNCTION OTASP_RELOAD_NAM_PARAMS

DESCRIPTION
   This function updates the cdma structure with newly committed info

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void otasp_reload_nam_params (void)
{

    byte i; /* loop counter */
    /* Variable to store value of memscpy() */
    size_t copy_size;  
    otasp_successful_commit  = TRUE;  

   /* Update the cdma structure to reflect the NV changes */
    if (nv_commit.commit_mob_dir)
    {
      cdma.mob_dir.n_digits = nv_commit.mob_dir.n_digits;
      copy_size = memscpy( cdma.mob_dir.digitn, sizeof(cdma.mob_dir.digitn),
                           nv_commit.mob_dir.digitn,
                     nv_commit.mob_dir.n_digits );
      if( copy_size != nv_commit.mob_dir.n_digits )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          nv_commit.mob_dir.n_digits,
          sizeof(cdma.mob_dir.digitn) );
      }

      nv_commit.commit_mob_dir = FALSE;
      M1X_MSG( DCP, LEGACY_MED,
            "mob_dir committed" );
    }

    if (nv_commit.commit_ftc_cdma)
    {
      /* Save MIN Based IMSI parameters in the cdma_ms structure */
      cdma.imsi_m_addr_num = nv_commit.ftc_cdma.imsi_addr_num;
      cdma.imsi_m_mcc = nv_commit.ftc_cdma.mcc;
      cdma.imsi_m_11_12 = nv_commit.ftc_cdma.imsi_11_12;
      qw_set( cdma.imsi_m_s,(dword)((nv_commit.ftc_cdma.imsi_s2 & 0x0300) >> 8),
            ((nv_commit.ftc_cdma.imsi_s1 & 0x00FFFFFFL) |
             (((dword) nv_commit.ftc_cdma.imsi_s2 & 0x000000FFL) << 24)) );
      cdma.imsi_m_s1 = nv_commit.ftc_cdma.imsi_s1;
      cdma.imsi_m_s2 = nv_commit.ftc_cdma.imsi_s2;

      /* Update modem statistics */
      cdma_stats.state_info.min.min1 = cdma.imsi_m_s1;
      cdma_stats.state_info.min.min2 = cdma.imsi_m_s2;
      cdma_stats.changed_bitmask |= SYS_CDMA_STATE_INFO_BIT_MASK;

      /* Update the operational IMSI params in the cdma structure */
      /* if the MIN based IMSI is currently the operational IMSI */
      if (cdma.imsi_type == CDMA_IMSI_M)
      {
        M1X_MSG( DCP, LEGACY_MED,
            "New IMSI_M is now the operational IMSI" );
        cdma.imsi_s1 = cdma.imsi_m_s1;
        cdma.imsi_s2 = cdma.imsi_m_s2;
        qw_equ( cdma.imsi_s, cdma.imsi_m_s);
        cdma.imsi_11_12 = cdma.imsi_m_11_12;
        cdma.mcc = cdma.imsi_m_mcc;
        cdma.imsi_addr_num = cdma.imsi_m_addr_num;
      }

      /* Save other related info */
      cdma.accolc = nv_commit.ftc_cdma.accolc;
      cdma.mob_term_home = nv_commit.ftc_cdma.mob_term_home;
      cdma.mob_term_for_sid = nv_commit.ftc_cdma.mob_term_for_sid;
      cdma.mob_term_for_nid = nv_commit.ftc_cdma.mob_term_for_nid;

      for (i = 0; i < OTASPI_SID_NID_MAX ; i++)
      {
         cdma.pair[i].sid = nv_commit.ftc_cdma.pair[i].sid;
         cdma.pair[i].nid = nv_commit.ftc_cdma.pair[i].nid;
      }

      nv_commit.commit_ftc_cdma = FALSE;
      M1X_MSG( DCP, LEGACY_MED, 
           "CDMA NAM info committed" );
    }

    if (nv_commit.commit_roam_list)
    {
      nv_commit.commit_roam_list = FALSE;
      M1X_MSG( DCP, LEGACY_MED,
            "Roaming List committed" );

      mcc_roaming_list = nv_pr_list;
      /* update the contents of the buffer */

      /* Remember dimensions of roaming list in case they are requested */
      otasp_get_rl_stats( &otasp_roam_size );
    }

    if (nv_commit.commit_spc)
    {
      nv_commit.commit_spc = FALSE;
      M1X_MSG( DCP, LEGACY_MED, 
           "SPC committed" );

      /* Load the SPC into CDMA structure  */
      copy_size = memscpy (cdma.spc, sizeof(cdma.spc),
                           nv_commit.spc.digits, NV_SEC_CODE_SIZE); 
      if( copy_size != NV_SEC_CODE_SIZE )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          NV_SEC_CODE_SIZE,
          sizeof(cdma.spc) );
      }
    }

    if (nv_commit.commit_imsi_t)
    {
      nv_commit.commit_imsi_t = FALSE;
      M1X_MSG( DCP, LEGACY_MED, 
           "IMSI_T info committed" );

      /* Save True IMSI parameters in the cdma_ms structure */
      cdma.imsi_t_s1 =  nv_commit.imsi_t.s1;
      cdma.imsi_t_s2 =  nv_commit.imsi_t.s2;
      qw_set(cdma.imsi_t_s, (dword)((cdma.imsi_t_s2 >> 8) & 0x03),
                  (((cdma.imsi_t_s2 & 0x00ff) << 24) | cdma.imsi_t_s1));

      cdma.imsi_t_11_12 =  nv_commit.imsi_t.imsi_t_11_12;
      cdma.imsi_t_mcc = nv_commit.imsi_t.mcc;
      cdma.imsi_t_addr_num = nv_commit.imsi_t.addr_num;

      /* Update the operational IMSI params in the cdma structure */
      /* if the True IMSI is currently the operational IMSI */
      if (cdma.imsi_type == CDMA_IMSI_T)
      {
        M1X_MSG( DCP, LEGACY_MED,
             "New IMSI_T is now the operational IMSI" );
        cdma.imsi_s1 = cdma.imsi_t_s1;
        cdma.imsi_s2 = cdma.imsi_t_s2;
        qw_equ(cdma.imsi_s, cdma.imsi_t_s);
        cdma.imsi_11_12 = cdma.imsi_t_11_12;
        cdma.mcc = cdma.imsi_t_mcc;
        cdma.imsi_addr_num = cdma.imsi_t_addr_num;
      }
    } /* end imsi_t commit check */

    if (nv_commit.commit_nam_lock)
    {
       nv_commit.commit_nam_lock = FALSE;
       M1X_MSG( DCP, LEGACY_MED, 
             "NAM_LOCK committed" );
       /* Load the NAM_LOCK into CDMA structure */
       cdma.nam_lock = nv_commit.nam_lock.enabled;
    }

} /* otasp_reload_nam_params */
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

#ifdef FEATURE_UIM_RUIM
/*===========================================================================
FUNCTION OTASP_PROCESS_RUIM_REPORT

DESCRIPTION
   This function processes the report received from UIM task for OTASP
   related messages.

DEPENDENCIES
  None.

RETURN VALUE
  The next CDMA sub state.

SIDE EFFECTS
  None.

===========================================================================*/
word otasp_process_ruim_report
(
   uim_rpt_type* otasp_ruim_rpt,
   /* Pointer to auth report type */
   word next_state
)
{
   byte i,x,y,z;
   byte rsp_index;
   word pos = 0;
   size_t copy_size;
  /* Return value of memscpy function */
#ifdef EFS_HAVE_SYNC
   int res;
#endif /* EFS_HAVE_SYNC */
   mccccim_event_type mccccim_evt;
    /* Event rpt struct for sending primitives to CCIM */

   /* Index of the message whose response is being currently processed */
   rsp_index = otasp_ruim_table.rsp_index;

   /* Number of blocks responded by the card */
   x = otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card;

   /* Number of blocks requested from the card */
   y = otasp_ruim_table.msg[rsp_index].num_blocks_req_card;

   /* Number of blocks requested in OTA message */
   z = otasp_ruim_table.msg[rsp_index].num_blocks_req_msg;

   switch(otasp_ruim_rpt->rpt_type)
   {
      case UIM_COMMIT_R:
      {

         M1X_MSG( DCP, LEGACY_MED,
           " Received UIM_COMMIT_R");

         /* If we were not committing anything and we got this bcos
            of an error, we ignore it */

         if (!otasp_commit_in_progress)
         {
           return(next_state);
         }

         /* Check for the status from the UIM */
         if (otasp_ruim_rpt->rpt_status == UIM_PASS)
         {
             otasp_int.commit_rsp.result_code =
                               otasp_ruim_rpt->rpt.commit.result;

             if (otasp_int.commit_rsp.result_code == OTASP_BLK_ACCEPT)
             {
                /* Set flag to indicate that something has been committed */
                otasp_successful_commit = TRUE;
                otasp_report_status_to_cm(CM_OTASP_STATUS_COMMITTED);
                otasp_report_commit_status_to_cm(FALSE);
             }

             /* If the roaming list was committed successfully in the card */
             if ((commit.commit_roam_list) &&
                   (otasp_ruim_rpt->rpt.commit.result) == OTASP_BLK_ACCEPT)
             {
                if (!mcc_pcs_band_is_supported())
                {
                 /* For 800 MHz CDMA phones, we enable the roaming list and if
                    validation fails, it would be set back to FALSE, while
                    reading the PRL */
                  otasp_nv_item.prl_enabled.nam = cdma.curr_nam;
                  otasp_nv_item.prl_enabled.enabled = TRUE;

                  /* Write to NV that the list should be enabled */
                  otasp_nv_buf.item = NV_PRL_ENABLED_I;
                  otasp_nv_buf.cmd = NV_WRITE_F;
                  otasp_nv_buf.data_ptr = &otasp_nv_item;

                  (void) mcc_nv_cmd( &otasp_nv_buf);
                }

#ifdef EFS_HAVE_SYNC
                if ((!mcc_pcs_band_is_supported()) ||
                    (mcc_cellular_band_is_supported() &&
                     mcc_pcs_band_is_supported()))
                {
                  /* Kick the watchdog */
                  mc_dog_report();

                  M1X_MSG( DCP, LEGACY_MED,
                    "Synchronizing the EFS");
                  res = efs_sync("/");
                  if (res != 0)
                  {
                    ERR_FATAL( "EFS sync failed",0,0,0);
                  }
                  M1X_MSG( DCP, LEGACY_MED,
                    "EFS sync after otasp proc ruim rep is successful.");
                }
#endif /* EFS_HAVE_SYNC */

                /* Read the PRL into the buffer 
                 **  If card mode then read through MMGSDI-API 
                 **  Otherwise use normal NV Read method */
                #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
                /* Determine if the R-UIM is available for this NAM */
                if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
                #endif /* FEATURE_UIM_RUN_TIME_ENABLE */
                {
                  /* Card mode so read NV#257-NV_ROAMING_LIST_683_I using 
                   * mmgsdi_session_read_prl()-API */
                  mcc_mmgsdi_read_prl();
                }
                #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
                else /* R-UIM is not available */
                #endif /* FEATURE_UIM_RUN_TIME_ENABLE */                              
                {
                  /* NV Mode: Read NV#257-NV_ROAMING_LIST_683_I with NV Cmd */ 
                  otasp_nv_buf.cmd = NV_READ_F;
                  otasp_nv_buf.data_ptr   = (nv_item_type*) &nv_pr_list;
                  otasp_nv_buf.item       = NV_ROAMING_LIST_683_I;
                  (void) mcc_nv_cmd( &otasp_nv_buf);
                }                

                /* Update the PRL dimensions structure */
                otasp_get_rl_stats( &otasp_roam_size );

                commit.commit_roam_list = FALSE;

             }
         }
         else
         {
             /* Reject the Commit Request since the R-UIM failed */
             otasp_int.commit_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
         }

         /* Send Commit Response Message */
         send_commit_resp_msg();

         /* Reset the Commit flag */
         otasp_commit_in_progress = FALSE;

         break;
      }

      case UIM_MS_KEY_REQ_R:
      {

         M1X_MSG( DCP, LEGACY_MED,
           " received UIM_MS_KEY_REQ_R");

         if (otasp_ruim_rpt->rpt_status == UIM_PASS)
         {
             otasp_int.ms_key_rsp.result_code =
                                otasp_ruim_rpt->rpt.ms_key.result;
         }
         else
         {
             otasp_int.ms_key_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
         }

         /* Send MS KEY Response Message */
         send_ms_key_resp_msg();

         break;
      }

      case UIM_KEY_GEN_REQ_R:
      {

         M1X_MSG( DCP, LEGACY_MED,
           " received UIM_KEY_GEN_REQ_R");

         if (otasp_ruim_rpt->rpt_status == UIM_PASS)
         {
             otasp_int.key_gen_rsp.result_code =
                    otasp_ruim_rpt->rpt.key_gen.result;
             otasp_int.key_gen_rsp.ms_result_len =
                    otasp_ruim_rpt->rpt.key_gen.ms_result_len ;

             pos = 0;


             /* Flag that the R-UIM has generated a new A-Key */
             a_key_temp_valid = TRUE;

             for (i = 0 ; i < (otasp_ruim_rpt->rpt.key_gen.ms_result_len
                                                 /sizeof(dword)); i++)
             {
                otasp_int.key_gen_rsp.ms_result[i] =
                     b_unpackd( otasp_ruim_rpt->rpt.key_gen.ms_result, pos,
                                         8* sizeof(dword));
                pos += (8 * sizeof(dword)) ;
             }

             /* Send OTASP status to CM */
             if(otasp_ruim_rpt->rpt.key_gen.result == OTASP_BLK_ACCEPT)
             {
               otasp_report_status_to_cm(CM_OTASP_STATUS_AKEY_EXCHANGED);
             }
         }
         else
         {
             otasp_int.key_gen_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
             otasp_int.key_gen_rsp.ms_result_len = 0;
         }

         /* Send the Key Generation Response Message */
         send_key_gen_resp_msg();

         break;
      }

      case UIM_OTAPA_REQ_R:
      {

         M1X_MSG( DCP, LEGACY_MED,
           " received UIM_OTAPA_REQ_R");

         if (otasp_ruim_rpt->rpt_status == UIM_PASS)
         {
             otasp_int.otapa_rsp.result_code = otasp_ruim_rpt->rpt.otapa.result;

             /* Send OTASP status to CM */
             if(cdma.otasp_call == NO_SRV_PROV)
             {
               /* Indicates that OTAPA request message had start_stop value 0 */
               otasp_report_status_to_cm(CM_OTASP_STATUS_OTAPA_STOPPED);
             }

             /* Since this variable was set when we began processing the req,
                we change this back if the result was not successful
                Also this would not have been set to Network initiated if this
                was an OTASP Call and hence there is no fear of resetting this
                variable */

             if (otasp_int.otapa_rsp.result_code != OTASP_BLK_ACCEPT)
             {
                if (cdma.otasp_call == NETWORK_INIT_SRV_PROV)
                {
                   cdma.otasp_call = NO_SRV_PROV;
                }

                /* Flag indicates of an OTAPA session has not occurred */
                otasp_otapa_session_occurred = FALSE;
             }

             /* Send OTASP status to CM */
             if(cdma.otasp_call == NETWORK_INIT_SRV_PROV)
             {
               otasp_report_status_to_cm(CM_OTASP_STATUS_OTAPA_STARTED);
             }

             otasp_int.otapa_rsp.nam_lock_ind =
                       otasp_ruim_rpt->rpt.otapa.nam_lock_ind;

             if (otasp_int.otapa_rsp.nam_lock_ind)
             {
                otasp_int.otapa_rsp.rand_otapa =
                                  otasp_ruim_rpt->rpt.otapa.rand_otapa;
             }
         }
         else
         {
             otasp_int.otapa_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
             otasp_int.otapa_rsp.nam_lock_ind = FALSE;
             if (cdma.otasp_call == NETWORK_INIT_SRV_PROV)
             {
                 cdma.otasp_call = NO_SRV_PROV;
             }

             /* Flag indicates of an OTAPA session has not occurred */
             otasp_otapa_session_occurred = FALSE;
         }
         /* Send OTAPA response message */
         send_otapa_resp_msg();

         break;
      }

      case UIM_VALIDATE_R:
      {

         M1X_MSG( DCP, LEGACY_MED,
           "Received UIM_VALIDATE_R");

         if (otasp_ruim_table.msg[rsp_index].msg_type ==
                                                  OTASP_VALIDATION_REQ_MSG)
         {
            /* fill data from the response into the otaspi structure */
            if (otasp_ruim_rpt->rpt_status == UIM_PASS)
            {
               /* Fill in the ID of the block */
               ruim_otasp_int.validn_rsp.block[x].block_id =
                      otasp_ruim_rpt->rpt.validate.block_id;

               /* Fill in the result of the block */
               ruim_otasp_int.validn_rsp.block[x].result_code =
                      otasp_ruim_rpt->rpt.validate.result;

               if (((otasp_ruim_rpt->rpt.validate.block_id==OTASP_VERIFY_SPC_BLOCK)
                    && (otasp_ruim_rpt->rpt.validate.result==OTASP_BLK_REJ_SPC)))
               {
                 if (cdma.otasp_call == USER_INIT_SRV_PROV)
                 {
                   /* OTASP call, power down if too many failed attempts */
                   if (++otasp_failed_unlock_attempts ==
                                               (uint8) mcc_otasp_max_unlock_attempts_allowed() )
                   {
                     M1X_MSG( DCP, LEGACY_ERROR,
                       "RUIM: %d SPC validation failures, terminate OTASP",
                       otasp_failed_unlock_attempts);

                     /* Send OTASP status to CM */
                     otasp_report_status_to_cm(CM_OTASP_STATUS_SPC_RETRIES_EXCEEDED);

                     /* Send Notification to MCCCCIM to release all CC instances */
                     mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
                     /* All CC instances should already be in release substate */
                     mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_REL_OTASP_ERR;
                     mccccim_process_event( &mccccim_evt );
                     /* No need to check return status in this case */

                     next_state = CDMA_RELEASE;  /*lint !e641 */
                     cdma.rel_orig = FALSE;
                     cdma.otasp_call = NO_SRV_PROV;

                     /* Events report: Release */
                     /* Treat it as BS originated because upper layers are not aware of it */
                     mclog_report_release(MCLOG_RELEASE_BS_ORDER);
                   }
                 } /* USER_INIT_SRV_PROV */

                 else
                 {
                   /* OTAPA call, don't power down */
                   if (++otasp_otapa_failed_spc_val_attempts ==
                                              (uint8) mcc_otasp_max_unlock_attempts_allowed() )
                   {
                     M1X_MSG( DCP, LEGACY_ERROR,
                       "RUIM: %d SPC validation failures, terminate OTAPA",
                       otasp_otapa_failed_spc_val_attempts);
                    /* We don't want to powerdown if OTAPA session occurs
                      ** in the middle of a conversation. */
                     cdma.otasp_call = NO_SRV_PROV;
                   }
                 } /* OTAPA call */

               }


               if (((otasp_ruim_rpt->rpt.validate.block_id==VALIDATE_SPASM)
                    && (otasp_ruim_rpt->rpt.validate.result==OTASP_BLK_REJ_SPASM)))
               {
                 if (cdma.otasp_call == NETWORK_INIT_SRV_PROV)
                 {
                   /* OTAPA call, don't power down */
                   if (++otasp_otapa_failed_spasm_val_attempts ==
                                              (uint8) mcc_otasp_max_unlock_attempts_allowed() )
                   {
                     /* We don't want to powerdown if OTAPA session occurs
                      ** in the middle of a conversation. */
                     M1X_MSG( DCP, LEGACY_ERROR,
                       "RUIM: %d SPASM validation failures, terminate OTAPA",
                       otasp_otapa_failed_spasm_val_attempts);
                     cdma.otasp_call = NO_SRV_PROV;
                   }
                 } /* OTAPA call */
               }

            } /* otasp_ruim_rpt->rpt_status == UIM_PASS */
            else
            {
               ruim_otasp_int.validn_rsp.block[x].block_id =
                        otasp_ruim_rpt->rpt.validate.block_id;

               ruim_otasp_int.validn_rsp.block[x].result_code =
                        OTASP_BLK_REJ_UNKNOWN;
            }

            /* Increment the number of blocks responded */
            ++ (otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card);

            /* Reassign it to the same variable */
            x = otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card;

            /* The below check is to see if we have recd responses to all
              the blocks sent to the card for that message */
            if (y == x )
            {
               /* Check to see if there are any blocks that were not sent
                  to the card */
               if ( y != z)
               {
                  /* We did not send (z-y) the blocks to the card */
                  for (i = 0; i < (z-y); i++)
                  {
                     /* Get the block id from the rejected block id corr
                       to the message */
                     ruim_otasp_int.validn_rsp.block[x+i].block_id =
                         otasp_ruim_table.msg[rsp_index].rej_block_id[i];

                     /* Reject the block */
                     ruim_otasp_int.validn_rsp.block[x+i].result_code =
                                OTASP_BLK_REJ_UNKNOWN;
                  }
               }

               /* Set the number of blocks in the response message */
               ruim_otasp_int.validn_rsp.num_blocks = otasp_ruim_table.
                               msg[rsp_index].num_blocks_req_msg;

               /* send the validation response message */
               send_validn_resp_msg();

               /* Clean up the buffer */
               for (i = 0; i < OTASP_RUIM_MAX_BLOCKS ; i++)
               {
                  otasp_ruim_table.msg[rsp_index].rej_block_id[i] = 0xFF;
               }

               otasp_ruim_table.msg[rsp_index].msg_type = 0xFF;

               otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card = 0;

               otasp_ruim_table.msg[rsp_index].num_blocks_req_card = 0;

               otasp_ruim_table.msg[rsp_index].num_blocks_req_msg = 0;

               /* consider this message as done and increment the response
                 index */
               otasp_ruim_table.rsp_index = (otasp_ruim_table.rsp_index + 1) %
                                OTASP_RUIM_MAX_MSG_BUFS;
            }
         }
         else
         {
             /* This is a pathological case here and wish this did not happen */
             M1X_MSG( DCP, LEGACY_ERROR,
               "OTASP RUIM req-rsp in incorrect order");

             /*  !!!!!!! CODE THIS CASE LATER !!!!!!! */
         }

         break;
      }

      case UIM_CONFIG_REQ_R:
      {
         M1X_MSG( DCP, LEGACY_MED,
           " received UIM_CONFIG_REQ_R");

         if (otasp_ruim_table.msg[rsp_index].msg_type == OTASP_CONFIG_REQ_MSG)
         {
             /* fill data from the response into the otaspi structure */
             if (otasp_ruim_rpt->rpt_status == UIM_PASS)
             {
                /* Fill in the ID of the block */
                ruim_otasp_int.config_rsp.block[x].block_id =
                      otasp_ruim_rpt->rpt.configuration.block_id;

                /* Fill in the result of the block */
                ruim_otasp_int.config_rsp.result_code[x] =
                      otasp_ruim_rpt->rpt.configuration.result;

                if( (otasp_ruim_rpt->rpt.configuration.block_length <= sizeof(otasp_ruim_rpt->rpt.configuration.data)) )
                {
                  /* Fill in the length of the block */
                  ruim_otasp_int.config_rsp.block[x].block_len =
                        otasp_ruim_rpt->rpt.configuration.block_length;
                  /* Fill in the data from the response */
                  copy_size = memscpy(ruim_otasp_int.config_rsp.block[x].blk.ruim_rsp_data,
                                      sizeof(otaspi_ruim_data_type),
                         otasp_ruim_rpt->rpt.configuration.data,
                         otasp_ruim_rpt->rpt.configuration.block_length);
                  if( copy_size != otasp_ruim_rpt->rpt.configuration.block_length )
                  {
                    M1X_MSG( DCP, LEGACY_ERROR,
                      "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                      copy_size,
                      otasp_ruim_rpt->rpt.configuration.block_length,
                      sizeof(otaspi_ruim_data_type));
                    }
                }
                else
                {
                  M1X_MSG( DCP, LEGACY_ERROR,
                    "Unexpected block_length:%d",
                    otasp_ruim_rpt->rpt.configuration.block_length);
                  ruim_otasp_int.config_rsp.block[x].block_len = 0;
                }

                /* There are a few ME specific info that needs to be filled in
                   the Configuration Response Message */
                switch (otasp_ruim_rpt->rpt.configuration.block_id)
                {
                    case OTASP_CDMA_ANALOG_NAM_BLK:

                        /* Pack SCM */
                        b_packb( cdma.scm,ruim_otasp_int.config_rsp.block[x].
                                 blk.ruim_rsp_data, 27,
                                 FSIZ( otaspe_rtc_cell_data_fix_type, scm ));

                        /* Pack mobile station protocol revision number */
                        b_packb( cdma.mob_cai_rev,ruim_otasp_int.config_rsp.
                                 block[x].blk.ruim_rsp_data, 35 ,
                                 FSIZ(otaspe_rtc_cell_data_fix_type,mob_p_rev));

                        /* Pack the local control as 0 since we don't support*/
                        b_packb( 0 ,ruim_otasp_int.config_rsp.
                                 block[x].blk.ruim_rsp_data, 102,
                                 FSIZ( otaspe_rtc_cell_data_fix_type,
                                       local_control ));
                    break;

                    case OTASP_CDMA_NAM_BLK:

                        /* Pack Slotted mode as TRUE */
                        b_packb( 1,ruim_otasp_int.config_rsp.
                                 block[x].blk.ruim_rsp_data, 2 ,
                                 FSIZ(otaspe_rtc_pcs_data_fix_type,
                                      slotted_mode));

                        /* Pack mobile station protocol revision number */
                        b_packb( cdma.mob_cai_rev,ruim_otasp_int.config_rsp.
                                 block[x].blk.ruim_rsp_data, 8 ,
                                 FSIZ(otaspe_rtc_pcs_data_fix_type,mob_p_rev));

                        /* Pack the local control as 0 since we don't support*/
                        b_packb( 0 ,ruim_otasp_int.config_rsp.
                                 block[x].blk.ruim_rsp_data, 75,
                                 FSIZ( otaspe_rtc_pcs_data_fix_type,
                                       local_control ));
                    break;
                } /* end of switch */  /*lint !e744*/
             }
             else
             {
                ruim_otasp_int.config_rsp.block[x].block_id =
                      otasp_ruim_rpt->rpt.configuration.block_id;

                ruim_otasp_int.config_rsp.result_code[x]= OTASP_BLK_REJ_UNKNOWN;

                ruim_otasp_int.config_rsp.block[x].block_len = 0;
             }

             /* Increment the number of blocks responded */
             ++ (otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card);

             /* Reassign it to the same variable */
             x = otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card;

             /* The below check is to see if we have recd responses to all
                the blocks sent to the card for that message */
             if (y == x )
             {
                /* Check to see if there are any blocks that were not sent
                    to the card */
                if ( y != z)
                {
                    /* We did not send (z-y) the blocks to the card */
                    for (i = 0; i < (z-y); i++)
                    {
                        /* Get the block id from the rejected block id corr
                           to the message */
                        ruim_otasp_int.config_rsp.block[x+i].block_id =
                             otasp_ruim_table.msg[rsp_index].rej_block_id[i];

                        /* Reject the block */
                        ruim_otasp_int.config_rsp.result_code[x+i] =
                                    OTASP_BLK_REJ_UNKNOWN;

                        /* Set the block length to zero */
                        ruim_otasp_int.config_rsp.block[x+i].block_len = 0;
                    }
                }

                /* Set the number of blocks in the response message */
                ruim_otasp_int.config_rsp.num_blocks = otasp_ruim_table.
                                   msg[rsp_index].num_blocks_req_msg;

                /* send the Configuration Response message */
                send_cfg_resp_msg();

                /* Clean up the buffer */
                for (i = 0; i < OTASP_RUIM_MAX_BLOCKS ; i++)
                {
                    otasp_ruim_table.msg[rsp_index].rej_block_id[i] = 0xFF;
                }

                otasp_ruim_table.msg[rsp_index].msg_type = 0xFF;

                otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card = 0;

                otasp_ruim_table.msg[rsp_index].num_blocks_req_card = 0;

                otasp_ruim_table.msg[rsp_index].num_blocks_req_msg = 0;

                /* consider this message as done and increment the response
                   index */
                otasp_ruim_table.rsp_index = (otasp_ruim_table.rsp_index + 1) %
                                  OTASP_RUIM_MAX_MSG_BUFS;
             }
         }
         else
         {
             /* This is a pathological case here and wish this did not happen */
             M1X_MSG( DCP, LEGACY_ERROR,
               "OTASP RUIM req-rsp in incorrect order");

             /*  !!!!!!! CODE THIS CASE LATER */
         }

         break;
      }

      case UIM_DOWNLOAD_REQ_R:
      {

         M1X_MSG( DCP, LEGACY_MED,
           " received UIM_DOWNLOAD_REQ_R");

         if (otasp_ruim_table.msg[rsp_index].msg_type == OTASP_DOWNLOAD_REQ_MSG)
         {
             /* fill data from the response into the otaspi structure */
             if (otasp_ruim_rpt->rpt_status == UIM_PASS)
             {
                if ((otasp_ruim_rpt->rpt.download.block_id == OTASP_VERIFY_SPC_BLK) &&
                    (otasp_ruim_rpt->rpt.download.result == OTASP_BLK_REJ_BAD_SPC))
                {
                  if (++otasp_failed_unlock_attempts >= (uint8) mcc_otasp_max_unlock_attempts_allowed() )
                  {
                      /* Send OTASP status to CM */
                      otasp_report_status_to_cm(CM_OTASP_STATUS_SPC_RETRIES_EXCEEDED);

                      /* Send Notification to MCCCCIM to release all CC instances */
                      mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
                      /* All CC instances should already be in release substate */
                      mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_REL_OTASP_ERR;
                      mccccim_process_event( &mccccim_evt );
                      /* No need to check return status in this case */

                      next_state = CDMA_RELEASE;  /*lint !e641 */
                      cdma.rel_orig = FALSE;
                      cdma.otasp_call = NO_SRV_PROV;

                      /* Events report: Release */
                      /* Treat it as BS originated because upper layers are not aware of it */
                      mclog_report_release(MCLOG_RELEASE_BS_ORDER);
                  }
                }
                /* Fill in the ID of the block */
                ruim_otasp_int.dload_rsp.block[x].block_id =
                      otasp_ruim_rpt->rpt.download.block_id;

                /* Fill in the result of the block */
                ruim_otasp_int.dload_rsp.block[x].result_code =
                      otasp_ruim_rpt->rpt.download.result;

               /* Send OTASP status to CM */
               if(otasp_ruim_rpt->rpt.download.result == OTASP_BLK_ACCEPT)
               {
                 switch(otasp_ruim_rpt->rpt.download.block_id)
                 {
                   case OTASP_MOB_DIR_NUM_BLK: /* Mobile Directory Number */
                     otasp_report_status_to_cm(CM_OTASP_STATUS_MDN_DOWNLOADED);
                   break;

                   case OTASP_CDMA_ANALOG_NAM_BLK:
                     /* IS-683 Cellular Indicator Download Data */
                   case OTASP_CDMA_NAM_BLK: /* PCS Indicator Download Data */
                     otasp_report_status_to_cm(CM_OTASP_STATUS_NAM_DOWNLOADED);
                   break;

                   case OTASP_IMSI_T_BLK: /* IMSI_T */
                     otasp_report_status_to_cm(CM_OTASP_STATUS_IMSI_DOWNLOADED);
                   break;
                 }
               } /* end if */

             } /* if (otasp_ruim_rpt->rpt_status == UIM_PASS) */
             else
             {
                ruim_otasp_int.dload_rsp.block[x].block_id =
                      otasp_ruim_rpt->rpt.download.block_id;

                ruim_otasp_int.dload_rsp.block[x].result_code=
                      OTASP_BLK_REJ_UNKNOWN;
             }

             /* Increment the number of blocks responded */
             ++ (otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card);

             /* Reassign it to the same variable */
             x = otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card;

             /* The below check is to see if we have recd responses to all
                the blocks sent to the card for that message */
             if (y == x )
             {
                /* Check to see if there are any blocks that were not sent
                    to the card */
                if ( y != z)
                {
                    /* We did not send (z-y) the blocks to the card */
                    for (i = 0; i < (z-y); i++)
                    {
                        /* Get the block id from the rejected block id corr
                           to the message */
                        ruim_otasp_int.dload_rsp.block[x+i].block_id =
                             otasp_ruim_table.msg[rsp_index].rej_block_id[i];

                        /* Reject the block */
                        ruim_otasp_int.dload_rsp.block[x+i].result_code =
                             OTASP_BLK_REJ_UNKNOWN;
                    }
                }

                /* Set the number of blocks in the response message */
                ruim_otasp_int.dload_rsp.num_blocks = otasp_ruim_table.
                                   msg[rsp_index].num_blocks_req_msg;

                /* send the Download Response message */
                send_dload_resp_msg();

                /* Clean up the buffer */
                for (i = 0; i < OTASP_RUIM_MAX_BLOCKS ; i++)
                {
                    otasp_ruim_table.msg[rsp_index].rej_block_id[i] = 0xFF;
                }

                otasp_ruim_table.msg[rsp_index].msg_type = 0xFF;

                otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card = 0;

                otasp_ruim_table.msg[rsp_index].num_blocks_req_card = 0;

                otasp_ruim_table.msg[rsp_index].num_blocks_req_msg = 0;

                /* consider this message as done and increment the response
                   index */
                otasp_ruim_table.rsp_index = (otasp_ruim_table.rsp_index + 1) %
                                  OTASP_RUIM_MAX_MSG_BUFS;
             }
         }
         else
         {
             /* This is a pathological case here and wish this did not happen */
             M1X_MSG( DCP, LEGACY_ERROR,
               "OTASP RUIM req-rsp in incorrect order");

             /*  !!!!!!! CODE THIS CASE LATER */
         }

         break;
      }

      case UIM_SSPR_CONFIG_REQ_R:
      {
         M1X_MSG( DCP, LEGACY_MED,
           " received UIM_SSPR_CONFIG_REQ_R");

         if (otasp_ruim_table.msg[rsp_index].msg_type ==
                                              OTASP_SSPR_CONFIG_REQ_MSG)
         {
             /* fill data from the response into the otaspi structure */
             if (otasp_ruim_rpt->rpt_status == UIM_PASS)
             {
                /* Fill in the ID of the block */
                ruim_otasp_int.sspr_cfg_rsp.block_id =
                      otasp_ruim_rpt->rpt.sspr_config.block_id;

                /* Fill in the result of the block */
                ruim_otasp_int.sspr_cfg_rsp.result_code =
                      otasp_ruim_rpt->rpt.sspr_config.result;

                /* Fill in the length of the block */
                ruim_otasp_int.sspr_cfg_rsp.block_len =
                      otasp_ruim_rpt->rpt.sspr_config.block_length;

                /* Re-pack NUM_ACQ_RECS and NUM_SYS_RECS fields because card
                 doesn't know 683C PRL and did not fill them in correctly. */
                if ( otasp_ruim_rpt->rpt.sspr_config.block_id ==
                        OTASP_CFG_PRL_DIMENSIONS_BLK )
                {
                  /* Fill NUM_ACQ_RECS and NUM_SYS_RECS fields. See IS-683A
                      3.5.3.1 */

                  /* Initialize the three bytes to zero */
                  otasp_ruim_rpt->rpt.sspr_config.data[PRL_NUM_REC_INDEX] =
                    otasp_ruim_rpt->rpt.sspr_config.data[PRL_NUM_REC_INDEX+1] =
                    otasp_ruim_rpt->rpt.sspr_config.data[PRL_NUM_REC_INDEX+2] =
                    0;

                  /* 7 MSB bits of NUM_ACQ_RECS */
                  otasp_ruim_rpt->rpt.sspr_config.data[PRL_NUM_REC_INDEX] |=
                          (byte)(otasp_roam_size.num_acq_recs >> 2);
                  /* 2 LSB bits of NUM_ACQ_RECS */
                  otasp_ruim_rpt->rpt.sspr_config.data[PRL_NUM_REC_INDEX+1] |=
                          (byte)(otasp_roam_size.num_acq_recs << 6);
                  /* 6 MSB bits of NUM_SYS_RECS */
                  otasp_ruim_rpt->rpt.sspr_config.data[PRL_NUM_REC_INDEX+1] |=
                          (byte)(otasp_roam_size.num_sys_recs >> 8);
                  /* 8 LSB bits of NUM_SYS_RECS */
                  otasp_ruim_rpt->rpt.sspr_config.data[PRL_NUM_REC_INDEX+2] =
                          (byte)otasp_roam_size.num_sys_recs;
                }

                if( (otasp_ruim_rpt->rpt.sspr_config.block_length <= sizeof(otasp_ruim_rpt->rpt.sspr_config.data)) )
                {
                  /* Fill in the data from the response */
                  copy_size = memscpy(ruim_otasp_int.sspr_cfg_rsp.block.blk.ruim_rsp_data,
                                      sizeof(otaspi_ruim_data_type),
                         otasp_ruim_rpt->rpt.sspr_config.data,
                         otasp_ruim_rpt->rpt.sspr_config.block_length);
                  if( copy_size != otasp_ruim_rpt->rpt.sspr_config.block_length )
                  {
                    M1X_MSG( DCP, LEGACY_ERROR,
                      "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                      copy_size,
                      otasp_ruim_rpt->rpt.sspr_config.block_length,
                      sizeof(otaspi_ruim_data_type));
                  }

                }
                else
                {
                  M1X_MSG( DCP, LEGACY_ERROR,
                    "Unexpected block_length:%d",
                    otasp_ruim_rpt->rpt.sspr_config.block_length);
                  /* Fill in the length of the block */
                  ruim_otasp_int.sspr_cfg_rsp.block_len = 0;
                }
             }
             else
             {
                ruim_otasp_int.sspr_cfg_rsp.block_id =
                      otasp_ruim_rpt->rpt.sspr_config.block_id;

                ruim_otasp_int.sspr_cfg_rsp.result_code= OTASP_BLK_REJ_UNKNOWN;

                ruim_otasp_int.sspr_cfg_rsp.block_len = 0;
             }

             /* send the SSPR Configuration Response message */
             send_sspr_cfg_resp_msg();

             /* Clean up the buffer */
             for (i = 0; i < OTASP_RUIM_MAX_BLOCKS ; i++)
             {
                otasp_ruim_table.msg[rsp_index].rej_block_id[i] = 0xFF;
             }

             otasp_ruim_table.msg[rsp_index].msg_type = 0xFF;

             otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card = 0;

             otasp_ruim_table.msg[rsp_index].num_blocks_req_card = 0;

             otasp_ruim_table.msg[rsp_index].num_blocks_req_msg = 0;

             /* consider this message as done and increment the response
                index */
             otasp_ruim_table.rsp_index = (otasp_ruim_table.rsp_index + 1) %
                               OTASP_RUIM_MAX_MSG_BUFS;
         }
         else
         {
             /* This is a pathological case here and wish this did not happen */
             M1X_MSG( DCP, LEGACY_ERROR,
               "OTASP RUIM req-rsp in incorrect order");

             /*  !!!!!!! CODE THIS CASE LATER */
         }

         break;
      }

      case UIM_SSPR_DOWNLOAD_REQ_R:
      {
         M1X_MSG( DCP, LEGACY_MED,
           " received UIM_SSPR_DOWNLOAD_REQ_R");

         if (otasp_ruim_table.msg[rsp_index].msg_type ==
                                         OTASP_SSPR_DOWNLOAD_REQ_MSG)
         {
             /* fill data from the response into the otaspi structure */
             if (otasp_ruim_rpt->rpt_status == UIM_PASS)
             {
                /* Fill in the ID of the block */
                ruim_otasp_int.sspr_dload_rsp.block_id =
                      otasp_ruim_rpt->rpt.sspr_download.block_id;

                /* Fill in the result of the block */
                ruim_otasp_int.sspr_dload_rsp.result_code =
                      otasp_ruim_rpt->rpt.sspr_download.result;

                /* Fill in the segment offset in the response  */
                ruim_otasp_int.sspr_dload_rsp.segment_offset =
                      otasp_ruim_rpt->rpt.sspr_download.segment_offset;

                /* Fill in the length of the block */
                ruim_otasp_int.sspr_dload_rsp.segment_size =
                      otasp_ruim_rpt->rpt.sspr_download.segment_size;

                /* If the response to the last segment was accepted, then
                   we would be committing the PRL when a commit is requested */
                if ((otasp_ruim_last_segment) &&
                    (otasp_ruim_rpt->rpt.sspr_download.result == OTASP_BLK_ACCEPT))
                {
                    commit.commit_roam_list = TRUE;

                    /* Send OTASP status to CM */
                    otasp_report_status_to_cm(CM_OTASP_STATUS_PRL_DOWNLOADED);
                }
             }
             else
             {
                ruim_otasp_int.sspr_dload_rsp.block_id =
                      otasp_ruim_rpt->rpt.sspr_download.block_id;

                ruim_otasp_int.sspr_dload_rsp.result_code=OTASP_BLK_REJ_UNKNOWN;

                ruim_otasp_int.sspr_dload_rsp.segment_offset = 0;

                ruim_otasp_int.sspr_dload_rsp.segment_size = 0;

                commit.commit_roam_list = FALSE;
             }

             /* send the SSPR Download Response message */
             send_sspr_dload_resp_msg();

             /* Clean up the buffer */
             for (i = 0; i < OTASP_RUIM_MAX_BLOCKS ; i++)
             {
                otasp_ruim_table.msg[rsp_index].rej_block_id[i] = 0xFF;
             }

             otasp_ruim_table.msg[rsp_index].msg_type = 0xFF;

             otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card = 0;

             otasp_ruim_table.msg[rsp_index].num_blocks_req_card = 0;

             otasp_ruim_table.msg[rsp_index].num_blocks_req_msg = 0;

             /* consider this message as done and increment the response
                index */
             otasp_ruim_table.rsp_index = (otasp_ruim_table.rsp_index + 1) %
                               OTASP_RUIM_MAX_MSG_BUFS;
         }
         else
         {
             /* This is a pathological case here and wish this did not happen */
             M1X_MSG( DCP, LEGACY_ERROR,
               "OTASP RUIM req-rsp in incorrect order");

             /*  !!!!!!! CODE THIS CASE LATER */
         }

         break;
      }

      default:
      {
         /* we got the wrong report */
         M1X_MSG( DCP, LEGACY_ERROR,
           "Wrong UIM report %d!",
           otasp_ruim_rpt->rpt_type );

         break;
      }
   } /* end of switch */

   /* Place the report buffer back on the free queue. */
   cmd_done( &otasp_ruim_rpt->rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
   mc_free_queue_buf( otasp_ruim_rpt, MC_UIM_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */

   return(next_state);

} /* otasp_process_ruim_report */
#endif /* FEATURE_UIM_RUIM */


#ifdef FEATURE_CSIM

/*===========================================================================
FUNCTION OTASP_PROCESS_MMGSDI_REPORT

DESCRIPTION
   This function processes the report received from MMGSDI for OTASP
   related messages.

DEPENDENCIES
  None.

RETURN VALUE
  The next CDMA sub state.

SIDE EFFECTS
  None.

===========================================================================*/

word otasp_process_mmgsdi_report
(
   mc_mmgsdi_rpt_type *mmgsdi_rpt_ptr,
   word next_state
)
{
  byte i,x,y,z;
  byte rsp_index;
  word pos = 0;
  mccccim_event_type mccccim_evt;
  size_t copy_size;
  /* Return value of memscpy function */
  /* Event rpt struct for sending primitives to CCIM */

  /* Index of the message whose response is being currently processed */
  rsp_index = otasp_ruim_table.rsp_index;

  /* Number of blocks responded by the card */
  x = otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card;

  /* Number of blocks requested from the card */
  y = otasp_ruim_table.msg[rsp_index].num_blocks_req_card;

  /* Number of blocks requested in OTA message */
  z = otasp_ruim_table.msg[rsp_index].num_blocks_req_msg;

  switch (mmgsdi_rpt_ptr->cnf)
  {
    case MMGSDI_SESSION_OTASP_COMMIT_CNF:
    {
      M1X_MSG( DCP, LEGACY_MED,
        " Received MMGSDI_SESSION_OTASP_COMMIT_CNF");
      /* If we were not committing anything and we got this bcos
      of an error, we ignore it */

      if (!otasp_commit_in_progress)
      {
        return(next_state);
      }

      /* Check for the status from the UIM */
      if( mmgsdi_rpt_ptr->cnf_rpt.session_otasp_commit_cnf.response_header.mmgsdi_status ==
         MMGSDI_SUCCESS )
      {
        /* It is assumed that mmgsdi status values of OTASP will be in the range of 8 bits */
        otasp_int.commit_rsp.result_code =
          (byte)mmgsdi_rpt_ptr->cnf_rpt.session_otasp_commit_cnf.response_header.mmgsdi_status;

        if (otasp_int.commit_rsp.result_code == (byte)MMGSDI_SUCCESS)
        {
          /* Set flag to indicate that something has been committed */
          otasp_successful_commit = TRUE;
          otasp_report_status_to_cm(CM_OTASP_STATUS_COMMITTED);
          otasp_report_commit_status_to_cm(FALSE);
        }

        /* If the roaming list was committed successfully in the card */
        if ((commit.commit_roam_list) &&
             (mmgsdi_rpt_ptr->cnf_rpt.session_otasp_commit_cnf.response_header.mmgsdi_status)
              == MMGSDI_SUCCESS)
        {
          if (!mcc_pcs_band_is_supported())
          {
            /* For 800 MHz CDMA phones, we enable the roaming list and if
               validation fails, it would be set back to FALSE, while
               reading the PRL */
            otasp_nv_item.prl_enabled.nam = cdma.curr_nam;
            otasp_nv_item.prl_enabled.enabled = TRUE;

            /* Write to NV that the list should be enabled */
            otasp_nv_buf.item = NV_PRL_ENABLED_I;
            otasp_nv_buf.cmd = NV_WRITE_F;
            otasp_nv_buf.data_ptr = &otasp_nv_item;
            (void) mcc_nv_cmd( &otasp_nv_buf);
          }

          /* Read the PRL into the buffer 
           **  If card mode then read through MMGSDI-API 
           **  Otherwise use normal NV Read method */
          #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
          /* Determine if the R-UIM is available for this NAM */
          if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
          #endif /* FEATURE_UIM_RUN_TIME_ENABLE */
          {
            /* Card mode so read NV#257-NV_ROAMING_LIST_683_I using 
             * mmgsdi_session_read_prl()-API */
            mcc_mmgsdi_read_prl();
          }
          #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
          else /* R-UIM is not available */
          #endif /* FEATURE_UIM_RUN_TIME_ENABLE */                              
          {
            /* NV Mode: Read NV#257-NV_ROAMING_LIST_683_I with NV Cmd */ 
            otasp_nv_buf.cmd = NV_READ_F;
            otasp_nv_buf.data_ptr   = (nv_item_type*) &nv_pr_list;
            otasp_nv_buf.item       = NV_ROAMING_LIST_683_I;
            (void) mcc_nv_cmd( &otasp_nv_buf);
          }          

          /* Update the PRL dimensions structure */
          otasp_get_rl_stats( &otasp_roam_size );

          commit.commit_roam_list = FALSE;
        }
      }
      else
      {
       /* Reject the Commit Request since the R-UIM failed */
        otasp_int.commit_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
      }

      /* Send Commit Response Message */
      send_commit_resp_msg();
      /* Reset the Commit flag */
      otasp_commit_in_progress = FALSE;
      break;
    }
    case MMGSDI_SESSION_OTASP_MS_KEY_CNF:
    {
      M1X_MSG( DCP, LEGACY_MED,
        " received MMGSDI_SESSION_OTASP_MS_KEY_CNF");
      if (mmgsdi_rpt_ptr->cnf_rpt.session_otasp_ms_key_cnf.response_header.mmgsdi_status
           == MMGSDI_SUCCESS )
      {
        otasp_int.ms_key_rsp.result_code =
          mmgsdi_rpt_ptr->cnf_rpt.session_otasp_ms_key_cnf.result;
      }
      else
      {
        otasp_int.ms_key_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
      }
      /* Send MS KEY Response Message */
      send_ms_key_resp_msg();
      break;
    }
    case MMGSDI_SESSION_OTASP_KEY_GEN_CNF:
    {
      M1X_MSG( DCP, LEGACY_MED,
        " received MMGSDI_SESSION_OTASP_KEY_GEN_CNF");
      if (mmgsdi_rpt_ptr->cnf_rpt.session_otasp_key_gen_cnf.response_header.mmgsdi_status
           == MMGSDI_SUCCESS )
      {
        byte num = (byte)(mmgsdi_rpt_ptr->cnf_rpt.session_otasp_key_gen_cnf.ms_result.data_len /
                   sizeof(dword));
        otasp_int.key_gen_rsp.result_code =
          mmgsdi_rpt_ptr->cnf_rpt.session_otasp_key_gen_cnf.result;
        otasp_int.key_gen_rsp.ms_result_len =(byte)
          (mmgsdi_rpt_ptr->cnf_rpt.session_otasp_key_gen_cnf.ms_result.data_len) ;
         pos = 0;


         /* Flag that the R-UIM has generated a new A-Key */
         a_key_temp_valid = TRUE;

         for (i = 0 ; i < num ; i++)
         {
           otasp_int.key_gen_rsp.ms_result[i] =
                b_unpackd( mmgsdi_rpt_ptr->cnf_rpt.session_otasp_key_gen_cnf.ms_result.data_ptr,
                           pos,
                           8* sizeof(dword));
           pos += (8 * sizeof(dword)) ;
         }
         /* Send OTASP status to CM */
         if(mmgsdi_rpt_ptr->cnf_rpt.session_otasp_key_gen_cnf.result == OTASP_BLK_ACCEPT)
         {
           otasp_report_status_to_cm(CM_OTASP_STATUS_AKEY_EXCHANGED);
         }
      }
      else
      {
        otasp_int.key_gen_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
        otasp_int.key_gen_rsp.ms_result_len = 0;
      }
      /* Send the Key Generation Response Message */
      send_key_gen_resp_msg();
      break;
    }
    case MMGSDI_SESSION_OTASP_OTAPA_CNF:
    {
      M1X_MSG( DCP, LEGACY_MED,
        " received MMGSDI_SESSION_OTASP_OTAPA_CNF");
      if (mmgsdi_rpt_ptr->cnf_rpt.session_otasp_otapa_cnf.response_header.mmgsdi_status
           == MMGSDI_SUCCESS)
      {
        otasp_int.otapa_rsp.result_code =
              mmgsdi_rpt_ptr->cnf_rpt.session_otasp_otapa_cnf.result;

        /* Send OTASP status to CM */
        if(cdma.otasp_call == NO_SRV_PROV)
        {
          /* Indicates that OTAPA request message had start_stop value 0 */
          otasp_report_status_to_cm(CM_OTASP_STATUS_OTAPA_STOPPED);
        }

        /* Since this variable was set when we began processing the req,
           we change this back if the result was not successful
           Also this would not have been set to Network initiated if this
           was an OTASP Call and hence there is no fear of resetting this
           variable */

        if (otasp_int.otapa_rsp.result_code != OTASP_BLK_ACCEPT)
        {
          if (cdma.otasp_call == NETWORK_INIT_SRV_PROV)
          {
            cdma.otasp_call = NO_SRV_PROV;
          }
          /* Flag indicates of an OTAPA session has not occurred */
          otasp_otapa_session_occurred = FALSE;
          if (cdma.is_check_bip_otasp_concurrency && cdma.is_start_otasp_stop_bip)
          {
            /* This means we have previously consulted GSTK to proceed with OTASP.
             * When GSTK allowed OTAPS, BIP would be blocked.. Now that we are done
             * with OTASP, unblock BIP by calling GSTK API.
             */
            M1X_MSG( DCP, LEGACY_HIGH,
              "Stopping OTASP, allowing BIP");
            gstk_io_ctrl(GSTK_IO_MANAGE_BIP_OTASP_CONCURRENCY,
                         &cdma.curr_slot_id,
                         &(cdma.is_start_otasp_stop_bip));
            cdma.is_start_otasp_stop_bip = FALSE;
          }
        }

        /* Send OTASP status to CM */
        if(cdma.otasp_call == NETWORK_INIT_SRV_PROV)
        {
          otasp_report_status_to_cm(CM_OTASP_STATUS_OTAPA_STARTED);
        }

        otasp_int.otapa_rsp.nam_lock_ind =
                 mmgsdi_rpt_ptr->cnf_rpt.session_otasp_otapa_cnf.nam_lock_ind;

         if (otasp_int.otapa_rsp.nam_lock_ind)
         {
           otasp_int.otapa_rsp.rand_otapa =
             mmgsdi_rpt_ptr->cnf_rpt.session_otasp_otapa_cnf.rand_otapa;
         }
      }
      else
      {
         otasp_int.otapa_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
         otasp_int.otapa_rsp.nam_lock_ind = FALSE;
         if (cdma.otasp_call == NETWORK_INIT_SRV_PROV)
         {
           cdma.otasp_call = NO_SRV_PROV;
         }

         /* Flag indicates of an OTAPA session has not occurred */
         otasp_otapa_session_occurred = FALSE;
         if (cdma.is_check_bip_otasp_concurrency && cdma.is_start_otasp_stop_bip)
         {
          /* This means we have previously consulted GSTK to proceed with OTASP.
           * When GSTK allowed OTAPS, BIP would be blocked.. Now that we are done
           * with OTASP, unblock BIP by calling GSTK API.
           */
          M1X_MSG( DCP, LEGACY_HIGH,
            "Stopping OTASP, allowing BIP");
          gstk_io_ctrl(GSTK_IO_MANAGE_BIP_OTASP_CONCURRENCY,
                       &cdma.curr_slot_id,
                       &(cdma.is_start_otasp_stop_bip));
          cdma.is_start_otasp_stop_bip = FALSE;
         }
      }
      /* Send OTAPA response message */
      send_otapa_resp_msg();
      break;
    }
    case MMGSDI_SESSION_OTASP_BLOCK_CNF:
    {
      switch ( mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_op_type )
      {
        case MMGSDI_OTASP_DOWNLOAD:
        {
          if (otasp_ruim_table.msg[rsp_index].msg_type == OTASP_DOWNLOAD_REQ_MSG )
          {
            M1X_MSG( DCP, LEGACY_MED,
              " received MMGSDI_OTASP_DOWNLOAD");
            /* fill data from the response into the otaspi structure */
            if(mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.response_header.mmgsdi_status ==
                                                                               MMGSDI_SUCCESS)
            {
              if ( (mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_id
                    == OTASP_VERIFY_SPC_BLK) &&
                   (mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.result
                    == OTASP_BLK_REJ_BAD_SPC))
              {
                if (++otasp_failed_unlock_attempts >= (uint8) mcc_otasp_max_unlock_attempts_allowed() )
                {
                  /* Send OTASP status to CM */
                  otasp_report_status_to_cm(CM_OTASP_STATUS_SPC_RETRIES_EXCEEDED);

                  /* Send Notification to MCCCCIM to release all CC instances */
                  mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
                  /* All CC instances should already be in release substate */
                  mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_REL_OTASP_ERR;
                  mccccim_process_event( &mccccim_evt );
                  /* No need to check return status in this case */
                  next_state = CDMA_RELEASE;  /*lint !e641 */
                  cdma.rel_orig = FALSE;
                  cdma.otasp_call = NO_SRV_PROV;

                  /* Events report: Release */
                  /* Treat it as BS originated because upper layers are not aware of it */
                  mclog_report_release(MCLOG_RELEASE_BS_ORDER);
                }
              }
              /* Fill in the ID of the block */
              ruim_otasp_int.dload_rsp.block[x].block_id =
                   mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_id;

              /* Fill in the result of the block */
               ruim_otasp_int.dload_rsp.block[x].result_code =
                    mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.result;

              /* Send OTASP status to CM */
              if(mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.result == OTASP_BLK_ACCEPT)
              {
                switch(mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_id)
                {
                  case OTASP_MOB_DIR_NUM_BLK: /* Mobile Directory Number */
                  otasp_report_status_to_cm(CM_OTASP_STATUS_MDN_DOWNLOADED);
                  break;

                  case OTASP_CDMA_ANALOG_NAM_BLK:
                    /* IS-683 Cellular Indicator Download Data */
                  case OTASP_CDMA_NAM_BLK: /* PCS Indicator Download Data */
                    otasp_report_status_to_cm(CM_OTASP_STATUS_NAM_DOWNLOADED);
                    break;

                  case OTASP_IMSI_T_BLK: /* IMSI_T */
                    otasp_report_status_to_cm(CM_OTASP_STATUS_IMSI_DOWNLOADED);
                    break;
                 }
               } /* end if */

            } /* if (otasp_ruim_rpt->rpt_status == MMGSDI_SUCCESS) */
            else
            {
              ruim_otasp_int.dload_rsp.block[x].block_id =
                    mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_id;

              ruim_otasp_int.dload_rsp.block[x].result_code=
                    OTASP_BLK_REJ_UNKNOWN;
            }

            /* Increment the number of blocks responded */
            ++ (otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card);

            /* Reassign it to the same variable */
            x = otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card;

            /* The below check is to see if we have recd responses to all
              the blocks sent to the card for that message */
            if (y == x )
            {
              /* Check to see if there are any blocks that were not sent
                  to the card */
              if ( y != z)
              {
                  /* We did not send (z-y) the blocks to the card */
                  for (i = 0; i < (z-y); i++)
                  {
                      /* Get the block id from the rejected block id corr
                         to the message */
                      ruim_otasp_int.dload_rsp.block[x+i].block_id =
                           otasp_ruim_table.msg[rsp_index].rej_block_id[i];

                      /* Reject the block */
                      ruim_otasp_int.dload_rsp.block[x+i].result_code =
                           OTASP_BLK_REJ_UNKNOWN;
                  }
              }

              /* Set the number of blocks in the response message */
              ruim_otasp_int.dload_rsp.num_blocks = otasp_ruim_table.
                                 msg[rsp_index].num_blocks_req_msg;

              /* send the Download Response message */
              send_dload_resp_msg();

              /* Clean up the buffer */
              for (i = 0; i < OTASP_RUIM_MAX_BLOCKS ; i++)
              {
                  otasp_ruim_table.msg[rsp_index].rej_block_id[i] = 0xFF;
              }

              otasp_ruim_table.msg[rsp_index].msg_type = 0xFF;

              otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card = 0;

              otasp_ruim_table.msg[rsp_index].num_blocks_req_card = 0;

              otasp_ruim_table.msg[rsp_index].num_blocks_req_msg = 0;

              /* consider this message as done and increment the response
                 index */
              otasp_ruim_table.rsp_index = (otasp_ruim_table.rsp_index + 1) %
                                OTASP_RUIM_MAX_MSG_BUFS;
            }
          }
          else
          {
           /* This is a pathological case here and wish this did not happen */
           M1X_MSG( DCP, LEGACY_ERROR,
             "OTASP RUIM req-rsp in incorrect order");

           /*  !!!!!!! CODE THIS CASE LATER */
          }
          break;
        }
        case MMGSDI_OTASP_SSPR_DOWNLOAD:
        {
          M1X_MSG( DCP, LEGACY_MED,
            " received MMGSDI_OTASP_SSPR_DOWNLOAD");

          if (otasp_ruim_table.msg[rsp_index].msg_type ==
                                         OTASP_SSPR_DOWNLOAD_REQ_MSG )
          {
            /* fill data from the response into the otaspi structure */
            if ( mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.response_header.mmgsdi_status
              == MMGSDI_SUCCESS)
            {
              /* Fill in the ID of the block */
              ruim_otasp_int.sspr_dload_rsp.block_id =
                    mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_id;

              /* Fill in the result of the block */
              ruim_otasp_int.sspr_dload_rsp.result_code =
                    mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.result;

              /* Fill in the segment offset in the response  */
              ruim_otasp_int.sspr_dload_rsp.segment_offset =
                    mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.segment_offset;

              /* Fill in the length of the block */
              ruim_otasp_int.sspr_dload_rsp.segment_size =
                    mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.segment_size;

              /* If the response to the last segment was accepted, then
                 we would be committing the PRL when a commit is requested */
              if ((otasp_ruim_last_segment) &&
                  (mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.result == OTASP_BLK_ACCEPT))
              {
                  commit.commit_roam_list = TRUE;

                  /* Send OTASP status to CM */
                  otasp_report_status_to_cm(CM_OTASP_STATUS_PRL_DOWNLOADED);
              }
            }
            else
            {
              ruim_otasp_int.sspr_dload_rsp.block_id =
                    mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_id;

              ruim_otasp_int.sspr_dload_rsp.result_code=OTASP_BLK_REJ_UNKNOWN;

              ruim_otasp_int.sspr_dload_rsp.segment_offset = 0;

              ruim_otasp_int.sspr_dload_rsp.segment_size = 0;

              commit.commit_roam_list = FALSE;
            }

            /* send the SSPR Download Response message */
            send_sspr_dload_resp_msg();

            /* Clean up the buffer */
            for (i = 0; i < OTASP_RUIM_MAX_BLOCKS ; i++)
            {
              otasp_ruim_table.msg[rsp_index].rej_block_id[i] = 0xFF;
            }

            otasp_ruim_table.msg[rsp_index].msg_type = 0xFF;

            otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card = 0;

            otasp_ruim_table.msg[rsp_index].num_blocks_req_card = 0;

            otasp_ruim_table.msg[rsp_index].num_blocks_req_msg = 0;

            /* consider this message as done and increment the response
              index */
            otasp_ruim_table.rsp_index = (otasp_ruim_table.rsp_index + 1) %
                               OTASP_RUIM_MAX_MSG_BUFS;
          }
          else
          {
              /* This is a pathological case here and wish this did not happen */
              M1X_MSG( DCP, LEGACY_ERROR,
                "OTASP RUIM req-rsp in incorrect order");

              /*  !!!!!!! CODE THIS CASE LATER */
          }
          break;
        }
        case MMGSDI_OTASP_VALIDATE:
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Received MMGSDI_OTASP_VALIDATE");

          if (otasp_ruim_table.msg[rsp_index].msg_type ==
                                                   OTASP_VALIDATION_REQ_MSG)
          {
             /* fill data from the response into the otaspi structure */
             if (mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS)
             {
                /* Fill in the ID of the block */
                ruim_otasp_int.validn_rsp.block[x].block_id =
                       mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_id;

                /* Fill in the result of the block */
                ruim_otasp_int.validn_rsp.block[x].result_code =
                       mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.result;

                if (((mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_id==OTASP_VERIFY_SPC_BLOCK)
                     && (mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.result==OTASP_BLK_REJ_SPC)))
                {
                  if (cdma.otasp_call == USER_INIT_SRV_PROV)
                  {
                    /* OTASP call, power down if too many failed attempts */
                    if (++otasp_failed_unlock_attempts ==
                                                (uint8) mcc_otasp_max_unlock_attempts_allowed() )
                    {
                      M1X_MSG( DCP, LEGACY_ERROR,
                        "RUIM: %d SPC validation failures, terminate OTASP",
                        otasp_failed_unlock_attempts);

                      /* Send OTASP status to CM */
                      otasp_report_status_to_cm(CM_OTASP_STATUS_SPC_RETRIES_EXCEEDED);

                      /* Send Notification to MCCCCIM to release all CC instances */
                      mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
                      /* All CC instances should already be in release substate */
                      mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_REL_OTASP_ERR;
                      mccccim_process_event( &mccccim_evt );
                      /* No need to check return status in this case */

                      next_state = CDMA_RELEASE;  /*lint !e641 */
                      cdma.rel_orig = FALSE;
                      cdma.otasp_call = NO_SRV_PROV;

                      /* Events report: Release */
                      /* Treat it as BS originated because upper layers are not aware of it */
                      mclog_report_release(MCLOG_RELEASE_BS_ORDER);
                    }
                  } /* USER_INIT_SRV_PROV */

                  else
                  {
                    /* OTAPA call, don't power down */
                    if (++otasp_otapa_failed_spc_val_attempts ==
                                               (uint8) mcc_otasp_max_unlock_attempts_allowed() )
                    {
                      /* We don't want to powerdown if OTAPA session occurs
                       ** in the middle of a conversation. */
                      cdma.otasp_call = NO_SRV_PROV;
                    }
                  } /* OTAPA call */

                }


                if (((mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_id==VALIDATE_SPASM)
                     && (mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.result==OTASP_BLK_REJ_SPASM)))
                {
                  if (cdma.otasp_call == NETWORK_INIT_SRV_PROV)
                  {
                    /* OTAPA call, don't power down */
                    if (++otasp_otapa_failed_spasm_val_attempts ==
                                               (uint8) mcc_otasp_max_unlock_attempts_allowed() )
                    {
                      /* We don't want to powerdown if OTAPA session occurs
                       ** in the middle of a conversation. */
                      M1X_MSG( DCP, LEGACY_ERROR,
                        "RUIM: %d SPASM validation failures, terminate OTAPA",
                        otasp_otapa_failed_spasm_val_attempts);
                      cdma.otasp_call = NO_SRV_PROV;
                    }
                  } /* OTAPA call */
                }

             } /* otasp_ruim_rpt->rpt_status == MMGSDI_SUCCESS */
             else
             {
                ruim_otasp_int.validn_rsp.block[x].block_id =
                         mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_id;

                ruim_otasp_int.validn_rsp.block[x].result_code =
                         OTASP_BLK_REJ_UNKNOWN;
             }

             /* Increment the number of blocks responded */
             ++ (otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card);

             /* Reassign it to the same variable */
             x = otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card;

             /* The below check is to see if we have recd responses to all
               the blocks sent to the card for that message */
             if (y == x )
             {
                /* Check to see if there are any blocks that were not sent
                   to the card */
                if ( y != z)
                {
                   /* We did not send (z-y) the blocks to the card */
                   for (i = 0; i < (z-y); i++)
                   {
                      /* Get the block id from the rejected block id corr
                        to the message */
                      ruim_otasp_int.validn_rsp.block[x+i].block_id =
                          otasp_ruim_table.msg[rsp_index].rej_block_id[i];

                      /* Reject the block */
                      ruim_otasp_int.validn_rsp.block[x+i].result_code =
                                 OTASP_BLK_REJ_UNKNOWN;
                   }
                }

                /* Set the number of blocks in the response message */
                ruim_otasp_int.validn_rsp.num_blocks = otasp_ruim_table.
                                msg[rsp_index].num_blocks_req_msg;

                /* send the validation response message */
                send_validn_resp_msg();

                /* Clean up the buffer */
                for (i = 0; i < OTASP_RUIM_MAX_BLOCKS ; i++)
                {
                   otasp_ruim_table.msg[rsp_index].rej_block_id[i] = 0xFF;
                }

                otasp_ruim_table.msg[rsp_index].msg_type = 0xFF;

                otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card = 0;

                otasp_ruim_table.msg[rsp_index].num_blocks_req_card = 0;

                otasp_ruim_table.msg[rsp_index].num_blocks_req_msg = 0;

                /* consider this message as done and increment the response
                  index */
                otasp_ruim_table.rsp_index = (otasp_ruim_table.rsp_index + 1) %
                                 OTASP_RUIM_MAX_MSG_BUFS;
             }
          }
          else
          {
              /* This is a pathological case here and wish this did not happen */
              M1X_MSG( DCP, LEGACY_ERROR,
                "OTASP RUIM req-rsp in incorrect order");

              /*  !!!!!!! CODE THIS CASE LATER !!!!!!! */
          }


          break;
        }
        case MMGSDI_OTASP_CONFIG:
        {
          M1X_MSG( DCP, LEGACY_MED,
            " received MMGSDI_OTASP_CONFIG");
          if (otasp_ruim_table.msg[rsp_index].msg_type == OTASP_CONFIG_REQ_MSG)
          {
            /* fill data from the response into the otaspi structure */
            if (mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS)
            {
               /* Fill in the ID of the block */
               ruim_otasp_int.config_rsp.block[x].block_id =
                     mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_id;

               /* Fill in the result of the block */
               ruim_otasp_int.config_rsp.result_code[x] =
                     mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.result;

               /* Fill in the length of the block */
               ruim_otasp_int.config_rsp.block[x].block_len = (byte)
                     (mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_len);

               /* Copy the data only when data_len is not zero */
               if(mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_len != 0)
               {
                 if( mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_len <= sizeof(ruim_otasp_int.config_rsp.block[x].blk.ruim_rsp_data) )
                 {
                   /* Fill in the data from the response */
                   copy_size = memscpy
                                 (ruim_otasp_int.config_rsp.block[x].blk.ruim_rsp_data,
                                  sizeof(otaspi_ruim_data_type),
                          mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_ptr,
                                  mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_len
                                 );
                   if( copy_size != mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_len )
                   {
                     M1X_MSG( DCP, LEGACY_ERROR,
                       "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                       copy_size,
                       mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_len,
                       sizeof(otaspi_ruim_data_type));
                   }
                 }
                 else
                 {
                   M1X_MSG( DCP, LEGACY_ERROR,
                     "Unexpected block_length:%d",
                     mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_len);
                   ruim_otasp_int.config_rsp.block[x].block_len = 0;
                 }

                 /* There are a few ME specific info that needs to be filled in
                    the Configuration Response Message */
                 switch (mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_id)
                 {
                   case OTASP_CDMA_ANALOG_NAM_BLK:
                   {
                     /* Pack SCM */
                     b_packb( cdma.scm,ruim_otasp_int.config_rsp.block[x].
                              blk.ruim_rsp_data, 27,
                              FSIZ( otaspe_rtc_cell_data_fix_type, scm ));

                     /* Pack mobile station protocol revision number */
                     b_packb( cdma.mob_cai_rev,ruim_otasp_int.config_rsp.
                              block[x].blk.ruim_rsp_data, 35 ,
                              FSIZ(otaspe_rtc_cell_data_fix_type,mob_p_rev));
                     /* Pack the local control as 0 since we don't support*/
                     b_packb( 0 ,ruim_otasp_int.config_rsp.
                              block[x].blk.ruim_rsp_data, 102,
                              FSIZ( otaspe_rtc_cell_data_fix_type,
                                    local_control ));
                     break;
                   }
                   case OTASP_CDMA_NAM_BLK:
                   {
                     /* Pack Slotted mode as TRUE */
                     b_packb( 1,ruim_otasp_int.config_rsp.
                              block[x].blk.ruim_rsp_data, 2 ,
                              FSIZ(otaspe_rtc_pcs_data_fix_type,
                              slotted_mode));
                     /* Pack mobile station protocol revision number */
                     b_packb( cdma.mob_cai_rev,ruim_otasp_int.config_rsp.
                              block[x].blk.ruim_rsp_data, 8 ,
                              FSIZ(otaspe_rtc_pcs_data_fix_type,mob_p_rev));

                     /* Pack the local control as 0 since we don't support*/
                     b_packb( 0 ,ruim_otasp_int.config_rsp.
                              block[x].blk.ruim_rsp_data, 75,
                              FSIZ( otaspe_rtc_pcs_data_fix_type,
                                    local_control ));
                     break;
                   }
                   default:
                     M1X_MSG( DCP, LEGACY_ERROR,
                       "Invalid Block ID");
                     break;
                 } /* end of switch */  /*lint !e744*/
               }
            }
            else
            {
              ruim_otasp_int.config_rsp.block[x].block_id =
                   mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_id;

              ruim_otasp_int.config_rsp.result_code[x]= OTASP_BLK_REJ_UNKNOWN;

              ruim_otasp_int.config_rsp.block[x].block_len = 0;
            }

            /* Increment the number of blocks responded */
            ++ (otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card);

            /* Reassign it to the same variable */
            x = otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card;
            /* The below check is to see if we have recd responses to all
               the blocks sent to the card for that message */
            if (y == x )
            {
              /* Check to see if there are any blocks that were not sent
                 to the card */
              if ( y != z)
              {
                /* We did not send (z-y) the blocks to the card */
                for (i = 0; i < (z-y); i++)
                {
                  /* Get the block id from the rejected block id corr
                     to the message */
                  ruim_otasp_int.config_rsp.block[x+i].block_id =
                    otasp_ruim_table.msg[rsp_index].rej_block_id[i];

                  /* Reject the block */
                  ruim_otasp_int.config_rsp.result_code[x+i] =
                    OTASP_BLK_REJ_UNKNOWN;

                  /* Set the block length to zero */
                  ruim_otasp_int.config_rsp.block[x+i].block_len = 0;
                }
              }

              /* Set the number of blocks in the response message */
              ruim_otasp_int.config_rsp.num_blocks = otasp_ruim_table.
                                  msg[rsp_index].num_blocks_req_msg;

              /* send the Configuration Response message */
              send_cfg_resp_msg();

              /* Clean up the buffer */
              for (i = 0; i < OTASP_RUIM_MAX_BLOCKS ; i++)
              {
                otasp_ruim_table.msg[rsp_index].rej_block_id[i] = 0xFF;
              }

              otasp_ruim_table.msg[rsp_index].msg_type = 0xFF;

              otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card = 0;

              otasp_ruim_table.msg[rsp_index].num_blocks_req_card = 0;

              otasp_ruim_table.msg[rsp_index].num_blocks_req_msg = 0;

              /* consider this message as done and increment the response
                 index */
              otasp_ruim_table.rsp_index = (otasp_ruim_table.rsp_index + 1) %
                                   OTASP_RUIM_MAX_MSG_BUFS;
            }
          }
          else
          {
              /* This is a pathological case here and wish this did not happen */
              M1X_MSG( DCP, LEGACY_ERROR,
                "OTASP RUIM req-rsp in incorrect order");

              /*  !!!!!!! CODE THIS CASE LATER */
          }
          break;
        }
        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Block Op Type");
      }
      break;
    }
    case MMGSDI_SESSION_OTASP_SSPR_CONFIG_CNF:
    {
      M1X_MSG( DCP, LEGACY_MED,
        " received MMGSDI_SESSION_OTASP_SSPR_CONFIG_CNF");

      if (otasp_ruim_table.msg[rsp_index].msg_type ==
                                          OTASP_SSPR_CONFIG_REQ_MSG)
      {
         /* fill data from the response into the otaspi structure */
         if (mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS)
         {
           /* Fill in the ID of the block */
           ruim_otasp_int.sspr_cfg_rsp.block_id =
                 mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_id;

           /* Fill in the result of the block */
           ruim_otasp_int.sspr_cfg_rsp.result_code =
                 mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.result;

           /* Fill in the length of the block */
           ruim_otasp_int.sspr_cfg_rsp.block_len = (byte)
                 (mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_len);

           /* Re-pack NUM_ACQ_RECS and NUM_SYS_RECS fields because card
            doesn't know 683C PRL and did not fill them in correctly. */
           if( (mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_id ==
                   OTASP_CFG_PRL_DIMENSIONS_BLK ) &&
               (mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_len != 0)
             )
           {
             /* Fill NUM_ACQ_RECS and NUM_SYS_RECS fields. See IS-683A
                 3.5.3.1 */

             /* Initialize the three bytes to zero */
             mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_ptr[PRL_NUM_REC_INDEX] =
             mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_ptr[PRL_NUM_REC_INDEX+1] =
             mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_ptr[PRL_NUM_REC_INDEX+2] =
             0;

             /* 7 MSB bits of NUM_ACQ_RECS */
             mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_ptr[PRL_NUM_REC_INDEX] |=
                     (byte)(otasp_roam_size.num_acq_recs >> 2);
             /* 2 LSB bits of NUM_ACQ_RECS */
             mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_ptr[PRL_NUM_REC_INDEX+1] |=
                     (byte)(otasp_roam_size.num_acq_recs << 6);
             /* 6 MSB bits of NUM_SYS_RECS */
             mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_ptr[PRL_NUM_REC_INDEX+1] |=
                     (byte)(otasp_roam_size.num_sys_recs >> 8);
             /* 8 LSB bits of NUM_SYS_RECS */
             mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_ptr[PRL_NUM_REC_INDEX+2] =
                     (byte)otasp_roam_size.num_sys_recs;
           }
           if(mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_len != 0)
           {
             if( mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_len <= sizeof(ruim_otasp_int.sspr_cfg_rsp.block.blk.ruim_rsp_data) )
             {
               /* Fill in the data from the response */
               copy_size = memscpy(ruim_otasp_int.sspr_cfg_rsp.block.blk.ruim_rsp_data,
                                   sizeof(otaspi_ruim_data_type),
                      mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_ptr,
                      mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_len);
               if( copy_size != mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_len )
               {
                 M1X_MSG( DCP, LEGACY_ERROR,
                   "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                   copy_size,
                   mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_len,
                   sizeof(otaspi_ruim_data_type));
               }
             }
             else
             {
               M1X_MSG( DCP, LEGACY_ERROR,
                 "Unexpected block_length:%d",
                 mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_len);
               ruim_otasp_int.sspr_cfg_rsp.block_len = 0;
             }
           }
         }
         else
         {
            ruim_otasp_int.sspr_cfg_rsp.block_id =
                  mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_id;

            ruim_otasp_int.sspr_cfg_rsp.result_code= OTASP_BLK_REJ_UNKNOWN;

            ruim_otasp_int.sspr_cfg_rsp.block_len = 0;
         }

         /* send the SSPR Configuration Response message */
         send_sspr_cfg_resp_msg();

         /* Clean up the buffer */
         for (i = 0; i < OTASP_RUIM_MAX_BLOCKS ; i++)
         {
            otasp_ruim_table.msg[rsp_index].rej_block_id[i] = 0xFF;
         }

         otasp_ruim_table.msg[rsp_index].msg_type = 0xFF;

         otasp_ruim_table.msg[rsp_index].num_blocks_rsp_card = 0;

         otasp_ruim_table.msg[rsp_index].num_blocks_req_card = 0;

         otasp_ruim_table.msg[rsp_index].num_blocks_req_msg = 0;

         /* consider this message as done and increment the response
            index */
         otasp_ruim_table.rsp_index = (otasp_ruim_table.rsp_index + 1) %
                           OTASP_RUIM_MAX_MSG_BUFS;
      }
      else
      {
         /* This is a pathological case here and wish this did not happen */
         M1X_MSG( DCP, LEGACY_ERROR,
           "OTASP RUIM req-rsp in incorrect order");

         /*  !!!!!!! CODE THIS CASE LATER */
      }
      break;
    }
    default:
    {
      /* we got the wrong report */
      M1X_MSG( DCP, LEGACY_ERROR,
        "Wrong MMGSDI report !" );
      break;
    }
  } /* end of switch */

/* Free the dynamically allocated memory for the deep copy of
 * MMGSDI Rpt if required */
    switch(mmgsdi_rpt_ptr->cnf)
    {
      case MMGSDI_SESSION_OTASP_KEY_GEN_CNF:
      {
        if(mmgsdi_rpt_ptr->cnf_rpt.session_otasp_key_gen_cnf.ms_result.data_len > 0)
        {
          if(mmgsdi_rpt_ptr->cnf_rpt.session_otasp_key_gen_cnf.ms_result.data_ptr != NULL)
          {
            M1X_MSG( DCP, LEGACY_MED,
              "MMGSDI_SESSION_OTASP_KEY_GEN_CNF rpt, free mem for:0X%x",
              mmgsdi_rpt_ptr->cnf_rpt.session_otasp_key_gen_cnf.ms_result.data_ptr);
            modem_mem_free(
             mmgsdi_rpt_ptr->cnf_rpt.session_otasp_key_gen_cnf.ms_result.data_ptr,
             MODEM_MEM_CLIENT_1X
                          );
            mmgsdi_rpt_ptr->cnf_rpt.session_otasp_key_gen_cnf.ms_result.data_ptr = NULL;
          } /* End if(data_ptr != NULL) */
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Data ptr is NULL for Session Otasp key gen cnf rpt and data_len:%d",
              mmgsdi_rpt_ptr->cnf_rpt.session_otasp_key_gen_cnf.ms_result.data_len);
          }
        } /* End if (data_len > 0) */
        break;
      } /* MMGSDrpt_ptrI_SESSION_OTASP_KEY_GEN_CNF */
      case MMGSDI_SESSION_OTASP_BLOCK_CNF:
      {
        switch(mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_op_type)
        {
          case MMGSDI_OTASP_CONFIG:
          {
            if(mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_len > 0)
            {
              if(mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_ptr != NULL)
              {
                M1X_MSG( DCP, LEGACY_MED,
                  "MMGSDI_SESSION_OTASP_BLOCK_CNF rpt, free mem for:0X%x",
                  mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_ptr);
                modem_mem_free(
                 mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_ptr,
                 MODEM_MEM_CLIENT_1X
                              );
                mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_ptr = NULL;
              } /* End if(data_ptr != NULL) */
              else
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                  "Data ptr is NULL for Session Otasp block cnf rpt and data_len:%d",
                  mmgsdi_rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_len);
              }
            } /* End if(data_len > 0) */
            break;
          } /* MMGSDI_OTASP_CONFIG */
          default:
          {
            break;
          }
        } /* End of switch(block_op_type) */
        break;
      } /* MMGSDI_SESSION_OTASP_BLOCK_CNF */
      case MMGSDI_SESSION_OTASP_SSPR_CONFIG_CNF:
      {
        if(mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_len > 0)
        {
          if(mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_ptr != NULL)
          {
            M1X_MSG( DCP, LEGACY_MED,
              "MMGSDI_SESSION_OTASP_SSPR_CONFIG_CNF rpt, free mem for:0X%x",
              mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_ptr);
            modem_mem_free(
             mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_ptr,
             MODEM_MEM_CLIENT_1X
                          );
             mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_ptr = NULL;
          } /* End if(data_ptr != NULL) */
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Data ptr is NULL for Session Otasp sspr config cnf rpt and data_len:%d",
              mmgsdi_rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_len);
          }
        } /* End if(data_len > 0) */
        break;
      } /* MMGSDI_SESSION_OTASP_SSPR_CONFIG_CNF */
      default:
      {
        break;
      }
    } /* End of switch(rpt_ptr->cnf) */
/* End of Free the dynamically allocated memory for the deep copy of
 * MMGSDI Rpt if required */

   /* Place the report buffer back on the free queue. */
   cmd_done( &mmgsdi_rpt_ptr->rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
   mc_free_queue_buf( mmgsdi_rpt_ptr, MC_MMGSDI_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */

   return(next_state);

} /* otasp_process_mmgsdi_report */
#endif

/*===========================================================================

FUNCTION SEND_OTAPA_RESP_MSG

DESCRIPTION
   This function sends the OTAPA Response Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_otapa_resp_msg(void)
{
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 *//* Packed OTASP chari data */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Translate and send the OTAPA Response Message */

  otasp_int.otapa_rsp.msg_type = OTASP_OTAPA_RSP_MSG;

  status = otaspx_int_to_ext( &len, &otasp_int, chari );
  if (status == OTASPX_DONE_S) /*lint !e641 */
  {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
  }
  else
  {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
  }
}  /* send_otapa_resp_msg */

/*===========================================================================

FUNCTION PROCESS_OTAPA_REQ

DESCRIPTION
  This function processes an OTAPA Request Message, formats the
  appropriate OTAPA Response Message, then calls the
  routine to send it.

DEPENDENCIES
  None.

RETURN VALUE
  Next state to return. CDMA_CONV or CDMA_EXIT for power down.

SIDE EFFECTS
  None.

===========================================================================*/

word process_otapa_req
(
  otaspi_otapa_req_msg_type* otapa_req_ptr
    /* Pointer to OTAPA Request Message */
)
{
  word next_state = CDMA_TRAFFIC; /*lint !e641 */
#ifdef FEATURE_UIM_RUIM
#ifdef FEATURE_CSIM
  mmgsdi_return_enum_type mmgsdi_status;
  byte start_stop;
#else
  /* UIM command pointer */
  uim_cmd_type *uim_cmd_ptr;
#endif
  /* Indicates if the OTAPA command is sent to the R-UIM */
  boolean sent_otapa_command = TRUE;
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  word status;  /* Translation status */
  word len; /* Length of packed OTASP chari data */
  byte chari[CAI_REV_TC_BURST_MAX]; /*lint !e834 */ /* Packed OTASP chari data */
  auth_cmd_type *auth_cmd_ptr;
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

  /* Check if we are in User Intiated OTASP session / if OTAPA is not allowed
   */
  if ( (cdma.otasp_call == USER_INIT_SRV_PROV)
       || !cdma.otapa_enabled) /* OTAPA is disabled for this NAM */
  {
    /* OTAPA req not supported in user intiated OTASP session, hence reject it
     * even before processing anything else.
     */
    M1X_MSG( DCP, LEGACY_ERROR,
      "otapa not supported in this mode");
    otasp_int.otapa_rsp.result_code = OTASP_BLK_REJ_MODE;
    otasp_int.otapa_rsp.nam_lock_ind = FALSE;
    /* Send OTAPA response message as Rejected */
    send_otapa_resp_msg();
    /* Indicates the the OTAPA command was not sent to the R-UIM */
    sent_otapa_command = FALSE;
    return (next_state);
  }

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    M1X_MSG( DCP, LEGACY_MED,
      "process otapa_req_msg");

    if (!otasp_is_otasp_allowed())
    {
      otasp_int.otapa_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
      otasp_int.otapa_rsp.nam_lock_ind = FALSE;
      /* Send OTAPA response message as Rejected */
      send_otapa_resp_msg();
      /* Indicates the the OTAPA command was not sent to the R-UIM */
      sent_otapa_command = FALSE;
      return (next_state);
    }
    else
    {
      /* Initialize OTASP OTAPA parameters */
      otasp_otapa_init();
    }

    if ((cdma.otasp_call != USER_INIT_SRV_PROV) &&
        (otapa_req_ptr->start_stop ))
    {
        /* Set the NAM lock state for an OTAPA Call */
        otasp_set_ruim_nam_lock_state( NETWORK_INIT_SRV_PROV );
    }
    #ifdef FEATURE_CSIM
    if (otapa_req_ptr->start_stop)
    {
      start_stop = 0x80;
    }
    else
    {
      start_stop = 0;
    }
    #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
    /* Don't send MMGSDI request if Sim Busy event has been received */  
    if (mc_get_sim_busy_status())
    {
      M1X_MSG( DCP, LEGACY_HIGH, 
               "SIM Busy, Otapa request failed" );
      otasp_int.otapa_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
      otasp_int.otapa_rsp.nam_lock_ind = FALSE;
      /* Send OTAPA response message as Rejected */
      send_otapa_resp_msg();
      /* Indicates the the OTAPA command was not sent to the R-UIM */
      sent_otapa_command = FALSE;
    }
    else
    #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
    { 
  
      mmgsdi_status = mmgsdi_session_otasp_otapa_req( cdma.session_id,
                                               start_stop,
                                               mc_sec_ran_next(),
                                               cdma.esn,
                                               otasp_mmgsdi_response_cb,
                                               0 );
      if(mmgsdi_status != MMGSDI_SUCCESS)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Bad status from MMGSDI status = %d",
          mmgsdi_status);
        otasp_int.otapa_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
        otasp_int.otapa_rsp.nam_lock_ind = FALSE;
        /* Send OTAPA response message as Rejected */
        send_otapa_resp_msg();
        /* Indicates the the OTAPA command was not sent to the R-UIM */
        sent_otapa_command = FALSE;
      }
    }
    #else
    /* The R-UIM card would perform the key exchange operation */
    if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
    {
       uim_cmd_ptr->hdr.command = UIM_OTAPA_REQ_F;

       if (otapa_req_ptr->start_stop)
       {
          uim_cmd_ptr->otapa_req.start_stop = 0x80;
       }
       else
       {
          uim_cmd_ptr->otapa_req.start_stop = 0;
       }

       uim_cmd_ptr->otapa_req.randseed = mc_sec_ran_next();

       uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

       mcc_uim_cmd( uim_cmd_ptr );

       if (uim_cmd_ptr->hdr.status != UIM_RECEIVED_S)
       {
           M1X_MSG( DCP, LEGACY_ERROR,
             "Bad status from UIM %d",
             uim_cmd_ptr->hdr.status);
           otasp_int.otapa_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
           otasp_int.otapa_rsp.nam_lock_ind = FALSE;
           /* Send OTAPA response message as Rejected */
           send_otapa_resp_msg();
           /* Indicates the the OTAPA command was not sent to the R-UIM */
           sent_otapa_command = FALSE;
       }
    }
    else
    {
       M1X_MSG( DCP, LEGACY_ERROR,
         "No free buffers on uim_free_q" );
       otasp_int.otapa_rsp.result_code = OTASP_BLK_REJ_UNKNOWN ;
       otasp_int.otapa_rsp.nam_lock_ind = FALSE;

       /* Send OTAPA response message as Rejected */
       send_otapa_resp_msg();

       /* Indicates the the OTAPA command was not sent to the R-UIM */
       sent_otapa_command = FALSE;
    }
    #endif
    /* If this is an user originated call, then this request would be rejected by
       the card, so we do not want to set the cdma OTASP state variable.
       Since we do not know if the card is going to reject this request, we
       will again change the state if required, upon receiving the report
       from R-UIM */

    /* If the OTAPA command was sent to the R-UIM */
    if (sent_otapa_command)
    {
      if (cdma.otasp_call != USER_INIT_SRV_PROV)
      {
        /* if the start stop bit is set, then we are starting an OTAPA session */
        if (otapa_req_ptr->start_stop)
        {
           cdma.otasp_call = NETWORK_INIT_SRV_PROV;

           /* Flag indicates of an OTAPA session has occurred */
           otasp_otapa_session_occurred = TRUE;
        }
        else if (!otapa_req_ptr->start_stop)
        {
           cdma.otasp_call = NO_SRV_PROV;
        }
      }
    }
    else
    {
      if (cdma.is_check_bip_otasp_concurrency && cdma.is_start_otasp_stop_bip)
      {
        /* This means we have previously consulted GSTK to proceed with OTASP.
         * When GSTK allowed OTAPS, BIP would be blocked.. Now that we are done
         * with OTASP, unblock BIP by calling GSTK API.
         */
        M1X_MSG( DCP, LEGACY_HIGH,
          "Stopping OTASP, allowing BIP");
        gstk_io_ctrl(GSTK_IO_MANAGE_BIP_OTASP_CONCURRENCY,
                     &cdma.curr_slot_id,
                     &(cdma.is_start_otasp_stop_bip));
        cdma.is_start_otasp_stop_bip = FALSE;
      }
    }
  }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  {
    M1X_MSG( DCP, LEGACY_MED,
      "process otapa_req_msg");
    otasp_int.otapa_rsp.msg_type = OTASP_OTAPA_RSP_MSG;

  /* mobile station supports OTAPA */

    if ((cdma.otasp_call == USER_INIT_SRV_PROV)
        || !cdma.otapa_enabled) /* OTAPA is disabled for this NAM */
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "otapa not supported in this mode");
      otasp_int.otapa_rsp.result_code = OTASP_BLK_REJ_MODE;
      otasp_int.otapa_rsp.nam_lock_ind = FALSE;
    }
    /* If too many SPC/SPASM validation failure, reject. */
    else if (!otasp_is_otasp_allowed())
    {
      otasp_int.otapa_rsp.result_code = OTASP_BLK_REJ_UNKNOWN;
      otasp_int.otapa_rsp.nam_lock_ind = FALSE;
    }
    else //it's OK
    {
      if (otapa_req_ptr->start_stop == FALSE) /* stop OTAPA session */
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "Stop OTAPA session");
        otasp_int.otapa_rsp.result_code = OTASP_BLK_ACCEPT;
        otasp_int.otapa_rsp.nam_lock_ind = FALSE;
        cdma.otasp_call = NO_SRV_PROV;

        /* Send OTASP status to CM */
        otasp_report_status_to_cm(CM_OTASP_STATUS_OTAPA_STOPPED);
      }
      else if (otapa_req_ptr->start_stop == TRUE) /* start OTAPA session */
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "Start OTAPA session");
        cdma.otasp_call = NETWORK_INIT_SRV_PROV;
        otasp_otapa_init();

        /* Send OTASP status to CM */
        otasp_report_status_to_cm(CM_OTASP_STATUS_OTAPA_STARTED);

        /* Flag indicates of an OTAPA session has occurred */
        otasp_otapa_session_occurred = TRUE;

        if (nam_locked_state)
        {

          otasp_int.otapa_rsp.result_code = OTASP_BLK_ACCEPT;
          otasp_int.otapa_rsp.nam_lock_ind = TRUE;
          rand_otapa = mc_sec_ran_next();
          otasp_int.otapa_rsp.rand_otapa = rand_otapa;
          M1X_MSG( DCP, LEGACY_MED,
            "Include rand_otapa: %lx",
            rand_otapa);

          if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
          {
            /* Send command to AUTH task to calculate auth_otapa */
            otasp_auth_otapa_in_progress = TRUE;
            auth_cmd_ptr->hdr.command = AUTH_SIGNATURE_SPASM_F;
            auth_cmd_ptr->sig.rand_chal = rand_otapa;

            /* Always use IMSI_S1 as the input. see IS-683A 3.3.7 */
            M1X_MSG( DCP, LEGACY_MED,
              "Use IMSI_S1 as input");
            auth_cmd_ptr->sig.auth_data = mcc_get_auth_imsi_s1();

            auth_cmd_ptr->sig.save_reg = FALSE;
            auth_cmd_ptr->sig.rpt_function = mcc_queue_auth_rpt;

            mcc_auth_cmd( auth_cmd_ptr );

            if (auth_cmd_ptr->hdr.status != AUTH_RECEIVED_S)
            {
              /* -----------------------------------------------
              ** We have a serious problem and can not calculate
              ** auth_otapa, so error fatal.
              ** ----------------------------------------------- */
              ERR_FATAL( "Bad status from AUTH %d",
                         (int) auth_cmd_ptr->hdr.status,0,0);
            }
          }
          else
          {
            ERR_FATAL( "No free buffers on auth_free_q", 0, 0, 0 );
          }

        }
        else /* nam_locked_state == FALSE */
        {
          otasp_int.otapa_rsp.result_code = OTASP_BLK_ACCEPT;
          otasp_int.otapa_rsp.nam_lock_ind = FALSE;
        }
      }
      else /* invalid value for START_STOP, translation error */
      {
        ERR_FATAL( "invalid stop/start value: %d", otapa_req_ptr->start_stop,0,0);
      }
    }

    /* Translate and send the OTAPA Response Message */

    status = otaspx_int_to_ext( &len, &otasp_int, chari );

    if (status == OTASPX_DONE_S) /*lint !e641 */
    {
      send_tc_data_burst( CAI_OTA_ACTIVATION, (byte) len, chari, TRUE );
    }
    else
    {
      ERR_FATAL( "Failed OTASP message translation",0,0,0);
    }
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
  return (next_state);

} /* process_otapa_req */


/*===========================================================================

FUNCTION OTASP_GET_OTAPA_SESSION

DESCRIPTION
  This function returns whether or not an OTAPA session has taken place

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if an OTAPA session has taken place
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean otasp_get_otapa_session (void)
{
  M1X_MSG( DCP, LEGACY_MED,
    "otapa_session_occurred: %d",
    otasp_otapa_session_occurred);
  return otasp_otapa_session_occurred;
}



/*===========================================================================

FUNCTION OTASP_REPORT_COMMIT_STATUS_TO_CM

DESCRIPTION
  The function sends a commit status command to CM

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void otasp_report_commit_status_to_cm
(
  boolean commit_in_progress
    /* Flag if an OTAPA session is in progress */
)
{
  cm_mc_rpt_type *cm_ptr;
    /* pointer to command to send to CM */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  M1X_MSG( DCP, LEGACY_MED,
    "send commit in prog %d to CM",
    commit_in_progress);
  if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)  {
    cm_ptr->otasp_commit_status.hdr.cmd = CM_OTASP_COMMIT_STATUS_F;
    cm_ptr->otasp_commit_status.commit_in_progress = commit_in_progress;
    mcc_cm_mc_rpt( cm_ptr );
  }
  else
  {
    ERR_FATAL( "No buffers on cm_cmd_rpt_free_q", 0, 0, 0 );
  }
} /* otasp_report_commit_status_to_cm() */

/*===========================================================================

FUNCTION OTASP_REPORT_STATUS_TO_CM

DESCRIPTION
  The function sends the otasp status command to CM

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void otasp_report_status_to_cm
(
  cm_otasp_status_e_type otasp_status
    /* OTASP state */
)
{
  cm_mc_rpt_type *cm_ptr;
    /* pointer to command to send to CM */
  boolean send_status = FALSE;
    /* flag to decide when to report status */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(otasp_status)
  {
    /* Status values to be sent only in case of user initiated session */
    case CM_OTASP_STATUS_SPL_UNLOCKED:
    case CM_OTASP_STATUS_SPC_RETRIES_EXCEEDED:
    case CM_OTASP_STATUS_AKEY_EXCHANGED:
    case CM_OTASP_STATUS_NAM_DOWNLOADED:
    case CM_OTASP_STATUS_MDN_DOWNLOADED:
    case CM_OTASP_STATUS_IMSI_DOWNLOADED:
    case CM_OTASP_STATUS_PRL_DOWNLOADED:
    case CM_OTASP_STATUS_COMMITTED:
    {
      if(cdma.otasp_call == USER_INIT_SRV_PROV)
      {
         send_status = TRUE;
      }
      break;
    }
    /* Status values to be sent only in case of network initiated session */
    case CM_OTASP_STATUS_OTAPA_STARTED:
    {
      if(cdma.otasp_call == NETWORK_INIT_SRV_PROV)
      {
         send_status = TRUE;
      }
      break;
    }
    /* Status values to be sent only in case of network initiated session.
       For STOP and ABORT cases, cdma.otasp_call will be NO_SRV_PROV, so this
       variable can not be used. It is assumed that the status will not be set
       to any of the following for a user initiated otasp session. */
    case CM_OTASP_STATUS_OTAPA_STOPPED:
    /* case CM_OTASP_STATUS_OTAPA_ABORTED:
       This status will be sent from CM itself to upper layers */
    {
      send_status = TRUE;
      break;
    }
    /* status values to be sent for both user and network initiated session */
    case CM_OTASP_STATUS_SSD_UPDATED:
    {
      send_status = TRUE;
      break;
    }
    default:
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid otasp status %d",
        otasp_status);
      break;
    }
  } /* end switch */

  if(send_status)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "sent OTASP status %d to CM",
      otasp_status);
    if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cm_ptr->otasp_status.hdr.cmd = CM_OTASP_STATUS_F;
      cm_ptr->otasp_status.otasp_code = otasp_status;
      mcc_cm_mc_rpt( cm_ptr );
    }
    else
    {
      ERR_FATAL( "No buffers on cm_cmd_rpt_free_q", 0, 0, 0 );
    }
  } /* end if */

} /* otasp_report_status_to_cm() */

/*===========================================================================

FUNCTION OTASP_IS_SUCCESSFUL_COMMIT

DESCRIPTION
  The function determines if an OTASP commit is successful or not.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if an OTASP commit is successful
  FALSE, else.

SIDE EFFECTS
  None.

===========================================================================*/
boolean otasp_is_successful_commit (void)
{
  M1X_MSG( DCP, LEGACY_MED,
    "otasp_is_successful_commit: %d",
    otasp_successful_commit);
  return (otasp_successful_commit);
}

/*===========================================================================

FUNCTION OTASP_IS_OTASP_ALLOWED

DESCRIPTION
  The function determines if OTASP is allowed or not

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if an OTASP is allowed
  FALSE, else.

SIDE EFFECTS
  None.

===========================================================================*/
boolean otasp_is_otasp_allowed (void)
{
  boolean otasp_allowed = TRUE;
  gstk_status_enum_type  mc_gstk_status;

  /* Otasp Max unlock attempts allowed */
  uint8  otasp_max_unlock_attempts = (uint8) mcc_otasp_max_unlock_attempts_allowed();

  if( (otasp_failed_unlock_attempts == otasp_max_unlock_attempts )
      || (otasp_otapa_failed_spc_val_attempts == otasp_max_unlock_attempts )
      || (otasp_otapa_failed_spasm_val_attempts == otasp_max_unlock_attempts ) )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Max invalid SPC/SPASM validation attempts. So OTASP not allowed.");
   otasp_allowed = FALSE;
  }

  /*  Customer specific changes to sequentialize BIP and OTASP.
   *  If BIP is in progress, dont initiate OTASP session.
   *  Consult GSTK to know if BIP was in progress.
   *  Consult GSTK only when we are past SPC checks.
   */

  if ( (cdma.is_check_bip_otasp_concurrency) && (otasp_allowed == TRUE) )
  {
    /* This function will be called for OTASP call intiatiation / OTAPA req
     * Hence checking GSTK for BIP covers both User / Network intiated
     * OTASP session.
     */
    M1X_MSG( DCP, LEGACY_HIGH,
      "Checking for BIP/OTASP Concurrency...");
    mc_gstk_status = gstk_io_ctrl(GSTK_IO_MANAGE_BIP_OTASP_CONCURRENCY,
                                  &cdma.curr_slot_id,
                                  &(cdma.is_start_otasp_stop_bip));
    if ( (mc_gstk_status == GSTK_SUCCESS) && (cdma.is_start_otasp_stop_bip) )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Starting OTASP, Stopping BIP");
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "OTASP not allowed.");
      otasp_allowed = FALSE;
      cdma.is_start_otasp_stop_bip = FALSE;
    }

  }

  M1X_MSG( DCP, LEGACY_HIGH,
    "otasp_allowed? = %d",
    otasp_allowed);
  return (otasp_allowed);
}

#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION OTASP_SET_RUIM_NAM_LOCK_STATE

DESCRIPTION
  This function indicates the nature of the otasp call to the R-UIM and
  accordingly sets the nam lock state in the R-UIM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void otasp_set_ruim_nam_lock_state
(
   otasp_call_type ota_call_type
)
{
  /* This is the value that we would use as default if the read FAILS
     indicating that the NAM is locked */
  byte temp_nam_lock = UIM_OTA_NAM_LOCK;
#ifdef FEATURE_CSIM
  mmgsdi_access_type  access_type;
  mmgsdi_offset_type offset;
  mmgsdi_len_type req_len;
  mmgsdi_write_data_type mmgsdi_write;
  mmgsdi_return_enum_type mmgsdi_status;
  size_t copy_size;
  /* Return value of memscpy function */
#else
 /* Pointer to command for UIM task */
  uim_cmd_type *uim_cmd_ptr;
  /* Pointer to report from UIM task */
  uim_rpt_type *uim_rpt_ptr;
#endif

#ifdef FEATURE_1X_CP_MEM_OPT
  byte *uim_access_buffer = (byte*)
                  modem_mem_alloc( UIM_MAX_CHARS, MODEM_MEM_CLIENT_1X );
  if( uim_access_buffer != NULL )
  {
    memset( uim_access_buffer, 0,UIM_MAX_CHARS );
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for uim access buffer");
  }

  #ifdef FEATURE_CSIM
  if( (mcc_mmgsdi_rpt_ptr = modem_mem_alloc
                              ( sizeof(mmgsdi_cnf_type), MODEM_MEM_CLIENT_1X ) ) != NULL )
  {
    memset( mcc_mmgsdi_rpt_ptr, 0, sizeof(mmgsdi_cnf_type) );
  }
  else
  {
     ERR_FATAL("Memory allocation failed for MMGSDI report", 0, 0, 0);
  }
  #endif /* FEATURE_CSIM */


#endif /* FEATURE_1X_CP_MEM_OPT */
  /* Read the NAM_LOCK EF from the card */
#ifdef FEATURE_CSIM
  access_type.access_method = MMGSDI_EF_ENUM_ACCESS;
  if (mcmmgsdi_app_type == MMGSDI_APP_CSIM )
  {
    access_type.file.file_enum = MMGSDI_CSIM_NAM_LOCK;
  }
  else if (mcmmgsdi_app_type == MMGSDI_APP_RUIM )
  {
    access_type.file.file_enum = MMGSDI_CDMA_NAM_LOCK;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      " Reading invalid app_type=%d - expect errors",
      mcmmgsdi_app_type);
  }
  #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
  /* Don't send MMGSDI request if Sim Busy event has been received */  
  if (mc_get_sim_busy_status())
  {
    M1X_MSG( DCP, LEGACY_HIGH,
            "SIM Busy, OTASP: MMGSDI read failed for the item = %d ",
             access_type.file.file_enum );
  }
  else
  #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
  { 
    offset = 0;
    req_len = 1;
    /* Read from MMGSDI and wait for the response. */
    mmgsdi_status = mmgsdi_session_read_transparent(cdma.session_id,
                                             access_type,
                                             offset,
                                             req_len,
                                             mcc_mmgsdi_read_write_opr_cb,
                                             0 );
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "MMGSDI read failed for the item = %d ",
        access_type.file.file_enum);
    }
    else
    {
      (void) mcc_mmgsdi_wait( MCC_MMGSDI_RSP_SIG );
      (void) rex_clr_sigs( MC_TCB_PTR, MCC_MMGSDI_RSP_SIG );
  
      if (mcc_mmgsdi_rpt_status == MMGSDI_SUCCESS)
      {
        if( (uim_access_buffer != NULL) && (mcc_mmgsdi_rpt_ptr != NULL) )
        {
          if( (mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len 
                                                        <= UIM_MAX_CHARS) &&
              (mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr != NULL)
            )
          {
            copy_size = memscpy(uim_access_buffer,
                                UIM_MAX_CHARS*sizeof(byte),
                   mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr,
                   mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len);
            if( copy_size != mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len )
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                copy_size,
                mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len,
                UIM_MAX_CHARS*sizeof(byte));
            }
          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
                     "Unexpected data_len:%d OR data_ptr:0X%x",
                     mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len,
                     mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr );
          }
          temp_nam_lock = uim_access_buffer[0];
        
        /* Free the dynamically allocated memory for the deep copy of
         * MMGSDI Rpt if required */
        if(mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len > 0)
        {
          if(mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr != NULL)
          {
            M1X_MSG( DCP, LEGACY_MED,
              "MMGSDI_READ_CNF rpt, free mem for:0X%x",
              mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr);
            modem_mem_free(
             mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr,
             MODEM_MEM_CLIENT_1X
                          );
            mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr = NULL;
            mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len = 0;
          } /* End if(data_ptr != NULL) */
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Data ptr is NULL for otasp MMGSDI read cnf rpt and data_len:%d",
              mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len);
          } /* (data_ptr == NULL) */
        } /* End if (data_len > 0) */
      } /* (uim_access_buffer != NULL) && (mcc_mmgsdi_rpt_ptr != NULL) */
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "NULL pointer, uim_access_buffer:0x%x, mcc_mmgsdi_rpt_ptr:0x%x",
            uim_access_buffer,
            mcc_mmgsdi_rpt_ptr);
          return;
        }
      } /* end if - the command passed */
      else
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "Read failed on access item = %d ",
          access_type.file.file_enum);
      }
    }
  }
#else

  if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
  {
      uim_cmd_ptr->access_uim.access = UIM_READ_F;
      uim_cmd_ptr->access_uim.item = UIM_CDMA_NAM_LOCK;
      uim_cmd_ptr->access_uim.num_bytes = 1;
      uim_cmd_ptr->access_uim.data_ptr = uim_access_buffer;
      uim_cmd_ptr->access_uim.offset = 0;
      uim_cmd_ptr->hdr.command = UIM_ACCESS_F;
      uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

      mcc_uim_cmd( uim_cmd_ptr );

      /* ----------------------------------
      ** Wait for MCC_UIM_Q_SIG to be set.
      ** ---------------------------------- */
      M1X_MSG( DCP, LEGACY_MED,
        "Waiting for UIM report" );

      (void) mcc_wait( MCC_UIM_Q_SIG );

      uim_rpt_ptr = (uim_rpt_type*) q_get( &mc_uim_q );

      if ( uim_cmd_ptr->hdr.status == UIM_RECEIVED_S )
      {
        if ((uim_rpt_ptr->rpt_type == UIM_ACCESS_R) &&
            (uim_rpt_ptr->rpt_status == UIM_PASS))
        {
          if( uim_access_buffer != NULL )
          {
            temp_nam_lock = uim_access_buffer[0];
          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "uim_access_buffer is NULL");
            return;
          }
        }
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Read Failed for OTA Features " );
        }

      }
      else
      {
         M1X_MSG( DCP, LEGACY_ERROR,
           "Bad status from UIM %d",
           uim_cmd_ptr->hdr.status);
      }

      /* Put the buffer back in the queue */
      if (uim_rpt_ptr != NULL)
      {
        cmd_done( &uim_rpt_ptr->rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( uim_rpt_ptr, MC_UIM_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
      }
  }
  else
  {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No free buffers on uim_free_q" );
  }
 #endif
  /* Only the least 3 bits of the EF contain info about the nam lock */
  temp_nam_lock &= UIM_NAM_LOCK_BITS;

  if (ota_call_type == USER_INIT_SRV_PROV)
  {
      /* Clear the OTA bit */
      temp_nam_lock = (temp_nam_lock & ~UIM_NAM_LOCK_OTA_MASK);

      /* Set the OTA bit as OTASP Call */
      temp_nam_lock |= UIM_OTA_OTASP;

      /* Clear the nam lock state bit */
      temp_nam_lock = (temp_nam_lock & ~UIM_NAM_LOCK_STATE_MASK);

      /* Set the nam lock state as unlocked */
      temp_nam_lock |= UIM_NL_STATE_UNLOCK;
  }

  else if (ota_call_type == NETWORK_INIT_SRV_PROV)
  {
      /* Clear the OTA bit */
      temp_nam_lock = (temp_nam_lock & ~UIM_NAM_LOCK_OTA_MASK);

      /* Set the OTA bit as OTASP Call */
      temp_nam_lock |= UIM_OTA_OTAPA;

      /* Clear the nam lock state bit */
      temp_nam_lock = (temp_nam_lock & ~UIM_NAM_LOCK_STATE_MASK);

      /* Set the nam locked state to reflect the nam lock */
      if (cdma.nam_lock)
      {
         temp_nam_lock |= UIM_NL_STATE_LOCK;
      }
      else
      {
         temp_nam_lock |= UIM_NL_STATE_UNLOCK;
      }
  }

  if( uim_access_buffer != NULL )
  {
    uim_access_buffer[0] = temp_nam_lock;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "uim_access_buffer is NULL");
    return;
  }

 #ifdef FEATURE_CSIM
    /* Read UIM_ID */
  access_type.access_method = MMGSDI_EF_ENUM_ACCESS;
  if (mcmmgsdi_app_type == MMGSDI_APP_CSIM )
  {
    access_type.file.file_enum = MMGSDI_CSIM_NAM_LOCK;
  }
  else if (mcmmgsdi_app_type == MMGSDI_APP_RUIM )
  {
    access_type.file.file_enum = MMGSDI_CDMA_NAM_LOCK;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Reading invalid app_type=%d - expect errors",
      mcmmgsdi_app_type );
  }
  #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS  
  /* Don't send MMGSDI request if Sim Busy event has been received */
  if (mc_get_sim_busy_status())
  {
    M1X_MSG( DCP, LEGACY_HIGH, 
             "SIM Busy, OTASP: MMGSDI write failed for the item = %d ",
             access_type.file.file_enum );
  }
  else
  #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
  {
    offset = 0;
    req_len = 1;
    mmgsdi_write.data_len = req_len;
    mmgsdi_write.data_ptr = uim_access_buffer;
  
    /* Read from MMGSDI and wait for the response. */
    mmgsdi_status = mmgsdi_session_write_transparent(cdma.session_id,
                                              access_type,
                                              offset,
                                              mmgsdi_write,
                                              mcc_mmgsdi_read_write_opr_cb,
                                              0);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "MMGSDI write failed for the item = %d ",
        access_type.file.file_enum);
    }
    else
    {
      (void) mcc_mmgsdi_wait( MCC_MMGSDI_RSP_SIG );
      (void) rex_clr_sigs( MC_TCB_PTR, MCC_MMGSDI_RSP_SIG );
  
      if (mcc_mmgsdi_rpt_status == MMGSDI_SUCCESS)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Write Success for NAM_LOCK " );
      } /* end if - the command passed */
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Write Failed for OTA Features " );
        M1X_MSG( DCP, LEGACY_ERROR,
          " NAM_LOCK not written correctly");
      }
    }
  }

#else

  /* Write the data into the NAM LOCK EF */
  if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
  {
      uim_cmd_ptr->access_uim.access = UIM_WRITE_F;
      uim_cmd_ptr->access_uim.item = UIM_CDMA_NAM_LOCK;
      uim_cmd_ptr->access_uim.num_bytes = 1;
      uim_cmd_ptr->access_uim.data_ptr = uim_access_buffer;
      uim_cmd_ptr->access_uim.offset = 0;
      uim_cmd_ptr->hdr.command = UIM_ACCESS_F;
      uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

      mcc_uim_cmd( uim_cmd_ptr );

      /* ----------------------------------
      ** Wait for MCC_UIM_Q_SIG to be set.
      ** ---------------------------------- */
      M1X_MSG( DCP, LEGACY_MED,
        "Waiting for UIM report" );

      (void) mcc_wait( MCC_UIM_Q_SIG );

      uim_rpt_ptr = (uim_rpt_type*) q_get( &mc_uim_q );

      if ( uim_cmd_ptr->hdr.status == UIM_RECEIVED_S )
      {

         if ((uim_rpt_ptr->rpt_type == UIM_ACCESS_R) &&
            (uim_rpt_ptr->rpt_status != UIM_PASS))
         {
            M1X_MSG( DCP, LEGACY_MED,
              "Write Failed for OTA Features " );
            M1X_MSG( DCP, LEGACY_ERROR,
              " NAM_LOCK not written correctly");
         }

      }
      else
      {
         M1X_MSG( DCP, LEGACY_ERROR,
           "Bad status from UIM %d",
           uim_cmd_ptr->hdr.status);
         M1X_MSG( DCP, LEGACY_HIGH,
           " NAM_LOCK not written correctly");
      }

      /* Put the buffer back in the queue */
      if (uim_rpt_ptr != NULL)
      {
        cmd_done( &uim_rpt_ptr->rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( uim_rpt_ptr, MC_UIM_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
      }
  }
  else
  {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No free buffers on uim_free_q" );
      M1X_MSG( DCP, LEGACY_HIGH,
        " NAM_LOCK not written correctly");
  }
   #endif

#ifdef FEATURE_1X_CP_MEM_OPT
  if( uim_access_buffer != NULL )
  {
    modem_mem_free( uim_access_buffer, MODEM_MEM_CLIENT_1X );
    uim_access_buffer = NULL;
  }
  #ifdef FEATURE_CSIM
  if( mcc_mmgsdi_rpt_ptr != NULL )
  {
    /* Free the dynamically allocated memory for the deep copy of
     * MMGSDI Rpt if required */
    if(mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len > 0)
    {
      if(mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr != NULL)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "MMGSDI_READ_CNF rpt, free mem for:0X%x",
          mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr);
        modem_mem_free(
         mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr,
         MODEM_MEM_CLIENT_1X
                      );
        mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr = NULL;
      } /* End if(data_ptr != NULL) */
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Data ptr is NULL for otasp MMGSDI read cnf rpt and data_len:%d",
          mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len);
      }
    } /* End if (data_len > 0) */

    modem_mem_free( mcc_mmgsdi_rpt_ptr, MODEM_MEM_CLIENT_1X );
    mcc_mmgsdi_rpt_ptr = NULL;
  }
  #endif /* FEATURE_CSIM */

#endif /* FEATURE_1X_CP_MEM_OPT */
} /* otasp_set_ruim_nam_lock_state */
#endif /* FEATURE_UIM_RUIM */

#if (defined(FEATURE_MDN_BASED_NAI) && defined(FEATURE_OTASP_UPDATE_MDN_NAI))
/*===========================================================================

FUNCTION OTASP_BCD_TO_CHAR

DESCRIPTION
  This function converts MDN (BCD form) into ASCII characters.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the characters for the numeric values between 1 - 9.
  Returns 0 for 10, * for 11 and # for 12.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL char otasp_bcd_to_char
(
  word num_to_convert
)
{
  switch(num_to_convert)
  {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      return '0' + num_to_convert;
    case 10:
      return '0';
    case 11:
      return '*';
    case 12:
      return '#';
    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid BCD digit (%d)",
        num_to_convert);
      return ' ';
  }
} /* otasp_bcd_to_char */

/*===========================================================================

FUNCTION OTASP_NAI_FROM_MDN

DESCRIPTION
  This function forms NAI based on given MDN and domain.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the formation of NAI is successful, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL boolean otasp_nai_from_mdn
(
  char*                   nai_to_write,
  uint8*                  len,   //  len should be set to MAX_NAI_LENGTH
  nv_mob_dir_data_type*   mob_dir,
  char*                   domain  // must be null terminated
)
{
  int  i, cpylen;

  ASSERT (nai_to_write);
  ASSERT (len);
  ASSERT (mob_dir);
  ASSERT (domain);

  if (*len < mob_dir->n_digits + 1)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Output nai buffer too small for MDN");
    return FALSE;
  }

  for (i = 0; i < mob_dir->n_digits; i++)
  {
    nai_to_write[i] = otasp_bcd_to_char(mob_dir->digitn[i]);
    if (nai_to_write[i] == ' ')
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid MDN when converting to NAI");
      return FALSE;  // item_to_be_written = FALSE;
    }
  }

  nai_to_write[i] = 0;

  cpylen = strlen(domain);
  if (cpylen > *len - i - 1)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Output nai buffer too small for domain");
    return FALSE;  // item_to_be_written = FALSE;
  }

  (void) strlcat(&(nai_to_write[i]), domain, *len - i - 1);
  nai_to_write[i+cpylen] = 0;  // NULL terminate
  *len = i + cpylen;
  return TRUE;
} /* otasp_nai_from_mdn */

/*===========================================================================

FUNCTION OTASP_GET_DFLT_MIP_PROFILE

DESCRIPTION
  Sets the current profile with the parameters for the default MIP profile.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void otasp_get_dflt_mip_profile
(
  nv_ds_mip_gen_user_prof_type * prof
)
{
  ASSERT (prof);

  prof->index             = MIP_DLFT_PROFILE_NUM;
  prof->nai_length        = 0;
  prof->nai[0]            = 0;
  prof->mn_ha_spi_set     = TRUE;
  prof->mn_ha_spi         = MIP_MD5_SPI;
  prof->mn_aaa_spi_set    = TRUE;
  prof->mn_aaa_spi        = MIP_CHAP_SPI;
  prof->rev_tun_pref      = FALSE;
  prof->home_addr         = DYNAMIC_HOME_ADDR;
  prof->primary_ha_addr   = DYNAMIC_HA_ADDR;
  prof->secondary_ha_addr = UNSET_HA_ADDR;
} /* otasp_get_dflt_mip_profile */

/*===========================================================================

FUNCTION OTASP_UPDATE_MDN_BASED_NAI

DESCRIPTION
  Updates the MDN based NAIs.

  Example :-
  Assume NAI was: 6191234567@vzw3g.com.
  MDN downloaded is : 8589996542
  1. For each NV item, parse and read the domain part.
     In this example, vzw3g.com
  2. Then update each NV item by changing the MDN portion.
     So NAI will become 8589996542@vzw3g.com
  3. If the NAI was inactive or no domain name can be extracted,
     then it is writing: 8589996542@default.com.

DEPENDENCIES
  The structure nv_commit should not be cleared before calling
  this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void otasp_update_mdn_based_nai
(
  void
)
{
  /* Define constants for the switch statement below */
  #define NAI_UPDATE_DS_MIP_GEN_USER_PROF      1
  #define NAI_UPDATE_PPP_USER_ID               2
  #define NAI_UPDATE_HDR_AN_AUTH_USER_ID_LONG  3
  #define NAI_UPDATE_DS_SIP_RM_NAI             4
  #define NAI_UPDATE_DS_MIP_RM_NAI             5
  #define NAI_UPDATE_DONE                    255

  /* use the largest of the source nai fields (MIP, SIP, AN) */
  #define MAX_NAI_LENGTH NV_MAX_PPP_USER_ID_LENGTH

  #define PS_NAI_DFLT_DOMAIN "@default.com"

  /* Command buffer for individual writes/reads issued internally. */
  nv_cmd_type   local_cmd;

  /* Data buffer for individual writes/reads issued internally. */
  nv_item_type  local_item;

  char    nai_to_write[MAX_NAI_LENGTH];
  char    domain[MAX_NAI_LENGTH + 1];
  char*   domain_ptr;
  uint8   nai_length;

  /* Return value of memscpy function */
  size_t copy_size;

  byte nai_update_state = NAI_UPDATE_DS_MIP_GEN_USER_PROF;
  boolean      item_to_be_written = FALSE;

  /* Set up generic command buffer parameters */
  local_cmd.cmd         = NV_WRITE_F;
  local_cmd.tcb_ptr     = NULL;
  local_cmd.sigs        = 0;
  local_cmd.done_q_ptr  = NULL;
  local_cmd.data_ptr    = &local_item;
  local_cmd.status      = NV_DONE_S;

  M1X_MSG( DCP, LEGACY_MED,
    "Updating MDN based NAIs...");
  while (nai_update_state != NAI_UPDATE_DONE)
  {
    item_to_be_written = FALSE;
    switch(nai_update_state)
    {
      case NAI_UPDATE_DS_MIP_GEN_USER_PROF:
      {
        /*-------------------------------------------------------------------
          Note: this defaults to read/write profile at index 0.
                Customer should use whichever index they assign
                to the active MIP profile.
        -------------------------------------------------------------------*/
        local_item.ds_mip_gen_user_prof.index = MIP_DLFT_PROFILE_NUM;
        local_cmd.cmd                         = NV_READ_F;
        local_cmd.item                        = NV_DS_MIP_GEN_USER_PROF_I;
        local_cmd.status                      = mcc_nv_cmd(&local_cmd);
        if (local_cmd.status == NV_NOTACTIVE_S)
        {
          otasp_get_dflt_mip_profile( &(local_item.ds_mip_gen_user_prof) );
        }
        else if (local_cmd.status != NV_DONE_S)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Error %d reading NV",
            local_cmd.status);
          nai_update_state = NAI_UPDATE_PPP_USER_ID;
          break;
        }

        if(local_item.ds_mip_gen_user_prof.nai_length < NV_MAX_NAI_LENGTH)
          local_item.ds_mip_gen_user_prof.nai[
            local_item.ds_mip_gen_user_prof.nai_length] = 0;
        domain_ptr =
          strchr( (char*) local_item.ds_mip_gen_user_prof.nai, '@' );
        if (domain_ptr == NULL)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Using default domain for MIP nai");
          domain_ptr = PS_NAI_DFLT_DOMAIN;
        }
        (void) strlcpy(domain,domain_ptr,NV_MAX_NAI_LENGTH);
        domain[NV_MAX_NAI_LENGTH] = 0;  // NULL terminate

        nai_length = NV_MAX_NAI_LENGTH;  // max bytes to write
        if( !otasp_nai_from_mdn( nai_to_write,
                           &nai_length,
                           &(nv_commit.mob_dir),
                           domain ) )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Couldn't convert MDN and domain to NAI!");
          nai_update_state = NAI_UPDATE_PPP_USER_ID;
          break;
        }

        item_to_be_written = TRUE;
        local_cmd.cmd = NV_WRITE_F;
        if( nai_length <= NV_MAX_NAI_LENGTH )
        {
          local_item.ds_mip_gen_user_prof.nai_length = nai_length;
          copy_size = memscpy( (char*) local_item.ds_mip_gen_user_prof.nai,
                                sizeof(local_item.ds_mip_gen_user_prof.nai),
                  nai_to_write,
                  nai_length );
          if( copy_size != nai_length )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
              copy_size,
              nai_length,
              sizeof(local_item.ds_mip_gen_user_prof.nai));
          }
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unexpected nai_length:%d",
            nai_length);
          local_item.ds_mip_gen_user_prof.nai_length = 0;
        }
        nai_update_state = NAI_UPDATE_PPP_USER_ID;
        break;
      }

      case NAI_UPDATE_PPP_USER_ID:
      {
        local_cmd.cmd    = NV_READ_F;
        local_cmd.item   = NV_PPP_USER_ID_I;
        local_cmd.status = mcc_nv_cmd(&local_cmd);
        if (local_cmd.status == NV_NOTACTIVE_S)
        {
          local_item.ppp_user_id.user_id_len = 0;
        }
        else if (local_cmd.status != NV_DONE_S)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Error %d reading NV",
            local_cmd.status);
          nai_update_state = NAI_UPDATE_HDR_AN_AUTH_USER_ID_LONG;
          break;
        }

        // copy the old domain from the user profile, or the default domain
        local_item.ppp_user_id.user_id[local_item.ppp_user_id.user_id_len] = 0;
        domain_ptr = strchr( (char*) local_item.ppp_user_id.user_id, '@' );
        if (domain_ptr == NULL)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Using default domain for SIP nai");
          domain_ptr = PS_NAI_DFLT_DOMAIN;
        }
        (void) strlcpy(domain,domain_ptr,NV_MAX_PPP_USER_ID_LENGTH);
        domain[NV_MAX_PPP_USER_ID_LENGTH] = 0;  // NULL terminate

        nai_length = NV_MAX_PPP_USER_ID_LENGTH;  // max bytes to write
        if( !otasp_nai_from_mdn( nai_to_write,
                           &nai_length,
                           &(nv_commit.mob_dir),
                           domain ) )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Couldn't convert MDN and domain to NAI!");
          nai_update_state = NAI_UPDATE_HDR_AN_AUTH_USER_ID_LONG;
          break;
        }

        item_to_be_written = TRUE;
        local_cmd.cmd      = NV_WRITE_F;
        local_cmd.item     = NV_PPP_USER_ID_I;
        if( nai_length <= MAX_NAI_LENGTH )
        {
          local_item.ppp_user_id.user_id_len = nai_length;
          copy_size = memscpy( (char*) local_item.ppp_user_id.user_id,
                                sizeof(local_item.ppp_user_id.user_id),
                  nai_to_write,
                  nai_length );
          if( copy_size != nai_length )
          {
           M1X_MSG( DCP, LEGACY_ERROR,
             "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
             copy_size,
             nai_length,
             sizeof(local_item.ppp_user_id.user_id));
          }
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unexpected nai_length:%d",
            nai_length);
          local_item.ppp_user_id.user_id_len = 0;
        }
        nai_update_state = NAI_UPDATE_HDR_AN_AUTH_USER_ID_LONG;
        break;
      }

      case NAI_UPDATE_HDR_AN_AUTH_USER_ID_LONG:
      {
        local_cmd.cmd    = NV_READ_F;
        local_cmd.item   = NV_HDR_AN_AUTH_USER_ID_LONG_I;
        local_cmd.status = mcc_nv_cmd(&local_cmd);
        if (local_cmd.status == NV_NOTACTIVE_S)
        {
          local_item.hdr_an_ppp_user_id.user_id_len = 0;
        }
        else if (local_cmd.status != NV_DONE_S)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Error %d reading NV",
            local_cmd.status);
          nai_update_state = NAI_UPDATE_DS_SIP_RM_NAI;
          break;
        }

        // copy the old domain from the user profile, or the default domain
        local_item.hdr_an_ppp_user_id.user_id[
          local_item.hdr_an_ppp_user_id.user_id_len] = 0;
        domain_ptr =
          strchr( (char*) local_item.hdr_an_ppp_user_id.user_id, '@' );
        if (domain_ptr == NULL)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Using default domain for AN nai");
          domain_ptr = PS_NAI_DFLT_DOMAIN;
        }
        (void) strlcpy(domain,domain_ptr,NV_MAX_AN_PPP_USER_ID_LENGTH);
        domain[NV_MAX_AN_PPP_USER_ID_LENGTH] = 0;  // NULL terminate

        nai_length = NV_MAX_AN_PPP_USER_ID_LENGTH;  // max bytes to write
        if( !otasp_nai_from_mdn( nai_to_write,
                           &nai_length,
                           &(nv_commit.mob_dir),
                           domain ) )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Couldn't convert MDN and domain to NAI!");
          nai_update_state = NAI_UPDATE_DS_SIP_RM_NAI;
          break;
        }

        item_to_be_written = TRUE;
        local_cmd.cmd      = NV_WRITE_F;
        local_cmd.item     = NV_HDR_AN_AUTH_USER_ID_LONG_I;
        if( nai_length <= MAX_NAI_LENGTH )
        {
          local_item.hdr_an_ppp_user_id.user_id_len = nai_length;
          copy_size = memscpy( (char*) local_item.hdr_an_ppp_user_id.user_id,
                                sizeof(local_item.hdr_an_ppp_user_id.user_id),
                  nai_to_write,
                  nai_length );
          if( copy_size != nai_length )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
              copy_size,
              nai_length,
              sizeof(local_item.hdr_an_ppp_user_id.user_id));
          }
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unexpected nai_length:%d",
            nai_length);
          local_item.hdr_an_ppp_user_id.user_id_len = 0;
        }
        nai_update_state = NAI_UPDATE_DS_SIP_RM_NAI;
        break;
      }

      case NAI_UPDATE_DS_SIP_RM_NAI:
      {
        local_cmd.cmd    = NV_READ_F;
        local_cmd.item   = NV_DS_SIP_RM_NAI_I;
        local_cmd.status = mcc_nv_cmd(&local_cmd);
        if (local_cmd.status == NV_NOTACTIVE_S)
        {
          local_item.ds_sip_rm_nai.nai_length = 0;
        }
        else if (local_cmd.status != NV_DONE_S)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Error %d reading NV",
            local_cmd.status);
          nai_update_state = NAI_UPDATE_DS_MIP_RM_NAI;
          break;
        }

        // copy the old domain from the user profile, or the default domain
        local_item.ds_sip_rm_nai.nai[local_item.ds_sip_rm_nai.nai_length] = 0;
        domain_ptr = strchr( (char*) local_item.ds_sip_rm_nai.nai, '@' );
        if (domain_ptr == NULL)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Using default domain for SIP nai");
          domain_ptr = PS_NAI_DFLT_DOMAIN;
        }
        (void) strlcpy(domain,domain_ptr,NV_MAX_NAI_LENGTH);
        domain[NV_MAX_NAI_LENGTH] = 0;  // NULL terminate

        nai_length = NV_MAX_NAI_LENGTH;  // max bytes to write
        if( !otasp_nai_from_mdn( nai_to_write,
                           &nai_length,
                           &(nv_commit.mob_dir),
                           domain ) )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Couldn't convert MDN and domain to NAI!");
          nai_update_state = NAI_UPDATE_DS_MIP_RM_NAI;
          break;
        }

        item_to_be_written = TRUE;
        local_cmd.cmd      = NV_WRITE_F;
        local_cmd.item     = NV_DS_SIP_RM_NAI_I;
        if( nai_length <= NV_MAX_NAI_LENGTH )
        {
          local_item.ds_sip_rm_nai.nai_length = nai_length;
          copy_size = memscpy( (char*) local_item.ds_sip_rm_nai.nai,
                                sizeof(local_item.ds_sip_rm_nai.nai),
                  nai_to_write,
                  nai_length );
          if( copy_size != nai_length )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
              copy_size,
              nai_length,
              sizeof(local_item.ds_sip_rm_nai.nai));
          }
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unexpected nai_length:%d",
            nai_length);
          local_item.ds_sip_rm_nai.nai_length = 0;
        }
        nai_update_state = NAI_UPDATE_DS_MIP_RM_NAI;
        break;
      }

      case NAI_UPDATE_DS_MIP_RM_NAI:
      {
        local_item.ds_mip_rm_nai.index = MIP_DLFT_PROFILE_NUM;
        local_cmd.cmd    = NV_READ_F;
        local_cmd.item   = NV_DS_MIP_RM_NAI_I;
        local_cmd.status = mcc_nv_cmd(&local_cmd);
        if (local_cmd.status == NV_NOTACTIVE_S)
        {
          local_item.ds_mip_rm_nai.nai_length = 0;
        }
        else if (local_cmd.status != NV_DONE_S)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Error %d reading NV",
            local_cmd.status);
          nai_update_state = NAI_UPDATE_DONE;
          break;
        }

        // copy the old domain from the user profile, or the default domain
        local_item.ds_mip_rm_nai.nai[local_item.ds_mip_rm_nai.nai_length] = 0;
        domain_ptr = strchr( (char*) local_item.ds_mip_rm_nai.nai, '@' );
        if (domain_ptr == NULL)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Using default domain for MIP nai");
          domain_ptr = PS_NAI_DFLT_DOMAIN;
        }
        (void) strlcpy(domain,domain_ptr,NV_MAX_NAI_LENGTH);
        domain[NV_MAX_NAI_LENGTH] = 0;  // NULL terminate

        nai_length = NV_MAX_NAI_LENGTH;  // max bytes to write
        if( !otasp_nai_from_mdn( nai_to_write,
                           &nai_length,
                           &(nv_commit.mob_dir),
                           domain ) )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Couldn't convert MDN and domain to NAI!");
          nai_update_state = NAI_UPDATE_DONE;
          break;
        }

        item_to_be_written = TRUE;
        local_cmd.cmd      = NV_WRITE_F;
        local_cmd.item     = NV_DS_MIP_RM_NAI_I;
        if( nai_length <= NV_MAX_NAI_LENGTH )
        {
          local_item.ds_mip_rm_nai.nai_length = nai_length;
          copy_size = memscpy( (char*) local_item.ds_mip_rm_nai.nai,
                                sizeof(local_item.ds_mip_rm_nai.nai),
                  nai_to_write,
                  nai_length );
          if( copy_size != nai_length )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
              copy_size,
              nai_length,
              sizeof(local_item.ds_mip_rm_nai.nai));
          }
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unexpected nai_length:%d",
            nai_length);
          local_item.ds_mip_rm_nai.nai_length = 0;
        }
        nai_update_state = NAI_UPDATE_DONE;
        break;
      }
    } /* switch */

    if (item_to_be_written == TRUE)
    {
      local_cmd.status = mcc_nv_cmd(&local_cmd);
      if (local_cmd.status != NV_DONE_S)
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "Failure writing %d to NV",
          (nv_items_enum_type)local_cmd.item);
        break;
      }
    }

    /* Reset data_ptr for next item */
    local_cmd.data_ptr = &local_item;

  } /* while */

} /* otasp_update_mdn_based_nai */
#endif /* FEATURE_MDN_BASED_NAI && FEATURE_OTASP_UPDATE_MDN_NAI */

#ifdef FEATURE_CSIM
/*===========================================================================

FUNCTION OTASP_MMGSDI_RESPONSE_CB

DESCRIPTION
  This function is called whenever MMGSDI returns with status.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

LOCAL void otasp_mmgsdi_response_cb
(
  mmgsdi_return_enum_type  mmgsdi_status,
                                     mmgsdi_cnf_enum_type     cnf,
                                     const mmgsdi_cnf_type    *cnf_ptr
                                    )
{
  size_t copy_size;
  /* Variable to store value  */
#ifdef FEATURE_1X_CP_MEM_OPT
  mmgsdi_cnf_type* otasp_mmgsdi_cnf_buf;
#endif /* FEATURE_1X_CP_MEM_OPT */

  if (cnf_ptr == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Null pointer passed");
  }
  else
  {
#ifdef FEATURE_1X_CP_MEM_OPT
    otasp_mmgsdi_cnf_buf = (mmgsdi_cnf_type*)modem_mem_alloc
                                    (sizeof(mmgsdi_cnf_type), MODEM_MEM_CLIENT_1X );
    if (otasp_mmgsdi_cnf_buf)
    {
      copy_size=memscpy(otasp_mmgsdi_cnf_buf, sizeof(mmgsdi_cnf_type),
                       cnf_ptr, sizeof(mmgsdi_cnf_type));

      if( copy_size != sizeof(mmgsdi_cnf_type) )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          sizeof(mmgsdi_cnf_type),
          sizeof(mmgsdi_cnf_type));
      }

      mcc_queue_mmgsdi_rpt(cnf, otasp_mmgsdi_cnf_buf);
      modem_mem_free(otasp_mmgsdi_cnf_buf, MODEM_MEM_CLIENT_1X);
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Memory allocation failed for otasp_mmgsdi_cnf_buf");
    }
#else /* !FEATURE_1X_CP_MEM_OPT */
    copy_size=memscpy(&(otasp_mmgsdi_cnf_buf), sizeof(mmgsdi_cnf_type),
                       cnf_ptr, sizeof(otasp_mmgsdi_cnf_buf));

    if( copy_size != sizeof(otasp_mmgsdi_cnf_buf) )
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
        copy_size,
        sizeof(otasp_mmgsdi_cnf_buf),
        sizeof(mmgsdi_cnf_type));
    }
    mcc_queue_mmgsdi_rpt(cnf, &(otasp_mmgsdi_cnf_buf) );
#endif /* FEATURE_1X_CP_MEM_OPT */
  }

  MODEM_1X_UNUSED(mmgsdi_status);
}
#endif

/*lint +e818 */
