/*===========================================================================

  S C R   A N D   H O   T R A N S A C T I O N S   C L I E N T   M O D U L E

GENERAL DESCRIPTION
  This module contains data structures and utilities related to SCR and HO
  transaction.

  Copyright (c) 2000 - 2014 Qualcomm Technologies, Inc.
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
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mcctxns.c_v   1.38   02 Oct 2002 17:13:56   phosabet  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mcctxns.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/09/16   ppr     1x2GTA Feature changes
05/06/14   gga     1X_CP F3 redistribution
03/07/13   abd     Fix MSVC compilor errors
05/31/12   cjb     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
05/24/12   ppr     Feature Cleanup
05/01/12   jj      CSFB: Made changes to have immediate transaction for CSFB
                   cases.
04/06/12   ssh     Skipped RCP transactions during non 1xA RCs to non 1xA RCs
02/21/12   ppr     Feature Cleanup
02/15/12   srk     Feature Cleanup: Removed FEATURE_GPS_MODE
02/04/12   ppr     Feature Cleanup
01/27/12   ppr     Feature Cleanup: Mainlined Always ON features
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
11/30/11   adw     Set flag to allow SRCH to differentiate between hard/soft HO.
06/15/11   ag      Changes to handle GEM related failure scenarios.
06/15/11   ag      Changes to keep FPC_MODE of RCPM and NNSCR in sync.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
01/04/11   ag      Fixed compiler and lint warnings.
11/15/10   jtm     Initial SU API split for CP.
09/15/10   azh     Fixed KlocWork warnings.
07/22/10   ag      Removed RCPM from pending_action_msg structure.
07/22/10   ag      Fixed issues in processing radio configration parameters.
07/15/10   ssh     Fixed Compiler Warnings.
10/21/09   jtm     Fixed ARM compiler warnings.
09/29/09   ag      1X Advanced Signaling Support - Phase 2
08/21/09   jtm     Lint Fixes.
08/02/09   jtm     Lint fixes.
05/14/09   jj      CMI Phase-II update.
04/29/09   jj      Lint fixes.
04/29/09   jj      Replaced assert.h with amssassert.h
03/20/09   jtm     Removed unnecessary include of mctrans.h.
04/08/08   pg      Featurization and general code cleanup for memory reduction
                   effort.
03/29/08   bb      Fixing the Klocwork errors
04/30/07   an      Fixed a REL_A featurization issue
04/04/07   pg      Fixed a debug message.
03/19/07   pg      Lint Cleanup.
04/30/06   fc      Added rif_ind to mctrans_rxc_hho_begin_action_type.
05/02/06   an      Fixed Lint Errors
04/24/06   fc      Fixed Rel C featurization.
03/14/06   fh      Added AES support
11/29/05   fc      Fixed compiler warnings.
11/04/05   va      Added null pointer checks for all pending_msg pointers
10/15/05   fc      Fixed default PLCM type for Rel D.
09/29/05   sb      Removed DB_VOICE_PRIVACY.
07/26/05   fc      Fixed lint errors.
07/07/05   va      Merged from MSMSHARED:
07/05/05   sb       Added MEID support.
03/28/05   fc      Made FCS also available to Rel D.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
03/01/05   fc      Merged the following from MSMSHARED:
02/23/05   sb       Set the chind in mcctxns_ho_pre.
02/18/05   fc      Merged the following from MSMSHARED:
02/08/05   an       Mainlined IS95B features
02/17/05   sb      Fixed featurization.
02/16/05   sb      Fixed lint warnings.
02/15/05   sb      Featurized variable to get rid of compiler warning.
02/13/05   sb      Set ext_ch_ind properly when generating channel masks.
02/08/05   sb      Returned check for rev_sch_cc_incl when checking num_rev_sch.
02/03/05   sb      Made sure soft_switching_delay is included for non-DV to DV.
02/03/05   az      Changes for Rel D DV TDSO.
01/17/05   sb      Fixed incorrect condition check.
01/14/05   sb      Release D interface updates.
01/03/05   va      mcctxns_begin_tx_on_off now takes an argument for ON/OFF
12/07/04   sb      Fix compiler error.
12/01/04   sb      Fixes for DV RIF HHO's.
11/22/04   sb      Miscellaneous fixes for Rel C DV Handoffs.
11/18/04   va      Fixed the eror which causs FCH to be restored errorneously
11/08/04   va      Fixed lint error
11/05/04   va      Merged the following from REL_B_C archive:
11/04/04   fc        Fixed lint errors
09/30/04   yll     Added the rc, sch_rate and code_type in F-SCH Action
                   Packet to Search.sch
11/05/04   sb      Added support for Rel C DV handoffs.
10/21/04   va      Interface changes for R-DV RLGC interface
10/07/04   fc      Added some P_REV checks.
09/28/04   fc      Changed R_DV_PARMS transaction packet looping to include
                   aggregate entry.
09/27/04   va      Merged the following from REL B/C archive
08/24/04   yll      Clean up the MC-SRCH interface.
08/23/04   va       Changing sending fpc_mode from UHDM to RXC if f-sch is
                    in progress
09/17/04   az      Added assignment for stamping action type in action packet
                   in mcctxns_dv_tdso_action_packet
09/16/04   fc      Added support for reverse DV.
08/20/04   bkm     Some lint cleanup.
08/19/04   ht      Changes to not involving RXC/TXC when only clear counter is needed.
08/09/04   fc      Service negotiation changes for Release C and D.
08/05/04   va      Made return txn an ordered one for non-GPS cases.
08/02/04   bkm     Fix compile issue wrt tc_gps_in_progress.
08/02/04   va      Changes CFS txn to be an ordered transaction
07/29/04   ht      Added TDSO support for Rel C DV channels
07/27/04   va      Made HHO_BEGIN an ordered txn; Fixed SRCH parms on Return txn
06/30/04   bkm     Lint cleanup.
06/29/04   bkm     Added srv.h to clean up warnings.
06/29/04   pg      Added function mcctxns_init_txc_f_dv_params().
06/28/04   bkm     Merged the following from IS2000_REL_B_C:
  06/24/04   sb      Release C addendum changes for PLCM.
  06/08/04   bkm     Send MC_TRANS_DONE_R for HHO as well as SHO transactions.
06/28/04   va      Fixed some compile errors
06/24/04   bkm     Do the +1 for num_arq_rounds_normal/boost for PCNM.
06/23/04   bkm     Added init of Rel C/D RLGC fields to mcctxns_init_rlgc().
06/21/04   va      Changes for HHO/CFS/GPS clean up
06/16/04   pg      Make mcctxns_init_txc_r_dv_params() global instead of static.
06/17/04   bkm     Ensure FFPC action fields set safely for no-fwd-fundicated
                   channel case.
06/04/04   bkm     Lint cleanup.
06/03/04   bkm     Merged the following from IS2000_REL_B_C:
  06/02/04   sb      Fixed lint errors.
  05/24/04   fc      Included mccsyncid.h.
05/28/04   bkm     Merged the following from IS2000_REL_B_C:
  05/07/04   fc      Changed gating_rate_to_pwr_delay to static.
  04/19/04   sb      Moved code to new fn: mcctcho_update_long_code_mask_info.
  04/09/04   sb      SCM no longer reverts to public if using private
04/22/04   bkm     MC stores FFPC info received when there is no active
                   fwd-fundicated channel, add checks before sending FPC info
                   to lower layers.
04/20/04   bkm     Use snm_update_nn_scr_switch_params() instead of
                   mccdv_set_num_soft/softer_switching_frames_chm() ifaces.
04/16/04   bkm     Fixed a few compile issues.
04/14/04   az      Added support for ERM - Rel C DV chan cfg change
04/08/04   bkm     Channel mask updates.
03/26/04   bkm     Merged the following from IS2000_REL_B_C:
  02/09/03   ph      When checking phy channels being sent to TXC, make exception
                     for TDSO/STDSO.
  03/19/04   sb      Added support for Release C Decoupling PLCM from ESN
  03/12/04   fc      Added support for Release C fast call setup.
03/04/04   bkm     Fill in F-PDCH/CPCCH fields for TXC and RXC SCR actions.
02/19/04   bkm     Fixed misc ERM compilation issues.
02/11/04   az      Added changes for supporting new transaction engine design
02/05/04   bkm     Merged from IS2000_REL_B_C
  01/27/04   va      If RAM involves sending SCR & FFPC action to RXC, make
                     sure SCR is included before FFPC
01/28/04   bkm     Misc updates for CHIND and EXTCHIND ERM handling.
01/15/04   bkm     Updated PCM handling for Rel C DV.
01/05/04   bkm     Misc updates for DV F-PDCH and chan masks.
12/02/03   bkm     Split internal chind into forward and reverse channel masks.
07/22/03   bkm     Some more lint cleanup.
06/24/03   bkm     Fixed comment in mcctxns_ext_rel_action_packet() wrt RLGC.
06/10/03   bkm     Lint cleanup.
05/29/03   bkm     Use NULL rather than 0 to initialize function pointers for
                   empty pre-processor, post-processor or action pkt generators.
05/28/03   sb      Removed redundant call to set_cur_chind in mcctxns_ho_pre.
05/20/03   fc      Changed mcctxns_ho_pre to have the saving of serving config
                   and the update of CCL for UHDM with RIF.
05/19/03   ph      Filled in payload for SRCH FFPC Action for SCR transaction.
10/02/02   ph      Fixed SVD featurization.
10/02/02   az      Mainlined FEATURE_IS2000 and FEATURE_IS2000_2_1_ADDEDNDUM
09/05/02   va      Cleaned up lint errors
09/04/02   va      Suport for turning rev FCH 1/8 rate gating ON if DCCH is
                   released through ERM.
08/14/02   va      Added RLGC action to TXC when FCH or DCCH is released or
                   added.
08/08/02   ph      Error checking before passing phy chans to mux updated.
08/07/02   yll     Fixed the compiling errors when FEATURE_IS95B_MAHHO is
                   turned off.
08/05/02   va      Code review comments' changes.
08/01/02   va      Fixed compiler errors when REL_A is turned off
07/19/02   va      Added support for storing FCH config on CH and restoring upon
                   going active, if required by the BS.
07/15/02   ph      Checks to make sure SCR TXC packet does not have signaling
                   phy channel (rev signaling) as zero.
06/26/02   ph      sr_id range check in scr trans to mux
06/19/02   va      Fixed compiler issues when REL_A is turned off
06/18/02   ph      Filter out phy channels before giving to mux in SCR transaction.
06/18/02   jrp     Changed enum to match updated cai_fpc_mode_type.
06/05/02   ph      Cleaned up LPM access to use pending cfg.
05/31/02   lcc     Added control hold action for RXC which is responsible for
                   programming DEM_GATING_MASK.
05/31/02   va      Added VP2 support.
05/29/02   ph      Added support for LPM table
04/18/02   ph      fix for handling num_con_rec = 0 for non-release A builds.
04/01/02   ph      Look in to num_sch in addition to incl fields before using supplemental
                   channel.
02/13/02   va      Merged from MSM archive
  02/06/02 vlc     In mcctxns_scr_action_packet, set primary SO only when
                   num_con != zero.
  01/18/02 lcc     In mcctxns_ho_pre, if UHDM is received with RIF, do not update
                   CCL.  It will be update later in mcctcho_process_tx_stop after
                   the serving config is saved.
  01/04/02  ht      Enclosed mccsch_stop_sch0_trans() within FEATURE_IS2000_SCH.
1/21/02    ph      Sync up SCR interface with MUX, revert them to REL_A featurization.
12/11/01   ph      merged from SVD branch
                   11/29/01   ph      specified SCR interface changes for SVD builds.
                   11/29/01   ph      Restoring old interface for SCR for non-relA builds.
                   08/24/01   ph      Updated SCR trans to mux action packet to only include CCs in
                                      non-release substates.
                   08/23/01   ph      Updated transaction that gives SCR information to MUX for handling multiple
                                      connections.
12/10/01   va      Merged from MSM_CP.05.00.108
           11/27/01 lcc     Added support for special values of 0xff
                    for min/max set points in, which are used to indicate
                    if they need to be set to the current set point.
          11/15/01 lcc     In mcctxns_init_ffpc, added initialization of
                   dcch_olpc.include to FALSE.  Without this, wrong DCCH
                   set points may be used.
11/27/01   va      Merged from MSM_CP.05.00.100.01.00
           10/25/01   lcc     In mcctxns_begin_ho_trans, do not
                      include control hold action if handing off to FCH
10/26/01   jrp     Added forward and reverse SCH0_MUX_OPTION to SCR message to driver.
10/23/01   kk      Merge to common archive cP VU 99.
           10/17/01   lcc     In mcctxns_begin_ho_trans, do not
                   include gating action when DCCH is included.
           10/05/01   lcc     Moved call to mccsch_update_sch_assign_with_uhdm to
                   send_ho_cmds_to_lower_layers in mcctcho.c
           09/25/01   sj      In mcctrans_begin_ho(), void pending
                   scr transactions only if ch_ind has not changed.
10/02/01   kk      Removed identifier "const" in function mcctxns_ho_pre().
10/01/01   kk      Merge from common archive
           08/22/01   lcc     Moved update of CCL and SCH info to mcctxns_ho_pre from
                              from mcctcho to make sure that if SCH transaction is included
                              in the message, the CCL and SCH info are current when the SCH
                              transaction starts.
08/09/01   jrp     Merged the following from the common archive (MSM_CP.05.00.81 to MSM_CP.05.00.87)
           08/02/01   lcc     Added check for rpc_incl before send RPC records to RLGC in
                              mcctxns_pwr_ctrl_action_packet.
           07/13/01   lcc     In mcctxns_control_hold_pre, removed code to clear gating rate
                              when gating rate is not included in ERM.
           06/21/01   lcc     Corrected a misplaced #ifdef for P2 which caused non-P2
                              builds to fail on FCH.
           06/20/01   lcc     Added RLGC action to SCR transaction when the R-FCH/DCCH
                              RC changes.
06/26/01   fc      Merged the following from common archive:
           06/21/01   lcc     Corrected a misplaced #ifdef for P2 which caused
                              non-P2 builds to fail on FCH.
           06/20/01   lcc     Added RLGC action to SCR transaction when the
                              R-FCH/DCCH RC changes.
           06/01/01   fc      Changed snm_update_non_neg_configuration to accept
                              constant pointer argument.
           06/01/01   jq      Wrapped some code in FEATURE_IS95B_MAHHO.
           05/29/01   fc      Changed snm_update_non_neg_configuration function
                              prototype.
06/14/01   va      Change tc_can_so_be_connected fn call to
                   mccccim_can_so_be_connected.
05/25/01   va      Changed the argument to snm_update_non_neg_cfg().
05/17/01   lh      Merged the following from common archive:
  05/03/01   jq      Modified HO txns related to control hold.
  04/10/01   lcc/jq  Added support for P2 mode and control hold.
  03/31/01   fc      Fixed incorrect condition checkings based on
                     service_included.
                     Corrected the update of service configuration for soft handoff
                     in mcctxns_ho_pre.
05/04/01   va      Added setting p_rev_in_use in HO action pkt.
04/17/01   va      Added support to pass on pilot_rec_types to SRCH in CF NSET
                   transaction.
04/05/01   va      Added p_rev_in_ise check when accessing srch_offset field in CFS
                   pilot_rec related fields.
03/02/01   va      Merged the following from common archive:
                   Req msg.
  01/16/01 jq      Made some naming changes to variables and functions. Added
                   FEAUTURE ifdefs to make it compilable when SCH is undefed
09/25/00   fc      Changed mcctxns_begin_scr_trans() such that during the
                   SCR transaction packet preparation, MCTRANS_GATING_ACTION
                   will not overwrite MCTRANS_FFPC_ACTION when both ffpc_incl
                   and rev_fch_gating_on are true.
08/23/00   lcc     1. Corrected mcctxns_ho_action_packet such that the field use_sch
                      is set properly for SRCH.
                   2. Added some comments.
07/26/00   va      Code Review Changes for 8th rate gating.
07/21/00   fc      Used enumerated type for FFPC mode.
07/21/00   va      Fixed the HHO problem when extra_params=0.
07/17/00   va      Added support for the new CFS transactions.
07/10/00   va      Fixed a bug under the new transaction mechanism which would
                   cause any HHO with RC change to fail.
06/19/00   lcc     1. Changes for making SCH init_setpt not included in PCNM for
                      mctrans_ffpc_action_type.
                   2. Added support for 2/1 addendum version of PCNM.
06/15/00   lcc     Following changes related to HO action:
                   1. Corrected a typo such that HO will fail is it's SHO and if
                      SCR is included.
                   2. Correctly pass the num_preamble value to TXC based on RC
                      and also if it MSM5000A or MSM5000B.
                   3. nom_pwr is now optional for TXC.  It's not included when
                      the handoff message is the old HDM.
04/12/00   lcc     Initial release.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "modem_1x_defs.h"
#include "modem_1x_utils.h"

// This file is only used for IS2000 targets

#include "amssassert.h"
//henti hack #include "bit.h"
#include "cai_v.h"
#include "caii_v.h"
#include "err.h"
#include "mccap_v.h"
#include "mccsch.h"
#include "mccsch_i.h"
#include "mcctcho_i.h"
#include "mcctci_i.h"
#include "mcctcsup.h"
#include "mcctxns.h"
#include "snm_v.h"
#include "snm_i.h"
#include "srch_rx_t.h"
#include "srch_rx_t_i.h"
#include "mccccim.h"
#include "srchtc_ta.h"
#include "m1x_time_i.h"

#if defined(FEATURE_IS2000_REL_C)
#include "mccsyncid.h"
#endif /* FEATURE_IS2000_REL_C */

/*===========================================================================

                 PROTOTYPES FOR SOME FUNCTIONS USED LOCALLY

===========================================================================*/
/* MCTRANS_LONG_CODE_MASK */
static void mcctxns_long_code_pre
(
  mctrans_trans_info_type * curr_trans
);

static void mcctxns_long_code_action_packet
(
  mctrans_trans_info_type *   curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
);

static void mcctxns_generic_post
(
  mctrans_trans_info_type * curr_trans
);

// MCTRANS_SCR related functions
static void mcctxns_scr_pre
(
  mctrans_trans_info_type * curr_trans
);
static void mcctxns_scr_action_packet
(
  mctrans_trans_info_type * curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
);

// MCTRANS_HO related functions
static void mcctxns_ho_pre
(
  mctrans_trans_info_type * curr_trans
);
static void mcctxns_ho_action_packet
(
  mctrans_trans_info_type * curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
);

// MCTRANS_FPC_SUBCHAN_GAIN related functions
static void mcctxns_subchan_gain_action_packet
(
  mctrans_trans_info_type * curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
);
static void mcctxns_subchan_gain_post
(
  mctrans_trans_info_type * curr_trans
);
static void mcctxns_subchan_gain_pre
(
  mctrans_trans_info_type * curr_trans
);

// MCTRANS_PWR_CTRL_MSG related functions
static void mcctxns_pwr_ctrl_action_packet
(
  mctrans_trans_info_type * curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
);
static void mcctxns_pwr_ctrl_pre
(
  mctrans_trans_info_type * curr_trans
);

// MCTRANS_CONTROL_HOLD related functions
static void mcctxns_ext_rel_action_packet
(
  mctrans_trans_info_type * curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
);
static void mcctxns_ext_rel_pre
(
  mctrans_trans_info_type * curr_trans
);
static void mcctxns_ext_rel_post
(
  mctrans_trans_info_type * curr_trans
);

// MCTRANS_ACTIVE related functions
static void mcctxns_active_action_packet
(
  mctrans_trans_info_type *   curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
);
static void mcctxns_active_pre
(
  mctrans_trans_info_type *   curr_trans
);
static void mcctxns_active_post
(
  mctrans_trans_info_type *   curr_trans
);

// MCTRANS_CFS_NSET and MCTRANS_CFS_CTRL related functions
extern const dword srch_period_value[];
static void mcctxns_cfs_nset_action_packet
(
  mctrans_trans_info_type * curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
);


static void mcctxns_cfs_pre
(
  mctrans_trans_info_type * curr_trans
);

// MCTRANS_HHO_BEGIN related functions
static void mcctxns_hho_begin_pre
(
  mctrans_trans_info_type * curr_trans
);
static void mcctxns_hho_begin_action_packet
(
  mctrans_trans_info_type * curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
);

// MCTRANS_RETURN related functions
static void mcctxns_return_pre
(
  mctrans_trans_info_type * curr_trans
);
static void mcctxns_return_action_packet
(
  mctrans_trans_info_type * curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
);

// MCTRANS_TX_ON_OFF related functions
static void mcctxns_tx_on_off_pre
(
  mctrans_trans_info_type * curr_trans
);
static void mcctxns_tx_on_off_action_packet
(
  mctrans_trans_info_type * curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
);

// MCTRANS_CFS_GPS_PREP related functions
static void mcctxns_cfs_gps_prep_pre
(
  mctrans_trans_info_type * curr_trans
);
static void mcctxns_cfs_gps_prep_action_packet
(
  mctrans_trans_info_type * curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
);

/* Helper function for preparing the SCR actionpacket values */
static void mcctxns_prepare_scr_action_packet
(
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet,
  const caii_srv_cfg_type  *config,
  const caii_non_neg_srv_cfg_type *nn_config
);

#ifdef FEATURE_IS2000_1X_ADV
/* MCTRANS_RCP related functions */
static void mcctxns_init_rcp_trans
(
  mctrans_trans_info_type * curr_trans
);
static void mcctxns_rcp_pre
(
  mctrans_trans_info_type * curr_trans
);
static void mcctxns_rcp_action_packet
(
  mctrans_trans_info_type *   curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
);
#endif /* FEATURE_IS2000_1X_ADV */

/* Need access to cand Freq SRCH param info in mcctcho.c */
extern mcctcho_cand_freq_srch_parm_type cand_freq_srch_parm;

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
// Converts pilot gating rate to reverse power control delay
static uint8 gating_rate_to_pwr_delay[] =
{
  CAI_DEFAULT_REV_PWR_CNTL_DELAY, // Default is 1, i.e. delay of 2
  0, // Half rate gating delay is 1, value is 1 less than actual value used
  1, // Quarter rate gating delay is 2, value is 1 less than actual value used
};
/*===========================================================================

FUNCTION mcctxns_begin_long_code_trans

DESCRIPTION
  Starts a Long Code Mask transaction.  This transaction can switch between
  private and public long code masks, and can also switch between public
  long code mask types.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_long_code_trans
(
  qword trans_time,
  mctrans_pending_action_msg_type* msg
)
{
  uint16 trans_num;

  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_COUNT,                     // Transaction type
    MCTRANS_MODE_EXPLICIT,
    NULL,
    NULL,
    NULL
  };

  despatch_info.transaction_type = MCTRANS_LONG_CODE_MASK;

  despatch_info.preprocessor = mcctxns_long_code_pre;
  despatch_info.postprocessor = mcctxns_generic_post;
  despatch_info.action_packet_gen = mcctxns_long_code_action_packet;

  trans_num = mctrans_despatch_transaction( trans_time, &despatch_info, msg );

  M1X_MSG( DCP, LEGACY_MED,
    "Long Code Mask transaction despatched, trans_num %d",
    trans_num );

  (void) trans_num; /* Added to satisfy compiler warning (set but not used) */

} /* mcctxns_begin_long_code_trans */

