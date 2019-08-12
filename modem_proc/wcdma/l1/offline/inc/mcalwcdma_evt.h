#ifndef MCALWCDMA_EVT_H
#define MCALWCDMA_EVT_H

/*============================================================================*/
/** @file
 * This module has definition and declaration related to MCAL UMTS downlink
 * channel setup module APIs
 */
/*============================================================================*/

/*============================================================================
Copyright (c) 2007 - 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/mcalwcdma_evt.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
03/10/15    pr      Reset mcal array with default evts as part of stmr deact init
10/13/14    pr      Deprecate RX_GEN_DEC_HDR_READ_EVENT and add a new event for comb chan cfg.
10/09/14    sr      Changes to increase gen TL events
08/06/14    sad     New macro for zero AGC settling time
07/18/14    sad     RF api changes and inter freq tune changes
07/01/14    hk      Header file inclusion cleanup
05/13/14    ymu     Feature name FEATURE_WCDMA_DC_HSUPA_FRAME replacement with FEATURE_WCDMA_DC_HSUPA 
03/06/14    pr      L1 support for MVS voice packet exchange.
10/02/13    pr      VSTMR changes for EUL FIQ
09/14/13    pr      VSTMR Changes for BOLT
01/09/14    pr      Code changes for UL Compression Feature
11/27/13    pr      L1 changes for FR1782 - Uplink Compression for WCDMA
09/12/13    jd      Searcher redesign
05/28/13    gsk     Adding a function to return the fire time of a evt on gen tl
05/22/13    oh      Adding DC-HSUPA framework support
05/09/13    gp      Code cleanup for FEATURE_WCDMA_UL_CM_INTLV_WORKAROUND and FEATURE_WCDMA_CONCURRENT_R99_EUL_TX_ENCODE
02/14/13    gsk     Externing fucntions for saving and restoring the TL events
09/12/12    sr      Changes to handle FDPCH deactivation cmd while executing
                    FDPCH activation.
08/07/12    dp      W2L PSHO time improvements
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
05/22/12    pr      STMR changes for Dime
07/05/12    pv      New handlers used for Offline PICH/Parallel Reacq.
06/08/12    gv      Feature HS RACH Initial code check in
06/07/12    vs      MBMS feature cleanup.
05/23/12    dp      Parallelized the gl1 init process when wl1 boots up
05/15/11    stk     Removing L1 solution to mitigate diversity shutdown transients on primary
03/27/11    srk     Created a new event for RF_tune_done_evt 
02/10/12    rgn     Nikel feature cleanup
02/01/12    gnk     Nikel Feature cleanup.
01/30/12    vs      Feature cleanup.
01/20/12    zr      Mainlined FEATURE_CELL_FACH_MEAS_OCCASION and FEATURE_8220_WCDMA_SW
12/02/11    stk     Added support for FEATURE_WCDMA_RXD_PCH and FEATURE_IDLEMODE_FILTER
11/29/11    gnk     Removed some unused features
03/30/11    gnk     Mainlined several features
03/18/11    hk      Support to block the interrupts when combiner config is in progress
03/11/11    dp      Ported CPC DRX changes to offline Architecture
09/02/10    gv      Code Checkin for CPC DTX
05/25/09    hk      Fixed compiler warnings
04/17/09    rgn     Fixed compiler warnings
03/02/09    vc      Removed f3 logging for mcalwcdma events.
12/10/08    ks      Mainlining FEATURE_WCDMA_DL_ENHANCED
10/03/08    hk      Bringing in 7k Mailine fixes
01/25/08    vsr     Checking in MCAL files onto mainline
11/05/07    vb      removed SEQ_CELL_TRANS_ALIGN_EVENT usage under DL enhanced feature
06/12/07    mc      Added MICH config event to general timeline.
05/17/07    vsr     Removed unused events SEQ_DCH_SFN_UPDATE_EVENT and 
                    SEQ_DCH_CFN_UPDATE_EVENT
04/02/07    vb      modifications to name changes in mcal evt module
03/05/07    mc      Mainlined FEATURE_WCDMA_HSUPA_ETFCI_0

===========================================================================*/
/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* map the sequencer interrupt sources to their appropriate STMR interrupt */
#include "wcdma_variation.h"
#include "stmr.h"
#include "l1const.h"
#include "srchhbevt.h"


