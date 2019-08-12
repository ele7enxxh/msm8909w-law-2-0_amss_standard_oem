/*===========================================================================

    S U P P L E M E N T A L   C H A N N E L   ( S C H )   S E R V I C E S

GENERAL DESCRIPTION
  This module contains utilities related to operation of SCH.

  Copyright (c) 2000 - 2015 Qualcomm Technologies, Inc. 
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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccsch.c_v   1.25   02 Oct 2002 22:52:30   louiel  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccsch.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/22/16   ppr     1X2GTA:Chgs to save rcvd ESCAM OTA msg.
03/09/16   ppr     1x2GTA Feature changes
07/08/15   ppr     Reverting chgs to stop F-SCH when num_for_sch=0 in ESCAM
16/01/14   bb      If For_SCH is ZERO while it is active, changes to stop F-SCH
                   right away
05/06/14   gga     1X_CP F3 redistribution
09/27/13   ppr     Reject ESCAM if Service Connect Msg not received.
09/04/13   agh     Fixed bug - Modem crashes due to reading of unlocked
                   TDEC registers
08/05/13   gga     Feature cleanup
02/28/13   ppr     PPP cleanup changes when moving from 1x to LTE in SV mode
09/15/12   ppr     FR#2370 - 1x memory optimization changes
08/10/12   ppr     SO32 rejected because ESCAM conflict with FSCH config
04/04/12   trc     Convert to m1x_time module
02/21/12   ppr     Feature Cleanup
02/04/12   ppr     Feature Cleanup
06/15/11   ag      Changes to handle GEM related failure scenarios.
05/02/11   ag      Added events for SCH burst assignment information.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
02/07/11   ag      Support for default fpc_mode_sch for RC8/11.
01/04/11   ag      Fixed compiler and lint warnings.
11/15/10   jtm     Initial SU API split for CP.
10/08/10   ag      Fixed KW errors.
07/22/10   ag      Added support for GEM containing ESCAM and RCP record.
07/15/10   ssh     Fixed Compiler Warnings.
05/26/10   ag      Replaced PACKED with PACK() to support QDSP6.
04/23/10   ag      Support for 1x Modem statistical report.
02/25/10   ssh     Cleaned up the Lint issues.
11/02/09   jtm     Fixed compiler warnings.
10/12/09   ssh     Initialized trans_time, cur_time variable to zero as part
                   of KW error fix.
08/26/09   adw     Replaced deprecated ts.h api's with new time api's.
08/02/09   jtm     Lint fixes.
05/03/09   jj      CMI Phase-II update.
04/17/09   jtm     Added includes to fix compiler warnings.
03/20/09   jtm     Removed unnecessary include of mctrans.h.
06/11/08   ag      Fixed more Klocwork errors.
04/08/08   pg      Featurization and general code cleanup for memory reduction
                   effort.
20/03/08   bb      Fixed klocwork errors
03/08/07   an      Fixed Critical Lint Errors
06/23/06   pg      RPC clean up.
05/30/06   fc      Fixed lint errors.
04/26/06   an      Incorrect parameter assignment error fixed
04/24/06   an      Fixed lint errors
07/11/05   fh      Cleaned up header file including from MUX:
                     Removed including of txc.h and rxc.h
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
02/08/05   va      Added sending SRCH sch rate/code/rc etc even for a stop txn
                   if stop actually reflects a start SCH action to SRCH
02/03/05   fc      Fixed lint error.
02/02/05   va      Added sending SRCH sch rate/code/rc etc even for a stop txn
                   if stop actually reflects a start SCH action to SRCH
12/10/04   sb      Fixed lint warning.
12/08/04   va      Merged following from REL_B_C archive:
11/11/04   yll       Removed the unncessary srch_notify_sch_active.
11/05/04   va      Merged following from REL_B_C archive:
10/20/04   an      Replaced inclusion of srch.h with srchmc.h
09/30/04   yll     Added the rc, sch_rate and code_type in F-SCH Action
                   Packet to Search.
11/05/04   sb      Set sch rc in rlgc packet outside of mcctxns_init_rlgc.
09/27/04   va      Merged from REL B/C archive:
07/21/04   va       Reject ESCAM with SCH CC but 0 duration assignment
06/17/04   bkm     Ensure FFPC action fields set safely for no-fwd-fundicated
                   channel case.
04/08/04   bkm     Channel mask updates and added check for F-SCH assignment
                   while F-PDCH is active.
03/26/04   bkm     Merged the following from IS2000_REL_B_C:
  02/23/04   ph      Added FFPC action for Search in Stop F-SCH
02/10/04   az      Added support for new transaction engine design
01/20/04   bkm     Fixed compile warning for 6500 no DV build.
01/05/04   bkm     Updated gating check for DV for stop SCH action packet.
12/02/03   bkm     Using mccccl.in_control_hold rather than checking chind.
08/15/03   ph      When sending FCH rev gating at stop r-sch time, check DCCH.
08/12/03   ph      When stopping r-sch, void any pending extension transactions.
05/29/03   bkm     Use NULL rather than 0 to initialize function pointers for
                   empty pre-processor, post-processor or action pkt generators.
05/23/03   va      Added check for pilot_rec_type match too when checking if
                   SCH pilot is a subset of fundicated active set.
03/28/03   va      Added support SCH cfg inclusion in ESCAM ( Release B)
03/10/03   va      Fixed the bug where REV SCH config is not checked if both
                   num_for_sch and num_rev_sch !=0
03/04/03   bkm     Added SCH coding and RC to burst start Events.
01/31/03   bkm     Adjusted msg level on several SCH F3 messages.
11/11/02   lcc     When R-SCH assignment is received, cancel any pending R-SCH
                   extension transaction as that will not be valid any more.  A
                   new extension transaction will be regenerated anyway.
10/02/02   yll     Linted the code.
10/02/02   az      Mainlined FEATURE)IS2000_2_1_ADDENDUM
09/26/02   bkm     Add SCH rate to burst start Events.
09/04/02   va      Fixed start time of active transaction despatched as a result
                   of a SCH assignment.
08/29/02   lcc     When ESCAM is used to stop a F-SCH burst, do not delay by 1
                   frame even if too close to start time.
06/28/02   lh      Reverse SCH MuxPDU 5 support.
06/20/02   lh      Fixed RELA off warnings.
06/19/02   va      Fixed RELA turned off related compilation issues.
05/31/02   va      Added support for sending FFPC action to SRCH
                   when ESCAM includes fpc_pri_chan value.
03/11/02   ht      (lh) Added SRCH throttling support when there is SCH activity.
03/06/02   jrp     Merged from common archive:
                   02/07/02   lcc     Added duration argument to the callback mccsch_sch_config_change_cb.
02/12/02   yll     Merged from the common archive CP VU 116.
                   01/24/02   lcc
                   When ESCAM is processed on the frame that a F-SCH burst is
                   supposed to start, delay and shorten the burst by 1 frame
                   the first frame is guaranteed to be lost; otherwise we
                   may end up in reading old data from decoder.
12/07/01   ph      Merged in from MSM_CP.05.00.108
                   11/28/01   fc      Added support for the update of DTX duration and
                                      USE_T_ADD_ABORT of R-SCH in progress by ESCAM containing
                                      only F-SCH assignment.
                   11/27/01   lcc     Changes to accomodate new definition for mctrans_fpc_olpc_param_type.
10/23/01   jrp     Merged the following from common archive (MSM_CP.05.00.99)
                   10/05/01   lcc     Added support for R-SCH burst extension.
10/02/01   yll     Merged following from common archive.
  09/13/01   lcc     Changed an FCH assignment to stop to use a "start SCH"
                     transaction instead of "stop SCH" since a "stop" is also
                     considered an assignment (which overrides earlier ones).
06/26/01   lh      Merged following from common archive.
  06/14/01   ht      Moved err.h inside feature label to avoid a compiler
                     warning.
  06/14/01   lcc     Added initialization of rev_sch_dtx_duration to 0 in
                     mccsch_initialize.
  06/01/01   fc      Replaced the function call to txc_get_sch_info with
                     mccsch_sch_is_active in mccsch_process_t_add_abort.
  05/22/01   lcc     1. Added some debug messages on assignment included in ESCAM.
                     2. Modified mccsch_compute_start_time to takes a reference time
                        as function argument.
06/25/01   lh      Fixed errors caused by RATE_SET_OF_FWD and RATE_SET_OF_REW
                   definition changes.
06/14/01   va      Changed call to send_rej_order to match the new prototype.
05/17/01   lh      Merged the following from common archive
  05/03/01   jq      Added mccsch_verify_fwd_assignment ()
  05/02/01   fc      Added support of T_ADD_ABORT.
  04/10/01   jq      Moved all SCRM related data structures and functions to
                     mccds.c.
  04/25/01   lcc     1. Removed a redundant error checking in mccsch_process_escam
                     2. Added validation of SCCL if ESCAM contains a F-SCH assignment.
  04/20/01   fc      Corrected mccsch_proc_r_dtx to zero out requested BLOB which
                     is used to build SCRM..
  04/13/01   fc      Changed sending SCRM in assured mode for cancellation of
                     resources.
  04/10/01   jq      Added ESCAM processing in control hold mode
             lcc     1. Added update of FPC_PRI_CHAN in NNSCR if changed by ESCAM.
                     2. Added initialization of for_sch_fer_rep.
  03/06/01   fc      Added support of events report: Start/End of forward
                     supplemental burst assignment and Start/End of reverse
                     supplemental burst assignment.
  03/06/01   lcc     Removed some unnecessary debug messages (some moved to mctrans.c).
05/10/01   lh      307K FSCH support added.
04/09/01   va      Made some changes in SCRM processing. Consolidates pilot_rec_type
                   checking for ESCAM by calling a common function defined for
                   that in mcctcsup.c
03/28/01   va      Changed pilot rec type handling in SCRM to match what is
                   in caii.h.
03/01/01   va      Merged the following from common archive
  02/12/01 rmd     Made changes to mccsch_adjust_clk_freq() so it supports dynamic
                   clock switching for both MSM5105 and MSM5000.
  01/16/01 jq      Moved/renamed mccsch_retry_delay_type, mccsch_retry_delay to
                   mccsup.c.
  01/15/01 lcc     Moved function mccsch_inform_sch_change outside of
                   FEATURE_IS2000_R_SCH.
  01/10/01 lcc     1. Added function mccsch_inform_sch_change for informing
                      "SCH change" client of SCH operational changes.
                   2. Changed function pointer mccsch_sch_config_inform to include
                      mccsch_sch_info_type pointer as parameter, and other changes
                      related to this.
  12/27/00 lcc     1. Added gating action to R_SCH START/STOP transactions in
                      process_escam.
                   2. Rearranged servers in despatch_info so that they are ordered
                      the same as the server names.
02/27/01   va      Added validation in ESCAM to make sure there are no 3X
                   pilots. Support for new SCRM message enhancements for
                   Rel A.
11/08/00   lcc     Corrected PDU size for 1X SCH to be single all the time.
10/19/00   lcc     Changed calls to ts_get_20ms_frame_time() to
                   ts_get_20ms_frame_offset_time() to take care of non-zero
                   frame offset cases.
09/18/00   jq      Moved declaration of mccsch_verify_rate() to mccsch.h.
09/14/00   jq      Modified ESCAM SCH verification in mccsch_process_escam().
09/05/00   jq      Fixed a bug in mccsch_proc_r_dtx().
08/28/00   fc/jq   Featurized mccsch_reg_retry_delay() and
                   mccsch_update_retry_delay() under FEATURE_IS2000.
                   Featurized mccsch_proc_r_dtx() under FEATURE_IS2000_R_SCH.
08/23/00   jq      Added mccsch_proc_r_dtx()
08/23/00   lcc     1. Corrected a problem in mccsch_begin_trans such that a
                      "stop F/R-SCH" from UHDM will stop at the right time.
                   2. Remove use of stack variable tmp_info in mccsch_begin_trans.
                      instead the static structure tmp_despatch_info is used
                      (which already exists).
                   3. Added mccsch_stop_f_sch0_trans_now to stop SCH 0
                      immediately.
08/18/00   fc      Added support for retry order.
08/11/00   va      Added FEATURE_IS2000_R_SCH around tmp_info declaration.
08/02/00   va      Support for dynamic clk freq change.
08/01/00   va      Moved the prototype for mccsch_adjust_clk_freq outside
                   FEATURE_IS2000_R_SCH
07/25/00   fc      Added call to initialize DS SCRM variables.
07/22/00   va      #ifdef-ed out the clk change support temporarily.
07/21/00   fc      Use enumeratd types for FPC mode and SCH rate.
07/21/00   va      Support for dynamic clock freq change.
07/17/00   va      Support to spawn gating_action along with start and stop R-SCH
                   actions if necessary
07/10/00   fc      Changed scrm_blob_generator prototype to support STDSO.
06/22/00   fc      Added support for SCRM.
06/21/00   lcc     1. Corrects a problem with pending messages for a transaction
                      not being released sometimes.
                   2. During F-SCH assignment, uses fpc_mode_sch even if fpc
                      info. is not included with the message.
06/19/00   lcc     Changes for making SCH init_setpt included only in ESCAM
                   for mctrans_ffpc_action_type.
06/16/00   lcc     1. Added debug message for cases when the ESCAM is rejected.
                   2. Corrected CR 13689.  Rate is now correctly calculated so
                      that ESCAM won't be reject mistakenly.
06/12/00   lcc     1. Moved mccsch_init_ffpc and mccsch_init_rlgc to mxtxns.c
                      as part of code consolidation.
                   2. Added RLGC_ACTION (always) to R-SCH actions since start/stop
                      is required for RLGC and related changes.
                   3. Added calls to void SCH stop transactions when starting SCH
                      transaction just in case a stop transaction happens exactly
                      at the same time and just before the start transaction is
                      executed.
                   4. Added verification of SCH rates when ESCAM is received.
                   5. Added function mccsch_sch_is_active.
06/02/00   lcc     1. Changes to support IS2000 Rev 0 published addendum.
                   2. Properly restores FPC_MODE when SCH is brought down.
                   3. Removed some obsolete code.
                   4. Added some debug messages and miscellaneous cleanup.
06/02/00   lcc     In mccsch_init_ffpc, includes RC in fch_ffpc_op and sch_ffpc_op
                   when the mode is either start or continuing.
06/01/00   lcc     In mccsch_process_escam, corrected a typo which caused reverse
                   burst assignments not to end (i.e. becomes infinite).
05/18/00   lcc     In mccsch_begin_trans, added support for assigning R-SCH
                   with UHDM.
05/16/00   lcc     Changed data member code_type of R_SCH action to turbo_code
                   to better reflect it's meaning.  Also added additional
                   verification of SCH assignment.
02/23/00   lcc     Initial release.

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

#include "cai_v.h"
#include "caii_v.h"
#include "mccap_v.h"
#include "mccccl.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mccds_v.h"
#include "mccrx.h"
#include "mccsch.h"
#include "mccsch_i.h"
#include "mccsrch_i.h"
#include "mcctcho_i.h"
#include "mcctcsup.h"
#include "mcctxns.h"
#include "qw.h"
#include "snm_v.h"
#include "snm_i.h"
#include "srchmc.h"
#include "m1x_time_i.h"
#include "event.h"

#include "err.h"
#include "rxtx_v.h"

#include "srchtc_ta.h"

/*===========================================================================

                 PROTOTYPES FOR SOME FUNCTIONS USED LOCALLY

===========================================================================*/
// MCTRANS_F_SCH0_START related functions
static void mccsch_start_f_sch0_pre
(
  mctrans_trans_info_type *   curr_trans
);
static void mccsch_start_f_sch0_action_packet
(
  mctrans_trans_info_type *   curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
);
static void mccsch_start_f_sch0_post
(
  mctrans_trans_info_type *   curr_trans
);

// FFPC related functions
static void mccsch_prepare_escam_ffpc_action_packet
(
  mctrans_trans_info_type *   curr_trans,
  mctrans_ffpc_action_type* packet
);

// MCTRANS_F_SCH0_STOP related functions
static void mccsch_stop_f_sch0_pre
(
  mctrans_trans_info_type *   curr_trans
);
static void mccsch_stop_f_sch0_action_packet
(
  mctrans_trans_info_type *   curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
);
static void mccsch_stop_f_sch0_post
(
  mctrans_trans_info_type *   curr_trans
);

static void mccsch_inform_sch_change
(
  mccsch_ch_dir_type  dir,
  uint8               sch_id
);

// MCTRANS_R_SCH0_START related functions
static void mccsch_start_r_sch0_pre
(
  mctrans_trans_info_type *   curr_trans
);
static void mccsch_start_r_sch0_action_packet
(
  mctrans_trans_info_type *   curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
);
static void mccsch_start_r_sch0_post
(
  mctrans_trans_info_type *   curr_trans
);

static void mccsch_escam_rlgc_action_packet
(
  mctrans_trans_info_type * curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
);

// MCTRANS_R_SCH0_STOP related functions
static void mccsch_stop_r_sch0_pre
(
  mctrans_trans_info_type * curr_trans
);
static void mccsch_stop_r_sch0_action_packet
(
  mctrans_trans_info_type * curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
);
static void mccsch_stop_r_sch0_post
(
  mctrans_trans_info_type * curr_trans
);

// MCTRANS_R_SCH0_EXTEND related functions
static void mccsch_r_sch0_extend_pre
(
  mctrans_trans_info_type * curr_trans
);

