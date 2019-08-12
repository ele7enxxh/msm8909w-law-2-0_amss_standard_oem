#ifndef L1TASK_H
#define L1TASK_H
/*===========================================================================

          L 1   T A S K   E X T E R N A L  D E C L A R A T I O N S

DESCRIPTION
This file provides an external declaration for the l1 task execution start
point function wcdma_l1_task.  This file is designed to be included by the UE
task controller so that it may start the L1 task.

EXTERNALIZED FUNCTIONS

  wcdma_l1_task
    This procedure is the entrance procedure for the L1 task.
    It performs queue initialization and handshake with the Main Control
    task. It then loops forever waiting for commands from the command queues,
    watchdog timer signals, Main Control task signals, and local signals.
    Upon receipt of the signals, it dispatches the command/signal to be
    processed by the appropriate handlers.

  l1_get_cmd_buf
    This function returns a free command buffer, if available, or NULL if
    no command buffers are available.

  l1_put_cmd
    This function is used by the external clients of the L1 to submit
    commands to the L1. The caller owns the pointer to the command. 
    This function is called in the context of the caller. The function
    puts the command onto the External Command Queue and sets the
    External Command Queue signal of the L1 task.

  l1_cphy_setup_cnf_get_status
    This function checks the operations status for a selected type and
    channel in the command packet of a CPHY_SETUP_CNF indication.
    This function returns the status of the requested operation as
    denoted in the indication packet, additionally the operation type is
    returned as an output parameter.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  The wcdma_l1_task function contains the L1 Manager task.  It must be
  specified in a call to rex_def task and started by a Task Controller.

  Copyright (c) 2000-2009 by Qualcomm Technologies, Inc.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/l1task.h_v   1.13   19 May 2002 03:29:12   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/l1task.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/08/15   nga     FR 23268: Configurable scaling of IDLE DRX for M2M devices (WCDMA)
                   FR 23280: Skipping IDLE mode measurements for M2M devices (WCDMA)
04/27/14   jkb     Add check for Feature dual sim with feature qta
09/23/13   as      Porting Dual Sim from Triton to Dime.
05/23/13   geg     WSW decoupling (except for component of QCHAT)
01/11/13   mm      Compiler error fixes for Triton DSDA
12/10/12   scm     Partial FEATURE_DUAL_SIM support for DSDA.
01/20/12   bs      free floating changes.
08/24/11   hk      Added command id parameter while sending the error
                   indication to higher layer
05/31/11   geg     Changes for RRC and L2 api files clean-up wcdma dependencies
04/25/11   geg     Separated l1task.h into l1task.h and l1task_v.h
04/25/11   geg     Recovered this file to /wcdma/api
03/30/09   rm      Added new featurization for FEATURE_CMI.
01/19/09   stk     Added support for R7 spec CR High Mobility Indicator( spec CR 2948(RP-060712))
12/09/08    uk      Mainling FEATURE_WCDMA_DL_ENHANCED, FEATURE_ENHANCED_CELL_RESELECTION, 
                    FEATURE_WCDMA_DRX_MANAGER, FEATURE_WCDMA_OFFLINE_SEARCHER 
                    and FEATURE_WCDMA_OFFLINE_DEMOD_SUPPORT 
10/03/08   hk      Bringing in 7k Mailine fixes
10/26/07   vsr     Mainlining MBMS changes
07/16/07   nd      added extern for l1_process_immediate_interf_meas_req()
05/29/06   kps     Support for CGPS
01/06/06   scm     Change SIB sleep feature name to FEATURE_INTER_SIB_SLEEP.
11/24/05  nd/gv    Modified the prototype of l1_is_s_criteria_met() to get the 
                   max_tx_pwr from RRC
11/08/05  gv/nd    Added code for WtoW BPLMN feature.
11/04/05    mc      Added a new command code to handle PhyChSetupFailure in case of 
                    HS->DCH transition.
09/25/05   kps     Preliminary SIB sleep optimization support. 
                   Code changes are inside #ifdef FEATURE_SIB_SLEEP_OPTIMIZATION
08/30/05   sh      Added function l1_is_s_criteria_met()
07/06/05   vp      Code changes to support new DL sync parms received in a 
                   CPHY_UPD_DL_SYNC_PARMS message. RRC sends this message
                   when it has received a UTRAN Mobility Information message.
05/04/05   sk/sh   Added T-barred Feature support. Support for new cmd CELL_BAR_REQ
02/04/05   scm     Support commands from BMC (Broadcast Multicast Control).
04/21/04   cpe     Changed l1_process_immediate_meas_req to include measurement quantity now
12/18/03   scm     Add struct and define for LSM cell measurement request.
12/15/03   scm     Remove unused command from LSM l1_cmd_enum_type.  Mainlined
                   FEATURE_L1_ACT_TIME.
11/24/03   scm     Add sdu_time to l1_cmd_hdr_type so WL1 can determine if it
                   missed an activation time or not.
04/28/03   djm     Addition of LSM Commands
12/18/02   ATM     Made COMMAND enum CFA feature friendly
09/18/02   scm     More work on activation time support in SUSPEND.
09/09/02   scm     Working on activation time support in SUSPEND.
08/21/02   scm     2 MAC sigs now: CPHY_MAC_UL_CONFIG_CNF & CPHY_MAC_DL_CONFIG_CNF.
08/07/02   scm     Add CPHY_MAC_CONFIG_CNF to l1_cmd_enum_type for activation times.
08/06/02   scm     Continued work on SUSPEND/RESUME support.
05/08/02   sh      Removed l1_aset_update_cmd_type from l1_cmd_union_type.
05/08/02   scm     Change l1_task to wcdma_l1_task.
05/06/02   scm     Change FEATURE_WCDMA_L1_START_STOP to
                   FEATURE_CHEETAH_BASELINE, to match RRC.
05/03/02   scm     Implementing support of START/STOP primitives from RRC.
                   Code wrapped in FEATURE_WCDMA_L1_START_STOP define.
02/15/02   sh      Added FEATURE_L1_ACT_TIME.
02/12/02   sh      Added CPHY_ACT_TIME_CANCEL_REQ command enum type
                   and l1_act_time_cancel_cmd_type to l1_cmd_union_type.
02/06/02   sh      Removed fields related to Activation Time from the
                   common command header.
10/19/01   sh      Added CPHY_DEACTIVATE_REQ to l1_cmd_enum type and
                   command type to l1_cmd_union_type.
08/31/01   sh      Added CPHY_FREQ_SCAN_REQ primitive and command type in
                   l1_cmd_union_type.
08/29/01   cpe     Added l1_process_immediate_meas_req() prototype
08/08/01   cpe     Removed  l1_immediate_meas_cmd_type from the union
                   l1_cmd_union_type.
01/03/01   sh      Removed CPHY_SET_SFN_REQ, CPHY_SET_CFN_REQ, and
                   CPHY_IMMEDIATE_CELL_EVAL_REQ.
11/22/2000 sh      Ported from l1ext.h.

============================================================================*/

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "l1macif.h"
#include "l1lsmif.h"
#include "bmcl1if.h"

