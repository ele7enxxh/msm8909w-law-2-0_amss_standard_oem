#ifndef TDSSTMR_H
#define TDSSTMR_H

/*===========================================================================

      S Y S T E M    T I M E R    H E A D E R    F I L E

DESCRIPTION
  The System Timer Driver module contains the data structure declarations
  and function prototypes to interface with the System Timer Block.

  The System Timer block generates all the frame related timing
  (frame/symbol) signals. It also maintains TX System Time within 80ms range
  in 1/8 chip resolution.
  
  The System Timer block provides registers to set and maintain System Time.
  
  The System Timer block provides three Programmable Interrupts, with the
  ability to schedule Events at programmable Firing Times.
  
  The System Timer Driver provides other interfaces for the units within L1
  to interact with the System Timer block.
  
EXTERNALIZED FUNCTIONS

  tdsstmr_init
    This function sets up the interrupt handlers for the three Programmable
    Interrupts.

  tdsstmr_set_isr
    This function allows the clients to set a handler to each
    of the Programmable Interrupts. The registered handler will be called
    whenever there is an Event on the particular interrupt. The Event Number
    will be passed to the handler.

  tdsstmr_set_event_frame
    This function allows the user to set an Event Frame on a specific
    programmable interrupt. The Event Frames are periodic and has the
    following parameters:
    Intr                   - specifies the programmable interrupt
    Frame Ref Sel          - for STMR_PROG_INT0 and STMR_PROG_INT1, specifies
                             whether to use the Frame Reference Counter or
                             to use one of 4 Combiner Counters.
                             For STMR_PROG_INT2, specifies whether to use the
                             Frame Reference Counter or the TX System Time.
    Offset                 - specifies the Frame Offset relative to the
                             current reference counter in units of 256 chips.
                             The offset must be less than or equal to the Frame
                             Period. The time of all Events is advanced by the
                             amount of the offset.
    Event Mask             - specifies which Event is enabled. Bit N is set
                             true to enable event N on the specified STMR
                             interrupt.                      
  
    This function requires that the interrupt source is driven by one of 4
    combiner counters or the frame reference counter and further that the
    combiner counter is slaved to a physical channel. This should be done
    by a call to the function tdsstmr_align_counter.

 
  
  tdsstmr_set_event_firing_times
    This function sets the Firing Times of Events on the specified interrupt
    source. The Events to set are specified in an Event Mask. The Firing Times
    of the Events are specified in an array. Element N of the array corresponds
    to Event N.
  
    The Event Firing Times are specified relative to the start of the offset
    Event Frame in units of 256 chips. The Firing Time must be less than the
    interrupt frame length (10 ms).

  tdsstmr_add_event
    This function adds the specified Event on the specified interrupt source
    and sets the Firing Time of the Event to the specified value. If the
    Event was already present, it will have the value of the new Firing
    Time.

    The Event Mask specifies which Event should be added. Bit N of the mask
    corresponds to Event N. The Event Mask should only have one Event set.
    If more than one bit is set in the Event Mask, only the Event that
    corresponds to the first bit set from the LSB is added.

    The Event Firing Time is specified relative to the start of the offset
    Event Frame in units of 256 chips. The Firing Time must be less than the
    interrupt frame length (10 ms).

  tdsstmr_disable_events
    This function disables the specified Event from the Programmable Interrupt
    specified. It reads the current Event Mask from the Frame Register and
    then unsets the Mask of the Events to be disabled.
  
    There is no need to reset the Firing Times in the Event Registers, since
    the Events will not fire if it is not programmed in the Frame Register.

  tdsstmr_enable_events
    This function will enable the Events specified in the Event Mask on the
    specified interrupt source. Bit N of the mask corresponds to Event N.
    The Firing Times of the Events should already be programmed.

  tdsstmr_modify_event_firing_time
    This function modifies the Firing Time of the specified Event
    on the specified Programmable Interrupt Source. The Event mask
    for this Event should already have been programmed in the Event Frame
    Register for this Programmable Interrupt.

  tdsstmr_get_event_num
    This function returns the Event Number of an Event given a Event Mask.
    Bit N of the Event Mask corresponds to Event Number N (Event N).
    The parameter Event Mask is assumed to have only one bit set.
    If more than one bit in the Event Mask is set, only the
    Event Number corresponding to the first (least significant bit) bit
    set is returned.

  tdsstmr_get_system_time
    This function gets the System Time from the System Time Status register.

  tdsstmr_get_events_and_enable_only
    This function enables a set of events   
	for the specified interrupt. Additionally, it returns the mask of events 
	that were enabled on the interrupt just prior to calling this function.
    

INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  tdsstmr_init() should be called before any of the functions can be used.

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

Copyright (c) 2000 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/stmr.h_v   1.12   21 May 2002 19:06:44   cemani  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsstmr.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when        who   what, where, why
--------    ---   -------------------------------------------------------- 
11/29/10    weijunz Remove tdsstmr_modify_event_firing_times_in_chips() as it is internal function in stmr device driver.
11/23/11    weijunz Remove function tdsstmr_clear_interrupts() as it isn't called anywhere. 
11/19/11    weijunz Added STMR Macros for Nikel+TD modem
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "msm.h"

#include "tdsl1const.h"
//#include "tds_hwio_reg.h"
#include "msm.h"

#include "tdshal_stmr.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*             System Timer Programmable Interrupts Declarations           */
/*-------------------------------------------------------------------------*/
/*
 * System Timer provides three programmable interrupts to the microprocessor.
 * The following enumerations specify the interrupt sources.
 * Programmable Interrupt 0 (STMR0) has 16 Events associated with it.
 * Programmable Interrupt 1 (STMR1) has 08 Events associated with it.
 * Programmable Interrupt 2 (STMR2) has 08 Events associated with it.
 */
