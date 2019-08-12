#ifndef HDRSRCHARD_H
#define HDRSRCHARD_H
/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            H D R   S E A R C H   A R D   S T A T E M A C H I N E

GENERAL DESCRIPTION

  This module provides routines for diversity RF control

EXTERNALIZED FUNCTIONS (Global)
  
EXTERNALIZED FUNCTIONS (Regional)

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2009 - 2014
                by Qualcomm Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/srch/inc/hdrsrchard.h#1 $
$DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when         who     what, where, why
--------     ---     ---------------------------------------------------------
08/27/13     wsh     Fixed crash when doing 2nd chain test
06/27/14     wsh     ARD/ASDiv incorrectly switching in RTAP mode 
05/25/12     wsh     Moved ARD logic into it's own state machine

==========================================================================*/

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/

#include "hdr_variation.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_HDR_FL_DATA_ADAPTIVE_RXD
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#define HDRSRCHARD_DIV_ON_DRC                          3
  /* Predicted DRC below which DIV is required regardless of FL activity */

#define HDRSRCHARD_DIV_OFF_DRC                         4
  /* Predicted DRC above which DIV is controlled by FL activity */

typedef enum
{
  HDRSRCHARD_EVENT_ENTER_TC,
  HDRSRCHARD_EVENT_EXIT_TC,
  HDRSRCHARD_EVENT_SUSPEND,
  HDRSRCHARD_EVENT_RESUME,
  HDRSRCHARD_EVENT_FL_ACTIVITY,
  HDRSRCHARD_EVENT_FL_INACT_TIMER_EXP,
  HDRSRCHARD_EVENT_DRC_CHANGED,  
  HDRSRCHARD_EVENT_SET_TAP,

  HDRSRCHARD_EVENT_RUNTIME_ENABLE,  
    /* Disable ARD during runtime (2nd chain test) */
  HDRSRCHARD_EVENT_RUNTIME_DISABLE,  
    /* Enable ARD during runtime (2nd chain test) */

  HDRSRCHARD_EVENT_ENUM_SIZE
} hdrsrchard_event_enum_type;

/*===========================================================================

                  COMMAND  FUNCTION  DECLARATIONS

===========================================================================*/

/*=========================================================================

FUNCTION HDRSRCHARD_START_STM

DESCRIPTION 
  Start ARD stm.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrsrchard_start_stm( void );

/*==========================================================================

FUNCTION  HDRSRCHARD_EVENT

DESCRIPTION
  This function reports an ARD event to the state machine. As a
  result, ARD state will be updated.
    
DEPENDENCIES
  None

INPUTS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

==========================================================================*/
void hdrsrchard_event
( 
  hdrsrchard_event_enum_type event
);

#endif /* FEATURE_HDR_FL_DATA_ADAPTIVE_RXD*/

#endif /* HDRSRCHARD_H */
