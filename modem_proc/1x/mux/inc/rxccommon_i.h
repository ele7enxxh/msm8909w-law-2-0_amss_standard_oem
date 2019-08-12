#ifndef RXCCOMMON_I_H
#define RXCCOMMON_I_H
/*===========================================================================

             C D M A    R E C E I V E   C O M M O N   M O D U L E

DESCRIPTION
  This header file contains definitions for the receive common module that
  are used by the rxc unit.  This header file is internal to the rxc unit.

  Copyright (c) 1997 - 2016 Qualcomm Technologies, Inc. 
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

$PVCSPath: O:/src/asw/COMMON/vcs/rxccommon.h_v   1.13   27 Aug 2002 09:20:18   vlechan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/mux/inc/rxccommon_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/02/16   nsh     QSH:Add API rxc_check_rx_agc_delta().
06/15/16   eye     Created get-set API for voc_config and reset voc_config
                   after receiving STANDBY_F.
05/12/16   srk     Use locally stored/passed in RC instead of global variable
                   to avoid race condition between RX Task and TC ISR.
02/05/16   eye     1x2GTA Feature changes
02/12/15   jh      Changes for SW Demback
11/23/14   eye     Added support for 0x1877 Power Reporting Log Packet.
11/12/14   eye     Added API to get and set MVS variable & reset in call end.
08/27/14   eye     Added mvs_standby variables reset after real mvs_enable.
08/21/14   eye     Added MVS_STANDBY during HHO with frame_offset change.
01/28/14   pap     Aggregating the variables into global structures.
01/08/14   pap     Resolving Strip/pack issue.
09/27/13   vks     Use rxccommon api to configure ch0 demod for sync/paging
09/13/13   pap     Adding changes for the m1x_stmr module to be integrated.
05/31/13   srk     Mainlined FEATURE_MODEM_1X_NON_SIG.
03/05/13   srk     Replace existing watchdog implementation with the new
                   Dog Heartbeat Mechanism.
11/07/12   cjb     Mainlining FEATURE_MODEM_1X_SUPPORTS_RCINIT feature.
08/13/12   eye     After PMRM is sent, Bad frame counter adjustments.
07/19/12   eye     After PMRM is sent, Total and Bad frame count adjustment.
06/15/12   srk     Removed FEATURE_IS2000_REL_A_CC_SMMC
06/05/12   srk     Externed rxc_state.
03/28/12   srk     Mainlined FEATURE_DS
02/24/12   srk     Mainlined FEATURE_IS2000_P2
02/13/12   srk     Added rxc_watchdog_init and failsafe timer.
01/17/12   jtm     Revert REL_A clean up.
01/04/12   srk     Feature Cleanup.
12/20/11   srk     Feature clean up: Mainlined FEATURE_MODEM_1X_SUPPORTS_Q6
11/22/11   srk     Feature clean up: Mainlined and Internalized Features
10/26/11   vks     Remove references to obsolete sleep* api and the code
                   associated with it.
09/28/11   srk     Mainlined Always On features and removed Always Off Features
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
03/28/11   jtm     Added capability for MSO RDA test to be enabled dynamically.
01/25/11   jtm     Added Mux Vocoder interface module.
12/15/10   jtm     Feature cleanup.
12/03/10   jtm     Removed MDR/SCCH support.
11/15/10   jtm     Initial SU API split for CP.
11/05/10   jtm     Added more _i.h includes for Modem 1x SU API support.
11/01/10   jtm     Modem 1x SU API changes.
10/30/10   jtm     Modem 1x SU API changes.
08/06/10   jtm     Removed declaration of fwd_rc_to_rate_set. Added declaration
                   of rxc_get_fwd_link_rate_set().
07/20/10   jtm/adw Added rxc_prepare_rx_to_sleep().
06/09/10   jtm     Remove temporary 1xA Log Code definitions.
05/07/10   jtm     Added API to update and retrieve F-FCH SER and FER stats.
03/30/10   vks     Use 1X specific feature - FEATURE_MODEM_1X_SUPPORTS_Q6
                   instead of FEATURE_QDSP6
02/04/10   jtm     Removed FEATURE_PLT.
01/10/10   vks     Use FEATURE_QDSP6 instead of T_QDSP6.
11/25/09   jtm     Created temporary define for LOG_1X_ADV_INFO_C.
08/22/09   jtm     Lint Fixes.
08/05/09   jtm     Lint fixes.
06/11/09   jtm     Replaced FEATURE_1X_Q6_CRIT_SECT with T_QDSP6.
04/13/09   adw     Replaced tdso.h with tdso_v.h include for CMI.
04/01/09   jtm     Added customer.h
03/19/09   jtm     Updated included headers.
03/17/09   jtm     Replaced INTLOCK/INTFREE, INTLOCK_SAV/INTFREE_SAV, and
                   MSM_LOCK/MSM_UNLOCK with macros that can be mapped correctly
                   based on the compiler and controlled internally to RXC.
11/19/08   jtm     Split various header files into internal and external
                   declarations for Common Modem Interface.
07/21/08   jtm     Added RXC_UPPER_BOUNDS_CHECK. Removed FEATURE_IS2000_REL_C_DV code.
05/03/07   cab     Moved to new sleep voting interface
04/27/05   vlc     Main lined code in current targets, cleaned out obsolete targets,
                   cleaned out obsolete features.
04/20/04   jrp     Fixed lint warnings.
03/10/04   vlc     Added support for centralized transaction engine handling
                   under feature FEATURE_TRANS_MANAGER.
03/03/04   sr      Changes for DV
08/08/03   vlc     Use dec_dcch_status_type instead of dec_fch_status_type for
                   rxc_dcch_status.
07/18/03   vlc     Release A changes:  Changed FCH and DCCH
                   channel set up to better indicate when decoder data is
                   available for the appropriate channel(s) for the RXC layer.
06/16/03   bn      Fixed the "out of buffer dec_free_q" problem.
05/05/03   bn      Added new frame skipping field into rxc_slot_type to handle SMMC
08/27/01   vlc     Changed power measurement report process for VP2.
08/08/02   vlc     Fixed compiler error when FEATURE_IS95B_MAHHO is turned off.
05/30/02   vlc     VP2 changes are now featurized under FEATURE_IS2000_REL_A_SVD
                   and FEATURE_IS2000_P2 per request of development team.
05/13/02   vlc     Added support for simultaneous DCCH & FCH channel set up
                   (feature FEATURE_IS2000_REL_A_VP2.)
02/27/02   lad     Added rxc_clear_decoder_queue().
02/25/02   bn      Added support for  MUX SVD signaling
10/30/01   bn      Added support for Release A FDCH logging
10/17/01   lad     Added inclusion of atmr.h and mccrx.h.
08/15/01   sr      Put back prototype for rxc_init_action_item(); it is called
                   in rxcpaging.c
08/02/01   lcc     Removed function prototypes for functions used only in
                   traffic channel.
06/15/01   day     Mainlined FEATURE_IS95B, FEATURE_IS2000
04/20/01   sr      Merged in from MSM5100 archive
04/18/01   sr      Merged with MSM_MUX1X.01.00.00
           lcc     Added support for FEATURE_IS2000_P2
01/29/01   bgc     Moved rxc_scr_trans_cmd() to rxctraffic.c
11/10/00   jc      Created after repartition of rxc.c

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
#include "rxc.h"

//drivers
#include "dec5000.h"
#include "dec5000_v.h"
#include "deint_i.h"

//cp
#include "tdso_v.h"
#include "mccrx.h"
#include "mccdma.h"
#include "mccdma_v.h"
#include "mccdma_i.h"

//other
#include "rex.h"
#include "dog.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* TCB for RX task */
extern rex_tcb_type* rx_tcb_ptr;

