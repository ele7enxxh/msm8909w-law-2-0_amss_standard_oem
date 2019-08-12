/*===========================================================================

             M A I N   D A L   T L M M   D R I V E R   F I L E

DESCRIPTION

  Contains the majority of functionality for the DAL TLMM driver.  The
  API in this module MUST always remain target independent.


===========================================================================
             Copyright (c) 2010 - 2014 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/tlmm/src/DALTLMM.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/09/09   dcf     Fix for immediateConfigs in restoring GPIOs from sleep.
02/23/09   an      Mainlining FEATURE_MANAGE_GPIOS_FOR_SLEEP and 
                   FEATURE_APPS_GPIOS_LOW_POWER by having internal flags 
                   in TLMM.
01/08/09   dcf     General clean up, removal of bare enumerated types and 
                   addition of uniform headers and comments.
===========================================================================*/

/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/


#include <HALtlmm.h>
#include <DALStdDef.h>
#include <DDIHWIO.h>
#include "DALTLMMState.h"
#include "DALTLMM.h"
#include "DALTLMMPropDef.h"
#include "DDITlmm.h"
#include "DDIPlatformInfo.h"
#include "DDIChipInfo.h"


/*==========================================================================

                     LOCAL TYPE DEFINES FOR THIS MODULE

==========================================================================*/


typedef struct
{
  uint32 ePort;
  uint32 nOffset;
  uint32 nMask;
  uint32 nEnableValue;
  uint32 nDisableValue;

}DALTLMM_PortCfgType;

typedef struct
{
  char              *pszGpioStr;
  uint32        nGpioKey;
  DALGpioIdType nGpioId;
  TLMMGpioIdCfgType *pGpioModes;
  uint32            nNumModes;
  uint32            nCurrentMode;

}DALGpioSecAccessType;

uint16 gnGpioAdder = 0;

static boolean bHasPlatIO;
static boolean bHasPlatIOCfgs;

#define DAL_GPIO_SET_KEY(nGpioNumber, nGpioId) \
           (nGpioNumber | (((0x0000FFFF & nGpioId) + ++gnGpioAdder) << 16))

#define DAL_GPIO_GET_KEY(nGpioId) ((nGpioId & 0xFFFF0000) >> 16)

#define DAL_GPIO_GET_PIN(nGpioId) (nGpioId & 0x0000FFFF)


/*==========================================================================

                     LOCAL DATA FOR THIS MODULE

==========================================================================*/

uint32 gnTotalGPIO;
extern DALTLMMStateType* pgtState;
static DALTLMM_PortCfgType gtHALPorts[DAL_TLMM_NUM_PORTS];
static uint32 gnTotalPorts;

/*
 * ganGpioIdUsers array keeps track of which GPIOs are currently in use
 * ensures they are not overwritten during this time. When a GPIO is available 
 * its entry in this array will be marked by a '0'; otherwise, it will 
 * contain the GPIO ID value of the current users. 
 */
static DALGpioSecAccessType *ganGpioIdUsers;

/*==========================================================================

      GENERIC DAL FUNCTIONS NOT CURRENTLY IMPLEMENTED IN THIS MODULE

==========================================================================*/

DALResult 
TLMM_DriverInit(TLMMDrvCtxt *pCtxt)
{
	return DAL_SUCCESS;
}

DALResult 
TLMM_DriverDeInit(TLMMDrvCtxt *pCtxt)
{
	return DAL_SUCCESS;
}

DALResult 
TLMM_PowerEvent(TLMMClientCtxt *pCtxt, DalPowerCmd PowerCmd, 
                    DalPowerDomain PowerDomain)
{
    return DAL_SUCCESS;
}

DALResult 
TLMM_Open(TLMMClientCtxt *pCtxt, uint32 dwaccessMode )
{
    return DAL_SUCCESS;
}

DALResult 
TLMM_Close(TLMMClientCtxt *pCtxt)
{
    return DAL_SUCCESS;
}

DALResult 
TLMM_Info(TLMMClientCtxt *pCtxt,DalDeviceInfo *pdeviceInfo, uint32 dwSize)
{
    return DAL_SUCCESS;
}

DALResult TLMM_PostInit( TLMMClientCtxt *pCtxt );

/*==========================================================================

                     APPLICATION PROGRAMMING INTERFACE

==========================================================================*/

