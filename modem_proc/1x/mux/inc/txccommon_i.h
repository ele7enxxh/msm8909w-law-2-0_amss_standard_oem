#ifndef TXCCOMMON_I_H
#define TXCCOMMON_I_H

/*===========================================================================

          C D M A    T R A N S M I T   C O M M O N   M O D U L E

 DESCRIPTION
  This header file contains definitions for the transmit common module that
  are used by the txc unit.  This header file is internal to the txc unit.

  Copyright (c) 1997 - 2015 Qualcomm Technologies, Inc. 
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/mux/inc/txccommon_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/16/16   eye     Mainline FEATURE_MODEM_1X_IRAT_LTO1X for L compileout build.
09/30/15   sst     Support for Modem Power Statistics
11/23/14   eye     Added support for 0x1877 Power Reporting Log Packet.
10/08/14   ssh     Avoid RliRC update during SCR transaction/HHO.
06/12/14   vks     Add api to register TX (access/traffic) pcg events
11/04/13   dkb     Clean up 1x RL and remove unsupported FW messages.
10/03/13   srk     Added device only DTX debug level messages.
09/10/13   dkb     Register Tx interrupts using m1x vstmr API
09/06/13   srk     Export new APIs to return Filtered TX power and Max TX power
08/28/13   cjb     Removed STX manager interactions
07/23/13   srk     Replace RF API calls with mux API.
05/07/13   eye     Backing out SAR updates done for COEX.
04/25/13   eye     Updating SAAR changes which is to be replaced by COEX.
03/05/13   srk     Replace existing watchdog implementation with the new
                   Dog Heartbeat Mechanism.
01/18/13   srk     Update MCS Subscription Priority Module with the current
                   1x state.
12/19/12   vks     Add support for stx manager
11/07/12   cjb     Mainlining FEATURE_MODEM_1X_SUPPORTS_RCINIT feature.
09/26/12   jtm     DIME_REVISIT clean up.
08/21/12   srk     Create an api to return if TX ON/OFF before doing CFS.
08/20/12   vks     Fix compiler warnings.
07/11/12   srk     Changed the returned values from RF to match legacy units
                   for proper tx power logging.
05/25/12   jtm     Moved SLOT_3 PCG for encoder frame loading to PCG 12.
05/16/12   srk     Workaround for missed hardware assertion of the summary line
05/15/12   srk     Call TX wrapper function to get TX power instead of directly
                   calling RF API.
03/12/12   jtm     Added time_trans flag to txc_tx_type.
04/04/12   srk     Remove txc_update_chan_config() API.
02/23/12   srk     Mainlined FEATURE_IS2000_SCH and FEATURE_IS2000_P2
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
02/13/12   srk     Added txc_watchdog_init and failsafe timer.
01/23/12   srk     Feature Cleanup
12/20/11   srk     Feature Cleanup: Mainlined FEATURE_MODEM_1X_SUPPORTS_Q6
12/13/11   srk     Mainlined FEATURE_RL_JUMP_ENHANCEMENT
12/01/11   jtm     Replaced TXC_INT_CMD with txc_isr_cmd to handle command
                   exchanges between TXC task and the encoder ISR
11/22/11   srk     Feature clean up: Mainlined and Internalized Features
08/31/11   adw     Unify TT and native TX jump to hyperspace.
08/17/11   trc     Mainline RF SVDO API
08/04/11   jtm     Moved TXC_PWR_MAX definition here.
07/07/11   trc     Remove special case timing handling of loopback SO's
07/01/11   jtm     Make txc_get_tx_device non-SVD_API friendly.
06/29/11   adw     Changes to keep track of RF state configured by Tx.
06/03/11   adw     SV support.
05/27/11   trc     NikeL Tx support
05/03/11   jtm     Allow dog report struct to be accessed externally.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
02/04/11   vks     Fix compilation issue.
02/04/11   vks     Use TXC_* for txc_rfnv_backoff_chan_config_type for enum
                   declaration.
01/28/11   vks     Featurize RF interface with FEATURE_MODEM_1X_SUPPORTS_RF.
01/21/11   vks     Use 1X specific RF SVDO feature - FEATURE_1X_RF_SVDO_API.
12/15/10   jtm     Feature cleanup.
12/01/10   bb      Add support for LTE to 1X feature
11/15/10   jtm     Initial SU API split for CP.
11/05/10   jtm     Added more _i.h includes for Modem 1x SU API support.
10/30/10   jtm     Modem 1x SU API changes.
10/30/10   jtm     Integrate from rel/1h10.
06/10/10   jtm     Eliminated dependency on log_dmss.h. RPCTL related functions
                   were moved to txctraffic.c/h.
04/20/10   jtm     Added txc_change_rf_tx_state().
04/15/10   jtm     Increased TXC Slot0 for loopback calls to PCG5 to account for
                   the use of slower clock speeds.
03/30/10   jtm     Replaced T_QDSP6 with FEATURE_MODEM_1X_SUPPORTS_Q6.
02/01/10   jtm     Removed feature TMOBILE.
11/30/09   vks     Group together mdsp writes for 1xADV channel gain ratios.
11/24/09   jtm     Moved dtx_non_crit_fch_frame to the txtc structure.
11/11/09   vks     Temporarily bring back txc_sch_gain for backwards
                   compatability.
11/07/09   vks     Remove TXC_SET_<CHANNEL>_GAIN macros and txc_<channel>_gain
                   variables.
10/28/09   jtm     Removed obsolete macro and updated 1xA variable name.
10/27/09   jtm     Lint fixes.
10/05/09   adw     Removed unnecessary bio.h include.
09/01/09   jtm     Changed macro ENC_RC_TO_RATE_SET to become a function.
08/22/09   jtm     Lint Fixes.
08/05/09   jtm     Lint fixes.
07/06/09   jtm     Update to use cai_rcp_rxc_txc_type.
06/11/09   jtm     1x Advanced Support. Added dtx_fch to txc_so_type
                   and TXC_SO_IS_VOICE macro to test for voice only SO.
06/11/09   jtm     Replaced FEATURE_1X_Q6_CRIT_SECT with T_QDSP6.
05/11/09   jtm     Added include for log_dmss.h to support diag decoupling.
04/08/09   jtm     Initial 1X Advanced feature support.
04/01/09   jtm     Added customer.h
03/20/09   jtm     Lint fix.
03/17/09   jtm     Replaced INTLOCK/INTFREE, INTLOCK_SAV/INTFREE_SAV, and
                   MSM_LOCK/MSM_UNLOCK with macros that can be mapped correctly
                   based on the compiler and controlled internally to TXC.
11/19/08   jtm     Split various header files into internal and external
                   declarations for Common Modem Interface.
07/21/08   jtm     Added TXC_UPPER_BOUNDS_CHECK macro. Removed txc_pwr_boost
                   and txc_pwr_boost_cnt since they are unused.
04/02/07   vlc     Added new PCG define for bin0 ENC ISRs specifically for loop
                   back calls.  Moved default PCG to 1 for bin0 ENC ISRs for
                   all other calls.
01/22/07   trc     Parameterize the PCG for bin0 and bin3 ENC ISRs and bump
                   bin0 from PCG 1 to PCG 3.
10/30/06   bn      Added code to support DO/UMTS to 1x handoff
09/05/06   vlc     Implemented TX gain enhancements to increase digital gain
                   resolution.
06/09/06   trc     Remove T_MSM6700 featurization
03/22/06   bn      Set the TXC_IS95A_FCH_GAIN to (255) / sqrt(2)
05/23/05   vlc     Changed FEATURE_IS2000_REL_D_JUMP to FEATURE_RL_JUMP_ENHANCEMENT.
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
04/06/05   vlc     Changed TXC_JUMP_CMD to TXC_JUMP.
03/07/05   vlc     Added support for Fast Call Set Up - New Jump to
                   Hyperspace command feature.
09/01/04   jrp     Added macros for differences between pre-6700 and post-6700
                   targets.
06/29/04   jrp     Added define for TXC_IS95A_FCH_GAIN
05/07/04   vlc     Modified RLP logging extensively to decouple MUX processing
                   and RLP processing, moved RLP processing to RLP module.
04/26/04   vlc     Implemented new way of handling RXTX confirmation of all
                   signalling messages for voice calls/data calls/svd calls.
04/20/04   jrp     Fixed lint warnings.
03/10/04   vlc     Added support for centralized transaction engine handling
                   under feature FEATURE_TRANS_MANAGER.
02/03/04   sab     Featurized confirmation_type under Release B.
08/25/03   vlc     Removed shared rate_set from txc_so (txc_so_type) to avoid
                   confusion between FCH and DCCH rate sets.  Now calculate
                   rate set from RC as it is needed.
08/06/03   sab     Added confirmation_type to txc_tx.
08/23/02   jrp     Changed MAX_REV_MSG_BYTES for Release A to allow space for
                   extended message size.
05/22/02   bn      Added new logics to support VP2 RPC
04/08/02   tc      Supported REACH 20ms frame, 38400bps pad size.
02/25/02   bn      Added support for  MUX SVD signaling
10/30/01   bn      Added support for Release A RDCH logging
08/28/01   lh      Back out SI bits changes for Release A Addendum 2.
08/13/01   bn,hrk  Added prototype for txc_frame_types_send_log().
06/28/01   vlc     Removed rev_sch_rc from txc_so_type (it's a duplication
                   of r_sch_rc in txtc_type.)
06/15/01   day     Mainlined FEATURE_FFPC, FEATURE_DATA_TIMELINE_ADVANCE, FEATURE_IS2000
                   Deleted FEATURE_FINGER_POLL
06/04/01   vlc     Changed FEATURE_GPSONE to FEATURE_GPS_MODE.
04/26/01   vlc     Added rev_sch_rc (to support SCH RC4) to txc_so_type.
04/20/01   sr      Merged in from MSM5100 archive
04/18/01   sr      Merged with MSM_MUX1X.01.00.00
           lcc     Added support for FEATURE_IS2000_P2
                   Moved txc_scr_action() and txc_scr_trans_cmd to another file.
03/07/01   lh      Changes for LAC support for Rel A.
03/01/01   tc      Merged MSM_MUX1X.00.00.15.
02/16/01   hrk     Added prototype for txc_rpctrl_send_log().
01/30/01   tc      Merged GPS support.
01/12/01   tc      Merged MSM_MUX1X.00.00.06.
11/13/00   tc      Defined txc_pch_gain, txc_fch_gain, txc_sch_gain.
11/09/00   tc      Defined feature T_MSM_5100.
11/08/00   tc      Supported MSM5100 modulator driver interface.
11/10/00   jc      Created after repartition of txc.c

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

//common
#include "1x_variation.h"
#include "comdef.h"
#include "customer.h"
#include "modem_1x_defs.h"
#include "m1x_diag.h"
//mux
#include "txc.h"
#include "txc_v.h"
#include "txc_i.h"
#include "rlgc.h"
#include "rlgc_i.h"
#include "ffpc_i.h"

//drivers
#include "enci.h"
#include "enci_i.h"             /* Encoder register & macro definitions */
#include "m1x_stmr_i.h"

