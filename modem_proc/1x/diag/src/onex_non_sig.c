/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
          1 X   N O N S I G N A L I N G   M O D U L E


GENERAL DESCRIPTION
      This file is an interface between the 1x protocol layer and factory
      test mode where 1x non-signaling tests are performed.

EXPORTED FUNCTIONS:

 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  Copyright (c) 2011 - 2016 Qualcomm Technologies, Inc.
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

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/diag/src/onex_non_sig.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/13/16   agh     Support QSH framework to collect periodic/event metrics
05/16/16   ppr     Chgs to honor UE mode change and 1X_to_LTE_RESEL Abort reqs
04/21/16   srk     1x2GTA cleanup and TX power shoot fixes.
01/23/15   agh     Enhance debugging interface between SRCH and CP
1/21/15    ssh     Added M1X_STMR initialization before placing a non sig call.
11/11/14   agh     1X overheads optimizations and direct TC->IDLE transition
10/08/14   srk     Print RSCH information.
10/04/14   pap     Resolving KW errors reported.
09/15/14   eye     Fixed Tx override disable issue.
08/06/14   agh     Debugging enhancement for MMOC sanity timer related cmds
07/28/14   gga     FR 20298: Changes in FTM and offline mode.
05/28/14   as      Jolokia compile clean-up
03/24/14   ppr     Fixed mc_cmd_q buffer memory leak issue in FTM Mode
03/03/14   ssh     1xSRLTE feature code.
02/03/14   dkb     Feature guard Triton UTX code under FEATURE_MODEM_1X_UTX
11/25/13   cjb     Switched logic for FEATURE_MODEM_1X_BOLT_OBSOLETE feature.
                   Mainlined FEATURE_MODEM_1X_TRITON.
10/16/13   dkb     Add changes for R-FCH functionality without FL dependency.
10/10/13   pap     Fixing alignment.
10/09/13   dkb     Add additional RL changes.
10/09/13   cjb     Fixed back-to-back ACQ, SYNC, PAGING from INIT state as well
10/09/13   vks     Fix setting up sch in ftm mode
10/08/13   cjb     Allow non-sig ACQ, SYNC, PAGING commands from NA state
09/30/13   cjb     Added RXC sleep/wakeup support for non-sig sleep
08/02/13   dkb     Changes to NS HHO to produce a RF OFF->ON transition
                   to force FW to refresh HW.
07/31/13   bb      Changes to revert back TCB_PTR MACRO changes to fix
                   NS issue
07/23/13   srk     Use TCB_PTR MACRO instead of rex_self() or tcb_ptr to make
                   the code compatible with RCINIT.
07/19/13   fg      Reorganize the nv read/write functions
05/31/13   srk     Mainlined FEATURE_MODEM_1X_NON_SIG.
05/06/13   cjb     Resolve compiler warnings
03/05/13   srk     Replace existing watchdog implementation with the new
                   Dog Heartbeat Mechanism.
02/01/13   bph     KW fixes
01/28/13   gga     Klocwork Reviews
01/25/13   vks     Cleanup srch mdsp module
10/01/12   eye     Freeing queue item when FEATURE_1X_CP_MEM_OPT enabled.
08/14/12   cjb     Added DIAG interface for SLPC verification in non-sig mode.
06/14/12   srk     Feature Cleanup.
06/06/12   srk     Fix for hang in FTM non-signalling mode due to Acq failure
05/30/12   eye     nonsig_txagc_set_power() returns NS1X_TEST_SUCCESS on successul execution
02/23/12   srk     Mainlined FEATURE_IS2000_SCH and FEATURE_IS2000_P2
01/31/12   srk     Feature Clean Up.
11/22/11   srk     Feature clean up: Mainlined and Internalized Features
10/31/11   adw     Allow SRCH to settle in paging before pushing to access.
10/28/11   adw     Push SRCH through ACCESS when lighting up non-sig traffic.
10/28/11   jtm     Mainline FTM_HAS_SRCH_HHO.
10/27/11   jtm     Enable NS1X_CMD_SET_REV_LNK_PWR.
08/18/11   vks     Remove the stray hdr call.
08/18/11   adw     Mainline 1x message router support.
07/12/11   adw     Fix infinite loop in FTM due to uncleared MC cmd signal.
05/26/11   adw     Improved release functionality.
05/25/11   adw     Misc fixes to rev fch flow and params.
04/28/11   jtm     Fixed compiler warnings.
04/25/11   jtm     Added include for srch_mdsp.h and srch_mdsp_i.h
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
04/20/11   adw     Disable timers and send immediate cmd ack on rumi.
03/23/11   adw     Added missing featurization.
03/23/11   adw     Fixes in onex_nonsig to FTM plumbing.
02/16/11   vks     Mainlined FEATURE_SRCH_INACTIVE_STATE.
02/09/11   vks     Featurize rf1x_mdsp* call temporarily.
02/08/11   jtm     Featurized use of macros and functions for compatibility with RF.
02/04/11   jtm     Refactored and moved command queue processing from FTM to 1X.
01/31/11   jtm     Convert ftm_mode_type to internal 1x non-sig type.
01/28/11   jtm     Exposed more functions to facilitate additional FTM decoupling.
                   Used FEATURE_MODEM_1X_NON_SIG to allow this module to be
                   mostly compiled out if not needed/supported.
01/21/11   jtm     Initial Revision.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "1x_variation.h"
#include "comdef.h"
#include "customer.h"
#include "modem_1x_defs.h"
#include "modem_1x_utils.h"
#include "target.h"

/* CP */
#include "mccdma.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mc_i.h"
#include "mc_v.h"
#include "mci.h"
#include "mcc_v.h"
#include "mcc_i.h"
#include "mccsrch_i.h"
#include "onex_non_sig.h"
#include "mccrx.h"
#include "mcctci_i.h"
#include "mar_i.h"
#include "parm_i.h"
#include "mcctcho_i.h"

/* DRIVERS */
#include "msm_drv_i.h"
#include "m1x_stmr_i.h"

/* MUX */
#include "rxccommon.h"
#include "txccommon_i.h"

/* SRCH */
#include "srch_mdsp_i.h"
#include "srch_lib_ext.h"

/* Other */
#include "nv.h"
#include "nv_items.h"
#include "sd.h"
#include "queue.h"
#include "ran.h"
#include "DalDevice.h"
#include "DDISoftwareSwitches.h"
#include "queue.h"
#include "diagi.h"
#include "rex.h"
#include "onex_nv_rd_wt.h"

/* Feature Dependent */
#include "ftm.h"
#include "ftm_task.h"
#ifdef FEATURE_MODEM_1X_SUPPORTS_RF
#include "test_mc.h"
#include "rfm_1x.h"
#include "rfm_device_types.h"
#endif /* FEATURE_MODEM_1X_SUPPORTS_RF */

#include "mmoc_v.h"

#ifdef FEATURE_1XCP_QSH_SUPPORT
#include "mccqsh_i.h"
#endif /* FEATURE_1XCP_QSH_SUPPORT */

/*===========================================================================

                    EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

extern void mccsa_set_access_in_progress(void);

/*===========================================================================

                   EXTERNAL VARIABLES

===========================================================================*/

/* Global variable defined in mcctc */
extern mcc_exit_name_type tc_exit;

/*===========================================================================

                   LOCAL VARIABLES

===========================================================================*/

/*--------------------------------------------------------------------------
                          Type and Variables
--------------------------------------------------------------------------*/
#define ONEX_NONSIG_CMD_BUFS_NUM 6
onex_nonsig_cmd_type      onex_ns_cmd_bufs[ONEX_NONSIG_CMD_BUFS_NUM];
q_type                    onex_ns_cmd_free_q;
q_type                    onex_ns_cmd_q;

typedef struct
{
  onex_nonsig_mode_type mobile_mode;            /* Current mode the mobile is operating in (AMSS, DMSS, FTM, etc.) */
  dword nv_esn;                                 /* ESN of the mobile read from NV */
  byte  sframe_num;                             /* sframe number that Sync message was received in */
  boolean wait_for_pc_msg;                      /* should 1x wait for a valid page during idle? */
  boolean cdma_enter_init_done;                 /* has 1x already performed cdma init? */
  boolean bypass_lc;                            /* Is bypass Long Code enabled? */
  db_acq_mode_type acq_mode;                    /* Current acquisition mode. */
  srch_rx_band_type band_class;                 /* Current band class. */
  onex_ns_state_enum_type ns_fwd_curr_state;    /* Current FTM state on the forward link */
  onex_ns_state_enum_type ns_rev_curr_state;    /* Current FTM state on the reverse link */
  mcc_entry_type cdma_entry;                    /* CDMA entry parameters */

} onex_nonsig_params_type;

static onex_nonsig_params_type onex_nonsig_params;

typedef enum
{
  TCI_1,  /* State in which the Search task is commanded to go to the
             Traffic Channel */
  TCI_2,  /* State in which frame quality metrics are checked waiting
             for a valid frame */
  TCI_3,  /* State in which the Receive task is commanded to go to the
             Traffic Channel */
  TCI_4,  /* State in which the Transmit task is commanded to go to
             the Traffic Channel and transmit preambles */
  TCI_5   /* State in which preambles are transmitted waiting for
             a valid message from the base station */
}nonsig_tci_substates;

/****************************************************************
            DEFAULT 1X FTM DATA STRUCTURES
*****************************************************************/

/* User defineable access probe parameters. Defaults set here. */
static onex_nonsig_cmd_access_probe_type onex_access_default_params =
{
  0,    /* Access Channel to use: TRUE = R-EACH, FALSE = R-ACH */
  0,    /* Access Channel preamble length */
  0,    /* Max Access Channel msg capsule size */
  1,    /* Current Paging Channel */
  200,  /* Base station identification */
  12,   /* Pilot PN sequence offset index */
  0,    /* initial power */
  0,    /* nominal power */
  1,    /* probe power step in dB */
  1,    /* number of steps */
  1     /* number of probe sequences */
};

/* User defineable paging channel parameters. Defaults set here */
static onex_nonsig_cmd_1x_paging_param_type onex_paging_default_params =
{
  (byte) CAI_PCH,                /* paging channel type */
  FALSE,                         /* wait for PC message */
  1,                             /* walsh code for PCH/FCCCH/BCCH */
  (byte) CAI_PC_1_RATE,          /* PCH bit rate - 9600 or 4800 */
  0xa0200000,                    /* long code mask for PCH */
  0,                             /* f-ccch channel id */
  (byte) CAI_FCCCH_9600_RATE,    /* f-ccch data rate */
  (byte) CAI_BCCH_9600_RATE,     /* f-bcch data rate */
  (byte) CAI_CODE_RATE_QTR,      /* code rates for either f-bcch or f-ccch */
};

#ifdef FEATURE_IS2000_1X_ADV
static onex_nonsig_cmd_radio_config_param_type onex_rcpm_default_params =
{
  0x1998,                         /* Fwd FCH ACK Mask for RL Blanking */
  0x0AAA,                         /* Fwd FCH ACK Mask for No RL Blanking */
  0x0018,                         /* Fwd SCH ACK Mask for RL Blanking */
  0x0028,                         /* Fwd SCH ACK Mask for No RL Blanking */
  CAI_FCH_BLANKING_DISABLED,      /* Duty cycle value for Rev Link */
  0,                              /* Chn adj gain for Rev Ack Chn (No Cells =1) */
  0,                              /* Chn adj gain for Rev Ack Chn (No Cells >1)*/
  CAI_PWR_CNTL_200_OR_400_HZ,     /* Power Control Mode */
  CAI_PWR_CNTL_STEP_1_DB,         /* Power control step size during RL Blanking */
  0x0AAA,                         /* Rev FCH ACK Mask */
  0x0028,                         /* Rev SCH ACK Mask */
  CAI_N2M_4_BAD_FRAMES,           /* Fwd N2M Indicator */
  CAI_FCH_BLANKING_DISABLED,      /* Duty cycle value for Fwd Link */
  0,                              /* Number of RC Parameters records */
};
#endif /* FEATURE_IS2000_1X_ADV */

/* Generic handle to the Switches Module Device.
------------------------------------------------------------------------*/
DalDeviceHandle * switches_handle_1x_ftm;
/* This flag ensures that Dal apis are only called when the Switches Module has
** attached to dal correctly
*/
static DALResult switches_dal_attach_1x_ftm;

/* Default values for ACQ CMD */
static onex_nonsig_cmd_type onex_nonsig_acq_cmd =
{
  .pilot_acq.param.band_class = CAI_BAND_CLASS0,
  .pilot_acq.param.cdma_freq  = 387,
  .pilot_acq.param.acq_mode   = (onex_nonsig_acq_mode_type)DB_ACQ_FULL
};

/* Timer to wake nonsig task */
rex_timer_type  onex_nonsig_timer;

/*===========================================================================

                    INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION onex_test_mcc

DESCRIPTION
  This is the main onex_nonsig subtask which runs under MC task context.
  It processes non signalling commands and invokes the proper handlers.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void onex_test_mcc( void )
{
  boolean      exit_ftm = FALSE;

  rex_sigs_type   sigs;       /* Signals returned by rex_get_sigs call */
  rex_sigs_type   mask;       /* Signals mask to wait on. */

  /*------------------------------------------------------------------*/

  onex_nonsig_initialize();

  /* Signal FTM task, indicating MC is FTM ready */
  ftm_sig_rex_set( FTM_MC_READY_SIG );

  /* Handle FTM commands and report command status to DIAG */

  while ( !exit_ftm )
  {
    /* Check if there is a new command */

    /* Check for 1xNS commands */
    if ( onex_nonsig_get_onex_ns_cmd_q_cnt() > 0 )
    {
      /* Process pending commands. */
      onex_nonsig_check_cmd_queue();
    }
    /* Check for MC commands on old MC command queue */
    else if ( onex_nonsig_get_mc_cmd_q_cnt() > 0 )
    {
      /* Process pending commands. */
      exit_ftm = onex_nonsig_process_mc_cmd();

      if ( exit_ftm )
      {
        //REVISIT: ftm_rfmode_exit();
      }
    }
    else /* No new command */
    {
      /* Clear any pending MC cmd sig */
      (void) rex_clr_sigs( rex_self(), ONEX_NONSIG_MC_CMD_Q_SIG );

      /* Loop while waiting for signals specified in mask and   */
      /* for watchdog signal.  If watchdog signal then kick it  */
      /* and loop again until some other signal in mask is set. */
      mask = ONEX_NONSIG_MC_CMD_Q_SIG;

      while (((sigs = rex_get_sigs(rex_self())) & mask) == 0)
      {
        mc_dog_report();
        (void) rex_wait( mask | MC_DOG_RPT_SIG );
      }
    }
  } /* End of while ( !exit_ftm ) */

  /* This point is unreachable, as there is not exit from FTM */

} /* onex_test_mcc */


/*===========================================================================

FUNCTION onex_nonsig_mc_wait

DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/
static rex_sigs_type nonsig_mc_wait
(
  rex_sigs_type   mask
)
{
  /*------------------------------------------------------------------*/

  M1X_MSG_SSID( DIAG, LEGACY_HIGH,
    "1xNonSig: Waiting for signals 0x%x",
    mask);

  return(mcc_wait(mask));

} /* onex_nonsig_mc_wait */


/*===========================================================================

FUNCTION nonsig_wait_for_valid_page_report

DESCRIPTION
  This function monitors rxtx for a valid PCH message report.

DEPENDENCIES
  None.

RETURN VALUE
  Page was successfully decoded? TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_wait_for_valid_page_report
(
  onex_nonsig_cmd_1x_paging_param_type *paging_params
)
{
  onex_nonsig_cmd_status_type pch_msg_status = NS1X_TEST_NO_SUCCESS;
  mccrxtx_cmd_type *msg_ptr;   /* Pointer to msg received on the mcc_rxtx_q */
  db_items_value_type db_value;/* Buffer to send data to DB */

  /*------------------------------------------------------------------*/

  M1X_MSG_SSID( DIAG, LEGACY_HIGH,
    "1xNonSig: Wait for RXC message in Idle state." );

  /* Wait for RX to decode a paging channel message.
     If a good message is not decoded before t30m expires
     then abort so we do not remain in this state forever.

     T30 timer = 3000ms, which is used in online mode.

     If the timer expires then by default there shouldn't
     be a message pending or its not a MCCRXTX_MSG_F
     message, so the test will fail gracefully.
  */
  #ifndef FEATURE_MODEM_1X_RUMI
  (void) rex_set_timer( &mcc_cmd_timer, cai_tmo.t30m);
  #endif /* !FEATURE_MODEM_1X_RUMI */

  (void) nonsig_mc_wait( MCC_RXTX_Q_SIG | MCC_CMD_TIMER_SIG );
  (void) rex_clr_timer (&mcc_cmd_timer);
  (void) rex_clr_sigs( rex_self(),  MCC_RXTX_Q_SIG | MCC_CMD_TIMER_SIG );

  if ((msg_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_q) ) != NULL)
  {
    /* Received a message --> process it */

    if ((msg_ptr->hdr.command == MCCRXTX_MSG_F) &&
        (msg_ptr->msg.chn == (caix_chan_type) paging_params->channel_type ))
    {
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Page received in Idle." );
      pch_msg_status = NS1X_TEST_SUCCESS;
    }

    /* Unexpected message */

    else
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: Unexpected RXC message:cmd=%d,chn=%d",
        msg_ptr->hdr.command,
        msg_ptr->msg.chn );

      pch_msg_status = NS1X_TEST_NO_SUCCESS;
    }

    /* Place message from layer 2 back on queue if requested */
    cmd_done( &msg_ptr->hdr.cmd_hdr );

    #ifdef FEATURE_1X_CP_MEM_OPT
    mc_free_queue_buf( msg_ptr, MC_MCC_RXTX_Q );
    #endif /* FEATURE_1X_CP_MEM_OPT */

    /* Update code channel */
    db_value.code_channel = paging_params->walsh_code;
    db_put( DB_CODE_CHANNEL, &db_value );

  } /* end if ((msg_ptr = q_get( &mcc_rxtx_q) ) != NULL) */
  else
  {
    M1X_MSG_SSID( DIAG, LEGACY_ERROR,
      "1xNonSig: Cannot get RXC message" );
    pch_msg_status = NS1X_TEST_NO_SUCCESS;
  }

  return pch_msg_status;

} /* nonsig_wait_for_valid_page_report */


/*===========================================================================

FUNCTION nonsig_nv_read_item

DESCRIPTION
  Read the specified item from NV.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  While this function is running, all other task activities are
  suspended except for watchdog kicking, and until the NV item is
  read.

===========================================================================*/
static void nonsig_nv_read_item
(
  nv_items_enum_type item_code
)
{
  /*------------------------------------------------------------------*/

  nv_cmd_type nv_cmd_data;
  nv_item_type nv_item = {{0}};

  nv_cmd_data.status = NV_FAIL_S;

  if (!onex_nv_read_write(item_code, &nv_item, NV_RD | NV_NONSIG,
                          &nonsig_mc_wait, &nv_cmd_data))
  {
    nv_item.esn.esn = 0xdeadf00d;  /* Set ESN=0xdeadf00d if reading NV fails */
    M1X_MSG_SSID( DIAG, LEGACY_FATAL,
      "1xNonSig: Bad NV read status %d, defaulting to 0x%x",
      nv_cmd_data.status,
      nv_item.esn.esn );
  }

  onex_nonsig_params.nv_esn = nv_item.esn.esn;

} /* onex_nonsig_nv_read_item */


/*===========================================================================

FUNCTION nonsig_init_cdma

DESCRIPTION
  This function initializes mobile for CDMA mode.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
static void nonsig_init_cdma( void )
{
  /*------------------------------------------------------------------*/

  /* First of all initialize the BS Overhead Info */
  mcc_init_ovhd_info();

  /* Send request to Transmit task to exit */
  mcc_tx_buf.cdma.hdr.command = TXC_EXIT_F;
  mcc_txc_cmd( &mcc_tx_buf );

  /* Send request to Receive task to go to CDMA mode */
  mcc_rx_buf.cdma.hdr.command = RXC_CDMA_F;
  mcc_rxc_cmd( &mcc_rx_buf );

  /* Send request to Searcher task to go to CDMA mode */
  mcc_srch_buf.cdma.hdr.cmd = SRCH_CDMA_F;
  mcc_srch_cmd( &mcc_srch_buf );

  onex_nonsig_params.ns_fwd_curr_state =
    onex_nonsig_params.ns_rev_curr_state = NS1X_INIT_CDMA;

} /* nonsig_init_cdma */


/*===========================================================================

FUNCTION nonsig_cdma_enter_init

DESCRIPTION
  This procedure initializes variables used in the entrance state for
  the CDMA state machine.  This is a special version just used for FTM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Certain variables used by the cdma_enter state will be initialized.

===========================================================================*/
static void nonsig_cdma_enter_init( void )
{
  /*------------------------------------------------------------------*/

  if (onex_nonsig_params.cdma_enter_init_done)
  {
    M1X_MSG_SSID( DIAG, LEGACY_HIGH,
      "1xNonSig: Skipping CDMA Entry Init, already done." );
  }
  else
  {
    M1X_MSG_SSID( DIAG, LEGACY_HIGH,
      "1xNonSig: CDMA Non Signaling Entry Init" );

    /* Read ESN from NV */
    nonsig_nv_read_item (NV_ESN_I);

    onex_nonsig_params.cdma_entry.entry = MCC_ACQUISITION;
    onex_nonsig_params.cdma_entry.gen.new_mode_or_blksys = TRUE;
    onex_nonsig_params.cdma_entry.gen.block_or_sys = (sd_blksys_e_type) 0;
    onex_nonsig_params.cdma_entry.gen.band_class = CAI_BAND_CLASS0;
    onex_nonsig_params.cdma_entry.gen.cdma_channel = 758;

    cdma.cdmach       = onex_nonsig_params.cdma_entry.gen.cdma_channel;
    cdma.band_class   = onex_nonsig_params.cdma_entry.gen.band_class;
    cdma.block_or_sys = onex_nonsig_params.cdma_entry.gen.block_or_sys;

    /* store entry parameters for later use */
    cdma.entry_ptr = &onex_nonsig_params.cdma_entry;

    /* Initialize rf for cdma */
    cdma_rf_init( cdma.cdmach, cdma.band_class );

    M1X_MSG_SSID( DIAG, LEGACY_HIGH,
      "1xNonSig: 1x NonSig: BAND_CLASS %d CHANNEL %d",
      onex_nonsig_params.cdma_entry.gen.band_class,
      onex_nonsig_params.cdma_entry.gen.cdma_channel );

    onex_nonsig_params.cdma_enter_init_done = TRUE;
  }

} /* nonsig_cdma_enter_init */


