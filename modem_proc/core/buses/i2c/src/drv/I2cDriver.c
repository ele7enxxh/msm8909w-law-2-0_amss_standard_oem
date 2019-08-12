/*=============================================================================

  FILE:   I2cDriver.c

  OVERVIEW:     This file contains the implementation of QUPI2CDriver API
  DEPENDENCIES: I2CDRV_AddDevice must be called first then other api's.
 
                Copyright c 2011-2016 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
  ===========================================================================*/

/*=========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/buses/i2c/src/drv/I2cDriver.c#1 $
  $DateTime: 2016/12/13 07:59:23 $$Author: mplcsds1 $

  When     Who    What, where, why
  -------- ---    -----------------------------------------------------------
  03/15/16 VG     Added ClkRefCnt for clock state
  08/25/15 VG     Added changes for I2C Lite driver
  08/20/14 MS     Created from 8916 MPSS

  ===========================================================================*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "I2cTypes.h"
#include "I2cDriver.h"
#include "I2cDriverTypes.h"
#include "I2cError.h"
#include "I2cDevice.h"
#include "I2cPlatSvc.h"
#include "I2cSys.h"
#include "I2cPlatConfig.h"
#include "I2cBsp.h"
#include "I2cPlatSvcTypes.h"
#include "I2cLog.h"
/*-------------------------------------------------------------------------
 * Global Data Definitions
 * ----------------------------------------------------------------------*/
I2cPlat_PropertyType i2cPlatPropertyArray[I2C_NUM_PLATFORM_DEVICES];
I2cDev_PropertyType i2cDevPropertyArray[I2C_NUM_PLATFORM_DEVICES];
I2cDrv_DriverProperty i2cDrvPropertyArray[I2C_NUM_PLATFORM_DEVICES];
I2cDrv_DescType i2cDrvDescArray[I2C_NUM_PLATFORM_DEVICES];


I2cDrv_DescType          *aI2cDev[I2CDRV_I2C_NUM];
I2cSys_SyncType           i2cDrvSync = I2CSYS_SYNC_INIT;
/** @brief This is a hook for the thread servicing the qup.

    This Function is called by the framework to handle the data
    transfer in response to qup irq events.

    @param[in] pdata    void pointer to data.

    @return             Nothing.
  */
void 
I2cDrv_IST_HOOK
(
   void *pdata
)
{
   I2cDrv_DescType *pDesc;
   I2cDev_State     state;
   
   if ( NULL == pdata) {
      return;
   }
   pDesc = (I2cDrv_DescType *)pdata;
   I2cSys_EnterCriticalSection(&pDesc->serialOpSync);
   I2cDev_EventCall(&pDesc->dev, I2CDEV_EVT_ISR, NULL);
   I2cDev_EventCall(&pDesc->dev, I2CDEV_EVT_IST, NULL);
   
   I2cDev_GetState(&pDesc->dev, &state);
   if ( I2CDEV_SEQSTATE_COMPLETE  == state.eIoState ) {
      I2cSys_SetEvent(&pDesc->qupEvent);
   }
   I2cSys_LeaveCriticalSection(&pDesc->serialOpSync);
}

/** @brief Polled wait for transfer completion.
 
    @param[in] pDesc          Pointer to sequence.
    @param[in] pSequence      Pointer to sequence.
    @param[in] pClntCfg       Pointer to Client configuration.
                           If null keep previous.
 
 
    @return             Nothing.
  */
static void
I2cDrv_SubmitAndPolledWait
(
   I2cDrv_DescType                      *pDesc,
   I2cSequence                          *pSequence,
   I2cClientConfig                      *pClntCfg
)
{
   I2cDev_State         *pState    = &pDesc->devState;
   I2cDev_Device        *pDev      = &pDesc->dev;
   uint32                uTimeout;
   int32                 res;
   
   res = I2cDev_SubmitSequence(&pDesc->dev, pSequence, pClntCfg);
   if ( I2C_RES_SUCCESS != res ) {
      pState->opResult.nOperationResult = res;
      return;
   }
   I2cDev_GetState(pDev, pState); // call get state to update the timeout
   uTimeout = pState->uTimeoutUs;
   
   while ( (!I2cDev_IsTransferComplete(pDev)) && uTimeout ) {
      I2cSys_BusyWait(5);     
      I2cDev_EventCall(pDev, I2CDEV_EVT_POLL, NULL);
      if (uTimeout > 5) {
         uTimeout -= 5;
      }
      else {
         uTimeout = 0;
      }
   }
   if (0 == uTimeout) {
      I2cDev_AbortSequence(pDev);
   }
   I2cDev_GetState(pDev, pState);
}
/** @brief Polled wait for transfer completion.
    
    @param[in] pDesc          Pointer to sequence.
    @param[in] pSequence      Pointer to sequence.
    @param[in] pClntCfg       Pointer to Client configuration.
                              If null keep previous.
 
 
    @return             Nothing.
  */
