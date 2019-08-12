#ifndef TDSL1M_L1M_H
#define TDSL1M_L1M_H
/*===========================================================================

                    L 1   T A S K   D E C L A R A T I O N S

DESCRIPTION
  This file contains global declarations and external references
  for the L1 Manager task.

  The L1 Manager interfaces the L1 Sequencer unit, the Device Driver layer,
  and the Processors within the L1 with the units external to the L1. It is
  the control interface between the L1 and external subsystems and provides
  a task context for the L1. The L1 Manager dispatches external and local
  commands to Processors in the L1. The L1 Manager also provides a basic state
  machine, and acts as a switcher between states of the phone.

  The L1 Manager waits on a set of signals (watchdog timer, task offline,
  task stop, command queues, etc). Whenever there is a signal the L1 Manager
  task will ascertain the source of the signal and performs appropriate
  processing.

  If there is a command on any of the command queues, the L1 Manager performs
  the following processing based on the command.

  Protocol Command: These are sub-states within a particular protocol or
  commands within a sub-state. Upon receipt of the above command, the L1
  Manager will invoke the currently active Protocol Processor to process
  the command.

  Local Command: These commands provide local state control and
  synchronization within the L1. They are also state independent commands.
  The Local Commands are sent through the Local Queue. The local state maps
  all local commands directly to protocol sub-state processing. Upon receipt
  of a Local Command, the L1M will invoke the corresponding Local Processor
  to process the command.

EXTERNALIZED FUNCTIONS

 tdsl1m_free_cmd_buf
   This function is used by L1 to free the external command buffer once L1 is
   finished processing the command. This function is executed in the context
   of the caller.

 tdsl1m_cmd_confirm
   This function sends a confirmation cmd to the upper layer.

 tdsl1m_cphy_setup_cnf_put_status
   This function is used by the L1 modules to update the tdsl1m_global.cphy_setup_status
   buffer in the course of performing the steps of a TDSL1_CPHY_SETUP_REQ.

 l1_put_local_cmd
   This function is used by the internal units of the L1 to submit local
   commands. This function puts the command onto the L1M Local Command
   Queue and sets the Local Command Queue for the L1 task.

   This function is called in the context of the L1M thread or in the
   context of an ISR within the L1.

 tdsl1m_get_local_cmd_buf
   This function is used by the internal clients of the L1 Manager to get a
   command buffer.

 tdsl1m_get_rrc_cmd_buf
   This function gets the command buffer to send a command/confirmation
   to RRC.

INTERNALIZED FUNCTIONS

  l1_process_ext_cmd
    This function de-queues a command from the protocol command queue,
    invokes the command processor of the currently active protocol processor
    and passes the command to it.
    If there are no more protocol commands pending on the protocol command
    queue, the protocol command queue signal is cleared.
    This function is called in the context of the L1 thread.

  tdsl1m_cmd_err_ind
    This function sends a command error indication to the upper layer.

  l1_free_local_cmd_buf
    This function frees the command buffer associated with the local command.

  tdsl1m_process_local_cmd
    This function de-queues a command from the local command queue,
    invokes the command processor of the currently active local processor
    and passes the command to it.
    If there are no more local commands pending on the local command
    queue, the local command queue signal is cleared.

    This function is called in the context of the L1M thread.

  tdsl1m_init
    This function does all initializations necessary for proper operation
    of the L1.

    This function should be called before the L1M can be used.

  tdsl1m_mdsp_init_done_isr
    This ISR is called after the MDSP is initialized. The MDSP_INT4 is
    muxed with the init done and rach done interrupts. This function will
    set the MDSP_INIT_DONE_SIG signal.

  L1M Idle State Functions
  tdsl1m_idle_init: Called when L1M transitions to Idle state.
  tdsl1m_idle_cleanup: Called when L1M leaves Idle state.
  tdsl1m_idle_cmd_processor: Processes external commands received by L1 in Idle
                          state.

  L1M ACQ State Functions
  tdsl1m_acq_init: Called when L1M transitions to ACQ state.
  tdsl1m_acq_cleanup: Called when L1M leaves ACQ state.
  tdsl1m_acq_cmd_processor: Processes external commands received by L1 in ACQ
                         state.

  L1M BCH State Functions
  tdsl1m_bch_init: Called when L1M transitions to BCH state.
  tdsl1m_bch_cleanup: Called when L1M leaves BCH state.
  tdsl1m_bch_cmd_processor: Processes external commands received by L1 in ACQ
                         state.

  L1M PCH State Functions
  tdsl1m_pch_init: Called when L1M transitions to PCH state.
  tdsl1m_pch_cleanup: Called when L1M leaves PCH state.
  tdsl1m_pch_cmd_processor: Processes external commands received by L1 in PCH
                         state.

  L1M FACH State Functions
  tdsl1m_fach_init: Called when L1M transitions to FACH state.
  tdsl1m_fach_cleanup: Called when L1M leaves FACH state.
  tdsl1m_fach_cmd_processor: Processes external commands received by L1 in FACH
                         state.

  L1M DCH State Functions
  tdsl1m_dch_init: Called when L1M transitions to DCH state.
  tdsl1m_dch_cleanup: Called when L1M leaves DCH state.
  tdsl1m_dch_cmd_processor: Processes external commands received by L1 in DCH
                         state.

  L1M Deactive State Functions
  tdsl1m_deact_init: Called when L1M transitions to Deactive state.
  tdsl1m_deact_cleanup: Called when L1M leaves Deactive state.
  tdsl1m_deact_cmd_processor: Processes external commands received by L1 in
                           Deactive state.

  L1M Wait for TRM Stop Functions
  tdsl1m_wait_for_trm_stop_init: Called when L1M transitions to Wait for TRM Stop state.
  tdsl1m_wait_for_trm_stop_cleanup: Called when L1M leaves Wait for TRM Stop state.
  tdsl1m_wait_for_trm_stop_cmd_processor: Processes external commands received by L1 in
                           Wait for TRM Stop state.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  The L1_task function contains the L1 Manager task.  It must be
  specified in a call to rex_def task and started by a Main Control task.

  Copyright (c) 2000 - 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsl1m.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
06/26/14    zli     Add timeout mechanism for X2T measurement messages
02/27/14   yufu    SGTDS +G DSDA mode settings
02/02/11   sq      Removed obsolete header file 
01/25/11   cxd     Merge MSGR queues for L1 task
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "customer.h"
#include "rex.h"
#include "queue.h"
#include "tdsl1def.h"
#include "tdsl1mtask.h"
#include "tdssrchcmd.h"
#include "tdsl1rrcif.h"

#ifdef FEATURE_QSH_EVENT_METRIC
#include <qsh.h>
#endif

#ifdef FEATURE_TDSCDMA_HSUPA
#endif

#ifdef FEATURE_TDSCDMA_CELL_RESELECT_INTER_S
#include "tdssrchgsm.h"
#endif

#include "tdsulcmd.h"
#include "tdsdltrchmeas.h"
#include "tdsdrxcmd.h"

#include "tdshscfg.h"

#ifdef FEATURE_L1_LOG_ON_DEMAND
#include "tdslogod.h"
#endif

#ifdef FEATURE_TDSCDMA_QUALITY_MEAS
#include "tdsqualmeas.h"
#endif

#include "tdsrrccmd_v.h"

#include "tdsdrxentity.h"

#include "tdstrm.h"

#include "tdsdlphch.h"
#include "tdsdlchmgr.h"
#include "tdsdlpccpch.h"
#include "tdsseq.h"
#include "msg.h"


#if defined(FEATURE_TDSCDMA_TO_LTE) && defined(FEATURE_TDSCDMA_ABSOLUTE_PRIORITY)
#include "tdssrchlte.h"
#endif

#ifdef FEATURE_TDSCDMA_L1_PERFORMANCE_WATCHDOG
#include "tdsstmr.h"
#endif

#ifdef FEATURE_SGLTE
#include "tdssrchsglte.h"
#endif
#include "tdsl1dsds.h"

#include "tdsl1_profiling_log.h"


#include "tdsl1msgr.h"

#include "wwan_coex_mgr.h"

#ifdef FEATURE_TDSCDMA_DSDA
#include "subs_prio.h"
#endif

#ifdef FEATURE_TDSCDMA_L1M_PROFILING_LOG_PACKET
#include "tdseulenc.h"
#endif

/*===========================================================================

                        FEATURE VALIDATIONS

===========================================================================*/


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                L1 MANAGER TASK SIGNAL DECLARATIONS
---------------------------------------------------------------------------*/
#define TDSL1M_RPT_TIMER_SIG                         0x00000001  /* Watchdog */
#define TDSL1M_EXT_CMD_Q_SIG                         0x00000002  /* Protocol Command Q */
#define TDSL1M_LOCAL_CMD_Q_SIG                     0x00000004  /* Local Command Q    */
#define TDSL1M_COMMAND_DONE_SIG                 0x00000008
//avisvana- removed after discussion with Andy; Out of signals and to provide signal for FF
//#define TDSL1M_SRCH_ABORT_SIG                       0x00000010
#ifdef FEATURE_TDSCDMA_FREE_FLOATING_TASK
#define TDSL1M_FF_CMD_Q_SIG                          0x00000010
#endif

#define TDSL1M_SLEEP_ABORT_TIMEOUT_SIG     0x00000020  /* Sleep abort timeout  */
#define TDSL1M_MSGR_SIG                                   0x00000040
#define TDSL1M_RF_TUNE_DONE_SIG                   0x00000080
#define TDSL1M_MDSP_DELAY_TIMER_SIG           0x00000100
#define TDSL1M_MSGR_FW_SIG           0x00000200
#define TDSL1M_ENA_CELL_RESEL_TIMER_SIG    0x00000400
#define TDSL1M_CPHY_DROP_ALL_TIMEOUT_SIG 0x00000800
#define TDSL1M_GSM_MEAS_DONE_SIG                0x00001000
/* These signals are defined in task.h                        0x00002000  ** TASK_OFFLINE_SIG   **
** and are global and should not be                           0x00004000  ** TASK_STOP_SIG      **
** reused by individual tasks!!!                                0x00008000  ** TASK_START_SIG     */
#define TDSL1M_SLEEP_PERIOD_COMPLETE_SIG   0x00010000  /* End of sleep       */


#ifdef FEATURE_FORCE_DROP_ADD_HSDPA
#define TDS_HS_FORCE_DROP_TIMEER_SIG 0x00020000
#else
#define TDSL1M__FREE_WL1_SIGNAL_3_              0x00020000
#endif //FEATURE_FORCE_DROP_ADD_HSDPA
#define TDSL1M_TIMEOUT_SIG                               0x00040000  /* mDSP init, TRM req */
#define TDSL1M_AUTO_BLER_MEAS_TIMER_SIG    0x00080000
#define TDSL1M_OUT_OF_SERVICE_TIMER_SIG     0x00100000
#define TDSL1M_NV_CMD_SIG                                0x00200000
#define TDSL1M_DRX_DELAYED_TIMER_SIG           0x00400000
//#define TDSL1M__RESERVED_FOR_WL1_RPC_1__           0x00800000
#define TDSL1M_PRI_RADIO_TIMEOUT_SIG                 0x00800000  /* primary antenna, TRM */

#define TDSL1M_STATE_CLEANUP_TIMEOUT_SIG   0x01000000
#define TDSL1M_CPHY_SETUP_TIMEOUT_SIG         0x02000000
//#define TDSL1M__RESERVED_FOR_WL1_RPC_2__           0x04000000 
#define TDSL1M_BAND_CHANGE_TIMEOUT_SIG               0x04000000  /* RF band change, TRM */

#define TDSRF_MSGR_RF_SIG                                  0x08000000 
#define TDSL1M_PCCPCH_DECODE_TIMER_SIG        0x10000000
#define TDSL1M_TFW_LOG_MASK_TIMER_SIG         0x20000000

#define TDSL1_TRM_LOCK_ACQ_SIG               0x00000080 /* using the mask of TDSL1M_ENA_CELL_RESEL_TIMER_SIG & TDSL1M_CPHY_DROP_ALL_TIMEOUT_SIG */
#define TDSL1_TRM_LOCK_FAILED_SIG            0x00800000 /* using the mask of TDSL1M_SLEEP_PERIOD_COMPLETE_SIG & TDSL1M_AUTO_BLER_MEAS_TIMER_SIG & TDSL1M__FREE_WL1_SIGNAL_3_ */
#define TDSL1_QTA_TIMER_EXPIRY               0x03000000 /* using the mask of TDSL1M_STATE_CLEANUP_TIMEOUT_SIG & TDSL1M_CPHY_SETUP_TIMEOUT_SIG */
/* when QTA start, there shouldn't be CPHY_SETUP and STATE_CLEANUP */

#ifdef FEATURE_TDSCDMA_MTPL_THERMAL_BACKOFF
#define TDSL1M_MTPL_BACKOFF_DUTYCYCLE_SIG            0x40000000
#define TDSL1M_MTPL_BACKOFF_STEP_SIG                 0x80000000
#endif /* FEATURE_TDSCDMA_MTPL_THERMAL_BACKOFF */

#define TDSL1M_FW_RSP_TIMEOUT_SIG                    0x00004000

/* If the define is called from task lock then it will not print F3 */

