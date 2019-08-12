/*=============================================================================

  FILE:   I2cPlatSvc.c

  OVERVIEW: This file contains the implementation for the platform services.
 
          Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated.
          All Rights Reserved.
          Qualcomm Confidential and Proprietary 

=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/i2c/src/dev/I2cPlatSvc.c#1 $
$DateTime: 2015/09/01 00:30:35 $$Author: pwbldsvc $
When        Who    What, where, why
--------  ---    -----------------------------------------------------------
03/13/14  ms     Created
=============================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cPlatSvc.h"
#include "I2cSys.h"
#include "I2cError.h"

#include "DALStdDef.h"
#include "DALSys.h"
#include "ClockBoot.h"
#include "DALDeviceId.h"
#include "DDITlmm.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define QUP_FREQ_HZ                            19200000

#define I2C_DEVICE_PLATBAM_MAX_BAM_THRESHOLD (32*1024)
#define I2C_DEVICE_PLATBAM_MAX_DESC_SIZE     0x100
#define I2C_DEVICE_PLATBAM_MAX_COMMAND_SIZE  0x100



/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum I2CPLATSVC_Error
{
   I2CPLATDEV_ERROR_BASE = I2C_RES_ERROR_CLS_DEV_PLATFORM,
   
   I2CPLATSVC_ERROR_INVALID_DEVICE_INDEX,
   I2CPLATSVC_ERROR_DAL_GET_PROPERTY_HANDLE,
   I2CPLATSVC_ERROR_DAL_GET_PROPERTY_VALUE,
  
   I2CPLATSVC_ERROR_INVALID_POWER_STATE,

   I2CPLATSVC_ERROR_FAILED_TO_ATTACH_TO_TLMM,
   I2CPLATSVC_ERROR_FAILED_TO_DETTACH_FROM_TLMM,
   I2CPLATSVC_ERROR_FAILED_TO_OPEN_TLMM,
   I2CPLATSVC_ERROR_FAILED_TO_CLOSE_TLMM,
   I2CPLATSVC_ERROR_FAILED_TO_CONFIGURE_GPIO,
   I2CPLATSVC_ERROR_FAILED_TO_ENABLE_CLOCK,
   I2CPLATSVC_ERROR_FAILED_TO_DISABLE_CLOCK,
   
} I2CPLATSVC_Error;

typedef enum I2CPLATDEV_PowerStates
{
   I2CPLATDEV_POWER_STATE_0,/**< Bus and clocks off. */
   I2CPLATDEV_POWER_STATE_1,/**< Bus on clocks off. */
   I2CPLATDEV_POWER_STATE_2,/**< Bus on clocks on. */
} I2CPLATDEV_PowerStates;

typedef enum I2CPLATDEV_TargetInitState
{
   I2CPLATDEV_TGT_INIT_NOT_DONE = 0,
   //I2CPLATDEV_TGT_INIT_CLK_ALLOCATED  = 0x01,
   //I2CPLATDEV_TGT_INIT_HWIO_ALLOCATED = 0x02,
   //I2CPLATDEV_TGT_INIT_BAM_INITED     = 0x04,
   I2CPLATDEV_TGT_INIT_TLMM_ALLOCATED = 0x08,
   I2CPLATDEV_TGT_INIT_TLMM_OPENED    = 0x10,
} I2CPLATDEV_TargetInitState;

#if 0
typedef struct I2cOs_IrqType
{
   ISR_HOOK IsrHook;
   IST_HOOK IstHook;
   void     *pIsrData;
   void     *pIstData;
   uint32   uInterruptId;
   uint32   uIrqTriggerCfg;
} I2cOs_IrqType;

typedef enum I2CPLATDEV_TargetInitState
{
   I2CPLATDEV_TGT_INIT_NOT_DONE = 0,
   I2CPLATDEV_TGT_INIT_CLK_ALLOCATED  = 0x01,
   I2CPLATDEV_TGT_INIT_HWIO_ALLOCATED = 0x02,
   I2CPLATDEV_TGT_INIT_BAM_INITED     = 0x04,
   I2CPLATDEV_TGT_INIT_TLMM_ALLOCATED = 0x08,
   I2CPLATDEV_TGT_INIT_TLMM_OPENED    = 0x10,
} I2CPLATDEV_TargetInitState;

