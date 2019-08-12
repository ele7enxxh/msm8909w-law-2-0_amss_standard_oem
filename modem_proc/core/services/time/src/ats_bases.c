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


Copyright (c) 2009 - 2015 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/ats_bases.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/15   abh     Changes for correctly handling ATS_PRIVATE bases.
08/17/10   din     Added check for validity in ats_bases_set_remote_update.
02/22/10   din     Increased storage threshold to 10 ms.
07/24/09   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "time_genoff_v.h"
#include "time_remote.h"

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
)
{
  /* Pointer to ats_bases in consideration */
  time_genoff_ptr        ptime_genoff;

  /* Index */
  int i;

  /* Temp value */
  boolean temp_value;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize Time_bases, start from ATS_RTC */
  for ( i = 0; i < ATS_MAX; i++ )
  {
    /* Get the pointer to time_base instance */
    ptime_genoff = time_genoff_get_pointer( (time_bases_type)i );
    
    /* Get the value to be set to remote_call_needed */
    temp_value = value & time_genoff_get_allow_remote_updates( ptime_genoff );

    /* Set whether to filter remote calls */
    time_genoff_set_remote_call_needed( ptime_genoff, temp_value );
   
    if ( check_validity == TRUE )
    {
      if ( time_genoff_is_valid( ptime_genoff ) )
      {
        /* Update ats_bases from the current proc to another proc */
        time_genoff_set_remote_genoff( ptime_genoff );
      }
    }
    else
    {
      /* Update ats_bases from the current proc to another proc */
      time_genoff_set_remote_genoff( ptime_genoff );
    }
  }
  
} /* ats_bases_remote_update */



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
)
{

  /* Pointer to ats_bases in consideration */
  time_genoff_ptr        ptime_genoff;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the pointer to time_base instance */
  ptime_genoff = time_genoff_get_pointer( time_base );

  /* Initialize ats_bases */
  time_genoff_pre_init( ptime_genoff, time_base );

  /* Add base subsys to ats_bases */
  time_genoff_add_base_subsys( ptime_genoff, subsys_base );

  #if defined(FEATURE_MULTIPROCESSOR) && !defined(FEATURE_TIME_NO_RPC)
  time_genoff_set_remote_call_needed( ptime_genoff, TRUE );
  #endif /* FEATURE_MULTIPROCESSOR && !FEATURE_TIME_NO_RPC */

  #ifndef IMAGE_APPS_PROC
  /* Initialize storage spec */
  time_genoff_updates_per_storage( ptime_genoff, f_name, 
		  TIME_GENOFF_UPDATE_THRESHOLD_MS );
  #endif /* IMAGE_APPS_PROC */

  /* Initialize ats_bases */
  time_genoff_post_init( ptime_genoff );

} /* ats_bases_init */


