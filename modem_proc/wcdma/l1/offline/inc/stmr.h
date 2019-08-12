#ifndef STMR_H
#define STMR_H
/*===========================================================================

     S I M B A    L 1    S Y S T E M    T I M E R    H E A D E R    F I L E

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

  stmr_init
    This function sets up the interrupt handlers for the three Programmable
    Interrupts.

  stmr_set_isr
    This function allows the clients to set a handler to each
    of the Programmable Interrupts. The registered handler will be called
    whenever there is an Event on the particular interrupt. The Event Number
    will be passed to the handler.

  stmr_set_event_frame
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
    by a call to the function stmr_align_counter.

  stmr_align_counter
    This function aligns the specified interrupt source to the specified
    counter source.
    counter sources for int0 and int1
    STMR_COMBINER_COUNTER0
    STMR_COMBINER_COUNTER1
    STMR_COMBINER_COUNTER2
    STMR_COMBINER_COUNTER3
    counter source for int2 only
    STMR_TX_SYSTEM_TIME_COUNTER
    counter source for int0, int1, and int2
    STMR_FRAME_REF_COUNTER
  
  stmr_set_event_firing_times
    This function sets the Firing Times of Events on the specified interrupt
    source. The Events to set are specified in an Event Mask. The Firing Times
    of the Events are specified in an array. Element N of the array corresponds
    to Event N.
  
    The Event Firing Times are specified relative to the start of the offset
    Event Frame in units of 256 chips. The Firing Time must be less than the
    interrupt frame length (10 ms).

  stmr_add_event
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

  stmr_disable_events
    This function disables the specified Event from the Programmable Interrupt
    specified. It reads the current Event Mask from the Frame Register and
    then unsets the Mask of the Events to be disabled.
  
    There is no need to reset the Firing Times in the Event Registers, since
    the Events will not fire if it is not programmed in the Frame Register.

  stmr_enable_events
    This function will enable the Events specified in the Event Mask on the
    specified interrupt source. Bit N of the mask corresponds to Event N.
    The Firing Times of the Events should already be programmed.

  stmr_modify_event_firing_time
    This function modifies the Firing Time of the specified Event
    on the specified Programmable Interrupt Source. The Event mask
    for this Event should already have been programmed in the Event Frame
    Register for this Programmable Interrupt.

  stmr_get_event_num
    This function returns the Event Number of an Event given a Event Mask.
    Bit N of the Event Mask corresponds to Event Number N (Event N).
    The parameter Event Mask is assumed to have only one bit set.
    If more than one bit in the Event Mask is set, only the
    Event Number corresponding to the first (least significant bit) bit
    set is returned.

  stmr_get_system_time
    This function gets the System Time from the System Time Status register.

  stmr_get_events_and_enable_only
    This function enables a set of events   
	for the specified interrupt. Additionally, it returns the mask of events 
	that were enabled on the interrupt just prior to calling this function.
    

INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  stmr_init() should be called before any of the functions can be used.

REFERENCES
  CDMA Mobile Station ASIC Specification Sheet

Copyright (c) 2000 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/stmr.h_v   1.12   21 May 2002 19:06:44   cemani  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/stmr.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who   what, where, why
--------    ---   --------------------------------------------------------
12/22/14    pr    Code cleanup.
08/21/14    pr    Code cleanup for JOLOKIA
07/01/14    hk    Replaced FEATURE_WCDMA_BOLT_STMR_TEST with FEATURE_DEBUG_STMR_TEST
04/15/14    pr    F3 reduction.
03/27/14    pr    Adjusts the aligned counter with the offset equal to the cell pos slew after sleep
04/21/14    kcm   Bolt Rumi Featurization Removal  
03/13/14    kcm   Changes to enable/release VFR
01/06/14    kcm   Added STMR_TX_SYS_TIME_FULL_SEG_STATUS macro to retrieve the
                  Full segment Tx status from W STMR read and corrected the macro
                  used to compute the TST
10/02/13    pr    VSTMR changes for EUL FIQ
09/14/13    pr    VSTMR Changes for BOLT
11/14/13    pj    Wrapped HWIO access with WL1_HWIO macro
05/09/13    pr    Featurized code under FEATURE_WCDMA_BOLT_RUMI_HWIO_DEMOD as part of Bolt Bringup.
02/14/12    pr    Remove Tx Timeline Check for STMR counter source.
07/24/12    geg   Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/05/12    abs   STMR hw_sw counters mapping changes for Dime
05/22/12    pr    STMR changes for Dime
03/28/12    zr    Added feature definitions for Dime
04/17/12    pr    Removed inclusion of "clk.h"
04/16/11    sj    Added #include "rex.h" to resolve compilation errors
03/23/11    hk    Removed the clk_busy_wait of 1us from STMR_STATUS_DUMP
02/04/11    dm    Added mutex protection to stmr_status_dump.
02/26/10    rvs   Fixes for Mustang.
11/23/09    rvs   Added function stmr_debug_print_sfn_cfn_update_info().
11/20/09    rvs   Extern stmr_prog_int_num_to_intr and stmr_cdma_prog_intr_isr.
09/14/09    sv    Removed WCDMA_INTLOCK's.
12/24/08    vsr   Added support for maintaining event processing status
12/10/08    ks    Mainlining FEATURE_WCDMA_DL_ENHANCED
10/03/08    hk    Bringing in 7k Mailine fixes
11/05/07    mg    Taxis stmr changes
12/11/07    mg    Use HWIO_ADDRI to access stmr registers
11/27/07    mg    Fix STMR_FRAME_NUM_STATUS() macro
10/24/07    vsr   Mainlining MBMS and Enhanced DL changes 
09/06/07    gnk   Added support for combiner log packet when FEATURE_WCDMA_DL_ENHANCED
                  is not defined
01/31/07    vr    Added STMR_COMBINER_COUNT_STATUS macro
                  Added function stmr_get_combiner_cfn() to return cfn of a 
                  combiner from the corresponding STMR register 
12/22/06    rmak  Added STMR_FRAME_NUM_STATUS, STMR_FRAME_REF_COUNT_DYNAMIC and updated STMR_FRAME_NUM_DYNAMIC
11/16/06    rmak  Added STMR_FRAME_NUM_DYNAMIC macro
09/12/06    gr/sh Removed macros to convert stmr ints to/from tramp ints.
05/08/06    au    Replaced calls to MSM_IN and MSM_OUT with HWIO_IN and HWIO_OUT.
09/14/05    gs    Updates for 6280 register name changes
05/31/05    vp    Merging of fixes for Lint errors
11/11/03    yus   Replace FEATURE_6250_COMPILE with FEATURE_MAP_6250_DEFINES 
09/29/03    gw    Added STMR_WALL_TIME_LOAD_ST_EVENT to commands for
                  STMR_LOAD_FRAME_REF_COUNTER macro.
07/24/03    yus   Map tramp_int_num_to type tramp_isr_type for MSM6250.
06/10/03    yus     Add HWIO_XXXX style macros support.
01/13/02    src   Added a new macro STMR_STATUS_DUMP_NO_WAIT(), which differs
                  from the existing macro STMR_STATUS_DUMP() in that the former
                  does not automatically do a clock-wait.
12/06/02    gw    Added STMR_WALL_TIME_LOAD_ON_LINE_START to commands for
                  STMR_LOAD_FRAME_REF_COUNTER macro.
10/30/02    src   Added function declaration for stmr_get_event_mask, a new
                  function.
07/26/02    sh    Modified STMR_STATUS_DUMP to include 1us delay for hardware
                  to latch.
05/21/02    src   Re-instated the frame-reference bit-mask in place of the full
                  read-mask in the macro STMR_FRAME_REF_COUNT_STATUS().
05/19/02    src   In enum stmr_counter_source_enum_type, added a last element
                  to count the numre of counter sources.
02/11/02    asr   Added prototype for function stmr_get_events_and_enable_only()
10/10/01    sh    Modified macros using write-only registers to use MSM_OUT.
04/30/01    sh    Added STMR_MAX_EVENT_FIRING_TIME constant define.
01/23/01    ml    Added ability to register/de-register stmr isrs with
                  tramp.
11/16/00    sh    Added support to align counter sources to phy chan.
                  Modified TX system time macros.
                  Modified STMR_IN STMR_OUT macros to use OUTP/INP macros.
10/25/00    sh    Added TX System Time related macros.
09/27/00    sh    Created file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#include "comdef.h"
#include "msm.h"
#include "DALSys.h"
#include "DDIInterruptController.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#include "l1const.h"
#include "rex.h"
#include "l1msm.h"
#include "vstmr_wcdma.h"
#include "vstmr.h"


#ifdef FEATURE_WCDMA_RUMI_SUPPORT
#define FEATURE_DEBUG_STMR_TEST
#endif


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
  STMR_INT_0,  /* Programmable Interrupt 0 */
  STMR_INT_1,  /* Programmable Interrupt 1 */
  STMR_INT_2,  /* Programmable Interrupt 2 */
  STMR_INT_3,  /*Programmable Interrupt 3(EUL IRQ)*/
  STMR_NUM_INT
} stmr_int_type;

