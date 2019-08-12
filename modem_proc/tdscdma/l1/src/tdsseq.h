#ifndef TDSSEQ_H
#define TDSSEQ_H
/*===========================================================================

            L 1    S E Q U E N C E R    H E A D E R    F I L E

DESCRIPTION
  The L1 Sequencer provides interfaces for the modules within L1 to program
  Event Frames, enable/disable Events, and set Event Firing Times to the
  programmable interrupts in the System Timer.

  The Sequencer provides a wrapper to the low-level STMR Driver routines.


EXTERNALIZED FUNCTIONS

tdsseq_set_event_frame()

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
  This function should be called after a call to the function tdsseq_stmr_align
  to align the interrupts to either one of 4 combiner counters or the
  frame reference counter and further that the combiner counter is slaved
  to a physical channel source.

tdsseq_stmr_align
  This function is called to align the RX, General, and TX Timelines to
  a counter source.

  If the request is to align to a Frame Reference Counter, RX, General,
  and TX Timelines are all aligned to the Frame Reference Counter.

  If the request is to align to a tdsseq_PCCPCH_ALIGN_TYPE, RX, and General
  Timelines are aligned to the counter source provided, and TX Timeline
  is aligned to the TX System Time Counter.

  If the request is to align to a tdsseq_DPCH_ALIGN_TYPE, RX, and General
  Timelines are aligned to the counter source provided, and TX Timeline
  is aligned to the TX System Time Counter.

  When we move from one counter source to the other, it is possible
  that the Events already programmed on the interrupt sources will move
  because of the difference in the counter values driving the interrupt
  source.

tdsseq_idle_init
  This function is called when L1 enters the TDSL1M_IDLE state. This will
  install the General Timeline, enable the SFN/CFN Event on the
  Timeline, and initialize the SFN and CFN counts to zero. The Timeline
  is then slaved to the Frame Reference Counter.

tdsseq_set_dpch
  This function is called when L1 transitions to DCH state. In the DCH
  state the General Timeline is slaved to a counter source that is driven
  by the DPCH physical channel. The CFN Event should be programmed to
  occur at the DPCH boundary and the SFN Event should be programmed to
  occur T_Dpch (256 chip) units from the DPCH boundary.
  This function is provided the T_Dpch value in units of 256 chips (tick).
  This function will disable the SFN/CFN Event that was programmed in
  the Non-DCH state and program two new Events for SFN and CFN as
  described above.

  tdsseq_NON_DCH_SFN_CFN_UPDATE_EVENT is disabled and tdsseq_DCH_CFN_UPDATE_EVENT
  is programmed to occur at the DPCH boundary and tdsseq_DCH_SFN_UPDATE_EVENT
  is programmed to occur at T_Dpch units from the DPCH boundary.

  This function will also calculate CFN based on the DOFF received
  during DPCH setup according to the formula given in TS 25.402.

tdsseq_set_pccpch
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

tdsseq_get_align_type
  This functions returns the type of channel the interrupt source is
  aligned to: PCCPCH_ALIGN_TYPE, DPCH_ALIGN_TYPE or Frame Ref Cntr
  and the current slot count and the tick count from the last slot.

tdstdsseq_get_sfn
  This function returns the value of the current SFN.

tdstdsseq_get_cfn
  This function returns the value of the current CFN.

tdsseq_set_sfn_cfn
  This function sets the value of the SFN. Further, it sets CFN to SFN mod 256.

tdsseq_load_sfn
  This function sets the value of the SFN to the new value specified.

tdsseq_load_cfn
  This function sets the value of the CFN to the new value specified.

tdsseq_set_rf_tune_timer
  This function is used to set a timer event to expire at a particular time
  from now. The timer value is passed to the function in units of Sequencer
  ticks.
   1 Sequencer Tick = 256 chips
   1 slot = 10 Sequencer Ticks
   10ms = 150 Sequencer Ticks

  This function must be immediately followed by a call to RF Tune. When
  the timer expires, it posts a RF_TUNE_DONE local command to L1M.

tdsseq_set_activation_time
  This function saves the command with the Activation Time.

tdsseq_cancel_activation_time
  This function cancels any command that has a pending Activation Time.

tdsseq_cancel_all_activation_times
  This function cancels all pending Activation Times.

tdsseq_dispatch_pending_cfn_cmds
  This function is called either at the SFN boundary or at the CFN boundary.
  This function checks to see if there is an aset update or a physical
  channel setup to be performed at the current CFN or SFN.
  If the Activation Time does not fall at the longest of the UL and DL TTIs
  the physical channel configuration will be postponed to occur at the
  longest TTI.

tdsseq_chnl_setup_event_handler
  This Event handler sends a command to the mDSP to demodulate the
  PI bits in the PICH frame.
  
tdsseq_prepare_for_sleep
  This function disables all but the required set of events 
  in preparation for sleep.

tdsseq_prepare_for_wakeup
  This function restores the required set of events upon wakeup.

EXTERNALIZED MACROS
  These macros are wrappers to the low-level STMR Driver routines.

tdsseq_ENABLE_EVENTS()

  This macro enables the Events on the Event Frame that is programmed on
  the specified interrupt source. Bit N of the Event Mask corresponds to
  Event N. The Firing Times of the Events should already be programmed.

  This function preserves the Events that are already programmed on the
  interrupt.

  This macro also updates the events_enabled mask for the interrupt. This
  value is used to check if all enabled Events have occurred.

tdsseq_DISABLE_EVENTS()

  This macro disables the specified Event from the Programmable Interrupt
  specified. It reads the current Event Mask from the Frame Register and
  then unsets the Mask of the Events to be disabled.

  There is no need to reset the Firing Times in the Event Registers, since
  the Events will not fire if it is not programmed in the Frame Register.

  This function also disables the corresponding Events from the
  events_enabled and events_occurred mask of this interrupt. These masks
  are used to check if all enabled Events on the programmable interrupts

tdsseq_MODIFY_EVENT_FIRING_TIME()

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

tdsseq_general_tl_handler( )
  This Event Dispatch Function is installed for the Event Frame executing
  on the General Timeline for both Non-DCH and DCH states.

  The SFN and CFN counters are updated at the P-CCPCH boundary in the
  Non-DCH state.

  In the DCH state, the SFN is updated at the P-CCPCH boundary, and the
  CFN is updated at the DPCH boundary.

  The Event Frame is slaved to run off the P-CCPCH physical channel source
  in the Non-DCH state and off the DPCH physical channel source in the
  DCH state.

tdsseq_sfn_cfn_update_event_handler
  This function is called in the Non-DCH state and updates both the
  SFN and CFN counts. The Event programmed to handle this Event is at the
  PCCPCH boundary.

  This Event Handler also checks to see if all the Enabled Events on
  all the Timelines have occurred. It will then update the Event Check
  List appropriately.

  This Handler also calls the mdsp and dec timeout functions.

tdsseq_dch_sfn_update_event_handler
  This command handler is called by the SFN update Event in the DCH state.
  This function updates the SFN count.

tdsseq_dch_cfn_update_event_handler
  This command handler is called by the CFN update Event in the DCH state.
  This function updates the CFN count.

  This Event Handler also checks to see if all the Enabled Events on
  all the Timelines have occurred. It will then update the Event Check
  List appropriately.

  This Handler also calls the mdsp and dec timeout functions.

tdsseq_rf_tune_done_event_handler
  This function posts a RF Tune Done local command to L1M and disables the
  RF Tune Done Event that was programmed on the General Timeline.

  This command handler for the RF Tune Done Event is used in both DCH
  and Non-DCH states.

tdsseq_check_events_before_align
  This function is called before the Sequencer is aligned to a new
  counter source. This function will update the evts_occurred mask
  with the mask of the Events that have occurred so far on the Timelines.
  A TDSL1_HARDWARE_FAILURE_IND indication is sent if any of the Events
  scheduled to occur so far have failed to occur.

tdsseq_check_events_after_align
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

  tdsstmr_init() should be called before any Sequencer and System Timer
  Driver functions can be used.

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

Copyright (c) 2001-2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/seq.h_v   1.25   11 Jun 2002 13:27:40   arao  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsseq.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/06/2013  yuw           Supported Bolt
12/13/2010  weijunz
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "customer.h"
#include "tdsstmr.h"
#include "msg.h"
#include "tdsl1def.h"
#include "tdsl1const.h"
#include "tdsl1mtask.h"
#include "mcaltds_stmr_evt.h"
#include "mcaltds_tfw_evt.h"
#include "tdsl1extif.h"






/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* RF Tune Time in 256 Chip units - set to 3 ms=15*256 chipx1 units */
#define TDSSEQ_RF_TUNE_TIME   15