//cp
#include "cai.h"
#include "cai_v.h"
#include "mccdma.h"
#include "mccdma_v.h"
#include "mccdma_i.h"

//other
#include "log.h"
#include "subs_prio.h"
#include "dog.h"

//feature dependent
#ifdef FEATURE_MODEM_1X_SUPPORTS_RF
#include "rfm_1x.h"
#else /* FEATURE_MODEM_1X_SUPPORTS_RF */
#include "enc.h"
#endif /* FEATURE_MODEM_1X_SUPPORTS_RF */

#include "rex.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* TCB for TX task */
extern rex_tcb_type* tx_tcb_ptr;

/* RCInit version */
#define TX_TCB_PTR ( tx_tcb_ptr )


#define TXC_PWR_MAX 0x80           /* Tx gain adjust maximum value */

/*--------------------------------------------------------------------------
                        Critical Section macros
--------------------------------------------------------------------------*/

/* The following defines create sections where other code is not allowed to
   run. The appropriate macro must be used depending on the type of critical
   section required. The following table provides a guide for selecting the
   correct macro:

   Q1. Will the critical section protect a shared resource?
       (Shared global data or shared code that more than one task can access
        at the same time.)

   Q2. Is the code time critical in nature?

   Q3. Can the code be called from interrupt context?

  /-----------------------------------------------------------------------\
  |   Question    |                       Resolution                      |
  |  -----------  |                      ------------                     |
  | 1    2    3   |                                                       |
  |---------------|-------------------------------------------------------|
  | No   No   -   | No critical section required.                         |
  |-----------------------------------------------------------------------|
  | No   Yes  -   | Use TXC_ENTER_ATOMIC_SECT()                           |
  |-----------------------------------------------------------------------|
  | Yes  No   No  | Use TXC_ENTER_ISR_SECT()                              |
  |-----------------------------------------------------------------------|
  | Yes  No   Yes | Use TXC_ENTER_CRIT_SECT()                             |
  |-----------------------------------------------------------------------|
  | Yes  Yes  No  | Use TXC_ENTER_ATOMIC_CRIT_SECT()                      |
  |-----------------------------------------------------------------------|
  | Yes  Yes  Yes | Use TXC_ENTER_ATOMIC_ISR_SECT()                       |
  \-----------------------------------------------------------------------/

 *Note* - For sections that are both time critical and a shared resource
          use the appropriate critical section macro with the time critical
          macro nested inside.
*/
#define TXC_ENTER_ATOMIC_SECTION()    REX_DISABLE_PREMPTION()
#define TXC_LEAVE_ATOMIC_SECTION()    REX_ENABLE_PREMPTION()

