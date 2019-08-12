/*
===========================================================================

FILE:   SpiDevicePlatSvc.c

DESCRIPTION:

===========================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/spi/src/device/SpiDevicePlatSvc.c#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
01/13/14 ms     Updated with common logging mechanism
08/19/13 sg     Added explicit vote for PNOC clock.
05/30/12 ddk    Added support for correct deassert wait calculation.
05/26/12 ddk    Added support for GPIO initialisation.
04/20/12 ddk    Enabled Clock initialisation in driver.
04/20/12 ddk    Added updates to enable logging.
04/19/12 ddk    Added changes for avoiding symbol match with I2C library.
04/13/12 ddk    Added de-init changes.
03/28/12 ddk    Added QUP physical address info for BAM cmd desc creation.
03/27/12 ddk	Fixed DAL clock API usage.
03/27/12 ag		Added SPI BAM mode requirements.
09/26/11 ag		Created (Based on Spi implementation by LK)

===========================================================================
        Copyright c 2011 - 2014 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "SpiDevicePlatSvc.h"
#include "SpiDeviceOsSvc.h"
#include "SpiDeviceError.h"
#include "SpiLog.h"

#include "stddef.h"
#include "stdio.h"

#include "DALStdDef.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "DDIClock.h"
#include "DDIHWIO.h"
#include "DDITlmm.h"
#include "icbarb.h"


#define QUP_FREQ_HZ              20000000

#define SPIDEVICEPLAT_CHK_RESULT_BREAK(res)\
   if (SPIDEVICE_RES_SUCCESS !=res) {\
      break;\
   }

#define SPI_ICB_CLIENT_CNT                1
#define SPI_PNOC_MSTRSLV_PAIRS_NUM	      1

typedef enum IBUSCFG_Error
{
   IBUSPLATCFG_ERROR_BASE = SPIDEVICE_RES_ERROR_CLS_DEV_PLATFORM,

   IBUSPLATCFG_RESULT_ERROR_DAL_GET_PROPERTY_HANDLE,
   IBUSPLATCFG_RESULT_ERROR_DAL_GET_PROPERTY_VALUE,
   IBUSPLATCFG_RESULT_ERROR_ATTACH_TO_CLOCKS,
   IBUSPLATCFG_RESULT_ERROR_ATTACH_TO_DALHWIO,
   IBUSPLATCFG_RESULT_ERROR_FAILED_TO_MAP_BLOCK_HWIO,
   IBUSPLATCFG_RESULT_ERROR_GETTING_CLK_ID,
   IBUSPLATCFG_RESULT_ERROR_CREATING_DISPATCH_WORKLOOP,
   IBUSPLATCFG_RESULT_ERROR_CREATING_DISPATCH_EVENT,
   IBUSPLATCFG_RESULT_ERROR_ASSOCIATING_EVENT_WITH_WORKLOOP,
   IBUSPLATCFG_RESULT_ERROR_INVALID_POWER_STATE,
   IBUSPLATCFG_RESULT_ERROR_FAILED_TO_SET_APPCLK_FREQ,
   IBUSPLATCFG_RESULT_ERROR_FAILED_TO_GET_APPCLK_FREQ,
   IBUSPLATCFG_RESULT_ERROR_FAILED_TO_ENABLE_APPCLK,
   IBUSPLATCFG_RESULT_ERROR_FAILED_TO_DISABLE_APPCLK,
   IBUSPLATCFG_RESULT_ERROR_FAILED_TO_ENABLE_HCLK,
   IBUSPLATCFG_RESULT_ERROR_FAILED_TO_DISABLE_HCLK,
   IBUSPLATCFG_RESULT_ERROR_TLMM_ATTACH,
   IBUSPLATCFG_RESULT_ERROR_TLMM_OPEN,
   IBUSPLATCFG_RESULT_ERROR_CONFIGURING_GPIOS,
   IBUSPLATCFG_RESULT_ERROR_NULL_PTR,
   IBUSPLATCFG_RESULT_ERROR_REMOVE_DEVCFG_NOT_FOUND,
   IBUSPLATCFG_RESULT_ERROR_FAILED_TO_CREATE_PNOC_CLIENT,
   IBUSPLATCFG_RESULT_ERROR_FAILED_TO_ISSUE_PNOC_REQ,
} IBUSCFG_Error;

#define QUPBUSOSSVC_NUM_DEVICES_MAX       12
int32 aQupDalDevIds[QUPBUSOSSVC_NUM_DEVICES_MAX] = 
{ DALDEVICEID_SPI_DEVICE_1, DALDEVICEID_SPI_DEVICE_2, DALDEVICEID_SPI_DEVICE_3,
  DALDEVICEID_SPI_DEVICE_4, DALDEVICEID_SPI_DEVICE_5, DALDEVICEID_SPI_DEVICE_6,
  DALDEVICEID_SPI_DEVICE_7, DALDEVICEID_SPI_DEVICE_8, DALDEVICEID_SPI_DEVICE_9,
  DALDEVICEID_SPI_DEVICE_10,DALDEVICEID_SPI_DEVICE_11,DALDEVICEID_SPI_DEVICE_12
};
uint32 IBUSLOGSPI_DynLogLevel = 1;

typedef enum IBUSPLATCFG_PowerStates
{
   IBUSPLATCFG_POWER_STATE_0,/**< Bus and clocks off. */
   IBUSPLATCFG_POWER_STATE_1,/**< Bus on clocks off. */
   IBUSPLATCFG_POWER_STATE_2,/**< Bus on clocks on. */
} IBUSPLATCFG_PowerStates;
 