/* Gen timeline events */
#define mcalwcdma_evt_non_dch_sfn_cfn_update_event_handler seq_non_dch_sfn_cfn_update_event_handler
#define mcalwcdma_evt_generic_event_handler seq_generic_event_handler
#define mcalwcdma_evt_rf_tune_done_event_handler seq_rf_tune_done_event_handler

#define mcalwcdma_evt_l1_cm_state_update l1_cm_state_update
#define mcalwcdma_evt_pich_demod_event_handler seq_pich_demod_event_handler

#define mcalwcdma_evt_drx_sccpch_start_event_handler drx_sccpch_start_event_done_handler

#define mcalwcdma_evt_drx_opts_svc_event_handler drx_opts_svc_handler

#define mcalwcdma_evt_early_rf_shutdown_event_handler drx_early_rf_shutdown_handler

#define mcalwcdma_evt_srchfach_sccpch_evt_handler srchfach_sccpch_evt_handler

#define mcalwcdma_evt_non_dch_fdpch_frame_evt_handler e_ul_set_eul_sw_local_cfn

#define mcalwcdma_evt_activate_non_dch_fdpch  dlphch_activate_hs_rach_fdpch_evt_handler

/* rx timeline events */
//#define mcalwcdma_evt_dl_tx_gen_set_mod_timing_event_handler dl_tx_gen_set_mod_timing_event_handler
#define mcalwcdma_evt_srch_heartbeat_event_handler srch_heartbeat_event_handler_sched
#define mcalwcdma_evt_dl_trblk_header_read_event_handler dl_trblk_header_read_event_handler
#define mcalwcdma_evt_dl_trblk_data_read_event_handler dl_trblk_data_read_event_handler
#define mcalwcdma_evt_dl_phychan_comb_chan_event_handler(evt) mcalwcdma_dl_phychan_comb_chan_event_handler(evt)
#define mcalwcdma_evt_hs_dl_comb_chan_event_handler hs_dl_comb_chan_event_handler

#define mcalwcdma_evt_dl_dch_maint_event_handler dl_dch_maint_event_handler
#define mcalwcdma_evt_dl_acq_pullin_event_handler dl_acq_pullin_event_handler

#define mcalwcdma_evt_dl_rxd_state_update_event_handler rxd_state_update_event_handler

/* tx timeline events */
#define mcalwcdma_evt_enc_pc_enable enc_pc_enable
#define mcalwcdma_evt_e_ul_edpch_cfg_event_handler e_ul_edpch_cfg_event_handler
#ifdef FEATURE_WCDMA_DC_HSUPA 
#define mcalwcdma_evt_e_ul_sec_edpch_cfg_event_handler e_ul_sec_edpch_cfg_event_handler
#endif
#ifdef FEATURE_WCDMA_HS_RACH
#define mcalwcdma_evt_enc_get_data  wenc_phychan_get_data
#define mcalwcdma_evt_hs_rach_cfg_evt_handler wl1_hs_rach_cfg_evt_handler
#else
#define mcalwcdma_evt_enc_get_data enc_get_data
#endif

#ifdef FEATURE_WCDMA_HS_RACH
#define mcalwcdma_evt_tx_gen_set_mod_timing_evt_handler wenc_phychan_set_mod_timing
#else
#define mcalwcdma_evt_tx_gen_set_mod_timing_evt_handler ul_tx_gen_set_mod_timing_event_handler
#endif

#define mcalwcdma_evt_ul_frame_boundry_event_handler ul_frame_boundry_event_handler

#define mcalwcdma_evt_enc_test_event enc_test_event

#define mcalwcdma_evt_hs_ul_chan_event_handler hs_ul_chan_event_handler

#define mcalwcdma_evt_enc_ul_maint_event_handler enc_ul_maint_event_handler

#define mcalwcdma_register_for_modem_timing wl1_register_for_modem_timing

/* WHL1 wrapers */
#define mcalwcdma_evt_seq_generic_evt_configure seq_generic_evt_configure

#define l1_tx_gen_tl_event_array mcalwcdma_evt_tx_tl_event_array