typedef enum {
  TDSSTMR_INT_0,  /* Programmable Interrupt 0 */
  TDSSTMR_INT_1,  /* Programmable Interrupt 1 */
  TDSSTMR_NUM_INT
} tdsstmr_int_type;


typedef enum {
  TDS_STMR_EVENT_PERIOD_5MS,  /* EVENT PERIOD is  5ms  */
  TDS_STMR_EVENT_PERIOD_10MS,  /* EVENT PERIOD is  10ms */
} tdsstmr_event_period_type;

/* this macro is used to convert a stmr_int_type enum, as defined above, to 
   it's corresponding actual STMR interrupt number as defined in tramp.h */



/*Register defintion for syetem timer in Nikel+TD modem*/
#define TDSSTMR_WALLTIME_COUNT_DYNAMIC_REG  TDS_STMR_WALLTIME_COUNT
#define TDSSTMR_WALLTIME_COUNT_STATUS_REG   TDS_STMR_WALLTIME_COUNT_STATUS
#define TDSSTMR_RXTIME_COUNT_DYNAMIC_REG    TDS_STMR_RXTIME_COUNT
#define TDSSTMR_RXTIME_COUNT_STATUS_REG     TDS_STMR_RXTIME_COUNT_STATUS

#ifndef TEST_FRAMEWORK 
#define TDSSTMR_STATUS_DUMP_CMD_REG         TDS_STMR_STATUS_DUMP_CMD 
#endif

#define TDSSTMR_WALLTIME_CMD_REG            TDS_STMR_WALLTIME_CMD
#define TDSSTMR_WALLTIME_MODIFIER_REG       TDS_STMR_WALL_TIME_MODIFIER
#define TDSSTMR_RXTIME_CMD_REG              TDS_STMR_RXTIME_CMD
#define TDSSTMR_RXTIME_MODIFIER_REG         TDS_STMR_RX_TIME_MODIFIER
#define TDSSTMR_ST_EVENT_CMD_REG            TDS_STMR_ST_EVENT_CMD
#define TDSSTMR_ST_EVENT_TIME_REG           TDS_STMR_ST_EVENT_TIME       
#define TDSSTMR_TIME_SYNC_CTL_REG           TDS_STMR_TIME_SYNC_CTL

/*
 * The Events on the Programmable Interrupts are specified by a bit mask.
 * Bit N specifies Event N on the specified interrupt.
 * Following are the definitions of the bit mask for each of the Events.
 * STMR0 has 16 events and STMR1/STMR2 have 8 events.
 */
