#ifndef HDRTS_H
#define HDRTS_H

/*==========================================================================

                 T I M E  S T A M P   H E A D E R    F I L E

DESCRIPTION
  This module contains defintions needed to interface with the time
  stamp module.

  Copyright (c) 2000 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

============================================================================


============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/srch/api/hdrts.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------- 
11/21/13   mbs     Added support for enabling VSTMR after STMR block is clocked
09/26/13   mbs     Added HDR SW changes to use VSTMR
08/09/13   dsp     Added hdrts_reset_rtc(). 
05/15/13   kss     Added hdrts APIs for checking tx chip time. 
01/31/13   ljl     Added hdrts_config_mod_mp_time_ref() and hdrts_save_mstr().
03/21/12   ljl     Added hdrts_clear_pn_roll_interrupt().
06/13/11   ssu     Added hdrts_update_ts_time(). 
08/12/09   wsh     Reverted CMI related change due to restructuring
02/18/09   wsh     Remove ext-AU definitions, renamed hdrts_v
05/16/06   ljl     Added hdrts_get_current_time().
02/27/06   kss     Add hdrts_get_frame_time2().
07/05/05   ajn     Added hdrts_abort_sleep()
11/12/03   sq      hdrts_get_roll_count now returns "uint16", not "int16".     
03/11/03   ajn     Updated Copyright notice
11/07/02   ajn     Roll handler moved to hdrts.
02/19/02   ajn     Extended the range of sleep beyond 100seconds
11/29/01   om      Changed hdrts_get_frame_time() to be sleep aware
10/24/01   ajn     Added a callback for when time becomes valid.
09/04/01   ajn     Removd hdrts_is_valid - hdrts_get now returns valid flag
08/09/01   ajn     Added hdrts_invalidate(), enabled 426.7ms CCC
06/21/01   kss     Added hdrts_is_valid().
02/27/01   ajn     Added slot/cycle functions
10/06/00   ajn     Added hdrts_get_roll_count()
09/25/00   kss     Added hdrts_get_frame_time().
04/21/00   ajn     Initial cut.

==========================================================================*/


/* Eject */
/*==========================================================================

                        INTERFACE DEPENDANCIES

==========================================================================*/

#include "hdr_variation.h"
#include "qw.h"
#include "hdrl1.h"
#ifdef FEATURE_HDR_BOLT_MODEM
#include "vstmr_hdr.h"
#endif /* FEATURE_HDR_BOLT_MODEM */

/*==========================================================================

                            TYPE DEFINITIONS

==========================================================================*/



/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/


#define SCC_IS_426MS
#ifdef SCC_IS_426MS

#define HDRTS_FRAMES_PER_SCC  16
#define HDRTS_SCC_PER_CCC     12
#define HDRTS_80MS_PER_CCC    64

#else

#define HDRTS_FRAMES_PER_SCC  15
#define HDRTS_SCC_PER_CCC     13
#define HDRTS_80MS_PER_CCC    65

#endif /* SCC_IS_426MS */



#define HDRTS_SLOTS_PER_FRAME 16
  /* 16 slots (1.67ms) per frame (26.67ms) */

#define HDRTS_SLOTS_PER_SCC   (HDRTS_SLOTS_PER_FRAME*HDRTS_FRAMES_PER_SCC)
  /* Slots (1.67ms) per Synchronous Control Cycle (426.7ms) */

#define HDRTS_SLOTS_PER_SEC   600
  /* 600 slots per second */

#define HDRTS_SLOTS_PER_CCC   (HDRTS_SLOTS_PER_SCC*HDRTS_SCC_PER_CCC)
  /* Slots (1.67ms) per Control Channel Cycle (5.12s) */

#define HDRTS_HDRFW_RTC_FCW_INIT 0x83126E98
  /* To reset RTC count */

