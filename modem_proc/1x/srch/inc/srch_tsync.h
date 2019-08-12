#ifndef SRCH_TSYNC_H
#define SRCH_TSYNC_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            S E A R C H E R    T S Y N C    C O N T R O L

                         H E A D E R   F I L E


GENERAL DESCRIPTION
   This file contains exports from srch_tsync.c.

  Copyright (c) 2011 - 2013 Qualcomm Technologies, Inc. 
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


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*===*/

/*=========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/srch/inc/srch_tsync.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/23/13   trc     Decouple TSYNC and its QMI interface
12/16/11   mca     Large TSYNC update, added QMI support
11/04/11   mca     Initial revision

==========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "1x_variation.h"
#include "comdef.h"
#include "customer.h"
#include "modem_1x_defs.h"

#ifdef FEATURE_MODEM_1X_TSYNC
/* Srch */
#include "srch_genlog.h"
#include "srch_genlog_i.h"

/* DAL */
#include "DalDevice.h"


/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/



/*--------------------------------------------------------------------------
                          General Global Variables
--------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------
                           Types and Declarations
---------------------------------------------------------------------------*/
typedef enum
{
  TSYNC_STATE_OUT_OF_SERVICE = 0x0,
  TSYNC_STATE_ENABLED,
  TSYNC_STATE_DISABLED
} tsync_state_type;

/* Common tsync data structure */
typedef struct
{
  /* Logging buffer */
  srch_genlog_tsync_sub_v1 tsync_sub;

  /* Status of TSYNC */
  boolean enabled;

  /* Tracks the first pulse */
  boolean first_pulse;

  /* Necessary correction */
  int32 correction;
  int32 prev_correction;

  /* 1x system time in qword format */
  qword system_time;
  qword prev_system_time;

  /* Time since last update in ms */
  uint32 delta;

  /* Offset to XO from rotators */
  int32 ppm_offset;

  /* CDMA RTC value latched at the last Sync80 tick */
  uint32 cdma_rtc;
  uint32 cdma_rtc_uncorrected;

  /* CDMA RTC value latched with the XO RTC */
  uint32 cdma_rtc_at_xo;

  /* XO RTC from the Univ Stmr dump */
  uint32 xo_rtc;

  /* CDMA RTC PHASE value latched at the last sync80 tick */
  uint32 cdma_rtc_phase;
  uint32 cdma_rtc_phase_uncorrected;

  /* CDMA RTC value when the next pulse should occur */
  uint32 target_rtc;

  /* When the next pulse is scheduled from HW (XO ticks) */
  uint32 next_pulse_from_xo;

  /* When the next pulse should occur */
  uint32 next_pulse;

  /* Difference in Cx8k between STMR & VSRC - read from FW in Cx8K */
  uint32 stmrVsrcDiff;

  /* FCW value at the last sync80 tick */
  uint32 fcw;

  /* The period programmed to hw */
  int32 period;
  int32 prev_period;

  /* Current 1x Ref PN offset */
  uint16 ref_pn;

  /* Current 1x serving frequency band associated to Ref PN */
  uint8 band;

  /* Current 1x serving frequency channel associated to Ref PN */
  uint16 channel;

  /* Rx AGC value of receive chain at the last tune to home band/channel */
  uint8 rx_agc;

  /* System identifier */
  word sid;

  /* Network identifier */
  word nid;

  /* Leap seconds since the start of system time */
  byte leap_seconds;

  /* Local time offset */
  byte local_offset;

  /* Indicates if daylight saving time is enabled */
  boolean daylight_saving;

  /* Julian time */
  time_julian_type julian_time;

  /* ms for julian time */
  uint16 ms;

  /* Current state of tsync */
  tsync_state_type state;

  /* True if a state change just occurred */
  boolean state_change;

  /* Holds the DAL information to control the GPIO */
  DalDeviceHandle *h_tlmm;

} tsync_type;


