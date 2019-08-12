#ifndef M1X_STMR_I_H
#define M1X_STMR_I_H
/*===========================================================================

          CDMA  VSTMR  DRIVER  INTERFACE  HEADER

DESCRIPTION
  This header file provides the interfaces to the driver m1x_stmr in 1x in
  order to interact with the VSTMR module.
  Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc.
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

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/21/16   srk     1x2GTA cleanup and TX power shoot fixes.
02/12/15   jh      Added enum M1X_STMR_CLIENT_SW_DEM
03/13/14   cjb     New API that returns entire scomb value read from vstmr
03/11/14   ssh     BO CGPS changes for TTR.
01/27/14   pap     Added API to re-register all the active m1x_stmr events.
01/14/14   pap     Modify 1x for chipx8 compensation due to updated MCS API.
12/10/13   pap     Added API to provide LTE and 1x synchronized time.
10/17/13   pap     Adding changes for VFR allocations.
09/30/13   pap     Updating the design of the module to have more manageable
                   functions.
09/09/13   pap     Added functions to get various current time parameters.
07/05/13   pap     Initial version of file.

===========================================================================*/

#include "modem_1x_defs.h"
#include "comdef.h"
#include "vstmr.h"
#include "vstmr_1x.h"


/* Max vstmr RTC value in cx8 */
#define MAX_VSTMR_RTC_CX8   ( 1 << 27 )

/* Type of events available to schedule using m1x_stmr module*/
typedef enum
{
  M1X_STMR_PCG_0 = 0,
  M1X_STMR_PCG_1,
  M1X_STMR_PCG_2,
  M1X_STMR_PCG_3,
  M1X_STMR_PCG_4,
  M1X_STMR_PCG_5,
  M1X_STMR_PCG_6,
  M1X_STMR_PCG_7,
  M1X_STMR_PCG_8,
  M1X_STMR_PCG_9,
  M1X_STMR_PCG_10,
  M1X_STMR_PCG_11,
  M1X_STMR_PCG_12,
  M1X_STMR_PCG_13,
  M1X_STMR_PCG_14,
  M1X_STMR_PCG_15,
  M1X_STMR_PNROLL,
  M1X_STMR_EVENT_MAX
} m1x_stmr_event_type;

/* This enum is used to find which client is trying to setup the event. */
typedef enum
{
  M1X_STMR_CLIENT_SRCH = 0,
  M1X_STMR_CLIENT_RX,
  M1X_STMR_CLIENT_TX,
  M1X_STMR_CLIENT_SW_DEM,
  M1X_STMR_CLIENT_MAX
} m1x_stmr_client_type;

/* Contains all possible error codes returned from the module*/
typedef enum
{
  M1X_STMR_SUCCESS         =  0,
  M1X_STMR_ERR_NO_SLOTS    = -1,
  M1X_STMR_ERR_BAD_PARAM   = -2,
  M1X_STMR_ERR_REG_DUP     = -3,
  M1X_STMR_ERR_UNREG_EVENT = -4,
  M1X_STMR_ERR_BAD_DRV_OP  = -5,
  M1X_STMR_ERR_VFR_ALLOC   = -6
} m1x_stmr_err_type;

typedef struct
{
  uint32 l1x_cx32;
  uint32 lte_cx25;
  uint32 xo_ticks;
} m1x_stmr_timer_dumps;

/* 1x RTC (cx8) and Phase (sub-cx8) type */
typedef union
{
  uint64 rtc_full;  /* CDMA RTC in full resolution. */
  struct
  {
    uint32 rtc_phase;   /* CDMA Phase in sub-cx8 units. */
    uint32 rtc_cx8; /* CDMA RTC in cx8 units. */
  };
} m1x_stmr_rtc_type;