#define TDSL1M_TDS_TASKLOCK_MSG3( LVL, FMT, A, B, C ) \
        if(tdsl1m_global.print_msgs_around_tasklock)           \
        MSG_SPRINTF_3( MSG_SSID_WCDMA_L1, MSG_LEGACY_ ## LVL, FMT, A, B, C)

/******************************************************************************
**                ADD NEW SIGNALS JUST ABOVE THIS COMMENT!!!                 **
** Make sure they don't match the signals described just below this comment! **
******************************************************************************/

/* These signals are also defined          0x20000000 ** VS_OP_COMPLETE_SIG  **
** in task.h and MAY be global and         0x40000000 ** FS_OP_COMPLETE_SIG  **
** should be verified to be OK to
** reuse here, if absolutely necessary.  Consider yourself warned!!!         */


#define TDSL1M_OUT_OF_SERVICE_TIME_FACH        4000        /* 4 seconds in FACH */
#define TDSL1M_OUT_OF_SERVICE_TIME_PCH   (tdsrrc_get_oos_timer_value())       /* 12 or 24 seconds in PCH */
/* Timeout for state cleanup during Idle, Stop, Deactivate or Suspend cmd   */
#define TDSL1M_STATE_CLEANUP_TIMEOUT           450  // 1000. make it smaller than cphy_setup timer length

#define TDSL1M_DROP_ALL_CHANNEL_TIMEOUT           1000

/* Timeout for phychan setup request */
#define TDSL1M_CPHY_SETUP_TIMEOUT              500 //10000

/* Timeout for searcher abort command */
#define TDSL1M_SRCH_ABORT_ALL_TIMEOUT             20 /* 20 ms should be enough */

/* timeout for FW RSP */
/*********************************************************************************************/
#define TDSL1M_FW_REACQ_RSP_TIMEOUT_MSEC                     15 /* 15 ms should be enough */
#define TDSL1M_FW_STATE_CFG_RSP_TIMEOUT_MSEC                 10 /* 10 ms should be enough */
#define TDSL1M_FW_IRAT_T2X_CLEANUP_RSP_TIMEOUT_MSEC          10 /* 10 ms should be enough */
#define TDSL1M_FW_SRCH_GAP_DET_RSP_TIMEOUT_MSEC              90 /* 90 ms should be enough */
#define TDSL1M_FW_SRCH_GAP_DET_RSP_IRAT_TIMEOUT_MSEC         50 /* 50 ms should be enough */
#define TDSL1M_FW_SRCH_SYNCDL_MID_DET_RSP_TIMEOUT_MSEC       380/* 380 ms should be enough */
#define TDSL1M_FW_MEAS_ALL_RSCP_RSP_TIMEOUT_MSEC             15 /* 15 ms should be enough */
#define TDSL1M_FW_IRAT_MEAS_ALL_RSCP_RSP_TIMEOUT_MSEC        30  /*for IRAT case, 30ms should be enough*/
#define TDSL1M_FW_SRCH_POWER_SCAN_RSP_TIMEOUT_MSEC           50 /* 50 ms should be enough */
/*********************************************************************************************/

/* Used when waiting for mDSP confirmations.  Reducing timeouts from 5 seconds
** to 1 second, but keep both around to avoid compilation problems. */
#define TDSL1M_ONE_SECOND                         1000
#define TDSL1M_FIVE_SECONDS                       5000

/*Maximum no. of barred cells*/
#define TDSL1M_MAX_BARRED_CELLS 12 /*Just an arbitary no. 2 * ASET size*/

/* for TSN Cx8 */
#define TDSL1M_TS2_START 16640 /* TS2 start: (2*864+352)*8 */

#define CXM_FREQID_IDX_BAND_A 0
#define CXM_FREQID_IDX_BAND_E 1
#define CXM_FREQID_IDX_BAND_F 2
#define CXM_FREQID_IDX_DL 3
#define CXM_FREQID_IDX_UL 4
#define CXM_FREQID_IDX_BHO_DL 5
#define CXM_FREQID_IDX_BHO_UL 6
#define CXM_FREQID_BAND_A_TYPICAL_FREQ  10055
#define CXM_FREQID_BAND_E_TYPICAL_FREQ  11505
#define CXM_FREQID_BAND_F_TYPICAL_FREQ  9405


#define DSDA_FREQ_ID_INTLOCK() (REX_ISR_LOCK(&tdsl1m_freq_id_mutex))
#define DSDA_FREQ_ID_INTFREE() (REX_ISR_UNLOCK(&tdsl1m_freq_id_mutex))

#ifdef FEATURE_TDSCDMA_CELL_SELECTION_INTER_SIB_SLEEP
#define TDSL1M_BCH_SLEEP() ((TDSL1M_BCH == tdsl1m_global.l1_state) && tdsl1m_support_sleep_in_bch_state)
#endif

/* Enum to indicate supported release type */
typedef enum
{
  TDSL1M_NV_REL_INDICATOR_R99  = 0,
  TDSL1M_NV_REL_INDICATOR_REL5 = 1,
  TDSL1M_NV_REL_INDICATOR_REL6 = 2,
  TDSL1M_NV_REL_INDICATOR_REL7 = 3,
  TDSL1M_NV_REL_INDICATOR_REL8 = 4,
  TDSL1M_NV_NUM_REL_INDICATOR  = 5
} tdsl1m_nv_rel_ind_enum_type;

/* default NV release indicator value in case NV value is invalid */
#ifdef FEATURE_TDSCDMA_REL8
#define TDSL1M_DEFAULT_NV_REL_IND TDSL1M_NV_REL_INDICATOR_REL8
#else
#define TDSL1M_DEFAULT_NV_REL_IND TDSL1M_NV_REL_INDICATOR_REL7
#endif
/* These are states of operation of WCDMA L1 */
typedef enum
{
  TDSL1M_IDLE = 0,
  TDSL1M_FS = 1,
  TDSL1M_ACQ = 2,
  TDSL1M_SYNC = 3,
  TDSL1M_BCH = 4,
  TDSL1M_PCH = 5,
  TDSL1M_FACH = 6,
  TDSL1M_DCH = 7,
  TDSL1M_PCH_SLEEP = 8,
  TDSL1M_STOPPED = 9,
  TDSL1M_SUSPENDED = 10,
  TDSL1M_PCH_BPLMN = 11,
  TDSL1M_WAIT_FOR_TRM_STOP = 12,
  TDSL1M_IRAT = 13,
  TDSL1M_NUM_STATES
} tdsl1m_state_enum_type;

#ifdef FEATURE_TDSCDMA_ANTENNA_SWITCH_DIVERSITY
typedef enum
{
  TDSL1M_ASDIV_SLEEP = 0,
  TDSL1M_ASDIV_RX,
  TDSL1M_ASDIV_TX,
  TDSL1M_ASDIV_RXACQ,
  TDSL1M_ASDIV_INACTIVE
} tdsl1m_asdiv_state_enum_type;
#endif

typedef enum
{
  TDSL1M_ACQ_INIT =0,
  TDSL1M_ACQ_SRCH_DONE,
  TDSL1M_ACQ_PCCPCH_ADD_DONE,
  TDSL1M_ACQ_DECODE_DONE,
  TDSL1M_ACQ_SLAM_DONE,
  TDSL1M_ACQ_COMPLETE,
  TDSL1M_ACQ_INVALID,
  TDSL1M_ACQ_NUM_SUBSTATES
} tdsl1m_acq_substate_enum_type;
/* these are the submodules of layer 1 */
typedef enum
{
  TDSL1M_SRCH_MODULE = 0,
  TDSL1M_DL_MODULE,
  TDSL1M_UL_MODULE,
  TDSL1M_SEQ_MODULE,
  TDSL1M_NUM_L1_MODULES,

  /* following module is DCH state spcific and is the
     part of DL_MODULE_CLEANUP */
  //TDSL1M_CM_MODULE = 6,
  //TDSL1M_CM_GSM_MEAS_MODULE,
  //TDSL1M_CM_INTERF_MEAS_MODULE,
  TDSL1M_HS_CFG_MODULE

#ifdef FEATURE_TDSCDMA_HSUPA
/*following modules are specific to DCH state */
  ,TDSL1M_E_DL_MODULE,
  TDSL1M_E_UL_MODULE
#endif

#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
   ,TDSL1M_GAPMGR_MODULE,
   TDSL1M_T2L_CM_MEAS_MODULE
#endif

  ,TDSL1M_HSD_A2_MODULE
  ,TDSL1M_MAX_MODULES

} tdsl1m_module_enum_type;

typedef enum
{
  TDSL1M_DROP_ALL_IDLE_REQ = 0,
  TDSL1M_DROP_ALL_SETUP_REQ,
  TDSL1M_DROP_ALL_SUSPEND_REQ,
  TDSL1M_DROP_ALL_STOP_REQ,
  TDSL1M_DROP_ALL_DEACT_REQ,
  TDSL1M_DROP_ALL_IMPLIED,
  TDSL1M_DROP_HSPA_BEFORE_DPCH_DROP,
  TDSL1M_DROP_ALL_INVALID
} tdsl1m_drop_all_cause_enum_type;

typedef enum
{
  DSDA_CONFIG_PCCPCH = 0,
  DSDA_CONFIG_SCCPCH = 1,
  DSDA_CONFIG_INTERNAL_PCCPCH = 2,
  DSDA_CONFIG_VOICE_CALL = 3,
  DSDA_CONFIG_PS_CALL = 4,
  DSDA_CONFIG_MRAB_CALL = 5,
  DSDA_CONFIG_FS = 6,
  DSDA_CONFIG_ACQ_GAP_DETECTION = 7,
  DSDA_CONFIG_ACQ_SYNCDL_MID_DET_SMALL_WIN = 8,
  DSDA_CONFIG_ACQ_SYNCDL_MID_DET_FULL_WIN = 9,
  DSDA_CONFIG_BPLMN_FS = 10,
  DSDA_CONFIG_BPLMN_MEASURE_ALL_RSCP = 11,
  DSDA_CONFIG_CONN_FACH = 12,
  DSDA_CONFIG_REACQ = 13,
  DSDA_CONFIG_PCH_CELL_RESEL = 14,
  DSDA_CONFIG_HHO_RESYNC = 15,
  DSDA_CONFIG_RRC_HIGH_PRI_SIG = 16,
  DSDA_CONFIG_BUMP_UP_UL_TX_PRI = 17,
  DSDA_CONFIG_SUB_PRI_CHG = 18,
  DSDA_CONFIG_CONFIG_INVALID
} tdsl1m_dsda_config_enum_type;


/* Flag to hold value of pci selection from mimo param cmd */
extern uint8 tdshscfg_hs_default_pci_selection;

/* this macro sets all the bits in the L1 module cleanup done mask */
#define TDSL1M_INIT_CLEANUP_MODULE_DONE() \
  (tdsl1m_global.module_cleanup_done_mask = ( (((uint16) 1) << (uint16)TDSL1M_NUM_L1_MODULES) - 1))


/* this macro sets all the bits in the L1 module cleanup done mask for DCH state*/
#ifdef FEATURE_TDSCDMA_HSUPA
#define TDSL1M_INIT_DCH_CLEANUP_MODULE_DONE() \
  TDSL1M_INIT_CLEANUP_MODULE_DONE(); \
  (tdsl1m_global.module_cleanup_done_mask |= ((uint16) (1 << (uint16)TDSL1M_HS_CFG_MODULE))); \
  (tdsl1m_global.module_cleanup_done_mask |= (TDSEDLCFG_IS_EUL_ACTIVE()) ? ((uint16) (1 << (uint16)TDSL1M_E_DL_MODULE)) : 0); \
  (tdsl1m_global.module_cleanup_done_mask |= (TDSEDLCFG_IS_EUL_ACTIVE()) ? ((uint16) (1 << (uint16)TDSL1M_E_UL_MODULE)) : 0);
#else
#define TDSL1M_INIT_DCH_CLEANUP_MODULE_DONE() \
  TDSL1M_INIT_CLEANUP_MODULE_DONE(); \
  (tdsl1m_global.module_cleanup_done_mask |= ((uint16) (1 << (uint16)TDSL1M_HS_CFG_MODULE)));
#endif

#define TDSL1M_INIT_MODULE_DROP_DONE_MASK() \
 (tdsl1m_global.module_drop_done_mask |= ((uint16) (1 << (uint16)TDSL1M_DL_MODULE))); \
 (tdsl1m_global.module_drop_done_mask |= ((uint16) (1 << (uint16)TDSL1M_UL_MODULE))); \
 (tdsl1m_global.module_drop_done_mask |= ((uint16) (1 << (uint16)TDSL1M_HS_CFG_MODULE))); \
 (tdsl1m_global.module_drop_done_mask |= (TDSEDLCFG_IS_EUL_ACTIVE() || TDSEDLCFG_IS_NON_SCHED_GRANT_ACTIVE()) ? ((uint16) (1 << (uint16)TDSL1M_E_DL_MODULE)) : 0); \
 (tdsl1m_global.module_drop_done_mask |= (TDSEDLCFG_IS_EUL_ACTIVE() || TDSEDLCFG_IS_NON_SCHED_GRANT_ACTIVE()) ? ((uint16) (1 << (uint16)TDSL1M_E_UL_MODULE)) : 0); \
 (tdsl1m_global.module_drop_done_mask |= ((uint16) (1 << (uint16)TDSL1M_HSD_A2_MODULE)));

/* this macro clears the bit in the L1 module cleanup done mask associated
   with the passed in L1 module */
#define TDSL1M_MODULE_CLEANUP_DONE(module) \
  (tdsl1m_global.module_cleanup_done_mask &= ~( ((1u) << ((uint16)module))))

#define TDSL1M_MODULE_DROP_DONE(module) \
  (tdsl1m_global.module_drop_done_mask &= ~( ((1u) << ((uint16)module))))

/* This macro sets all the bits in the L1 module cleanup done mask */
#define TDSL1M_MODULE_SET_ALL_CLEANUP_DONE() \
  (tdsl1m_global.module_cleanup_done_mask = 0)

/* this macro detects that all module cleanup is complete by checking if the
   L1 module cleanup done mask is equal to 0 */
#define TDSL1M_MODULE_CLEANUP_COMPLETE() (tdsl1m_global.module_cleanup_done_mask == 0)

#define TDSL1M_MODULE_DROP_COMPLETE() (tdsl1m_global.module_drop_done_mask == 0)

/* Init DL and SRCH module cleanup flags (used for SIB mod) */
#define TDSL1M_INIT_MODULE_CLEANUP_DONE_FOR_SIB_MOD( )               \
  (tdsl1m_global.module_cleanup_done_mask = ((uint16) (1 << (uint16)TDSL1M_SRCH_MODULE)))

/* added FEATURE_TRITON_MODEM switch to remove warnings for Triton initial builds */
/*  ; \
  (tdsl1m_global.module_cleanup_done_mask |= ((uint16) (1 << (uint16)TDSL1M_CM_INTERF_MEAS_MODULE)));*/
/* end of warning removal switch */

#define TDSL1M_SET_FW_ISR_IS_RUN_FLAG( ) (tdsl1m_global.fw_isr_is_run_flag = TRUE)
#define TDSL1M_CLR_FW_ISR_IS_RUN_FLAG( ) (tdsl1m_global.fw_isr_is_run_flag = FALSE)

#define TDSL1M_SET_STMR_ISR_IS_RUN_FLAG( ) (tdsl1m_global.stmr_isr_is_run_flag = TRUE)
#define TDSL1M_CLR_STMR_ISR_IS_RUN_FLAG( ) (tdsl1m_global.stmr_isr_is_run_flag = FALSE)

#define TDSL1M_IS_PROC_STOP() (tdsl1m_global.l1m_stop_proc_flag == TRUE)

#define TDSL1M_WAIT_ISR_LOOP 200
#define TDSL1M_WAIT_ISR_TIME 50

typedef struct
{
  tdsl1def_l1_local_cmd_hdr_type hdr;

  tdsl1m_module_enum_type module;

} tdsl1m_module_cleanup_done_cmd_type;

typedef struct
{
  tdsl1def_l1_local_cmd_hdr_type hdr;

  tdsl1m_module_enum_type module;

} tdsl1m_module_drop_done_cmd_type;

#define TDSL1M_IS_FLD_PERF_OPT_DISABLED( ) ( FALSE )


#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED
typedef struct
{
	/* local command header type */
    tdsl1def_l1_local_cmd_hdr_type hdr; /*for sub-log pkt type identify*/

	/*sub type to identify which dl logpkt type*/
	tdsl1def_l1_local_cmd_enum_type  sub_cmd_code;

} tdsl1_dl_gen_logpkt_cmd_type;
#endif
#ifdef FEATURE_TDSCDMA_HSUPA
/* Local command structure for EUL TDSL1_CPHY_SETUP_DONE cmd */
typedef struct 
{
  tdsl1def_l1_local_cmd_hdr_type hdr;
  boolean status;
} tdsl1m_eul_cphy_setup_local_cmd_type;

/* Local command structure for EUL DL COMB_CHAN_CFG cmd */
typedef struct 
{
  tdsl1def_l1_local_cmd_hdr_type hdr;
  uint16 dummy;
} tdsl1m_eul_dl_comb_chan_cfg_cmd_type;

/* Local command structure for EUL DL FRM_BDRY cmd */
typedef struct 
{
  tdsl1def_l1_local_cmd_hdr_type hdr;
  uint16 dummy;
} tdsl1m_eul_dl_frm_bdry_cmd_type;

/* Local command structure for EUL DL PHYCHAN_DONE cmd */
typedef struct 
{
  tdsl1def_l1_local_cmd_hdr_type hdr;
  uint16 dummy;
} tdsl1m_eul_dl_phychan_done_cmd_type;

/* Local command structure for EUL UL PHYCHAN_DONE cmd */
typedef struct 
{
  tdsl1def_l1_local_cmd_hdr_type hdr;
  uint16 dummy;
} tdsl1m_eul_ul_phychan_done_cmd_type;

/*Local command structure for EDL LOGGING CMD*/
typedef struct
{
  tdsl1def_l1_local_cmd_hdr_type hdr;
  uint16 dummy;
}tdsl1m_edl_logging_local_cmd_type;
#if defined( FEATURE_TDSCDMA_L1_LOGGING_ENABLED )
typedef struct 
{
  tdsl1def_l1_local_cmd_hdr_type hdr;
  tdseullog_logging_done_cmd_enum_type log_type;
} tdsl1m_eul_logging_local_cmd_type;
#endif

#endif

typedef struct
{
  tdsl1def_l1_local_cmd_hdr_type hdr;
  uint32 evt_idx;
}tdsl1m_gsm_periodic_meas_cmd_type;

typedef struct
{
  tdsl1def_l1_local_cmd_hdr_type hdr;
}tdsl1m_pending_cleanup_proc_cmd_type;

#ifdef FEATURE_TDSCDMA_INTERF_MEAS_RES_OPT
#define TDSL1M_DEFAULT_INTERF_MEAS_RES_OPT_IND FALSE
#endif

typedef enum
{
  TDSL1M_NOT_D2D_F2D = 0,
  TDSL1M_D2D_RECFG_REGULAR,
  TDSL1M_D2D_RECFG_WORKING_FREQ_CHANGE,
  TDSL1M_D2D_HHO_INTRAF,
  TDSL1M_D2D_HHO_INTERF,
  TDSL1M_D2D_BHO_T0_INTRAF,
  TDSL1M_D2D_BHO_T0_INTERF,
  TDSL1M_D2D_BHO_T80_INTRAF,
  TDSL1M_D2D_BHO_T80_INTERF,
  TDSL1M_F2D_SETUP_REGULAR,
  TDSL1M_X2D_SETUP_WORKING_FREQ_CHANGE,
  TDSL1M_F2D_HHO_INTRAF,
  TDSL1M_F2D_HHO_INTERF,
  TDSL1M_F2D_BHO_T0_INTRAF,
  TDSL1M_F2D_BHO_T0_INTERF,
  TDSL1M_DCH_RECOVER_HHO,
  TDSL1M_DCH_RECOVER_BHO
#ifdef FEATURE_TDSCDMA_L2TD_PS_HANDOVER 
  ,
  TDSL1M_DCH_X2T_PS_HHO,
  TDSL1M_DCH_X2T_PS_BHO
#endif
  ,
  TDSL1M_X2D_BHO_T0_INTRAF,
  TDSL1M_X2D_BHO_T0_INTERF,
} tdsl1m_dch_setup_req_enum_type;

#ifdef FEATURE_TDSCDMA_MOTION_SENSOR_AID_MEAS_SUPPRESS

typedef struct
{
  tdsl1def_l1_local_cmd_hdr_type hdr;
  boolean start_sensor;
}tdssrchmeas_sensor_aid_cmd_type;
#endif

#ifdef FEATURE_TDSCDMA_BAND_AVOID
/* Local command structure for Band Avoidance power sampling command */
typedef struct
{
  /* local command header type */
  tdsl1def_l1_local_cmd_hdr_type       hdr;

  /* Rx Power */
  int32 rx_pwr;

  /* Tx Power */
  int32 tx_pwr;

}tdsl1m_cxm_band_avoid_cmd_type;

#endif

typedef struct
{
  /* local command header type */
  tdsl1def_l1_local_cmd_hdr_type       hdr;
}tdsirat_x2t_fw_cmd_type;

#if defined(FEATURE_QSH_EVENT_METRIC) || defined(FEATURE_QSH_DUMP)
typedef struct
{
  /* local command header type */
  tdsl1def_l1_local_cmd_hdr_type       hdr;

  /* the memory address allocated by QSH for tds */
  qsh_client_cb_params_s     param;

}tdsl1m_qsh_report_cmd_type;

typedef struct
{
  /* start address for metric */
  void                            *data_ptr;

  /* metric size in bytes */
  size_t                          size_bytes;

  /* metric context id that the client needs to pass into 
     qsh_client_metric_log(...) after writing a metric */
  qsh_client_metric_context_id_t  metric_context_id;

  /* if the metric is started (TRUE) or stopped (FALSE) */
  boolean                         started;
}tdsl1m_qsh_metric_cfg_struct_type;

#endif /*defined(FEATURE_QSH_EVENT_METRIC) || defined(FEATURE_QSH_DUMP)*/

/*--------------------------------------------------------------------------
                      UNION OF ALL L1 LOCAL COMMANDS

--------------------------------------------------------------------------*/
typedef union
{
  tdsl1def_l1_local_cmd_hdr_type        hdr;

  tdssrchcmd_done_cmd_type           srch_done;
  tdssrchcmd_ops_cmd_type            srch_ops;
//jintao add for dl fach race condition
  tdsdlchmgr_cell_change_struct_type  cell_change_ind;

#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
  tdssrchlte_eutra_periodic_meas_cmd_type    eutra_meas;
#endif
  tdsdlphch_cfg_done_cmd_type      dl_phch_cfg_done;
  tdsdlpccpch_acq_bch_decode_done_cmd_type    dl_acq_bch_decode_done;
  dl_bcch_weak_ind_cmd_type    dl_bcch_send_weak_ind;
  tdsdlpccpch_bch_crc_status_type       dl_bch_crc_status;
  tdsdltrchmeas_trblk_crc_status_type     dl_trblk_crc_status;

#ifdef FEATURE_TDSCDMA_QUALITY_MEAS
  /* indicates that CRC info available for a transport channel */
  tdsqualmeas_crc_update_cmd_type  trch_crc_cmd;
#endif


  /* activation time related local command */
  tdsseq_act_time_local_cmd_type  act_time_cmd;

  /* physical channel setup related local commands */
  //tdsdlchmgr_dl_cphy_aset_done_cmd_type   dl_cphy_aset_done;
  tdsdlchmgr_dl_cphy_setup_local_cmd_type dl_cphy_setup_done;
  tdsulcmd_cphy_setup_local_cmd_type ul_cphy_setup_done;

  /* cell transition support commands */
  tdsdlchmgr_dl_cell_trans_drop_done_cmd_type  dl_cell_trans_drop_done;

  /* indicates that a mDSP command timed out */
  //mdsp_sync_timeout_cmd_type   mdsp_cmd_timeout;

  /* local commands for the sleep state machine */
  tdsdrxcmd_sleep_cmd_type           drx_sleep_cmd;

  tdsl1m_module_cleanup_done_cmd_type module_cleanup_done;
  
  tdsl1m_module_drop_done_cmd_type module_drop_done;

#ifdef FEATURE_L1_LOG_ON_DEMAND
  l1_log_on_demand_type log_on_demand_cmd;
#endif

#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED	

    /**add in new type***/
    tdsl1_dl_gen_logpkt_cmd_type     dl_gen_logpkt;
#endif
  /* Local command for HSDPA channel configuration done */
  tdshscfg_chan_cfg_done_cmd_type hs_chan_cfg_done;

  /* Local command for submitting accumulated log packet */
  tdshscfg_log_submit_cmd_type    tdshscfg_do_log_submit;

  /* Local command to cleanup DL after HS tear down */
  tdshscfg_module_cleanup_after_hs_teardown_cmd_type hs_module_cleanup_after_hs_teardown;


#ifdef FEATURE_TDSCDMA_HSUPA
  tdsl1m_eul_dl_comb_chan_cfg_cmd_type eul_dl_comb_chan_cfg;
  tdsl1m_eul_dl_frm_bdry_cmd_type      eul_dl_frm_bdry;
  tdsl1m_eul_dl_phychan_done_cmd_type  eul_dl_phychan_done;
  tdsl1m_eul_ul_phychan_done_cmd_type  eul_ul_phychan_done;
  tdsl1m_eul_cphy_setup_local_cmd_type eul_cphy_setup_done;
  tdsl1m_edl_logging_local_cmd_type  edl_logging_cmd;
  #if defined( FEATURE_TDSCDMA_L1_LOGGING_ENABLED )
  tdsl1m_eul_logging_local_cmd_type  eul_logging_cmd;
  #endif
  /* + defang 02/18/2011 */
  tdsl1_eul_e_rucch_tx_cmd_type      eul_e_rucch_cfg;
  /* - defang 02/18/2011 */
#endif


  /* TDS_DRX_MANAGER_CMD for commands from other subsystems, like SRCH.
  ** TDS_DRX_ENTITY_CMD for commands for a particular channel state machine. */
  tdsdrxmgr_cmd_type  drx_manager_cmd;
  tdsdrxentity_cmd_type   drx_entity_cmd;

#ifndef FEATURE_XO
  tdsl1trm_tcxo_restriction_local_cmd_type tcxo_restriction;
#endif


  /*change the w name to tds name.*/
  #if defined(FEATURE_TDSCDMA_TO_LTE) && defined(FEATURE_TDSCDMA_ABSOLUTE_PRIORITY)
  tdssrchlte_local_cmd_type tdssrchlte_srch_done_cmd;
  #endif
  
  tdsl1_dsds_trm_grant_callback_cmd_type    l1m_dsds_trm_grant_callback_cmd;

#ifdef FEATURE_TDSCDMA_TRM_NEW_API
  tdsl1_dsds_trm_secondary_grant_callback_cmd_type l1m_dsds_trm_secondary_grant_callback_cmd;
  tdsl1_dsds_trm_modify_reason_cmd_type	l1m_dsds_trm_modify_reason_cmd;
  tdsl1_dsds_trm_modify_band_cmd_type l1m_dsds_trm_modify_band_cmd;
  tdsl1_dsds_trm_unlock_cmd_type l1m_dsds_trm_unlock_cmd;
#endif

  tdsl1m_gsm_periodic_meas_cmd_type    gsm_meas;

#ifdef FEATURE_TDSCDMA_L1M_PROFILING_LOG_PACKET
  tdsl1m_log_packet_cmd_type  tdsl1m_log_packet_cmd;
#endif  

#ifdef FEATURE_TDSCDMA_MOTION_SENSOR_AID_MEAS_SUPPRESS
  tdssrchmeas_sensor_aid_cmd_type sensor_aid_cmd;
#endif

#ifdef FEATURE_TDSCDMA_BAND_AVOID
  tdsl1m_cxm_band_avoid_cmd_type tdsl1m_band_avoid_cmd;
#endif  

tdsl1m_pending_cleanup_proc_cmd_type pending_cleanup_proc_cmd;

  tdsirat_x2t_fw_cmd_type  tdsirat_x2t_fw_cmd;

#ifdef FEATURE_QSH_EVENT_METRIC
  tdsl1m_qsh_report_cmd_type  tdsl1m_qsh_report_cmd;
#endif

} tdsl1m_local_cmd_type   ;

#ifdef FEATURE_TDSCDMA_L1_PERFORMANCE_WATCHDOG
typedef enum
{
  TDSL1PERM_ACTION_INIT = 0,
  TDSL1PERM_ACTION_IN_PROGRESS,
  TDSL1PERM_ACTION_DONE,
  TDSL1PERM_ACTION_FAIL,
  TDSL1PERM_ACTION_TIMEOUT
} tdsl1perm_action_status_enum;

typedef enum
{
  TDSL1PERM_L1M_STATE_CLEANUP = 0,
  TDSL1PERM_UL_PHYCHAN_DROP,
  TDSL1PERM_DL_PHYCHAN_DROP,
  TDSL1PERM_DL_CCTRCH_SETUP,
  TDSL1PERM_UL_CCTRCH_SETUP,
  TDSL1PERM_DL_PHYCHAN_ADD,
  TDSL1PERM_UL_PHYCHAN_ADD,
  TDSL1PERM_DL_PHYCHAN_CFG,
  TDSL1PERM_UL_PHYCHAN_CFG,
  TDSL1PERM_UL_RESYNC,
  TDSL1PERM_TUNE_RF_FREQ,
  TDSL1PERM_HSDPA_PHYCHAN_CFG,
  TDSL1PERM_HSUPA_NON_SCHED_CFG,
  TDSL1PERM_HSUPA_EDL_PHYCHAN_CFG,
  TDSL1PERM_HSUPA_EUL_PHYCHAN_CFG,
  TDSL1PERM_CPHY_SETUP_REQ,
  TDSL1PERM_CPHY_SETUP_REQ_DROP_ALL,
#ifdef FEATURE_TDSCDMA_L1_PERFORMANCE_WATCHDOG_PHASE2
  TDSL1PERM_SFN_CFN_UPDATE_EVENT,
  TDSL1PERM_CHNL_SETUP_EVENT,
  TDSL1PERM_TX_HSUPA_EDCH_EVENT,
  TDSL1PERM_RX_GEN_SRCH_HB_EVENT,
  TDSL1PERM_RX_GEN_DL_MAINT_EVENT,
  TDSL1PERM_TX_GEN_GET_DATA_EVENT,
  TDSL1PERM_GSM_MEAS_SCHED_EVENT,
  TDSL1PERM_RX_READ_SHARED_MEMORY_EVENT,
  TDSL1PERM_RUNTIME_EVENT_1,
  TDSL1PERM_RUNTIME_EVENT_2,
  TDSL1PERM_RUNTIME_EVENT_3,
  TDSL1PERM_RX_DPA_DEC_MAINT_EVENT,     
  TDSL1PERM_RX_TIME_ADJUST_EVENT,
  TDSL1PERM_TFW_INT_PICH_DEC_DONE,
  TDSL1PERM_TFW_INT_FPACH_DEC_DONE,
  TDSL1PERM_TFW_INT_NONHS_DEC_DONE,
  TDSL1PERM_TFW_INT_HS_DEC_DONE,
  TDSL1PERM_TFW_INT_HS_SCCH_DEC_DONE,
  TDSL1PERM_TFW_INT_EAGCH_DEC_DONE,
  TDSL1PERM_TFW_INT_SCHLD_EHICH_DEC_DONE,
  TDSL1PERM_TFW_INT_NON_SCHLD_EHICH_DEC_DONE,
  TDSL1PERM_TFW_INT_UL_PWR_INFO_UPDATE,
  TDSL1PERM_TFW_INT_SW_CMD_ERROR_FLAG,
#endif
  TDSL1PERM_ACTION_NUM
} tdsl1perm_action_type_enum;

typedef struct
{
  uint32 univ_stmr;
  /*current rxtime counter sub frame number*/
  uint16 rx_sfn ;
  /*current rxtime counter chipx8 count*/
  uint16 rx_cx8 ;
} tdsl1perm_stmr_info_type ;

typedef struct
{
  tdsl1perm_action_status_enum     status;
  tdsl1perm_stmr_info_type  start;
  tdsl1perm_stmr_info_type  end;
  uint32 duration;
  tdsl1m_state_enum_type l1_state;
  int16 cctrch_type;
  int16 phy_channel_type;
  int16 hspa_ops;
}tdsl1perm_action_record_type;
#define TDSL1PERM_ACTION_DB_SIZE 16
typedef struct
{
  tdsl1perm_action_type_enum action;
  tdsl1perm_action_record_type history[TDSL1PERM_ACTION_DB_SIZE];
  uint8 index;
  uint32 deadline;
}tdsl1perm_action_db_type;
#endif 

#ifdef FEATURE_TDSCDMA_DSDA
typedef enum
{
  TDSL1M_NON_VOICE_OR_PS_CALL,
  TDSL1M_VOICE_CALL,
  TDSL1M_PS_CALL,
  TDSL1M_MRAB_CALL
}tdsl1m_call_type;

#ifdef FEATURE_TDSCDMA_BAND_AVOID
/* Band Avoidance */
#define TDSL1_CXM_RX_TX_PWR_RPT_PERIOD 20 /* L1 -> MCS Power Report Period, 20 ms */
#define TDSL1_CXM_BAND_AVOID_RX_TX_PWR_RPT_PERIOD 2000 /* L1 -> MCS Band Avoidance Filtered Power Report Period, 2 seconds */
#define TDSL1_CXM_BAND_AVOID_RX_PERCENTILE 10 /* Bottom 10 Percentile */
#define TDSL1_CXM_BAND_AVOID_TX_PERCENTILE 10 /* Top 10 Percentile */
#define TDSL1_CXM_BAND_AVOID_RX_INVALID_PWR_DBM10 -1280 /* During gap, we report -128 dBm, filter this out */
#define TDSL1_CXM_BAND_AVOID_PWR_MIN INT32_MIN
#define TDSL1_CXM_BAND_AVOID_PWR_MAX INT32_MAX
#define TDSL1_CXM_BAND_AVOID_RX_SAMPLE_CNT (TDSL1_CXM_BAND_AVOID_RX_TX_PWR_RPT_PERIOD/TDSL1_CXM_RX_TX_PWR_RPT_PERIOD)
#define TDSL1_CXM_BAND_AVOID_TX_SAMPLE_CNT (TDSL1_CXM_BAND_AVOID_RX_TX_PWR_RPT_PERIOD/TDSL1_CXM_RX_TX_PWR_RPT_PERIOD)
#define TDSL1_CXM_BAND_AVOID_RX_HIST       ((TDSL1_CXM_BAND_AVOID_RX_PERCENTILE*TDSL1_CXM_BAND_AVOID_RX_SAMPLE_CNT)/100)
#define TDSL1_CXM_BAND_AVOID_TX_HIST       ((TDSL1_CXM_BAND_AVOID_TX_PERCENTILE*TDSL1_CXM_BAND_AVOID_TX_SAMPLE_CNT)/100)

typedef struct
{
  /* Sample Count */
  uint8 sample_count;
  
  /* Rx power history buffer */
  int32 rx_pwr_hist[TDSL1_CXM_BAND_AVOID_RX_HIST];

  /* Tx power history buffer */
  int32 tx_pwr_hist[TDSL1_CXM_BAND_AVOID_TX_HIST];
  
}tdsl1m_cxm_band_avoid_pwr_info;

typedef struct
{   
  coex_ba_blist_data ba_blist;
}tdsl1m_cxm_band_avoid_db;
#endif

#endif
/*-------------------------------------------*/
/*   L1 Manager global structure             */
/*-------------------------------------------*/

typedef struct
{
  tdsl1m_state_enum_type  l1_prev_state;
  tdsl1m_state_enum_type  l1_state;
  tdsl1m_state_enum_type  l1_next_state;  
  
  /* Indicates if L1 has set the current command as pending (Activation Time) */
  boolean                  cmd_pending;
  
  /* Pending commands to be used during state transition */
  tdsl1_acq_cmd_type       pending_acq_cmd;

#ifdef FEATURE_TDSCDMA_ANTENNA_SWITCH_DIVERSITY
  /* ASDiv state according to L1 state */
  tdsl1m_asdiv_state_enum_type l1_asdiv_state;
#endif

  /* sub-state for TDSL1M_ACQ state */
  tdsl1m_acq_substate_enum_type   l1_acq_substate;

  tdsl1_idle_cmd_type      pending_idle_cmd;

  tdsl1_freq_scan_cmd_type pending_fs_cmd;
  
  /* Global variable used to hold the L1 current state as a mask. The
   * mask depends on the types of CCTrCh currently setup by L1.
   * Setup mask for current tranport channel type set */
  uint8 trch_setup_mask;
  /* Setup mask for next tranport channel type set */
  uint8 next_trch_setup_mask;

  /*-----------------------------------------------------------------------------*/
  /*                   L1 Module Cleanup Done Bitmask                            */
  /* This bitmask is used to determine when all of the L1 modules have completed */
  /* their cleanup procedures when making state transitions.                     */
  /*---------------------------------------------------------------------------- */
  uint16                   module_cleanup_done_mask;

  /*-----------------------------------------------------------------------------*/
  /*                   L1 Module Drop Done Bitmask                               */
  /* This bitmask is used to determine when all of the L1 modules have completed */
  /* their channel DROP procedures                                               */
  /*-----------------------------------------------------------------------------*/
  uint16                   module_drop_done_mask;
  
  /*-------------------------------------------*/
  /*   TDSL1_CPHY_SETUP_REQ related flags      */
  /*-------------------------------------------*/
  uint16                   cphy_primary_freq;
  uint16                   cphy_working_freq;
    
  /* buffer to hold the current L1 TDSL1_CPHY_SETUP_REQ cmd */
  tdsl1_setup_cmd_type     cphy_setup_cmd;
  
  /* global variable that holds the set of CPHY setup
   * operations still remaining.*/
  uint16                   cphy_setup_req_mask;

  uint16                   cphy_setup_req_mask_done;
  
  /* Indicates if a TDSL1_CPHY_SETUP_REQ command is in progress in L1 */
  boolean                  cphy_setup_in_progress;
 
  /* Indicates if we are in a state transition during physical channel setup */
  boolean                  state_transition_in_cphy_setup;
  
  tdsl1m_drop_all_cause_enum_type  drop_all_cause;
  
  /*--------------------------------------------------------------------------*/
  /* Static CPHY_SETUP_CNF packet used to keep setup status while handling    */
  /* a TDSL1_CPHY_SETUP_REQ                                                   */
  /*--------------------------------------------------------------------------*/
  tdsl1_setup_cnf_type     cphy_setup_status;

  /* If TDSL1_CPHY_SETUP_REQ has an activation time, store it in here. */
  uint8                    cphy_setup_cmd_seq_num;
  uint16                   cphy_setup_cmd_act_time;

  /* Saved request mask used for cleanup in the case of cphy setup timeout */
  uint16                   setup_req_mask_for_cleanup;
  
  
  int32 current_rf_freq; 
  int32 bho_tgt_cell_freq;
//#ifdef FEATURE_TDSCDMA_DSDA
  boolean bho_started;
  boolean bho_ended;
  boolean hho_resync_ended;
  boolean hho_resync_started;
//#endif
  /*--------------------------------------------------------------------
   * Identify the category of SETUP REQ (HO/Regular/recover) 
   *-------------------------------------------------------------------*/
  tdsl1m_dch_setup_req_enum_type dch_setup_req_type;
  
  boolean dch_setup_t2g_failure_recovery;
  /*--------------------------------------------------------------------
   * Next state is DCH, is working freq and aset got handled already?
   *-------------------------------------------------------------------*/
  boolean dch_setup_working_freq_and_aset_update_is_handled;

  
#ifdef FEATURE_TDSCDMA_L1_PERFORMANCE_WATCHDOG
  tdsl1perm_action_db_type performance_db[TDSL1PERM_ACTION_NUM];
 #endif
 
 #ifdef FEATURE_TDSCDMA_L1M_PROFILING_LOG_PACKET  
  /* the first [TDSL1M_LOG_SOURE_NUM]: task and IRQ use different buffs;
    The second [2]:  Ping-Pang buff for record and output at the same time*/
  tdsl1mlog_profiling_result_struct_type l1m_log_packet_db[TDSL1M_LOG_SOURE_NUM][TDSL1MLOG_MAX_TIMING_RECORD_NUM];
  /* control data about the log buffer */
  tdsl1m_profiling_control_data l1m_log_packet_cb;
 #endif
 
 /* Mask of physical channels that are being dropped */
  uint8                    phychan_drop_mask;

  /* The CCTrCh index returned by the CCTrCh setup command. This is used
   * to config or add a DL Physical Channel.*/
  uint8                    cctrch_index;
  
  uint16                   sfn_at_start_of_handover;
 
  /* buffer to hold the current L1 TDSL1_CPHY_CELL_TRANSITION_REQ cmd */
  tdsl1_cell_transition_cmd_type     cphy_cell_trans_cmd;

  
  /* Indicates if we are in state transition during cell transition */
  boolean                  state_transition_in_cell_trans;

  /* Indicates if a cell transition command is in progress in L1 */
  boolean                  cell_trans_in_progress;

  /* Indicates if we have to drop phychans during cell transition */
  boolean                  cell_trans_phychan_drop_pending;
  
  /* For saving sequence number to identify SUSPEND command on confirmation. */
  uint8                    suspend_cmd_seq_num;

  /* A variable used to save the cause for suspend to be used later in the code for some
   * search related clean up */
  tdsl1_suspend_resume_cause_enum_type         suspend_cause;

  /*A temporary variable to keep track whether the L1 is suspended for W2G BPLMN*/
  boolean                  suspended_w2g_bplmn;

  /*flag to indicate whether or not L1 is suspended due to HHO*/
  boolean                  suspended_due_to_hho;
  
  /*--------------------------------------------------------------------------
   * Restoring compressed mode gaps after a RESUME is handled a little differently
   * from setting them up normally because we keep some state info around from the
   * time of the SUSPEND.  Here's a flag to know if we're doing a RESUME or not. 
   *--------------------------------------------------------------------------*/
  boolean                  resuming_tdscdma_flag;/* would be cleared when L1 sends CPHY_SETUP_CNF */
  
  boolean                  resuming_tdscdma_flag_for_dch; /* would be cleared when L1 sends ESTABLISHED_IND */
  
  /* This Flag will be used to idenitfy the resume cause. This flag was introduced 
   * to selectively enable or disable the GSM clock from WL1. */
  tdsl1_suspend_resume_cause_enum_type         resume_cause;
 
 /*Index into sorted_tbar_list*/
  uint16                   tbar_index;

  /*Sorted list of tbarred timers to be run*/
  rex_timer_cnt_type       sorted_tbar_list[TDSL1M_MAX_BARRED_CELLS];
 
  /* Total expiry happened since we last accounted for in the barred cell list*/
  rex_timer_cnt_type       tbarred_elapsed_count;

  /*--------------------------------------------------------------------------
   * RRC can ask for different types of mDSP images to be loaded when it
   * activates WL1.  WL1 then asks mDSP Services for the proper image(s) when
   * starting up.  WL1 also uses this info when shutting down the mDSP. 
   *-------------------------------------------------------------------------*/
  tdsl1_mdsp_image_enum_type  mdsp_image;
  
  boolean fw_app_enabled;

  boolean tfw_intf_ready;
  
  boolean                  print_msgs_around_tasklock;

  /* during handling of error scenarios in start_agc repsonse from mdsp,
     this variable stores whether stmr needs to be restarted */
  boolean                  restart_stmr_on_start_agc_failure;


  /* flag indicating completion of call for forcing sleep clk freq estimation (FEE). */
  /* set to true when DCH is being torn down */
  boolean                  just_completed_call;

  /* flag to indicate that we're about to set up or drop PCCPCH S or N in idle mode
   Used in Idle mode/sleep abort processing. */
  boolean                  about_to_setup_or_drop_pccpch_in_idle;



  boolean                  during_dl_sync_flag; /* add for SRCH */
  
  boolean                  fs_finished_flag; /* SRCH finished FREQ SCAN indication */
  boolean                  ue_timing_lost_ind; /* when OOS/RLF/DL WEAK IND, set the flag and L1M go into IDLE after drop all */
  
  boolean                  dl_drop_status;
  
  #ifdef FEATURE_TDSCDMA_L1M_SELF_RESET_FW
  boolean                  fw_err_sw_reset_ind;
  uint32                   FW_err_ind_count;
  #endif
  
  /* xiaoyuli add for CR377869 */
  uint8                   phchan_drop_add_flag;  /* CPHY_SETUP or cell reselection(SUCCESS) have drop+add type */
  uint8                   dl_drop_parallel_prach_flag;/* CPHY_SETUP have UL & DL drop, need to parallel DL drop with PRACH preprocess  */
  uint8                   cphy_setup_cell_tran_pend_flag; 
  uint8                    tdsl1m_cleanup_immediate_flag;/* 0: init; 1: cleanup done immediatly */
  
  uint16                  cphy_drop_sfn;
  boolean                  drop_add_npccpch_in_fach_flag;/* 1: drop/add npccpch in cphy_setup in FACH state; or drop npccpch in cell_trans_req(fail) in FACH/DRX */
  uint8                    rrc_cmd_seq_id;    
  
  uint16                   cphy_setup_activation_time;
  uint16                   cphy_setup_act_time_valid_flag;
  boolean                  during_baton_handover_rxlm_buf_switch_flag;
  
  /* for TRM new interface with band info */
  boolean                  band_info_change_is_needed;
  
  #ifdef FEATURE_TDSCDMA_QTUNER
  boolean l1m_qtuner_start;
  uint32  l1_qtuner_cnt;
  #endif  
  boolean                  l1m_disable_meas_heartbeat;
  
  boolean                  dpa_drop_20ms_ahead_flag;
  boolean                  dpa_drop_finish_flag;
  boolean                  dpa_ho_process_pending;
  
  boolean                  bch_dl_weak_ind_set_flag; /* if DL sends DL_weak_ind in BCH, after RRC drop_all and then sends ACQ, L1M would do real ACQ in SYNC state */
  
  /*--------------------------------------------------------------------
   * Upper layer controller type RRC/PLT
   *-------------------------------------------------------------------*/
  tdsl1_conroller_enum_type upper_layer_controller;
  /*--------------------------------------------------------------------
   * Function pointer to get command buffer from upper layer RRC/PLT
   *-------------------------------------------------------------------*/
  tdsrrc_l1_cmd_type *(*get_controller_cmd_buf)( void );
  /*--------------------------------------------------------------------
   * Function pointer to send command to upper layer RRC/PLT
   *-------------------------------------------------------------------*/
  void (*send_msg_to_controller)(tdsrrc_l1_cmd_type *cmd_ptr);
  /*--------------------------------------------------------------------
   * Function pointer for malloc function provide by upper layer RRC/PLT
   *-------------------------------------------------------------------*/
  void *(*controller_malloc)(size_t size);
 
  /*-------------------------------------------*/
  /*   L1 Manager timers & related             */
  /*-------------------------------------------*/
  /* Timer to enable cell reselection evaluation after cell camp, set to 
     tdsl1m_ena_cell_resel_time_ms = 1000 */
  rex_timer_type           ena_cell_resel_timer;
 
  /* Out of service area timer */
  rex_timer_type           out_of_service_timer;

//#ifdef FEATURE_L1_RECOVER_FROM_STUCK_IN_DRX
  rex_timer_type           drx_delayed_timer;
//#endif

  /* Timer to countdown the 2 second DRX sleep abort timeout */
  rex_timer_type           sleep_abort_timer;

  /* Timer to countdown the L1 state cleanup */
  rex_timer_type           state_cleanup_timer;

  /* Timer to countdown the time required for phychan setup */
  rex_timer_type           cphy_setup_timer;

  /* Timer to countdown the time required for phychan drop all */
  rex_timer_type           cphy_drop_all_timer;

 #ifdef FEATURE_TDSCDMA_TCXO_RESOURCE_SUPPORT
  /* Timer used in FACH and DCH to notify VCTCXO SRM and TCXO Mgr 3.0 clients 
   * that the frequency error (i.e. rotator) is accurate and ready to be read.
   * This happens once a second. */
  rex_timer_type           tcxo_freq_err_timer;
#endif

  /* T-barred timer*/
  rex_timer_type           tbarred_timer;

  /* Timer used to update tfw log mask periodically */
  rex_timer_type           tfw_log_mask_timer;
  
  boolean                  exiting_ul_active;

  #ifdef FEATURE_TDSCDMA_TRM
  /* timer used to monitor theprimary radio granted or not */
  rex_timer_type           trm_pri_timer;
  #endif

  /* timer used to monitor the secondary radio granted or not */
  rex_timer_type           trm_band_change_timer;
  
  /* Used when waiting for confirmations from TDRM */
  rex_timer_type  tdrm_timeout_timer;
  //rex_timer_type  trm_band_change_timer;
  
  tdsl1_stop_cause_enum_type  stop_cause;

  boolean is_updating_mcs;
#ifdef FEATURE_TDSCDMA_DSDA
  tdsl1m_call_type curr_call_type;

  uint16 dch_ts_allocation;
  
  uint16 dl_dpch_ts_allocation;
  
  uint16 last_multi_sim_cfg_actionSfn;
  
  tdsl1m_dsda_config_enum_type curr_multi_sim_cfg_type;
  
  boolean bump_up_ul_tx_priority;
  
  uint16 subs_prio_cb_trigger_sfn;
  
  sp_subs_states_enum_t curr_sub_prio;
  
  sp_subs_states_enum_t next_sub_prio;
  #endif

  uint8 cphy_setup_in_suspend_flag;//this flag is only set to 1 when resume failed and rrc send cphy_setup_req(drop all)
  //record the fw msgr l1 is waiting for
  msgr_umid_type fw_msg_timer_set_for;
  uint8 oos_report_flag;//this flag is used to indicate whether l1 has reported oos before.1: l1 has report oos; 0: l1 hasn't reported
  uint8 stop_suspend_in_process;//this flag is set when l1m is doing stop_tdscdma_req or suspend_tdscdma_req
  uint32 stmr_isr_is_run_flag;//this flag is set when stmr event handler is running; define uint32 to get one cycle process in CPU
  uint32 fw_isr_is_run_flag;//this flag is set when fw isr is running; define uint32 to get one cycle process in CPU
  uint32 l1m_stop_proc_flag;//this flag is set when l1m is doing stop_init, it's used to other threads to judge whether stop proc is doing
#ifdef FEATURE_TDSCDMA_FATAL_RECOVERY
  boolean fw_is_err_state;
#endif

  boolean fs_acq_in_progress;
  boolean ext_cmd_in_process;
}tdsl1m_global_type;

#ifdef FEATURE_TDSCDMA_QTUNER
#define TDSL1M_QTUNER_PERIOD 40  /* 40*5=200ms */
#endif

typedef enum
{
  TDSL1M_TEST_CONTROL_UL_SS_FROZEN = 0,
	TDSL1M_TEST_CONTROL_UL_PWR_TFC_ELMN = 1,
  TDSL1M_TEST_CONTROL_FEATURE_INVALID
} tdsl1m_nv_test_control_feature;
#define TDSL1M_TEST_CONTROL_NV_FEATURE_ENABLE 1

extern tdsl1m_global_type  tdsl1m_global;
extern cxm_freqid_info_ind_s tdsl1_cxm_freq_info;

typedef enum {
  TDSL1_REQ_TDRM_LOCK=0, /* L1 task first step when DSDS */
  TDSL1_REQ_TDRM_BAND=1, /* L1 task second step when DSDS; L1 task first step when non-DSDS */
  TDSL1_REQ_TDRM_NONE=2  /* L1 task third step when DSDS;  L1 task second step when non-DSDS */
}tdsl1_req_tdrm_type;

#define TDSL1_DEFAULT_GRANT_CB (tdsl1_ds_grant_cb)

#ifdef FEATURE_TDSCDMA_CELL_SELECTION_INTER_SIB_SLEEP
extern boolean tdsl1m_support_sleep_in_bch_state ;
extern boolean tdsl1m_ok_to_handle_ext_cmd_in_bch_state;
#endif

/*-------------------------------------------*/
/*   Command Queues                          */
/*-------------------------------------------*/

/*
 * L1 Manager Protocol Command queue.
 * Protocol related commands are placed on this queue.
 */
extern q_type tdsl1m_ext_cmd_q;
#ifdef FEATURE_TDSCDMA_FREE_FLOATING_TASK
extern q_type tdsl1m_ff_cmd_q;
#endif

/*
 * L1 Manager Local Command queue.
 * Local Commands are placed on this queue.
 */
extern q_type tdsl1m_local_cmd_q;

/*
 * L1 Local Free Queue. The Protocol/Local State Processor
 * is responsible in filling up this queue with queue items.
 * The L1 Manager will use this free queue to get a command buffer
 * for a local command and passes the buffer to the client.
 */
extern q_type tdsl1m_local_free_q;


/*
 * The following structure of function pointers are used to define
 * a Protocol Processor. This structure encapsulates all the functions
 * necessary for operation on the Protocol Command.
 * For a particular protocol, implementation should be provided
 * for these functions, and an instance of this type should be
 * defined. This instance will be used by the L1 Manager whenever there
 * is a switch to the Protocol.
 */
typedef struct
{
  /*
   * Pointer to a function that initializes a Protocol Processor.
   * This is called from the L1M Main Loop when a Processor is
   * installed. This function takes a pointer to the Protocol
   * Command Queue, a pointer the Protocol Free Queue, pointer to
   * the Local Command Queue and a pointer to the Local Free Queue.
   * The Processor will initialize the free queues with the desired
   * number of free queue items whose types are known to the Processor
   * and save the pointers to the command queues and the free queues
   * for it's use.
   */
  void (*init)(void);

  /*
   * Pointer to a function that de-initializes a Processor.
   * This is called from the L1M Main Loop when the currently active
   * Processor is de-installed. This function will de-queue all the
   * command and free queues.
   */
  void (*cleanup)(tdsl1m_state_enum_type next_state);

  /*
   * This is the state machine of the Processor called by the L1M
   * processing loop when there is a command on the Protocol Command
   * Queue. This function takes a pointer to the Protocol Command and
   * then acts on this Protocol Command. It may call the cleanup procedure
   * of the Local Processor, and may initialize the Local Command Pointer
   * to point to a new Local Processor. It should initialize the new
   * Local Processor, which will fill up the local free queue. The function
   * is responsible in returning the command pointer to the free queue
   * once the command is completed.
   */
  void (*cmd_processor)
  (
    tdsl1_ext_cmd_type* cmd_item_ptr
  );

} tdsl1m_ext_processor_type;


/******************************************************************************
*******************************************************************************
                      EXTERNAL VARIABLE DECLARATIONS
*******************************************************************************
******************************************************************************/

#ifdef FEATURE_DUAL_SIM
extern sys_modem_as_id_e_type as_id;
#endif

/*--------------------------------------------------------------------------
                 EXTERN VARIABLE:  L1_EXT_STATE_PROCESSOR
--------------------------------------------------------------------------*/
extern tdsl1m_ext_processor_type* tdsl1m_ext_state_processor[TDSL1M_NUM_STATES + 1];

extern rex_timer_cnt_type tdsl1m_ena_cell_resel_time_ms;

extern rex_timer_type  tdsl1m_fw_rsp_timer;

#ifdef FEATURE_TDSCDMA_TCXO_RESOURCE_SUPPORT
/* Timer used in FACH and DCH to notify VCTCXO SRM and TCXO Mgr 3.0 clients 
** that the frequency error (i.e. rotator) is accurate and ready to be read.
** This happens once a second. */
#define TDSL1M_TCXO_FREQ_ERR_READY_RATE      100
/* Timeout for Timer used in FACH, BCH and DCH to push the frequency error if we didnt
** send any frequency error samples, because no samples have been collected yet. 
** Meant to be shorter than TDSL1M_TCXO_FREQ_ERR_READY_RATE. */
#define TDSL1M_TCXO_INITIAL_FREQ_ERR_READY   100
#endif

#ifdef FEATURE_TDSCDMA_DSDS
extern boolean tdsl1m_gsm_init_after_get_trm_lock;
#endif


/*===========================================================================
FUNCTION     WL1_TCXO_FREQ_ERR_READY

DESCRIPTION
  This function is called periodically in FACH and DCH to notify VCTCXO SRM
  (Shared Resource Manager) and TCXO Mgr 3.0 clients (like GPS) that the 
  frequency error (i.e. rotator) value is accurate and may be read.  
  The client(s) may then retrieve the frequency error from SRM.

  Only do notification if we have at least one finger in lock.  Otherwise
  assume our frequency error is not accurate because we have no fingers
  tracking the downlink.

DEPENDENCIES
  None.

PARAMETERS
  uint32 unused - Not used.  Included to make REX happy.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdsl1m_tcxo_freq_err_ready(uint32 unused);

/*===========================================================================
FUNCTION     L1M_QUERY_REL5_SUPPORTED

DESCRIPTION
  This function return boolean based on support of release 5 enabled or not

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdsl1m_query_rel5_supported(void);

/*===========================================================================

FUNCTION L1M_PROCESS_EXT_CMD

DESCRIPTION
  This function de-queues a command from the protocol command queue,
  invokes the command processor of the currently active protocol processor
  and passes the command to it.

  If there are no more protocol commands pending on the protocol command
  queue, the protocol command queue signal is cleared.

  This function is called in the context of the L1 thread.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The Protocol Command Queue signal is cleared if there are no
  protocol commands on the Protocol Command Queue.

===========================================================================*/
void tdsl1m_process_ext_cmd(tdsl1_req_tdrm_type req_for_lock);

/*===========================================================================

FUNCTION L1_FREE_CMD_BUF

DESCRIPTION
  This function is used by L1 to free the external command buffer once L1 is
  finished processing the command. This function is executed in the context
  of the caller.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tdsl1m_free_cmd_buf
(
  /* Command buffer to be freed */
  tdsl1_ext_cmd_type* cmd_ptr
);