/* -------------------------------------- */
/* PUBLIC API of l1task.h                 */
/* -------------------------------------- */
/*===========================================================================

FUNCTION    L1_TASK

FILE NAME   l1m.c

DESCRIPTION
  This procedure is the entrance procedure for the L1 task.
  It performs queue initialization and handshake with the Main Control
  task. It then loops forever waiting for commands from the command queues,
  watchdog timer signals, Main Control task signals, and local signals.
  Upon receipt of the signals, it dispatches the command/signal to be
  processed by the appropriate handlers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/* Public portion of the API exists in wcdmatmcif.h */
#include "wcdmatmcif.h"

/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

/*--------------------------------------------------------------------------
                      UNION OF ALL L1 EXTERNAL COMMANDS

--------------------------------------------------------------------------*/


/* Basic command type to be included by all custom command types.
 * The Processors and other units within the L1 and their associated
 * clients define command types that include the l1_cmd_hdr_type
 * as the first field in the command.
 *
 * Once the Processors/Units within the L1 are done with the command,
 * the command is returned to the free queue. 
 */

/*--------------------------------------------------------------------------
                      L1 HEADER COMMAND

  Header command is command to all the external command.
--------------------------------------------------------------------------*/
typedef struct
{
  q_link_type            link;

  /* enumerated command value */
  l1_cmd_enum_type       cmd_id;

  /* The 3 activation time attributes:
  ** - Type: NONE, SFN, or CFN.
  ** - SFN or CFN frame number, actual activation time.
  ** - Sequence number, for identifying the command. */
  l1_act_time_enum_type  act_time_type;
  uint16                 act_time;
  uint8                  seq_num;

  /* Frame number at which all PDUs of the SDU resulting in this command from
  ** RRC to WL1 were received by RLC.  In other words, the time at which the
  ** UE actually received the command from the network.  This value is set by
  ** RRC to help L1 determine if it has missed the activation time.  Only
  ** applies if the command actually has an activation time. */
  uint16                 sdu_time;

  /* AS ID populated by RRC and passed to L1 for DSDA */
  #ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type sys_modem_as_id;
  #endif

} l1_cmd_hdr_type;