#define TDSSTMR_INT_EVENT_0  0x1
#define TDSSTMR_INT_EVENT_1  0x2
#define TDSSTMR_INT_EVENT_2  0x4
#define TDSSTMR_INT_EVENT_3  0x8
#define TDSSTMR_INT_EVENT_4  0x10
#define TDSSTMR_INT_EVENT_5  0x20
#define TDSSTMR_INT_EVENT_6  0x40
#define TDSSTMR_INT_EVENT_7  0x80
#define TDSSTMR_INT_EVENT_8  0x100
#define TDSSTMR_INT_EVENT_9  0x200
#define TDSSTMR_INT_EVENT_10 0x400
#define TDSSTMR_INT_EVENT_11 0x800
#define TDSSTMR_INT_EVENT_12 0x1000
#define TDSSTMR_INT_EVENT_13 0x2000
#define TDSSTMR_INT_EVENT_14 0x4000
#define TDSSTMR_INT_EVENT_15 0x8000
  
/* max number of events on each of the interrupt sources */
#define TDSSTMR_RXTIME_INT_NUM_EVENTS 16

#define TDSSTMR_MAX_NUM_EVENTS 16
#define TDSSTMR_INT0_NUM_EVENTS  16

/* This Macro holds an invalid CFN value.*/
#define TDSSTMR_INVALID_CFN 0xFF

/* Max value of the Firing Time for any Event in units of 256 Chips */
#define TDSSTMR_MAX_EVENT_FIRING_TIME 49


#define TDSSTMR_MAX_EVENT_FIRING_TIME_IN_CHIPS 6399

/*TDS STMR MACRO DEFINITION START HERE*/
/*Not: SUBFRAME_COUNT is define as uint8 in one heder file for logging*/
#ifdef TEST_FRAMEWORK
#error code not present
#else
/*This Macro reads chipx8  counts in register TDS_STMR_WALLTIME_COUNT and returns chip counts*/

/*TDS STMR MACRO DEFINITION START HERE*/

#define TDSSTMR_WALLTIME_CHIP_COUNT_DYNAMIC()                   \
  (                                                             \
    tdshal_stmr_get_walltime_cx8() >> 3                         \
  )

/*This Macro reads chipx8  counts in register TDS_STMR_WALLTIME_COUNT and returns chipX8 counts*/
#define TDSSTMR_WALLTIME_CHIPX8_COUNT_DYNAMIC()                 \
  (                                                             \
    tdshal_stmr_get_walltime_cx8()                              \
  )

/*This Macro reads sub frame number count  in register TDS_STMR_WALLTIME_COUNT */
#define TDSSTMR_WALLTIME_SUBFRAME_NUM_DYNAMIC()                 \
  (                                                             \
    tdshal_stmr_get_walltime_sfn()                              \
   )
/*This Macro reads both sub frame number count and chipx8 count  in register TDS_STMR_WALLTIME_COUNT */
#define TDSSTMR_WALLTIME_DYNAMIC()                              \
  (                                                             \
    tdshal_stmr_get_walltime()                                  \
  )

/*This Macro reads chipx8  counts in register TDS_STMR_RXTIME_COUNT and returns chip counts*/
#define TDSSTMR_RXTIME_CHIP_COUNT_DYNAMIC()                     \
  (                                                             \
    tdshal_stmr_get_rxtime_cx8() >> 3                           \
  )

/*This Macro reads chipx8  counts in register TDS_STMR_RXTIME_COUNT and returns chipx8 counts*/
#define TDSSTMR_RXTIME_CHIPX8_COUNT_DYNAMIC()                   \
  (                                                             \
    tdshal_stmr_get_rxtime_cx8()                                \
  )

/*This Macro reads sub frame  number in register TDS_STMR_RXTIME_COUNT */
#define TDSSTMR_RXTIME_SUBFRAME_NUM_DYNAMIC()                   \
  (                                                             \
    tdshal_stmr_get_rxtime_sfn()                                \
  )

