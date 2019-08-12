/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

          S O F T W A R E  S W I T C H E S    W R A P P E R   A P I
GENERAL DESCRIPTION
  This module provides the software switches module routines to register with 
  the DIAG.



Copyright (c) 2009 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/SoftwareSwitches/src/wrapper/SoftwareSwitches.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/30/09   aratin     Created to separate switches from legacy BIO.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "SoftwareSwitches.h"
#include "DALDeviceId.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "diagdiag.h"
#include "DDISoftwareSwitches.h"
#include "DalSoftwareSwitches.h"

/*==========================================================================
  Generic handle to the Switches Module Device. 
  ------------------------------------------------------------------------*/
DalDeviceHandle * switches_handle;
/* This flag ensures that Dal apis are only called when the Switches Module has 
** attached to dal correctly
*/
static DALResult switches_dal_attach;
/* The dispatch table for the switches module. */
static const diagpkt_user_table_entry_type switches_diag_tbl[] =
{
  {DIAG_GET_DIPSW_F, DIAG_GET_DIPSW_F, SoftwareSwitches_GetState},
  {DIAG_SET_DIPSW_F, DIAG_SET_DIPSW_F, SoftwareSwitches_SetState},
};

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

  DALSYS_InitMod(NULL); 
  switches_dal_attach = DAL_DeviceAttach(DALDEVICEID_SOFTWARESWITCHES,
                                         &switches_handle);
  if(switches_dal_attach == DAL_SUCCESS)
  {
    DalDevice_Open(switches_handle,DAL_OPEN_SHARED);
    DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_MODEM_PROC, 
                                         (int)DIAGPKT_NO_SUBSYS_ID, 
                                         switches_diag_tbl);
  }
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
  uint32 state;
  const int rsp_len = sizeof(SoftwareSwitches_DIAGPacketType);
  
  if(switches_dal_attach == DAL_SUCCESS)
  {
    rsp_ptr = (SoftwareSwitches_DIAGPacketType *)diagpkt_alloc(DIAG_GET_DIPSW_F, rsp_len);
    if(rsp_ptr != NULL)
    {
      DalSoftwareSwitches_GetMask(switches_handle,(uint16)DAL_MAX_NUMBER_OF_SWITCHES,&state);
      rsp_ptr->switches = (uint16)state;
    }
    else
      return NULL;
  }
  else
    rsp_ptr = NULL;
  return (rsp_ptr);
}


/*===========================================================================

FUNCTION SoftwareSwitches_SetState

DESCRIPTION
  Processes a request to set all dip switch settings.This function is mainly 
  used by DIAG legacy code.

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
  uint32 state;
  uint16 value;
  SoftwareSwitches_DIAGPacketType *req = 
                                (SoftwareSwitches_DIAGPacketType *) req_pkt;
  const int rsp_len = sizeof(SoftwareSwitches_DIAGPacketType);
  value = (uint16)req->switches;
  if(switches_dal_attach == DAL_SUCCESS)
  {
    DalSoftwareSwitches_SetAll(switches_handle,(uint32)value);
    rsp = 
    (SoftwareSwitches_DIAGPacketType *)diagpkt_alloc(DIAG_SET_DIPSW_F, rsp_len);
    if(rsp != NULL)
    {
      DalSoftwareSwitches_GetMask(switches_handle,
                                  DAL_MAX_NUMBER_OF_SWITCHES,&state);
      rsp->switches = (uint16)state;
    }
    else
      return NULL;
  }
  else
      rsp = NULL;
  return (rsp);
}

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
uint16 SoftwareSwitches_GetSwitchMask(uint16 mask)
{
  uint32 state = 0;
  if(switches_dal_attach == DAL_SUCCESS)
  {
    DalSoftwareSwitches_GetMask(switches_handle,(uint32)mask,&state);
  }
  return (uint16)state;
}

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
void SoftwareSwitches_SetSwitchMask(uint16 mask)
{
  if(switches_dal_attach == DAL_SUCCESS)
  {
    DalSoftwareSwitches_SetAll(switches_handle,(uint32)mask);
  }
}

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
)
{
  if(switches_dal_attach == DAL_SUCCESS)
  {
    DalSoftwareSwitches_SetSwitch(switches_handle,software_switch);
  }
}

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
)
{
  if(switches_dal_attach == DAL_SUCCESS)
  {
    DalSoftwareSwitches_ClearSwitch(switches_handle,(uint32)software_switch);
  }
}





