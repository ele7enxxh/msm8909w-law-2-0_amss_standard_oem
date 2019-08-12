/*
===========================================================================

FILE:   SpiDeviceBam.c

DESCRIPTION:

===========================================================================

        Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spi/src/device/SpiDeviceBam.c#1 $

When     Who    What, where, why
-------- ---    -----------------------------------------------------------
04/25/16 dpk    Added BAM Pipe deinit.
05/30/12 ddk    Added support for common qup hal.
04/20/12 ddk    Added some more logging.
04/20/12 ddk    Added updates to enable logging.
04/19/12 ddk    Added changes for avoiding symbol match with I2C library.
04/07/12 ddk    Updated BAM flags being used.
03/29/12 ddk    Added deinit steps.
12/10/12 ag     Created.
===========================================================================
        Copyright c 2011-2016 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/
#include "SpiDeviceBam.h"
#include "SpiDeviceOsSvc.h"
#include <string.h>
#include "SpiDevicePlatSvc.h"
#define SPIDEVICE_BAM_LOG_H
#include "SpiLog.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define SPIDEVICEBAM_CHK_RESULT_BREAK(res)\
   if (SPIDEVICE_RES_SUCCESS !=res) {\
      break;\
   }
#define SPI_BAM_TIMEOUT     (200*1000) //200 ms
#define SPI_BAM_WAIT_PER_ITER  50 //in microseconds
#define SPI_BAM_MAX_ITERATIONS (SPI_BAM_TIMEOUT/SPI_BAM_WAIT_PER_ITER)
void busywait (uint32 pause_time_us);

static int32 spiDeviceBamPollWait(bam_handle pipeHandle, bam_result_type *pipeRes)
{
    int32 res = SPIDEVICE_RES_ERROR_CLS_QUP_DEV_TIMEOUT;
    uint32 timeout = 0;
    if ((NULL == pipeHandle) || (res == NULL))
    {
        return -1;
    }
    SpiDeviceOsSvc_Memset((void *)pipeRes, 0, sizeof(bam_result_type));
    //Set up polling wait loop for BAM operation
    do 
    {
        if (BAM_SUCCESS != bam_pipe_poll(pipeHandle, pipeRes))
        {
            break;
        }

        if ((BAM_EVENT_EOT == pipeRes->event) ||(BAM_EVENT_DESC_DONE == pipeRes->event))
        {
           res = SPIDEVICE_RES_SUCCESS;
           break;
        }

        busywait(SPI_BAM_WAIT_PER_ITER);
        timeout++;
    } while (pipeRes->event == BAM_EVENT_INVALID  && (timeout < SPI_BAM_MAX_ITERATIONS));

    if ( timeout >= SPI_BAM_MAX_ITERATIONS )
    {
      res = SPIDEVICE_RES_ERROR_CLS_QUP_DEV_TIMEOUT;
    }


  return res;
}


static SpiDeviceBamResult *spiDeviceBamPoll(SpiDeviceBamType *spiBam)
{
   int32 res = 0;
   if (NULL == spiBam)
   {
      return NULL;
   }
   //Set up polling wait loop for BAM operation
   if (FALSE == spiBam->transfer->bWriteTransferDone)
   {
   res =  spiDeviceBamPollWait(spiBam->txPipeHandle, &spiBam->pipeResult);
      spiBam->transfer->bWriteTransferDone = TRUE;
   }
   if (FALSE == spiBam->transfer->bReadTransferDone)
   {
   res |= spiDeviceBamPollWait(spiBam->rxPipeHandle, &spiBam->pipeResult);
      spiBam->transfer->bReadTransferDone = TRUE;
   }
   spiBam->result.result = res;
   return &spiBam->result;
}


static int32  
spiDeviceBamQueueWriteDescs
(
 SpiDeviceBamType *spiBam
)
{
   int32 bamRes = SPIDEVICE_RES_SUCCESS;
   SpiDeviceTransferDataResult bufAvail, bufNextAvail;
   uint8 *dataBuf;
   uint32 uDataSize;
   uint32 uFlags = BAM_IOVEC_FLAG_NO_SUBMIT;
   SpiDeviceTransferType *transfer = spiBam->transfer;

   /*If the buffer is not the last buffer of the list then queue to 
    * platform BAM layer*/
   do{
      bamRes = SPIDEVICE_RES_SUCCESS;
      bufAvail = transfer->getWriteBufPhys(transfer, &dataBuf);
      uDataSize = transfer->getWriteBufLen(transfer);
      bufNextAvail = transfer->moveWriteNext(transfer);
   }while((SPIDEVICE_TRANSFER_BUFF_AVAILABLE == bufNextAvail) &&
         (0 == (bamRes = bam_pipe_transfer(spiBam->txPipeHandle, 
            (uint32)dataBuf, uDataSize, uFlags, (void *)NULL))));

   /*Check if we got the last buffer, then check if EOT is required
    * then set the flags according before queuing to BAM */
   if((SPIDEVICE_TRANSFER_BUFF_AVAILABLE == bufAvail)&& !bamRes)
   {
      uFlags = 0; 
      
      if(spiBam->eot){
         uFlags |= BAM_IOVEC_FLAG_EOT |BAM_IOVEC_FLAG_NWD | BAM_IOVEC_FLAG_INT ;
      }

      bamRes = bam_pipe_transfer(spiBam->txPipeHandle, 
               (uint32)dataBuf, uDataSize, uFlags, (void*)NULL);
   }
  
   return bamRes;
}

