/*=============================================================================

  FILE:   I2cPlatSvc.c

  OVERVIEW: This file contains the implementation for the platform services.
 
          Copyright (c) 2011 - 2014 Qualcomm Technologies Incorporated.
          All Rights Reserved.
          Qualcomm Confidential and Proprietary 

=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/i2c/src/dev/I2cPlatSvc.c#1 $
$DateTime: 2016/06/17 14:31:11 $$Author: pwbldsvc $
When     Who    What, where, why
-------- ---    -----------------------------------------------------------
09/05/14 MS     TLMM protection support by MPU
07/17/13 LK     Added xml properties.
09/26/11 LK     Created
=============================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cPlatSvc.h"
#include "I2cSys.h"
#include "I2cError.h"


#include "DALStdDef.h"
#include "DALSys.h"

#include "tzbsp_xpu.h"
#include "tzbsp_xpu_def.h"
#include "tzbsp_vmid_config.h"
#include "tzbsp_isr.h"
#include "tzbsp_sys.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define QUP_FREQ_HZ                            19200000

#define I2C_DEVICE_PLATBAM_MAX_BAM_THRESHOLD (32*1024)
#define I2C_DEVICE_PLATBAM_MAX_DESC_SIZE     0x100
#define I2C_DEVICE_PLATBAM_MAX_COMMAND_SIZE  0x100


#define I2C_XPU_QUP_RESOURCE_LOCKED           0x00000001
#define I2C_INTERRUPT_TRANSFERED_TO_TZ        0x00000002

#define I2C_BLSP_TO_XPU2_TYPE(BLSP_NUM)\
                     (BLSP_NUM == 1)? HAL_XPU2_BAM_BLSP1_DMA :\
                                      HAL_XPU2_BAM_BLSP2_DMA

//#define I2C_BLSP_TO_TZ_BLSP_RG(BLSP_NUM, CORE_ID)\
//                     (BLSP_NUM == 1)? HAL_XPU2_BAM_BLSP1_DMA :\
//                                      HAL_XPU2_BAM_BLSP2_DMA

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum I2CPLATSVC_Error
{
   I2CPLATDEV_ERROR_BASE = I2C_RES_ERROR_CLS_DEV_PLATFORM,
   
   I2CPLATSVC_ERROR_INVALID_DEVICE_INDEX,
   I2CPLATSVC_ERROR_DAL_GET_PROPERTY_HANDLE,
   I2CPLATSVC_ERROR_DAL_GET_PROPERTY_VALUE,
   I2CPLATSVC_ERROR_XPU_REMOVE_PERMS,
   I2CPLATSVC_ERROR_XPU_ADD_PERMS,
   I2CPLATSVC_ERROR_RPU_RECONFIGURE_SET,
   I2CPLATSVC_ERROR_RPU_RECONFIGURE_CLEAR,
   I2CPLATSVC_ERROR_MPU_RECONFIGURE_SET,  
   I2CPLATSVC_ERROR_MPU_RECONFIGURE_CLEAR,   
   I2CPLATSVC_ERROR_REGISTERING_INTERRUPT_ON_TZ,
   I2CPLATSVC_ERROR_UNREGISTERING_INTERRUPT_ON_TZ,
   I2CPLATSVC_ERROR_INVALID_POWER_STATE,

} I2CPLATSVC_Error;

typedef enum I2CPLATDEV_PowerStates
{
   I2CPLATDEV_POWER_STATE_0,/**< Bus and clocks off. */
   I2CPLATDEV_POWER_STATE_1,/**< Bus on clocks off. */
   I2CPLATDEV_POWER_STATE_2,/**< Bus on clocks on. */
} I2CPLATDEV_PowerStates;

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

typedef struct I2CPLATDEV_TargetCfgType
{
  uint32                        uPlatDevId;
  uint32                        tmpPowerState;
  I2CPLATDEV_PropertyType       devProp;

  uint32                        uBlspNum;
  uint32                       *pGpioNumArr;
  uint32                        uNumGpios;
  uint32                        uNumLockedGpios;
  uint32                        uInterruptId;
  uint32                        uQupRg;
  uint32                        uLockState;

} I2CPLATDEV_TargetCfgType, *I2CPLATDEV_PTargetCfgType;