#define MCALWCDMA_EVT_GEN_TL_INTR STMR_INT_1
#define MCALWCDMA_EVT_TX_TL_INTR  STMR_INT_2
#define MCALWCDMA_EVT_RX_TL_INTR  STMR_INT_0
#define MCALWCDMA_EVT_EUL_TL_INTR STMR_INT_3
#define MCALWCDMA_EVT_NUM_INTR    STMR_NUM_INT


/* RF Tune Time in 256 Chip units - set to 3 ms=45*256 chipx1 units */
#define RF_TUNE_TIME   20
#define ZERO_RF_TUNE_TIME 0

#define MCALWCDMA_EVT_EVENT_ID_TO_MASK(event) ((uint16) (1 << ((uint16)event)))

#ifdef FEATURE_WCDMA_CPC_DTX
/* This macro is defined to configure DTX parameters to MDSP */
#define TX_GEN_DTX_CFG_EVENT_SLOT  5
#endif

/** event handler callback functionm type declaration */
typedef void (*MCALWCDMA_GEN_EVENT_CB_TYPE)(
  /** Event Id of this callback */
  uint8);

typedef struct
{
  /** is the evt_id in use?   */
  boolean in_use;

  /** evt handler */
  MCALWCDMA_GEN_EVENT_CB_TYPE evt_cb_func;

}mcalwcdma_gen_evt_id_mapping_info_struct_type;

/* Events on General Timeline */
typedef enum
{
  SEQ_NON_DCH_SFN_CFN_UPDATE_EVENT,
  SEQ_CM_UPDATE_EVENT,

  /* CELL_FACH SCCPCH frame event for inter-RAT and inter-F meas */
  SEQ_SRCH_FACH_SCCPCH_EVENT,
  
  SEQ_PICH_DEMOD_EVENT,
  
  SEQ_DRX_OPTS_SVC_EVENT,

  SEQ_EARLY_RF_SHUTDOWN_EVENT,

  /* RF Tune Done Event */
  SEQ_RF_TUNE_DONE_EVENT,

  SEQ_DRX_SCCPCH_START_EVENT,

  SEQ_NON_DCH_FDPCH_FRAME_EVENT,

  SEQ_GENERIC_EVENT,
  SEQ_GENERIC_EVENT0,
  SEQ_GENERIC_EVENT1,
  SEQ_GENERIC_EVENT2,
  SEQ_GENERIC_EVENT3,
  SEQ_NUM_GEN_TL_EVENTS,

} seq_gen_tl_event_id_enum_type;

#define MCALWCDMA_GEN_TL_EVT_AVAIL_POOL_START_IDX  SEQ_GENERIC_EVENT0
#define MCALWCDMA_GEN_TL_EVT_AVAIL_POOL_END_IDX  SEQ_GENERIC_EVENT3
#define MCALWCDMA_GEN_TL_EVT_AVAIL_POOL \
                       (MCALWCDMA_GEN_TL_EVT_AVAIL_POOL_END_IDX - MCALWCDMA_GEN_TL_EVT_AVAIL_POOL_START_IDX + 1)
#define MCAL_EVT_VALIDATE_GEN_EVT_ID(evt) \
       ((evt >= MCALWCDMA_GEN_TL_EVT_AVAIL_POOL_START_IDX) && \
        (evt <= MCALWCDMA_GEN_TL_EVT_AVAIL_POOL_END_IDX)) 

/* The following enums defines the different types of events used
   on the SEQ_GENERIC_EVENT - to get around the fact that we are
   running out of events on the General Timeline.
*/
typedef enum
{
  SEQ_GENERIC_EVENT_NONE,

  /* hsupa uplink etfci0 rmram event*/
  SEQ_TX_GEN_HSUPA_ETFCI_0_RMRAM_EVENT,

#ifdef FEATURE_WCDMA_CPC_DRX
  SEQ_CPC_DRX_SCHED_CTRL_EVENT,
  SEQ_CPC_DRX_RTR_WARMUPDONE_EVENT,
  SEQ_CPC_DRX_MDSP_WARMUPDONE_EVENT,
  SEQ_CPC_DRX_DTX_DRX_OFFSET_DONE_EVENT,
  SEQ_CPC_DRX_SCCH_ORDER_ACTIVATION_EVENT,
#endif

  SEQ_NUM_GENERIC_EVENTS
} seq_generic_event_enum_type;