// MCTRANS_R_SCH_UPDATE related functions
static void mccsch_update_r_sch_parm_action_packet
(
  mctrans_trans_info_type * curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
);

void mccsch_init_update_r_sch_parm_trans( mctrans_trans_info_type * curr_trans );

#ifdef FEATURE_1XCP_FTD
static void mccsch_report_event_sch_burst_assign
(
  mccsch_ch_dir_type dir
);
#endif /* FEATURE_1XCP_FTD */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

// Contains SCH assignment information
sch_assign_type sch_assign;

// Table to map duration values into 20ms units.  Defined in IS2000 Vol 5
// table 3.7.3.3.2.37-3.  Entry 0 is a filler.
static const uint16 duration_table[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 16, 32, 64, 96, 128, 256 };

// Table to map rate values into 1X, 2X, ...
extern  const uint8 snm_rs1_rate_table[];
extern  const uint8 snm_rs2_rate_table[];


//LTU length is fixed for MuxPDU type 3.
//368 bits for rate set 1. 560 bits for rate set 2.
#define MUX_PDU_3_RS1_LTU_LEN       368
#define MUX_PDU_3_RS2_LTU_LEN       560

#define NUM_DEFINED_RATE  9        //Rate defined in table 3.7.3.3.2.37-2
#define NUM_RATE_SET      2        //Number of rate set.

// Default table for the number of LTUs in an SDU for a given rate.
// Table 2-10 in the MAC spec.
// Each row corresponding a rate given in Table 3.7.3.3.2.37-2 in
// IS-2000.5-A. The first row is for RS1, the 2nd row is for RS2.
static const uint8 def_num_ltu_table[NUM_DEFINED_RATE][NUM_RATE_SET] =
{   //  RS1      RS2             RS1       RS2
    //-----------------     ----------------------
    {   0,        0   },    //  172       267    bits
    {   0,        0   },    //  360       552
    {   2,        2   },    //  744      1128
    {   4,        4   },    // 1512      2280
    {   8,        8   },    // 3048      4584
    {   8,        0   },    // 6120      5178
    {   0,        0   },    //12264      9192
    {   0,        0   },    //reserved  10356
    {   0,        0   }     //reserved  20712
};

#ifdef FEATURE_IS2000_REL_A //32X
// Default LTU length for MuxPDU 5, based on
// Table 3.7.3.3.2.37-2 in L3 spec, table 2-10 in the MAC spec,
// and the following equation:
//
//       ltu_len = ((int)(sdu_len/8/num_ltu))*8
//
// LTU_LEN is used in the Interrupt context. This lookup table is
// designed to save calculation time.

static const uint16 def_pdu5_ltu_len_table[NUM_DEFINED_RATE][NUM_RATE_SET] =
{   //  RS1          RS2
    //----------------------
    {     0,          0    },  // rate 0,  1X
    {     0,          0    },  // rate 1,  2X
    {   368,        560    },  // rate 2,  4X
    {   376,        568    },  // rate 3,  8X
    {   376,        568    },  // rate 4, 16X
    {   760,          0    },  // rate 5, 32X
    {     0,          0    },  // rate 6, 64X
    {     0,          0    },  // rate 7,
    {     0,          0    }   // rate 8,
};
#endif /* FEATURE_IS2000_REL_A */

/* Supplemental Channel Callbacks */
static mccsch_sch_callback_type sch_callback = { 0, 0, 0, 0 };

/* Typedef for EVENT_START_FWD/REV_SUPP_BURST_ASSIGN event */
typedef PACK (struct)
{
  unsigned char rate;
  unsigned char coding;
  unsigned char rc;
} event_start_burst;

#ifdef FEATURE_1XCP_FTD
typedef PACK (struct)
{
  uint32 start_time_hi;   /* Most significant 32 bits of Start time in 20ms */
  uint32 start_time_lo;   /* Least significant 32 bits of Start time in 20ms */
  uint8 duration;         /* duration of assignment burst in 20ms */
} mccsch_event_fsch_burst_assign_type;

typedef PACK (struct)
{
  uint32 start_time_hi;   /* Most significant 32 bits of Start time in 20ms */
  uint32 start_time_lo;   /* Least significant 32 bits of Start time in 20ms */
  uint8 duration;         /* duration of assignment burst in 20ms */
  uint8 dtx_duration;     /* discontinuous transmission duration on R-SCH */
} mccsch_event_rsch_burst_assign_type;
#endif /* FEATURE_1XCP_FTD */

/*===========================================================================

FUNCTION mccsch_initialize

DESCRIPTION
  This function performs the initialization for SCH processing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_initialize( void )
{
  uint8 i;

  sch_assign.start_time_unit = 0;

  #ifdef FEATURE_IS2000_1X_ADV
  if ( (cdma.chnasn.mode.am0.for_rc == CAI_RC_11) ||
       (cdma.chnasn.mode.am0.rev_rc == CAI_RC_8) )
  {
    sch_assign.fpc_mode_sch = CAI_FPC_MODE_200_PRI_600_SEC;
  }
  else
  #endif /* FEATURE_IS2000_1X_ADV */
  {
    sch_assign.fpc_mode_sch = CAI_FPC_MODE_800_PRI;
  }

  for ( i = 0; i < CAI_MAX_NUM_FOR_SCH; ++i ) // Max num for and rev are both 2
  {
    sch_assign.forward[i].active = sch_assign.reverse[i].active = FALSE;
  }

  sch_assign.rev_parms.rev_sch_dtx_duration = 0;

  /* Initialize use T_ADD abort indicator */
  sch_assign.rev_parms.use_t_add_abort         =
  sch_assign.rev_parms.current_use_t_add_abort = FALSE;

  /* Initialize IGNORE_ESCAM idicator */
  mccsch_update_ignore_escam(FALSE);

  /* Initialize SCRM related parameters */
  mccds_init_scrm();

  /* Update modem statistics */
  cdma_stats.channel_config_info.scrm_seq_num = mccds_get_scrm_seq_num();

  /* Update modem statistics */
  cdma_stats.channel_config_info.fsch0_active = sch_assign.forward[0].active;
  cdma_stats.channel_config_info.rsch0_active = sch_assign.reverse[0].active;
  cdma_stats.changed_bitmask |= SYS_CDMA_CHANNEL_CONFIG_INFO_BIT_MASK;

} /* mccsch_initialize */

/*===========================================================================

FUNCTION mccsch_begin_trans

DESCRIPTION
  Starts a transaction of type "trans_type" based on information contained in the
  static arrays defined in this file.  The purpose of this function is to provide
  encapsulation such that entities that those static arrsy do not have to be public.
  This function only despatches SCH start transactions that happen to be invoked from
  outside this file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_begin_trans
(
  mctrans_transaction_type  trans_type,
  mctrans_pending_action_msg_type* msg
)
{
  qword trans_time = {0, 0};

  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_COUNT,                 // Transaction type
    MCTRANS_MODE_EXPLICIT,
    NULL,
    NULL,
    NULL
  };

  // First find despatch info.
  switch ( trans_type )
  {
    case MCTRANS_F_SCH0_START :

      despatch_info.transaction_type = MCTRANS_F_SCH0_START;

      despatch_info.preprocessor = mccsch_start_f_sch0_pre;
      despatch_info.postprocessor = mccsch_start_f_sch0_post;
      despatch_info.action_packet_gen = mccsch_start_f_sch0_action_packet;

      qw_equ( trans_time, sch_assign.forward[0].start_time );
      break;

    case MCTRANS_R_SCH0_START :

      despatch_info.transaction_type = MCTRANS_R_SCH0_START;

      despatch_info.preprocessor = mccsch_start_r_sch0_pre;
      despatch_info.postprocessor = mccsch_start_r_sch0_post;
      despatch_info.action_packet_gen = mccsch_start_r_sch0_action_packet;

      qw_equ( trans_time, sch_assign.reverse[0].start_time );

      // Invoke RSCH callback if registered and pass in duration
      if ( sch_callback.rsch_assign_cb )
      {
        sch_callback.rsch_assign_cb( sch_assign.reverse[0].rev_sch_duration );
      }
      break;

    default :
      M1X_MSG( DCP, LEGACY_ERROR,
        "Txn not supported by this function" );
      return;
  }

  (void)mctrans_despatch_transaction( trans_time, &despatch_info, msg );

} // mccsch_begin_trans
/*===========================================================================

FUNCTION mccsch_init_trans

DESCRIPTION
  Starts a transaction of type "trans_type" based on information contained in the
  static arrays defined in this file.  The purpose of this function is to provide
  encapsulation such that entities that those static arrsy do not have to be public.
  This function only despatches SCH start transactions that happen to be invoked from
  outside this file.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_init_trans
(
  mctrans_trans_info_type * curr_trans
)
{
  mctrans_pending_action_msg_type* msg;

  // First find despatch info.
  switch ( mctrans_get_transaction_type(curr_trans))
  {
    case MCTRANS_F_SCH0_START:
      mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_F_SCH_ACTION);
      mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_FFPC_ACTION);

      mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_F_SCH_ACTION);

      // Include FFPC action for SRCH only if escam is included and has
      // add_info_incl set to TRUE
      msg = mctrans_get_pending_msg(curr_trans->trans_num);

      if (!msg)
      {
        // No message is included..
        break;
      }

      if (msg->gen.msg_type == CAI_EXT_SUP_CHN_ASN_MSG)
      {
        if (msg->escam.add_info_incl)
        {
          mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_FFPC_ACTION);
        }
      }

      break;

    case MCTRANS_R_SCH0_START :

      mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_R_SCH_ACTION);
      mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_RLGC_ACTION);

      if (cdma.rev_fch_gating_on)
      {
        mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_GATING_ACTION);
      }

      break;

    default :
      M1X_MSG( DCP, LEGACY_ERROR,
        "Txn not supported by this function" );
      return;
  }

} // mccsch_init_trans

/*===========================================================================

FUNCTION mccsch_start_f_sch0_pre

DESCRIPTION
  Pre-processor for "start f-sch 0" transaction.
  This function is called from context of the transaction server (RXC, SRCH).
  Must be brief and not be blocking.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mccsch_start_f_sch0_pre
(
  mctrans_trans_info_type *   curr_trans
)
{
  // Add actions to the transaction
  mccsch_init_trans(curr_trans);

  // If this is a stop, invoke the corresponding "stop" routine.
  if ( sch_assign.forward[0].for_sch_duration == 0)
  {
    mccccl_code_channel_list.deactivate_f_sch( 0 );

    // Mark SCH as inactive
    sch_assign.forward[0].active = FALSE;
  }
  else
  {
    // Activate assignment for SCH 0
    mccccl_code_channel_list.activate_f_sch_assign( 0, sch_assign.forward[0].sccl_index );

    // Mark SCH as active
    sch_assign.forward[0].active = TRUE;
  }

  /* 1xTA: For TA allowed cases do not void the F_SCH0_STOP txn 
   * for HHO in prog case because F_SCH0_STOP might have dispacted 
   * as part of HHO txns execution and HHO txns might got deferred 
   * to execute at later time for TA case.
   * So for this scenario by not voiding F_SCH0_STOP txn will make sure
   * F_SCH0_STOP txn executed before HHO txns executed to avoid crash on FW
   * side while executing HHO begin while SCH0 is active.
   */
  if( (!srchtc_is_ta_supported()) ||
      (!mcc_is_hho_in_prog())
    )
  {
    // Just in case a stop transaction is schedule at exactly the same time but processed after
    // this start transaction, just go ahead and void the stop transaction here.  It's harmless
    // to do it even if it does not exist.
    mctrans_void_trans( MCTRANS_F_SCH0_STOP );
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
             "1xTA:Not voiding MCTRANS_F_SCH0_STOP, as HHO is in Prog");
  }  
} //lint !e715

/*===========================================================================

FUNCTION mccsch_start_f_sch0_action_packet

DESCRIPTION
  The function generates the data needed for "action" in the action packet
  for the "start f-sch" transaction.  It takes care of all actions involved
  in the transaction.
  This function is called from context of the transaction server (RXC, SRCH).
  Must be brief and not be blocking.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mccsch_start_f_sch0_action_packet
(
  mctrans_trans_info_type *   curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
)
{
  // Stamp action type in action packet
  packet->action_type = action;

  // If this is actually a stop, just use the action packet generator for stop transaction
  if ( sch_assign.forward[0].for_sch_duration == 0)
  {
    mccsch_stop_f_sch0_action_packet( curr_trans, action, server_token, packet );
  }
  else  // An actual "start" transaction
  {
    const caii_srv_cfg_type  *config = snm_get_current_config();  // Get current configuration
    const mccccl_sccl_entry_type* sccl_entry =
            mccccl_code_channel_list.get_sccl_entry( 0, sch_assign.forward[0].sccl_index );

    switch ( server_token )
    {
      case MCTRANS_SERVER_RXC :
      {
        switch ( action )
        {
          case MCTRANS_FFPC_ACTION :
            mccsch_prepare_escam_ffpc_action_packet( curr_trans, &packet->action.ffpc );
            packet->action.ffpc.sch_ffpc_op.mode = MCTRANS_FFPC_START;
            break;

          case MCTRANS_F_SCH_ACTION :
          {
            packet->action.rxc_f_sch.sch_id = 0;
            packet->action.rxc_f_sch.start = TRUE;
            packet->action.rxc_f_sch.for_sch_fer_rep = sch_assign.for_sch_fer_rep;
            packet->action.rxc_f_sch.rc = (cai_radio_config_type)config->for_sch[0].sch_chn_cfg.sch_rc;
            // Make sure a valid pointer is returned.  If error happens, 0 is returned from
            // the function.
            if ( sccl_entry != 0 )
            {
              // set the data rate first.
              packet->action.rxc_f_sch.sch_rate = sccl_entry->sch_rate;

              // Update current rate
              sch_assign.forward[0].cur_rate = sccl_entry->sch_rate;

#ifdef FEATURE_IS2000_REL_A //32X
              // Some special code here for MuxPDU 5.
              if( (config->for_sch[0].for_sch_mux > CAI_MUX16) &&
                  (((config->for_sch[0].for_sch_mux & CAI_MO_MUX_PDU_TYPE_MASK) >> 10)
                   == CAI_MUX_PDU_TYPE_5))  /*lint !e641 */
              {
                // If data rate is 1X, force coding to convolutional
                if (packet->action.rxc_f_sch.sch_rate == CAI_SCH_RATE_RS1_1X_RS2_1X)
                {
                  packet->action.rxc_f_sch.code_type = CAI_CONVOLUTIONAL_CODING;
                }
                else
                {
                  // For data rate higher than 1X, use the assigned coding scheme.
                  // This is the same as non-MuxPDU5 situations.
                  packet->action.rxc_f_sch.code_type =
                    (cai_sch_coding_type) config->for_sch[0].sch_chn_cfg.coding;
                }

                // MuxPDU size is variable for type 5. This info is not useful.
                // Just default it to FALSE.
                packet->action.rxc_f_sch.double_size_pdu = FALSE;
                packet->action.rxc_f_sch.mux_pdu_type = CAI_MUX_PDU_TYPE_5;

                //Set number of LTU and LTU length if convolutional coding is used.
                // check for valid range of sch_rate value
                if ((packet->action.rxc_f_sch.code_type == CAI_CONVOLUTIONAL_CODING) &&
                    (((uint32) packet->action.rxc_f_sch.sch_rate) < NUM_DEFINED_RATE))
                {
                  //Look up tables for number of LTUs and LTU length.
                  packet->action.rxc_f_sch.num_ltu =
                        def_num_ltu_table[packet->action.rxc_f_sch.sch_rate]
                         [RATE_SET_OF_FWD_RC(packet->action.rxc_f_sch.rc)];  //lint !e641

                  packet->action.rxc_f_sch.ltu_len =
                        def_pdu5_ltu_len_table[packet->action.rxc_f_sch.sch_rate]
                         [RATE_SET_OF_FWD_RC(packet->action.rxc_f_sch.rc)];  //lint !e641
                }
                else
                {
                  //LTU is not used in Turbo coding.
                  packet->action.rxc_f_sch.num_ltu = 0;
                  packet->action.rxc_f_sch.ltu_len = 0;
                }
              }
              else //For MuxPDU 1, 2, 3