/*===========================================================================

FUNCTION mcctxns_init_long_code_trans

DESCRIPTION
  Starts a Long Code Mask transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_init_long_code_trans
(
  mctrans_trans_info_type * curr_trans
)
{
  #ifdef FEATURE_IS2000_REL_C
  cai_plcm_type new_plcm_type;
  #endif /* FEATURE_IS2000_REL_C */

  const mctrans_pending_action_msg_type* pending_msg =
    mctrans_get_pending_msg( curr_trans->trans_num );

  if (pending_msg && (pending_msg->gen.msg_type == CAI_TC_FWD_ORD_MSG)
      && (pending_msg->order.ordq.order == CAI_PLC_ORD))
  {
    /* A Long Code Transition Request Order toggles the mobile between public and
       private long code mask.  If the LCTRO requests privacy and the mobile is
       using public, or the LCTRO requests public and the mobile is using privacy,
       we must setup the long code action. */


    if (((pending_msg->order.ordq.ordq == CAI_PLC_REQ_PRI)
         && !cdma.long_code.private_lcm_active)
        || ((pending_msg->order.ordq.ordq == CAI_PLC_REQ_PUB)
            && cdma.long_code.private_lcm_active))
    {
      mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_LCM_ACTION);
      mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_LCM_ACTION);
    }


  }

  #ifdef FEATURE_IS2000_REL_C
  else if (P_REV_IS_9_OR_GREATER && pending_msg &&
           (pending_msg->gen.msg_type == CAI_SRV_CON_MSG) &&
           pending_msg->service_connect.use_type0_plcm)
  {
    /* P_REV >= 11 SCM with use_type0_plcm = 1, means MS must use MEID-based plcm.
       P_REV 9 SCM with use_type0_plcm = 1, means MS must use ESN-based plcm,.
       So if the mobile is using privacy or a different public long code mask,
       we must setup the long code mask action. */
    new_plcm_type = CAI_PLCM_TYPE_ESN;

    if ( ( cdma.long_code.public_lcm_type != new_plcm_type )

         || cdma.long_code.private_lcm_active

       )
    {
      mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_LCM_ACTION);
      mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_LCM_ACTION);
    }
  }
  #endif /* FEATURE_IS2000_REL_C */

} /* mcctxns_init_long_code_trans */

/*===========================================================================

FUNCTION mcctxns_long_code_pre

DESCRIPTION
  Preprocessor for the Long Code Mask transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_long_code_pre
(
  mctrans_trans_info_type * curr_trans
)
{
  const mctrans_pending_action_msg_type* pending_msg =
    mctrans_get_pending_msg( curr_trans->trans_num );

  mcctxns_init_long_code_trans(curr_trans);

  if (pending_msg && (pending_msg->gen.msg_type == CAI_TC_FWD_ORD_MSG)
      && (pending_msg->order.ordq.order == CAI_PLC_ORD))
  {

    /* ---------------------------------------------------------
    ** A Long Code Transition Request Order can switch between
    ** public and private long code mask.  When switching to
    ** public, the mobile uses cdma.long_code.public_lcm_type.
    ** --------------------------------------------------------- */
    if ((pending_msg->order.ordq.ordq == CAI_PLC_REQ_PRI)
        && !cdma.long_code.private_lcm_active)
    {
      cdma.long_code.private_lcm_active = TRUE;
    }
    else if ((pending_msg->order.ordq.ordq == CAI_PLC_REQ_PUB)
             && cdma.long_code.private_lcm_active)
    {
      cdma.long_code.private_lcm_active = FALSE;
    }

  }
#ifdef FEATURE_IS2000_REL_C
  else if (P_REV_IS_9_OR_GREATER && pending_msg &&
           (pending_msg->gen.msg_type == CAI_SRV_CON_MSG) &&
           pending_msg->service_connect.use_type0_plcm)
  {

      /* ---------------------------------------------------------
      ** If the use_type0_plcm field in the Service Connect Message
      ** is 1, the mobile uses the ESN-based public long code mask.
      ** --------------------------------------------------------- */
      cdma.long_code.public_lcm_type = CAI_PLCM_TYPE_ESN;


    if (cdma.long_code.private_lcm_active)
    {
      cdma.long_code.private_lcm_active = FALSE;
    }

  }
#endif /* FEATURE_IS2000_REL_C */

} /* mcctxns_long_code_pre */

/*===========================================================================

FUNCTION mcctxns_generic_post

DESCRIPTION
  A generic Post-processor for most of the transactions.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_generic_post
(
 mctrans_trans_info_type * curr_trans
)
{
  mctrans_report_trans_done( curr_trans );

}

/*===========================================================================

FUNCTION mcctxns_long_code_action_packet

DESCRIPTION
  Generates the action packet for a long code action (part of the Long Code
  Mask Transaction).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void mcctxns_long_code_action_packet
(
 mctrans_trans_info_type *   curr_trans,
 mctrans_action_type         action,
 mctrans_server_token_type   server_token,
 mctrans_action_packet_type* packet
)
{
  switch ( server_token )
  {
    case MCTRANS_SERVER_TXC:

      switch (action)
      {
        case MCTRANS_LCM_ACTION:
          mcctcsup_get_long_code_mask(packet->action.txc_lcm.long_code_mask);
          break;

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action );
          break;
      }
      break;

    case MCTRANS_SERVER_SRCH :

      switch (action)
      {
        case MCTRANS_LCM_ACTION:
          mcctcsup_get_long_code_mask(packet->action.srch_lcm.long_code_mask);
          break;

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action );
          break;
      }
      break;

      default:
        M1X_MSG( DCP, LEGACY_ERROR,
          "Invalid Server Token %d", server_token );
        break;
  }
  MODEM_1X_UNUSED(curr_trans);
} /* mcctxns_long_code_action_packet */

/*===========================================================================

FUNCTION mcctxns_begin_scr_trans

DESCRIPTION
  Starts a SCR transaction.

DEPENDENCIES
  Pending service configuration must be valid.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_scr_trans
(
  qword trans_time
)
{
  uint16 trans_num;
  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_COUNT,                     // Transaction type
    MCTRANS_MODE_EXPLICIT,
    NULL,
    NULL,
    NULL
  };

  despatch_info.transaction_type = MCTRANS_SCR;

  despatch_info.preprocessor = mcctxns_scr_pre;
  despatch_info.postprocessor = mcctxns_generic_post;
  despatch_info.action_packet_gen = mcctxns_scr_action_packet;

  trans_num = mctrans_despatch_transaction( trans_time, &despatch_info, 0 );

  M1X_MSG( DCP, LEGACY_MED,
    "SCR transaction despatched, trans_num %d",
    trans_num );

  (void) trans_num; /* Added to satisfy compiler warning (set but not used) */
}

/*===========================================================================

FUNCTION mcctxns_init_scr_trans

DESCRIPTION
  Fills the SCR transaction with all the actions.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_init_scr_trans
(
  mctrans_trans_info_type * curr_trans
)
{
  const caii_non_neg_srv_cfg_type* nnscr;
  const caii_srv_cfg_type  *config;
  const caii_complete_srv_cfg_type* pending_config = snm_get_complete_pending_config();

  mcc_fwd_chan_mask_type cur_fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask();
  mcc_rev_chan_mask_type cur_rev_chan_mask = mccccl_code_channel_list.get_rev_chan_mask();

  nnscr = snm_get_current_nnscr();
  config = snm_get_current_config();

  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_SCR_ACTION);
  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_SCR_ACTION);

  /*  MCTRANS_FFPC_ACTION
  */

  if ( pending_config->non_neg_scr.fpc_incl ||
       ((cur_fwd_chan_mask & MCC_F_FCH) && pending_config->neg_scr.fch_cc_incl
        && pending_config->neg_scr.for_fch_rc != config->for_fch_rc) ||
       ((cur_fwd_chan_mask & MCC_F_DCCH) && pending_config->neg_scr.dcch_cc_incl
        && pending_config->neg_scr.for_dcch_rc != config->for_dcch_rc)
     )
  {
    /* If FPC info is included in the SCR, or if RC changes, we need to add the
       FFPC action to the default transaction despatch data */
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_FFPC_ACTION);
  }

#if defined (FEATURE_IS2000_REL_A)
  if (pending_config->non_neg_scr.fpc_incl &&
      (pending_config->non_neg_scr.fpc_pri_chan != nnscr->fpc_pri_chan))
  {
    /* Just fill server token alone, FFPC action is already set up */
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_FFPC_ACTION);
  }
#endif /* FEATURE_IS2000_REL_A*/

  /*  MCTRANS_RLGC_ACTION
  */

  if (((cur_rev_chan_mask & MCC_R_FCH) && pending_config->neg_scr.fch_cc_incl &&
       (pending_config->neg_scr.rev_fch_rc != config->rev_fch_rc)) ||
      ((cur_rev_chan_mask & MCC_R_DCCH) && pending_config->neg_scr.dcch_cc_incl &&
       (pending_config->neg_scr.rev_dcch_rc != config->rev_dcch_rc)))

  {
    /* If RC changes, we need to add the RLGC action to the default transaction despatch data */
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_RLGC_ACTION);
  }

  /*  MCTRANS_GATING_ACTION
  */

  if (cdma.rev_fch_gating_on)
  {
    /* Need to add gating action to make sure gating mode is consistent with
       RC we are changing to */
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_GATING_ACTION);
  }

} /* mctrans_init_scr_trans */

/*===========================================================================

FUNCTION mcctxns_begin_ho_trans

DESCRIPTION
  Starts a HO transaction.

DEPENDENCIES
  Pending service configuration must be valid.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_ho_trans
(
  qword trans_time
)
{
  uint16 trans_num;
  mctrans_trans_mode_type  transaction_mode;
  mctrans_despatch_info_type despatch_info;
  mcctcho_ho_data_type *ho_data;
  mcc_rev_chan_mask_type cur_rev_chan_mask, ho_rev_chan_mask;
  cai_ext_chind_type ota_ext_chind = CAI_EXT_CHIND_UNSET;

  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry == MCC_CSFB)
  {
    /* If it is CSFB, we dont need to wait for the action time to start the
     * transaction, we can start the transactions immediately.
     */
    m1x_time_get_20ms_frame_offset_time(trans_time);
    transaction_mode = MCTRANS_MODE_IMMEDIATE;
  }
  else
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    transaction_mode = MCTRANS_MODE_EXPLICIT;
  }

  despatch_info.transaction_type = MCTRANS_COUNT;
  despatch_info.trans_mode = transaction_mode;
  despatch_info.preprocessor = NULL;
  despatch_info.postprocessor = NULL;
  despatch_info.action_packet_gen = NULL;
  // First get pointer to ho data
  ho_data = mcctcho_get_ho_data();

#ifdef FEATURE_IS2000_REL_C
  if (ho_data->ho_msg.ch_ind == (byte)CAI_CHIND_RESERVED_1)
  {
    ota_ext_chind = (cai_ext_chind_type) ho_data->ho_msg.ext_ch_ind;
  }
#endif /* FEATURE_IS2000_REL_C */

  /* Retrieve the current internal fwd/rev channel indicator masks */
  cur_rev_chan_mask = mccccl_code_channel_list.get_rev_chan_mask();

  /* Split OTA chind into fwd/rev channel masks (what channels to use) */
  /* Note: ch_ind in the msg is already properly initialized regardless of the type of HDM. */
  ho_rev_chan_mask = mccccl_code_channel_list.gen_rev_chan_mask((cai_chind_type) ho_data->ho_msg.ch_ind, ota_ext_chind);



  despatch_info.transaction_type = MCTRANS_HO;

  despatch_info.preprocessor = mcctxns_ho_pre;
  despatch_info.postprocessor = mcctxns_generic_post;
  despatch_info.action_packet_gen = mcctxns_ho_action_packet;

  if ( ho_data->pending_scr_nnscr_changed)
  {
    /* For whatever reason (SCR/NNSCR cmae with HDM or CHIND got changed so
       we have to regenerate LPM etc , the pending CFG has been changed by
       this HDM, so no point keeping any other pending SCR active */
      mctrans_void_trans( MCTRANS_SCR );
  }

  if (ho_data->hard_handoff_in_progress)
  {
    /* The transmitter will be turned off at the frame boundary for hard handoffs. */
    cdma.tc_tx_to_be_on = FALSE;
  }

  // if the most sig bit of channel mask changes from the old one, and we are not handing off to
  // FCH only, then MS is transitioning into or out of control hold
  if ((cur_rev_chan_mask & MCC_R_PICH) != (ho_rev_chan_mask & MCC_R_PICH) &&
      ho_rev_chan_mask != (MCC_R_FCH | MCC_R_PICH) )
  {
    if ((ho_rev_chan_mask & MCC_R_PICH)==0)
    {
      /* if the pilot is off, we're going into control hold, so stop SCH */
      mccsch_stop_sch0_trans(trans_time);
    }
  }

  trans_num = mctrans_despatch_transaction( trans_time, &despatch_info, 0 );
  M1X_MSG( DCP, LEGACY_MED,
    "HO transaction despatched, trans_num %d ",
    trans_num );

  (void) trans_num; /* Added to satisfy compiler warning (set but not used) */
}

/*===========================================================================

FUNCTION mcctxns_init_ho_trans

DESCRIPTION
  Fills the HO transaction with all the actions

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_init_ho_trans(
  mctrans_trans_info_type * curr_trans
)
{
  // First get pointer to ho data
  mcctcho_ho_data_type *ho_data = mcctcho_get_ho_data();
  boolean include_ffpc = FALSE; // Flag to indicate if ffpc action is to be included in the transaction

  const caii_non_neg_srv_cfg_type* nnscr;
  const caii_srv_cfg_type  *config;

#ifdef FEATURE_IS2000_REL_A
  boolean include_srch_ffpc = FALSE;
#endif

  mcc_fwd_chan_mask_type cur_fwd_chan_mask, ho_fwd_chan_mask;
  mcc_rev_chan_mask_type cur_rev_chan_mask, ho_rev_chan_mask;

  boolean chind_changed;

  cai_ext_chind_type ota_ext_chind = CAI_EXT_CHIND_UNSET;

#ifdef FEATURE_IS2000_REL_C
  if (ho_data->ho_msg.ch_ind == CAI_CHIND_RESERVED_1)  /*lint !e641 */
  {
    ota_ext_chind = (cai_ext_chind_type) ho_data->ho_msg.ext_ch_ind;
  }
#endif /* FEATURE_IS2000_REL_C */

  /* Retrieve the current internal fwd/rev channel indicator masks */
  cur_fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask();
  cur_rev_chan_mask = mccccl_code_channel_list.get_rev_chan_mask();

  /* Split OTA chind into fwd/rev channel masks (what channels to use) */
  /* Note: ch_ind in the msg is already properly initialized regardless of the type of HDM. */
  ho_fwd_chan_mask = mccccl_code_channel_list.gen_fwd_chan_mask((cai_chind_type) ho_data->ho_msg.ch_ind, ota_ext_chind);
  ho_rev_chan_mask = mccccl_code_channel_list.gen_rev_chan_mask((cai_chind_type) ho_data->ho_msg.ch_ind, ota_ext_chind);

  /* For now fwd only reflects FCH & DCCH, but keep the 0x3 mask on reverse
  ** the test != 0 converts numeric to boolean. */
  chind_changed = ((word) ((cur_fwd_chan_mask ^ ho_fwd_chan_mask) || ((cur_rev_chan_mask ^ ho_rev_chan_mask) & 0x3)) != 0); /*lint !e641 */


  nnscr = snm_get_current_nnscr();
  config = snm_get_current_config();


  /* MCTRANS_HO_ACTION
  */

  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_HO_ACTION);

  /* Add ASET action to SRCH only if this is a soft HO, For HHO this
     action will be part of HHO Begin transaction */
  if (!ho_data->hard_handoff_in_progress)
  {
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_ASET_ACTION);
    #ifdef FEATURE_IS2000_1X_ADV
    if(!mcc_is_ho_internal_to_non_1xa_rc)
    {
      /* Soft handoff will update active set. So add RCP action to SRCH */
      mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_RCP_ACTION);
    }
    #endif /* FEATURE_IS2000_1X_ADV */
  }

  if (
     // If extra parms included, HO action needed for RXC and TXC
     (ho_data->ho_msg.extra_parms) ||

     /* This is a HHO and hence inform RXC TXC any way */
     (ho_data->hard_handoff_in_progress) ||


     /* If all other conditions are not satisfied,  we stillneed to find out if
        parms_incl is TRUE for UHDM as this may result in a p_rev change which RXC and
        TXC have to know about */
     (((ho_data->ho_msg.msg_type == CAI_UNIV_HO_DIR_MSG )

       || (ho_data->ho_msg.msg_type == CAI_MEID_UNIV_HO_DIR_MSG)

      ) && ho_data->ho_msg.parms_incl))
  {
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_HO_ACTION);
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_HO_ACTION);
  }
  else
  {
    /* If all other conditions are not satisfied,  we stillneed to find out if
    RXC has to be informed because of the rev pwr cntl delay parameter change */
    if (ho_data->ho_msg.rev_pwr_cntl_delay_incl)
    {
      mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_HO_ACTION);
    }
  }

  /* The transmitter will be turned off at the frame boundary for hard handoffs. */
  if (ho_data->hard_handoff_in_progress)
  {
    /* Note that we do not setup a TXC on/off action for hard handoffs; TXC knows
       to turn off the transmitter when it gets a handoff action. */
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_TX_ON_OFF_ACTION);
  }



  /*  MCTRANS_SCR_ACTION
  */
  if ((ho_data->ho_msg.extra_parms)

      &&

      // If service configuration is included, include SCR action for both RXC and TXC
      ( ho_data->ho_msg.extra.scr_included
        // If lower 2 bits of chind is changed, we need SCR action since FCH/DCCH
        // may be assigned/deassigned.
        || chind_changed
#ifdef FEATURE_IS2000_REL_A
        /* This is to handle the possibility that the BS may send the nnscr and no scr.
        In this case, the BS may change the LPM, we need to send SCR with updated (possibly)
        mapping. We trigger this only if the new nnscr specifies a LPM_IND value
        that changes the current LPM */
        || ( ho_data->ho_msg.extra.nnscr_included
             &&
             ( (ho_data->ho_msg.extra.non_neg_cfg.lpm_ind == CAI_LPM_SUPPLIED) || /*lint !e641 */
               ( (ho_data->ho_msg.extra.non_neg_cfg.lpm_ind == CAI_LPM_DEFAULT) && /*lint !e641 */
                 (snm_get_current_nnscr()->lpm_ind != CAI_LPM_DEFAULT) /*lint !e641 */
               )
             )
           )
#endif

      ))
  {
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_SCR_ACTION);
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_SCR_ACTION);
  }


  /* MCTRANS_GATING_ACTION
  */

  /* If SCR changes or gating mode changes we have to change the gating mode */
  /* Not all SCR changes will result in gating mode changes but this is simpler */
  /* rev_fch_gating is not allowed when any other reverse channels, except R-PICH */
  /* are in use. When F-PDCH is in use, continuous R-QQICH is in use, so no */
  /* no FCH gating is allowed. */
  if (
       (

         (ho_data->ho_msg.extra_parms)

         &&

         (
          (
            (!(ho_rev_chan_mask & MCC_R_DCCH)) &&
            (!(ho_fwd_chan_mask & MCC_F_PDCH)) &&
            (ho_data->ho_msg.extra.scr_included ||
              (ho_data->ho_msg.rev_fch_gating_mode != cdma.rev_fch_gating_on)
            )
          )
          ||
          (chind_changed)
          /* If DCCH was included before and not anymore, may be it is time to turn on
             REV FCH gating */
         )
       )

       ||

       (
         /* If gating mode is different initiate a gating  action */
         (!ho_data->ho_msg.extra_parms) &&
         (ho_data->ho_msg.rev_fch_gating_mode != cdma.rev_fch_gating_on)
       )
     )
  {
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_GATING_ACTION);
  }



  /* MCTRANS_FFPC_ACTION
  */

  if ( ho_data->ho_msg.extra_parms )
  {
    // See if we need to include the FFPC action due to FPC info in nnscr.
    if ( ho_data->ho_msg.extra.nnscr_included &&
         ho_data->ho_msg.extra.non_neg_cfg.fpc_incl )
    {
      include_ffpc = TRUE;
#if defined (FEATURE_IS2000_REL_A)
      if (ho_data->ho_msg.extra.non_neg_cfg.fpc_pri_chan != nnscr->fpc_pri_chan)
      {
        include_srch_ffpc = TRUE;
      }
#endif /* FEATURE_IS2000_REL_A */
    }

    // If RC or CH_IND changes, we need to add the FFPC action
    if (((ho_data->ho_msg.extra.scr_included) && (cur_fwd_chan_mask & MCC_F_FCH) &&
         (ho_data->ho_msg.extra.cfg.fch_cc_incl) &&
         (ho_data->ho_msg.extra.cfg.for_fch_rc != config->for_fch_rc)) ||
        ((cur_fwd_chan_mask & MCC_F_DCCH) && (ho_data->ho_msg.extra.cfg.dcch_cc_incl)
          && (ho_data->ho_msg.extra.cfg.for_dcch_rc != config->for_dcch_rc)) ||
        (chind_changed)
       )
    {
      include_ffpc = TRUE;
    }
  }
  // Now see if FFPC action needs to be included due to fpc_subchan_gain
  // If subchan_gain is included and if it doesn't have action time or the
  // action time is same as the message, then include FFPC action.
  ho_data->process_subchan_gain_at_ho = FALSE;
  if ( ho_data->fpc_subchan_gain_incl )
  {
    if ( !ho_data->ho_msg.use_pc_time ||
         ho_data->ho_msg.action_time == ho_data->ho_msg.pc_action_time )
    {
      // Mark flag in mcctcho_data so that when action time come, we know that subchan_gain
      // is to be processed.
      ho_data->process_subchan_gain_at_ho = TRUE;
      include_ffpc = TRUE;
    }
    else  // The subchan_gain update is delayed
    {
      // Store it action time in mcctcho_ho_data such that when the HO action is completed, the
      // subchan_gain transaction is started.  The advantage of doing it this way is that if for
      // some reason the HO transaction failed (overriden by another HO - note this is not mandated
      // by the standard, but it makes sense to do so - or failed HHO), this transaction will not be
      // started.
      get_abs_action_time( ho_data->subchan_gain_action_time, ho_data->ho_msg.pc_action_time );
    }
  }
  if (P_REV_IS_6_OR_GREATER)
  {

    /* Since we are P_REV 6 or higher, nnscr is valid
    fpc_pri_chan can be 0 or 1, 1 is left shifted by fpc_pri_chan to
    see if the corresponding channel is assigned in the channel mask */
    if ( !((1 << nnscr->fpc_pri_chan) & ho_fwd_chan_mask) )
    {
      // CH_IND has taken off the original FPC_PRI_CHAN, we ned to
      // move it to the other channel when the handoff actually takes place
      ho_data->new_fpc_pri_chan = include_ffpc = TRUE;
#ifdef FEATURE_IS2000_REL_A
      include_srch_ffpc = TRUE;
#endif
    }

  }

#if defined (FEATURE_IS2000_REL_A)
  if (include_srch_ffpc)
  {
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_FFPC_ACTION);
  }
