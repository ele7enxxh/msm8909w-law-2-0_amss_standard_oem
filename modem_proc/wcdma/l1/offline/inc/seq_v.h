#ifndef SEQ_V_H
#define SEQ_V_H

/*===========================================================================

            L 1    S E Q U E N C E R    H E A D E R    F I L E

DESCRIPTION
  The L1 Sequencer provides interfaces for the modules within L1 to program
  Event Frames, enable/disable Events, and set Event Firing Times to the
  programmable interrupts in the System Timer.

  The Sequencer provides a wrapper to the low-level STMR Driver routines.


EXTERNALIZED FUNCTIONS

seq_set_event_frame()

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

seq_stmr_align
  This function is called to align the RX, General, and TX Timelines to
  a counter source.

  If the request is to align to a Frame Reference Counter, RX, General,
  and TX Timelines are all aligned to the Frame Reference Counter.

  If the request is to align to a SEQ_PCCPCH_ALIGN_TYPE, RX, and General
  Timelines are aligned to the counter source provided, and TX Timeline
  is aligned to the TX System Time Counter.

  If the request is to align to a SEQ_DPCH_ALIGN_TYPE, RX, and General
  Timelines are aligned to the counter source provided, and TX Timeline
  is aligned to the TX System Time Counter.

  When we move from one counter source to the other, it is possible
  that the Events already programmed on the interrupt sources will move
  because of the difference in the counter values driving the interrupt
  source.

seq_idle_init
  This function is called when L1 enters the L1M_IDLE state. This will
  install the General Timeline, enable the SFN/CFN Event on the
  Timeline, and initialize the SFN and CFN counts to zero. The Timeline
  is then slaved to the Frame Reference Counter.

seq_set_dpch
  This function is called when L1 transitions to DCH state. In the DCH
  state the General Timeline is slaved to a counter source that is driven
  by the DPCH physical channel. The CFN Event should be programmed to
  occur at the DPCH boundary and the SFN Event should be programmed to
  occur T_Dpch (256 chip) units from the DPCH boundary.
  This function is provided the T_Dpch value in units of 256 chips (tick).
  This function will disable the SFN/CFN Event that was programmed in
  the Non-DCH state and program two new Events for SFN and CFN as
  described above.

  SEQ_NON_DCH_SFN_CFN_UPDATE_EVENT is disabled and SEQ_DCH_CFN_UPDATE_EVENT
  is programmed to occur at the DPCH boundary and SEQ_DCH_SFN_UPDATE_EVENT
  is programmed to occur at T_Dpch units from the DPCH boundary.

  This function will also calculate CFN based on the DOFF received
  during DPCH setup according to the formula given in TS 25.402.

seq_set_pccpch
  This function is called when L1 transitions to a Non-DCH state. In this
  state the General Timeline is slaved to a counter source that is driven
  by the PCCPCH physical channel. The CFN and SFN should be counted at
  the PCCPCH boundary and so one Event is programmed at this boundary that
  keeps track of both the values.

  This function will disable the SFN and CFN Events that were programmed
  (if any) in the Non-DCH state. It will program one Event at the PCCPCH
  boundary to count both SFN and CFN.

  This function will recalculate the CFN value for the non-DCH (IDLE mode)
  according to the formula CFN = SFN mod 256.

seq_get_align_type
  This functions returns the type of channel the interrupt source is
  aligned to: PCCPCH_ALIGN_TYPE, DPCH_ALIGN_TYPE or Frame Ref Cntr
  and the current slot count and the tick count from the last slot.

seq_get_sfn
  This function returns the value of the current SFN.

seq_get_cfn
  This function returns the value of the current CFN.

seq_set_sfn_cfn
  This function sets the value of the SFN. Further, it sets CFN to SFN mod 256.

seq_load_sfn
  This function sets the value of the SFN to the new value specified.

seq_load_cfn
  This function sets the value of the CFN to the new value specified.

seq_set_rf_tune_timer
  This function is used to set a timer event to expire at a particular time
  from now. The timer value is passed to the function in units of Sequencer
  ticks.
   1 Sequencer Tick = 256 chips
   1 slot = 10 Sequencer Ticks
   10ms = 150 Sequencer Ticks

  This function must be immediately followed by a call to RF Tune. When
  the timer expires, it posts a RF_TUNE_DONE local command to L1M.

seq_set_activation_time
  This function saves the command with the Activation Time.

seq_cancel_activation_time
  This function cancels any command that has a pending Activation Time.

seq_cancel_all_activation_times
  This function cancels all pending Activation Times.

seq_dispatch_pending_cfn_cmds
  This function is called either at the SFN boundary or at the CFN boundary.
  This function checks to see if there is an aset update or a physical
  channel setup to be performed at the current CFN or SFN.
  If the Activation Time does not fall at the longest of the UL and DL TTIs
  the physical channel configuration will be postponed to occur at the
  longest TTI.

seq_pich_demod_event_handler
  This Event handler sends a command to the mDSP to demodulate the
  PI bits in the PICH frame.
  
seq_prepare_for_sleep
  This function disables all but the required set of events 
  in preparation for sleep.

seq_prepare_for_wakeup
  This function restores the required set of events upon wakeup.

EXTERNALIZED MACROS
  These macros are wrappers to the low-level STMR Driver routines.

SEQ_ENABLE_EVENTS()

  This macro enables the Events on the Event Frame that is programmed on
  the specified interrupt source. Bit N of the Event Mask corresponds to
  Event N. The Firing Times of the Events should already be programmed.

  This function preserves the Events that are already programmed on the
  interrupt.

  This macro also updates the events_enabled mask for the interrupt. This
  value is used to check if all enabled Events have occurred.

SEQ_DISABLE_EVENTS()

  This macro disables the specified Event from the Programmable Interrupt
  specified. It reads the current Event Mask from the Frame Register and
  then unsets the Mask of the Events to be disabled.

  There is no need to reset the Firing Times in the Event Registers, since
  the Events will not fire if it is not programmed in the Frame Register.

  This function also disables the corresponding Events from the
  events_enabled and events_occurred mask of this interrupt. These masks
  are used to check if all enabled Events on the programmable interrupts

SEQ_MODIFY_EVENT_FIRING_TIME()

  This macro modifies the Firing Time of the specified Event Number
  on the specified Programmable Interrupt Source. The Event Mask
  for this Event Number should already have been programmed in the Event
  Frame Register for this Programmable Interrupt.

  The Event Firing Time is specified relative to the start of the offset
  Event Frame in units of 256 chips. The Firing Time must be less than the
  interrupt frame length (10 ms) and should take the value between 0 and 149
  chips.

  If the Event is scheduled to occur in the next 10 ms interval, the
  Event Check for this interrupt is disabled for this interval because
  the Enabled Event mask will not be the same as the number of Events
  that will occur in the current 10ms frame.

INTERNALIZED FUNCTIONS

seq_general_tl_handler( )
  This Event Dispatch Function is installed for the Event Frame executing
  on the General Timeline for both Non-DCH and DCH states.

  The SFN and CFN counters are updated at the P-CCPCH boundary in the
  Non-DCH state.

  In the DCH state, the SFN is updated at the P-CCPCH boundary, and the
  CFN is updated at the DPCH boundary.

  The Event Frame is slaved to run off the P-CCPCH physical channel source
  in the Non-DCH state and off the DPCH physical channel source in the
  DCH state.

seq_non_dch_sfn_cfn_update_event_handler
  This function is called in the Non-DCH state and updates both the
  SFN and CFN counts. The Event programmed to handle this Event is at the
  PCCPCH boundary.

  This Event Handler also checks to see if all the Enabled Events on
  all the Timelines have occurred. It will then update the Event Check
  List appropriately.

  This Handler also calls the mdsp and dec timeout functions.

seq_dch_sfn_update_event_handler
  This command handler is called by the SFN update Event in the DCH state.
  This function updates the SFN count.

seq_dch_cfn_update_event_handler
  This command handler is called by the CFN update Event in the DCH state.
  This function updates the CFN count.

  This Event Handler also checks to see if all the Enabled Events on
  all the Timelines have occurred. It will then update the Event Check
  List appropriately.

  This Handler also calls the mdsp and dec timeout functions.

seq_rf_tune_done_event_handler
  This function posts a RF Tune Done local command to L1M and disables the
  RF Tune Done Event that was programmed on the General Timeline.

  This command handler for the RF Tune Done Event is used in both DCH
  and Non-DCH states.

seq_check_events_before_align
  This function is called before the Sequencer is aligned to a new
  counter source. This function will update the evts_occurred mask
  with the mask of the Events that have occurred so far on the Timelines.
  A L1_HARDWARE_FAILURE_IND indication is sent if any of the Events
  scheduled to occur so far have failed to occur.

seq_check_events_after_align
  This function is called after the Sequencer has been aligned to a different
  counter source. It checks to see where in time the Sequencer starts.
  Case I: bef_time < aft_time, which means that the Timelines start at
  a point that is after the bef_time. This may cause us to lose some
  Events.
  Case II: bef_time > aft_time, which means that the Timelines shifted
  in such a way that the new counter source starts from a point behind
  bef_time. This may cause some Events to be repeated.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  PIC Driver (Trampoline Services) should be initialized.

  stmr_init() should be called before any Sequencer and System Timer
  Driver functions can be used.

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

Copyright (c) 2001-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/seq.h_v   1.25   11 Jun 2002 13:27:40   arao  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/seq_v.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/19/14   sl      Handle incorrect SFN decode gracefully 
08/06/14   sad     New macro for zero AGC settling time
07/18/14   sad     RF api changes and inter freq tune changes
07/01/14   hk      Header file inclusion cleanup
05/21/14   pg      To check in the changes of CR588625 and CR509021 in same CR.
09/14/13   pr      VSTMR Changes for BOLT
05/20/13   sad     Support for Enhanced Layer-1 Manager
03/05/12   hk      Control CFN update concurrency while registering CFN 
                   update callback
02/10/12   rgn     Nikel feature cleanup
02/03/12   raj     Mainlined the feature FEATURE_WCDMA_HSUPA_2MS_TTI
02/01/12   gnk     Nikel Feature cleanup.
01/30/12   vs      Feature cleanup.
01/20/12   zr      Mainlined FEATURE_L1_CELL_RESELECTION and FEATURE_CELL_FACH_MEAS_OCCASION
11/29/11   gnk     Removed some unused features
04/25/11   geg     Separated seq.h into seq.h and seq_v.h   
04/01/11   mk      Replaced INTLOCK with SFN_CFN update mutex   
12/02/10   geg     Created file  

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"
#include "stmr.h"
#include "msg.h"
#include "l1def.h"
#include "l1const.h"
#include "l1task_v.h"
#include "mcalwcdma_evt.h"
#include "l1extif.h"
#include "seq.h"
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* map the stmr combiner counters to the sequencer */
#define SEQ_COMBINER_COUNTER0      STMR_COMBINER_COUNTER0
#define SEQ_COMBINER_COUNTER1      STMR_COMBINER_COUNTER1
#define SEQ_COMBINER_COUNTER2      STMR_COMBINER_COUNTER2
#define SEQ_TX_SYSTEM_TIME_COUNTER STMR_TX_SYSTEM_TIME_COUNTER
#define SEQ_FRAME_REF_COUNTER      STMR_FRAME_REF_COUNTER

