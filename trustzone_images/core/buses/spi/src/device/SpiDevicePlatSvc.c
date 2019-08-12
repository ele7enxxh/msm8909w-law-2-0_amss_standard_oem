/*
===========================================================================

FILE:   SpiDevicePlatSvc.c

DESCRIPTION:

===========================================================================

        Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spi/src/device/SpiDevicePlatSvc.c#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
04/25/16 dpk    Added GPIO/CLOCK configuration in SPI driver.
05/15/13 ag	Created 

===========================================================================
        Copyright c 2013-16 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "SpiDevicePlatSvc.h"
#include "SpiDeviceOsSvc.h"
#include "SpiDeviceError.h"
#include "tzbsp_spi.h"

#include "stddef.h"
#include "stdio.h"

#include "DALStdDef.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "Tlmm.h"
#include "ClockTZ.h"

#define MAX_NUM_QUP_HW_CORES 12
 static DALDEVICEID DalSpi_DeviceId[] =
{
  DALDEVICEID_SPI_DEVICE_1,
  DALDEVICEID_SPI_DEVICE_2,
  DALDEVICEID_SPI_DEVICE_3,
  DALDEVICEID_SPI_DEVICE_4,
  DALDEVICEID_SPI_DEVICE_5,
  DALDEVICEID_SPI_DEVICE_6,
  DALDEVICEID_SPI_DEVICE_7,
  DALDEVICEID_SPI_DEVICE_8,
  DALDEVICEID_SPI_DEVICE_9,
  DALDEVICEID_SPI_DEVICE_10,
  DALDEVICEID_SPI_DEVICE_11,
  DALDEVICEID_SPI_DEVICE_12,
};
const DALREG_DriverInfo
DALSpi_DriverInfo = { NULL,
                      MAX_NUM_QUP_HW_CORES,
                      DalSpi_DeviceId
                    };


#define QUP_FREQ_HZ              20000000

#define SPIDEVICEPLAT_CHK_RESULT_BREAK(res)\
   if (SPIDEVICE_RES_SUCCESS !=res) {\
      break;\
   }

typedef enum SpiDevicePlatCfg_Error
{
   SpiDevicePlatCfg_ERROR_BASE = SPIDEVICE_RES_ERROR_CLS_DEV_PLATFORM,

   SPIDEVICE_PLATCFG_RESULT_ERROR_DAL_GET_PROPERTY_HANDLE,
   SPIDEVICE_PLATCFG_RESULT_ERROR_DAL_GET_PROPERTY_VALUE,
   SPIDEVICE_PLATCFG_RESULT_ERROR_ATTACH_TO_CLOCKS,
   SPIDEVICE_PLATCFG_RESULT_ERROR_ATTACH_TO_DALHWIO,
   SPIDEVICE_PLATCFG_RESULT_ERROR_FAILED_TO_MAP_BLOCK_HWIO,
   SPIDEVICE_PLATCFG_RESULT_ERROR_GETTING_CLK_ID,
   SPIDEVICE_PLATCFG_RESULT_ERROR_CREATING_DISPATCH_WORKLOOP,
   SPIDEVICE_PLATCFG_RESULT_ERROR_CREATING_DISPATCH_EVENT,
   SPIDEVICE_PLATCFG_RESULT_ERROR_ASSOCIATING_EVENT_WITH_WORKLOOP,
   SPIDEVICE_PLATCFG_RESULT_ERROR_INVALID_POWER_STATE,
   SPIDEVICE_PLATCFG_RESULT_ERROR_FAILED_TO_SET_APPCLK_FREQ,
   SPIDEVICE_PLATCFG_RESULT_ERROR_FAILED_TO_GET_APPCLK_FREQ,
   SPIDEVICE_PLATCFG_RESULT_ERROR_FAILED_TO_ENABLE_APPCLK,
   SPIDEVICE_PLATCFG_RESULT_ERROR_FAILED_TO_DISABLE_APPCLK,
   SPIDEVICE_PLATCFG_RESULT_ERROR_FAILED_TO_ENABLE_HCLK,
   SPIDEVICE_PLATCFG_RESULT_ERROR_FAILED_TO_DISABLE_HCLK,
   SPIDEVICE_PLATCFG_RESULT_ERROR_TLMM_ATTACH,
   SPIDEVICE_PLATCFG_RESULT_ERROR_TLMM_OPEN,
   SPIDEVICE_PLATCFG_RESULT_ERROR_CONFIGURING_GPIOS,
   SPIDEVICE_PLATCFG_RESULT_ERROR_NULL_PTR,
   SPIDEVICE_PLATCFG_RESULT_ERROR_REMOVE_DEVCFG_NOT_FOUND,
   SPIDEVICE_PLATCFG_RESULT_ERROR_BAM_INITIALIZATION,
   SPIDEVICE_PLATCFG_RESULT_ERROR_BAM_DEINITIALIZATION,
} SpiDevicePlatCfg_Error;

typedef struct SpiDevicePlatCfg_DevTargetCfgType
{
   char *pQupHClkName;
   char *pQupAppClkName;
   uint32 uOsDevId;
   uint32 uQupCoreNum;
   uint32 qupVirtBlockAddr;
   uint32 qupPhysBlockAddr;
   uint32 tcsrVirtBlockAddr;
   boolean bInterruptBased;
   uint32 gpioConfigured;
   boolean bBamSupported;
   boolean bTcsrInterruptRequired;
   uint32 uTcsrInterruptBitMask;
   uint32 uTcsrInterruptBitShift;
   uint32 uClkGpioId;
   uint32 uMosiGpioId;
   uint32 uMisoGpioId;
   uint32 uCsGpioId;
   uint32 QupHClkId;
   uint32 QupAppClkId;
   SpiDeviceBamType *spiBam;
   SpiDeviceBamCfg_t spiBamCfg;
} SpiDevicePlatCfg_DevTargetCfgType;

typedef SpiDevicePlatCfg_DevTargetCfgType *SpiDevicePlatCfg_DevTargetCfgArrayType[12];

static SpiDevicePlatCfg_DevTargetCfgArrayType *platCfgArray = NULL;

static SpiDevicePlatCfg_DevTargetCfgType* SpiDevicePlatCfg_GetTargetConfig(uint32 uOsDevId)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar PropVar;
   uint32 uCoreNum = 0;
   char *pGpioMosiName = NULL;
   char *pGpioMisoName = NULL;
   char *pGpioCsName = NULL;
   char *pGPIOClkName = NULL;
   SpiDevicePlatCfg_DevTargetCfgType *tgtCfg = NULL;

   if (NULL == platCfgArray)
   {
      if (SPIDEVICEOSSVC_RESULT_OK != SpiDeviceOsSvc_Malloc((void **)&platCfgArray,
                                                            MAX_NUM_QUP_HW_CORES*sizeof(SpiDevicePlatCfg_DevTargetCfgType *)))
      {
         return NULL;
      }
   }

   do
   {
      if (DAL_SUCCESS != DALSYS_GetDALPropertyHandle(uOsDevId, hProp))
      {
         break;
      }

      if (DAL_SUCCESS == DALSYS_GetPropertyValue
          (hProp, //Prop handle
           "QUP_CORE_NUM",     //Prop Name String
           0,                  //Not interested in Prop Id
           &PropVar))          //Output
      {
         uCoreNum = PropVar.Val.dwVal;
      }
      else
      {
         break;
      }

      if (NULL != (*(platCfgArray))[uCoreNum - 1])
      {
         tgtCfg = (*(platCfgArray))[uCoreNum - 1];
         break;
      }

      if (SPIDEVICEOSSVC_RESULT_OK != SpiDeviceOsSvc_Malloc((void **)&tgtCfg,
                                                            sizeof(SpiDevicePlatCfg_DevTargetCfgType)))
      {
         tgtCfg = NULL;
         break;
      }

      tgtCfg->uQupCoreNum = uCoreNum;
      tgtCfg->uOsDevId = uOsDevId;

      if (DAL_SUCCESS == DALSYS_GetPropertyValue
          (hProp, //Prop handle
           "QUP_BASE_ADDR",     //Prop Name String
           0,                  //Not interested in Prop Id
           &PropVar))          //Output
      {
         tgtCfg->qupVirtBlockAddr = PropVar.Val.dwVal;
         tgtCfg->qupPhysBlockAddr = PropVar.Val.dwVal;
      }
      else
      {
         SpiDeviceOsSvc_Free((void *)tgtCfg);
         tgtCfg = NULL;
         break;
      }

      if (DAL_SUCCESS == DALSYS_GetPropertyValue
          (hProp, //Prop handle
           "SW_ENABLE_BAM",     //Prop Name String
           0,                  //Not interested in Prop Id
           &PropVar))          //Output
      {
         tgtCfg->bBamSupported = PropVar.Val.dwVal;
      }
      else
      {
         SpiDeviceOsSvc_Free((void *)tgtCfg);
         tgtCfg = NULL;
         break;
      }

      if (DAL_SUCCESS == DALSYS_GetPropertyValue
          (hProp, //Prop handle
           "BAM_BLSP_ID",     //Prop Name String
           0,                  //Not interested in Prop Id
           &PropVar))          //Output
      {
         tgtCfg->spiBamCfg.uBamBlspId = PropVar.Val.dwVal;
      }
      else
      {
         SpiDeviceOsSvc_Free((void *)tgtCfg);
         tgtCfg = NULL;
         break;
      }

      if (DAL_SUCCESS == DALSYS_GetPropertyValue
          (hProp, //Prop handle
           "BAM_BASE_ADDR",     //Prop Name String
           0,                  //Not interested in Prop Id
           &PropVar))          //Output
      {
         tgtCfg->spiBamCfg.uBamBaseAddr = PropVar.Val.dwVal;
      }
      else
      {
         SpiDeviceOsSvc_Free((void *)tgtCfg);
         tgtCfg = NULL;
         break;
      }

      if (DAL_SUCCESS == DALSYS_GetPropertyValue
          (hProp, //Prop handle
           "BAM_INTERRUPT_ID",     //Prop Name String
           0,                  //Not interested in Prop Id
           &PropVar))          //Output
      {
         tgtCfg->spiBamCfg.uBamInterruptId = PropVar.Val.dwVal;
      }
      else
      {
         SpiDeviceOsSvc_Free((void *)tgtCfg);
         tgtCfg = NULL;
         break;
      }

      if (DAL_SUCCESS == DALSYS_GetPropertyValue
          (hProp, //Prop handle
           "BAM_TX_PIPE_NUM",     //Prop Name String
           0,                  //Not interested in Prop Id
           &PropVar))          //Output
      {
         tgtCfg->spiBamCfg.uBamTxPipeNum = PropVar.Val.dwVal;
      }
      else
      {
         SpiDeviceOsSvc_Free((void *)tgtCfg);
         tgtCfg = NULL;
         break;
      }

      if (DAL_SUCCESS == DALSYS_GetPropertyValue
          (hProp, //Prop handle
           "BAM_RX_PIPE_NUM",     //Prop Name String
           0,                  //Not interested in Prop Id
           &PropVar))          //Output
      {
         tgtCfg->spiBamCfg.uBamRxPipeNum = PropVar.Val.dwVal;
      }
      else
      {
         SpiDeviceOsSvc_Free((void *)tgtCfg);
         tgtCfg = NULL;
         break;
      }

      /* If the Clocks and GPIO properties are not entered in xml, it means that they will be 
             enable/configured externally. In such case calling respective functions will return success */
         
      if (DAL_SUCCESS == DALSYS_GetPropertyValue 
          (hProp, //Prop handle
           "CHIP_HCLK_NAME",     //Prop Name String
           0,                    //Not interested in Prop Id
           &PropVar))            //Output
      {
         tgtCfg->pQupHClkName = PropVar.Val.pszVal;
		 if (DAL_SUCCESS != Clock_GetClockId(tgtCfg->pQupHClkName, (ClockId *)&tgtCfg->QupHClkId))
         {
            SpiDeviceOsSvc_Free((void *)tgtCfg);
            tgtCfg = NULL;
            break;
         }
      }
	  else 
      {
         tgtCfg->QupHClkId = 0;
      }

	  if (DAL_SUCCESS == DALSYS_GetPropertyValue 
          (hProp, //Prop handle
           "CHIP_APP_CLK_NAME",   //Prop Name String
           0,                    //Not interested in Prop Id
           &PropVar))            //Output
      {
         tgtCfg->pQupAppClkName = PropVar.Val.pszVal;
		 if (DAL_SUCCESS != Clock_GetClockId(tgtCfg->pQupAppClkName, (ClockId *)&tgtCfg->QupAppClkId))
         {
            SpiDeviceOsSvc_Free((void *)tgtCfg);
            tgtCfg = NULL;
            break;
         }
      }
	  else
      {
         tgtCfg->QupAppClkId = 0;
      }

      //Read CLK GPIO
      if (DAL_SUCCESS == DALSYS_GetPropertyValue 
          (hProp, //Prop handle
           "gpio_spi_clk_str",      //Prop Name String
           0,                       //Not interested in Prop Id
           &PropVar))               //Output
      {
         pGPIOClkName = PropVar.Val.pszVal;
		 if (DAL_SUCCESS != Tlmm_GetGpioId(pGPIOClkName, (TLMMGpioIdKeyType *)&tgtCfg->uClkGpioId))
         { 
            SpiDeviceOsSvc_Free((void *)tgtCfg);
            tgtCfg = NULL;
            break;
         }
      }
      else
      {
         tgtCfg->uClkGpioId = 0;
      }
   
      //Read MOSI GPIO
      if (DAL_SUCCESS == DALSYS_GetPropertyValue 
          (hProp, //Prop handle
           "gpio_spi_mosi_str",    //Prop Name String
           0,                      //Not interested in Prop Id
           &PropVar))              //Output
      {
         pGpioMosiName = PropVar.Val.pszVal;
		 if (DAL_SUCCESS != Tlmm_GetGpioId(pGpioMosiName, (TLMMGpioIdKeyType *)&tgtCfg->uMosiGpioId))
         { 
            SpiDeviceOsSvc_Free((void *)tgtCfg);
            tgtCfg = NULL;
            break;
         }
      }
      else
      {
         tgtCfg->uMosiGpioId = 0;
      }	  

      //Read MISO GPIO
      if (DAL_SUCCESS == DALSYS_GetPropertyValue 
          (hProp, //Prop handle
           "gpio_spi_miso_str",   //Prop Name String 
           0,                     //Not interested in Prop Id
           &PropVar))             //Output
      {
         pGpioMisoName = PropVar.Val.pszVal;
		 if (DAL_SUCCESS != Tlmm_GetGpioId(pGpioMisoName, (TLMMGpioIdKeyType *)&tgtCfg->uMisoGpioId))
         { 
            SpiDeviceOsSvc_Free((void *)tgtCfg);
            tgtCfg = NULL;
            break;
         }
      }
      else
      {
         tgtCfg->uMisoGpioId = 0;
      }
	  
      //Read CS GPIO
      if (DAL_SUCCESS == DALSYS_GetPropertyValue 
          (hProp, //Prop handle
           "gpio_spi_cs_str",    //Prop Name String 
           0,                    //Not interested in Prop Id
           &PropVar))            //Output
      {
         pGpioCsName = PropVar.Val.pszVal;
		 if (DAL_SUCCESS != Tlmm_GetGpioId(pGpioCsName, (TLMMGpioIdKeyType *)&tgtCfg->uCsGpioId))
         { 
            SpiDeviceOsSvc_Free((void *)tgtCfg);
            tgtCfg = NULL;
            break;
         }
      }
      else
      {
         tgtCfg->uCsGpioId = 0;
      }
	  
      (*(platCfgArray))[uCoreNum - 1] = tgtCfg;
   }
   while (0);

   return tgtCfg;
}


