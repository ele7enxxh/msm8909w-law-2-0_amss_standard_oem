/*=============================================================================

  FILE:   I2cDriver.c

  OVERVIEW:     This file contains the implementation of QUPI2CDriver API
  DEPENDENCIES: I2cDrv_AddDevice must be called first then other api's.
 
                Copyright c 2015 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
  ===========================================================================*/

/*=========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/drv/I2cDriverInit.c#1 $
  $DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $

  When     Who    What, where, why
  -------- ---    -----------------------------------------------------------
  08/25/15 VG     Created

  ===========================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

#include "I2cDriver.h"
#include "I2cDriverTypes.h"
#include "I2cError.h"
#include "I2cDevice.h"
#include "I2cPlatSvc.h"
#include "I2cSys.h"
#include "I2cBsp.h"
#include "I2cLog.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
void 
I2cDrv_IST_HOOK
(
   void *pdata
) ;

/*-------------------------------------------------------------------------
 * Static Variable Definitions
 * ----------------------------------------------------------------------*/
/* Array of pointers to device structures. */

boolean                   bSyncInitialised = FALSE;



/*-------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * ----------------------------------------------------------------------*/


/** @brief This function initialises the event structures.
 
 
 @param[in] pDesc    Pointer to driver structure.
 
 @return             I2C_RES_SUCCESS if successful, error otherwise.
 */

static int32
I2cDrv_InitEvents
(
   I2cDrv_DescType* pDesc
)
{
   int32 res = I2C_RES_SUCCESS;
   enum I2cDrv_InitEvents_State
   {
      I2CDRV_IE_STATE_START,
      I2CDRV_IE_STATE_OP_SYNC_CREATED,
      I2CDRV_IE_STATE_IST_EVENTS_CREATED,
      I2CDRV_IE_STATE_IST_REGISTERED,

   } initEvtsState;
   I2cDrv_DriverProperty *pDrvProps = pDesc->pDrvProps;

   if ( !pDrvProps->bInterruptBased ) {
      return I2C_RES_SUCCESS;
   }

   do {
      initEvtsState = I2CDRV_IE_STATE_START;
      res = I2cSys_CreateCriticalSection(&pDesc->serialOpSync);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      initEvtsState = I2CDRV_IE_STATE_OP_SYNC_CREATED;
      
      res = I2cSys_CreateEvent(&pDesc->qupEvent);
      if ( I2C_RES_SUCCESS != res ) {
          break;
      }
      initEvtsState =       I2CDRV_IE_STATE_IST_EVENTS_CREATED;
      
      res = I2cPlat_RegisterIstHooks(&pDesc->plat, NULL, NULL, I2cDrv_IST_HOOK, (void*) pDesc);
      if ( I2C_RES_SUCCESS != res ) {
         break;
      }
      initEvtsState = I2CDRV_IE_STATE_IST_REGISTERED;
   } while(0);
    
   if ( I2C_RES_SUCCESS != res ) {
      switch (initEvtsState) {
         case I2CDRV_IE_STATE_IST_EVENTS_CREATED:
            I2cSys_DestroyEvent(&pDesc->qupEvent);
            
         case I2CDRV_IE_STATE_OP_SYNC_CREATED:
            I2cSys_DestroyCriticalSection(&pDesc->serialOpSync);
            
         default:
            break;
      }
    }
    
    return res;
}


/** @brief This function initialises the event structures.
 
 
 @param[in] pDesc    Pointer to driver structure.
 
 @return             I2C_RES_SUCCESS if successful, error otherwise.
 */

static int32
I2cDrv_DeInitEvents
(
   I2cDrv_DescType* pDesc
)
{
   int32 critRes,evtRes, istRes;
   I2cDrv_DriverProperty *pDrvProps = pDesc->pDrvProps;
    
   if ( !pDrvProps->bInterruptBased ) {
      return I2C_RES_SUCCESS;
   }
   
   critRes = I2cSys_DestroyCriticalSection(&pDesc->serialOpSync);
   evtRes = I2cSys_DestroyEvent(&pDesc->qupEvent);
   istRes = I2cPlat_UnRegisterIstHooks(&pDesc->plat);

   if ( I2C_RES_SUCCESS != critRes ) {
	   return critRes;
	}
	else if ( I2C_RES_SUCCESS != evtRes ) {
	   return evtRes;
	}
	else if ( I2C_RES_SUCCESS != istRes ) {
	   return istRes;
	}
    
   return I2C_RES_SUCCESS;
}