static void
I2cDrv_SubmitAndEventWait
(
   I2cDrv_DescType                      *pDesc,
   I2cSequence                          *pSequence,
   I2cClientConfig                      *pClntCfg
)
{
   int32                 res       = I2C_RES_SUCCESS;
   I2cDev_State         *pState    = &pDesc->devState;
   I2cDev_Device        *pDev      = &pDesc->dev;
   I2cSys_EventType     *pEvent    = &pDesc->qupEvent;
   
   I2cSys_EnterCriticalSection(&pDesc->serialOpSync);
   res = I2cDev_SubmitSequence(&pDesc->dev, pSequence, pClntCfg);
   I2cSys_LeaveCriticalSection(&pDesc->serialOpSync);
   if ( I2C_RES_SUCCESS != res ) {
      pState->opResult.nOperationResult = res;
      return;
   }
   I2cDev_GetState(pDev, pState); // call this to update timeout
   res = I2cSys_Wait(pEvent, pState->uTimeoutUs);
   if (I2C_RES_SUCCESS != res) {
      I2cDev_AbortSequence(pDev);
   }
   I2cDev_GetState(pDev, pState);
}

/** @brief waits for transfer completion.
 
    @param[in] pDesc          Pointer to sequence.
    @param[in] pSequence      Pointer to sequence.
    @param[in] pClntCfg       Pointer to Client configuration.
                              If null keep previous.
 
 @return             Nothing.
  */
static void
I2cDrv_SubmitAndWaitForCompletion
(
   I2cDrv_DescType                      *pDesc,
   I2cSequence                          *pSequence,
   I2cClientConfig                      *pClntCfg
)
{
   I2cDrv_DriverProperty *pDrvProps = pDesc->pDrvProps;
   
   if ( !pDrvProps->bInterruptBased ) {
      I2cDrv_SubmitAndPolledWait(pDesc, pSequence, pClntCfg);
   }
   else {
      I2cDrv_SubmitAndEventWait(pDesc, pSequence, pClntCfg);
   }
}




/*-------------------------------------------------------------------------
 * Externalized Function Definitions
 * ----------------------------------------------------------------------*/

/** @brief Initializes the device.

    This Function opens the i2c bus and initializes the i2c bus structure.
    Also this function votes to turn on power.

    @param[in] eI2cBusId     I2C Bus ID.
    @param[in] pI2cBus       Pointer to I2C bus struct.
    @param[in]  dwaccessMode Access mode.

    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2cDrv_Open
(
   I2cDrv_I2cBusId                          eI2cBusId,
   I2cDrv_I2cBus                           *pI2cBus  ,
   uint32                                   dwaccessMode
)
{
   int32 res = I2C_RES_SUCCESS;
   I2cDrv_DescType *pDesc;
   
   
   I2cLog_Init();
   I2cBsp_Init();

   RETURN_IF_INVALID_BUS_ID(eI2cBusId);
   
   if (NULL == pI2cBus) {
      I2CDEVICE_LOG(ERROR,"i2c_open: Invalid device id \n");
      return I2CDRV_ERR_NULL_BUS_PTR;     
   }
   // the driver sync is used to protect against remove.
   I2cSys_EnterCriticalSection(&i2cDrvSync);
   pDesc = aI2cDev[eI2cBusId];
   if (NULL != pDesc) {
      /* add accounting. */
      I2cSys_EnterCriticalSection(&pDesc->serialReqSync);
      pDesc->opState |= I2CDRV_OPSTATE_DEV_OPEN;
      
      pDesc->uOpenCnt++;  
      if( 1 == pDesc->uOpenCnt) { //first usage of this QUP device
         I2cPlat_EnableI2cGpios(&pDesc->plat);
      }
      pI2cBus->i2cId = eI2cBusId;
	  pI2cBus->cookie = I2C_CLIENT_COOKIE;
      I2cSys_LeaveCriticalSection(&pDesc->serialReqSync);
   }
   else {
   	  res = I2CDRV_ERR_DEVICE_NOT_INITIALISED;
	  I2CDEVICE_LOG(ERROR,"i2_open: I2CDrv_open failed, result = 0x%x \n", res);         
   }
   
   I2cSys_LeaveCriticalSection(&i2cDrvSync);
   
   return res;
}/* I2cDrv_Open */