/* Union of all L1 commands from Upper Layers */
typedef union
{
  /* Commands from RRC */
  l1_cap_cmd_type                 cap;
  l1_idle_cmd_type                idle;
  l1_deactivate_cmd_type          deact;
  l1_stop_cmd_type                stop;
  l1_start_cmd_type               start;
  l1_suspend_cmd_type             suspend;
  l1_resume_cmd_type              resume;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_QTA)
  l1_start_qta_cmd_type           start_qta;
  l1_stop_qta_cmd_type            stop_qta;
#endif
  l1_freq_scan_cmd_type           freq_scan;
  l1_acq_cmd_type                 acq;
  l1_cell_parm_cmd_type           cell_parm;
  l1_meas_cmd_type                meas;
  l1_hard_handoff_req_cmd_type    hho;
  l1_bch_schedule_cmd_type        bch_sched;
  l1_setup_cmd_type               setup;
  l1_drx_cmd_type                 drx;
  l1_ciphering_key_cmd_type       cipher_key;
  l1_cell_selec_cmd_type          cell_sel;
  l1_intg_key_cmd_type            intg_key;
  l1_intg_key_mauc_cmd_type       key_mauc;
  l1_immediate_reselec_cmd_type   immed_rsel;
  l1_cell_reselect_rsp_cmd_type   rsel_rsp;
  l1_cell_transition_cmd_type     cell_trans;
  l1_act_time_cancel_cmd_type     act_cancel;

  l1_cell_bar_cmd_type            cell_bar;
  l1_upd_dl_sync_parms_cmd_type dl_sync_parms;
  
  /* Commands from MAC */
  l1_ul_tx_ctrl_req_type          ul_tx_ctrl;

  l1_mac_config_cnf_type          mac_config_cnf;

  /* Commands from LSM (Location Services Manager) */
  l1_lsm_timetag_type             lsm_timetag;
  l1_lsm_cell_meas_type           lsm_cell_meas;

  /* Commands from BMC (Broadcast Multicast Control) */
  bmc_l1_scheduling_req_type      bmc_scheduling_req;
  bmc_l1_skip_req_type            bmc_skip_req;
  bmc_l1_deactivate_req_type      bmc_deactivate_req;

  /* SIB sleep commands from RRC */
  l1_sib_sched_change_ind_type    sib_sched_change_ind;

  l1_upd_ota_parms_ind_cmd_type   upd_ota_parms_ind;
  
  l1_rrc_nudge_update_cmd_type      rrc_nudge_status_update;
  l1_get_modem_stat_req_cmd_type    get_modem_stat_req;
  l1_s_criteria_req_type            s_criteria_req;
  l1_sleep_rsp_type                 l1_sleep_rsp;
  l1_utran_nmr_info_req_type        utran_nmr_info_req;
  l1_additional_meas_info_req_type  add_meas_info_req;
  l1_process_imm_meas_req_type      imm_meas_req;

  #ifdef FEATURE_DUAL_SIM
  l1_ds_status_change_ind_type      ds_status_change_ind;
  #endif

  l1_m2m_cmd_type                   m2m;
} l1_cmd_union_type;

/* Command Structure for the Actual Command from Upper Layers to L1 */
typedef struct
{
  l1_cmd_hdr_type                 cmd_hdr;
  l1_cmd_union_type               cmd;

} l1_ext_cmd_type;


/*===========================================================================

FUNCTION    L1_GET_CMD_BUF

FILE NAME   l1m.c

DESCRIPTION
  This function returns a free command buffer, if available, or NULL if
  no command buffers are available.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to l1_ext_cmd_type

SIDE EFFECTS
  None

===========================================================================*/
extern l1_ext_cmd_type* l1_get_cmd_buf
(
  void
);

/*===========================================================================

FUNCTION    L1_PUT_CMD

FILE NAME   l1m.c

DESCRIPTION
  This function is used by the external clients of the L1 to submit
  commands to the L1. The caller owns the pointer to the command. 
  This function is called in the context of the caller. The function
  puts the command onto the External Command Queue and sets the
  External Command Queue signal of the L1 task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The External Command Queue signal is set.

===========================================================================*/
extern void l1_put_cmd
(
  /* Command to be sent to L1 task */
  l1_ext_cmd_type* cmd_ptr
);
/*===========================================================================

FUNCTION L1_PUT_FF_CMD

DESCRIPTION
  This function is used by the external clients of the L1 to submit
  Free floating commands to the L1. The caller owns the pointer to
  the command.  This function is called in the context of the caller. 
  The function puts the command onto the External Command Queue 
  and sets the RRC blocking command Queue signal of the L1 task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The Protocol Command Queue signal is set.

===========================================================================*/

void l1_put_ff_cmd
(
  l1_ext_cmd_type* cmd_ptr
);
#endif /* L1TASK_H */