#define TXC_ENTER_ISR_SECTION()       REX_ISR_LOCK( &txc_crit_sect )
#define TXC_LEAVE_ISR_SECTION()       REX_ISR_UNLOCK( &txc_crit_sect )

#define TXC_ENTER_CRITICAL_SECTION()  rex_enter_crit_sect( &txc_crit_sect )
#define TXC_LEAVE_CRITICAL_SECTION()  rex_leave_crit_sect( &txc_crit_sect )

#define TXC_ENTER_ATOMIC_ISR_SECTION()      \
    REX_ISR_LOCK( &txc_crit_sect );         \
    REX_DISABLE_PREMPTION()

#define TXC_LEAVE_ATOMIC_ISR_SECTION()      \
    REX_ENABLE_PREMPTION();                 \
    REX_ISR_UNLOCK( &txc_crit_sect )

#define TXC_ENTER_ATOMIC_CRIT_SECT()        \
    rex_enter_crit_sect( &txc_crit_sect );  \
    REX_DISABLE_PREMPTION()

#define TXC_LEAVE_ATOMIC_CRIT_SECT()        \
    REX_ENABLE_PREMPTION();                 \
    rex_leave_crit_sect( &txc_crit_sect )

/* declare a critical section that is common throughout the TX task */
extern rex_crit_sect_type txc_crit_sect;

/* TX Debug Masks */
#define TX_DBG_MSG( ... ) \
    M1X_MSG( MUX, DBG_TX, __VA_ARGS__ )

/* Device only DTX Debug Masks */
#define DTX_DBG_MSG( ... ) \
    M1X_MSG( MUX, DBG_DTX, __VA_ARGS__ )

#define PREAMBLES_TO_GATING_PATN(p) ((word) ( (word)((1 << (p)) - 1) << (16-(p))))

#ifdef FEATURE_IS2000_1X_ADV
extern cai_rcp_rxc_txc_type txc_rcpm_data;
#endif /* FEATURE_IS2000_1X_ADV */

typedef struct
{
  struct
  {
    boolean included;
    cai_radio_config_type rc;
  }
  rev_fch, rev_dcch;
  fpc_mode_type fpc_mode;
  struct
  {
    word    so;                 /* service option */
    word    phy_channels;       /* physical channel mapping */
  } pri, sec, stored_pri;

  word signaling_phy_channel;   /* FCH or/and DCCH carrying signaling data */
  word rev_sch_mux_option;
} txc_so_type;

extern txc_so_type txc_so;

/* commands to the interrupt handlers */
typedef enum
{
  INT_TC_F,                       /* traffic channel */
  INT_FRAME_F                     /* initialize frame count */
} txc_int_cmd_type;