/** @brief Adds a device and sets the handle.
  
    @param[in]  eI2cBusId  I2C bus id.
    @param[in] pDesc       Pointer to the I2C driver structure to be inited.
    
    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
static int32
I2cDrv_AddDevice
(
   I2cDrv_I2cBusId  eI2cBusId,
   I2cDrv_DescType  *pDesc
)
{
   int32 res = I2C_RES_SUCCESS;
   enum I2cDrv_InitState{
	   I2CDRV_INITSTATE_UNINITIALISED,
      I2CDRV_INITSTATE_CRITSEC_INITED,
      I2CDRV_INITSTATE_EVENTS_INITED,
	   I2CDRV_INITSTATE_TARGET_INITED,
	   I2CDRV_INITSTATE_DEVICE_INITED,
   } eAddDeviceState;

   //DALSYS_memset(pDesc, 0x0, sizeof(I2cDrv_DevType));
   eAddDeviceState = I2CDRV_INITSTATE_UNINITIALISED;

   do
   {
      res = I2cSys_CreateCriticalSection(&pDesc->serialReqSync);
      if ( I2C_RES_SUCCESS != res ) {
	     I2CDEVICE_LOG(ERROR,"I2cDrv_AddDevice: I2cSys_CreateCriticalSection failed, result = 0x%x \n", res);
         break;
      }
      eAddDeviceState = I2CDRV_INITSTATE_CRITSEC_INITED;

      res = I2cDrv_InitEvents(pDesc);
      if ( I2C_RES_SUCCESS != res ) {
	     I2CDEVICE_LOG(ERROR,"I2cDrv_AddDevice: I2cDrv_InitEvents failed, result = 0x%x \n", res);
         break;
      }
      eAddDeviceState = I2CDRV_INITSTATE_EVENTS_INITED;
       
	  res = I2cPlat_InitTarget(&pDesc->plat, pDesc->pPlatProps);
	  if ( I2C_RES_SUCCESS != res ) {
	     I2CDEVICE_LOG(ERROR,"I2cDrv_AddDevice: I2cPlat_InitTarget failed, result = 0x%x \n", res);
	     break;
	  }
	  eAddDeviceState = I2CDRV_INITSTATE_TARGET_INITED;
     
     pDesc->pDevProps->virtBlockAddr = I2cPlat_GetQupAddress(&pDesc->plat);
	  res = I2cDev_Init(&pDesc->dev, pDesc->pDevProps);
     if ( I2C_RES_SUCCESS != res ) {
	    I2CDEVICE_LOG(ERROR,"I2cDrv_AddDevice: I2cDev_Init failed, result = 0x%x \n", res);
        break;
     }
     eAddDeviceState = I2CDRV_INITSTATE_DEVICE_INITED;
     aI2cDev[eI2cBusId] = pDesc;
   } while ( 0 );

   if ( I2C_RES_SUCCESS != res ) {
      switch ( eAddDeviceState ) {
              
	      case I2CDRV_INITSTATE_TARGET_INITED:
            I2cPlat_DeInitTarget(&pDesc->plat);
              
         case I2CDRV_INITSTATE_EVENTS_INITED:
            I2cDrv_DeInitEvents(pDesc);

	      case I2CDRV_INITSTATE_CRITSEC_INITED:
            I2cSys_DestroyCriticalSection(&pDesc->serialReqSync);

         default:
            break;
	    }
   }
      
   return res;
}

/** @brief Removes a device from the list and releases the
    	   object.
  
    @param[in]  eI2cBusId  device id.
    
    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
static int32
I2cDrv_RemoveDevice
(
   I2cDrv_I2cBusId  eI2cBusId
)
{
   int32 res0,res1,res2, res3;
   I2cDrv_DescType *pDesc;

   pDesc = aI2cDev[eI2cBusId];
   if (NULL == pDesc ) {
      return I2C_RES_SUCCESS; // no device to remove
   }
   res0 = res1 = res2 = res3 = I2C_RES_SUCCESS;

   do 
   {
      I2cSys_EnterCriticalSection(&pDesc->serialReqSync);
      if (pDesc->opState & I2CDRV_OPSTATE_DEV_BUSY) {
	      res0 = I2CDRV_ERR_DEVICE_BUSY;
         break;
      }
      res1 = I2cDev_DeInit(&pDesc->dev);
      res2 = I2cPlat_DeInitTarget(&pDesc->plat);

      res3 = I2cDrv_DeInitEvents(pDesc);

      aI2cDev[eI2cBusId] = NULL;
      I2cSys_LeaveCriticalSection(&pDesc->serialReqSync);
      I2cSys_DestroyCriticalSection(&pDesc->serialReqSync);
   }while(0);
   
   if ( I2C_RES_SUCCESS != res0 ) {
      return res0;     
   }
   else if ( I2C_RES_SUCCESS != res1 ) {
	  return res1;
   }
   else if ( I2C_RES_SUCCESS != res2 ) {
	  return res2;
   }
   else if ( I2C_RES_SUCCESS != res3 ) {
	  return res3;
   }
      
   return I2C_RES_SUCCESS;
}

/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/

/** @brief Initializes the driver.

    This Function initializes the device descriptor and data structures to support other calls into the
    driver.

    @param[in] eI2cBusId     I2C Bus ID.
    @param[in] pDesc         Pointer to the I2C driver structure to be inited.
                             This structure should be persistent.

    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2cDrv_InitDevice
(
   I2cDrv_I2cBusId  eI2cBusId,
   I2cDrv_DescType* pDesc
)
{
   int32 res;
   RETURN_IF_INVALID_BUS_ID(eI2cBusId);
   I2cSys_EnterCriticalSection(&i2cDrvSync);
   res = I2cDrv_AddDevice(eI2cBusId, pDesc);
   I2cSys_LeaveCriticalSection(&i2cDrvSync);
   return res;
}

/** @brief Deinitializes the device.

    This Function Deinitializes the device and releases
    resources acquired during init.
    
    @param[in] eI2cBusId     I2C Bus ID.

    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2cDrv_DeInitDevice
(
   I2cDrv_I2cBusId  eI2cBusId
)
{
   int32 res;

   RETURN_IF_INVALID_BUS_ID(eI2cBusId);
   I2cSys_EnterCriticalSection(&i2cDrvSync);
   res = I2cDrv_RemoveDevice(eI2cBusId);
   I2cSys_LeaveCriticalSection(&i2cDrvSync);
   return res;
}

/** @brief Initializes the driver.

    This Function Initializes the driver and creates the
    necessary data structures to support other calls into the
    driver.


    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2cDrv_Init
(
   void
)
{
   return I2cSys_CreateCriticalSection(&i2cDrvSync);
}

/** @brief Deinitializes the device.

    This Function Deinitializes the driver and releases
    resources acquired during init.
    

    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2cDrv_DeInit
(
   void
)
{
   return I2cSys_DestroyCriticalSection(&i2cDrvSync);
}
