#ifndef HDROVHDS_H
#define HDROVHDS_H

/*===========================================================================

          O V E R H E A D  M E S S A G E S   P R O T O C O L

DESCRIPTION
  This contains the declarations for regional types and variables used
  in the state change module of the overhead protocol.

Copyright (c) 2000 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrovhds.h_v   1.3   09 May 2001 11:31:04   vsali  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrovhds.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/01   vas     Re-naming for consistency
09/07/00   vas     Created Module

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

                        DATA  DECLARATIONS

===========================================================================*/
/* 
   Note: The state enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/
                                 
typedef enum
{
  HDROVHDS_INACTIVE              =0,
  HDROVHDS_PROCESS_ALL_MSGS      =1,
  HDROVHDS_SLEEP                 =2,
  HDROVHDS_FREQ_CHANGE_IN_PROG   =3,
  HDROVHDS_ACC_HO_IN_PROG        =4,
  HDROVHDS_WAIT_FOR_LINK         =5,
  HDROVHDS_TUNNEL_ACTIVE         =6
} hdrovhds_state_enum_type;
                                
/*===========================================================================

FUNCTION HDROVHDS_POWERUP_INIT

DESCRIPTION
  Initializes the OVHD state module.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrovhds_powerup_init( void );

/*===========================================================================

FUNCTION HDROVHDS_CHANGE_STATE

DESCRIPTION
  Called to execute a state transition in OVHD.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state to be transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrovhds_change_state
( 
  hdrovhds_state_enum_type new_state
);

/*===========================================================================

FUNCTION HDROVHDS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the OVHD protocol.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern hdrovhds_state_enum_type hdrovhds_get_state( void );

#endif /* HDROVHDS_H */