/* Gain value for IS-95 waveform (RC1 & RC2)
** Only 1 traffic channel on RL
** Allocate all power to traffic channel
** Gain setting for traffic channel is a constant
** OLD (8-bit):     255 / sqrt(2) = 180
** NEW (15-bit):    (255 << 7) / sqrt(2) = 23079
**   Sqrt(2) is for gain adjustment for QPSK
*/
#define TXC_IS95A_FCH_GAIN (23079)

/* define buffers to hold the messages to be sent */
#ifdef FEATURE_IS2000_REL_A
#define MAX_REV_MSG_BYTES   MAX( CAI_REV_TC_MAX_MSG_BYTES, CAI_REV_CSCH_MAX_MSG_BYTES)
  /* This constant will be used for boundary checking for SAR sublayer*/
#else
#define MAX_REV_MSG_BYTES   MAX( CAI_REV_TC_MAX_MSG_BYTES, CAI_AC_MAX_MSG_BYTES)
  /* This constant will be used for boundary checking for SAR sublayer*/
#endif // FEATURE_IS2000_REL_A

typedef struct
{
  txc_cmd_type *cmd_ptr;               /* command pointer */
  boolean      blank;                  /* blank and burst. */
  word         len;                    /* length, in bits of the message */
  word         pos;                    /* current position, in bits */
  qword        mask;                   /* access channel mask */
  /*lint -save -e506 Constant value Boolean - neither constant is boolean*/
  byte         msg[MAX_REV_MSG_BYTES]; /* the message */
  /*lint -restore*/
} txc_msg_buf_type;

/* This moves the conv encoder interrupt to 10ms from the begining of the
** frame. This is done to separate the viterbi decoder interrupt which happens
** at 4.5ms into the frame and the conv encoder interrupt which *used to*
** happen at 5ms previously. Applicable only to voice calls.
*/
#define TXC_INT_BIN_MASK_BIN_0       0x0001      /* 20 ms bin 0    */
#define TXC_INT_BIN_MASK_BIN_1       0x0002      /* 20 ms bin 1    */
#define TXC_INT_BIN_MASK_BIN_2       0x0004      /* 20 ms bin 2    */
#define TXC_INT_BIN_MASK_BIN_3       0x0008      /* 20 ms bin 3    */
#define TXC_INT_OFFSET_PCG_0         0x0000      /* 5 ms  pcg 0    */
#define TXC_INT_OFFSET_PCG_1         0x0001      /* 5 ms  pcg 1    */
#define TXC_INT_OFFSET_PCG_2         0x0002      /* 5 ms  pcg 2    */
#define TXC_INT_OFFSET_PCG_3         0x0003      /* 5 ms  pcg 3    */
#define TXC_INT_OFFSET_WALSH_0       0x0000      /* walsh symbol 0 */
#define TXC_INT_OFFSET_WALSH_1       0x0001      /* walsh symbol 1 */
#define TXC_INT_OFFSET_WALSH_2       0x0002      /* walsh symbol 2 */
#define TXC_INT_OFFSET_WALSH_3       0x0003      /* walsh symbol 3 */
#define TXC_INT_OFFSET_WALSH_4       0x0004      /* walsh symbol 4 */
#define TXC_INT_OFFSET_WALSH_5       0x0005      /* walsh symbol 5 */

#define TXC_INT_OFFSET_PCG      8                /* 1 PCG = 1.25 ms */

#define TXC_INT_OFFSET_PCG_DATA 4                /* 1 PCG = 1.25 ms */

#define TXC_INT_OFFSET_WALSH    0

/* Tx PCG masks */
#define TX_PCG_MASK_DISABLED          0x0000
#define DTX_MASK_ENABLED              0x3333
#define DTX_MASK_ENABLED_FULL_RATE    0xFF33
#define SB_MASK_ENABLED               0x9999
#define DTX_SB_MASK_DISABLED          0xFFFF
#define RPC_MASK_200_HZ               0x4444
#define RPC_MASK_200_400_HZ           0x5555
#define RPC_MASK_300_HZ               0x5544
#define FPC_MASK_200_HZ               0x1111
#define FPC_MASK_400_HZ               0x5555
#define FPC_MASK_300_HZ               0x5511

/* Queue definitions */
extern q_type txc_cmd_q;

/* Timer to wake Transmit task so that it can set its Watchdog task
   Report signal */
extern rex_timer_type     txc_rf_timer;

extern rex_timer_type     txc_failsafe_timer;

extern rex_timer_type     txc_shutdown_wait_timer;

/* Data rate of current frame */
extern cai_data_rate_type txc_data_rate;

#ifdef FEATURE_IS2000_REL_A_CC

/* REACH 20ms frame, 38400bps data rate, 768 bits per frame. */
/* It needs 96 bytes of zeros for padding the REACH frame.   */

#define TXC_REACH_20MS_FRAME_38400_DATA_RATE_SIZE   96

extern byte zeros[TXC_REACH_20MS_FRAME_38400_DATA_RATE_SIZE];

#else

extern byte zeros[ENC_14400_FULL_RATE_SIZE];

#endif // FEATURE_IS2000_REL_A_CC

/*
**   Modem Power Statistics
*/
/* Number of bins to use for Modem Power Stats log */
#define TXC_NUM_MODEM_POWER_BINS 5
//#if DS3G_TX_POWER_BIN_MAX != TXC_NUM_MODEM_POWER_BINS
//  #error " DS3G_TX_POWER_BIN_MAX != TXC_NUM_MODEM_POWER_BINS "
//#endif

typedef struct
{
  boolean                     enable;            /* logging en/disabled       */
  rex_crit_sect_type          crit_sect;         /* critical section          */
  uint32                      tx_power_bins[ TXC_NUM_MODEM_POWER_BINS ];
                                                 /* Array for binning of
                                                    durations by power level  */
  uint32                      tx_overall;        /* Total tx duration         */
} txc_modem_pwr_stats;