#endif

// In SBL only qup6 and qup11 are in use.
// this macro is to save space
//#define I2C_APPS_CLK(IDX) ((IDX==5)? CLK_BLSP1_QUP6_I2C_APPS:\
//                (IDX == 10)?CLK_BLSP2_QUP5_I2C_APPS:CLK_BLSP1_QUP6_I2C_APPS) 

typedef struct I2CPLATDEV_TargetCfgType
{
  uint32                        uPlatDevId;
  ClockQUPI2CType               uClkIdx;
  uint32                        tmpPowerState;
  I2CPLATDEV_PropertyType       devProp;

  uint32                       *pGpioCfgArr;
  uint32                        uNumGpios;
  uint32                        uNumLockedGpios;
  DalDeviceHandle              *hTlmm;
  I2CPLATDEV_TargetInitState    initState;
  

} I2CPLATDEV_TargetCfgType, *I2CPLATDEV_PTargetCfgType;


/*-------------------------------------------------------------------------
 * Static Variable Definitions
 * ----------------------------------------------------------------------*/
//static const char                    *pPeriphSsAddrName;
//static uint8                         *pPeriphSsBasePtr;



/*-------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * ----------------------------------------------------------------------*/

/** @brief Initializes the scl,sda gpios.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
static int32
I2CPLATDEV_InitGpios
(
   I2CPLATDEV_TargetCfgType     *pDev
)
{
   DALResult dalRes;
   uint32 uGpioInd;

   if (0 == pDev->uNumGpios) {
      return I2C_RES_SUCCESS;
   }
   if (NULL == pDev->hTlmm) {
      dalRes = DAL_DeviceAttachEx(NULL, DALDEVICEID_TLMM,
                                  DALTLMM_INTERFACE_VERSION, &pDev->hTlmm);
      if ( (DAL_SUCCESS != dalRes) || (NULL == pDev->hTlmm) ) {
         return I2CPLATSVC_ERROR_FAILED_TO_ATTACH_TO_TLMM;
      }
   }
   pDev->initState |= I2CPLATDEV_TGT_INIT_TLMM_ALLOCATED;
   
   dalRes = DalDevice_Open(pDev->hTlmm, DAL_OPEN_SHARED);
   if ( DAL_SUCCESS != dalRes ) {
      return I2CPLATSVC_ERROR_FAILED_TO_OPEN_TLMM;
   }
   pDev->initState |= I2CPLATDEV_TGT_INIT_TLMM_OPENED;

   for (uGpioInd = 0; uGpioInd <  pDev->uNumGpios ; uGpioInd++) {
      dalRes = DalTlmm_ConfigGpio(pDev->hTlmm, pDev->pGpioCfgArr[uGpioInd],
                                  DAL_TLMM_GPIO_ENABLE);
      if ( DAL_SUCCESS != dalRes ) {
         return I2CPLATSVC_ERROR_FAILED_TO_CONFIGURE_GPIO;
      }
   }

   return I2C_RES_SUCCESS;
}

/** @brief Initializes the scl,sda gpios.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
static int32
I2CPLATDEV_DeInitGpios
(
   I2CPLATDEV_TargetCfgType     *pDev
)
{
   DALResult dalRes;
   uint32 uGpioInd;

   if ( 0 == pDev->uNumGpios ) {
      return I2C_RES_SUCCESS;
   }
   if ( pDev->initState & I2CPLATDEV_TGT_INIT_TLMM_OPENED ) {
      for (uGpioInd = 0; uGpioInd <  pDev->uNumGpios ; uGpioInd++) {
         dalRes = DalTlmm_ConfigGpio(pDev->hTlmm, pDev->pGpioCfgArr[uGpioInd],
                                  DAL_TLMM_GPIO_DISABLE);
         if ( DAL_SUCCESS != dalRes ) {
            return I2CPLATSVC_ERROR_FAILED_TO_CONFIGURE_GPIO;
         }
      }
      dalRes = DalDevice_Close(pDev->hTlmm);
      if ( DAL_SUCCESS != dalRes ) {
         return I2CPLATSVC_ERROR_FAILED_TO_CLOSE_TLMM;
      }
   }
   if ( pDev->initState & I2CPLATDEV_TGT_INIT_TLMM_ALLOCATED ) {
      dalRes = DAL_DeviceDetach(pDev->hTlmm);
      if ( DAL_SUCCESS != dalRes ) {
         return I2CPLATSVC_ERROR_FAILED_TO_DETTACH_FROM_TLMM;
      }
   }

   return I2C_RES_SUCCESS; 
}

/** @brief Initializes bam.
  
    @param[in]  uPlatDevId  Platform device ID.
    @param[out] pDev        Pointer to platform device
          structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CPLATDEV_ReadPlatConfig
(
   uint32                    uPlatDevId,
   I2CPLATDEV_TargetCfgType *pDev
)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar               PropVar;
   int32 result;

   if ( DAL_SUCCESS != DALSYS_GetDALPropertyHandle(uPlatDevId, hProp ) ) {
      return I2CPLATSVC_ERROR_DAL_GET_PROPERTY_HANDLE;
   }

   result = I2CPLATSVC_ERROR_DAL_GET_PROPERTY_VALUE; /* set the error. */
   do {
      if ( DAL_SUCCESS !=
            DALSYS_GetPropertyValue(hProp, "CHIP_CLK_INDEX", 0, &PropVar) ) {
         break;
      }
      pDev->uClkIdx = (ClockQUPI2CType)PropVar.Val.dwVal;

      if ( DAL_SUCCESS !=
            DALSYS_GetPropertyValue(hProp, "CHIP_QUP_BASE_ADDR", 0, &PropVar) ) {
         break;
      }
      pDev->devProp.virtBlockAddr = (uint8 *)PropVar.Val.dwVal;

      if ( DAL_SUCCESS ==
            DALSYS_GetPropertyValue(hProp, "CHIP_GPIO_CONFIG_ARR", 0, &PropVar )) {
         pDev->pGpioCfgArr = PropVar.Val.pdwVal;
         pDev->uNumGpios = PropVar.dwLen;
      }
      else {
         pDev->uNumGpios = 0;
      }

      if ( DAL_SUCCESS !=
            DALSYS_GetPropertyValue(hProp, "CHIP_APPS_CLK_FREQ_KHZ", 0, &PropVar) ) {
         break;
      }
      pDev->devProp.uI2cInputClkKhz = (uint32)PropVar.Val.dwVal;

      result = I2C_RES_SUCCESS;
   } while ( 0 );

   return result;
}



  


 
/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/

