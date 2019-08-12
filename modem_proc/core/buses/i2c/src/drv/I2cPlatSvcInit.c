/*=============================================================================

  FILE:   I2cPlatSvc.c

  OVERVIEW: This file contains the implementation for the platform services.
 
          Copyright (c) 2015-2016 Qualcomm Technologies Incorporated.
          All Rights Reserved.
          Qualcomm Confidential and Proprietary 

=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/drv/I2cPlatSvcInit.c#1 $
$DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $
When     Who    What, where, why
-------- ---    -----------------------------------------------------------
 03/15/16 VG     Adding InitClock in InitTarget
 08/25/15 VG     Created
=============================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cPlatSvc.h"
#include "I2cSys.h"
#include "I2cError.h"
#include "I2cLog.h"

//DEBUG
#include "DALDeviceId.h"
#include "DDITlmm.h"
#include "DDIHWIO.h"



/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define QUP_FREQ_HZ                            19200000

#define I2C_DEVICE_PLATBAM_MAX_BAM_THRESHOLD (32*1024)
#define I2C_DEVICE_PLATBAM_MAX_DESC_SIZE     0x100

#define I2C_ICB_CLIENT_CNT          1
#define I2C_PNOC_MSTRSLV_PAIRS_NUM	1

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
#endif


/*-------------------------------------------------------------------------
 * Static Variable Definitions
 * ----------------------------------------------------------------------*/
static uint8                         *pPeriphSsBasePtr  = NULL;
static DalDeviceHandle               *phDalHWIO         = NULL;
static DalDeviceHandle               *hTlmm             = NULL;
static uint32                         uTgtCnt           = 0;

/*-------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * ----------------------------------------------------------------------*/

/** @brief Initializes the tlmm handle gpios.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2cPlat_Error.
  */
static int32
I2cPlat_InitTlmm
(
   I2cPlat_DescType *pPlat
)
{
   DALResult dalRes;

   if (NULL == hTlmm) {
      dalRes = DAL_DeviceAttachEx(NULL, DALDEVICEID_TLMM,
                                  DALTLMM_INTERFACE_VERSION, &hTlmm);
      if ( (DAL_SUCCESS != dalRes) || (NULL == hTlmm) ) {
         hTlmm = NULL;     
         I2CDEVICE_LOG(ERROR,"DAL_DeviceAttachEx: DALDevice_Attach Failed result = 0x%x \n", dalRes);                  		 
		 return I2CPLAT_ERROR_FAILED_TO_ATTACH_TO_TLMM;
      }
   
      dalRes = DalDevice_Open(hTlmm, DAL_OPEN_SHARED);
      if ( DAL_SUCCESS != dalRes ) {
         DAL_DeviceDetach(hTlmm);
         hTlmm = NULL;
         I2CDEVICE_LOG(ERROR,"DalDevice_Open: DALDevice_Open Failed result = 0x%x \n", dalRes);                  		 
         return I2CPLAT_ERROR_FAILED_TO_OPEN_TLMM;
      }
   }
   
   return I2C_RES_SUCCESS;
}

/** @brief releases tlmm handle gpios.
              
    @param[in] pDev     Pointer to platform device structure.
              
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2cPlat_Error.
  */
static int32
I2cPlat_DeInitTlmm
(
   I2cPlat_DescType *pPlat
)
{
   DALResult dalRes;
   int32 res = I2C_RES_SUCCESS;

   if (NULL != hTlmm) {
      dalRes = DalDevice_Close(hTlmm);
      if ( DAL_SUCCESS != dalRes ) {
		 I2CDEVICE_LOG(ERROR,"DeInitTlmm: DALDevice_Close Failed result = 0x%x \n", dalRes);                  
         res = I2CPLAT_ERROR_FAILED_TO_OPEN_TLMM;
      }
      dalRes = DAL_DeviceDetach(hTlmm);
      if ( DAL_SUCCESS != dalRes ) {
		 I2CDEVICE_LOG(ERROR,"DeInitTlmm: DAL_DeviceDetach Failed result = 0x%x \n", dalRes);                  
         res = I2CPLAT_ERROR_FAILED_TO_ATTACH_TO_TLMM;
      }
      hTlmm = NULL;
      }
   
   return res;
   }
   
/** @brief Configures and enables I2C gpios.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2cPlat_Error.
  */
int32
I2cPlat_EnableI2cGpios
(
   I2cPlat_DescType *pPlat
)
{
   DALResult dalRes;
   uint32 uGpioInd;

   for (uGpioInd = 0; uGpioInd <  pPlat->props.uNumGpios ; uGpioInd++) {
      dalRes = DalTlmm_ConfigGpio(hTlmm, pPlat->props.aGpioCfg[uGpioInd],
                                  DAL_TLMM_GPIO_ENABLE);
      if ( DAL_SUCCESS != dalRes ) {
	  	 I2CDEVICE_LOG(ERROR,"EnableI2cGpios: Unable to enable i2c GPIO's result = 0x%x \n", dalRes);                  
         return I2CPLAT_ERROR_FAILED_TO_CONFIGURE_GPIO;
      }
   }
   return I2C_RES_SUCCESS;
}