#endif /* FEATURE_IS2000_REL_A */
              {
                if ( packet->action.rxc_f_sch.sch_rate == CAI_SCH_RATE_RS1_1X_RS2_1X )
                {
                  // If SCH rate is 1X, code type is always convoluational...
                  packet->action.rxc_f_sch.code_type = CAI_CONVOLUTIONAL_CODING;
                  // ...and PDU size is always single
                  packet->action.rxc_f_sch.double_size_pdu = FALSE;
                  packet->action.rxc_f_sch.mux_pdu_type = CAI_MUX_PDU_TYPE_1_2_4;
                }
                else
                {
                  packet->action.rxc_f_sch.code_type =
                    (cai_sch_coding_type) config->for_sch[0].sch_chn_cfg.coding;
                  packet->action.rxc_f_sch.double_size_pdu = (config->for_sch[0].for_sch_mux >> 8) & 0x3;
                  packet->action.rxc_f_sch.mux_pdu_type = (cai_mux_pdu_type_type) ((config->for_sch[0].for_sch_mux >> 10) & 0x3);
                }

                // If convolutional coding is used and LTU is used, then LTU length
                // is 368 bits for RS1, 560 bits for RS2.
                // check for valid range of sch_rate value
                if ((packet->action.rxc_f_sch.code_type == CAI_CONVOLUTIONAL_CODING) &&
                    (((uint32) packet->action.rxc_f_sch.sch_rate) < NUM_DEFINED_RATE))
                {
                  packet->action.rxc_f_sch.num_ltu =
                                def_num_ltu_table[packet->action.rxc_f_sch.sch_rate]
                                [RATE_SET_OF_FWD_RC(packet->action.rxc_f_sch.rc)];  //lint !e641

                  if (packet->action.rxc_f_sch.num_ltu != 0)
                  {
                    //LTU length is fixed for MuxPDU type 3.
                    //368 bits for rate set 1. 560 bits for rate set 2.
                    if ((RATE_SET_OF_FWD_RC(packet->action.rxc_f_sch.rc)+1) == CAI_MO_RATE_SET_1 )  //lint !e641
                    {
                      packet->action.rxc_f_sch.ltu_len = MUX_PDU_3_RS1_LTU_LEN;
                    }
                    else if ((RATE_SET_OF_FWD_RC(packet->action.rxc_f_sch.rc)) == (CAI_MO_RATE_SET_2 - 1)) //lint !e650 !e641
                    {
                      packet->action.rxc_f_sch.ltu_len = MUX_PDU_3_RS2_LTU_LEN;
                    }
                    else
                    {
                      // The code should not be here, because the macro
                      // returns either 0, or 1.
                      ERR_FATAL("Invalid RS", 0, 0, 0);
                    }
                  }
                  else
                  {
                    // Number of LTU from the lookup table is 0, LTU
                    // is not used, set LTU_LEN to 0.
                    packet->action.rxc_f_sch.ltu_len = 0;
                  }
                }
                else
                {
                  // Turbo coding does not use LTU.
                  packet->action.rxc_f_sch.num_ltu = 0;
                  packet->action.rxc_f_sch.ltu_len = 0;
                }
              }

#ifdef T_WINNT
              #error code not present
#endif /* T_WINNT */
            }
            else
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "Missing SCCL entry:%d",
                sch_assign.forward[0].sccl_index );
            }

            break;
          } // of case MCTRANS_F_SCH_ACTION

          default:
            M1X_MSG( DCP, LEGACY_ERROR,
              "Invalid Action %d ", action );
            break;

        }
        break;
      } // of case MCTRANS_SERVER_RXC
      case MCTRANS_SERVER_SRCH :
      {
        switch(action)
        {
          case MCTRANS_F_SCH_ACTION :
          {
            // For start SCH0 transaction, there is only 1 action for SRCH, so
            // don't check the action and just generate the packet.
            packet->action.srch_f_sch.sch[0].rc        = (cai_radio_config_type)config->for_sch[0].sch_chn_cfg.sch_rc;
            // Make sure a valid pointer is returned.  If error happens, 0 is returned from
            // the function.
            if ( sccl_entry != 0 )
            {
              // set the data rate first.
              packet->action.srch_f_sch.sch[0].sch_rate = sccl_entry->sch_rate;

              // If data rate is 1X, force coding to convolutional
              if (packet->action.srch_f_sch.sch[0].sch_rate == CAI_SCH_RATE_RS1_1X_RS2_1X)
              {
                packet->action.srch_f_sch.sch[0].code_type = CAI_CONVOLUTIONAL_CODING;
              }
              else
              {
                // For data rate higher than 1X, use the assigned coding scheme.
                // This is the same for MuxPDU5 as for non-MuxPDU5 situations.
                packet->action.srch_f_sch.sch[0].code_type =
                  (cai_sch_coding_type) config->for_sch[0].sch_chn_cfg.coding;
              }
            }

            (void) mccccl_code_channel_list.generate_sch_aset( &packet->action.srch_f_sch );
            break;
          }

          #ifdef FEATURE_IS2000_REL_A
          case MCTRANS_FFPC_ACTION:
          {
            mctrans_pending_action_msg_type* pending_msg;
            pending_msg = mctrans_get_pending_msg( curr_trans->trans_num );
            if ( pending_msg != NULL )
            {
              if (pending_msg->escam.add_info_incl)
              {
                if (mccccl_code_channel_list.fwd_fundicated_chan_is_active())
                {
                  packet->action.srch_ffpc.fpc_pri_chan =
                    (cai_fpc_pri_chan_type)pending_msg->escam.fpc_pri_chan;
                }
                else
                {
                  packet->action.srch_ffpc.fpc_pri_chan = CAI_FPC_PRI_CHAN_CPCCH;
                }
              }
              else
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                  " No data available for FFPC action");
              }
            }
            else
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                " No data available for FFPC action");
            }
            break;
          }
          #endif

          default:
            M1X_MSG( DCP, LEGACY_ERROR,
              "Invalid Action %d ", action);
            break;

        }
        break;
      }

      default:
        M1X_MSG( DCP, LEGACY_ERROR,
          "Invalid Server Token %d", server_token );
        break;
    }
  }

} // mccsch_start_f_sch0_action_packet

/*===========================================================================

FUNCTION mccsch_start_f_sch0_post

DESCRIPTION
  Post-processor for "start f-sch 0" transaction.
  This function is called from context of the transaction server (RXC, SRCH).
  Must be brief and not be blocking.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mccsch_start_f_sch0_post
(
  mctrans_trans_info_type *   curr_trans
)
{
  event_start_burst event_info;
  const caii_srv_cfg_type  *config;

  /* Set MCC_TRANS_DONE_SIG on MC task for below cases
   * 1. If not infinite duration or stop, need to schedule a "stop" transaction
   * 2. OR If it is Tune Away case and inifinite duration */
  if ( ( sch_assign.forward[0].for_sch_duration != INFINITE_DURATION &&
         sch_assign.forward[0].for_sch_duration != 0 
       ) ||
       ( cdma.tc_data_ta.ta_type == SRCH_TA_LTA &&
         sch_assign.forward[0].for_sch_duration == INFINITE_DURATION
       )
     )
  {
    // Report to MC (bear in mind that this function is called from the server's
    // context) that this transaction is done, so that it will spawn the next
    // transaction (to stop the f_sch).
    mctrans_report_trans_done( curr_trans );
  }

  // Release stored ESCAM, not needed anymore.  Note that ESCAM may not have been stored,
  // but it's harmless to release anyway.  It saves a lot of checking to see if releasing
  // is needed
  mctrans_release_pending_msg( curr_trans->trans_num );

  // See if client wants to be informed of SCH configuration changes
  mccsch_inform_sch_change( MCCSCH_FORWARD, 0 );

  // Report proper event
 if ( sch_assign.forward[0].for_sch_duration == 0)
 {
   event_report(EVENT_END_FWD_SUPP_BURST_ASSGN);
 }
 else
 {
   config = snm_get_current_config();
   event_info.rate = (unsigned char) sch_assign.forward[0].cur_rate;
   event_info.coding = (unsigned char) config->for_sch[0].sch_chn_cfg.coding;
   event_info.rc = (unsigned char) config->for_sch[0].sch_chn_cfg.sch_rc;
   event_report_payload(EVENT_START_FWD_SUPP_BURST_ASSIGN,
                        sizeof(event_info),
                        (void *) &event_info);
 }

 /* Setting SCH related stats here. This is called in server context.
    If, in future, there is a need to set some signal then this code can
    be considered for moving to CP's context. */
 cdma_stats.channel_config_info.fsch0_active = sch_assign.forward[0].active;
 cdma_stats.channel_config_info.fsch0_duration = sch_assign.forward[0].for_sch_duration;
 cdma_stats.channel_config_info.fsch0_sccl_index = sch_assign.forward[0].sccl_index;
 cdma_stats.changed_bitmask |= SYS_CDMA_CHANNEL_CONFIG_INFO_BIT_MASK;

} // mccsch_start_f_sch0_post

/*===========================================================================

FUNCTION mccsch_stop_f_sch0_trans

DESCRIPTION
  Function to generate a transaction to stop the SCH 0 processing.  This is
  call after the completion of a start transaction and is called from the
  transaction client's (MC) context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_stop_f_sch0_trans( void )
{

  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_COUNT,                     // Transaction type
    MCTRANS_MODE_EXPLICIT,
    NULL,
    NULL,
    NULL
  };

  despatch_info.transaction_type = MCTRANS_F_SCH0_STOP;

  despatch_info.preprocessor = mccsch_stop_f_sch0_pre;
  despatch_info.postprocessor = mccsch_stop_f_sch0_post;
  despatch_info.action_packet_gen = mccsch_stop_f_sch0_action_packet;

  M1X_MSG( DCP, LEGACY_LOW,
    "Preparing F-SCH0 stop txn" );
  (void)mctrans_despatch_transaction( sch_assign.forward[0].stop_time, &despatch_info, 0 );
}
/*===========================================================================

FUNCTION mccsch_init_stop_f_sch0_trans

DESCRIPTION
  Function to generate a transaction to stop the SCH 0 processing.  This is
  call after the completion of a start transaction and is called from the
  transaction client's (MC) context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_init_stop_f_sch0_trans( mctrans_trans_info_type *   curr_trans )
{
#ifdef FEATURE_IS2000_REL_A
  mctrans_pending_action_msg_type* msg;
#endif

  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_F_SCH_ACTION);
  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_FFPC_ACTION);

  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_F_SCH_ACTION);
#ifdef FEATURE_IS2000_REL_A

   // Include FFPC action for SRCH only if escam is included and has
   // add_info_incl set to TRUE
   msg = mctrans_get_pending_msg(curr_trans->trans_num);

   if ((msg != NULL) && (msg->gen.msg_type == CAI_EXT_SUP_CHN_ASN_MSG))
   {
     if (msg->escam.add_info_incl)
     {
       mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_FFPC_ACTION);
     }
   }
#endif

}

/*===========================================================================

FUNCTION mccsch_stop_f_sch0_trans_now

DESCRIPTION
  Function to generate a transaction to stop the SCH 0 processing immediately.
  This is used after a SHO which drops the last leg on F-SCH (shouldn't have
  happened though).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_stop_f_sch0_trans_now( void )
{
  qword cur_time = {0, 0};

  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_COUNT,                     // Transaction type
    MCTRANS_MODE_EXPLICIT,
    NULL,
    NULL,
    NULL
  };

  despatch_info.transaction_type = MCTRANS_F_SCH0_STOP;

  despatch_info.preprocessor = mccsch_stop_f_sch0_pre;
  despatch_info.postprocessor = mccsch_stop_f_sch0_post;
  despatch_info.action_packet_gen = mccsch_stop_f_sch0_action_packet;

  // Get current time and...
  m1x_time_get_20ms_frame_offset_time( cur_time );
  // ...generate transaction to stop at that time.
  (void)mctrans_despatch_transaction( cur_time, &despatch_info, 0 );
}
/*===========================================================================

FUNCTION mccsch_stop_f_sch0_pre

DESCRIPTION
  Pre-processor for "stop f-sch 0" transaction.
  This function is called from context of the transaction server (RXC, SRCH).
  Must be brief and not be blocking.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mccsch_stop_f_sch0_pre
(
  mctrans_trans_info_type *   curr_trans
)
{
  // Add actions to the transaction
  mccsch_init_stop_f_sch0_trans(curr_trans);


  mccccl_code_channel_list.deactivate_f_sch( 0 );

  // Mark SCH as inactive
  sch_assign.forward[0].active = FALSE;

} //lint !e715 :function parameters not used here

/*===========================================================================

FUNCTION mccsch_stop_f_sch0_action_packet

DESCRIPTION
  The function generates the data needed for "action" in the action packet
  for the "stop f-sch" transaction.  It takes care of all actions involved
  in the transaction.
  This function is called from context of the transaction server (RXC, SRCH).
  Must be brief and not be blocking.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mccsch_stop_f_sch0_action_packet
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
    case MCTRANS_SERVER_RXC :
      switch ( action )
      {
        case MCTRANS_FFPC_ACTION :
        {
          const caii_non_neg_srv_cfg_type* nnscr;

          mccsch_prepare_escam_ffpc_action_packet( curr_trans, &packet->action.ffpc );

          // Since this is a stop SCH transaction, we have to revert back to fpc_mode_no_sch
          nnscr = snm_get_current_nnscr();
          packet->action.ffpc.fpc_mode.included = TRUE;
          if (mccccl_code_channel_list.fwd_fundicated_chan_is_active())
          {
            packet->action.ffpc.fpc_mode.value = (cai_fpc_mode_type) nnscr->fpc_mode;
          }
          else
          {
            packet->action.ffpc.fpc_mode.value = CAI_FPC_MODE_RESERVED;
          }
          packet->action.ffpc.sch_ffpc_op.mode = MCTRANS_FFPC_STOP; // Note: RC is not needed for stopping.
          break;
        }

        case MCTRANS_F_SCH_ACTION :
        {
          packet->action.rxc_f_sch.sch_id = 0;
          packet->action.rxc_f_sch.start = FALSE;
          packet->action.rxc_f_sch.for_sch_fer_rep = sch_assign.for_sch_fer_rep;
          break;
        }

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action );
          break;
      }
      break;

    case MCTRANS_SERVER_SRCH :
      switch (action)
      {
        case MCTRANS_F_SCH_ACTION:
          (void) mccccl_code_channel_list.generate_sch_aset( &packet->action.srch_f_sch );
          if (packet->action.srch_f_sch.sch[0].num_pilots)
          {
            const caii_srv_cfg_type  *config = snm_get_current_config();  // Get current configuration
            const mccccl_sccl_entry_type* sccl_entry =
                    mccccl_code_channel_list.get_sccl_entry( 0, sch_assign.forward[0].sccl_index );
            /* This is a case where we have back to back SCH burst,and the
               start pre-processor has already run before SRCH can
               execute the stop SCH action, so CCL will return non-zero
               SCH pilots, we better populate RC & rate correctly as SRCH
               is expecting good values for these if num_pilots > 0
            */
            M1X_MSG( DCP, LEGACY_HIGH,
              "B to B burst: Stop action pkt Found >0 SCH pilots");
            packet->action.srch_f_sch.sch[0].rc =
               (cai_radio_config_type)config->for_sch[0].sch_chn_cfg.sch_rc;
            // Make sure a valid pointer is returned.  If error happens, 0 is returned from
            // the function.
            if ( sccl_entry != 0 )
            {
              // set the data rate first.
              packet->action.srch_f_sch.sch[0].sch_rate = sccl_entry->sch_rate;

              // If data rate is 1X, force coding to convolutional
              if (packet->action.srch_f_sch.sch[0].sch_rate == CAI_SCH_RATE_RS1_1X_RS2_1X)
              {
                packet->action.srch_f_sch.sch[0].code_type = CAI_CONVOLUTIONAL_CODING;
              }
              else
              {
                // For data rate higher than 1X, use the assigned coding scheme.
                // This is the same for MuxPDU5 as for non-MuxPDU5 situations.
                packet->action.srch_f_sch.sch[0].code_type =
                  (cai_sch_coding_type) config->for_sch[0].sch_chn_cfg.coding;
              }
            }

          }
          break;

        #ifdef FEATURE_IS2000_REL_A
        case MCTRANS_FFPC_ACTION:
        {
          mctrans_pending_action_msg_type* pending_msg;
          pending_msg = mctrans_get_pending_msg( curr_trans->trans_num );
          if ( pending_msg != NULL )
          {
            if (pending_msg->escam.add_info_incl)
            {
              if (mccccl_code_channel_list.fwd_fundicated_chan_is_active())
              {
                packet->action.srch_ffpc.fpc_pri_chan = (cai_fpc_pri_chan_type)pending_msg->escam.fpc_pri_chan;
              }
              else
              {
                packet->action.srch_ffpc.fpc_pri_chan = CAI_FPC_PRI_CHAN_CPCCH;
              }
            }
            else
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                " No data available for FFPC action");
            }
          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              " No data available for FFPC action");
          }
          break;
        }
        #endif

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action);
          break;
      }
      break;

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid Server Token %d", server_token);
      break;

  }

} // mccsch_stop_f_sch0_action_packet

/*===========================================================================

FUNCTION mccsch_stop_f_sch0_post

DESCRIPTION
  Post-processor for "stop f-sch 0".
  This function is called from context of the transaction server (RXC, TXC, SRCH).
  Must be brief and not be blocking.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

/*lint -e818*/
static void mccsch_stop_f_sch0_post
(
  mctrans_trans_info_type *   curr_trans
)
{
  // Release stored message, if present.  Note that a message may not have been stored,
  // but it's harmless to release anyway.  It saves a lot of checking to see if releasing
  // is needed.
  mctrans_release_pending_msg( curr_trans->trans_num );

  // See if client wants to be informed of SCH configuration changes
  mccsch_inform_sch_change( MCCSCH_FORWARD, 0 );

  /* Event report: End of forward supplemental burst assignment */
  event_report(EVENT_END_FWD_SUPP_BURST_ASSGN);

  /* Setting SCH related stats here. This is called in server context.
     If, in future, there is a need to set some signal then this code can
     be considered for moving to CP's context. */
  cdma_stats.channel_config_info.fsch0_active = sch_assign.forward[0].active;
  cdma_stats.changed_bitmask |= SYS_CDMA_CHANNEL_CONFIG_INFO_BIT_MASK;

} // mccsch_stop_f_sch0_post
/*lint +e818*/