static int32
spiDeviceBamQueueReadDescs
(
 SpiDeviceBamType *spiBam
)
{
   int32 bamRes = SPIDEVICE_RES_SUCCESS;
   SpiDeviceTransferDataResult bufAvail, bufNextAvail;
   uint8 *dataBuf;
   uint32 uDataSize;
   uint32 uFlags = BAM_IOVEC_FLAG_NO_SUBMIT;
   SpiDeviceTransferType *transfer = spiBam->transfer;

   /*If the buffer is not the last buffer of the list then queue to 
    * platform BAM layer*/
   do{
      bamRes = SPIDEVICE_RES_SUCCESS;
      bufAvail = transfer->getReadBufPhys(transfer, &dataBuf);
      uDataSize = transfer->getReadBufLen(transfer);
      bufNextAvail = transfer->moveReadNext(transfer);
   }while((SPIDEVICE_TRANSFER_BUFF_AVAILABLE == bufNextAvail) &&
         (0 == (bamRes = bam_pipe_transfer(spiBam->rxPipeHandle, 
                     (uint32)dataBuf, uDataSize, uFlags, (void *)NULL))));

   /*Check if we got the last buffer, then check if EOT is required
    * then set the flags according before queuing to BAM */
   if((SPIDEVICE_TRANSFER_BUFF_AVAILABLE == bufAvail) && !bamRes){
      /*We always queue (meaning remove the BAM_IOVEC_FLAG_NO_SUBMIT 
       * flag) the last read transfer*/
      uFlags = 0;
      if(spiBam->eot){
         uFlags |= BAM_IOVEC_FLAG_EOT | BAM_IOVEC_FLAG_INT;
      }
      bamRes = bam_pipe_transfer(spiBam->rxPipeHandle, 
            (uint32)dataBuf, uDataSize, uFlags, (void*)NULL);
   }

   return bamRes;
}

int32
spiDeviceBamStartTransfer
(
 SpiDeviceBamType *spiBam, 
 SpiDeviceTransferType *transfer,
 void *pUser,
 boolean eot
)
{
   int32 res = SPIDEVICE_RES_SUCCESS;
   uint32 do_while_constant = 0;

   bam_pipe_disable(spiBam->rxPipeHandle);
   bam_pipe_enable(spiBam->rxPipeHandle);
   bam_pipe_disable(spiBam->txPipeHandle);
   bam_pipe_enable(spiBam->txPipeHandle);

   do
   {
      spiBam->result.pUser = pUser;
      spiBam->transfer = transfer;
      spiBam->eot = eot;
      
      /*Queue the descriptors to BAM*/
      res = spiDeviceBamQueueReadDescs(spiBam);
      SPIDEVICEBAM_CHK_RESULT_BREAK(res);
   
      res = spiDeviceBamQueueWriteDescs(spiBam);
      SPIDEVICEBAM_CHK_RESULT_BREAK(res);
   }while(do_while_constant);
   
   return res;
}

