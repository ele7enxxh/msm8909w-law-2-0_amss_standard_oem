#ifndef TDSL1MTASK_H
#define TDSL1MTASK_H
/*===========================================================================

          L 1   T A S K   E X T E R N A L  D E C L A R A T I O N S

DESCRIPTION
This file provides an external declaration for the l1 task execution start
point function tds_l1_task.  This file is designed to be included by the UE
task controller so that it may start the L1 task.

EXTERNALIZED FUNCTIONS

  tds_l1_task
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

  tdsl1m_cphy_setup_cnf_get_status
    This function checks the operations status for a selected type and
    channel in the command packet of a CPHY_SETUP_CNF indication.
    This function returns the status of the requested operation as
    denoted in the indication packet, additionally the operation type is
    returned as an output parameter.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  The tds_l1_task function contains the L1 Manager task.  It must be
  specified in a call to rex_def task and started by a Task Controller.

  Copyright (c) 2000-2009 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/l1task.h_v   1.13   19 May 2002 03:29:12   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/inc/tdsl1mtask.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/20/11   cxd     Remove L1 external command TDSL1_CPHY_IMMEDIATE_MEAS_REQ
01/14/11   cxd     Remove the following L1 external commands
                   TDSL1_CPHY_CAPABILITY_REQ           =   0x0,
                   TDSL1_CPHY_ASET_UPDATE_REQ          =   0xA,
                   TDSL1_CPHY_HARD_HANDOFF_REQ         =   0xB,
                   TDSL1_CPHY_BCH_SCHEDULE_REQ         =   0xC,
                   TDSL1_CPHY_INTEGRITY_KEY_REQ        =   0x13,
                   TDSL1_CPHY_INTEGRITY_KEY_MAUC_REQ   =   0x14,
                   TDSL1_CPHY_IMMEDIATE_RESELECTION_REQ=   0x15,
                   TDSL1_CPHY_CELL_RESELECTION_RSP     =   0x16,
============================================================================*/

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "tdsl1rrcif.h"
#include "tdsl1macif.h"
#include "bmcl1if.h"

//#if defined(FEATURE_MMGPS) ||  defined(FEATURE_CGPS_TIME_TAGGING)
#include "tdsl1lsmif.h"
//#endif

#ifndef FEATURE_CMI
/* -------------------------------------- */
/* PUBLIC API of tdsl1mtask.h                 */
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
extern void tds_l1_task
(
  /* Required for REX, ignore */
  uint32 dummy
);

#else

/* Public portion of the API exists in tdstmcif.h */
#include "tdstmcif.h"

#endif /* FEATURE_CMI */

/* ----------------------------------------------------------------------- */
/* Private Section.                                                        */
/* ----------------------------------------------------------------------- */

/*--------------------------------------------------------------------------
                      UNION OF ALL L1 EXTERNAL COMMANDS

--------------------------------------------------------------------------*/

/* RRC or MAC to L1 Interface Primitives */
/* ************************************** *
 * This list is used by logging!!!        *
 * Send updates to cfa.team               *
 * ************************************** */