typedef struct IBUSPLATCFG_DevTargetCfgType
{
   char                 *pQupHClkName;
   char                 *pQupAppClkName;
   uint32               uOsDevId;
   uint32               uQupCoreNum;
   ClockIdType          QupHClkId;
   ClockIdType          QupAppClkId;
   uint32               qupVirtBlockAddr;
   uint32               qupPhysBlockAddr;
   uint32               tcsrVirtBlockAddr;
   boolean              bInterruptBased;
   uint32               gpioConfigured;
   boolean              bBamSupported;
   boolean              bTcsrInterruptRequired;
   uint32				uTcsrInterruptBitMask;
   uint32				uTcsrInterruptBitShift;
   SpiDeviceBamType  *spiBam;
   uint32                 uActiveDevCount;
   boolean              bPnocEnable;   
   uint64               uPnocIBVal;
   uint64               uPnocABVal;
   uint32               uPnocMaster;
   uint32               uPnocSlave; 
   npa_client_handle    pNpaHandle;

} IBUSPLATCFG_DevTargetCfgType;


typedef struct IBUSPLATCFG_DevTargetCfgLinkType IBUSPLATCFG_DevTargetCfgLinkType;
struct IBUSPLATCFG_DevTargetCfgLinkType{
   IBUSPLATCFG_DevTargetCfgLinkType *pNextLink;
   IBUSPLATCFG_DevTargetCfgType devCfg;
};

struct {
   IBUSPLATCFG_DevTargetCfgLinkType *pHead;
   uint32 numDevCfgs;
} IBUSPLATCFG_DevTgtCfgList;

static DalDeviceHandle               *pClkHandle;
static DalDeviceHandle               *pTlmmHandle;
static DalDeviceHandle               *phDalHWIO;
static uint32                        uPeriphSsVirtAddr = 0;
static uint32                        uPeriphSsPhysAddr = 0;
static uint32 						 uTcsrVirtAddr = 0;

int32 IBUSPLATCFG_ClkDeInit(IBUSPLATCFG_DevTargetCfgType     *pDevTgtCfg);

static boolean IBUSPLATCFG_SearchDevCfg
(
   uint32 devId,
   IBUSPLATCFG_DevTargetCfgType **ppDevCfg
)
{
   IBUSPLATCFG_DevTargetCfgLinkType *pLink;

   if (NULL == IBUSPLATCFG_DevTgtCfgList.pHead) {
      return FALSE;
   }
   pLink = IBUSPLATCFG_DevTgtCfgList.pHead;
   do {
      if(pLink->devCfg.uOsDevId == devId) {
         *ppDevCfg = &pLink->devCfg;
         return TRUE;
      }
      pLink = pLink->pNextLink;
   } while(NULL != pLink);

   return FALSE;
}
static int32 IBUSPLATCFG_AddDevCfg
(
   uint32 uOsDevId,
   IBUSPLATCFG_DevTargetCfgType **ppDevCfg
)
{
   int32 res;
   IBUSPLATCFG_DevTargetCfgLinkType *pLink;

   res = IBUSOS_Malloc((void **)&pLink, sizeof(IBUSPLATCFG_DevTargetCfgLinkType));

   if (IBUSOS_RESULT_OK != res) 
   {
      SPIDEVICE_LOG(ERROR,"IBUSPLATCFG_AddDevCfg: Malloc failure\n");
      return res;
   }
   IBUSOS_Memset((void *)pLink, 0, sizeof(IBUSPLATCFG_DevTargetCfgLinkType));

   pLink->pNextLink = IBUSPLATCFG_DevTgtCfgList.pHead;
   IBUSPLATCFG_DevTgtCfgList.pHead = pLink;
   pLink->devCfg.uOsDevId = uOsDevId;
   pLink->devCfg.pQupAppClkName    = NULL;
   pLink->devCfg.pQupHClkName      = NULL;
   pLink->devCfg.QupHClkId         = 0;
   pLink->devCfg.QupAppClkId       = 0;
   pLink->devCfg.gpioConfigured    = 0;
   pLink->devCfg.bBamSupported     = 0;
   pLink->devCfg.bTcsrInterruptRequired = 0;
   pLink->devCfg.uTcsrInterruptBitMask = 0;
   pLink->devCfg.uTcsrInterruptBitShift = 0;
   *ppDevCfg = &pLink->devCfg;

   return SPIDEVICE_RES_SUCCESS;
}