int32 SpiDeviceBamInit
(
  SpiDeviceBamType **pSpiBam, 
  SPIDEVICE_PLATCFG_HANDLE	hPlat
 )
{
   SpiDeviceBamType *spiBam = NULL;
   int32 res = SPIDEVICE_RES_SUCCESS;
   int32 return_value = SPIDEVICE_RES_SUCCESS;
   uint32 do_while_constant = 0;
   bam_config_type      bamCfg;
   bam_callback_type    bamDummyCB;
   bam_pipe_config_type bamTxPipeCfg; 
   bam_pipe_config_type bamRxPipeCfg;
   SpiDeviceBamCfg_t *SpiBamCfg;
   
   do
   {
      if (NULL == (SpiBamCfg = SpiDevicePlatCfg_PlatInternal_GetBamCfg(hPlat)))
      {
         SPIDEVICE_LOG(ERROR,"SpiDeviceBamInit: BAM configuration is NULL\n");
         break;
      }
      if(SPIDEVICEOSSVC_RESULT_OK != (res = SpiDeviceOsSvc_Malloc((void **)&spiBam, 
                                              sizeof(SpiDeviceBamType))))
      {
         SPIDEVICE_LOG(ERROR,"SpiDeviceBamInit: spiBam Malloc failure\n");
         break;
      }

      SpiDeviceOsSvc_Memset((void *)spiBam, 0, sizeof(SpiDeviceBamType));

      bamCfg.bam_va = 0;
      bamCfg.bam_pa = SpiBamCfg->uBamBaseAddr;
      bamCfg.bam_irq = 0;
      bamCfg.sum_thresh = 0x2000;
      bamCfg.bam_irq_mask = 0;
      bamCfg.options = 0;

      if(NULL == (spiBam->bamHandle = bam_init(&bamCfg, &bamDummyCB))){
         res = -1;//FIXME
         break;
      }

      if(SPIDEVICEOSSVC_RESULT_OK != 
          (res = SpiDeviceOsSvc_PhysMemAlloc(0x100, 
                                            &(spiBam->txPipeMem.physMemHandle),
                                            &(spiBam->txPipeMem.virtualAddr),
                                            &(spiBam->txPipeMem.physAddr))))
      {
         SPIDEVICE_LOG(ERROR,"SpiDeviceBamInit: Reset cmd desc Malloc failure\n");
         break;
      }
      
   
      bamTxPipeCfg.options      = BAM_O_DESC_DONE | BAM_O_EOT;
      bamTxPipeCfg.dir          = BAM_DIR_CONSUMER;
      bamTxPipeCfg.mode         = BAM_MODE_SYSTEM;
      bamTxPipeCfg.desc_base_pa = spiBam->txPipeMem.physAddr;
      bamTxPipeCfg.desc_base_va = spiBam->txPipeMem.virtualAddr;
      bamTxPipeCfg.desc_size    = 0x100;

      if(0 == (spiBam->txPipeHandle = bam_pipe_init(spiBam->bamHandle, 
                                    SpiBamCfg->uBamTxPipeNum,
                                     &bamTxPipeCfg,
                                     (bam_callback_type *)NULL)))
      {
         res = -1;
         break;
      }

      if(SPIDEVICEOSSVC_RESULT_OK != 
          (res = SpiDeviceOsSvc_PhysMemAlloc(0x100, 
                                            &(spiBam->rxPipeMem.physMemHandle),
                                            &(spiBam->rxPipeMem.virtualAddr),
                                            &(spiBam->rxPipeMem.physAddr))))
      {
         SPIDEVICE_LOG(ERROR,"SpiDeviceBamInit: Settings cmd desc Malloc failure\n");
         break;
      }

      bamRxPipeCfg.options      = BAM_O_DESC_DONE | BAM_O_EOT;
      bamRxPipeCfg.dir          = BAM_DIR_PRODUCER;
      bamRxPipeCfg.mode         = BAM_MODE_SYSTEM;
      bamRxPipeCfg.desc_base_pa = spiBam->rxPipeMem.physAddr;
      bamRxPipeCfg.desc_base_va = spiBam->rxPipeMem.virtualAddr;
      bamRxPipeCfg.desc_size    = 0x100;

      if(0 == (spiBam->rxPipeHandle = bam_pipe_init(spiBam->bamHandle, 
                                 SpiBamCfg->uBamRxPipeNum,
                                  &bamRxPipeCfg,
                                  (bam_callback_type *)NULL)))
      {
         res = -1;
         break;
      }
      spiBam->startBamTransfer = spiDeviceBamStartTransfer;
      spiBam->cancelBamTransfer = NULL;
      spiBam->pollBamTransfer = spiDeviceBamPoll;
      *pSpiBam = spiBam;
   
   }while(do_while_constant);   

   if (SPIDEVICE_RES_SUCCESS != res)
   {
      return_value = res;

      res = SpiDeviceBamDeInit(&spiBam);
   }

   return return_value;
}