#endif /* FEATURE_IS2000_REL_A */

  // Here include FFPC action for RXC if needed
  if ( include_ffpc )
  {
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_FFPC_ACTION);
  }


  /* MCTRANS_RLGC_ACTION
  */

  // For P_REV 6 or greater, if extra_parms included or if power control step included,
  // we need RLGC action for TXC
  if (((P_REV_IS_6_OR_GREATER) &&
       ((ho_data->ho_msg.extra_parms) || (ho_data->ho_msg.use_pwr_cntl_step))) ||
       // If CH_IND is different, certainly RLGC needs to know
      (chind_changed)
     )
  {
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_RLGC_ACTION);
  }


  /*  MCTRANS_CTRL_HLD_GATING_ACTION
  */

  // if the most sig bit of channel mask changes from the old one, and we are not handing off to
  // FCH only, then MS is transitioning into or out of control hold
  if ((cur_rev_chan_mask & MCC_R_PICH) != (ho_rev_chan_mask & MCC_R_PICH) &&
       ho_rev_chan_mask != (MCC_R_FCH | MCC_R_PICH) )
  {
    // Add gating action for TXC and RXC
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_CTRL_HLD_GATING_ACTION);
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_CTRL_HLD_GATING_ACTION);
  }

  if (mcctcho_different_long_code_mask(&ho_data->ho_msg))
  {
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_LCM_ACTION);
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_LCM_ACTION);
  }

#ifdef FEATURE_IS2000_1X_ADV
  if(!mcc_is_ho_internal_to_non_1xa_rc)
  {
    /* RC parameters should be sent to RXC and TXC.
     * Setup the rcp action for rxc and txc. */
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_RCP_ACTION);
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_RCP_ACTION);
  }
#endif /* FEATURE_IS2000_1X_ADV */

} /* mcctxns_init_ho_trans */

/*===========================================================================

FUNCTION mcctxns_begin_subchan_gain_trans

DESCRIPTION
  Starts a fpc subchan transaction.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_subchan_gain_trans
(
  qword trans_time
)
{
  uint16 trans_num;
  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_COUNT,                     // Transaction type
    MCTRANS_MODE_EXPLICIT,
    NULL,
    NULL,
    NULL
  };

  despatch_info.transaction_type = MCTRANS_FPC_SUBCHAN_GAIN;

  despatch_info.preprocessor = mcctxns_subchan_gain_pre;
  despatch_info.postprocessor = mcctxns_subchan_gain_post;
  despatch_info.action_packet_gen = mcctxns_subchan_gain_action_packet;

  trans_num = mctrans_despatch_transaction( trans_time, &despatch_info, 0 );

  M1X_MSG( DCP, LEGACY_MED,
    "FPC_SUBCHAN transaction despatched, trans_num %d",
    trans_num );

  (void) trans_num; /* Added to satisfy compiler warning (set but not used) */
}

/*===========================================================================

FUNCTION mcctxns_init_subchan_gain_trans

DESCRIPTION
  Fills the FPC_CHAN_GAIN transaction with all the actions.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_init_subchan_gain_trans
(
  mctrans_trans_info_type * curr_trans
)
{
  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_FFPC_ACTION);

} /* mcctxns_init_subchan_gain_trans */



/*===========================================================================

FUNCTION mcctxns_begin_pwr_ctrl_trans

DESCRIPTION
  Starts a pwr ctrl transaction.

DEPENDENCIES
None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_pwr_ctrl_trans
(
  qword trans_time,
  mctrans_pending_action_msg_type* msg
)
{
  uint16 trans_num;
  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_COUNT,                     // Transaction type
    MCTRANS_MODE_EXPLICIT,
    NULL,
    NULL,
    NULL
  };

  despatch_info.transaction_type = MCTRANS_PWR_CTRL_MSG;

  despatch_info.preprocessor = mcctxns_pwr_ctrl_pre;
  despatch_info.postprocessor = mcctxns_generic_post;
  despatch_info.action_packet_gen = mcctxns_pwr_ctrl_action_packet;

  trans_num = mctrans_despatch_transaction( trans_time, &despatch_info, msg );

  M1X_MSG( DCP, LEGACY_MED,
    "PWR_CTRL transaction despatched, trans_num %d",
    trans_num );

  (void) trans_num; /* Added to satisfy compiler warning (set but not used) */
}

/*===========================================================================

FUNCTION mcctxns_init_pwr_ctrl_trans

DESCRIPTION
  Fills the PWR_CTRL_MSG transaction with all the actions.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_init_pwr_ctrl_trans
(
  mctrans_trans_info_type * curr_trans
)
{
  const caii_non_neg_srv_cfg_type *nnscr = snm_get_current_nnscr();
  mctrans_pending_action_msg_type* msg =
                        mctrans_get_pending_msg(curr_trans->trans_num);

  if (msg != NULL)
  {
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_RLGC_ACTION);

    if (msg->pwr_ctrl.fpc_incl)
    {
      /* If FPC info is included in the PCNM, we need to add the action to the default
         transaction despatch data. */
      mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_FFPC_ACTION);

      if (nnscr->fpc_pri_chan != msg->pwr_ctrl.fpc_pri_chan)
      {
        mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_FFPC_ACTION);
      }
    }

  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "No actions added for %d due to no mesg found",
      curr_trans->trans_num);
  }
} /* mcctxns_init_pwr_ctrl_trans */

/*===========================================================================

FUNCTION mcctxns_begin_cfs_trans

DESCRIPTION
  Starts a CFS transaction.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_cfs_trans
(
  qword trans_time,
  mctrans_pending_action_msg_type* msg
)
{
  uint16 trans_num;

  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_COUNT,                     // Transaction type
    MCTRANS_MODE_EXPLICIT,
    NULL,
    NULL,
    NULL
  };

  if (msg->gen.msg_type == CAI_CFS_CTRL_MSG)
  {
    despatch_info.transaction_type = MCTRANS_CFS_CTRL;
    despatch_info.action_packet_gen = NULL;
  }
  else {
    despatch_info.transaction_type = MCTRANS_CFS_NSET;
    despatch_info.action_packet_gen = mcctxns_cfs_nset_action_packet;
  }

  despatch_info.preprocessor = mcctxns_cfs_pre;;
  despatch_info.postprocessor = mcctxns_generic_post;

  // Now go ahead and despatch transaction
  trans_num = mctrans_despatch_transaction( trans_time, &despatch_info, msg );

  M1X_MSG( DCP, LEGACY_MED,
    "CFS transaction despatched, trans_num %d, trans_type %d",
    trans_num,
    despatch_info.transaction_type );

  (void) trans_num; /* Added to satisfy compiler warning (set but not used) */
}
/*===========================================================================

FUNCTION mcctxns_init_cfs_trans

DESCRIPTION
  Fills the transaction with all the actions

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_init_cfs_trans
(
  mctrans_trans_info_type * curr_trans
)
{
  mctrans_pending_action_msg_type* msg =
                        mctrans_get_pending_msg(curr_trans->trans_num);
  if (msg != NULL)
  {
    if (msg->gen.msg_type == CAI_CFS_CTRL_MSG)
    {
      mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_NULL_ACTION);
    }
    else
    {
      mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH,
                                               MCTRANS_CFS_NSET_ACTION);
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "No actions added for %d due to no mesg found",
      curr_trans->trans_num);
  }

} /*  mcctxns_begin_cfs_trans */

/*===========================================================================

FUNCTION mcctxns_prepare_scr_ffpc_action_packet

DESCRIPTION
  Prepares the action packet for FFPC action from an SCR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_prepare_scr_ffpc_action_packet
(
  const caii_non_neg_srv_cfg_type* nnscr,
  mctrans_ffpc_action_type*  packet
)
{
  mcc_fwd_chan_mask_type fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask();

  /* First initialize packet to update nothing. */
  mcctxns_init_ffpc( packet );

  if (!nnscr->fpc_incl)
  {
    /* If fpc not included in NNSCR, we are done */
    return;
  }

  /* If we are here, FPC info in nnscr is valid */

  /* Update DCCH FPC parameters, if included */
  if ((fwd_chan_mask & MCC_F_DCCH) && nnscr->fpc_olpc_dcch_incl)
  {
    packet->dcch_olpc.included = TRUE;
    packet->dcch_olpc.param.max_setpt.value = nnscr->fpc_dcch_max_setpt;
    packet->dcch_olpc.param.min_setpt.value = nnscr->fpc_dcch_min_setpt;
    packet->dcch_olpc.param.target_fer = nnscr->fpc_dcch_fer;
  }

  /* Update FCH FPC parameters, if included */
  if ((fwd_chan_mask & MCC_F_FCH) && nnscr->fpc_olpc_fch_incl)
  {
    packet->fch_olpc.included = TRUE;
    packet->fch_olpc.param.max_setpt.value = nnscr->fpc_fch_max_setpt;
    packet->fch_olpc.param.min_setpt.value = nnscr->fpc_fch_min_setpt;
    packet->fch_olpc.param.target_fer = nnscr->fpc_fch_fer;
  }

  /* FPC_PRI_CHAN is always set properly in NNSCR either through OTA msg or
     default initialization */
  packet->fpc_pri_chan.included = TRUE;
  packet->fpc_pri_chan.value = nnscr->fpc_pri_chan;

  /* But they are included in the action packet only if there is no
     active SCH (we don't want to override any value as given in ESCAM */
  if ( !mccsch_sch_is_active( MCCSCH_FORWARD, 0 ) )
  {
    packet->fpc_mode.included = TRUE;
    /* FPC_MODE is always set properly in NNSCR either through OTA msg or
       default initialization */
    packet->fpc_mode.value = (cai_fpc_mode_type) nnscr->fpc_mode;
  }

} // mcctxns_prepare_scr_ffpc_action_packet

/*===========================================================================

FUNCTION mcctxns_prepare_scr_action_packet

DESCRIPTION
  Prepares the action packet for SCR action from an SCR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void mcctxns_prepare_scr_action_packet
(
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet,
  const caii_srv_cfg_type  *config,
  const caii_non_neg_srv_cfg_type *nn_config
)
{
#ifdef FEATURE_IS2000_REL_A
  uint8 i;
#endif /* FEATURE_IS2000_REL_A */

  mcc_fwd_chan_mask_type fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask();
  mcc_rev_chan_mask_type rev_chan_mask = mccccl_code_channel_list.get_rev_chan_mask();

  switch ( server_token )
  {
    case MCTRANS_SERVER_RXC :
    {

#ifndef FEATURE_IS2000_REL_A
      /* Don't support secondary SO yet */
      packet->action.rxc_scr.sec_so.included = FALSE;
#endif /* FEATURE_IS2000_REL_A */

      if (fwd_chan_mask & MCC_F_FCH)
      {
        packet->action.rxc_scr.fch.included = TRUE;
        packet->action.rxc_scr.fch.rc = (cai_radio_config_type)config->for_fch_rc;
        M1X_MSG( DCP, LEGACY_HIGH,
          "Fwd FCH RC %d ",
          packet->action.rxc_scr.fch.rc);
      }
      else
      {
        packet->action.rxc_scr.fch.included = FALSE;
      }

      if (fwd_chan_mask & MCC_F_DCCH)
      {
        packet->action.rxc_scr.dcch.included = TRUE;
        packet->action.rxc_scr.dcch.rc = (cai_radio_config_type)config->for_dcch_rc;
        M1X_MSG( DCP, LEGACY_HIGH,
          "Fwd DCCH RC %d ",
          packet->action.rxc_scr.dcch.rc);
      }
      else
      {
        packet->action.rxc_scr.dcch.included = FALSE;
      }

      /* If DCCH or FCH is included, we are receiving RC in the SCR.  The
         rev_pwr_cntl_delay can change because of the RC change.  For simplicity
         just include the proper value for rev_pwr_cntl_delay whether it changes
         or not */
      if (packet->action.rxc_scr.fch.included
          || packet->action.rxc_scr.dcch.included
         )
      {
        packet->action.rxc_scr.rev_pwr_cntl_delay.included = TRUE;
        if ((packet->action.rxc_scr.fch.included &&
             ((config->for_fch_rc > CAI_RC_2) || (config->rev_fch_rc > CAI_RC_2)))
             || packet->action.rxc_scr.dcch.included
           )
        {
          packet->action.rxc_scr.rev_pwr_cntl_delay.value =
            (rev_chan_mask & MCC_R_PICH ||
             (nn_config->pilot_gate_rate == (byte)CAI_GATING_RATE_ONE) ||
             (nn_config->pilot_gate_rate > (byte)CAI_GATING_RATE_QUARTER) ) ?
            cdma.rev_pwr_cntl_delay : gating_rate_to_pwr_delay[nn_config->pilot_gate_rate];
        }
        else
        {
          /* Low RC.  Use the default. */
          packet->action.rxc_scr.rev_pwr_cntl_delay.value = CAI_DEFAULT_REV_PWR_CNTL_DELAY;
        }
      }
      else
      {
        packet->action.rxc_scr.rev_pwr_cntl_delay.included = FALSE;
      }

#ifdef FEATURE_IS2000_REL_A
      packet->action.rxc_scr.num_of_so_recs = 0;

      /* Let us go through the connections and fill in the structure accordingly. */
      for (i = 0; (i < config->num_con) && ( i< MAX_NO_OF_SO_RECS); i++)
      {
        if ((config->con[i].fwd_traf == CAI_NUL_TRAF) ||
            (mccccim_con_is_in_rel_substate(config->con[i].con_ref))
           )
        {
          /* No traffic or connection in rel substate, no record to send */
          continue;
        }

        packet->action.rxc_scr.so_cfgs[packet->action.rxc_scr.num_of_so_recs].so =
          config->con[i].so;

        if ((packet->action.rxc_scr.so_cfgs[packet->action.rxc_scr.num_of_so_recs].sr_id =
             config->con[i].sr_id) > MAX_SR_ID_VALUE)
        {
          /* Should not happen, as we initialize sr_id to a legal value for PREV < 6 */
          ERR_FATAL("con ref %d has illegal sr_id %d", config->con[i].con_ref, config->con[i].sr_id,0);
        }

        packet->action.rxc_scr.so_cfgs[packet->action.rxc_scr.num_of_so_recs].traffic_type =
          config->con[i].fwd_traf;

        #ifdef FEATURE_IS2000_REL_A_AES
        if ( P_REV_IS_6_OR_GREATER )
        {
          packet->action.rxc_scr.so_cfgs[packet->action.rxc_scr.num_of_so_recs].ui_encrypt_mode =
            config->con[i].ui_encrypt_mode;
        }
        #endif /* FEATURE_IS2000_REL_A_AES */

        packet->action.rxc_scr.num_of_so_recs++;

      } /* end for */
#else
      /* We know that we only support 1 service option, so just use that assumption
         and take the first connection from service config. */
      packet->action.rxc_scr.pri_so.included = TRUE;
      packet->action.rxc_scr.pri_so.value = config->con[0].so;
#endif //FEATURE_IS2000_REL_A

      if (config->for_sch_cc_incl && (config->num_for_sch != 0))
      {
        packet->action.rxc_scr.for_sch0_mux_option.included = TRUE;
        packet->action.rxc_scr.for_sch0_mux_option.value = config->for_sch[0].for_sch_mux;
      }
      else
      {
        packet->action.rxc_scr.for_sch0_mux_option.included = FALSE;
      }

      break;
    }

    case MCTRANS_SERVER_TXC :
    {

#ifdef FEATURE_IS2000_REL_A
      /* Use this array to gather phy channels for sr_ids in nnscr, this
         allows us to traverse the LPM table just once. The array is indexed on
         sr_id. However for PREV < 6 (sr_id invalid), we'll use the index '0'
         of the array to store the phy channels for the only service in the
         configuration */
      uint8 phy_chan_for_sr_id[MAX_SR_ID_VALUE+1];

      /* Map LPM phy channels to MCTRANS phy channels. The reason we use MCTRANS
         phy channels in the this action packet is that they can be OR-ed to
         create a bit mapping, while the standard-defined values of the LPM
         phy channels cann't */
      uint8 lpm_to_mctrans_phy_chan_map[CAI_LPM_PDCH+1] = /*lint !e641 */

        {MCTRANS_PHY_CHAN_FCH, MCTRANS_PHY_CHAN_DCCH, MCTRANS_PHY_CHAN_SCH0, 0, 0};

      /* Check current channel mask to see if the mapping needs to be adjusted
         to block out channels  - arbitrarily use reverse for now */
      if ((rev_chan_mask & MCC_R_FCH) == 0)
      {
        /* No FCH */
        lpm_to_mctrans_phy_chan_map[CAI_LPM_FCH] = 0;
      }

      if ((rev_chan_mask & MCC_R_DCCH) == 0)
      {
        /* No DCCH */
        lpm_to_mctrans_phy_chan_map[CAI_LPM_DCCH] = 0;
      }

      packet->action.txc_scr.signaling_phy_channel = 0;

      for (i = 0; i <= MAX_SR_ID_VALUE; i++)
      {
        phy_chan_for_sr_id[i] = 0;
      }

      /* Go through the LPM table and gather phy channels for services/signaling */
      for (i=0; i<nn_config->num_lpm_entries; i++)
      {
        /* Diagnostics only */
        M1X_MSG( DCP, LEGACY_MED,
          "LPM: SR_ID %d LOG %d PHY %d",
          nn_config->lpm_entries[i].sr_id,
          nn_config->lpm_entries[i].logical_resource,
          nn_config->lpm_entries[i].physical_resource);
        M1X_MSG( DCP, LEGACY_MED,
          "                             FWD %d REV %d",
          nn_config->lpm_entries[i].forward_flag,
          nn_config->lpm_entries[i].reverse_flag);

        /* check for reverse signaling entry in LPM */
        if (nn_config->lpm_entries[i].reverse_flag &&
            (nn_config->lpm_entries[i].logical_resource == CAI_LPM_DSCH)) /*lint !e641 */
        {
          packet->action.txc_scr.signaling_phy_channel |=
            lpm_to_mctrans_phy_chan_map[nn_config->lpm_entries[i].physical_resource];
        }
        /* reverse traffic entry in the LPM. */
        else if (nn_config->lpm_entries[i].reverse_flag &&
                 (nn_config->lpm_entries[i].sr_id <= MAX_SR_ID_VALUE))
        {
          if (P_REV_IS_6_OR_GREATER)
          {
            phy_chan_for_sr_id[nn_config->lpm_entries[i].sr_id] |=
              lpm_to_mctrans_phy_chan_map[nn_config->lpm_entries[i].physical_resource];
          }
          else
          { /* sr_id does not make sense here, so use index 0 which will be copied below */
            phy_chan_for_sr_id[0] |=
              lpm_to_mctrans_phy_chan_map[nn_config->lpm_entries[i].physical_resource];
          }
        } /* else if (nn_config->lpm_entries[i].reverse_flag && .. */
      } /* for */
#endif /* FEATURE_IS2000_REL_A */

      /* We know that there is only 1 action in this transaction for TXC, so
         don't bother check for action type here. */

#ifndef FEATURE_IS2000_REL_A
      /* Don't support secondary SO yet. */
      packet->action.txc_scr.sec_so.included = FALSE;
#endif /* FEATURE_IS2000_REL_A */

      if (rev_chan_mask & MCC_R_FCH)
      {
        packet->action.txc_scr.fch.included = TRUE;
        packet->action.txc_scr.fch.rc = (cai_radio_config_type)config->rev_fch_rc;
        M1X_MSG( DCP, LEGACY_HIGH,
          "Rev FCH RC %d ",
          packet->action.txc_scr.fch.rc);
      }
      else
      {
        packet->action.txc_scr.fch.included = FALSE;
      }

      if (rev_chan_mask & MCC_R_DCCH)
      {
        packet->action.txc_scr.dcch.included = TRUE;
        packet->action.txc_scr.dcch.rc = (cai_radio_config_type)config->rev_dcch_rc;
        M1X_MSG( DCP, LEGACY_HIGH,
          "Rev DCCH RC %d ",
          packet->action.txc_scr.dcch.rc);
      }
      else
      {
        packet->action.txc_scr.dcch.included = FALSE;
      }

      if (config->rev_sch_cc_incl && (config->num_rev_sch != 0))
      {
        packet->action.txc_scr.rev_sch0_mux_option.included = TRUE;
        packet->action.txc_scr.rev_sch0_mux_option.value = config->rev_sch[0].rev_sch_mux;
      }
      else
      {
        packet->action.txc_scr.rev_sch0_mux_option.included = FALSE;
      }

#ifndef FEATURE_IS2000_REL_A
      /* We know that we only support 1 service option, so just use that
         assumption and take the first connection from service config. */
      packet->action.txc_scr.pri_so.included = TRUE;
      packet->action.txc_scr.pri_so.value = config->con[0].so;
      packet->action.txc_scr.pri_so.connect_so = mccccim_can_so_be_connected(
        config->con[0].con_ref,0,config->con[0].so);
#else
      packet->action.txc_scr.num_of_so_recs = 0;

      /* Let us go through the connections and fill in the structure accordingly. */
      for (i = 0; (i < config->num_con) && (i < MAX_NO_OF_SO_RECS); i++)
      {
        snm_chan_mask_type so_chan_mask = snm_gen_so_chan_mask(config->con[i].so);

        if ((config->con[i].rev_traf == CAI_NUL_TRAF) ||
            (mccccim_con_is_in_rel_substate(config->con[i].con_ref))
           )
        {
          /* No traffic or connection in rel substate, no record to send */
          continue;
        }

        packet->action.txc_scr.so_cfgs[packet->action.txc_scr.num_of_so_recs].so =
          config->con[i].so;
        if ((packet->action.txc_scr.so_cfgs[packet->action.txc_scr.num_of_so_recs].sr_id =
             config->con[i].sr_id) > MAX_SR_ID_VALUE)
        {
          /* Should not happen, as we initialize sr_id to a legal value for PREV < 6 */
          ERR_FATAL("con ref %d has illegal sr_id %d", config->con[i].con_ref, config->con[i].sr_id,0);
        }

        packet->action.txc_scr.so_cfgs[packet->action.txc_scr.num_of_so_recs].traffic_type =
          config->con[i].rev_traf;
        packet->action.txc_scr.so_cfgs[packet->action.txc_scr.num_of_so_recs].connect_so =
          mccccim_can_so_be_connected(config->con[i].con_ref,i,config->con[i].so);

        if (P_REV_IS_6_OR_GREATER)
        {
          packet->action.txc_scr.so_cfgs[packet->action.txc_scr.num_of_so_recs].phy_channel =
            phy_chan_for_sr_id[config->con[i].sr_id];

          #ifdef FEATURE_IS2000_REL_A_AES
          packet->action.txc_scr.so_cfgs[packet->action.txc_scr.num_of_so_recs].ui_encrypt_mode =
            config->con[i].ui_encrypt_mode;
          #endif /* FEATURE_IS2000_REL_A_AES */

        }
        else
        {
          packet->action.txc_scr.so_cfgs[packet->action.txc_scr.num_of_so_recs].phy_channel =
            phy_chan_for_sr_id[0];
        }

        /* The following section does the following:
           Before we give mux phy channel for this sr_id, we need to filter out
           phy channels that are not applicable to the SO associated with this
           sr_id. For instance, even though the default LPM may configure voice
           to go on both FCH and DCCH, we will consult snm and figure out which
           channels don't apply (in this case, DCCH). */

        /* FCH */
        if (packet->action.txc_scr.so_cfgs[packet->action.txc_scr.num_of_so_recs].phy_channel
            & MCTRANS_PHY_CHAN_FCH)
        {
          if ((so_chan_mask & SNM_FCH) == 0) /*lint !e641 */
          {
            /* Should exclude from what is being given to mux */
            M1X_MSG( DCP, LEGACY_HIGH,
              "Excluding FCH for sr_id %d given to mux",
              config->con[i].sr_id);
            packet->action.txc_scr.so_cfgs[packet->action.txc_scr.num_of_so_recs].phy_channel
              &= (~MCTRANS_PHY_CHAN_FCH);
          }
        }

        /* DCCH */
        if (packet->action.txc_scr.so_cfgs[packet->action.txc_scr.num_of_so_recs].phy_channel
            & MCTRANS_PHY_CHAN_DCCH)
        {
          if ((so_chan_mask & SNM_DCCH) == 0) /*lint !e641 */
          {
            /* Should exclude from what is being given to mux */
            M1X_MSG( DCP, LEGACY_HIGH,
              "Excluding DCCH for sr_id %d given to mux",
              config->con[i].sr_id);
            packet->action.txc_scr.so_cfgs[packet->action.txc_scr.num_of_so_recs].phy_channel
              &= (~MCTRANS_PHY_CHAN_DCCH);
          }
        }

        /* SCH0 */
        if (packet->action.txc_scr.so_cfgs[packet->action.txc_scr.num_of_so_recs].phy_channel
            & MCTRANS_PHY_CHAN_SCH0)
        {
          if (!P_REV_IS_6_OR_GREATER || ((so_chan_mask & SNM_R_SCH) == 0)) /*lint !e641 */
          {
            /* Should exclude from what is being given to mux */
            M1X_MSG( DCP, LEGACY_HIGH,
              "Excluding SCH0 for sr_id %d given to mux",
              config->con[i].sr_id);
            packet->action.txc_scr.so_cfgs[packet->action.txc_scr.num_of_so_recs].phy_channel
              &= (~MCTRANS_PHY_CHAN_SCH0);
          }
        }

        /* Finally, ensure that there is at least one fundicated channel or PDCH
           available for this sr_id */
        /* An exception to this rule is if the SO is TDSO/Full TDSO in which case it is
           okay to not have a fundicated channel or PDCH */
        if ((packet->action.txc_scr.so_cfgs[packet->action.txc_scr.num_of_so_recs].so
             != CAI_SO_FULL_TDSO) &&
            (packet->action.txc_scr.so_cfgs[packet->action.txc_scr.num_of_so_recs].so
             != CAI_SO_TDSO) &&
            ((packet->action.txc_scr.so_cfgs[packet->action.txc_scr.num_of_so_recs].phy_channel
              & (MCTRANS_PHY_CHAN_FCH | MCTRANS_PHY_CHAN_DCCH)) == 0)
           )
        {
          ERR_FATAL("No fundicated phy channel associated with sr_id %d", config->con[i].sr_id,0,0);
        }

        packet->action.txc_scr.num_of_so_recs++;

      } /* end for */

      if (
          ((packet->action.txc_scr.signaling_phy_channel
            & (MCTRANS_PHY_CHAN_FCH | MCTRANS_PHY_CHAN_DCCH)) == 0)
         )
      {
        /* Probably because a channel was dropped and the LPM was not re-sent */
        ERR_FATAL("No phy channel available for signaling",0,0,0);
      }

#endif /* FEATURE_IS2000_REL_A */

      break;

    }

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid Server Token %d", server_token );
      break;

  }

} /* mcctxns_prepare_scr_action_packet */