typedef void (*TDSSEQ_SUB_FRAME_BOUNDARY_CB_TYPE) (uint16,uint32);

/* Reason for Activation Time local command */
typedef enum
{
  /* free rrc command buffer */
  TDSSEQ_FREE_RRC_BUF,
  /* free rrc command buffer and start performing phychan setups */
  TDSSEQ_PHYCHAN_SETUP_START_CMD,
  TDSSEQ_PHYCHAN_SETUP_START_CMD_FOR_CELL_TRANS,
  TDSSEQ_PHYCHAN_SETUP_INTERNAL_RESUME,
  /* free rrc buffer send confirmation to rrc and */
  TDSSEQ_PHYCHAN_SETUP_ERROR,
  /* free rrc command buffer and begin suspending L1 operations */
  TDSSEQ_SUSPEND_REQ_START_CMD,
  TDSSEQ_DRX_REQ_START_CMD
} tdsseq_act_time_local_cmd_enum_type;

/* Activation Time local command */
typedef struct
{
  /* local command header */
  tdsl1def_l1_local_cmd_hdr_type hdr;

  /* reason for the local command */
  tdsseq_act_time_local_cmd_enum_type reason;
  /* the rrc command to be freed */
  tdsl1_ext_cmd_type* ext_cmd;

} tdsseq_act_time_local_cmd_type;

