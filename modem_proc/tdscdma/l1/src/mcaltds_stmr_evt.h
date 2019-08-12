#ifndef MCALTDS_STMR_EVT_H
#define MCALTDS_STMR_EVT_H

/*============================================================================*/
/** @file
 * This module has definition and declaration related to MCAL UMTS downlink
 * channel setup module APIs
 */
/*============================================================================*/

/*============================================================================
Copyright (c) 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/mcaltds_stmr_evt.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
09/06/13    yuw      Supported Bolt
02/08/11    weijunz code clean up 
12/03/10    weijunz Add allocate/deallocate/set handler for runtime events
12/01/10    weijunz Add event defintion for TD-SCDMA L1 stmr events.

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
#include "tdscdma_variation.h"
#include "tdsstmr.h"
#include "tdsl1def.h"
#include "tdsl1const.h"

#if defined(FEATURE_TDSCDMA_PLT)
#include "tplt_cmd_upa_encoder_test.h"
#endif

/* Gen timeline events */
#define mcaltds_evt_sfn_cfn_update_event_handler tdsseq_sfn_cfn_update_event_handler
#if defined(FEATURE_TDSCDMA_PLT) && defined(FEATURE_TDSCDMA_PLT_JOLOKIA_SERIES)
#define mcaltds_evt_tx_hsupa_edch_event_handler tplt_eul_process_handling
#elif defined(FEATURE_TDSCDMA_WLAN_COEX)
#define mcaltds_evt_tx_hsupa_edch_event_handler tdseulsg_ts3_event_handler
#else
#define mcaltds_evt_tx_hsupa_edch_event_handler tdseulsg_start_edch_data_path_fiq_handler
#endif

#if defined(FEATURE_TDSCDMA_PLT)
#if defined(FEATURE_TDSCDMA_PLT_JOLOKIA_SERIES)
#define mcaltds_evt_tx_hsupa_plt_chkpnt_event_handler tplt_eul_encoding_done_handler
#else
#define mcaltds_evt_tx_hsupa_plt_event_handler tplt_eul_encoding_trigger
#endif
#endif

#define mcaltds_evt_rf_tune_done_event_handler tdsseq_rf_tune_done_event_handler


#define mcaltds_evt_chnl_setup_event_handler tdsseq_chnl_setup_event_handler

#define mcaltds_evt_rx_read_shared_mem_event_handler tdssrchmeas_internal_meas_heartbeat_event
/* rx timeline events */
#define mcaltds_evt_srch_heartbeat_event_handler tdssrchmeas_heartbeat_event_handler

#define mcaltds_evt_gsm_meas_sche_event_handler tdssrchgsm_begin_do_gsm_handler

#define mcaltds_evt_dl_trblk_header_read_event_handler tdsl1dec_trblk_header_read_event_handler
#define mcaltds_evt_dl_trblk_data_read_event_handler tdsl1dec_trblk_data_read_event_handler


#define mcaltds_evt_dl_dch_maint_event_handler tdsdlstates_dl_dch_maint_event_handler

#define mcaltds_evt_dl_rxd_state_update_event_handler tdsrxdiv_state_update_event_handler

//#ifdef FEATURE_TDSCDMA_CM_LTE_SEARCH
#if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
#define mcaltds_evt_tfw_slot_suspend_handler tdsgapmgr_chan_update_event_handler
#endif

/* tx timeline events */
#define mcaltds_evt_enc_get_data tdsenc_get_data

#ifdef FEATURE_TDSCDMA_L1M_PROFILING_LOG_PACKET
#define mcaltds_evt_profiling_trigger_event_handler tdsseq_profiling_trigger_event_handler
#endif

#define MCALTDS_EVT_GEN_TL_INTR TDSSTMR_INT_1

/* RF Tune Time in 256 Chip units - set to 3 ms=45*256 chipx1 units */
#define TDSSEQ_RF_TUNE_TIME   15

/*Event firing time is in unit of 32 chips*/
#define MCALTDS_EVT_EVENT_FIRING_TIME_UINT  32
#define MCALTDS_EVT_MAX_FIRING_TIME_UNIT_IN_SUBFRAME 200
#define MCALTDS_EVT_MAX_FIRING_TIME_UNIT_IN_FRAME 400

typedef void (*mcaltds_event_handler_type) (void) ;




/* Events on General Timeline */