void tzbsp_enable_xpu_config_clocks(boolean enable);

/*I2C[0] GPIO protection */
extern int tzbsp_mpu_lock_area(uint32 mpu, uint32 index, uint32 start,
							   uint32 end, uint32 rvmids, uint32 wvmids);
extern int tzbsp_mpu_unlock_area(uint32 mpu, uint32 index);


/*-------------------------------------------------------------------------
 * Static Variable Definitions
 * ----------------------------------------------------------------------*/
//static const char                    *pPeriphSsAddrName;
//static uint8                         *pPeriphSsBasePtr;



/*-------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * ----------------------------------------------------------------------*/




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
            DALSYS_GetPropertyValue(hProp, "CHIP_BUS_INDEX", 0, &PropVar) ) {
         break;
      }
      pDev->uPlatDevId = (uint32)PropVar.Val.dwVal;

      if ( DAL_SUCCESS !=
            DALSYS_GetPropertyValue(hProp, "CHIP_IRQ_NUM", 0, &PropVar) ) {
         break;
      }
      pDev->uInterruptId = (uint32)PropVar.Val.dwVal;

      if ( DAL_SUCCESS !=
            DALSYS_GetPropertyValue(hProp, "CHIP_QUP_BASE_ADDR", 0, &PropVar) ) {
         break;
      }
      pDev->devProp.virtBlockAddr = (uint8 *)PropVar.Val.dwVal;

      if ( DAL_SUCCESS !=
            DALSYS_GetPropertyValue(hProp, "CHIP_BLSP_NUM", 0, &PropVar) ) {
         break;
      }
      pDev->uBlspNum = (uint32)PropVar.Val.dwVal;

      if ( DAL_SUCCESS !=
            DALSYS_GetPropertyValue(hProp, "CHIP_QUP_RG_NUM", 0, &PropVar) ) {
         break;
      }
      pDev->uQupRg = (uint32)PropVar.Val.dwVal;

      if(DAL_SUCCESS == DALSYS_GetPropertyValue
               (hProp, //Prop handle
                "GPIO_NUM_ARR",     //Prop Name String
                0,                  //Not interested in Prop Id
                &PropVar))          //Output
      {
         pDev->pGpioNumArr = PropVar.Val.pdwVal;
         pDev->uNumGpios = PropVar.dwLen;
      }
      else {
         pDev->pGpioNumArr = NULL;
         pDev->uNumGpios = 0;     
      }

      if ( DAL_SUCCESS !=
            DALSYS_GetPropertyValue(hProp, "CHIP_APPS_CLK_FREQ_KHZ", 0, &PropVar) ) {
         break;
      }
      pDev->devProp.uI2cInputClkKhz = (uint32)PropVar.Val.dwVal;

      if ( DAL_SUCCESS !=
            DALSYS_GetPropertyValue(hProp, "SW_ENABLE_INTERRUPTS", 0, &PropVar) ) {
         break;
      }
      pDev->devProp.bInterruptBased = ((uint32)PropVar.Val.dwVal == 1)? 1: 0;

	  if ( DAL_SUCCESS ==  DALSYS_GetPropertyValue(hProp, "GPIO_MPU_PROTECTED", 0, &PropVar) )
	  {
         pDev->devProp.bIsGpiosMPUProtected = ((uint32)PropVar.Val.dwVal == 1)? 1: 0;
	  }

      if(pDev->devProp.bIsGpiosMPUProtected)
      {
         /*Note : Please confirm all these values from TZ team in case of TLMM protection by MPU*/
         if ( DAL_SUCCESS !=
    			 DALSYS_GetPropertyValue(hProp, "GPIO_IN_SEQUENCE", 0, &PropVar) ) {
    			 break;
    	 }
    	 pDev->devProp.bIsGpiosInSequence = ((uint32)PropVar.Val.dwVal == 1)? 1: 0;  
   
         if ( DAL_SUCCESS !=
         	     DALSYS_GetPropertyValue(hProp, "I2C_TZ_GPIO_MPU_INDEX_1", 0, &PropVar) ) {
                break;
         }
         pDev->devProp.i2cMPUIndex_1 = (uint32)PropVar.Val.dwVal;

         if ( DAL_SUCCESS !=
         	     DALSYS_GetPropertyValue(hProp, "I2C_TZ_GPIO_MPU_INDEX_2", 0, &PropVar) ) {
                break;
         }
         pDev->devProp.i2cMPUIndex_2 = (uint32)PropVar.Val.dwVal;		 
   
         if ( DAL_SUCCESS !=
         	     DALSYS_GetPropertyValue(hProp, "I2C_GPIO_SDA_START_ADDR", 0, &PropVar) ) {
                break;
         }
         pDev->devProp.i2cSDAStartGpioAddr = (uint32)PropVar.Val.dwVal;
   	     
         if ( DAL_SUCCESS !=
         	     DALSYS_GetPropertyValue(hProp, "I2C_GPIO_SDA_END_ADDR", 0, &PropVar) ) {
                break;
         }
         pDev->devProp.i2cSDAEndGpioAddr = (uint32)PropVar.Val.dwVal;
   	  
         if ( DAL_SUCCESS !=
         	     DALSYS_GetPropertyValue(hProp, "I2C_GPIO_SCL_START_ADDR", 0, &PropVar) ) {
                break;
         }
         pDev->devProp.i2cSCLStartGpioAddr = (uint32)PropVar.Val.dwVal;
   	     
         if ( DAL_SUCCESS !=
         	     DALSYS_GetPropertyValue(hProp, "I2C_GPIO_SCL_END_ADDR", 0, &PropVar) ) {
                break;
         }
         pDev->devProp.i2cSCLEndGpioAddr = (uint32)PropVar.Val.dwVal;
	    
      }
	  /****************************************/

      result = I2C_RES_SUCCESS;
   } while ( 0 );

   return result;
}