/** @brief disables I2C gpios.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2cPlat_Error.
  */
int32
I2cPlat_DisableI2cGpios
(
   I2cPlat_DescType *pPlat
)
{
   DALResult dalRes;
   uint32 uGpioInd;

      for (uGpioInd = 0; uGpioInd <  pPlat->props.uNumGpios ; uGpioInd++) {
         dalRes = DalTlmm_ConfigGpio(hTlmm, pPlat->props.aGpioCfg[uGpioInd],
                                  DAL_TLMM_GPIO_DISABLE);
         if ( DAL_SUCCESS != dalRes ) {
			I2CDEVICE_LOG(ERROR,"GPIOs_Disable: Unable to disable i2c GPIO's result = 0x%x \n", dalRes);                  
			return I2CPLAT_ERROR_FAILED_TO_CONFIGURE_GPIO;
         }
      }
   return I2C_RES_SUCCESS; 
}


/** @brief Initializes bam.
  
    @param[in]  uPlatDevId  Platform device ID.
    @param[out] pDev        Pointer to platform device
          structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2cPlat_Error.
  */
int32
I2cPlat_ReadPlatConfig
(
   uint32                    uPlatDevId,
   I2cPlat_DescType    *pPlat
)
{
#if 0
// Reading configs is in the I2cPlatConfig.c file for this driver
#endif
   return I2C_RES_SUCCESS;
}

/** @brief Maps the hwio address space.
  
    @param[in] pPlat    Pointer to platform struct.
    @param[in] pProps   Pointer to platform properties.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLAT_Error.
  */
static int32
I2cPlat_InitHwio
(
   I2cPlat_DescType *pPlat,
   I2cPlat_PropertyType  *pProps
)
{
   DALResult dalRes;

   if ( NULL == pPeriphSsBasePtr ) {
      dalRes = DAL_DeviceAttach(DALDEVICEID_HWIO, &phDalHWIO);
      if ( (DAL_SUCCESS != dalRes) || (NULL == phDalHWIO) ) {
	  	 I2CDEVICE_LOG(ERROR,"InitHWIO: Dal Device Attach failure , result = 0x%x \n", dalRes);                  
         return I2CPLAT_ERROR_ATTACH_TO_DALHWIO;
      }

      
      dalRes = DalHWIO_MapRegion(phDalHWIO,pProps->pPeriphSsAddrName,&pPeriphSsBasePtr);
      if ( DAL_SUCCESS !=dalRes){

		 I2CDEVICE_LOG(ERROR,"InitHWIO: Unable to map HWIO Region , result = 0x%x \n", dalRes);                  
         return I2CPLAT_ERROR_FAILED_TO_MAP_BLOCK_HWIO;
       }
   }
   pPlat->pQupAddr = pPeriphSsBasePtr + pPlat->props.coreOffset;
   pPlat->initState |= I2CPLAT_TGT_INIT_HWIO_ALLOCATED;

   return I2C_RES_SUCCESS;
}


/** @brief Registers a suppressible client for i2c.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLAT_Error.
  */

/*
static int32
I2cPlat_RegisterSuppressibleClient
(
   I2cPlat_PropertyType  *pProps
)
{ 
//TODO: Get PNOC voting working
   //if ( NULL == hNpaClient ) {
   //   hNpaClient = npa_create_sync_client("/clk/pnoc", "I2C", NPA_CLIENT_SUPPRESSIBLE);
   //   if( hNpaClient == NULL ) 
   //   {
   //      return I2CPLAT_ERROR_FAILED_TO_CREATE_PNOC_CLIENT;
   //   }
   //}
   //
   //return I2C_RES_SUCCESS;
   
   
   ICBArb_RequestType     i2cPnocRequest;
   ICBArb_MasterSlaveType i2cArbMasterSlave;
   ICBArb_ErrorType       i2cArbError;
   
   i2cArbMasterSlave.eMaster = pProps->uPnocMaster; 
   i2cArbMasterSlave.eSlave  = pProps->uPnocSlave;  

   // How to name the node?
   hNpaClient = icbarb_create_suppressible_client_ex
                                          ("/node/core/bus/i2c/clk", 
   											&i2cArbMasterSlave,
   											I2C_ICB_CLIENT_CNT, NULL );
   if( hNpaClient == NULL) 
   {
     return I2CPLAT_ERROR_FAILED_TO_CREATE_PNOC_CLIENT;
   }  

   i2cPnocRequest.arbType =    ICBARB_REQUEST_TYPE_3;     // Ib/Ab pair
  
   // Bandwidth in Bytes/second 
   i2cPnocRequest.arbData.type3.uIb = (uint64) pProps->uPnocIBVal;      
   i2cPnocRequest.arbData.type3.uAb = (uint64) pProps->uPnocABVal;     

   i2cArbError = icbarb_issue_request(hNpaClient, &i2cPnocRequest,
                                      I2C_PNOC_MSTRSLV_PAIRS_NUM);
   
   if(i2cArbError != ICBARB_ERROR_SUCCESS)
   {
     return I2CPLAT_ERROR_FAILED_TO_ISSUE_PNOC_REQ;
   }

   return I2C_RES_SUCCESS;
}
*/
/** @brief Unregisters suppressible client for i2c.
 
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLAT_Error.
  */