int32 SpiDeviceBamDeInit
(
   SpiDeviceBamType **pSpiBam 
)
{
   SpiDeviceBamType *spiBam = (SpiDeviceBamType *)*pSpiBam;
   int32 result = SPIDEVICE_RES_SUCCESS;
   uint32 do_while_constant = 0;

   do
   {
      if (NULL == spiBam)
      {
         result = -1;
         break;
      }

      if (SPIDEVICE_RES_SUCCESS != result)
      {
         SPIDEVICE_LOG(ERROR,"SpiDevice_DeInitPlatBam failed err 0x%x\n",
                   (unsigned int)result);
      }
 	  if(spiBam->txPipeHandle)
 	  {
		 result = bam_pipe_deinit(spiBam->txPipeHandle);
		 if(BAM_SUCCESS != result)
		 {
			SPIDEVICE_LOG(ERROR,"SpiDeviceBamDeInit: TX bam_pipe_deinit failed");
			break;
		 }
 	  }
	  
 	  if(spiBam->rxPipeHandle)
 	  {
	     result = bam_pipe_deinit(spiBam->rxPipeHandle);
		 if(BAM_SUCCESS != result)
		 {
			SPIDEVICE_LOG(ERROR,"SpiDeviceBamDeInit: RX bam_pipe_deinit failed");
			break;
		 }
 	  }
      
      if (NULL != spiBam->rxPipeMem.physMemHandle)
      {
         result = SpiDeviceOsSvc_PhysMemFree(spiBam->rxPipeMem.physMemHandle);

         spiBam->rxPipeMem.physMemHandle = NULL;
         spiBam->rxPipeMem.virtualAddr = NULL;         
         spiBam->rxPipeMem.physAddr = NULL;         
         
         if (SPIDEVICE_RES_SUCCESS != result)
         {
            SPIDEVICE_LOG(ERROR,"SpiDeviceBamDeInit: Free failed err 0x%x\n",
                          (unsigned int)result);
         }
      }

      if (NULL != spiBam->txPipeMem.physMemHandle)
      {
         result = SpiDeviceOsSvc_PhysMemFree(spiBam->txPipeMem.physMemHandle);

         spiBam->txPipeMem.physMemHandle = NULL;
         spiBam->txPipeMem.virtualAddr = NULL;         
         spiBam->txPipeMem.physAddr = NULL;          
         
         if (SPIDEVICE_RES_SUCCESS != result)
         {
            SPIDEVICE_LOG(ERROR,"SpiDeviceBamDeInit: Free failed err 0x%x\n",
                           (unsigned int)result);
         }
      }

	  /*Recommendation from BAM team is to only call BAM deinit if we are sure no one will 
	  ever call SPI open again. Deiniting BAM does not save any power, and the resources it 
	  frees are negligible */
	  if (NULL != spiBam->bamHandle)
	  {
	  	  result = bam_deinit(spiBam->bamHandle, 0);
		  if(BAM_SUCCESS != result)
		  {
			 SPIDEVICE_LOG(ERROR,"SpiDeviceBamDeInit: bam_deinit failed");
			 break;
		  }
	  }

      result = SpiDeviceOsSvc_Free(spiBam);
	  if (SPIDEVICE_RES_SUCCESS == result) 
	  {
          *pSpiBam = NULL;
	  }

   }while(do_while_constant);
   
   return result;
}