/*===========================================================================

FUNCTION L1M_GET_LOCAL_CMD_BUF

DESCRIPTION
  This function is used by the internal clients of the L1 Manager to get a
  free command buffer.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the free buffer.

SIDE EFFECTS
  None.

===========================================================================*/
extern tdsl1m_local_cmd_type   * tdsl1m_get_local_cmd_buf( void );

/*===========================================================================

FUNCTION L1M_PUT_LOCAL_CMD

DESCRIPTION
  This function is used by the internal units of the L1 to submit local
  commands. This function puts the command onto the L1M Local Command
  Queue and sets the Local Command Queue for the L1 task.

  This function is called in the context of the L1M thread or in the
  context of an ISR within the L1.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The Local Command buffer is put on the Local Command Queue and the
  Local Command Queue signal is set.

===========================================================================*/
extern void tdsl1m_put_local_cmd
(
  tdsl1m_local_cmd_type   * cmd_ptr
);

/*===========================================================================

FUNCTION L1_FREE_LOCAL_CMD_BUF

DESCRIPTION
  This function frees the command buffer associated with the local command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tdsl1m_free_local_cmd_buf
(
  /* Command buffer to be freed */
  tdsl1m_local_cmd_type   * cmd_ptr
);

/*===========================================================================

FUNCTION     WL1_SEND_MSG_TO_CONTROLLER

DESCRIPTION
  This function sends a message to the software module that is currently
  controlling WL1.  During normal operation, this is RRC.  During Factory
  Test Mode, this is FTM.  The messages are generally confirmations to
  commands or indications, like measurement data available or out of sync.

DEPENDENCIES
  None.

PARAMETERS
  tdsrrc_l1_cmd_type *cmd_ptr - Pointer to command structure to send to either
                          RRC or FTM (Factory Test Mode).
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void tdsl1m_send_msg_to_controller(tdsrrc_l1_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION l1M_PROCESS_LOCAL_CMD

DESCRIPTION
  This function de-queues a command from the local command queue,
  invokes the command processor of the currently active local processor
  and passes the command to it.

  If there are no more local commands pending on the local command
  queue, the local command queue signal is cleared.

  This function is called in the context of the L1M thread.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The Local Command Queue signal is cleared if there are no
  local commands on the Local Command Queue.

===========================================================================*/
void tdsl1m_process_local_cmd( void );