typedef enum
{
  /* Commands from RRC */
  TDSL1_CPHY_IDLE_REQ                 =   0x1,
  TDSL1_CPHY_DEACTIVATE_REQ           =   0x2,
  TDSL1_CPHY_STOP_TDSCDMA_MODE_REQ    =   0x3,
  TDSL1_CPHY_START_TDSCDMA_MODE_REQ   =   0x4,
  TDSL1_CPHY_SUSPEND_TDSCDMA_MODE_REQ =   0x5,
  TDSL1_CPHY_RESUME_TDSCDMA_MODE_REQ    =   0x6,
  TDSL1_CPHY_FREQ_SCAN_REQ            =   0x7,
  TDSL1_CPHY_ACQ_REQ                  =   0x8,
  TDSL1_CPHY_CELL_PARM_REQ            =   0x9,
  TDSL1_CPHY_SETUP_REQ                =   0xD,
  TDSL1_CPHY_DRX_REQ                  =   0xE,
  TDSL1_CPHY_CIPHERING_KEY_REQ        =   0xF,
  TDSL1_CPHY_MEASUREMENT_REQ          =   0x10,
  TDSL1_CPHY_CELL_SELECTION_REQ       =   0x11,
  TDSL1_CPHY_CELL_TRANSITION_REQ      =   0x17,
  TDSL1_CPHY_ACT_TIME_CANCEL_REQ      =   0x18,

  /* Commands from MAC */
  TDSL1_CPHY_UL_TX_CONTROL_REQ        =   0x19,
  TDSL1_CPHY_MAC_UL_CONFIG_CNF        =   0x1A,
  TDSL1_CPHY_MAC_DL_CONFIG_CNF        =   0x1B,
  TDSL1_CPHY_MAC_DL_CONFIG_AT_L1_FAIL_CNF =  0x1C,

  /* Commands from LSM (Location Services Manager) */
  TDSL1_CPHY_LSM_TIMETAG_REQ          =   0x1D,
  TDSL1_CPHY_LSM_CELL_MEAS_REQ        =   0x1E,
  TDSL1_CPHY_CELL_BAR_REQ             =   0x22,
  TDSL1_CPHY_UPD_DL_SYNC_PARMS        =   0x23,

  /* Commands for SIBS */
  TDSL1_CPHY_SIB_SCHED_CHANGE_IND     =   0x24,

  TDSL1_CPHY_GO_TO_SLEEP_IND          =   0x25,

  /* MBMS related command and indication */
  /* ----------------------------------- */

  /* MCCH DRX request */
  TDSL1_CPHY_MBMS_MCCH_DRX_REQ         =  0x26,
  /* MBMS IDLE request */
  TDSL1_CPHY_MBMS_IDLE_REQ             =  0x27,
  /* MBMS Preferred layer (FLC) request */
  TDSL1_CPHY_MBMS_PL_PARM_REQ          =  0x28,

  TDSL1_CPHY_UPD_OTA_PARMS_IND         = 0x29,

  /* Commands from RRC */
  /* Uplink Physical Channel Control */
  TDSL1_CPHY_UL_PHY_CHANNEL_CTRL_REQ   =   0x2A,
  
  /* L1 internal command */
  TDSL1_INTERNAL_SETUP_REQ_RESUME_AFTER_UL_RESYNC = 0x2B,
  TDSL1_INTERNAL_SETUP_REQ_RESUME_BHO_ACT_PLUS_8 = 0x2C,

#ifdef FEATURE_TDSCDMA_FREE_FLOATING_TASK
  TDSL1_CPHY_NUDGE_FOR_SFN_MISS_MATCH_RSP = 0x2D,
  TDSL1_CPHY_CHECK_S_CRIT_REQ = 0x2E,
  TDSL1_CPHY_ADDITIONAL_MEAS_DATA_REQ = 0x32,
  TDSL1_CPHY_GET_IMM_MEAS_REQ = 0x33,
  TDSL1_CPHY_NMR_INFO_REQ = 0x34,
  TDSCPHY_CHECK_SLEEP_RSP = 0x35,
  TDSCPHY_RSSI_REQ = 0x36,
  TDSCPHY_DATA_FOR_UI_DISP_REQ = 0x37,
  TDSCPHY_GET_MODEM_STATISTICS_REQ = 0x38,
#endif

#ifdef FEATURE_TDSCDMA_DSDS
 /*Dual SIM Commands*/
  TDSL1CPHY_DS_STATUS_CHANGE_IND       =   0x3A,
  
  TDSL1CPHY_RECOVER_WCDMA_REQ          =   0x3B,
  TDSL1CPHY_START_QTA_REQ              =   0x3C,
  
  TDSL1CPHY_STOP_QTA_REQ               =   0x3D,
#endif

#ifdef FEATURE_TDSCDMA_G2T_TUNE_AWAY
  TDSL1CPHY_DRX_TIMER_EXPIRY_IND       =   0X3E,
#endif

  TDSL1_CPHY_NUM_EXT_REQ_CMDS          =   0x3F
} tdsl1_cmd_enum_type;


/* Basic command type to be included by all custom command types.
 * The Processors and other units within the L1 and their associated
 * clients define command types that include the tdsl1_cmd_hdr_type
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
  tdsl1_cmd_enum_type       cmd_id;

  /* The 3 activation time attributes:
  ** - Type: NONE, SFN, or CFN.
  ** - SFN or CFN frame number, actual activation time.
  ** - Sequence number, for identifying the command. */
  tdsl1_act_time_enum_type  act_time_type;
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

} tdsl1_cmd_hdr_type;