/*===========================================================================

FUNCTION nonsig_fwd_hho_cb

DESCRIPTION
  This callback function will signal MC task that the SRCH tune is complete.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
static void nonsig_fwd_hho_cb(void)
{
  /*------------------------------------------------------------------*/

  (void) rex_set_sigs ( MC_TCB_PTR, ONEX_NONSIG_MC_CMD_SRCH_COMPLETE );

} /* nonsig_fwd_hho_cb */


/*===========================================================================

FUNCTION nonsig_clear_mc_timers

DESCRIPTION
  Clear MC action and dtmf timers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void nonsig_clear_mc_timers(void)
{
  /*------------------------------------------------------------------*/

  /* Clear action timers in case action is pending */
  (void) mcc_clr_callback_timer( &mcc_action_timer, MCC_ACTION_TIMER_ID );
  (void) mcc_clr_callback_timer( &mcc_so_ctl_timer, MCC_SO_CTL_TIMER_ID );

  /* clear dtmf timer in case dtmf is pending */
  (void) mcc_clr_callback_timer( &mcc_bs_insanity_timer,
                                 MCC_BS_INSANITY_TIMER_ID );

} /* nonsig_clear_mc_timers */


/*===========================================================================

FUNCTION nonsig_send_srch_tc_f

DESCRIPTION
  Send SRCH_TC_F to the Searcher task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void nonsig_send_srch_tc_f
(
  boolean fch_active,
  boolean dcch_active,
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  boolean bypass_lc = FALSE;

  /*------------------------------------------------------------------*/

  /* Send a message to Searcher to switch to Traffic Channel */
  M1X_MSG_SSID( DIAG, LEGACY_HIGH,
    "1xNonSig: Send SRCH_TC_F.");
  mcc_srch_buf.tc.hdr.cmd = SRCH_TC_F;
  mcc_srch_buf.tc.aset_cnt  = 1;
  mcc_srch_buf.tc.cdma_freq = cdma.cdmach;
  mcc_srch_buf.tc.band_class = (srch_rx_band_type) cdma.band_class;
  mcc_srch_buf.tc.aset[0].pilot = cur_bs_ptr->pilot_pn;
  mcc_srch_buf.tc.aset[0].pilot_rec_type = CAI_PILOT_1X_COMMON;
  mcc_srch_buf.tc.aset[0].pwr_comb_ind = 0;

  if (fch_active && dcch_active)
  {
    mcc_srch_buf.tc.aset[0].fch_info.active = TRUE;
    mcc_srch_buf.tc.aset[0].fch_info.walsh_chn =
      cmd_ptr->demod_fch_dcch.param.fch.walsh.walsh;
    mcc_srch_buf.tc.aset[0].fch_info.qof_mask_id =
       cmd_ptr->demod_fch_dcch.param.fch.walsh.qof_mask_id;

    mcc_srch_buf.tc.aset[0].dcch_info.active = TRUE;
    mcc_srch_buf.tc.aset[0].dcch_info.walsh_chn =
      cmd_ptr->demod_fch_dcch.param.dcch.walsh.walsh;
    mcc_srch_buf.tc.aset[0].dcch_info.qof_mask_id =
       cmd_ptr->demod_fch_dcch.param.dcch.walsh.qof_mask_id;

    if ( (cmd_ptr->demod_fch_dcch.param.fch.bypass_lc) ||
         (cmd_ptr->demod_fch_dcch.param.dcch.bypass_lc) )
    {
      bypass_lc = TRUE;
    }
  }
  else if (fch_active)
  {
    mcc_srch_buf.tc.aset[0].fch_info.active = TRUE;
    mcc_srch_buf.tc.aset[0].fch_info.walsh_chn =
      cmd_ptr->demod_fch.param.walsh.walsh;
    mcc_srch_buf.tc.aset[0].fch_info.qof_mask_id =
       cmd_ptr->demod_fch.param.walsh.qof_mask_id;

    if (cmd_ptr->demod_fch.param.bypass_lc)
    {
      bypass_lc = TRUE;
    }
  }
  else if (dcch_active)
  {
    mcc_srch_buf.tc.aset[0].dcch_info.active = TRUE;
    mcc_srch_buf.tc.aset[0].dcch_info.walsh_chn =
      cmd_ptr->demod_dcch.param.walsh.walsh;
    mcc_srch_buf.tc.aset[0].dcch_info.qof_mask_id =
       cmd_ptr->demod_dcch.param.walsh.qof_mask_id;

    if (cmd_ptr->demod_dcch.param.bypass_lc)
    {
      bypass_lc = TRUE;
    }
  }

  /* Calculate Traffic Channel Mask */
  if (bypass_lc)
  {
    qw_set( mcc_srch_buf.tc.pn_mask, 0L, 0L);
    onex_nonsig_params.bypass_lc = TRUE;
  }
  else
  {
    dword p_esn;
    qw_set(mcc_srch_buf.tc.pn_mask, 0x318L,
           (p_esn = permute_esn(onex_nonsig_params.nv_esn)));
    M1X_MSG_SSID( DIAG, LEGACY_HIGH,
      "1xNonSig: Using pn_mask 0x318 0x%x",
      p_esn);
    onex_nonsig_params.bypass_lc = FALSE;
  }

  #ifdef FEATURE_IS2000_1X_ADV
  M1X_MSG_SSID( DIAG, LEGACY_HIGH,
    "1xNonSig: Initialize RCP data to default values");
  mcc_update_rcp_data(NULL);

  /* Include RC Param records for all pilots */
  mcc_get_pilots_rc_param_recs(&mcc_srch_buf.tc.rc_params_recs);
  #endif /* FEATURE_IS2000_1X_ADV */

  mcc_srch_cmd( &mcc_srch_buf );

} /* nonsig_send_srch_tc_f */


/*===========================================================================

FUNCTION nonsig_send_rxc_tci_f

DESCRIPTION
  Send RXC_TCI_F to the RX task.

  NOTE: Presently no support for SVD with DCCH as primary FPC channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void nonsig_send_rxc_tci_f
(
  boolean fch_active,
  boolean dcch_active,
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  boolean fpc_active = FALSE;

  /*------------------------------------------------------------------*/

  M1X_MSG_SSID( DIAG, LEGACY_HIGH,
    "1xNonSig: Send RXC_TCI_F.");
  mcc_rx_buf.tci.hdr.command = RXC_TCI_F;
  mcc_rx_buf.tci.for_fch.included = fch_active;
  mcc_rx_buf.tci.for_dcch.included = dcch_active;
  mcc_rx_buf.tci.enable_skip_tci = TRUE;

  if(fch_active && dcch_active)
  {
    /* SVD case */
    fpc_active = cmd_ptr->demod_fch_dcch.param.fch.ffpc_enable;
    mcc_rx_buf.tci.ilpc_enabled = fpc_active;
    mcc_rx_buf.tci.olpc_enabled = fpc_active;
    mcc_rx_buf.tci.frame_offset = cmd_ptr->demod_fch_dcch.param.fch.frame_offset;

    mcc_rx_buf.tci.for_fch.rc =
      (cai_radio_config_type)cmd_ptr->demod_fch_dcch.param.fch.radio_cfg;

    mcc_rx_buf.tci.for_dcch.rc =
      (cai_radio_config_type)cmd_ptr->demod_fch_dcch.param.dcch.radio_cfg;

  }
  else if(fch_active)
  {
    /* FCH only case. */
    fpc_active = cmd_ptr->demod_fch.param.ffpc_enable;
    mcc_rx_buf.tci.ilpc_enabled = fpc_active;
    mcc_rx_buf.tci.olpc_enabled = fpc_active;
    mcc_rx_buf.tci.frame_offset = cmd_ptr->demod_fch.param.frame_offset;

    mcc_rx_buf.tci.for_fch.rc =
      (cai_radio_config_type)cmd_ptr->demod_fch.param.radio_cfg;
  }
  else if(dcch_active)
  {
    /* DCCH only case. */
    fpc_active = (cmd_ptr->demod_dcch.param.ilpc_enabled |
                  cmd_ptr->demod_dcch.param.olpc_enabled);
    mcc_rx_buf.tci.ilpc_enabled = cmd_ptr->demod_dcch.param.ilpc_enabled;
    mcc_rx_buf.tci.olpc_enabled = cmd_ptr->demod_dcch.param.olpc_enabled;
    mcc_rx_buf.tci.frame_offset = cmd_ptr->demod_dcch.param.frame_offset;

    mcc_rx_buf.tci.for_dcch.rc =
      (cai_radio_config_type)cmd_ptr->demod_dcch.param.radio_cfg;
  }

  /* Determine if FFPC is enabled. If so, load the FPC parameters for RX. */
  mcc_rx_buf.tci.fpc.included = fpc_active;

  if(fpc_active)
  {
    /* FPC is active. */
    mcc_rx_buf.tci.fpc.fch_olpc.included = (fpc_active && fch_active);
    mcc_rx_buf.tci.fpc.dcch_olpc.included
      = (fpc_active && dcch_active && !fch_active);
    mcc_rx_buf.tci.fpc.fch_olpc.params.min_setpt.set_to_current = FALSE;
    mcc_rx_buf.tci.fpc.fch_olpc.params.max_setpt.set_to_current = FALSE;

    if(fch_active && dcch_active)
    {
      /* SVD case, FCH and DCCH simultaneously.
         NOTE: DCCH support for primary FPC channel does not
         exist in SVD mode during non-signalling. */
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "FCH is FPC pri channel");
      mcc_rx_buf.tci.fpc.fpc_pri_chan = CAI_FPC_PRI_CHAN_FCH;

      mcc_rx_buf.tci.fpc.fch_olpc.init_setpt =
        cmd_ptr->demod_fch_dcch.param.fch.init_setpt;

      mcc_rx_buf.tci.fpc.fpc_subchan_gain =
        cmd_ptr->demod_fch_dcch.param.fch.subchan_gain;

      mcc_rx_buf.tci.fpc.fpc_mode =
        (cai_fpc_mode_type) cmd_ptr->demod_fch.param.fpc_mode;

      mcc_rx_buf.tci.fpc.fch_olpc.params.target_fer =
        cmd_ptr->demod_fch_dcch.param.fch.olc_params.target_fer;

      mcc_rx_buf.tci.fpc.fch_olpc.params.min_setpt.value =
        cmd_ptr->demod_fch_dcch.param.fch.olc_params.min_setpt;

      mcc_rx_buf.tci.fpc.fch_olpc.params.max_setpt.value =
        cmd_ptr->demod_fch_dcch.param.fch.olc_params.max_setpt;
    }
    else if(fch_active)
    {
      /* FCH is the only channel active. */
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: FCH is FPC pri channel");
      mcc_rx_buf.tci.fpc.fpc_pri_chan = CAI_FPC_PRI_CHAN_FCH;

      mcc_rx_buf.tci.fpc.fpc_mode =
        (cai_fpc_mode_type) cmd_ptr->demod_fch.param.fpc_mode;

      mcc_rx_buf.tci.fpc.fch_olpc.init_setpt =
        cmd_ptr->demod_fch.param.init_setpt;

      mcc_rx_buf.tci.fpc.fpc_subchan_gain =
        cmd_ptr->demod_fch.param.subchan_gain;

      mcc_rx_buf.tci.fpc.fch_olpc.params.target_fer =
        cmd_ptr->demod_fch.param.olc_params.target_fer;

      mcc_rx_buf.tci.fpc.fch_olpc.params.min_setpt.value =
        cmd_ptr->demod_fch.param.olc_params.min_setpt;

      mcc_rx_buf.tci.fpc.fch_olpc.params.max_setpt.value =
        cmd_ptr->demod_fch.param.olc_params.max_setpt;
    }
    else if(dcch_active)
    {
      /* DCCH is the only active channel. */
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: DCCH is FPC pri channel");
      mcc_rx_buf.tci.fpc.fpc_pri_chan = CAI_FPC_PRI_CHAN_DCCH;
      mcc_rx_buf.tci.fpc.fpc_mode = CAI_FPC_MODE_800_PRI;

      mcc_rx_buf.tci.fpc.dcch_olpc.init_setpt =
        cmd_ptr->demod_dcch.param.init_setpt;

      mcc_rx_buf.tci.fpc.fpc_subchan_gain =
        cmd_ptr->demod_dcch.param.fpc_subchan_gain;

      mcc_rx_buf.tci.fpc.dcch_olpc.params.target_fer =
        cmd_ptr->demod_dcch.param.dcch_olc_params.target_fer;

      mcc_rx_buf.tci.fpc.dcch_olpc.params.min_setpt.value =
        cmd_ptr->demod_dcch.param.dcch_olc_params.min_setpt;

      mcc_rx_buf.tci.fpc.dcch_olpc.params.max_setpt.value =
        cmd_ptr->demod_dcch.param.dcch_olc_params.max_setpt;
    }
  }

  #ifdef FEATURE_IS2000_1X_ADV
  /* Load the stored RCPM TCI values. */
  mcc_rx_buf.tci.rcp_init.for_fch_ack_mask_no_rl_blanking =
    onex_rcpm_default_params.for_fch_ack_mask_no_rl_blanking;

  mcc_rx_buf.tci.rcp_init.for_fch_ack_mask_rl_blanking =
    onex_rcpm_default_params.for_fch_ack_mask_rl_blanking;

  mcc_rx_buf.tci.rcp_init.for_sch_ack_mask_no_rl_blanking =
    onex_rcpm_default_params.for_sch_ack_mask_no_rl_blanking;

  mcc_rx_buf.tci.rcp_init.for_sch_ack_mask_rl_blanking =
    onex_rcpm_default_params.for_sch_ack_mask_rl_blanking;

  mcc_rx_buf.tci.rcp_init.rev_gain_adj_acs1 =
    onex_rcpm_default_params.rev_gain_adj_acs1;

  mcc_rx_buf.tci.rcp_init.rev_gain_adj_acs2plus =
    onex_rcpm_default_params.rev_gain_adj_acs2plus;

  mcc_rx_buf.tci.rcp_init.power_control_mode =
    (cai_rcpm_pwr_cntl_mode_type) onex_rcpm_default_params.power_control_mode;

  mcc_rx_buf.tci.rcp_init.power_control_step =
    (cai_pwr_cntl_step_type) onex_rcpm_default_params.power_control_step;

  mcc_rx_buf.tci.rcp_init.rev_fch_blanking_dutycycle =
    (cai_fch_blanking_dutycycle_type) onex_rcpm_default_params.rev_fch_blanking_dutycycle;

  mcc_rx_buf.tci.rcp_init.rev_fch_ack_mask =
    onex_rcpm_default_params.rev_fch_ack_mask;

  mcc_rx_buf.tci.rcp_init.rev_sch_ack_mask =
    onex_rcpm_default_params.rev_sch_ack_mask;

  mcc_rx_buf.tci.rcp_init.for_fch_blanking_dutycycle =
    (cai_fch_blanking_dutycycle_type) onex_rcpm_default_params.for_fch_blanking_dutycycle;

  mcc_rx_buf.tci.rcp_init.for_n2m_ind =
    (cai_for_n2m_ind_type) onex_rcpm_default_params.for_n2m_ind;
  #endif /* FEATURE_IS2000_1X_ADV */

  mcc_rxc_cmd( &mcc_rx_buf );

} /* nonsig_send_rxc_tci_f */


/*===========================================================================

FUNCTION nonsig_send_txc_tc_f

DESCRIPTION
  Send TXC_TC_F to the TX task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void nonsig_send_txc_tc_f
(
  boolean fch_active,
  boolean dcch_active,
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  /*------------------------------------------------------------------*/

  M1X_MSG_SSID( DIAG, LEGACY_HIGH,
    "1xNonSig: Send TXC_TC_F.");
  mcc_tx_buf.tc.hdr.command = TXC_TC_F;
  mcc_tx_buf.tc.sigs        = MCC_TXC_DONE_SIG;
  mcc_tx_buf.tc.tcb_ptr     = MC_TCB_PTR;

  if( onex_nonsig_params.bypass_lc )
  {
    M1X_MSG_SSID( DIAG, LEGACY_HIGH,
      "1xNonSig: Bypass LC enabled.");
    qw_set( mcc_tx_buf.tc.mask, 0L, 0L);
  }
  else
  {
    dword p_esn;
    qw_set(mcc_tx_buf.tc.mask, 0x318L,
           (p_esn = permute_esn(onex_nonsig_params.nv_esn)));
    M1X_MSG_SSID( DIAG, LEGACY_HIGH,
      "1xNonSig: Rev TC pn_mask 0x318 0x%x",
      p_esn);
  }

  mcc_tx_buf.tc.rev_fch.included = fch_active;
  mcc_tx_buf.tc.rev_dcch.included = dcch_active;

  mcc_tx_buf.tc.rev_fch_gating_on = FALSE;
  mcc_tx_buf.tc.rlgain_traffic_pilot = 0;
  mcc_tx_buf.tc.dcch_rate = ENC_FULL_RATE;
  mcc_tx_buf.tc.sch_on = FALSE;

  #ifdef FEATURE_MODEM_1X_SUPPORTS_RF
  /* Enable/Disable Reverse Power Control based on the FTM param enable_rpc */
  {
    boolean rpc_active = FALSE;

    /* Determine if RPC is enabled. */
    if(fch_active && dcch_active)
    {
      rpc_active = cmd_ptr->mod_fch_dcch.param.fch.enable_rpc;
    }
    else if(fch_active)
    {
      rpc_active = cmd_ptr->mod_fch.param.enable_rpc;
    }
    else if(dcch_active)
    {
      rpc_active = FALSE;
    }

    /* Replacing bio with DAL equivalent*/
    switches_dal_attach_1x_ftm = DAL_DeviceAttach(DALDEVICEID_SOFTWARESWITCHES,
                                                  &switches_handle_1x_ftm);
    if(switches_dal_attach_1x_ftm == DAL_SUCCESS)
    {
      (void)DalDevice_Open(switches_handle_1x_ftm,DAL_OPEN_SHARED);
    }

    if (rpc_active)
    {
      if(switches_dal_attach_1x_ftm == DAL_SUCCESS)
      {
        /* clear the 'disable closed loop power control' dip switch */
        (void)DalSoftwareSwitches_ClearSwitch(switches_handle_1x_ftm,
                                              (uint32) DAL_SOFTWARE_SWITCH_5);
      }

      msmdrv_enable_rl_power_control();

    }
    else
    {
      if(switches_dal_attach_1x_ftm == DAL_SUCCESS)
      {
        /* set the 'disable closed loop power control' dip switch */
        (void)DalSoftwareSwitches_SetSwitch(switches_handle_1x_ftm,
                                            (uint32) DAL_SOFTWARE_SWITCH_5);
      }

      msmdrv_disable_rl_power_control();
    }
  }
  #endif /* FEATURE_MODEM_1X_SUPPORTS_RF */

  if(dcch_active && fch_active)
  {
    /* SVD case. */
    mcc_tx_buf.tc.rev_fch.rc =
      (cai_radio_config_type) cmd_ptr->mod_fch_dcch.param.fch.radio_cfg;

    mcc_tx_buf.tc.rev_dcch.rc =
      (cai_radio_config_type) cmd_ptr->mod_fch_dcch.param.dcch.dcch_rc;

    mcc_tx_buf.tc.fch_rate =
      (enc_rate_type) cmd_ptr->mod_fch_dcch.param.fch.fch_rate;

    mcc_tx_buf.tc.sch_on = cmd_ptr->mod_fch_dcch.param.dcch.sch_on;

    if( mcc_tx_buf.tc.sch_on )
    {
       mcc_tx_buf.tc.rev_sch_rc =
         (cai_radio_config_type) cmd_ptr->mod_fch_dcch.param.dcch.sch_rc;

       mcc_tx_buf.tc.sch_rate = (enc_sch_rate_type)
                                   cmd_ptr->mod_fch_dcch.param.dcch.sch_rate;

       mcc_tx_buf.tc.sch_turbo = cmd_ptr->mod_fch_dcch.param.dcch.sch_turbo;
       mcc_tx_buf.tc.sch_ltu_size = cmd_ptr->mod_fch_dcch.param.dcch.sch_ltu_size;
    }

    mcc_tx_buf.tc.num_preambles = cmd_ptr->mod_fch_dcch.param.fch.num_preambles;
    mcc_tx_buf.tc.tx_pattern = cmd_ptr->mod_fch_dcch.param.fch.tx_pattern;
    mcc_tx_buf.tc.frame_offset = cmd_ptr->mod_fch_dcch.param.fch.frame_offset;
  }
  else if(fch_active)
  {
    /* FCH only Case.*/
    mcc_tx_buf.tc.rev_fch.rc = (cai_radio_config_type)
                                  cmd_ptr->mod_fch.param.radio_cfg;

    mcc_tx_buf.tc.fch_rate = (enc_rate_type)
                                cmd_ptr->mod_fch.param.fch_rate;

    mcc_tx_buf.tc.num_preambles = cmd_ptr->mod_fch.param.num_preambles;
    mcc_tx_buf.tc.tx_pattern = cmd_ptr->mod_fch.param.tx_pattern;
    mcc_tx_buf.tc.frame_offset = cmd_ptr->mod_fch.param.frame_offset;
  }
  else if(dcch_active)
  {
    /* DCCH only case. */
    mcc_tx_buf.tc.rev_dcch.rc = (cai_radio_config_type)
                                  cmd_ptr->mod_dcch.param.dcch_rc;

    mcc_tx_buf.tc.sch_on = cmd_ptr->mod_dcch.param.sch_on;

    if( mcc_tx_buf.tc.sch_on )
    {
       mcc_tx_buf.tc.rev_sch_rc =
         (cai_radio_config_type) cmd_ptr->mod_dcch.param.sch_rc;

       mcc_tx_buf.tc.sch_rate = (enc_sch_rate_type)
                                   cmd_ptr->mod_dcch.param.sch_rate;

       mcc_tx_buf.tc.sch_turbo = cmd_ptr->mod_dcch.param.sch_turbo;
       mcc_tx_buf.tc.sch_ltu_size = cmd_ptr->mod_dcch.param.sch_ltu_size;
    }

    mcc_tx_buf.tc.num_preambles = cmd_ptr->mod_dcch.param.num_preambles;
    mcc_tx_buf.tc.tx_pattern = cmd_ptr->mod_dcch.param.tx_pattern;
    mcc_tx_buf.tc.frame_offset = cmd_ptr->mod_dcch.param.frame_offset;
  }

  #ifdef FEATURE_IS2000_1X_ADV
  /* Load the RCPM parameters for TX. */
  mcc_tx_buf.tc.rcp_init.for_fch_ack_mask_no_rl_blanking =
    onex_rcpm_default_params.for_fch_ack_mask_no_rl_blanking;

  mcc_tx_buf.tc.rcp_init.for_fch_ack_mask_rl_blanking =
    onex_rcpm_default_params.for_fch_ack_mask_rl_blanking;

  mcc_tx_buf.tc.rcp_init.for_sch_ack_mask_no_rl_blanking =
    onex_rcpm_default_params.for_sch_ack_mask_no_rl_blanking;

  mcc_tx_buf.tc.rcp_init.for_sch_ack_mask_rl_blanking =
    onex_rcpm_default_params.for_sch_ack_mask_rl_blanking;

  mcc_tx_buf.tc.rcp_init.rev_gain_adj_acs1 =
    onex_rcpm_default_params.rev_gain_adj_acs1;

  mcc_tx_buf.tc.rcp_init.rev_gain_adj_acs2plus =
    onex_rcpm_default_params.rev_gain_adj_acs2plus;

  mcc_tx_buf.tc.rcp_init.power_control_mode =
    (cai_rcpm_pwr_cntl_mode_type) onex_rcpm_default_params.power_control_mode;

  mcc_tx_buf.tc.rcp_init.power_control_step =
    (cai_pwr_cntl_step_type) onex_rcpm_default_params.power_control_step;

  mcc_tx_buf.tc.rcp_init.rev_fch_blanking_dutycycle =
    (cai_fch_blanking_dutycycle_type) onex_rcpm_default_params.rev_fch_blanking_dutycycle;

  mcc_tx_buf.tc.rcp_init.rev_fch_ack_mask =
    onex_rcpm_default_params.rev_fch_ack_mask;

  mcc_tx_buf.tc.rcp_init.rev_sch_ack_mask =
    onex_rcpm_default_params.rev_sch_ack_mask;

  mcc_tx_buf.tc.rcp_init.for_fch_blanking_dutycycle =
    (cai_fch_blanking_dutycycle_type) onex_rcpm_default_params.for_fch_blanking_dutycycle;

  mcc_tx_buf.tc.rcp_init.for_n2m_ind =
    (cai_for_n2m_ind_type) onex_rcpm_default_params.for_n2m_ind;
  #endif /* FEATURE_IS2000_1X_ADV */

  (void) rex_clr_sigs( rex_self(), MCC_TXC_DONE_SIG );
  mcc_txc_cmd( &mcc_tx_buf );

} /* nonsig_send_txc_tc_f */