static int32 SpiDevicePlatCfg_EnableClocks (SpiDevicePlatCfg_DevTargetCfgType *tgtCfg)
{
   int32 res = SPIDEVICE_RES_SUCCESS;
  

   do
   {
      /* If clock properties were not entered in the xml file, return success. 
         In this case, it is recommended to enable clocks externally (From HLOS) */
      if (!(tgtCfg->QupHClkId && tgtCfg->QupAppClkId))
      {
         break;
      }
   
      /* Enable AHB Clock */
      if (DAL_SUCCESS != Clock_EnableClock((ClockId)tgtCfg->QupHClkId))
      {
         res = SPIDEVICE_PLATCFG_RESULT_ERROR_FAILED_TO_ENABLE_HCLK;
		 break;
      }
	  
      /* Enable APPS Clock */
      if (DAL_SUCCESS != Clock_EnableClock((ClockId)tgtCfg->QupAppClkId))
      {
         res = SPIDEVICE_PLATCFG_RESULT_ERROR_FAILED_TO_ENABLE_APPCLK;
		 break;
      }
   }
   while(0);
   
   return res;
}

static int32 SpiDevicePlatCfg_DisableClocks (SpiDevicePlatCfg_DevTargetCfgType *tgtCfg)
{
   int32 res = SPIDEVICE_RES_SUCCESS;	

   
   do 
   {
      /* If clock properties were not entered in the xml file, return success. 
         In this case, it is recommended to disable clocks externally (From HLOS) */
      if (!(tgtCfg->QupHClkId && tgtCfg->QupAppClkId))
      {
         break;
      }
   
      if (DAL_SUCCESS != Clock_DisableClock((ClockId)tgtCfg->QupAppClkId)) 
      {
         res = SPIDEVICE_PLATCFG_RESULT_ERROR_FAILED_TO_DISABLE_APPCLK;
		 break;
	  }
	  
	  if (DAL_SUCCESS != Clock_DisableClock((ClockId)tgtCfg->QupHClkId)) 
      {
         res = SPIDEVICE_PLATCFG_RESULT_ERROR_FAILED_TO_DISABLE_HCLK;
		 break;
	  }
   }
   while(0);

   return res;
}

