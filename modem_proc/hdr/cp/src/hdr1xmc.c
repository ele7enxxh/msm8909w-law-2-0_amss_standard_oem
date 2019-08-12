/*===========================================================================

                  HDR TO 1X EXTERNAL INTERFACE


DESCRIPTION
  This file is the external interface exported by HDRMC to the 1X protocols

EXTERNALIZED FUNCTIONS (Global)
  hdr1xmc_get_location() - Retrieves the HDR location value if available

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.


Copyright (c) 2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdr1xmc.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/08/03   mpa     Created module

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "hdr1xmc.h"
#include "hdrlup.h"

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      FUNCTION  DEFINITIONS

===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION HDR1XMC_GET_LOCATION

DESCRIPTION
  This function externalizes the HDR Location Information if available
  in IS2000 format (i.e. SID/NID/PZID)

DEPENDENCIES
  None

PARAMETERS
  location_ptr - A pointer to the location value of the current HDR system

RETURN VALUE
  TRUE - The location is valid and returned in location_ptr
  FALSE - The location is unknown or in a format unknown to IS2000-A

SIDE EFFECTS
  None
===========================================================================*/

boolean hdr1xmc_get_location ( hdr1xmc_location_type * location_ptr )
{

   /* Holds the HDR location information in IS2000 compatible format */
   uint16 sid;
   uint16 nid;
   uint8  pzid;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if ( hdrlup_get_location(&sid, &nid, &pzid) )
   {
     location_ptr->sid  = sid;
     location_ptr->nid  = nid;
     location_ptr->pzid = pzid;

     return TRUE;
   }

   return FALSE;

} /* hdr1xmc_get_location() */