/*===========================================================================

FUNCTION nonsig_pil_acq

DESCRIPTION
  This function attempts to acquire a pilot.

DEPENDENCIES
  None.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_pil_acq
(
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  onex_nonsig_cmd_status_type acq_test_status = NS1X_TEST_NO_SUCCESS;

  mccsrch_rpt_type *srch_ptr;
  boolean          done_srch_report = FALSE;

  /*------------------------------------------------------------------*/

  M1X_MSG_SSID( DIAG, LEGACY_HIGH,
    "1xNonSig: Prepare for ACQ mode" );

  switch( onex_nonsig_params.ns_fwd_curr_state )
  {
    /* ----------------------------- NA -> ACQ ------------------------------ */
    case NS1X_ENTER:
    case NS1X_INIT_CDMA:
    {
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Prepare for NA -> ACQ" );

      if(cmd_ptr == NULL)
      {
        M1X_MSG_SSID( DIAG, LEGACY_FATAL,
          "1xNonSig: NULL CMD PTR" );
        break; /* Exit */
      }

      /* Initialize M1X_STMR module */
      m1x_stmr_init();

      nonsig_cdma_enter_init();
      nonsig_init_cdma();

      /* Check for a change in band class and channel. */
      if ( (cmd_ptr->pilot_acq.param.cdma_freq != cdma.cdmach) ||
           (cmd_ptr->pilot_acq.param.band_class != cdma.band_class) )
      {
        cdma.band_class = cmd_ptr->pilot_acq.param.band_class;
        cdma.cdmach = cmd_ptr->pilot_acq.param.cdma_freq;
        M1X_MSG_SSID( DIAG, LEGACY_HIGH,
          "1xNonSig: Different Band (%d) and Channel (%d).",
          cdma.band_class,
          cdma.cdmach);
      }

      /* Request that Searcher task enter Acquire mode */
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send SRCH_ACQ_F.");
      mcc_srch_buf.acq.hdr.cmd    = SRCH_ACQ_F;
      mcc_srch_buf.acq.cdma_freq  =
        cmd_ptr->pilot_acq.param.cdma_freq;
      mcc_srch_buf.acq.band_class =
        (srch_rx_band_type)(cmd_ptr->pilot_acq.param.band_class);
      mcc_srch_buf.acq.acq_mode   =
        (db_acq_mode_type) (cmd_ptr->pilot_acq.param.acq_mode);
      mcc_srch_cmd( &mcc_srch_buf );

      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Waiting for SRCH ACQ report" );
      (void)nonsig_mc_wait( MCC_SRCH_Q_SIG | MCC_Q_SIG );

      /* Get SRCH acquisition report */
      while ( !done_srch_report )
      {
        if ((srch_ptr = (mccsrch_rpt_type*) q_get( &mcc_srch_q )) != NULL)
        {
          /* Keep track of the SRCH report for debugging purpose */
          mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, cdma.curr_state,
                              cdma.curr_state, MCC_NON_SLEEP, NONSIG_PIL_ACQ );
          
          if (srch_ptr->hdr.rpt == SRCH_ACQ_R)
          {
            M1X_MSG_SSID( DIAG, LEGACY_HIGH,
              "1xNonSig: Pilot Acquisition Successful.");
            done_srch_report = TRUE;
            acq_test_status = NS1X_TEST_SUCCESS;
            onex_nonsig_params.ns_fwd_curr_state = NS1X_PIL_ACQ;
            cmd_done( &srch_ptr->hdr.rpt_hdr );

            #ifdef FEATURE_1X_CP_MEM_OPT
            mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
            #endif /* FEATURE_1X_CP_MEM_OPT */
          }
          else if (srch_ptr->hdr.rpt == SRCH_ACQFAIL_R)
          {
            /* Searcher failed to acquire */
            done_srch_report = TRUE;
            cmd_done( &srch_ptr->hdr.rpt_hdr );

            #ifdef FEATURE_1X_CP_MEM_OPT
            mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
            #endif /* FEATURE_1X_CP_MEM_OPT */
          }
          else
          {
            /* unexpected command. continue waiting. */
            cmd_done( &srch_ptr->hdr.rpt_hdr );

            #ifdef FEATURE_1X_CP_MEM_OPT
            mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
            #endif /* FEATURE_1X_CP_MEM_OPT */

            (void) nonsig_mc_wait( MCC_SRCH_Q_SIG | MCC_Q_SIG );
          }
        }
      }

      break;
    }

    /* ---------------------------- INVALID STATE --------------------------- */
    default:
    {
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,  "1xNonSig: Invalid FWD state (%d)", onex_nonsig_params.ns_fwd_curr_state, 0, 0 );
      acq_test_status = NS1X_TEST_INVALID_STATE;
      break;
    }

  } /* end of switch */

  return acq_test_status;

} /* nonsig_pil_acq */


/*===========================================================================

FUNCTION nonsig_mc_sync

DESCRIPTION
  This function attempts configure 1X to decode the sync channel message.

DEPENDENCIES
  None.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_mc_sync
(
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  onex_nonsig_cmd_status_type sync_test_status = NS1X_TEST_NO_SUCCESS;

  mccrxtx_cmd_type *msg_ptr;
  bs_info_id_type  bs_info_id;
  rex_sigs_type    sync_sigs;

  /*------------------------------------------------------------------*/

  MODEM_1X_UNUSED(cmd_ptr);

  M1X_MSG_SSID( DIAG, LEGACY_HIGH,
    "1xNonSig: Prepare for SYNC mode" );

  switch( onex_nonsig_params.ns_fwd_curr_state )
  {
    /* -------------------------- NA -> ACQ -> SYNC ------------------------- */
    case NS1X_ENTER:
    case NS1X_INIT_CDMA:
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: Prepare for NA -> ACQ -> SYNC" );

      /* Finish ACQ attempt and then move on to SYNC if successful */
      (void) nonsig_pil_acq( &onex_nonsig_acq_cmd );

      /* Check if ACQ was successful */
      if( onex_nonsig_params.ns_fwd_curr_state != NS1X_PIL_ACQ )
      {
        M1X_MSG_SSID( DIAG, LEGACY_ERROR,
          "Failed to ACQ on Band (%d) and Channel (%d)",
          cdma.band_class,
          cdma.cdmach );

        /* ACQ failed. Can't continue to SYNC */
        break;
      }
      else
      {
        M1X_MSG_SSID( DIAG, LEGACY_HIGH,
          "ACQ Successful on Band (%d) and Channel (%d)",
          cdma.band_class,
          cdma.cdmach );

        /* Continue on to SYNC state */
      }
    }

    /* ----------------------------- ACQ -> SYNC ---------------------------- */
    case NS1X_PIL_ACQ:
    case NS1X_SYNC:
    case NS1X_IDLE:
    {
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send RXC_SC_F." );
      mcc_rx_buf.sc.hdr.command = RXC_SC_F;
      mcc_rxc_cmd( &mcc_rx_buf );

      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send SRCH_SC_F." );
      mcc_srch_buf.sc.hdr.cmd = SRCH_SC_F;
      mcc_srch_cmd( &mcc_srch_buf );

      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Wait for valid SYNC message" );

      /* Wait for RX to decode a sync channel message.
         If a good message is not decoded in 1s then abort
         so we do not remain in this state forever.

         T21 timer = 1000ms, which is used in online mode.

         If the timer expires then by default there shouldn't
         be a message pending or its not a RXC_CDMA_SC channel
         message, so the test will fail gracefully. */
      (void) rex_set_timer( &mcc_cmd_timer, cai_tmo.t21m );

      sync_sigs = nonsig_mc_wait( MCC_RXTX_Q_SIG | MCC_CMD_TIMER_SIG );

      while ( ( ( msg_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_q) ) != NULL) &&
              ( ( sync_sigs & MCC_CMD_TIMER_SIG) == 0 ) )
      {
        /* Received a message --> process it */

        if ((msg_ptr->hdr.command == MCCRXTX_MSG_F) &&
            (msg_ptr->msg.chn == (caix_chan_type) RXC_CDMA_SC))
        {
          /* Message from Sync Channel */
          parm_store_sync_time( msg_ptr->msg.msg.sync.sys_time );

          /* We have got the sync channel message, which means we need
             to start storing information about this base station.
             Initialize a new entry in bs_info array (or find the old
             one if it exists. */
          bs_info_id.ovhd_chan = CAIX_PC;
          bs_info_id.pilot_pn = msg_ptr->msg.msg.sync.pilot_pn;
          bs_info_id.cdma_ch = cdma.cdmach;
          bs_info_id.ovhd_walsh_ch = PRI_PAGECH;
          bs_info_id.band_class = cdma.band_class;
          bs_info_id.sid = INVALID_SID_NID_VAL;
          bs_info_id.nid = INVALID_SID_NID_VAL;
          mcc_entry_init(&bs_info_id, FALSE);

          /* Place the received Sync Channel msg into the entry for this BS */
          /* store sync msg and mark msg received */
          cur_bs_ptr->sy = *(caii_sync_type *)&msg_ptr->msg.msg ;
          cur_bs_ptr->rx.sy_rx = TRUE;

          // Notify reception of PREV and SID.  For special MDR, we use base station's
          // prev instead of prev_in_use

          /* Remember the subframe number that message is received in */
          onex_nonsig_params.sframe_num = msg_ptr->msg.frm.sframe;

          cdma.pagech = PRI_PAGECH;
          cdma.page_chan_s = 1;

          M1X_MSG_SSID( DIAG, LEGACY_HIGH,
            "1xNonSig: SYNC Successful!, sframe_num=%d, systime=%x,%x",
            onex_nonsig_params.sframe_num,
            msg_ptr->msg.msg.sync.sys_time[1],
            msg_ptr->msg.msg.sync.sys_time[0]);

          sync_test_status = NS1X_TEST_SUCCESS;
          onex_nonsig_params.ns_fwd_curr_state = NS1X_SYNC;

          /* Place message from layer 2 back on queue if requested */
          cmd_done( &msg_ptr->hdr.cmd_hdr );

          #ifdef FEATURE_1X_CP_MEM_OPT
          mc_free_queue_buf( msg_ptr, MC_MCC_RXTX_Q );
          #endif /* FEATURE_1X_CP_MEM_OPT */

          break;
        }
        else
        {
          /* Place message from layer 2 back on queue if requested */
          cmd_done( &msg_ptr->hdr.cmd_hdr );

          #ifdef FEATURE_1X_CP_MEM_OPT
          mc_free_queue_buf( msg_ptr, MC_MCC_RXTX_Q );
          #endif /* FEATURE_1X_CP_MEM_OPT */

          sync_sigs = nonsig_mc_wait( MCC_RXTX_Q_SIG | MCC_CMD_TIMER_SIG );
        }
      } /* end if ((msg_ptr = q_get( &mcc_rxtx_q) ) != NULL) */

      (void) rex_clr_timer (&mcc_cmd_timer);
      (void) rex_clr_sigs( rex_self(),  MCC_RXTX_Q_SIG | MCC_CMD_TIMER_SIG );

      break;
    }

    /* ---------------------------- INVALID STATE --------------------------- */
    default:
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: Invalid FWD state (%d)",
        onex_nonsig_params.ns_fwd_curr_state );
      sync_test_status = NS1X_TEST_INVALID_STATE;
      break;
    }
  } /* end of switch */

  return sync_test_status;

} /* nonsig_mc_sync */


/*===========================================================================

FUNCTION nonsig_paging

DESCRIPTION
  This function configures 1x for idle state entry.

DEPENDENCIES
  None.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_paging
(
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  onex_nonsig_cmd_status_type pch_enter_status = NS1X_TEST_NO_SUCCESS;

  uint8 i = 0;

  mccsrch_rpt_type *srch_ptr;  /* Pointer to report from Searcher task */
  boolean done_slew = FALSE;   /* Indicate if slew is done */
  db_items_value_type db_value;/* Buffer to send data to DB */
  onex_nonsig_cmd_1x_paging_param_type *params_ptr;

  /*------------------------------------------------------------------*/

  if(cmd_ptr != NULL)
  {
    params_ptr = &cmd_ptr->demod_paging.param;
  }
  else
  {
    params_ptr = &onex_paging_default_params;
  }

  M1X_MSG_SSID( DIAG, LEGACY_HIGH,
    "1xNonSig: Prepare for IDLE mode" );

  switch( onex_nonsig_params.ns_fwd_curr_state )
  {
    /* ----------------- NA -> ACQ -> SYNC -> SLEW -> PAGING ---------------- */
    case NS1X_ENTER:
    case NS1X_INIT_CDMA:
    {
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Prepare for ACQ -> SYNC -> SLEW -> PAGING" );
      /* Continue on to NS1X_SYNC case which takes care of ACQ as well */
    }

    /* -------------------- ACQ -> SYNC -> SLEW -> PAGING ------------------- */
    case NS1X_PIL_ACQ:
    case NS1X_SYNC:
    {
      /* In FTM SYNC state we do not continue to update the LC state, so before
         idle we need to grab another sync message */
      pch_enter_status = nonsig_mc_sync( NULL );

      if(pch_enter_status != NS1X_TEST_SUCCESS)
      {
        M1X_MSG_SSID( DIAG, LEGACY_HIGH,
          "Failed to update LC state" );

        return pch_enter_status;
      }

      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send TXC_CDMA_F." );
      mcc_tx_buf.cdma.hdr.command = TXC_CDMA_F;
      mcc_txc_cmd( &mcc_tx_buf );

      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send SRCH_SLEW_F, sframe_num=%d, pilotpn=%d, ch=%d",
        onex_nonsig_params.sframe_num,
        cur_bs_ptr->pilot_pn,
        cdma.cdmach );

      mcc_srch_buf.slew.hdr.cmd = SRCH_SLEW_F;
      mcc_srch_buf.slew.pilot_index = (int2) cur_bs_ptr->pilot_pn;
      mcc_srch_buf.slew.phase_80ms =  onex_nonsig_params.sframe_num;
      mcc_srch_buf.slew.cdma_freq = cdma.cdmach;
      mcc_srch_buf.slew.band_class = (srch_rx_band_type)(cdma.band_class);
      mcc_srch_cmd( &mcc_srch_buf );

      while ( !done_slew )
      {
        if ( (srch_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q)) != NULL )
        {
          if ( srch_ptr->hdr.rpt == SRCH_SLEW_R )
          {
            done_slew = TRUE;
            M1X_MSG_SSID( DIAG, LEGACY_HIGH,
              "1xNonSig: Search in Slew." );
          }
          /* Keep track of the SRCH report for debugging purpose */
          mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, cdma.curr_state,
                             cdma.curr_state, MCC_NON_SLEEP, NONSIG_PAGING );

          /* Place item back on queue and set signal if requested */
          cmd_done( &srch_ptr->hdr.rpt_hdr );

          #ifdef FEATURE_1X_CP_MEM_OPT
          mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
          #endif /* FEATURE_1X_CP_MEM_OPT */
        }
        else
        {
          /* Wait for report */
          (void) nonsig_mc_wait( MCC_SRCH_Q_SIG | MCC_Q_SIG );
        }
      }

      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send TXC_PC_F." );
      mcc_tx_buf.pc.hdr.command = TXC_PC_F;
      mcc_txc_cmd( &mcc_tx_buf );

      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send RXC_IDLE_F." );
      mcc_rx_buf.idle.hdr.command = RXC_IDLE_F;
      mcc_rx_buf.idle.chan_info.chan_id = CAI_PCH;
      mcc_rx_buf.idle.chan_info.config.pch.rate =
        (cai_pch_rate_type) params_ptr->pc_rate;

      /* Multiply time received in message by 4 to get
         frame number for Receive task */
      qw_mul(mcc_rx_buf.idle.frame, cur_bs_ptr->sy.sys_time, 4L);

      /* seed random number generator */
      ran_seed( (onex_nonsig_params.nv_esn ^ qw_lo( mcc_rx_buf.idle.frame )) %
                0x7FFFFFFF );

      mcc_rxc_cmd( &mcc_rx_buf );

      onex_nonsig_params.wait_for_pc_msg = params_ptr->wait_for_pc_msg;

      /* Time to tell Searcher to go to Idle state */
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send SRCH_IDLE_F, systime=%x,%x",
        mcc_srch_buf.idle.sys_time[1],
        mcc_srch_buf.idle.sys_time[0]);

      mcc_srch_buf.idle.hdr.cmd = SRCH_IDLE_F;
      mcc_srch_buf.idle.band_class = (srch_rx_band_type)(cdma.band_class);
      mcc_srch_buf.idle.cdma_freq = cdma.cdmach;

      /* Place paging channel long code state @ next 80ms into buffer */
      qw_equ( mcc_srch_buf.idle.pn_state, cur_bs_ptr->sy.lc_state );

      /* Account for extra 80ms count in system time about to occur in TS */
      qw_equ( mcc_srch_buf.idle.sys_time, cur_bs_ptr->sy.sys_time );
      qw_dec( mcc_srch_buf.idle.sys_time, 1L );

      /* calculate Paging Channel or Broadcast Control Channel mask */
      qw_set( mcc_srch_buf.idle.pn_mask,  0x319L, params_ptr->long_code_mask);

      mcc_srch_buf.idle.pgslot = 0;

      mcc_srch_cmd( &mcc_srch_buf );

      db_value.pilot_pn_off = cur_bs_ptr->sy.pilot_pn;
      db_put(DB_PILOT_PN_OFF, &db_value);

      db_value.code_channel = params_ptr->walsh_code;
      db_put(DB_CODE_CHANNEL, &db_value);

      if ( onex_nonsig_params.wait_for_pc_msg )
      {
        /* Wait for a Paging Channel message */
        pch_enter_status = nonsig_wait_for_valid_page_report(params_ptr);
      }
      else
      {
        /* Don't wait for a PC message */
        (void) srch_start_pc_state(FALSE);
        pch_enter_status = NS1X_TEST_SUCCESS;
          /* Remember the new state */
      }

      if(pch_enter_status == NS1X_TEST_SUCCESS)
      {
        /* Calculate HASH_KEY (see IS-95 section 6.6.7.1) */
        cdma.imsi_hash_key = cdma.imsi_s1 + (((dword) cdma.imsi_s2) << 24);

        /* Initialize variables used for Idle State */
        mccidl_ftm_init();

        onex_nonsig_params.ns_fwd_curr_state = NS1X_IDLE;

        M1X_MSG_SSID( DIAG, LEGACY_HIGH,
          "1xNonSig: Demodulate Paging Channel" );
      }

      break;
    }

    /* ------------------------------- PAGING ------------------------------- */
    case NS1X_IDLE:
    {
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Already in Idle. Update channel configurations." );

      mcc_srch_buf.chan_config.hdr.cmd = SRCH_CHAN_CONFIG_F;
      mcc_srch_buf.chan_config.num_chan = 1;

      mcc_rx_buf.chan_config.hdr.command = RXC_CHAN_CONFIG_F;
      mcc_rx_buf.chan_config.num_chan = 1;
      mcc_rx_buf.chan_config.skip_frame = FALSE;
      mcc_rx_buf.chan_config.chan_info[i].ebcsms = FALSE;

      if (params_ptr->channel_type == (byte) CAI_F_CCCH )
      {
        mcc_srch_buf.chan_config.chan_info[i].chan_id = (cai_rx_channel_id_type) params_ptr->channel_type;
        mcc_srch_buf.chan_config.chan_info[i].config.fccch.fccch_id = params_ptr->fccch_id;
        mcc_srch_buf.chan_config.chan_info[i].config.fccch.walsh_chn = params_ptr->walsh_code;
        mcc_srch_buf.chan_config.chan_info[i].config.fccch.c_rate = (cai_code_rate_type) params_ptr->code_rates;
        mcc_srch_buf.chan_config.chan_info[i].config.fccch.rate = (cai_fccch_data_rate_type) params_ptr->fccch_rate;
        mcc_srch_buf.chan_config.chan_info[i].config.fccch.size = CAI_FCCCH_20MS_FRAME;

        mcc_rx_buf.chan_config.chan_info[i].chan_id = (cai_rx_channel_id_type) params_ptr->channel_type;
        mcc_rx_buf.chan_config.chan_info[i].config.fccch.code_chan = params_ptr->walsh_code;
        mcc_rx_buf.chan_config.chan_info[i].config.fccch.crate = (cai_code_rate_type) params_ptr->code_rates;
        mcc_rx_buf.chan_config.chan_info[i].config.fccch.rate = (cai_fccch_data_rate_type) params_ptr->fccch_rate;
        mcc_rx_buf.chan_config.chan_info[i].config.fccch.frame_size = CAI_FCCCH_20MS_FRAME;
      }
      else if (params_ptr->channel_type == (byte) CAI_BCCH )
      {
        mcc_srch_buf.chan_config.chan_info[i].chan_id = (cai_rx_channel_id_type) params_ptr->channel_type;
        mcc_srch_buf.chan_config.chan_info[i].config.bcch.walsh_chn = params_ptr->walsh_code;
        mcc_srch_buf.chan_config.chan_info[i].config.bcch.rate = (cai_bcch_rate_type) params_ptr->bcch_rate;
        mcc_srch_buf.chan_config.chan_info[i].config.bcch.c_rate = (cai_code_rate_type) params_ptr->code_rates;

        mcc_rx_buf.chan_config.chan_info[i].chan_id = (cai_rx_channel_id_type) params_ptr->channel_type;
        mcc_rx_buf.chan_config.chan_info[i].config.bcch.code_chan = params_ptr->walsh_code;
        mcc_rx_buf.chan_config.chan_info[i].config.bcch.rate = (cai_bcch_rate_type) params_ptr->bcch_rate;
        mcc_rx_buf.chan_config.chan_info[i].config.bcch.crate = (cai_code_rate_type) params_ptr->code_rates;
      }
      else
      {
        mcc_srch_buf.chan_config.chan_info[i].chan_id = (cai_rx_channel_id_type) params_ptr->channel_type;
        mcc_srch_buf.chan_config.chan_info[i].config.pch.walsh_chn = params_ptr->walsh_code;
        mcc_srch_buf.chan_config.chan_info[i].config.pch.rate = (cai_pch_rate_type) params_ptr->pc_rate;

        mcc_rx_buf.chan_config.chan_info[i].chan_id = (cai_rx_channel_id_type) params_ptr->channel_type;
        mcc_rx_buf.chan_config.chan_info[i].config.pch.rate = (cai_pch_rate_type) params_ptr->pc_rate;
      }

      onex_nonsig_params.wait_for_pc_msg = params_ptr->wait_for_pc_msg;

      /* Send command to RXC */
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send RXC_CHAN_CONFIG_F.");
      mcc_rxc_cmd( &mcc_rx_buf );

      /* Send command to SRCH */
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send SRCH_CHAN_CONFIG_F.");
      mcc_srch_cmd( &mcc_srch_buf );

      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Channel update complete.");

      if ( onex_nonsig_params.wait_for_pc_msg )
      {
        pch_enter_status = nonsig_wait_for_valid_page_report(params_ptr);
      } /* Wait for a Paging Channel message */
      else
      {
        /* Don't wait for a PC message */
        pch_enter_status = NS1X_TEST_SUCCESS;
      }
      break;
    }

    /* ---------------------------- INVALID STATE --------------------------- */
    default:
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: Invalid FWD state (%d)",
        onex_nonsig_params.ns_fwd_curr_state );
      pch_enter_status = NS1X_TEST_INVALID_STATE;
      break;
    }
  } /* end of switch */

  return pch_enter_status;

} /* nonsig_paging */