/* RCInit version */
#define RX_TCB_PTR ( rx_tcb_ptr )

#define RX_DBG_MSG( ... ) \
    M1X_MSG( MUX, DBG_RX, __VA_ARGS__ )

/* Command block for Interrupt handlers */
typedef struct
{
  q_link_type      link;   /* Queue links */
  rxc_cmd_name_type cmd;   /* Command to be processed */
  word sci_mask;           /* current sci_mask */
  word idle_pgslot;        /* assigned paging slot */
} rxc_isr_cmd_type;

typedef struct
{
  /* Queues */

  q_type          rxc_cmd_q;
    /* Queue holding commands for the Receive task */

  q_type          rxc_dec_q;
    /* Queue for signaling data packets received from the decoder */

  q_type          rxc_dec_free_q;
    /* Queue for free buffers to be placed, when filled,
       on the rxc_dec_q queue */

  q_type          rxc_isr_q;
    /* Queue for command packets to be processed by the Receive interrupt
       handler */

  q_type          rxc_isr_free_q;
  /* Queue for free buffers to be placed, when filled,
       on the rxc_isr_q queue */

  byte rxc_frame_offset;
    /* Stores the current frame offset of the forward link traffic
       channel */
  byte rxc_frame_offset_countdown;
    /* Used as a counter to determine of rxc_dec_frame needs to be recomputed */

  rxc_state_type rxc_state;     /* State of RXC state machine */


  byte rxc_rate;        /* Command buffer */

  rxc_isr_cmd_type *rxc_isr_cmd_buf_ptr;
} rxc_common_vars_type;

