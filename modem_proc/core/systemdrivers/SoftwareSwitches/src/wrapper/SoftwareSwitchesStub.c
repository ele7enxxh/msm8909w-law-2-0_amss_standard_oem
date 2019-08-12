/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          S O F T W A R E  S W I T C H E S    W R A P P E R   A P I
GENERAL DESCRIPTION
  This module provides the software switches stub apis for legacy boot drivers.

EXTERNALIZED FUNCTIONS
    SoftwareSwitchesSet()
    Sets the software switches state.
    SoftwareSwitchesGet()
    Gets the software switches state.


Copyright (c) 2009 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/SoftwareSwitches/src/wrapper/SoftwareSwitchesStub.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/30/09   aratin  Created to separate switches from legacy BIO. 
                   This file is a stub file for boot.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "SoftwareSwitches.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "diagdiag.h"


/*==========================================================================
  Generic handle to the Switches Module Device. 
  ------------------------------------------------------------------------*/



/*==========================================================================

               FUNCTION DECLARATIONS FOR MODULE

==========================================================================*/

/*===========================================================================

  FUNCTION SoftwareSwitches_Init

  DESCRIPTION
    Initializes the switches module. This function is a wrapper for attaching 
    to the switches DAL.
  PARAMETERS
    None.

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
===========================================================================*/
void SoftwareSwitches_Init(void)
{

}

/*===========================================================================

FUNCTION SoftwareSwitches_GetState

DESCRIPTION
  Processes a request to retrieve current dip switch settings.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * SoftwareSwitches_GetState (
  PACKED void *req_ptr,      /* pointer to request packet  */
  word pkt_len                 /* length of request packet   */
)
{
  SoftwareSwitches_DIAGPacketType *rsp_ptr;
  rsp_ptr = NULL;
  return (rsp_ptr);
}


/*===========================================================================

FUNCTION SoftwareSwitches_SetState

DESCRIPTION
  Processes a request to set dip switch settings.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * SoftwareSwitches_SetState(
  PACKED void *req_pkt,   /* pointer to diag request packet  */
  word pkt_len            /* length of diag request packet   */
)
{
  SoftwareSwitches_DIAGPacketType *rsp;
  rsp = NULL;
  return (rsp);
}


/*===========================================================================

FUNCTION SoftwareSwitches_GetMask

DESCRIPTION
  This function is a wrapper for the legacy bio function bio_set_sw

DEPENDENCIES 
  None.

PARAMETERS 
  uint16 mask mask of the switches whose state is being queried.

RETURN VALUE
  uint16 mask of the switches to be set.

SIDE EFFECTS
  None.
===========================================================================*/
uint16 SoftwareSwitches_GetSwitchMask (
  uint16 mask
)
{
  uint32 state = 0;
  return (uint16)state;
}

/*===========================================================================

FUNCTION SoftwareSwitches_SetMask

DESCRIPTION
  This function is a wrapper for the legacy bio function bio_set_sw

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void SoftwareSwitches_SetMask(
  uint16 mask
)
{

}




