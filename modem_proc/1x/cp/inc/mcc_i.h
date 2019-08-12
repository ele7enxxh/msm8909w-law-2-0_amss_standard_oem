#ifndef MCC_I_H
#define MCC_I_H
/*===========================================================================

                        MCC Declarations

DESCRIPTION
   This header file contains the definitions and external interfaces
   to the MC CDMA Main Control subtask.

  Copyright (c) 1991 - 2014 Qualcomm Technologies, Inc. 
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mcc.h_v   1.9   01 Oct 2002 13:41:44   sbrelsfo  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mcc_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/28/14   ppr     Changes to support new cmd from MMODE, to abort  ongoing 
                   LTE_to_1X REDIR Attempt   
05/31/13   srk     Mainlined FEATURE_MODEM_1X_NON_SIG.
03/11/13   ppr     PPP cleanup changes when moving from 1x to LTE
                   in Non-SV mode
01/10/13   ppr     Avoid NV# writes for PROT_OPRT_MODE_LPM case
02/21/12   ppr     Feature Cleanup
02/04/12   ppr     Feature Cleanup
11/30/11   srk     Feature clean up: Mainlined and Internalized Features
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
01/19/11   jtm     Export mccidl_ftm_init.
12/13/10   ag      Replaced inclusion of prot.h by prot_v.h.
11/15/10   jtm     Initial SU API split for CP.
04/26/10   jj      CSFB change: IRAT core services code update.
01/15/10   jj      Implemented IRAT core services.
03/04/09   adw     Moved mcc_fwd_chan_mask_type to external api header.
01/08/08   ag      Mainlined FEATURE_LPM
03/19/07   pg      Lint Cleanup.
02/23/07   sb      Added support for operational mode feature.
01/15/07   sb      VOIP-1X handoff updates.
11/21/06   sb      Added support for VOIP-1X handoffs.
10/25/06   fc      Changes to not use entry reason to indicate TC to Idle
                   transition.
10/12/06   fc      Changes per code review comments.
09/21/06   fh      AKA/AES: added a new access state
                     MCC_ACCESS_STOPPED_MI_AES_RESYNC -- access stopped for
                     AKA/AES crypto-sync procedure
08/07/06   fc      Cleanup featurization.
07/10/06   fc      Renamed MCC_PSEUDO_IDLE_FOR_UMTS_HO to MCC_PSEUDO_IDLE.
07/06/06   fc      Renamed MCC_LOST_RF_RESOURCE to MCC_SRCH_REQUEST_SYS_RESTART.
06/29/06   fc      Added entry reason MCC_PSEUDO_IDLE_FOR_UMTS_HO.
05/19/06   fc      Added exit reason MCC_LOST_RF_RESOURCE.
04/26/06   an      Fixed lint errors
01/20/06   fc      Merged the following :
           fh      Added support for AKA and MI.
03/23/05   fc      Merged the following from MSMSHARED:
03/18/05   fc       Put back entry reason MCC_ENTER_IDLE_FROM_TC.
02/18/05   fc      Merged the following from MSMSHARED:
02/08/05   an       Mainlined IS95B features
02/03/05   fc      Fixed lint error.
02/03/05   fc      Removed unused entry reason MC_ENTER_IDLE_FROM_TC.
01/07/05   an      Removed feature FEATURE_SMS and FEATURE_UASMS
04/08/04   bkm     Moved channel mask enums from mc.h to this header.
01/05/04   pg      Mainlined FEATURE_MULTIMODE_ARCH.
09/11/03   yll     Added FEATURE_ACP for some analog code.
04/14/03   va      Added new entry reason MCC_ENTER_IDLE_FROM_TC to support
                   ReleaseB TC to idle transition.
09/30/02   sb      Mainlined FEATURE_SD20.
09/26/02   fc      Added exit reason MCC_ACCESS_FAILED_GSRM.
08/13/02   HQ      Added FEATURE_CDSMS.
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination
06/07/02   sj      Added CP support for multimode architecture.
02/13/02   sb      Merged the following from the common archive:
                   02/06/02   abh     Added code to go into GPS state when Phone is in
                                      Acquisition state.
                   02/06/02   sh      Added hybird operation support
                   01/30/02   az      CR 20247: Added an entry for access fail due to fade
                              in mcc_exit_name_type
                   01/12/02   az      Replaced 2 variables in mcc_sr_type with a single
                              variable favored_roam_ind.
10/01/01   kk      Merge from Common Archive.
                   09/19/01   az      Added a boolean flag fwd_link_failure in
                     mcc_sr_type to determine whether it is a forward link failure
                     or reverse link failure. This flag is used during the
                     building of silent redial scan list.
08/21/01   fc      Added exit reasons MCC_ENCRYPTION_FAILED and
                   MCC_ACCT_BLOCKED (SD 2.0 only).
02/16/01   fc      Added support for system determination 2.0. All changes
                   are featurized under FEATURE_SD20.
09/11/00   yll     Added support for Position Determination DBM.
03/06/00   ry      Merged the following from PP (MSM3100_CP.03.00.10):
           kk      mcc_exit_type modified to include mcc_system_lost_type
           ych     Added support for Tri-mode Silent Redial
11/01/99   lcc     Moved Pseudo MC (used in PLT) related to plt_mc.
09/16/99   lcc     Merged in PLT support from PLT archive.
08/18/99   lh      Bug fix in mcc_entry_orig_type.
08/13/99   lh      CM and SMS phase 2 implementations.
07/07/99   ht      Add mcc_new_system_type structure.
06/22/99   ck      Changed the prototype of function mcc_is_spc_locked to
                   mcc_ignore_activation_code.
05/06/99   kmp     Merge IS-95B changes below & change T_ACP to FEATURE_ACP
           kk      NDSS feature modifications; new exit type (MCC_NDSS_OFF)
                   and ndss parameters added.
           kka     New exit code for system reselection added.
01/18/99   abh     Changes for OTASP 3.0
08/04/98   ych     Merged E911, SSPR-800, and Acq. Improvements
07/20/98   pms     Added the MCC_AHO_EXIT_TYPE() macro.
06/25/98   kis     Added the ASCII related fields in mcc_entry_orig_type,
                   mcc_auto_nam_change_type and mcc_orig_cmd_rxed_type.
02/18/98   dna     Added a .gen member to the mcc_entry union to allow
                   easy reference to common channel and band class fields.
01/27/98   ldg     FEATURE_DS updates (by hand).
10/17/97   jrs     Added a redirection type field to the redirection exit
                   structure from CDMA. This is used to identify the type
                   of redirection as either global or directed.
06/10/97   jrs     Removed MCC_OTASP_ORIG_RXED exit reason.
05/20/97   ych     Added a new exit reason MCC_LPM.
02/06/97   jca     Added mcc_aho_page_type.
12/13/96   jca     Modified mcc_entry_orig_type.  Added mcc_aho_orig_type
                   and mcc_orig_cmd_rxed_type.
09/26/96   dna     Added MCC_OTASP_ORIG_RXED exit reason for FEATURE_OTASP
09/13/96   dna     Added new entry and exit reasons for FEATURE_OTASP
06/24/96   jrs     Added support for Preferred Roaming.
05/13/96   jca     Added band_class and block_or_sys to mcc_entry_acq_type.
02/08/96   jca     Added MCC_MAX_ACCESS_PROBES exit reason.
11/29/95   jca     Deleted MCC_ACQ_SID_ACQ enum from mcc_sys_acq_enum_type.
07/20/95   jca     Changes to support Subscriber Access Control.
06/15/95   jca     Added MCC_AUTO_NAM_CHANGE exit reason.
11/14/94   dna     Added MCC_AHO_WAIT_FOR_PAGE exit reason and removed
                   aho_page_response from the mcc_exit_type.
04/20/94   jca     Externalized MCC_POWERUP_LOAD function to facilitate
                   resetting NAM.
05/04/93   jca     Added new entry and exit reasons.
09/03/92    ip     Added normal_exit structure to ACQ_LOST exit reason.
08/29/92    ip     Changed mcc_powerup_init to typed (boolean) procedure.
08/22/92    ip     Added exit types to idle timeout and acq terminated.
07/04/92    ip     Initial creation.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "target.h"
#include "customer.h"
#include "nv.h"
#include "comdef.h"

#include "caii.h"

#include "prot_v.h"
#include "mcc.h"
#include "mcc_v.h"

/* <EJECT> */
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------------*/
/* Macro MCC_AHO_EXIT_TYPE() returns TRUE if arg is one of the 5 AHO types */