static int32 IBUSPLATCFG_RemoveDevCfg
(
   uint32 uOsDevId
)
{
   int32 res = IBUSPLATCFG_RESULT_ERROR_REMOVE_DEVCFG_NOT_FOUND;
   IBUSPLATCFG_DevTargetCfgLinkType *pLink;
   IBUSPLATCFG_DevTargetCfgLinkType *oldpLink;

   if (NULL == IBUSPLATCFG_DevTgtCfgList.pHead) {
      return res;
   }
   pLink = IBUSPLATCFG_DevTgtCfgList.pHead;

   do 
   {
      if(pLink->devCfg.uOsDevId == uOsDevId)
      {
         if(IBUSPLATCFG_DevTgtCfgList.pHead == pLink)
         {
            IBUSPLATCFG_DevTgtCfgList.pHead = pLink->pNextLink;
         }
         else
         {
            oldpLink->pNextLink = pLink->pNextLink;
         }
         IBUSPLATCFG_DevTgtCfgList.numDevCfgs--;        
         res = IBUSOS_Free(pLink);
		 break;
      }
      oldpLink = pLink;
      pLink = pLink->pNextLink;
   } while(NULL != pLink);


   if (IBUSOS_RESULT_OK != res) {
      SPIDEVICE_LOG(ERROR,"IBUSPLATCFG_RemoveDevCfg: Free failure\n");
      return res;
   }
  
   return SPIDEVICE_RES_SUCCESS;
}


int32 IBUSPLATCFG_InitDalClock(void)
{
   DALResult dalRes;

   /* Get the DAL Handle only the first time */
   dalRes = DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &pClkHandle);
   if ((DAL_SUCCESS != dalRes) || (NULL == pClkHandle))
   {
      return IBUSPLATCFG_RESULT_ERROR_ATTACH_TO_CLOCKS;
   }

   return SPIDEVICE_RES_SUCCESS;
}

static DALResult IBUSPLATCFG_InitSpiGpios
(
   IBUSPLATCFG_DevTargetCfgType *pDev
)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar PropVar;
   uint32 clkSig, mosiSig, misoSig;
   DALSYSPropertyVar csSig;
   DALBOOL bCsGPIOUsed = FALSE;
   csSig.dwType  = DALSYS_PROP_TYPE_UINT32_PTR;
   csSig.dwLen = 0;

   if (NULL == pTlmmHandle)
   {
      SPIDEVICE_LOG(ERROR,"spi pTlmmHandle not initialised");
      return DAL_ERROR;
   }

   if( DAL_SUCCESS == DALSYS_GetDALPropertyHandle(pDev->uOsDevId, hProp))
   {
      if(DAL_SUCCESS == DALSYS_GetPropertyValue
            (hProp, //Prop handle
             "gpio_sig_spi_clk",     //Prop Name String
             0,                  //Not interested in Prop Id
             &PropVar))          //Output
      {
         clkSig = PropVar.Val.dwVal;
      }
      else
      {
         SPIDEVICE_LOG(ERROR,"spi fail to get clk gpio number");
         return DAL_ERROR;
      }

      if(DAL_SUCCESS == DALSYS_GetPropertyValue
            (hProp, //Prop handle
             "gpio_sig_spi_cs",     //Prop Name String
             0,                  //Not interested in Prop Id
             &PropVar))          //Output
      {
         csSig = PropVar;
         bCsGPIOUsed = TRUE;
      }
      else
      {
         SPIDEVICE_LOG(ERROR,"No GPIO assigned for spi cs");

         /* We do not fail here. Not having a GPIO for CS maybe intentional.
          * For example, 3 wire config.
          */
      }

      if(DAL_SUCCESS == DALSYS_GetPropertyValue
            (hProp, //Prop handle
             "gpio_sig_spi_mosi",     //Prop Name String
             0,                  //Not interested in Prop Id
             &PropVar))          //Output
      {
         mosiSig = PropVar.Val.dwVal;
      }
      else
      {
         SPIDEVICE_LOG(ERROR,"spi fail to get mosi gpio number");
         return DAL_ERROR;
      }

      if(DAL_SUCCESS == DALSYS_GetPropertyValue
            (hProp, //Prop handle
             "gpio_sig_spi_miso",     //Prop Name String
             0,                  //Not interested in Prop Id
             &PropVar))          //Output
      {
         misoSig = PropVar.Val.dwVal;
      }
      else
      {
         SPIDEVICE_LOG(ERROR,"spi fail to get miso gpio number");
         return DAL_ERROR;
      }
   }
   else
   {
      SPIDEVICE_LOG(ERROR,"spi ConfigGPIO fail to get property handle");
      return DAL_ERROR;
   }
   /* ready to enable gpio line */
   if(DAL_SUCCESS != DalTlmm_ConfigGpio(pTlmmHandle, 
                           (DALGpioSignalType) clkSig, 
                           DAL_TLMM_GPIO_ENABLE))
   {
      SPIDEVICE_LOG(ERROR,"spi ConfigGPIO fail on clkSig");
      return DAL_ERROR;
   }

   if(bCsGPIOUsed)
   {
      uint32 uIdx;
      if (csSig.dwLen > 0)
      {
         if (csSig.dwType != DALSYS_PROP_TYPE_UINT32_PTR)
         {
            SPIDEVICE_LOG(ERROR,"spi ConfigGPIO fail on csSig");
            return DAL_ERROR;
         }
         /* configure all the GPIOs for CS */
         for (uIdx = 0; uIdx < csSig.dwLen; ++uIdx)
         {
            if(DAL_SUCCESS != DalTlmm_ConfigGpio(pTlmmHandle, 
                                     (DALGpioSignalType) csSig.Val.pdwVal[uIdx], 
                                     DAL_TLMM_GPIO_ENABLE))
            {
               SPIDEVICE_LOG(ERROR,"spi ConfigGPIO fail on csSig");
               return DAL_ERROR;
            }
         }
      }
   }

   if(DAL_SUCCESS != DalTlmm_ConfigGpio(pTlmmHandle, 
                           (DALGpioSignalType) mosiSig,
                           DAL_TLMM_GPIO_ENABLE))
   {
      SPIDEVICE_LOG(ERROR,"spi ConfigGPIO fail on mosiSig");
      return DAL_ERROR;
   }

   if(DAL_SUCCESS != DalTlmm_ConfigGpio(pTlmmHandle, 
                          (DALGpioSignalType) misoSig, 
                          DAL_TLMM_GPIO_ENABLE))
   {
      SPIDEVICE_LOG(ERROR,"spi ConfigGPIO fail on misoSig");
      return DAL_ERROR;
   }

   return DAL_SUCCESS;
}