/*===========================================================================

FUNCTION mccsch_is_r_sch_finite_burst

DESCRIPTION
  Function to check whether specified sch_id reverse supplemental assignment
  is a finite burst.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccsch_is_r_sch_finite_burst
(
  uint8 sch_id
)
{
  if ((sch_assign.reverse[sch_id].rev_sch_duration != INFINITE_DURATION) &&
      (sch_assign.reverse[sch_id].rev_sch_duration != 0))
  {
    return (TRUE);
  }
  else
  {
    return (FALSE);
  }
} // mccsch_is_r_sch_finite_burst

/*===========================================================================

FUNCTION mccsch_is_use_t_add_abort_changed

DESCRIPTION
  Function to check whether we need to inform Searcher task of the update of
  USE_T_ADD_ABORT.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccsch_is_use_t_add_abort_changed
(
  mctrans_transaction_type trans_type
)
{
  if (((trans_type == MCTRANS_R_SCH0_START) &&
       (sch_assign.rev_parms.current_use_t_add_abort !=
        sch_assign.rev_parms.use_t_add_abort)) ||
      ((trans_type == MCTRANS_R_SCH0_STOP) &&
        sch_assign.rev_parms.current_use_t_add_abort))
  {
    return (TRUE);
  }
  else
  {
    return (FALSE);
  }
} //mccsch_is_use_t_add_abort_changed

/*===========================================================================

FUNCTION mccsch_start_r_sch0_pre

DESCRIPTION
  Pre-processor for "start r-sch 0" transaction.
  This function is called from context of the transaction server (TXC).
  Must be brief and not be blocking.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mccsch_start_r_sch0_pre
(
  mctrans_trans_info_type *   curr_trans
)
{
  // Add actions to the transaction
  mccsch_init_trans(curr_trans);

  // If this is a stop, invoke the corresponding "stop" routine.
  if ( sch_assign.reverse[0].rev_sch_duration == 0 )
  {
    mccccl_code_channel_list.deactivate_r_sch( 0 );

    // Mark SCH as inactive
    sch_assign.reverse[0].active = FALSE;
  }
  else
  {
    // Activate assignment for SCH 0
    mccccl_code_channel_list.activate_r_sch_assign( 0, sch_assign.reverse[0].new_rate );

    // Mark SCH as active
    sch_assign.reverse[0].active = TRUE;
  }

    /* 1xTA: For TA allowed cases do not void the R_SCH0_STOP txn 
   * for HHO in prog case because R_SCH0_STOP might have dispacted 
   * as part of HHO txns execution and HHO txns might got deferred 
   * to execute at later time for TA case.
   * So for this scenario by not voiding R_SCH0_STOP txn will make sure
   * R_SCH0_STOP txn executed before HHO txns executed to avoid crash on FW
   * side while executing HHO begin while SCH0 is active.
   */
  if( (!srchtc_is_ta_supported()) ||
      (!mcc_is_hho_in_prog())
    )
  {
    // Just in case a stop transaction is schedule at exactly the same time but processed after
    // this start transaction, just go ahead and void the stop transaction here.  It's harmless
    // to do it even if it does not exist.
    mctrans_void_trans( MCTRANS_R_SCH0_STOP );
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
             "1xTA:Not voiding MCTRANS_R_SCH0_STOP, as HHO is in Prog");
  }
} //lint !e715

/*===========================================================================

FUNCTION mccsch_start_r_sch0_action_packet

DESCRIPTION
  The function generates the data needed for "action" in the action packet
  for the "start r-sch" transaction.  It takes care of all actions involved
  in the transaction.
  This function is called from context of the transaction server (TXC).
  Must be brief and not be blocking.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mccsch_start_r_sch0_action_packet
(
  mctrans_trans_info_type *   curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
)
{
  const caii_srv_cfg_type  *config = snm_get_current_config();  // Get current service configuration

  // Stamp action type in action packet
  packet->action_type = action;

  // If this is actually a stop, just use the action packet generator for stop transaction
  if ( sch_assign.reverse[0].rev_sch_duration == 0)
  {
    mccsch_stop_r_sch0_action_packet( curr_trans, action, server_token, packet );
  }
  else  // An actual "start" transaction
  {
    switch ( server_token )
    {
      case MCTRANS_SERVER_TXC :
      {
        switch ( action )
        {
          case MCTRANS_RLGC_ACTION :
            mccsch_escam_rlgc_action_packet( curr_trans, action, server_token, packet );

            if ((config->num_rev_sch != 0) && config->rev_sch_cc_incl)
            {
              packet->action.rlgc.sch_rlgc_op.mode = MCTRANS_RLGC_START;
              /* Assume SCH 0 here since we support only 1 SCH */
              packet->action.rlgc.sch_rlgc_op.rc =
              (cai_radio_config_type)config->rev_sch[0].sch_chn_cfg.sch_rc;
            }

            if ( (packet->action.rlgc.sch_rlgc_op.sch_rate =
                  sch_assign.reverse[0].new_rate) == CAI_SCH_RATE_RS1_1X_RS2_1X )
            {
              // For 1X SCH, code type is always convolutional
              packet->action.rlgc.sch_rlgc_op.sch_code_type = CAI_CONVOLUTIONAL_CODING;
            }
            else
            {
              packet->action.rlgc.sch_rlgc_op.sch_code_type =
                (cai_sch_coding_type) config->rev_sch[0].sch_chn_cfg.coding;
            }
            break;

          case MCTRANS_R_SCH_ACTION :
          {

            packet->action.txc_r_sch.sch_id = 0;
            packet->action.txc_r_sch.mode = MCTRANS_R_SCH_START;
            packet->action.txc_r_sch.rc = (cai_radio_config_type)config->rev_sch[0].sch_chn_cfg.sch_rc;
            packet->action.txc_r_sch.walsh_id = mccccl_code_channel_list.get_r_sch_walsh( 0,
                                                sch_assign.reverse[0].new_rate );  /*lint !e641 */
            packet->action.txc_r_sch.sch_rate = sch_assign.reverse[0].new_rate;
            packet->action.txc_r_sch.rev_sch_dtx_duration = sch_assign.rev_parms.rev_sch_dtx_duration;

#ifdef FEATURE_IS2000_REL_A //32X
                        // Some special code here for MuxPDU 5.
            if ((config->rev_sch[0].rev_sch_mux > CAI_MUX16) &&
                 (((config->rev_sch[0].rev_sch_mux & CAI_MO_MUX_PDU_TYPE_MASK) >> 10)
                   == CAI_MUX_PDU_TYPE_5))  /*lint !e641 */
            {
              if ( packet->action.txc_r_sch.sch_rate == CAI_SCH_RATE_RS1_1X_RS2_1X )
              {
                // For 1X SCH, code type is always convolutional...
                packet->action.txc_r_sch.turbo_code = CAI_CONVOLUTIONAL_CODING;
              }
              else
              {
                packet->action.txc_r_sch.turbo_code =
                  (cai_sch_coding_type) config->rev_sch[0].sch_chn_cfg.coding;
              }
              // MuxPDU size is variable for type 5. This info is not useful.
              // Just default it to FALSE.
              packet->action.txc_r_sch.double_size_pdu = FALSE;
              packet->action.txc_r_sch.mux_pdu_type = CAI_MUX_PDU_TYPE_5;

              //Set number of LTU and LTU length if convolutional coding is used.
              if ((packet->action.txc_r_sch.turbo_code == CAI_CONVOLUTIONAL_CODING) &&
                      (((uint32) packet->action.txc_r_sch.sch_rate) < NUM_DEFINED_RATE))
              {
                //Look up tables for number of LTUs and LTU length.
                packet->action.txc_r_sch.num_ltu =
                          def_num_ltu_table[packet->action.txc_r_sch.sch_rate]
                           [RATE_SET_OF_REV_RC(packet->action.txc_r_sch.rc)];
                packet->action.txc_r_sch.ltu_len =
                          def_pdu5_ltu_len_table[packet->action.txc_r_sch.sch_rate]
                           [RATE_SET_OF_REV_RC(packet->action.txc_r_sch.rc)];
              }
              else
              {
                //LTU is not used in Turbo coding.
                packet->action.txc_r_sch.num_ltu = 0;
                packet->action.txc_r_sch.ltu_len = 0;
              }
            }
            else //For MuxPDU 1, 2, 3
#endif // FEATURE_IS2000_REL_A
            {
              if ( packet->action.txc_r_sch.sch_rate == CAI_SCH_RATE_RS1_1X_RS2_1X )
              {
                // For 1X SCH, code type is always convolutional...
                packet->action.txc_r_sch.turbo_code = CAI_CONVOLUTIONAL_CODING;
                // ... and PDU size is always single
                packet->action.txc_r_sch.double_size_pdu = FALSE;
                packet->action.txc_r_sch.mux_pdu_type = CAI_MUX_PDU_TYPE_1_2_4;
              }
              else
              {
                packet->action.txc_r_sch.turbo_code =
                  (cai_sch_coding_type) config->rev_sch[0].sch_chn_cfg.coding;
                packet->action.txc_r_sch.double_size_pdu = (config->rev_sch[0].rev_sch_mux >> 8) & 0x3;
                packet->action.txc_r_sch.mux_pdu_type = (cai_mux_pdu_type_type)((config->rev_sch[0].rev_sch_mux >> 10) & 0x3);
              }

              // If convolutional coding is used and LTU is used, then LTU length
              // is 368 bits for RS1, 560 bits for RS2.
              // check for valid range of sch_rate value
              if ((packet->action.txc_r_sch.turbo_code == CAI_CONVOLUTIONAL_CODING) &&
                                    (((uint32) packet->action.txc_r_sch.sch_rate) < NUM_DEFINED_RATE))
              {
                packet->action.txc_r_sch.num_ltu =
                                          def_num_ltu_table[packet->action.txc_r_sch.sch_rate]
                                                                                    [RATE_SET_OF_REV_RC(packet->action.txc_r_sch.rc)];
                if (packet->action.txc_r_sch.num_ltu != 0)
                {
                  //LTU length is fixed for MuxPDU type 3.
                  //368 bits for rate set 1. 560 bits for rate set 2.
                  if ((RATE_SET_OF_REV_RC(packet->action.txc_r_sch.rc)+1) == CAI_MO_RATE_SET_1 ) /*lint !e641 */
                  {
                    packet->action.txc_r_sch.ltu_len = MUX_PDU_3_RS1_LTU_LEN;
                  }
                  else if ((RATE_SET_OF_REV_RC(packet->action.txc_r_sch.rc)) == (CAI_MO_RATE_SET_2 - 1)) /*lint !e641 !e650 */
                  {
                    packet->action.txc_r_sch.ltu_len = MUX_PDU_3_RS2_LTU_LEN;
                  }
                  else
                  {
                    // The code should not be here, because the macro
                    // returns either 0, or 1.
                    ERR_FATAL("Invalid RS", 0, 0, 0);
                  }
                }
                else
                {
                  // Number of LTU from the lookup table is 0, LTU
                  // is not used, set LTU_LEN to 0.
                  packet->action.rxc_f_sch.ltu_len = 0;
                }
              }
              else
              {
                // Turbo coding does not use LTU.
                packet->action.rxc_f_sch.num_ltu = 0;
                packet->action.rxc_f_sch.ltu_len = 0;
              }
            }

#ifdef T_WINNT
              #error code not present
#endif /* T_WINNT */

            // Update current rate
            sch_assign.reverse[0].cur_rate = sch_assign.reverse[0].new_rate;
            break;
          } // of case MCTRANS_R_SCH_ACTION
          case MCTRANS_GATING_ACTION:
          {
            packet->action.txc_gating.rev_fch_gating_on = FALSE;
            break;
          }
          default:
            M1X_MSG( DCP, LEGACY_ERROR,
              "Invalid action %d", action );
            break;
        }
        break;
      } // of case MCTRANS_SERVER_TXC

      default:
        M1X_MSG( DCP, LEGACY_ERROR,
          "Invalid Server Token %d", server_token );
        break;
    }
  }

} // mccsch_start_r_sch0_action_packet

/*===========================================================================

FUNCTION mccsch_start_r_sch0_post

DESCRIPTION
  Post-processor for "start r-sch 0" transaction.
  This function is called from context of the transaction server (TXC).
  Must be brief and not be blocking.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mccsch_start_r_sch0_post
(
  mctrans_trans_info_type *   curr_trans
)
{
  event_start_burst event_info;
  const caii_srv_cfg_type  *config;

  /* Set MCC_TRANS_DONE_SIG on MC task for below cases
   * 1. If USE_T_ADD_ABORT changed, need to schedule a "update" transaction OR
   * 2. If not infinite duration or stop, need to schedule a "stop" transaction
   * 3. OR If it is Tune Away case and inifinite duration */
  if (mccsch_is_r_sch_finite_burst(0) ||
       mccsch_is_use_t_add_abort_changed(MCTRANS_R_SCH0_START) ||
       ( cdma.tc_data_ta.ta_type == SRCH_TA_LTA &&
         sch_assign.reverse[0].rev_sch_duration == INFINITE_DURATION
       )
     )
  {
    // Report to MC (bear in mind that this function is called from the server's
    // context) that this transaction is done, so that it will spawn the next
    // transaction (to update USE_T_ADD_ABORT for the r_sch, to stop the r_sch
    // or to extend burst assignment).
    mctrans_report_trans_done(curr_trans);
  }

  // Release stored ESCAM, not needed anymore.  Note that ESCAM may not have been stored,
  // but it's harmless to release anyway.  It saves a lot of checking to see if releasing
  // is needed
  mctrans_release_pending_msg( curr_trans->trans_num );

  // See if client wants to be informed of SCH configuration changes
  mccsch_inform_sch_change( MCCSCH_REVERSE, 0 );

  // Report proper event
 if ( sch_assign.reverse[0].rev_sch_duration == 0)
 {
   event_report(EVENT_END_REV_SUPP_BURST_ASSGN);
 }
 else
 {
   config = snm_get_current_config();
   event_info.rate = (unsigned char) sch_assign.reverse[0].cur_rate;
   event_info.coding = (unsigned char) config->rev_sch[0].sch_chn_cfg.coding;
   event_info.rc = (unsigned char) config->rev_sch[0].sch_chn_cfg.sch_rc;
   event_report_payload(EVENT_START_REV_SUPP_BURST_ASSIGN,
                        sizeof(event_info),
                        (void *) &event_info);
 }

 /* Setting SCH related stats here. This is called in server context.
    If, in future, there is a need to set some signal then this code can
    be considered for moving to CP's context. */
 cdma_stats.channel_config_info.rsch0_active = sch_assign.reverse[0].active;
 cdma_stats.channel_config_info.rsch0_duration = sch_assign.reverse[0].rev_sch_duration;
 cdma_stats.changed_bitmask |= SYS_CDMA_CHANNEL_CONFIG_INFO_BIT_MASK;

} // mccsch_start_r_sch0_post

/*===========================================================================

FUNCTION mccsch_stop_r_sch0_trans

DESCRIPTION
  Function to generate a transaction to stop the R-SCH 0 processing.  This is
  called after the completion of a start transaction and is called from the
  transaction client's (MC) context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_stop_r_sch0_trans( void )
{
  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_COUNT,                     // Transaction type
    MCTRANS_MODE_EXPLICIT,
    NULL,
    NULL,
    NULL
  };

  despatch_info.transaction_type = MCTRANS_R_SCH0_STOP;

  despatch_info.preprocessor = mccsch_stop_r_sch0_pre;
  despatch_info.postprocessor = mccsch_stop_r_sch0_post;
  despatch_info.action_packet_gen = mccsch_stop_r_sch0_action_packet;

  (void)mctrans_despatch_transaction( sch_assign.reverse[0].stop_time, &despatch_info, 0 );
}
/*===========================================================================

FUNCTION mccsch_init_stop_r_sch0_trans

DESCRIPTION
  Function to generate a transaction to stop the R-SCH 0 processing.  This is
  called after the completion of a start transaction and is called from the
  transaction client's (MC) context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_init_stop_r_sch0_trans( mctrans_trans_info_type * curr_trans )
{
  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_R_SCH_ACTION);
  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_RLGC_ACTION);

  if (cdma.rev_fch_gating_on)
  {
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_GATING_ACTION);
  }

}

/*===========================================================================

FUNCTION mccsch_stop_r_sch0_pre

DESCRIPTION
  Pre-processor for "stop r-sch 0" transaction.
  This function is called from context of the transaction server (TXC).
  Must be brief and not be blocking.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mccsch_stop_r_sch0_pre
(
  mctrans_trans_info_type * curr_trans
)
{
  // Add actions to the transaction
  mccsch_init_stop_r_sch0_trans(curr_trans);

  mccccl_code_channel_list.deactivate_r_sch( 0 );

  // Mark SCH as inactive
  sch_assign.reverse[0].active = FALSE;

  /* Since we are stopping R-SCH0, void any extensions that were scheduled */
  mctrans_void_trans( MCTRANS_R_SCH0_EXTEND );

} //lint !e715 :function parameters not used here

