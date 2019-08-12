#ifndef CMSHUTDOWN_H
#define CMSHUTDOWN_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  C A L L   M A N A G E R   S T A T S  M O D U L E

GENERAL DESCRIPTION


GENERAL DESCRIPTION
  This module is created for handling the RCINIT shutdown signal
  start LPM for network deregistration and acknowledge to RCINIT

EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2004 - 2009 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmshutdown.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/13   KC      Initial release
===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "mmcp_variation.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "sys.h"       /* system wide common types */

#define RCINIT_TERM_PILOT_GROUP RCINIT_GROUP_7

/*===========================================================================

FUNCTION cmstats_update_stats_and_post_event

DESCRIPTION
  creates a new client (since it need to listen when UE moves to LPM)
  and sends start-LPM to CM , waits for confirmation from CM

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_shutdown_start(void);


/*===========================================================================

FUNCTION cm_shutdown_ack_to_rcinit

DESCRIPTION
  sends cnf to RCINIT to proceed with shutdown of other tasks

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_shutdown_ack_to_rcinit(void);
#endif
