#ifndef TDSHAL_STMR_H
#define TDSHAL_STMR_H

/*===========================================================================

                    L 1   HAL TDS STMR 

DESCRIPTION
  This file contains hardware specific code to support STMR/SQE/EVENT operation.

EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000-2014 by Qualcomm Technologies, Incorporated. All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdshal_stmr.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $

when       who      what, where, why
--------   ---     ----------------------------------------------------------
06/30/13   jz       Initial release.


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "tdscdma_variation.h"
#include "comdef.h"

#ifdef FEATURE_TDSCDMA_VSTMR
#include "vstmr_tds.h"
#endif

/* max number of events on each of the interrupt sources */
#define TDSSTMR_RXTIME_INT_NUM_EVENTS 16


typedef enum {
  TDSHAL_STMR_FAILURE,
  TDSHAL_STMR_SUCCESS,
} tdshal_stmr_status;

typedef enum {
  HAL_TDS_STMR_EVENT_PERIOD_5MS,  /* EVENT PERIOD is  5ms  */
  HAL_TDS_STMR_EVENT_PERIOD_10MS,  /* EVENT PERIOD is  10ms */
} tdshal_stmr_event_period_type;



#ifdef FEATURE_TDSCDMA_VSTMR
/*
 * STMR interrupt states used in void* arg.
 * During execution of stmr interrupt ISR, current ISR state will decide which sub isr to invoke.
 * 
 */
#define TDSHAL_STMR_INT_STATE_UNINIT 0
#define TDSHAL_STMR_INT_STATE_SLAM_ISR 1
#define TDSHAL_STMR_INT_STATE_EVENTS 2

typedef void tdshal_stmr_rxtime_int_isr_type(vstmr_tds_event_tbl_s *event_tbl, void *arg, uint32 pending);

#else
typedef void tdshal_stmr_rxtime_int_isr_type(void);
#endif

#define TDSHAL_STMR_CX8_PER_SF 51200
#define TDSHAL_STMR_CX1_PER_SF 6400

/*===========================================================================

FUNCTION TDSHAL_STMR_INIT

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
uint32 tdshal_stmr_init(boolean reset);

/*===========================================================================

FUNCTION TDSHAL_STMR_INIT

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
uint32 tdshal_stmr_reinit(void);


/*===========================================================================

FUNCTION TDSHAL_STMR_INIT_IRAT

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
uint32 tdshal_stmr_init_irat(void);

/*===========================================================================

FUNCTION TDSHAL_STMR_INT_STATUS

DESCRIPTION
  This function returns a bitmask of list of events fired in current stmr Interrupts.

DEPENDENCIES
  None

RETURN VALUE
  uint32 - bitmask of events fired.
  
SIDE EFFECTS
  None
===========================================================================*/
uint32 tdshal_stmr_int_status(void);

/*===========================================================================

FUNCTION TDSHAL_STMR_GET_RXTIME

DESCRIPTION
  This function returns RXTIME subframe number+rxtime

DEPENDENCIES
  None

RETURN VALUE
  uint32 - Subframe number+rxtime
  
SIDE EFFECTS
  None
===========================================================================*/
uint32 tdshal_stmr_get_rxtime(void);


/*===========================================================================

FUNCTION TDSHAL_STMR_GET_RXTIME_SFN

DESCRIPTION
  This function returns RXTIME subframe number

DEPENDENCIES
  None

RETURN VALUE
  uint32 - Rxtime Subframe number
  
SIDE EFFECTS
  None
===========================================================================*/
uint16 tdshal_stmr_get_rxtime_sfn(void);


/*===========================================================================

FUNCTION TDSHAL_STMR_GET_RXTIME_CX8

DESCRIPTION
  This function returns RXTIME ChipX8 count

DEPENDENCIES
  None

RETURN VALUE
  uint32 - Rxtime  Chipx8 count
  
SIDE EFFECTS
  None
===========================================================================*/
uint16 tdshal_stmr_get_rxtime_cx8(void);


/*===========================================================================

FUNCTION TDSHAL_STMR_GET_WALLTIME

DESCRIPTION
  This function returns WALLTIME subframe number+rxtime

DEPENDENCIES
  None

RETURN VALUE
  uint32 - Subframe number+rxtime
  
SIDE EFFECTS
  None
===========================================================================*/
uint32 tdshal_stmr_get_walltime(void);