/* Type of generic event currently active */
extern seq_generic_event_enum_type seq_generic_event;


/*******************************************************************************
                          GENERAL STATE RX TIMELINE
                          
This is the RX timeline used in the BCH, PCH (non-slotted), and FACH states.
*******************************************************************************
******************************************************************************/
typedef enum {
  TX_GEN_SET_MOD_TIMING_EVENT,

  RX_GEN_SRCH_HB_EVENT,

  RX_GEN_DEC_DATA_READ_EVENT,

/* combiner channel events.  These MUST be contiguous.  Do not insert
   events in between any of them */
  RX_GEN_PHYCHAN_HWCH0_EVENT,
  RX_GEN_PHYCHAN_HWCH1_EVENT,
  RX_GEN_PHYCHAN_HWCH2_EVENT,
  RX_GEN_PHYCHAN_HWCH3_EVENT,
  RX_GEN_PHYCHAN_HWCH4_EVENT,

  RX_GEN_PHYCHAN_HWCH5_EVENT,

  /* DL DCH maintenance event */
  RX_GEN_DL_MAINT_EVENT,
  
  RX_GEN_RXD_STATE_UPDATE_EVENT,

/* events may be appended after this point */
  
  RX_GEN_DL_PULLIN0_EVENT,
  RX_GEN_DL_PULLIN1_EVENT,
  RX_GEN_DL_PULLIN2_EVENT,
  RX_GEN_DL_PULLIN3_EVENT,
  RX_GEN_DL_PULLIN4_EVENT,

  NUM_RX_GEN_TL_EVENTS
} rx_gen_tl_event_enum_type;

/******************************************************************************
*******************************************************************************
                          GENERAL TX TIMELINE
                          
This is the TX timeline used for uplink data processing.
*******************************************************************************
******************************************************************************/
typedef enum {
  /* Power Control Re-enable Event */
  TX_GEN_PC_ENABLE_EVENT,
  
  /*define TX_GEN_EDPCH_CFG_EVENT.This should happen at the same time of 
    get data event and the EDPCH cfg event should be of high priority than
    the get data event so that the EDPCH event is processed before the 
    get data event so the EDPCH shall be dropped first and then the R99 
    Uplink channels*/
  TX_GEN_EDPCH_CFG_EVENT,
  TX_GEN_EDPCH_SEC_CFG_EVENT,
  TX_GEN_HS_RACH_CFG_EVT,
  TX_GEN_GET_DATA_EVENT,
  TX_GEN_FRAME_BDRY_EVENT,
  TX_GEN_HS_PHYCHAN_EVENT,
  TX_GEN_UL_MAINT_EVENT,

  TX_GEN_DTX_CFG_EVENT,

  /* not really associated with TX, but we use the TX timeline to parallelize 
     the gl1 initializations so we don't delay the wl1 task */
  TX_GEN_GL1_INIT_EVENT,

  TX_GEN_UL_COMP_RLC_EVENT_1,
  
  TX_GEN_UL_COMP_RLC_EVENT_2,

  TX_GEN_UL_COMP_RLC_EVENT_3,

  TX_GEN_UL_COMP_RLC_EVENT_4,

  TX_MVS_TIMING_EVT,

  NUM_TX_GEN_TL_EVENTS
} tx_gen_tl_event_enum_type;

typedef enum {
  EUL_GEN_2MS_TTI_EVENT,
  EUL_GEN_10MS_TTI_EVENT,
  NUM_EUL_GEN_TL_EVENTS
} eul_gen_tl_event_enum_type;

#define MCALWCDMA_EVT_GENERIC_EVT_IS_CPCDRX_CTRL_EVENT(gen_evt) \
        ((gen_evt == SEQ_CPC_DRX_SCHED_CTRL_EVENT) ||      \
         (gen_evt == SEQ_CPC_DRX_RTR_WARMUPDONE_EVENT))

#ifdef FEATURE_WCDMA_UL_COMPR
#define TX_EVENTS_MASK (MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_GET_DATA_EVENT)       |    \
                        MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_PC_ENABLE_EVENT)      |    \
                        MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_UL_COMP_RLC_EVENT_1)  |    \
                        MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_UL_COMP_RLC_EVENT_2)  |    \
                        MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_UL_COMP_RLC_EVENT_3)  |    \
                        MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_UL_COMP_RLC_EVENT_4))