/*===========================================================================

FUNCTION mcctxns_scr_pre

DESCRIPTION
  Preprocessor for SCR transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_scr_pre
(
  mctrans_trans_info_type * curr_trans
)
{
  mcctxns_init_scr_trans(curr_trans);
  snm_update_configuration();

#if defined(FEATURE_IS2000_REL_C)
  /* Update SYNC ID info to be restored */
  mccsyncid_update_restore_info();
#endif /* FEATURE_IS2000_REL_C */

} /* mcctxns_scr_pre */

/*===========================================================================

FUNCTION mcctxns_scr_action_packet

DESCRIPTION
  Prepares the action packets for SCR transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_scr_action_packet
(
  mctrans_trans_info_type *curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
)
{
    mcc_rev_chan_mask_type rev_chan_mask = mccccl_code_channel_list.get_rev_chan_mask();
    /* Obtain the pending configuration. Bear in mind that the service
     configuration update can be incremental. */
  const caii_complete_srv_cfg_type *comp_config = snm_get_complete_pending_config();

  /* Stamp action type in action packet */
  packet->action_type = action;

  switch (server_token)
  {
    case MCTRANS_SERVER_RXC :
    {
      switch (action)
      {
        case MCTRANS_SCR_ACTION :
        {
          mcctxns_prepare_scr_action_packet(MCTRANS_SERVER_RXC, packet, &comp_config->neg_scr,
            &comp_config->non_neg_scr);
          break;
        }

        case MCTRANS_FFPC_ACTION :
        {
          mcctxns_prepare_scr_ffpc_action_packet( &comp_config->non_neg_scr, &packet->action.ffpc );
          break;
        }

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action );
          break;

      }
      break;
    } /* end of case MCTRANS_SERVER_RXC */

    case MCTRANS_SERVER_TXC :
    {
      switch (action)
      {
        case MCTRANS_SCR_ACTION:
        {
          mcctxns_prepare_scr_action_packet(MCTRANS_SERVER_TXC, packet, &comp_config->neg_scr,
            &comp_config->non_neg_scr);
          break;
        }
        case MCTRANS_RLGC_ACTION:
        {
          mcctxns_init_rlgc( &packet->action.rlgc );
          /* RLGC needs to be notified if FCH/DCCH is changing RC */
          if (rev_chan_mask & MCC_R_DCCH)
          {
            packet->action.rlgc.dcch_rlgc_op.mode = MCTRANS_RLGC_START;
            packet->action.rlgc.dcch_rlgc_op.rc = (cai_radio_config_type) comp_config->neg_scr.rev_dcch_rc;
          }

          if (rev_chan_mask & MCC_R_FCH)
          {
            packet->action.rlgc.fch_rlgc_op.mode = MCTRANS_RLGC_START;
            packet->action.rlgc.fch_rlgc_op.rc = (cai_radio_config_type) comp_config->neg_scr.rev_fch_rc;
          }

          break;
        } /* end case MCTRANS_RLGC_ACTION */

        case MCTRANS_GATING_ACTION:
        {
          /* Gating can be turned on if we are operating no R-SCH & are in RC 3
             or higher mode */
          const caii_srv_cfg_type  *config = snm_get_current_config();

          packet->action.txc_gating.rev_fch_gating_on =
            !mccsch_sch_is_active(MCCSCH_REVERSE,0) &&
            ((rev_chan_mask & MCC_R_DCCH) == 0) &&
              config->for_fch_rc >= CAI_RC_3 && cdma.rev_fch_gating_on;
          break;
        }

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action);
          break;

      }
      break;
    } /* end of case MCTRANS_SERVER_TXC */

#if defined (FEATURE_IS2000_REL_A)
    case MCTRANS_SERVER_SRCH:
    {
      packet->action.srch_ffpc.fpc_pri_chan = (cai_fpc_pri_chan_type) comp_config->non_neg_scr.fpc_pri_chan;
      break;
    }
#endif /* FEATURE_IS2000_REL_A */

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid Server Token %d", server_token);
      break;
  }
  /* end of switch (server_token) */
  MODEM_1X_UNUSED(curr_trans);
} /* mcctxns_scr_action_packet */

/*===========================================================================

FUNCTION mcctxns_ho_pre

DESCRIPTION
  Pre-processor for HO transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_ho_pre
(
  mctrans_trans_info_type * curr_trans
)
{
  // First get pointer to ho data
  mcctcho_ho_data_type *ho_data = mcctcho_get_ho_data();

  // Initialize transaction data
  mcctxns_init_ho_trans(curr_trans);

  /* 1X2G TA:Check if TA allowed */
  if( srchtc_is_ta_allowed() )
  {
    /* 1X2G TA: Check if we need clear any TA HHO/HO global variables */
    if(ho_data->hard_handoff_in_progress)
    {
      /* Clear TA HHO Defer Timer */
      (void) mcc_clr_callback_timer( &mcc_ta_hho_defer_timer,
                                     MCC_TA_HHO_DEFER_TIMER_ID );
      
      /* Reset HHO defer timer flag */
      cdma.tc_data_ta.is_hho_defer_timer_expired = FALSE; 

      /* 1xTA: Update HHO progress state */
      cdma.tc_data_ta.hho_prog = HHO_TXNS_IN_PROG;
      
      /* Start HHO Exec timer with maximum time to execute HHO, 
       * before honoring any further TA requests while HHO in progress */ 
      timer_reg( &mcc_ta_hho_exec_timer,
            (timer_t2_cb_type)mcctcsup_proc_ta_hho_exec_timer_expiry,
                 (timer_cb_data_type)NULL,
                 (uint32) ONEX2GTA_HHO_ALLOWED_EXEC_TIME, 0L );
               
      M1X_MSG( DCP, LEGACY_HIGH,
               "1xTA:Started HHO Exec timer:%d-msec,hho_defer_timer_expired:%d", 
               ONEX2GTA_HHO_ALLOWED_EXEC_TIME, 
               cdma.tc_data_ta.is_hho_defer_timer_expired );               
    }
    else /* Soft HO case */
    {
      /* Clear TA SHO Defer Timer */
      (void) mcc_clr_callback_timer( &mcc_ta_sho_defer_timer,
                                     MCC_TA_SHO_DEFER_TIMER_ID );
      
      /* Reset SHO defer timer flag */
      cdma.tc_data_ta.is_sho_defer_timer_expired = FALSE; 
    }
  } /* 1X2G TA */

#if defined FEATURE_IS2000_REL_A_SVD
  /* No need remember about FCH restoration , HO changes the entire
  active set anyway */
  mccccl_code_channel_list.set_fch_to_be_restored( FALSE );
#endif /* FEATURE_IS2000_REL_A_SVD */


  if (ho_data->ho_msg.extra_parms && ho_data->ho_msg.extra.return_if_ho_fail)
  {
    /* If we are to return to the current configuration after a hard handoff
       failure, we must store the existing configuration parameters. */
    save_serving_config_parms();

    /* Save all actions that were sent to lower layers */
    mcctcho_save_actions_sent_to_lower_layers( curr_trans);
  }

  /* Update CCL with new ASET here */
  mcctcho_update_ccl_with_new_aset();

#ifdef FEATURE_IS2000_1X_ADV
  /* Section 2.6.4.3 and 2.6.16 */
  /* If General Extension record for RCP is received then the received
     parameters will be used */
  if (mcc_rcp_extended)
  {
    /* For HHO, set all the radio config parameters to default. */
    if (ho_data->hard_handoff_in_progress)
    {
      /* Initialize all parameters to defaults */
      M1X_MSG( DCP, LEGACY_MED,
        "Initializing RCP data to default values");
      mcc_update_rcp_data(NULL);
    }

    /* Now update RC parameters with those received in RCP record */
    M1X_MSG( DCP, LEGACY_MED,
      "Updating RCP data with received values");
    mcc_update_rcp_data(&mcc_gem_rcp_rec);

    /* Update NNSCR with the FPC_MODE from RCP data */
    snm_update_nn_scr_from_rcp_info();
  }
  else /* if(!mcc_rcp_extended) */
  {
    /* For HHO, set all the radio config parameters to default. */
    if (ho_data->hard_handoff_in_progress)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Initializing RCP data to default values");
      mcc_update_rcp_data(NULL);
    }
  }

  /* Synchronize pilot records in RCP data with the current ASET */
  mcc_sync_rcp_data_and_aset();

  /* Set RCP extension record indicator to FALSE */
  mcc_rcp_extended = FALSE;
#endif /* FEATURE_IS2000_1X_ADV */

  /* Do things here which were previously done upon rxing TXC_FRAME_SIG from TXC */

  if (ho_data->hard_handoff_in_progress)
  {
    /* Mark transmitter as being OFF */
    cdma.tc_tx_on = FALSE;
  }

  /* If service configuration is included in the HDM, and if it's not a RIF HHO,
     we need to perform the time critical part of update configuration.
     Note, for RIF HHO this is done later. */
  if (ho_data->ho_msg.extra_parms &&
      !(ho_data->hard_handoff_in_progress && ho_data->ho_msg.extra.return_if_ho_fail))
  {
    if (ho_data->ho_msg.extra.scr_included)
    {
      snm_update_configuration();
    }
    /* Update only NNSCR */
    else if (ho_data->ho_msg.extra.nnscr_included)
    {
      snm_update_non_neg_configuration();
    }
#if defined(FEATURE_IS2000_REL_C)
    /* Update SYNC ID info to be restored */
    mccsyncid_update_restore_info();
#endif /* FEATURE_IS2000_REL_C */
  }

  /* --------------------------------------------------------------------
  ** If the long code mask information contained in the Handoff Direction
  ** Message is different from what we are currently using, update our
  ** internal variables.
  ** --------------------------------------------------------------------- */
  if (mcctcho_different_long_code_mask(&ho_data->ho_msg))
  {
    mcctcho_update_long_code_mask_info(&ho_data->ho_msg);
  }

} //lint !e715  trans_num not used.

/*===========================================================================

FUNCTION mcctxns_ho_action_packet

DESCRIPTION
  Prepares the action packets for HO transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_ho_action_packet
(
  mctrans_trans_info_type * curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
)
{
  // Obtain pointer to mcctcho_ho_data...
  const mcctcho_ho_data_type *ho_data = mcctcho_get_ho_data();
  // ...and determine if it is a hard handoff and if so whether return-if-fail
  boolean hho = ho_data->hard_handoff_in_progress;
  boolean rif_hho = ho_data->ho_msg.extra_parms && ho_data->ho_msg.extra.return_if_ho_fail;

  mcc_fwd_chan_mask_type ho_fwd_chan_mask;
  mcc_rev_chan_mask_type ho_rev_chan_mask;

  // Obtain pointer to configuration containing the RC to use after the HO.  If
  // the ?HDM does not contain SCR, then it is the current SCR; otherwise, use
  // pending SCR.
  const caii_srv_cfg_type* config;
  const caii_non_neg_srv_cfg_type* nn_config;

  if ( ho_data->pending_scr_nnscr_changed)
  {
    /* We need to look at the values in pending cfg */
    config = &snm_get_complete_pending_config()->neg_scr;
    nn_config = &snm_get_complete_pending_config()->non_neg_scr;
  }
  else
  {
    /* We need to look at the values in  current cfg as
    there is no update to this */
    config = snm_get_current_config();
    nn_config = snm_get_current_nnscr();
  }

  /* The new channel mask was generated in the handoff pre-processor. */
  ho_fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask();
  ho_rev_chan_mask = mccccl_code_channel_list.get_rev_chan_mask();

  /* Stamp action type in action packet */
  packet->action_type = action;

  switch ( server_token )
  {
    case MCTRANS_SERVER_SRCH :
    {
      switch (action)
      {
        case MCTRANS_HO_ACTION:
        {

          packet->action.srch_ho.hho_ind = hho;
          packet->action.srch_ho.rif_ind = rif_hho;

          /* Fill in search parameters if included */
          if ( ho_data->ho_msg.srch_inc )
          {
            packet->action.srch_ho.srch_parms.included = TRUE;
            packet->action.srch_ho.srch_parms.values = ho_data->ho_msg.srch;
          }
          else
          {
            packet->action.srch_ho.srch_parms.included = FALSE;
          }

          break;
        }

        case MCTRANS_ASET_ACTION:
        {
          (void) mcctcho_generate_aset( &packet->action.srch_aset.tc_aset.aset_cnt,
                                             packet->action.srch_aset.tc_aset.aset);
          /* Since this is a soft handoff, use current band class and frequency */
          packet->action.srch_aset.tc_aset.cdma_freq =
            (srch_rx_chan_type) cdma.cdmach;
          packet->action.srch_aset.tc_aset.band_class =
            (srch_rx_band_type) cdma.band_class;

          /* Soft handoff, clear the HHO flag */
          packet->action.srch_aset.tc_aset.is_hho = FALSE;
          break;
        }


        case MCTRANS_TX_ON_OFF_ACTION:
        {
          /* If we got this action, it has to be a HHO, so inform SRCH that TX is
             about to be off */
          packet->action.srch_tx_on_off.tx_on = FALSE;
          break;
        }

        #if defined ( FEATURE_IS2000_REL_A)
        case MCTRANS_FFPC_ACTION:
        {
          /* The FPC_PRI_CHAN has changed either due to a new NN-SCR or because
          the channel mask has changed requiring this to change , If it is because of
          latter, no need to worry about the value in NN-SCR anyway */
          if (ho_data->new_fpc_pri_chan)
          {
            if ((ho_fwd_chan_mask & (MCC_F_FCH|MCC_F_DCCH)) != 0)
            {
              packet->action.srch_ffpc.fpc_pri_chan =
                             (ho_fwd_chan_mask >> 1) & 1;
              /* RXC FFPC action takes care of updating this in NN-SCR data base */
            }
            else
            {
              packet->action.srch_ffpc.fpc_pri_chan = CAI_FPC_PRI_CHAN_CPCCH;
            }
          }
          else if ( ho_data->ho_msg.extra_parms && ho_data->ho_msg.extra.nnscr_included &&
                    ho_data->ho_msg.extra.non_neg_cfg.fpc_incl )
          {
              if ((ho_fwd_chan_mask & (MCC_F_FCH|MCC_F_DCCH)) != 0)
              {
                packet->action.srch_ffpc.fpc_pri_chan =
                  (cai_fpc_pri_chan_type) ho_data->ho_msg.extra.non_neg_cfg.fpc_pri_chan;
              }
              else
              {
                packet->action.srch_ffpc.fpc_pri_chan = CAI_FPC_PRI_CHAN_CPCCH;
              }
          }
          break;
        }
#endif /* FEATURE_IS2000_REL_A */

        case MCTRANS_LCM_ACTION:
          mcctcsup_get_long_code_mask(packet->action.srch_lcm.long_code_mask);
          break;

        #ifdef FEATURE_IS2000_1X_ADV
        case MCTRANS_RCP_ACTION:
        {
          mcc_get_pilots_rc_param_recs((cai_rcp_srch_type*)&packet->action.srch_rcp);
          break;
        }
        #endif /* FEATURE_IS2000_1X_ADV */

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action );
          break;

      }
      break;
    }
    case MCTRANS_SERVER_RXC :
    {
      switch ( action )
      {
        case MCTRANS_SCR_ACTION :
          mcctxns_prepare_scr_action_packet(MCTRANS_SERVER_RXC, packet , config, nn_config);
          /* If it is a HO related SCR action, then we do not want SCR function
          to change the rev pwr cntl delay value, This is handled by the HO action */
          packet->action.rxc_scr.rev_pwr_cntl_delay.included = FALSE;
          break;

        case MCTRANS_HO_ACTION :
          packet->action.rxc_ho.hho_ind = hho;
          packet->action.rxc_ho.rif_ind = rif_hho;
          if (ho_data->ho_msg.extra_parms)
          {
            packet->action.rxc_ho.reset_fpc = ho_data->ho_msg.extra.reset_fpc;
          }
          else
          {
            packet->action.rxc_ho.reset_fpc = FALSE;
          }
          packet->action.rxc_ho.rev_pwr_cntl_delay.included =
                    ho_data->ho_msg.rev_pwr_cntl_delay_incl;
          if (ho_data->ho_msg.rev_pwr_cntl_delay_incl)
          {
            if (ho_data->high_rc_after_ho)
            {
              packet->action.rxc_ho.rev_pwr_cntl_delay.value =
                ho_data->ho_msg.rev_pwr_cntl_delay;
            }
            else
            {
              /* After HO we are going to be in an old RC, just use the
                 default value */
              packet->action.rxc_ho.rev_pwr_cntl_delay.value =
                CAI_DEFAULT_REV_PWR_CNTL_DELAY;
            }
          }
          else
          {
            /* Okay this HO msg did not have this value included but our RC config
            could have changed to warrant the change of this value, check for that */
            if (ho_data->high_rc_after_ho != ho_data->high_rc_before_ho)
            {
              packet->action.rxc_ho.rev_pwr_cntl_delay.included = TRUE;
              packet->action.rxc_ho.rev_pwr_cntl_delay.value = (ho_data->high_rc_after_ho)?
                cdma.rev_pwr_cntl_delay:CAI_DEFAULT_REV_PWR_CNTL_DELAY;
            }
          }
#ifdef FEATURE_IS2000_REL_A
          packet->action.rxc_ho.p_rev_in_use = ho_data->new_p_rev_in_use;
#endif
          break;

        case MCTRANS_FFPC_ACTION :
        {
          // First initialize action packet
          mcctxns_init_ffpc( &packet->action.ffpc );

          if ( ho_data->ho_msg.extra_parms && ho_data->ho_msg.extra.nnscr_included &&
               ho_data->ho_msg.extra.non_neg_cfg.fpc_incl )
          {
            mcctxns_prepare_scr_ffpc_action_packet( &ho_data->ho_msg.extra.non_neg_cfg, &packet->action.ffpc );
          }

          // FFPC needs to be notified if FCH/DCCH is starting/stopping and if RC is changing
          if ( ho_fwd_chan_mask & MCC_F_DCCH )
          {
            packet->action.ffpc.dcch_ffpc_op.mode = MCTRANS_FFPC_START;
            // We have already verified that DCCH is included in SCR since we are here
            packet->action.ffpc.dcch_ffpc_op.rc = (cai_radio_config_type) config->for_dcch_rc;
          }
          else
          {
            packet->action.ffpc.dcch_ffpc_op.mode = MCTRANS_FFPC_STOP;
          }

          if ( ho_fwd_chan_mask & MCC_F_FCH )
          {
            packet->action.ffpc.fch_ffpc_op.mode = MCTRANS_FFPC_START;
            packet->action.ffpc.fch_ffpc_op.rc = (cai_radio_config_type) config->for_fch_rc;
          }
          else
          {
            packet->action.ffpc.fch_ffpc_op.mode = MCTRANS_FFPC_STOP;
          }

          // Now fpc_subchan_gain.  process_subchan_gain is set properly when the transaction
          // is set up
          if ( ho_data->process_subchan_gain_at_ho )
          {
            // No separate action time for fpc_subchan_gain
            packet->action.ffpc.fpc_subchan_gain.included = TRUE;
            packet->action.ffpc.fpc_subchan_gain.value = ho_data->ho_msg.fpc_subchan_gain;
          }

          if ( ho_data->new_fpc_pri_chan )
          {
            // When this happens, we know that lower 2 bits of CH_IND are
            // either changing from 01 to 10 or from 10 to 01.
            packet->action.ffpc.fpc_pri_chan.included = TRUE;
            if (mccccl_code_channel_list.fwd_fundicated_chan_is_active())
            {
              packet->action.ffpc.fpc_pri_chan.value = (ho_fwd_chan_mask >> 1) & 1;
            }
            else
            {
              packet->action.ffpc.fpc_pri_chan.value = CAI_FPC_PRI_CHAN_CPCCH; /*lint !e641 */
              packet->action.ffpc.fpc_mode.included = TRUE;
              packet->action.ffpc.fpc_mode.value = CAI_FPC_MODE_RESERVED;
            }

            /* Make sure NNSCR is updated respectively */
            snm_update_fpc_pri_chan(packet->action.ffpc.fpc_pri_chan.value);
          }
          break;
        }

        case MCTRANS_CTRL_HLD_GATING_ACTION :
        {

          if (!(mccccl_code_channel_list.in_control_hold()))
          {
            /* if mccccl says MS is not in CHS, then, we must be exiting CHS */
            packet->action.rxc_ctrl_hld_gating.gating_rate = CAI_GATING_RATE_NONE;
          }
          else
          {
            packet->action.rxc_ctrl_hld_gating.gating_rate = (cai_pilot_gating_rate_type) nn_config->pilot_gate_rate;
          }

          if ( packet->action.rxc_ctrl_hld_gating.gating_rate == CAI_GATING_RATE_NONE ||
               packet->action.rxc_ctrl_hld_gating.gating_rate == CAI_GATING_RATE_ONE )
          {
            // No gating, use BS assigned power control delay
            packet->action.rxc_ctrl_hld_gating.rev_pwr_control_delay =
              (ho_data->ho_msg.rev_pwr_cntl_delay_incl) ?
              ho_data->ho_msg.rev_pwr_cntl_delay : cdma.rev_pwr_cntl_delay;
          }
          else if(packet->action.rxc_ctrl_hld_gating.gating_rate != CAI_GATING_RATE_RESERVED)
          {
            // Otherwise, use appropriate delay for gating rate
            packet->action.rxc_ctrl_hld_gating.rev_pwr_control_delay =
              gating_rate_to_pwr_delay[nn_config->pilot_gate_rate];
          }
          break;
        }

        #ifdef FEATURE_IS2000_1X_ADV
        case MCTRANS_RCP_ACTION:
        {
          mcc_get_rxc_txc_rc_params( &packet->action.rxc_rcp );
          break;
        }
        #endif /* FEATURE_IS2000_1X_ADV */

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action );
          break;
      }
      break;
    } // of case MCTRANS_SERVER_RXC

    case MCTRANS_SERVER_TXC :
    {
      switch ( action )
      {
        case MCTRANS_CTRL_HLD_GATING_ACTION :
        {

          if (!(mccccl_code_channel_list.in_control_hold()))
          {
            /* if mccccl says MS is not in CHS, then, we must be exiting CHS */
            packet->action.txc_ctrl_hld_gating.gating_rate = CAI_GATING_RATE_NONE;
          }
          else
          {
            packet->action.txc_ctrl_hld_gating.gating_rate =
              (cai_pilot_gating_rate_type) nn_config->pilot_gate_rate;
          }
          break;
        }

        case MCTRANS_SCR_ACTION :
          mcctxns_prepare_scr_action_packet(MCTRANS_SERVER_TXC, packet , config, nn_config);
          break;

        case MCTRANS_LCM_ACTION:
          mcctcsup_get_long_code_mask(packet->action.txc_lcm.long_code_mask);
          break;

        case MCTRANS_HO_ACTION :
        {
          packet->action.txc_ho.hho_ind = hho;
          packet->action.txc_ho.rif_ind = rif_hho;

          /* Initialize FO value, TXC looks at this value always */
          packet->action.txc_ho.frame_offset = cdma.frame_offset;
          if (ho_data->ho_msg.extra_parms)
          {
            packet->action.txc_ho.frame_offset = ho_data->ho_msg.extra.frame_offset;
            // See if this is a regular old HDM.  If so, both nom_pwr and nu_preamble
            // are not included
            if ( ho_data->ho_msg.msg_type == CAI_HO_DIR_MSG )
            {
              packet->action.txc_ho.nom_pwr.included = FALSE;
              // Standard says that num_preamble is 0 for this message
              packet->action.txc_ho.num_preamble.unit = MCTRANS_PREAMBLE_FRAME;
              packet->action.txc_ho.num_preamble.value = 0;
              // We are done for this action here.
              break;
            }
            // If we are here, it's E/G/U HDM.
            if ( ho_data->high_rc_after_ho )
            {
              // Tiger3 supports PCG preambles
              packet->action.txc_ho.num_preamble.unit = MCTRANS_PREAMBLE_PCG;
              // Convert value into actual number of PCGs
              packet->action.txc_ho.num_preamble.value = ho_data->ho_msg.extra.num_preamble*2;  //lint !e734
              if ( ho_data->ho_msg.extra.num_preamble == 7 )
              {
                packet->action.txc_ho.num_preamble.value += 2;
              }
            }
            else
            {
              // Low RC uses frames
              packet->action.txc_ho.num_preamble.unit = MCTRANS_PREAMBLE_FRAME;
              packet->action.txc_ho.num_preamble.value = ho_data->ho_msg.extra.num_preamble;
            }
            packet->action.txc_ho.nom_pwr.included = TRUE;
            // nom_pwr has bit #3 as sign bit (4-bit field)
            if ( ho_data->ho_msg.extra.nom_pwr & 0x08 )
            {
              packet->action.txc_ho.nom_pwr.value = ho_data->ho_msg.extra.nom_pwr | 0xF0;
            }
            else
            {
              packet->action.txc_ho.nom_pwr.value = ho_data->ho_msg.extra.nom_pwr;
            }
            if ( ho_data->ho_msg.extra.nom_pwr_ext )
            {
               /* ----------------------------------------------------------------
               ** (nom_pwr_ext == 1) means nom_pwr is in the range (-24dB to -9dB)
               ** instead of (-8dB to 7dB), so we subtract 16dB from the nom_pwr
               ** adjustment to put it in the right range.
               ** ---------------------------------------------------------------- */
              packet->action.txc_ho.nom_pwr.value -= 16;
            }
          }
          else
          {
            /* Set some default values here */
            packet->action.txc_ho.frame_offset = cdma.frame_offset;
            packet->action.txc_ho.num_preamble.unit = MCTRANS_PREAMBLE_FRAME;
            packet->action.txc_ho.num_preamble.value = 0;
            packet->action.txc_ho.nom_pwr.included = FALSE;
          }
#ifdef FEATURE_IS2000_REL_A
          packet->action.txc_ho.p_rev_in_use = ho_data->new_p_rev_in_use;
#endif

          break;
        }
        case MCTRANS_RLGC_ACTION :
        {
          mcctxns_init_rlgc( &packet->action.rlgc );

          // If extra parms included, and if P_REV is 6 or higher, use_default_rlag and
          // rlgain_traffic_pilot are included with GHDM and UHDM.  We don't need to check
          // for P_REV here since this action is include only if P_REV is 6 or higher.
          if ( ho_data->ho_msg.extra_parms &&
               ( ho_data->ho_msg.msg_type == CAI_GEN_HO_DIR_MSG ||
                 ho_data->ho_msg.msg_type == CAI_UNIV_HO_DIR_MSG

                 || ho_data->ho_msg.msg_type == CAI_MEID_UNIV_HO_DIR_MSG

               ) )
          {
            packet->action.rlgc.use_default_rlag = ho_data->ho_msg.extra.default_rlag;
            packet->action.rlgc.rlgain_traffic_pilot.included = TRUE;
            // Note: rlgain_traffic_pilot is a 6-bit 2's compliment number
            packet->action.rlgc.rlgain_traffic_pilot.value =
              ( ho_data->ho_msg.extra.rlgain_traffic_pilot & 0x20 ) ?
              ho_data->ho_msg.extra.rlgain_traffic_pilot | 0xc0 :
              ho_data->ho_msg.extra.rlgain_traffic_pilot; //lint !e734
          }

          if ( (packet->action.rlgc.pwr_cntl_step.included = ho_data->ho_msg.use_pwr_cntl_step )
               != FALSE )
          {
            packet->action.rlgc.pwr_cntl_step.value = ho_data->ho_msg.pwr_cntl_step;
          }

          // RLGC needs to be notified if FCH/DCCH is starting/stopping
          if (ho_rev_chan_mask & MCC_R_DCCH)
          {
            packet->action.rlgc.dcch_rlgc_op.mode = MCTRANS_RLGC_START;
            // We have already verified that DCCH is included since we are here
            packet->action.rlgc.dcch_rlgc_op.rc = (cai_radio_config_type) config->rev_dcch_rc;
          }

          if (ho_rev_chan_mask & MCC_R_FCH)
          {
            packet->action.rlgc.fch_rlgc_op.mode = MCTRANS_RLGC_START;
            packet->action.rlgc.fch_rlgc_op.rc = (cai_radio_config_type) config->rev_fch_rc;
          }

          /* We are done here. HO does not have SCH RLGC parameters. */
          break;
        }

        case MCTRANS_GATING_ACTION:
        {
          /* When DCCH is supported, change this to check for DCCH
          assignment as well */
          packet->action.txc_gating.rev_fch_gating_on =
            /* If R-DCCH is in use, cannot use R-FCH gating */
            !(ho_rev_chan_mask & MCC_R_DCCH) &&
            !mccsch_sch_is_active( MCCSCH_REVERSE,0) &&
            ho_data->high_rc_after_ho && ho_data->ho_msg.rev_fch_gating_mode;
          break;
        }

        #ifdef FEATURE_IS2000_1X_ADV
        case MCTRANS_RCP_ACTION:
        {
          mcc_get_rxc_txc_rc_params( &packet->action.txc_rcp );
          break;
        }
        #endif /* FEATURE_IS2000_1X_ADV */

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action );
          break;
      }
      break;
    } // of case MCTRANS_SERVER_TXC

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid Server Token %d", server_token);
      break;
  }

}