/**
    This function is the main device close  function.

    @param[in] pI2cBus       Pointer to I2C bus struct.

    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2cDrv_Close
(
   I2cDrv_I2cBus                           *pI2cBus
)
{
   int32 res = I2C_RES_SUCCESS;
   I2cDrv_DescType *pDesc;

   RETURN_IF_INVALID_BUS_PTR(pI2cBus);

   // the driver sync is used to protect agains remove.
   I2cSys_EnterCriticalSection(&i2cDrvSync);
   pDesc = aI2cDev[pI2cBus->i2cId];
   if (NULL != pDesc) {
      /* add accounting. */
      I2cSys_EnterCriticalSection(&pDesc->serialReqSync);

      if( 1 == pDesc->uOpenCnt ) {
         pDesc->opState &= ~I2CDRV_OPSTATE_DEV_OPEN;
      }

	  pDesc->uOpenCnt--;

	  if( 0 == pDesc->uOpenCnt) {
         I2cPlat_DisableI2cGpios(&pDesc->plat);
      }

      pI2cBus->i2cId  = I2CDRV_I2C_DEV_INVALID;
      pI2cBus->cookie = 0;
 
      I2cSys_LeaveCriticalSection(&pDesc->serialReqSync);
   }
   else {
      res = I2CDRV_ERR_DEVICE_NOT_INITIALISED;
   }
   
   I2cSys_LeaveCriticalSection(&i2cDrvSync);
 
   return res;
}/* I2cDrv_Close */

/** @brief Read a buffer from i2c device.

    Read a buffer from i2c device.

    @param[in] pI2cBus         Pointer to I2C bus struct.
    @param[in] pTransfer       Pointer where transfer data will
          be stored.
    @param[in] pClntCfg        Pointer to Client configuration.
                                If null keep previous.
    @param[in] puNumCompleted  Pointer to return completed
          reads.

    @return             I2C_RES_SUCCESS if successful, error
                        otherwise.
  */
int32
I2cDrv_Read
(
   I2cDrv_I2cBus                        *pI2cBus,
   I2cTransfer                          *pTransfer,
   uint32                               *puNumCompleted
)
{
   I2cSequence            seq;
   I2cIoResult            ioRes;
   int32                  res;

   seq.pTransfer = pTransfer;
   seq.uNumTransfers = 1;
   res = I2cDrv_BatchTransfer(pI2cBus, &seq, &ioRes);
   *puNumCompleted = ioRes.uInByteCnt;

   return res;
} 

/** @brief Write a buffer to i2c device.

    Write a buffer to i2c device.

    @param[in] pI2cBus         Pointer to I2C bus struct.
    @param[in] pTransfer       Pointer to transfer data.
    @param[in] puNumCompleted  Pointer to return completed
          bytes.

    @return          I2C_RES_SUCCESS if successful, error
                     otherwise.
  */
int32
I2cDrv_Write
(
   I2cDrv_I2cBus             *pI2cBus,
   I2cTransfer               *pTransfer,
   uint32                    *puNumCompleted
)
{
   I2cSequence            seq;
   I2cIoResult            ioRes;
   int32                  res;

   seq.pTransfer = pTransfer;
   seq.uNumTransfers = 1;
   res = I2cDrv_BatchTransfer(pI2cBus, &seq, &ioRes);
   *puNumCompleted = ioRes.uOutByteCnt;
   
   return res;
}

/** @brief Does a batch of transfers in a sequence.

    Does a batch of transfers in a sequence.

    @param[in] pI2cBus        Pointer to I2C bus struct.
    @param[in] pSequence      Pointer to a sequence of transfer
          data.
    @param[in] pIoRes         Pointer to io result.

    @return          I2C_RES_SUCCESS if successful, error
                     otherwise.
  */