#define TX_EVENTS_DCH_MASK (MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_GET_DATA_EVENT)      |    \
                            MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_PC_ENABLE_EVENT)     |    \
                            MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_UL_MAINT_EVENT)      |    \
                            MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_UL_COMP_RLC_EVENT_1) |    \
                            MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_UL_COMP_RLC_EVENT_2) |    \
                            MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_UL_COMP_RLC_EVENT_3) |    \
                            MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_UL_COMP_RLC_EVENT_4))

#else
#define TX_EVENTS_MASK (MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_GET_DATA_EVENT) |    \
                        MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_PC_ENABLE_EVENT) )
#define TX_EVENTS_DCH_MASK (MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_GET_DATA_EVENT) |    \
                            MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_PC_ENABLE_EVENT) | \
                            MCALWCDMA_EVT_EVENT_ID_TO_MASK(TX_GEN_UL_MAINT_EVENT) )
#endif /*FEATURE_WCDMA_UL_COMPR*/

/* Type definition for the Event Dispatch Function */
typedef void seq_event_dispatch_func_type
(
  uint16 event_mask
);
/* The following structure defines an Event Frame */
typedef struct
{

  /* Interrupt Frame Offset relative to interrupt time. This is in
   * units of 256 chips. The Offset must be less than equal to the
   * interrupt Frame Length, i.e. should be between 0 and 149.
   * The Firing Time of all Events on the Event Frame is ADVANCED
   * by the amount of the offset.
   */
   uint8 offset;

  /* Number of Events on the Event Frame.
   * For STMR_PROG_INT0, maximum number of Events is 16.
   * For STMR_PROG_INT1, maximum number of Events is 8.
   * For STMR_PROG_INT2, maximum number of Events is 8.
   */
   uint8 num_events;

  /* Event Mask of Events on the Event Frame. This defines
   * the initial state with which the Event Frame executes.
   * Bit N of the Event Mask corresponds to Event N on the
   * Event Frame.
   */
   uint16 event_mask;

  /* Pointer to an array of Firing Times of Events on the Event Frame.
   * Array Element N corresponds to Event N on the Reference Frame.
   */
   uint8* firing_time_ptr;

   /* Event Dispatch Function. This function is called when an
    * Event occurs on the Event Frame. The Event Mask is passed
    * as the parameter to the function. Bit N of the Event Mask
    * corresponds to Event N on the Event Frame. Only one bit is
    * set when this function is called.
    */
   seq_event_dispatch_func_type* event_proc_func;

} seq_event_frame_type;

extern seq_event_frame_type mcalwcdma_evt_rx_tl;

extern uint8 mcalwcdma_evt_tx_tl_event_array[NUM_TX_GEN_TL_EVENTS];
extern uint8 mcalwcdma_evt_eul_tl_event_array[NUM_EUL_GEN_TL_EVENTS];
extern seq_event_frame_type mcalwcdma_evt_tl_array[MCALWCDMA_EVT_NUM_INTR];

/*===========================================================================

FUNCTION mcalwcmda_evt_gen_tl_handler

DESCRIPTION
  This Event Dispatch Function is installed for the Event Frame executing
  on the General Timeline for both Non-DCH and DCH states.

  The SFN and CFN counters are updated at the P-CCPCH boundary in the
  Non-DCH state.

  In the DCH state, the SFN is updated at the P-CCPCH boundary, and the
  CFN is updated at the DPCH boundary.

  The Event Frame is slaved to run off the P-CCPCH physical channel source
  in the Non-DCH state and off the DPCH physical channel source in the
  DCH state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcalwcdma_evt_gen_tl_handler( uint16 event_mask );

/*===========================================================================
FUNCTION        mcalwcmda_evt_rx_tl_handler

DESCRIPTION     This function dispatches the RX timeline events to their
                associated handler for the BCH, PCH, and FACH state timelines

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void mcalwcdma_evt_rx_tl_handler(uint16 event);

/*===========================================================================
FUNCTION        mcalwcdma_evt_tx_tl_handler

DESCRIPTION     This function dispatches the TX timeline events to their
                associated handler.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void mcalwcdma_evt_tx_tl_handler
(
  uint16 mask
);

/*===========================================================================
FUNCTION        mcalwcdma_evt_eul_tl_handler

DESCRIPTION     This function dispatches the TX timeline events to their
                associated handler.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
void mcalwcdma_evt_eul_tl_handler
(
  uint16 mask
);

/*--------------------------------------------------------------------------
                     GENERAL TX TIMELINE EVENT FRAME
                   
This structure is the event frame for the general TX timeline.                   
--------------------------------------------------------------------------*/
/*===========================================================================

                      FUNCTION MACROS

===========================================================================*/