/* The possible states the TXC task can be in */
typedef enum
{
  TXC_CDMA_S,             /* Initializing CDMA */
  TXC_ACC_IDLE_S,         /* Idle on Paging channel */
  TXC_ACC_JUMP_S,         /* Performing jump to hyperspace */
  TXC_ACC_ATT_S,          /* Performing an access attempt */
  TXC_TT_S,               /* Set up the Time Transfer to be ready on the RL */
  TXC_HOLD_ENC_CLK_S,     /* Hold ENC clocks ON */
  TXC_TRAFFIC_S,          /* Traffic channel */
  TXC_EXIT_S              /* Stop state */
} txc_state_type;
/* Curent state of TXC state machine */
extern txc_state_type  txc_state;

/* command block for interrupt handler commands */
typedef struct
{
  boolean             iscmd;      /* is there a command? */
  txc_int_cmd_type    cmd;        /* command to process  */
  qword               mask;       /* long pn mask to use */
} txc_int_cmd_type_main;

extern txc_int_cmd_type_main txc_int_cmd;

/* Values of txc_tx.delay.
*/
typedef enum
{
  TXC_NO_CONF,       /* Do not need to send signalling confirmation */
  TXC_DELAY_CONF,    /* Delay sending confirmation by 1 frame */
  TXC_SEND_CONF      /* Send signalling confirmation this frame */
} txc_delay_conf_type;

/* Values of txc_tx.jump_source.
*/
typedef enum
{
  TXC_READY_TO_JUMP, /* Has not jump to hyperspace yet */
  TXC_JUMP,          /* Jump to hyperspace */
  TXC_JUMP_ACCESS,   /* Jump to hyperspace prior to access state */
  TXC_JUMP_TRAFFIC,  /* Jump to hyperspace prior to traffic state */
  TXC_JUMP_TT        /* Jump to hyperspace in time xfer state */
} txc_jump_source_type;

/* RF Tx state
*/
typedef enum
{
  TXC_RF_STATE_DISABLE,
  TXC_RF_STATE_ENABLE,
  TXC_RF_STATE_STOP,
  TXC_RF_STATE_START
} txc_rf_state_type;

/* hold most of the global variables for the transmit task */
typedef struct
{
  word                  int_cnt;           /* Count Interrupts */
  qword                 frame_cnt;         /* Count transmit frames */
  qword                 mask;              /* Long PN mask to use */
  boolean               active_msg;        /* Flag for active message buffer */
  boolean               tx_on;             /* If Tx is on */
  boolean               tx_on_before_cfs;  /* If Tx was on before CFS */
  txc_rf_state_type     rf_state;          /* RF state as configured by Tx */
  boolean               tx_blank;          /* Expect a blank from vocoder */
  txc_jump_source_type  jump_source;       /* Why TXC performs jump to
                                              hyperspace */
  txc_delay_conf_type   delay;             /* May need to delay signalling msg
                                              confirmation to MC by 1 frame */
  txc_msg_buf_type      buf;               /* Buffer to send */
  #ifdef FEATURE_IS2000_REL_B
  txc_confirmation_type confirmation_type; /* Indicates when rxtx wants the
                                              confirmation */
  #endif /* FEATURE_IS2000_REL_B */
  boolean               time_trans;        /* We entering traffic via TT? */
  boolean               is_rc_update_deferred; /* RC update is deffered 
                                                  for SCR Transaction */
  txc_modem_pwr_stats   mdm_pwr_stats;     /* Modem Power Stats */
} txc_tx_type;
extern txc_tx_type txc_tx;

typedef struct
{
  boolean pending;     /* Indicates whether a frame offset cmd is pending */
  byte frame_offset;   /* Traffic Channel frame offset */
  qword action_frame;  /* Frame when frame offset takes effect */
} txc_frame_offset_cmd_type_pend;
extern txc_frame_offset_cmd_type_pend txc_frame_offset_cmd;

extern byte txc_frame_offset;
/* Offset of the transmit interrupt from the frame boundary for Traffic
** Channel.*/

#ifdef FEATURE_IS2000_REL_A
#define SR_ID_MAX    8          /* service reference option */

typedef struct
{
   word   traffic_type;         /* 0 for primary, 1 for secondary */
   word   phy_channels;         /* which channel FCH or/and SCH or/and DCCH carries
                                primary data.We are only expecting the corresponding
                                data type(pri or sec) will come from the corresponding
                                phy_channels */

   word   so;                   /* service option */
} txc_srid_ref_type ;

extern txc_srid_ref_type  txc_srid_ref[SR_ID_MAX];

#endif /* FEATURE_IS2000_REL_A */

extern txc_so_type txc_so;

/* gain table taken from Tige2 HLD */
extern txc_gain_type txc_pch_fch_sch_gain_tab[ ENC_NUM_SCH_RATES ];

extern txc_gain_type txc_pch_fch_gain_tab[ ENC_NUM_FRAC_RATES ];

/* Adjusted R-PCH Gain Table when only R-FCH is active
** and R-SCH is not assigned */
extern int16 txc_pch_adj_gain_tab[ ENC_NUM_FRAC_RATES ];

extern int16 txc_sch_pch_adj_gain_tab[ ENC_NUM_FRAC_RATES ];

/* Gain Table when R-FCH and R-SCH is assigned and R-SCH is active */
extern txc_gain_type txc_gain_tab[ ENC_NUM_FRAC_RATES ];