static int32 SpiDevicePlatCfg_ConfigGpios (SpiDevicePlatCfg_DevTargetCfgType *tgtCfg)
{
   TLMMGpioConfigIdType gpiosettings;
   int32 res = SPIDEVICE_RES_SUCCESS;

   gpiosettings.eDirection = TLMM_GPIO_OUTPUT; 
   gpiosettings.ePull = TLMM_GPIO_PULL_DOWN;  
   gpiosettings.eDriveStrength = TLMM_GPIO_8MA;
   
   do
   {
      /* If GPIO properties were not entered in the xml file, return success. 
         In this case, it is recommended to configure GPIO's externally (From HLOS) */
      if (!(tgtCfg->uClkGpioId && tgtCfg->uMosiGpioId && tgtCfg->uMisoGpioId && tgtCfg->uCsGpioId))
      {
         break;
      }

      /* Config CLK Pin */
      if (DAL_SUCCESS != Tlmm_ConfigGpioId((TLMMGpioIdKeyType)tgtCfg->uClkGpioId, &gpiosettings))
      {
         res = SPIDEVICE_PLATCFG_RESULT_ERROR_CONFIGURING_GPIOS;      
         break;
      }
	  
      /* Config MOSI Pin */
      if (DAL_SUCCESS != Tlmm_ConfigGpioId((TLMMGpioIdKeyType)tgtCfg->uMosiGpioId, &gpiosettings))
      {
         res = SPIDEVICE_PLATCFG_RESULT_ERROR_CONFIGURING_GPIOS;      
         break;
      }
      
      /* Config MISO Pin */
      if (DAL_SUCCESS != Tlmm_ConfigGpioId((TLMMGpioIdKeyType)tgtCfg->uMisoGpioId, &gpiosettings))
      {
         res = SPIDEVICE_PLATCFG_RESULT_ERROR_CONFIGURING_GPIOS;      
         break;
      }
	  
      /* Config CS Pin */
      if (DAL_SUCCESS != Tlmm_ConfigGpioId((TLMMGpioIdKeyType)tgtCfg->uCsGpioId, &gpiosettings))
      {
         res = SPIDEVICE_PLATCFG_RESULT_ERROR_CONFIGURING_GPIOS;      
         break;
      }	    

   }
   while(0);
   
   return res;
}

