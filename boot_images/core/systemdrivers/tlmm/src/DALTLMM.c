/*===========================================================================

             M A I N   D A L   T L M M   D R I V E R   F I L E

DESCRIPTION

  Contains the majority of functionality for the DAL TLMM driver.  The
  API in this module MUST always remain target independent.


===========================================================================
             Copyright (c) 2010 - 2013 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/systemdrivers/tlmm/src/DALTLMM.c#1 $

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
#include <DDIChipInfo.h>
#include "DALTLMMState.h"
#include "DALTLMM.h"


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



/*==========================================================================

                     PROTOTYPES FOR THIS FILE

==========================================================================*/


DALResult TLMM_PostInit( TLMMClientCtxt *pCtxt );

/*==========================================================================

                     LOCAL DATA FOR THIS MODULE

==========================================================================*/
static boolean gbTLMMInitialized = FALSE;
uint32 gnTotalGPIO;
extern DALTLMMStateType* pgtState;
static DALTLMM_PortCfgType gtHALPorts[DAL_TLMM_NUM_PORTS];
static uint32 gnTotalPorts = 0;
static boolean gNonDalTlmmInit = FALSE;
uint32* pgLowPowerConfigs = NULL;
DALSYS_PROPERTY_HANDLE_DECLARE(hNonDalProps);

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


/*==========================================================================

                     APPLICATION PROGRAMMING INTERFACE

==========================================================================*/



/*==========================================================================

  FUNCTION      TLMM_DeviceInit

  DESCRIPTION   See DALTLMM.h

==========================================================================*/ 