/* this macro is used to convert a stmr_int_type enum, as defined above, to 
   it's corresponding actual STMR interrupt number as defined in tramp.h */

typedef void (*tramp_void_handler_type) (void);
typedef tramp_void_handler_type isr_ptr_type;
#define DALInterruptID tramp_isr_type

/* The following are register definitions depending on the System timer revision */

/* Currently these register definitions are in taxis modem system timer */

#define STMR_REF_COUNT_STATUS_REG     STMR_REF_COUNT_STATUS_PRI
#define STMR_REF_COUNT_REG            STMR_REF_COUNT
#define STMR_TX_TIME_STATUS_REG       STMR_TX_TIME_STATUS_PRI
#define STMR_STATUS_DUMP_CMD_REG      STMR_STATUS_DUMP_CMD_PRI
#define STMR_Cd_COUNT_STATUS_REG      STMR_Cd_COUNT_STATUS
#define STMR_TIME_CMD_REG             STMR_TIME_CMD_PRI

/*
 * The Events on the Programmable Interrupts are specified by a bit mask.
 * Bit N specifies Event N on the specified interrupt.
 * Following are the definitions of the bit mask for each of the Events.
 * STMR0 has 16 events and STMR1/STMR2 have 8 events.
 */
#define STMR_INT_EVENT_0  0x1
#define STMR_INT_EVENT_1  0x2
#define STMR_INT_EVENT_2  0x4
#define STMR_INT_EVENT_3  0x8
#define STMR_INT_EVENT_4  0x10
#define STMR_INT_EVENT_5  0x20
#define STMR_INT_EVENT_6  0x40
#define STMR_INT_EVENT_7  0x80
#define STMR_INT_EVENT_8  0x100
#define STMR_INT_EVENT_9  0x200
#define STMR_INT_EVENT_10 0x400
#define STMR_INT_EVENT_11 0x800
#define STMR_INT_EVENT_12 0x1000
#define STMR_INT_EVENT_13 0x2000
#define STMR_INT_EVENT_14 0x4000
#define STMR_INT_EVENT_15 0x8000
  