void TLMM_InitPlatformIO(TLMMClientCtxt *pCtxt)
{
  char* pszPlatformIO = NULL;
  char* pszPlatformIOCfgs = NULL;
  DalDeviceHandle *hPlatformInfo = NULL;
  DalPlatformInfoPlatformInfoType PlatformInfo;
  DALResult eResult = DAL_ERROR;

  if( DAL_SUCCESS != DAL_DeviceAttach(DALDEVICEID_PLATFORMINFO, &hPlatformInfo) )
  {
    return;
  }

  if( hPlatformInfo != NULL )
  {
    DalPlatformInfo_GetPlatformInfo(hPlatformInfo, &PlatformInfo);

    /*
     * Check if we're on a Fusion subtype.
     */
    if( DalChipInfo_ChipFamily() == DALCHIPINFO_FAMILY_MDM9x45 )
    {
      if( (PlatformInfo.platform == DALPLATFORMINFO_TYPE_CDP) ||
          (PlatformInfo.platform == DALPLATFORMINFO_TYPE_MTP_MDM) ||
          (PlatformInfo.platform == DALPLATFORMINFO_TYPE_MTP_MSM) )
      {
        if( PlatformInfo.fusion == FALSE )
        {
          pszPlatformIO = "TLMMPlatformIO_MTPCDP";
          pszPlatformIOCfgs = "TLMMPlatformIO_MTPCDP_Config";
        }
        else
        {
          /*
           * Get Fusion subtypes.
           */
          if( PlatformInfo.subtype == 1 )
          {
            pszPlatformIO = "TLMMPlatformIO_MTPCDPFusion";
            pszPlatformIOCfgs = "TLMMPlatformIO_MTPCDPFusion_Config";
          }
          else
          {
            pszPlatformIO = "TLMMPlatformIO_MTPCDPT2Fusion";
            pszPlatformIOCfgs = "TLMMPlatformIO_MTPCDPT2Fusion_Config";
        }
      }
    }
    else
    {
        pszPlatformIO = "TLMMPlatformIO_MTPCDP";
        pszPlatformIOCfgs = "TLMMPlatformIO_MTPCDP_Config";
      }
      }
    else
    {
      if( PlatformInfo.platform == DALPLATFORMINFO_TYPE_CDP )
      {
        pszPlatformIO = "TLMMPlatformIO_CDP";
        pszPlatformIOCfgs = "TLMMPlatformIO_CDP_Config";
      }
      else if( PlatformInfo.platform == DALPLATFORMINFO_TYPE_MTP )
      {
        pszPlatformIO = "TLMMPlatformIO_MTP";
        pszPlatformIOCfgs = "TLMMPlatformIO_MTP_Config";
      }
      else if( PlatformInfo.platform == DALPLATFORMINFO_TYPE_QRD )
      {
        pszPlatformIO = "TLMMPlatformIO_QRD";
        pszPlatformIOCfgs = "TLMMPlatformIO_QRD_Config";
      }
      else if( PlatformInfo.platform == DALPLATFORMINFO_TYPE_FLUID )
      {
        pszPlatformIO = "TLMMPlatformIO_FLUID";
        pszPlatformIOCfgs = "TLMMPlatformIO_FLUID_Config";
      }
      else if( PlatformInfo.platform == DALPLATFORMINFO_TYPE_LIQUID )
      {
        pszPlatformIO = "TLMMPlatformIO_LiQUID";
        pszPlatformIOCfgs = "TLMMPlatformIO_LiQUID_Config";
      }
      else if( PlatformInfo.platform == DALPLATFORMINFO_TYPE_RCM )
      {
        pszPlatformIO = "TLMMPlatformIO_RCM";
        pszPlatformIOCfgs = "TLMMPlatformIO_RCM_Config";
      }
    }

    /*
     * If nothing found yet, attempt to use the device-config platform.
     */
    if( pszPlatformIO == NULL )
    {
      pszPlatformIO = "TLMMPlatformIO";
      pszPlatformIOCfgs = "TLMMPlatformIO_Config";
    }

    /*
     * Attempt to get the platform-specific GPIO configurations where supported.
     */
    eResult = DALSYS_GetDALPropertyHandleStr(pszPlatformIO, pCtxt->pDevCtxt->hPlatIO);
   
    if(DAL_SUCCESS == eResult)
    {
      bHasPlatIO = TRUE;

      eResult = DALSYS_GetDALPropertyHandleStr(pszPlatformIOCfgs, pCtxt->pDevCtxt->hPlatIOCfgs);

      if( DAL_SUCCESS == eResult )
      {
        bHasPlatIOCfgs = TRUE;
      }
    }
    else
    {
      /*
       * Platform not found, so default to MTP.
       */
      pszPlatformIO = "TLMMPlatformIO_MTP";
      pszPlatformIOCfgs = "TLMMPlatformIO_MTP_Config";

      eResult = DALSYS_GetDALPropertyHandleStr(pszPlatformIO, pCtxt->pDevCtxt->hPlatIO);

      if( DAL_SUCCESS == eResult )
      {
        bHasPlatIO = TRUE;

        eResult = DALSYS_GetDALPropertyHandleStr(pszPlatformIOCfgs, pCtxt->pDevCtxt->hPlatIOCfgs);

        if( DAL_SUCCESS == eResult )
        {
          bHasPlatIOCfgs = TRUE;
        }
      }
    }
  }

} /* TLMM_InitPlatformIO */



