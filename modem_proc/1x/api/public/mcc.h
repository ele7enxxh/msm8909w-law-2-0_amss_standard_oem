#ifndef MCC_H
#define MCC_H
/*===========================================================================

                        MCC Declarations

DESCRIPTION
   This header file contains the definitions and external interfaces
   to the MC CDMA Main Control subtask.

  Copyright (c) 1991 - 2013 Qualcomm Technologies, Inc. 
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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/mcc.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/13/09   jtm     Removed FEATURE_1X_DISABLE_CMI.
04/01/09   adw     Added temporary CMI-lite featurization.
03/04/09   adw     Added mcc_fwd_chan_mask_type from internal header.
03/04/09   adw     Removed the temporary _v.h include.
12/02/08   adw     Temporarily include the internal _v.h file.
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

/* Used for internal representation of active channels */
typedef uint8    mcc_fwd_chan_mask_type;

typedef uint8    mcc_rev_chan_mask_type;


#endif /* MCC_H */