#define MCC_AHO_EXIT_TYPE(arg) \
        (((arg) == MCC_AHO_VOICE) || \
        ((arg) == MCC_AHO_ORIGINATION) || \
        ((arg) == MCC_AHO_PAGE_RESPONSE) || \
        ((arg) == MCC_AHO_WAIT_FOR_PAGE) || \
        ((arg) == MCC_AHO_TC_ASSIGNMENT))

/* Used for internal representation of active channels */

#define MCC_F_RESERVED  ((mcc_fwd_chan_mask_type)0)
#define MCC_F_FCH       ((mcc_fwd_chan_mask_type)1)
#define MCC_F_DCCH      ((mcc_fwd_chan_mask_type)2)
#define MCC_F_CPCCH     ((mcc_fwd_chan_mask_type)4)
#define MCC_F_PDCH      ((mcc_fwd_chan_mask_type)8)


#define MCC_R_RESERVED  ((mcc_fwd_chan_mask_type)0)
#define MCC_R_FCH       ((mcc_fwd_chan_mask_type)1)
#define MCC_R_DCCH      ((mcc_fwd_chan_mask_type)2)
#define MCC_R_PICH      ((mcc_fwd_chan_mask_type)4)
#define MCC_R_PDCH      ((mcc_fwd_chan_mask_type)8)



