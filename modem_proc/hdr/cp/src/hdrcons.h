#ifndef HDRCONS_H
#define HDRCONS_H

/*===========================================================================

                C O N N E C T E D   S T A T E   P R O T O C O L
                    I N T E R N A L   D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for regional types and variables used
  in maintaining the state of CON.

Copyright (c) 2000, 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrcons.h_v   1.4   18 May 2001 16:00:34   nneft  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrcons.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/28/00   nmn     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "hdrcon.h"
#include "hdrconi.h"
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
  HDRCONS_INACTIVE = 0, /* Inactive State */
  HDRCONS_OPEN = 1 /* Connected State */
} hdrcons_state_enum_type;

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRCONS_CHANGE_STATE

DESCRIPTION
  Called to execute a state transition in CON.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state to be transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcons_change_state
( 
  hdrcons_state_enum_type new_state 
);

/*===========================================================================

FUNCTION HDRCONS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the CON protocol.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  current state 

SIDE EFFECTS
  None

===========================================================================*/

hdrcons_state_enum_type hdrcons_get_state( void );

#endif               /* HDRCONS_H */