/* max number of events on each of the interrupt sources */
#define STMR_MAX_NUM_EVENTS 16
#define STMR_INT0_NUM_EVENTS  16
#define STMR_INT1_NUM_EVENTS  16
#define STMR_INT2_NUM_EVENTS  16
#define STMR_INT3_NUM_EVENTS  16

/* This Macro holds an invalid CFN value.*/
#define STMR_INVALID_CFN 0xFF

/* Max value of the Firing Time for any Event in units of 256 Chips */
#define STMR_MAX_EVENT_FIRING_TIME 149

#define STMR_MAX_EVENT_FIRING_TIME_IN_CHIPS (38400 -1)

/* This Macro holds the value that needs to be written to STMR_CFN_MODIFIER_SEL
 * field so that microprocessor can take control of the CFN field modifier.
*/
#define STMR_GET_MICRO_ACCESS_FOR_CFN_MODIFIER 0x1

/* counter source to slave the Programmable interrupts to;
 * These are the non-hsdpa counters; They are numbered according to the
 * mdsp convention
 */
typedef enum
{
  /* counter sources for int0 and int1 */
  STMR_COMBINER_COUNTER0=0,
  STMR_COMBINER_COUNTER1,
  STMR_COMBINER_COUNTER2,
  /* counter source for int2 only */
  STMR_TX_SYSTEM_TIME_COUNTER,

  /* counter source for int0, int1, and int2 */
  STMR_FRAME_REF_COUNTER,
  STMR_MAX_NUM_COUNTERS
} stmr_counter_source_enum_type;

extern uint32 stmr_prog_int_num_to_intr[STMR_NUM_INT];
extern isr_ptr_type stmr_cdma_prog_intr_isr[STMR_NUM_INT];

#ifdef FEATURE_DEBUG_STMR_TEST
  extern vstmr_wstmr_t stmr_vstmr_status_dump[STMR_MAX_NUM_COUNTERS];
#endif /*FEATURE_DEBUG_STMR_TEST*/

extern rex_crit_sect_type stmr_status_dump_mutex;

#define STMR_STATUS_DUMP_LOCK()  REX_ISR_LOCK(&stmr_status_dump_mutex)
#define STMR_STATUS_DUMP_UNLOCK()  REX_ISR_UNLOCK(&stmr_status_dump_mutex)

/*The following combiners are supported:
  Dime - 3
  Pre-Dime - 4*/
#define STMR_SUPPORTED_COMBINERS 3

/*PR_NOTES: New API for frc dynamic to read the frame and cx1 value*/
#define STMR_FRC_DYNAMIC(frame_number, frame_cx1)                                       \
do{  \
      vstmr_wstmr_t stmr_count;  \
      stmr_count = vstmr_wcdma_get_current_wstmr(stmr_view[STMR_FRAME_REF_COUNTER]);  \
      frame_number = stmr_count.frame_no;  \
      frame_cx1 = (stmr_count.subframecx8)>>3;  \
   }while(0)
