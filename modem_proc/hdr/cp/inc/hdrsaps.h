#ifndef HDRSAPS_H
#define HDRSAPS_H

/*===========================================================================

            S I G N A L I N G   A D A P T A T I O N   P R O T O C O L
                    I N T E R N A L   D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for regional types and variables used
  in maintaining the state of SAP.

Copyright (c) 2011-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdrsaps.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/11/11   cnx     Fixed compiler errors.
01/01/11   pba     Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"

#ifdef FEATURE_LTE_TO_HDR_OH
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
  HDRSAPS_CLOSE = 0,     /* Close State */
  HDRSAPS_SETUP = 1,     /* Setup State */
  HDRSAPS_OPEN = 2       /* Open State */
} hdrsaps_state_enum_type;

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRSAPS_CHANGE_STATE

DESCRIPTION
  Called to execute a state transition in SAP.

DEPENDENCIES
  None

PARAMETERS
  new_state - The state to be transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void hdrsaps_change_state
( 
  hdrsaps_state_enum_type new_state 
);

/*===========================================================================

FUNCTION HDRSAPS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the SAP.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  current state 

SIDE EFFECTS
  None

===========================================================================*/

extern hdrsaps_state_enum_type hdrsaps_get_state( void );

#endif /* FEATURE_LTE_TO_HDR_OH */
#endif /* HDRSAPS_H */