/*==========================================================================

  FUNCTION      TLMM_DeviceInit

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult TLMM_DeviceInit(TLMMClientCtxt *pCtxt)
{
  DALSYSPropertyVar tPropVar;
  HAL_tlmm_InitType tHALInit;
  DALResult eResult = DAL_ERROR;
  DalDeviceHandle * hHWIODevice = NULL;
  uint32 nPortIdx = 0, nHALPort = 0;
  uint32 nIdx = 0;

  /*
   * Check for shared memory.  If already initialized, get a pointer and 
   * we're done.
   */
  if( !pgtState )
  {
    memset((void *)&tHALInit, 0x0, sizeof(HAL_tlmm_InitType));

    for( nIdx=0; nIdx<DAL_TLMM_NUM_PORTS; nIdx++ )
    {
      gtHALPorts[nIdx].ePort = DAL_TLMM_NUM_PORTS;
    }

    /* 
     * Initialize the synchronization.
     */
    if(DAL_SUCCESS != DALSYS_SyncCreate(
                                      DALSYS_SYNC_ATTR_RESOURCE,
                                      &(pCtxt->pDevCtxt->hTlmmSync),
                                      &(pCtxt->pDevCtxt->hTlmmSyncObj)))
    {
      DALSYS_LogEvent(pCtxt->pDevCtxt->DevId, 
                        DALSYS_LOGEVENT_FATAL_ERROR, 
                        "TLMM_DeviceInit: Failed to initialize sychronization.");
      return(DAL_ERROR);
    }

    /* 
     * Ensure synchronization for critical initialization parameters.
     */
    (void)DALSYS_SyncEnter(pCtxt->pDevCtxt->hTlmmSync);

    /* 
     * Attempt to retrieve a handle to the TLMM properties file.
     */
    eResult = DALSYS_GetDALPropertyHandle(pCtxt->pDevCtxt->DevId, pCtxt->pDevCtxt->hProp);

    if( DAL_SUCCESS != eResult )
    {
      DALSYS_LogEvent(pCtxt->pDevCtxt->DevId, 
                  DALSYS_LOGEVENT_FATAL_ERROR, 
                  "TLMM_DeviceInit: Failed to retrieve properties handle.");

      (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);
      return(DAL_ERROR);
    }

    /* 
     * Attempt to retrieve the total supported GPIO for the current target.
     */
    eResult = DALSYS_GetPropertyValue( pCtxt->pDevCtxt->hProp, "tlmm_total_gpio", 0, &tPropVar );

    if( DAL_SUCCESS != eResult )
    {
      DALSYS_LogEvent(pCtxt->pDevCtxt->DevId, 
                  DALSYS_LOGEVENT_FATAL_ERROR, 
                  "TLMM_DeviceInit: Failed to retrieve number of GPIO.");

      (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);
      return(DAL_ERROR);
    }

    tHALInit.nNumGpio = gnTotalGPIO = tPropVar.Val.dwVal;

    /* 
     * Allocate an array for GPIO user tracking.
     */
    if(DAL_SUCCESS != DALSYS_Malloc((gnTotalGPIO*sizeof(DALGpioSecAccessType)), (void**)&ganGpioIdUsers))
    {
      DALSYS_LogEvent(pCtxt->pDevCtxt->DevId, 
                  DALSYS_LOGEVENT_FATAL_ERROR, 
                  "TLMM_DeviceInit: Failed to retrieve number of GPIO.");

      (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);
      return(DAL_ERROR);
    }
 
    memset((void *)ganGpioIdUsers, NULL, gnTotalGPIO*sizeof(DALGpioSecAccessType));

    /* 
     * Attempt to retrieve the base address for TLMM.
     */
    eResult = DALSYS_GetPropertyValue( pCtxt->pDevCtxt->hProp, "tlmm_base", 0, &tPropVar );

    if( DAL_SUCCESS != eResult )
    {
      DALSYS_LogEvent(pCtxt->pDevCtxt->DevId, 
                  DALSYS_LOGEVENT_FATAL_ERROR, 
                  "TLMM_DeviceInit: Failed to retrieve the base address.");

      (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);
      return(DAL_ERROR);
    }

    TLMM_InitPlatformIO(pCtxt);

    /* 
     * Get a virtual or physical base depending on the memory mapping.
     */
    eResult = DAL_DeviceAttach(DALDEVICEID_HWIO, &hHWIODevice);

    if((DAL_SUCCESS != eResult) || (hHWIODevice == NULL))
    {
      DALSYS_LogEvent(pCtxt->pDevCtxt->DevId, 
                  DALSYS_LOGEVENT_FATAL_ERROR, 
                  "TLMM_DeviceInit: Failed to attach to HWIO DAL Device.");

      (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);
      return(DAL_ERROR);
    }

    eResult = DalHWIO_MapRegionByAddress(hHWIODevice, 
                                         (uint8 *)tPropVar.Val.dwVal, 
                                         (uint8 **)&tHALInit.nBaseAddress);

    if( DAL_SUCCESS != eResult )
    {
      DALSYS_LogEvent(pCtxt->pDevCtxt->DevId, 
                  DALSYS_LOGEVENT_FATAL_ERROR, 
                  "TLMM_DeviceInit: Failed to map base address.");

      (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);
      return(DAL_ERROR);
    }


    /* 
     * Get the TLMM Base offset for GPIO.
     */
    eResult = DALSYS_GetPropertyValue( pCtxt->pDevCtxt->hProp, "tlmm_offset", 0, &tPropVar );

    if( DAL_SUCCESS != eResult )
    {
      DALSYS_LogEvent(pCtxt->pDevCtxt->DevId, 
                  DALSYS_LOGEVENT_FATAL_ERROR, 
                  "TLMM_DeviceInit: Failed to retrieve the register base offset.");

      (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);
      return(DAL_ERROR);
    }

    tHALInit.nBaseAddress += (uint32)tPropVar.Val.dwVal;


    /* 
     * Attempt to retrieve any port configuration structures.
     */
    eResult = DALSYS_GetPropertyValue( pCtxt->pDevCtxt->hProp, "tlmm_ports", 0, &tPropVar );

    if( DAL_SUCCESS != eResult )
    {
      DALSYS_LogEvent(pCtxt->pDevCtxt->DevId, 
                  DALSYS_LOGEVENT_WARNING, 
                  "TLMM_DeviceInit:  No TLMM port configurations found on this device.");
    }
    else
    {
      while( tPropVar.Val.pdwVal[nPortIdx] != 0xFFFFFFFF )
      {
        nHALPort = tPropVar.Val.pdwVal[nPortIdx++];

        gtHALPorts[nHALPort].ePort = nHALPort;
        gtHALPorts[nHALPort].nOffset = tPropVar.Val.pdwVal[nPortIdx++];
        gtHALPorts[nHALPort].nMask = tPropVar.Val.pdwVal[nPortIdx++];
        gtHALPorts[nHALPort].nEnableValue = tPropVar.Val.pdwVal[nPortIdx++];
        gtHALPorts[nHALPort].nDisableValue = tPropVar.Val.pdwVal[nPortIdx++];
        gnTotalPorts++;
      }
    }

    /* 
     * Initialize the HAL interface.
     */
    HAL_tlmm_Init(&tHALInit);


    /* 
     * Try to allocate or attach to shared memory.
     */
    eResult = TLMM_PostInit(pCtxt);

    if((DAL_SUCCESS != eResult) || (!pgtState))
    {  
      DALSYS_LogEvent(pCtxt->pDevCtxt->DevId, 
                  DALSYS_LOGEVENT_FATAL_ERROR, 
                  "TLMM_DeviceInit: Failed to initialize shared memory data structure.");

      (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);
      return(DAL_ERROR);
    }

    (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);

  }

  return(DAL_SUCCESS);

} /* TLMM_DeviceInit */


/*==========================================================================

  FUNCTION      TLMM_PostInit

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_PostInit( TLMMClientCtxt *pCtxt )
{

  if(DAL_SUCCESS == DALGLBCTXT_Init())
  {
    /* Initialize or retrieve a pointer to shared memory. */
    if( DAL_SUCCESS == DALGLBCTXT_AllocCtxt("DALTLMM",
                                 (sizeof(DALTLMMStateType)+ 
                                  (sizeof(DALTLMMStateGPIOType)*gnTotalGPIO)), 
                                  DALGLBCTXT_LOCK_TYPE_SPINLOCK, 
                                 (void **)&pgtState))
    {
      /* 
       * If shared structure was not alrady initialized.
       */

      if( pgtState )
      {
        /* 
         * Ensure local driver has initialized.
         */

        if( !pgtState->tFlags.bDriverInited )
        {
          DALTLMMState_Init(pCtxt);
        }
   
        pgtState->tFlags.bSMInited = TRUE;
        return(DAL_SUCCESS);
      }
      else
      {
        /* 
         * Pointer to data structure is NULL.
         */
        return(DAL_ERROR);
      }
    }
  }

  return(DAL_ERROR);

} /* TLMM_PostInit */


/*==========================================================================

  FUNCTION      TLMM_DeviceDeInit

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult TLMM_DeviceDeInit
(
  TLMMClientCtxt *pCtxt
)
{
  return DAL_SUCCESS;

} /* TLMM_DeviceDeInit */


/*==========================================================================

  FUNCTION      TLMM_ConfigGpioGroupInternal

  DESCRIPTION   Internal function to configure a group of GPIO.  This API 
                handles GPIO IDs to ensure incoming configurations do not 
                overwrite a current client's GPIO settings.

==========================================================================*/ 

