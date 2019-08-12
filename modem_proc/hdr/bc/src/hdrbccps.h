#ifndef HDRBCCPS_H
#define HDRBCCPS_H

/*===========================================================================

           B R O A D C A S T    P R O T O C O L
    D E F I N I T I O N S   F O R   S T A T E   C H A N G E   M O D U L E

DESCRIPTION
  This contains the declarations for regional types and variables used
  in the state change module of the Broadcast Control Protocol

Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrbccps.h_v   1.7   08 May 2001 18:59:52   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbccps.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/05   mpa     Added hdrbccps_powerup_init().
08/28/05   mpa     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/* <EJECT> */ 
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
                                 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
                                 
/* 
   Note: The state enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/

/* Number of protocol states. */
#define HDRBCCPS_NUM_STATES   2

typedef enum 
{
  
  HDRBCCPS_CLOSE_STATE = 0,                      
  HDRBCCPS_OPEN_STATE  = 1

} hdrbccps_state_enum_type;

/*===========================================================================

FUNCTION HDRBCCPS_POWERUP_INIT

DESCRIPTION
  Initializes the BCCP state module.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrbccps_powerup_init( void );

/*===========================================================================

FUNCTION HDRBCCPS_CHANGE_STATE

DESCRIPTION
  Called to execute a state transition in BCCP.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state to be transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrbccps_change_state
( 
  hdrbccps_state_enum_type new_state 
);

/*===========================================================================

FUNCTION HDRBCCPS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the Broadcast Control Protocol.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern hdrbccps_state_enum_type hdrbccps_get_state( void );

#endif               /* HDRBCCPS_H */