/* Gain table when R-FCH and R-SCH is assigned but R-SCH is inactive */
extern txc_gain_type txc_dtx_gain_tab[ ENC_NUM_FRAC_RATES ];

extern int16 txc_dtx_pch_adj_gain_tab[ ENC_NUM_FRAC_RATES ];

/* gain table for various combinations of DCCH and SCH operating modes */
extern txc_dcch_sch_gain_type txc_dcch_sch_gain_tab[TXC_DCCH_SCH_CONFIG_MODES];

#ifdef FEATURE_IS2000_REL_A_SVD
/* VP2 RPC new structure */
  extern txc_fch_dcch_sch_gain_type txc_fch_dcch_sch_gain_tab[ENC_NUM_FRAC_RATES][TXC_DCCH_SCH_CONFIG_MODES];
#endif /* FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_IS2000_1X_ADV
extern txc_linear_gain_ratio_type txc_fch_ack_gain_ratio_tab[ ENC_NUM_FRAC_RATES ][ TXC_NUM_ACS_TYPES ];
extern txc_linear_gain_ratio_type txc_fch_sch_ack_gain_ratio_tab[ ENC_NUM_FRAC_RATES ][ TXC_NUM_ACS_TYPES ];
extern txc_linear_gain_ratio_type txc_fch_schdtx_ack_gain_ratio_tab[ ENC_NUM_FRAC_RATES ][ TXC_NUM_ACS_TYPES ];
#endif /* FEATURE_IS2000_1X_ADV */

extern word  txc_action_time;
  /* System Action Time as specified in OTA message */

extern boolean txc_action_time_chk_enabled;
  /* When TRUE, txc_traffic() should check if the action time is up
   * while processing "TXC_INT_SIG". If the action time is up,
   * TXC will send "MCC_TXC_FRAME_SIG" to MCC
   */

extern boolean txc_action_time_stop_tx_enabled;
  /* When TRUE, TXC will command the transmitter to be shut down at
   * action time, i.e. while the first MCC_TXC_FRAME_SIG is generated
   */

extern boolean txc_next_int_occurred;

#define TXC_UPPER_BOUNDS_CHECK(index, array_size) \
          (((uint32) (index) < (uint32) (array_size)) ? (uint32) (index):((uint32) (array_size)-1))

/*
  Type used by MUX1X to relay the reverse link channel configuration
  to the RF PA Backoff module
*/
typedef enum
{
  TXC_PILOT_FCH                   = 0,
  TXC_PILOT_DCCH_9600             = 1,
  TXC_NUMBER_OF_BACKOFF_CONFIGS,
  TXC_BACKOFF_STEADY_STATE        = 1000,
  TXC_BACKOFF_FIRST_RUN           = 2000,
  TXC_REFRESH_BACKOFF             = 3000,
  TXC_NO_BACK_OFF_CONFIG          = 4000,
  TXC_BACKOFF_CHAN_CONFIG_VALID   = 5000
} txc_rfnv_backoff_chan_config_type ;

#ifdef FEATURE_MODEM_1X_SUPPORTS_RF

#define txc_rfm_1x_txc_chan_config_type rfm_1x_txc_chan_config_type
#define txc_rfm_1x_txc_pwr_limit_type rfm_1x_txc_pwr_limit_type

#else /* FEATURE_MODEM_1X_SUPPORTS_RF */

typedef struct
{

  uint16 tx_pwr_limit;
  /*!< Power limit in 1/12th dB resolution */

  int16  tx_pwr_limit_adj;
  /*!< Power limit adjustment in 1/12th dB resolution */

} txc_rfm_1x_txc_pwr_limit_type;

typedef struct
{
   boolean                       fch_on;
   /*!< Indicates if the R-FCH channel is enabled. */

   cai_radio_config_type         fch_rc;
   /*!< The R-FCH RC if the channel is enabed. */

   boolean                       dcch_on;
   /*!< Indicates if the R-DCCH channel is enabled. */

   cai_radio_config_type         dcch_rc;
   /*!< The R-DCCH RC if the channel is enabed. */

   boolean                       sch_on;
   /*!< Indicates if the R-SCH channel is enabled. */

   cai_radio_config_type         sch_rc;
   /*!< The R-SCH RC if the channel is enabed. */

   enc_sch_rate_type             sch_rate;
   /*!< The R-SCH rate if the channel is enabed. */

   boolean                       sch_to_pch_over_6dB;
   /*!< Indicates if the SCH to PCH ratio is over 6dB. */

} txc_rfm_1x_txc_chan_config_type;

#endif /* FEATURE_MODEM_1X_SUPPORTS_RF */

/* dog parameters */
extern dog_report_type txc_dog_rpt_id;

/* Enum to distinguish different conditions to setup PCG events
   with m1x stmr */
typedef enum
{
  TXC_PCG_EVENT_TRAFFIC = 0,
  TXC_PCG_EVENT_ACCESS,
  TXC_PCG_EVENT_HOLD_ENC_CLK
} txc_pcg_event_enum_type;

/* PCGs at which L1 needs the various m1x_stmr interrupts to build/config access
   and tx frames */
#define TXC_ACCESS_ISR_PCG       8
#define TXC_HOLD_ENC_CLK_ISR_PCG 1
#define TXC_TC_FIRST_ISR_PCG     1
#define TXC_TC_SECOND_ISR_PCG    10

/* Max PCG by which SW should RL_FRAME_CONFIG to FW */
#define TXC_MAX_ALLOWED_PCG_FOR_FW_CONFIG 12

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION TXC_DOG_REPORT