/*===========================================================================

FUNCTION mccsch_stop_r_sch0_action_packet

DESCRIPTION
  The function generates the data needed for "action" in the action packet
  for the "stop r-sch" transaction.  It takes care of all actions involved
  in the transaction.
  This function is called from context of the transaction server (TXC).
  Must be brief and not be blocking.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mccsch_stop_r_sch0_action_packet
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
    case MCTRANS_SERVER_TXC :
    {
      switch ( action )
      {
        case MCTRANS_RLGC_ACTION :
          mccsch_escam_rlgc_action_packet( curr_trans, action, server_token, packet );
          packet->action.rlgc.sch_rlgc_op.mode = MCTRANS_RLGC_STOP;
          break;

        case MCTRANS_R_SCH_ACTION :
          packet->action.txc_r_sch.sch_id = 0;
          packet->action.txc_r_sch.mode = MCTRANS_R_SCH_STOP;
          break;

        case MCTRANS_GATING_ACTION:
        {
          const caii_srv_cfg_type  *config = snm_get_current_config();
          mcc_rev_chan_mask_type rev_chan_mask =
            mccccl_code_channel_list.get_rev_chan_mask();

          /* Now that we are tearing down R-SCH, it would be okay
          to turn FCH gating on if some conditions are met.
          Turn Reverse FCH gating on only if we have been
          requested by the BS and if the DCCH is not present. */
          /* R-FCH 1/8 rate gating is not allowed when there are any other
          reverse channels (except pilot).  As for F-PDCH, since R-CQICH
          transmission must be continuous (assuming not in CH), the pilot
          cannot be gated and as a result no R-FCH 1/8 rate gating */
          packet->action.txc_gating.rev_fch_gating_on =
            ((!(rev_chan_mask & MCC_R_DCCH)) &&  /*lint !e655 */
            config->for_fch_rc >= CAI_RC_3 && cdma.rev_fch_gating_on);  /*lint !e641 */
          break;
        }

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action );
          break;
      }
      break;
    } // of case MCTRANS_SERVER_TXC

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid Server Token %d", server_token );
      break;
  }

} // mccsch_stop_r_sch0_action_packet

/*===========================================================================

FUNCTION mccsch_stop_f_sch0_post

DESCRIPTION
  Post-processor for "stop r-sch 0".
  This function is called from context of the transaction server (RXC, TXC, SRCH).
  Must be brief and not be blocking.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mccsch_stop_r_sch0_post
(
  mctrans_trans_info_type * curr_trans
)
{
  // USE_T_ADD_ABORT should be turned off, need to schedule an "update"
  // transaction
  if (mccsch_is_use_t_add_abort_changed(MCTRANS_R_SCH0_STOP))
  {
    // Report to MC (bear in mind that this function is called from the server's
    // context) that this transaction is done, so that it will spawn the next
    // transaction (to update USE_T_ADD_ABORT for the r_sch).
    mctrans_report_trans_done( curr_trans );
  }

  // Release stored message, if present.  Note that a message may not have been stored,
  // but it's harmless to release anyway.  It saves a lot of checking to see if releasing
  // is needed.
  mctrans_release_pending_msg( curr_trans->trans_num );

  // See if client wants to be informed of SCH configuration changes
  mccsch_inform_sch_change( MCCSCH_REVERSE, 0 );

  /* Event report: End of reverse supplemental burst assignment */
  event_report(EVENT_END_REV_SUPP_BURST_ASSGN);

  /* Setting SCH related stats here. This is called in server context.
     If, in future, there is a need to set some signal then this code can
     be considered for moving to CP's context. */
  cdma_stats.channel_config_info.rsch0_active = sch_assign.reverse[0].active;
  cdma_stats.changed_bitmask |= SYS_CDMA_CHANNEL_CONFIG_INFO_BIT_MASK;

} // mccsch_stop_r_sch0_post

/*===========================================================================

FUNCTION mccsch_r_sch0_extend_trans

DESCRIPTION
  Function to generate a transaction to notify DS that it's time to issue a
  burst extension if necessary (more data to send).  This is called after the
  completion of a start transaction and is called from the transaction
  client's (MC) context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_begin_r_sch0_extend_trans( void )
{
  qword trans_time = {0, 0};

  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_COUNT,                     // Transaction type
    MCTRANS_MODE_EXPLICIT,
    NULL,
    NULL,
    NULL
  };

  /* initialize the trans time to zero */
  qw_set(trans_time, 0, 0);

  if ( !sch_callback.rsch_extension_cb )
  {
    // If burst extension callback is not registered, just don't do anything
    return;
  }

  // First see if the duration is shorter than the trigger time.  If so, we need to set the
  // transaction time to now.
  if ( sch_callback.trigger_time >= duration_table[sch_assign.reverse[0].rev_sch_duration] )
  {
    m1x_time_get_20ms_frame_offset_time( trans_time );
  }
  else
  {
    qword trigger_t = {0, 0};

    // First set quadword trigger_t to trigger_time
    trigger_t[1] = 0;
    trigger_t[0] = sch_callback.trigger_time;

    // If not, the transaction time is end of burst - trigger time
    qw_sub( trans_time, sch_assign.reverse[0].stop_time, trigger_t );
  }

  despatch_info.transaction_type = MCTRANS_R_SCH0_EXTEND;

  despatch_info.preprocessor = mccsch_r_sch0_extend_pre;
  despatch_info.postprocessor = NULL;
  despatch_info.action_packet_gen = NULL;

  M1X_MSG( DCP, LEGACY_HIGH,
    "Despatch R-SCH0 extension at %lx",
    trans_time[0] );
  (void)mctrans_despatch_transaction( trans_time, &despatch_info, 0 );
}
/*===========================================================================

FUNCTION mccsch_init_r_sch0_extend_trans

DESCRIPTION
  Function to generate a transaction to notify DS that it's time to issue a
  burst extension if necessary (more data to send).  This is called after the
  completion of a start transaction and is called from the transaction
  client's (MC) context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_init_r_sch0_extend_trans( mctrans_trans_info_type * curr_trans )
{
  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_RXC, MCTRANS_NULL_ACTION);

}

/*===========================================================================

FUNCTION mccsch_r_sch0_extend_pre

DESCRIPTION
  Pre-processor for "r-sch 0 extend" transaction.
  This function is called from context of the transaction server (TXC).
  Must be brief and not be blocking.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mccsch_r_sch0_extend_pre
(
  mctrans_trans_info_type * curr_trans
)
{
  /* Add actions to the transaction
  */
  mccsch_init_r_sch0_extend_trans(curr_trans);

  // Calls callback for burst extension if registered
  if ( sch_callback.rsch_extension_cb )
  {
    sch_callback.rsch_extension_cb();
  }

} //lint !e715

/*===========================================================================

FUNCTION mccsch_update_r_sch_parm_pre

DESCRIPTION
  Pre-processor for "r-sch 0 Update" transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mccsch_update_r_sch_parm_pre(
  mctrans_trans_info_type * curr_trans
)
{
  mccsch_init_update_r_sch_parm_trans(curr_trans);

}

/*===========================================================================

FUNCTION mccsch_update_r_sch_parm_action_packet

DESCRIPTION
  The function generates the data needed for "action" in the action packet
  for the "update r-sch parm" transaction.  It takes care of all actions
  involved in the transaction.
  This function is called from context of the transaction server (TXC or SRCH).
  Must be brief and not be blocking.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mccsch_update_r_sch_parm_action_packet
(
  mctrans_trans_info_type * curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
)
{
  MODEM_1X_UNUSED(curr_trans);

  // Stamp action type in action packet
  packet->action_type = action;

  switch ( server_token )
  {
    case MCTRANS_SERVER_TXC :
    {
      switch ( action )
      {
        case MCTRANS_R_SCH_ACTION :
        {
          packet->action.txc_r_sch.mode = MCTRANS_R_SCH_DTX;
          packet->action.txc_r_sch.rev_sch_dtx_duration =
            sch_assign.rev_parms.rev_sch_dtx_duration;
          break;
        }

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action );
          break;
      }
      break;
    } // of case MCTRANS_SERVER_TXC

    case MCTRANS_SERVER_SRCH:
    {
      switch ( action )
      {
        case MCTRANS_R_SCH_ACTION :
        {
          if (mccsch_sch_is_active(MCCSCH_REVERSE, 0))
          {
            packet->action.srch_r_sch.use_t_add_abort    =
            sch_assign.rev_parms.current_use_t_add_abort =
              sch_assign.rev_parms.use_t_add_abort;
          }
          else
          {
            packet->action.srch_r_sch.use_t_add_abort    =
            sch_assign.rev_parms.current_use_t_add_abort = FALSE;
          }
          packet->action.srch_r_sch.t_mulchan = mccds_get_t_mulchan();
          break;
        }

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid Action %d", action );
          break;
      }
      break;
    } // of case MCTRANS_SERVER_SRCH

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid Server Token %d", server_token);
      break;

  }

}

/*===========================================================================

FUNCTION mccsch_update_r_sch_parm_trans

DESCRIPTION
  Function to generate a transaction to notify Searcher to update
  USE_T_ADD_ABORT. This is called after the completion of a start transaction
  and is called from the transaction client's (MC) context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_update_r_sch_parm_trans( void )
{
  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_COUNT,                     // Transaction type
    MCTRANS_MODE_EXPLICIT,
    NULL,
    NULL,
    NULL
  };

  // Start rightaway
  m1x_time_get_20ms_frame_offset_time(sch_assign.reverse[0].start_time);

  despatch_info.transaction_type = MCTRANS_R_SCH_UPDATE;

  despatch_info.preprocessor = mccsch_update_r_sch_parm_pre;
  despatch_info.postprocessor = NULL;
  despatch_info.action_packet_gen = mccsch_update_r_sch_parm_action_packet;

  // Now despatch the transaction.
  (void) mctrans_despatch_transaction(sch_assign.reverse[0].start_time,
    &despatch_info , NULL);

  M1X_MSG( DCP, LEGACY_MED,
    "ESCAM:Upd RSCH (SRCH) at %lx",
    sch_assign.reverse[0].start_time[0]);
} // mccsch_update_r_sch_parm_trans
/*===========================================================================

FUNCTION mccsch_init_update_r_sch_parm_trans

DESCRIPTION
  Function to generate a transaction to notify Searcher to update
  USE_T_ADD_ABORT. This is called after the completion of a start transaction
  and is called from the transaction client's (MC) context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_init_update_r_sch_parm_trans( mctrans_trans_info_type * curr_trans )
{
  mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_SRCH, MCTRANS_R_SCH_ACTION);

  // Check to see if reverse supplemental burst is active
  if (mccsch_sch_is_active(MCCSCH_REVERSE, 0))
  {
    // It is active. Hence include R_SCH_ACTION for TXC as well.
    mctrans_add_action_to_server(curr_trans, MCTRANS_SERVER_TXC, MCTRANS_R_SCH_ACTION);
  }

} // mccsch_update_r_sch_parm_trans

/*===========================================================================

FUNCTION mccsch_update_sch_assign_with_uhdm

DESCRIPTION
  Update SCH assignment that came from a UHDM.  The information is stored locally
  in a data structure sch_assign.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

/*lint -e818*/
void mccsch_update_sch_assign_with_uhdm
(
  caii_ho_msg_type* uhdm
)
{

  uint8 bn, sch;

  // Forward SCH assignment
  for ( bn = 0; bn < uhdm->num_for_assign; ++bn )
  {
    // Retrive sch ID and SCCL index for ease of reading
    sch = uhdm->for_assign[bn].for_sch_id;

    // Store duration and index for use later.  Note that the start time is not used here.
    // The entity that processes the message take care of scheduling the start time.
    sch_assign.forward[sch].for_sch_duration = uhdm->for_assign[bn].for_sch_duration;
    sch_assign.forward[sch].sccl_index = uhdm->for_assign[bn].sccl_index;
    // UHDM can't assign for_sch_fer_rep, Stardard says just use whatever we had before.  So don't touch it.
  }
  // ...and now reverse SCH assignment
  for ( bn = 0; bn < uhdm->num_rev_assign; ++bn )
  {
    // Retrive sch ID and SCCL index for ease of reading
    sch = uhdm->rev_assign[bn].rev_sch_id;

    // Store duration and index for use later.  Note that the start time is not used here.
    // The entity that processes the message take care of scheduling the start time.
    sch_assign.reverse[sch].rev_sch_duration = uhdm->rev_assign[bn].rev_sch_duration;
    sch_assign.reverse[sch].new_rate =
      (cai_sch_rate_type) uhdm->rev_assign[bn].rev_sch_rate;
  }

} // mccsch_update_sch_assign_with_uhdm
/*lint +e818*/

/*===========================================================================

FUNCTION mccsch_compute_start_time

DESCRIPTION
  Computes the start time of a start/stop SCH transaction.  "start_time"
  is as specified in the Standard which is in units of "start_time_unit+1",
  i.e. (abs_start_time/(start_time_unit+1) - start_time)mod 32 = 0.  Since
  start_time is always less than 32, we have :

    (abs_start_time/time_unit)mod 32 - start_time = 0

  where time_unit = start_time_unit - 1

  Let cur_time be the nearest multiple of time_unit less than the actual
  current time,

    ((cur_time+wait_time)/time_unit) mod 32 - start_time = 0

    ((cur_time/time_unit)mode 32 + (wait_time/time_unit)mod 32)mod 32 - start_time = 0

  wait_time is chosen such that it is the smallest value that satisfies the above
  equation, so we are sure that wait_time/time_unit is less that 32, so

    ((cur_time/time_unit)mode 32 + wait_time/time_unit)mod 32 = start_time

  If (cur_time/time_unit)mode 32 is less than start_time, then:

    (cur_time/time_unit)mode 32 + wait_time/time_unit = start_time

    wait_time = (start_time - (cur_time/time_unit)mode 32)*time_unit

  If (cur_time/time_unit)mode 32 >= start_time, then:

    wait_time = (start_time + 32 - (cur_time/time_unit)mode 32)*time_unit

  So we have:
  abs_start_time = cur_time + wait_time;

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_compute_start_time
(
  qword               ref_time,
  uint8               start_time,
  uint8               sch_id,
  mccsch_ch_dir_type  dir
)
{
  qword   abs_start_time = {0, 0}, cur_time = {0, 0}, tmp_time = {0, 0};
  dword   wait_time;
  uint8   mod32_time;
  uint8   time_unit = sch_assign.start_time_unit+1;

  /* initialize the current time to zero */
  qw_set(cur_time, 0, 0);

  // Get current time
  m1x_time_get_20ms_frame_offset_time(cur_time);

  // Calculate start time based on ref_time
  qw_equ( abs_start_time, ref_time );

  // Compute wait time if not implicit action time
  if ( start_time != MCCTCHO_IMPLICIT_ACTION_TIME )
  {
    (void)qw_div( tmp_time, abs_start_time, time_unit );

    // Lower 5 bits of tmp_time[0] is tmp_time mod 32
    mod32_time = (uint8) (qw_lo( tmp_time ) & 0x1f);
    // mod32_time is now (cur_time/time_unit)mode 32

    if ( mod32_time > start_time )
    {
      start_time += 32;
    }

    wait_time = ( start_time - mod32_time )*time_unit;

    // Now figure out the cur_time as defined above
    qw_mul( tmp_time, tmp_time, time_unit );

    // Put wait_time in abs_start_time to prepare for addition
    qw_set( abs_start_time, 0UL, wait_time );
  }

  // Add tmp_time to abs_start_time to get the actual start time.
  qw_add( abs_start_time, abs_start_time, tmp_time );

  if ( dir == MCCSCH_FORWARD )
  {
    // For forward burst and if we are already at the frame we are supposed to start,
    // delay and shorten the burst by 1 frame since we are sure the frame will be erased.
    // This prevents the problem of having the decoder ready by not the demodulator, which
    // can cause old frames to be read.
    // Note: we only need to do this when duration is non-zero. SCH is assumed to
    // be SCH 0.
    if ( qw_cmp( abs_start_time, cur_time ) == 0 &&
         sch_assign.forward[0].for_sch_duration != 0 )
    {
      // We are already at the start time of the burst, so move the
      // start time 1 frame into the future.
      qw_inc( abs_start_time, 1 );
      M1X_MSG( DCP, LEGACY_ERROR,
        "Delayed burst by 1 frames." );
      sch_assign.forward[sch_id].start_time_shift = 1;
    }
    else
    {
      sch_assign.forward[sch_id].start_time_shift = 0;
    }
    qw_equ( sch_assign.forward[sch_id].start_time, abs_start_time );
  }
  else
  {
    qw_equ( sch_assign.reverse[sch_id].start_time, abs_start_time );
  }
} // mccsch_compute_start_time