extern rxc_common_vars_type rxc_common_data;

/* declare a critical section that is common throughout the RX task */
extern rex_crit_sect_type rxc_crit_sect;

/* This structure is used to update the current frame F-SCH demod statistics */
typedef struct
{
  uint8  sch_rate;                   /* SCH decoder rate i.e 1x,2x,4x,8x,16x,32x */
  uint8  decision;                   /* Good,DTX,Bad,No -> SCH frames */
  uint16 iq_acc;                     /* IQ accumulation over a frame */
  uint16 rt_nt_io;                   /* Weighted sqrt(Nt/Io) over a frame */
  uint16 rt_ecp_io;                  /* Weighted pilot magnitude accumulation over a frame */
  uint8  code_type;                  /* Conv->0 and Turbo->1 */
  uint8  padding;
  uint16 ffpc_setpoint;              /* F-SCH Current FPC SetPoint in dB8 */
  uint32 sch_energy;                 /* Conv-16bit,Turbo-21 bit energy metric */
  uint32 total_frames;               /* Total number of SCH frames */
  uint32 erasure_frames;             /* Total number of SCH erasure frames */
  uint32 dtx_frames;                 /* Total number of SCH DTXed frames */
  uint32 rate_count[6];              /* 0-1x,1-2x,2-4x,3-8x,4-16x and 5-32x */
  union
  {
    struct
    {
      uint16 conv_ser;               /* Convolutional symbol error rate -13 bit */
      uint16 conv_crc;               /* Logical Transmission Unit , CRC mask */
    } conv_sch_stats;
    struct
    {
      uint8  turbo_iteration;        /* Turbo decoder iterations (0 to 15) */
      uint8  turbo_crc;              /* 0->Fail and 1->Pass */
      uint16 turbo_min_llr;          /* Log Likelihood Ratio, 14 bits */
    } turbo_sch_stats;
  } dec_stats;
} rxc_sch_demod_log_type;

/* This structure is used to update SRCH and MUX power related stats */
typedef struct
{
  uint64  ustmr_ref;                 /* USTMR time reference */ 
  uint8   num_fing;                  /* Number of fingers ( 0-8 ) */
  uint8   aset_size;                 /* Active Set size ( 0-6 ) */
  uint8   rxd_enabled;               /* Receive Diversity enabled */
  int8    tx_pwr;                    /* Tranmsit Power in steps of 0.5dBm */
  uint8   num_drx_pcg;               /* Number of DRX PCG's in current RX frame */
  uint8   fl_frame_smartblanked;     /* FL Frame Smart Blanking */
  uint8   dtx_percentage;            /* 0 (no DTX), 25% (1) or 50% (2) gating */
  uint8   rl_frame_smartblanked;     /* RL Frame Smart Blanking */
  uint8   fl_fet_num_pcg;            /* Number of early terminated PCG's in FL frame */
  uint8   rl_fet_num_pcg;            /* Number of early terminated PCG's in RL frame */
} rxc_tx_srch_pwr_log_type;

extern rxc_tx_srch_pwr_log_type rxc_srch_mux_pwr_stats;

#define LOGGING_FRAME_OFFSET_SB_DTX 2

/* Delay adjustment structure for Rev Link params */
typedef struct
{
  uint8  sb_decision[LOGGING_FRAME_OFFSET_SB_DTX];
  uint16 dtx_mask[LOGGING_FRAME_OFFSET_SB_DTX];
} rxc_rev_sb_dtx_param_type;

extern rxc_rev_sb_dtx_param_type rxc_tx_sb_dtx_param;

/* Indicator of whether at the traffic entry MVS is already enabled or not */
extern boolean rxc_traffic_entry_mvs_enabled;