DALResult TLMM_DeviceInit(TLMMClientCtxt *pCtxt)
{
  DALSYSPropertyVar tPropVar;
  HAL_tlmm_InitType tHALInit;
  DALResult eResult = DAL_ERROR;
  uint32 nPortIdx = 0, nHALPort = 0;
  uint32 nIdx = 0;
 
  if( !gbTLMMInitialized )
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
      /*
       * This is fatal.
       */
      return( DAL_ERROR );
    }

    /* 
     * Attempt to retrieve the total supported GPIO for the current target.
     */

    if( DalChipInfo_ChipFamily() == DALCHIPINFO_FAMILY_MSM8926 )
    {
      /* Special handling for MSM8926 configuration compatiblity with MSM8x26. */
      eResult = DALSYS_GetPropertyValue( pCtxt->pDevCtxt->hProp, "tlmm_total_gpio8926", 0, &tPropVar );
    }
    else
    {
      eResult = DALSYS_GetPropertyValue( pCtxt->pDevCtxt->hProp, "tlmm_total_gpio", 0, &tPropVar );
    }

    if( DAL_SUCCESS != eResult )
    {
      /*
       * This is fatal.
       */
      return( DAL_ERROR );
    }

    tHALInit.nNumGpio = gnTotalGPIO = tPropVar.Val.dwVal;

    /* 
     * Attempt to retrieve the base address for TLMM.
     */

    eResult = DALSYS_GetPropertyValue( pCtxt->pDevCtxt->hProp, "tlmm_base", 0, &tPropVar );

    if( DAL_SUCCESS != eResult )
    {
      /*
       * This is fatal.
       */
      return( DAL_ERROR );
    }

    tHALInit.nBaseAddress = (uint32)tPropVar.Val.dwVal;

    /* 
     * Get the TLMM Base offset for GPIO.
     */

    eResult = DALSYS_GetPropertyValue( pCtxt->pDevCtxt->hProp, "tlmm_offset", 0, &tPropVar );

    if( DAL_SUCCESS != eResult )
    {
      DALSYS_LogEvent(pCtxt->pDevCtxt->DevId, 
                  DALSYS_LOGEVENT_FATAL_ERROR, 
                  "TLMM_DeviceInit: Failed to retrieve the register base offset.");
    }

    tHALInit.nBaseAddress += (uint32)tPropVar.Val.dwVal;


    /* 
     * Attempt to retrieve any port configuration structures.
     */

    eResult = DALSYS_GetPropertyValue( pCtxt->pDevCtxt->hProp, "tlmm_ports", 0, &tPropVar );

    if( DAL_SUCCESS == eResult )
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

    gbTLMMInitialized = TRUE;

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
  if(DAL_SUCCESS == DALSYS_DestroyObject(pCtxt->pDevCtxt->hTlmmSync))
  {
    return DAL_SUCCESS;
  }
  else
  {
    return DAL_ERROR;
  }

} /* TLMM_DeviceDeInit */


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
  return(TLMM_ConfigGpioGroup(pCtxt, eEnable, &eGpioConfig, 1));

} /* TLMM_ConfigGpio */


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
  DALResult eResult = DAL_ERROR;

  uint32 nIdx;
  DALGpioSignalType nCfg;
  uint32 nGpioNumber;

  (void)DALSYS_SyncEnter(pCtxt->pDevCtxt->hTlmmSync);

  switch(eEnable)
  {
    case DAL_TLMM_GPIO_ENABLE:

      for( nIdx = 0; nIdx < nSize; nIdx++ )
      {
        nGpioNumber = HAL_GPIO_NUMBER(eGpioGroup[nIdx]);

        if( nGpioNumber < gnTotalGPIO )
        {
          /* 
           * Program the hardware.
           */
          HAL_tlmm_ConfigGpio(eGpioGroup[nIdx]);
          if( pgtState )
          {
            pgtState->ptGPIO[nGpioNumber].tAttr.nActive = 1;
          }
        }
      }
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_GPIO_DISABLE:

      for( nIdx = 0; nIdx < nSize; nIdx++ )
      {
        nGpioNumber = HAL_GPIO_NUMBER(eGpioGroup[nIdx]);

        if( (nGpioNumber < gnTotalGPIO) && pgtState )
        {
          /*
           * Convert the configuration to macro format.
           */

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
  HAL_tlmm_WriteGpio(eGpioConfig, eValue);

  return(DAL_SUCCESS);

} /* TLMM_GpioOut */


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
  if( (nGpioNumber < gnTotalGPIO) && pgtState )
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


boolean Tlmm_Init
(
   void
)
{
  DALResult eResult = DAL_ERROR;
  HAL_tlmm_InitType tHALInit;
  DALSYSPropertyVar tPropVar;

  tHALInit.nNumGpio = 200;

  DALSYS_GetDALPropertyHandle(DALDEVICEID_TLMM, hNonDalProps);

  /* 
   * Attempt to retrieve the base address for TLMM.
   */
  eResult = DALSYS_GetPropertyValue( hNonDalProps, "tlmm_base", 0, &tPropVar );

  if( DAL_SUCCESS != eResult )
  {
    /*
     * This is fatal.
     */
    return( FALSE );
  }

  tHALInit.nBaseAddress = (uint32)tPropVar.Val.dwVal;

  /* 
   * Get the TLMM Base offset for GPIO.
   */
  eResult = DALSYS_GetPropertyValue( hNonDalProps, "tlmm_offset", 0, &tPropVar );

  if( DAL_SUCCESS != eResult )
  {
    return( FALSE );
  }

  tHALInit.nBaseAddress += (uint32)tPropVar.Val.dwVal;

  HAL_tlmm_Init(&tHALInit);

  eResult = DALSYS_GetPropertyValue( hNonDalProps, "tlmm_sleep", 0, &tPropVar );

  gNonDalTlmmInit = TRUE;

  return( TRUE );

}


DALResult Tlmm_ConfigGpioGroup
(
  DALGpioEnableType  eEnable,
  DALGpioSignalType* eGpioGroup, 
  uint32             nSize
)
{
  uint32 nIdx;
  uint32 nGpioNumber = 0;
  static DALSYSPropertyVar tPropVar;
  DALResult eResult = DAL_ERROR;

  if( eGpioGroup == NULL )
  {
    return(DAL_ERROR);
  }

  if( gNonDalTlmmInit == FALSE )
  {
    if(FALSE == Tlmm_Init())
    {
      return(DAL_ERROR);
    }
  }

  switch(eEnable)
  {
    case DAL_TLMM_GPIO_ENABLE:

      for( nIdx = 0; nIdx < nSize; nIdx++ )
      {
        /* 
         * Program the hardware.
         */
        HAL_tlmm_ConfigGpio(eGpioGroup[nIdx]);
      }
      eResult = DAL_SUCCESS;
      break;

    case DAL_TLMM_GPIO_DISABLE:

      if( pgLowPowerConfigs == NULL )
      {
        if( DalChipInfo_ChipFamily() == DALCHIPINFO_FAMILY_MSM8926 )
        {
          /* Special handling for MSM8926 configuration compatiblity with MSM8x26. */
          eResult = DALSYS_GetPropertyValue( hNonDalProps, "tlmm_sleep8926", 0, &tPropVar );
        }
        else
        {
          eResult = DALSYS_GetPropertyValue( hNonDalProps, "tlmm_sleep", 0, &tPropVar );
        }
        if( eResult == DAL_ERROR )
        {
          return(DAL_ERROR);
        }
        pgLowPowerConfigs = (uint32 *) tPropVar.Val.pdwVal;
      }
      for( nIdx = 0; nIdx < nSize; nIdx++ )
      {
        nGpioNumber = HAL_GPIO_NUMBER(eGpioGroup[nIdx]);

        /* 
         * Program the hardware.
         */
        HAL_tlmm_ConfigGpio(((nGpioNumber & 0x3FF) << 4) | pgLowPowerConfigs[nGpioNumber]);

      }
      eResult = DAL_SUCCESS;
      break;

    default:
      eResult = DAL_ERROR;
      break;
  }
  return(eResult);

}