static DALResult TLMM_ConfigGpioGroupInternal
(
  TLMMClientCtxt*     pCtxt,
  DALGpioEnableType   eEnable,
  DALGpioSignalType*  eGpioGroup,
  uint32              nSize,
  DALGpioIdType       nGpioId
)
{
  DALResult eResult = DAL_ERROR;
  uint32 nIdx;
  DALGpioSignalType nCfg;
  uint32 nGpioNumber;

  if( eGpioGroup == NULL )
  {
    return(DAL_ERROR);
  }

  /*
   * Validate input.  Ensure that there is no one stepping on a GPIO in use. 
   * nGpioId users get priority over the GPIOs.  If more than one nGpioId exists 
   * for the same GPIO, access is granted to the first to configure. 
   */
  for(nIdx = 0; nIdx < nSize; nIdx++)
  {
    nGpioNumber = HAL_GPIO_NUMBER(eGpioGroup[nIdx]);
    if( (nGpioNumber >= gnTotalGPIO) ||
        (nGpioId != ganGpioIdUsers[nGpioNumber].nGpioKey) )
    {
      return(DAL_ERROR);
    }
  }

  (void)DALSYS_SyncEnter(pCtxt->pDevCtxt->hTlmmSync);

  switch(eEnable)
  {
    case DAL_TLMM_GPIO_ENABLE:

      for( nIdx = 0; nIdx < nSize; nIdx++ )
      {
          /* 
           * Program the hardware.
           */
        nGpioNumber = HAL_GPIO_NUMBER(eGpioGroup[nIdx]);
          HAL_tlmm_ConfigGpio(eGpioGroup[nIdx]);  
          pgtState->ptGPIO[nGpioNumber].tAttr.nActive = 1;
        }
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_GPIO_DISABLE:

      for( nIdx = 0; nIdx < nSize; nIdx++ )
      {
          /*
           * Convert the configuration to macro format.
           */
        nGpioNumber = HAL_GPIO_NUMBER(eGpioGroup[nIdx]);
          nCfg = DALTLMMState_StructToMacro(nGpioNumber,
                                            &pgtState->ptGPIO[nGpioNumber].tInactive);

          /* 
           * Write any low-power output values.
           */

          TLMM_GpioOut(pCtxt,
                     eGpioGroup[nIdx],
                     (DALGpioValueType)pgtState->ptGPIO[nGpioNumber].tAttr.nSleepVal);

          /* 
           * Program the hardware.
           */

          HAL_tlmm_ConfigGpio(nCfg);
          pgtState->ptGPIO[nGpioNumber].tAttr.nActive = 0;
      }
      eResult = DAL_SUCCESS;
      break;

    default:
      eResult = DAL_ERROR;
      break;

  }

  (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);

  return(eResult);

} /* TLMM_ConfigGpioGroup */


/*==========================================================================

  FUNCTION      TLMM_ConfigGpioGroup

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult TLMM_ConfigGpioGroup
(
  TLMMClientCtxt*     pCtxt,
  DALGpioEnableType   eEnable,
  DALGpioSignalType*  eGpioGroup,
  uint32              nSize
)
{
  return(TLMM_ConfigGpioGroupInternal(pCtxt, eEnable, eGpioGroup, nSize, NULL));

} /* TLMM_ConfigGpioGroup */


/*==========================================================================

  FUNCTION      TLMM_ConfigGpioInternal

  DESCRIPTION   Internal function to configure a GPIO.  This API handles
                GPIO IDs to ensure incoming configurations do not overwrite
                a current client's setting.

==========================================================================*/ 

static DALResult TLMM_ConfigGpioInternal
(
  TLMMClientCtxt*     pCtxt,   
  DALGpioSignalType   eGpioConfig,
  DALGpioEnableType   eEnable,
  DALGpioIdType       nGpioId
)
{
  return(TLMM_ConfigGpioGroupInternal(pCtxt, eEnable, &eGpioConfig, 1, nGpioId));

} /* TLMM_ConfigGpioInternal */


/*==========================================================================

  FUNCTION      TLMM_ConfigGpio

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult TLMM_ConfigGpio
(
  TLMMClientCtxt*     pCtxt,   
  DALGpioSignalType   eGpioConfig,
  DALGpioEnableType   eEnable
)
{
  return(TLMM_ConfigGpioInternal(pCtxt, eGpioConfig, eEnable, NULL));

} /* TLMM_ConfigGpio */


/*==========================================================================

  FUNCTION      TLMM_ConfigGpiosForSleep

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult  TLMM_ConfigGpiosForSleep
( 
  TLMMClientCtxt*         pCtxt,   
  DALGpioSleepConfigType  eGpioSleepConfig
) 
{
  return DAL_ERROR;

} /* TLMM_ConfigGpiosForSleep */


/*==========================================================================

  FUNCTION      TLMM_RestoreGpiosFromSleep

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult  TLMM_RestoreGpiosFromSleep
( 
  TLMMClientCtxt*         pCtxt,   
  DALGpioSleepConfigType  eGpioSleepConfig
) 
{
  return(DAL_ERROR);

} /* TLMM_RestoreGpiosFromSleep */
    
    
/*==========================================================================
    
  FUNCTION      TLMM_SwitchGpioIntOwnership
    
  DESCRIPTION   See DALTLMM.h
    
==========================================================================*/ 
    
DALResult TLMM_SwitchGpioIntOwnership
(   
  TLMMClientCtxt*  pCtxt,   
  uint32           owner
) 
{
  return(DAL_ERROR);

} /* TLMM_SwitchGpioIntOwnership */


/*==========================================================================

  FUNCTION      TLMM_SetPort

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult  TLMM_SetPort
( 
  TLMMClientCtxt*  pCtxt,   
  DALGpioPortType  ePort,   
  uint32           nValue
)
{
  DALResult eResult = DAL_ERROR;

  if( (ePort < DAL_TLMM_NUM_PORTS) && (gtHALPorts[ePort].ePort != DAL_TLMM_NUM_PORTS) )
  {
    (void)DALSYS_SyncEnter(pCtxt->pDevCtxt->hTlmmSync);

    if( nValue != 0 )
    {
      HAL_tlmm_SetPort(gtHALPorts[ePort].nOffset, 
                       gtHALPorts[ePort].nMask, 
                       gtHALPorts[ePort].nEnableValue);
    }
    else
    {
      HAL_tlmm_SetPort(gtHALPorts[ePort].nOffset, 
                       gtHALPorts[ePort].nMask, 
                       gtHALPorts[ePort].nDisableValue);
    }
    eResult = DAL_SUCCESS;

    (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);
  }

  return(eResult);

} /* TLMM_SetPort */


/*==========================================================================

  FUNCTION      TLMM_RequestGpio

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult  TLMM_RequestGpio
( 
  TLMMClientCtxt*    pCtxt,   
  DALGpioSignalType  eGpioConfig
) 
{
  /* 
   * Currently, request/release APIs are not supported.
   */

  return(DAL_ERROR);

} /* TLMM_RequestGpio */


/*==========================================================================

  FUNCTION      TLMM_ReleaseGpio

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult  TLMM_ReleaseGpio
( 
  TLMMClientCtxt*     pCtxt,   
  DALGpioSignalType   eGpioConfig
)
{
  /* 
   * Currently, request/release APIs are not supported.
   */

  return(DAL_ERROR);

} /* TLMM_ReleaseGpio */