/*===========================================================================

FUNCTION mcctxns_subchan_gain_action_packet

DESCRIPTION
  This function fills in the action packet for FPC_SUBCHAN_GAIN transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_subchan_gain_action_packet
(
  mctrans_trans_info_type * curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
)
{
  const mcctcho_ho_data_type *ho_data = mcctcho_get_ho_data();

  // Stamp action type in action packet
  packet->action_type = action;

  // FPC_SUBCHAN_GAIN transaction only contains 1 action which is for RXC.  We can just make the
  // assumption here and not check for the server type or action type.
  // First initialize packet to update nothing.
  mcctxns_init_ffpc( &packet->action.ffpc );

  // Take fpc subgain gain value from the message.  If this transaction ever get dispatched, we
  // are assuming that the subchan gain value is not overwritten by another message.
  packet->action.ffpc.fpc_subchan_gain.included = TRUE;
  packet->action.ffpc.fpc_subchan_gain.value = ho_data->ho_msg.fpc_subchan_gain;

} //lint !e715  server_token, curr_trans not used.
// mcctxns_subchan_gain_action_packet

/*===========================================================================

FUNCTION mcctxns_subchan_gain_post

DESCRIPTION
  Post-processor for FPC_SUNCHAN_GAIN transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_subchan_gain_post
(
  mctrans_trans_info_type * curr_trans
)
{
  const mcctcho_ho_data_type *ho_data = mcctcho_get_ho_data();

  // When fpc_subchan_gain is updated, update that in cdma structure
  cdma.fpc_subchan_gain = ho_data->ho_msg.fpc_subchan_gain;

} //lint !e715  curr_trans not used.
// mcctxns_subchan_gain_post
/*===========================================================================

FUNCTION mcctxns_subchan_gain_pre

DESCRIPTION
  Pre-processor for FPC_SUNCHAN_GAIN transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_subchan_gain_pre
(
  mctrans_trans_info_type * curr_trans
)
{
  mcctxns_init_subchan_gain_trans(curr_trans);

}
// mcctxns_subchan_gain_pre

/*===========================================================================

FUNCTION mcctxns_pwr_ctrl_action_packet

DESCRIPTION
  This function fills in the action packets actions in PWR_CTRL_MSG transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_pwr_ctrl_action_packet
(
  mctrans_trans_info_type * curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
)
{
  uint8 i;
  const mctrans_pending_action_msg_type* pending_msg = mctrans_get_pending_msg( curr_trans->trans_num );

  // Stamp action type in action packet
  packet->action_type = action;

  switch (server_token)
  {
    case MCTRANS_SERVER_RXC :
    {
      // For RXC, only FFPC action can be included.  Just assume that here.
      mcctxns_init_ffpc( &packet->action.ffpc );

      if (pending_msg != NULL)
      {
        // We know that FPC is included in the message, no need to check here.
        // FPC_MODE and FPC_PRI_CHAN always included if FPC is included
        // Standard mandates that FPC_MODE included in this message cannot be be made
        // current if we are in a F-SCH burst, so check for that first (just check for
        // SCH 0 since we only support that ).  When the burst ends, this value will be
        // recovered.
        if ( !mccsch_sch_is_active( MCCSCH_FORWARD, 0 ) )
        {
          packet->action.ffpc.fpc_mode.included = TRUE;
          /* If there is no forward fundicated channel active, then do not
          use the information from the  message */
          if (mccccl_code_channel_list.fwd_fundicated_chan_is_active())
          {
            packet->action.ffpc.fpc_mode.value =
              (cai_fpc_mode_type) pending_msg->pwr_ctrl.fpc_mode;
          }
          else
          {
            packet->action.ffpc.fpc_mode.value = CAI_FPC_MODE_RESERVED;
          }
        }
        // FPC_PRI_CHAN is include in the message.  We already verified that the channel
        // is actually active when the message is received.  We assume that it still is at
        // the action time (otherwise we don't know what we can do!)
        packet->action.ffpc.fpc_pri_chan.included = TRUE;

        /* If there is no forward fundicated channel active, Mux should not
           apply this information, but store it for later */
        if (mccccl_code_channel_list.fwd_fundicated_chan_is_active())
        {
          packet->action.ffpc.fpc_pri_chan.value = pending_msg->pwr_ctrl.fpc_pri_chan;
        }
        else
        {
          packet->action.ffpc.fpc_pri_chan.value = CAI_FPC_PRI_CHAN_CPCCH; /*lint !e641 */
        }

        // Now the FCH OLPC parameters, if included
        if (pending_msg->pwr_ctrl.fpc_olpc_fch_incl)
        {
          packet->action.ffpc.fch_olpc.included    = TRUE;
          if ( pending_msg->pwr_ctrl.fpc_fch_max_setpt == 0xff )  // This special value means set to current set point
          {
            packet->action.ffpc.fch_olpc.param.max_setpt.set_to_current = TRUE;
          }
          else
          {
            packet->action.ffpc.fch_olpc.param.max_setpt.value = pending_msg->pwr_ctrl.fpc_fch_max_setpt;
          }

          if ( pending_msg->pwr_ctrl.fpc_fch_min_setpt == 0xff )  // This special value means set to current set point
          {
            packet->action.ffpc.fch_olpc.param.min_setpt.set_to_current = TRUE;
          }
          else
          {
            packet->action.ffpc.fch_olpc.param.min_setpt.value = pending_msg->pwr_ctrl.fpc_fch_min_setpt;
          }

          packet->action.ffpc.fch_olpc.param.target_fer  = pending_msg->pwr_ctrl.fpc_fch_fer;
        }

        // Now the DCCH OLPC parameters, if included
        if (pending_msg->pwr_ctrl.fpc_olpc_dcch_incl)
        {
          packet->action.ffpc.dcch_olpc.included   = TRUE;
          if ( pending_msg->pwr_ctrl.fpc_dcch_max_setpt == 0xff )  // This special value means set to current set point
          {
            packet->action.ffpc.dcch_olpc.param.max_setpt.set_to_current = TRUE;
          }
          else
          {
            packet->action.ffpc.dcch_olpc.param.max_setpt.value = pending_msg->pwr_ctrl.fpc_dcch_max_setpt;
          }

          if ( pending_msg->pwr_ctrl.fpc_dcch_min_setpt == 0xff )  // This special value means set to current set point
          {
            packet->action.ffpc.dcch_olpc.param.min_setpt.set_to_current = TRUE;
          }
          else
          {
            packet->action.ffpc.dcch_olpc.param.min_setpt.value = pending_msg->pwr_ctrl.fpc_dcch_min_setpt;
          }

          packet->action.ffpc.dcch_olpc.param.target_fer = pending_msg->pwr_ctrl.fpc_dcch_fer;
        }

        // Now the SCH OLPC parameters, if any
        for ( i = 0; i < pending_msg->pwr_ctrl.num_sup; ++i )
        {
          uint8 sch_id = pending_msg->pwr_ctrl.sup_chns[i].sch_id;  // Get sch_id for readability

          packet->action.ffpc.sch.included = TRUE;
          packet->action.ffpc.sch.sch_olpc[sch_id].included   = TRUE;

          // Note: init setpoint is not included with PCNM
          if ( pending_msg->pwr_ctrl.sup_chns[i].fpc_sch_max_setpt == 0xff )  // This special value means set to current set point
          {
            packet->action.ffpc.sch.sch_olpc[sch_id].param.max_setpt.set_to_current = TRUE;
          }
          else
          {
            packet->action.ffpc.sch.sch_olpc[sch_id].param.max_setpt.value =
            pending_msg->pwr_ctrl.sup_chns[i].fpc_sch_max_setpt;
          }

          if ( pending_msg->pwr_ctrl.sup_chns[i].fpc_sch_min_setpt == 0xff )  // This special value means set to current set point
          {
            packet->action.ffpc.sch.sch_olpc[sch_id].param.min_setpt.set_to_current = TRUE;
          }
          else
          {
            packet->action.ffpc.sch.sch_olpc[sch_id].param.min_setpt.value =
              pending_msg->pwr_ctrl.sup_chns[i].fpc_sch_min_setpt;
          }

          packet->action.ffpc.sch.sch_olpc[sch_id].param.target_fer = pending_msg->pwr_ctrl.sup_chns[i].fpc_sch_fer;
        }

        // Now include setpoint threshold if included
        if ( pending_msg->pwr_ctrl.fpc_thresh_incl )
        {
          packet->action.ffpc.fpc_setpt_thresh.included = TRUE;
          packet->action.ffpc.fpc_setpt_thresh.value = pending_msg->pwr_ctrl.fpc_setpt_thresh;
        }

        if ( pending_msg->pwr_ctrl.fpc_thresh_sch_incl )
        {
          packet->action.ffpc.sch.included = TRUE;
          packet->action.ffpc.sch.fpc_thresh_sch.included = TRUE;
          packet->action.ffpc.sch.fpc_thresh_sch.value = pending_msg->pwr_ctrl.fpc_setpt_thresh_sch;
        }

        // Now see if we need to include FPC_SEC_CHAN
        if ( pending_msg->pwr_ctrl.fpc_mode == CAI_FPC_MODE_400_PRI_400_SEC
             || pending_msg->pwr_ctrl.fpc_mode == CAI_FPC_MODE_200_PRI_600_SEC /*lint !e641 */
             #ifdef FEATURE_IS2000_REL_A
             || (pending_msg->pwr_ctrl.fpc_mode == CAI_FPC_MODE_QIB_50_PRI_EIB_50_SEC) /*lint !e641 */
             || (pending_msg->pwr_ctrl.fpc_mode == CAI_FPC_MODE_400_PRI_EIB_50_SEC) /*lint !e641 */
             #endif
           )
        {
          // For these modes, FPC_SEC_CHAN is included
          packet->action.ffpc.sch.included = TRUE;
          packet->action.ffpc.sch.fpc_sec_chan.included = TRUE;
          packet->action.ffpc.sch.fpc_sec_chan.value = pending_msg->pwr_ctrl.fpc_sec_chan;
        }
      } /* Pending_msg != NULL */

      // fpc_sch_init_setpt_op is not included in this message.  Just default it to false
      // regardless of whether it's needed or not.
      packet->action.ffpc.sch.fpc_sch_init_setpt_op = FALSE;

      break;
    } // of case MCTRANS_SERVER_RXC

    case MCTRANS_SERVER_TXC :
    {
      switch (action)
      {
        case MCTRANS_RLGC_ACTION:
        {
          mcctxns_init_rlgc( &packet->action.rlgc );

          if (pending_msg != NULL)
          {
            // Power control step is always included
            packet->action.rlgc.pwr_cntl_step.included = TRUE;
            packet->action.rlgc.pwr_cntl_step.value = pending_msg->pwr_ctrl.pwr_cntl_step;

            // If RPC info is included, go through all the RPC records.
            if ( pending_msg->pwr_ctrl.rpc_incl )
            {
              packet->action.rlgc.num_of_rpc_records = pending_msg->pwr_ctrl.rpc_num_rec;
              for ( i = 0; i < pending_msg->pwr_ctrl.rpc_num_rec; ++i )
              {
                packet->action.rlgc.rpc_records[i] = pending_msg->pwr_ctrl.rpc_recs[i];
              }
            }
            else
            {
              packet->action.rlgc.num_of_rpc_records = 0;
            }
          }

          break;
        } // end case MCTRANS_RLGC_ACTION

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action );
          break;

      }
      break;
    } //case MCTRANS_SERVER_TXC

    #ifdef FEATURE_IS2000_REL_A
    case MCTRANS_SERVER_SRCH:
    {
      if (pending_msg != NULL)
      {
        //For SRCH, the only action if FFPC action
        //If this action is included it means that fpc_pri_chan value was valid
        if (mccccl_code_channel_list.fwd_fundicated_chan_is_active())
        {
          packet->action.srch_ffpc.fpc_pri_chan =
            (cai_fpc_pri_chan_type) pending_msg->pwr_ctrl.fpc_pri_chan;
        }
        else
        {
          packet->action.srch_ffpc.fpc_pri_chan = CAI_FPC_PRI_CHAN_CPCCH;
        }
      }
      else
      {
        const caii_non_neg_srv_cfg_type *nnscr = snm_get_current_nnscr();
        packet->action.srch_ffpc.fpc_pri_chan =
          (cai_fpc_pri_chan_type) nnscr->fpc_pri_chan;
      }

      break;
    }
    #endif

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid Server Token %d", server_token );
      break;
  }
  // of   switch ( server_token )

} // mcctxns_pwr_ctrl_action_packet