/*===========================================================================

                      FUNCTION DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH_TSYNC_INIT

DESCRIPTION    This function initializes the TSYNC system

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_tsync_init( void );


/*===========================================================================

FUNCTION       SRCH_TSYNC_DEINIT

DESCRIPTION    This function shuts down the TSYNC system

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_tsync_deinit( void );


/*===========================================================================

FUNCTION       SRCH_TSYNC_ENABLE

DESCRIPTION    This function enables the TSYNC system

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_tsync_enable( void );


/*===========================================================================

FUNCTION       SRCH_TSYNC_DISABLE

DESCRIPTION    This function disables the TSYNC system

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_tsync_disable( tsync_state_type state );


/*===========================================================================

FUNCTION       SRCH_TSYNC_IS_ENABLED

DESCRIPTION    Checks if the TSYNC system is enabled

DEPENDENCIES   srch_tsync_init must have been called.

RETURN VALUE   TRUE if TSYNC is enabled, otherwise FALSE

SIDE EFFECTS   None

===========================================================================*/
extern boolean srch_tsync_is_enabled( void );


/*===========================================================================

FUNCTION       SRCH_TSYNC_GET_UPDATE_PERIOD

DESCRIPTION    Gets the update period if TSYNC is enabled

DEPENDENCIES   None

RETURN VALUE   The update period in ms if TSYNC is enabled, otherwise 0.

SIDE EFFECTS   None

===========================================================================*/
extern uint16 srch_tsync_get_update_period( void );


/*===========================================================================

FUNCTION       SRCH_TSYNC_IS_FIRST_PULSE

DESCRIPTION    Checks if the upcoming TSYNC pulse will be the first since
               being enabled

DEPENDENCIES   None.

RETURN VALUE   TRUE if TSYNC is enabled and the first pulse has not yet
               occurred, otherwise FALSE

SIDE EFFECTS   None

===========================================================================*/
extern boolean srch_tsync_is_first_pulse( void );


/*===========================================================================

FUNCTION       SRCH_TSYNC_GET_SYS_TIME

DESCRIPTION    Checks if the TSYNC system is enabled

DEPENDENCIES   None

RETURN VALUE   TRUE if TSYNC is enabled, otherwise FALSE

SIDE EFFECTS   None

===========================================================================*/
extern void srch_tsync_get_sys_time( void );


/*===========================================================================

FUNCTION       SRCH_TSYNC_SETUP_PULSE

DESCRIPTION    This function sets up the next TSYNC pulse if the system is
               enabled.

DEPENDENCIES   srch_tsync_enable must be called if a pulse needs to be
               generated.  Otherwise, none.

RETURN VALUE   TRUE if a pulse will be generated. Otherwise, FALSE.

SIDE EFFECTS   The tsync data structure is updated.

===========================================================================*/
extern boolean srch_tsync_setup_pulse( void );


/*===========================================================================

FUNCTION       SRCH_TSYNC_SETUP_PULSE_IF_NECESSARY

DESCRIPTION    This function sets up the next TSYNC pulse if the system if
               it is the correct time

DEPENDENCIES   srch_tsync_enable must be called if a pulse needs to be
               generated.  Otherwise, none.

RETURN VALUE   TRUE if a pulse will be generated. Otherwise, FALSE.

SIDE EFFECTS   The tsync data structure is updated.

===========================================================================*/
extern boolean srch_tsync_setup_pulse_if_necessary( void );


/*===========================================================================

FUNCTION       SRCH_TSYNC_BUILD_TSYNC_SUB

DESCRIPTION    This function builds the TSYNC sub packet and should only be
               called by genlog.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_tsync_build_tsync_sub
(
  srch_genlog_packet_id_type id,                 /* id of packet           */
  sub_commit_func_type       commit_fp           /* function to call to
                                                    commit the subpacket   */
);

#endif /* FEATURE_MODEM_1X_TSYNC */
#endif /* SRCH_TSYNC_H */