/*===========================================================================

FUNCTION mccsch_compute_stop_time

DESCRIPTION
  Computes the stop time of a SCH burst assignment based on "duration" value,
  as defined in the Standard.


DEPENDENCIES
  Start time must have been computed before.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_compute_stop_time
(
  uint8               duration,
  uint8               sch_id,
  mccsch_ch_dir_type  dir
)
{
  qword actual_duration = {0, 0}, stop_time = {0, 0};
  uint8 shift;

  // First, if duration is infinite, we can just return
  if ( duration == INFINITE_DURATION )
  {
    return;
  }

  // Now get the start_time
  if ( dir == MCCSCH_FORWARD )
  {
    qw_equ( stop_time, sch_assign.forward[sch_id].start_time );
    shift = sch_assign.forward[sch_id].start_time_shift;
  }
  else
  {
    qw_equ( stop_time, sch_assign.reverse[sch_id].start_time );
    shift = 0;
  }

  // Convert duration in actual frame time and compute stop time
  // Adjust duration by "shift" value
  qw_set( actual_duration, (dword) (0UL), (dword) (duration_table[duration] - shift) );
  qw_add( stop_time, stop_time, actual_duration );

  // Now store the stop time so that we can use it later (in MC context) when
  // generating the stop transaction.
  if ( dir == MCCSCH_FORWARD )
  {
    qw_equ( sch_assign.forward[sch_id].stop_time, stop_time );
  }
  else
  {
    qw_equ( sch_assign.reverse[sch_id].stop_time, stop_time );
  }
} // mccsch_compute_stop_time

/*===========================================================================

FUNCTION mccsch_process_escam

DESCRIPTION
  Process message ESCAM received in various states in traffic channel.  It
  first validates the message and sends reject code if not valid.  Then it
  will go ahead a actually process the content of the message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_process_escam
(
  caii_escam_type* escam
)
{
  #ifdef FEATURE_IS2000_REL_A
  uint8 max_rate;
  #endif

  size_t copy_size;

  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_COUNT,                     // Transaction type
    MCTRANS_MODE_EXPLICIT,
    NULL,
    NULL,
    NULL
  };

  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
  uint8 bn, error = 0;
  uint8 num_f_sch_supported = mccap_num_f_sch_supported();
  uint8 num_r_sch_supported = mccap_num_r_sch_supported();
  const caii_srv_cfg_type  *config = snm_get_current_config();

#ifdef FEATURE_PPP_CLEANUP_SV_MODE
  /* PPP_CLEANUP_SV_MODE: If this is a special service originated data call
   * then reject ESCAM */
  if( cdma.is_special_srv_data_call )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "ESCAM rejected for spl srv orig data call");
    error = CAI_REJ_STATE;
    goto function_end; /*lint !e801 */
  } /* End  if( cdma.is_special_srv_data_call ) */
#endif /* FEATURE_PPP_CLEANUP_SV_MODE */

  if (!P_REV_IS_6_OR_GREATER)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "ESCAM received in wrong P_REV");
    error = CAI_REJ_CODE;
    goto function_end; /*lint !e801 */
  }

  /* If service option is not Data SO, send Reject order with indication
  ** that message cannot be handled by current mobile station configuration */
  if(  CAI_IS_SO_SMS(cdma.so_req) ||
       CAI_IS_SO_VOICE(cdma.so_req) )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "ESCAM received - SO %d is not data SO",
      cdma.so_req);
    error = CAI_REJ_CFG;
    goto function_end; /*lint !e801 */
  }

  // 1. See if we get any invalid values
  // Flag "capability not supported" if too many SCH or SCH 1 specified
  // Note: only SCH 0 assumed here
  if ( (escam->num_for_sch > num_f_sch_supported) ||
       ((escam->num_for_sch > 0) &&( escam->for_sch[0].for_sch_id == 1)) ||
       (escam->num_rev_sch > num_r_sch_supported) ||
       ((escam->num_rev_sch > 0) && (escam->rev_sch[0].rev_sch_id == 1))
     )
  {
    error = CAI_REJ_CAP;
    goto function_end; /*lint !e801 */
  }

#ifdef FEATURE_IS2000_REL_B
  if ( P_REV_IS_8_OR_GREATER && escam->for_sch_cc_incl
       &&
       ((escam->num_for_sch==0) || (escam->for_sch[0].for_sch_duration==0))
     )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "F-SCH config with no assignment!" );
    error=CAI_REJ_FIELD;
    goto function_end; /*lint !e801 */
  }
  if ( P_REV_IS_8_OR_GREATER && escam->rev_sch_cc_incl
       &&
       ((escam->num_rev_sch==0) || (escam->rev_sch[0].rev_sch_duration==0))
     )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "R-SCH config with no assignment!" );
    error=CAI_REJ_FIELD;
    goto function_end; /*lint !e801 */
  }
#endif

  if ( escam->num_for_sch > 0 )
  {
    const mccccl_sccl_entry_type* sccl_entry;

    //  If sch cc is not included but we are assigned SCH, then it's an
    // error.  Since we don't support SCH 1, we don't check for the SCH ID here.
    if ( !config->for_sch_cc_incl || config->num_for_sch == 0 )
    {
#ifdef FEATURE_IS2000_REL_B
      if (!P_REV_IS_8_OR_GREATER || !escam->for_sch_cc_incl)
#endif
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "ESCAM conflicts with fsch config!" );
        error=CAI_REJ_CFG;
        goto function_end;  /*lint !e801 */
      }
    }

#ifdef FEATURE_IS2000_REL_B
    if (P_REV_IS_8_OR_GREATER && escam->for_sch_cc_incl)
    {
      if (snm_validate_for_sch_cfg(escam->num_for_sch,
             (caii_for_sch_type *) escam->for_sch_cc) == SNM_FAILURE)
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          " Bad ESCAM fsch config!" );
        error=CAI_REJ_FIELD;
        goto function_end;  /*lint !e801 */
      }
    }
#endif
    // Now check to see if the SCCL entry is valid.  If it has never been downloaded before,
    // then num_pilots will be 0.
    // Note: SCH 0 assumed here
    sccl_entry = mccccl_code_channel_list.get_sccl_entry( 0, escam->for_sch[0].sccl_index );
    if ( sccl_entry->num_pilots == 0 )
    {
      // Initialize to error return
      error = CAI_REJ_CFG;

      // SCCL was not downloaded earlier, now we need to check if it is included in this message
      if ( escam->for_cfg_included )
      {
        for ( bn = 0; bn <= escam->num_for_cfg_recs; ++bn )
        {
          if ( escam->for_cfg[bn].sccl_index == escam->for_sch[0].sccl_index )
          {
            // The SCCL entry is included in the message, we are fine.
            error = 0;
            break;
          }
        }
      }

      if ( error )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "SCCL entry %d not valid!",
          escam->for_sch[0].sccl_index );
        goto function_end;  /*lint !e801 */
      }
    }
  }

  if ( escam->num_rev_sch > 0 )
  {
    if ( !config->rev_sch_cc_incl || config->num_rev_sch == 0 )
    {
#ifdef FEATURE_IS2000_REL_B
      if (!P_REV_IS_8_OR_GREATER || !escam->rev_sch_cc_incl)
#endif
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "ESCAM conflicts with rsch config!" );
        error=CAI_REJ_CFG;
        goto function_end;  /*lint !e801 */
      }
    }
#ifdef FEATURE_IS2000_REL_B
    if (P_REV_IS_8_OR_GREATER && escam->rev_sch_cc_incl)
    {
      if (snm_validate_rev_sch_cfg(escam->num_rev_sch,
             (caii_rev_sch_type *) escam->rev_sch_cc) == SNM_FAILURE)
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          " Bad ESCAM rsch config!" );
        error=CAI_REJ_FIELD;
        goto function_end;  /*lint !e801 */
      }
    }
#endif
  }

  // Flag "message field not in valid range" if pilot not in active set
  if ( escam->for_cfg_included )
  {
    // Go over num_for_sch_cfg+1 entries for error
    for ( bn = 0; bn <= escam->num_for_cfg_recs; ++bn )
    {
      uint8 pilot;

      // If wrong SCH ID is specified, we also reject the message with "capability
      // not supported" although this is not specified in the Standard.
      if ( escam->for_cfg[bn].for_sch_id >= num_f_sch_supported )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "SCH %d not supported!",
          escam->for_cfg[bn].for_sch_id );
        error = CAI_REJ_CAP;
        goto function_end;   /*lint !e801 */
      }

#ifdef FEATURE_IS2000_REL_A //32X
      // This checking is simple in release A. In the service config record of
      // release A, there is MAX_RATE field, which is the maximun data rate
      // that both BS and MS agreed upon. The rate came in ESCAM should not
      // be greater than MAX_RATE. If P_REV_IN_USE is 6 or below, max_rate is
      // set to default valud based on the mux option.
      max_rate = config->for_sch[0].sch_chn_cfg.max_rate;
#ifdef FEATURE_IS2000_REL_B
      if (P_REV_IS_8_OR_GREATER && escam->for_sch_cc_incl)
      {
        max_rate = escam->for_sch_cc[0].sch_chn_cfg.max_rate;
      }
#endif
      if ( escam->for_cfg[bn].for_sch_rate > max_rate)
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "F-Rate %d is > than MAX_RATE %d",
          escam->for_cfg[bn].for_sch_rate,
          max_rate );
        error = CAI_REJ_FIELD;
        goto function_end;   /*lint !e801 */
      }
#else /* FEATURE_IS2000_REL_A */
      // Validate the rate based on mux option negotiated.  We know that for_sch_cc must be
      // included if we are here.
      // Note: This is not specified in the standard, but we should do it anyway.
      // We support only SCH 0, so make that assumption here.
      if ( !mccsch_verify_rate( config->for_sch[0].for_sch_mux,
           (cai_sch_rate_type) escam->for_cfg[bn].for_sch_rate ) )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "Rate %d not allowed by for_sch_mux 0x%x",
          escam->for_cfg[bn].for_sch_rate,
          config->for_sch[0].for_sch_mux );
        error = CAI_REJ_FIELD;
        goto function_end;
      }
#endif /* FEATURE_IS2000_REL_A */

      // Note: num_sup_sho + 1 pilots
      for ( pilot = 0; pilot <= escam->for_cfg[bn].num_sup_sho; ++pilot )
      {
        if (!escam->for_cfg[bn].sup_sho[pilot].add_pilot_rec_incl)
        {
           escam->for_cfg[bn].sup_sho[pilot].active_pilot_rec_type =
             CAI_PILOT_1X_COMMON; /*lint !e641 */
        }
        else
        {
          if (!validate_pilot_rec_type(
            (cai_pilot_rec_type)escam->for_cfg[bn].sup_sho[pilot].active_pilot_rec_type,
            &escam->for_cfg[bn].sup_sho[pilot].add_pilot_rec))
          {
            error = CAI_REJ_CAP;
            goto function_end;  /*lint !e801 */
          }
        }
        // If found 1 pilot not in active set, return with error
        if ( !mccccl_code_channel_list.pilot_is_in_active_set(
            escam->for_cfg[bn].sup_sho[pilot].pilot_pn,
            (cai_pilot_rec_type)escam->for_cfg[bn].sup_sho[pilot].active_pilot_rec_type ,TRUE )
           )
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "SCH pilot %d not in active set!",
            escam->for_cfg[bn].sup_sho[pilot].pilot_pn );
          error = CAI_REJ_FIELD;
          goto function_end;  /*lint !e801 */
        }
      }
    }
  }

  // Now check if rate specified in R-SCH is too high.
  if ( escam->rev_cfg_included )
  {
#ifdef FEATURE_IS2000_REL_A //32X
    // This checking is simple in release A. In the service config record of
    // release A, there is MAX_RATE field, which is the maximun data rate
    // that both BS and MS agreed upon. The rate came in ESCAM should not
    // be greater than MAX_RATE. For P_REV_IN_USE <=6, max_rate is set to
    // default value based on the MO. Safe to use.
    max_rate = config->rev_sch[0].sch_chn_cfg.max_rate;
#ifdef FEATURE_IS2000_REL_B //32X
    if (P_REV_IS_8_OR_GREATER && escam->rev_sch_cc_incl)
    {
      max_rate = escam->rev_sch_cc[0].sch_chn_cfg.max_rate;
    }
#endif
    for ( bn = 0; bn <= escam->num_rev_cfg_recs; ++bn )
    {
      if ( escam->rev_cfg[bn].rev_sch_rate > max_rate)
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "R-Rate %d is > than MAX_RATE %d",
          escam->rev_cfg[bn].rev_sch_rate,
          max_rate );
        error = CAI_REJ_FIELD;
        goto function_end;  /*lint !e801 */
      }
    }
#else /* FEATURE_IS2000_REL_A */
    // Validate the rate based on mux option negotiated.  We know that rev_sch_cc must be
    // included if we are here.
    // Note: This is not specified in the standard, but we should do it anyway.
    // We support only SCH 0, so make that assumption here.
    for ( bn = 0; bn <= escam->num_rev_cfg_recs; ++bn )
    {
      if ( !mccsch_verify_rate( config->rev_sch[0].rev_sch_mux,
           (cai_sch_rate_type) escam->rev_cfg[bn].rev_sch_rate ) )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "Rate %d not allowed by rev_sch_mux 0x%x",
          escam->rev_cfg[bn].rev_sch_rate,
          config->rev_sch[0].rev_sch_mux );
        error = CAI_REJ_FIELD;
        goto function_end;
      }
    }
#endif /* FEATURE_IS2000_REL_A */

  }

  // If FPC_PRI_CHAN included, make sure it specifis a valid channel
  if ( escam->add_info_incl )
  {
    // Error if inconsistent primary channel specified
    if ( ((escam->fpc_pri_chan == 0) && (!config->fch_cc_incl)) ||
         ((escam->fpc_pri_chan == 1) && (!config->dcch_cc_incl))
       )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Inconsistent FPC primary channel %d!",
        escam->fpc_pri_chan );
      error = CAI_REJ_CAP;
      goto function_end;   /*lint !e801 */
    }
  }

  // Now check for invalid R-SCH IDs
  // If wrong SCH ID is specified, we reject the message with capability not supported
  // although this is not specified in the Standard.
  if ( escam->rev_cfg_included && num_r_sch_supported < CAI_MAX_NUM_REV_SCH )
  {
    // Note: num_rev_cfg_recs + 1 occurrences
    for ( bn = 0; bn <= escam->num_rev_cfg_recs; ++bn )
    {
      if ( escam->rev_cfg[bn].rev_sch_id >= num_r_sch_supported )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "SCH %d not supported!",
          escam->rev_cfg[bn].rev_sch_id );
        error = CAI_REJ_CAP;
        goto function_end;   /*lint !e801 */
      }
    }
  }

  // Detect error cases in the SCH FFPC configuration
  if ( escam->fpc_incl )
  {
    if ( escam->num_sup > num_f_sch_supported ||   // Too many SCH
         (escam->num_sup && escam->sup[0].sch_id >= num_f_sch_supported) || // Wrong SCH ID
         ( (escam->fpc_mode_sch == CAI_FPC_MODE_400_PRI_400_SEC || escam->fpc_mode_sch == CAI_FPC_MODE_200_PRI_600_SEC)  /*lint !e641 */
           && escam->fpc_sec_chan >= num_f_sch_supported ) )  // Wrong fpc_sec_chan
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "SCH FPC config error!" );
      error = CAI_REJ_CAP;
      goto function_end;   /*lint !e801 */
    }
  }

  /* T_ADD_ABORT processing */
  if (sch_assign.rev_parms.ignore_escam)
  {
    if (!escam->use_scrm_seq_num || (escam->scrm_seq_num !=
      mccds_get_scrm_seq_num()))
    {
      /* SCRM sequence number mismatch, ignore R-SCH assignment in ESCAM */
      M1X_MSG( DCP, LEGACY_MED,
        "SCRM seq # mismatch, ignore R-SCH assign");
      escam->num_rev_sch = 0;
    }
    else
    {
      /* SCRM sequence number match, clear ignore ESCAM indicator */
      mccsch_update_ignore_escam(FALSE);
    }
  }

#ifdef FEATURE_IS2000_REL_A
  if (P_REV_IS_7_OR_GREATER)
  {
    if (escam->threex_sch_info_incl)
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "3X configuration for pilots!" );
      error = CAI_REJ_CAP;
      goto function_end;   /*lint !e801 */
    }
  }
#endif

#ifdef FEATURE_IS2000_REL_B
  if (P_REV_IS_8_OR_GREATER && escam->for_sch_cc_incl)
  {
    snm_update_for_sch_cfg( escam->num_for_sch, escam->for_sch_cc);
  }
  if (P_REV_IS_8_OR_GREATER && escam->rev_sch_cc_incl)
  {
    snm_update_rev_sch_cfg( escam->num_rev_sch, escam->rev_sch_cc);
  }
