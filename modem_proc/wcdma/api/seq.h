#ifndef SEQ_H
#define SEQ_H
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

Copyright (c) 2001-2007 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/seq.h_v   1.25   11 Jun 2002 13:27:40   arao  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/seq.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/14   hk      Header file inclusion cleanup
04/25/11   geg     Separated seq.h into seq.h and seq_v.h
04/25/11   geg     Moved this file to /wcdma/api from /l1/inc
03/11/11   dp      Ported CPC DRX changes to offline Architecture
05/03/10   hk      Fixed compiler warnings
09/29/09   hk      Added support for Extended SFN counter
09/14/09   sv      Removed WCDMA_INTLOCK's.
09/02/09   mg      Remove unused seq_set_sfn and seq_set_cfn
12/16/08   sv      Changes to support critical section in SEQ module.
12/10/08   ks      Mainlining FEATURE_WCDMA_DL_ENHANCED
10/22/08   rmak    Remove SEQ_SFN_DEBUG
10/03/08   hk      Bringing in 7k Mailine fixes
02/21/07   vb      Added support for FEATURE_MCAL_WCDMA_SEQ
26/01/07   vb      added code to integrate mcalwcdma evt module
01/12/07   vb      Added code for WHL1 Event Management support
02/02/07   vr      Added functions to maintain seq cfn to rx tl combiner cfn diff
11/20/06   yiz     Add runtime detection of HW EUL cipher support
08/14/06   yiz     Mainline HSUPA code
04/20/06   mc      Added support for HS performance optimization.
02/07/06   scm     Add extern to seq_get_current_align_type().
09/14/05   gv      Added function prototype for seq_get_ul_dl_longest_tti()
07/05/05   vb      Code Addition to include stmt_combiner_counter_source in 
                   seq_align_struct_type 
05/31/05   vp      Merging of fixes for Lint errors
06/01/05   mg      Extern seq_forced_sfn_set and seq_ignore_first
04/11/05   vb      Added func prototype seq_set_dpch_to_old_cell_dch_timing() to 
                   resuming to old config upon HHO failure.
02/16/05   kps     lint error fixes.
04/15/04   yiz     Rename FACH inter-RAT CR as FEATURE_CELL_FACH_MEAS_OCCASION
12/10/03   yiz     Add SEQ_SRCH_FACH_SCCPCH_EVENT for 
                   FEATURE_INTER_RAT_CELL_FACH_RE_SELECTION
09/02/03   scm     Externalized function seq_set_activation_time_now();
04/10/03   gs      Externalized function seq_gen_cphy_setup_req_activate_action_time
02/12/03   asr     Added declarations of seq_en/disable_pich_demod_event()
10/30/02   src     Added function declaration seq_save_event_masks_for_sleep,
                   a new method to only save time-line event masks without
                   disabling the time-lines.
09/18/02   scm     Adding activation time support to SUSPEND.
09/10/02   sh      Added seq_cell_trans_stmr_align() to set flag for SFN/CFN
                   event to align sequencer timing to new cell timing.
08/29/02   src     Changed the signature of the function seq_load_cfn to take
                   the CFN argument as a 16-bit (unsigned) integer.
08/27/02   sh      Added SEQ_CELL_TRANS_ALIGN_EVENT to align to new cel
                   timing during cell reselection and related variable.
                   Fixed seq_set_cfn() to take int16.
08/19/02   scm     Added seq_prepare_for_suspend() for SUSPEND processing.
06/12/02   asr     Added function seq_set_sfn_sfn()
05/08/02   sh      Added function seq_set_pccpch_n() to be called during
                   cell transition to set timing to the new cell.
                   Added PICH and AICH align types.
                   Added functions seq_set_aich() and seq_set_pich().
03/13/02   src     Merged-in fix for D-OFF computation during soft hand-over.
                   Too many code changes and re-organisations to individually
                   list as a whole, but the effect on this file is in small -
                   the prev_dl_dpch_offset variable has been re-located to a
                   more appropriate location: dldem.*.
02/25/02   sh      Created local command seq_act_time_local_cmd_type and
                   function seq_act_time_cmd().
02/11/02   asr     Added macro SEQ_GET_STMR_TICKS_BEFORE_NEXT_SFN()
02/15/02   sh      Changed proto for seq_set_activation_time() functions.
02/12/02   sh      Added flag FEATURE_L1_ACT_TIME.
02/06/02   sh      Changed protos for seq_set_activation_time() and
                   seq_cancel_activation_time().
                   Added seq_dispatch_pending_cfn_cmds(), and
                   seq_cancel_all_activation_times().