typedef enum
{
  TDSSEQ_ACTTIME_CANCEL_BOTH_FAIL,            /* cancel disable MEAS/build RF script(act time - 20ms) and pending CMD(act time - 10ms) all failed */
  TDSSEQ_ACTTIME_CANCEL_PENDING_CMD_SUCC,     /* cancel disable pending CMD(act time - 10ms) success */            
  TDSSEQ_ACTTIME_CANCEL_DISMEAS_RFSCRIPT_SUCC,/* cancel disable MEAS/build RF script(act time - 20ms) success */
  TDSSEQ_ACTTIME_CANCEL_SUCC     /* cancel disable MEAS/build RF script(act time - 20ms) and pending CMD(act time - 10ms) all success */
}tdsseq_act_time_cancel_return_enum_type;

/*===========================================================================

FUNCTION TDSSEQ_INIT

DESCRIPTION
  This function initializes the critical section for the SEQ module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsseq_init( void );





/*===========================================================================

FUNCTION TDSSEQ_GET_SFN

DESCRIPTION
  This function returns the value of the current SFN.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 tdsseq_get_sfn( void );

/*===========================================================================

FUNCTION TDSSEQ_GET_SUB_FRAME_NUMBER

DESCRIPTION
  This function returns the value of the current Sub frame number

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 tdsseq_get_sub_frame_num( void );



/*===========================================================================

FUNCTION tdsseq_get_cfn

DESCRIPTION
  This function returns the value of the current CFN.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint8 tdsseq_get_cfn( void );


/*===========================================================================

FUNCTION tdsseq_get_full_sys_cnt

DESCRIPTION
  This function returns the value of RX time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 tdsseq_get_full_sys_cnt( void );


/*===========================================================================

FUNCTION TDSSEQ_SET_SFN_CFN

DESCRIPTION
  This function sets the value of the SFN and CFN (CFN is set to SFN modulo 256)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function locks interrupts for updating the sfn value.
===========================================================================*/
extern void tdsseq_set_sfn_cfn(uint16 new_sfn);