/*===========================================================================

FUNCTION    L1M_DISCARD_LOCAL_CMDS

DESCRIPTION
  This function throws away all pending local commands, conditionally.  It
  may or may not discard the command depending upon the current tdsl1m_global.l1_state
  and what the command actually is.  Some commands we don't want to lose.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1m_discard_local_cmds( void );

/*===========================================================================

FUNCTION L1_CMD_CONFIRM

DESCRIPTION
  This function sends a confirmation cmd to the upper layer.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tdsl1m_cmd_confirm( tdsrrc_l1_cmd_type* cmd);

/*===========================================================================
FUNCTION        L1_CMD_ERR_IND

FILE NAME       l1m.c

DESCRIPTION
  This function sends a command error indication to the upper layer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1m_cmd_err_ind( tdsl1_error_enum_type error, uint8 err_type );

/*===========================================================================
FUNCTION        L1_CPHY_SETUP_CNF_PUT_STATUS

FILE NAME       l1m.c

DESCRIPTION     This function is used by the L1 modules to update the
                tdsl1m_global.cphy_setup_status buffer in the course of performing
                the steps of a TDSL1_CPHY_SETUP_REQ.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    The l1_cphy_status buffer is updated accordingly
===========================================================================*/
void tdsl1m_cphy_setup_cnf_put_status
(
  /* this is the type of status to be updated, UL/DL Phychan/TrCH */
  tdsl1_cphy_setup_cnf_stat_type_enum_type stat_type,

  /* this is the channel being referred to cast to a uint8.  The cast allows
     all the various channels to be referenced even though they are of
     differing enum types */
  uint8 ch,

  /* the operation that was performed on the given channel */
  tdsl1_setup_ops_enum_type op,

  /* TRUE if the operation passed */
  boolean pass
);

