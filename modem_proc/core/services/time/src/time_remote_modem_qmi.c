/*=============================================================================

                  T I M E   S E R V I C E   S U B S Y S T E M                 
                        
                M O D E M   T O   A P P ' S   F U N C T I O N S

GENERAL DESCRIPTION

  Implements modem <-> applications processor time communication functions 
  when QMI is used


EXTERNALIZED FUNCTIONS

  TBD
    tbd


INITIALIZATION AND SEQUENCING REQUIREMENTS
  TBD


Copyright(c) 2005 - 2013 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------

06/26/11   gp     File created based 8960

=============================================================================*/



/*=============================================================================

                                 INCLUDE FILES

=============================================================================*/
#include "time_genoff_v.h"
#include "ats_v.h"
/*=============================================================================

FUNCTION TIME_REMOTE_ATOM_GENOFF_SET_GENERIC_OFFSET                               

DESCRIPTION
  Sets Generic offset for genoff passed as argument.

DEPENDENCIES
  Should be used only by remote calls

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_remote_atom_genoff_set_generic_offset
(
  /* Base of Genoff in consideration */
  time_bases_type base,

  /* Generic Offset to be set */
  int64           offset
)
{
  /* Pointer to Time_genoff instance in consideration */
  time_genoff_ptr                 ptime_genoff;
     
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the pointer to genoff in consideration */
  ptime_genoff = time_genoff_get_pointer( base );

  /*we will allow setting of ATS_TOD only if 
   it has not been set yet since boot up by either 
   modem or apps. If ptime_genoff->valid is set,
   it means that ATS_TOD was set by some entity*/
  if((ATS_TOD == base) 
     && ptime_genoff->valid)
  {
    ATS_ERR("Setting of ATS_TOD request on modem filtered out"); 
    return;
  }

  /* Sets Generic offset */
  time_genoff_set_generic_offset(ptime_genoff, offset );
  
} /* time_remote_atom_genoff_set_generic_offset */