/** @brief Sets the power state.
  
    @param[in] pDev     Platform device config pointer.
   
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
static int32
I2CPLATDEV_PowerState_Inactive
(
   I2CPLATDEV_TargetCfgType     *pDev
)
{
   boolean res;     
   
   res = Clock_DisableI2CClock(pDev->uClkIdx);
   if (FALSE == res) {
      return I2CPLATSVC_ERROR_FAILED_TO_DISABLE_CLOCK;     
   }
   return I2C_RES_SUCCESS;    
}

/** @brief Sets the power state.
  
    @param[in] pDev     Platform device config pointer.
   
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */

static int32
I2CPLATDEV_PowerState_Active
(
   I2CPLATDEV_TargetCfgType     *pDev
)
{
   boolean res;

  
   //CLOCK_BOOT_PERF_MIN  (19.2 MHz)
   res = Clock_SetI2CClockFrequency(CLOCK_BOOT_PERF_MIN,
                                    pDev->uClkIdx);
   if (FALSE == res) {
      return I2CPLATSVC_ERROR_FAILED_TO_ENABLE_CLOCK;     
   }

   return I2C_RES_SUCCESS;    
}

/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/

/** @brief Sets the power state.
  
    @param[in] hPlat  Platform handle.
    @param[in] state  New power state.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CPLATDEV_SetPowerState
(
   I2CPLATDEV_HANDLE hPlat,
   uint32            state
)
{
   I2CPLATDEV_TargetCfgType     *pDev = (I2CPLATDEV_TargetCfgType *)hPlat;
   
   int32                         res = I2C_RES_SUCCESS;

   if ( ( I2CPLATDEV_POWER_STATE_0 == state ) ||
        ( I2CPLATDEV_POWER_STATE_1 == state ) ) {
      res = I2CPLATDEV_PowerState_Inactive(pDev); /*  bus state is inactive. */
   }
   else if ( I2CPLATDEV_POWER_STATE_2 == state ) {      
      res = I2CPLATDEV_PowerState_Active(pDev); // bus state is active. 
   }
   else {
      return I2CPLATSVC_ERROR_INVALID_POWER_STATE;
   }

   if ( I2C_RES_SUCCESS == res ) {
      pDev->tmpPowerState = state;
   }

   return res;
}