/* RF Tune Time in 256 Chip units - set to 3 ms=45*256 chipx1 units */
#define RF_TUNE_TIME   20
#define ZERO_RF_TUNE_TIME 0

/* Max Number of possible clients that can register to get cfn
 for every cfn update event. Current clients are: MAC and dlnschmgr*/
#define L1_SEQ_CFN_REQ_MAX_CLIENTS 9

extern boolean seq_ts_enabled;

/*Whenever S_BCH SFN sanity check handler runs it sets the below flag to ensure TLM_SFN & SEQ_SFN are same*/
extern boolean seq_perform_sfn_sanity;

/* Reason for Activation Time local command */
typedef enum
{
  /* free rrc command buffer and start performing phychan setups */
  CPHY_SETUP_PRE_START_CMD,

  /* free rrc command buffer and begin suspending L1 operations */
  SUSPEND_REQ_START_CMD
} seq_act_time_local_cmd_enum_type;

/* Activation Time local command */
typedef struct
{
  /* local command header */
  l1_local_cmd_hdr_type hdr;

  /* reason for the local command */
  seq_act_time_local_cmd_enum_type reason;
  /* the rrc command to be freed */
  l1_ext_cmd_type* ext_cmd;

} seq_act_time_local_cmd_type;

/*===========================================================================

FUNCTION SEQ_INIT

DESCRIPTION
  This function initializes the critical section for the SEQ module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void seq_init( void );

/*===========================================================================

FUNCTION SEQ_STMR_ALIGN

DESCRIPTION
  This function is called to align the RX, General, and TX Timelines to
  a counter source.

  If the request is to align to a Frame Reference Counter, RX, General,
  and TX Timelines are all aligned to the Frame Reference Counter.

  If the request is to align to a SEQ_PCCPCH_ALIGN_TYPE, RX, and General
  Timelines are aligned to the counter source provided, and TX Timeline
  is aligned to the TX System Time Counter.

  If the request is to align to a SEQ_DPCH_ALIGN_TYPE, RX, and General
  Timelines are aligned to the counter source provided, and TX Timeline
  is aligned to the TX System Time Counter.

  When we move from one counter source to the other, it is possible
  that the Events already programmed on the interrupt sources will move
  because of the difference in the counter values driving the interrupt
  source.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void seq_stmr_align
(
  seq_align_type align_type,
  stmr_counter_source_enum_type cntr_source
);

/*===========================================================================

FUNCTION SEQ_GET_ALIGN_TYPE

DESCRIPTION
  This functions returns the type of channel the interrupt source is
  aligned to: PCCPCH_ALIGN_TYPE, DPCH_ALIGN_TYPE or Frame Ref Cntr
  and the current slot count and the tick count from the last slot.

DEPENDENCIES
  None

RETURN VALUE
  A structure containing the value of the current slot/tick

SIDE EFFECTS
  None
===========================================================================*/
extern seq_align_struct_type seq_get_align_type( void );