/*PR_NOTES: New API for frc dynamic to read the frame and cx1 value*/

/* Macro to load the value of the Modifier register into the Frame
 * Reference Counters
 * If cmd is STMR_WALL_TIME_LOAD_IMMEDIATE, load the contents of
 * the Modifier register into the Frame Reference Counter immediately.
 */
#define STMR_WALL_TIME_LOAD_IMMEDIATE        0x4
#define STMR_WALL_TIME_LOAD_ON_LINE_START    0x5
#define STMR_WALL_TIME_LOAD_ST_EVENT         0x6


#define STMR_LOAD_FRAME_REF_COUNTER( cmd ) \
  MSG_ERROR("WCDMA_BOLT_DEMOD:Commented stmr_load_frc for Bringup , counter %d ", cmd, 0, 0);

/* This macro sets the STMR_WALL_TIME_MODIFIER register with
 * Slot Counter and Frame Counter values. The mapping is
 * <21:19> Slot Counter
 * <18:0>  Frame Counter
 */

#define STMR_SET_WALL_TIME_MODIFIER( slot_counter, frame_counter )   \
  MSG_ERROR("Commented for Bringup(SLEEP) ,slt ctr%d  fr ctr %d", slot_counter, frame_counter, 0);


#define STMR_CNTR_MAX_COMB_CNTR (STMR_COMBINER_COUNTER2)

#define STMR_IS_CNTR_IN_COMB_CNTR_RANGE(cntr)                        \
    (cntr <= STMR_CNTR_MAX_COMB_CNTR)

#define STMR_IS_CNTR_EQ_TO_TX_CNTR(cntr)                             \
    (cntr == STMR_TX_SYSTEM_TIME_COUNTER)

#define STMR_IS_CNTR_EQ_TO_FRC(cntr)                                 \
    (cntr == STMR_FRAME_REF_COUNTER)

#define STMR_IS_INTR_RX_TL(intr)    (intr == MCALWCDMA_EVT_RX_TL_INTR)
#define STMR_IS_INTR_GEN_TL(intr)    (intr == MCALWCDMA_EVT_GEN_TL_INTR)

#define STMR_IS_INTR_TX_TL(intr)    (intr == MCALWCDMA_EVT_TX_TL_INTR)
#define STMR_IS_INTR_EUL_TL(intr)    (intr == MCALWCDMA_EVT_EUL_TL_INTR)

#define STMR_IS_CNTR_SRC_WRONG_FOR_RX_TL(intr, cntr_src)             \
    (STMR_IS_INTR_RX_TL(intr) && (STMR_IS_CNTR_EQ_TO_TX_CNTR(cntr_src)))

#define STMR_IS_CNTR_SRC_WRONG_FOR_GEN_TL(intr, cntr_src)            \
    (STMR_IS_INTR_GEN_TL(intr) && (STMR_IS_CNTR_EQ_TO_TX_CNTR(cntr_src)))

#define STMR_IS_CNTR_SRC_WRONG_FOR_TX_TL(intr, cntr_src)             \
    (STMR_IS_INTR_TX_TL(intr) && (STMR_IS_CNTR_IN_COMB_CNTR_RANGE(cntr_src)))

#define STMR_IS_CNTR_SRC_WRONG(intr, cntr_src)                       \
    (STMR_IS_CNTR_SRC_WRONG_FOR_RX_TL(intr, cntr_src) ||             \
    STMR_IS_CNTR_SRC_WRONG_FOR_GEN_TL(intr, cntr_src))


#define STMR_GET_CNTR_SRC_VALUE_FOR_RX_GEN(cntr_src)                 \
    (stmr_ref_sel_for_rx_gen[cntr_src])

/* This macro commands a status dump of the System Time values to be
 * captured into their respective status registers. The following
 * action times are supported:
 * STMR_STATUS_DUMP_IMMEDIATE: immediate operation
 */
#define STMR_STATUS_DUMP_CMD_IMMEDIATE 0x1

typedef uint16 stmr_event_mask_type;

/*
 * This enum defines the grouping for combiners as defined for the cfn count registers
 * in the STMR Software Interface Document. 
 */
typedef enum {
  STMR_COMBINER_GROUP_COMBINERS_0_TO_3
} stmr_hw_combiner_grouping_enum_type;

/*
 * The following data structure carries the data for writing CFN to STMR
 * registers
 */
typedef struct {
  uint8 hwch;

  uint8 cfn;
}stmr_comb_cfn_info_struct_type;

/*
 * Type for handler registered by the units in L1 for the STMR
 * interrupts. This handler will be called by the STMR whenever an
 * Event occurs on the interrupt.
 */
typedef void stmr_int_isr_type
(
  stmr_event_mask_type event_mask
);