/*===========================================================================
FUNCTION  tdsl1m_move_fw_to_standby_state_blocking_wait

DESCRIPTION
  This function sends command to FW to put FW in the STANDBY state. This function
  will not return utill receiving the state change confirmation from FW.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1m_move_fw_to_standby_state_blocking_wait(void);


/*===========================================================================
FUNCTION  tdsl1m_blocking_wait_fw_rsp

DESCRIPTION
  This function sends command to FW to put FW in the STANDBY state. This function
  will not return utill receiving the state change confirmation from FW.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1m_blocking_wait_fw_rsp(msgr_umid_type msg_to_wait_for);


/*===========================================================================

FUNCTIONS L1M IDLE STATE FUNCTIONS

DESCRIPTION
  tdsl1m_idle_init
    This function does all initializations necessary for proper operation
    of the l1 in the IDLE state. This function is called when L1M transitions
    to the TDSL1M_IDLE state.
    All L1 modules perform Idle state initialization procedures.

  tdsl1m_idle_cleanup
    This function is called when L1M is transitioning to another state from
    the TDSL1M_IDLE state. This function calls the idle state cleanup functions
    of the L1 modules and executes all the local commands pending in the
    local queue.

  tdsl1m_idle_cmd_processor
    This function handles all external commands while in the TDSL1M_IDLE state.

===========================================================================*/
void tdsl1m_idle_init( void );
void tdsl1m_idle_cleanup( tdsl1m_state_enum_type next_state );
void tdsl1m_idle_cmd_processor( tdsl1_ext_cmd_type* cmd_ptr );