/*===========================================================================

FUNCTION SEQ_GET_CURRENT_ALIGN_TYPE

DESCRIPTION
  This functions returns the type of channel the timelines are aligned to.

DEPENDENCIES
  None

RETURN VALUE
  One of the following:
    SEQ_FRAME_REF_ALIGN_TYPE
    SEQ_PCCPCH_ALIGN_TYPE
    SEQ_PICH_ALIGN_TYPE
    SEQ_AICH_ALIGN_TYPE
    SEQ_DPCH_ALIGN_TYPE

SIDE EFFECTS
  None
===========================================================================*/
extern seq_align_type seq_get_current_align_type( void );

/*===========================================================================

FUNCTION SEQ_GET_SFN

DESCRIPTION
  This function returns the value of the current SFN.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 seq_get_sfn( void );

/*===========================================================================

FUNCTION SEQ_GET_EXTENDED_SFN

DESCRIPTION
  This function returns the value of the current extended SFN.This function should not be used
  outside TLM module since this can return erroneous values close to frame boundary.

DEPENDENCIES
  None

RETURN VALUE
  Returns Current Extended SFN

SIDE EFFECTS
  None
===========================================================================*/
extern int32 seq_get_extended_sfn(void);

/*===========================================================================

FUNCTION SEQ_GET_CUR_EUL_SUBFN

DESCRIPTION
  This function returns the value of the current EUL sub fn

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 seq_get_cur_eul_subfn(void);

/*===========================================================================

FUNCTION SEQ_SET_SFN_CFN

DESCRIPTION
  This function sets the value of the SFN and CFN (CFN is set to SFN modulo 256)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function locks interrupts for updating the sfn value.
===========================================================================*/
extern void seq_set_sfn_cfn(uint16 new_sfn);