/*==========================================================================

  FUNCTION      TLMM_TristateGpio

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult  TLMM_TristateGpio
( 
  TLMMClientCtxt*    pCtxt,   
  DALGpioSignalType  eGpioConfig
)
{
  uint32 nGpioNumber = HAL_GPIO_NUMBER(eGpioConfig);

  if( nGpioNumber < gnTotalGPIO )
  {
    (void)DALSYS_SyncEnter(pCtxt->pDevCtxt->hTlmmSync);

    HAL_tlmm_SetDirection(eGpioConfig, 
                          (HAL_tlmm_DirType)HAL_DIR_VAL(eGpioConfig));

    (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);
  }
  return(DAL_SUCCESS);

} /* TLMM_TristateGpio */


/*==========================================================================

  FUNCTION      TLMM_TristateGpioGroup

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult  TLMM_TristateGpioGroup
( 
  TLMMClientCtxt*    pCtxt,   
  DALGpioSignalType* aeGpioGroup,
  uint32             nSize
)
{
  DALResult eResult = DAL_ERROR;
  uint32 nIdx;

  if(aeGpioGroup)
  {
    (void)DALSYS_SyncEnter(pCtxt->pDevCtxt->hTlmmSync);

    for( nIdx = 0; nIdx < nSize; ++nIdx )
    {
      TLMM_TristateGpio(pCtxt, aeGpioGroup[nIdx]);
    }

    (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);
  }
  return(eResult);

} /* TLMM_TristateGpioGroup */


/*==========================================================================

  FUNCTION      TLMM_GetGpioNumber

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult  TLMM_GetGpioNumber
( 
  TLMMClientCtxt*    pCtxt,
  DALGpioSignalType  eGpioConfig,
  uint32*            nGpioNumber
) 
{
  *nGpioNumber = (uint32)HAL_GPIO_NUMBER(eGpioConfig);
  return(DAL_SUCCESS);

} /* TLMM_GetGpioNumber */


/*==========================================================================

  FUNCTION      TLMM_GpioIn

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult  TLMM_GpioIn
( 
  TLMMClientCtxt*    pCtxt,   
  DALGpioSignalType  eGpioConfig,  
  DALGpioValueType*  eValue
) 
{
  DALResult eResult = DAL_ERROR;
  boolean bHalRetVal = FALSE;
  uint32 nGpioNumber = HAL_GPIO_NUMBER(eGpioConfig);

  if( nGpioNumber < gnTotalGPIO )
  {
    bHalRetVal = HAL_tlmm_ReadGpio(eGpioConfig);
    eResult = DAL_SUCCESS;
  }
  
  if( !bHalRetVal  )
  {  
    *eValue = DAL_GPIO_LOW_VALUE;
  }
  else
  {  
    *eValue = DAL_GPIO_HIGH_VALUE;
  }

  return(eResult);

} /* TLMM_GpioIn */


/*==========================================================================

  FUNCTION      TLMM_GpioOutInternal

  DESCRIPTION   Internal API to output a value HIGH/LOW to a GPIO.  This
                API handles GPIO ID contention to ensure that the owner
                of a GPIO ID does not have it's GPIO(s) values changed
                by another user.

==========================================================================*/ 

static DALResult  TLMM_GpioOutGroupInternal
( 
  TLMMClientCtxt*    pCtxt,   
  DALGpioSignalType* aeGpioConfigGroup,  
  uint32             nSize,
  DALGpioValueType   eValue,
  DALGpioIdType      nGpioId
) 
{
  DALResult eResult = DAL_ERROR;
  uint32 nIdx, nGpioNumber;

  if( aeGpioConfigGroup )
  {
    /*
     * Validate input.  Ensure that there is no one stepping on a GPIO in use. 
     * nGpioId users get priority over the GPIOs.  If more than one nGpioId exists 
     * for the same GPIO, access is granted to the first to configure. 
     */
    for(nIdx = 0; nIdx < nSize; nIdx++)
    {
      nGpioNumber = HAL_GPIO_NUMBER(aeGpioConfigGroup[nIdx]);
      if( (nGpioNumber >= gnTotalGPIO) || 
          (nGpioId != ganGpioIdUsers[nGpioNumber].nGpioKey) )
      {
        return(DAL_ERROR);
      }
    }

    (void)DALSYS_SyncEnter(pCtxt->pDevCtxt->hTlmmSync);

    HAL_tlmm_WriteGpioGroup((uint32*)aeGpioConfigGroup,
                            (uint16)nSize,
                           (boolean)eValue);

    (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);

    eResult = DAL_SUCCESS;
  }
  return(eResult);

} /* TLMM_GpioOutGroupInternal */


/*==========================================================================

  FUNCTION      TLMM_GpioOutGroup

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult  TLMM_GpioOutGroup
( 
  TLMMClientCtxt*    pCtxt,   
  DALGpioSignalType* aeGpioConfigGroup,  
  uint32             nSize,
  DALGpioValueType   eValue
)
{
  return(TLMM_GpioOutGroupInternal(pCtxt, 
                                   aeGpioConfigGroup, 
                                   nSize, 
                                   eValue, 
                                   (DALGpioValueType)NULL));

} /* TLMM_GpioOutGroup */


/*==========================================================================

  FUNCTION      TLMM_GpioOutInternal

  DESCRIPTION   Internal API to output a value HIGH/LOW to a GPIO.  This
                API handles GPIO ID contention to ensure that the owner
                of a GPIO ID does not have it's GPIO(s) values changed
                by another user.

==========================================================================*/ 

static DALResult  TLMM_GpioOutInternal
( 
  TLMMClientCtxt*    pCtxt,   
  DALGpioSignalType  eGpioConfig,   
  DALGpioValueType   eValue,
  DALGpioIdType      nGpioId
) 
  { 
  return(TLMM_GpioOutGroupInternal(pCtxt, &eGpioConfig, 1, eValue, nGpioId));

} /* TLMM_GpioOutInternal */


/*==========================================================================

  FUNCTION      TLMM_GpioOut

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult  TLMM_GpioOut
( 
  TLMMClientCtxt*    pCtxt,   
  DALGpioSignalType  eGpioConfig,   
  DALGpioValueType   eValue
) 
{
  return(TLMM_GpioOutInternal(pCtxt, eGpioConfig, eValue, (DALGpioValueType)NULL));

} /* TLMM_GpioOut */


/*==========================================================================

  FUNCTION      TLMM_GetGpioOwner

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult TLMM_GetGpioOwner
( 
  TLMMClientCtxt*   pCtxt, 
  uint32            nGpioNumber, 
  DALGpioOwnerType* eOwner
) 
{
  DALResult eResult = DAL_ERROR;

  if( nGpioNumber < gnTotalGPIO )
  {
    /*
     * Return 0 for the master processor since this is all that is supported.
     */

    *eOwner = (DALGpioOwnerType)0;
    eResult = DAL_SUCCESS;
  }
  return(eResult);

} /* TLMM_GetGpioOwner */