/*===========================================================================

FUNCTION TDSSEQ_LOAD_SFN

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
extern void tdsseq_load_sfn( uint16 new_sfn );


/*===========================================================================

FUNCTION TDSSEQ_LOAD_SUB_FRAME_NUM_FROM_RX_COUNTER

DESCRIPTION
  This function load  the value of the Sub frame  number from Rx time Counter 
  to Seq module .Sub frame number  in L1 SW and Sub frame number in counter  are 
  the same after ACQ and STMR event TDSSEQ_SFN_CFN_UPDATE_EVENT is enabled. 
   
  This function locks interrupts for loading the new sub frame number. 
  At the same time, SFN in SEQ is also set to new value.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function locks interrupts for loading the new sfn value.
  SFN is set to the new value.
===========================================================================*/
extern void tdsseq_load_sub_frame_num_from_rx_counter( void );

extern void tdsseq_load_sub_fn(uint16 sub_frame_num);


/*===========================================================================

FUNCTION TDSSEQ_LOAD_CFN

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
extern void tdsseq_load_cfn( uint16 new_cfn );


/*===========================================================================

FUNCTION tdsseq_caculate_cfn

DESCRIPTION
  This function caculates CFN with doff value from NW.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function locks interrupts for updating the new cfn value.
  CFN is set to the new value.
===========================================================================*/
extern void tdsseq_caculate_cfn( uint8 doff );


/*===========================================================================

FUNCTION     TDSSEQ_CELL_TRANS_STMR_ALIGN

DESCRIPTION
  This function will set seq_cell_trans_stmr_align_pending, so that
  the next SFN/CFN event can decide to align the Sequencer to the new
  cell timing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsseq_cell_trans_stmr_align( void );

/*===========================================================================

FUNCTION TDSSEQ_SET_RF_TUNE_TIMER

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
extern void tdsseq_set_rf_tune_timer
(
  uint16 timer_tick_cnt
);

/*===========================================================================

FUNCTION     TDSSEQ_SET_ACTIVATION_TIME

DESCRIPTION
  This function saves the command with the Activation Time.

DEPENDENCIES
  None

RETURN VALUE
  Boolean - TRUE if the Activation Time was programmed, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdsseq_set_activation_time(
  tdsl1_ext_cmd_type* cmd
 #ifdef FEATURE_HSDPA_PERFORMANCE_OPTIMIZATION
  /* number frame before command is fired */  
  , uint8 delta_frame_act
 #endif
  , boolean need_state_transition
 );


/*===========================================================================

FUNCTION    TDSSEQ_CANCEL_ACTIVATION_TIME

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
extern tdsseq_act_time_cancel_return_enum_type tdsseq_cancel_activation_time( uint8 seq_num );

	
/*===========================================================================

FUNCTION    TDSSEQ_CANCEL_ACTIVATION_TIME

DESCRIPTION
  This function cancels any command that match the config_id.

DEPENDENCIES
  None

RETURN VALUE
  Boolean - TRUE if the command with the configu_id was cancelled,
            FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsseq_cancel_sub_frame_boundary_cb_func( uint8 config_id );

/*===========================================================================

FUNCTION    TDSSEQ_CANCEL_ALL_ACTIVATION_TIMES

DESCRIPTION
  This function cancels all pending Activation Times.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsseq_cancel_all_activation_times
(
  void
);

/*===========================================================================

FUNCTION       TDSSEQ_ACT_TIME_CMD

DESCRIPTION
  This function is called during local command queue processing in the
  context of the WCDMA L1 task.  It processes commands with activation times
  once their activation times have been reached.  The possible actions are:
  - Free RRC command buffer.
  - Start phychan setups.
  - Indicate phychan setup error to RRC
  - Start processing TDSL1_CPHY_SUSPEND_TDSCDMA_MODE_REQ from RRC.

  The RRC command buffer associated with the Activation Time is freed here
  in L1 task context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsseq_act_time_cmd( tdsseq_act_time_local_cmd_type* act_cmd );

/*===========================================================================

                SEQUENCER GENERAL TIMELINE EVENT HANDLERS

===========================================================================*/