/*===========================================================================

FUNCTION TDSHAL_STMR_GET_WALLTIME_SFN

DESCRIPTION
  This function returns WALLTIME subframe number

DEPENDENCIES
  None

RETURN VALUE
  uint32 - Rxtime Subframe number
  
SIDE EFFECTS
  None
===========================================================================*/
uint16 tdshal_stmr_get_walltime_sfn(void);


/*===========================================================================

FUNCTION TDSHAL_STMR_GET_WALLTIME_CX8

DESCRIPTION
  This function returns WALLTIME ChipX8 count

DEPENDENCIES
  None

RETURN VALUE
  uint32 - Rxtime  Chipx8 count
  
SIDE EFFECTS
  None
===========================================================================*/
uint16 tdshal_stmr_get_walltime_cx8(void);


/*===========================================================================

FUNCTION TDSHAL_STMR_DUMP

DESCRIPTION
  This function returns RXTIME subframe number + Walltime

DEPENDENCIES
  None

RETURN VALUE
  uint32 - Rxtime Subframe number tdshal_stmr_dump_type
  
SIDE EFFECTS
  None
===========================================================================*/
void tdshal_stmr_dump(void);

/*===========================================================================

FUNCTION TDSHAL_STMR_DUMP_WALLTIME_CX8

DESCRIPTION
  This function returns walltime chipx8 count

DEPENDENCIES
  None

RETURN VALUE
  uint16 - walltime chipx8 count
  
SIDE EFFECTS
  None
===========================================================================*/
uint16 tdshal_stmr_dump_walltime_cx8 ( void );

/*===========================================================================

FUNCTION TDSHAL_STMR_DUMP_WALLTIME_SFN

DESCRIPTION
  This function returns walltime chipx8 count

DEPENDENCIES
  None

RETURN VALUE
  uint16 - walltime chipx8 count
  
SIDE EFFECTS
  None
===========================================================================*/
uint16 tdshal_stmr_dump_walltime_sfn( void );

/*===========================================================================

FUNCTION TDSHAL_STMR_DUMP_WALLTIME

DESCRIPTION
  This function returns walltime count

DEPENDENCIES
  None

RETURN VALUE
  uint32 - walltime count
  
SIDE EFFECTS
  None
===========================================================================*/
uint32 tdshal_stmr_dump_walltime( void );

/*===========================================================================

FUNCTION TDSHAL_STMR_DUMP_RXTIME

DESCRIPTION
  This function returns RXTIME chipx8 time

DEPENDENCIES
  None

RETURN VALUE
  uint16 - chipx8
  
SIDE EFFECTS
  None
===========================================================================*/
uint16 tdshal_stmr_dump_rxtime_cx8(void);


/*===========================================================================

FUNCTION TDSHAL_STMR_DUMP_RXTIME_SFN

DESCRIPTION
  This function returns RXTIME subframe number 

DEPENDENCIES
  None

RETURN VALUE
  uint16 - Rxtime Subframe number 
  
SIDE EFFECTS
  None
===========================================================================*/
uint16 tdshal_stmr_dump_rxtime_sfn(void);


/*===========================================================================

FUNCTION TDSHAL_STMR_DUMP_RXTIME

DESCRIPTION
  This function returns RXTIME subframe number + Walltime

DEPENDENCIES
  None

RETURN VALUE
  uint32 - Rxtime Subframe number and Chipx8
  
SIDE EFFECTS
  None
===========================================================================*/
uint32 tdshal_stmr_dump_rxtime(void);


/*===========================================================================
  
  FUNCTION TDSHAL_STMR_DUMP_USTMR
  
  DESCRIPTION
    This function dump universal stmr one time dump
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    uint32 status
    
  SIDE EFFECTS
    None
  ===========================================================================*/
uint32 tdshal_stmr_dump_ustmr(void);


/*===========================================================================
  
  FUNCTION TDSHAL_STMR_DUMP_PHASE
  
  DESCRIPTION
    This function dump universal stmr one time dump
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    uint32 status
    
  SIDE EFFECTS
    None
  ===========================================================================*/
uint32 tdshal_stmr_dump_phase(void);


/*===========================================================================

FUNCTION TDSHAL_STMR_INT_SET

DESCRIPTION
  This function set event interrupt mask.

DEPENDENCIES
  None

RETURN VALUE
  uint32 - status
  
SIDE EFFECTS
  None
===========================================================================*/
void tdshal_stmr_int_set(uint32 event_mask);