/*==========================================================================

  FUNCTION      TLMM_GetCurrentConfig

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult TLMM_GetCurrentConfig
(
  TLMMClientCtxt*    pCtxt,
  uint32             nGpioNumber,
  DALGpioSignalType* eGpioConfig
)
{
  DALResult eResult = DAL_ERROR;
  HAL_tlmm_GpioType tTempCurrent;

  if( nGpioNumber < gnTotalGPIO )
  {
    /* 
     * Read the current configuration.
     */

    HAL_tlmm_GetConfig(nGpioNumber, &tTempCurrent);

    /* 
     * Convert to DALGpioSignalType format.
     */

    *eGpioConfig = DALTLMMState_StructToMacro(nGpioNumber, &tTempCurrent);
    eResult = DAL_SUCCESS;
  }
  return(eResult);

} /* TLMM_GetCurrentConfig */


/*==========================================================================

  FUNCTION      TLMM_GetPinStatus

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult TLMM_GetPinStatus
(
  TLMMClientCtxt*     pCtxt,
  uint32              nGpioNumber,
  DALGpioStatusType*  eStatus
)
{
  DALResult eResult = DAL_ERROR;

  if( nGpioNumber < gnTotalGPIO )
  {
    *eStatus = (DALGpioStatusType)pgtState->ptGPIO[nGpioNumber].tAttr.nActive;
    eResult = DAL_SUCCESS;
  }
  return(eResult);

} /* TLMM_GetPinStatus */


/*==========================================================================

  FUNCTION      TLMM_SetInactiveConfig

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_SetInactiveConfig
(
  TLMMClientCtxt*   pCtxt,
  uint32            nGpioNumber,
  DALGpioSignalType nInactiveConfig
)
{
  if( nGpioNumber < gnTotalGPIO )
  {
    /* 
     * Update the low-power configuration.
     */

    DALTLMMState_MacroToStruct(nInactiveConfig, 
                               &pgtState->ptGPIO[nGpioNumber].tInactive);

    /* 
     * Update the output value for the low-power configuration.
     */

    pgtState->ptGPIO[nGpioNumber].tAttr.nSleepVal = 
                               DAL_GPIO_GET_OUTVAL(nInactiveConfig);

    return(DAL_SUCCESS);
  }

  return(DAL_ERROR);

} /* TLMM_SetInactiveConfig */


/*==========================================================================

  FUNCTION      TLMM_GetInactiveConfig

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_GetInactiveConfig
(
  TLMMClientCtxt*    pCtxt,
  uint32             nGpioNumber,
  DALGpioSignalType* eGpioConfig
)
{
  DALTLMMStateGPIOType* ptState = NULL;

  if( nGpioNumber < gnTotalGPIO )
  {
    ptState = &pgtState->ptGPIO[nGpioNumber];

    /* 
     * Need to explicitly convert because of added nSleepVal.
     */

    *eGpioConfig = 
      (DALGpioSignalType)DAL_GPIO_CFG_OUT(nGpioNumber,
                                          ptState->tInactive.nFunc,
                                          ptState->tInactive.nDir,
                                          ptState->tInactive.nPull,
                                          ptState->tInactive.nDrive,
                                          ptState->tAttr.nSleepVal);

    return(DAL_SUCCESS);
  }
  return(DAL_ERROR);

} /* TLMM_GetInactiveConfig */


/*==========================================================================

  FUNCTION      TLMM_LockGpio

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_LockGpio
(
  TLMMClientCtxt*    pCtxt,
  DALGpioClientType  eClient,
  uint32             nGpioNumber
)
{
  return(DAL_ERROR);

} /* TLMM_LockGpio */


/*==========================================================================

  FUNCTION      TLMM_UnlockGpio

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_UnlockGpio
(
  TLMMClientCtxt*    pCtxt,
  DALGpioClientType  client,
  uint32             gpio_number
)
{
  return(DAL_ERROR);

} /* TLMM_LockGpio */


/*==========================================================================

  FUNCTION      TLMM_IsGpioLocked

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_IsGpioLocked
(
  TLMMClientCtxt*    pCtxt,
  uint32             gpio_number,
  DALGpioLockType*   lock
)
{
  return(DAL_ERROR);

} /* TLMM_IsGpioLocked */


/*==========================================================================

  FUNCTION      TLMM_InterruptConfig

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_BypassSleep
(
  TLMMClientCtxt*    pCtxt,
  uint32             gpio_number,
  DALGpioEnableType  enable
)
{
  /* 
   * GPIO sleep management is no longer a part of TLMM.
   */

  return(DAL_ERROR);

} /* TLMM_BypassSleep */


/*==========================================================================

  FUNCTION      TLMM_AttachRemote

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_AttachRemote
( 
  TLMMClientCtxt*  pCtxt, 
  uint32           processor
)
{
  DALResult eResult = DAL_ERROR;

  return(eResult);

} /* TLMM_AttachRemote */


/*==========================================================================

  FUNCTION      TLMM_GetOutput

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_GetOutput
( 
  TLMMClientCtxt*   pCtxt, 
  uint32            nGpioNumber, 
  DALGpioValueType* nValue
)
{
  if( nGpioNumber < gnTotalGPIO )
  {
    if(HAL_tlmm_GetOutput(nGpioNumber)) 
      *nValue = DAL_GPIO_HIGH_VALUE;
    else
      *nValue = DAL_GPIO_LOW_VALUE;
    return(DAL_SUCCESS);
  }
  return(DAL_ERROR);

} /* TLMM_GetOutput */