#ifdef FEATURE_WCDMA_DAL_INT_CONTROLLER
typedef struct {
  uint32 dal_intr_id;
  DalDeviceHandle* dal_device_handle;
}stmr_intr_mapping_tbl_struct_type;
#else

typedef struct{
vstmr_wcdma_event_tbl_s  *evt_tbl;
}stmr_intr_mapping_tbl_struct_type;
#endif /* FEATURE_WCDMA_DAL_INT_CONTROLLER */

  extern vstmr_wcdma_event_tbl_s stmr_evt_table[STMR_NUM_INT];

  extern vstmr_wcdma_view_s* stmr_view[STMR_MAX_NUM_COUNTERS];

  #ifdef FEATURE_DEBUG_STMR_TEST
  extern boolean debug_stmr_test_enable;
  #endif /*FEATURE_DEBUG_STMR_TEST*/

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

void stmr_default_isr( stmr_event_mask_type event_mask );

/*===========================================================================

FUNCTION STMR_INIT

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
extern void stmr_init( void );

/*===========================================================================

FUNCTION STMR_INT0_ISR

DESCRIPTION
  This function is called whenever a Event occurs on the Programmable
  Interrupt 0 (STMR0).
  This function
  - reads the Event Masks that caused the interrupt from the STMR0
    status register.
  - clears the corresponding mask by writing to the STMR0 command register.
  - calls the handler for this interrupt that is registered with the
    System Timer Driver and pass the Event Masks to the handler.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void stmr_int0_isr(vstmr_wcdma_event_tbl_s *event_tbl, void *arg, uint32 event_mask);
/*===========================================================================

FUNCTION STMR_INT1_ISR

DESCRIPTION
  This function is called whenever a Event occurs on the Programmable
  Interrupt 1 (STMR1).
  This function
  - reads the Event Masks that caused the interrupt from the STMR1
    status register.
  - clears the corresponding mask by writing to the STMR1 command register.
  - calls the handler for this interrupt that is registered with the
    System Timer Driver and pass the Event Masks to the handler.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void stmr_int1_isr(vstmr_wcdma_event_tbl_s *event_tbl, void *arg, uint32 event_mask);

/*===========================================================================

FUNCTION STMR_INT2_ISR

DESCRIPTION
  This function is called whenever a Event occurs on the Programmable
  Interrupt 2 (STMR2).
  This function
  - reads the Event Masks that caused the interrupt from the STMR2
    status register.
  - clears the corresponding mask by writing to the STMR2 command register.
  - calls the handler for this interrupt that is registered with the
    System Timer Driver and pass the Event Masks to the handler.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void stmr_int2_isr(vstmr_wcdma_event_tbl_s *event_tbl, void *arg, uint32 event_mask);

/*===========================================================================

FUNCTION STMR_INT3_ISR

DESCRIPTION
  This function is called whenever a Event occurs on the Programmable
  Interrupt 3 (STMR3).
  This function
  - reads the Event Masks that caused the interrupt from the STMR3
    status register.
  - clears the corresponding mask by writing to the STMR3 command register.
  - calls the handler for this interrupt that is registered with the
    System Timer Driver and pass the Event Masks to the handler.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void stmr_int3_isr(vstmr_wcdma_event_tbl_s *event_tbl, void *arg, uint32 event_mask);

/*===========================================================================

FUNCTION STMR_SET_ISR

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
extern void stmr_set_isr
(
 stmr_int_type      intr,
 stmr_int_isr_type* isr_ptr
);

/*===========================================================================

FUNCTION STMR_DEACT_INIT

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
extern void stmr_deact_init( void );

/*===========================================================================

FUNCTION STMR_CLEAR_INTERRUPT

DESCRIPTION
  This function clears any pending STMR interrupts.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void stmr_clear_interrupts( void );

/*===========================================================================

FUNCTION STMR_SET_EVENT_FRAME

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
  by a call to the function stmr_align_counter.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void stmr_set_event_frame
(
 stmr_int_type intr,
 uint8         offset,
 stmr_event_mask_type event_mask
);

/*===========================================================================

FUNCTION STMR_ENABLE_EVENTS

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
extern void stmr_enable_events(stmr_int_type intr, uint16 event_mask, uint8* firing_time);
/*===========================================================================

FUNCTION STMR_DISABLE_EVENTS

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
extern void stmr_disable_events
(
 stmr_int_type intr,
 stmr_event_mask_type event_mask
);

/*===========================================================================

FUNCTION STMR_GET_EVENT_NUM

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
extern uint8 stmr_get_event_num
(
  stmr_event_mask_type event_mask
);

/*===========================================================================

FUNCTION STMR_ALIGN_COUNTER

DESCRIPTION
  This function aligns the specified interrupt source to the specified
  counter source.

  counter sources for int0 and int1
  STMR_COMBINER_COUNTER0
  STMR_COMBINER_COUNTER1
  STMR_COMBINER_COUNTER2
  
  counter source for int2 only
  STMR_TX_SYSTEM_TIME_COUNTER

  counter source for int0, int1, and int2
  STMR_FRAME_REF_COUNTER

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void stmr_align_counter
(
  stmr_int_type intr,
  stmr_counter_source_enum_type cntr_src
);

/*===========================================================================

FUNCTION STMR_GET_CNTR_SRC

DESCRIPTION
  This function returns the enumerated value of the counter source that
  is currently driving the specified interrupt source.

DEPENDENCIES
  None

RETURN VALUE
  The enum value of the counter source that is currently driving the
  interrupt source.

SIDE EFFECTS
  None

===========================================================================*/
stmr_counter_source_enum_type stmr_get_cntr_src
(
  stmr_int_type intr
);