/*===========================================================================

FUNCTION TDSSEQ_SFN_CFN_UPDATE_EVENT_HANDLER

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
void tdsseq_sfn_cfn_update_event_handler( void );


/*===========================================================================

FUNCTION TDSSEQ_RF_TUNE_DONE_EVENT_HANDLER

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
void tdsseq_rf_tune_done_event_handler( void );

/*===========================================================================

FUNCTION     TDSSEQ_CHNL_SETUP_EVENT_HANDLER

DESCRIPTION
  This Event handler sends a command to the mDSP to activate the channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tdsseq_chnl_setup_event_handler( void );


/*===========================================================================

FUNCTION     TDSSEQ_EN/DISABLE_PICH_DEMOD_EVENT

DESCRIPTION  These functions enable/disable the pich demod event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tdsseq_enable_chnl_setup_event(uint16 firing_time);
extern void tdsseq_disable_chnl_setup_event( void );


/*===========================================================================

FUNCTION     tdsseq_rf_early_sleep_event_handler

DESCRIPTION
  This Event handler tdsseq_rf_early_sleep_event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tdsseq_rf_early_sleep_event_handler( void );

/*===========================================================================

FUNCTION     tdsseq_rx_time_adjust_event_handler

DESCRIPTION
  This Event handler adjust rx-time counter according to FW TTL feedback.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tdsseq_rx_time_adjust_event_handler( void );

/*===========================================================================

FUNCTION tdsseq_enable_rx_time_adjust_event

DESCRIPTION
  These functions enable the rx-time adjustment event

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  

===========================================================================*/
extern void tdsseq_enable_rx_time_adjust_event(void);

/*===========================================================================

FUNCTION tdsseq_disable_rx_time_adjust_event

DESCRIPTION
  These functions disable the rx-time adjustment event

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  

===========================================================================*/
extern void tdsseq_disable_rx_time_adjust_event(void);

/*===========================================================================

FUNCTION TDSSEQ_ENABLE_HSUPA_EDCH_EVENT/TDSSEQ_DISABLE_HSUPA_EDCH_EVENT

DESCRIPTION
  These functions are used to enable/disable the SEQ_TX_GEN_EDPCH_EVENT
  that is programmed as a SEQ_GENERIC_EVENT.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  

===========================================================================*/
void tdsseq_enable_hsupa_edch_event( void );
void tdsseq_disable_hsupa_edch_event( void );

#ifdef FEATURE_TDSCDMA_PLT
/*===========================================================================

FUNCTION TDSSEQ_ENABLE_HSUPA_PLT_EVENT/TDSSEQ_DISABLE_HSUPA_PLT_EVENT

DESCRIPTION
  These functions are used to enable/disable the SEQ_TX_GEN_EDPCH_EVENT
  that is programmed as a SEQ_GENERIC_EVENT.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  

===========================================================================*/
#if defined(FEATURE_TDSCDMA_PLT_JOLOKIA_SERIES)
void tdsseq_enable_hsupa_plt_chkpnt_event( void );
void tdsseq_disable_hsupa_plt_chkpnt_event( void );
#else
void tdsseq_enable_hsupa_plt_event( void );
void tdsseq_disable_hsupa_plt_event( void );
#endif
#endif


/*===========================================================================
FUNCTION     TDSSEQ_L1_REGISTER_CFN_UPDATE_SB_FUNC

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
extern boolean tdsseq_l1_register_cfn_update_cb_func(
  /** cfn update notify struct */
  tdsl1extif_cfn_update_notify_struct_type *cfn_update_notify_struct);