/* Indication to MC informing MVS_STANDBY in progress or not */
extern boolean mvs_cdma_standby_in_progress;

/* Status of MVS_STANDBY operation received from MC task */
extern mc_mvs_standby_enum_type mvs_standby_status;

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
  | No   Yes  -   | Use RXC_ENTER_ATOMIC_SECT()                           |
  |-----------------------------------------------------------------------|
  | Yes  No   No  | Use RXC_ENTER_ISR_SECT()                              |
  |-----------------------------------------------------------------------|
  | Yes  No   Yes | Use RXC_ENTER_CRIT_SECT()                             |
  |-----------------------------------------------------------------------|
  | Yes  Yes  No  | Use RXC_ENTER_ATOMIC_CRIT_SECT()                      |
  |-----------------------------------------------------------------------|
  | Yes  Yes  Yes | Use RXC_ENTER_ATOMIC_ISR_SECT()                       |
  \-----------------------------------------------------------------------/

 *Note* - For sections that are both time critical and a shared resource
          use the appropriate critical section macro with the time critical
          macro nested inside.
*/
#define RXC_ENTER_ATOMIC_SECTION()    REX_DISABLE_PREMPTION()
#define RXC_LEAVE_ATOMIC_SECTION()    REX_ENABLE_PREMPTION()

#define RXC_ENTER_ISR_SECTION()       REX_ISR_LOCK( &rxc_crit_sect )
#define RXC_LEAVE_ISR_SECTION()       REX_ISR_UNLOCK( &rxc_crit_sect )

#define RXC_ENTER_CRITICAL_SECTION()  rex_enter_crit_sect( &rxc_crit_sect )
#define RXC_LEAVE_CRITICAL_SECTION()  rex_leave_crit_sect( &rxc_crit_sect )

#define RXC_ENTER_ATOMIC_ISR_SECTION()      \
    REX_ISR_LOCK( &rxc_crit_sect );         \
    REX_DISABLE_PREMPTION()

#define RXC_LEAVE_ATOMIC_ISR_SECTION()      \
    REX_ENABLE_PREMPTION();                 \
    REX_ISR_UNLOCK( &rxc_crit_sect )

#define RXC_ENTER_ATOMIC_CRIT_SECT()        \
    rex_enter_crit_sect( &rxc_crit_sect );  \
    REX_DISABLE_PREMPTION()

#define RXC_LEAVE_ATOMIC_CRIT_SECT()        \
    REX_ENABLE_PREMPTION();                 \
    rex_leave_crit_sect( &rxc_crit_sect )


/*-------------------------------------------------------------------------*/
/*  Forward Traffic Channel Power Control Parameters                       */
/*-------------------------------------------------------------------------*/

typedef struct
{
  boolean threshold_on;
    /* Threshold report mode indicator */
  boolean periodic_on;
    /* Periodic report mode indicator */
  word delay_frames;
    /* Number of frames to delay after a Power Measurement message is
       sent before recording */
  word pwr_thresh;
    /* Threshold for bad frames before Power Measurement message is sent */
  word pwr_frames;
    /* Maximum number of frames before a Power Measurement message is sent */
  word num_bad_frames;
    /* Number of category 9 and 10 frames received */
  word num_bad_frames_current;
    /* Snapshot of number of category 9 and 10 frames received when Power
       Measurement Report is provided to CP */
  word num_frames;
    /* Total number of frames received */
  word skipped_num_frames;
    /* Total number of frames skipped during TA GAP */
  word num_frames_current;
    /* Snapshot of total number of frames received when Power Measurement
       Report is provided to CP */
#ifdef FEATURE_IS2000_REL_A_SVD
  word dcch_num_bad_frames;
    /* For VP2 only, num_bad_frames applies to FCH and dcch_num_bad_frames
    ** applies to DCCH.  This is done for both "DCCH only" and "FCH & DCCH"
    ** scenarios.
    */
  word dcch_num_bad_frames_current;
    /* For VP2 only, num_bad_frames_current applies to FCH and
    ** dcch_num_bad_frames_current applies to DCCH.  This is done for both
    ** "DCCH only" and "FCH & DCCH"scenarios.(Snapshot while sending PWR RPT to CP)
    */
  word dcch_num_frames;
    /* For VP2 only, num_frames applies to FCH and dcch_num_frames
    ** applies to DCCH.  This is done for both "DCCH only" and "FCH & DCCH"
    ** scenarios.
    */
  word skipped_dcch_num_frames;
    /* Total number of DCCH frames skipped during TA GAP */
  word dcch_num_frames_current;
    /* For VP2 only, num_frames_current applies to FCH and dcch_num_frames_current
    ** applies to DCCH.  This is done for both "DCCH only" and "FCH & DCCH"
    ** scenarios.(Snapshot while sending PWR RPT to CP)
    */
#endif /* FEATURE_IS2000_REL_A_SVD */

  word num_delay_frames;
    /* Number of frames delayed */

  word sch_tot_frames[2]; /* for SCH0 and SCH1 */
  word skipped_sch_tot_frames[2];
    /* Skipped frames for SCH0&1 during TA GAP*/

  word sch_bad_frames[2]; /* for SCH0 and SCH1 */
  boolean sch_fer_rep[2]; /* for SCH0 and SCH1 */

}  rxc_pwr_type;


