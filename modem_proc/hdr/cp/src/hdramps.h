#ifndef HDRAMPS_H
#define HDRAMPS_H

/*===========================================================================

            A D D R E S S   M A N A G E M E N T   P R O T O C O L
                    I N T E R N A L   D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for regional types and variables used
  in maintaining the state of AMP.

Copyright (c) 2000,2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdramps.h_v   1.4   09 May 2001 14:55:48   nneft  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdramps.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/27/07   wsh     Added support for new prior session handling
05/08/01   nmn     Beautification
12/11/00   nmn     Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "hdramp.h"
#include "hdrampi.h"
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
  HDRAMP_INACTIVE = 0, /* Inactive State */
  HDRAMP_SETUP = 1, /* Setup State */
  HDRAMP_OPEN = 2 /* Connected State */
} hdramps_state_enum_type;

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRAMPS_CHANGE_STATE

DESCRIPTION
  Called to execute a state transition in AMP.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state to be transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdramps_change_state
( 
  hdramps_state_enum_type new_state 
);

/*===========================================================================

FUNCTION HDRAMPS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the AMP protocol.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  current state 

SIDE EFFECTS
  None

===========================================================================*/

extern hdramps_state_enum_type hdramps_get_state( void );

/*===========================================================================

FUNCTION HDRAMP_SILENT_RESTORE_PRIOR_SESSION

DESCRIPTION
  This function silently restore prior session

DEPENDENCIES
  hdrsmps_silent_restore_prior_session() must be called before this.

PARAMETERS
  None
  
RETURN VALUE
  TRUE if Success
  
SIDE EFFECTS
  None

===========================================================================*/
void hdramps_silent_restore_prior_session( void );

#endif               /* HDRAMPS_H */