DESCRIPTION
  This function sends report to Dog heartbeat mechanism and clears the
  dog report signal.

DEPENDENCIES
  Depends on previously initialized DOG report ID.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void txc_dog_report( void );

/*===========================================================================

FUNCTION TXC_WATCHDOG_INIT

DESCRIPTION
  This function is used to initialize the TX task with the DOG subsystem

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  WDOG timer system is started for TX task
===========================================================================*/
extern void txc_watchdog_init( void );

/*===========================================================================

FUNCTION TXC_CMD_DONE

DESCRIPTION
  Dispose of a command buffer when we're done with it.  Depending of
  the contents of the buffer, this will involve placing it on a free
  queue if a free queue is designated,  and if a tcb is given,  the
  appropriate task will be notified.

DEPENDENCIES
  None.
RETURN VALUE
  Just returns the status passed to it
SIDE EFFECTS
  Could signal another task
===========================================================================*/
extern void txc_cmd_done
(
  txc_cmd_type   *cmd_ptr,         /* buffer to dispose of */
  txc_status_type status           /* set status of buffer */
);


/*===========================================================================

FUNCTION TXC_CLR_MSGS

DESCRIPTION
  Move any pending messages in the messages buffer over to the free
  queue,  effectively erasing any queued messages.

DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
extern void txc_clr_msgs( void );

/*===========================================================================

FUNCTION TXC_WAIT

DESCRIPTION
  Wait for the watchdog and for any other signals that might get set.
  Return if one of the other signals is set.  Always kick the watchdog
  whenever we wake up.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the signals for the current task.

SIDE EFFECTS
  None.

===========================================================================*/
extern rex_sigs_type txc_wait
(
  word requested_mask
    /* Mask of signals to wait for */
);

/*===========================================================================

FUNCTION TXC_CDMA_ENC_SHUTDOWN

DESCRIPTION
  Shut down the encoder / interleaver / deinterleaver & PA for CDMA

DEPENDENCIES
  None
RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
extern void txc_cdma_enc_shutdown( void );

/*===========================================================================

FUNCTION TXC_EXIT

DESCRIPTION
  This function performs processing necessary for exiting CDMA Transmit
  subtask.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void txc_exit ( void );

/* <EJECT> */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION TXC_RDCH_FRAME_INFO_SEND_LOG

DESCRIPTION
  Terminate and send the accumulated RDCH frame information log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A RDCH frame information log may be sent.

===========================================================================*/
extern void txc_rdch_frame_info_send_log( void );
#else
/*===========================================================================

FUNCTION TXC_FRAME_TYPES_SEND_LOG

DESCRIPTION
  Terminate and send the accumulated reverse power control frames log.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  A reverse power control log may be sent.

===========================================================================*/
extern void txc_frame_types_send_log( void );

#endif /* FEATURE_IS2000_REL_A */

/*===========================================================================

FUNCTION      TXC_GET_TX_DEVICE

DESCRIPTION   Returns the RF Tx device in use.

DEPENDENCIES  None

RETURN VALUE  The RF Tx device.

SIDE EFFECTS  None

===========================================================================*/
extern rfm_device_enum_type txc_get_tx_device( void );

/*===========================================================================

FUNCTION TXC_GET_FILTERED_TX_POWER_DBM10

DESCRIPTION
  Returns Filtered TX Power in dBm10 if TX is ON for the given RF device.

DEPENDENCIES
  None.

RETURN VALUE
  Filtered Tx power value in dBm10

SIDE EFFECTS
  None
===========================================================================*/
extern int16 txc_get_filtered_tx_power_dbm10( void );

/*===========================================================================

FUNCTION TXC_GET_TX_AGC_DBM10

DESCRIPTION
  Returns TX AGC in 1/10th dBm if TX is ON for the given RF device.

DEPENDENCIES
  None.

RETURN VALUE
  Tx ON  : Total 1x transmit power in 1/10th dBm and 0x8000 on failure
  Tx OFF : 0x8000 = -32768

SIDE EFFECTS
  None
===========================================================================*/
extern int16 txc_get_tx_agc_dbm10( void );

/*===========================================================================

FUNCTION TXC_GET_TX_AGC_IN_LEGACY_UNITS_DBM12

DESCRIPTION
  Converts TX AGC to 1/12th dBm to match legacy units.

DEPENDENCIES
  None.

RETURN VALUE
  Tx AGC in 1/12th dBm

SIDE EFFECTS
  None
===========================================================================*/
extern int16 txc_get_tx_agc_in_legacy_units_dbm12( void );

/*===========================================================================

FUNCTION TXC_GET_MAX_TX_AGC_DBM12

DESCRIPTION
  Returns MAX TX AGC in dBm12 for the given RF device.

DEPENDENCIES
  None.

RETURN VALUE
  Max 1x transmit power in dBm12 and 0 dBm12 (-52.33 dBm) on failure

SIDE EFFECTS
  None
===========================================================================*/
extern int16 txc_get_max_tx_agc_dbm12( void );

/*===========================================================================

FUNCTION TXC_GET_MAX_TX_AGC_DBM10

DESCRIPTION
  Returns MAX TX AGC in dBm for the given RF device.

DEPENDENCIES
  None.

RETURN VALUE
  Max 1x transmit power in dBm10

SIDE EFFECTS
  None
===========================================================================*/
extern int16 txc_get_max_tx_agc_dbm10( void );