/*-------------------------------------------------------------------------*/
/* Data structures and variables for RXC ACTION TIME PROCESSING            */
/*-------------------------------------------------------------------------*/

typedef struct rxc_action_list_item_struct
{
  rxc_cmd_msg_type action_cmd;
  qword action_time;
  struct rxc_action_list_item_struct *prev_ptr;
  struct rxc_action_list_item_struct *next_ptr;
} rxc_action_list_item_type;

/*-------------------------------------------------------------------------*/
/* Service Option variables                                                */
/*-------------------------------------------------------------------------*/

typedef struct
{
  dec_rate_set_type rate_set;           /* rate set in use */
  struct
  {
    boolean included;               /* Indicates if channel is included */
    #ifdef FEATURE_IS2000_REL_A_SVD
    boolean data_valid;             /* Indicates when data is valid */
    word data_valid_countdown;      /* Counts down when data is valid */
    #endif /* FEATURE_IS2000_REL_A_SVD */
    cai_radio_config_type rc;       /* RC of channel */
  }
  for_fch, for_dcch;
  boolean high_rc;         /* Indicate if RC 3 or above */
  word pri;                /* Primary service option */
  word sec;                /* Secondary service option */
  boolean enable_skip_tci;   /* disables check for good frames in TCI */
  byte transaction_num;  /* transaction number for action times */

#ifdef FEATURE_IS2000_REL_A /* copy the SCH MUX OPTION from high layer to the mux for FDCH logging */
  word for_sch_mux_option;
#endif /* FEATURE_IS2000_REL_A */
} rxc_so_type;

/* function prototype for transaction callback function */
typedef void (*transaction_processor_type) (mctrans_action_packet_type *);

/*-------------------------------------------------------------------------*/
/* Variables for counting slots                                            */
/*-------------------------------------------------------------------------*/
typedef struct
{
  boolean active;     /* indicator of whether we are checking slots */
  boolean valid_msg;  /* indicator of whether a valid msg was received */
  word slot_frames;   /* number of frames remaining in the 2 slot period */
  word slot_now;      /* current slot */
  byte slot_mask;     /* the slot mask indicating the type of slot(s) RXC is about to enter */
  byte valid_msg_mask; /* the type of slots that valid msg reporting is for */
  word bcpg_slot_frames; /* number of frames remaining in the broadcast page slots */
  word bcsms_slot_frames; /* number of frames remaining in the broadcast SMS slots */
  boolean bc_started; /* the scheduled BC has started in the first slot */
  word slot;          /* assigned paging slot modulo sci_mask */
  word idle_pgslot;   /* Hashed paging channel slot */
  word sci_mask;      /* Mask to get position-in-slot from pgslot */
  boolean after_slot_boundary;
  boolean skipping;   /* Decoder interrupts skipped until 20ms after slot */
  boolean page_match; /* Enable page matching for first frame after slot */
  boolean blank;      /* Enable blank for first *int* (not frame) aft slot */
  byte frame_skip_cnt;/* Count used to know when to jump out of skipping state */
} rxc_slot_type;


#define RXC_TIMER_NOT_SET -1

extern rex_timer_type  rxc_fade_timer;
  /* Timer for amount of time since mobile last detected a valid SAT while
     communicating with the base station over the voice channel */

extern rex_timer_type rxc_aho_fade_timer;
  /* This timer is used for T72M timer monitoring */