/*===========================================================================

FUNCTION TDSHAL_STMR_EVENT_ON

DESCRIPTION
  This function turn on a specific event

DEPENDENCIES
  None

RETURN VALUE
  uint32 status
  
SIDE EFFECTS
  None
===========================================================================*/
uint32 tdshal_stmr_event_on(uint32 event_num);

/*===========================================================================

FUNCTION TDSHAL_STMR_EVENT_OFF

DESCRIPTION
  This function turn off a specific event

DEPENDENCIES
  None

RETURN VALUE
  uint32 status
  
SIDE EFFECTS
  None
===========================================================================*/
uint32 tdshal_stmr_event_off(uint32 event_num);


/*===========================================================================

FUNCTION TDSHAL_STMR_EVENT_SET_PERIOD

DESCRIPTION
  This function turn off a specific event

DEPENDENCIES
  None

RETURN VALUE
  uint32 status
  
SIDE EFFECTS
  None
===========================================================================*/
uint32 tdshal_stmr_event_set_period(uint32 event_num, tdshal_stmr_event_period_type event_period);

/*===========================================================================

FUNCTION TDSHAL_STMR_EVENT_GET_PERIOD

DESCRIPTION
  This function returns specific event period

DEPENDENCIES
  None

RETURN VALUE
  uint32 status
  
SIDE EFFECTS
  None
===========================================================================*/
uint32 tdshal_stmr_event_get_period(uint32 event_num);

/*===========================================================================

FUNCTION TDSHAL_STMR_EVENT_SET_SUBFRAME

DESCRIPTION
  This function turn off a specific event

DEPENDENCIES
  None

RETURN VALUE
  uint32 status
  
SIDE EFFECTS
  None
===========================================================================*/
uint32 tdshal_stmr_event_set_subframe(uint32 event_num, uint32 subframe);

/*===========================================================================

FUNCTION TDSHAL_STMR_EVENT_SET_FIRING_TIME

DESCRIPTION
  This function turn off a specific event

DEPENDENCIES
  None

RETURN VALUE
  uint32 status
  
SIDE EFFECTS
  None
===========================================================================*/
uint32 tdshal_stmr_event_set_firing_time(uint32 event_num, uint32 firing_time);

/*===========================================================================

FUNCTION TDSHAL_STMR_ST_SET_TIME

DESCRIPTION
  This function turn off a specific event

DEPENDENCIES
  None

RETURN VALUE
  uint32 status
  
SIDE EFFECTS
  None
===========================================================================*/
void tdshal_stmr_st_now(uint32 sfn, uint32 cx8);

/*===========================================================================

FUNCTION TDSHAL_STMR_ST_EVENT_UPDATE

DESCRIPTION
  This function perform slaming based on a slam event 

DEPENDENCIES
  None

RETURN VALUE
  uint32 status
  
SIDE EFFECTS
  None
===========================================================================*/
uint32 tdshal_stmr_st_event_update(uint32 slam_value, uint32 event_time);


/*===========================================================================
  
  FUNCTION TDSHAL_STMR_ST_ONLINE_UPDATE
  
  DESCRIPTION
    This function perform slaming based on modem online start 
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    uint32 status
    
  SIDE EFFECTS
    None
  ===========================================================================*/
uint32 tdshal_stmr_st_online_update(uint32 sfn, uint32 cx8);


/*===========================================================================
  
  FUNCTION TDSHAL_STMR_VFR_REQUEST
  
  DESCRIPTION
    This function request ownership of VFR for TDS operation
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    uint32 status
    
  SIDE EFFECTS
    None
  ===========================================================================*/

uint32 tdshal_stmr_vfr_request(void);

/*===========================================================================

FUNCTION TDSHAL_STMR_INSTALL_ISR

DESCRIPTION
  This function is called to setup ISR to handle STMR EVENTs

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void tdshal_stmr_install_isr(tdshal_stmr_rxtime_int_isr_type* isr_ptr );


/*===========================================================================

FUNCTION TDSHAL_STMR_UNINSTALL_ISRS

DESCRIPTION
  This function uninstalls/deregisters STMR interrupt handlers with the tramp services.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tdshal_stmr_uninstall_isr( void );

/*===========================================================================

  FUNCTION TDSHAL_STMR_VFR_RELEASE
  
  DESCRIPTION
    This function release ownership of VFR for TDS operation
  
  DEPENDENCIES
    None
  
  RETURN VALUE
    uint32 status
    
  SIDE EFFECTS
    None
  ===========================================================================*/

uint32 tdshal_stmr_vfr_release(void);
#endif /* TDSHAL_STMR_H */