int32 SpiDevicePlatCfg_InitTarget(uint32 uOsDevId, SPIDEVICE_PLATCFG_HANDLE *phPlat)
{
   int32 res = SPIDEVICE_RES_ERROR_CLS_DEV_PLATFORM;
   SpiDevicePlatCfg_DevTargetCfgType *tgtCfg = NULL;
   uint32 devId = (uint32)DalSpi_DeviceId[uOsDevId - 1];

   
   do
   {
      tgtCfg = (SPIDEVICE_PLATCFG_HANDLE)SpiDevicePlatCfg_GetTargetConfig(devId);
      if (NULL == tgtCfg)
      {
	     break;
      }
      else
      {
	     *phPlat = (SPIDEVICE_PLATCFG_HANDLE)tgtCfg;
      }	  

      /* Here Clocks are enabled to configure SPI & BAM registers */
	  res = SpiDevicePlatCfg_EnableClocks(tgtCfg);
	  if (SPIDEVICE_RES_SUCCESS != res)
      {
         break;
      }
	  
      res = SpiDevicePlatCfg_ConfigGpios(tgtCfg);
      if (SPIDEVICE_RES_SUCCESS != res)
      {
         break;
      }

      if (NULL == tgtCfg->spiBam)
      {
         res = SpiDeviceBamInit(&tgtCfg->spiBam, (SPIDEVICE_PLATCFG_HANDLE)tgtCfg);
         if (SPIDEVICE_RES_SUCCESS != res)
         {
            res = SPIDEVICE_PLATCFG_RESULT_ERROR_BAM_INITIALIZATION;
         }
      }
      else
      {
         res = SPIDEVICE_RES_SUCCESS;
      }
   }
   while(0);

   return res;
}