/*===========================================================================

FUNCTION STMR_GET_CURRENT_TIME

DESCRIPTION
  This function gets the current time in units of 256 chips of the
  counter source that is currently driving the specified interrupt
  source.

DEPENDENCIES
  None

RETURN VALUE
  Firing Time of Event in units of 256 chips.

SIDE EFFECTS
  None

===========================================================================*/
uint32 stmr_get_current_time
(
  stmr_int_type intr
);

/*===========================================================================

FUNCTION STMR_GET_CURRENT_TIME

DESCRIPTION
   Gets the value in chipx8 of the counter driving the interrupt specified by
   the argument intr. It comands a status dump as part of the operation.

DEPENDENCIES
  None

RETURN VALUE
  Counter value in chipx8 for the specified counter.

SIDE EFFECTS
  Does a status dump as part of its operation.
===========================================================================*/
uint32 stmr_get_current_time_chipx8
(
  stmr_int_type intr
);

/*===========================================================================

FUNCTION STMR_GET_EVENTS_AND_ENABLE_ONLY

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
extern uint16 stmr_get_events_and_enable_only
(
  stmr_int_type intr,
  uint16        enable_only_event_mask
);

extern uint16 stmr_get_event_mask
(
  stmr_int_type intr
);

/*===========================================================================

FUNCTION STMR_GET_COMBINER_CFN

DESCRIPTION
  This function reads the cfn of a combiner from the corresponding
  STMR register and returns it.

DEPENDENCIES
  None

RETURN VALUE
  CFN of the combiner

SIDE EFFECTS
  None

===========================================================================*/
extern uint8 stmr_get_combiner_cfn(stmr_counter_source_enum_type combiner);

/*===========================================================================

FUNCTION STMR_GET_COMBINER_CFN

DESCRIPTION
  This function reads the cfn of a combiner from the corresponding
  STMR register and returns it.

DEPENDENCIES
  None

RETURN VALUE
  CFN of the combiner

SIDE EFFECTS
  None

===========================================================================*/
extern int16 hw_sw_cfn_diff;

/*===========================================================================
FUNCTION STMR_GET_CNTR_TIME_CHIPX8

DESCRIPTION
  This function reads the chipx8 quantity of any STMR counter and returns it.

DEPENDENCIES
  None

RETURN VALUE
  counter value in cx8

SIDE EFFECTS
  None

===========================================================================*/
extern uint32 stmr_get_cntr_time_chipx8 (stmr_counter_source_enum_type cntr);

/*===========================================================================

FUNCTION STMR_IS_EVENT_PENDING_PROCESS

DESCRIPTION
  This function checks if the given evt has fired and is being processed

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean stmr_is_event_pending_process(stmr_int_type intr, uint8 evt);

/*===========================================================================

FUNCTION STMR_UPDATE_EVENT_PROCESSING_STATUS

DESCRIPTION
  This function updates the processed event status

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
void stmr_update_event_processing_status(stmr_int_type intr, uint8 evt);

/*===========================================================================

FUNCTION STMR_ADVANCE_COMBINER_COUNT

DESCRIPTION
  This function advances the combiner counter by one chipx8 count in the next 
  256 chip boundary.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void stmr_advance_combiner_count(stmr_counter_source_enum_type combiner);

/*===========================================================================

FUNCTION STMR_RETARD_COMBINER_COUNT

DESCRIPTION
  This function retards the combiner counter by one chipx8 count in the next 
  256 chip boundary.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void stmr_retard_combiner_count(stmr_counter_source_enum_type combiner);

/*===========================================================================
FUNCTION stmr_status_dump

DESCRIPTION
   This function takes a dump of the RAW USTMR XO count. This is the actual status dump value which can then be used
   to calculate the needed times for the specific views currently active.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void stmr_status_dump(void);

/*===========================================================================
FUNCTION STMR_COMBINER_COUNT_DYNAMIC

DESCRIPTION
  Return the dynamic value of the Combiner counts in 256 chip resolution.

DEPENDENCIES
  None

RETURN VALUE
  Dynamic value of Combiner counts in 256 chip resolution.

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 STMR_COMBINER_COUNT_DYNAMIC(uint8 counter);

/*===========================================================================
FUNCTION STMR_FRAME_REF_COUNT_DYNAMIC

DESCRIPTION
  Return the dynamic value of the Frame Ref Counter in Chipx1 resolution

DEPENDENCIES
  None

RETURN VALUE
  Dynamic value of the Frame Ref Counter in Chipx1 resolution

SIDE EFFECTS
  None
===========================================================================*/
extern uint16 STMR_FRAME_REF_COUNT_DYNAMIC(void);