/*
static int32
I2cPlat_UnregisterSuppresibleClient
(
   void
)
{
   if ( NULL != hNpaClient ) {
      npa_destroy_client (hNpaClient);
     hNpaClient = NULL;  
   }
   
   return I2C_RES_SUCCESS; 
}
*/
/** @brief sets the vote for PNOC.
  
    @param[in] pDev     Pointer to platform device structure.
    @param[in] hClnt    Npa client handle.
    @param[in] uIb      PNOC IB value.
    @param[in] uAb      PNOC AB value.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLAT_Error.
  */

int32
I2cPlat_AddPnocVote
(
   I2cPlat_DescType *pDesc
)
{
   ICBArb_RequestType     i2cPnocRequest;
   ICBArb_MasterSlaveType i2cArbMasterSlave;
   ICBArb_ErrorType   i2cArbError;
   	  
   i2cArbMasterSlave.eMaster = pDesc->props.uPnocMaster; 
   i2cArbMasterSlave.eSlave  = pDesc->props.uPnocSlave;  

   /*How to name the node?*/
   pDesc->hNpaClient = icbarb_create_suppressible_client_ex
                                          ("/node/core/bus/i2c/clk", 
   											&i2cArbMasterSlave,
   											I2C_ICB_CLIENT_CNT, NULL );
   if( pDesc->hNpaClient == NULL) 
   {
   	 I2CDEVICE_LOG(ERROR,"AddPNOCVote:Failed to create NPA Client , result = 0x%x \n", pDesc->hNpaClient); 
     return I2CPLAT_ERROR_FAILED_TO_CREATE_PNOC_CLIENT;
   }  

   i2cPnocRequest.arbType =    ICBARB_REQUEST_TYPE_3;     /* Ib/Ab pair */
  
   /* Bandwidth in Bytes/second */
   i2cPnocRequest.arbData.type3.uIb = (uint64) pDesc->props.uPnocIBVal;      
   i2cPnocRequest.arbData.type3.uAb = (uint64) pDesc->props.uPnocABVal;     

   i2cArbError = icbarb_issue_request(pDesc->hNpaClient, &i2cPnocRequest,
                                      I2C_PNOC_MSTRSLV_PAIRS_NUM);
   
   if(i2cArbError != ICBARB_ERROR_SUCCESS)
   {
     I2CDEVICE_LOG(ERROR,"AddPNOCVote:Failed to Issue PNOC Req , result = 0x%x \n", i2cArbError); 
     return I2CPLAT_ERROR_FAILED_TO_ISSUE_PNOC_REQ;
   }
    
   pDesc->initState |= I2CPLAT_TGT_INIT_PNOC_INITED;
   return I2C_RES_SUCCESS;
}

/** @brief unvote for PNOC.
  
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLAT_Error.
  */

int32
I2cPlat_RemovePnocVote
(
   I2cPlat_DescType *pDesc
)
{ 
   if ( pDesc->initState & I2CPLAT_TGT_INIT_PNOC_INITED) {
     icbarb_complete_request(pDesc->hNpaClient);
     icbarb_destroy_client(pDesc->hNpaClient);  
   }
   return I2C_RES_SUCCESS; 
}

/** @brief detaches from DALDEVICEID_HWIO.
  
    @param[in] pDev     Pointer to platform device structure.
    
    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2CPLAT_Error.
  */
static int32
I2cPlat_DeInitHwio
(
   I2cPlat_DescType     *pDev
)
{
   DALResult dalRes;


   if ( NULL != phDalHWIO ) {
      dalRes = DAL_DeviceDetach(phDalHWIO);
      if ( DAL_SUCCESS != dalRes ) {
	  	 I2CDEVICE_LOG(ERROR,"DeInitHwio: DeInitHwio failed, result = 0x%x \n", dalRes);         
         return I2CPLAT_ERROR_DETACH_FROM_DALHWIO;
      }
   }
   pDev->initState &= ~I2CPLAT_TGT_INIT_HWIO_ALLOCATED;
   phDalHWIO         = NULL;
   return I2C_RES_SUCCESS;
}