/*===========================================================================

MACRO MCALWCDMA_EVT_ENABLE_EVENTS

DESCRIPTION
  This macro enables the Events on the Event Frame that is programmed on
  the specified interrupt source. Bit N of the Event Mask corresponds to
  Event N. The Firing Times of the Events should already be programmed.

  This function preserves the Events that are already programmed on the
  interrupt.

  This macro also updates the events_enabled mask for the interrupt. This
  value is used to check if all enabled Events have occurred.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

#define MCALWCDMA_EVT_ENABLE_EVENTS( intr, evt_mask )                \
do {                                                         \
        mcalwcdma_evt_tl_array[intr].event_mask |= evt_mask;   \
       stmr_enable_events( (stmr_int_type)intr, (evt_mask), mcalwcdma_evt_tl_array[intr].firing_time_ptr);   \
} while(0)

/*===========================================================================

MACRO MCALWCDMA_EVT_DISABLE_EVENTS

DESCRIPTION
  This macro disables the specified Event from the Programmable Interrupt
  specified. It reads the current Event Mask from the Frame Register and
  then unsets the Mask of the Events to be disabled.

  There is no need to reset the Firing Times in the Event Registers, since
  the Events will not fire if it is not programmed in the Frame Register.

  This function also disables the corresponding Events from the
  events_enabled and events_occurred mask of this interrupt. These masks
  are used to check if all enabled Events on the programmable interrupts
  occurred.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

#define MCALWCDMA_EVT_DISABLE_EVENTS( intr, evt_mask )          \
do {                                                        \
        stmr_disable_events( (stmr_int_type)intr, (evt_mask) );               \
        mcalwcdma_evt_tl_array[intr].event_mask &= ~(evt_mask);  \
} while(0)


/*===========================================================================

MACRO SEQ_DRV_MODIFY_EVENT_FIRING_TIME

DESCRIPTION
  This macro modifies the Firing Time of the specified Event
  on the specified Programmable Interrupt Source. The Event Mask
  for this Event should already have been programmed in the Event Frame
  Register for this Programmable Interrupt.

  The Event Firing Time is specified relative to the start of the offset
  Event Frame in units of 256 chips. The Firing Time must be less than the
  interrupt frame length (10 ms) and should take the value between 0 and 149
  chips.

  If the Event is scheduled to occur in the next 10 ms interval, the
  Event Check for this interrupt is disabled for this interval because
  the Enabled Event mask will not be the same as the number of Events
  that will occur in the current 10ms frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#define MCALWCDMA_EVT_MODIFY_EVENT_FIRING_TIME( intr, event_num, firing_time ) \
do {                                                                 \
  mcalwcdma_evt_tl_array[intr].firing_time_ptr[event_num]= firing_time; \
} while(0)

/* This macro returns the event firing time of the specified event in units
 * of 256 chips
 */
#define MCALWCDMA_EVT_GET_FIRING_TIME( intr, event_num ) (mcalwcdma_evt_tl_array[intr].firing_time_ptr[event_num])

#define MCALWCDMA_EVT_DEFAULT_RX_TL_MASK 0x0
#define MCALWCDMA_EVT_DEFAULT_GEN_TL_MASK MCALWCDMA_EVT_EVENT_ID_TO_MASK(SEQ_NON_DCH_SFN_CFN_UPDATE_EVENT)
#define MCALWCDMA_EVT_DEFAULT_TX_TL_MASK 0x0
#define MCALWCDMA_EVT_DEFAULT_EUL_TL_MASK 0x0