#endif

  // If we are here, the message is okay, we can start processing

  /* 1X2GTA:Save the ESCAM OTA msg rcvd so that we can use stored msg to 
   * execute R/F-SCH0 Start txns as we do in this API after LTA complete */
  memset( &cdma.escam, 0, sizeof(caii_escam_type) );
  
  copy_size = memscpy( &cdma.escam, sizeof(caii_escam_type),
                       escam, sizeof(caii_escam_type) );
  
  if( copy_size != sizeof(mccdma_chnasn_type) )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "1xTA:Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
      copy_size,
      sizeof(caii_escam_type),
      sizeof(caii_escam_type));
  } 

  // 2. Update CCL and for_sch_fer_rpt
  mccccl_code_channel_list.update_ccl_with_escam( escam );
  if ( escam->for_cfg_included )
  {
    sch_assign.for_sch_fer_rep = escam->for_sch_fer_rep;
  }

  // 3. Update SCH assignment
  // First store some general parameters
  sch_assign.start_time_unit = escam->start_time_unit;
  sch_assign.rev_parms.use_t_add_abort = escam->use_t_add_abort;

  // Update reverse SCH assignment if needed
  for ( bn = 0; bn < escam->num_rev_sch; ++bn )
  {
    uint8 sch = escam->rev_sch[bn].rev_sch_id;
    sch_assign.reverse[sch].new_rate =
      (cai_sch_rate_type) escam->rev_sch[bn].rev_sch_rate;
    sch_assign.reverse[sch].rev_sch_duration = escam->rev_sch[bn].rev_sch_duration;
  }

  // Update forward SCH assignment if needed
  for ( bn = 0; bn < escam->num_for_sch; ++bn )
  {
    uint8 sch = escam->for_sch[bn].for_sch_id;
    sch_assign.forward[sch].sccl_index = escam->for_sch[bn].sccl_index;
    sch_assign.forward[sch].for_sch_duration = escam->for_sch[bn].for_sch_duration;
  }

  // Throttle back SRCH task when there is a SCH burst.
  //srch_notify_sch_active();

  // 4. Despatch R_SCH UPDATE transactions if necessary
  // If R-SCH is currently active, we need to inform Transmit task of
  // REV_SCH_DTX_DURATION change and Searcher task of USE_T_ADD_ABORT change.
  if (mccsch_sch_is_active(MCCSCH_REVERSE, 0) &&
      ((escam->rev_sch_dtx_duration !=
        sch_assign.rev_parms.rev_sch_dtx_duration) ||
       (sch_assign.rev_parms.use_t_add_abort !=
        sch_assign.rev_parms.current_use_t_add_abort)))
  {
    // Start rightaway
    m1x_time_get_20ms_frame_offset_time(sch_assign.reverse[0].start_time);

    despatch_info.transaction_type = MCTRANS_R_SCH_UPDATE;

    despatch_info.preprocessor      = mccsch_update_r_sch_parm_pre;
    despatch_info.postprocessor     = NULL;
    despatch_info.action_packet_gen = mccsch_update_r_sch_parm_action_packet;

    // Store REV_SCH_DTX_DURATION
    sch_assign.rev_parms.rev_sch_dtx_duration = escam->rev_sch_dtx_duration;

    // Now despatch the transaction.
    (void) mctrans_despatch_transaction(sch_assign.reverse[0].start_time,
      &despatch_info , NULL);

    M1X_MSG( DCP, LEGACY_MED,
      "ESCAM:Upd RSCH at %lx",
      sch_assign.reverse[0].start_time[0]);

    #ifdef FEATURE_1XCP_FTD
    /* Send event for burst assignment information */
    mccsch_report_event_sch_burst_assign (MCCSCH_REVERSE);
    #endif /* FEATURE_1XCP_FTD */
  }
  else
  {
    // Store REV_SCH_DTX_DURATION
    sch_assign.rev_parms.rev_sch_dtx_duration = escam->rev_sch_dtx_duration;
  }

  // 5. Despatch F-SCH transactions if necessary
  // Note that if F-SCH assignment is not included, we do not process the FFPC parameters since base
  // station can only include them if F-SCH assignment is included.
  // mandate
  if ( escam->num_for_sch > 0 ) // Assignment included
  {
    uint8   start_time;
    mctrans_pending_action_msg_type* msg_to_save = 0;
    uint32  stop;
    qword   cur_time = {0, 0};

    // Note: we support only 1 SCH and num_for_sch can only be 1 here.  Also SCH
    // ID must be zero (checked above). We are making these assumptions here.

    // First of all, see if start time specified
    start_time = escam->for_sch[0].for_sch_start_time_incl ? escam->for_sch[0].for_sch_start_time :
                                                             MCCTCHO_IMPLICIT_ACTION_TIME;



    m1x_time_get_20ms_frame_offset_time( cur_time );
    mccsch_compute_start_time( cur_time, start_time, 0, MCCSCH_FORWARD );

    despatch_info.transaction_type = MCTRANS_F_SCH0_START;

    despatch_info.preprocessor      = mccsch_start_f_sch0_pre;
    despatch_info.postprocessor     = mccsch_start_f_sch0_post;
    despatch_info.action_packet_gen = mccsch_start_f_sch0_action_packet;

    // Then determine type of transaction
    if ( escam->for_sch[0].for_sch_duration == 0 )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "ESCAM:Stop FSCH at %lx",
        sch_assign.forward[0].start_time[0] );
    }
    else
    {
      mccsch_compute_stop_time( escam->for_sch[0].for_sch_duration, 0, MCCSCH_FORWARD );
      // Note: end time of 0 is used to indicate infinite duration in the debug msg
      stop = ( escam->for_sch[0].for_sch_duration == INFINITE_DURATION ) ?
             0 : sch_assign.forward[0].stop_time[0];
      M1X_MSG( DCP, LEGACY_HIGH,
        "ESCAM:FSCH %lx--%lx",
        sch_assign.forward[0].start_time[0],
        stop );
    }

    // Save the ESCAM if FFPC parms included
    if ( escam->add_info_incl || escam->fpc_incl )
    {
      // Note that the escam here is same as the one used for RLGC transaction which may be
      // despatched below.  Transaction engine currently does not deal with multiple
      // transactions sharing the same message yet so let's just do it the easy way
      // and possibly store the message twice.
      msg_to_save = (mctrans_pending_action_msg_type*)escam;
    }

    /* 1xTA:If HHO is in progress (RIF-HHO True OR HHO with Frame Offset)
     * Differ SCH assigments to lower layers till HHO is successful 
     * to avoid collission b/w FSCH0_Start & FSCH0_Stop transactons and 
     * these these differed txns will be despatched again in 
     * do_successful_handoff_operations() API after successl HHO */
    if( srchtc_is_ta_supported() &&
        mcctcho_ho_data_ptr->hard_handoff_in_progress &&
        mcctcho_ho_data_ptr->ho_msg.extra_parms &&
         ( mcctcho_ho_data_ptr->ho_msg.extra.return_if_ho_fail ||     
           mcctcho_ho_data_ptr->different_frame_offset_ho
         )         
      )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
               "1xTA:Differed F_SCH0_START txn for FrameOffset Or RIF=1 HHO" );
    }
    else
    {
      // Now despatch the transaction.
      (void)mctrans_despatch_transaction( sch_assign.forward[0].start_time, 
                                          &despatch_info, msg_to_save );

       #ifdef FEATURE_1XCP_FTD
       /* Send event for burst assignment information */
       mccsch_report_event_sch_burst_assign (MCCSCH_FORWARD);
       #endif /* FEATURE_1XCP_FTD */
    } /* (!HHO) */
  } // of if ( escam->num_for_sch > 0 )

  // 6. Finally despatch R-SCH transactions if necessary
  if ( escam->num_rev_sch > 0 )
  {
    uint8   start_time;
    mctrans_pending_action_msg_type* msg_to_save = 0;
    uint32  stop;
    qword   cur_time;

    // Note: we support only 1 R-SCH and num_rev_assign can only be 1 here.  Also SCH
    // ID must be zero (checked above). We are making these assumptions here.

    // First of all, see if start time specified
    start_time = escam->rev_sch[0].rev_sch_start_time_incl ? escam->rev_sch[0].rev_sch_start_time :
                                                             MCCTCHO_IMPLICIT_ACTION_TIME;
    m1x_time_get_20ms_frame_offset_time( cur_time );
    mccsch_compute_start_time( cur_time, start_time, 0, MCCSCH_REVERSE );

    despatch_info.transaction_type = MCTRANS_R_SCH0_START;

    despatch_info.preprocessor = mccsch_start_r_sch0_pre;
    despatch_info.postprocessor = mccsch_start_r_sch0_post;
    despatch_info.action_packet_gen = mccsch_start_r_sch0_action_packet;


    // Then determine type of transaction
    if ( escam->rev_sch[0].rev_sch_duration == 0 )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "ESCAM:Stop RSCH at %lx",
        sch_assign.reverse[0].start_time[0] );
    }
    else
    {
      mccsch_compute_stop_time( escam->rev_sch[0].rev_sch_duration, 0, MCCSCH_REVERSE );
      // Note: end time of 0 is used to indicate infinite duration in the debug msg
      stop = ( escam->rev_sch[0].rev_sch_duration == INFINITE_DURATION ) ?
             0 : sch_assign.reverse[0].stop_time[0];
      M1X_MSG( DCP, LEGACY_HIGH,
        "ESCAM:RSCH %lx--%lx",
        sch_assign.reverse[0].start_time[0],
        stop );
      /* We received a new R-SCH burst, so we can cancel the pending R-SCH extension
         transaction, if any. */
      mctrans_void_trans( MCTRANS_R_SCH0_EXTEND );
    }

    // Invoke callback for RSCH assignment if registered
    if ( sch_callback.rsch_assign_cb )
    {
      sch_callback.rsch_assign_cb( escam->rev_sch[0].rev_sch_duration );
    }

    // If rpc parameters are included, save the message for use at transction time
    if ( escam->rpc_incl )
    {
      msg_to_save = (mctrans_pending_action_msg_type*)escam;
    }

    /* 1xTA:If HHO is in progress (RIF-HHO True OR HHO with Frame Offset)
     * Differ SCH assigments to lower layers till HHO is successful 
     * to avoid collission b/w FSCH0_Start & FSCH0_Stop transactons and 
     * these these differed txns will be despatched again in 
     * do_successful_handoff_operations() API after successl HHO */
    if( srchtc_is_ta_supported() &&
        mcctcho_ho_data_ptr->hard_handoff_in_progress &&
        mcctcho_ho_data_ptr->ho_msg.extra_parms &&
         ( mcctcho_ho_data_ptr->ho_msg.extra.return_if_ho_fail ||     
           mcctcho_ho_data_ptr->different_frame_offset_ho
         )         
      )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
               "1xTA:Differed R_SCH0_START txn for FrameOffset Or RIF=1 HHO" );
    }
    else
    {
      // Now despatch the transaction.
      (void)mctrans_despatch_transaction( sch_assign.reverse[0].start_time, 
                                          &despatch_info, msg_to_save );

       #ifdef FEATURE_1XCP_FTD
       /* Send event for burst assignment information */
       mccsch_report_event_sch_burst_assign (MCCSCH_REVERSE);
       #endif /* FEATURE_1XCP_FTD */
    } /* (!HHO) */
  } // of if ( escam->num_rev_sch > 0 )

  /* if num_sch is not zero, we need to check for control hold state */
  /* if in control hold mode, add active transaction */
  if ( (escam->num_for_sch || escam->num_rev_sch) && mccccl_code_channel_list.in_control_hold() )
  {
    /* The action time of the active transaction is the same as the start time of either FWD
    or REV SCH if only one of the assignment is given OR the earlier of the two if both FWD
    AND REV SCH assignments are given */
    if (escam->num_for_sch)
    {
      if (escam->num_rev_sch)
      {
        /* Both are present, check which one has an earlier start time */
        if (qw_cmp(sch_assign.forward[0].start_time, sch_assign.reverse[0].start_time))
        {
          /* Positive value of qw_cmp means forward > reverse */
          mcctxns_begin_active_trans(sch_assign.reverse[0].start_time, NULL);
        }
        else
        {
          mcctxns_begin_active_trans(sch_assign.forward[0].start_time, NULL);
        }
      }
      else
      {
        /* Only forward assignment present */
        /* No need to store mesg for this transaction */
        mcctxns_begin_active_trans(sch_assign.forward[0].start_time, NULL);
      }
    }
    else
    {
      /* Only reverse assignment present */
      /* No need to store mesg for this transaction */
      mcctxns_begin_active_trans(sch_assign.reverse[0].start_time, NULL);
    }
  }

  #ifdef FEATURE_IS2000_1X_ADV
  if (mcc_rcp_extended)
  {
    qword abs_action_time;

    /* Update RCP data with the values received in RCP record */
    mcc_update_rcp_data(&mcc_gem_rcp_rec);

    /* Setup transaction for radio configuration parameters */
    get_abs_action_time(abs_action_time, MCCTCHO_IMPLICIT_ACTION_TIME);
    mcctxns_begin_rcp_trans(abs_action_time);
  }
  #endif /* FEATURE_IS2000_1X_ADV */

function_end:
  if ( error )
  {
    rej_parms.rej_msg_type = CAI_EXT_SUP_CHN_ASN_MSG;
    rej_parms.rej_code = error;
    send_rej_order( &rej_parms);
  }

  #ifdef FEATURE_IS2000_1X_ADV
  mcc_rcp_extended = FALSE;
  #endif /* FEATURE_IS2000_1X_ADV */

  return;

} // mccsch_process_escam

/*===========================================================================

FUNCTION mccsch_verify_rate

DESCRIPTION
  Verifies if the rate passed into the function is less than or equal to the rate
  embedded in the given sch mux option.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if rate is allowed by the mux option.  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccsch_verify_rate
(
  uint16            sch_mux,
  cai_sch_rate_type sch_rate
)
{
  uint8 rate;

  // First extract rate in ??X from sch_mux
  // First handle the special case of MUX = 3 or 4, i.e. 1X SCH
  if ( sch_mux <= 0x4 )  // Note:Mux 0 - 2 are invalid and won't even get here
  {
    rate = 1;   // 1X for mux 3 or 4
  }
  else
  {
    // Compute the rate in 2 stages
    rate = sch_mux >> 2;                     //lint !e734 loss of precision intended
    // Now upper 2 bits of rate is single/double-size PDU, and lower 6 bits is number of
    // data blocks.
    rate = ((rate >> 6) + 1)*(rate & 0x3f);  // Now rate in in ??X
  }

  if ( sch_mux & 1 )       // Rate set 1
  {
    // If rate is out of range or if it's higher than negotiated rate
    if ( sch_rate > CAI_MAX_SCH_RATE_RS1 ||
         snm_rs1_rate_table[sch_rate] > rate )
    {
      return FALSE;
    }
  }
  else  // Rate set 2
  {
    // If rate is out of range or if it's higher than negotiated rate
    if ( sch_rate > CAI_MAX_SCH_RATE_RS2 ||
         snm_rs2_rate_table[sch_rate] > rate )
    {
      return FALSE;
    }
  }

  // If it gets here, we are okay.
  return TRUE;
} // mccsch_verify_rate

/*===========================================================================

FUNCTION mccsch_escam_rlgc_action_packet

DESCRIPTION
  The function generates the data needed for "action" in the action packet
  for the "escam rlgc" transaction.  It takes care of all actions involved
  in the transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

/*lint -e818*/
static void mccsch_escam_rlgc_action_packet
(
  mctrans_trans_info_type *   curr_trans,
  mctrans_action_type         action,
  mctrans_server_token_type   server_token,
  mctrans_action_packet_type* packet
)
{
  mctrans_pending_action_msg_type* pending_msg;

  // First initialize packet to update nothing.
  mcctxns_init_rlgc( &packet->action.rlgc );

  // Retrive pending message (ESCAM) if exist.
  pending_msg = mctrans_get_pending_msg( curr_trans->trans_num );
  if ( pending_msg == 0 )
  {
    // If no pending message, we are done now.
    return;
  }

  packet->action_type = action;

  // Only TXC can be a server for this transaction, so use that assumption here
  // for efficiency.

  // ESCAM can only contain 1 RLGC parameter for this addendum
  if ( pending_msg->escam.rpc_incl )
  {
    uint8 m;
    // Note: rpc_num_sup + 1 records
    for ( m = 0; m <= pending_msg->escam.rpc_num_sup; ++m )
    {
      packet->action.rlgc.rlgain_sch_pilot[pending_msg->escam.rpc_sup[m].sch_id].included = TRUE;
      packet->action.rlgc.rlgain_sch_pilot[pending_msg->escam.rpc_sup[m].sch_id].value =
        ( pending_msg->escam.rpc_sup[m].rlgain_sch_pilot & 0x20 ) ?
        pending_msg->escam.rpc_sup[m].rlgain_sch_pilot | 0xc0 :
        pending_msg->escam.rpc_sup[m].rlgain_sch_pilot;
    }
  }

} //lint !e715
// mccsch_escam_rlgc_action_packet

