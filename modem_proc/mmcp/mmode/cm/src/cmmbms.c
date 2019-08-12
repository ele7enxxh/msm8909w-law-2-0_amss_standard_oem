/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  C A L L   M A N A G E R   M B M S   M O D U L E

GENERAL DESCRIPTION

  This module contains the Call Manager MBMS module.

  The MBMS object is responsible for:
  1. Processing clients' MBMS commands.
  2. Processing LL replies for clients' MBMS commands.
  3. Processing LL MBMS related notifications.
  4. Notifying the client list of MBMS events.


EXTERNALIZED FUNCTIONS

  cmmbms_timer_proc
  cmmbms_client_cmd_proc
  cmmbms_init
  cmmbms_ptr
  cmmbms_mc_rpt_proc


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2004 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmmbms.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/11/11   am      Cleaning stubs and runnning qtf SUTE
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
07/01/08   rm      Mainlining of MBMS changes
06/25/07   rp      Adding support for CM_MBMS_EVENT_GENERIC_CMD
03/13/07   rp      Initial release for MBMS phase 2A integration

===========================================================================*/



/****************************************************************************

 Organization of the file:

    The file is divided into multiple sections.
    You can jump from one sections to the other by searching for / followed
    by 2 *'s. The order of objects defined is as follows:

        includes
        forward declarations
        #defines

        functions
        - Internal
        - External

****************************************************************************/

#include "mmcp_variation.h"
#include "customer.h"
/*lint -esym(766,customer.h)*/

#if defined (FEATURE_MBMS)
#error code not present
#else
/*===========================================================================
FUNCTION cmmbms_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmmbms_dummy( void )
/*lint -esym(714,cmmbms_dummy) */
/*lint -esym(528,cmmbms_dummy) */
/*lint -esym(765,cmmbms_dummy)
** Can't be static as it would result in compiler warnings 
*/ 
{
}
#endif /* FEATURE_MBMS */
