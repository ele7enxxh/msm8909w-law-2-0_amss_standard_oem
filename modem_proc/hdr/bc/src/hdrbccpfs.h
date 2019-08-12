#ifndef HDRBCCPFS_H
#define HDRBCCPFS_H

/*===========================================================================

                 D E F I N I T I O N S   F O R   
          F L O W   S T A T E   C H A N G E   M O D U L E

DESCRIPTION
  This contains the declarations for regional types and variables used
  in the state change module of the BCCP Flows

Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrbccpfs.h_v   1.7   08 May 2001 18:59:52   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/src/hdrbccpfs.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/05   mpa     Added hdrbccpfs_get_new_flow_ptr().
08/28/05   mpa     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "hdrbit.h"
#include "hdrbccpfsi.h"
#include "hdrbccpi.h"
/* <EJECT> */ 
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
                                 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRBCCPFS_CHANGE_STATE

DESCRIPTION
  Called to execute a state transition in BCCPF.

DEPENDENCIES
  None

PARAMETERS
  flow_ptr - Pointer to the flow being processed 
  new_state - The state to be transitioned to

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrbccpfs_change_state
( 
  hdrbccp_flow_buf_type * flow_ptr,
  hdrbccpfs_state_enum_type new_state 
);

/*===========================================================================

FUNCTION HDRBCCPFS_GET_STATE

DESCRIPTION
  Called to retrieve the current state of the BCCPF protocol.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern hdrbccpfs_state_enum_type hdrbccpfs_get_state( void );


/*===========================================================================

FUNCTION HDRBCCPFS_GET_NEW_FLOW_PTR

DESCRIPTION
  Helper function to get a new flow entry.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern hdrbccp_flow_buf_type* hdrbccpfs_get_new_flow_ptr( void );

#endif               /* HDRBCCPFS_H */