//Interface: IBusPower
int32 IBUSPLATCFG_ClkInit(IBUSPLATCFG_DevTargetCfgType     *pDevTgtCfg)
{
   DALResult dalRes;
   int32 res = SPIDEVICE_RES_SUCCESS;

   do
   {   
      dalRes = DalClock_GetClockId(pClkHandle, 
                       pDevTgtCfg->pQupAppClkName, 
                       &pDevTgtCfg->QupAppClkId);
   
      if (DAL_SUCCESS != dalRes)
      {
         res  = IBUSPLATCFG_RESULT_ERROR_GETTING_CLK_ID;
         break;
      }
   
      dalRes = DalClock_GetClockId(pClkHandle, 
                          pDevTgtCfg->pQupHClkName, 
                          &pDevTgtCfg->QupHClkId);
   
      if (DAL_SUCCESS != dalRes)
      {
         res = IBUSPLATCFG_RESULT_ERROR_GETTING_CLK_ID;
         break;
      }
   
   
      /* Enable QUP Clock */
      dalRes = DalClock_EnableClock(pClkHandle, pDevTgtCfg->QupAppClkId);
      if (DAL_SUCCESS != dalRes)
      {
         res =  IBUSPLATCFG_RESULT_ERROR_FAILED_TO_ENABLE_APPCLK;
         break;
      }
   
      /* Enable HCLK */
      dalRes = DalClock_EnableClock(pClkHandle, pDevTgtCfg->QupHClkId);
      if (DAL_SUCCESS != dalRes)
      {
         res =  IBUSPLATCFG_RESULT_ERROR_FAILED_TO_ENABLE_HCLK;
         break;
      }
   }while(0);
   
   if (res != SPIDEVICE_RES_SUCCESS)
   {
      SPIDEVICE_LOG(ERROR,
         "IBUSPLATCFG_ClkInit: Failure err 0x%x\n",(unsigned int)dalRes);
   }

   return res;
}

