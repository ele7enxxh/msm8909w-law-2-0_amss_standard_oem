#ifndef HDRHMPSA_H
#define HDRHMPSA_H
/*===========================================================================

                 H D R   M E S S A G I N G   P R O T O C O L
                  S T A R T   A C C E S S    A T T E M P T
                           D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the HMPSA module.

Copyright (c) 2000, 2001, 2004 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrhmpsa.h_v   1.1   04 May 2001 11:08:36   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrhmpsa.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/14   vko     Fixed race between amac start attempt and bundling due to ROR
03/16/04   dna     Re-start HMPSA state machine if it is in the middle of
                   starting another access attempt when one ends.
08/22/00   dna     Created Module

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "comdef.h"

/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRHMPSA_INIT

DESCRIPTION
  This function performs initialization required for HMPSA every time HDR
  mode is entered.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrhmpsa_init( void );

/*===========================================================================

FUNCTION HDRHMPSA_START

DESCRIPTION
  This function commands the LMAC into access mode, if necessary, and 
  changes the state of the access attempt setup to indicate we are waiting
  for LMAC to enter access mode.
  

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrhmpsa_start( void );

/*===========================================================================

FUNCTION HDRHMPSA_RESTART

DESCRIPTION
  If HMPSA is in the middle of starting an access attempt, this command 
  re-starts the state machine from the beginning.  This is needed if
  HDRSRCH returns to idle mode after HMPSA commands it to access mode but
  before valid overhead info is received.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrhmpsa_restart( void );

/*===========================================================================

FUNCTION HDRHMPSA_CHECK_OVHD

DESCRIPTION
  This function activates the ACMAC if necessary and possible.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrhmpsa_check_ovhd( void );

/*===========================================================================

FUNCTION HDRHMPSA_LMAC_READY

DESCRIPTION
  This function changes the state to indicate we are now ready for 
  overhead messages.
  

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrhmpsa_lmac_ready( void );

/*===========================================================================

FUNCTION HDRHMPSA_ABORT

DESCRIPTION
  This function aborts any attempt that may be in progress to set up an
  access attempt.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrhmpsa_abort( void );

/*===========================================================================

FUNCTION HDRHMPSA_AMAC_GIVEN_ATTEMPT

DESCRIPTION
  This function returns the current value of amac given attempt flag

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  current value of amac given attempt flag

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrhmpsa_amac_given_attempt( void );

/*===========================================================================

FUNCTION HDRHMPSA_RESET_AMAC_GIVEN_ATTEMPT

DESCRIPTION
  This function resets the amac given attempt flag to FALSE.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrhmpsa_reset_amac_given_attempt( void );

#endif /* HDRHMPSA_H */