/*===========================================================================

FUNCTION mcctxns_pwr_ctrl_pre

DESCRIPTION
  Pre-processor for PWR_CTRL_MSG transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_pwr_ctrl_pre
(
  mctrans_trans_info_type * curr_trans
)
{
  mcctxns_init_pwr_ctrl_trans(curr_trans);

} // mcctxns_pwr_ctrl_pre

/*===========================================================================

FUNCTION mcctxns_init_ffpc

DESCRIPTION
  Initializes the action packet for FFPC to included nothing.  All "included"
  fields are initialized to FALSE, operation modes are initialized to
  MCTRANS_FFPC_CONTINUE

DEPENDENCIES
  Service configuration must already exist.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_init_ffpc
(
  mctrans_ffpc_action_type* ffpc
)
{
  uint8 n;
  mcc_fwd_chan_mask_type fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask();
  const caii_srv_cfg_type  *config = snm_get_current_config();

  // Initialize FCH FFPC operation
  if (fwd_chan_mask & MCC_F_FCH)
  {
    ffpc->fch_ffpc_op.mode = MCTRANS_FFPC_CONTINUE;
    ffpc->fch_ffpc_op.rc = (cai_radio_config_type)config->for_fch_rc;
  }
  else
  {
    ffpc->fch_ffpc_op.mode = MCTRANS_FFPC_STOP;
  }

  // Initialize DCCH FFPC operation
  if (fwd_chan_mask & MCC_F_DCCH)
  {
    ffpc->dcch_ffpc_op.mode = MCTRANS_FFPC_CONTINUE;
    ffpc->dcch_ffpc_op.rc = (cai_radio_config_type)config->for_dcch_rc;
  }
  else
  {
    ffpc->dcch_ffpc_op.mode = MCTRANS_FFPC_STOP;
  }

  // Initialize SCH FFPC operation
  if (config->for_sch_cc_incl && (config->num_for_sch != 0))
  {
    ffpc->sch_ffpc_op.mode = MCTRANS_FFPC_CONTINUE;
    // Assume SCH 0 here since we support only 1 SCH
    ffpc->sch_ffpc_op.rc = (cai_radio_config_type)config->for_sch[0].sch_chn_cfg.sch_rc;
  }
  else
  {
    ffpc->sch_ffpc_op.mode = MCTRANS_FFPC_STOP;
  }

  ffpc->dcch_olpc.included = FALSE;
  ffpc->dcch_olpc.param.max_setpt.set_to_current = FALSE;
  ffpc->dcch_olpc.param.min_setpt.set_to_current = FALSE;

  ffpc->fpc_mode.included = FALSE;
  ffpc->fpc_subchan_gain.included = FALSE;
  ffpc->fpc_pri_chan.included = FALSE;

  ffpc->fch_olpc.included = FALSE;
  ffpc->fch_olpc.param.max_setpt.set_to_current = FALSE;
  ffpc->fch_olpc.param.min_setpt.set_to_current = FALSE;

  ffpc->fpc_setpt_thresh.included = FALSE;
  ffpc->sch.included = FALSE;
  ffpc->sch.fpc_sec_chan.included = FALSE;
  ffpc->sch.fpc_thresh_sch.included = FALSE;
  for ( n = 0; n < CAI_MAX_NUM_FOR_SCH; ++n )
  {
    ffpc->sch.sch_olpc[n].included = FALSE;
    ffpc->sch.sch_olpc[n].init_setpt.included = FALSE;
    ffpc->sch.sch_olpc[n].param.max_setpt.set_to_current = FALSE;
    ffpc->sch.sch_olpc[n].param.min_setpt.set_to_current = FALSE;
  }

} // mcctxns_init_ffpc

/*===========================================================================

FUNCTION mcctxns_init_rlgc

DESCRIPTION
  Initializes the action packet for RLGC to included nothing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_init_rlgc
(
  mctrans_rlgc_action_type* rlgc
)
{
  uint8 n;

  mcc_rev_chan_mask_type rev_chan_mask = mccccl_code_channel_list.get_rev_chan_mask();
  const caii_srv_cfg_type  *config = snm_get_current_config();

  /* Initialize R-SCH RLGC operation.  Note that here, we must check rev_sch_cc_incl:
     num_rev_sch is not valid unless rev_sch_cc_incl is 1. */
  if (config->rev_sch_cc_incl && (config->num_rev_sch != 0))
  {
    rlgc->sch_rlgc_op.mode = MCTRANS_RLGC_CONTINUE;
  }
  else
  {
    rlgc->sch_rlgc_op.mode = MCTRANS_RLGC_STOP;
  }

  // Initialize FCH RLGC operation
  if (rev_chan_mask & MCC_R_FCH)
  {
    rlgc->fch_rlgc_op.mode = MCTRANS_RLGC_CONTINUE;
  }
  else
  {
    rlgc->fch_rlgc_op.mode = MCTRANS_RLGC_STOP;
  }

  // Initialize DCCH RLGC operation
  if (rev_chan_mask & MCC_R_DCCH)
  {
    rlgc->dcch_rlgc_op.mode = MCTRANS_RLGC_CONTINUE;
  }
  else
  {
    rlgc->dcch_rlgc_op.mode = MCTRANS_RLGC_STOP;
  }

  rlgc->use_default_rlag = FALSE;
  rlgc->pwr_cntl_step.included = FALSE;
  rlgc->rlgain_traffic_pilot.included = FALSE;
  for ( n = 0; n < CAI_MAX_NUM_REV_SCH; ++n )
  {
    rlgc->rlgain_sch_pilot[n].included = FALSE;
  }
  rlgc->num_of_rpc_records = 0;

} // mcctxns_init_rlgc

/*===========================================================================

FUNCTION mcctxns_cfs_nset_action_packet

DESCRIPTION
  This function fills in the action packet for CFS_NSET transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_cfs_nset_action_packet
(
  mctrans_trans_info_type * curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
)
{
  uint8 i;
  const caii_cfs_req_type * cfs_req =
    (caii_cfs_req_type *)  mctrans_get_pending_msg( curr_trans->trans_num );

  // Stamp action type in action packet
  packet->action_type = action;


  switch ( server_token )
  {
    case MCTRANS_SERVER_SRCH :
    {
      /* Save only those CFSReqM parameters that will be used in MC.
      * Others are sent to SRCH via the SRCH_CF_NSET_F command
      */

      if (cfs_req == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Pending msg NULL and no defaults available for CFS NSET");
        packet->action.srch_cfs_nset.pilot_update = 0;
        packet->action.srch_cfs_nset.cf_nset_cnt = 0;
        return;
      }

      packet->action.srch_cfs_nset.search_period =
      srch_period_value[cfs_req->fix.srch_period];
      packet->action.srch_cfs_nset.band_class = cfs_req->var.mod0.band_class;
      packet->action.srch_cfs_nset.cdma_freq = cfs_req->var.mod0.cdma_freq;
      packet->action.srch_cfs_nset.diff_rx_pwr_thresh =
        (int1) cfs_req->var.mod0.diff_rx_pwr_thresh;
      packet->action.srch_cfs_nset.min_total_pilot_ec_io =
        cfs_req->var.mod0.min_total_pilot_ecio;
      packet->action.srch_cfs_nset.sf_total_ec_thresh =
        cfs_req->var.mod0.sf_total_ec_thresh;
      packet->action.srch_cfs_nset.sf_total_ecio_thresh =
        cfs_req->var.mod0.sf_total_ec_io_thresh;
      packet->action.srch_cfs_nset.cf_t_add = cfs_req->var.mod0.cf_t_add;
      packet->action.srch_cfs_nset.pilot_inc = cfs_req->var.mod0.cf_pilot_inc;
      packet->action.srch_cfs_nset.cf_srch_win_n = cfs_req->var.mod0.cf_srch_win_n;
      packet->action.srch_cfs_nset.cf_srch_win_r = cfs_req->var.mod0.cf_srch_win_r;
      packet->action.srch_cfs_nset.cf_nghbr_srch_mode =
        cfs_req->var.mod0.cf_nghbr_srch_mode;
      packet->action.srch_cfs_nset.pilot_update = cfs_req->var.mod0.pilot_update;
      if ( cfs_req->var.mod0.pilot_update )
      {
        packet->action.srch_cfs_nset.cf_nset_cnt = cfs_req->var.mod0.num_pilot;
      }
      else
      {
        packet->action.srch_cfs_nset.cf_nset_cnt = 0;
      }
      for ( i = 0; i < packet->action.srch_cfs_nset.cf_nset_cnt; i++ )
      {
        packet->action.srch_cfs_nset.cf_nset[i].pilot =
          (int2)cfs_req->var.mod0.pilot_update_rec[i].nghbr_pn;
        packet->action.srch_cfs_nset.cf_nset[i].srch_flag =
          cfs_req->var.mod0.pilot_update_rec[i].search_set;
        packet->action.srch_cfs_nset.cf_nset[i].srch_pri =
          cfs_req->var.mod0.pilot_update_rec[i].search_priority;
        packet->action.srch_cfs_nset.cf_nset[i].srch_win =
          cfs_req->var.mod0.pilot_update_rec[i].srch_win_nghbr;
        if( (P_REV_IS_6_OR_GREATER) && (cfs_req->var.mod0.cf_srch_offset_incl) &&
           ((cfs_req->var.mod0.cf_nghbr_srch_mode == SRCH_PRI_AND_WIN) ||
           (cfs_req->var.mod0.cf_nghbr_srch_mode == SRCH_WIN_ONLY)))
        {
          packet->action.srch_cfs_nset.cf_nset[i].srch_offset_nghbr =
          cfs_req->var.mod0.add_nghbr[i].srch_offset_nghbr;
        }
        else
        {
          packet->action.srch_cfs_nset.cf_nset[i].srch_offset_nghbr = 0;
        }
        if (P_REV_IS_6_OR_GREATER && cfs_req->var.mod0.add_nghbr[i].add_pilot_rec_incl)
        {
          packet->action.srch_cfs_nset.cf_nset[i].pilot_rec_type =
            (cai_pilot_rec_type) cfs_req->var.mod0.add_nghbr[i].nghbr_pilot_rec_type;
          packet->action.srch_cfs_nset.cf_nset[i].pilot_rec =
            cfs_req->var.mod0.add_nghbr[i].add_pilot_rec;
        }
        else
        {
          packet->action.srch_cfs_nset.cf_nset[i].pilot_rec_type =
            CAI_PILOT_1X_COMMON;
        }

      }
      break;
    } // of case MCTRANS_SERVER_RXC

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid Server Token %d", server_token);
      break;
  }

} // mcctxns_cfs_nset_action_packet


/*===========================================================================

FUNCTION mcctxns_cfs_pre

DESCRIPTION
  Pre-processor for CFS_NSET transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_cfs_pre
(
  mctrans_trans_info_type * curr_trans
)
{
  mcctxns_init_cfs_trans(curr_trans);

} // mcctxns_cfs_pre

/*===========================================================================

FUNCTION mcctxns_begin_ext_rel_trans

DESCRIPTION
  Starts a Extended Release transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_ext_rel_trans
(
  qword trans_time,
  mctrans_pending_action_msg_type* msg
)
{
  uint16 trans_num;
  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_COUNT,                     // Transaction type
    MCTRANS_MODE_EXPLICIT,
    NULL,
    NULL,
    NULL
  };

  mcc_rev_chan_mask_type rev_rel_mask;
  cai_ext_chind_type ota_extchind = CAI_EXT_CHIND_UNSET;

  rev_rel_mask = mccccl_code_channel_list.gen_rev_chan_mask_erm((cai_chind_type) msg->erm.ch_ind,
                                                      ota_extchind);


  despatch_info.transaction_type = MCTRANS_EXTENDED_RELEASE;

  despatch_info.preprocessor = mcctxns_ext_rel_pre;
  despatch_info.postprocessor = mcctxns_ext_rel_post;
  despatch_info.action_packet_gen = mcctxns_ext_rel_action_packet;

  trans_num = mctrans_despatch_transaction( trans_time, &despatch_info, msg );

  M1X_MSG( DCP, LEGACY_MED,
    "Extended Release transaction despatched, trans_num %d",
    trans_num );

  (void) trans_num; /* Added to satisfy compiler warning (set but not used) */

  if (rev_rel_mask & MCC_R_PICH)
  {
    /* If pilot is released stop all SCH processing */
    mccsch_stop_sch0_trans(trans_time); // stop all sch
  }

}
/*===========================================================================

FUNCTION mcctxns_init_ext_rel_trans

DESCRIPTION
  Fills the Extended Release transaction with all the actions

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_init_ext_rel_trans
(
  mctrans_trans_info_type * curr_trans
)
{
  const caii_non_neg_srv_cfg_type *nnscr = snm_get_current_nnscr();
  mctrans_pending_action_msg_type* msg;


  mcc_fwd_chan_mask_type fwd_rel_mask;
  mcc_rev_chan_mask_type rev_rel_mask;
  cai_ext_chind_type ota_extchind = CAI_EXT_CHIND_UNSET;

  msg = mctrans_get_pending_msg(curr_trans->trans_num);

  if (msg == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No ERM found for the trans %d; no action added",
      curr_trans->trans_num);
    return;
  }

  fwd_rel_mask = mccccl_code_channel_list.gen_fwd_chan_mask_erm((cai_chind_type) msg->erm.ch_ind, ota_extchind);
  rev_rel_mask = mccccl_code_channel_list.gen_rev_chan_mask_erm((cai_chind_type) msg->erm.ch_ind, ota_extchind);

  /*  MCTRANS_CTRL_HLD_GATING_ACTION
  */
  if (rev_rel_mask & MCC_R_PICH)
  {
    /* Cont rev pilot is being released so include the control hold action for TXC */
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_CTRL_HLD_GATING_ACTION);
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_CTRL_HLD_GATING_ACTION);
  }

  /*  MCTRANS_FFPC_ACTION
  */

  /* BKM: Need to revisit for DV, use reverse for now */
  if (((fwd_rel_mask & MCC_F_FCH) && (nnscr->fpc_pri_chan == CAI_FPC_PRI_CHAN_FCH))||
      ((fwd_rel_mask & MCC_F_DCCH) && (nnscr->fpc_pri_chan == CAI_FPC_PRI_CHAN_DCCH))
     )
  {
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_FFPC_ACTION);
  }


  /*
   * MCTRANS_FFPC_ACTION
   * MCTRANS_SCR_ACTION
   * MCTRANS_RLGC_ACTION
   * MCTRANS_ASET_ACTION
  */

  /* If forward channels are changing send new FFPC action, TXC SCR and
  Active set actions to the lower layers */
  if (fwd_rel_mask & (MCC_F_FCH | MCC_F_DCCH))
  {
    /* One of the channels is released, need to stop FFPC on it */
    /* If either FCH or DCCH is being released, tell MUX to not use those channels
    by giving a new SCR action and give SRCH  a new aset */
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_FFPC_ACTION);
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_SCR_ACTION);

    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_ASET_ACTION);
  }

  /* If reverse channels are changing send TXC SCR and RLGC actions to the
  lower layers */
  if (rev_rel_mask & (MCC_R_FCH | MCC_R_DCCH))
  {
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_SCR_ACTION);
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_RLGC_ACTION);
  }


  /*  MCTRANS_GATING_ACTION
  */

  /* If DCCH is released, check if FCH 1/8 rate gating can be turned back on */
  if ((rev_rel_mask & MCC_R_DCCH) && cdma.rev_fch_gating_on)
  {
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_GATING_ACTION);
  }


  /* F-PDCH and R-CQICH being released
  */
  if (fwd_rel_mask & MCC_F_PDCH)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "F_PDCH must get released");

    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_SCR_ACTION);

    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_ASET_ACTION);

    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_SCR_ACTION);
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_RLGC_ACTION);

  }

  /* R-PDCH being released
  */
  if (rev_rel_mask & MCC_R_PDCH)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "R_PDCH not supported");
  }

}

/*===========================================================================

FUNCTION mcctxns_ext_rel_pre

DESCRIPTION
  pre-processor for Extended Release transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_ext_rel_pre
(
  mctrans_trans_info_type * curr_trans
)
{
  mcc_fwd_chan_mask_type fwd_chans_to_release;
  mcc_rev_chan_mask_type rev_chans_to_release;

  cai_ext_chind_type ota_erm_extchind = CAI_EXT_CHIND_UNSET;

  // ERM trigger this transaction so we are sure to get the ERM in the following
  // mctrans_get_pending_msg call.
  const mctrans_pending_action_msg_type* pending_msg =
                                   mctrans_get_pending_msg( curr_trans->trans_num );

  /* Initialize transaction data
  */
  mcctxns_init_ext_rel_trans(curr_trans);


  if (pending_msg != NULL)
  {
    /* store the pilot gating rate into SNM */
    if (pending_msg->erm.gating_rate_incl)
    {
      snm_update_pilot_gating_rate
        ((cai_pilot_gating_rate_type)pending_msg->erm.pilot_gating_rate);
    }

    /* update the stored channel mask */
    mccccl_code_channel_list.set_chan_mask_erm
    (
      (cai_chind_type) pending_msg->erm.ch_ind,
      ota_erm_extchind
    );

    /* Operate on the internal format */
    fwd_chans_to_release = mccccl_code_channel_list.gen_fwd_chan_mask_erm (
      (cai_chind_type) pending_msg->erm.ch_ind, ota_erm_extchind );

    rev_chans_to_release = mccccl_code_channel_list.gen_rev_chan_mask_erm (
      (cai_chind_type) pending_msg->erm.ch_ind, ota_erm_extchind );

    #ifdef FEATURE_IS2000_REL_A_SVD
    if ((rev_chans_to_release & MCC_R_PICH) && (rev_chans_to_release & MCC_R_FCH ))
    {
      /* Remember that FCH has to be restored when MS goes back active
         through a RAM message or ESCAM */
      mccccl_code_channel_list.set_fch_to_be_restored(TRUE);
    }
    #endif

    /* Update FPC_PRI_CHAN */
    /* BKM: Must revisit for DV, can have F-CPCCCH and no FPC_PRI_CHAN */
    if (fwd_chans_to_release & MCC_F_FCH)
    {
      snm_update_fpc_pri_chan(CAI_FPC_PRI_CHAN_DCCH);  /*lint !e641 */
    }
    else if ( fwd_chans_to_release & MCC_F_DCCH )
    {
      snm_update_fpc_pri_chan(CAI_FPC_PRI_CHAN_FCH);  /*lint !e641 */
    }
  }

} // mcctxns_ext_rel_pre

/*===========================================================================

FUNCTION mcctxns_ext_rel_action_packet

DESCRIPTION
  This function fills in the action packets actions in Ext Rel transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_ext_rel_action_packet
(
  mctrans_trans_info_type * curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
)
{
  mcc_fwd_chan_mask_type fwd_chans_to_release = MCC_F_RESERVED;
  mcc_rev_chan_mask_type rev_chans_to_release = MCC_R_RESERVED;
  cai_ext_chind_type ota_erm_extchind;

  const mctrans_pending_action_msg_type* pending_msg = mctrans_get_pending_msg(curr_trans->trans_num);

  /* ptr to retrieve the non negotiable srv config records */
  const caii_non_neg_srv_cfg_type *nnscr = snm_get_current_nnscr();

  ota_erm_extchind = CAI_EXT_CHIND_UNSET;

  if (pending_msg != NULL)
  {
    /* Have already stored the new channel mask, but we want to determine
       which of the channels are being released. */
    fwd_chans_to_release = mccccl_code_channel_list.gen_fwd_chan_mask_erm (
      (cai_chind_type) pending_msg->erm.ch_ind,
      ota_erm_extchind );

    rev_chans_to_release = mccccl_code_channel_list.gen_rev_chan_mask_erm (
      (cai_chind_type) pending_msg->erm.ch_ind,
      ota_erm_extchind );
  }

  // Stamp action type in action packet
  packet->action_type = action;

  switch ( server_token )
  {
    case MCTRANS_SERVER_RXC:
    {
      switch( action)
      {
        case MCTRANS_FFPC_ACTION:
        {
          mcctxns_init_ffpc( &packet->action.ffpc );
          /* if either DCCH or FCH is off, we need to set FPC_PRI_CHAN to the
             remaining channel */
          packet->action.ffpc.fpc_pri_chan.included = TRUE;
          if (mccccl_code_channel_list.fwd_fundicated_chan_is_active())
          {
            packet->action.ffpc.fpc_pri_chan.value = nnscr->fpc_pri_chan;
          }
          else
          {
            packet->action.ffpc.fpc_pri_chan.value = CAI_FPC_PRI_CHAN_CPCCH;
            packet->action.ffpc.fpc_mode.included = TRUE;
            packet->action.ffpc.fpc_mode.value = CAI_FPC_MODE_RESERVED;
          }

          if ( fwd_chans_to_release & MCC_F_FCH )
          {
            packet->action.ffpc.fch_ffpc_op.mode = MCTRANS_FFPC_STOP;
          }
          else if ( fwd_chans_to_release & MCC_F_DCCH)
          {
            packet->action.ffpc.dcch_ffpc_op.mode = MCTRANS_FFPC_STOP;
          }

          break;
        }

        case MCTRANS_SCR_ACTION:
        {
          const caii_srv_cfg_type  *config = snm_get_current_config();
          mcctxns_prepare_scr_action_packet(MCTRANS_SERVER_RXC, packet , config, nnscr);
          break;
        }

        case MCTRANS_CTRL_HLD_GATING_ACTION :
        {
          packet->action.rxc_ctrl_hld_gating.gating_rate =
            (cai_pilot_gating_rate_type) nnscr->pilot_gate_rate;
          // Use BS assigned rev power control delay if no gating, otherwise use
          // appropriate delay for rate
          packet->action.rxc_ctrl_hld_gating.rev_pwr_control_delay =
            ( nnscr->pilot_gate_rate == (byte)CAI_GATING_RATE_ONE ||
              nnscr->pilot_gate_rate > (byte)CAI_GATING_RATE_QUARTER) ?
            cdma.rev_pwr_cntl_delay : gating_rate_to_pwr_delay[nnscr->pilot_gate_rate];
          break;
        }

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action );
          break;
      }
      break;
    }

    case MCTRANS_SERVER_TXC :
    {
      switch (action)
      {
        case MCTRANS_SCR_ACTION:
        {
          const caii_srv_cfg_type  *config = snm_get_current_config();
          mcctxns_prepare_scr_action_packet(MCTRANS_SERVER_TXC, packet , config, nnscr);
          break;
        }

        case MCTRANS_RLGC_ACTION:
        {
          pending_msg = mctrans_get_pending_msg( curr_trans->trans_num );
          mcctxns_init_rlgc( &packet->action.rlgc );

          // RLGC needs to be notified if channel configuration is changed.
          if ( rev_chans_to_release & MCC_R_FCH )
          {
            packet->action.rlgc.fch_rlgc_op.mode = MCTRANS_RLGC_STOP;
          }
          else if ( rev_chans_to_release & MCC_R_DCCH )
          {
            packet->action.rlgc.dcch_rlgc_op.mode = MCTRANS_RLGC_STOP;
          }
          break;
        }

        case MCTRANS_CTRL_HLD_GATING_ACTION:
        {
          packet->action.txc_ctrl_hld_gating.gating_rate =
            (cai_pilot_gating_rate_type) nnscr->pilot_gate_rate;
          break;
        }

        case MCTRANS_GATING_ACTION:
        {
          /* Gating can be turned on if we are operaing no R-SCH & are in RC 3
          or higher mode */
          const caii_srv_cfg_type  *config = snm_get_current_config();

          packet->action.txc_gating.rev_fch_gating_on =
            !mccsch_sch_is_active(MCCSCH_REVERSE,0) &&
            config->for_fch_rc >= CAI_RC_3 && cdma.rev_fch_gating_on; /*lint !e641 */
          break;
        }

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action );
          break;
      }
      break;
    }

#ifdef FEATURE_IS2000_REL_A
    case MCTRANS_SERVER_SRCH :
    {
      switch (action)
      {
        case MCTRANS_ASET_ACTION:
        {
          packet->action.srch_aset.tc_aset.aset_cnt = mccccl_code_channel_list.
                  generate_aset_default_sups(packet->action.srch_aset.tc_aset.aset,
                                             (mccccl_sup_state_type *) NULL);
          packet->action.srch_aset.tc_aset.cdma_freq =
            (srch_rx_chan_type) cdma.cdmach;
          packet->action.srch_aset.tc_aset.band_class =
            (srch_rx_band_type) cdma.band_class;
          packet->action.srch_aset.tc_aset.is_hho = FALSE;

          break;
        }

        case MCTRANS_FFPC_ACTION:
        {
          nnscr = snm_get_current_nnscr();
          packet->action.srch_ffpc.fpc_pri_chan =
            (cai_fpc_pri_chan_type) nnscr->fpc_pri_chan;
          break;
        }

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action );
          break;
      }
      break;
    }
#endif

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Incorrect server token %d",
        server_token);
      break;
  }

}