/*===========================================================================

FUNCTION nonsig_srch_to_access

DESCRIPTION
    The mobile will be instructed to transmit a series of access probe
    sequences to the basestation. It will continue to do so until the
    max access probe limit is reached.

DEPENDENCIES
    Must be in IDLE forward state before calling this function.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_srch_to_access
(
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  onex_nonsig_cmd_status_type access_status = NS1X_TEST_NO_SUCCESS;

  mccsrch_rpt_type *rpt_ptr;
  dword wait_mask, sigs;
  boolean continue_proc = TRUE;
  mcc_transceiver_request_status_type request_status =
    MCC_TRANSCEIVER_RESOURCE_GRANTED;

  /*------------------------------------------------------------------*/

  MODEM_1X_UNUSED( cmd_ptr );

  /* Push SRCH to ACCESS state... */

  /* Clear mcc_cmd_timer and MCC_CMD_TIMER_SIG signal and timer before we use it */
  (void) rex_clr_timer( &mcc_cmd_timer );
  (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_TIMER_SIG );

  /* allow SRCH to settle in paging before pushing to access... */
  (void)rex_set_timer( &mcc_cmd_timer, 20 );
  (void)mcc_wait( MCC_CMD_TIMER_SIG );

  /* Clear mcc_cmd_timer and MCC_CMD_TIMER_SIG signal and timer before we use it */
  (void) rex_clr_timer( &mcc_cmd_timer );
  (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_TIMER_SIG );

  /* mcc_state_time has already been associated with MCC_STATE_TIMER_SIG
   * T33m is a timer that defines maximum time to enter Update Overhead Information
   * Substate of the System Access State, which may be the most close to
   * this waiting timer scenario
   *
   * The value of T33m is 300ms      */
  (void)rex_set_timer(&mcc_cmd_timer, CAI_T33M);

  /* set parameters for Searcher command SRCH_ACCESS_F */
  mcc_srch_buf.hdr.cmd = SRCH_ACCESS_F;
  mcc_srch_buf.access.access_perf = SRCH_HIGH_PERF_ACCESS;
  mcc_srch_buf.access.access_type = SRCH_ACCESS_MOBILE_ORIG_IMMED_TYPE;

  /* send command SRCH_ACCESS_F to searcher */
  mcc_srch_cmd(&mcc_srch_buf);

  M1X_MSG_SSID( DIAG, LEGACY_HIGH,
    "Sent blocking transceiver request to Searcher");

  while ( continue_proc )
  {
    /* set the wait mask to be MCC_SRCH_Q_SIG | MCC_CMD_TIMER_SIG
     * so it will return either by a timer expiration, or by a
     * Searcher Report */
    wait_mask = MCC_SRCH_Q_SIG | MCC_CMD_TIMER_SIG;

    /* block waiting here after sending searcher command out */
    sigs = mcc_wait(wait_mask);

    /* otherwise, there must be a searcher report have come in.
     * check if there is a SRCH_ACCESS_READY_R came, if not,
     * issue another mcc_wait
     */
    if ( (sigs & MCC_SRCH_Q_SIG) != 0 )
    {
      if ( (rpt_ptr = (mccsrch_rpt_type *)q_get( &mcc_srch_q )) != NULL )
      {
        if ( rpt_ptr->hdr.rpt == SRCH_ACCESS_READY_R )
        {
          M1X_MSG_SSID( DIAG, LEGACY_HIGH,
            "Received the SRCH_ACCESS_READY_R" );
          request_status = MCC_TRANSCEIVER_RESOURCE_GRANTED;
          continue_proc = FALSE;
        }

        else if ( rpt_ptr->hdr.rpt == SRCH_REQUEST_SYS_RESTART_R )
        {
          M1X_MSG_SSID( DIAG, LEGACY_HIGH,
            "SRCH request sys restart" );
          request_status = MCC_TRANSCEIVER_RESOURCE_LOST;
          continue_proc = FALSE;
        }

        else
        {
          M1X_MSG_SSID( DIAG, LEGACY_HIGH,
            "Invalid SRCH  rpt %d",
            rpt_ptr->hdr.rpt );
        }
        /* Keep track of the SRCH report for debugging purpose */
        mcc_enqueue_dbg_srch_cmd( rpt_ptr->hdr.rpt, cdma.curr_state,
                     cdma.curr_state, MCC_NON_SLEEP, NONSIG_SRCH_TO_ACCESS );

        /* clean up report processing */
        cmd_done( &rpt_ptr->hdr.rpt_hdr );

        #ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( rpt_ptr, MCC_SRCH_Q );
        #endif /* FEATURE_1X_CP_MEM_OPT */
      }
    }

    else if ( (sigs & MCC_CMD_TIMER_SIG) != 0 )
    {
      /* if the waiting returns a timer expiry, then return a FALSE */
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "Timer expired to get the transceiver resource" );
      request_status = MCC_TRANSCEIVER_REQUEST_TIMEOUT;
      continue_proc = FALSE;
    }

  } /* end while */

  /* Clear mcc_cmd_timer and MCC_CMD_TIMER_SIG signal and timer before return*/
  (void) rex_clr_timer( &mcc_cmd_timer );
  (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_TIMER_SIG );

  if(request_status == MCC_TRANSCEIVER_RESOURCE_GRANTED)
  {
    access_status = NS1X_TEST_SUCCESS;
  }
  else
  {
    access_status = NS1X_TEST_NO_SUCCESS;
  }

  return access_status;

} /* nonsig_srch_to_access */


/*===========================================================================

FUNCTION nonsig_access

DESCRIPTION
    The mobile will be instructed to transmit a series of access probe
    sequences to the basestation. It will continue to do so until the
    max access probe limit is reached.

DEPENDENCIES
  None

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_access
(
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  onex_nonsig_cmd_status_type access_status = NS1X_TEST_NO_SUCCESS;

  static byte acc_msg[33];
  onex_nonsig_cmd_access_probe_type *params_ptr;

  /*------------------------------------------------------------------*/

  M1X_MSG_SSID( DIAG, LEGACY_HIGH,
    "1xNonSig: Prepare for ACCESS mode" );

  switch ( onex_nonsig_params.ns_fwd_curr_state )
  {
    /* ----------------------------- NA -> ACQ ------------------------------ */
    case NS1X_ENTER:
      /* Prepare for ACQ, ensure ACQ will fail to only test RL
         REVISIT: Find a way to enable FW without passing through ACQ */
      (void) nonsig_pil_acq( &onex_nonsig_acq_cmd );

    case NS1X_INIT_CDMA:
    {
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send TXC_CDMA_F." );
      mcc_tx_buf.cdma.hdr.command = TXC_CDMA_F;
      mcc_txc_cmd( &mcc_tx_buf );

      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send TXC_PC_F." );
      mcc_tx_buf.pc.hdr.command = TXC_PC_F;
      mcc_txc_cmd( &mcc_tx_buf );
      break;
    }

    /* ---------------------------- INVALID STATE --------------------------- */
    default:
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Invalid FWD state (%d)",
        onex_nonsig_params.ns_fwd_curr_state );
      break;
  }

  switch( onex_nonsig_params.ns_rev_curr_state )
  {
    /* ------------------------- INIT_CDMA -> R-ACH ------------------------- */
    case NS1X_INIT_CDMA:
    case NS1X_SYS_ACC:
    {
      /* Prep Tx for ACCESS and send probes... */

      if(cmd_ptr != NULL)
      {
        params_ptr = &cmd_ptr->access_probe.param;
      }
      else
      {
        params_ptr = &onex_access_default_params;
      }

      if( params_ptr->use_reach == TRUE )
      {
        #ifndef FEATURE_IS2000_REL_A_CC
        M1X_MSG_SSID( DIAG, LEGACY_FATAL,
          "1xNonSig: R-EACH not supported." );
        return (NS1X_TEST_NO_SUCCESS);
        #else

        M1X_MSG_SSID( DIAG, LEGACY_HIGH,
          "1xNonSig: Configuring for R-EACH." );
        M1X_MSG_SSID( DIAG, LEGACY_HIGH,
          "1xNonSig: Send TXC_REACH_F" );
        mcc_tx_buf.hdr.command = TXC_REACH_F;
        mcc_tx_buf.hdr.cmd_hdr.done_q_ptr = NULL;
        mcc_tx_buf.hdr.cmd_hdr.task_ptr = NULL;

        /* Signal when access attempt is finished */
        mcc_tx_buf.reach.tcb_ptr = rex_self();
        mcc_tx_buf.reach.sigs = MCC_TXC_DONE_SIG;

        /* fill reach cmd body */
        mcc_tx_buf.reach.req = TRUE;
        mcc_tx_buf.reach.len = 33*8;
        mcc_tx_buf.reach.msg_ptr = acc_msg;
        mcc_tx_buf.reach.init_pwr = params_ptr->init_pwr;
        mcc_tx_buf.reach.nom_pwr = params_ptr->nom_pwr;
        mcc_tx_buf.reach.pwr_step = params_ptr->pwr_step;
        mcc_tx_buf.reach.num_step = params_ptr->num_step;
        mcc_tx_buf.reach.seq_num = params_ptr->seq_num;
        mcc_tx_buf.reach.acc_tmo = 0;
        mcc_tx_buf.reach.bkoff = 1;
        mcc_tx_buf.reach.probe_bkoff = 0;
        mcc_tx_buf.reach.p = 0xffffffff;

        /* The following three parameters are used for constructing the
           Access Channel long code mask */
        mcc_tx_buf.reach.fccch   = params_ptr->pagech;  // Current FCC Channel.
        mcc_tx_buf.reach.base_id  = params_ptr->base_id;
        mcc_tx_buf.reach.pilot_pn = params_ptr->pilot_pn;

        mcc_tx_buf.reach.rate = TXC_EACH_RATE_9600_20_V;
        mcc_tx_buf.reach.preamble_enabled = FALSE;
        mcc_tx_buf.reach.preamble_num_frac = 0;
        mcc_tx_buf.reach.preamble_frac_duration = 0;
        mcc_tx_buf.reach.preamble_off_duration = 0;
        mcc_tx_buf.reach.preamble_add_duration = 0;
        mcc_tx_buf.reach.slot = 0;
        mcc_tx_buf.reach.slot_offset1 = 0;
        mcc_tx_buf.reach.slot_offset2 = 0;
        mcc_tx_buf.reach.num_each_ba = 1;
        mcc_tx_buf.reach.rlgain_common_pilot = 0;
        mcc_tx_buf.reach.ic_thresh = 0;
        mcc_tx_buf.reach.ic_max = 0;
        #endif /* !FEATURE_IS2000_REL_A_CC */
      }
      else
      {
        M1X_MSG_SSID( DIAG, LEGACY_HIGH,
          "1xNonSig: Configuring for R-ACH." );

        mcc_tx_buf.acc.len = 33*8;

        mcc_tx_buf.acc.msg_ptr = acc_msg;

        M1X_MSG_SSID( DIAG, LEGACY_HIGH,
          "1xNonSig: Send TXC_ACCESS_F" );
        mcc_tx_buf.hdr.command = TXC_ACCESS_F;
        mcc_tx_buf.hdr.cmd_hdr.done_q_ptr = NULL;
        /* Setup the command header so you're notified when the command
           is processed */
        mcc_tx_buf.hdr.cmd_hdr.task_ptr = MC_TCB_PTR;
        mcc_tx_buf.hdr.cmd_hdr.sigs = MCC_CMD_SIG;

        /* Signal when access attempt is finished */
        mcc_tx_buf.acc.tcb_ptr = rex_self();
        mcc_tx_buf.acc.sigs = MCC_TXC_DONE_SIG;
        mcc_tx_buf.acc.pam_sz = params_ptr->pam_sz;
        mcc_tx_buf.acc.max_cap_sz = params_ptr->max_cap_sz;
        mcc_tx_buf.acc.req = TRUE;
        mcc_tx_buf.acc.acc_tmo = 0;
        mcc_tx_buf.acc.bkoff = 1;
        mcc_tx_buf.acc.probe_bkoff = 0;
        mcc_tx_buf.acc.acc_chan = 0;
        mcc_tx_buf.acc.use_chan = 0;

        mcc_tx_buf.acc.init_pwr = params_ptr->init_pwr;
        mcc_tx_buf.acc.nom_pwr = params_ptr->nom_pwr;
        mcc_tx_buf.acc.pwr_step = params_ptr->pwr_step;
        mcc_tx_buf.acc.num_step = params_ptr->num_step;
        mcc_tx_buf.acc.seq_num  = params_ptr->seq_num;

        /* The following three parameters are used for constructing the
           Access Channel long code mask */
        mcc_tx_buf.acc.pagech   = params_ptr->pagech;
        mcc_tx_buf.acc.base_id  = params_ptr->base_id;
        mcc_tx_buf.acc.pilot_pn = params_ptr->pilot_pn;

        /* Hash the probe pn randomization even though we dont do it for now */
        mcc_tx_buf.acc.rn = 0;
        mcc_tx_buf.acc.p = 0xffffffff;
      }

      (void)rex_clr_sigs(rex_self(), MCC_CMD_SIG);
      txc_cmd( &mcc_tx_buf );

      /* Wait for TX task to process the command.Wait for TX to notify you that
      the command was processed.*/
      (void) nonsig_mc_wait( MCC_CMD_SIG );
      (void)rex_clr_sigs(rex_self(), (MCC_TXC_DONE_SIG | MCC_CMD_SIG));

      /* check the status of the command processing. Was it done successfully? */
      if(mcc_tx_buf.hdr.status == TXC_DONE_S)
      {
        /* Indicate that we are starting a new access attempt */
        mccsa_set_access_in_progress();

        M1X_MSG_SSID( DIAG, LEGACY_HIGH,
          "1xNonSig: Wait for access probes to complete" );
        (void) nonsig_mc_wait(MCC_TXC_DONE_SIG);
        M1X_MSG_SSID( DIAG, LEGACY_HIGH,
          "1xNonSig: Access probes completed" );

        /* Clear the state timer.  We set the timer when we complete
           an access attempt in Page Response or Origination Substates,
           in case a delayed response never comes. */
        (void) rex_clr_timer( &mcc_state_timer );
        (void) rex_clr_sigs( MC_TCB_PTR,  MCC_STATE_TIMER_SIG );

        access_status = NS1X_TEST_SUCCESS;
        onex_nonsig_params.ns_rev_curr_state = NS1X_SYS_ACC;
      }
      else
      {
        /* Error processing the command. Don't want to wait for access attempts to complete. */
        access_status = NS1X_TEST_NO_SUCCESS;
      }

      break;
    }

    /* ---------------------------- INVALID STATE --------------------------- */
    default:
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: Invalid REV state (%d)",
        onex_nonsig_params.ns_rev_curr_state );
      access_status = NS1X_TEST_INVALID_STATE;
      break;
    }

  } /* end of switch */

  return access_status;

} /* nonsig_access */


/*===========================================================================

FUNCTION nonsig_paging

DESCRIPTION
  This function attempts 1x sleep.

DEPENDENCIES
  None.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_sleep
(
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  onex_nonsig_cmd_status_type sleep_status = NS1X_TEST_NO_SUCCESS;

  mccsrch_rpt_type *srch_ptr;  /* Pointer to report from Searcher task */
  boolean done_sleep = FALSE;  /* Indicate if sleep is done */

  /*------------------------------------------------------------------*/

  MODEM_1X_UNUSED(cmd_ptr);

  switch( onex_nonsig_params.ns_fwd_curr_state )
  {
    /* --------------------- PAGING -> SLEEP -> PAGING ---------------------- */
    case NS1X_IDLE:
    {
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Entering Sleep");

      /* Send a command to the Search task to sleep.
      ** Treat it as an assigned slot. this
      ** might need optimization later
      */
      mcc_srch_buf.sleep.hdr.cmd = SRCH_SLEEP_F;
      mcc_srch_buf.sleep.hdr.cmd_hdr.task_ptr = NULL;
      mcc_srch_buf.sleep.hdr.cmd_hdr.done_q_ptr = NULL;
      mcc_srch_buf.sleep.cur_slot_mask = PAGE_SLOT;
      mcc_srch_buf.sleep.forced_sleep = FALSE;
      mcc_srch_cmd( &mcc_srch_buf );

      /* Search is going to sleep at this point
      ** Tell RXC to go night-night, too
      */
      if (mcc_srch_buf.sleep.hdr.status == SRCH_DONE_S)
      {
        mcc_rx_buf.sleep.hdr.command = RXC_SLEEP_F;
        mcc_rx_buf.sleep.hdr.cmd_hdr.done_q_ptr = NULL;
        mcc_rxc_cmd( &mcc_rx_buf );
      }
      else
      {
        M1X_MSG_SSID( DIAG, LEGACY_HIGH,
          "1xNonSig: Sleep failed" );
        return sleep_status;
      }

      while ( !done_sleep )
      {
        if ( (srch_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q)) != NULL )
        {
          if ( srch_ptr->hdr.rpt == SRCH_WAKEUP_R )
          {
            done_sleep = TRUE;
            M1X_MSG_SSID( DIAG, LEGACY_HIGH,
              "1xNonSig: Search completed Sleep.");
          }
          /* Keep track of the SRCH report for debugging purpose */
          mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, cdma.curr_state,
                              cdma.curr_state, MCC_NON_SLEEP, NONSIG_SLEEP );

          /* Place item back on queue and set signal if requested */
          cmd_done( &srch_ptr->hdr.rpt_hdr );
        }
        else
        {
          /* Wait for report */
          (void) nonsig_mc_wait( MCC_SRCH_Q_SIG | MCC_Q_SIG );
        }
      }

      /* Restore RXC */
      {
        M1X_MSG_SSID( DIAG, LEGACY_HIGH,
          "RXC_WAKE_F, mask=%d; slot=0x%X; pgslot=%u",
          srch_ptr->wakeup.cur_slot_mask,
          qw_lo( srch_ptr->wakeup.valid_time ),
          mcc_get_idle_pgslot() );

        /* Shift as 1x-CP is doing the same */
        qw_shift(srch_ptr->wakeup.valid_time, -2);

        mcc_rx_buf.wake.hdr.command = RXC_WAKE_F;
        mcc_rx_buf.wake.idle_pgslot = mcc_get_idle_pgslot();
        mcc_rx_buf.wake.sci_mask = mccidl_get_sci_mask();
        mcc_rx_buf.wake.hdr.cmd_hdr.done_q_ptr = NULL;
        mcc_rx_buf.wake.slot_mask = srch_ptr->wakeup.cur_slot_mask;
        mcc_rx_buf.wake.slot = qw_lo( srch_ptr->wakeup.valid_time );
        mcc_rx_buf.wake.page_match = srch_ptr->wakeup.page_match;

        mcc_rxc_cmd( &mcc_rx_buf );
      }

      /* Successfully completed SLEEP */
      sleep_status = NS1X_TEST_SUCCESS;

      break;
    }

    /* ---------------------------- INVALID STATE --------------------------- */
    default:
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: Invalid FWD state (%d)",
        onex_nonsig_params.ns_fwd_curr_state );
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: SLEEP allowed only in IDLE mode");

      /* Sleep request failed */
      sleep_status = NS1X_TEST_INVALID_STATE;
      break;
    }

  } /* end of switch */

  return sleep_status;

} /* nonsig_sleep */