int32 SpiDevicePlatCfg_DeInitTarget(SPIDEVICE_PLATCFG_HANDLE hPlat)
{
   /* SpiDevicePlatCfg_InitTarget, just does data structure allocation.
    * The allocated data structures will be available for the life-time 
    * of the driver. There is no point in allocating and de-allocating 
    * them.
    */
   int32 return_value = SPIDEVICE_RES_SUCCESS;
   int32 res = SPIDEVICE_RES_SUCCESS;
   SpiDevicePlatCfg_DevTargetCfgType *tgtCfg = (SpiDevicePlatCfg_DevTargetCfgType *)hPlat;

   do
   {
      /* Clocks are enabled to access BAM registers while de-Initializing. 
       * And disabled after doing so.
	  */
      res = SpiDevicePlatCfg_EnableClocks(tgtCfg);
      if (SPIDEVICE_RES_SUCCESS != res)
      {
         return_value = res;
         break;
      }
   
      if (tgtCfg->spiBam)
      {
         if (SPIDEVICE_RES_SUCCESS != SpiDeviceBamDeInit(&tgtCfg->spiBam))
         {
            return_value = SPIDEVICE_PLATCFG_RESULT_ERROR_BAM_DEINITIALIZATION;
         }
      }

      res = SpiDevicePlatCfg_DisableClocks(tgtCfg);
      if ((SPIDEVICE_RES_SUCCESS != res) && (SPIDEVICE_RES_SUCCESS == return_value))
      {
         return_value = res;
         break;
      }
   }
   while(0);
   
   return return_value;
}