/*===========================================================================
FUNCTION     TDSL1_SEQ_DEREGISTER_GET_CFN_CB_FUNC

DESCRIPTION  This function deregisters the call back function which is used to notify the cfn., 
                    last entry of the call back func table is copied to the deleted entry and Number of 

DEPENDENCIES
  None

RETURN VALUE
  Boolean TRUE if deregistration succeed else FALSE

SIDE EFFECTS
  None
===========================================================================*/
extern boolean tdsseq_l1_deregister_cfn_update_cb_func(
  /** cfn update notify struct */
  tdsl1extif_cfn_update_notify_struct_type *cfn_update_notify_struct);


/*===========================================================================

FUNCTION TDSSEQ_SFN_CFN_LOCK_MUTEX

DESCRIPTION
  This function locks the critical section for the sfn, cfn variable in SEQ module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsseq_sfn_cfn_lock_mutex( void ); 

/*===========================================================================

FUNCTION TDSSEQ_SFN_CFN_UNLOCK_MUTEX

DESCRIPTION
  This function unlocks the critical section for the sfn, cfn variable in SEQ module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void tdsseq_sfn_cfn_unlock_mutex( void );



/*===========================================================================
FUNCTION     TDSSEQ_REGISTER_SUB_FRMAE_BOUNDARY_CB_FUNC

DESCRIPTION  This function registers a call back function that will be called 
at the boundary of matched sub frame number. The callback  function will be 
called only once and deregistered automatically after being invoked. 
 
Application should call this function before the designated sub frame boundary. 
If  designated sub frame number is less than current sub frame number , call 
back function will be called after sub frame number is wrapped around at 8192. 

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsseq_register_sub_frame_boundary_cb_func(uint16 sub_frmae_num, uint32 reg_id,TDSSEQ_SUB_FRAME_BOUNDARY_CB_TYPE cb_func);

/*===========================================================================
FUNCTION     tdsseq_cphy_setup_cmd_post_process

DESCRIPTION  This function process TDSL1_CPHY_SETUP_REQ at (act time - 1) in SEQ 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdsseq_cphy_setup_cmd_post_process(tdsl1_ext_cmd_type* ext_cmd, boolean act_cmd_trigger_needed);

#ifdef FEATURE_TDSCDMA_L1M_PROFILING_LOG_PACKET
void tdsseq_profiling_trigger_event_handler( void );

void tdsseq_enable_profiling_trigger_event( void );

void tdsseq_disable_profiling_trigger_event( void );

#endif


typedef struct
{
  /* Pointer to the pending command */
  tdsl1_ext_cmd_type* ext_cmd;
  /* Whether this command is pending */
  boolean pending;
 #ifdef FEATURE_HSDPA_PERFORMANCE_OPTIMIZATION
  /* number of frame before command is dispatched */
  uint8 delta_frame_act;
 #endif
} tdsseq_pending_cmd_struct_type;

/* Max number of pending Activation Times at any instant */
#define TDSSEQ_L1_NUM_PENDING_ACT_TIMES 5

/* Array of pending Activation Times */
extern tdsseq_pending_cmd_struct_type tdsseq_pending_cmd[TDSSEQ_L1_NUM_PENDING_ACT_TIMES];

extern boolean tdsseq_check_pending_cmds( void );

#ifdef FEATURE_TDSCDMA_DSDS_ALLOW_QTA_BEFORE_ACT_TIME
#ifdef FEATURE_TDSCDMA_DSDS_QTA
/*===========================================================================

FUNCTION    TDSSEQ_CHECK_PENDING_SETUP_REQ_OK_FOR_QTA

DESCRIPTION
  This function checks if QTA can be granted based pending Activation Time.

DEPENDENCIES
  None

RETURN VALUE
  Boolean - TRUE if QTA can be allowed
            FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean tdsseq_check_pending_setup_req_ok_for_qta(uint16 rrc_qta_adv_ms);
#endif /* FEATURE_TDSCDMA_DSDS_QTA */
#endif /* FEATURE_TDSCDMA_DSDS_ALLOW_QTA_BEFORE_ACT_TIME */

#endif /* TDSSEQ_H */