/*===========================================================================

FUNCTION nonsig_send_rcpm

DESCRIPTION
  This function sends radio configuration parameters normally provided in an
  OTA RCPM to the Layer 1 tasks.

DEPENDENCIES
  None.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_send_rcpm
(
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  onex_nonsig_cmd_status_type fwd_rcpm_status = NS1X_TEST_NO_SUCCESS;

  /*------------------------------------------------------------------*/

  #ifdef FEATURE_IS2000_1X_ADV
  if(cmd_ptr != NULL)
  {
    /* Making the assumption here that the RCPM will NOT be sent to L1 when the mobile
       is not in a traffic call state. Doing otherwise is a waste since the parameters
       will be overwritten with RXC_TCI_F and TXC_TC_F commands */
    if (onex_nonsig_params.ns_fwd_curr_state == NS1X_TRAFFIC)
    {
      /* Save updated RCPM parameters. */
      onex_rcpm_default_params = cmd_ptr->rcpm_data.param;

      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Executing Radio Configuration Parameters Message" );

      /* Build and send RCP command for TXC */
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send TXC_FTM_RCP_DATA_F.");
      mcc_tx_buf.rcp.hdr.command = TXC_FTM_RCP_DATA_F;

      mcc_tx_buf.rcp.rcp_data.for_fch_ack_mask_no_rl_blanking =
        onex_rcpm_default_params.for_fch_ack_mask_no_rl_blanking;

      mcc_tx_buf.rcp.rcp_data.for_fch_ack_mask_rl_blanking =
        onex_rcpm_default_params.for_fch_ack_mask_rl_blanking;

      mcc_tx_buf.rcp.rcp_data.for_sch_ack_mask_no_rl_blanking =
        onex_rcpm_default_params.for_sch_ack_mask_no_rl_blanking;

      mcc_tx_buf.rcp.rcp_data.for_sch_ack_mask_rl_blanking =
        onex_rcpm_default_params.for_sch_ack_mask_rl_blanking;

      mcc_tx_buf.rcp.rcp_data.rev_gain_adj_acs1 =
        onex_rcpm_default_params.rev_gain_adj_acs1;

      mcc_tx_buf.rcp.rcp_data.rev_gain_adj_acs2plus =
        onex_rcpm_default_params.rev_gain_adj_acs2plus;

      mcc_tx_buf.rcp.rcp_data.power_control_mode =
        (cai_rcpm_pwr_cntl_mode_type) onex_rcpm_default_params.power_control_mode;

      mcc_tx_buf.rcp.rcp_data.power_control_step =
        (cai_pwr_cntl_step_type) onex_rcpm_default_params.power_control_step;

      mcc_tx_buf.rcp.rcp_data.rev_fch_blanking_dutycycle =
        (cai_fch_blanking_dutycycle_type) onex_rcpm_default_params.rev_fch_blanking_dutycycle;

      mcc_tx_buf.rcp.rcp_data.rev_fch_ack_mask =
        onex_rcpm_default_params.rev_fch_ack_mask;

      mcc_tx_buf.rcp.rcp_data.rev_sch_ack_mask =
        onex_rcpm_default_params.rev_sch_ack_mask;

      mcc_tx_buf.rcp.rcp_data.for_fch_blanking_dutycycle =
        (cai_fch_blanking_dutycycle_type) onex_rcpm_default_params.for_fch_blanking_dutycycle;

      mcc_tx_buf.rcp.rcp_data.for_n2m_ind =
        (cai_for_n2m_ind_type) onex_rcpm_default_params.for_n2m_ind;

      mcc_txc_cmd( &mcc_tx_buf );

      /* Build and send RCP command for RXC */
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send RXC_FTM_RCP_DATA_F.");
      mcc_rx_buf.rcp.hdr.command = RXC_FTM_RCP_DATA_F;
      mcc_rx_buf.rcp.rcp_data = mcc_tx_buf.rcp.rcp_data;
      mcc_rxc_cmd( &mcc_rx_buf );

      /* Build and send RCP command for SRCH */
      #if 0 /*TODO: RCPM message structure not created yet. */
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send SRCH_FTM_RCP_DATA_F." 0, 0, 0);
      mcc_srch_buf.rcp.hdr.cmd = SRCH_FTM_RCP_DATA_F;
      mcc_srch_buf.rcp.num_rc_params_rec = cmd_ptr->rcpm_data.param.num_rc_params_rec;

      for ( i = 0; i < cmd_ptr->param.num_rc_params_rec; i++ )
      {
        mcc_srch_buf.rcp.pilot_params.rc_params_recs[i].pilot_pn =
            onex_rcpm_default_params.rc_params_recs[i].pilot_pn;

        mcc_srch_buf.rcp.pilot_params.rc_params_recs[i].rc_pilot_params.for_fch_ccsh_interleaver_type =
            onex_rcpm_default_params.rc_params_recs[i].rc_pilot_params.for_fch_ccsh_interleaver_type;

        mcc_srch_buf.rcp.pilot_params.rc_params_recs[i].rc_pilot_params.qof_set_in_use =
            onex_rcpm_default_params.rc_params_recs[i].rc_pilot_params.qof_set_in_use;

        mcc_srch_buf.rcp.pilot_params.rc_params_recs[i].rc_pilot_params.rev_sch0_ack_bit =
            onex_rcpm_default_params.rc_params_recs[i].rc_pilot_params.rev_sch0_ack_bit;

        mcc_srch_buf.rcp.pilot_params.rc_params_recs[i].rc_pilot_params.rev_sch0_ack_ch_walsh =
            onex_rcpm_default_params.rc_params_recs[i].rc_pilot_params.rev_sch0_ack_ch_walsh;

        mcc_srch_buf.rcp.pilot_params.rc_params_recs[i].rc_pilot_params.rev_sch1_ack_bit =
            onex_rcpm_default_params.rc_params_recs[i].rc_pilot_params.rev_sch1_ack_bit;

        mcc_srch_buf.rcp.pilot_params.rc_params_recs[i].rc_pilot_params.rev_sch1_ack_ch_walsh =
            onex_rcpm_default_params.rc_params_recs[i].rc_pilot_params.rev_sch1_ack_ch_walsh;
      }

      mcc_srch_cmd( &mcc_srch_buf );
      #endif

      fwd_rcpm_status = NS1X_TEST_SUCCESS;
    }
    else
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: Invalid FWD state (%d)",
        onex_nonsig_params.ns_fwd_curr_state );
      fwd_rcpm_status = NS1X_TEST_INVALID_STATE;
    }
  }
  else
  {
    M1X_MSG_SSID( DIAG, LEGACY_FATAL,
      "1xNonSig: NULL PTR");
  }
  #else
  M1X_MSG_SSID( DIAG, LEGACY_FATAL,
    "1xA RCP Command not supported.");
  MODEM_1X_UNUSED(cmd_ptr);
  fwd_rcpm_status = NS1X_TEST_INVALID_CMD;
  #endif /* FEATURE_IS2000_1X_ADV */

  return fwd_rcpm_status;

} /* nonsig_send_rcpm */


/*===========================================================================

FUNCTION nonsig_fwd_fch

DESCRIPTION
  This function demodulates Forward Fundamental Channel.

DEPENDENCIES
  None.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_fwd_fch
(
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  onex_nonsig_cmd_status_type fwd_fch_status = NS1X_TEST_NO_SUCCESS;

  boolean exit_loop = FALSE;
  nonsig_tci_substates sub_state;
  mccrx_rpt_type *rpt_ptr;

  /*------------------------------------------------------------------*/

  if(cmd_ptr != NULL)
  {
    if ((onex_nonsig_params.ns_fwd_curr_state == NS1X_SYNC) ||
        (onex_nonsig_params.ns_fwd_curr_state == NS1X_PIL_ACQ))
    {
      /* prep for idle */
      fwd_fch_status = nonsig_paging(NULL);

      if(fwd_fch_status != NS1X_TEST_SUCCESS)
      {
        return fwd_fch_status;
      }

      /* prep SRCH for access */
      fwd_fch_status = nonsig_srch_to_access(NULL);

      if(fwd_fch_status != NS1X_TEST_SUCCESS)
      {
        return fwd_fch_status;
      }
    }

    if ((onex_nonsig_params.ns_fwd_curr_state == NS1X_IDLE) ||
        (onex_nonsig_params.ns_fwd_curr_state == NS1X_TRAFFIC))
    {
      nonsig_clear_mc_timers();

      /* Begin by verifying channels */
      sub_state  = TCI_1;

      /* Initialize primary and secondary service options to NULL
         to indicate that there is no active service option */
      cdma.so_cur_pri = CAI_SO_NULL;
      cdma.so_cur_sec = CAI_SO_NULL;

      /* Initialize the state of the Markov model to unsynchronized */
      mar_rate = (word) MAR_VAR_RATE;
      mar_init_synch();

      /* Initialize exit reason to normal */
      tc_exit = MCC_RELEASE;

      /* Initialize handoff in progress indicators to FALSE */
      mcctcho_tc_init();

      /* Initialize HDM_SEQ to indicate no Handoff Direction Message has been
         received during this call (IS-95 section 6.7.2.3.2.6 LAST_HDM_SEQ) */
      cdma.hdm_seq = 0x03;

      while( !exit_loop )
      {
        switch( sub_state )
        {
          case TCI_1:
          {
            M1X_MSG_SSID( DIAG, LEGACY_HIGH,
              "1xNonSig: Send SRCH_PARM_F.");
            mcc_send_srch_parms();

            cdma.code_chan = cmd_ptr->demod_fch.param.walsh.walsh;

            nonsig_send_srch_tc_f(TRUE, FALSE, cmd_ptr);

            /* Tell Receieve task to go to Traffic channel Initialization */
            sub_state = TCI_2;
            break;
          } /* end of case TCI_1 */

          case TCI_2:
          {
            nonsig_send_rxc_tci_f(TRUE, FALSE, cmd_ptr);

            /* REVISIT: Should be a T50m timer check here so we don't
               loop forever. */
            (void)mcc_wait( MCC_RX_RPT_Q_SIG );

            /* Check for frame reports from RXC task */
            if ((rpt_ptr = (mccrx_rpt_type*) q_get( &mcc_rx_rpt_q )) != NULL)
            {
              if (rpt_ptr->hdr.rpt == RXC_VALID_R)
              {
                /* Valid frame has been received */
                sub_state = TCI_3;
              }

              cmd_done( &rpt_ptr->hdr.rpt_hdr );
            }

            break;
          } /* TCI_2 */

          case TCI_3:
          {
            /* Set the Forward and Reverse Traffic Channel frame offsets to
               the assigned frame offset as determined by FRAME_OFFSETs. */
            M1X_MSG_SSID( DIAG, LEGACY_HIGH,
              "1xNonSig: Send TXC_FRAME_OFFSET_F.");
            mcc_tx_buf.hdr.command     = TXC_FRAME_OFFSET_F;
            mcc_tx_buf.fo.frame_offset = cmd_ptr->demod_fch.param.frame_offset;

            /* Set action_frame to zero so that command will be immediately
               carried out */
            qw_set( mcc_tx_buf.fo.action_frame, 0UL, 0UL );
            mcc_txc_cmd( &mcc_tx_buf );

            /* Send command to the Receive task to start receiving messages */
            M1X_MSG_SSID( DIAG, LEGACY_HIGH,
              "1xNonSig: Send RXC_TC_F.");
            mcc_rx_buf.tc.hdr.command = RXC_TC_F;
            mcc_rxc_cmd( &mcc_rx_buf );

            nonsig_clear_mc_timers();

            /* Clear indicators that hard handoff is in progress */
            mcctcho_tc_init();
            fwd_fch_status = NS1X_TEST_SUCCESS;
            onex_nonsig_params.ns_fwd_curr_state = NS1X_TRAFFIC;
            exit_loop = TRUE;
            break;
          } /* TCI_3 */

          default:
          {
            M1X_MSG_SSID( DIAG, LEGACY_FATAL,
              "1xNonSig: Invalid TCI substate %d",
              sub_state );
            fwd_fch_status = NS1X_TEST_NO_SUCCESS;
            exit_loop = TRUE;
          }
        } /* switch */
      } /* while */
    }
    else
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: Invalid FWD state (%d)",
        onex_nonsig_params.ns_fwd_curr_state );
      fwd_fch_status = NS1X_TEST_INVALID_STATE;
    }
  }
  else
  {
    M1X_MSG_SSID( DIAG, LEGACY_FATAL,
      "1xNonSig: NULL PTR");
    fwd_fch_status = NS1X_TEST_NO_SUCCESS;
  }

  return fwd_fch_status;

} /* nonsig_fwd_fch */


/*===========================================================================

FUNCTION nonsig_fwd_fch_dcch

DESCRIPTION
  This function demodulates simultaneously the Forward Fundamental Channel
  and the Forward Dedicated Control Channel.

DEPENDENCIES
  None.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_fwd_fch_dcch
(
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  onex_nonsig_cmd_status_type fwd_fch_dcch_status = NS1X_TEST_NO_SUCCESS;

  #ifdef FEATURE_IS2000_REL_A
  nonsig_tci_substates sub_state;
  mccrx_rpt_type *rpt_ptr;
  boolean exit_loop = FALSE;

  /*------------------------------------------------------------------*/

  if(cmd_ptr != NULL)
  {
    if ((onex_nonsig_params.ns_fwd_curr_state == NS1X_SYNC) ||
        (onex_nonsig_params.ns_fwd_curr_state == NS1X_PIL_ACQ))
    {
      fwd_fch_dcch_status = nonsig_paging(NULL);

      if(fwd_fch_dcch_status != NS1X_TEST_SUCCESS)
      {
        return fwd_fch_dcch_status;
      }
    }

    if ((onex_nonsig_params.ns_fwd_curr_state == NS1X_IDLE) ||
        (onex_nonsig_params.ns_fwd_curr_state == NS1X_TRAFFIC))
    {
      nonsig_clear_mc_timers();

      /* Begin by verifying channels */
      sub_state  = TCI_1;

      /* Initialize primary and secondary service options to NULL
       to indicate that there is no active service option */
      cdma.so_cur_pri = CAI_SO_NULL;
      cdma.so_cur_sec = CAI_SO_NULL;

      /* Initialize the state of the Markov model to unsynchronized */
      mar_rate = (word) MAR_VAR_RATE;
      mar_init_synch();

      /* Initialize exit reason to normal */
      tc_exit = MCC_RELEASE;

      /* --------------------------------------------------
      ** Initialize handoff in progress indicators to FALSE
      ** -------------------------------------------------- */
      mcctcho_tc_init();

      /* Initialize HDM_SEQ to indicate no Handoff Direction Message has been
         received during this call (IS-95 section 6.7.2.3.2.6 LAST_HDM_SEQ) */
      cdma.hdm_seq = 0x03;

      while( !exit_loop )
      {
        switch( sub_state )
        {
          case TCI_1:
          {
            M1X_MSG_SSID( DIAG, LEGACY_HIGH,
              "1xNonSig: Send SRCH_PARM_F.");
            mcc_send_srch_parms();

            /* vlechan what should cdma.code_chan be set up to for VP2? */
            cdma.code_chan = cmd_ptr->demod_fch_dcch.param.fch.walsh.walsh;

            nonsig_send_srch_tc_f(TRUE, FALSE, cmd_ptr);

            /* Tell Receieve task to go to Traffic channel Initialization */
            sub_state = TCI_2;
            break;
          } /* end of case TCI_1 */

          case TCI_2:
          {
            nonsig_send_rxc_tci_f(TRUE, TRUE, cmd_ptr);

            (void)mcc_wait( MCC_RX_RPT_Q_SIG );

            /* Check for frame reports from RXC task */

            if ((rpt_ptr = (mccrx_rpt_type*) q_get( &mcc_rx_rpt_q )) != NULL)
            {
              if (rpt_ptr->hdr.rpt == RXC_VALID_R)
              {
                /* Valid frame has been received */
                sub_state = TCI_3;
              }

              cmd_done( &rpt_ptr->hdr.rpt_hdr );
            }

            break;
          } /* TCI_2 */

          case TCI_3:
          {
            /* Set the Forward and Reverse Traffic Channel frame offsets to
               the assigned frame offset as determined by FRAME_OFFSETs. */
            M1X_MSG_SSID( DIAG, LEGACY_HIGH,
              "1xNonSig: Send TXC_FRAME_OFFSET_F.");
            mcc_tx_buf.hdr.command     = TXC_FRAME_OFFSET_F;
            mcc_tx_buf.fo.frame_offset =
              cmd_ptr->demod_fch_dcch.param.fch.frame_offset;

            /* Set action_frame to zero so that command will be immediately
               carried out */
            qw_set( mcc_tx_buf.fo.action_frame, 0UL, 0UL );
            mcc_txc_cmd( &mcc_tx_buf );

            /* Send command to the Receive task to start receiving messages */
            M1X_MSG_SSID( DIAG, LEGACY_HIGH,
              "1xNonSig: Send RXC_TC_F.");
            mcc_rx_buf.tc.hdr.command = RXC_TC_F;
            mcc_rxc_cmd( &mcc_rx_buf );

            nonsig_clear_mc_timers();

            /* Clear indicators that hard handoff is in progress */
            mcctcho_tc_init();

            fwd_fch_dcch_status = NS1X_TEST_SUCCESS;
            onex_nonsig_params.ns_fwd_curr_state = NS1X_TRAFFIC;
            exit_loop = TRUE;
            break;
          } /* TCI_3 */

          default:
          {
            M1X_MSG_SSID( DIAG, LEGACY_FATAL,
              "1xNonSig: Invalid TCI substate %d",
              sub_state );
            fwd_fch_dcch_status = NS1X_TEST_NO_SUCCESS;
            exit_loop = TRUE;
          }

        } /* switch */
      } /* while */
    }
    else
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: Invalid FWD state (%d)",
        onex_nonsig_params.ns_fwd_curr_state );
      fwd_fch_dcch_status = NS1X_TEST_INVALID_STATE;
    }
  }
  else
  {
    M1X_MSG_SSID( DIAG, LEGACY_FATAL,
      "1xNonSig: NULL PTR");
    fwd_fch_dcch_status = NS1X_TEST_NO_SUCCESS;
  }
  #else
  M1X_MSG_SSID( DIAG, LEGACY_FATAL,
    "1xNonSig: Command not supported.");
  MODEM_1X_UNUSED(cmd_ptr);
  fwd_fch_dcch_status = NS1X_TEST_INVALID_CMD;
  #endif /* FEATURE_IS2000_REL_A */

  return fwd_fch_dcch_status;

} /* nonsig_fwd_fch_dcch */


/*===========================================================================

FUNCTION nonsig_fwd_dcch

DESCRIPTION
  This function demodulates Forward Dedicated Control Channel.

DEPENDENCIES
  None.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_fwd_dcch
(
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  onex_nonsig_cmd_status_type fwd_dcch_status = NS1X_TEST_NO_SUCCESS;

  nonsig_tci_substates sub_state;
  mccrx_rpt_type *rpt_ptr;
  boolean exit_loop = FALSE;

  /*------------------------------------------------------------------*/

  if(cmd_ptr != NULL)
  {
    if ((onex_nonsig_params.ns_fwd_curr_state == NS1X_SYNC) ||
        (onex_nonsig_params.ns_fwd_curr_state == NS1X_PIL_ACQ))
    {
      fwd_dcch_status = nonsig_paging(NULL);

      if(fwd_dcch_status != NS1X_TEST_SUCCESS)
      {
        return fwd_dcch_status;
      }
    }

    if ((onex_nonsig_params.ns_fwd_curr_state == NS1X_IDLE) ||
        (onex_nonsig_params.ns_fwd_curr_state == NS1X_TRAFFIC))
    {
      nonsig_clear_mc_timers();

      /* Begin by verifying channels */
      sub_state  = TCI_1;

      /* Initialize primary and secondary service options to NULL
         to indicate that there is no active service option */
      cdma.so_cur_pri = CAI_SO_NULL;
      cdma.so_cur_sec = CAI_SO_NULL;

      /* Initialize the state of the Markov model to unsynchronized */
      mar_rate = (word) MAR_VAR_RATE;
      mar_init_synch();

      /* Initialize exit reason to normal */
      tc_exit = MCC_RELEASE;

      /* Initialize handoff in progress indicators to FALSE */
      mcctcho_tc_init();

      /* Initialize HDM_SEQ to indicate no Handoff Direction Message has been
         received during this call (IS-95 section 6.7.2.3.2.6 LAST_HDM_SEQ) */
      cdma.hdm_seq = 0x03;

      while( !exit_loop )
      {
        switch( sub_state )
        {
          case TCI_1:
          {
            M1X_MSG_SSID( DIAG, LEGACY_HIGH,
              "1xNonSig: Send SRCH_PARM_F.");
            mcc_send_srch_parms();

            cdma.code_chan = cmd_ptr->demod_dcch.param.walsh.walsh;

            nonsig_send_srch_tc_f(TRUE, FALSE, cmd_ptr);

            /* Tell Receieve task to go to Traffic channel Initialization */
            sub_state = TCI_2;
            break;
          } /* end of case TCI_1 */

          case TCI_2:
          {
            nonsig_send_rxc_tci_f(FALSE, TRUE, cmd_ptr);

            (void)mcc_wait( MCC_RX_RPT_Q_SIG );

            /* Check for frame reports from RXC task */
            if ((rpt_ptr = (mccrx_rpt_type*) q_get( &mcc_rx_rpt_q )) != NULL)
            {
              if (rpt_ptr->hdr.rpt == RXC_VALID_R)
              {
                /* Valid frame has been received */
                sub_state = TCI_3;
              }

              cmd_done( &rpt_ptr->hdr.rpt_hdr );
            }

            break;
          } /* TCI_2 */

          case TCI_3:
          {

            /* Set the Forward and Reverse Traffic Channel frame offsets to
               the assigned frame offset as determined by FRAME_OFFSETs. */
            M1X_MSG_SSID( DIAG, LEGACY_HIGH,
              "1xNonSig: Send TXC_FRAME_OFFSET_F.");
            mcc_tx_buf.hdr.command     = TXC_FRAME_OFFSET_F;
            mcc_tx_buf.fo.frame_offset = cmd_ptr->demod_dcch.param.frame_offset;

            /* Set action_frame to zero so that command will be immediately
               carried out */
            qw_set( mcc_tx_buf.fo.action_frame, 0UL, 0UL );
            mcc_txc_cmd( &mcc_tx_buf );

            /* Send command to the Receive task to start receiving messages */
            mcc_rx_buf.tc.hdr.command = RXC_TC_F;
            mcc_rxc_cmd( &mcc_rx_buf );

            nonsig_clear_mc_timers();

            /* Clear indicators that hard handoff is in progress */
            mcctcho_tc_init();

            fwd_dcch_status = NS1X_TEST_SUCCESS;
            onex_nonsig_params.ns_fwd_curr_state = NS1X_TRAFFIC;
            exit_loop = TRUE;
            break;
          } /* TCI_3 */

          default:
          {
            M1X_MSG_SSID( DIAG, LEGACY_FATAL,
              "1xNonSig: Invalid TCI substate %d",
              sub_state );
            fwd_dcch_status = NS1X_TEST_NO_SUCCESS;
            exit_loop = TRUE;
          }

        } /* switch */
      } /* while */
    }
    else
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: Invalid FWD state (%d)",
        onex_nonsig_params.ns_fwd_curr_state );
      fwd_dcch_status = NS1X_TEST_INVALID_STATE;
    }
  }
  else
  {
    M1X_MSG_SSID( DIAG, LEGACY_FATAL,
      "1xNonSig: NULL PTR");
    fwd_dcch_status = NS1X_TEST_NO_SUCCESS;
  }

  return fwd_dcch_status;

} /* nonsig_fwd_dcch */