#define MCALWCDMA_EVT_SET_DEFAULT_RX_TL_EVENTS()  (mcalwcdma_evt_tl_array[MCALWCDMA_EVT_RX_TL_INTR].event_mask = MCALWCDMA_EVT_DEFAULT_RX_TL_MASK)
#define MCALWCDMA_EVT_SET_DEFAULT_GEN_TL_EVENTS() (mcalwcdma_evt_tl_array[MCALWCDMA_EVT_GEN_TL_INTR].event_mask = MCALWCDMA_EVT_DEFAULT_GEN_TL_MASK)
#define MCALWCDMA_EVT_SET_DEFAULT_TX_TL_EVENTS()  (mcalwcdma_evt_tl_array[MCALWCDMA_EVT_TX_TL_INTR].event_mask = MCALWCDMA_EVT_DEFAULT_TX_TL_MASK)
#define MCALWCDMA_EVT_SET_DEFAULT_EUL_TL_EVENTS() (mcalwcdma_evt_tl_array[MCALWCDMA_EVT_EUL_TL_INTR].event_mask = MCALWCDMA_EVT_DEFAULT_EUL_TL_MASK)
#define MCALWCDMA_EVT_SET_DEFAULT_TL_EVENTS()     \
do {                                              \
  MCALWCDMA_EVT_SET_DEFAULT_GEN_TL_EVENTS();      \
  MCALWCDMA_EVT_SET_DEFAULT_RX_TL_EVENTS();       \
  MCALWCDMA_EVT_SET_DEFAULT_TX_TL_EVENTS();       \
  MCALWCDMA_EVT_SET_DEFAULT_EUL_TL_EVENTS();      \
}while(0)

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================*/
/** This function sets the event frame on timeline intrupt sources
*/
/*=========================================================================*/
extern void mcalwcdma_evt_init(void);

/*===========================================================================

FUNCTION SEQ_SET_EVENT_FRAME

DESCRIPTION
  This function allows the user to program an Event Frame onto a specific
  programmable interrupt source. The Event Frames are periodic and has the
  following parameters:
    Intr                   - specifies the programmable interrupt.
    Frame Ref Sel          - for STMR_PROG_INT0 and STMR_PROG_INT1, specifies
                             whether to use the Frame Reference Counter or
                             to use one of 4 Combiner Counters.
                             For STMR_PROG_INT2, specifies whether to use the
                             Frame Reference Counter or the TX System Time.
    Offset                 - specifies the Interrupt Frame Offset relative to
                             current frame reference in units of 256 chips.
                             The offset must be less than or equal to the
                             Frame Period (10 ms) - between 0 and 149 units.
                             The time of all Events on the Event Frame is
                             advanced by the amount of the offset.
    Event Mask             - specifies which Event is enabled. Bit N is set
                             true to enable event N on the specified STMR
                             interrupt.

  This function should be called after a call to the function seq_stmr_align
  to align the interrupts to either one of 4 combiner counters or the
  frame reference counter and further that the combiner counter is slaved
  to a physical channel source.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcalwcdma_evt_set_event_frame
(
  uint32                intr,
  seq_event_frame_type* event_frame
);

/*===========================================================================
FUNCTION     MCALWCDMA_EVT_SAVE_EVENT_MASKS_FOR_SLEEP

DESCRIPTION
  This function saves the current event masks from the timelines of interest,
  i.e., RX, TX and general, in preparation for sleep.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mcalwcdma_evt_save_event_masks_for_sleep ( void );

/*===========================================================================

FUNCTION     SEQ_PREPARE_FOR_SUSPEND

DESCRIPTION
  This function is called in response to the CPHY_SUSPEND_WCDMA_MODE_REQ
  primitive from RRC.  We are performing SUSPEND processing in preparation
  for handover/cell reselection to GSM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Disables all events except SFN/CFN update.

===========================================================================*/
void mcalwcdma_evt_prepare_for_suspend( void );

extern void mcalwcdma_evt_seq_generic_evt_configure(
  /** configure/deconfigure */
  boolean configure, 
  /** tick count */
  uint8 tick_cnt, 
  /** event type */
  seq_generic_event_enum_type generic_event_type);

extern seq_generic_event_enum_type mcalwcdma_evt_get_configured_generic_evt_type(void);