/*===========================================================================

FUNCTION L1M ACQ STATE FUNCTIONS

DESCRIPTION

  tdsl1m_acq_init
    This function does all initializations necessary for proper operation
    of the l1 in the ACQ state.
    All L1 modules perform ACQ state initialization procedures.

  tdsl1m_acq_cleanup
    This function is called when L1M is transitioning to another state from
    the TDSL1M_ACQ state. This function calls the ACQ state cleanup functions
    of the L1 modules and executes all the local commands pending in the
    local queue.

  tdsl1m_acq_cmd_processor
    This function handles all external commands while in the TDSL1M_ACQ state.

===========================================================================*/
void tdsl1m_acq_init( void );
void tdsl1m_acq_cleanup( tdsl1m_state_enum_type next_state );
void tdsl1m_acq_cmd_processor( tdsl1_ext_cmd_type* cmd_ptr );

/*===========================================================================

FUNCTION L1M SYNC STATE FUNCTIONS

DESCRIPTION

  tdsl1m_sync_init
    This function does all initializations necessary for proper operation
    of the l1 in the SYNC state.
    All L1 modules perform SYNC state initialization procedures.

  tdsl1m_sync_cleanup
    This function is called when L1M is transitioning to another state from
    the TDSL1M_SYNC state. This function calls the SYNC state cleanup functions
    of the L1 modules and executes all the local commands pending in the
    local queue.

  tdsl1m_sync_cmd_processor
    This function handles all external commands while in the TDSL1M_SYNC state.

===========================================================================*/
void tdsl1m_sync_init( void );
void tdsl1m_sync_cleanup( tdsl1m_state_enum_type next_state );
void tdsl1m_sync_cmd_processor( tdsl1_ext_cmd_type* cmd_ptr );

/*===========================================================================

FUNCTION L1M FREQ SCAN StATE FUNCTIONS

DESCRIPTION

  tdsl1m_fs_init
    This function does all initializations necessary for proper operation
    of the l1 in the FS state.
    All L1 modules perform FS state initialization procedures.

  tdsl1m_fs_cleanup
    This function is called when L1M is transitioning to another state from
    the TDSL1M_FS state. This function calls the FS state cleanup functions
    of the L1 modules and executes all the local commands pending in the
    local queue.

  tdsl1m_fs_cmd_processor
    This function handles all external commands while in the TDSL1M_FS state.

===========================================================================*/
void tdsl1m_fs_init( void );
void tdsl1m_fs_cleanup( tdsl1m_state_enum_type next_state );
void tdsl1m_fs_cmd_processor( tdsl1_ext_cmd_type* cmd_ptr );

/*===========================================================================

FUNCTIONS L1M BCH STATE FUNCTIONS

DESCRIPTION
  tdsl1m_bch_init
    This function does all initializations necessary for proper operation
    of the l1 in the BCH state.
    All L1 modules perform BCH state initialization procedures.

  tdsl1m_bch_cleanup
    This function is called when L1M is transitioning to another state from
    the TDSL1M_BCH state. This function calls the BCH state cleanup functions
    of the L1 modules and executes all the local commands pending in the
    local queue.

  tdsl1m_bch_cmd_processor
    This function handles all external commands while in the TDSL1M_BCH state.

===========================================================================*/
void tdsl1m_bch_init( void );
void tdsl1m_bch_cleanup( tdsl1m_state_enum_type next_state );
void tdsl1m_bch_cmd_processor(tdsl1_ext_cmd_type* cmd_ptr);

/*===========================================================================

FUNCTION L1M PCH STATE FUNCTIONS

DESCRIPTION
  tdsl1m_pch_init
    This function does all initializations necessary for proper operation
    of the l1 in the PCH state.
    All L1 modules perform PCH state initialization procedures.

  tdsl1m_pch_cleanup
    This function is called when L1M is transitioning to another state from
    the TDSL1M_PCH state. This function calls the PCH state cleanup functions
    of the L1 modules and executes all the local commands pending in the
    local queue.

  tdsl1m_pch_cmd_processor
    This function handles all external commands while in the TDSL1M_PCH state.

===========================================================================*/
void tdsl1m_pch_init( void );
void tdsl1m_pch_cleanup( tdsl1m_state_enum_type next_state );
void tdsl1m_pch_cmd_processor(tdsl1_ext_cmd_type* cmd_ptr);

/*===========================================================================

FUNCTIONS L1M FACH STATE FUNCTIONS

DESCRIPTION
  tdsl1m_fach_init
    This function does all initializations necessary for proper operation
    of the l1 in the FACH state.
    All L1 modules perform FACH state initialization procedures.

  tdsl1m_fach_cleanup
    This function is called when L1M is transitioning to another state from
    the TDSL1M_FACH state. This function calls the FACH state cleanup functions
    of the L1 modules and executes all the local commands pending in the
    local queue.

  tdsl1m_fach_cmd_processor
    This function handles all external commands while in the TDSL1M_FACH state.

===========================================================================*/
void tdsl1m_fach_init( void );
void tdsl1m_fach_cleanup( tdsl1m_state_enum_type next_state );
void tdsl1m_fach_cmd_processor(tdsl1_ext_cmd_type* cmd_ptr);

/*===========================================================================

FUNCTIONS L1M DCH STATE FUNCTIONS

DESCRIPTION
  tdsl1m_dch_init
    This function does all initializations necessary for proper operation
    of the l1 in the DCH state.
    All L1 modules perform DCH state initialization procedures.

  tdsl1m_dch_cleanup
    This function is called when L1M is transitioning to another state from
    the TDSL1M_DCH state. This function calls the DCH state cleanup functions
    of the L1 modules and executes all the local commands pending in the
    local queue.

  tdsl1m_dch_cmd_processor
    This function handles all external commands while in the TDSL1M_DCH state.

===========================================================================*/
void tdsl1m_dch_init( void );
void tdsl1m_dch_cleanup( tdsl1m_state_enum_type next_state );
void tdsl1m_dch_cmd_processor(tdsl1_ext_cmd_type* cmd_ptr);

/*===========================================================================

FUNCTIONS L1M DEACTIVE STATE FUNCTIONS

DESCRIPTION
  tdsl1m_deact_init

  tdsl1m_deact_cleanup

  tdsl1m_deact_cmd_processor

===========================================================================*/
void tdsl1m_deact_init( void );

/*===========================================================================

FUNCTIONS L1M PCH SLEEP STATE FUNCTIONS

DESCRIPTION
  tdsl1m_pch_sleep_init

  tdsl1m_pch_sleep_cleanup

  tdsl1m_pch_sleep_cmd_processor

===========================================================================*/
void tdsl1m_pch_sleep_init( void );
void tdsl1m_pch_sleep_cleanup( tdsl1m_state_enum_type next_state );
void tdsl1m_pch_sleep_cmd_processor(tdsl1_ext_cmd_type* cmd_ptr);

/*===========================================================================

FUNCTIONS L1M PCH BPLMN STATE FUNCTIONS

DESCRIPTION
  tdsl1m_pch_bplmn_init

  tdsl1m_pch_bplmn_cleanup

  tdsl1m_pch_bplmn_cmd_processor

===========================================================================*/
void tdsl1m_pch_bplmn_init(void);
void tdsl1m_pch_bplmn_cleanup( tdsl1m_state_enum_type next_state );
void tdsl1m_pch_bplmn_cmd_processor(tdsl1_ext_cmd_type* cmd_ptr);


/*===========================================================================

FUNCTIONS     L1M WAIT FOR TRM STOP FUNCTIONS

DESCRIPTION
  tdsl1m_wait_for_trm_stop_init
    Supports transition into TDSL1M_WAIT_FOR_TRM_STOP state, caused by an unlock
    request from TRM for the primary rf chain before the "Point of No Return" 
    is reached. WL1 also goes into the wait_for_stop state if it fails to get
    a lock on the the primary antenna.

  tdsl1m_wait_for_trm_stop_cleanup
    Supports transition out of TDSL1M_WAIT_FOR_TRM_STOP state to TDSL1M_STOPPED
    Actually does nothing, for there is nothing to do.

  tdsl1m_wait_for_trm_stop_cmd_processor
    This function handles all external commands while in the TDSL1M_WAIT_FOR_TRM_STOP 
    state. All commands except TDSL1_CPHY_DEACTIVATE_REQ  and TDSL1_CPHY_STOP_TDSCDMA_MODE_REQ
    with a special cause are ignored.

===========================================================================*/
void tdsl1m_wait_for_trm_stop_init( void );
void tdsl1m_wait_for_trm_stop_cleanup( tdsl1m_state_enum_type next_state );
void tdsl1m_wait_for_trm_stop_cmd_processor(tdsl1_ext_cmd_type* cmd_ptr);


/*===========================================================================

FUNCTIONS     L1M STOPPED STATE FUNCTIONS

DESCRIPTION
  tdsl1m_stopped_init
    Supports transition into TDSL1M_STOPPED state, caused by reception of
    TDSL1_CPHY_STOP_TDSCDMA_MODE_REQ primitive from RRC with TDSL1_RAT_CHANGE cause.

  tdsl1m_stopped_cleanup
    Supports transition out of TDSL1M_STOPPED state to TDSL1M_IDLE 
    Actually does nothing, for there is nothing to do.

  tdsl1m_stopped_cmd_processor
    This function handles all external commands while in the TDSL1M_STOPPED state.

===========================================================================*/
void tdsl1m_stopped_init( void );
void tdsl1m_stopped_cleanup( tdsl1m_state_enum_type next_state );
void tdsl1m_stopped_cmd_processor(tdsl1_ext_cmd_type* cmd_ptr);