/*===========================================================================

FUNCTION nonsig_fwd_sch

DESCRIPTION
  This function sends message to RXC & SRCH to demodulate/decode SCH.

DEPENDENCIES
  None.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_fwd_sch
(
   onex_nonsig_cmd_type  *cmd_ptr
)
{
  onex_nonsig_cmd_status_type fwd_sch_status = NS1X_TEST_NO_SUCCESS;

  /*------------------------------------------------------------------*/

  if(cmd_ptr != NULL)
  {
    if (onex_nonsig_params.ns_fwd_curr_state == NS1X_TRAFFIC)
    {
      /* Send the command to the searcher to demodulate variable rate
         spreading supplemental channel. (IS95C). */

      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send SRCH_TC_ADD_SCH_F.");
      mcc_srch_buf.tc_sch.hdr.cmd = SRCH_TC_ADD_SCH_F;
      mcc_srch_buf.tc_sch.num_sch = 1;
      mcc_srch_buf.tc_sch.srch_tc_sch_type[0].pilot =  cur_bs_ptr->pilot_pn;
      mcc_srch_buf.tc_sch.srch_tc_sch_type[0].walsh_chn =
                                     cmd_ptr->demod_sch.param.walsh.walsh;
      mcc_srch_buf.tc_sch.srch_tc_sch_type[0].qof_mask_id =
                                     cmd_ptr->demod_sch.param.walsh.qof_mask_id;
      mcc_srch_buf.tc_sch.srch_tc_sch_type[0].rc =
        (deint_rc_type)cmd_ptr->demod_sch.param.radio_cfg;
      mcc_srch_buf.tc_sch.srch_tc_sch_type[0].sch_rate =
        (cai_sch_rate_type)cmd_ptr->demod_sch.param.sch_rate;
      mcc_srch_buf.tc_sch.srch_tc_sch_type[0].code_type =
        (cai_sch_coding_type)cmd_ptr->demod_sch.param.coding_type;

      mcc_srch_cmd( &mcc_srch_buf);

      /* Send the command to the Receive task to set up the deinterleaver/
         decoder to decode supplemental channels. */

      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send RXC_SCH_F.");
      mcc_rx_buf.sch.hdr.command = RXC_SCH_F;

      /* Pass in the Radio Configuration for  SCH */
      mcc_rx_buf.sch.rc = (deint_rc_type)cmd_ptr->demod_sch.param.radio_cfg;

      /* Pass in the SCH rate / spreading rate ( 2X, 4X, 8X , 16X */
      mcc_rx_buf.sch.sch_rate =
        (dec_sch_rate_type)cmd_ptr->demod_sch.param.sch_rate;
      mcc_rx_buf.sch.double_size_pdu = TRUE;
      mcc_rx_buf.sch.code_type =
        (dec_coding_type)cmd_ptr->demod_sch.param.coding_type;
      mcc_rxc_cmd(&mcc_rx_buf);

      fwd_sch_status = NS1X_TEST_SUCCESS;
    }
    else
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: Invalid FWD state (%d)",
        onex_nonsig_params.ns_fwd_curr_state );
      return NS1X_TEST_INVALID_STATE;
    }
  }
  else
  {
    M1X_MSG_SSID( DIAG, LEGACY_FATAL,
      "1xNonSig: NULL PTR");
    fwd_sch_status = NS1X_TEST_NO_SUCCESS;
  }

  return fwd_sch_status;

} /* nonsig_fwd_sch */


/*===========================================================================

FUNCTION nonsig_rev_dcch

DESCRIPTION
  This function modulates Reverse DCCH

DEPENDENCIES
  None.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_rev_dcch
(
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  onex_nonsig_cmd_status_type rev_dcch_status = NS1X_TEST_NO_SUCCESS;

  nonsig_tci_substates sub_state;
  boolean exit_loop = FALSE;

  /*------------------------------------------------------------------*/

  if(cmd_ptr != NULL)
  {
    if ((onex_nonsig_params.ns_rev_curr_state == NS1X_INIT_CDMA) &&
        (onex_nonsig_params.ns_fwd_curr_state == NS1X_TRAFFIC))
    {
      rev_dcch_status = nonsig_access(NULL);

      if(rev_dcch_status != NS1X_TEST_SUCCESS)
      {
        return rev_dcch_status;
      }
    }

    if ((onex_nonsig_params.ns_rev_curr_state == NS1X_SYS_ACC) ||
        (onex_nonsig_params.ns_rev_curr_state == NS1X_REV_TRAFFIC))
    {
      /* Begin by verifying channels */
      sub_state  = TCI_4;

      /* Initialize exit reason to normal */
      tc_exit = MCC_RELEASE;

      /* Initialize handoff in progress indicators to FALSE */
      mcctcho_tc_init();

      /* Initialize HDM_SEQ to indicate no Handoff Direction Message has been
         received during this call (IS-95 section 6.7.2.3.2.6 LAST_HDM_SEQ) */
      cdma.hdm_seq = 0x03;

      while( !exit_loop )
      {
        switch( sub_state )
        {
          case TCI_4:
          {
            /* Set the reverse frame offset
               THIS IS ONLY VALID IF TX TASK IS IN ACCESS STATE
            */
            M1X_MSG_SSID( DIAG, LEGACY_HIGH,
              "1xNonSig: Send TXC_FRAME_OFFSET_F.");
            mcc_tx_buf.hdr.command = TXC_FRAME_OFFSET_F;
            mcc_tx_buf.fo.frame_offset = cmd_ptr->mod_dcch.param.frame_offset;

            /* Set action_frame to zero so that command will be immediately
               carried out */
            qw_set( mcc_tx_buf.fo.action_frame, 0UL, 0UL );
            mcc_txc_cmd( &mcc_tx_buf );

            /* Command TX to go to Traffic channel which will cause it to
               start sending Traffic Channel preambles */
            nonsig_send_txc_tc_f(FALSE, TRUE, cmd_ptr);

            /* Wait for TX to acquire timing & go to traffic state */
            (void) mcc_wait( (dword) MCC_TXC_DONE_SIG );
            (void) rex_clr_sigs( rex_self(), MCC_TXC_DONE_SIG );

            M1X_MSG_SSID( DIAG, LEGACY_HIGH,
              "1xNonSig: Turned on Reverse Link traffic" );

            sub_state = TCI_5;
            rev_dcch_status = NS1X_TEST_SUCCESS;
            onex_nonsig_params.ns_rev_curr_state = NS1X_REV_TRAFFIC;
            exit_loop = TRUE;
            break;
          } /* case TCI_4 */

          default:
          {
            M1X_MSG_SSID( DIAG, LEGACY_FATAL,
              "1xNonSig: Invalid TCI substate %d",
              sub_state );
            rev_dcch_status = NS1X_TEST_NO_SUCCESS;
            exit_loop = TRUE;
            break;
          }

        } /* switch */
      } /* while */
    }
    else
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: Invalid REV state (%d)",
        onex_nonsig_params.ns_rev_curr_state );
      rev_dcch_status = NS1X_TEST_INVALID_STATE;
    }
  }
  else
  {
    M1X_MSG_SSID( DIAG, LEGACY_FATAL,
      "1xNonSig: NULL PTR");
    rev_dcch_status = NS1X_TEST_NO_SUCCESS;
  }

  return rev_dcch_status;

} /* nonsig_rev_dcch */


/*===========================================================================

FUNCTION nonsig_rev_fch

DESCRIPTION
  This function modulates Reverse Fundamental Channel.

DEPENDENCIES
  None.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_rev_fch
(
  onex_nonsig_cmd_type  *cmd_ptr
    /* Pointer to the current command */
)
{
  onex_nonsig_cmd_status_type rev_fch_status = NS1X_TEST_NO_SUCCESS;

  nonsig_tci_substates sub_state;
  boolean exit_loop = FALSE;

  /*------------------------------------------------------------------*/

  if(cmd_ptr != NULL)
  {
    if ( ( onex_nonsig_params.ns_rev_curr_state == NS1X_ENTER ) ||
         ( onex_nonsig_params.ns_rev_curr_state == NS1X_INIT_CDMA ) )
    {
      rev_fch_status = nonsig_access(NULL);

      if(rev_fch_status != NS1X_TEST_SUCCESS)
      {
        return rev_fch_status;
      }
    }

    if ((onex_nonsig_params.ns_rev_curr_state == NS1X_SYS_ACC) ||
        (onex_nonsig_params.ns_rev_curr_state == NS1X_REV_TRAFFIC))
    {
      /* Begin by verifying channels */
      sub_state  = TCI_4;

      /* Initialize exit reason to normal */
      tc_exit = MCC_RELEASE;

      /* Initialize handoff in progress indicators to FALSE */
      mcctcho_tc_init();

      /* Initialize HDM_SEQ to indicate no Handoff Direction Message has been
         received during this call (IS-95 section 6.7.2.3.2.6 LAST_HDM_SEQ) */
      cdma.hdm_seq = 0x03;

      while( !exit_loop )
      {
        switch( sub_state )
        {
          case TCI_4:
          {
            #if 0 /* REVISIT */
            /* Set the reverse frame offset
               ONLY VALID WHILE TX TASK IS IN ACCESS STATE
            */
            M1X_MSG_SSID( DIAG, LEGACY_HIGH,
              "1xNonSig: Send TXC_FRAME_OFFSET_F.");
            mcc_tx_buf.hdr.command = TXC_FRAME_OFFSET_F;
            mcc_tx_buf.fo.frame_offset = cmd_ptr->mod_fch.param.frame_offset;

            /* Set action_frame to zero so that command will be immediately
               carried out */
            qw_set( mcc_tx_buf.fo.action_frame, 0UL, 0UL );
            mcc_txc_cmd( &mcc_tx_buf );
            #endif

            /* Command TX to go to Traffic channel which will cause it to
               start sending Traffic Channel preambles */
            nonsig_send_txc_tc_f(TRUE, FALSE, cmd_ptr);

            /* Wait for TX to acquire timing & go to traffic state */
            (void) mcc_wait( (dword) MCC_TXC_DONE_SIG );
            (void) rex_clr_sigs( rex_self(), MCC_TXC_DONE_SIG );

            M1X_MSG_SSID( DIAG, LEGACY_HIGH,
              "1xNonSig: Turned on Reverse Link traffic" );

            sub_state = TCI_5;
            rev_fch_status = NS1X_TEST_SUCCESS;
            onex_nonsig_params.ns_rev_curr_state = NS1X_REV_TRAFFIC;
            exit_loop = TRUE;
            break;
          } /* case TCI_4 */

          default:
          {
            M1X_MSG_SSID( DIAG, LEGACY_FATAL,
              "1xNonSig: Invalid TCI substate %d",
              sub_state );
            rev_fch_status = NS1X_TEST_NO_SUCCESS;
            exit_loop = TRUE;
          }

        } /* switch */
      } /* while */
    }
    else
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: Invalid REV state (%d)",
        onex_nonsig_params.ns_rev_curr_state );
      rev_fch_status = NS1X_TEST_INVALID_STATE;
    }
  }
  else
  {
    M1X_MSG_SSID( DIAG, LEGACY_FATAL,
      "1xNonSig: NULL PTR");
    rev_fch_status = NS1X_TEST_NO_SUCCESS;
  }

  return rev_fch_status;

} /* nonsig_rev_fch */


/*===========================================================================

FUNCTION nonsig_rev_fch_dcch

DESCRIPTION
  This function modulates Reverse FCH and DCCH

DEPENDENCIES
  None.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_rev_fch_dcch
(
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  onex_nonsig_cmd_status_type rev_fch_dcch_status = NS1X_TEST_NO_SUCCESS;

  #ifdef FEATURE_IS2000_REL_A
  nonsig_tci_substates sub_state;
  boolean exit_loop = FALSE;

  /*------------------------------------------------------------------*/

  if(cmd_ptr != NULL)
  {
    if ((onex_nonsig_params.ns_rev_curr_state == NS1X_INIT_CDMA) &&
        (onex_nonsig_params.ns_fwd_curr_state == NS1X_TRAFFIC))
    {
      rev_fch_dcch_status = nonsig_access(NULL);

      if(rev_fch_dcch_status != NS1X_TEST_SUCCESS)
      {
        return rev_fch_dcch_status;
      }
    }

    if ((onex_nonsig_params.ns_rev_curr_state == NS1X_SYS_ACC) ||
        (onex_nonsig_params.ns_rev_curr_state == NS1X_REV_TRAFFIC))
    {
      /* Begin by verifying channels */
      sub_state  = TCI_4;

      /* Initialize exit reason to normal */
      tc_exit = MCC_RELEASE;

      /* Initialize handoff in progress indicators to FALSE */
      mcctcho_tc_init();

      /*Initialize HDM_SEQ to indicate no Handoff Direction Message has been
        received during this call (IS-95 section 6.7.2.3.2.6 LAST_HDM_SEQ) */
      cdma.hdm_seq = 0x03;

      while( !exit_loop )
      {
        switch( sub_state )
        {
          case TCI_4:
          {
            /* Set the reverse frame offset
               THIS IS ONLY VALID IF TX TASK IS IN ACCESS STATE
            */
            M1X_MSG_SSID( DIAG, LEGACY_HIGH,
              "1xNonSig: Send TXC_FRAME_OFFSET_F.");
            mcc_tx_buf.hdr.command = TXC_FRAME_OFFSET_F;
            mcc_tx_buf.fo.frame_offset =
              cmd_ptr->mod_fch_dcch.param.fch.frame_offset;

            /* Set action_frame to zero so that command will be immediately
               carried out */
            qw_set( mcc_tx_buf.fo.action_frame, 0UL, 0UL );
            mcc_txc_cmd( &mcc_tx_buf );

            /* Command TX to go to Traffic channel which will cause it to
               start sending Traffic Channel preambles */
            nonsig_send_txc_tc_f(TRUE, TRUE, cmd_ptr);

            /* Wait for TX to acquire timing & go to traffic state */
            (void) mcc_wait( (dword) MCC_TXC_DONE_SIG );
            (void) rex_clr_sigs( rex_self(), MCC_TXC_DONE_SIG );

            M1X_MSG_SSID( DIAG, LEGACY_HIGH,
              "1xNonSig: Turned on Reverse Link traffic" );

            sub_state = TCI_5;
            rev_fch_dcch_status = NS1X_TEST_SUCCESS;
            onex_nonsig_params.ns_rev_curr_state = NS1X_REV_TRAFFIC;
            exit_loop = TRUE;
            break;
          } /* case TCI_4 */

          default:
          {
            M1X_MSG_SSID( DIAG, LEGACY_FATAL,
              "1xNonSig: Invalid TCI substate %d",
              sub_state );
            rev_fch_dcch_status = NS1X_TEST_NO_SUCCESS;
            exit_loop = TRUE;
          }
        } /* switch */
      } /* while */
    }
    else
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: Invalid REV state (%d)",
        onex_nonsig_params.ns_rev_curr_state );
      rev_fch_dcch_status = NS1X_TEST_INVALID_STATE;
    }
  }
  else
  {
    M1X_MSG_SSID( DIAG, LEGACY_FATAL,
      "1xNonSig: NULL PTR");
    rev_fch_dcch_status = NS1X_TEST_NO_SUCCESS;
  }
  #else
  M1X_MSG_SSID( DIAG, LEGACY_FATAL,
    "1xNonSig: Command not supported.");
  MODEM_1X_UNUSED(cmd_ptr);
  rev_fch_dcch_status = NS1X_TEST_INVALID_CMD;
  #endif /* FEATURE_IS2000_REL_A */

  return rev_fch_dcch_status;

} /* nonsig_rev_fch_dcch */


/*===========================================================================

FUNCTION nonsig_rev_sch

DESCRIPTION
  This function modulates Reverse Fundamental Channel.

DEPENDENCIES
  None.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_rev_sch
(
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  onex_nonsig_cmd_status_type rev_sch_status = NS1X_TEST_NO_SUCCESS;

  /*------------------------------------------------------------------*/

  if(cmd_ptr != NULL)
  {
    if (onex_nonsig_params.ns_rev_curr_state == NS1X_REV_TRAFFIC)
    {
      tc_exit = MCC_RELEASE;

      /* Initialize HDM_SEQ to indicate no Handoff Direction Message has been
         received during this call (IS-95 section 6.7.2.3.2.6 LAST_HDM_SEQ) */
      cdma.hdm_seq = 0x03;

      /* Command TX to go to Traffic channel which will cause it to
         start sending Traffic Channel preambles */

      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send TXC_FTM_SCH_F.");
      mcc_tx_buf.ftm_sch.hdr.command = TXC_FTM_SCH_F;
      mcc_tx_buf.ftm_sch.radio_cfg = (cai_radio_config_type)
                                    cmd_ptr->mod_sch.param.radio_cfg;

      mcc_tx_buf.ftm_sch.sch_rate = (enc_sch_rate_type)
                                  cmd_ptr->mod_sch.param.sch_rate;
      mcc_tx_buf.ftm_sch.sch_on = TRUE;
      mcc_tx_buf.ftm_sch.sch_turbo =
           ( (onex_nonsig_sch_coding_type)cmd_ptr->mod_sch.param.coding_type ==
             NS1X_DECODE_TURBO )? TRUE : FALSE;
      mcc_tx_buf.ftm_sch.tx_pattern = (dword)cmd_ptr->mod_sch.param.tx_pattern;

      (void) rex_clr_sigs( rex_self(), MCC_TXC_DONE_SIG );

      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: R-SCH - RC: %d, Rate: %d, Turbo: %d",
        mcc_tx_buf.ftm_sch.radio_cfg,
        mcc_tx_buf.ftm_sch.sch_rate,
        mcc_tx_buf.ftm_sch.sch_turbo );

      mcc_txc_cmd( &mcc_tx_buf );

      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Turned on Reverse SCH traffic" );

      rev_sch_status = NS1X_TEST_SUCCESS;
    }
    else
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: Invalid REV state (%d)",
        onex_nonsig_params.ns_rev_curr_state );
      rev_sch_status = NS1X_TEST_INVALID_STATE;
    }
  }
  else
  {
    M1X_MSG_SSID( DIAG, LEGACY_FATAL,
      "1xNonSig: NULL PTR");
    rev_sch_status = NS1X_TEST_NO_SUCCESS;
  }

  return rev_sch_status;

} /* nonsig_rev_sch */


/*===========================================================================

FUNCTION nonsig_fch_loopback

DESCRIPTION
  This function modulates Reverse Fundamental Channel.

DEPENDENCIES
  None.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_fch_loopback
(
  boolean lb_enable
    /* enable loopback */
)
{
  onex_nonsig_cmd_status_type fch_lb_status = NS1X_TEST_NO_SUCCESS;

  /*------------------------------------------------------------------*/

  if ( (onex_nonsig_params.ns_rev_curr_state == NS1X_REV_TRAFFIC) &&
       (onex_nonsig_params.ns_fwd_curr_state == NS1X_TRAFFIC) )
  {
    /* inform RXC of loopback status */
    M1X_MSG_SSID( DIAG, LEGACY_HIGH,
      "1xNonSig: Send RXC_FTM_FCH_LOOPBACK.");
    mcc_rx_buf.loopback.hdr.command = RXC_FTM_FCH_LOOPBACK;
    mcc_rx_buf.loopback.loopback_on = lb_enable;
    mcc_rxc_cmd( &mcc_rx_buf );

    /* inform TXC of loopback status */
    M1X_MSG_SSID( DIAG, LEGACY_HIGH,
      "1xNonSig: Send TXC_FTM_FCH_LOOPBACK.");
    mcc_tx_buf.loopback.hdr.command = TXC_FTM_FCH_LOOPBACK;
    mcc_tx_buf.loopback.loopback_on = lb_enable;
    (void)rex_clr_sigs( rex_self(), MCC_TXC_DONE_SIG );
    mcc_txc_cmd( &mcc_tx_buf );
    fch_lb_status = NS1X_TEST_SUCCESS;
  }
  else
  {
    M1X_MSG_SSID( DIAG, LEGACY_ERROR,
      "1xNonSig: Invalid states: FWD(%d), REV(%d)",
      onex_nonsig_params.ns_fwd_curr_state,
      onex_nonsig_params.ns_rev_curr_state );
    fch_lb_status = NS1X_TEST_INVALID_STATE;
  }

  return fch_lb_status;

} /* nonsig_fch_loopback */