/*===========================================================================

FUNCTION TXC_GET_TX_AGC_DBM2

DESCRIPTION
  Returns TX AGC in 1/2th dBm if TX is ON for the given RF device.

DEPENDENCIES
  None.

RETURN VALUE
  Tx ON  : Total 1x transmit power in 1/2th dBm and 0x80 on failure
  Tx OFF : 0x80 = -128

SIDE EFFECTS
  None
===========================================================================*/
extern int8 txc_get_tx_agc_dbm2( void );

#ifdef FEATURE_IS2000_1X_ADV
/*===========================================================================

FUNCTION TXC_SET_PCH_FCH_SCH_GAIN_RATIOS

DESCRIPTION
  Save the channel to pilot linear gain ratios

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void txc_set_pch_fch_sch_ratios
(
  enc_rate_type rate,
  txc_linear_gain_ratio_type ratio,
  txc_chan_status_type chan_status,
  txc_active_cell_size_types acs
);
#endif /* FEATURE_IS2000_1X_ADV */

/*===========================================================================

FUNCTION TXC_GET_REV_LINK_RATE_SET

DESCRIPTION
  Returns the rate set for a revese link channel given the current RC of
  that channel.

DEPENDENCIES
  None

RETURN VALUE
  The rate set corresponding to the provide radio configuration.

SIDE EFFECTS
  None
===========================================================================*/
extern enc_rate_set_type txc_get_rev_link_rate_set( cai_radio_config_type rc );

/*===========================================================================

FUNCTION TXC_UPDATE_POWER_STATS

DESCRIPTION
  Update statistics Tx power level.
  The expectation is that this will be called upon each and every frame isr.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void txc_update_power_stats( void );

/*===========================================================================

FUNCTION      TXC_CHANGE_RF_TX_STATE

DESCRIPTION   This function handles changing the transmitter state
              (enabled/disabled/started/stopped) using RF APIs and safeguards
              against calling these RF APIs redundantly.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS

              is_enabled  is_stopped  requested state      |   result
              ------------------------------------------------------------
                  0           0      TXC_RF_STATE_ENABLE   |   TX Enabled
                  0           0      TXC_RF_STATE_DISABLE  |   No Change
                  0           0      TXC_RF_STATE_START    |   No Change
                  0           0      TXC_RF_STATE_STOP     |   No Change
                                                           |
                  0           1      TXC_RF_STATE_ENABLE   |   INVALID
                  0           1      TXC_RF_STATE_DISABLE  |   INVALID
                  0           1      TXC_RF_STATE_START    |   INVALID
                  0           1      TXC_RF_STATE_STOP     |   INVALID
                                                           |
                  1           0      TXC_RF_STATE_ENABLE   |   No Change
                  1           0      TXC_RF_STATE_DISABLE  |   TX Disabled
                  1           0      TXC_RF_STATE_START    |   No Change
                  1           0      TXC_RF_STATE_STOP     |   TX Stopped
                                                           |
                  1           1      TXC_RF_STATE_ENABLE   |   No Change
                  1           1      TXC_RF_STATE_DISABLE  |   TX Disabled
                  1           1      TXC_RF_STATE_START    |   TX Started
                  1           1      TXC_RF_STATE_STOP     |   No Change


              NOTE: is_stopped will be cleared by default whenever RF TX
                    transitions to enabled or disabled states.

              -------------------------------------------------------------

===========================================================================*/
extern void txc_change_rf_tx_state( txc_rf_state_type state );

/*===========================================================================

FUNCTION      TXC_ISR_CMD

DESCRIPTION
  This procedure posts a command for the interrupt handler and waits for the
  signal from the interrupt handler indicating that the command was processed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void txc_isr_cmd( txc_int_cmd_type command, qword lc_mask );

#ifdef FEATURE_MODEM_1X_DEVICE_ONLY_DTX
/*===========================================================================

FUNCTION       TXC_DEVICE_ONLY_DTX_EFS_INIT

DESCRIPTION    This function initializes DTX parameters

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void txc_device_only_dtx_efs_init( void );
#endif /* FEATURE_MODEM_1X_DEVICE_ONLY_DTX */

/*===========================================================================

FUNCTION TXC_TX_WAS_ON_BEFORE_CFS

DESCRIPTION
  Returns if TX was ON prior to doing CFS.

DEPENDENCIES
  None

RETURN VALUE
  TRUE:  If TX was ON prior to doing CFS.
  FALSE: Otherwise

SIDE EFFECTS
  None
===========================================================================*/
extern boolean txc_tx_was_on_before_cfs( void );

/*===========================================================================

FUNCTION TXC_SETUP_PCG_EVENTS_ISR

DESCRIPTION
  This function sets up the m1x stmr events for the specified condition.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void txc_setup_pcg_events_isr
(
  txc_pcg_event_enum_type event_type,
  void (*pcg_event_isr)(m1x_stmr_event_type)
);

/*-------------------------------------------------------------------------*/
/* APIs to interact with MCS Co-ex Simultaneous Tranmsit Manager (STX)     */
/*-------------------------------------------------------------------------*/

typedef enum
{
  TXC_STX_OFF                   = 0,
  TXC_STX_ON,
  TXC_STX_TX_POWER,
} txc_stx_notify_enum_type;

/*===========================================================================

FUNCTION TXC_STX_NOTIFY

DESCRIPTION
  Sends a message to STX according to the notify type

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void txc_stx_notify( txc_stx_notify_enum_type stx_notify );

/*-------------------------------------------------------------------------*/
/*         API to interact with MCS Susbcription Priority Module           */
/*-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION TXC_UPDATE_SP_STATE

DESCRIPTION
  Updates the MCS Subscription Priority Module with the current 1x State.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void txc_update_sp_state( sp_tech_states_enum_t state );
#endif /*TXC_COMMON_I_H*/