/*This Macro reads both sub frame  number and chipx8 count  in register TDS_STMR_RXTIME_COUNT */
#define TDSSTMR_RXTIME_DYNAMIC()                                \
  (                                                             \
    tdshal_stmr_get_rxtime()                                    \
  )


#define TDSSTMR_STATUS_DUMP_NOW()                               \
  (                                                             \
    tdshal_stmr_dump()                                          \
  )   

/*This Macro reads chipx8  counts in register TDS_STMR_WALLTIME_COUNT_STATUS and returns chip counts*/
#define TDSSTMR_WALLTIME_CHIP_COUNT_STATUS()                    \
  (                                                             \
    tdshal_stmr_dump_walltime_cx8() >>3                         \
  )

/*This Macro reads chipx8  counts in register TDS_STMR_WALLTIME_COUNT_STATUS and returns chipX8 counts*/
#define TDSSTMR_WALLTIME_CHIPX8_COUNT_STATUS()                 \
  (                                                             \
    tdshal_stmr_dump_walltime_cx8()                             \
  )



/*This Macro reads chipx8  counts in register TDS_STMR_WALLTIME_COUNT_STATUS and returns chipX8 counts*/
#define TDSSTMR_WALLTIME_SUBFRAME_NUM_STATUS()                 \
  (                                                            \
    tdshal_stmr_dump_walltime_sfn()                            \
  )

/*This Macro reads both sub frame count and chipx8 count in register TDS_STMR_WALLTIME_COUNT_STATUS */
#define TDSSTMR_WALLTIME_STATUS()                               \
  (                                                             \
    tdshal_stmr_dump_walltime()                                 \
  )

/*This Macro reads chipx8  counts in register TDS_STMR_RXTIME_COUNT_STATUS and returns chip counts*/
#define TDSSTMR_RXTIME_CHIP_COUNT_STATUS()                      \
  (                                                             \
    tdshal_stmr_dump_rxtime_cx8()                               \
    >>3                                                         \
  )

/*This Macro reads chipx8  counts in register TDS_STMR_RXTIME_COUNT_STATUS and returns chipX8 counts*/
#define TDSSTMR_RXTIME_CHIPX8_COUNT_STATUS()                    \
  (                                                             \
    tdshal_stmr_dump_rxtime_cx8()                               \
  )

/*This Macro reads SUB FRAME NUMBER in register TDS_STMR_RXTIME_COUNT_STATUS */
#define TDSSTMR_RXTIME_SUBFRAME_NUM_STATUS()                    \
  (                                                             \
    tdshal_stmr_dump_rxtime_sfn()                               \
  )

/*This Macro reads both SUB FRAME NUMBER  and Chipx8 count in register TDS_STMR_RXTIME_COUNT_STATUS */
#define TDSSTMR_RXTIME_STATUS()                                 \
  (                                                             \
    tdshal_stmr_dump_rxtime()                                   \
  )

#define TDSSTMR_PHASE_STATUS()                                  \
  (                                                             \
    tdshal_stmr_dump_phase()                                    \
  )

#define TDSSTMR_USTMR_STATUS()                                  \
  (                                                             \
    tdshal_stmr_dump_ustmr()                                    \
  )

/*This Macro select the source of ON_LINE_START signal 
* '00' select TD-SCDMA sleep controller 'ON_LINE_START' signal 
* '01' select GSM sleep controller 'ON_LINE_START' signal 
* '10' select OFDMA sleep controller  'ON_LINE_START' signal 
* '11' reserved
*/
#define TDSSTMR_TDS_SC_ON_LINE_START 0x0
#define TDSSTMR_GSM_SC_ON_LINE_START 0x1


#define TDSSTMR_SEL_ON_LINE_START(selcmd)                                        \
       HWIO_OUT(                                                                  \
                TDSSTMR_TIME_SYNC_CTL_REG,                            \
                selcmd                                                \
               )                                                 
                                                              
    

/*This Macro set Wall time modifier*/
#define TDSSTMR_SET_WALL_TIME_MODIFIER(subf_num,cx8_count)      \
         HWIO_OUT(                                               \
            TDSSTMR_WALLTIME_MODIFIER_REG,                      \
            ( ( (uint32)subf_num << 16 ) | cx8_count)  \
        )                                                       
                                                                         