/*===========================================================================

FUNCTION nonsig_dcch_loopback

DESCRIPTION
  This function modulates Dedicated Control Channel.

DEPENDENCIES
  None.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_dcch_loopback
(
  boolean lb_enable
    /* enable loopback */
)
{
  onex_nonsig_cmd_status_type dcch_lb_status = NS1X_TEST_NO_SUCCESS;

  /*------------------------------------------------------------------*/

  if ( (onex_nonsig_params.ns_rev_curr_state == NS1X_REV_TRAFFIC) &&
       (onex_nonsig_params.ns_fwd_curr_state == NS1X_TRAFFIC) )
  {
    /* inform RXC of loopback status */
    M1X_MSG_SSID( DIAG, LEGACY_HIGH,
      "1xNonSig: Send RXC_FTM_DCCH_LOOPBACK.");
    mcc_rx_buf.loopback.hdr.command = RXC_FTM_DCCH_LOOPBACK;
    mcc_rx_buf.loopback.loopback_on = lb_enable;
    mcc_rxc_cmd( &mcc_rx_buf );

    /* inform TXC of loopback status */
    M1X_MSG_SSID( DIAG, LEGACY_HIGH,
      "1xNonSig: Send TXC_FTM_DCCH_LOOPBACK.");
    mcc_tx_buf.loopback.hdr.command = TXC_FTM_DCCH_LOOPBACK;
    mcc_tx_buf.loopback.loopback_on = lb_enable;
    (void)rex_clr_sigs( rex_self(), MCC_TXC_DONE_SIG );
    mcc_txc_cmd( &mcc_tx_buf );
    dcch_lb_status = NS1X_TEST_SUCCESS;
  }
  else
  {
    M1X_MSG_SSID( DIAG, LEGACY_ERROR,
      "1xNonSig: Invalid states: FWD(%d), REV(%d)",
      onex_nonsig_params.ns_fwd_curr_state,
      onex_nonsig_params.ns_rev_curr_state );
    dcch_lb_status = NS1X_TEST_INVALID_STATE;
  }

  return dcch_lb_status;

} /* nonsig_dcch_loopback */


/*===========================================================================

FUNCTION nonsig_sch_loopback

DESCRIPTION
  This function modulates Reverse Fundamental Channel.

DEPENDENCIES
  None.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_sch_loopback
(
  boolean lb_enable              /* enable loopback */
)
{
  onex_nonsig_cmd_status_type sch_lb_status = NS1X_TEST_NO_SUCCESS;

  /*------------------------------------------------------------------*/

  if ( (onex_nonsig_params.ns_rev_curr_state == NS1X_REV_TRAFFIC) &&
       (onex_nonsig_params.ns_fwd_curr_state == NS1X_TRAFFIC) )
  {
    /* inform RXC of loopback status */
    M1X_MSG_SSID( DIAG, LEGACY_HIGH,
      "1xNonSig: Send RXC_FTM_SCH_LOOPBACK.");
    mcc_rx_buf.loopback.hdr.command = RXC_FTM_SCH_LOOPBACK;
    mcc_rx_buf.loopback.loopback_on = lb_enable;
    mcc_rxc_cmd( &mcc_rx_buf );

    /* inform TXC of loopback status */
    M1X_MSG_SSID( DIAG, LEGACY_HIGH,
      "1xNonSig: Send TXC_FTM_SCH_LOOPBACK.");
    mcc_tx_buf.loopback.hdr.command = TXC_FTM_SCH_LOOPBACK;
    mcc_tx_buf.loopback.loopback_on = lb_enable;
    (void)rex_clr_sigs( rex_self(), MCC_TXC_DONE_SIG );
    mcc_txc_cmd( &mcc_tx_buf );
    sch_lb_status = NS1X_TEST_SUCCESS;
  }
  else
  {
    M1X_MSG_SSID( DIAG, LEGACY_ERROR,
      "1xNonSig: Invalid states: FWD(%d), REV(%d)",
      onex_nonsig_params.ns_fwd_curr_state,
      onex_nonsig_params.ns_rev_curr_state );
    sch_lb_status = NS1X_TEST_INVALID_STATE;
  }

  return sch_lb_status;

} /* nonsig_sch_loopback */


/*===========================================================================

FUNCTION nonsig_release

DESCRIPTION
  This function releases active channels.

DEPENDENCIES
  None.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_release
(
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  onex_nonsig_cmd_status_type release_status = NS1X_TEST_NO_SUCCESS;

  /*------------------------------------------------------------------*/

  MODEM_1X_UNUSED(cmd_ptr);

  if((onex_nonsig_params.ns_fwd_curr_state == NS1X_TRAFFIC)    ||
     (onex_nonsig_params.ns_rev_curr_state == NS1X_REV_TRAFFIC) )
  {
    if(onex_nonsig_params.ns_fwd_curr_state == NS1X_TRAFFIC)
    {
      /* Issue RXC a RELEASE Command to release resources in TC state */
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send RXC_RELEASE_F.");
      mcc_rx_buf.rel.hdr.command = RXC_RELEASE_F;
      mcc_rxc_cmd( &mcc_rx_buf );
    }

    if(onex_nonsig_params.ns_rev_curr_state == NS1X_REV_TRAFFIC)
    {
      /* Issue TXC a RELEASE Command to release resources in TC state */
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Send TXC_RELEASE_F.");
      mcc_tx_buf.rel.hdr.command = TXC_RELEASE_F;
      mcc_txc_cmd( &mcc_tx_buf );
    }

    nonsig_init_cdma();

    release_status = NS1X_TEST_SUCCESS;
  }
  else if((onex_nonsig_params.ns_fwd_curr_state == NS1X_PIL_ACQ) ||
          (onex_nonsig_params.ns_fwd_curr_state == NS1X_SYNC)    ||
          (onex_nonsig_params.ns_fwd_curr_state == NS1X_IDLE)    ||
          (onex_nonsig_params.ns_fwd_curr_state == NS1X_SYS_ACC))
  {
    /* Send tasks back to initial state */
    M1X_MSG_SSID( DIAG, LEGACY_HIGH,
      "1xNonSig: Release." );
    nonsig_init_cdma();

    release_status = NS1X_TEST_SUCCESS;
  }
  else
  {
    M1X_MSG_SSID( DIAG, LEGACY_ERROR,
      "1xNonSig: Invalid states: FWD(%d), REV(%d)",
      onex_nonsig_params.ns_fwd_curr_state,
      onex_nonsig_params.ns_rev_curr_state );

    release_status = NS1X_TEST_INVALID_STATE;
  }

  return release_status;

} /* nonsig_release */


/*===========================================================================

FUNCTION nonsig_fwd_hho

DESCRIPTION
  This function attempts a hard handoff

DEPENDENCIES
  None.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_fwd_hho
(
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  onex_nonsig_cmd_status_type fwd_hho_status = NS1X_TEST_NO_SUCCESS;
  uint32 tx_frame_offset = 0;

  /*------------------------------------------------------------------*/

  if( cmd_ptr != NULL )
  {
    if( ( onex_nonsig_params.ns_fwd_curr_state == NS1X_TRAFFIC ) ||
        ( onex_nonsig_params.ns_rev_curr_state == NS1X_REV_TRAFFIC ) )
    {
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
         "1xNonSig: Hard Handoff SC" );

      /* Replacing bio with DAL equivalent*/
      switches_dal_attach_1x_ftm =
        DAL_DeviceAttach(DALDEVICEID_SOFTWARESWITCHES,&switches_handle_1x_ftm);
      if( switches_handle_1x_ftm == NULL )
      {
       return NS1X_TEST_NO_SUCCESS;
      }
      if( switches_dal_attach_1x_ftm == DAL_SUCCESS )
      {
       (void)DalDevice_Open( switches_handle_1x_ftm, DAL_OPEN_SHARED );
      }

      if (cmd_ptr->fwd_hho.param.enable_rpc == TRUE)
      {
       /* clear the 'disable closed loop power control' dip switch */
       (void)DalSoftwareSwitches_ClearSwitch( switches_handle_1x_ftm,
                                             (uint32) DAL_SOFTWARE_SWITCH_5 );
      }
      else
      {
       /* set the 'disable closed loop power control' dip switch */
       (void)DalSoftwareSwitches_SetSwitch( switches_handle_1x_ftm,
                                           (uint32) DAL_SOFTWARE_SWITCH_5 );
      }

      /* Send frame config to notify FW that RF config is being changed.
         FW needs a RF OFF to ON transition in order to refresh HW.
         Save the current frame cnt and compare with the running frame cnt
         until a frame boundary has occured indicating that the new frame
         config has taken effect. */
      enc_tx_enable( FALSE );

      /* frame config takes effect at the end of the frame. So delay for
         the remainder of the frame and wait 1 additional frame.
         Add extra 100us margin.
         Delay = 20ms + 1.25ms * (16 - current pcg) + 0.1ms
         This ensures the frame config will complete if the txc_traffic_isr
         has already fired in this frame and the frame config will not be sent
         until the end of the next frame */
      m1x_stmr_get_frame_and_pcg_num_with_frame_offset( NULL, &tx_frame_offset );

      rex_timed_wait( ONEX_NONSIG_TMO_SIG, &onex_nonsig_timer,
                      20 + ( 1250 * ( 16 - tx_frame_offset ) / 1000 ) + 1 );

      (void) rex_clr_sigs ( MC_TCB_PTR, ONEX_NONSIG_MC_CMD_SRCH_COMPLETE );

      srch_ftm_rx_tune_w_cb( (srch_rx_band_type)cmd_ptr->fwd_hho.param.band_class,
                             (srch_rx_chan_type)cmd_ptr->fwd_hho.param.cdma_freq,
                              nonsig_fwd_hho_cb );

      (void)nonsig_mc_wait( ONEX_NONSIG_MC_CMD_SRCH_COMPLETE );

      enc_tx_enable(TRUE);

      fwd_hho_status = NS1X_TEST_SUCCESS;
    }
    else
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: Invalid states: FWD(%d), REV(%d)",
        onex_nonsig_params.ns_fwd_curr_state,
        onex_nonsig_params.ns_rev_curr_state );

      fwd_hho_status = NS1X_TEST_INVALID_STATE;
    }
  }
  else
  {
    M1X_MSG_SSID( DIAG, LEGACY_FATAL,
      "1xNonSig: NULL PTR" );
    fwd_hho_status = NS1X_TEST_NO_SUCCESS;
  }

  return fwd_hho_status;

} /* nonsig_fwd_hho */

/*===========================================================================

FUNCTION nonsig_txagc_set_power

DESCRIPTION
  This function sets the Tx power to a particular dBm value.

DEPENDENCIES
  None.

RETURN VALUE
  Test command status.

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_txagc_set_power
(
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  #ifdef FEATURE_MODEM_1X_SUPPORTS_RF
  rfm_device_enum_type tx_device = txc_get_tx_device();
  #endif /* FEATURE_MODEM_1X_SUPPORTS_RF */

  onex_nonsig_cmd_status_type txagc_set_pwr_status = NS1X_TEST_NO_SUCCESS;

  /*------------------------------------------------------------------*/

  if(cmd_ptr != NULL)
  {
    if (onex_nonsig_params.ns_rev_curr_state == NS1X_REV_TRAFFIC)
    {
      #ifdef FEATURE_MODEM_1X_SUPPORTS_RF
      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: TX AGC Set Power, override_enable %d, tx_power %d",
        cmd_ptr->rev_link_pwr_setting.param.override_enable,
        cmd_ptr->rev_link_pwr_setting.param.tx_power_10th_dbm );

        if( cmd_ptr->rev_link_pwr_setting.param.override_enable )
        {
          rfm_1x_set_tx_closed_loop_power( tx_device,
                                          RFM_1X_TX_CL_OVERRIDE,
                                       0 );
          rfm_1x_set_tx_open_loop_power( tx_device,
                                    RFM_1X_TX_OL_OVERRIDE,
                                    cmd_ptr->rev_link_pwr_setting.param.tx_power_10th_dbm );
        }
        else
        {
          rfm_1x_set_tx_closed_loop_power( tx_device,
                                           RFM_1X_TX_CL_NORMAL,
                                           0 );
          rfm_1x_set_tx_open_loop_power( tx_device,
                                    RFM_1X_TX_OL_NORMAL,
                                    cmd_ptr->rev_link_pwr_setting.param.tx_power_10th_dbm );
        }

      txagc_set_pwr_status = NS1X_TEST_SUCCESS;
      #else
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: NO RF Support for TxAGC Set Power" );
      #endif /* FEATURE_MODEM_1X_SUPPORTS_RF */
    }
    else
    {
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: Invalid state: REV(%d)",
        onex_nonsig_params.ns_rev_curr_state );
      txagc_set_pwr_status = NS1X_TEST_INVALID_STATE;
    }
  }
  else
  {
    M1X_MSG_SSID( DIAG, LEGACY_FATAL,
      "1xNonSig: NULL PTR");
    txagc_set_pwr_status = NS1X_TEST_NO_SUCCESS;
  }

  return txagc_set_pwr_status;

} /* nonsig_txagc_set_power */


/*===========================================================================

FUNCTION nonsig_high_level_state_cmd_proc

DESCRIPTION
  This function processes high-level 1X NS state commands and calls
  the appropriate function that sends commands to low-level tasks.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
static onex_nonsig_cmd_status_type nonsig_high_level_state_cmd_proc
(
  onex_nonsig_cmd_type  *cmd_ptr
)
{
  onex_nonsig_cmd_status_type response_status = NS1X_TEST_NO_SUCCESS;

  /*------------------------------------------------------------------*/

  switch (cmd_ptr->hdr.cmd)
  {
    case NS1X_CMD_PILOT_ACQ:
      response_status = nonsig_pil_acq(cmd_ptr);
      break;

    case NS1X_CMD_RELEASE:
      response_status = nonsig_release(cmd_ptr);
      break;

    case NS1X_CMD_DEMOD_SYNC:
      response_status = nonsig_mc_sync(cmd_ptr);
      break;

    case NS1X_CMD_DEMOD_PAGING:
      response_status = nonsig_paging(cmd_ptr);
      break;

    case NS1X_CMD_ACCESS:
      response_status = nonsig_access(cmd_ptr);
      break;

    case NS1X_CMD_SLEEP:
      response_status = nonsig_sleep(cmd_ptr);
      break;

    case NS1X_CMD_1XA_RC_PARAM:
      response_status = nonsig_send_rcpm(cmd_ptr);
      break;

    case  NS1X_CMD_FWD_HHO:
      response_status = nonsig_fwd_hho(cmd_ptr);
      break;

    case NS1X_CMD_DEMOD_FCH:
      response_status = nonsig_fwd_fch(cmd_ptr);
      break;

    case NS1X_CMD_DEMOD_FDCCH:
      response_status = nonsig_fwd_dcch(cmd_ptr);
      break;

    case NS1X_CMD_DEMOD_FFCH_FDCCH:
      response_status = nonsig_fwd_fch_dcch(cmd_ptr);
      break;

    case NS1X_CMD_MOD_DCCH:
      response_status = nonsig_rev_dcch(cmd_ptr);
      break;

    case NS1X_CMD_MOD_FCH_DCCH:
      response_status = nonsig_rev_fch_dcch(cmd_ptr);
      break;

    case NS1X_CMD_DCCH_LOOPBACK:
      response_status = nonsig_dcch_loopback(TRUE);
      break;

    case NS1X_CMD_DEMOD_FSCH:
      response_status = nonsig_fwd_sch(cmd_ptr);
      break;

    case NS1X_CMD_MOD_FCH:
      response_status = nonsig_rev_fch(cmd_ptr);
      break;

    case NS1X_CMD_SET_REV_LNK_PWR:
      response_status = nonsig_txagc_set_power(cmd_ptr);
      break;

    case NS1X_CMD_MOD_SCH:
      response_status = nonsig_rev_sch(cmd_ptr);
      break;

    case NS1X_CMD_FCH_LOOPBACK:
      response_status = nonsig_fch_loopback(TRUE);
      break;

    case NS1X_CMD_SCH_LOOPBACK:
      response_status = nonsig_sch_loopback(TRUE);
      break;

    default:
       M1X_MSG_SSID( DIAG, LEGACY_FATAL,
         "1xNonSig: Unknown cmd %d",
         cmd_ptr->hdr.cmd);
       response_status = NS1X_TEST_INVALID_CMD;
  }

  return response_status;

} /* nonsig_high_level_state_cmd_proc */


/*===========================================================================

                    EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION onex_nonsig_check_cmd_queue

DESCRIPTION
   This function checks the onex_ns_cmd_q for any pending commands. It then
   calls the appropriate function to handle the command. Once the command
   processing has completed, the command status is updated and the command
   is placed on the requested queue and the requested signal is set for the
   client task.

   NOTE: Runs in MC task context.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void onex_nonsig_check_cmd_queue(void)
{
  onex_nonsig_cmd_type *cmd_ptr;
  rex_sigs_type sigs;
  rex_tcb_type *tcb_ptr;

  /*------------------------------------------------------------------*/

  /* Clear the new-command signal */
  (void) rex_clr_sigs ( MC_TCB_PTR, ONEX_NONSIG_MC_CMD_Q_SIG );

  if ((cmd_ptr = (onex_nonsig_cmd_type *) q_get(&onex_ns_cmd_q)) != NULL)
  {
    cmd_ptr->hdr.status = nonsig_high_level_state_cmd_proc( cmd_ptr );

    M1X_MSG_SSID( DIAG, LEGACY_HIGH,
      "1xNonSig: States: FWD=%d, REV=%d, Status=%d",
      onex_nonsig_params.ns_fwd_curr_state,
      onex_nonsig_params.ns_rev_curr_state,
      cmd_ptr->hdr.status );

    /* Save task pointer and signals for using after the q_put() */
    tcb_ptr = cmd_ptr->hdr.cmd_hdr.task_ptr;
    sigs    = cmd_ptr->hdr.cmd_hdr.sigs;

    /* Check to see if the command wants to be put on a queue when finished */
    if ( cmd_ptr->hdr.cmd_hdr.done_q_ptr != NULL )
    {
      /* Indeed it does. Do as the command asks */
      q_put ( cmd_ptr->hdr.cmd_hdr.done_q_ptr, &cmd_ptr->hdr.cmd_hdr.link );
    }

    /* Check to see if some task wants to be signalled when command is done. */
    if ( tcb_ptr != NULL )
    {
      /* Someone does. Notify the task requesting signaling */
      (void) rex_set_sigs ( tcb_ptr, sigs );
    }
  }

} /* onex_nonsig_check_cmd_queue */


/*============================================================================

FUNCTION      onex_nonsig_cmd_dispatcher

DESCRIPTION
    This function will call the appropriate subcommand for a given 1x NS command.

DEPENDENCIES

RETURN VALUE
    The result of the executed command (pass/fail/etc).

SIDE EFFECTS
    A command will be placed on the FTM_MCC command queue

============================================================================*/
onex_nonsig_cmd_status_type onex_nonsig_cmd_dispatcher
(
  byte sub_cmd_code,
  word length,
  onex_nonsig_packed_param_type *param
)
{
  onex_nonsig_cmd_status_type status = NS1X_TEST_NO_SUCCESS;

  /*------------------------------------------------------------------*/

  if(length > sizeof(onex_nonsig_packed_param_type))
  {
    M1X_MSG_SSID( DIAG, LEGACY_ERROR,
      "1xNonSig: Length %d exceeds the max",
      length);
    status = NS1X_TEST_INVALID_CMD;
  }
  else
  {
    status = onex_nonsig_queue_cmd(param, sub_cmd_code);
  }

  return status;

} /* onex_nonsig_cmd_dispatcher */