int32 SpiDevicePlatCfg_GetPlatformInfo
(
   SPIDEVICE_PLATCFG_HANDLE hPlat,
   SpiDevicePlatInfoType *platInfo
)
{
   SpiDevicePlatCfg_DevTargetCfgType *pDev;

   pDev = (SpiDevicePlatCfg_DevTargetCfgType *)hPlat;

   //*platInfo = pDev->devProps;
   platInfo->uQupCoreNum = pDev->uQupCoreNum;
   platInfo->qupVirtBlockAddr = pDev->qupVirtBlockAddr;
   platInfo->qupPhysBlockAddr = pDev->qupPhysBlockAddr;
   platInfo->tcsrVirtBlockAddr = pDev->tcsrVirtBlockAddr;
   platInfo->bInterruptBased = pDev->bInterruptBased;
   platInfo->bBamSupported = pDev->bBamSupported;
   platInfo->spiBam = pDev->spiBam;
   platInfo->bTcsrInterruptRequired = pDev->bTcsrInterruptRequired;
   platInfo->uTcsrInterruptBitMask = pDev->uTcsrInterruptBitMask;
   platInfo->uTcsrInterruptBitShift = pDev->uTcsrInterruptBitShift;

   return SPIDEVICE_RES_SUCCESS;
}