/*===========================================================================
FUNCTION STMR_REF_COUNT_DYNAMIC

DESCRIPTION
  Return the dynamic value of the Frame Reference counter as a  full segmented cx8 qty.

DEPENDENCIES
  None

RETURN VALUE
  dynamic value of the Frame Reference counter as a  full segmented cx8 qty.

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 STMR_REF_COUNT_DYNAMIC(void);

/*===========================================================================
FUNCTION STMR_COMBINER_COUNT_STATUS

DESCRIPTION
  Returns the status of the combiner counter in Chipx8 resolution based on the XO dump taken

DEPENDENCIES
  None

RETURN VALUE
  Status of the combiner counter in Chipx8 resolution based on the XO dump taken

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 STMR_COMBINER_COUNT_STATUS(stmr_counter_source_enum_type counter);

/*===========================================================================
FUNCTION STMR_TX_SYS_TIME_STATUS

DESCRIPTION
  Return the status of Tx Counter in Chipx8 resolution based on the XO dump taken

DEPENDENCIES
  None

RETURN VALUE
  Status of Tx Counter in Chipx8 resolution based on the XO dump taken

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 STMR_TX_SYS_TIME_STATUS(void);


/*===========================================================================
FUNCTION STMR_TX_SYS_TIME_FULL_SEG_STATUS

DESCRIPTION
  Return the Full segment status of Tx Counter based on the XO dump taken

DEPENDENCIES
  None

RETURN VALUE
  Full segment status of Tx Counter in Chipx8 resolution based on the XO dump taken

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 STMR_TX_SYS_TIME_FULL_SEG_STATUS(void);

/*===========================================================================
FUNCTION STMR_TX_SYS_TIME_STATUS

DESCRIPTION
  Return the status of Tx TTI(0-255) based on the XO dump taken

DEPENDENCIES
  None

RETURN VALUE
  Status of Tx TTI(0-255) based on the XO dump taken

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 STMR_GET_TX_TTI_COUNT_STATUS(void);

/*===========================================================================
FUNCTION STMR_FRAME_REF_COUNT_STATUS

DESCRIPTION
  Return the status of Frame Reference Counter in Chipx8 resolution based on the XO dump taken.

DEPENDENCIES
  None

RETURN VALUE
  Status of Frame Reference Counter in Chipx8 resolution based on the XO dump taken.

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 STMR_FRAME_REF_COUNT_STATUS(void);

/*===========================================================================
FUNCTION STMR_FRAME_NUM_STATUS

DESCRIPTION
  Return the status of Frame Number(0-4095) based on the XO dump taken.

DEPENDENCIES
  None

RETURN VALUE
  Status of Frame Number based on the XO dump taken.

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 STMR_FRAME_NUM_STATUS(void);

/*===========================================================================
FUNCTION STMR_COMBINER_COUNT_DYNAMIC

DESCRIPTION
  Return the dynamic value of the Combiner counts in 256 chip resolution.

DEPENDENCIES
  None

RETURN VALUE
  Dynamic value of Combiner counts in 256 chip resolution.

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 STMR_FRAME_REF_STATUS(void);

/*===========================================================================
FUNCTION STMR_CTRL_VFR

DESCRIPTION
  Invokes the VSTMR API's to allocate / release the VFR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void stmr_ctrl_vfr(boolean enable);

/*===========================================================================
FUNCTION stmr_adjust_cntr_slew

DESCRIPTION
  Adjusts the aligned counter with the offset equal to the cell pos slew after sleep

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void stmr_adjust_cntr_slew(uint32 cell_ref_pos);

#ifdef FEATURE_DEBUG_STMR_TEST

/*PR_NOTES: New API for frc dynamic to read the frame and cx1 value*/
#define STMR_C0_DYNAMIC(conn_frame_number, frame_cx8)                                       \
do{  \
      vstmr_wstmr_t stmr_count;  \
      stmr_count = vstmr_wcdma_get_current_wstmr(stmr_view[STMR_COMBINER_COUNTER0]);  \
      conn_frame_number = stmr_count.conn_frame_no;  \
      frame_cx8 = stmr_count.subframecx8;  \
   }while(0)