static int32 IBUSPLATCFG_GetDalProperties(IBUSPLATCFG_DevTargetCfgType *pDev)
{
	DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
	DALSYSPropertyVar PropVar;
	int32 res = SPIDEVICE_RES_ERROR_CLS_DEV_PLATFORM;
   	uint8 *pBasePtr;
	do{
		if (DAL_SUCCESS != DALSYS_GetDALPropertyHandle(pDev->uOsDevId, hProp )) {
			res = IBUSPLATCFG_RESULT_ERROR_DAL_GET_PROPERTY_HANDLE;
			break;
		}

		res = IBUSPLATCFG_RESULT_ERROR_DAL_GET_PROPERTY_VALUE;// set the error
		if (DAL_SUCCESS !=
				DALSYS_GetPropertyValue(hProp, "CHIP_BUS_INDEX",0, &PropVar)) {
			break;
		}
		pDev->uQupCoreNum = (uint32)PropVar.Val.dwVal;

		if (DAL_SUCCESS !=
				DALSYS_GetPropertyValue(hProp, "SW_USE_POLLING_MODE",0, &PropVar)) {
			break;
		}
		//FIXME:
		pDev->bInterruptBased = ((uint32)PropVar.Val.dwVal == 1)? 0:1;
		pDev->bInterruptBased = 1;

		if (DAL_SUCCESS !=
				DALSYS_GetPropertyValue(hProp, "SW_ENABLE_BAM",0, &PropVar)) {
			break;
		}
		pDev->bBamSupported = ((uint32)PropVar.Val.dwVal == 1)? 1:0;
      
		if (NULL == pDev->pQupAppClkName) {
			if (DAL_SUCCESS !=
					DALSYS_GetPropertyValue(hProp, "CHIP_APP_CLK_NAME",0, &PropVar)) {
				break;
			}
			pDev->pQupAppClkName = PropVar.Val.pszVal;
		}

		if (NULL == pDev->pQupHClkName) {
			if (DAL_SUCCESS !=
					DALSYS_GetPropertyValue(hProp, "CHIP_HCLK_NAME",0, &PropVar )) {
				break;
			}
			pDev->pQupHClkName = PropVar.Val.pszVal;
		}
      
        if ( DAL_SUCCESS != DALSYS_GetPropertyValue(
                                  hProp,"CHIP_PERIPH_SS_BASE", 
                                  0, &PropVar ))
        {
           break;
        }

        uPeriphSsPhysAddr = (uint32)PropVar.Val.dwVal ;
         
		if (0 == uPeriphSsVirtAddr) {

			if ( DAL_SUCCESS != DALSYS_GetPropertyValue(
						hProp,"CHIP_PERIPH_SS_BASE_NAME", 
						0, &PropVar ))
			{
				break;
			}

			if ( DAL_SUCCESS !=  DalHWIO_MapRegion(phDalHWIO, PropVar.Val.pszVal, &pBasePtr))
			{
				res = IBUSPLATCFG_RESULT_ERROR_FAILED_TO_MAP_BLOCK_HWIO;
				break;
			}
			uPeriphSsVirtAddr = (uint32)pBasePtr;
		}

		if ( DAL_SUCCESS != DALSYS_GetPropertyValue(
					hProp,"CHIP_BLOCK_OFFSET", // "QUPX_BASE"
					0, &PropVar ))
		{
			break;
		}
		pDev->qupVirtBlockAddr = (uint32)PropVar.Val.dwVal + uPeriphSsVirtAddr;
		pDev->qupPhysBlockAddr    = (uint32)PropVar.Val.dwVal + uPeriphSsPhysAddr;

		if ( DAL_SUCCESS == DALSYS_GetPropertyValue(
					hProp,"TCR_INTERRUPT_REQUIRED", // "TCSR"
					0, &PropVar ))
		{
			if(1 == (uint32)PropVar.Val.dwVal)
			{
				pDev->bTcsrInterruptRequired = 1;
				if (0 == uTcsrVirtAddr) {
		
					if ( DAL_SUCCESS != DALSYS_GetPropertyValue(
								hProp,"CHIP_TCSR_BASE_NAME", 
								0, &PropVar ))
					{
						break;
					}

					if ( DAL_SUCCESS !=  DalHWIO_MapRegion(phDalHWIO, PropVar.Val.pszVal, &pBasePtr))
					{
						res = IBUSPLATCFG_RESULT_ERROR_FAILED_TO_MAP_BLOCK_HWIO;
						break;
					}
					uTcsrVirtAddr = (uint32)pBasePtr;
				}

				if ( DAL_SUCCESS != DALSYS_GetPropertyValue(
							hProp,"CHIP_TCSR_BLOCK_OFFSET", // "TCSR_BASE"
							0, &PropVar ))
				{
					break;
				}
				pDev->tcsrVirtBlockAddr = (uint32)PropVar.Val.dwVal + uTcsrVirtAddr;

				if (DAL_SUCCESS !=
						DALSYS_GetPropertyValue(hProp, "CHIP_TCSR_ENABLE_MASK",0, &PropVar)) {
					break;
				}
				pDev->uTcsrInterruptBitMask = (uint32)PropVar.Val.dwVal;

				if (DAL_SUCCESS !=
						DALSYS_GetPropertyValue(hProp, "CHIP_TCSR_ENABLE_SHIFT",0, &PropVar)) {
					break;
				}
				pDev->uTcsrInterruptBitShift = (uint32)PropVar.Val.dwVal;
			}
		}

        if (DAL_SUCCESS !=
                    DALSYS_GetPropertyValue(hProp, "LOGGING_LEVEL",0, &PropVar)) {
                 break;
        }
        SpiLog_SetLogLevel(PropVar.Val.dwVal);
				
        /*PNOC settings*/
        if (DAL_SUCCESS == 
              DALSYS_GetPropertyValue(hProp, "PNOC_VOTE_ENABLE",0, &PropVar))
        {
           if (1 == (uint32)PropVar.Val.dwVal)
           {
              pDev->bPnocEnable = 1;

              if (DAL_SUCCESS !=
                    DALSYS_GetPropertyValue(hProp, "PNOC_IB_VAL",0, &PropVar)) {
                 break;
              }
              pDev->uPnocIBVal = (uint64)PropVar.Val.dwVal;

              if (DAL_SUCCESS !=
                    DALSYS_GetPropertyValue(hProp, "PNOC_AB_VAL",0, &PropVar)) {
                 break;
              }
              pDev->uPnocABVal = (uint64)PropVar.Val.dwVal;

              if (DAL_SUCCESS !=
                    DALSYS_GetPropertyValue(hProp, "PNOC_ARB_MASTER",0, &PropVar)) {
                 break;
              }
              pDev->uPnocMaster = PropVar.Val.dwVal;

              if (DAL_SUCCESS !=
                    DALSYS_GetPropertyValue(hProp, "PNOC_ARB_SLAVE",0, &PropVar)) {
                 break;
              }
              pDev->uPnocSlave = PropVar.Val.dwVal;
           }
        }
        res = SPIDEVICE_RES_SUCCESS; 
		
	}while(0);

   if (res != SPIDEVICE_RES_SUCCESS)
   {
      SPIDEVICE_LOG(ERROR,
         "IBUSPLATCFG_GetDalProperties: Failure err 0x%x\n",(unsigned int)res);
   }
   
	return res;
}

static int32 IBUSPLATCFG_GetBamInfo(IBUSPLATCFG_DevTargetCfgType *pDev)
{
	return SpiDeviceBamInit(&pDev->spiBam,(IBUSPLATCFG_HANDLE)pDev);
}

