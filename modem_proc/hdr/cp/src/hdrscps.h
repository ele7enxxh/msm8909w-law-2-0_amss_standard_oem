#ifndef HDRSCPS_H
#define HDRSCPS_H

/*===========================================================================

         S E S S I O N   C O N F I G U R A T I O N   P R O T O C O L
                  I N T E R N A L   D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for regional types and variables used
  in maintaining the state of SCP.

Copyright (c) 2000,2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrscps.h_v   1.7   24 Sep 2001 12:57:32   nneft  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/src/hdrscps.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/22/07   pba     Obsoleted substates in SCP.
03/27/07   wsh     Added support for new prior session handling
01/02/01   nmn     Initial Revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "hdrscp.h"
#include "hdrscpi.h"
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
  HDRSCPS_INACTIVE = 0, /* Inactive State */
  HDRSCPS_OPEN = 1,     /* Connected State */
  HDRSCPS_AT_INIT = 2,  /* AT Initiated State */
  HDRSCPS_AN_INIT = 3,  /* AN Initiated State */
  HDRSCPS_PS_GAUP = 4   /* Prior Session GAUP */
} hdrscps_state_enum_type;

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSCPS_CHANGE_STATE

DESCRIPTION
  Called to execute a state transition in SCP.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state to be transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrscps_change_state
( 
  hdrscps_state_enum_type new_state 
);

/*===========================================================================

FUNCTION HDRSCPS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the SCP protocol.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  current state 

SIDE EFFECTS
  None

===========================================================================*/

extern hdrscps_state_enum_type hdrscps_get_state( void );

#endif               /* HDRSCPS_H */