01/31/02   asr     Added prototypes for seq_prepare_for_sleep() and 
                   seq_prepare_for_wakeup()
01/02/02   gs      Added the CM update event at slot 2 on general timeline.
12/18/01   src     Added an extern declaration for prev_dl_dpch_offset, which
                   holds the Tau-DPCH offset for the previous DPCH setup.
12/05/01   src     Removed feature flag for DOFF - FEATURE_L1_DOFF.
11/16/01   sh      Use DOFF to set CFN when we set up DPCH.
11/02/01   sh      Added variable to snapshot Default DPCH Offset: DOFF
09/05/01   sh      Added function seq_pich_demod_event_handler().
                   Added event SEQ_PICH_DEMOD_EVENT.
07/14/01   sh      Added functions to set and cancel activation times.
06/05/01   sh      Removed code under flag SEQ_EVENT_CHECK_ENABLED.
                   Added define RF_TUNE_TIME.
02/29/01   sh      Removed STMR_TICKS_PER_SLOT and STMR_CHIPS_PER_TICK.
02/21/01   mdl     externed seq_ts_enable
01/25/01   sh      Wrapped Event Check feature in SEQ_EVENT_CHECK_ENABLED
                   flag.
01/23/01   sh      Added feature to check if all Events on all Timelines have
                   occurred.
                   Modified macros SEQ_ENABLE_EVENTS,
                   SEQ_DISABLE_EVENTS, SEQ_MODIFY_EVENT_FIRING_TIME.
                   Added functions seq_check_events_before_align(),
                   seq_check_events_after_align(),
                   seq_update_enabled_event_check_list(),
                   seq_update_disabled_event_check_list() to update Event
                   Check List data structures when the STMR is aligned to a
                   different counter source, and when Events have been
                   enabled and disabled.
                   Added function seq_report_event() for other modules to
                   report occurrence of an Event to the Sequencer.

01/04/01    sh     Added function seq_set_pccpch() to reprogram the General
                   Timeline when we transition to a non-DCH state.
                   Renamed seq_set_tdpch() to seq_set_dpch().

01/03/01    sh     Added function seq_set_tdpch() to reprogram the General
                   Timeline when we transition to DCH state.
12/12/2000 mdl     Added macro SEQ_NORMALIZE_TICKS() which calls the function
                   normalize with STMR_TICKS_PER_FRAME as the second
                   parameter, and does some casting knowing that sequencer
                   ticks are uint16's.
11/16/00    sh     Changed General Timeline to use one Timeline for both
                   DCH and non-DCH states.
                   Added support for RF_TUNE_DONE event.
                   Added Event Handlers for each Event.
                   Added support to load sfn/cfn.
                   Changed interface to set event frame. A counter source
                   has to be aligned to a physical channel source for RX
                   and TX System Time Counter source for TX or all timelines
                   should be aligned to Frame Reference Counter before setting
                   an Event Frame (Timeline).
09/05/2000 sh      Created file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "stmr.h"


#ifndef TLM_H
/* These enums define whether a STMR interrupt source is aligned to
 * a physical channel source or the frame reference counter.
 */
typedef enum
{
  SEQ_FRAME_REF_ALIGN_TYPE,
  SEQ_PCCPCH_ALIGN_TYPE,
  SEQ_PICH_ALIGN_TYPE,
  SEQ_AICH_ALIGN_TYPE,
  SEQ_DPCH_ALIGN_TYPE,
  SEQ_NUM_ALIGN_TYPE
} seq_align_type;
#endif

/* The following structure returns the Physical Channel type or
 * the frame reference counter we are currently aligned to, and
 * the current time in slots and the ticks from the last slot.
 */
typedef struct
{
  /* source currently aligned to */
  seq_align_type phch_align_type;
  /* current slot count */
  uint8 slot_cnt;
  /* current tick count from slot boundary */
  uint8 tick_cnt;
  
  /* Current stmr counter source for the phch_align_type */
  stmr_counter_source_enum_type cntr_src;
} seq_align_struct_type;


/*===========================================================================

FUNCTION SEQ_GET_CFN

DESCRIPTION
  This function returns the value of the current CFN.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 seq_get_cfn( void );

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

#endif /* SEQ_H */