#ifdef FEATURE_HDR_BOLT_MODEM
/* This enum defines 3 main timelines for HDR RTC, RX, Tx
*/
typedef enum
{
  HDRTS_RTC,   /* RTC timeline */
  HDRTS_MOD,   /* Tx timeline */
  HDRTS_DMOD,  /* Rx timeline */
  TOTAL_TIMELINE_ITEMS
}hdrts_timeline_enum_type;

/* vstmr callback function type */
typedef void(*hdrts_vstmr_cb_type)(vstmr_hdr_event_tbl_s *, void *, uint32);
#endif /* FEATURE_HDR_BOLT_MODEM */

/* Eject */
/*==========================================================================

                          FUNCTION DECLARATIONS

==========================================================================*/


/*==========================================================================

CALLBACK HDRTS_TIME_VALID_CB_TYPE

DESCRIPTION
  A callback function that is invoked when time has been set valid.

DEPENDENCIES
  The callback must be passed to hdrts_set( ) and hdrts_wake( ) functions.

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

typedef
void (*hdrts_time_valid_cb_type)( void );


/*==========================================================================

CALLBACK HDRTS_ROLL_CB_TYPE

DESCRIPTION
  A callback function that is invoked when a PN roll occurs.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/

typedef
void (*hdrts_roll_cb_type)( void );


/*==========================================================================

FUNCTION HDRTS_INIT

DESCRIPTION
  This function initializes the time stamp services module.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Time is marked as invalid.

==========================================================================*/

void hdrts_init( void );


/*==========================================================================

FUNCTION HDRTS_INVALIDATE

DESCRIPTION
  This function asserts that HDR CDMA time is no longer reliable.  1x CDMA
  time, or Time-of-Day should be used instead.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Time is marked as invalid.

==========================================================================*/

void hdrts_invalidate( void );


/* Eject */
/*==========================================================================

FUNCTION HDRTS_SET

DESCRIPTION
  This function is used sets the current HDR time to the indicated number of
  26.7ms frames from the start of time (UTC=0, Jan 6, 00:00:00, 1980).

DEPENDENCIES
  hdrts_init()

RETURN VALUE
  None

SIDE EFFECTS
  Temporarily installs a modulator frame ISR to set time valid.
  Callback is called when HDR CDMA time is marked as valid.
  An Even-Second interrupt handler is installed to help maintain "gross
  time".

==========================================================================*/

void hdrts_set
(
  uint64                          frame_time,
    /* Time in frames (26.7ms) from UTC=0 (Jan 6, 1980 00:00:00 GMT) */

  hdrts_time_valid_cb_type        time_valid_cb
    /* Function to call when time has been set */
);


/*==========================================================================

FUNCTION HDRTS_GET

DESCRIPTION
  This function gets the current HDR time.

DEPENDENCIES
  hdrts_set()

RETURN VALUE
  TRUE if HDR Time is valid, and return the timestamp in 'ts_val'
  FALSE if HDR Time is invalid.

SIDE EFFECTS
  None

==========================================================================*/

boolean hdrts_get
(
  hdr_timestamp_t                 ts
);


/* EJECT */
/*==========================================================================

FUNCTION HDRTS_GET_80MS_TIME

DESCRIPTION
  This function gets the current HDR time (since THE BEGINNING) in 80ms
  superframes.

DEPENDENCIES
  hdrts_set()

RETURN VALUE
  HDR frame count is returned in 'superframe_val'

SIDE EFFECTS
  None

==========================================================================*/

void hdrts_get_80ms_time
(
  qword                           superframe_time
);


/*==========================================================================

FUNCTION HDRTS_GET_FRAME_TIME

DESCRIPTION
  This function gets the current HDR time (since THE BEGINNING) in frames.

DEPENDENCIES
  hdrts_set()

RETURN VALUE
  TRUE if time is available, otherwise FALSE

SIDE EFFECTS
  None

==========================================================================*/

boolean hdrts_get_frame_time
(
  qword                           frame_time
);


