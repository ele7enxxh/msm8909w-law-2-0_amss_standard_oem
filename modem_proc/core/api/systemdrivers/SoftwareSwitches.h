#ifndef __SWITCHES_H__
#define __SWITCHES_H__
/*===========================================================================

                       S O F T W A R E  S W I T C H E S   
                      M O D U L E   D E F I N I T I O N S

DESCRIPTION
  This module implements the wrappers for the software switches DAL.

REFERENCES

Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/SoftwareSwitches.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/05/09   aratin  Created as a wrapper for the Switches DAL
===========================================================================*/

/* Include the target-specific file */
#include "comdef.h"

/* ----------------------------------------------------------------------------
 * The Data Types Used
 *------------------------------------------------------------------------------*/
/*===========================================================================

PACKET   SoftwareSwitches_DIAGPacketType

ID       DIAG_DIPSW_F

PURPOSE  Sent by DM to retreive the current dip switch settings

RESPONSE DMSS performs the test, then responds.

===========================================================================*/
typedef PACK(struct)
{
  byte cmd_code;
  word switches;
} SoftwareSwitches_DIAGPacketType;

/* ----------------------------------------------------------------------------
 * The externalized functions
 *------------------------------------------------------------------------------*/

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
void SoftwareSwitches_Init(void);

/*===========================================================================

FUNCTION SoftwareSwitchesGetState

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
);
/*===========================================================================

FUNCTION SoftwareSwitchesSetState

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
  PACKED void *req_pkt,   /* pointer to request packet  */
  word pkt_len               /* length of request packet   */
);

/*===========================================================================

FUNCTION SoftwareSwitches_GetSwitchMask

DESCRIPTION
  This function is a wrapper for the legacy bio function BIO_GET_SW

DEPENDENCIES 
  None.

PARAMETERS 
  uint16 mask : mask of the switches whose state is being queried.

RETURN VALUE
  uint16 state of the switches.

SIDE EFFECTS
  None.
===========================================================================*/
uint16 SoftwareSwitches_GetSwitchMask(uint16 mask);

/*===========================================================================

FUNCTION SoftwareSwitches_SetSwitchMask

DESCRIPTION
  This function is a wrapper for the legacy bio function bio_set_sw

DEPENDENCIES 
  None.

PARAMETERS 
  uint16 mask mask of the switches whose state is being queried.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void SoftwareSwitches_SetSwitchMask(uint16 mask);

/*===========================================================================

FUNCTION SoftwareSwitches_SetSw

DESCRIPTION
  This function is a wrapper for the legacy bio function bio_set_sw

DEPENDENCIES 
  None.

PARAMETERS 
  uint32 software_switch : enum of the switches to be set.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void SoftwareSwitches_SetSw(
  uint32 software_switch
);

/*===========================================================================

FUNCTION SoftwareSwitches_ClearSw

DESCRIPTION
  This function clears a single switch.

DEPENDENCIES
  None.
PARAMETERS 
  uint32 software_switch : enum of the switch being cleared.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void SoftwareSwitches_ClearSw(
  uint32 software_switch
);

#endif /* __SWITCHES_H__ */