static int32 I2CPLATDEV_TransferInterruptToTZ(I2CPLATDEV_TargetCfgType *pDev)
{
    uint32 uState;
    int32 res;
    int32 retVal  = I2C_RES_SUCCESS;

    if ( (pDev->uLockState & I2C_INTERRUPT_TRANSFERED_TO_TZ) ||
         ( 0 == pDev->uInterruptId) ) {
       return I2C_RES_SUCCESS;     
    }

   /* Register for the I2C interrupt in TZBSP if this device is using
     * interrupts thereby transfering the interrupt from the HLOS to TZ. If the
     * interrupt_id is 0 then polling is being used on the HLOS so skip this
     * step */
    do
    {
       /* Disable all interrupts before transferring the interrupt from the HLOS
        * to the TZ side */
       uState = tzbsp_int_disable_all();

       /* Register the interrupt on the TZ side */
       res = tzbsp_register_isr(pDev->uInterruptId,
                                "I2C Device", NULL, NULL,
                                TZBSP_INTF_SAVE_INT | TZBSP_INTF_ALL_CPUS,
                                TRUE);
       if (res != I2C_RES_SUCCESS)
       {
          retVal = I2CPLATSVC_ERROR_REGISTERING_INTERRUPT_ON_TZ;
          break;
       }

       /* Disable the I2C device interrupt */
       tzbsp_disable_int(pDev->uInterruptId);
       pDev->uLockState |= I2C_INTERRUPT_TRANSFERED_TO_TZ;

    }while (0);

    /* Renable interrupts again */
    tzbsp_int_restore(uState);

    return retVal;   
}

