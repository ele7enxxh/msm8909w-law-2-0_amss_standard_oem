/*==============================================================================

  FILE:      Dalmpm.c

  DESCRIPTION: This file implements a sample DeviceDriver.

  DEPENDENCIES: None

                Copyright (c) 2014 QUALCOMM Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

===============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/src/dal/Dalmpm.c#1 $
$DateTime: 2016/12/13 07:59:23 $
=============================================================================*/ 
#include "Dalmpm.h"
#include "vmpm_internal.h"
/*------------------------------------------------------------------------------
Following functions are for DALDriver specific functionality
------------------------------------------------------------------------------*/
DALResult mpm_DriverInit(mpmDrvCtxt *pCtxt)
{
  return DAL_SUCCESS;
}

DALResult mpm_DriverDeInit(mpmDrvCtxt *pCtxt)
{
  return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are declared in DalDevice Interface. 
------------------------------------------------------------------------------*/
DALResult mpm_DeviceInit(mpmClientCtxt *pCtxt)
{
  return DAL_SUCCESS;
}

DALResult mpm_DeviceDeInit(mpmClientCtxt *pCtxt)
{
  return DAL_SUCCESS;
}

DALResult mpm_PowerEvent(mpmClientCtxt *pCtxt,
                         DalPowerCmd    PowerCmd, 
                         DalPowerDomain PowerDomain)
{
  return DAL_SUCCESS;
}

DALResult mpm_Open(mpmClientCtxt *pCtxt, uint32 dwaccessMode )
{
  return DAL_SUCCESS;
}

DALResult mpm_Close(mpmClientCtxt *pCtxt)
{
  return DAL_SUCCESS;
}

DALResult mpm_Info(mpmClientCtxt *pCtxt,
                   DalDeviceInfo *pdeviceInfo,
                   uint32         dwSize)
{
  return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Following functions are extended in Dalmpm Interface. 
------------------------------------------------------------------------------*/

DALResult mpm_ConfigPin( mpmClientCtxt      * pCtxt,
                         uint32               mpmPinNum,
                         mpm_pin_config_type *pinConfigInfo)
{
  mpm_status_type status = MPM_FAIL;

  status = vmpm_configPin(mpmPinNum, pinConfigInfo);

  if (MPM_SUCCESS == status ) 
  {
    return DAL_SUCCESS;
  }
  else
  {
    return DAL_ERROR;
  }
}

DALResult mpm_ConfigAndEnablePin( mpmClientCtxt * pCtxt,
                                  uint32               mpmPinNum,
                                  mpm_pin_config_type *pinConfigInfo)
{
  mpm_status_type status = MPM_FAIL;

  status = vmpm_configAndEnablePin(mpmPinNum, pinConfigInfo);

  if (MPM_SUCCESS == status ) 
  {
    return DAL_SUCCESS;
  }
  else
  {
    return DAL_ERROR;
  }
}

DALResult mpm_EnablePin( mpmClientCtxt       *pCtxt,
                         uint32               mpmPinNum)
{
  mpm_status_type status = MPM_FAIL;

  status = vmpm_enablePin(mpmPinNum);

  if (MPM_SUCCESS == status ) 
  {
    return DAL_SUCCESS;
  }
  else
  {
    return DAL_ERROR;
  }
}

DALResult mpm_DisablePin( mpmClientCtxt       *pCtxt,
                         uint32               mpmPinNum)
{
  mpm_status_type status = MPM_FAIL;

  status = vmpm_disablePin(mpmPinNum);

  if (MPM_SUCCESS == status ) 
  {
    return DAL_SUCCESS;
  }
  else
  {
    return DAL_ERROR;
  }
}

DALResult mpm_GetNumMappedIrq( mpmClientCtxt * pCtxt, uint32 *pnum) 
{
  if(NULL == pnum)
  {
    return DAL_ERROR;
  }
   
  *pnum = vmpm_getNumMappedIrqs();
  return DAL_SUCCESS;
}

DALResult mpm_GetNumMappedGpio( mpmClientCtxt * pCtxt, uint32 *pnum) 
{
  if(NULL == pnum)
  {
    return DAL_ERROR;
  }
   
  *pnum = vmpm_getNumMappedGpios();
  return DAL_SUCCESS;
}

DALResult mpm_MapInterrupts( mpmClientCtxt           *pCtxt,
                             mpm_config_info_type    *irqArray,
                             uint32                  irqCount,
                             uint32                  *pIrqsMapped) 
{
  mpm_status_type status = MPM_FAIL;

  status = vmpm_MapIrqsWithMaster(irqArray, irqCount, pIrqsMapped);

  if ( MPM_SUCCESS == status ) 
  {
    return DAL_SUCCESS;
  }
  else
  {
    return DAL_ERROR;
  }
}

DALResult mpm_SetupInterrupts( mpmClientCtxt           *pCtxt,
                               mpm_config_info_type    *pIrqArray,
                               uint32                  irqCount) 
{

  mpm_status_type status = MPM_FAIL;

  status = vmpm_SetupIrqs(pIrqArray, irqCount);

  if ( MPM_SUCCESS == status ) 
  {
    return DAL_SUCCESS;
  }
  else
  {
    return DAL_ERROR;
  }
}

DALResult mpm_QueryPinMapping( mpmClientCtxt  *pCtxt, 
                               uint32         localPinNum, 
                               mpm_pin_type   pinType, 
                               uint32         *pMpmPinNum)
{
  mpm_status_type status = MPM_FAIL;

  status = vmpm_queryPinMapping(localPinNum, pinType, pMpmPinNum);

  if ( MPM_SUCCESS == status ) 
  {
    return DAL_SUCCESS;
  }
  else
  {
    return DAL_ERROR;
  }
}

DALResult mpm_QueryPinConfigInfo( mpmClientCtxt           *pCtxt, 
                                  uint32                  mpmPinNum, 
                                  mpm_pin_query_info_type *pMpmPinInfo )
{
  mpm_status_type status = MPM_FAIL;

  status = vmpm_queryPinConfigInfo(mpmPinNum, pMpmPinInfo);

  if ( MPM_SUCCESS == status ) 
  {
    return DAL_SUCCESS;
  }
  else
  {
    return DAL_ERROR;
  }
}

DALResult mpm_AddIrqPinToMappingTbl( mpmClientCtxt            *pCtxt, 
                                     uint32                   mpmPinNum, 
                                     mpm_int_pin_mapping_type *pNewMapTblEntryInfo )
{
  mpm_status_type status = MPM_FAIL;

  status = vmpm_addIrqPinToMappingTbl(mpmPinNum, pNewMapTblEntryInfo );

  if (MPM_SUCCESS == status) 
  {
    return DAL_SUCCESS;
  }
  else
  {
    return DAL_ERROR;
  }
}