/*===========================================================================

FUNCTION mccsch_prepare_escam_ffpc_action_packet

DESCRIPTION
  Prepares the action packet for FFPC action.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

/*lint -e818*/
static void mccsch_prepare_escam_ffpc_action_packet
(
  mctrans_trans_info_type *   curr_trans,
  mctrans_ffpc_action_type* packet
)
{
  mctrans_pending_action_msg_type* pending_msg;

  // First initialize packet to update nothing.
  mcctxns_init_ffpc( packet );

  // Now retrive pending message.  For now, we can assume that it's ESCAM
  // Note that we may not have a pending message stored.  If that's the case, that means we
  // don't have any FFPC parameters to update (cant happen with UHDM)..  In that case,
  // we can just return with the initialized FFPC packet.

  pending_msg = mctrans_get_pending_msg( curr_trans->trans_num );
  if ( pending_msg == NULL )
  {
    if (mccccl_code_channel_list.fwd_fundicated_chan_is_active())
    {
      // Always include FPC_MODE since SCH uses a different FPC_MODE (FPC_MODE_SCH)
      // Assume that the mode was already validated against active channels.
      packet->fpc_mode.included = TRUE;
      packet->fpc_mode.value = sch_assign.fpc_mode_sch;
    }
    else
    {
      // If there is no forward fundicated channel, then ensure that the FPC_PRI_CHAN
      // and FPC_MODE are set to indicate that no FFPC is active
      packet->fpc_pri_chan.included = TRUE;
      packet->fpc_pri_chan.value = CAI_FPC_PRI_CHAN_CPCCH;  /*lint !e641 */
      packet->fpc_mode.included = TRUE;
      packet->fpc_mode.value = CAI_FPC_MODE_RESERVED;
    }
    return;
  }

  // Then fill out the action packet according to what's included in the ESCAM
  if ( pending_msg->escam.add_info_incl )
  {
    packet->fpc_pri_chan.included = TRUE;
    if (mccccl_code_channel_list.fwd_fundicated_chan_is_active())
    {
      packet->fpc_pri_chan.value = pending_msg->escam.fpc_pri_chan;
    }
    else
    {
      // If there is no forward fundicated channel, set to CPCCH
      packet->fpc_pri_chan.value = CAI_FPC_PRI_CHAN_CPCCH;  /*lint !e641 */
    }
    // Make sure FPC_PRI_CHAN in our NNSCR is updated
    snm_update_fpc_pri_chan( pending_msg->escam.fpc_pri_chan );
  }

  if ( pending_msg->escam.fpc_incl )
  {
    uint8 n;

    // First update FPC_MODE_SCH.  packet->fpc_mode is updated below
    sch_assign.fpc_mode_sch =
      (cai_fpc_mode_type) pending_msg->escam.fpc_mode_sch;
    packet->sch.included = TRUE;  // Mark packet as containing SCH info.
    packet->sch.fpc_sec_chan.included = TRUE;

    if ( pending_msg->escam.fpc_mode_sch == CAI_FPC_MODE_400_PRI_400_SEC ||  /*lint !e641 */
         pending_msg->escam.fpc_mode_sch == CAI_FPC_MODE_200_PRI_600_SEC )   /*lint !e641 */
    // For published addendum, sec_chan is included only for FPC_MODE is
    // one of the above 2.
    {
      packet->sch.fpc_sec_chan.value = pending_msg->escam.fpc_sec_chan;
    }
    packet->sch.fpc_sch_init_setpt_op = pending_msg->escam.fpc_sch_init_setpt_op;

    // Do the SCH setpoints if included
    for ( n = 0; n < pending_msg->escam.num_sup; ++n )
    {
      // Mark packet as containing SCH info.  It may not done multiple times but it's
      // harmless.
      packet->sch.included = TRUE;

      packet->sch.sch_olpc[pending_msg->escam.sup[n].sch_id].included = TRUE;
      packet->sch.sch_olpc[pending_msg->escam.sup[n].sch_id].init_setpt.included = TRUE;
      packet->sch.sch_olpc[pending_msg->escam.sup[n].sch_id].init_setpt.value =
        pending_msg->escam.sup[n].fpc_sch_init_setpt;
      // Note: SCH setpoints in ESCAM do not have special value of 0xff as in PCNM
      packet->sch.sch_olpc[pending_msg->escam.sup[n].sch_id].param.min_setpt.value =
        pending_msg->escam.sup[n].fpc_sch_min_setpt;
      packet->sch.sch_olpc[pending_msg->escam.sup[n].sch_id].param.max_setpt.value =
        pending_msg->escam.sup[n].fpc_sch_max_setpt;
      packet->sch.sch_olpc[pending_msg->escam.sup[n].sch_id].param.target_fer =
        pending_msg->escam.sup[n].fpc_sch_fer;
    }

    // Finally the SCH setpoint threshold if included
    if ( pending_msg->escam.fpc_thresh_sch_incl )
    {
      packet->sch.included = TRUE;  // Mark packet as containing SCH info.
      packet->sch.fpc_thresh_sch.included = TRUE;
      packet->sch.fpc_thresh_sch.value = pending_msg->escam.fpc_setpt_thresh_sch;
    }
  } // of if ( pending_msg->escam.fpc_incl )

  if (mccccl_code_channel_list.fwd_fundicated_chan_is_active())
  {
    // Always include FPC_MODE since SCH uses a different FPC_MODE (FPC_MODE_SCH)
    // Assume that the mode was already validated against active channels.
    packet->fpc_mode.included = TRUE;
    packet->fpc_mode.value = sch_assign.fpc_mode_sch;
  }
  else
  {
    // If there is no forward fundicated channel, then ensure that the FPC_PRI_CHAN
    // and FPC_MODE are set to indicate that no FFPC is active
    packet->fpc_mode.included = TRUE;
    packet->fpc_mode.value = CAI_FPC_MODE_RESERVED;
  }

} /* mccsch_prepare_escam_ffpc_action_packet */
/*lint +e818*/

/*===========================================================================

FUNCTION mccsch_sch_is_active

DESCRIPTION
   This function checks whether the specified SCH is active.  Note the SCH_ID is
   assumed to be valid.  If invalid SCH_ID is specified, return value is
   unpredictable.

DEPENDENCIES
  None.

RETURN VALUE
  True if SCH is active, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccsch_sch_is_active
(
  mccsch_ch_dir_type  dir,
  uint8               sch_id
)
{
  if ( dir == MCCSCH_FORWARD )
  {
    return sch_assign.forward[sch_id].active;
  }
  else
  {
    return sch_assign.reverse[sch_id].active;
  }
} // mccsch_sch_is_active

/*===========================================================================

FUNCTION mccsch_update_ignore_escam

DESCRIPTION
  This function updates ignore ESCAM indicator.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_update_ignore_escam
(
  boolean ignore_escam
)
{
  if (sch_assign.rev_parms.ignore_escam != ignore_escam)
  {
    sch_assign.rev_parms.ignore_escam = ignore_escam;
    M1X_MSG( DCP, LEGACY_MED,
      "IGNORE_ESCAM = %d",
      sch_assign.rev_parms.ignore_escam);
  }

} /* mccsch_update_ignore_escam */

/*===========================================================================

FUNCTION mccsch_reg_sch_callback

DESCRIPTION
  This function binds the following 3 callback functions to the SCH manager:
  1. Function to call when SCH configuration changes.
  2. Function to call when ESCAM including R-SCH assignment (not stop) is received
     (not at the start time but the time the message is received).
  3. Function to call when there are "n" frames left in the burst, or at beginning
     of burst if burst length is less than n.  "n" is passed in as a function
     argument.  This is used for burst extension.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

/*lint -e818*/
void mccsch_reg_sch_callback
(
  mccsch_sch_callback_type* sch_cb
)
{
  sch_callback = *sch_cb;

} /* mccsch_reg_sch_callback */
/*lint +e818*/

/*===========================================================================

FUNCTION mccsch_reg_sch_config_change_cb

DESCRIPTION
  This function binds the following callback function to the SCH manager:
  Function to call when SCH configuration changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_reg_sch_config_change_cb
(
  mccsch_sch_config_change_cb_type sch_config_change_cb_func
)
{
  sch_callback.sch_config_change_cb = sch_config_change_cb_func;
}

/*===========================================================================

FUNCTION mccsch_reg_rsch_assign_cb

DESCRIPTION
  This function binds the following callback function to the SCH manager:
  Function to call when ESCAM including R-SCH assignment (not stop) is received
  (not at the start time but the time the message is received).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_reg_rsch_assign_cb
(
  mccsch_rsch_assign_cb_type rsch_assign_cb_func
)
{
  sch_callback.rsch_assign_cb = rsch_assign_cb_func;
}

/*===========================================================================

FUNCTION mccsch_reg_rsch_extension_cb

DESCRIPTION
  This function binds the following callback function to the SCH manager:
  Function to call when there are "n" frames left in the burst, or at beginning
  of burst if burst length is less than n.  "n" is passed in as a function
  argument.  This is used for burst extension.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_reg_rsch_extension_cb
(
  mccsch_rsch_extension_cb_type rsch_extension_cb_func,
  uint8 trigger_time
)
{
  sch_callback.rsch_extension_cb = rsch_extension_cb_func;
  sch_callback.trigger_time = trigger_time;
}

/*===========================================================================

FUNCTION mccsch_proc_r_dtx

DESCRIPTION
  This function is invoked when DTX is detected. It tears down the RSCH and
  sends a SCRM to the BS.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_proc_r_dtx
(
  void
)
{
  rxtx_cmd_type *cmd_ptr;

  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_COUNT,                     // Transaction type
    MCTRANS_MODE_EXPLICIT,
    NULL,
    NULL,
    NULL
  };

  // Start rightaway
  m1x_time_get_20ms_frame_offset_time(sch_assign.reverse[0].start_time);


  despatch_info.transaction_type = MCTRANS_R_SCH0_STOP;

  despatch_info.preprocessor = mccsch_stop_r_sch0_pre;
  despatch_info.postprocessor = mccsch_stop_r_sch0_post;
  despatch_info.action_packet_gen = mccsch_stop_r_sch0_action_packet;

  // Now despatch the transaction.
  (void)mctrans_despatch_transaction( sch_assign.reverse[0].start_time, &despatch_info, NULL );

#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    /* Zero out requested BLOB */
    mccds_clear_scrm_req_blob();
    /* Build Supplemental Channel Request Message */
    mccds_build_sup_chan_req_msg(&cmd_ptr->tc_msg.msg, NULL, FALSE);
    cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q( cmd_ptr );
    rxtx_cmd( cmd_ptr );
  }

} /* mccsch_proc_r_dtx */

/*===========================================================================

FUNCTION mccsch_process_t_add_abort

DESCRIPTION
  This function dispatches transaction to stop R-SCH at the end of the
  current 20 ms frame, set ignore ESCAM indicator and send SCRM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_process_t_add_abort
(
  mccsrch_rpt_type *srch_rpt_ptr /* Searcher T_ADD_ABORT report */
)
{
  rxtx_cmd_type     *cmd_ptr;

  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_COUNT,                     // Transaction type
    MCTRANS_MODE_EXPLICIT,
    NULL,
    NULL,
    NULL
  };

  /* Not in T_ADD_ABORT processing and R-SCH is active */
  if (!sch_assign.rev_parms.ignore_escam &&
      mccsch_sch_is_active(MCCSCH_REVERSE, 0))
  {
    /* Report event: T_ADD_ABORT */
    event_report(EVENT_T_ADD_ABORT);

    // Start rightaway
    m1x_time_get_20ms_frame_offset_time(sch_assign.reverse[0].start_time);


    despatch_info.transaction_type = MCTRANS_R_SCH0_STOP;

    despatch_info.preprocessor = mccsch_stop_r_sch0_pre;
    despatch_info.postprocessor = mccsch_stop_r_sch0_post;
    despatch_info.action_packet_gen = mccsch_stop_r_sch0_action_packet;

    // Now despatch the transaction.
    (void) mctrans_despatch_transaction(sch_assign.reverse[0].start_time,
                                                   &despatch_info , NULL);

#ifdef FEATURE_1X_CP_MEM_OPT
    if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Memory allocation failed for RXTX Buf" );
    }
#else /* !FEATURE_1X_CP_MEM_OPT */
    if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No free buffers on rxtx_mc_free_q" );
    }
#endif /* FEATURE_1X_CP_MEM_OPT */
    else
    {
      /* Zero out requested BLOB */
      mccds_clear_scrm_req_blob();
      /* Build Supplemental Channel Request Message */
      mccds_build_sup_chan_req_msg(&cmd_ptr->tc_msg.msg, srch_rpt_ptr, TRUE);
      /* Send SCRM in assured mode */
      cmd_ptr->tc_msg.ack_req = TRUE;

      /* Fill in the command header */
      mcctc_fill_rxtx_hdr_free_q( cmd_ptr );
      rxtx_cmd( cmd_ptr );
    }

    /* Mark T_ADD_ABORT has been detected */
    mccsch_update_ignore_escam(TRUE);
  }

} /* mccsch_process_t_add_abort */

/*===========================================================================

FUNCTION mccsch_inform_sch_change

DESCRIPTION
  This function informs DS that sch operation has just changed.  It uses the
  the function pointer mccsch_sch_config_inform.

DEPENDENCIES
  mccsch_sch_config_inform must be registered; otherwise nothing will be
  performed.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void mccsch_inform_sch_change
(
  mccsch_ch_dir_type  dir,
  uint8               sch_id
)
{
  mccsch_sch_info_type sch_info;

  if ( !sch_callback.sch_config_change_cb )
  {
    return;
  }

  // Indicate the channel that changes
  if ( dir == MCCSCH_FORWARD )
  {
    sch_info.f_sch_0.changed = TRUE;
    sch_info.r_sch_0.changed = FALSE;
  }
  else
  {
    // If it's not f-sch 0 that is changing, must be r-sch 0 since sch 1 is not
    // supported
    sch_info.f_sch_0.changed = FALSE;
    sch_info.r_sch_0.changed = TRUE;
  }

  // Fill in F-SCH assignment info.  SCH 0 assumed here
  if ( (sch_info.f_sch_0.active = sch_assign.forward[0].active) != FALSE )
  {
    sch_info.f_sch_0.sch_rate = sch_assign.forward[0].cur_rate;
  }
  // Fill in R-SCH assignment info.  SCH 0 assumed here
  if ( (sch_info.r_sch_0.active = sch_assign.reverse[0].active) != FALSE )
  {
    sch_info.r_sch_0.sch_rate = sch_assign.reverse[0].cur_rate;
  }

  // Now inform the client
  sch_callback.sch_config_change_cb( &sch_info );

} //lint !e715 sch_id not used // mccsch_inform_sch_change

/*===========================================================================

FUNCTION mccsch_stop_sch0_trans

DESCRIPTION
  Function to generate two transactions to stop the Fwd and Rev SCH 0
  processing at the action time. It is called from the transaction client's
  (MC) context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccsch_stop_sch0_trans
(
  qword action_time
)
{

  mctrans_despatch_info_type despatch_info =
  {
    MCTRANS_COUNT,                     // Transaction type
    MCTRANS_MODE_EXPLICIT,
    NULL,
    NULL,
    NULL
  };

  if (mccsch_sch_is_active(MCCSCH_REVERSE, 0))
  {
    despatch_info.transaction_type = MCTRANS_R_SCH0_STOP;

    despatch_info.preprocessor = mccsch_stop_r_sch0_pre;
    despatch_info.postprocessor = mccsch_stop_r_sch0_post;
    despatch_info.action_packet_gen = mccsch_stop_r_sch0_action_packet;

    (void)mctrans_despatch_transaction( action_time,&despatch_info,0);
    M1X_MSG( DCP, LEGACY_MED,
      "r_sch0_stop transaction dispatched");
  }

  if (mccsch_sch_is_active(MCCSCH_FORWARD, 0))
  {
    despatch_info.transaction_type = MCTRANS_F_SCH0_STOP;

    despatch_info.preprocessor = mccsch_stop_f_sch0_pre;
    despatch_info.postprocessor = mccsch_stop_f_sch0_post;
    despatch_info.action_packet_gen = mccsch_stop_f_sch0_action_packet;

    (void)mctrans_despatch_transaction( action_time, &despatch_info, 0 );
    M1X_MSG( DCP, LEGACY_MED,
      "f_sch0_stop transaction dispatched");
  }
} /* mccsch_stop_sch0_trans */

/*===========================================================================

FUNCTION mccsch_verify_fwd_assigment

DESCRIPTION
  Function verify the sch_assign fwd entries after a HO is completed.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: if the assignment is still valid
  FALSE:  ..              .. not valid

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccsch_verify_fwd_assignment
(
  void
)
{
  int j;
  const mccccl_sccl_entry_type * sccl;

  /* for now only check sch0 */
  if (mctrans_is_trans_pending(MCTRANS_F_SCH0_START))
  {
    sccl=mccccl_code_channel_list.get_sccl_entry(0, sch_assign.forward[0].sccl_index);
    for (j=0; j<sccl->num_pilots; j++)
    {
      if (mccccl_code_channel_list.pilot_is_in_active_set(sccl->pilots[j].pilot_pn,
                                                          sccl->pilots[j].pilot_rec_type,
                                                          TRUE)
         )
      {
        // the forward assignment is valid if at least one pilot is in the active set
        return TRUE;
      }
    }
    return FALSE;
  }

  return TRUE; // if no assignment is pending, no need to verify

} /* mccsch_verify_fwd_assignment */

#ifdef FEATURE_1XCP_FTD
/*===========================================================================

FUNCTION MCCSCH_REPORT_EVENT_BURST_ASSIGN

DESCRIPTION
  Reports events with burst assignment duration and start time information when FSCH
  or RSCH is assigned.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void mccsch_report_event_sch_burst_assign
(
  mccsch_ch_dir_type dir
)
{
  if (dir == MCCSCH_FORWARD)
  {
    mccsch_event_fsch_burst_assign_type event_sch_burst_assign;
    event_sch_burst_assign.start_time_hi = qw_hi(sch_assign.forward[0].start_time);
    event_sch_burst_assign.start_time_lo = qw_lo(sch_assign.forward[0].start_time);
    event_sch_burst_assign.duration = sch_assign.forward[0].for_sch_duration;
    event_report_payload(EVENT_1X_FSCH0_BURST_ASSIGN,
                         sizeof(event_sch_burst_assign),
                         (void *) &event_sch_burst_assign);
  }
  else
  {
    mccsch_event_rsch_burst_assign_type event_sch_burst_assign;
    event_sch_burst_assign.start_time_hi = qw_hi(sch_assign.reverse[0].start_time);
    event_sch_burst_assign.start_time_lo = qw_lo(sch_assign.reverse[0].start_time);
    event_sch_burst_assign.duration = sch_assign.reverse[0].rev_sch_duration;
    event_sch_burst_assign.dtx_duration = sch_assign.rev_parms.rev_sch_dtx_duration;
    event_report_payload(EVENT_1X_RSCH0_BURST_ASSIGN,
                         sizeof(event_sch_burst_assign),
                         (void *) &event_sch_burst_assign);
  }
} /* mccsch_report_event_sch_burst_assign */
#endif /* FEATURE_1XCP_FTD */