typedef enum
{
  TDSSEQ_SFN_CFN_UPDATE_EVENT,
  TDSSEQ_CHNL_SETUP_EVENT,
  TDSSEQ_RXD_STATE_UPDATE_EVENT,
  TDSSEQ_TX_HSUPA_EDCH_EVENT,
  TDSSEQ_RX_GEN_SRCH_HB_EVENT,
//  TDSSEQ_MCVS_CTRL_EVENT_TS6,
  TDSSEQ_RX_GEN_DL_MAINT_EVENT,
  TDSSEQ_RX_DPA_DEC_MAINT_EVENT,
  TDSSEQ_TX_GEN_GET_DATA_EVENT,
  TDSSEQ_GSM_MEAS_SCHED_EVENT,
  TDSSEQ_RX_READ_SHARED_MEMORY_EVENT,
  TDSSEQ_RX_TIME_ADJUST_EVENT,
  TDSSEQ_RUNTIME_EVENT_1,
  TDSSEQ_RUNTIME_EVENT_2,
  #if defined(FEATURE_TDSCDMA_IDLEINTERVAL_INFO_SUPPORT) || defined(FEATURE_TDSCDMA_DMO_SUPPORT)
  TDSSEQ_FW_TIMESLOT_SUSPEND_EVENT,
  #else  
  TDSSEQ_RUNTIME_EVENT_3,
  #endif
  TDSSEQ_MCVS_CTRL_EVENT_TS2,
  TDSSEQ_RUNTIME_EVENT_TS6,
  TDSSEQ_NUM_GEN_TL_EVENTS,
  TDSSEQ_INVALID_EVENT_ID
} mcaltds_evt_gen_tl_event_id_enum_type;


/* Type definition for the Event Dispatch Function */
typedef void mcaltds_evt_event_dispatch_func_type
(
  uint16 event_mask
);
/* The following structure defines an Event Frame */
typedef struct
{

    /* Number of Events on the Event Frame.
   * For STMR_PROG_INT0, maximum number of Events is 16.
   * 
   */
   uint8 num_events;

  /* Event Mask of Events on the Event Frame. This defines
   * the initial state with which the Event Frame executes.
   * Bit N of the Event Mask corresponds to Event N on the
   * Event Frame.
   */
   uint16 event_mask;

   /*EVENT_PERIOD MASK. This defines the initial period for
   *the event. Bit N of this mask correspnds to Event N in
   *event frame. 0 means that event is fired every subframe(5ms)
    1 means that event is fired every frame(10ms).
   */
   tdsstmr_event_period_type *event_period_ptr;
   
  /* Pointer to an array of Firing Times of Events on the Event Frame.
   * Array Element N corresponds to Event N on the Reference Frame.
   */
   uint16 *firing_time_ptr;

   /* Event Dispatch Function. This function is called when an
    * Event occurs on the Event Frame. The Event Mask is passed
    * as the parameter to the function. Bit N of the Event Mask
    * corresponds to Event N on the Event Frame. Only one bit is
    * set when this function is called.
    */
   mcaltds_evt_event_dispatch_func_type *event_proc_func;

} mcaltds_evt_event_frame_type;



/*Current event firing time setting*/
extern uint16 mcaltds_evt_seq_event_firing_time_array[TDSSEQ_NUM_GEN_TL_EVENTS] ;

extern tdsstmr_event_period_type mcaltds_evt_seq_event_period_array[TDSSEQ_NUM_GEN_TL_EVENTS] ;

/*Current event ENABLE/DISABLE bitmask*/
extern uint16 mcaltds_evt_gen_tl_events_mask ;

extern void tdssrchmeas_heartbeat_event_handler(void);
extern void tdsdlstates_dl_dch_maint_event_handler(void);
extern void tdsenc_get_data(void);
extern void tdssrchmeas_internal_meas_heartbeat_event(void );
extern void tdsdechs_update_stats(void);
#ifdef FEATURE_TDSCDMA_PLT
extern void tplt_eul_encoding_trigger(void);
#endif


/*===========================================================================

FUNCTION mcaltds_evt_gen_tl_handler

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
extern void mcaltds_evt_gen_tl_handler( uint16 event_mask );


#define MCALTDS_EVT_EVENT_ID_TO_MASK(event) ((uint16) (1 << ((uint16)event)))

/*===========================================================================

                      FUNCTION MACROS

===========================================================================*/