/* Union of all L1 commands from Upper Layers */
typedef union
{
  /* Commands from RRC */
  tdsl1_idle_cmd_type                idle;
  tdsl1_deactivate_cmd_type          deact;
  tdsl1_stop_cmd_type                stop;
  tdsl1_start_cmd_type               start;
  tdsl1_suspend_cmd_type             suspend;
  tdsl1_resume_cmd_type              resume;
  tdsl1_freq_scan_cmd_type           freq_scan;
  tdsl1_acq_cmd_type                 acq;
  tdsl1_cell_parm_cmd_type           cell_parm;
  tdsl1_meas_cmd_type                meas;
  tdsl1_setup_cmd_type               setup;
  tdsl1_drx_cmd_type                 drx;
  tdsl1_ciphering_key_cmd_type       cipher_key;
  tdsl1_cell_selec_cmd_type          cell_sel;
  tdsl1_cell_transition_cmd_type     cell_trans;
  tdsl1_act_time_cancel_cmd_type     act_cancel;

  tdsl1_cell_bar_cmd_type            cell_bar;
  tdsl1_upd_dl_sync_parms_cmd_type dl_sync_parms;
  
  /* Commands from MAC */
  tdsl1_ul_tx_ctrl_req_type          ul_tx_ctrl;

  tdsl1_mac_config_cnf_type          mac_config_cnf;

//#if defined(FEATURE_MMGPS) || defined(FEATURE_CGPS_TIME_TAGGING)
  /* Commands from LSM (Location Services Manager) */
  tdsl1_lsm_timetag_type             lsm_timetag;
  //l1_lsm_cell_meas_type           lsm_cell_meas;
//#endif

  /* SIB sleep commands from RRC */
  tdsl1_sib_sched_change_ind_type    sib_sched_change_ind;


  tdsl1_upd_ota_parms_ind_cmd_type   upd_ota_parms_ind;

  /* Uplink Physical Channel Control */
  tdsl1_ul_phych_control_cmd_type         upphych;

  /* Add below type to make tdsl1_req_cmd union the subset of tdsl1_cmd_union_type */
  tdsl1_immediate_cell_eval_cmd_type immed_cell_eval;
  tdsl1_sfn_cmd_type                 sfn;
  tdsl1_cfn_cmd_type                 cfn;

  /* For Free Floating */
  tdsl1_rrc_nudge_update_cmd_type      rrc_nudge_status_update;
  tdsl1_get_modem_stat_req_cmd_type    get_modem_stat_req;
  tdsl1_s_criteria_req_type            s_criteria_req;
  tdsl1_sleep_rsp_type                 l1_sleep_rsp;
#ifdef FEATURE_TDSCDMA_PLI_NMR_UTRAN
  tdsl1_utran_nmr_info_req_type        utran_nmr_info_req;
#endif      
  tdsl1_additional_meas_info_req_type  add_meas_info_req;
  tdsl1_process_imm_meas_req_type      imm_meas_req;
#ifdef FEATURE_TDSCDMA_DSDS
  tdsl1_ds_status_change_ind_type ds_status_change_ind;
  tdsl1_start_qta_cmd_type           start_qta;
  tdsl1_stop_qta_cmd_type            stop_qta;
#endif 
} tdsl1_cmd_union_type;

/* Command Structure for the Actual Command from Upper Layers to L1 */
typedef struct
{
  tdsl1_cmd_hdr_type                 cmd_hdr;
  tdsl1_cmd_union_type               cmd;

} tdsl1_ext_cmd_type;


/*===========================================================================

FUNCTION    L1_GET_CMD_BUF

FILE NAME   l1m.c

DESCRIPTION
  This function returns a free command buffer, if available, or NULL if
  no command buffers are available.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to tdsl1_ext_cmd_type

SIDE EFFECTS
  None

===========================================================================*/
extern tdsl1_ext_cmd_type* tdsl1_get_cmd_buf
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
extern void tdsl1_put_cmd
(
  /* Command to be sent to L1 task */
  tdsl1_ext_cmd_type* cmd_ptr
);


/*===========================================================================
FUNCTION        tdsl1mcmd_process_immediate_meas_req

FILE NAME       l1mcmd.c

DESCRIPTION     This function processes the immediate measurement request
                from RRC.

DEPENDENCIES    None

RETURN VALUE    None
===========================================================================*/
/*it will replace l1_process_immediate_meas_req above in future*/
void tdsl1mcmd_process_immediate_meas_req_tmp 
( 
   tdsl1_immediate_meas_type_tmp *immed_meas, 
   tdsl1_intra_freq_rach_rpt_quan_struct_type *meas_quan_ptr
);

/*===========================================================================
FUNCTION        tdsl1mcmd_is_s_criteria_met

FILE NAME       l1mcmd.c

DESCRIPTION     This function computes S criteria based on Qqualmin and
                Qrxlevmin given by RRC

DEPENDENCIES    None

RETURN VALUE    TRUE if S criteria is met, FALSE otherwise
===========================================================================*/
extern boolean tdsl1mcmd_is_s_criteria_met_tmp( int16 q_rxlevmin, int16 ue_tx_max_pwr);

#endif /* TDSL1MTASK_H */