/*===========================================================================

FUNCTION SEQ_SET_SFN_CFN_WRC

DESCRIPTION
  This function sets the value of the SFN and CFN (CFN is set to SFN modulo 256)
  It also adjusts the wrc_tfn_diff value (Does not touch absolute WRC)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function locks interrupts for updating the sfn value.
===========================================================================*/
extern void seq_set_sfn_cfn_wrc(uint16 new_sfn);

/*===========================================================================

FUNCTION SEQ_LOAD_SFN

DESCRIPTION
  This function sets the value of the SFN to the new value specified.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function locks interrupts for loading the new sfn value.
  SFN is set to the new value.
===========================================================================*/
extern void seq_load_sfn( uint16 new_sfn );

/*===========================================================================

FUNCTION SEQ_LOAD_CFN

DESCRIPTION
  This function sets the value of the CFN to the new value specified.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function locks interrupts for updating the new cfn value.
  CFN is set to the new value.
===========================================================================*/
extern void seq_load_cfn( uint16 new_cfn );

/*===========================================================================

FUNCTION SEQ_SET_RF_TUNE_TIMER

DESCRIPTION
  This function is used to set a timer event to expire at a particular time
  from now. The timer value is passed to the function in units of Sequencer
  ticks.
   1 Sequencer Tick = 256 chips
   1 slot = 10 Sequencer Ticks
   10ms = 150 Sequencer Ticks

  This function must be immediately followed by a call to RF Tune. When
  the timer expires, it posts a RF_TUNE_DONE local command to L1M.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void seq_set_rf_tune_timer
(
  uint16 timer_tick_cnt
);

/*===========================================================================

FUNCTION     SEQ_SET_ACTIVATION_TIME

DESCRIPTION
  This function saves the command with the Activation Time.

DEPENDENCIES
  None

RETURN VALUE
  Boolean - TRUE if the Activation Time was programmed, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean seq_set_activation_time(
  l1_ext_cmd_type* cmd
  /* number frame before command is fired */  
  , uint8 delta_frame_act
 );