static int32 I2CPLATDEV_TransferInterruptFromTZ(I2CPLATDEV_TargetCfgType *pDev)
{
    uint32 uState;
    int32 res;
    int32 retVal  = I2C_RES_SUCCESS;

    if ( (!(pDev->uLockState & I2C_INTERRUPT_TRANSFERED_TO_TZ)) ||
         ( 0 == pDev->uInterruptId) ) {
       return I2C_RES_SUCCESS;     
    }

   /* Register for the I2C interrupt in TZBSP if this device is using
     * interrupts thereby transfering the interrupt from the HLOS to TZ. If the
     * interrupt_id is 0 then polling is being used on the HLOS so skip this
     * step */
    do
    {
       /* Disable all interrupts before transferring the interrupt from the HLOS
        * to the TZ side */
       uState = tzbsp_int_disable_all();

       /* Re-enable the I2C interrupt */
       tzbsp_enable_int(pDev->uInterruptId);

       /* Deregister for the I2C interrupt thereby transfering the
        * I2C interrupt back to the HLOS */
        res = tzbsp_unregister_isr(pDev->uInterruptId,
                                   TZBSP_INTF_INT_FLAG_NON_SECURE |
                                   TZBSP_INTF_ALL_CPUS);
        if (res != I2C_RES_SUCCESS)
        {
          retVal = I2CPLATSVC_ERROR_UNREGISTERING_INTERRUPT_ON_TZ;
          break;
        }
        pDev->uLockState &= ~I2C_INTERRUPT_TRANSFERED_TO_TZ;

    }while (0);

    /* Renable interrupts again */
    tzbsp_int_restore(uState);

    return retVal;   
}

static int32 I2CPLATDEV_UnlockResources(I2CPLATDEV_TargetCfgType     *pDev)
{
  int32    retVal              = I2C_RES_SUCCESS;
  int32    xpu_res;
  HAL_xpu2_XPU2Type xpu_blsp   = (pDev->uBlspNum == 1)? HAL_XPU2_BAM_BLSP1_DMA:
                                                          HAL_XPU2_BAM_BLSP2_DMA;
  uint32 qup_rg                = pDev->uQupRg;
  HAL_xpu2_XPU2Type xpu_tlmm   = HAL_XPU2_TLMM;
  tzbsp_rpu_rg_t rpu_tlmm_gpio = {0, TZBSP_XPU_NON_SEC | TZBSP_RWGE, 
                                   TZBSP_VMID_AP, TZBSP_VMID_AP};
  int32    nGpioInd;

  if ( !(pDev->uLockState & I2C_XPU_QUP_RESOURCE_LOCKED) ) {
     return I2C_RES_SUCCESS;     
  }

  tzbsp_enable_xpu_config_clocks(TRUE);

  do {
    if (pDev->uLockState & I2C_XPU_QUP_RESOURCE_LOCKED) {
       /* QUP block protection */
       xpu_res = tzbsp_xpu_add_vmid_perms(xpu_blsp,
                                          qup_rg,
                                          TZBSP_VMID_AP_BIT,
                                          TZBSP_VMID_AP_BIT);
       if (xpu_res != I2C_RES_SUCCESS)
       {
         retVal = I2CPLATSVC_ERROR_XPU_ADD_PERMS;
         break;
       }
       pDev->uLockState &= ~I2C_XPU_QUP_RESOURCE_LOCKED; 
    }


	if(pDev->devProp.bIsGpiosMPUProtected)
	{
	   if(pDev->devProp.bIsGpiosInSequence)
	   {
    	   xpu_res = tzbsp_mpu_unlock_area(xpu_tlmm, pDev->devProp.i2cMPUIndex_1); /*SDA + SCL */
    	   if (xpu_res != I2C_RES_SUCCESS)
    	   {
    		  retVal = I2CPLATSVC_ERROR_MPU_RECONFIGURE_CLEAR;
    		  break;
    	   }
	   }
	   else
	   {
       	   xpu_res = tzbsp_mpu_unlock_area(xpu_tlmm, pDev->devProp.i2cMPUIndex_1); /*SDA */
    	   if (xpu_res != I2C_RES_SUCCESS)
    	   {
    		  retVal = I2CPLATSVC_ERROR_MPU_RECONFIGURE_CLEAR;
    		  break;
    	   }
    	   
    	   xpu_res = tzbsp_mpu_unlock_area(xpu_tlmm, pDev->devProp.i2cMPUIndex_2); /*SCL */
    	   if (xpu_res != I2C_RES_SUCCESS)
    	   {
    		  retVal = I2CPLATSVC_ERROR_MPU_RECONFIGURE_CLEAR;
    		  break;
    	   }
	   }
	   
	}
    else
    {
       for (nGpioInd = 0; nGpioInd < pDev->uNumLockedGpios; nGpioInd++)
	   {	   	
          rpu_tlmm_gpio.index = pDev->pGpioNumArr[nGpioInd];
          /* GPIO I2C DAT */
          xpu_res = tzbsp_rpu_reconfigure(xpu_tlmm, &rpu_tlmm_gpio, 0);
          if (xpu_res != I2C_RES_SUCCESS)
          {
             retVal = I2CPLATSVC_ERROR_RPU_RECONFIGURE_CLEAR;
             break;
          } 
	   }	
	}
	
  } while (0);
  tzbsp_enable_xpu_config_clocks(FALSE);

  return retVal;
}
  