/*===========================================================================
 
  
MACRO MCALTDS_EVT_ENABLE_EVENTS

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

#define MCALTDS_EVT_ENABLE_EVENTS( tdsintr, event_mask )                \
do {                                                         \
  tdsstmr_enable_events( (tdsstmr_int_type)tdsintr, (event_mask) );   \
} while(0)

/*===========================================================================

MACRO MCALTDS_EVT_DISABLE_EVENTS

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

#define MCALTDS_EVT_DISABLE_EVENTS( tdsintr, event_mask )              \
do {                                                        \
  tdsstmr_disable_events( (tdsstmr_int_type)tdsintr, (event_mask) ); \
} while(0)


/*===========================================================================

MACRO MCALTDS_EVT_MODIFY_EVENT_FIRING_TIME

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
#define MCALTDS_EVT_MODIFY_EVENT_FIRING_TIME( tdsintr, event_num, firing_time ) \
do {                                                                 \
  tdsstmr_modify_event_firing_time( tdsintr,                               \
                                event_num,                             \
                                 firing_time );                    \
} while(0)

/* This macro returns the event firing time of the specified event in units
 * of 32 chips
 */
#define MCALTDS_EVT_GET_FIRING_TIME( tdsintr, event_num ) \
  tdsstmr_get_firing_time( (tdsstmr_int_type)tdsintr, event_num )

/*===========================================================================

MACRO MCALTDS_EVT_MODIFY_EVENT_PERIOD

DESCRIPTION
  This macro modifies the period of the specified Event  on the specified Programmable
  Interrupt Source. The Event mask  for this Event should already have been programmed
  in the  Event Frame Register for this Programmable Interrupt for the Event  to occur.

  The Event Mask specifies which Event should be modified. Bit N of the mask  corresponds
  to Event N. The Event Mask should specify only one Event.  If more than one Event is
  specified in the Event Mask, only the period for the event that corresponds to
  the first bit set from the LSB is modified.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#define MCALTDS_EVT_MODIFY_EVENT_PERIOD( tdsintr, event_num, period ) \
do {                                                                 \
  tdsstmr_modify_event_period( tdsintr,                               \
                                 event_num,  \
                                 (tdsstmr_event_period_type)period );                    \
 } while(0)

/* This macro returns the event period */
#define MCALTDS_EVT_GET_EVENT_PERIOD( tdsintr, event_num ) \
  tdsstmr_get_event_period((tdsstmr_int_type)tdsintr, event_num )




/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================
FUNCTION     MCALTDS_EVT_INIT

DESCRIPTION
  This function initalizes timeline .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void mcaltds_evt_init(void);


/*===========================================================================
FUNCTION     MCALTDS_EVT_SAVE_EVENT_MASKS_FOR_SLEEP

DESCRIPTION
  This function saves the current event masks for general timeline in
  preparation for sleep.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcaltds_evt_save_event_masks_for_sleep ( void );

/*===========================================================================

FUNCTION     MCALTDS_EVT_PREPARE_FOR_SUSPEND

DESCRIPTION
  This function is called in response to the TDSL1_CPHY_SUSPEND_TDSCDMA_MODE_REQ
  primitive from RRC.  We are performing SUSPEND processing in preparation
  for handover/cell reselection to GSM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Disables all events except SFN/CFN update.

===========================================================================*/
void mcaltds_evt_prepare_for_suspend( void );


/*===========================================================================

FUNCTION     MCALTDS_EVT_ALLOCATE_EVENT_ID()

DESCRIPTION
  This function allows the user to request MCAL_EVT to return a run time event ID.
  If there is spare run time event id , MCAl return event ID .
  Otherwise, TDSSEQ_INVALID_EVENT_ID is return.

DEPENDENCIES
  None.

RETURN VALUE
 tdsseq_gen_tl_event_id_enum_type

SIDE EFFECTS
  No.

===========================================================================*/

mcaltds_evt_gen_tl_event_id_enum_type mcaltds_evt_allocate_event_id(void);

/*===========================================================================

FUNCTION     MCALTDS_EVT_SET_RUNTIME_EVENT_HANDLER()

DESCRIPTION
  This function allows the user to set event handler for one of run time event ID.
  Before this function called, user need to request MCAL_EVT to allocate a run time
  event ID by calling mcaltds_evt_allocateevent_id().
 

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  No

===========================================================================*/
boolean mcaltds_evt_set_runtime_event_handler(mcaltds_evt_gen_tl_event_id_enum_type runtime_event_id,mcaltds_event_handler_type  event_handler);

/*===========================================================================

FUNCTION     MCALTDS_EVT_DEALLOCATE_EVENT_ID()

DESCRIPTION
  This function allows the user to deallocate  a run time event ID that is
  allocated previously.  So that runtime event ID can be used by others .
  Event handler for that runtime event ID is also removed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  No.

===========================================================================*/
void mcaltds_evt_deallocate_event_id(uint8 runtime_event_id);

#endif /* MCALTDS_STMR_EVT_H*/

