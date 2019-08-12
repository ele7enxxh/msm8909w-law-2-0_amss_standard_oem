#ifndef NPA_INIT_H
#define NPA_INIT_H
/*==============================================================================

FILE:      npa_init.h

DESCRIPTION: NPA framework initialization

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2009 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/power/npa_init.h#1 $

==============================================================================*/

/*============================================================================

FUNCTION NPA_INIT

DESCRIPTION

  This function initializes the NPA subsystem. 
  
DEPENDENCIES
  No other NPA functions can be invoked prior to npa_init

RETURN VALUE
  None

SIDE EFFECTS
  Initializes internal NPA data structures

============================================================================*/

void npa_init( void );

#endif /* NPA_INIT_H */
