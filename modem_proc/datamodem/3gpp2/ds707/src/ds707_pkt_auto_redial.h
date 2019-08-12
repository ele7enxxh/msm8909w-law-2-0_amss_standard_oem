#ifndef DS707_PKT_AUTO_REDIAL_H
#define DS707_PKT_AUTO_REDIAL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                    P A C K E T   A U T O   R E D I A L

GENERAL DESCRIPTION
                   This file contains functionality that is needed to redial
the call if the data call is preempted by CM before it get to the initializing
of the PPP. If the call is interrupted after PPP start happens, then the
PPP configuration parameters take care of bringing up the call. If the data
call is interrupted before PPP start happens, then this module will automatically
redial the call.
  
EXTERNALIZED FUNCTIONS
 ds707_pkt_auto_redial_init() - Registers for Physical link up/down events.

 ds707_pkt_auto_redial_process_cmd - Processes the auto redial timer
                                       expired command.
 ds707_pkt_auto_redial_process_cmd - Starts the Auto Redial timer when
                                    the data call is interrupted by CM.

  
  Copyright (c) 2008 - 2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_pkt_auto_redial.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/09/09    sa      AU level CMI modifications.
01/22/08    sy      Initial Revision.

===========================================================================*/

/*===========================================================================
                       INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DEDICATED_C2K_MODE_DATA
#include "dstask_v.h"


/*===========================================================================
                                TYPEDEFS
===========================================================================*/
/**/
#define DS707_AUTO_REDIAL_TIMER_DURATION_MS     2000
#define DS707_AUTO_REDIAL_TIMER_ITERATIONS      10


/*===========================================================================
                                 EXTERNS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKT_AUTO_REDIAL_INIT

DESCRIPTION   Called at startup. This function registers for
              physical link up/down events.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_auto_redial_init(void);

/*===========================================================================
FUNCTION      DS707_PKT_AUTO_REDIAL_PROCESS_CMD

DESCRIPTION   Processes the auto redial timer commands. This function
              will redial the call.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_pkt_auto_redial_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
);

/*===========================================================================
FUNCTION      DS707_AUTO_REDIAL_TIMER_START

DESCRIPTION   Starts the auto redial timer.If the timer is not started 
              started already and if the retry attempts are not zero,
              the timer will be started again.

DEPENDENCIES  NONE

RETURN VALUE  TRUE: Successfully started the timer.
              FALSE: Failed to start the timer. 

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_pkt_auto_redial_timer_start
(
  void
);


#endif /* FEATURE_DEDICATED_C2K_MODE */
#endif /* DS707_PKT_AUTO_REDIAL_H */