/*PR_NOTES: New API for frc dynamic to read the frame and cx1 value*/

/*PR_NOTES: New API for C1 dynamic to read the frame and cx1 value*/
#define STMR_C1_DYNAMIC(conn_frame_number, frame_cx8)                                       \
do{  \
      vstmr_wstmr_t stmr_count;  \
      stmr_count = vstmr_wcdma_get_current_wstmr(stmr_view[STMR_COMBINER_COUNTER1]);  \
      conn_frame_number = stmr_count.conn_frame_no;  \
      frame_cx8 = stmr_count.subframecx8;  \
   }while(0)
/*PR_NOTES: New API for C1 dynamic to read the frame and cx1 value*/

/*PR_NOTES: New API for C2 dynamic to read the frame and cx1 value*/
#define STMR_C2_DYNAMIC(conn_frame_number, frame_cx8)                                       \
do{  \
      vstmr_wstmr_t stmr_count;  \
      stmr_count = vstmr_wcdma_get_current_wstmr(stmr_view[STMR_COMBINER_COUNTER2]);  \
      conn_frame_number = stmr_count.conn_frame_no;  \
      frame_cx8 = stmr_count.subframecx8;  \
   }while(0)
/*PR_NOTES: New C2 for frc dynamic to read the frame and cx1 value*/

/*PR_NOTES: New API for Tx dynamic to read the frame and cx1 value*/
#define STMR_TX_DYNAMIC(conn_frame_number, frame_cx8)                                       \
do{  \
      vstmr_wstmr_t stmr_count;  \
      stmr_count = vstmr_wcdma_get_current_wstmr(stmr_view[STMR_TX_SYSTEM_TIME_COUNTER]);  \
      conn_frame_number = stmr_count.conn_frame_no;  \
      frame_cx8 = stmr_count.subframecx8;  \
   }while(0)

/*PR_NOTES: New API for Tx dynamic to read the frame and cx1 value*/

  /*===========================================================================

  FUNCTION stmr_test_c0_evt_handler

  DESCRIPTION
   STMR Test event handler

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  void stmr_test_c0_evt_handler(uint16 mask);

  /*===========================================================================

  FUNCTION stmr_test_tx_evt_handler

  DESCRIPTION
   STMR Test event handler

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  void stmr_test_tx_evt_handler(uint16 mask);

  /*===========================================================================

  FUNCTION stmr_test_init

  DESCRIPTION
   Init views with specific offsets and kickstart them

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  void stmr_test_init(void);

  /*===========================================================================

  FUNCTION stmr_test_views

  DESCRIPTION
   Perform dynamic reads for different views and check if increment happens correctly.

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  extern void stmr_test_views(void);

  /*===========================================================================

  FUNCTION stmr_c0_test_evt_0

  DESCRIPTION
   Event handler for test evt 0 on Combiner C0

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  void stmr_c0_test_evt_0(void);

  /*===========================================================================

  FUNCTION stmr_c0_test_evt_1

  DESCRIPTION
   Event handler for test evt 1 on Combiner C0

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  void stmr_c0_test_evt_1(void);

  /*===========================================================================

  FUNCTION stmr_c0_test_evt_2

  DESCRIPTION
   Event handler for test evt 2 on Combiner C0

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  void stmr_c0_test_evt_2(void);

  /*===========================================================================

  FUNCTION stmr_c0_test_evt_3

  DESCRIPTION
   Event handler for test evt 3 on Combiner C0

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  void stmr_c0_test_evt_3(void);

  /*===========================================================================

  FUNCTION stmr_tx_test_evt_0

  DESCRIPTION
   Event handler for test evt 0 on Tx Timeline

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  void stmr_tx_test_evt_0(void);

  /*===========================================================================

  FUNCTION stmr_tx_test_evt_1

  DESCRIPTION
   Event handler for test evt 1 on Tx Timeline

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  void stmr_tx_test_evt_1(void);

  /*===========================================================================

  FUNCTION stmr_tx_test_evt_2

  DESCRIPTION
   Event handler for test evt 2 on Tx Timeline

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  void stmr_tx_test_evt_2(void);

  /*===========================================================================

  FUNCTION stmr_tx_test_evt_3

  DESCRIPTION
   Event handler for test evt 3 on Tx Timeline

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

  ===========================================================================*/
  void stmr_tx_test_evt_3(void);

  /*===========================================================================
  
  FUNCTION stmr_test_intrs
  
  DESCRIPTION
   Map events in event table to specific views and program them to fire periodically.
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
  
  ===========================================================================*/
  extern void stmr_test_intrs(void);

#endif /*FEATURE_DEBUG_STMR_TEST*/

#endif /* STMR_H */