/*This Macro load Wall Time counter*/
#define COUNTER_LOAD_IMMEDIATELY 0x3
#define COUNTER_LOAD_ON_LINE_START 0x4
#define COUNTER_LOAD_ST_EVENT  0x5

#define TDSSTMR_LOAD_WALL_TIME_COUNTER(load_cmd)            \
     (                                                      \
        HWIO_OUT(                                           \
            TDSSTMR_WALLTIME_CMD_REG,load_cmd               \
           )                                                \
     )                                                      

#define TDSSTMR_SET_RX_TIME_MODIFIER(subf_num,cx8_count)      \
     (                                                          \
        HWIO_OUT(                                               \
            TDSSTMR_RXTIME_MODIFIER_REG,                      \
            (subf_num << 16 | cx8_count)                      \
        )                                                       \
     )       

#define TDSSTMR_LOAD_RX_TIME_COUNTER(load_cmd)            \
     (                                                      \
        HWIO_OUT(                                           \
            TDSSTMR_RXTIME_CMD_REG,load_cmd               \
           )                                                \
     )         

/*This Macro enable ST_EVENT_INT. When ST_EVENT fires, IRQ td_wall_stmr_st_event_int is rasied*/
#define TDSSTMR_ENABLE_ST_EVENT_INT()                                   \
           OUTP32M(                                                        \
                   HWIO_ADDR(TDSSTMR_ST_EVENT_CMD_REG),                        \
                   HWIO_FMSK(TDSSTMR_ST_EVENT_CMD_REG,ST_EVENT_INT),           \
                   (0x1 << HWIO_SHFT(TDSSTMR_ST_EVENT_CMD_REG,ST_EVENT_INT))   \
            )                                                                  
                 


#endif /*end of ifdef TDS_STMR*/

/*TDS STMR MACRO DEFINITION END HERE*/
/*=================================================================*/









#define TDSSTMR_COMBINER0_COUNT_DYNAMIC( ) 1000


/* Macro to load the value of the Modifier register into the Frame
 * Reference Counters
 * If cmd is TDSSTMR_WALL_TIME_LOAD_IMMEDIATE, load the contents of
 * the Modifier register into the Frame Reference Counter immediately.
 */
#define TDSSTMR_WALL_TIME_LOAD_IMMEDIATE        0x4
#define TDSSTMR_WALL_TIME_LOAD_ON_LINE_START    0x5
#define TDSSTMR_WALL_TIME_LOAD_ST_EVENT         0x6


/*
 * Type for handler registered by the units in L1 for the STMR
 * interrupts. This handler will be called by the STMR whenever an
 * Event occurs on the interrupt.
 */
typedef struct
{
    /*current walltime counter sub frame number */
    uint16 wall_sfn ;
    /*current walltime counter chipx8 count*/
    uint16 wall_cx8 ;
    /*current rxtime counter sub frame number*/
    uint16 rx_sfn ;
    /*current rxtime counter chipx8 count*/
    uint16 rx_cx8 ;

} tdsstmr_counter_debug_info_type ;


typedef uint16 tdsstmr_event_mask_type ;


typedef void tdsstmr_int_isr_type
(
  tdsstmr_event_mask_type event_mask
);

typedef void tdsstmr_st_event_int_isr_type(void) ;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION TDSSTMR_INIT

DESCRIPTION
  This function sets up the interrupt handlers for the three Programmable
  Interrupts.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
 
 
===========================================================================*/
void tdsstmr_init( boolean reset );