/** @brief Gets the power state.
  
    @param[in] hPlat  Platform handle.
    @param[in] pState Pointer to power state.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CPLATDEV_GetPowerState
(
   I2CPLATDEV_HANDLE  hPlat,
   uint32            *pState
)
{
   I2CPLATDEV_TargetCfgType     *pDev = (I2CPLATDEV_TargetCfgType *)hPlat;

   *pState = pDev->tmpPowerState;
   return I2C_RES_SUCCESS;
}


/** @brief Reads the properties for this device.

    @param[in]  hPlat     Platform handle.
    @param[out] pDevProp  Pointer to device properties.

    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CPLATDEV_ReadProperties
(
   I2CPLATDEV_HANDLE        hPlat,
   I2CPLATDEV_PropertyType *pDevProp
)
{
   I2CPLATDEV_TargetCfgType     *pDev;
   
   pDev = (I2CPLATDEV_TargetCfgType*)hPlat;
   
   *pDevProp = pDev->devProp;

   return I2C_RES_SUCCESS;
}

/**
    This Function initializes target support structures and subsystems.

    @param[in]  uPlatDevId  Platform specific device identifier.
    @param[out] phPlat      Pointer to platform handle.

    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CPLATDEV_InitTarget
(
   uint32             uPlatDevId,
   I2CPLATDEV_HANDLE *phPlat
)
{

   I2CPLATDEV_TargetCfgType       *pDev;
   int32                           res;
 
   res = I2CSYS_Malloc((void **)&pDev, sizeof(I2CPLATDEV_TargetCfgType));
   if (I2C_RES_SUCCESS != res) {
      return res;
   }
   
   do
   {
      res = I2CSYS_Memset(pDev, 0, sizeof(I2CPLATDEV_TargetCfgType));
      if ( I2C_RES_SUCCESS != res )  {
         break;
      }

      res = I2CPLATDEV_ReadPlatConfig(uPlatDevId, pDev);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      res = I2CPLATDEV_InitGpios(pDev);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
#if 0     
      res = I2CPLATDEV_InitClocks(pDev);
      if ( I2C_RES_SUCCESS != res ) {
       break;
      }

      res = I2CPLATDEV_InitHwio(pDev);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
     
      
#endif
      pDev->uPlatDevId = uPlatDevId;
      *phPlat = (I2CPLATDEV_HANDLE) pDev;
      res = I2C_RES_SUCCESS;
    } while ( 0 );

   if ( I2C_RES_SUCCESS != res ) {
      I2CPLATDEV_DeInitTarget(pDev);
   }
   return res;
}


/**
    This Function deinitializes target support structures and
    subsystems.

    @param[in]  hPlatDev     Platform handle.

    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CPLATDEV_DeInitTarget
(
   I2CPLATDEV_HANDLE hPlatDev
)
{
   int32                           gpioRes, freeRes;
   
   gpioRes = I2CPLATDEV_DeInitGpios((I2CPLATDEV_TargetCfgType *)hPlatDev);

   freeRes = I2CSYS_Free(hPlatDev);
   if (gpioRes) {
      return gpioRes;     
   }
   return freeRes;

#if 0        
   I2CPLATDEV_TargetCfgType       *pDev;
   int32                           res ;
   int32                           clkRes, hwioRes,bamRes,gpioRes,freeRes;

   pDev = (I2CPLATDEV_TargetCfgType *)hPlatDev;
   res = clkRes = hwioRes = bamRes = gpioRes = freeRes = I2C_RES_SUCCESS;

   clkRes = I2CPLATDEV_DeInitClocks(pDev);
   hwioRes = I2CPLATDEV_DeInitHwio(pDev);
   bamRes = I2CPLATDEV_BamDeInit(pDev);
   gpioRes = I2CPLATDEV_DeInitGpios(pDev);
   freeRes = I2CSYS_Free(pDev);

   if (clkRes)
   {
      res = clkRes;
   }
   else if (hwioRes)
   {
      res = hwioRes;
   }
   else if (bamRes)
   {
      res = bamRes;
   }
   else if (gpioRes)
   {
      res = gpioRes;
   }
   else if (freeRes) {
      res = freeRes;
   }

   return res;
#endif
}

/**
    This Function registers IST and ISR callbacks.

    @param[in]  hPlatDev     Platform handle.
    @param[in]  IsrHook      ISR function pointer.
    @param[in]  pIsrData     pointer to ISR callback data.
    @param[in]  IstHook      IST function pointer.
    @param[in]  pIstData     pointer to IST callback data.

    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CSYS_RegisterISTHooks
(
   I2CPLATDEV_HANDLE hPlatDev,
   ISR_HOOK          IsrHook,
   void*             pIsrData,
   IST_HOOK          IstHook,
   void*             pIstData
)
{
#if 0
   DALResult                     dalRes;
   I2CPLATDEV_TargetCfgType     *pDev;
   
   pDev = (I2CPLATDEV_TargetCfgType*)hPlatDev;
   
   if ( NULL == pDev->hInterruptCtrl ) {
      dalRes =
          DAL_InterruptControllerDeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER,
                                              &pDev->hInterruptCtrl);
      if( (DAL_SUCCESS != dalRes) || (NULL == pDev->hInterruptCtrl) ) {
         return I2CPLATSVC_ERROR_FAILED_ATTACH_TO_IRQCTRL; 
      }
   }

   pDev->irq.IsrHook = IsrHook;
   pDev->irq.IstHook = IstHook;
   pDev->irq.pIsrData = pIsrData;
   pDev->irq.pIstData = pIstData;
   pDev->irq.uInterruptId = pDev->platProps.uInterruptId;
   pDev->irq.uIrqTriggerCfg = pDev->platProps.uIrqTriggerCfg;
   dalRes = DalInterruptController_RegisterISR(
                        pDev->hInterruptCtrl,
                        pDev->irq.uInterruptId,
                        I2CPLATDEV_Isr,
                        (DALISRCtx)pDev,
                        pDev->irq.uIrqTriggerCfg);
   if ( DAL_SUCCESS != dalRes ) {
      return I2CPLATSVC_ERROR_FAILED_TO_REGISTER_IST;
   }
#endif  
   return I2C_RES_SUCCESS;
}

/**
    This Function unregisters IST and ISR callbacks.

    @param[in]  hPlatDev     Platform handle.

    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLATSVC_Error.
  */
int32
I2CSYS_UnRegisterISTHooks
(
   I2CPLATDEV_HANDLE hPlatDev
)
{
#if 0        
   DALResult                     dalRes;
   I2CPLATDEV_TargetCfgType     *pDev;
   pDev = (I2CPLATDEV_TargetCfgType*)hPlatDev;
   
   if ( NULL == pDev->hInterruptCtrl ) {
      return I2CPLATSVC_ERROR_FAILED_ATTACH_TO_IRQCTRL; 
   }

   dalRes = DalInterruptController_Unregister(pDev->hInterruptCtrl,pDev->irq.uInterruptId);
   if ( DAL_SUCCESS != dalRes ) {
      return I2CPLATSVC_ERROR_FAILED_TO_UNREGISTER_IST;
   }
#endif
   return I2C_RES_SUCCESS;
}

#if 0
/**
    This Function gets the app clock freq in KHz.

    @param[in]  hPlatDev     Platform handle.

    @return             frequency in KHz.
  */
uint32
I2CPLATDEV_GetAppClkKhz
(
   I2CPLATDEV_HANDLE hPlat
)
{
   /* TODO: BRINGUP fix this when clocks are running. */
   return 20000;
}


#endif