/*==========================================================================

  FUNCTION      TLMM_GetGpioId

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_GetGpioId
( 
  TLMMClientCtxt*   pCtxt,
  const char*       pszGpioStr,
  DALGpioIdType*    pnGpioId
)
{
  DALSYSPropertyVar tPropVar;
  uint32 nGpioNumber = 0;
  DALGpioIdType nTempGpioId = 0;

  if(pszGpioStr == NULL ||
      pnGpioId == NULL)
  {
    return(DAL_ERROR);
  }

  (void)DALSYS_SyncEnter(pCtxt->pDevCtxt->hTlmmSync); 
  if( bHasPlatIO )
  {
    if(DAL_SUCCESS == 
       DALSYS_GetPropertyValue( pCtxt->pDevCtxt->hPlatIO, 
                                pszGpioStr, 
                                0, 
                                &tPropVar ))
    {
      nTempGpioId = (DALGpioIdType)tPropVar.Val.pStruct;

      nGpioNumber = ((TLMMGpioIdType*)nTempGpioId)->nGpioNumber;

      if( nGpioNumber >= gnTotalGPIO )
      {
        (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);
        return(DAL_ERROR);
      }

      if( (ganGpioIdUsers[nGpioNumber].pGpioModes == NULL) &&
          (bHasPlatIOCfgs) )
      {
          if(DAL_SUCCESS == 
           DALSYS_GetPropertyValue( pCtxt->pDevCtxt->hPlatIOCfgs, 
                                    pszGpioStr, 
                                    0, 
                                    &tPropVar ))
          {
            ganGpioIdUsers[nGpioNumber].pszGpioStr = (char*)pszGpioStr;
            ganGpioIdUsers[nGpioNumber].pGpioModes = (TLMMGpioIdCfgType*)tPropVar.Val.pStruct;
            ganGpioIdUsers[nGpioNumber].nNumModes = (uint32)(tPropVar.dwLen/sizeof(TLMMGpioIdCfgType));
          }
      }


      /*
       * Only return the GPIO ID if it is not already in use by another entity.
       */
      if(ganGpioIdUsers[nGpioNumber].nGpioKey == 0)
      {
        ganGpioIdUsers[nGpioNumber].nGpioKey = DAL_GPIO_SET_KEY(nGpioNumber, nTempGpioId);
        *pnGpioId = ganGpioIdUsers[nGpioNumber].nGpioKey;
        ganGpioIdUsers[nGpioNumber].nGpioId = nTempGpioId;
        ganGpioIdUsers[nGpioNumber].pszGpioStr = (char*)pszGpioStr;

        (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);

      return(DAL_SUCCESS);
    }
      else
      {
        *pnGpioId = NULL;
        DALSYS_LogEvent(pCtxt->pDevCtxt->DevId, 
                        DALSYS_LOGEVENT_ERROR, 
                        "TLMM_GetGpioId:  GPIO ID for GPIO %d is already allocated to another entity.",
                        nGpioNumber);
      }
    }
  }
  (void)DALSYS_SyncLeave(pCtxt->pDevCtxt->hTlmmSync);

  return(DAL_ERROR);

} /* TLMM_GetGpioId */


/*==========================================================================

  FUNCTION      TLMM_ConfigGpioId

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_ConfigGpioId
( 
  TLMMClientCtxt*          pCtxt, 
  DALGpioIdType            nGpioId,
  DalTlmm_GpioConfigIdType *pUserSettings
)
{
  uint32 nGpioNumber = DAL_GPIO_GET_PIN(nGpioId);
  TLMMGpioIdType* pGpioId = NULL;

  if(nGpioNumber < gnTotalGPIO)
  {
    if(nGpioId == ganGpioIdUsers[nGpioNumber].nGpioKey)
    {
      pGpioId = (TLMMGpioIdType*)ganGpioIdUsers[nGpioNumber].nGpioId;

  if( (pGpioId != NULL) && (pUserSettings != NULL) )
  {
    TLMM_ConfigGpioInternal(pCtxt, 
                    DAL_GPIO_CFG(pGpioId->nGpioNumber,
                                 pGpioId->nFunctionSelect,
                                 pUserSettings->eDirection,
                                 pUserSettings->ePull,
                                 pUserSettings->eDriveStrength),
                            DAL_TLMM_GPIO_ENABLE,
                            nGpioId);
    return(DAL_SUCCESS);
  }
    }
  }

  return(DAL_ERROR);

} /* TLMM_ConfigGpioId */


/*==========================================================================

  FUNCTION      TLMM_ConfigGpioIdInactive

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_ConfigGpioIdInactive
( 
  TLMMClientCtxt*          pCtxt, 
  DALGpioIdType            nGpioId
)
{
  uint32 nGpioNumber = DAL_GPIO_GET_PIN(nGpioId);
  TLMMGpioIdType* pGpioId = NULL;

  if(nGpioNumber < gnTotalGPIO)
  {
    if(nGpioId == ganGpioIdUsers[nGpioNumber].nGpioKey)
    {
      pGpioId = (TLMMGpioIdType*)ganGpioIdUsers[nGpioNumber].nGpioId;

  if( pGpioId != NULL )
  {
    /*
     * We only care about the GPIO number since all other 
     * parameters are extracted from the global BSP. 
    */
    TLMM_ConfigGpioInternal(pCtxt, 
                    DAL_GPIO_CFG(pGpioId->nGpioNumber,
                                 0,
                                 0,
                                 0,
                                 0),
                            DAL_TLMM_GPIO_DISABLE,
                            nGpioId);

    return(DAL_SUCCESS);
  }
    }
  }

  return(DAL_ERROR);

} /* TLMM_ConfigGpioIdInactive */


/*==========================================================================

  FUNCTION      TLMM_GpioIdOut

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_GpioIdOut
(
  TLMMClientCtxt*          pCtxt, 
  DALGpioIdType            nGpioId,
  DALGpioValueType         eGpioValue
)
{
  uint32 nGpioNumber = DAL_GPIO_GET_PIN(nGpioId);
  TLMMGpioIdType* pGpioId = NULL;

  if(nGpioNumber < gnTotalGPIO)
  {
    if(nGpioId == ganGpioIdUsers[nGpioNumber].nGpioKey)
    {
      pGpioId = (TLMMGpioIdType*)ganGpioIdUsers[nGpioNumber].nGpioId;
  if( pGpioId != NULL )
  {
    return TLMM_GpioOutInternal(pCtxt, DAL_GPIO_CFG(pGpioId->nGpioNumber,
                                     pGpioId->nFunctionSelect,
                                     DAL_GPIO_OUTPUT,
                                     DAL_GPIO_NO_PULL,
                                                    DAL_GPIO_2MA),
                                eGpioValue,
                                nGpioId);
  }
    }
  }
  return DAL_ERROR;

} /* TLMM_GpioIdOut */


/*==========================================================================

  FUNCTION      TLMM_GpioIdIn

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_GpioIdIn
(
  TLMMClientCtxt*          pCtxt, 
  DALGpioIdType            nGpioId,
  DALGpioValueType         *eGpioValue
)
{
  uint32 nGpioNumber = DAL_GPIO_GET_PIN(nGpioId);
  TLMMGpioIdType* pGpioId = NULL;
  if(nGpioNumber < gnTotalGPIO)
  {
    if(nGpioId == ganGpioIdUsers[nGpioNumber].nGpioKey)
    {
      pGpioId = (TLMMGpioIdType*)ganGpioIdUsers[nGpioNumber].nGpioId;
  if( pGpioId != NULL )
  {
    return TLMM_GpioIn(pCtxt, 
                       DAL_GPIO_CFG(pGpioId->nGpioNumber,
                                    pGpioId->nFunctionSelect,
                                    DAL_GPIO_INPUT,
                                    DAL_GPIO_NO_PULL,
                                    DAL_GPIO_2MA),
                       eGpioValue);
  }
    }
  }
  return DAL_ERROR;

} /* TLMM_GpioIdIn */