/*===========================================================================

FUNCTION TDSSTMR_SET_ISR

DESCRIPTION
  This function allows the clients of L1 to set a handler to each
  of the Programmable Interrupts. The registered handler will be called
  whenever there is an Event(s) on the particular interrupt. The Event Mask
  of the occurring events will be passed to the handler.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tdsstmr_set_isr
(
 tdsstmr_int_type      tdsintr,
 tdsstmr_int_isr_type* isr_ptr
);

/*===========================================================================

FUNCTION TDSSTMR_REGISTER_ST_EVENT_ISR()

DESCRIPTION
  This function allows the clients of L1 to set a handler to TDS_WALL_STMR_ST_EVENT Interrupt. The registered handler will be called
  whenever TDS_WALL_STMR_ST_EVENT INTERRUPT IS RASIED.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tdsstmr_register_st_event_isr
(
  tdsstmr_st_event_int_isr_type* isr_ptr
);

/*===========================================================================

FUNCTION TDSSTMR_UNREGISTER_ST_EVENT_ISR()

DESCRIPTION
  This function allows the clients of L1 to set a handler to TDS_WALL_STMR_ST_EVENT Interrupt. The registered handler will be called
  whenever TDS_WALL_STMR_ST_EVENT INTERRUPT IS RASIED.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tdsstmr_unregister_st_event_isr( void);

/*===========================================================================

FUNCTION TDSSTMR_INSTALL_ISRS

DESCRIPTION
  This function installs/registers STMR interrupt handlers with the tramp services.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void tdsstmr_install_isrs( void );

/*===========================================================================

FUNCTION TDSSTMR_UNINSTALL_ISRS

DESCRIPTION
  This function uninstalls/deregisters STMR interrupt handlers with the tramp services.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tdsstmr_uninstall_isrs( void );

/*===========================================================================

FUNCTION TDSSTMR_DEACT_INIT

DESCRIPTION
  This function is called when WCDMA mode is deactivated. This function 
  disables all the events for the interrupt sources uninstalls/deregisters 
  STMR interrupt handlers with the tramp services.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tdsstmr_deact_init( void );


/*===========================================================================

FUNCTION TDSSTMR_SET_EVENT_FRAME

DESCRIPTION
  This function allows the user to set an Event Frame on a specific
  programmable interrupt. The Event Frames are periodic and has the
  following parameters:
  Intr                   - specifies the programmable interrupt
  Frame Ref Sel          - for STMR_PROG_INT0 and STMR_PROG_INT1, specifies
                           whether to use the Frame Reference Counter or
                           to use one of 4 Combiner Counters.
                           For STMR_PROG_INT2, specifies whether to use the
                           Frame Reference Counter or the TX System Time.
  Offset                 - specifies the Frame Offset relative to the
                           current reference counter in units of 256 chips.
                           The offset must be less than or equal to the Frame
                           Period. The time of all Events is advanced by the
                           amount of the offset.
  Event Mask             - specifies which Event is enabled. Bit N is set
                           true to enable event N on the specified STMR
                           interrupt.                      

  This function requires that the interrupt source is driven by one of 4
  combiner counters or the frame reference counter and further that the
  combiner counter is slaved to a physical channel. This should be done
  by a call to the function tdsstmr_align_counter.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tdsstmr_set_event_frame
(
  tdsstmr_event_mask_type event_mask
);

/*===========================================================================

FUNCTION TDSSTMR_SET_EVENT_PERIODS

DESCRIPTION
  This function sets the  period of Events on the specified interrupt  source.
  The event periods of the Events are specified in an uint16 variable. MSB of this variable
  corresponds to event 15  and LSB correspondes to event 0. The number of Events for which the
  event period  should be programmed is passed to this function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tdsstmr_set_event_periods
(
  tdsstmr_event_period_type* event_period_ptr,
  uint8  num_events
);
/*===========================================================================

FUNCTION TDSSTMR_SET_EVENT_FIRING_TIMES

DESCRIPTION
  This function sets the Firing Times of Events on the specified interrupt
  source. The Firing Times of the Events are specified in an array. Element
  N of the array corresponds to Event N. The number of Events for which the
  Firing Times should be programmed is passed to this function.

  The Event Firing Times are specified relative to the start of the offset
  Event Frame in units of 32 chips. The Firing Time must be less than the
  interrupt frame length i.e. should take the value between 0 and 399.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tdsstmr_set_event_firing_times
(
 uint16  *event_firing_time_array_ptr,
 uint8  tdsnum_events
);

/*===========================================================================

FUNCTION TDSSTMR_ENABLE_EVENTS

DESCRIPTION
  This function enables the Events specified in the Event Mask on the
  specified interrupt source. This function assumes that the Firing Times
  of these Events have already been programmed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tdsstmr_enable_events
(
 tdsstmr_int_type tdsintr,
 tdsstmr_event_mask_type event_mask
);

/*===========================================================================

FUNCTION TDSSTMR_DISABLE_EVENTS

DESCRIPTION
  This function disables the specified Events from the Programmable
  Interrupt specified. It reads the current Event Mask from the Frame
  Register and then unsets the Mask of the Events to be disabled and programs
  the new Event Mask to the Frame Register.

  There is no need to reset the Firing Time in the Event Register, since the
  Event will not fire if it is not programmed in the Frame Register.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tdsstmr_disable_events
(
 tdsstmr_int_type tdsintr,
 tdsstmr_event_mask_type event_mask
);



/*===========================================================================

FUNCTION TDSSTMR_MODIFY_EVENT_FIRING_TIME

DESCRIPTION
  This function modifies the firing time of the specified Event
  on the specified Programmable Interrupt Source. The Event mask
  for this Event should already have been programmed in the
  Event Frame Register for this Programmable Interrupt.

  The Event Mask specifies which Event should be modified. Bit N of the mask
  corresponds to Event N. The Event Mask should specify only one Event.
  If more than one Event is specified in the Event Mask, only the firing time of the 
  Event that corresponds to the first bit set from the LSB is modified.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tdsstmr_modify_event_firing_time
(
 tdsstmr_int_type tdsintr,
 uint8         event_num,
 uint16       firing_time
);


/*===========================================================================

FUNCTION TDSSTMR_MODIFY_EVENT_PERIOD

DESCRIPTION
  This function modifies the period of the specified Event
  on the specified Programmable Interrupt Source. The Event mask
  for this Event should already have been programmed in the
  Event Frame Register for this Programmable Interrupt.

  The Event Mask specifies which Event should be modified. Bit N of the mask
  corresponds to Event N. The Event Mask should specify only one Event.
  If more than one Event is specified in the Event Mask, only the Period of the 
  Event that corresponds to the first bit set from the LSB is modified.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tdsstmr_modify_event_period
(
 tdsstmr_int_type tdsintr,
 uint8 event_num,
 tdsstmr_event_period_type       period
);

/*===========================================================================

FUNCTION TDSSTMR_GET_EVENT_NUM

DESCRIPTION
  Return the Event Number of an Event given a Event Mask.
  Bit N of the Event Mask corresponds to Event Number N (Event N).
  The parameter Event Mask is assumed to have only one bit set.
  
  If more than one bit in the Event Mask is set, only the
  Event Number corresponding to the first (least significant bit) bit
  set is returned.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern uint8 tdsstmr_get_event_num
(
  tdsstmr_event_mask_type event_mask
);




/*===========================================================================

FUNCTION TDSSTMR_GET_FIRING_TIME

DESCRIPTION
  This function gets the firing time of the specified event on the
  specified interrupt.

DEPENDENCIES
  None

RETURN VALUE
  Firing Time of Event in units of 256 chips.

SIDE EFFECTS
  None

===========================================================================*/
uint32 tdsstmr_get_firing_time
(
  tdsstmr_int_type tdsintr,
  uint8         event_num
);