/*===========================================================================

FUNCTION HDRTS_GET_FRAME_TIME2

DESCRIPTION
  This function gets the current HDR time (since THE BEGINNING) in frames.
  This function will return a valid frame time, even if sync is in progress.

DEPENDENCIES
  hdrts_set() or hdrts_wake()

RETURN VALUE
  HDR frame count is returned in 'frame_val'

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrts_get_frame_time2
(
  qword                          frame_time
    /* Output: Timestamp value */
);


/*==========================================================================

FUNCTION HDRTS_GET_SLOT_TIME

DESCRIPTION
  This function returns the current slot (1.67ms) index into the current
  5.12s "paging period"  [Range: 0..3071]

DEPENDENCIES
  hdrts_valid == TRUE

RETURN VALUE
  Slots (1.67ms) since latest page boundary.

SIDE EFFECTS
  None

==========================================================================*/

uint16 hdrts_get_slot_time( void );


/* EJECT */
/*===========================================================================

FUNCTION HDRTS_GET_CYCLE_AND_SLOT

DESCRIPTION
  This function returns the current time in terms of arbitrary length cycles
  and slots into the cycle

DEPENDENCIES
  hdrts_valid == TRUE

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrts_get_cycle_and_slot
(
  qword                           cycle,
    /* Output: arbitrary length cycle index since UTC=0 */

  uint32                        * slot,
    /* Output: slot in the above cycle */

  uint32                          slots_per_cycle
    /* Arbitrary cycle length, in slots */
);


/*===========================================================================

FUNCTION HDRTS_FROM_CYCLE_AND_SLOT

DESCRIPTION
  This function returns the time corresponding to a cycle and slot in cycle,
  for an arbitrary length cycle.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrts_from_cycle_and_slot
(
  qword                           timestamp,
    /* Output: Time corresponding to the given cycle/slot */

  qword                           cycle,
    /* Arbitrary length cycle index since UTC=0 */

  uint32                          slot,
    /* Slot index after the start of the above cycle.  May be much larger
       than slots_per_cycle */

  uint32                          slots_per_cycle
    /* Arbitrary cycle length, in slots */
);


/* EJECT */
/*==========================================================================

FUNCTION HDR_TS_GET_PAGE_CYCLE

DESCRIPTION
  This function returns the number of paging cycle periods (5.12 seconds)
  since the beginning of time (UTC=0)

DEPENDENCIES
  hdrts_valid == TRUE

RETURN VALUE
  Paging cycles since UTC=0

SIDE EFFECTS
  None

==========================================================================*/

uint32 hdrts_get_page_cycle( void );


/*==========================================================================

FUNCTION HDR_TS_GET_ROLL_COUNT

DESCRIPTION
  Returns a number of PN rolls, wrapping around every 65536 rolls, or
  approximately every 29 minutes.

  HDRTS_PN_ROLL_COUNT_COMPARE( ) can be used to compare two count times
  within a 14 minute range, to determine which is earlier.

DEPENDENCIES
  None

RETURN VALUE
  Roll index.

SIDE EFFECTS
  None

==========================================================================*/

uint16 hdrts_get_roll_count( void );


/*==========================================================================

MACRO HDRTS_PN_ROLL_COUNT_COMPARE

DESCRIPTION
  Compares two "hdrts_get_roll_count()" values.

DEPENDENCIES
  Both roll count values must be within 14 minutes of each other.

RETURN VALUE
  Less than 0 -> first time is earlier than second.
  Equal to 0  -> both times are equal.
  Greater than 0  -> first time is later than second.

SIDE EFFECTS
  None

==========================================================================*/

#define HDRTS_ROLL_COUNT_COMPARE(a, b)  ( (int2) ( (a) - (b) ) )


/* EJECT */
/*==========================================================================

FUNCTION HDRTS_SLEEP

DESCRIPTION
  This function is used to inform the HDR Time Stamp Services about an
  upcoming discontinuity in the HDR CDMA time, cause by sleep.

DEPENDENCIES
  hdr_ts_set()

RETURN VALUE
  None

SIDE EFFECTS
  HDR CDMA time is marked invalid

==========================================================================*/

