#ifndef HDRALMPS_H
#define HDRALMPS_H

/*===========================================================================

          A I R   L I N K   M A N A G E M E N T   P R O T O C O L
    D E F I N I T I O N S   F O R   S T A T E   C H A N G E   M O D U L E

DESCRIPTION
  This contains the declarations for regional types and variables used
  in the state change module of the ALMP protocol.

Copyright (c) 2000, 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdralmps.h_v   1.7   08 May 2001 18:59:52   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdralmps.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/08/01   dna     Re-naming for consistency.
05/01/01   dna     Added externs for get_pending_state and change_pending_state
07/13/00   dna     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "hdralmp.h"
#include "hdrbit.h"

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

/* The next new ALMP state must be added as state #4 */
#define HDRALMPS_NUM_STATES 5
typedef enum
{
  HDRALMPS_INACTIVE = 0, /* Inactive State */
  HDRALMPS_INIT = 1, /* Initialization State */
  HDRALMPS_IDLE = 2, /* Idle State */
  HDRALMPS_CONNECTED = 3, /* Connected State */
  HDRALMPS_SUSPEND = 4 /* Suspended State */
} hdralmps_state_enum_type;

/*===========================================================================

FUNCTION HDRALMPS_CHANGE_STATE

DESCRIPTION
  Called to execute a state transition in ALMP.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state to be transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdralmps_change_state
( 
  hdralmps_state_enum_type new_state 
);

/*===========================================================================

FUNCTION HDRALMPS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the ALMP protocol.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern hdralmps_state_enum_type hdralmps_get_state( void );

/*===========================================================================

FUNCTION HDRALMPS_GET_PENDING_STATE

DESCRIPTION
  Called to retrieve the pending state of the ALMP protocol.  
  This funciton is only to be used by other code in the ALMP protocol.
  The pending state is the same as the current state unless it was 
  explicitly set differently since the last state transition.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  The pending state of ALMP

SIDE EFFECTS
  None

===========================================================================*/

extern hdralmps_state_enum_type hdralmps_get_pending_state( void );

/*===========================================================================

FUNCTION HDRALMPS_CHANGE_PENDING_STATE

DESCRIPTION
  Called to set the pending state of the ALMP protocol.  
  This funciton is only to be used by other code in the ALMP protocol.
  The pending state is a way for ALMP to specify a future state but delay
  the transition until some other action occurs.
  The pending state is the same as the current state unless it was 
  explicitly set differently since the last state transition.

DEPENDENCIES
  None

PARAMETERS
  pending_state - The state that ALMP is to transition to at some future 
  time.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

extern void hdralmps_change_pending_state
( 
  hdralmps_state_enum_type pending_state 
);

#endif               /* HDRALMPS_H */
