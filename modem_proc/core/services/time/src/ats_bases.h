/*=============================================================================

       T I M E   S E R V I C E   A T S  A P P S   S U B S Y S T E M

GENERAL DESCRIPTION
  Initializes and maintains Apps User time in the  Generic Framework.

REGIONAL FUNCTIONS
  ats_bases_init
    Initialize the ats_bases subsystem

  ats_bases_remote_update
    Update Time_apps from the current proc to another proc

  ats_bases_genoff_cb
    Callback called to remotely update offset

INITIALIZATION AND SEQUENCING REQUIREMENTS
    ats_bases_init() must be called.


Copyright (c) 2009 - 2013 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/ats_bases.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/17/10   din     Added check for validity in ats_bases_set_remote_update.
07/24/09   din     File created.

=============================================================================*/

#include "time_genoff.h"


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/


/*--------------------------------------------------------------------------
  Linting...
--------------------------------------------------------------------------*/

/*lint -esym(715,unused_*)  parameters prefixed with unused_ are not used */



/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/


/*=============================================================================

FUNCTION ATS_BASES_SET_REMOTE_UPDATE                                        REGIONAL

DESCRIPTION
  Update Time_apps from the current proc to another proc

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates remote generic offset if set to True

=============================================================================*/

void ats_bases_set_remote_update
( 
  /* Value set to remote_call_needed */
  boolean         value,

  /* Check for validity of bases */
  boolean check_validity
);



/*=============================================================================

FUNCTION ATS_BASES_INIT                                               REGIONAL

DESCRIPTION
  Initializes the time-apps subsystem

DEPENDENCIES
  Must be called when the phone first powers on, from time_init( ).

RETURN VALUE
  None

SIDE EFFECTS
 None

=============================================================================*/

void ats_bases_init
(
  /* Base type for a genoff in consideration */
  time_bases_type        time_base,

  /* Subsys_Base type for a genoff in consideration */
  time_bases_type        subsys_base,

  /* EFS item file */
  char                   *f_name
);