/*-------------------------------------------------------------------------*/
/* General entry parameters.  All members of the 'param' union must have   */
/* These fields first.                                                     */

typedef struct
{
  byte             band_class;         /* CDMA band class        */
  word             cdma_channel;       /* Channel to acquire     */
  sd_blksys_e_type block_or_sys;       /* Cellular serv-sys (A or B) */
  boolean          new_mode_or_blksys; /* Is new mode or blksys      */
  boolean          enter_idle_from_tc; /* Indicates whether MS enter Idle
                                          directly from TC */
} mcc_entry_gen_type;


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  Entry reason and parameters structure.                                 */
/*                                                                         */
/*-------------------------------------------------------------------------*/

typedef struct {
  mcc_entry_name_type            entry;      /* Specifies entry type      */
  mcc_entry_gen_type             gen;        /* Items in common for all   */
                                             /* MCC_ACQUISITION           */
                                             /* MCC_ENTER_OFFLINE         */
} mcc_entry_type;

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*  Reasons for exit out of CDMA Call Processing.                          */
/*                                                                         */
/*-------------------------------------------------------------------------*/
typedef enum {
  MCC_RESELECTION,        /* Normal exit indication                        */
  MCC_ACQ_FAILED,         /* Could not acquire CDMA system                 */
  MCC_RELEASE,            /* Call has been released                        */
  MCC_SYSTEM_LOST,        /* Acquisition lost                              */
  MCC_MAX_ACCESS_PROBES,  /* Max access probes txed with no BS response    */
  MCC_NEW_SYSTEM,         /* Idle Handoff to unknown configuration         */
  MCC_RESCAN,             /* RESCAN field in System Params Msg set to '1'  */
  MCC_REDIRECTION,        /* Service redirection commanded by base station */
  MCC_REG_REJECTED,       /* Registration request rejected by base station */
  MCC_ACCESS_DENIED,      /* Mobile station failed access persistence test */
  MCC_AHO_VOICE,          /* Analog handoff: acquire and talk              */
  MCC_AHO_ORIGINATION,    /* Analog handoff: originate a call              */
  MCC_AHO_PAGE_RESPONSE,  /* Analog handoff: perform a page response       */
  MCC_AHO_WAIT_FOR_PAGE,  /* Analog handoff: wait for a page               */
  MCC_AHO_TC_ASSIGNMENT,  /* Analog handoff: traffic channel assignment    */
  MCC_LOCK,               /* Received LOCK or LOCK_P order                 */
  MCC_UNLOCK,             /* Received Unlock Order                         */
  MCC_INCOMP_CAI_REV,     /* MOB_CAI_REV < MIN_CAI_REV                     */
  MCC_PREF_SYS_CHANGED,   /* System preferences have changed               */
  MCC_NAM_CHANGED,        /* NAM has changed                               */
  MCC_OFFLINE_ANALOG,     /* Received command to go into offline analog    */
  MCC_OFFLINE_DIGITAL,    /* Received command to go into offline digital   */
  MCC_RESET,              /* Received command to reset                     */
  MCC_POWERDOWN,          /* Received command to powerdown                 */
  MCC_NOT_PREFERRED,      /* Acquired system is not preferred              */
  MCC_LPM,                /* Received command to enter Low Power Power Mode*/
  MCC_SYSTEM_RESELECTION, /* Switch to analog                              */
  MCC_NDSS_OFF,
  MCC_ACCESS_COMPLETED,   /* Mobile station completed access               */
  MCC_ACCESS_FAILED,      /* Received RXC fade report */
  MCC_ACCESS_FAILED_GSRM, /* Access failed due to GSRM */
  MCC_ACCESS_FAILED_SYS_LOST, /* Access failed due to fade */
  MCC_ACCESS_TIMEOUT,     /* Mobile station timeout access                 */
  MCC_INTERCEPT,          /* Received INTERCEPT order                      */
  MCC_REORDER,            /* Received REORDER order                        */

  MCC_ENCRYPTION_MI_FAILED,  /* Encryption / Message Integrity failure     */
  MCC_ACCT_BLOCKED,          /* ACCT blocked                                  */

  MCC_ACCESS_STOPPED_MI_AES_RESYNC,
  /* when MI or AES fails, MS starts a crypto-resync procedure. Before SMRM
   * is sent for this procedure, we need stop all the current access
   */

  MCC_ACQ_ABORT_GPS_START,

  MCC_SRCH_REQUEST_SYS_RESTART, /* Searcher requested system restart */
  MCC_VOIP_TO_1X_HO_FAILED,     /* VOIP to 1X handoff failed */
  MCC_SHUTDOWN,                 /* Shutdown 1X in order to activate other protocol */
  MCC_IRAT_SWITCH_ABORT,        /* Abort the 1X redir/resel triggered from LTE-RRC */
  MCC_IRAT_SWITCH_FAIL,         /* Failure while doing 1X redir/resel switch */
  MCC_PPP_CLEANUP,              /* Failure while processing deregister_f cmd 
                                 * (PPP_CLEANUP_NON_SV_MODE) */
  MCC_REDIR_ABORTED             /* MMODE aborted ongoing REDIR Attempt */

} mcc_exit_name_type;