/*==========================================================================

  FUNCTION      TLMM_ReleaseGpioId

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_ReleaseGpioId
(
  TLMMClientCtxt*          pCtxt, 
  DALGpioIdType            nGpioId
)
{
  uint32 nGpioNumber = DAL_GPIO_GET_PIN(nGpioId);

  if(nGpioNumber < gnTotalGPIO)
  {
  /*
   * Only the holder of the GPIO ID for this GPIO number can 
   * unlock access to it for others to use. 
   */
    if( ganGpioIdUsers[nGpioNumber].nGpioKey == nGpioId )
  {
      ganGpioIdUsers[nGpioNumber].nGpioKey = NULL;
    return(DAL_SUCCESS);
  }
  }

  return(DAL_ERROR);
 
} /* TLMM_ReleaseGpioId */


/*==========================================================================

  FUNCTION      TLMM_SelectGpioIdMode

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_SelectGpioIdMode
(
  TLMMClientCtxt*           pCtxt, 
  DALGpioIdType             nGpioId,
  DalGpioModeType           eMode,
  DalTlmm_GpioConfigIdType* pUserSettings
)
{
  TLMMGpioIdType* pGpioId;
  uint32 nFunction;
  uint32 nGpioNumber = DAL_GPIO_GET_PIN(nGpioId);

  /*
   * Only the holder of the GPIO ID for this GPIO number can 
   * unlock access to it for others to use. 
   */
  if(nGpioNumber < gnTotalGPIO)
  {
    if( ganGpioIdUsers[nGpioNumber].nGpioKey == nGpioId )
  {
      pGpioId = (TLMMGpioIdType*)ganGpioIdUsers[nGpioNumber].nGpioId;
      nGpioNumber = pGpioId->nGpioNumber;
      nFunction = pGpioId->nFunctionSelect;

    if( eMode == DAL_GPIO_MODE_PRIMARY )
    {
      if( pUserSettings == NULL )
      {
        /*
         * Place the function select of this GPIO to its primary setting.
        */
        HAL_tlmm_SetFunction(nGpioNumber, nFunction);
      }
      else
      {
        /*
         * Program the entire configuration based on the user settings.
         */
        TLMM_ConfigGpioInternal(pCtxt, 
                                DAL_GPIO_CFG(nGpioNumber,
                                             nFunction,
                                             pUserSettings->eDirection,
                                             pUserSettings->ePull,
                                             pUserSettings->eDriveStrength),
                                DAL_TLMM_GPIO_ENABLE,
                                nGpioId);
      }

    }
    else
    {
      if( pUserSettings == NULL )
      {
        /*
         * Set to IO mode on this pin.
         */
        HAL_tlmm_SetFunction(nGpioNumber, 0);
      }
      else
      {
        /*
         * Program the user settings to use with IO mode.
         */
        TLMM_ConfigGpioInternal(pCtxt, 
                                DAL_GPIO_CFG(nGpioNumber,
                                             0,
                                             pUserSettings->eDirection,
                                             pUserSettings->ePull,
                                             pUserSettings->eDriveStrength),
                                DAL_TLMM_GPIO_ENABLE,
                                nGpioId);
      }
    }
      return(DAL_SUCCESS);
    }
  }

  return(DAL_ERROR);

} /* TLMM_SelectGpioIdMode */


/*==========================================================================

  FUNCTION      TLMM_GetGpioIdSettings

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_GetGpioIdSettings
(
  TLMMClientCtxt*            pCtxt,
  DALGpioIdType              nGpioId,
  DalTlmm_GpioIdSettingsType *pGpioSettings
)
{
  uint32 nGpioNumber = DAL_GPIO_GET_PIN(nGpioId);
  HAL_tlmm_GpioType GpioParams;

  if(pGpioSettings == NULL)
  {
    return(DAL_ERROR);
  }

  /*
   * If the GPIO number is valid we can retreive the current HW settings.
   */
  if(nGpioNumber < gnTotalGPIO)
  {
    pGpioSettings->nGpioNumber = nGpioNumber;

    /*
     * Call HAL to do a HW read and return the current settings.
     */
    HAL_tlmm_GetConfig(nGpioNumber, &GpioParams);

    pGpioSettings->nFunctionSelect = GpioParams.nFunc;
    pGpioSettings->Settings.eDirection = (DALGpioDirectionType)GpioParams.nDir;
    pGpioSettings->Settings.ePull = (DALGpioPullType)GpioParams.nPull;
    pGpioSettings->Settings.eDriveStrength = (DALGpioDriveType)GpioParams.nDrive;

    /*
     * Check for any output value being driven.
     */
    if(HAL_tlmm_GetOutput(nGpioNumber) == TRUE)
    {
      pGpioSettings->eOutputDrive = DAL_GPIO_HIGH_VALUE;
    }
    else
    {
      pGpioSettings->eOutputDrive = DAL_GPIO_LOW_VALUE;
    }

    return(DAL_SUCCESS);
  }
  else
  {
    return(DAL_ERROR);
  }

} /* TLMM_GetGpioIdSettings */


/*==========================================================================

  FUNCTION      TLMM_ConfigGpioIdModeIndexed

  DESCRIPTION   See DALTLMM.h

==========================================================================*/

DALResult TLMM_ConfigGpioIdModeIndexed
(
  TLMMClientCtxt* pCtxt,
  DALGpioIdType   nGpioId,
  uint32          nMode
)
{
  TLMMGpioIdType* pGpioId;
  uint32 nFunction;
  uint32 nGpioNumber = DAL_GPIO_GET_PIN(nGpioId);

  if(nGpioNumber < gnTotalGPIO)
  {
    if( ganGpioIdUsers[nGpioNumber].nGpioKey == nGpioId )
    {
      pGpioId = (TLMMGpioIdType*)ganGpioIdUsers[nGpioNumber].nGpioId;
      nGpioNumber = pGpioId->nGpioNumber;
      nFunction = pGpioId->nFunctionSelect;

      if((bHasPlatIOCfgs) && (ganGpioIdUsers[nGpioNumber].pGpioModes != NULL))
      {
        if(nMode < ganGpioIdUsers[nGpioNumber].nNumModes)
        {
          /*
           * If selected mode was low-power.
           */
          if (nMode == 0)
          {
            nFunction = 0;
          }

          /*
           * Program the entire configuration based on the user settings.
           */
          return TLMM_ConfigGpioInternal(pCtxt, 
              DAL_GPIO_CFG(nGpioNumber,
                nFunction,
                ganGpioIdUsers[nGpioNumber].pGpioModes[nMode].nDir,
                ganGpioIdUsers[nGpioNumber].pGpioModes[nMode].nPull,
                ganGpioIdUsers[nGpioNumber].pGpioModes[nMode].nDrive),
              DAL_TLMM_GPIO_ENABLE,
              nGpioId);
        }
      }
    }
  }

  return(DAL_ERROR);

}