extern rex_timer_type rxc_failsafe_timer;

extern dec_fch_status_type rxc_fch_status;

#ifdef FEATURE_IS2000_REL_A_SVD
/* Used in simultaneous F-FCH and F-DCCH channel set up.
*/
extern dec_dcch_status_type rxc_dcch_status;
#endif /* FEATURE_IS2000_REL_A_SVD */

extern byte  rxc_zero_count; /* Indicator of whether SER statistics should be set to
                        zero - anything other than 0 = reset */

/*-------------------------------------------------------------------------*/
/* Interrupt Service Routine variables */
/*-------------------------------------------------------------------------*/

/* Frame number */
extern qword  rxc_dec_frame;

/* logging AGC values */
extern byte log_rx_agc;
extern byte log_tx_pwr;
extern byte log_tx_gain_adj;

/*  Forward Traffic Channel Power Control Parameters  */

extern rxc_pwr_type rxc_pwr;

/* Service Option variables */

extern rxc_so_type rxc_so;

#ifdef FEATURE_IS2000_REL_A_SVD
#define RXC_SR_ID_MAX   8
typedef struct
{
   word   traffic_type;         /* 0 for primary, 1 for secondary */
   word   so;                   /* service option */
} rxc_srid_ref_type ;

extern rxc_srid_ref_type  rxc_srid_ref[RXC_SR_ID_MAX];
#endif /* FEATURE_IS2000_REL_A_SVD */

/*-------------------------------------------------------------------------*/
/* Variables for counting slots                                            */
/*-------------------------------------------------------------------------*/
extern rxc_slot_type rxc_slot;

extern word rxc_log_msg_drop;     /* number of messages dropped due to no
                                 available buffer */

/* Indicator to ignore good frame while doing a Return-If-Fail HHO      */
extern boolean rxc_ignore_good_frame;

extern q_type rxc_action_item_free_q;

extern tdso_frame_block_type rxc_tdso_frame_block;

#define RXC_UPPER_BOUNDS_CHECK(index, array_size) \
          (((uint32) (index) < (uint32) (array_size)) ? (uint32) (index):((uint32) (array_size)-1))

/* dog parameters */
extern dog_report_type rxc_dog_rpt_id;

#ifdef FEATURE_1XCP_QSH_SUPPORT
#define RXC_RX_AGC_DIFF_ALLOWED                  10
#define RXC_CONSEC_PAGE_DEC_FAIL                  3
#endif /* FEATURE_1XCP_QSH_SUPPORT */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#ifdef FEATURE_1XCP_QSH_SUPPORT
/*===========================================================================

FUNCTION RXC_CHECK_RX_AGC_DELTA

DESCRIPTION
  This function checks Rx AGC delta between pri and sec channel. if diff is 
  greater than RXC_RX_AGC_DIFF_ALLOWED, it will log it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_check_rx_agc_delta ( void );
#endif /* FEATURE_1XCP_QSH_SUPPORT */

/*===========================================================================

FUNCTION RXC_DOG_REPORT

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
void rxc_dog_report ( void );

/*===========================================================================

FUNCTION RXC_WATCHDOG_INIT

DESCRIPTION
  This function is used to initialize the RX task with the DOG subsystem

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  WDOG timer system is started for RX task
===========================================================================*/
void rxc_watchdog_init( void );

/*===========================================================================

FUNCTION RXC_EXIT

DESCRIPTION
  This function performs processing necessary when leaving the RX CDMA
  subtask

DEPENDENCIES
  none.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_exit( void );

/*========================================================================

FUNCTION RXC_CONFIG_DEMOD_FOR_SYNC

DESCRIPTION This function setups demod channel 0 for sync

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
extern void rxc_config_demod_for_sync( void );

/*========================================================================

FUNCTION RXC_CONFIG_DEMOD_FOR_PAGING

DESCRIPTION This function setups demod channel 0 for paging

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
extern void rxc_config_demod_for_paging( void );

/*========================================================================

FUNCTION RXC_CONFIG_DEMOD_FOR_TRAFFIC

DESCRIPTION This function setups demod channel 0 for traffic

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
extern void rxc_config_demod_for_traffic( deint_rc_type rc );

#ifdef FEATURE_IS2000_REL_A_SVD
/*========================================================================

FUNCTION RXC_CONFIG_DEMOD_CH1

DESCRIPTION This function configures the demodulator and combiner for
            Channel 1 based on Radio Configuration.

DEPENDENCIES ??

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
extern void rxc_config_demod_ch1 ( deint_rc_type rc );

/*========================================================================

FUNCTION RXC_SET_DCCH_MODE

DESCRIPTION This function handles the process of setting up DCCH.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
extern void rxc_set_dcch_mode
(
  deint_rc_type dcch_rc,
  boolean power_control_channel
);

/*========================================================================

FUNCTION RXC_SET_FCH_MODE

DESCRIPTION This function handles the process of setting up FCH.

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
extern void rxc_set_fch_mode
(
  deint_rc_type fch_rc,
  boolean power_control_channel
);

#endif /* FEATURE_IS2000_REL_A_SVD */