/*===========================================================================
FUNCTIONS     L1M SUSPENDED STATE FUNCTIONS

DESCRIPTION
  tdsl1m_suspended_init
    Supports transition into TDSL1M_SUSPENDED state, caused by reception of
    TDSL1_CPHY_SUSPEND_TDSCDMA_MODE_REQ primitive from RRC.

  tdsl1m_suspended_cleanup
    Supports transition out of TDSL1M_SUSPENDED state.  There are 2 possibilities
    here.  We are leaving TDSL1M_SUSPENDED because we got a RESUME.  Or we are
    leaving because we're getting shut down.  In the shut down scenario, call
    the cleanup routine of the state that got SUSPENDED to do final cleaning
    up.  Currently the only state requiring this is TDSL1M_DCH.  In the RESUME case,
    don't do that because we're going to be heading back into that state and
    resuming where we left off.

  tdsl1m_suspended_cmd_processor
    This function handles all external commands while in the TDSL1M_SUSPENDED state.
    The only valid commands are TDSL1_CPHY_RESUME_TDSCDMA_MODE_REQ,
    TDSL1_CPHY_STOP_TDSCDMA_MODE_REQ, and TDSL1_CPHY_DEACTIVATE_REQ.
===========================================================================*/
void tdsl1m_suspended_init( void );
void tdsl1m_suspended_cleanup( tdsl1m_state_enum_type next_state );
void tdsl1m_suspended_cmd_processor(tdsl1_ext_cmd_type* cmd_ptr);


/*===========================================================================
FUNCTIONS     TDSL1M IRAT STATE FUNCTIONS

DESCRIPTION
  tdsl1m_irat_init
    Supports transition into TDSL1M_IRAT state .

  tdsl1m_irat_cleanup
    Supports transition out of TDSL1M_IRAT state.  

  tdsl1m_irat_cmd_processor
    This function handles all external commands while in the TDSL1M_IRAT state.
    No RRC command is valid in this state

===========================================================================*/
void tdsl1m_irat_init( void );
void tdsl1m_irat_cleanup( tdsl1m_state_enum_type next_state );
void tdsl1m_irat_cmd_processor(tdsl1_ext_cmd_type* cmd_ptr);

/*===========================================================================
FUNCTION        tdsl1m_send_start_agc_cmd_to_mdsp

DESCRIPTION
  This function calls mdsp_sync_send_start_agc_cmd() to send the
  MDSP_CMD_START_AGC command to mDSP firmware.  It will wait up to five
  seconds for a confirmation.  It then waits 2 msec to allow the AGC
  to settle.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdsl1m_send_start_agc_cmd_to_mdsp(void);

/*===========================================================================
FUNCTION        L1M_SEND_STOP_AGC_CMD_TO_MDSP

DESCRIPTION
  This function calls tdsmdsp_sync_send_agc_cmd() to stop the AGC.
  This is done by sending the MDSP_CMD_START_AGC command with the 
  appropriate parameters to stop the AGC on the primary antenna.
  It will wait up to one second for a confirmation.

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void tdsl1m_send_stop_agc_cmd_to_mdsp(void);


/*===========================================================================

FUNCTION L1M_CLEAR_SRCH_DONE_CMD

DESCRIPTION
  This function deletes (clears) ALL "TDS_SRCH_DONE_CMD" from the L1 local queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tdsl1m_clear_srch_done_cmd (void);

/*===========================================================================

FUNCTION L1M_POWER_DOWN

DESCRIPTION
  This function is called when L1 receives the TASK_STOP_SIG signal, so that
  the L1 task can prepare for the phone to be powered down.  There's nothing
  to do, so just dump a debug message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tdsl1m_power_down (void);

/*===========================================================================

FUNCTION L1M_OFFLINE

DESCRIPTION
  This function is called when L1 receives the TASK_OFFLINE_SIG signal.
  There's nothing to do, so just dump a debug message.  In offline mode,
  L1 shall only handle service programming/diagnostics/test commands.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tdsl1m_offline (void);

/*===========================================================================

FUNCTION L1_WAIT

DESCRIPTION
  This function is L1's interface to rex_wait(). It handles reporting to the
  watchdog task in case the watchdog signal arrives before the expected signal.
  
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tdsl1m_wait (rex_sigs_type mask);

/*===========================================================================

FUNCTION L1_REPORT_WATCHDOG

DESCRIPTION
  This function reports to the watchdog task and restarts the timer for the next report.
  
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tdsl1m_report_watchdog ( void );

/*===========================================================================

FUNCTION     L1M_WAKE_UP

DESCRIPTION
  This helper function is called from other functions to make L1 ready to process
  external commands (if any), and enable the RF TX chain.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void tdsl1m_wake_up( void );

/*===========================================================================

FUNCTION     L1M_PREPARE_FOR_SLEEP

DESCRIPTION
  This function is called to indicate to L1 that a sleep cycle will shortly begin.
  It sends the tdsslp_go_to_sleep command to the mDSP, shuts off the mDSP clock, prepares the
  Sequencer for sleep, and changes state to PCH_SLEEP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void tdsl1m_prepare_for_sleep( void );

/*===========================================================================

FUNCTION     L1M_PREPARE_FOR_WAKE_UP

DESCRIPTION
  This function is called to indicate to L1 that a sleep cycle has ended.
  It sends the MDSP_CMD_START_AGC command to the mDSP and busy waits until
  the AGC has settled.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void tdsl1m_prepare_for_wake_up( tfw_state_t fw_state );

/*===========================================================================

FUNCTION     L1M_WOKEN_UP

DESCRIPTION
  This function is called to indicate that L1 is ready for business after waking up 
  : reacquisition search has succeeded and DL channels are set up.
  It changes state back to TDSL1M_PCH (state before sleeping), 
  sends L1 task the TDSL1M_EXT_CMD_Q_SIG signal to start processing external commands 
  (which were on hold during sleep). 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void tdsl1m_woken_up( void );

/*===========================================================================

FUNCTION     L1M_IN_DRX

DESCRIPTION
  This function is called to indicate to L1M that L1 is in DRX or sleep mode.
  tdsl1m_global.l1_state is changed to TDSL1M_PCH_SLEEP

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void tdsl1m_in_drx( void );

/*===========================================================================

FUNCTION     L1M_OUT_OF_DRX

DESCRIPTION
  This function is called to confirm that L1M is back in PCH state (from PCH_SLEEP)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void tdsl1m_out_of_drx( void );

/*===========================================================================

FUNCTION     L1M_IN_BPLMN

DESCRIPTION
  This function is called to indicate to L1M that L1 is in BPLMN srch mode.
  tdsl1m_global.l1_state is changed to TDSL1M_PCH_BPLMN.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void tdsl1m_in_bplmn( void );

/*===========================================================================

FUNCTION      tdsl1m_resuming_tdscdma

DESCRIPTION
  This function returns whether or not we are in the process of resuming
  TDSCDMA after inter-RAT handover or cell reselection failure.

DEPENDENCIES
  Whether or not we've recently received TDSL1_CPHY_RESUME_TDSCDMA_MODE_REQ
  from RRC.

RETURN VALUE
  boolean:
    TRUE  - In the process of resuming WCDMA.
    FALSE - Otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdsl1m_resuming_tdscdma(void);

/*===========================================================================

FUNCTION      tdsl1m_resuming_tdscdma_in_DCH

DESCRIPTION
  This function returns whether or not we are in the process of resuming
  TDSCDMA after inter-RAT handover or cell reselection failure or LTA under DCH state.

DEPENDENCIES
  Whether or not we've recently received TDSL1_CPHY_RESUME_TDSCDMA_MODE_REQ
  from RRC.

RETURN VALUE
  boolean:
    TRUE  - In the process of resuming WCDMA.
    FALSE - Otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean tdsl1m_resuming_tdscdma_in_dch(void);


#ifdef FEATURE_L1_LOG_ON_DEMAND
/*===========================================================================

FUNCTION      tdsl1m_process_log_on_demand

DESCRIPTION
  This function routes log on demand requests to various handlers
  using the log code as the key

DEPENDENCIES
  None

RETURN VALUE
   tdsl1m_global.l1_state

SIDE EFFECTS

===========================================================================*/
extern void tdsl1m_process_log_on_demand(log_code_type log_code);
#endif
 
//#ifdef FEATURE_L1_RECOVER_FROM_STUCK_IN_DRX
/*===========================================================================

FUNCTION      tdsl1m_clear_drx_timer_sig

DESCRIPTION
  This function clears the signal associated with the rex timer to detect "stuck in  DRX."

DEPENDENCIES
  None

RETURN VALUE
   tdsl1m_global.l1_state

SIDE EFFECTS

===========================================================================*/
extern void tdsl1m_clear_drx_timer_sig(void);
//#endif

#ifdef FEATURE_TDSCDMA_INTERF_MEAS_RES_OPT
/*===========================================================================
FUNCTION     L1M_QUERY_INTERF_MEAS_RES_OPT_SUPPORTED

DESCRIPTION
  This function return boolean based on support of inter freq meas result optimization

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdsl1m_query_interf_meas_res_opt_supported(void);
#endif


/*===========================================================================
FUNCTION        TDSEULCFG_EUL_DCH_CLEANUP

FILE NAME       

DESCRIPTION     
  This is the DCH state cleanup routine for the HSUPA uplink controller.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
boolean tdseulcfg_dch_cleanup(tdsl1m_state_enum_type next_state) ;

/*===========================================================================
FUNCTION        TDSEULCFG_EUL_DCH_CLEANUP

FILE NAME       

DESCRIPTION     
  This is the DCH state cleanup routine for the HSUPA uplink controller.

DEPENDENCIES    

RETURN VALUE    

SIDE EFFECTS    
===========================================================================*/
boolean tdsedl_dch_cleanup(tdsl1m_state_enum_type next_state);

/*===========================================================================
FUNCTION tds_hsdpa_dch_cleanup

DESCRIPTION
  This function do DL states cleanup for HS channel particulary.

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None
===========================================================================*/
extern boolean tds_hsdpa_dch_cleanup(void);

/*===========================================================================
FUNCTION tdshscfg_states_init

DESCRIPTION
  This function do DL states init for HS channel particulary.

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdshscfg_states_init(void);

/*===========================================================================
FUNCTION     tdsl1m_is_ue_in_ftm_mode

DESCRIPTION
  Return Whether UE is in FTM mode.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tdsl1m_is_ue_in_ftm_mode(void);


/*===========================================================================

FUNCTION tdsl1m_get_controller_buf_default

DESCRIPTION

  Default function to get controller command buffer

DEPENDENCIES

  None.

RETURN VALUE

  A buffer of type tdsrrc_cmd_type. If memory is not available, NULL
  is returned.

SIDE EFFECTS

  None.

===========================================================================*/
tdsrrc_l1_cmd_type *tdsl1m_get_controller_buf_default( void );


/*===========================================================================

FUNCTION tdsl1m_send_msg_to_controller_default

DESCRIPTION

  Default function to send message to controller

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS
 

===========================================================================*/
void tdsl1m_send_msg_to_controller_default
(
  tdsrrc_l1_cmd_type *cmd_buf  /* The command buffer to be put in the queue */
);


/*===========================================================================

FUNCTION tdsl1m_controller_malloc_default

DESCRIPTION

  Default function for controller malloc

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS
 

===========================================================================*/
void* tdsl1m_controller_malloc_default
(
  size_t size
);

/*===========================================================================

FUNCTION tdsl1m_get_rrc_cmd_buf

DESCRIPTION
  This function gets the command buffer to send a command/confirmation
  to RRC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
tdsrrc_l1_cmd_type* tdsl1m_get_rrc_cmd_buf( void );

/*===========================================================================
FUNCTION   tdsl1m_g2t_irat_init_cnf

DESCRIPTION
  This function sends TDSCDMA_IRAT_INIT_CNF to GRAN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1m_g2t_irat_init_cnf(errno_enum_type err);

/*===========================================================================
FUNCTION     tdsl1m_handle_g2t_irat_init_cmd

DESCRIPTION:
  - handle TDSCDMA_IRAT_INIT_CMD
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================*/
void tdsl1m_handle_g2t_irat_init_cmd(void);

/*===========================================================================
FUNCTION     tdsl1m_handle_g2t_irat_stop_cmd

DESCRIPTION:
  - handle TDSCDMA_IRAT_STOP_CMD
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================*/
void tdsl1m_handle_g2t_irat_stop_cmd(void);

/*===========================================================================
FUNCTION     tdsl1m_handle_l2t_irat_init_cmd

DESCRIPTION:
  - handle TDSCDMA_IRAT_L2T_INIT_CMD
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================*/
void tdsl1m_handle_l2t_irat_init_cmd(void);

/*===========================================================================
FUNCTION     tdsl1m_handle_l2t_irat_stop_cmd

DESCRIPTION:
  - handle TDSCDMA_IRAT_L2T_STOP_CMD
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================*/
void tdsl1m_handle_l2t_irat_stop_cmd(void);

/*===========================================================================
FUNCTION     tdsl1m_handle_cxm_wwan_tech_state_update_req

DESCRIPTION:
  - handle MCS_CXM_WWAN_TECH_STATE_UPDATE_REQ
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================*/
void tdsl1m_handle_cxm_wwan_tech_state_update_req(void);


/*===========================================================================
FUNCTION     tdsl1m_handle_cxm_freqid_ind

DESCRIPTION:
  - handle MCS_CXM_FREQID_LIST_TDSCDMA_IND
  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================*/
void tdsl1m_handle_cxm_freqid_ind(tdsl1_msgr_cmd_type* cmd_ptr);