/** @brief Returns the physical address of the QUP device.

    @param[in] hPlat platform device handle.

    @return              int32.
  */
SpiDeviceBamCfg_t* SpiDevicePlatCfg_PlatInternal_GetBamCfg
(
   SPIDEVICE_PLATCFG_HANDLE hPlat
)
{
   SpiDevicePlatCfg_DevTargetCfgType *pDev;
   
   pDev = (SpiDevicePlatCfg_DevTargetCfgType*)hPlat;
   return &pDev->spiBamCfg;
}

/** @brief Sets the power state of the device.

    @param[in] hPlat platform device handle.

    @return          int32 .
  */
int32 SpiDevicePlatCfg_SetPowerState(SPIDEVICE_PLATCFG_HANDLE hPlat, uint32 state)
{
   SpiDevicePlatCfg_DevTargetCfgType *pDev;
   int32 res = SPIDEVICE_RES_SUCCESS;
   
   
   pDev = (SpiDevicePlatCfg_DevTargetCfgType*)hPlat;
   
   switch (state)
   {
      case SPIDEVICE_POWER_STATE_2:
           res = SpiDevicePlatCfg_EnableClocks(pDev);           
           
           break;
      case SPIDEVICE_POWER_STATE_1:
      case SPIDEVICE_POWER_STATE_0:
          res = SpiDevicePlatCfg_DisableClocks(pDev);
           
           break;
      default:
           break;
   }

   return res;
}

/** @brief Configures Apps clock frequency

    Configures Apps clock frequency
    @param[in]  pDev            Pointer to device struct.
    @param[in]  pQupConfig      Pointer to the QUP config for data transfer.
    @param[out] pNewFrequency   the new frequency the SPI core is running (if successful)
          
    @return          int32 .
  */

uint32 SpiDevicePlatCfg_SetAppClkHz (SPIDEVICE_PLATCFG_HANDLE hPlat, uint32 reqFrequencyHz, uint32 *pFinalFrequencyHz)
{
   SpiDevicePlatCfg_DevTargetCfgType *tgtCfg = (SpiDevicePlatCfg_DevTargetCfgType *)hPlat;
   uint32 res = SPIDEVICE_PLATCFG_RESULT_ERROR_FAILED_TO_SET_APPCLK_FREQ;


   do
   {   
      if (!(tgtCfg->QupAppClkId))
      {
         res = SPIDEVICE_RES_SUCCESS;
         break;
      }
	  
      if (SPIDEVICE_RES_SUCCESS == Clock_SetClockFrequency((ClockId)tgtCfg->QupAppClkId, (reqFrequencyHz/1000), pFinalFrequencyHz))
      {
         *pFinalFrequencyHz = (*pFinalFrequencyHz) * 1000;
         res = SPIDEVICE_RES_SUCCESS;
      }
   }
   while(0);      
   
   return res;
}


