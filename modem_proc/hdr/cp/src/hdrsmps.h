#ifndef HDRSMPS_H
#define HDRSMPS_H

/*===========================================================================

            S E S S I O N   M A N A G E M E N T   P R O T O C O L
                    I N T E R N A L   D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for regional types and variables used
  in maintaining the state of SMP.

Copyright (c) 2000-2004 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrsmps.h_v   1.4   08 May 2001 14:22:56   nneft  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrsmps.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/27/07   wsh     Added support for new prior session handling
10/26/04   mpa     Added new HDRSMPS_CLOSING state.
11/21/00   nmn     Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "hdrsmp.h"
#include "hdrsmpi.h"
#include "bit.h"

/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* 
   Note: The state enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/

typedef enum
{
  HDRSMPS_INACTIVE = 0,  /* Inactive State */
  HDRSMPS_AMP_SETUP = 1, /* AMP Setup State */
  HDRSMPS_OPEN = 2,      /* Open State */
  HDRSMPS_CLOSING = 3    /* Session Closing State */
} hdrsmps_state_enum_type;

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSMPS_CHANGE_STATE

DESCRIPTION
  Called to execute a state transition in SMP.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state to be transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrsmps_change_state
( 
  hdrsmps_state_enum_type new_state 
);

/*===========================================================================

FUNCTION HDRSMPS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the SMP protocol.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  current state 

SIDE EFFECTS
  None

===========================================================================*/

extern hdrsmps_state_enum_type hdrsmps_get_state( void );

/*===========================================================================

FUNCTION HDRSMPS_SILENT_RESTORE_PRIOR_SESSION

DESCRIPTION
  Called to set SMP to OPEN state without giving any indications.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  current state

SIDE EFFECTS
  None

===========================================================================*/
void hdrsmps_silent_restore_prior_session ( void );

#endif               /* HDRSMPS_H */