static int32 IBUSPLATCFG_BamDeInit(IBUSPLATCFG_DevTargetCfgType *pDev)
{
	return SpiDeviceBamDeInit(pDev->spiBam,(IBUSPLATCFG_HANDLE)pDev);
}


/** @brief sets the vote for PNOC.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             SPIDEVICE_RES_SUCCESS is successful,
                        otherwise IBUSCFG_Error.
  */

static int32 IBUSPLATCFG_AddPNOCVote(IBUSPLATCFG_DevTargetCfgType *pDev)
{
   ICBArb_RequestType     pnocRequest;
   ICBArb_MasterSlaveType icbMasterSlave;
   ICBArb_ErrorType       icbReqStatus;
   	  
   icbMasterSlave.eMaster = pDev->uPnocMaster; 
   icbMasterSlave.eSlave  = pDev->uPnocSlave;  

   /*NPA handle for PNOC*/
   pDev->pNpaHandle = icbarb_create_suppressible_client_ex
                                       ( "/node/core/bus/spi/clk", 
                                         &icbMasterSlave,
                                         SPI_ICB_CLIENT_CNT, NULL );
   if( pDev->pNpaHandle == NULL) {
     return IBUSPLATCFG_RESULT_ERROR_FAILED_TO_CREATE_PNOC_CLIENT;
   }  

   pnocRequest.arbType =    ICBARB_REQUEST_TYPE_3;     /* Ib/Ab pair */
  
   /* Bandwidth in Bytes/second */
   pnocRequest.arbData.type3.uIb = (uint64) pDev->uPnocIBVal;      
   pnocRequest.arbData.type3.uAb = (uint64) pDev->uPnocABVal;     

   icbReqStatus = icbarb_issue_request(pDev->pNpaHandle, &pnocRequest,
                                      SPI_PNOC_MSTRSLV_PAIRS_NUM);
   
   if(icbReqStatus != ICBARB_ERROR_SUCCESS) {
     return IBUSPLATCFG_RESULT_ERROR_FAILED_TO_ISSUE_PNOC_REQ;
   }
    
   return SPIDEVICE_RES_SUCCESS;
}


/** @brief unvote for PNOC.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             SPIDEVICE_RES_SUCCESS is successful,
                        otherwise
                        SPIDEVICE_RES_ERROR_CLS_SPI_DRV.
  */

static int32 IBUSPLATCFG_RemovePNOCVote(IBUSPLATCFG_DevTargetCfgType *pDev)
{ 
   if(pDev->pNpaHandle == NULL)
   {
      return SPIDEVICE_RES_ERROR_CLS_DEV_PLATFORM;
   }
   else
   {
      icbarb_complete_request(pDev->pNpaHandle);
      icbarb_destroy_client(pDev->pNpaHandle);
	  pDev->pNpaHandle = NULL;  
   }
   return SPIDEVICE_RES_SUCCESS; 
}


//Interface: IBusPlatCfg
int32 IBUSPLATCFG_InitTarget(uint32 uOsDevId, IBUSPLATCFG_HANDLE *phPlat)
{
	DALResult dalRes;
	IBUSPLATCFG_DevTargetCfgType *pDev;
	int32 res = SPIDEVICE_RES_SUCCESS;

   do 
   {
	   if (IBUSPLATCFG_SearchDevCfg(uOsDevId, &pDev)) {
		   res = SPIDEVICE_RES_SUCCESS;
		   break;
	   }
	   if (SPIDEVICE_RES_SUCCESS != (res =  IBUSPLATCFG_AddDevCfg(uOsDevId, &pDev))){
		   break;
	   }

	   if (NULL == pClkHandle) {
		   dalRes = DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &pClkHandle);
		   if ((DAL_SUCCESS != dalRes) || (NULL == pClkHandle)){
			   res = IBUSPLATCFG_RESULT_ERROR_ATTACH_TO_CLOCKS;
			   break;
		   }
	   }
	   if (NULL == pTlmmHandle) {
		   /* Initialize variables that applies to all the devices here.
		   */
		   if(DAL_SUCCESS != DAL_DeviceAttachEx(NULL, DALDEVICEID_TLMM, 
					   DALTLMM_INTERFACE_VERSION, &pTlmmHandle)){
			   res = IBUSPLATCFG_RESULT_ERROR_TLMM_ATTACH;
			   break;
		   }

		   if(DAL_SUCCESS != DalDevice_Open(pTlmmHandle, DAL_OPEN_SHARED)){
			   DAL_DeviceDetach(pTlmmHandle);
			   pTlmmHandle = NULL;
			   res = IBUSPLATCFG_RESULT_ERROR_TLMM_OPEN;
			   break;
		   }
	   }

	   if (NULL == phDalHWIO) {
		   dalRes = DAL_DeviceAttach(DALDEVICEID_HWIO, &phDalHWIO);

		   if ((DAL_SUCCESS != dalRes) || (NULL == phDalHWIO))
		   {
			   res = IBUSPLATCFG_RESULT_ERROR_ATTACH_TO_DALHWIO;
			   break;
		   }
	   }


	   if(SPIDEVICE_RES_SUCCESS != (res = IBUSPLATCFG_GetDalProperties(pDev))){
		   break;
	   }

      if(SPIDEVICE_RES_SUCCESS != (res = IBUSPLATCFG_ClkInit(pDev))){
         break;
      }
 
      if(SPIDEVICE_RES_SUCCESS != (res = IBUSPLATCFG_InitSpiGpios(pDev))){
         break;
      }

      if (pDev->bBamSupported){
         if(SPIDEVICE_RES_SUCCESS != (res = IBUSPLATCFG_GetBamInfo(pDev))){
            break;
        }
      }
	  
      if (TRUE == pDev->bPnocEnable) {
         if(SPIDEVICE_RES_SUCCESS != (res = IBUSPLATCFG_AddPNOCVote(pDev))){
            break;
         }
      }
   } while (0);

   if (res == SPIDEVICE_RES_SUCCESS)
   {
      *phPlat = (IBUSPLATCFG_HANDLE) pDev;

      /* Increment the active dev count */
      pDev->uActiveDevCount++;
      SPIDEVICE_LOG(INFO,
         "IBUSPLATCFG_InitTarget: uActiveDevCount 0x%x\n",
         (unsigned int)pDev->uActiveDevCount);
   }
   else
   {
      SPIDEVICE_LOG(ERROR,
         "IBUSPLATCFG_InitTarget: Failure err 0x%x\n",
         (unsigned int)res);
   }
  
	return res;
}