/*===========================================================================

FUNCTION TDSSTMR_GET_EVENT_PERIOD

DESCRIPTION
  This function gets the period of the specified event on the
  specified interrupt.

DEPENDENCIES
  None

RETURN VALUE
  Enum: tdsstmr_event_period_type

SIDE EFFECTS
  None

===========================================================================*/
tdsstmr_event_period_type tdsstmr_get_event_period
(
  tdsstmr_int_type tdsintr,
  uint8         event_num
);

/*===========================================================================

FUNCTION TDSSTMR_GET_CURRENT_TIME

DESCRIPTION
  This function gets the current time in units of 32 chips of the
  Rx time counter with regard to the frame boundary.

DEPENDENCIES
  None

RETURN VALUE
  current time in units of 32 chips.

SIDE EFFECTS
  None

===========================================================================*/
uint32 tdsstmr_get_current_time(void);

/*===========================================================================

FUNCTION TDSSTMR_GET_CURRENT_TIME

DESCRIPTION
   Gets the value in chipx8 of the Rx time counter with regard to frame boundary.
  It comands a status dump as part of the operation.

DEPENDENCIES
  None

RETURN VALUE
  Counter value in chipx8 for the specified counter.

SIDE EFFECTS
  Does a status dump as part of its operation.
===========================================================================*/
uint32 tdsstmr_get_current_time_chipx8
(
  tdsstmr_int_type tdsintr
);