/*===========================================================================

FUNCTION onex_nonsig_queue_cmd

DESCRIPTION
   This function handles requests to run tests and other primitives
   by translating the FTM sub-command type to the 1x NS type and
   queuing the determined command for use by 1x. The result of the
   command processing is then returned once 1x completes processing.

   NOTE: Runs in FTM Task context.

DEPENDENCIES
   None.

RETURN VALUE
   test command status.

SIDE EFFECTS
   None.

===========================================================================*/
onex_nonsig_cmd_status_type onex_nonsig_queue_cmd
(
  onex_nonsig_packed_param_type *param,
  onex_ftm_subcmd_type          command
)
{
  onex_nonsig_cmd_status_type response_status;
  onex_nonsig_cmd_type        *onex_ns_cmd_ptr;

  /*------------------------------------------------------------------*/

  if(param != NULL)
  {
    /* Get a command buffer */
    if ( ( onex_ns_cmd_ptr =
             ( onex_nonsig_cmd_type *) q_get( &onex_ns_cmd_free_q ) ) ==
          NULL )
    {
      M1X_MSG_SSID( DIAG, LEGACY_FATAL,
        "1xNonSig: No buffers on ftm_mc_cmd_free_q" );
      response_status = NS1X_TEST_NO_SUCCESS;
    }
    else  /* A command buffer is available */
    {
      switch (command)
      {
        case ONEX_FTM_ACQUIRE_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_PILOT_ACQ;
          onex_ns_cmd_ptr->pilot_acq.param = param->acq;
          break;

        case ONEX_FTM_SYNC_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_DEMOD_SYNC;
          break;

        case ONEX_FTM_FWD_FCH_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_DEMOD_FCH;
          onex_ns_cmd_ptr->demod_fch.param = param->f_fch;
          break;

        case ONEX_FTM_FWD_DCCH_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_DEMOD_FDCCH;
          onex_ns_cmd_ptr->demod_dcch.param = param->f_dcch;
          break;

        case ONEX_FTM_FWD_SCH_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_DEMOD_FSCH;
          onex_ns_cmd_ptr->demod_sch.param = param->f_sch;
          break;

        case ONEX_FTM_REV_FCH_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_MOD_FCH;
          onex_ns_cmd_ptr->mod_fch.param = param->r_fch;
          break;

        case ONEX_FTM_REV_DCCH_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_MOD_DCCH;
          onex_ns_cmd_ptr->mod_dcch.param = param->r_dcch;
          break;

        case ONEX_FTM_REV_SCH_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_MOD_SCH;
          onex_ns_cmd_ptr->mod_sch.param = param->r_sch;
          break;

        case ONEX_FTM_FCH_LOOP_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_FCH_LOOPBACK;
          break;

        case ONEX_FTM_DCCH_LOOP_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_DCCH_LOOPBACK;
          break;

        case ONEX_FTM_SCH_LOOP_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_SCH_LOOPBACK;
          break;

        case ONEX_FTM_RELEASE_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_RELEASE;
          break;

        case ONEX_FTM_FWD_HHO_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_FWD_HHO;
          onex_ns_cmd_ptr->fwd_hho.param = param->f_hho;
          break;

        case ONEX_FTM_FWD_FCH_DCCH_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_DEMOD_FFCH_FDCCH;
          onex_ns_cmd_ptr->demod_fch_dcch.param = param->f_fch_dcch;
          break;

        case ONEX_FTM_REV_FCH_DCCH_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_MOD_FCH_DCCH;
          onex_ns_cmd_ptr->mod_fch_dcch.param = param->r_fch_dcch;
          break;

        case ONEX_FTM_1XA_RCPM_DATA_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_1XA_RC_PARAM;
          onex_ns_cmd_ptr->rcpm_data.param = param->rcpm;
          break;

        case ONEX_FTM_PAGING_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_DEMOD_PAGING;
          onex_ns_cmd_ptr->demod_paging.param = param->paging;
          break;

        case ONEX_FTM_ACCESS_PROBE_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_ACCESS;
          onex_ns_cmd_ptr->access_probe.param = param->access;
          break;

        case ONEX_FTM_REV_SET_PWR_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_SET_REV_LNK_PWR;
          onex_ns_cmd_ptr->rev_link_pwr_setting.param = param->r_tx_power;
          break;

        case ONEX_SLEEP_SC:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_SLEEP;
          M1X_MSG_SSID( DIAG, LEGACY_HIGH,
            "1xNonSig: FTM MC SLEEP Command");
          break;

        default:
          onex_ns_cmd_ptr->hdr.cmd = NS1X_CMD_INVALID;
          M1X_MSG_SSID( DIAG, LEGACY_ERROR,
            "1xNonSig: Unknown FTM MC Command %d",
            command);
          break;
      }

      if(onex_ns_cmd_ptr->hdr.cmd != NS1X_CMD_INVALID)
      {
        M1X_MSG_SSID( DIAG, LEGACY_HIGH,
          "1xNonSig: Dispatching command id: %d",
          onex_ns_cmd_ptr->hdr.cmd);

        /* Prep the command for MC task. */
        onex_ns_cmd_ptr->hdr.cmd_hdr.sigs = FTM_CMD_RSP_SIG;
        onex_ns_cmd_ptr->hdr.cmd_hdr.task_ptr = rex_self();
        (void) rex_clr_sigs( rex_self(), FTM_CMD_RSP_SIG );
        (void) q_link(onex_ns_cmd_ptr, &onex_ns_cmd_ptr->hdr.cmd_hdr.link );

        /* Place the command on the onex nonsig queue. */
        q_put( &onex_ns_cmd_q, &onex_ns_cmd_ptr->hdr.cmd_hdr.link );

        /*Set the signal and wait for the command to be completed. */
        (void) rex_set_sigs( MC_TCB_PTR, ONEX_NONSIG_MC_CMD_Q_SIG );

        #ifdef FEATURE_MODEM_1X_RUMI
        /* Fire back "success" immediately to prevent QRCT from entering a
           retry cycle and messing us up... */
        response_status = NS1X_TEST_SUCCESS;
        #else /* !FEATURE_MODEM_1X_RUMI */
        (void) ftm_sig_rex_wait( FTM_CMD_RSP_SIG );

        /* Command completed, Clear the received signal and save
           the command status. */
        (void) rex_clr_sigs( rex_self(), FTM_CMD_RSP_SIG );
        response_status = onex_ns_cmd_ptr->hdr.status;
        #endif /* FEATURE_MODEM_1X_RUMI */
      }
      else
      {
        response_status = NS1X_TEST_NO_SUCCESS;

        /* Release the queue. Check to see if the command wants
           to be put on a queue when finished */
        if ( onex_ns_cmd_ptr->hdr.cmd_hdr.done_q_ptr != NULL )
        {
          /* Indeed it does. Do as the command asks */
          q_put( onex_ns_cmd_ptr->hdr.cmd_hdr.done_q_ptr,
                 &onex_ns_cmd_ptr->hdr.cmd_hdr.link );
        }
      }
    }
  }
  else
  {
    response_status = NS1X_TEST_NO_SUCCESS;
    M1X_MSG_SSID( DIAG, LEGACY_FATAL,
      "1xNonSig: Null command pointer");
  }

  return response_status;

} /* onex_nonsig_queue_cmd */


/*===========================================================================

FUNCTION onex_nonsig_is_ftm_mode

DESCRIPTION
  Function used to return the whether or not the mobile is in FTM mode.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if mode is FTM, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean onex_nonsig_is_ftm_mode ( void )
{
  /*------------------------------------------------------------------*/

  #ifdef FEATURE_MODEM_1X_SUPPORTS_RF
  return (ftm_get_mode() == FTM_MODE);
  #else
  return (onex_nonsig_params.mobile_mode == NS1X_FTM_MODE);
  #endif /* FEATURE_MODEM_1X_SUPPORTS_RF */

} /* onex_nonsig_is_ftm_mode */


/*===========================================================================

FUNCTION onex_nonsig_set_ftm_mode

DESCRIPTION
  Used to change the ftm mode if the mode provided is in the valid range.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void onex_nonsig_set_ftm_mode ( onex_nonsig_mode_type mode )
{
  /*------------------------------------------------------------------*/

  switch(mode)
  {
    case NS1X_FTM_MODE:
      #ifdef FEATURE_MODEM_1X_SUPPORTS_RF
      /*REVISIT: FTM sets this on entry and exit of FTM mode.
        MC also writes to this variable. Is it necessary for MC to? */
      ftm_mode = FTM_MODE;
      #else
      onex_nonsig_params.mobile_mode = mode;
      #endif /* FEATURE_MODEM_1X_SUPPORTS_RF */

      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Mode changed to FTM");
      break;

    case NS1X_DMSS_MODE:
      #ifdef FEATURE_MODEM_1X_SUPPORTS_RF
      /*REVISIT: FTM sets this on entry and exit of FTM mode.
        MC also writes to this variable. Is it necessary for MC to? */
      ftm_mode = DMSS_MODE;
      #else
      onex_nonsig_params.mobile_mode = mode;
      #endif /* FEATURE_MODEM_1X_SUPPORTS_RF */

      M1X_MSG_SSID( DIAG, LEGACY_HIGH,
        "1xNonSig: Mode changed to DMSS");
      break;

    default:
      M1X_MSG_SSID( DIAG, LEGACY_ERROR,
        "1xNonSig: Invalid mode type (%d)",
        mode);
      break;
  }

} /* onex_nonsig_set_ftm_mode */


/*===========================================================================

FUNCTION onex_nonsig_ftm_init

DESCRIPTION
  Wrapper used to call into FTM to intialize the RF side. Placing this here
  to provide some seperation from RF for the 1x tasks.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void onex_nonsig_ftm_init ( void )
{
  int i;

  /*------------------------------------------------------------------*/

  /* Initialize queues. */
  (void) q_init(&onex_ns_cmd_q);
  (void) q_init(&onex_ns_cmd_free_q);

  /* Fill onex_ns_mc_cmd_free_q. */
  for (i = 0; i < ONEX_NONSIG_CMD_BUFS_NUM; i++)
  {
    onex_ns_cmd_bufs[i].hdr.cmd_hdr.done_q_ptr = &onex_ns_cmd_free_q;
    q_put(&onex_ns_cmd_free_q,
          q_link(&onex_ns_cmd_bufs[i], &onex_ns_cmd_bufs[i].hdr.cmd_hdr.link));
  }

} /* onex_nonsig_ftm_init */


/*===========================================================================

FUNCTION onex_nonsig_initialize

DESCRIPTION
  Perform any necessary initialization prior to begining 1x non signaling.
  This should be called prior to begining any non signaling operations.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void onex_nonsig_initialize ( void )
{
  /*------------------------------------------------------------------*/

  onex_nonsig_params.ns_fwd_curr_state = NS1X_ENTER;
  onex_nonsig_params.ns_rev_curr_state = NS1X_ENTER;

  /* Initialize timers */
  rex_def_timer (&onex_nonsig_timer, MC_TCB_PTR, ONEX_NONSIG_TMO_SIG );

  M1X_MSG_SSID( DIAG, LEGACY_HIGH,
    "1xNonSig: Initialize 1X Non Signaling Mode");

} /* onex_nonsig_initialize */


/*===========================================================================

FUNCTION onex_nonsig_system_determination

DESCRIPTION
  This is a special simplified version of system determination used only in
  FTM (hardcoded conditions used).

DEPENDENCIES
  None.

RETURN VALUE
  The action code causing the exit.

SIDE EFFECTS
  None.

===========================================================================*/
void onex_nonsig_system_determination(void)
{
  db_items_value_type db_item;

  /*------------------------------------------------------------------*/

  db_item.acq_mode = DB_ACQ_MINI;
  db_put(DB_ACQ_MODE, &db_item);

  db_item.dmss_state = DB_DMSS_ONLINE_DIGITAL_S;
  db_put(DB_DMSS_STATE, &db_item);

  (void) rex_clr_sigs( MC_TCB_PTR, MC_DOG_RPT_SIG );

  mc_update_last_cdma_entry(MCC_ONLINE);

  onex_nonsig_params.ns_fwd_curr_state = NS1X_ENTER;
  onex_nonsig_params.ns_rev_curr_state = NS1X_ENTER;

  M1X_MSG_SSID( DIAG, LEGACY_HIGH,
    "1xNonSig: Entering 1X Non Signaling");

  /***********************
     Call the MCC subtask
  ************************/
  onex_test_mcc();

} /* onex_nonsig_system_determination */


/*===========================================================================

FUNCTION onex_nonsig_disable_mdsp

DESCRIPTION
  Disable the mdsp if it is currently running.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void onex_nonsig_disable_mdsp (void)
{
  /*------------------------------------------------------------------*/

  if( srch_mdsp_awake() )
  {
    M1X_MSG_SSID( DIAG, LEGACY_HIGH,
      "1xNonSig: Disabling MDSP.");
    srch_mdsp_disable();
  }

} /* onex_nonsig_exit */


/*===========================================================================

FUNCTION onex_nonsig_enable_mdsp

DESCRIPTION
  Enable the mdsp and download the image.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void onex_nonsig_enable_mdsp (void)
{
  /*------------------------------------------------------------------*/

  /* check if the MDSP app is ready and valid */
  if (!srch_mdsp_awake())
  {
    /* download the MDSP image for searcher4 and demod */
    srch_mdsp_enable( NULL );
    M1X_MSG_SSID( DIAG, LEGACY_MED,
      "1xNonSig: Srch MDSP App download request");
  }

} /* onex_nonsig_exit */


/*===========================================================================

FUNCTION onex_nonsig_exit

DESCRIPTION
  Perform any necessary cleanup prior to exiting 1x non signaling.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void onex_nonsig_exit (void)
{
  srch_cmd_type cmd;

  /*------------------------------------------------------------------*/

  M1X_MSG_SSID( DIAG, LEGACY_HIGH,
    "1xNonSig: Exiting 1X Non Signaling Mode.");
  M1X_MSG_SSID( DIAG, LEGACY_HIGH,
    "1xNonSig: Send SRCH_DEACTIVATE_F.");
  cmd.hdr.cmd = SRCH_DEACTIVATE_F;
  cmd.hdr.cmd_hdr.task_ptr = NULL;
  cmd.hdr.cmd_hdr.done_q_ptr = NULL;

  /* Send Command to Search task */
  srch_cmd( &cmd );

} /* onex_nonsig_exit */


/*===========================================================================

FUNCTION onex_nonsig_get_mc_cmd_q_cnt

DESCRIPTION
  Retrieve the count of queued commands in mc_cmd_q.

DEPENDENCIES
  None

RETURN VALUE
  Number of queued commands.

SIDE EFFECTS
  None

===========================================================================*/
int onex_nonsig_get_mc_cmd_q_cnt (void)
{

  return (q_cnt(&mc_cmd_q));

} /* onex_nonsig_get_mc_cmd_q_cnt */


/*===========================================================================

FUNCTION onex_nonsig_get_onex_ns_cmd_q_cnt

DESCRIPTION
  Retrieve the count of queued commands in onex_ns_cmd_q.

DEPENDENCIES
  None

RETURN VALUE
  Number of queued commands.

SIDE EFFECTS
  None

===========================================================================*/
int onex_nonsig_get_onex_ns_cmd_q_cnt (void)
{

  return (q_cnt(&onex_ns_cmd_q));

} /* onex_nonsig_get_mc_cmd_q_cnt */


/*===========================================================================

FUNCTION onex_nonsig_process_mc_cmd

DESCRIPTION
  Process commands present on the mc_cmd_q.

DEPENDENCIES
  None

RETURN VALUE
  True if non-signaling mode should be exited, False otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean onex_nonsig_process_mc_cmd (void)
{
  boolean     exit_ftm = FALSE;
  boolean is_cmd_ignored = FALSE;
  mc_msg_type *mc_cmd_ptr;

  /*------------------------------------------------------------------*/

  if ((mc_cmd_ptr = (mc_msg_type *)q_get(&mc_cmd_q)) != NULL)
  {
    switch (mc_cmd_ptr->hdr.cmd)
    {
      case MC_CDMA_PROT_DEACT_CMD_F:
      {
        M1X_MSG_SSID( DIAG, LEGACY_HIGH,
          "1xNonSig: Processing MC_CDMA_PROT_DEACT_CMD_F.");
        exit_ftm = TRUE;

        if (onex_nonsig_params.cdma_enter_init_done == TRUE)
        {
          cdma_exit( FALSE );
          onex_nonsig_params.cdma_enter_init_done = FALSE;
        }

        mmoc_rpt_prot_deactd_cnf( mc_cmd_ptr->deact_prot.info.trans_id );

        /* Keep track of the MMOC cnf for debugging purpose */
        mcc_enqueue_dbg_mmoc_cnf( ONEX_NONSIG_PROCESS_MC_CMD, 
                                  MMOC_RPT_PROT_DEACTD_CNF,
                                  mc_cmd_ptr->deact_prot.info.trans_id );
        mc_cmd_ptr->hdr.status = MC_DONE_S;
        break;
      } /* MC_CDMA_PROT_DEACT_CMD_F */

      case MC_UE_MODE_CHGD_CMD_F:
      {
        /* Update cdma UE mode global var's */
        cdma.ue_mode = mc_cmd_ptr->ue_mode_cmd.ue_mode;
        cdma.is_ue_mode_substate_srlte = 
                            mc_cmd_ptr->ue_mode_cmd.is_ue_mode_substate_srlte;

        /* Send UE Mode cnf to MMOC */
        mmoc_1x_rpt_ue_mode_switch_cnf(mc_cmd_ptr->ue_mode_cmd.trans_id, mcc_asid);

        /* Keep track of the MMOC cnf for debugging purpose */
        mcc_enqueue_dbg_mmoc_cnf( ONEX_NONSIG_PROCESS_MC_CMD, 
                                  MMOC_RPT_PROT_UE_MODE_CMD_CNF,
                                  mc_cmd_ptr->ue_mode_cmd.trans_id );
        M1X_MSG( DCP, LEGACY_MED,
          "1xNonSig:Sent UE Mode Cnf, UE_Mode:%d, Substate:%d, "
          "Trans_id:%d, mcc_asid:%d",
                 mc_cmd_ptr->ue_mode_cmd.ue_mode,
                 mc_cmd_ptr->ue_mode_cmd.is_ue_mode_substate_srlte,
                 mc_cmd_ptr->ue_mode_cmd.trans_id,
                 mcc_asid );
                 
        mc_cmd_ptr->hdr.status = MC_DONE_S;       
        break;
      } /* MC_UE_MODE_CHGD_CMD_F */
          
      #ifdef FEATURE_MODEM_CONFIG_REFRESH
      case MC_NV_REFRESH_F:
      {
        M1X_MSG_SSID( DIAG, LEGACY_HIGH,
          "1xNonSig: Processing MC_NV_REFRESH_F,req_module: %d",
           mc_cmd_ptr->nv_refresh.req_module);

        /* This API will refresh all the required NV items */
        mc_process_nv_refresh();

        /* Req_module indicates whether this NV refresh is cominmg from
        ** Diag or MCFG module. If the NV refresh is from MCFG, then
        ** we need to inform MCFG of refresh done indication */
        if (mc_cmd_ptr->nv_refresh.req_module == NV_REFRESH_MCFG)
        {
          mc_send_refresh_done( mc_cmd_ptr->nv_refresh.reference_id );
        }
        mc_cmd_ptr->hdr.status = MC_DONE_S;
        break;
      } /* MC_NV_REFRESH_F */
      #endif /* FEATURE_MODEM_CONFIG_REFRESH */

#ifdef FEATURE_1XCP_QSH_SUPPORT
      case MC_QSH_CB_CMD_F:
      {
        mc_process_qsh_cmd(&mc_cmd_ptr->qsh_cmd.qsh_params); 
        break;
      }
#endif /* FEATURE_1XCP_QSH_SUPPORT */

      default:
      {
        M1X_MSG_SSID( DIAG, LEGACY_ERROR,
          "1xNonSig: Ignoring MC cmd %d",
          mc_cmd_ptr->hdr.cmd);
        mc_cmd_ptr->hdr.status = MC_DONE_S;
        is_cmd_ignored = TRUE;
      } /* default */
    } /* switch */

    /* Keep track of the MMOC cmd for debugging purpose */
    mcc_enqueue_dbg_mmoc_cmd( mc_cmd_ptr, cdma.curr_state, cdma.curr_state,
                              is_cmd_ignored, ONEX_NONSIG_PROCESS_MC_CMD );

    /* Free the command buffer */
    mc_free_queue_buf( mc_cmd_ptr, MC_CMD_BUF_Q );

  } /* (mc_cmd_ptr != NULL) */

  return exit_ftm;

} /* onex_nonsig_process_mc_cmd */


/*===========================================================================

FUNCTION onex_nonsig_get_rev_state

DESCRIPTION
  This function returns the current reverse link test state.

DEPENDENCIES

RETURN VALUE
   Current reverse link test state.

SIDE EFFECTS
  None.

===========================================================================*/
onex_ns_state_enum_type onex_nonsig_get_rev_state (void)
{
  return onex_nonsig_params.ns_rev_curr_state;

} /* onex_nonsig_get_rev_state */


/*===========================================================================

FUNCTION onex_nonsig_get_fwd_state

DESCRIPTION
  This function returns the current forward link test state.

DEPENDENCIES

RETURN VALUE
   Current forward link test state.

SIDE EFFECTS
  None.

===========================================================================*/
onex_ns_state_enum_type onex_nonsig_get_fwd_state (void)
{
  return onex_nonsig_params.ns_fwd_curr_state;

} /* onex_nonsig_get_fwd_state */


/*===========================================================================

FUNCTION onex_nonsig_get_acq_mode

DESCRIPTION
  This function returns the current non-signaling acquisition mode.

DEPENDENCIES
  None

RETURN VALUE
  Returns the value of onex_nonsig_acq_mode..

SIDE EFFECTS
  None.

===========================================================================*/
db_acq_mode_type onex_nonsig_get_acq_mode(void)
{
  return onex_nonsig_params.acq_mode;

} /* onex_nonsig_get_acq_mode */


/*===========================================================================

FUNCTION onex_nonsig_set_acq_mode

DESCRIPTION
  This function updates the current non-signaling acquisition mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void onex_nonsig_set_acq_mode(db_acq_mode_type mode)
{
  onex_nonsig_params.acq_mode = mode;

} /* onex_nonsig_set_acq_mode */


/*===========================================================================

FUNCTION onex_nonsig_get_band_class

DESCRIPTION
  This function returns the current non-signaling band_class.

DEPENDENCIES
  None

RETURN VALUE
  Current non-signaling CDMA band class

SIDE EFFECTS
  None.

===========================================================================*/
srch_rx_band_type onex_nonsig_get_band_class(void)
{
  return onex_nonsig_params.band_class;

} /* onex_nonsig_get_band_class */


/*===========================================================================

FUNCTION onex_nonsig_set_band_class

DESCRIPTION
  This function updates the current non-signaling band_class.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void onex_nonsig_set_band_class(srch_rx_band_type band_class)
{
  onex_nonsig_params.band_class = band_class;

} /* onex_nonsig_set_band_class */


/*===========================================================================

FUNCTION onex_nonsig_wait_for_pc_msg

DESCRIPTION
  This function returns whether or not 1x should wait for a valid pc message
  before reporting successful paging demod.

DEPENDENCIES
  None

RETURN VALUE
  TRUE = wait for page, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean onex_nonsig_wait_for_pc_msg(void)
{
  return onex_nonsig_params.wait_for_pc_msg;

} /* onex_nonsig_wait_for_pc_msg */


/*===========================================================================

FUNCTION onex_nonsig_reset_fch_stats

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  TEST_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or
  INVALID_STATE, indicating the command is issued at a wrong phone state.

SIDE EFFECTS
  None.

===========================================================================*/
onex_nonsig_cmd_status_type onex_nonsig_reset_fch_stats (void)
{
  /*------------------------------------------------------------------*/

  if(rxc_get_fer_ser_report( NULL, TRUE ) == 0)
  {
   return NS1X_TEST_SUCCESS;
  }
  else
  {
   return NS1X_TEST_NO_SUCCESS;
  }

} /* onex_nonsig_reset_fch_stats */


/*===========================================================================

FUNCTION onex_nonsig_get_fch_stats

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  TEST_SUCCESS, indicating the command has been executed, INVALID_CMD,
  indicating the command is invalid or has invalid parameters, or
  INVALID_STATE, indicating the command is issued at a wrong phone state.

SIDE EFFECTS
  None.

===========================================================================*/
onex_nonsig_cmd_status_type onex_nonsig_get_fch_stats
(
  onex_nonsig_cmd_rxc_fer_ser_type *fer_ser_report
)
{
  rxc_fer_ser_report_type     rxc_fer_ser_report;
  onex_nonsig_cmd_status_type status;

  /*------------------------------------------------------------------*/

  if(rxc_get_fer_ser_report( &rxc_fer_ser_report, FALSE ) == 0)
  {
   status = NS1X_TEST_SUCCESS;
  }
  else
  {
   status = NS1X_TEST_NO_SUCCESS;
  }

  fer_ser_report->fch_sym_errors   = rxc_fer_ser_report.fch_sym_errors;
  fer_ser_report->fch_sym_total    = rxc_fer_ser_report.fch_sym_total;
  fer_ser_report->fch_frame_errors = rxc_fer_ser_report.fch_frame_errors;
  fer_ser_report->fch_frame_total  = rxc_fer_ser_report.fch_frame_total;

  return status;

} /* onex_nonsig_get_fch_stats */