int32 IBUSPLATCFG_DeInitTarget(IBUSPLATCFG_HANDLE hPlat)
{
   int32 res = SPIDEVICE_RES_SUCCESS;
   IBUSPLATCFG_DevTargetCfgType *pDev = (IBUSPLATCFG_DevTargetCfgType *)hPlat;

   do
   {
      if (pDev == NULL)
      {
         res = IBUSPLATCFG_RESULT_ERROR_NULL_PTR;
         break;
      }

      IBUSPLATCFG_SetPowerState(hPlat, IBUSPLATCFG_POWER_STATE_2);

      if (pDev->bBamSupported)
      {
         res = IBUSPLATCFG_BamDeInit(pDev);

         if (res != SPIDEVICE_RES_SUCCESS)
         {
            SPIDEVICE_LOG(ERROR,
               "IBUSPLATCFG_DeInitTarget: BAM Deinit Failure err 0x%x\n",
               (unsigned int)res);
            break;
         }
      }

      IBUSPLATCFG_SetPowerState(hPlat, IBUSPLATCFG_POWER_STATE_0);

      if (pDev->uActiveDevCount == 1)   
      {   
         if (TRUE == pDev->bPnocEnable)
         {
            res = IBUSPLATCFG_RemovePNOCVote(pDev);
            SPIDEVICEPLAT_CHK_RESULT_BREAK(res);
         }

         res = IBUSPLATCFG_ClkDeInit(pDev);
         SPIDEVICEPLAT_CHK_RESULT_BREAK(res);

         res = IBUSPLATCFG_RemoveDevCfg(pDev->uOsDevId);
         SPIDEVICEPLAT_CHK_RESULT_BREAK(res);
      }      

      /* Decrement the active dev count */
      pDev->uActiveDevCount--; 
   }while(0);

   return res;
}

/** @brief Gets the app clock frequency in hz.

    @param[in] hPlat platform device handle.
    @param[out] pFrequencyHz Frequency of the core.

    @return          int32 .
  */
uint32 IBUSPLATCFG_GetAppClkHz
(
   IBUSPLATCFG_HANDLE hPlat,
   uint32 * pFrequencyHz
)
{
   IBUSPLATCFG_DevTargetCfgType *pDev = (IBUSPLATCFG_DevTargetCfgType *)hPlat;
   DALResult dalRes;

   /* Disable QUP Clock */
   dalRes = DalClock_GetClockFrequency(pClkHandle, 
                            pDev->QupAppClkId,
                            pFrequencyHz);
   
   if (DAL_SUCCESS != dalRes)
   {
       return IBUSPLATCFG_RESULT_ERROR_FAILED_TO_GET_APPCLK_FREQ; 
   }

   return SPIDEVICE_RES_SUCCESS;
}


/** @brief Sets the app clock frequency in Hz.

    @param[in] hPlat platform device handle.
    @param[in] reqFrequencyHz Frequency requested
    @param[out] pFinalFrequencyHz Frequency after the request.

    @return          int32 .
  */
uint32 IBUSPLATCFG_SetAppClkHz
(
   IBUSPLATCFG_HANDLE hPlat,
   uint32    reqFrequencyHz,
   uint32 * pFinalFrequencyHz
)
{
   IBUSPLATCFG_DevTargetCfgType *pDev = (IBUSPLATCFG_DevTargetCfgType *)hPlat;
   DALResult dalRes;

   /* Disable QUP Clock */
   dalRes = DalClock_SetClockFrequency(pClkHandle, 
                            pDev->QupAppClkId,
                            reqFrequencyHz,
                            CLOCK_FREQUENCY_HZ_AT_MOST,
                            pFinalFrequencyHz);
   
   if (DAL_SUCCESS != dalRes)
   {
       return IBUSPLATCFG_RESULT_ERROR_FAILED_TO_SET_APPCLK_FREQ; 
   }

   return SPIDEVICE_RES_SUCCESS;
}