/*===========================================================================

FUNCTION     SEQ_SET_ACTIVATION_TIME_NOW

DESCRIPTION
  This is currently called only when UL config is performed

DEPENDENCIES
  None

RETURN VALUE
  Boolean - TRUE if the Activation Time was programmed, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean seq_set_activation_time_now( l1_ext_cmd_type* cmd );

#ifdef FEATURE_QTA
/*===========================================================================

FUNCTION    SEQ_CHECK_PENDING_SETUP_REQ_OK_FOR_QTA

DESCRIPTION
  This function checks if QTA can be granted based on pending Activation Time.

DEPENDENCIES
  None

RETURN VALUE
  Boolean - TRUE if QTA can be allowed
            FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean seq_check_pending_setup_req_ok_for_qta(void);
#endif

/*===========================================================================

FUNCTION    SEQ_CANCEL_ACTIVATION_TIME

DESCRIPTION
  This function cancels any command that has a pending Activation Time.

DEPENDENCIES
  None

RETURN VALUE
  Boolean - TRUE if the command with the Activation Time was cancelled,
            FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean seq_cancel_activation_time( uint8 seq_num );

/*===========================================================================

FUNCTION    SEQ_CANCEL_ALL_ACTIVATION_TIMES

DESCRIPTION
  This function cancels all pending Activation Times.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void seq_cancel_all_activation_times
(
  void
);

/*===========================================================================
FUNCTION    SEQ_GEN_CPHY_SETUP_REQ_ACTIVATE_ACTION_TIME

DESCRIPTION
  This function generates teh activation tine for CPHY_SETUP_REQ.

DEPENDENCIES
  None

RETURN VALUE
  Activation tine

SIDE EFFECTS
  None
===========================================================================*/
uint16 seq_gen_cphy_setup_req_activate_action_time(
  /* Pointer to external command with cphy setup request */
  l1_ext_cmd_type* ext_cmd);

/*===========================================================================

FUNCTION       SEQ_DISPATCH_PENDING_CMDS

DESCRIPTION
  This function is called either at the SFN boundary or at the CFN boundary.
  This function checks to see if there is an aset update, a physical
  channel setup, or a CPHY_SUSPEND_WCDMA_MODE_REQ to be processed at the
  current CFN or SFN.  If the Activation Time does not fall at the longest
  of the UL and DL TTIs the physical channel configuration will be postponed
  to occur at the longest TTI.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the Activation Time does not fall at the longest of the UL and DL TTIs
  the physical channel configuration will be postponed to occur at the
  longest TTI.
===========================================================================*/
void seq_dispatch_pending_cmds
(
  l1_act_time_enum_type
);

