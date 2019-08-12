#ifndef HDRRUPS_H
#define HDRRUPS_H

/*===========================================================================

                 R O U T E  U P D A T E  P R O T O C O L

DESCRIPTION
  This contains the declarations for regional types and variables used
  in the state change module of the RUP protocol.

Copyright (c) 2000 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrrups.h_v   1.5   04 Jun 2001 16:24:04   vsali  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrrups.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/27/11   cnx     Merged Optimized Handoff changes.
03/07/05   pba     Added new state HDRRUPS_SYNCHRONIZE_CONNECTION, to
                   facilitate synchronization between RUP and PAC
06/01/01   vas     Re-naming for consistency. Code cleanup. Some re-organizing
07/31/00   vas     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "hdrrup.h"
#include "hdrbit.h"

/* <EJECT> */ 
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
                                 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
                                 
/* Number of RUP states */

/* 
   Note: The state enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/


#define NUM_RUP_STATES 6

typedef enum
{
  HDRRUPS_INACTIVE                     =0, /* Inactive State */
  HDRRUPS_IDLE                         =1, /* Idle State */
  HDRRUPS_CONNECTION_SETUP             =2, /* Connection Setup State */ 
  HDRRUPS_CONNECTED                    =3, /* Connected State */
  HDRRUPS_SYNCHRONIZE_CONNECTION_SETUP =4,
                                           /* Synchronize Connection Setup State*/
  HDRRUPS_TUNNEL                       =5  /* Tunnel State*/
} hdrrups_state_enum_type;
                                
/*===========================================================================

FUNCTION HDRRUPS_POWERUP_INIT

DESCRIPTION
  Initializes the RUP state module.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrrups_powerup_init( void );

/*===========================================================================

FUNCTION HDRRUPS_CHANGE_STATE

DESCRIPTION
  Called to execute a state transition in RUP.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state to be transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrrups_change_state
( 
  hdrrups_state_enum_type new_state
);

/*===========================================================================

FUNCTION HDRRUPS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the RUP protocol.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern hdrrups_state_enum_type hdrrups_get_state( void );

#endif               /* HDRRUPS_H */