/*===========================================================================

FUNCTION M1X_STMR_INIT

DESCRIPTION
  This function initializes the view provided by the VSTMR.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void m1x_stmr_init( void );

/*===========================================================================

FUNCTION M1X_STMR_DEINIT

DESCRIPTION
  This function will release all the event tables associated with the view
  and determine if the call was made before releasing all the events from the
  event table associated with the view.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void m1x_stmr_deinit( void );

/*=============================================================================

FUNCTION M1X_STMR_REGISTER

DESCRIPTION
  This function will start an event in the VSTMR.It will register the event
  and will allocate the event table in case it hasnt already been done.

DEPENDENCIES
  None.

RETURN VALUE
  M1X_STMR_SUCCESS        - Started and registered event and callback function.
  M1X_STMR_ERR_NO_SLOTS   - The registration was not successful due to all
                            periodic slots being full
  M1X_STMR_ERR_BAD_PARAM  - The registration was not successful because of
                            incorrect parameters being passed
  M1X_STMR_ERR_REG_DUP    - The event is already registered for the client,
                            should not register such an event.
  M1X_STMR_ERR_BAD_DRV_OP - Incorrect driver operation, debug driver

SIDE EFFECTS
  None.
=============================================================================*/
m1x_stmr_err_type m1x_stmr_register
(
  m1x_stmr_client_type client,                              /* Caller identity  */
  m1x_stmr_event_type  events,                              /* Event to trigger */
  void                 (*event_callbk)(m1x_stmr_event_type) /* Callback function*/
);

/*===========================================================================

FUNCTION M1X_STMR_DEREGISTER

DESCRIPTION
  This function will stop the event that had been scheduled using
  m1x_stmr_register and if successful return 0 otherwise an error code.

DEPENDENCIES
  None.

RETURN VALUE
  M1X_STMR_SUCCESS         - Successfully stopped the event.
  M1X_STMR_ERR_UNREG_EVENT - The event had not been registered by the client.
  M1X_STMR_ERR_BAD_PARAM   - Incorrect parameters passed to the function.
  M1X_STMR_ERR_BAD_DRV_OP  - Incorrect driver operation, debug driver

SIDE EFFECTS
  None.
===========================================================================*/
m1x_stmr_err_type m1x_stmr_deregister
(
  m1x_stmr_client_type client,  /*calling client*/
  m1x_stmr_event_type  events   /*event ID to be cancelled*/
);

/*===========================================================================

FUNCTION M1X_STMR_SET_FRAME_OFFSET

DESCRIPTION
  This function adjusts the PCG offset which will reschdule PCG events .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void m1x_stmr_set_frame_offset
(
  uint16 offset                    /*offset for PCG interrupts*/
);

/*===========================================================================

FUNCTION M1X_STMR_GET_FRAME_OFFSET

DESCRIPTION
  This function returns the PCG offset which affects on what actual PCG on
  the VSTMR are supposed to fire.

DEPENDENCIES
  None.

RETURN VALUE
  The current offset value being used for the PCG events to be offset.

SIDE EFFECTS
  None.
===========================================================================*/
uint16 m1x_stmr_get_frame_offset( void );

/*===========================================================================

FUNCTION M1X_STMR_DEREGISTER_CLIENT

DESCRIPTION
  This function deregisters all the events from the client passed as parameter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void m1x_stmr_deregister_client
(
  m1x_stmr_client_type client  /* Caller identity*/
);

/*===========================================================================

FUNCTION      M1X_STMR_GET_SCOMB_PHASE

DESCRIPTION
  This function returns the phase of the SYNC_80 frame.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the current phase within the Sync_80 frame.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 m1x_stmr_get_scomb_phase( void );

/*===========================================================================

FUNCTION      M1X_STMR_GET_SCOMB_CHIP_COUNT

DESCRIPTION
  This function returns the number of chips from the last SYNC_80 boundary.

DEPENDENCIES
  None.

RETURN VALUE
  The current number of chips elapsed since the last SYNC_80 boundary.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 m1x_stmr_get_scomb_chip_count( void );

/*===========================================================================

FUNCTION      M1X_STMR_GET_SCOMB_CHIPX8_COUNT

DESCRIPTION
  This function returns current number of chips x8 elapsed since the last
  SYNC_80 boundary.

DEPENDENCIES
  None.

RETURN VALUE
  The current number of chips x8 elapsed since the last SYNC_80 boundary.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 m1x_stmr_get_scomb_chipx8_count( void );

/*===========================================================================

FUNCTION      M1X_STMR_GET_SCOMB

DESCRIPTION
  This function returns the current symbol combiner time.

DEPENDENCIES
  None.

RETURN VALUE
  The current scomb time.

SIDE EFFECTS
  None.
===========================================================================*/
vstmr_1xscomb_t m1x_stmr_get_scomb( void );