/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/



/** @brief Initializes data structures necessary to support the target calls.
 
 @param[in] pPlat  pointer to platform device structure.
 @param[in] pProps pointer to platform properties.
 
 @return          int32 .
 */
int32
I2cPlat_InitTarget
(
 I2cPlat_DescType      *pPlat ,
 I2cPlat_PropertyType  *pProps
 )
{
   int32                           res;

   do
   {
	  uTgtCnt++;
      
	  pPlat->initState = I2CPLAT_TGT_INIT_NOT_DONE;
	  pPlat->props = *pProps;
      	  
      res = I2cPlat_InitTlmm(pPlat);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
	  
      res = I2CPLATDEV_InitClocks(pPlat);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
	  
      res = I2cPlat_InitHwio(pPlat, pProps);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
     
      res = I2C_RES_SUCCESS;
   } while ( 0 );

   if ( I2C_RES_SUCCESS != res ) {
      I2cPlat_DeInitTarget(pPlat);
   }
   
   return res;
}


/** @brief Releases data structures necessary to support the target calls.

    @param[in] pPlat platform device pointer.

    @return          int32 .
  */
int32
I2cPlat_DeInitTarget
(
   I2cPlat_DescType *pPlat
)
{
   int32      res, ret = I2C_RES_SUCCESS;

   uTgtCnt--;
   
   if ( 0 == uTgtCnt) {
      res = I2cPlat_DeInitTlmm(pPlat);
	  if ( I2C_RES_SUCCESS != res ) {
	     ret = res;     
	  }
	
	  res = I2CPLATDEV_DeInitClocks(pPlat);
	  if ( I2C_RES_SUCCESS != res ) {
         ret = res;     
	  }

	  res = I2cPlat_DeInitHwio(pPlat);
	  if ( I2C_RES_SUCCESS != res ) {
	     ret = res;     
	  }
   }

   return ret;
}

/** @brief Returns the QUP virtual address.

    @param[in]  pPlat platform device pointer.

    @return          int32 .
  */
uint8*
I2cPlat_GetQupAddress
(
   I2cPlat_DescType      *pPlat
)
{
   return pPlat->pQupAddr;

}

/**
    This Function registers IST callback.

    @param[in]  pPlat        Platform device pointer.
    @param[in]  IsrHook      ISR function pointer.
    @param[in]  pIsrData     pointer to ISR callback data.
    @param[in]  IstHook      IST function pointer.
    @param[in]  pIstData     pointer to IST callback data.

    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2cPlat_Error.
  */
int32
I2cPlat_RegisterIstHooks
(
   I2cPlat_DescType  *pDev,
   ISR_HOOK                IsrHook,
   void*                   pIsrData,
   IST_HOOK                IstHook,
   void*                   pIstData
)
{
#if 0
   DALResult                     dalRes;
   
   if ( NULL == pDev->hInterruptCtrl ) {
      dalRes =
          DAL_InterruptControllerDeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER,
                                              &pDev->hIrqCtrl);
      if( (DAL_SUCCESS != dalRes) || (NULL == pDev->hIrqCtrl) ) {
         return I2CPLAT_ERROR_FAILED_ATTACH_TO_IRQCTRL; 
      }
   }

   dalRes = DalInterruptController_RegisterISR(
                        pDev->hIrqCtrl,
                        pDev->props.uInterruptId,
                        IstHook,
                        (DALISRCtx)pIstData,
                        DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER);
   if ( DAL_SUCCESS != dalRes ) {
      return I2CPLAT_ERROR_FAILED_TO_REGISTER_IST;
   }
#endif
   return I2C_RES_SUCCESS;
}

/**
    This Function unregisters IST and ISR callbacks.

    @param[in]  hPlatDev     Platform handle.

    @return             I2C_RES_SUCCESS is successful, otherwise
                        I2cPlat_Error.
  */
int32
I2cPlat_UnRegisterIstHooks
(
   I2cPlat_DescType  *pDev
)
{
#if 0
   DALResult                     dalRes;
   
   if ( NULL == pDev->hInterruptCtrl ) {
      return I2CPLAT_ERROR_IRQCTRL_NOT_INITIALISED; 
   }

   dalRes = DalInterruptController_Unregister(pDev->hIrqCtrl, pDev->props.uInterruptId);
   if ( DAL_SUCCESS != dalRes ) {
      return I2CPLAT_ERROR_FAILED_TO_UNREGISTER_IST;
   }
#endif
   return I2C_RES_SUCCESS;
}