/*-------------------------------------------------------------------------*/
/* MCC_NAM_CHANGED exit parameters                                     */

typedef struct
{
  byte                    nam;        /* NAM */
  sd_ss_orig_mode_e_type  orig_mode;  /* Origination mode */
  sd_ss_mode_pref_e_type  mode_pref;  /* Mode preference */
  sd_ss_band_pref_e_type  band_pref;  /* Band preference */
  sd_ss_roam_pref_e_type  roam_pref;  /* Roam preference */
  sd_ss_hybr_pref_e_type  hybr_pref;
} nam_change_type;

/*-------------------------------------------------------------------------*/
/*                                                                         */
/* Exit structure.                                                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/

/* Note that all cdma-to-analog exit reasons are imported from file        */
/* apcmc.h, where they are defined as entry reasons.                       */

typedef struct {
  mcc_exit_name_type exit;        /* Specifies exit reason                 */

  /* MMoC's transaction Id. If this is set PROT_TRANS_NONE and protocol
  ** deactivated report has to be sent to MMOC, then the
  ** autonomous protocol deactivated indication will be sent instead
  ** of deactivated confirmation.
  */
  prot_trans_type    mmoc_trans_id;

  union {                         /* Parameters for:                       */
    nam_change_type          nam_change;        /* MCC_NAM_CHANGED       */
  } param;
} mcc_exit_type;