/*===========================================================================

FUNCTION mcctxns_ext_rel_post

DESCRIPTION
  Post-processor for Extended Release transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_ext_rel_post
(
  mctrans_trans_info_type * curr_trans
)
{
  /* release the msg */
  mctrans_release_pending_msg(curr_trans->trans_num );

}

/*===========================================================================

FUNCTION mcctxns_begin_active_trans

DESCRIPTION
  Starts a control hold to active transaction.
  This function can be trigger by either RAM or ESCAM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_active_trans
(
  qword trans_time,
  mctrans_pending_action_msg_type* msg
)
{
  uint16 trans_num;
  mctrans_pending_action_msg_type* stored_msg = NULL;
  const caii_non_neg_srv_cfg_type *nnscr = snm_get_current_nnscr();

  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_COUNT,                     // Transaction type
    MCTRANS_MODE_EXPLICIT,
    NULL,
    NULL,
    NULL
  };


  despatch_info.transaction_type = MCTRANS_ACTIVE;

  despatch_info.preprocessor = mcctxns_active_pre;
  despatch_info.postprocessor = mcctxns_active_post;
  despatch_info.action_packet_gen = mcctxns_active_action_packet;


  if (
       (msg != NULL) &&
       (msg->ram.fpc_pri_chan != nnscr->fpc_pri_chan)
     )
  {
    stored_msg = msg;
  }

  trans_num = mctrans_despatch_transaction( trans_time, &despatch_info, stored_msg );

  M1X_MSG( DCP, LEGACY_MED,
    "Control Hold to active transaction despatched, trans_num %d",
    trans_num );

  (void) trans_num; /* Added to satisfy compiler warning (set but not used) */

}

/*===========================================================================

FUNCTION mcctxns_init_active_trans

DESCRIPTION
  Fills the transaction with all the actions

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_init_active_trans
(
  mctrans_trans_info_type * curr_trans
)
{
  const caii_non_neg_srv_cfg_type *nnscr = snm_get_current_nnscr();
  mctrans_pending_action_msg_type* msg =
              mctrans_get_pending_msg(curr_trans->trans_num);

  boolean rxc_ffpc_action_incl = FALSE;

  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_CTRL_HLD_GATING_ACTION);
  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_CTRL_HLD_GATING_ACTION);

#ifdef FEATURE_IS2000_REL_A_SVD
  if (mccccl_code_channel_list.get_fch_to_be_restored())
  {
    /* We have to restore FCH , so just include all lower layer actions that
    would help with that */
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_ASET_ACTION);
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_SCR_ACTION);
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_SCR_ACTION);
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_RLGC_ACTION);

    /* Need to include FFPC action to start power control on FCH
       again
    */
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_FFPC_ACTION);
    rxc_ffpc_action_incl = TRUE;

  }
#endif

  /* only need the FFPC action if FFPC pri chan changes */
  /* Note: if ESCAM triggers this function, msg is not included.*/
  if ((msg != NULL) &&
      (msg->ram.fpc_pri_chan != nnscr->fpc_pri_chan))
  {
    if (!rxc_ffpc_action_incl)
    {
      /* Need to add FPPC action to RXC, If already not included */
      mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_FFPC_ACTION);
    }


#ifdef FEATURE_IS2000_REL_A
    // Note that active_despatch_info already set up for this action.  Just need to add the
    // server token.
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_FFPC_ACTION);
#endif
  }

}

/*===========================================================================

FUNCTION mcctxns_active_pre

DESCRIPTION
  Pre-processor for control hold to active transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_active_pre
(
  mctrans_trans_info_type *   curr_trans
)
{
  const mctrans_pending_action_msg_type* pending_msg =
             mctrans_get_pending_msg( curr_trans->trans_num );
  const caii_non_neg_srv_cfg_type *nnscr = snm_get_current_nnscr();
  boolean restore_fch = FALSE;

  // Initialize transaction data.
  mcctxns_init_active_trans(curr_trans);

  /* only update FFPC pri chan if FFPC pri chan changes */
  /* both RAM and ESCAM can cause this transcation, but only RAM need FFPC action */
  if ((pending_msg != NULL) && (pending_msg->gen.msg_type == CAI_RES_ALLOC_MSG) &&
      (pending_msg->ram.fpc_pri_chan != nnscr->fpc_pri_chan))
  {
    snm_update_fpc_pri_chan(pending_msg->ram.fpc_pri_chan);
  }

#ifdef FEATURE_IS2000_REL_A_SVD
  if (mccccl_code_channel_list.get_fch_to_be_restored())
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "FCH is being restored");
    restore_fch = TRUE;
    mccccl_code_channel_list.set_fch_to_be_restored(FALSE);
  }
#endif

  /* update MCCCCL to indicate that MS has exited control hold */
  mccccl_code_channel_list.exit_control_hold(restore_fch);

}

/*===========================================================================

FUNCTION mcctxns_active_action_packet

DESCRIPTION
  This function fills in the action packets actions in control hold transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_active_action_packet
(
  mctrans_trans_info_type *   curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
)
{
#ifdef FEATURE_IS2000_REL_A
  const mctrans_pending_action_msg_type* pending_msg =
                         mctrans_get_pending_msg( curr_trans->trans_num );
#endif

  // Stamp action type in action packet
  packet->action_type = action;

  switch ( server_token )
  {
    case MCTRANS_SERVER_RXC:
    {
      switch ( action )
      {
        case MCTRANS_CTRL_HLD_GATING_ACTION :
        {
          packet->action.rxc_ctrl_hld_gating.gating_rate = CAI_GATING_RATE_NONE;
          packet->action.rxc_ctrl_hld_gating.rev_pwr_control_delay = cdma.rev_pwr_cntl_delay;
          break;
        }

#ifdef FEATURE_IS2000_REL_A
        // This part is not needed until both DCCH and FCH are supported simultaneously
        case MCTRANS_FFPC_ACTION :
        {
          const caii_srv_cfg_type  *config = snm_get_current_config();
          mcc_fwd_chan_mask_type fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask();
          mcctxns_init_ffpc( &packet->action.ffpc );

          /* If FCH is being restored, start FFPC on it again */
          if (fwd_chan_mask & MCC_F_FCH)
          {
            packet->action.ffpc.fch_ffpc_op.mode = MCTRANS_FFPC_START;
            // We have already verified that FCH is included since we are here
            packet->action.ffpc.fch_ffpc_op.rc =
              (cai_radio_config_type) config->for_fch_rc;
          }

          if (pending_msg != NULL)
          {
            packet->action.ffpc.fpc_pri_chan.included = TRUE;
            if (mccccl_code_channel_list.fwd_fundicated_chan_is_active())
            {
              packet->action.ffpc.fpc_pri_chan.value = pending_msg->ram.fpc_pri_chan;
            }
            else
            {
              packet->action.ffpc.fpc_pri_chan.value = CAI_FPC_PRI_CHAN_CPCCH; /*lint !e641 */
              packet->action.ffpc.fpc_mode.included = TRUE;
              packet->action.ffpc.fpc_mode.value = CAI_FPC_MODE_RESERVED;
            }
          }
          break;
        }

        case MCTRANS_SCR_ACTION:
        {
          const caii_srv_cfg_type  *config = snm_get_current_config();
          const caii_non_neg_srv_cfg_type  *nnscr = snm_get_current_nnscr();
          mcctxns_prepare_scr_action_packet(MCTRANS_SERVER_RXC, packet , config, nnscr);
          break;
        }
#endif //FEATURE_IS2000_REL_A

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action );
          break;
      }
      break;
    }  // of case MCTRANS_SERVER_RXC

    case MCTRANS_SERVER_TXC :
      switch ( action )
      {
        case MCTRANS_CTRL_HLD_GATING_ACTION :
          packet->action.txc_ctrl_hld_gating.gating_rate = CAI_GATING_RATE_NONE;
          break;
#ifdef FEATURE_IS2000_REL_A
        case MCTRANS_SCR_ACTION:
        {
          const caii_srv_cfg_type  *config = snm_get_current_config();
          const caii_non_neg_srv_cfg_type  *nnscr = snm_get_current_nnscr();
          mcctxns_prepare_scr_action_packet(MCTRANS_SERVER_TXC, packet , config, nnscr);
          break;
        }
        case MCTRANS_RLGC_ACTION:
        {
          const caii_srv_cfg_type  *config = snm_get_current_config();
          mcc_rev_chan_mask_type rev_chan_mask = mccccl_code_channel_list.get_rev_chan_mask();
          mcctxns_init_rlgc( &packet->action.rlgc );
          // RLGC needs to be notified if FCH is being added
          if ( rev_chan_mask & MCC_R_FCH )
          {
            packet->action.rlgc.fch_rlgc_op.mode = MCTRANS_RLGC_START;
            packet->action.rlgc.fch_rlgc_op.rc = (cai_radio_config_type) config->for_fch_rc;
          }
          break;
        }
#endif //FEATURE_IS2000_REL_A

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action );
          break;
      }
      break;

#ifdef FEATURE_IS2000_REL_A
    case MCTRANS_SERVER_SRCH:
      switch ( action )
      {
        case MCTRANS_FFPC_ACTION:
          //If we are here, pending msg must be non-NULL
          if (pending_msg != NULL)
          {
            if (mccccl_code_channel_list.fwd_fundicated_chan_is_active())
            {
              packet->action.srch_ffpc.fpc_pri_chan =
                (cai_fpc_pri_chan_type) pending_msg->ram.fpc_pri_chan;
            }
            else
            {
              packet->action.srch_ffpc.fpc_pri_chan = CAI_FPC_PRI_CHAN_CPCCH;
            }
          }
          else
          {
            const caii_non_neg_srv_cfg_type *nnscr = snm_get_current_nnscr();
            packet->action.srch_ffpc.fpc_pri_chan =
              (cai_fpc_pri_chan_type) nnscr->fpc_pri_chan;
          }

          break;

        case MCTRANS_ASET_ACTION:
        {
          packet->action.srch_aset.tc_aset.aset_cnt = mccccl_code_channel_list.
                  generate_aset_default_sups(packet->action.srch_aset.tc_aset.aset,
                                             (mccccl_sup_state_type *) NULL);
          packet->action.srch_aset.tc_aset.cdma_freq =
            (srch_rx_chan_type) cdma.cdmach;
          packet->action.srch_aset.tc_aset.band_class =
            (srch_rx_band_type) cdma.band_class;
          packet->action.srch_aset.tc_aset.is_hho = FALSE;

          break;
        }

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action);
          break;
      }
      break;
#endif

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Incorrect server token %d",
        server_token);
      break;
  }

}

/*===========================================================================

FUNCTION mcctxns_active_post

DESCRIPTION
  Post-processor for control hold to active transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_active_post
(
  mctrans_trans_info_type *   curr_trans
)
{
  /* release the msg */
  mctrans_release_pending_msg(curr_trans->trans_num );
}

/*===========================================================================

FUNCTION mcctxns_begin_hho_begin_trans

DESCRIPTION
  Starts a "HHO Begin" transaction.
  This is the transaction that gives ASET to SRCH for a HHO and tells
  RXC (& SRCH in case of RIF-HHO) to start looking for good frame.
  This txn is scheduled 1 frame after the action time of the actual
  HO transaction

DEPENDENCIES
 The actual "HO transaction that sets up params for this trans shd
 have already started. The assumption is the RF is fully tuned to the
 new freq by the time we act on this txn.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_hho_begin_trans
(
  qword trans_time
)
{
  mctrans_server_token_type server_order[MCTRANS_SERVER_COUNT];
  uint16 trans_num;
  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_COUNT,                     // Transaction type
    MCTRANS_MODE_EXPLICIT,
    NULL,
    NULL,
    NULL
  };

  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry == MCC_CSFB)
  {
    /* In CSFB mode, we dont need to wait for action time to process
     * the transaction.
     */
    m1x_time_get_20ms_frame_offset_time(trans_time);
    despatch_info.trans_mode = MCTRANS_MODE_IMMEDIATE;
  }
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

  despatch_info.transaction_type = MCTRANS_HHO_BEGIN;

  despatch_info.preprocessor = mcctxns_hho_begin_pre;
  despatch_info.postprocessor = mcctxns_generic_post;
  despatch_info.action_packet_gen = mcctxns_hho_begin_action_packet;

  server_order[0] = MCTRANS_SERVER_RXC;
  server_order[1] = MCTRANS_SERVER_SRCH;
  server_order[2] = MCTRANS_SERVER_TXC;

  trans_num = mctrans_despatch_ordered_transaction( trans_time,
                          &despatch_info,
                          0,
                          server_order
                          );

  M1X_MSG( DCP, LEGACY_MED,
    "HHO Begin transaction despatched, trans_num %d",
    trans_num );

  (void) trans_num; /* Added to satisfy compiler warning (set but not used) */
}


/*===========================================================================

FUNCTION mcctxns_init_hho_begin_trans

DESCRIPTION
  Inits actions of a HHO Begin transaction.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_init_hho_begin_trans
(
  mctrans_trans_info_type * curr_trans
)
{
  // Obtain pointer to mcctcho_ho_data...
  const mcctcho_ho_data_type *ho_data = mcctcho_get_ho_data();
  /*  RXC always have the HHO Begin action */
  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_HHO_BEGIN_ACTION);

  if (ho_data->ho_msg.extra_parms && ho_data->ho_msg.extra.return_if_ho_fail)
  {
    /* If it is a RIF HO, we also need the HHO Begin action for SRCH for diff rx pwr
       measurements which is not needed for the non-RIF case */
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_HHO_BEGIN_ACTION);
  }
  /* SRCH always needs to have the ASET action for HHO and this shd be present AFTER
   * THE HHO_BEGIN action for SRCH*/
  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_ASET_ACTION);

  #ifdef FEATURE_IS2000_1X_ADV
  if(!mcc_is_ho_internal_to_non_1xa_rc)
  {
    /* SRCH needs the radio config parameters for all the pilots in active set */
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_RCP_ACTION);
  }
  #endif /* FEATURE_IS2000_1X_ADV */

}

/*===========================================================================

FUNCTION mcctxns_hho_begin_pre

DESCRIPTION
  Pre-processor for HHO Begin (looking for good frames) transaction

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_hho_begin_pre
(
  mctrans_trans_info_type *   curr_trans
)
{
  // Initialize transaction data.
  mcctxns_init_hho_begin_trans(curr_trans);

}

/*===========================================================================

FUNCTION mcctxns_hho_begin_action_packet

DESCRIPTION
  Prepares the action packets for HHO BEGIN transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_hho_begin_action_packet
(
  mctrans_trans_info_type *   curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
)
{
  // Stamp action type in action packet
  packet->action_type = action;


  switch ( server_token )
  {
    case MCTRANS_SERVER_RXC:
    {
      // Obtain pointer to mcctcho_ho_data...
      const mcctcho_ho_data_type *ho_data = mcctcho_get_ho_data();

      if (action != MCTRANS_HHO_BEGIN_ACTION )
      {
        ERR_FATAL ("Wrong action type %d", (int) action,0,0);
      }

      /* Put HHO return-if-fail indicator */
      packet->action.rxc_hho_begin.rif_ind =
        ho_data->ho_msg.extra_parms && ho_data->ho_msg.extra.return_if_ho_fail;

      /* Put HHO Time out value here */
      packet->action.rxc_hho_begin.hho_timeout = cand_freq_srch_parm.tf_wait_time;
      break;
    }

    case MCTRANS_SERVER_SRCH:
    {
      // Obtain pointer to mcctcho_ho_data...
      const mcctcho_ho_data_type *ho_data = mcctcho_get_ho_data();
      /* The only actions possible are ASET & HHO_Begin (No payload) */
      switch (action)
      {
        case MCTRANS_ASET_ACTION:
        {
          /* Set band class and frequency */
          packet->action.srch_aset.tc_aset.cdma_freq =
            (srch_rx_chan_type) ho_data->ho_msg.extra.cdma_freq;
          packet->action.srch_aset.tc_aset.band_class =
            (srch_rx_band_type) ho_data->ho_msg.extra.band_class;

          /* This is a hard-handoff, set the HHO flag */
          packet->action.srch_aset.tc_aset.is_hho = TRUE;

          /* Return value not needed below */
          (void)mcctcho_generate_aset( &packet->action.srch_aset.tc_aset.aset_cnt,
                                       packet->action.srch_aset.tc_aset.aset
                                     );
          break;
        }

        #ifdef FEATURE_IS2000_1X_ADV
        case MCTRANS_RCP_ACTION:
        {
          mcc_get_pilots_rc_param_recs((cai_rcp_srch_type*)&packet->action.srch_rcp );
          break;
        }
        #endif /* FEATURE_IS2000_1X_ADV */

        default:
        {
          if (action != MCTRANS_HHO_BEGIN_ACTION )
          {
            ERR_FATAL ("Wrong action type %d", (int) action,0,0);
          }
          break;
        }
      }
      /* No Payload for SRCH */
      break;
    }
    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid Server Token %d", server_token );
      break;
  }

}


/*===========================================================================

FUNCTION mcctxns_begin_tx_on_off_trans

DESCRIPTION
  Starts a "TX on or off" transaction

DEPENDENCIES
  This can transmitter to be turned on or off; This txn is always
  "immediate". If TX on/off is to be synced up with anything else
  it will be an action under a different txn (Eg: TX_ON_OFF_ACTION as part of
  HO txn )


RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_tx_on_off_trans
(
  boolean tx_on_off
)
{
  /* For now, this is always an immediate transaction */
  uint16 trans_num;
  qword trans_time;
  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_TX_ON_OFF,
    MCTRANS_MODE_IMMEDIATE,
    mcctxns_tx_on_off_pre,
    mcctxns_generic_post,
    mcctxns_tx_on_off_action_packet
  };

  m1x_time_get_20ms_frame_offset_time(trans_time);

  /* Remember  if TX is to be turned off or on , This value is set to
     cdma.tc_tx_on in the pre-processor */
  cdma.tc_tx_to_be_on = tx_on_off;

  trans_num = mctrans_despatch_transaction( trans_time, &despatch_info, 0 );

  M1X_MSG( DCP, LEGACY_MED,
    "TX on/off txn despatched %d",
    trans_num );

  (void) trans_num; /* Added to satisfy compiler warning (set but not used) */
}

/*===========================================================================

FUNCTION mcctxns_init_tx_on_off_trans

DESCRIPTION
  Inits actions of a TX (on or Off) action

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_init_tx_on_off_trans
(
  mctrans_trans_info_type * curr_trans
)
{
  /*  Both TXC & SRCH shd know if TX is to be turned ON or Off */
  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_TX_ON_OFF_ACTION);
  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_TX_ON_OFF_ACTION);

}

/*===========================================================================

FUNCTION mcctxns_tx_on_off_pre

DESCRIPTION
  Pre-processor for TX transaction ( To turn TX on or off )

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_tx_on_off_pre
(
  mctrans_trans_info_type *   curr_trans
)
{
  /* The TX "on or off" transaction is used whenever TX on or off
     is to be issued w/o being part of any other transaction , specifically
     for a fade (To issue TX off) and when we get enough good frames after
     a CFS or GPS or a HHO which does not involve a switch */
  /* cdma.tc_tx_to_be_on is looked at only for a TX transaction and not for
    a TX action for which we would know if TX is to be turned
    on or off based on the TXN it is a part of */

  // Initialize transaction data.
  mcctxns_init_tx_on_off_trans(curr_trans);

  /* Update the value of "tc_tx_on" with the new value */
  cdma.tc_tx_on = cdma.tc_tx_to_be_on;

}

/*===========================================================================

FUNCTION mcctxns_tx_on_off_action_packet

DESCRIPTION
  Prepares the action packets for TX (ON or Off ) transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_tx_on_off_action_packet
(
  mctrans_trans_info_type *   curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
)
{
  // Stamp action type in action packet
  packet->action_type = action;

  if (action != MCTRANS_TX_ON_OFF_ACTION)
  {
    ERR_FATAL ("Wrong action type %d", (int) action,0,0);
  }

  switch ( server_token )
  {
    case MCTRANS_SERVER_TXC:
    {
      packet->action.txc_tx_on_off.tx_on = cdma.tc_tx_to_be_on;
      /* Put TX on or off indication here */
      break;
    }

    case MCTRANS_SERVER_SRCH:
    {
      packet->action.srch_tx_on_off.tx_on = cdma.tc_tx_to_be_on;
      /* Put TX on or off indication here */
      break;
    }
    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid Server Token %d", server_token );
      break;
  }

}


/*===========================================================================

FUNCTION mcctxns_begin_return_trans

DESCRIPTION
  Starts a "Return" transaction after a failed HHO

DEPENDENCIES
 The actual "HO transaction that sets up params for this trans shd
 have already started. The assumption is the RF is fully tuned to the
 new freq by the time we act on this txn.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_return_trans
(
  qword trans_time
)
{
  mctrans_server_token_type server_order[MCTRANS_SERVER_COUNT];
  uint16 trans_num;
  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_RETURN,
    MCTRANS_MODE_EXPLICIT,
    mcctxns_return_pre,
    mcctxns_generic_post,
    mcctxns_return_action_packet
  };

  /* It is important that this be ordered for CFS and even HHO return, we can only tune
     before the tx is on, so it is important to tell RX abt this AFTER
     the tuning is all done (Just in case it decides to turn TX back on right away
     (which can happen if we have tuned away for < 12 frames*/

  server_order[0] = MCTRANS_SERVER_SRCH;
  server_order[1] = MCTRANS_SERVER_RXC;
  server_order[2] = MCTRANS_SERVER_TXC;
  trans_num = mctrans_despatch_ordered_transaction( trans_time,
                          &despatch_info,
                          0,
                          server_order
                          );
  M1X_MSG( DCP, LEGACY_MED,
    "Return transaction despatched %d",
    trans_num );

  (void) trans_num; /* Added to satisfy compiler warning (set but not used) */
}

/*===========================================================================

FUNCTION mcctxns_init_return_trans

DESCRIPTION
  Initializes actions for a return transaction

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_init_return_trans
(
  mctrans_trans_info_type * curr_trans
)
{
  uint8 i,j;
  const mcctcho_ho_data_type *ho_data = mcctcho_get_ho_data();

  if (ho_data->hard_handoff_in_progress)
  {
    /* 1xTA: Update HHO progress state */
    cdma.tc_data_ta.hho_prog = HHO_RIF_RETURN_TXN_IN_PROG;
    
    /* ASET shd be part of SRCH txn, it wouldn't have been part of HO txn,
       (Only HHO Begin txn) , so add it here explicitly */
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_ASET_ACTION);

    #ifdef FEATURE_IS2000_1X_ADV
    if(!mcc_is_ho_internal_to_non_1xa_rc)
    {
      /* SRCH and MUX need to restore Radio Configuration Parameters */
      mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_RCP_ACTION);
      mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_RCP_ACTION);
      mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_RCP_ACTION);
    }
    #endif /* FEATURE_IS2000_1X_ADV */

    /* Now add all the other actions that were part of the HO txn */
    for (i=0; i<MCTRANS_SERVER_COUNT; i++)  /*lint !e641 */
    {
      for (j=0; j<MCTRANS_MAX_ACTION_IN_TRANS; j++)
      {
        if (ho_data->server[i].actions[j] == MCTRANS_MAX_ACTION )
        {
          /* All done with actions for this server */
          break;
        }
        else
        {
          /* TX on or off action is not needed for a return transaction as this
             cannot change the TX off/on state anyway ; TX should be off here */
          if (ho_data->server[i].actions[j] != MCTRANS_TX_ON_OFF_ACTION )
          {
            mctrans_add_action_to_server(curr_trans,
                                         (mctrans_server_token_type) i, ho_data->server[i].actions[j]);
          }

        }
      }

    }
    /* Return action shd be part of all servers */
    /* Note : Return action shd be AFTER all other actions */
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_RETURN_ACTION);
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_RETURN_ACTION);
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_RETURN_ACTION);
  }
  else /* Have to differentiate b/w CFS & GPS here */
  {
    /* CFS needs these actions for SRCH */
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_ASET_ACTION);
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_RETURN_ACTION);
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_RETURN_ACTION);
  }

}

