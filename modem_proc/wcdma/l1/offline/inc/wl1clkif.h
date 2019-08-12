#ifndef WL1CLKIF_H
#define WL1CLKIF_H

/*===========================================================================
                WL1 CLK Driver interface hrader file

DESCRIPTION
  This file is CLK Driver interface header file that supports multimode
  external interface while maintaning backward compatibility. This file
  contains all declarations and definitions necessary for WL1 to use the
  clock drivers. WL1 modules call inline function in this and this takes care of
  all CLK API changes featurization. FEATURE_CMI is used to maintain backward
  compatibility with older definitions of clk api's.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None. All are passive inline function calling CLK modules based on feature.
  All initialization requirment of respective CLK modules should be followed

Copyright (c) 2009 - 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1clkif.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/29/14   km      Removed FEATURE_WCDMA_DIME_SW featurization
08/14/12   zr      Revising declarations to allow file to be included multiple times
04/06/12   zr      Removed reference to deprecated time_cdma.h
03/28/12   zr      Added feature definitions for Dime
01/30/12   vs      Feature cleanup.
01/24/11   stk     Replaced deprecated clk_uptime with time_get_uptime_secs for nikel
05/05/09   ms      Fixed compiler warnings
09/19/09   rmsd    Made interface CMI related changes
08/25/09   rmsd    Created intial version of the file
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "wcdma_variation.h"
#ifdef FEATURE_CMI
#include "time_svc.h"
#include "time_tod.h"  
#else
#include "ts.h"
#include "clkrtc.h"
#endif

#include "time_svc.h"

/*===========================================================================

FUNCTION wl1_time_get_uptime_secs

DESCRIPTION
  Return the number of seconds the phone has been on.

DEPENDENCIES
  None

RETURN VALUE
  Number of seconds since the phone powered on. The value is not rounded.

SIDE EFFECTS
  None

===========================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
uint32 wl1_time_get_uptime_secs(void)
{
   return time_get_uptime_secs();
}

/*=============================================================================

FUNCTION wl1_time_get

DESCRIPTION
  Returns the current time

DEPENDENCIES
  A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.

RETURN VALUE
  void.

SIDE EFFECTS
  None

=============================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_time_get(qword time)
{
 #ifdef FEATURE_CMI
   (void)time_get(time);
 #else
   (void)ts_get(time);
 #endif
}

/*=============================================================================

FUNCTION wl1_time_get_uptime_ms

DESCRIPTION
  Get time the phone has been powered on for
  Number of milliseconds phone has been powered on for returned to 'uptime_ms'
DEPENDENCIES
  A valid CDMA time, or a valid HDR time, or a valid Time-of-Day, etc.

RETURN VALUE
   VOID 

SIDE EFFECTS
  None

=============================================================================*/
#ifdef __GNUC__
static inline 
#else 
INLINE 
#endif
void wl1_time_get_uptime_ms(qword uptime_ms)
{
   (void)time_get_uptime_ms(uptime_ms);
}

#endif
