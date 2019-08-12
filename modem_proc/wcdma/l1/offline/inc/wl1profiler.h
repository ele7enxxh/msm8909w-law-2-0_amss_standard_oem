#ifndef WL1TRACER_EVENTS_H
#define WL1TRACER_EVENTS_H
/*===========================================================================
                    WCDMA L1 PROFILER MODULE

GENERAL DESCRIPTION
  This file contains the code for controlling the operation of WCDMA L1 SLEEP

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2013 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/drx.h_v   1.16   11 Jul 2002 21:26:56   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1profiler.h#1 $ 
$DateTime: 2016/12/13 08:00:12 $ 
$Author: mplcsds1 $ 

 
 when       who    what, where, why 
------------------------------------------------------------------------------------- 
05/13/14    skk     Checked in initial version of file. 
 
===========================================================================*/



/* ==========================================================================
**
** Header Includes
**
** ========================================================================*/ 
#include "tracer.h"
#include "wcdma_tracer_event_ids.h"

#include "wl1api.h"

/*===========================================================================
FUNCTION     DRX_PWR_STROBE_INIT

DESCRIPTION
  Configures a gpio for use as a power strobe..

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean drx_pwr_strobe_init(void);

/*===========================================================================
FUNCTION     DRX_PWR_STROBE_CNTR_RESET

DESCRIPTION
  Reset the power strobe cntr in a drx cycle

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void drx_pwr_strobe_cntr_reset(void);

/*===========================================================================
FUNCTION                   DRX_PWR_STROBE_TOGGLE

DESCRIPTION
  Toggle the gpio used as a power strobe. Does nothing if 
  drx_pwr_strobe.enabled is false.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void drx_pwr_strobe_toggle(wl1_prof_name_type sender);

/*===========================================================================

FUNCTION                  DRX_SET_PWR_STROBE

DESCRIPTION
  This function sets the internal DRX power strobe state 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  drx_pwr_strobe_active is updated to the value of the parameter, which 
  will enable GPIO strobe profiling if TRUE.
===========================================================================*/
extern void drx_set_pwr_strobe(boolean enabled);

/*===========================================================================
FUNCTION     DRX_PWR_STROBE_INIT_ALT_GPIO

DESCRIPTION
 
 
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean drx_pwr_strobe_init_alt_gpio(void);

/*===========================================================================
FUNCTION     DRX_PWR_STROBE_TOGGLE_ALT_GPIO

DESCRIPTION
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
extern void drx_pwr_strobe_toggle_alt_gpio(wl1_prof_name_type sender);

#endif