/*===========================================================================

FUNCTION mcctxns_return_pre

DESCRIPTION
  Pre-Processor for the return txn

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_return_pre
(
  mctrans_trans_info_type * curr_trans
)
{
  mcctxns_init_return_trans(curr_trans);

}

/*===========================================================================

FUNCTION mcctxns_return_action_packet

DESCRIPTION
  Prepares the action packets for RETURN transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_return_action_packet
(
  mctrans_trans_info_type *   curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
)
{
  const mcctcho_ho_data_type *ho_data = mcctcho_get_ho_data();
  // Obtain pointer to configuration containing the RC to use after the HO.  If
  // the ?HDM does not contain SCR, then it is the current SCR; otherwise, use
  // pending SCR.
  const caii_srv_cfg_type* config;
  const caii_non_neg_srv_cfg_type* nn_config;

  mcc_fwd_chan_mask_type ho_fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask();
  mcc_rev_chan_mask_type ho_rev_chan_mask = mccccl_code_channel_list.get_rev_chan_mask();

  config = snm_get_current_config();
  nn_config = snm_get_current_nnscr();

  // Stamp action type in action packet
  packet->action_type = action;

  switch ( server_token )
  {
    case MCTRANS_SERVER_SRCH :
    {
      switch (action)
      {
        case MCTRANS_HO_ACTION:
        {
          packet->action.srch_ho.hho_ind = FALSE;
          packet->action.srch_ho.rif_ind = FALSE;

          // Fill in search parameters if included
          if ( ho_data->ho_msg.srch_inc )
          {
            packet->action.srch_ho.srch_parms.included = TRUE;
            /* Fill in the restored srch param values from mcc_ho_real*/
            packet->action.srch_ho.srch_parms.values.add_intercept=
              mcc_ho_real.add_intercept;
            packet->action.srch_ho.srch_parms.values.drop_intercept =
              mcc_ho_real.drop_intercept;
            packet->action.srch_ho.srch_parms.values.soft_slope =
              mcc_ho_real.soft_slope;
            packet->action.srch_ho.srch_parms.values.srch_win_a =
              mcc_ho_real.win_a;
            packet->action.srch_ho.srch_parms.values.srch_win_n =
              mcc_ho_real.win_n;
            packet->action.srch_ho.srch_parms.values.srch_win_r =
              mcc_ho_real.win_r;
            packet->action.srch_ho.srch_parms.values.t_add =
              mcc_ho_real.t_add;
            packet->action.srch_ho.srch_parms.values.t_comp =
              mcc_ho_real.t_comp;
            packet->action.srch_ho.srch_parms.values.t_drop =
              mcc_ho_real.t_drop;
            packet->action.srch_ho.srch_parms.values.t_tdrop =
              mcc_ho_real.t_tdrop;
          }
          else
          {
            packet->action.srch_ho.srch_parms.included = FALSE;
          }

          break;
        }

        #if defined (FEATURE_IS2000_REL_A)
        case MCTRANS_FFPC_ACTION:
        {
          packet->action.srch_ffpc.fpc_pri_chan =
            (cai_fpc_pri_chan_type) nn_config->fpc_pri_chan;

          break;
        }
        #endif /* FEATURE_IS2000_REL_A */

        case MCTRANS_LCM_ACTION:
        {
          mcctcsup_get_long_code_mask(packet->action.srch_lcm.long_code_mask);
          break;
        }

        case MCTRANS_ASET_ACTION:
        {
          mccccl_sup_state_type sup_state;
          packet->action.srch_aset.tc_aset.aset_cnt = mccccl_code_channel_list.
                  generate_aset_default_sups(packet->action.srch_aset.tc_aset.aset,
                                              &sup_state);
          packet->action.srch_aset.tc_aset.cdma_freq =
             (srch_rx_chan_type) cdma.cdmach;
          packet->action.srch_aset.tc_aset.band_class =
            (srch_rx_band_type) cdma.band_class;
          packet->action.srch_aset.tc_aset.is_hho = FALSE;
          break;
        }

        #ifdef FEATURE_IS2000_1X_ADV
        case MCTRANS_RCP_ACTION:
        {
          mcc_get_pilots_rc_param_recs((cai_rcp_srch_type*)&packet->action.srch_rcp);
          break;
        }
        #endif /* FEATURE_IS2000_1X_ADV */

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action );
          break;

      }
      break;
    }
    case MCTRANS_SERVER_RXC :
    {
      switch ( action )
      {
        case MCTRANS_SCR_ACTION :
          mcctxns_prepare_scr_action_packet(MCTRANS_SERVER_RXC, packet , config, nn_config);
          /* If it is a HO related SCR action, then we do not want SCR function
          to change the rev pwr cntl delay value, This is handled by the HO action */
          packet->action.rxc_scr.rev_pwr_cntl_delay.included = FALSE;
          break;

        case MCTRANS_HO_ACTION :
          /* This is a return, so mark HHO as FALSE */
          packet->action.rxc_ho.hho_ind = FALSE;
          packet->action.rxc_ho.rif_ind = FALSE;

          packet->action.rxc_ho.reset_fpc = FALSE;
          packet->action.rxc_ho.rev_pwr_cntl_delay.included =
                    ho_data->ho_msg.rev_pwr_cntl_delay_incl;
          if (ho_data->ho_msg.rev_pwr_cntl_delay_incl)
          {
            if (ho_data->high_rc_before_ho)
            {
              packet->action.rxc_ho.rev_pwr_cntl_delay.value =
                cdma.rev_pwr_cntl_delay;
            }
            else
            {
              /* After "Return" we are going to be in an old RC, just use the
                 default value */
              packet->action.rxc_ho.rev_pwr_cntl_delay.value =
                CAI_DEFAULT_REV_PWR_CNTL_DELAY;
            }
          }
          else
          {
            /* Okay this HO msg did not have this value included but our RC config
            could have changed to warrant the change of this value, check for that */
            if (ho_data->high_rc_after_ho != ho_data->high_rc_before_ho)
            {
              packet->action.rxc_ho.rev_pwr_cntl_delay.included = TRUE;
              packet->action.rxc_ho.rev_pwr_cntl_delay.value = (ho_data->high_rc_before_ho)?
                cdma.rev_pwr_cntl_delay:CAI_DEFAULT_REV_PWR_CNTL_DELAY;
            }
          }
          #ifdef FEATURE_IS2000_REL_A
          packet->action.rxc_ho.p_rev_in_use = mc_get_p_rev_in_use();
          #endif

          break;

        case MCTRANS_FFPC_ACTION :
        {
          // First initialize action packet
          mcctxns_init_ffpc( &packet->action.ffpc );

          if ( ho_data->ho_msg.extra_parms && ho_data->ho_msg.extra.nnscr_included &&
               ho_data->ho_msg.extra.non_neg_cfg.fpc_incl )
          {
            mcctxns_prepare_scr_ffpc_action_packet( nn_config , &packet->action.ffpc );
          }

          // FFPC needs to be notified if FCH/DCCH is starting/stopping and if RC is changing
          if ( ho_fwd_chan_mask & MCC_F_DCCH )
          {
            packet->action.ffpc.dcch_ffpc_op.mode = MCTRANS_FFPC_START;
            // We have already verified that DCCH is included in SCR since we are here
            packet->action.ffpc.dcch_ffpc_op.rc =
              (cai_radio_config_type) config->for_dcch_rc;
          }
          else
          {
            packet->action.ffpc.dcch_ffpc_op.mode = MCTRANS_FFPC_STOP;
          }
          if ( ho_fwd_chan_mask & MCC_F_FCH )
          {
            //lint -e539
            packet->action.ffpc.fch_ffpc_op.mode = MCTRANS_FFPC_START;
            // We have already verified that FCH is included since we are here
            packet->action.ffpc.fch_ffpc_op.rc =
              (cai_radio_config_type) config->for_fch_rc;
            //lint +e539
          }
          else
          {
            packet->action.ffpc.fch_ffpc_op.mode = MCTRANS_FFPC_STOP;
          }

          // Now fpc_subchan_gain.  process_subchan_gain is set properly when the transaction
          // is set up
          if ( ho_data->process_subchan_gain_at_ho )
          {
            // No separate action time for fpc_subchan_gain
            packet->action.ffpc.fpc_subchan_gain.included = TRUE;
            packet->action.ffpc.fpc_subchan_gain.value = cdma.fpc_subchan_gain;
          }

          if ( ho_data->new_fpc_pri_chan )
          {
            /* If we changed FPC PRI CHAN due to changing CHIND, change it
               back  */
            packet->action.ffpc.fpc_pri_chan.included = TRUE;
            packet->action.ffpc.fpc_pri_chan.value = nn_config->fpc_pri_chan;
          }
          break;
        }

        case MCTRANS_CTRL_HLD_GATING_ACTION :
        {

          if (!(mccccl_code_channel_list.in_control_hold()))
          {
            /* if the current CHIND contains pilot, we must be going out of control hold */
            packet->action.rxc_ctrl_hld_gating.gating_rate = CAI_GATING_RATE_NONE;
          }
          else
          {
            packet->action.rxc_ctrl_hld_gating.gating_rate =
              (cai_pilot_gating_rate_type) nn_config->pilot_gate_rate;
          }

          if ( packet->action.rxc_ctrl_hld_gating.gating_rate == CAI_GATING_RATE_NONE ||
               packet->action.rxc_ctrl_hld_gating.gating_rate == CAI_GATING_RATE_ONE )
          {
            // No gating, use BS assigned power control delay
            packet->action.rxc_ctrl_hld_gating.rev_pwr_control_delay =
               cdma.rev_pwr_cntl_delay;
          }
          else if(packet->action.rxc_ctrl_hld_gating.gating_rate != CAI_GATING_RATE_RESERVED)
          {
            // Otherwise, use appropriate delay for gating rate
            packet->action.rxc_ctrl_hld_gating.rev_pwr_control_delay =
              gating_rate_to_pwr_delay[nn_config->pilot_gate_rate];
          }
          break;
        }

        #ifdef FEATURE_IS2000_1X_ADV
        case MCTRANS_RCP_ACTION:
        {
          mcc_get_rxc_txc_rc_params( &packet->action.rxc_rcp );
          break;
        }
        #endif /* FEATURE_IS2000_1X_ADV */

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action );
          break;
      }
      break;
    } // of case MCTRANS_SERVER_RXC

    case MCTRANS_SERVER_TXC :
    {
      switch ( action )
      {
        case MCTRANS_CTRL_HLD_GATING_ACTION :
        {

          if (!(mccccl_code_channel_list.in_control_hold()))
          {
            /* if the current CHIND contains pilot, we must be going out of control hold */
            packet->action.txc_ctrl_hld_gating.gating_rate = CAI_GATING_RATE_NONE;
          }
          else
          {
            packet->action.txc_ctrl_hld_gating.gating_rate =
              (cai_pilot_gating_rate_type) nn_config->pilot_gate_rate;
          }
          break;
        }

        case MCTRANS_SCR_ACTION :
          mcctxns_prepare_scr_action_packet(MCTRANS_SERVER_TXC, packet , config, nn_config);
          break;

        case MCTRANS_HO_ACTION :
        {
          packet->action.txc_ho.hho_ind = FALSE;
          packet->action.txc_ho.rif_ind = FALSE;
          /* Set some default values here */
          packet->action.txc_ho.frame_offset = cdma.frame_offset;
          packet->action.txc_ho.num_preamble.unit = MCTRANS_PREAMBLE_FRAME;
          packet->action.txc_ho.num_preamble.value = 0;
          packet->action.txc_ho.nom_pwr.included = FALSE;

          #ifdef FEATURE_IS2000_REL_A
          packet->action.txc_ho.p_rev_in_use = mc_get_p_rev_in_use();
          #endif

          break;
        }
        case MCTRANS_RLGC_ACTION :
        {
          mcctxns_init_rlgc( &packet->action.rlgc );

          /* Vanitha, revisit */
          packet->action.rlgc.use_default_rlag = FALSE;
          packet->action.rlgc.rlgain_traffic_pilot.included = FALSE;

          if ( (packet->action.rlgc.pwr_cntl_step.included = ho_data->ho_msg.use_pwr_cntl_step )
               != FALSE )
          {
            packet->action.rlgc.pwr_cntl_step.value = cdma.pwr_cntl_step;
          }

          /* RLGC needs to be notified if FCH/DCCH/F-PDCH is starting/stopping */
          if (ho_rev_chan_mask & MCC_R_DCCH)
          {
            packet->action.rlgc.dcch_rlgc_op.mode = MCTRANS_RLGC_START;
            packet->action.rlgc.dcch_rlgc_op.rc =
              (cai_radio_config_type)config->rev_dcch_rc;
          }

          if (ho_rev_chan_mask & MCC_R_FCH)
          {
            //lint -e539
            packet->action.rlgc.fch_rlgc_op.mode = MCTRANS_RLGC_START;
            packet->action.rlgc.fch_rlgc_op.rc =
              (cai_radio_config_type)config->rev_fch_rc;
            //lint +e539
          }

          /* We are done here. HO does not have SCH RLGC parameters. */
          break;
        }

        case MCTRANS_GATING_ACTION:
        {
          /* Note R-SCH need not be checked here because this a return trans, no R-SCH
             can be active dyrung this time */
          packet->action.txc_gating.rev_fch_gating_on =
            /* If R-DCCH is in use, cannot use R-FCH gating */
            !(ho_rev_chan_mask & MCC_R_DCCH) &&
            ho_data->high_rc_before_ho && cdma.rev_fch_gating_on;
          break;
        }

        case MCTRANS_LCM_ACTION:
        {
          mcctcsup_get_long_code_mask(packet->action.txc_lcm.long_code_mask);
          break;
        }

        #ifdef FEATURE_IS2000_1X_ADV
        case MCTRANS_RCP_ACTION:
        {
          mcc_get_rxc_txc_rc_params( &packet->action.txc_rcp );
          break;
        }
        #endif /* FEATURE_IS2000_1X_ADV */
        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action );
          break;
      }
      break;
    } // of case MCTRANS_SERVER_TXC

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid Server Token %d", server_token);
      break;
  }

}

/*===========================================================================

FUNCTION mcctxns_init_cfs_gps_prep_trans

DESCRIPTION
  Initializes actions for a CFS/GPS Prep transaction

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_init_cfs_gps_prep_trans
(
  mctrans_trans_info_type * curr_trans
)
{
  /* Clear TA CFS Defer Timer */
  (void) mcc_clr_callback_timer( &mcc_ta_cfs_defer_timer,
                                 MCC_TA_CFS_DEFER_TIMER_ID );
  
  /* Reset CFS defer timer flag */
  cdma.tc_data_ta.is_cfs_defer_timer_expired = FALSE;
  
  /* Reset TA global var to remember the returned status to SRCH */
  mcctcsup_update_srch_cfs_proc_status(FALSE); 
  
  /* 1xTA: Update CFS progress state */ 
  cdma.tc_data_ta.cfs_prog = CFS_IN_PROG;

  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_CFS_GPS_PREP_ACTION);
  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_TX_ON_OFF_ACTION);
  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_TX_ON_OFF_ACTION);
  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_CFS_MEAS_ACTION);
}

/*===========================================================================

FUNCTION mcctxns_cfs_gps_prep_pre

DESCRIPTION
  Pre-Processor for a CFS/GPS Prep transaction

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=========================================================================== */
static void mcctxns_cfs_gps_prep_pre
(
  mctrans_trans_info_type * curr_trans
)
{
  mcctxns_init_cfs_gps_prep_trans(curr_trans);
  cdma.tc_tx_on = FALSE;

}


/*===========================================================================

FUNCTION mcctxns_begin_cfs_gps_prep_trans

DESCRIPTION
  Starts a "CFS GPS Prep" transaction

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_cfs_gps_prep_trans
(
  qword trans_time
)
{
  uint16 trans_num;
  mctrans_server_token_type server_order[MCTRANS_SERVER_COUNT];
  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_CFS_GPS_PREP,
    MCTRANS_MODE_IMMEDIATE,
    mcctxns_cfs_gps_prep_pre,
    mcctxns_generic_post,
    mcctxns_cfs_gps_prep_action_packet
  };

  /* It is important that this be ordered for CFS, we can only tune
     after the tx is off and RX has been told to stop fade processing */
  server_order[0] = MCTRANS_SERVER_TXC;
  server_order[1] = MCTRANS_SERVER_RXC;
  server_order[2] = MCTRANS_SERVER_SRCH;
  trans_num = mctrans_despatch_ordered_transaction( trans_time,
                          &despatch_info,
                          0,
                          server_order
                          );

  M1X_MSG( DCP, LEGACY_MED,
    "CFS/GPS Prep transaction despatched %d",
    trans_num );

  (void) trans_num; /* Added to satisfy compiler warning (set but not used) */
}

/*===========================================================================

FUNCTION mcctxns_cfs_gps_prep_action_packet

DESCRIPTION
  Prepares the action packets for CFS GPS PREP transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_cfs_gps_prep_action_packet
(
  mctrans_trans_info_type *   curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
)
{
  // Stamp action type in action packet
  packet->action_type = action;

  switch ( server_token )
  {
    case MCTRANS_SERVER_RXC:
    {
      switch (action)
      {
        case MCTRANS_CFS_GPS_PREP_ACTION:
        {
          packet->action.cfs_gps_prep.is_gps = FALSE;
          /* Set is_gps flag here */
          break;
        }
        default:
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unknown Action %d",
            action);
          break;
        }
      }
      break;
    }

    case MCTRANS_SERVER_SRCH:
    {
      switch (action)
      {
        case MCTRANS_TX_ON_OFF_ACTION:
        {
          packet->action.txc_tx_on_off.tx_on = FALSE;
          /* Put TX on or off indication here */
          break;
        }
        case MCTRANS_CFS_MEAS_ACTION:
        {
          packet->action.srch_cfs_meas.cfs_seq = cand_freq_srch_parm.cur_srch_seq_num;;
          break;
        }
        default:
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "Unknown Action %d",
            action);
          break;
        }
      }
      break;
    }

    case MCTRANS_SERVER_TXC:
    {
      switch (action)
      {
        case MCTRANS_TX_ON_OFF_ACTION:
        {
          packet->action.txc_tx_on_off.tx_on = FALSE;
          break;
          /* Set tx_on to 0 here */
        }
        default:
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unknown Action %d",
            action);
          break;
        }
      }
      break;
    }

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid Server Token %d", server_token );
      break;
  }

}

/*===========================================================================

FUNCTION MCCTXNS_EXT_GEN_SCR_ACTION_PACKET_FOR_TXC

DESCRIPTION
  Generates SCR information to be sent to TXC via a command
  so as to avoid executing SCR transaction just for providing SCR params 
  to TXC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_ext_gen_scr_action_packet_for_txc
(
  mctrans_action_packet_type* packet
)
{
/* Send SCR information to TXC via a command and there is no need 
 * to start entire SCR transaction */
  mcctxns_prepare_scr_action_packet( MCTRANS_SERVER_TXC, 
                                     packet, snm_get_current_config(), 
                                     snm_get_current_nnscr());
} /* mcctxns_ext_gen_scr_action_packet_for_txc */


#ifdef FEATURE_IS2000_1X_ADV
/*===========================================================================

FUNCTION mcctxns_begin_rcp_trans

DESCRIPTION
  Starts a RCP transaction

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctxns_begin_rcp_trans
(
  qword trans_time
)
{
  uint16 trans_num;
  mctrans_server_token_type server_order[MCTRANS_SERVER_COUNT];

  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_RCP,
    MCTRANS_MODE_EXPLICIT,
    mcctxns_rcp_pre,
    NULL,
    mcctxns_rcp_action_packet
  };

  server_order[0] = MCTRANS_SERVER_SRCH;
  server_order[1] = MCTRANS_SERVER_RXC;
  server_order[2] = MCTRANS_SERVER_TXC;

  trans_num = mctrans_despatch_ordered_transaction( trans_time,
                  &despatch_info,
                  NULL,
                  server_order
              );

  M1X_MSG( DCP, LEGACY_MED,
    "RCP transaction despatched, trans_num %d",
    trans_num );

}

/*===========================================================================

FUNCTION mcctxns_init_rcp_trans

DESCRIPTION
  Initializes actions for an RCP transaction

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_init_rcp_trans
(
  mctrans_trans_info_type * curr_trans
)
{
  M1X_MSG( DCP, LEGACY_MED,
    "RCP Transaction added to servers");
  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_RCP_ACTION);
  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_RCP_ACTION);
  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_RCP_ACTION);
}

/*===========================================================================

FUNCTION mcctxns_rcp_pre

DESCRIPTION
  Pre-Processor for the RCP txn

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_rcp_pre
(
  mctrans_trans_info_type * curr_trans
)
{
  mcctxns_init_rcp_trans(curr_trans);

  /* Synchronize pilot records in RCP data with the current ASET */
  mcc_sync_rcp_data_and_aset();

  /* Update NNSCR with the FPC_MODE received in RCPM */
  snm_update_nn_scr_from_rcp_info();
}

/*===========================================================================

FUNCTION mcctxns_rcp_action_packet

DESCRIPTION
  Prepares the action packets for RCP transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mcctxns_rcp_action_packet
(
  mctrans_trans_info_type *   curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
)
{
  /* Stamp action type in action packet */
  packet->action_type = action;

  if (action != MCTRANS_RCP_ACTION)
  {
    ERR_FATAL ("Wrong action type %d", (int) action,0,0);
  }

  switch ( server_token )
  {
    case MCTRANS_SERVER_RXC:
    {
      switch (action)
      {
        case MCTRANS_RCP_ACTION:
        {
          mcc_get_rxc_txc_rc_params( &packet->action.rxc_rcp );
          break;
        }
        default:
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unknown Action %d",
            action);
          break;
        }
      }
      break;
    }

    case MCTRANS_SERVER_TXC:
    {
      switch (action)
      {
        case MCTRANS_RCP_ACTION:
        {
          mcc_get_rxc_txc_rc_params( &packet->action.txc_rcp );
          break;
        }
        default:
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unknown Action %d",
            action);
          break;
        }
      }
      break;
    }

    case MCTRANS_SERVER_SRCH:
    {
      switch (action)
      {
        case MCTRANS_RCP_ACTION:
        {
          mcc_get_pilots_rc_param_recs((cai_rcp_srch_type*) &packet->action.srch_rcp);
          break;
        }
        default:
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unknown Action %d",
            action);
          break;
        }
      }
      break;
    }

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
          "Invalid Server Token %d", server_token );
      break;

  } /* End of Switch */

}
#endif /* FEATURE_IS2000_1X_ADV */