/*===========================================================================

FUNCTION       SEQ_ACT_TIME_CMD

DESCRIPTION
  This function is called during local command queue processing in the
  context of the WCDMA L1 task.  It processes commands with activation times
  once their activation times have been reached.  The possible actions are:
  - Free RRC command buffer.
  - Start phychan setups.
  - Indicate phychan setup error to RRC
  - Start processing CPHY_SUSPEND_WCDMA_MODE_REQ from RRC.

  The RRC command buffer associated with the Activation Time is freed here
  in L1 task context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void seq_act_time_cmd( seq_act_time_local_cmd_type* act_cmd );

/*===========================================================================

FUNCTION SEQ_GEN_TL_HANDLER

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
extern void seq_gen_tl_handler( uint16 event_mask );

/*===========================================================================

                SEQUENCER GENERAL TIMELINE EVENT HANDLERS

===========================================================================*/

/*===========================================================================

FUNCTION SEQ_NON_DCH_SFN_CFN_UPDATE_EVENT_HANDLER

DESCRIPTION
  This function is called in the Non-DCH state and updates both the
  SFN and CFN counts. The Event programmed to handle this Event is at the
  PCCPCH boundary.

  This Event Handler also checks to see if all the Enabled Events on
  all the Timelines have occurred. It will then update the Event Check
  List appropriately.

  This Handler also calls the mdsp and dec timeout functions.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void seq_non_dch_sfn_cfn_update_event_handler( void );

/*===========================================================================

FUNCTION SEQ_RF_TUNE_DONE_EVENT_HANDLER

DESCRIPTION
  This function posts a RF Tune Done local command to L1M and disables the
  RF Tune Done Event that was programmed on the General Timeline.

  This command handler for the RF Tune Done Event is used in both DCH
  and Non-DCH states.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Disables the RF Tune Done Event.

===========================================================================*/
void seq_rf_tune_done_event_handler( void );

/*===========================================================================

FUNCTION     SEQ_PICH_DEMOD_EVENT_HANDLER

DESCRIPTION
  This Event handler sends a command to the mDSP to demodulate the
  PI bits in the PICH frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void seq_pich_demod_event_handler( void );

/*===========================================================================

FUNCTION     SEQ_EN/DISABLE_PICH_DEMOD_EVENT

DESCRIPTION  These functions enable/disable the pich demod event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void seq_enable_pich_demod_event( void );
extern void seq_disable_pich_demod_event( void );

/*===========================================================================

FUNCTION       SEQ_GET_UL_DL_LONGEST_TTI

DESCRIPTION    This function gets the maximum TTI among the 
               various transport channels that comprise the CCTrChs.

DEPENDENCIES   None

RETURN VALUE   Returns the number of frames in the max TTI value.

SIDE EFFECTS   None

===========================================================================*/
extern uint8 seq_get_ul_dl_longest_tti(l1_ext_cmd_type* ext_cmd);

/*===========================================================================

FUNCTION SEQ_GENERIC_EVENT_HANDLER

DESCRIPTION
  This function dispatches the appropriate handler registered for the
  SEQ_GENERIC_EVENT.

DEPENDENCIES
  seq_generic_event is assumed to be set properly

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void seq_generic_event_handler( void );

/*===========================================================================
FUNCTION     L1_SEQ_REGISTER_GET_CFN_CB_FUNC

DESCRIPTION  This function registers the call back function which is used to notify the cfn

DEPENDENCIES
  The callback function will be called in the interrupt context. Hence the callback 
  function has to be crisp. Also, L1 may have to do some modifications if any new
  modules deicdes to use this module.

RETURN VALUE
  Boolean TRUE if registration succeed else FALSE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_seq_register_cfn_update_cb_func(
  /** cfn update notify struct */
  l1_cfn_update_notify_struct_type *cfn_update_notify_struct);

/*===========================================================================
FUNCTION     L1_SEQ_DEREGISTER_GET_CFN_CB_FUNC

DESCRIPTION  This function deregisters the call back function which is used to notify the cfn., 
                    last entry of the call back func table is copied to the deleted entry and Number of 

DEPENDENCIES
  None

RETURN VALUE
  Boolean TRUE if deregistration succeed else FALSE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_seq_deregister_cfn_update_cb_func(
  /** cfn update notify struct */
  l1_cfn_update_notify_struct_type *cfn_update_notify_struct);