void hdrts_sleep( void );


/*==========================================================================

FUNCTION HDRTS_WAKE

DESCRIPTION
  This function is used to inform the HDR Time Stamp Services that the MSM
  has woke from sleep, as well as the length of the sleep.  The Time Stamp
  Services will then adjust for the discontinuity.

DEPENDENCIES
  hdrts_sleep()

RETURN VALUE
  None

SIDE EFFECTS
  Temporarily installs a modulator frame ISR to set time valid.
  Callback is called when HDR CDMA time is marked as valid.
  HDR CDMA time is advanced by the duration of the sleep interval.

==========================================================================*/

void hdrts_wake
(
  uint32                          sleep_dur_slots,
    /* The length of the sleep, in slots (1.67ms) */

  hdrts_time_valid_cb_type        time_valid_cb
    /* Function to call when time has been set */
);


/*===========================================================================

FUNCTION HDRTS_ABORT_SLEEP

DESCRIPTION
  This function is used to inform the HDR Time Stamp Services that the MSM
  has woke from sleep, but HDR timing does not need to be maintained.

DEPENDENCIES
  hdrts_sleep()

RETURN VALUE
  None

SIDE EFFECTS
  Demod Roll ISR processing may be re-enabled

===========================================================================*/

void hdrts_abort_sleep( void );


/* EJECT */
/*===========================================================================

FUNCTION HDRTS_REGISTER_ROLL_CALLBACK

DESCRIPTION
  This function registers PN ROLL callback function which will be called
  after receiving the PN ROLL ISR.

  The callback functions are executed in the PN ROLL ISR context so it is
  very important to have these callback functions quick and simple.  It is
  recommended that these callback functions just set a Rex Signal to the
  concerned task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May begin periodic PN roll ISR

===========================================================================*/

void hdrts_register_roll_callback
(
  hdrts_roll_cb_type              cb
    /* callback function to be registered */
);


/*===========================================================================

FUNCTION HDRTS_DEREGISTER_ROLL_CALLBACK

DESCRIPTION
  This function unregisters PN ROLL callback function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May end periodic PN roll ISR

===========================================================================*/

void hdrts_deregister_roll_callback
(
  hdrts_roll_cb_type              cb
    /* callback function to be registered */
);


/*===========================================================================

FUNCTION HDRTS_GET_CURRENT_TIME

DESCRIPTION
  This function gets the current HDR time (since THE BEGINNING) in slots 
  and the current sclk count.

DEPENDENCIES
  hdrts_set()

RETURN VALUE
  TRUE: 

SIDE EFFECTS
  None

===========================================================================*/

boolean hdrts_get_current_time
(
  qword                          now_slot,
    /* Time stamp in slots */
 
  uint32                         *now_sclk
    /* Sclk count */

);


/*===========================================================================

FUNCTION HDRTS_UPDATE_TS_TIME

DESCRIPTION
  This function updates the time managed by time services.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrts_update_ts_time( void );


/*===========================================================================

FUNCTION HDRTS_CLEAR_PN_ROLL_INTERRUPT

DESCRIPTION
  This function clears PN roll interrupt.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrts_clear_pn_roll_interrupt( void );


/*===========================================================================

FUNCTION HDRTS_CONFIG_MODTIME_REF

DESCRIPTION
  This function configures modtime_ref.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrts_save_mstr( uint32 );


/*===========================================================================

FUNCTION HDRTS_CONFIG_MODTIME_REF

DESCRIPTION
  This function configures modtime_ref.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrts_config_mod_mp_time_ref( void );


/*===========================================================================

FUNCTION HDRTS_UINV_DUMP

DESCRIPTION
  This function dumps RTC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrts_univ_dump( void );


/*===========================================================================

FUNCTION HDRTS_GET_TX_CHIP_TIME

DESCRIPTION
  Returns Tx time from modulator, in chips.

DEPENDENCIES
  Clocks must be on. Function does not check clock state.

RETURN VALUE
  Tx time in chips. Wraps at 75 frames. 

SIDE EFFECTS
  None

===========================================================================*/
uint32 hdrts_get_tx_chip_time( void );