/*===========================================================================

FUNCTION      M1X_STMR_GET_RTC_COUNT_CX8

DESCRIPTION
  This function returns current RTC value in chip x8.

DEPENDENCIES
  None.

RETURN VALUE
  The current RTC value.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 m1x_stmr_get_rtc_count_cx8( void );

/*===========================================================================

FUNCTION      M1X_STMR_GET_FRAME_AND_PCG_NUM_WITH_FRAME_OFFSET

DESCRIPTION
  This function populates the current PCG number and current frame number to
  the parameters passed. The values are Frame Offset adjusted.
  The range of the frame number is 0-511(10.24sec/20ms). The
  range of the PCG number is 0-15.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void m1x_stmr_get_frame_and_pcg_num_with_frame_offset
(
uint32 *frame_num,
uint32 *pcg_num
);

/*===========================================================================

FUNCTION      M1X_STMR_ALLOCATE_VFR

DESCRIPTION
  This function will request for an allocation of the primary VFR to 1x.

DEPENDENCIES
  None.

RETURN VALUE
  M1X_STMR_SUCCESS        - The VFR was successfully allocated.
  M1X_STMR_ERR_VFR_ALLOC  - The VFR has already been allocated.

SIDE EFFECTS
  None.
===========================================================================*/
m1x_stmr_err_type m1x_stmr_allocate_vfr
(
vstmr_rtc_id vstmr_id
);

/*===========================================================================

FUNCTION      M1X_STMR_RELEASE_VFR

DESCRIPTION
  This function will release the primary VFR that has been allocated to 1x.

DEPENDENCIES
  None.

RETURN VALUE
  M1X_STMR_SUCCESS        - The VFR was successfully released.
  M1X_STMR_ERR_VFR_ALLOC  - The VFR has not been requested hence cannot release.

SIDE EFFECTS
  None.
===========================================================================*/
m1x_stmr_err_type m1x_stmr_release_vfr( void );

/*===========================================================================

FUNCTION      M1X_STMR_GET_LTE_1X_TIME

DESCRIPTION
  This function returns the LTE and 1x time based on the same XO count.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void m1x_stmr_get_lte_1x_time( m1x_stmr_timer_dumps *ret_time );

/*===========================================================================

FUNCTION      M1X_STMR_GET_SUB_ID

DESCRIPTION
  This function provides VSTMR subscription ID currently used by 1x.

DEPENDENCIES
  None.

RETURN VALUE
  1x subscription ID: VSTMR_RTC_1X_SUB0 .

SIDE EFFECTS
  None.
===========================================================================*/
vstmr_rtc_id m1x_stmr_get_sub_id( void );

/*===========================================================================

FUNCTION      M1X_STMR_RE_REG_EVENTS

DESCRIPTION
  This function re-registers all currently active events in order to update
  them to current view timelines for their next trigger time.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void m1x_stmr_re_reg_events( void );

/*===========================================================================

FUNCTION      M1X_STMR_GET_USTMR_1X_TIME

DESCRIPTION
  This function provides full 1x time (RTC + Phase) based on the
  input XO count.

DEPENDENCIES
  None.

RETURN VALUE
  1X Time (RTC + Phase).

SIDE EFFECTS
  None.
===========================================================================*/
m1x_stmr_rtc_type m1x_stmr_get_ustmr_1x_time( uint32 ustmr );

/*===========================================================================

FUNCTION      M1X_STMR_GET_FCW

DESCRIPTION
  This function returns current FCW value in uint32.

DEPENDENCIES
  None.

RETURN VALUE
  FCW value in Q30 format where 2 MSBs represent the decimal value and the
  remaining bits represent the CDMA chip rate cx8/XO freq.

  For example, with 19.2 MHz XO frequency, the value be calculated as below.

  CDMA Cx8: (1.2288 MHz * 8) / 19.2e6 = 0.512 * 2^30 = 0x20c49ba6 (rounded)

SIDE EFFECTS
  None.

===========================================================================*/
uint32 m1x_stmr_get_fcw( void );

/*===========================================================================

FUNCTION      M1X_STMR_GET_LAST_SYNC80_RTC

DESCRIPTION
  This function returns the RTC value at the last SYNC80 boundary

DEPENDENCIES
  None.

RETURN VALUE
  RTC at previous sync80

SIDE EFFECTS
  None.
===========================================================================*/
uint32 m1x_stmr_get_last_sync80_rtc( void );
#endif   /*M1X_STMR_I_H*/