/*===========================================================================
FUNCTION     l1_seq_check_cfn_notify_cb_status

DESCRIPTION  This function checks if a cfn notify is already programmed. Should be     
             called with interrupts locked 

DEPENDENCIES
  None

RETURN VALUE
  Boolean TRUE if cfn notify is already programmed else FALSE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean l1_seq_check_cfn_notify_cb_status(
  /** cfn update notify struct */
  l1_cfn_update_notify_struct_type *cfn_update_notify_struct);

extern void l1_seq_deregister_all_given_input_cfn_cb(
  /** cfn update notify struct */
  l1_cfn_update_notify_struct_type *cfn_update_notify_struct);

extern void seq_generic_evt_configure(
  /** configure/deconfigure */
  boolean, 
  /** tick count */
  uint8, 
  /** event type */
  seq_generic_event_enum_type);
extern seq_generic_event_enum_type seq_get_configured_generic_evt_type(void);


/*===========================================================================

FUNCTION SEQ_SFN_CFN_LOCK_MUTEX

DESCRIPTION
  This function locks the critical section for the sfn, cfn variable in SEQ module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void seq_sfn_cfn_lock_mutex( void ); 

/*===========================================================================

FUNCTION SEQ_SFN_CFN_UNLOCK_MUTEX

DESCRIPTION
  This function unlocks the critical section for the sfn, cfn variable in SEQ module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void seq_sfn_cfn_unlock_mutex( void );

/*===========================================================================
FUNCTION     SEQ_NOTIFY_CFN_UPDATE

DESCRIPTION  This function notifies the cfn update event for all registered modules 
                  through the registered call back functions

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void seq_notify_cfn_update(uint8 cfn);

/*===========================================================================
FUNCTION     SEQ_GET_CUR_EUL_SUBFN_FOR_RX

DESCRIPTION  this function returns the current sub frame number used for 
             eul tx 
             

DEPENDENCIES
  None

RETURN VALUE
  Subframe number

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 seq_get_cur_eul_subfn_for_rx(void);


/*===========================================================================
FUNCTION     L1_SEQ_DEREGISTER_ALL_GIVEN_INPUT_CFN_CB

DESCRIPTION  This fucntion deregisters entries whose function callback matches
             with that of the input callback provided

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void l1_seq_deregister_all_given_input_cfn_cb(
  /** cfn update notify struct */
  l1_cfn_update_notify_struct_type *cfn_update_notify_struct);

/*===========================================================================

FUNCTION seq_control_sfn_cfn_concurrency

DESCRIPTION
  This function is used to control the SFN & CFN update concurrency

  grab_mutex = TRUE -> Grab the mutex
  grab_mutex = FALSE -> Release the mutex

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void seq_control_sfn_cfn_concurrency(boolean grab_mutex);

/*===========================================================================

FUNCTION seq_control_cfn_update_concurrency

DESCRIPTION
  This function is used to control the CFN update concurrency
    which is used to invoke the CFN update callbacks

  grab_mutex = TRUE -> Grab the mutex
  grab_mutex = FALSE -> Release the mutex

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void seq_control_cfn_update_concurrency(boolean grab_mutex);


#ifdef FEATURE_WCDMA_JOLOKIA_MODEM
/*===========================================================================
FUNCTION       SEQ_UPDATE_L1_CM_FREEZE_ACTION_IF_CMD_PENDING

DESCRIPTION
  This function is to  program Freeze CM at appropriate CFN if there is a command
  pending with activation timer yet to expire.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the Activation Time is already expired and command pending is not yet performed. 
  This function should not be called.
===========================================================================*/
extern void seq_update_l1_cm_freeze_action_if_cmd_pending(void);


/*===========================================================================
FUNCTION       SEQ_IS_PENDING_CMD_ACTIVATION_TIMER_EXPIRED

DESCRIPTION
  This function is called while doing unfreeze after a QTA is over, to find out 
  if there is a command pending with activation timer either expired or about to 
  expire. 

DEPENDENCIES
  None

RETURN VALUE
  TRUE: if cmd is pending with act. timer expired or about to expired
  FALSE: no cmd is pending with above condition.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean seq_is_pending_cmd_activation_timer_expired(void);

#endif


#endif /* SEQ_V_H */