/*===========================================================================

FUNCTION RXC_MC_RPT

DESCRIPTION
  This function generates a report to the Main Control task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_mc_rpt(mccrx_rpt_type *rpt_ptr);


/*===========================================================================

FUNCTION RXC_ISR_CMD

DESCRIPTION
  This procedure posts a command for the interrupt handler, continues to
  kick the watchdog while waiting for a response and returns the response.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_isr_cmd( rxc_isr_cmd_type *cmd_ptr );

/*===========================================================================

FUNCTION RXC_FLAG_ERASURE

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_flag_erasure( boolean erasure_flag );

/*===========================================================================

FUNCTION SEND_PWR_RPT

DESCRIPTION
  This function sends a power report to the Main Control task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void send_pwr_rpt( void );

/*===========================================================================

FUNCTION RXC_WAIT

DESCRIPTION
  This function continues to reset the Watchdog signal and timer while
  waiting for any signal in a specified signal mask to be set. If this
  procedure detects that any signal in the specified signal mask is set,
  it will return the entire mask of signals set in the task'stcb.

DEPENDENCIES
  None.

RETURN VALUE
  Mask of signals returned by the rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/
extern rex_sigs_type rxc_wait( rex_sigs_type wait_sigs );

/*========================================================================

FUNCTION RXC_CLEANUP_SCH_MUX

DESCRIPTION This function cleansup the array that is used to hold
            the SCH MUX frames

DEPENDENCIES None

RETURN VALUE None

SIDE EFFECTS None

=========================================================================*/
extern void rxc_cleanup_sch_mux( void );

/*===========================================================================
FUNCTION RXC_RESET_FRAME_COUNT

DESCRIPTION
  This function resets the frame counters of valid message reporting

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  Some RXC global counters will be changed.

===========================================================================*/
extern void rxc_reset_frame_count( void );

/*==========================================================================
FUNCTION RXC_SC_ser_ACCUM

DESCRIPTION
  This function accumulates the status information for SYNC Channel.
  The status information includes CRC/Quality, Energy Metrics and SER

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void rxc_sc_ser_accum( void );

/*===========================================================================

FUNCTION RXC_CLEAR_DECODER_QUEUE

DESCRIPTION
  This function clears the RXC decoder queue by placing all items on the
  decoder free queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxc_clear_decoder_queue( void );

/*===========================================================================

FUNCTION RXC_CLEAR_ISR_CMD_QUEUE

DESCRIPTION
  This function clears the RXC ISR COMMAND queue by placing all items on the
  ISR COMMAND free queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void rxc_clear_isr_cmd_queue( void );

/*===========================================================================

FUNCTION RXC_UPDATE_FER_SER_COUNTS

DESCRIPTION
  Update the FER and SER report statistics based on the fch_status and
  determined rate.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rxc_update_fer_ser_counts
(
  dec_fch_status_type fch_status,
  deint_rc_type       curr_rc,
  cai_data_rate_type  frame_rate
);

/*===========================================================================

FUNCTION       RXC_PREPARE_RX_TO_SLEEP

DESCRIPTION    This function performs the necessary preparations for placing
               MUX into sleep state.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   Decoder clocks will be disabled.

===========================================================================*/
void rxc_prepare_rx_to_sleep( void );

/*========================================================================

FUNCTION RXC_GET_FWD_LINK_RATE_SET

DESCRIPTION This function returns current rate set used on the forward
            linke based on the RC provided.
            This is used by the UI in the data display.

            PARAMETERS:
            rc = the current RC of a FL traffic channel.

DEPENDENCIES None

RETURN VALUE
            RC 1, 3, 4, 11
            0 = RS1 ->  9600 bps

            RC 2, 5
            1 = RS2 -> 14400 bps

            RC invalid
            default to RS1.

SIDE EFFECTS None

=========================================================================*/
dec_rate_set_type rxc_get_fwd_link_rate_set(cai_radio_config_type rc );