/*===========================================================================

FUNCTION TDSSTMR_GET_EVENTS_AND_ENABLE_ONLY

DESCRIPTION
  This function enables only the events specified
  by the enable_only_event_mask, for the specified
  interrupt, other events are effectively disabled. 
  Additionally, it returns the mask of
  events that were enabled on the interrupt 
  just prior to calling 
  this function.

DEPENDENCIES
  None

RETURN VALUE
  mask of events that were enabled on the interrupt 
  just prior to calling this function

SIDE EFFECTS

===========================================================================*/
extern uint16 tdsstmr_get_events_and_enable_only
(
  tdsstmr_int_type tdsintr,
  uint16        enable_only_event_mask
);

/*===========================================================================

FUNCTION TDSSTMR_GET_EVENT_MASK

DESCRIPTION
  This function returns the current event mask on the interrupt specified.

DEPENDENCIES
  None

RETURN VALUE
  mask of events that were enabled on the interrupt 
  just prior to calling this function

SIDE EFFECTS

===========================================================================*/
extern uint16 tdsstmr_get_event_mask(void);




extern int16 tdsstmr_hw_sw_cfn_diff;



/*===========================================================================

FUNCTION TDSSTMR_UPDATE_EVENT_PROCESSING_STATUS

DESCRIPTION
  This function updates the processed event status

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
void tdsstmr_update_event_processing_status(tdsstmr_int_type tdsintr, uint16 evt);


/*===========================================================================

FUNCTION TDSSTMR_SLAM_RXTIME_COUNTER

DESCRIPTION
  This function sets the combiner with the passed value
  - coutner   : The counter to set
  - slam_value: The value to programmed to the counter
  - frc_target: The value of frc when the counter value will be slammed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tdsstmr_slam_rxtime_counter(uint32 slam_value, 
                           uint32 frc_target);

extern void tdsstmr_default_isr( tdsstmr_event_mask_type event_mask );

void tdsstmr_get_all_counter_info(tdsstmr_counter_debug_info_type* stmr_counter_info_ptr);

/*! \brief rx-time and wall-time STMR Offset including both subframe and chipx8 count */
typedef struct
{  
  /* the offset between rx-time chipx8 (x) and wall-time chipx8 (y)
     = (y >= x) ? (y-x) : ( y + 51200 - x) */
  uint32  cx8_ofs : 16;
  uint32  subFrm_ofs : 13;
  uint32  : 3;
}tdsstmr_offset_structure_type;
extern tdsstmr_offset_structure_type tdsstmr_offset;
/* the threshold to adjust rx-time counter
   if the change of rx-time counter (cx8) is larger than this threshold, 
      adjust rx-time counter */
extern const uint16 RX_TIME_ADJ_TH;
extern const uint16 MAX_NUM_RX_TIME_ADJ_CX8;
extern boolean tdsstmr_rx_time_adj_ongoing;

/*===========================================================================

FUNCTION TDSSTMR_INIT_RX_TIME_OFFSET_CX8

DESCRIPTION
  after acquiring a cell and slaming rx-time done, initilize the offset in chipx8 
  between rx-time counter and wall-time counter;

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  

===========================================================================*/
extern void tdsstmr_init_rx_time_offset_cx8(uint16 offset_sub_fn, uint16 offset_cx8);


/*===========================================================================

FUNCTION TDSSTMR_INIT_RX_TIME_OFFSET_CX8

DESCRIPTION
  after getting FW TTL (time tracking loop) response, 
  we might need re-program rx-time counter if the offset change exceed a threshold;

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  

===========================================================================*/
extern void tdsstmr_update_rx_time_offset_cx8(uint16 offset_subFrm, uint16 offset_cx8);


extern void tdsstmr_test_dump_register(void);

#endif /* TDSSTMR_H */