/*===========================================================================

FUNCTION     MCALWCDMA_EVT_CONTROL_DEMOD_CONCURRENCY

DESCRIPTION
  This function is used to control the Demod config mutex and sets the variable
    internally to decide whether to check for Mutex in STMR interrupt handlers
  grab_mutex = TRUE -> Grab the mutex
  grab_mutex = FALSE -> Release the mutex

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void mcalwcdma_evt_control_demod_concurrency(boolean grab_mutex);

/*===========================================================================

FUNCTION mcalwcdma_dl_configure_nondch_fdpch_frame_event

DESCRIPTION
  This configures the fdpch frame event on the gen tl. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  

===========================================================================*/

extern void mcalwcdma_dl_configure_nondch_fdpch_frame_event(boolean configure, uint8 offset);


/*===========================================================================

FUNCTION     MCALWCDMA_EVT_GEN_TL_ALLOCATE_EVT_ID

DESCRIPTION
  This function returns the first available evt id on the Gen TL

DEPENDENCIES
  None.

RETURN VALUE
  first available evt id from the available pool

SIDE EFFECTS
  None

===========================================================================*/

extern uint8 mcalwcdma_evt_gen_tl_allocate_evt_id(void);

/*===========================================================================

FUNCTION     MCALWCDMA_EVT_GEN_TL_DE_ALLOCATE_EVT_ID

DESCRIPTION
  This function de-allocates the evt id on the Gen TL

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if deallocation successful
  FALSE if deallocation unsuccessful

SIDE EFFECTS
  None

===========================================================================*/

extern boolean mcalwcdma_evt_gen_tl_de_allocate_evt_id(
  /*evt id to be deallocated*/
  uint8 evt_id_to_be_deallocated);

/*===========================================================================

FUNCTION     MCALWCDMA_EVT_GEN_TL_CONFIGURE_EVT

DESCRIPTION
  This function configures the allocated evt id on the Gen TL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

extern void mcalwcdma_evt_gen_tl_configure_evt(
  /*tick cnt where the evt needs to be configured*/
  uint8 tick_cnt,
  /*evt id that needs to be configured*/
  uint8 evt_id_to_be_configured,
  /*registered cb for the evt*/
  MCALWCDMA_GEN_EVENT_CB_TYPE evt_cb_func);                                 

/*===========================================================================

FUNCTION     MCALWCDMA_EVT_GEN_TL_DECONFIGURE_EVT

DESCRIPTION
  This function deconfigures the allocated evt id on the Gen TL.

DEPENDENCIES
  None.

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/

extern void mcalwcdma_evt_gen_tl_deconfigure_evt(
  /*evt id that needs to be de-configured*/
  uint8 evt_id_to_be_de_configured);

/*===========================================================================

FUNCTION     MCALWCDMA_EVT_GEN_TL_HANDLER

DESCRIPTION
  This function executes the registered event callback on the Gen TL.

DEPENDENCIES
  None.

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/

extern void mcalwcdma_evt_generic_tl_handler(uint8 evt);

/*===========================================================================
FUNCTION     MCALWCDMA_EVT_RESTORE_EVENT_MASKS_AFTER_SLEEP

DESCRIPTION
  This function restores the current event masks from the timelines of interest,
  i.e., RX, TX and general, in preparation for wakeup.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mcalwcdma_evt_restore_event_masks_after_sleep(void);

/*===========================================================================
FUNCTION     MCALWCDMA_EVT_ENABLE_EVENT_ON_GEN_TL

DESCRIPTION
  This function enables the event on general TL and programs/saves the fire time
  for the event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mcalwcdma_evt_enable_event_on_gen_tl(stmr_int_type intr, seq_gen_tl_event_id_enum_type evt_id, uint8 fire_time);

/*===========================================================================
FUNCTION     MCALWCDMA_EVT_QUERY_FIRE_TIME_FOR_EVENT_ON_GEN_TL

DESCRIPTION
  This function returns the fire time of the event on general TL 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern uint8 mcalwcdma_evt_query_fire_time_for_event_on_gen_tl(seq_gen_tl_event_id_enum_type evt_id);

/*===========================================================================
FUNCTION             mcalwcdma_evt_tx_mvs_timing_evt_handler

DESCRIPTION          This function is the evt handler for the Tx frame boundary evt that is setup as part of mod timing
                     It calls the cb registered by MVS module 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcalwcdma_evt_tx_mvs_timing_evt_handler(void);
#endif /* MCALWCDMA_EVT_H */