static int I2CPLATDEV_LockResources(I2CPLATDEV_TargetCfgType     *pDev)
{
  int32             retVal          = I2C_RES_SUCCESS;
  HAL_xpu2_XPU2Type xpu_blsp        = I2C_BLSP_TO_XPU2_TYPE(pDev->uBlspNum);

  uint32 qup_rg                     = pDev->uQupRg;
  HAL_xpu2_XPU2Type xpu_tlmm        = HAL_XPU2_TLMM;  
  tzbsp_rpu_rg_t    rpu_tlmm_gpio   = {0, TZBSP_XPU_ENABLE | TZBSP_XPU_SEC | TZBSP_RWE, 
                                       TZBSP_VMID_TZ, TZBSP_VMID_TZ};
  int32    xpu_res;
  int32    nGpioInd;

  if ( pDev->uLockState & I2C_XPU_QUP_RESOURCE_LOCKED ) {
     return I2C_RES_SUCCESS;     
  }

  tzbsp_enable_xpu_config_clocks(TRUE);

  do {
    /* QUP block protection */
    xpu_res = tzbsp_xpu_remove_vmid_perms(xpu_blsp,
                                         qup_rg,
                                         TZBSP_VMID_AP_BIT,
                                         TZBSP_VMID_AP_BIT);
    if (xpu_res != I2C_RES_SUCCESS)
    {
      retVal = I2CPLATSVC_ERROR_XPU_REMOVE_PERMS;
      break;
    }
    pDev->uLockState |= I2C_XPU_QUP_RESOURCE_LOCKED;

    
	if(pDev->devProp.bIsGpiosMPUProtected) /* MPU*/
	{
	   if(pDev->devProp.bIsGpiosInSequence)
	   {
  	  	 /*Protect all GPIOs using one index, mention START and END range for included GPIOs sharing the boundry */
         xpu_res = tzbsp_mpu_lock_area(xpu_tlmm,
                             pDev->devProp.i2cMPUIndex_1 ,
                             pDev->devProp.i2cSDAStartGpioAddr,
                             pDev->devProp.i2cSCLEndGpioAddr,
                             TZBSP_VMID_TZ_BIT, TZBSP_VMID_TZ_BIT);
		 
         if (xpu_res != I2C_RES_SUCCESS)
         {
            retVal = I2CPLATSVC_ERROR_MPU_RECONFIGURE_SET;
            break;
         }  
	   }
	   else
	   {
   	      /*SDA*/
          xpu_res = tzbsp_mpu_lock_area(xpu_tlmm,
                              pDev->devProp.i2cMPUIndex_1,
                              pDev->devProp.i2cSDAStartGpioAddr,
                              pDev->devProp.i2cSDAEndGpioAddr,
                              TZBSP_VMID_TZ_BIT, TZBSP_VMID_TZ_BIT);
   	   
          if (xpu_res != I2C_RES_SUCCESS)
          {
             retVal = I2CPLATSVC_ERROR_MPU_RECONFIGURE_SET;
             break;
          }     
   
   	      /*SCL */
          xpu_res = tzbsp_mpu_lock_area(xpu_tlmm,
                              pDev->devProp.i2cMPUIndex_2,
                              pDev->devProp.i2cSCLStartGpioAddr,
                              pDev->devProp.i2cSCLEndGpioAddr,
                              TZBSP_VMID_TZ_BIT, TZBSP_VMID_TZ_BIT);  	   
          if (xpu_res != I2C_RES_SUCCESS)
          {
             retVal = I2CPLATSVC_ERROR_MPU_RECONFIGURE_SET;
             break;
          }  			  
		  
       }
	   
	}
	else /* RPU Protection */
	{
       pDev->uNumLockedGpios = 0;
	   
       for (nGpioInd = 0; nGpioInd < pDev->uNumGpios; nGpioInd++)
	   {
          rpu_tlmm_gpio.index = pDev->pGpioNumArr[nGpioInd];     
          /* GPIO I2C clock and data */
          xpu_res = tzbsp_rpu_reconfigure(xpu_tlmm, &rpu_tlmm_gpio, 1);
          if (xpu_res != I2C_RES_SUCCESS)
          {
             retVal = I2CPLATSVC_ERROR_RPU_RECONFIGURE_SET;
             break;
          }		
       }		
       pDev->uNumLockedGpios = nGpioInd;
	}
    
  } while (0);
  tzbsp_enable_xpu_config_clocks(FALSE);

  if (I2C_RES_SUCCESS != retVal) {
    /* Intentionally ignore the return value of tzbsp_i2c_unlock_resources()
     * to make sure the original error number is returned to the caller */
    (void) I2CPLATDEV_UnlockResources(pDev);
  }

  return retVal;
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
   int32 unlockRes, irqTrRes;

   unlockRes = I2CPLATDEV_UnlockResources(pDev);
   irqTrRes  = I2CPLATDEV_TransferInterruptFromTZ(pDev);
   
   if ( I2C_RES_SUCCESS != unlockRes ) {
      return unlockRes;
   }
   if ( I2C_RES_SUCCESS != irqTrRes ) {
      return irqTrRes;
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
   int32 res;

   do
   {
      res = I2CPLATDEV_TransferInterruptToTZ(pDev);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      res = I2CPLATDEV_LockResources(pDev); /* bus state is active. */
      //if ( I2C_RES_SUCCESS == res ) {
      //   break;
      //}     
   } while (0);

    if ( I2C_RES_SUCCESS != res ) {
      I2CPLATDEV_PowerState_Inactive(pDev);
   }
   return res;    
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
      res = I2CPLATDEV_PowerState_Active(pDev); /* bus state is active. */
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
#if 0     
      res = I2CPLATDEV_InitClocks(pDev);
      if ( I2C_RES_SUCCESS != res ) {
       break;
      }

      res = I2CPLATDEV_InitHwio(pDev);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
     
      res = I2CPLATDEV_InitGpios(pDev);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
#endif
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
   return I2CSYS_Free(hPlatDev);
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

/** @brief Acquires an interrupt lock.

    @param[in] hPlatDev platform device handle.
    @return          I2CSYS_Result .
  */
void
I2CPLATDEV_AcquireIntLock
(
   I2CPLATDEV_HANDLE hPlatDev
)
{
  
}

/** @brief Releases the interrupt lock.

    @param[in] hPlatDev platform device handle.
    @return          I2CSYS_Result .
  */
void
I2CPLATDEV_ReleaseIntLock
(
   I2CPLATDEV_HANDLE hPlatDev
)
{
  
}

/** @brief Checks if BAM is enabled.

    @param[in] hPlatDev platform device handle.
    @return    TRUE if BAM is enabled FALSE otherwise .
  */
boolean
I2CPLATDEV_IsBamEnabled
(
   I2CPLATDEV_HANDLE hPlatDev
)
{
   I2CPLATDEV_TargetCfgType     *pDev;
   pDev = (I2CPLATDEV_TargetCfgType*)hPlatDev;

   return pDev->platProps.bBamEnabled;
}
#endif