int32
I2cDrv_BatchTransfer
(
   I2cDrv_I2cBus                       *pI2cBus,
   I2cSequence                         *pSequence,
   I2cIoResult                         *pIoRes
)
{
   I2cDrv_DescType *pDesc;
   int32            res;
   I2cDev_State         *pState;

   RETURN_IF_INVALID_BUS_PTR(pI2cBus);
   RETURN_IF_INVALID_TRAN(pSequence,pIoRes);

   pDesc = aI2cDev[pI2cBus->i2cId];
   I2cSys_EnterCriticalSection(&pDesc->serialReqSync);
   pState    = &pDesc->devState;
 
	   do{
		     if (pI2cBus->cookie != I2C_CLIENT_COOKIE) {
			    res = I2CDRV_ERR_INVALID_CLIENT_COOKIE;
				break;
			 }
			 if (NULL == pDesc) {
			    res = I2CDRV_ERR_DEVICE_NOT_INITIALISED;
			    break;
			 }
			 
			 if ( !I2C_IS_DEVICE_POWERED(pDesc) ) {
			    res = I2CDRV_ERR_DEVICE_NOT_POWERED;
			    break;
			 }

			 I2cDrv_SubmitAndWaitForCompletion(pDesc, pSequence, &pI2cBus->clntCfg);
			 *pIoRes = pState->opResult;
			 res = pState->opResult.nOperationResult;
	     }while (0);
		  
		 if ( res != I2C_RES_SUCCESS ) {
		    I2CDEVICE_LOG(ERROR,"I2cDrv_BatchTransfer:Unable to Initiate RWs.Result = 0x%x \n", res);
		 }
 
    I2cSys_LeaveCriticalSection(&pDesc->serialReqSync);
    return res;
}

/** @brief Send a command to the i2c driver.

    @param[in] pI2cBus         Pointer to I2C bus struct.
    @param[in] cmd             command to send to the driver.
    @param[in] pData           data related to the command.

    @return          I2C_RES_SUCCESS if successful, error
                     otherwise.
  */
int32
I2cDrv_Cmd
(
   I2cDrv_I2cBus             *pI2cBus,
   I2cDrvCmdType              cmd    ,
   void                      *pData
)
{
   I2cDrv_DescType *pDesc;
   int32            res = I2C_RES_SUCCESS;

   RETURN_IF_INVALID_BUS_PTR(pI2cBus);
   
   if (pI2cBus->cookie != I2C_CLIENT_COOKIE) {
      return I2CDRV_ERR_INVALID_CLIENT_COOKIE;
   }
   pDesc = aI2cDev[pI2cBus->i2cId];
   I2cSys_EnterCriticalSection(&pDesc->serialReqSync);
   switch (cmd) {
   case I2cDrvCmd_Vote_PnocOn:
      res = I2cPlat_AddPnocVote(&pDesc->plat); 
	  if ( I2C_RES_SUCCESS == res ) { 
	     pDesc->pwState |= I2CDRV_PWSTATE_PNOC_ON;
	  }
	  break;

   case I2cDrvCmd_Vote_PnocOff :
      res = I2cPlat_RemovePnocVote(&pDesc->plat);
      if ( I2C_RES_SUCCESS == res ) { 
	     pDesc->pwState &= ~I2CDRV_PWSTATE_PNOC_ON;
      }
	  break;

   case I2cDrvCmd_Vote_I2cClocsOn:
      if(0 == pDesc->clkRefCnt){
	     res = I2cPlat_VoteI2cClkOn(&pDesc->plat);
		 if ( I2C_RES_SUCCESS == res ) { 
		    pDesc->pwState |= I2CDRV_PWSTATE_I2C_CLKS_ON;
		 }
		 else{
		    I2CDEVICE_LOG(ERROR,"I2cDrvCmd:Unable to on I2cPlat_VoteI2cClk.Result = 0x%x \n",res);
		 }
	  }
	  pDesc->clkRefCnt ++;
	  break;

   case I2cDrvCmd_Vote_I2cClocsOff :
      if(1 == pDesc->clkRefCnt){
         res = I2cPlat_VoteI2cClkOff(&pDesc->plat);
		 if ( I2C_RES_SUCCESS == res ) { 
		    pDesc->pwState &= ~I2CDRV_PWSTATE_I2C_CLKS_ON;
		 }
		 else{
		    I2CDEVICE_LOG(ERROR,"I2cDrvCmd:failed to I2cPlat_VoteI2cClkOff.Result = 0x%x \n",res);
		 }
	  }
	  if(pDesc->clkRefCnt > 0 ){
	     pDesc->clkRefCnt --;
	  }
	  break;
	   
   }
   I2cSys_LeaveCriticalSection(&pDesc->serialReqSync); 
   return res;
}