int32 IBUSPLATCFG_ClkDeInit(IBUSPLATCFG_DevTargetCfgType     *pDevTgtCfg)
{
   DALResult dalRes;

   /* Disable QUP Clock */
   dalRes = DalClock_DisableClock(pClkHandle, pDevTgtCfg->QupAppClkId);
   if (DAL_SUCCESS != dalRes)
   {
       return IBUSPLATCFG_RESULT_ERROR_FAILED_TO_DISABLE_APPCLK; 
   }

   /* Disable HCLK */
   dalRes = DalClock_DisableClock(pClkHandle, pDevTgtCfg->QupHClkId);
   if (DAL_SUCCESS != dalRes)
   {
       return IBUSPLATCFG_RESULT_ERROR_FAILED_TO_DISABLE_HCLK;
   }

   return SPIDEVICE_RES_SUCCESS;
}

int32 IBUSPLATCFG_SetPowerState(IBUSPLATCFG_HANDLE hPlat, uint32 state)
{

   IBUSPLATCFG_DevTargetCfgType     *pDev = (IBUSPLATCFG_DevTargetCfgType *)hPlat;

   /* Bus state is OFF */
   if (IBUSPLATCFG_POWER_STATE_0 == state)
   {
      return IBUSPLATCFG_ClkDeInit(pDev);
   }
   /* Bus state is POWERED */
   else if (IBUSPLATCFG_POWER_STATE_1 == state)
   {
      return IBUSPLATCFG_ClkDeInit(pDev);
   }
   /* Bus state is ACTIVE */
   else if (IBUSPLATCFG_POWER_STATE_2 == state)
   {      
      return IBUSPLATCFG_ClkInit(pDev);
   }
   else
   {
      return IBUSPLATCFG_RESULT_ERROR_INVALID_POWER_STATE;
   }
   return SPIDEVICE_RES_SUCCESS;
}

int32 IBUSPLATCFG_GetPowerState(IBUSPLATCFG_HANDLE hPlat, uint32 state)
{
   return SPIDEVICE_RES_SUCCESS;
}


int32 IBUSPLATCFG_SetResourceExclusive
(
   IBUSPLATCFG_HANDLE hPlat,
   QUPRES_HANDLE     hRes
)
{
   return SPIDEVICE_RES_SUCCESS;
}

int32 IBUSPLATCFG_ReleaseResourceExclusive
(
    IBUSPLATCFG_HANDLE hPlat,
    QUPRES_HANDLE     hRes
)
{
   return SPIDEVICE_RES_SUCCESS;
}

int32 IBUSPLATCFG_GetPlatformInfo
(
 IBUSPLATCFG_HANDLE hPlat, 
 IBusPlatInfoType *platInfo
 )
{
   IBUSPLATCFG_DevTargetCfgType *pDev;
   
   pDev = (IBUSPLATCFG_DevTargetCfgType*)hPlat;
   
   //*platInfo = pDev->devProps;
   platInfo->uQupCoreNum            = pDev->uQupCoreNum;
   platInfo->qupVirtBlockAddr       = pDev->qupVirtBlockAddr;
   platInfo->qupPhysBlockAddr       = pDev->qupPhysBlockAddr;
   platInfo->tcsrVirtBlockAddr      = pDev->tcsrVirtBlockAddr;
   platInfo->bInterruptBased        = pDev->bInterruptBased;
   platInfo->bBamSupported          = pDev->bBamSupported;
   platInfo->spiBam	                = pDev->spiBam;
   platInfo->bTcsrInterruptRequired = pDev->bTcsrInterruptRequired;
   platInfo->uTcsrInterruptBitMask   = pDev->uTcsrInterruptBitMask;
   platInfo->uTcsrInterruptBitShift  = pDev->uTcsrInterruptBitShift;

   return SPIDEVICE_RES_SUCCESS;
}

int32 IBUSPLATCFG_PlatInternal_GetDalId
(
 IBUSPLATCFG_HANDLE hPlat
 )
{
   IBUSPLATCFG_DevTargetCfgType *pDev;
   
   pDev = (IBUSPLATCFG_DevTargetCfgType*)hPlat;
   return pDev->uOsDevId;

}

int32 IBUSPLATCFG_PlatInternal_GetQupCoreNum
(
 IBUSPLATCFG_HANDLE hPlat
)
{
   IBUSPLATCFG_DevTargetCfgType *pDev;
   
   pDev = (IBUSPLATCFG_DevTargetCfgType*)hPlat;
   return pDev->uQupCoreNum;
}

/** @brief Returns the physical address of the QUP device.

    @param[in] hPlat platform device handle.

    @return              int32.
  */
int32 IBUSPLATCFG_PlatInternal_GetQupCorePhysicalAddr
(
   IBUSPLATCFG_HANDLE hPlat
)
{
   IBUSPLATCFG_DevTargetCfgType *pDev;
   
   pDev = (IBUSPLATCFG_DevTargetCfgType*)hPlat;
   return pDev->qupPhysBlockAddr;
}

/** @brief Returns the virtual address of the QUP device.

    @param[in] hPlat platform device handle.

    @return              int32.
  */
int32 IBUSPLATCFG_PlatInternal_GetQupCoreVirtualAddr
(
   IBUSPLATCFG_HANDLE hPlat
)
{
   IBUSPLATCFG_DevTargetCfgType *pDev;
   
   pDev = (IBUSPLATCFG_DevTargetCfgType*)hPlat;
   return pDev->qupVirtBlockAddr;
}