/*===========================================================================

FUNCTION HDRTS_GET_TX_CHIP_TIME_ELAPSED

DESCRIPTION
  Compares and returns the difference between two Tx chips times.

DEPENDENCIES
  Time between samples should be < 2 seconds

RETURN VALUE
  Difference in chips

SIDE EFFECTS
  None

===========================================================================*/
uint32 hdrts_get_tx_chip_time_elapsed
(
  uint32 start,
  uint32 end
);

#ifdef FEATURE_HDR_BOLT_MODEM
/*===========================================================================

FUNCTION HDRTS_GET_RTC_CX1

DESCRIPTION
  A simple access function to return RTC in CX1 format from VSTMR

DEPENDENCIES
  Expects VSTMR to be active and event table registered

RETURN VALUE
  RTC in CX1

SIDE EFFECTS
  None

===========================================================================*/
uint32 hdrts_get_rtc_cx1(void);

/*===========================================================================

FUNCTION HDRTS_GET_RTC_CX2

DESCRIPTION
  A simple access function to return RTC in CX2 format from VSTMR

DEPENDENCIES
  Expects VSTMR to be active and event table registered

RETURN VALUE
  RTC in CX2

SIDE EFFECTS
  None

===========================================================================*/
uint32 hdrts_get_rtc_cx2(void);

/*===========================================================================

FUNCTION HDRTS_GET_RTC_CX8

DESCRIPTION
  A simple access function to return RTC in CX8 format from VSTMR

DEPENDENCIES
  Expects VSTMR to be active and event table registered

RETURN VALUE
  RTC in CX8

SIDE EFFECTS
  None

===========================================================================*/
uint32 hdrts_get_rtc_cx8(void);

/*===========================================================================

FUNCTION HDRTS_GET_FRAME_CNT

DESCRIPTION
  A simple access function to return current frame count from VSTMR

DEPENDENCIES
  Expects VSTMR to be active and event table registered

RETURN VALUE
  Frame count

SIDE EFFECTS
  None

===========================================================================*/
uint32 hdrts_get_frame_cnt(void);

/*===========================================================================

FUNCTION HDRTS_REGISTER_TIMELINE

DESCRIPTION
  Allows various timeline to be registered with vstmr

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  VSTMR views are registered

===========================================================================*/
void hdrts_register_timeline(hdrts_timeline_enum_type timeline);

/*===========================================================================

FUNCTION HDRTS_ENABLE_TIMELINE

DESCRIPTION
  Allows clients to request enabling of a timeline

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrts_enable_timeline
(
  hdrts_timeline_enum_type timeline,
  hdrts_vstmr_cb_type cb
);

/*===========================================================================

FUNCTION HDRTS_DISABLE_TIMELINE

DESCRIPTION
  Allows clients to request disabling of a timeline

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrts_disable_timeline(hdrts_timeline_enum_type timeline);

/*===========================================================================

FUNCTION HDRTS_ACTIVATE_PERIODIC_EVENT

DESCRIPTION
  Allows clients to request activation of a periodic event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrts_activate_periodic_event
(
  hdrts_timeline_enum_type timeline,
  uint32  first_trigger_point_in_slots,
  uint32  period_in_slots
);

/*===========================================================================

FUNCTION HDRTS_DEACTIVATE_TIMELINE_EVENT

DESCRIPTION
  Allows clients to request deactivate the events on a timeline

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrts_deactivate_timeline_event
(
  hdrts_timeline_enum_type timeline
);
#endif /* FEATURE_HDR_BOLT_MODEM */

/*===========================================================================

FUNCTION HDRTS_RESET_RTC

DESCRIPTION
  Resets RTC count

DEPENDENCIES
  None
 
RETURN VALUE
  None
 
SIDE EFFECTS
  None

===========================================================================*/
void hdrts_reset_rtc( void );

#endif /* HDRTS_H */

