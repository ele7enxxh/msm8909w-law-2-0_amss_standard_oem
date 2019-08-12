/*==============================================================================

FILE:      DalSoftwareSwitches.c

DESCRIPTION: This file implements the Software Switches DAL module.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2008-2012 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

=================================================================================



                         EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/SoftwareSwitches/src/DalSoftwareSwitches.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/09   aratin  Created to separate Dip switches from legacy bio and 
                   consolidate the remaining DIAG dip switch functionality.
==============================================================================*/
#include "DalSoftwareSwitches.h"
#include "DALGlbCtxt.h"

#define SW_TRUE 1
#define SW_FALSE 0
#define SWITCHES_TOTAL_NUMBER 16

DALSoftwareSwitches switches_state;
uint32   Single_Proc_Init; 
/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult 
SoftwareSwitches_DriverInit(SoftwareSwitchesDrvCtxt *pCtxt)
{
	return DAL_SUCCESS;
}

DALResult 
SoftwareSwitches_DriverDeInit(SoftwareSwitchesDrvCtxt *pCtxt)
{
	return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are declared in DalDevice Interface. 
------------------------------------------------------------------------------*/
/*===========================================================================

  FUNCTION SoftwareSwitches_DeviceInit

  DESCRIPTION  See the Description of this API in DalSoftwareSwitches.h

===========================================================================*/
DALResult 
SoftwareSwitches_DeviceInit(SoftwareSwitchesClientCtxt *pCtxt)
{
  if(Single_Proc_Init != SW_TRUE)
    {
    if(DAL_SUCCESS != DALSYS_SyncCreate(
                                      DALSYS_SYNC_ATTR_RESOURCE,
                                      &(pCtxt->pSoftwareSwitchesDevCtxt->SoftwareSwitchesSync),
                                      &(pCtxt->pSoftwareSwitchesDevCtxt->SwSwitchesSyncObject)))
    {
      return DAL_ERROR;
    }
    (void)DALSYS_SyncEnter(pCtxt->pSoftwareSwitchesDevCtxt->SoftwareSwitchesSync);
      pCtxt->pSoftwareSwitchesDevCtxt->switches_ptr = &switches_state;
      pCtxt->pSoftwareSwitchesDevCtxt->switches_ptr->state = 0;
      pCtxt->pSoftwareSwitchesDevCtxt->switches_ptr->switches_initialized = SW_TRUE;   
      Single_Proc_Init = SW_TRUE;

    (void)DALSYS_SyncLeave(pCtxt->pSoftwareSwitchesDevCtxt->SoftwareSwitchesSync);
    }
    return DAL_SUCCESS;
}

DALResult 
SoftwareSwitches_DeviceDeInit(SoftwareSwitchesClientCtxt *pCtxt)
{
    return DAL_SUCCESS;
}

DALResult 
SoftwareSwitches_PowerEvent(SoftwareSwitchesClientCtxt *pCtxt, DalPowerCmd PowerCmd, 
                    DalPowerDomain PowerDomain)
{
    return DAL_SUCCESS;
}

DALResult 
SoftwareSwitches_Open(SoftwareSwitchesClientCtxt *pCtxt, uint32 dwaccessMode )
{
    return DAL_SUCCESS;
}

DALResult 
SoftwareSwitches_Close(SoftwareSwitchesClientCtxt *pCtxt)
{
    return DAL_SUCCESS;
}

DALResult 
SoftwareSwitches_Info(SoftwareSwitchesClientCtxt *pCtxt,DalDeviceInfo *pdeviceInfo, uint32 dwSize)
{
    return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in DalSoftwareSwitches Interface. 
------------------------------------------------------------------------------*/
/*===========================================================================

  FUNCTION SoftwareSwitches_SetSwitch

  DESCRIPTION  See the Description of this API in DalSoftwareSwitches.h

===========================================================================*/
DALResult 
SoftwareSwitches_SetSwitch( SoftwareSwitchesClientCtxt * pCtxt, 
                     uint32  software_switch) 
{
  /* there are only 16 bits so we make sure we are setting only 
  those 16 bits specified.*/
  software_switch &= DAL_MAX_NUMBER_OF_SWITCHES;
  (void)DALSYS_SyncEnter(pCtxt->pSoftwareSwitchesDevCtxt->SoftwareSwitchesSync);
  pCtxt->pSoftwareSwitchesDevCtxt->switches_ptr->state |= software_switch;
  (void)DALSYS_SyncLeave(pCtxt->pSoftwareSwitchesDevCtxt->SoftwareSwitchesSync);

  return DAL_SUCCESS;
}

/*===========================================================================

  FUNCTION SoftwareSwitches_ClearSwitch

  DESCRIPTION  See the Description of this API in DalSoftwareSwitches.h

===========================================================================*/
DALResult 
SoftwareSwitches_ClearSwitch( SoftwareSwitchesClientCtxt * pCtxt, 
                     uint32  software_switch) 
{
  /* there are only 16 bits so we make sure we are setting only those 16 
  bits specified.*/
  software_switch &= DAL_MAX_NUMBER_OF_SWITCHES;
  (void)DALSYS_SyncEnter(pCtxt->pSoftwareSwitchesDevCtxt->SoftwareSwitchesSync);
  pCtxt->pSoftwareSwitchesDevCtxt->switches_ptr->state &= (~software_switch);
  (void)DALSYS_SyncLeave(pCtxt->pSoftwareSwitchesDevCtxt->SoftwareSwitchesSync);
  
  return DAL_SUCCESS;
}

/*===========================================================================

  FUNCTION SoftwareSwitches_SetAll

  DESCRIPTION  See the Description of this API in DalSoftwareSwitches.h

===========================================================================*/
DALResult 
SoftwareSwitches_SetAll( SoftwareSwitchesClientCtxt * pCtxt, 
                     uint32  software_switch) 
{
  /* there are only 16 bits so we make sure we are setting only those 16 
  bits specified.*/
  software_switch &= DAL_MAX_NUMBER_OF_SWITCHES;
  (void)DALSYS_SyncEnter(pCtxt->pSoftwareSwitchesDevCtxt->SoftwareSwitchesSync);
  pCtxt->pSoftwareSwitchesDevCtxt->switches_ptr->state = software_switch;
  (void)DALSYS_SyncLeave(pCtxt->pSoftwareSwitchesDevCtxt->SoftwareSwitchesSync);
  return DAL_SUCCESS;
}

/*===========================================================================

  FUNCTION SoftwareSwitches_GetMask

  DESCRIPTION  See the Description of this API in DalSoftwareSwitches.h

===========================================================================*/
DALResult 
SoftwareSwitches_GetMask( SoftwareSwitchesClientCtxt * pCtxt, 
                     uint32  software_switch, uint32*  state) 
{
  software_switch &= DAL_MAX_NUMBER_OF_SWITCHES;
  *state = software_switch & pCtxt->pSoftwareSwitchesDevCtxt->switches_ptr->state;
  return DAL_SUCCESS;
}