/* -------------------------------------------------------*/
/*                                                        */
/* Reasons of entry to CDMA call processing for Diag info */
/*                                                        */
/* -------------------------------------------------------*/
typedef enum
{
  MCC_ONLINE = 0, /* Digital online  */
  MCC_OFFLINE = 2 /* Digital offline */
} mcc_entry_reason_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION MCC_SUBTASK

DESCRIPTION
    This is the entry procedure for the MCC sub-task. It contains the main
    processing loop for the task.

DEPENDENCIES
    mcc_powerup_init() should be called once on powerup before calling
    this procedure.

RETURN VALUE
    mcc_exit_type - pointer to structure indicating reason for exit

SIDE EFFECTS
    None

===========================================================================*/

extern mcc_exit_type *mcc_subtask
(
  mcc_entry_type *mcc_entry_reason_ptr  /* Reason for entry */
);


/*===========================================================================

FUNCTION MCC_POWERUP_INIT

DESCRIPTION
    This procedure should be called once upon powerup to allow MCC to
    perform internal initialization

DEPENDENCIES
    This procedure loads parameters from NV.  The NV task should be ready
    to receive commands when this procedure is called.

RETURN VALUE
    TRUE  - NV parameters successfully loaded.
    FALSE - NV parameters could not be loaded.

SIDE EFFECTS
    None

===========================================================================*/

extern boolean mcc_powerup_init( void );

/* <EJECT> */
/*===========================================================================

FUNCTION MCC_POWERUP_LOAD

DESCRIPTION
  This function loads CDMA parameters from NV.

DEPENDENCIES
  None

RETURN VALUE
  Indicator of whether all parameters were successfully read from NV.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mcc_powerup_load( void );


/*===========================================================================

FUNCTION MCC_OFFLINE

DESCRIPTION
    This procedure should be called when the DMSS is to transition to
    OFFLINE state.  It performs the necessary processing required before
    OFFLINE state.

DEPENDENCIES
    ???

RETURN VALUE
    None

SIDE EFFECTS
    None

===========================================================================*/

extern void mcc_offline (void);


/*===========================================================================

FUNCTION MCC_POWERDOWN

DESCRIPTION
    This procedure should be called once when the DMSS is to power down.
    It performs the necessary processing required before powering down.

DEPENDENCIES
    This procedure saves parameters in NV.  The NV task should be ready
    to receive commands when this procedure is called.

RETURN VALUE
    None

SIDE EFFECTS
    None

===========================================================================*/

extern void mcc_powerdown ( prot_oprt_mode_e_type oprt_mode );

/*===========================================================================

FUNCTION MCC_IGNORE_ACTIVATION_CODE

DESCRIPTION
  This function checks the status of the SPC and validity of PRL
  to decide if it should not use the act_code dialed by user to acquire
  a system for Activation.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if MS should ignore activation code else FALSE.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean mcc_ignore_activation_code(void);

/*===========================================================================

FUNCTION MCCIDL_FTM_INIT

DESCRIPTION
  This procedure initializes idle state non-slotted mode operation.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccidl_ftm_init(void);
#endif /*MCC_I_H*/