/*===========================================================================

FUNCTION RXC_IS_MSO_RDA_TEST_ENABLED

DESCRIPTION
  Return the value of the Markov Service Option test flag. This flag indicates
  if the extended test capabilities for this service option are enabled.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if test feature is enabled, FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean rxc_is_mso_rda_test_enabled( void );

/*===========================================================================

FUNCTION rxc_get_sch_stats

DESCRIPTION
  This function updates various F-SCH parameters used for logging purposes.
  Such as Channel params,Frame stats,Counters,Decoder specific params etc..

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_get_sch_stats( rxc_sch_demod_log_type *rxc_srch_fsch_stats );

/*===========================================================================

FUNCTION rxc_get_pwr_stats

DESCRIPTION
  This function updates various power related parameters used for logging purposes.
  Such as Tx pwr, SB, FET, DRX, DTX etc

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_get_pwr_stats
(
  cai_radio_config_type    curr_rc,
  rxc_tx_srch_pwr_log_type *rxc_tx_srch_pwr_stats
);

/*===========================================================================

FUNCTION rxc_tx_update_flush_pwr_log

DESCRIPTION
  This function updates and flushes pwr log param for 0x1877 every 20ms

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_tx_update_flush_pwr_log ( void );

/*===========================================================================

FUNCTION RXC_MVS_SET_STANDBY_PROGRESS_STATUS

DESCRIPTION
  This function sets the mvs_cdma_standby_in_progress variable with desired 
  value ( TRUE or FALSE )

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_mvs_set_standby_progress_status( boolean standby_progress_status );

/*===========================================================================

FUNCTION RXC_MVS_GET_STANDBY_PROGRESS_STATUS

DESCRIPTION
  This function gets the value mvs_cdma_standby_in_progress variable

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean rxc_mvs_get_standby_progress_status( void );

/*===========================================================================

FUNCTION RXC_MVS_SET_STANDBY_STATUS

DESCRIPTION
  This function sets the mvs_standby_status variable with desired 
  value ( MVS_STANDBY_SUCCESS or MVS_STANDBY_FAIL )

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_mvs_set_standby_status( mc_mvs_standby_enum_type standby_status );

/*===========================================================================

FUNCTION RXC_MVS_GET_STANDBY_STATUS

DESCRIPTION
  This function gets the value mvs_standby_status variable

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern mc_mvs_standby_enum_type rxc_mvs_get_standby_status( void );

/*===========================================================================

FUNCTION RXC_SET_MVS_STATUS_ON_TC_ENTRY

DESCRIPTION
  This function sets the rxc_traffic_entry_mvs_enabled variable with desired 
  value ( TRUE or FALSE ).To know if MVS is enabled already on traffic entry or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_set_mvs_status_on_tc_entry( boolean mvs_status );

/*===========================================================================

FUNCTION RXC_GET_MVS_STATUS_ON_TC_ENTRY

DESCRIPTION
  This function gets the value rxc_traffic_entry_mvs_enabled variable

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean rxc_get_mvs_status_on_tc_entry( void );

/*===========================================================================

FUNCTION RXC_SET_VOC_CONFIG_DONE

DESCRIPTION
  This function sets the rxc_frame_data.rxc_voc_config_done variable with desired
  value ( TRUE or FALSE ).To know if vocoder is already configured or not..

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void rxc_set_voc_config_done ( boolean config_done );

/*===========================================================================

FUNCTION RXC_GET_VOC_CONFIG_DONE

DESCRIPTION
  This function gets the value rxc_frame_data.rxc_voc_config_done variable

DEPENDENCIES
  None

RETURN VALUE
  rxc_frame_data.rxc_voc_config_done

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean rxc_get_voc_config_done( void );

/*===========================================================================

FUNCTION RXC_USE_SW_DEMBACK

DESCRIPTION
  This function returns TRUE if the HW demback is used by another RAT,
  thereby needing to use the SW implementation.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - When SW demback needs to be used.
  FALSE - When HW demback is available

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean rxc_use_sw_demback(void);

#endif /* RXCCOMMON_I_H */