#ifdef FEATURE_TDSCDMA_L1_PERFORMANCE_WATCHDOG
/*===========================================================================
FUNCTION     tdsl1perm_action_start

DESCRIPTION
  Save start time of an action to TDS L1 performance watchdog database  

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1perm_action_start(tdsl1perm_action_type_enum action_type);

/*===========================================================================
FUNCTION     tdsl1perm_action_end

DESCRIPTION
  Save end time of an action to TDS L1 performance watchdog database  

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1perm_action_end(tdsl1perm_action_type_enum action_type);

/*===========================================================================
FUNCTION     tdsl1perm_action_timeout

DESCRIPTION
  Save timeout time of an action to TDS L1 performance watchdog database  

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1perm_action_timeout(tdsl1perm_action_type_enum action_type);

/*===========================================================================
FUNCTION     tdsl1perm_action_fail

DESCRIPTION
  Save failure time of an action to TDS L1 performance watchdog database  

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1perm_action_fail(tdsl1perm_action_type_enum action_type);
#endif /* FEATURE_TDSCDMA_L1_PERFORMANCE_WATCHDOG */

/*===========================================================================
FUNCTION     tdsl1_update_tfw_log_mask

DESCRIPTION
  This function Check AMSS log masks for TFW packets and send a log update message to
  TFW if they have changed.

DEPENDENCIES
  None.

PARAMETERS
 boolean enabled: TRUE - to enable the log mask periodic update
                  FALSE - to disable update

RETURN VALUE
  none

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsl1m_update_tfw_log_mask(boolean enabled);

/*===========================================================================
FUNCTION   tdsl1m_g2t_irat_stop_cnf

DESCRIPTION
  This function sends TDSCDMA_IRAT_STOP_CNF to GRAN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1m_g2t_irat_stop_cnf(void);

/*===========================================================================
FUNCTION   tdsl1m_l2t_irat_stop_cnf

DESCRIPTION
  This function sends TDSCDMA_IRAT_L2T_STOP_CNF to LTE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1m_l2t_irat_stop_cnf(void);

/*===========================================================================
FUNCTION   tdsl1m_x2t_irat_stop_cnf

DESCRIPTION
  This function sends X2T STOP_CNF  LTE or GRAN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1m_x2t_irat_stop_cnf(void);

/*===========================================================================

FUNCTION    tdsl1m_dyn_mem_ptr_init

DESCRIPTION
          when L1M task start, L1M initialize all dynamic ptrs to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1m_dyn_mem_ptr_init(void);
/*===========================================================================

FUNCTION    tdsl1_alloc_dynamic_mem

DESCRIPTION
          when receive TDSL1_CPHY_START_TDSCDMA_MODE_REQ, L1M start to malloc memory for L1 modules, and initialize them.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1_alloc_dynamic_mem(void);
/*===========================================================================

FUNCTION    tdsl1_free_dynamic_mem

DESCRIPTION
          when receive TDSL1_CPHY_STOP_TDSCDMA_MODE_REQ, L1M start to free memory for L1 modules.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1_free_dynamic_mem(void);

/*===========================================================================

FUNCTION    tdsl1m_get_test_control_nv

DESCRIPTION
          read L1M test control NV

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

boolean tdsl1m_get_test_control_nv(tdsl1m_nv_test_control_feature feature_bit_index);

#ifdef FEATURE_TDSCDMA_FREE_FLOATING_TASK
/*===========================================================================

FUNCTION TDSL1_PUT_FF_CMD

DESCRIPTION
  This function is used by the external clients of the L1 to submit
  Free floating commands to the L1. The caller owns the pointer to
  the command.  This function is called in the context of the caller. 
  The function puts the command onto the Free-floating Command Queue 
  and sets the RRC blocking command Queue signal of the L1 task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The Protocol Command Queue signal is set.

===========================================================================*/

void tdsl1_put_ff_cmd
(
  tdsl1_ext_cmd_type* cmd_ptr
);

/*===========================================================================

FUNCTION tdsl1m_clr_ff_sigs

DESCRIPTION


DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void tdsl1m_clr_ff_sigs(void);
#endif


/*=============================================================================================

FUNCTION  tds_l1_task_init

DESCRIPTION
  This task initializes layer 1 queues.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================================*/

void  tds_l1_task_init ( void );


/*===========================================================================

FUNCTION TDSL1M_PROCESS_MSGR_FW_CMD

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
extern void tdsl1m_process_msgr_fw_cmd(void);

#ifdef FEATURE_TDSCDMA_DSDA
/*===========================================================================
FUNCTION   tdsl1m_send_fw_multi_sim_config_ind

DESCRIPTION
  This function TDSCDMA_FW_MULTI_SIM_CONFIG_IND

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1m_send_fw_multi_sim_config_cmd(tdsl1m_dsda_config_enum_type dsda_slot_config);

void tdsl1m_rx_pwr_rpt_at_drx_mode(void);
/*===========================================================================
FUNCTION   tdsl1m_send_fw_multi_sim_config_cmd

DESCRIPTION
   send multi sim cmd to FW in DCH state
		  
DEPENDENCIES
  

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1m_dch_send_fw_multi_sim_config_cmd(void);

/*===========================================================================
FUNCTION   tdsl1m_set_fw_in_single_sim_mode

DESCRIPTION
  This function send TDSCDMA_FW_MULTI_SIM_CONFIG_IND to FW, configure it as Single SIM mode

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1m_set_fw_in_single_sim_mode(void);
/*===========================================================================
FUNCTION   tdsl1m_coex_mgr_g2t_cmd_handler

DESCRIPTION
  This function handles TDSCDMA_FW_MULTI_SIM_CONFIG_IND for G2T cmd
  ctr_freq: frequency measured
  
DEPENDENCIES
  only called when L1 is in L1M_IRAT state

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1m_coex_mgr_g2t_cmd_handler(uint16 ctr_freq);

/*===========================================================================
FUNCTION   tdsl1m_coex_mgr_cxmFreqId

DESCRIPTION
  This function returns the freq_ID for given freqh
  
DEPENDENCIES
	
RETURN VALUE
  CxM freq_ID

SIDE EFFECTS
  None
===========================================================================*/
uint32 tdsl1m_coex_mgr_cxmFreqId(uint16 meas_freq);
/*===========================================================================
FUNCTION   tdsl1m_coex_bump_ul_tx_cxm_priority

DESCRIPTION
  This is API function for UL to invoke, bump up UL TX CxM priority, or revert
  back
  
  uint16 action_sfn: activation time, in unit of sub frame
  boolean raise_priority: TRUE: bump up priority; FALSE: return back to normal priority

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1m_coex_bump_ul_tx_cxm_priority(uint16 action_sfn, boolean raise_priority);

#ifdef FEATURE_TDSCDMA_BAND_AVOID
/*===========================================================================
FUNCTION tdsl1_band_avoid_freq_ind

DESCRIPTION
  Send a Message to MCS through the Message Router
  
DEPENDENCIES
  None

RETURN VALUE
  None 

SIDE EFFECTS
  None
  
===========================================================================*/
void tdsl1m_coex_band_avoid_freq_ind(boolean stop_tds);

/*===========================================================================
FUNCTION tdsl1m_coex_band_avoid_blist_ind

DESCRIPTIONto
To receive theBA trigger decision with a freq blaacklist
  
DEPENDENCIES
  None

RETURN VALUE
  None 

SIDE EFFECTS
  None
  
===========================================================================*/
void tdsl1m_coex_band_avoid_blist_ind(cxm_coex_ba_ind_s* ba_ind_ptr);

/*===========================================================================
FUNCTION tdsl1m_band_avoid_blist_rsp

DESCRIPTION
  Send the average to mcs in the connected mode
  
DEPENDENCIES
  None

RETURN VALUE
  None 

SIDE EFFECTS
  None
  
===========================================================================*/
void tdsl1m_coex_band_avoid_blist_rsp(void);

/*===========================================================================
FUNCTION tdsl1m_cxm_band_avoid_clear_hist

DESCRIPTION
  - Clear global power history information for band avoidance rx/tx sampling
  
DEPENDENCIES
  None

RETURN VALUE
  None 

SIDE EFFECTS
  None
  
===========================================================================*/
void tdsl1m_cxm_band_avoid_clear_hist(void);

/*===========================================================================
FUNCTION tdsl1m_cxm_band_avoid_pwr_sample_process_cmd

DESCRIPTION
  - Process RX & TX power sample for T+G Band Avoidance in DSDA mode
  
DEPENDENCIES
  None

RETURN VALUE
  None 

SIDE EFFECTS
  None
  
===========================================================================*/
void tdsl1m_cxm_band_avoid_pwr_sample_process_cmd(tdsl1m_cxm_band_avoid_cmd_type *cmd_ptr);
#endif
#endif

/*===========================================================================
FUNCTION     tdsl1m_tdrm_band_change_req

DESCRIPTION
  This function is called to request band change.

DEPENDENCIES
  None.

PARAMETERS
  BAND information.

RETURN VALUE
  NONE.

SIDE EFFECTS
  None.
===========================================================================*/
tdrm_band_change_return_enum_type tdsl1m_tdrm_band_change_req(trm_frequency_type_t  tds_new_band);

/*===========================================================================
FUNCTION     tdsl1m_tdrm_band_grant_cb

DESCRIPTION
  This function is callback for request band change.

DEPENDENCIES
  None.

PARAMETERS
  BAND information.

RETURN VALUE
  NONE.

SIDE EFFECTS
  None.
===========================================================================*/
void tdsl1m_tdrm_band_grant_cb(trm_client_enum_t client, trm_band_type band, trm_band_grant_return_type granted, trm_request_tag_t tag);

/*===========================================================================
FUNCTION     tdsl1m_tdrm_request_and_wait_lock

DESCRIPTION
  This function requests TDRM LOCK and wait for TRM callback.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  boolean - FALSE if TRM request times out.

SIDE EFFECTS
  None.
===========================================================================*/
boolean tdsl1m_tdrm_request_and_wait_lock(tdsl1_ext_cmd_type*  ext_cmd_ptr, trm_request_tag_t tag);

/*===========================================================================
FUNCTION     TDSL1_DS_GRANT_CB

DESCRIPTION
  This function is called by TDRM with lock status for TDSL1.

DEPENDENCIES
  None.

PARAMETERS
  trm_client_enum_t: The client which needs the RF resource
  event: The RF resource which is being granted
  tag: Anonymous payload passed in tdrm_request_and_notify()

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void tdsl1_ds_grant_cb(trm_client_enum_t client,
  trm_chain_grant_return_type event, trm_request_tag_t tag);

/*===========================================================================

FUNCTION     tdsl1m_drop_done_cmp_proc

DESCRIPTION tdsl1m process after module drop done complete
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void tdsl1m_drop_done_state_trans(void);

/*===========================================================================

FUNCTION    tdsl1m_report_oos

DESCRIPTION
          API to report oos to rrc

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

#ifdef FEATURE_TDSCDMA_DSDS
void tdsl1m_report_oos(tdsl1_common_fail_enum_type oos_cause);
#else
void tdsl1m_report_oos(void);
#endif

#ifdef FEATURE_TDSCDMA_DR_DSDS
extern void tdsl1m_cxm_report_rx_power_at_wakeup(void);
#endif
/*===========================================================================

FUNCTION tdsl1m_set_wait_fw_rsp_timer

DESCRIPTION: this function is to set timer for the fw_rsp

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void tdsl1m_set_wait_fw_rsp_timer(msgr_umid_type msg_to_wait_for);

/*===========================================================================

FUNCTION tdsl1m_wait_global_done

DESCRIPTION
  This function is wait one global variable stable and done to invoid using mutex

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tdsl1m_busywait_global_done (uint32* wait_global, uint32 wait_val, uint32 wait_loop, uint32 wait_time);

#define FEATURE_MSCGEN

#ifndef FEATURE_TDSCDMA_HSUPA
   #define FEATURE_TDSCDMA_HSUPA
#endif

#ifdef FEATURE_TDSCDMA_L1M_PROFILING_LOG_PACKET
void tdsl1m_log_packet_record_init(void);
void tdsl1m_trigger_log_packet_output(uint32 current_source_bmp);
void tdsl1m_log_packet_output(uint8 output_source_bmp);
void tdsl1m_stmr_sync_log_packet(void);
void tdsl1m_log_packet_save_stmr(void);
void tdsl1m_log_packet_record_ext_cmd(tdsl1_ext_cmd_type* cmd_ptr);

void tdsl1m_report_cnf_profiling(tdsrrc_l1_cmd_type *cmd_ptr);

#endif


extern uint8 tdsl1m_get_current_call_type();

/*===========================================================================
FUNCTION tdsl1m_send_msg

DESCRIPTION
  Send a Message from L1M to other SW entity through the Message Router
  
DEPENDENCIES
  None

RETURN VALUE
  None 

SIDE EFFECTS
  None
  
===========================================================================*/
extern void tdsl1m_send_msg(msgr_umid_type umid, void *msg, uint32 msg_size);


/*===========================================================================

FUNCTION tdsl1m_set_ext_cmd_in_proc

DESCRIPTION
  set ext_cmd_in_process flag when receive below ext_cmd.
  stop/suspend/resume_tdscdma_req;cphy_setup; cell_transition

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tdsl1m_set_ext_cmd_in_proc(tdsl1_ext_cmd_type* cmd_ptr);

#ifdef FEATURE_QSH_EVENT_METRIC
/*===========================================================================
FUNCTION TDSL1M_PROCESS_QSH_METRICS_IND

DESCRIPTION
  This function processes the QSH report info indication. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1m_process_qsh_message_ind
( 
  tdsl1m_qsh_report_cmd_type*  cmd_ptr
);

/*===========================================================================
FUNCTION TDSL1M_UPDATE_METRIC_PTR

DESCRIPTION
  This function handles the metric configuration action. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1m_update_metric_ptr(qsh_client_metric_cfg_s* qsh_metric_cfg_p);

/*===========================================================================
FUNCTION TDSL1M_QSHWRITE_METRIC

DESCRIPTION
  This function handles the metric configuration action. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1m_qsh_write_metric(qsh_client_cb_params_s* cb_param_ptr);

/*===========================================================================
FUNCTION TDSL1M_UPDATE_METRIC_PTR_BY_ID

DESCRIPTION
  This function update the collected metric to qsh data buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsl1m_update_metric_ptr_by_id(qsh_metric_id_t metric_id, uint8* addr);

/*===========================================================================
FUNCTION TDSL1_QSH_TC_METRICS_INIT

DESCRIPTION
  This function does the initialization to the metric data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1_qsh_tc_metrics_init();

/*===========================================================================
FUNCTION TDSL1M_QSH_REGISTER

DESCRIPTION
  This function registers TL1 as QSH client to QSH framework.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsl1m_qsh_register(void);

#endif /*FEATURE_QSH_EVENT_METRIC*/

#endif /* TDSL1M_L1M_H */

