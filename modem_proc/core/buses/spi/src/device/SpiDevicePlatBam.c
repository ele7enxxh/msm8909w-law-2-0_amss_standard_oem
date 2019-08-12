/*
===========================================================================

FILE:   SpiDevicePlatBam.c

DESCRIPTION: This file contains the definitions and the descriptions of BAM interface usage for 
                       SPI-BAM interactions.

===========================================================================

        Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/spi/src/device/SpiDevicePlatBam.c#1 $

When           Who    What, where, why
--------      ---    -----------------------------------------------------------
01/13/14      ms     Updated with common logging mechanism
04/07/12      ddk    Added updates for de-init.
03/14/12      ddk    Added updates for pipe management.
02/09/12      ddk    Initial creation.

===========================================================================
        Copyright c 2012 - 2014 Qualcomm Technologies Incorporated.
            All Rights Reserved.
            Qualcomm Proprietary/GTDR

===========================================================================
*/

#include "stddef.h"
#include "stdio.h"
#include "string.h"

#include "SpiDevicePlatSvc.h"
#include "SpiDeviceOsSvc.h"
#include "SpiDeviceError.h"
#include "SpiDevicePlatBam.h"
#include "HALqupe.h"

#include "DALStdDef.h"
#include "DALSys.h"
#include "DALDeviceId.h"
#include "bam.h"
#include "SpiLog.h"



/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define SPIDEVICE_PLATBAM_CHK_RES_BREAK(res)\
   if (SPIDEVICE_RES_SUCCESS !=res) {\
      break;\
   }


#define SPI_DEVICE_PLATBAM_MAX_BAM_THRESHOLD (32*1024)
#define SPI_DEVICE_PLATBAM_MAX_DESC_SIZE           0x100
#define SPI_DEVICE_PLATBAM_MAX_COMMAND_SIZE    0x100


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

 typedef enum SpiDevicePlatBam_BamPipeState
{
   SPIDEVICEPLATBAM_BAMPIPESTATE_UNINITIALIZED = 0x0,
   SPIDEVICEPLATBAM_BAMPIPESTATE_INIT_DONE   = 0x1,
   SPIDEVICEPLATBAM_BAMPIPESTATE_ACTIVE,
   SPIDEVICEPLATBAM_BAMPIPESTATE_INACTIVE,
   SPIDEVICEPLATBAM_BAMPIPESTATE_ERROR
} SpiDevicePlatBam_BamPipeState;

typedef enum SpiDevicePlatBam_BamState
{
   SPIDEVICEPLATBAM_BAMSTATE_UNINITIALIZED     = 0x0,
   SPIDEVICEPLATBAM_BAMSTATE_TARGETINIT_DONE   = 0x1,
   SPIDEVICEPLATBAM_BAMSTATE_ERROR
} SpiDevicePlatBam_BamState;




typedef enum IBAMPLATCFG_Error
{
   IBAMPLATCFG_ERROR_BASE = SPIDEVICE_RES_ERROR_CLS_DEV_PLATFORM,

   IBAMPLATCFG_RESULT_ERROR_DAL_GET_PROPERTY_HANDLE,
   IBAMPLATCFG_RESULT_ERROR_DAL_GET_PROPERTY_VALUE,
} IBAMPLATCFG_Error;



typedef bam_pipe_config_type SpiDevicePlatBam_PipeCfgType;

typedef bam_callback_type SpiDevicePlatBam_PipeCallbackType;

typedef bam_config_type  SpiDevicePlatBam_BamCfgType;


typedef struct SpiDevicePlatBam_BamCallbackType
{
    SPIDEVICE_PLATBAM_CB func; /**< Function to be called via callback */
    void *data; /** User provided data to be supplied with the callback */
} SpiDevicePlatBam_BamCallbackType;

typedef struct SpiDevicePlatBam_PropertyType
{
      /* HWIO base physical address */
   uint32 uBamHwioBase;

   uint32  devBamMaxPipePair;  

   /* The BAM interrupt ID */
   uint32 uBamIntId;
} SpiDevicePlatBam_PropertyType;

typedef struct SpiDevicePlatBam_DescFifoType{
   void     *phMemHandle;
   uint32   physAddr;
   uint32   virtualAddr;
}SpiDevicePlatBam_DescFifoType;   

typedef struct SpiDevicePlatBam_PipeCtxt
{
   uint32                              hBamPipeDev;  
   uint32                              devBamPipeNum;  
   SpiDevicePlatBam_BamPipeState       devBamPipeState;
   SpiDevicePlatBam_PipeCfgType        devBamPipeConfig;
   SpiDevicePlatBam_PipeCallbackType   devBamPipeCallbackCfg;
   SpiDevicePlatBam_DescFifoType       devBamPipeDescFifo;
} SpiDevicePlatBam_PipeCtxt;


typedef struct SpiDevicePlatBam_PipeInfoType
{
   uint32                        uCoreNum;
   SpiDevicePlatBam_PipeCtxt     devBamRxPipeCtxt;  
   SpiDevicePlatBam_PipeCtxt     devBamTxPipeCtxt;  
} SpiDevicePlatBam_PipeInfoType;

typedef struct SpiDevicePlatBam_DevCfgType
{
   IBUSPLATCFG_HANDLE                 hPlat;
   uint32                             uBamDevId;
   uint32                             hBamDev;  
   SpiDevicePlatBam_BamCfgType        devBamConfig;
   SpiDevicePlatBam_PropertyType      devBamProps;
   SpiDevicePlatBam_BamState          devBamState;
   SpiDevicePlatBam_BamCallbackType   devBamCallbackCfg;
   uint32                             uActiveBamPipePair;
   SpiDevicePlatBam_PipeInfoType      *devBamPipeConfig;   
} SpiDevicePlatBam_DevCfgType, *SpiDevicePlatBam_PDevCfgType;


typedef struct SpiDevicePlatBam_DevCfgLinkType SpiDevicePlatBam_DevCfgLinkType;

struct SpiDevicePlatBam_DevCfgLinkType{
   SpiDevicePlatBam_DevCfgLinkType *pNextLink;
   SpiDevicePlatBam_DevCfgType devCfg;
};


/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
struct {
   SpiDevicePlatBam_DevCfgLinkType *pHead;
   uint32 numDevCfgs;
} SpiDevicePlatBam_DevTgtCfgList;


 /*---------------------------------------------------------------------------- 
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static boolean SpiDevicePlatBam_SearchDevCfg
(
   uint32 uBamDevId,
   SpiDevicePlatBam_PDevCfgType *ppDevCfg
)
{
   SpiDevicePlatBam_DevCfgLinkType *pLink;

   if (NULL == SpiDevicePlatBam_DevTgtCfgList.pHead) {
      return FALSE;
   }
   pLink = SpiDevicePlatBam_DevTgtCfgList.pHead;
   do {
      if(pLink->devCfg.uBamDevId == uBamDevId) {
         *ppDevCfg = &pLink->devCfg;
         return TRUE;
      }
      pLink = pLink->pNextLink;
   } while(NULL != pLink);

   return FALSE;
}

static int32 SpiDevicePlatBam_AddDevCfg
(
   uint32 uBamDevId,
   SpiDevicePlatBam_PDevCfgType *ppDevCfg
)
{
   int32 res;
   SpiDevicePlatBam_DevCfgLinkType *pLink;

   res = IBUSOS_Malloc((void **)&pLink, sizeof(SpiDevicePlatBam_DevCfgLinkType));
   if (IBUSOS_RESULT_OK != res) {
      return res;
   }
   IBUSOS_Memset((void *)pLink, 0, sizeof(SpiDevicePlatBam_DevCfgLinkType));
   pLink->pNextLink = SpiDevicePlatBam_DevTgtCfgList.pHead;
   SpiDevicePlatBam_DevTgtCfgList.pHead = pLink;
   SpiDevicePlatBam_DevTgtCfgList.numDevCfgs++;

   pLink->devCfg.uBamDevId = uBamDevId;
   
   *ppDevCfg = &pLink->devCfg;

   return SPIDEVICE_RES_SUCCESS;
}

/* TODO: BAM denit is not fully supported and hence bam_deinit is not working 
** Enable the snippet when it is supported.
*/
#if 0
static int32 SpiDevicePlatBam_RemoveDevCfg
(
   uint32 uBamDevId
)
{
   int32 res = IBUSOS_RESULT_OK;
   SpiDevicePlatBam_DevCfgLinkType *pLink;
   SpiDevicePlatBam_DevCfgLinkType *oldpLink;

   if (NULL == SpiDevicePlatBam_DevTgtCfgList.pHead) {
      return FALSE;
   }
   pLink = SpiDevicePlatBam_DevTgtCfgList.pHead;

   do 
   {
      if(pLink->devCfg.uBamDevId == uBamDevId)
      {
         if(SpiDevicePlatBam_DevTgtCfgList.pHead == pLink)
         {
            SpiDevicePlatBam_DevTgtCfgList.pHead = pLink->pNextLink;
         }
         else
         {
            oldpLink->pNextLink = pLink->pNextLink;
         }

         SpiDevicePlatBam_DevTgtCfgList.numDevCfgs--;        

         res = IBUSOS_Free(pLink);         
      }
      oldpLink = pLink;
      pLink = pLink->pNextLink;
   } while(NULL != pLink);


   if (IBUSOS_RESULT_OK != res) {
      return res;
   }
  
   return SPIDEVICE_RES_SUCCESS;
}

#endif

static int32 SpiDevicePlatBam_GetDevCfg
(
   uint32 uBamDevId,
   SpiDevicePlatBam_PDevCfgType *ppDevCfg
)
{
   if (SpiDevicePlatBam_SearchDevCfg(uBamDevId, ppDevCfg)) {
      return SPIDEVICE_RES_SUCCESS;
   }

   return SpiDevicePlatBam_AddDevCfg(uBamDevId, ppDevCfg);
}


static void SpiDevicePlatBam_BamCallback(bam_result_type bam_result)
{
   SpiDevicePlatBam_DevCfgType *devCfg = 
      (SpiDevicePlatBam_DevCfgType *)(bam_result.cb_data);

   SpiDevicePlatBamResult pResult ;

   if (devCfg != NULL)
   {
      pResult.pUser = NULL; /* TODO : Need to decide the values here */

      switch(bam_result.event)
      {
         case BAM_EVENT_EOT:
                  pResult.result = IBUSBAM_RESULT_OK;
         break;
         
         case BAM_EVENT_ERROR:
                  pResult.result = IBUSBAM_BAM_TRANSACTION_ERROR;
         break;
         
         default:
                  pResult.result = IBUSBAM_BAM_TRANSACTION_ERROR;
         break;
      };

     /* Call the callback */
      (devCfg->devBamCallbackCfg.func)(&pResult);
   }
   else
   {
      /* TODO: Should we error fatal or indicate some logs and just return ? */
      SPIDEVICE_LOG(ERROR, "SpiDevicePlatBam_BamCallback : Null ptr error");	  
   }

}

static void SpiDevicePlatBam_BamPipeCallback(bam_result_type bam_result)
{
   SpiDevicePlatBam_DevCfgType *devCfg = 
      (SpiDevicePlatBam_DevCfgType *)(bam_result.cb_data);

   SpiDevicePlatBam_PipeCtxt *pipeCtxt = NULL;

   SpiDevicePlatBamResult pResult ;

   if (devCfg != NULL)
   {
      pipeCtxt = bam_result.data.xfer.xfer_cb_data;

      pResult.pUser = pipeCtxt->devBamPipeCallbackCfg.data;

      switch(bam_result.event)
      {
         case BAM_EVENT_EOT:
                  pResult.result = IBUSBAM_RESULT_OK;
         break;
         
         case BAM_EVENT_DESC_DONE:
                  pResult.result = IBUSBAM_RESULT_OK;
         break;
         
         case BAM_EVENT_ERROR:
                  pResult.result = IBUSBAM_BAM_TRANSACTION_ERROR;
         break;
         
         default:
                  pResult.result = IBUSBAM_BAM_TRANSACTION_ERROR;
         break;
      };

     /* Call the callback */
      (devCfg->devBamCallbackCfg.func)(&pResult);
   }
   else
   {
      /* TODO: Should we error fatal or indicate some logs and just return ? */
      SPIDEVICE_LOG(ERROR, "SpiDevicePlatBam_BamPipeCallback : Null ptr error");	 	  
   }

}


static int32 SpiDevicePlatBam_InitTarget
(
   IBUSPLATCFG_HANDLE hPlat,
   SPIDEVICE_PLATBAM_HANDLE *hPlatBam,
   SPIDEVICE_PLATBAM_CB pCallBackFn
)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   DALSYSPropertyVar PropVar;
   int32 result = SPIDEVICE_RES_SUCCESS;
   uint32 uBamDevId = 0;
   uint32 uPeriphbasepaddr = 0;
   SpiDevicePlatBam_DevCfgType *devCfg = NULL;
   uint32 qupDalId = 0;
   uint32 uQupCoreNum = 0;

   do
   {
      if (hPlat == NULL)
      {
         result = IBUSBAM_NULLPTR_ERROR;
         break;
      }

      qupDalId = IBUSPLATCFG_PlatInternal_GetDalId(hPlat);
		uQupCoreNum = IBUSPLATCFG_PlatInternal_GetQupCoreNum(hPlat);

      if (DAL_SUCCESS != DALSYS_GetDALPropertyHandle(qupDalId, hProp )) 
      {
         result = IBAMPLATCFG_RESULT_ERROR_DAL_GET_PROPERTY_HANDLE;
         break;
      }


      if (DAL_SUCCESS !=DALSYS_GetPropertyValue(hProp, 
               "BAM_BLSP_ID",0, &PropVar)) 
      {
         result = IBAMPLATCFG_RESULT_ERROR_DAL_GET_PROPERTY_VALUE;
         break;
      }
      uBamDevId = (uint32)PropVar.Val.dwVal;


      if (SpiDevicePlatBam_GetDevCfg(uBamDevId,&devCfg) 
            != SPIDEVICE_RES_SUCCESS)
      {
         break;
      }


      if (DAL_SUCCESS !=DALSYS_GetPropertyValue(hProp, 
               "CHIP_PERIPH_SS_BASE",0, &PropVar)) 
      {
         result = IBAMPLATCFG_RESULT_ERROR_DAL_GET_PROPERTY_VALUE;
         break;
      }
      uPeriphbasepaddr = (uint32)PropVar.Val.dwVal;

      if (DAL_SUCCESS !=DALSYS_GetPropertyValue(hProp, 
               "BAM_BLOCK_OFFSET",0, &PropVar)) 
      {
         result = IBAMPLATCFG_RESULT_ERROR_DAL_GET_PROPERTY_VALUE;
         break;
      }
      devCfg->devBamProps.uBamHwioBase = uPeriphbasepaddr + (uint32)PropVar.Val.dwVal;

      if (DAL_SUCCESS !=DALSYS_GetPropertyValue(hProp, 
               "BAM_INTRERRUPT_ID",0, &PropVar)) 
      {
         result = IBAMPLATCFG_RESULT_ERROR_DAL_GET_PROPERTY_VALUE;
         break;
      }
      devCfg->devBamProps.uBamIntId = (uint32)PropVar.Val.dwVal;

      if (DAL_SUCCESS !=DALSYS_GetPropertyValue(hProp, 
               "BAM_MAX_PIPE_PAIR",0, &PropVar)) 
      {
         result = IBAMPLATCFG_RESULT_ERROR_DAL_GET_PROPERTY_VALUE;
         break;
      }
      devCfg->devBamProps.devBamMaxPipePair = (uint32)PropVar.Val.dwVal;


      if (devCfg->devBamPipeConfig == NULL)
      {
         result = IBUSOS_Malloc((void **)&(devCfg->devBamPipeConfig), 
                               (devCfg->devBamProps.devBamMaxPipePair *
                               sizeof(SpiDevicePlatBam_PipeInfoType)));
      
         if (IBUSOS_RESULT_OK != result) {
            break ;
         }

		IBUSOS_Memset((void *)devCfg->devBamPipeConfig, 
				0, (devCfg->devBamProps.devBamMaxPipePair *
                        sizeof(SpiDevicePlatBam_PipeInfoType)));
      }

      devCfg->devBamPipeConfig[uQupCoreNum].uCoreNum = uQupCoreNum;
      
      if (DAL_SUCCESS !=DALSYS_GetPropertyValue(hProp, 
               "BAM_TX_PIPE_NUM",0, &PropVar)) 
      {
         result = IBAMPLATCFG_RESULT_ERROR_DAL_GET_PROPERTY_VALUE;
         break;
      }

      devCfg->devBamPipeConfig[uQupCoreNum].devBamTxPipeCtxt.devBamPipeNum = 
                   (uint32)PropVar.Val.dwVal;      
      
      if (DAL_SUCCESS !=DALSYS_GetPropertyValue(hProp, 
               "BAM_RX_PIPE_NUM",0, &PropVar)) 
      {
         result = IBAMPLATCFG_RESULT_ERROR_DAL_GET_PROPERTY_VALUE;
         break;
      }
      devCfg->devBamPipeConfig[uQupCoreNum].devBamRxPipeCtxt.devBamPipeNum = 
                    (uint32)PropVar.Val.dwVal;      
     
      devCfg->devBamCallbackCfg.func = pCallBackFn;

      devCfg->uActiveBamPipePair++; 

      devCfg->hPlat = hPlat;

      devCfg->devBamState = SPIDEVICEPLATBAM_BAMSTATE_TARGETINIT_DONE;

      *hPlatBam = (SPIDEVICE_PLATBAM_HANDLE) devCfg;

   }while(0);

   return result;

}

static int32 SpiDevicePlatBam_InitBamPipe
(
   SPIDEVICE_PLATBAM_HANDLE hPlatBam,
   uint32 hBamDev,
   SpiDevicePlatBam_PipeCtxt          *pipeCtxt,
   uint32  pipedir
)
{
   int32 result = SPIDEVICE_RES_SUCCESS;
   
   bam_pipe_config_type *pipe_cfg   = &(pipeCtxt->devBamPipeConfig);
   bam_callback_type *pipe_cb       = &(pipeCtxt->devBamPipeCallbackCfg);
   
   do
   {
      if (NULL == pipeCtxt)
      {
         result = IBUSBAM_NULLPTR_ERROR;
         break;
      }

      if (NULL == pipeCtxt->hBamPipeDev)
      {
         if (pipeCtxt->devBamPipeDescFifo.phMemHandle == NULL)
         {
            if(IBUSOS_RESULT_OK != 
                   IBUSOSSPI_PhysMemAlloc(SPI_DEVICE_PLATBAM_MAX_DESC_SIZE, 
                    &(pipeCtxt->devBamPipeDescFifo.phMemHandle),
                    &(pipeCtxt->devBamPipeDescFifo.virtualAddr),
                    &(pipeCtxt->devBamPipeDescFifo.physAddr)))
            {
               result = IBUSBAM_INIT_ERROR;
               break;
            }
         }

         pipe_cfg->options       = BAM_O_DESC_DONE|BAM_O_EOT|BAM_O_ERROR;
         pipe_cfg->dir           = pipedir;
         pipe_cfg->mode          = BAM_MODE_SYSTEM;
         pipe_cfg->desc_base_va  = pipeCtxt->devBamPipeDescFifo.virtualAddr;
         pipe_cfg->desc_base_pa  = pipeCtxt->devBamPipeDescFifo.physAddr;
         pipe_cfg->desc_size     = SPI_DEVICE_PLATBAM_MAX_DESC_SIZE;
      
         pipe_cb->func = SpiDevicePlatBam_BamPipeCallback;
         pipe_cb->data = hPlatBam;

         pipeCtxt->hBamPipeDev = 
            bam_pipe_init(hBamDev, 
                                     pipeCtxt->devBamPipeNum, 
                                     pipe_cfg, 
                                     pipe_cb);

         if (NULL == pipeCtxt->hBamPipeDev)
         {
            result = IBUSBAM_INIT_ERROR;
            break;
         }
         pipeCtxt->devBamPipeState = SPIDEVICEPLATBAM_BAMPIPESTATE_INIT_DONE;
      }      
      
   }while(0);
   
   return result;
}

static int32 SpiDevicePlatBam_InitBam
(
   IBUSPLATCFG_HANDLE hPlat, 
   SPIDEVICE_PLATBAM_HANDLE hPlatBam,
   void *pArg
)
{
   int32 result = SPIDEVICE_RES_SUCCESS;
   SpiDevicePlatBam_DevCfgType *devCfg = (SpiDevicePlatBam_DevCfgType *)(hPlatBam);
   uint32 uQupCoreNum = IBUSPLATCFG_PlatInternal_GetQupCoreNum(hPlat);
   SpiDevicePlatBam_PipeCtxt          *pipeCtxt = NULL;
   bam_callback_type bam_cb;
      
   do
   {
      if (NULL == devCfg)
      {
         result = IBUSBAM_NULLPTR_ERROR;
         break;
      }

      if (NULL == devCfg->hBamDev)
      {         
         devCfg->devBamConfig.bam_pa = devCfg->devBamProps.uBamHwioBase;        
         devCfg->devBamConfig.bam_va = 0; /* BAM should remap if this is zero*/
         devCfg->devBamConfig.bam_irq = devCfg->devBamProps.uBamIntId;        

         devCfg->devBamConfig.bam_irq_mask = BAM_IRQ_HRESP_ERR_EN|BAM_IRQ_ERR_EN;
         devCfg->devBamConfig.sum_thresh = SPI_DEVICE_PLATBAM_MAX_BAM_THRESHOLD;        
         devCfg->devBamConfig.options = 0;/* No use, set it to zero for now */
         
         bam_cb.func = SpiDevicePlatBam_BamCallback;
         bam_cb.data = hPlatBam; 

         devCfg->hBamDev = bam_init(&(devCfg->devBamConfig),&bam_cb);
      }
      
      if (NULL == devCfg->hBamDev)
      {
         result = IBUSBAM_INIT_ERROR;
         break;
      }

      /* TX pipe configuration */
      pipeCtxt = &(devCfg->devBamPipeConfig[uQupCoreNum].devBamTxPipeCtxt);

      if (NULL == pipeCtxt->hBamPipeDev)
      {
         result = SpiDevicePlatBam_InitBamPipe(hPlatBam,
                               devCfg->hBamDev,
                               pipeCtxt,
                               BAM_DIR_CONSUMER);
      }

      /* RX pipe configuration */
      pipeCtxt = &(devCfg->devBamPipeConfig[uQupCoreNum].devBamRxPipeCtxt);  

      if (NULL == pipeCtxt->hBamPipeDev)
      {
         result = SpiDevicePlatBam_InitBamPipe(hPlatBam,
                               devCfg->hBamDev,
                               pipeCtxt,
                               BAM_DIR_PRODUCER);
      }
      
   }while(0);
   
   return result;
}


/**
 * @brief Detach pipe configurations from the BAM driver
 *
 * This function detaches from the BAM driver
 * 
 * @param[in] hPlat   Handle to target Platform.
 * @param[in] hBAM  BAM driver handle
 * 
 * @return SPI_DEVICE_SUCCESS on success, error code on error
 */
static int32 SpiDevicePlatBam_DeInitBamPipe
(
   SpiDevicePlatBam_PipeCtxt          *pipeCtxt
)
{
   int32 result = SPIDEVICE_RES_SUCCESS;

   do
   {
      if (NULL == pipeCtxt)
      {
         result = IBUSBAM_NULLPTR_ERROR;
         break;
      }

      if (NULL != pipeCtxt->hBamPipeDev)
      {
         if (bam_pipe_deinit(pipeCtxt->hBamPipeDev) != BAM_SUCCESS)
         {
            result = IBUSBAM_DEINIT_ERROR;
            break;
         }

         pipeCtxt->hBamPipeDev = NULL;
         pipeCtxt->devBamPipeState = 
            SPIDEVICEPLATBAM_BAMPIPESTATE_UNINITIALIZED;

         if (pipeCtxt->devBamPipeDescFifo.phMemHandle != NULL)
         {
            result = IBUSOSSPI_PhysMemFree(
                       pipeCtxt->devBamPipeDescFifo.phMemHandle);
   
            pipeCtxt->devBamPipeDescFifo.phMemHandle = NULL;
            pipeCtxt->devBamPipeDescFifo.virtualAddr = NULL;         
            pipeCtxt->devBamPipeDescFifo.physAddr = NULL;         
            
            SPIDEVICE_PLATBAM_CHK_RES_BREAK(result);
         }
      }
   }while(0);
  
   return result;
}


 /*---------------------------------------------------------------------------- 
 * Global Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
 
/**
 * @brief Init platform BAM driver
 *
 * This function attaches to the BAM driver
 * 
 * @param[in] hPlat   Handle to target Platform.
 * @param[out] hPlatBam Pointer to the location to store the Plat
 *             BAM handle
 * @return SPI_DEVICE_SUCCESS on sucess, error code on error
 */
int32 SpiDevice_InitPlatBam
(
   IBUSPLATCFG_HANDLE hPlat, 
   SPIDEVICE_PLATBAM_HANDLE *hPlatBam,
   SPIDEVICE_PLATBAM_CB pCallBackFn,
   void *pArg
)
{
   int32 result = SPIDEVICE_RES_SUCCESS;

   do
   {

      result = SpiDevicePlatBam_InitTarget(hPlat,hPlatBam,pCallBackFn);
      
      if(SPIDEVICE_RES_SUCCESS != result) 
      {
         break;
      }

      result =  SpiDevicePlatBam_InitBam(hPlat,*hPlatBam,pArg);

      if(SPIDEVICE_RES_SUCCESS != result) 
      {
         break;
      }
      
    
   }while(0);

   return result;
}

/**
 * @brief Detach from the BAM driver
 *
 * This function detaches from the BAM driver
 * 
 * @param[in] hPlat   Handle to target Platform.
 * @param[in] hBAM  BAM driver handle
 * 
 * @return SPI_DEVICE_SUCCESS on success, error code on error
 */
int32 SpiDevice_DeInitPlatBam
(
   IBUSPLATCFG_HANDLE hPlat, 
   SPIDEVICE_PLATBAM_HANDLE hBAM
)
{
   int32 result = SPIDEVICE_RES_SUCCESS;
   SpiDevicePlatBam_DevCfgType *devCfg = (SpiDevicePlatBam_DevCfgType *)(hBAM);
   uint32 uQupCoreNum = IBUSPLATCFG_PlatInternal_GetQupCoreNum(hPlat);

   do
   {
      if (NULL == devCfg)
      {
         result = IBUSBAM_NULLPTR_ERROR;
         break;
      }

      result = SpiDevicePlatBam_DeInitBamPipe(
               &(devCfg->devBamPipeConfig[uQupCoreNum].devBamTxPipeCtxt));

      SPIDEVICE_PLATBAM_CHK_RES_BREAK(result);

      result = SpiDevicePlatBam_DeInitBamPipe(
               &(devCfg->devBamPipeConfig[uQupCoreNum].devBamRxPipeCtxt));

      SPIDEVICE_PLATBAM_CHK_RES_BREAK(result);

      /* TODO: BAM denit is not fully supported and hence bam_deinit is not working 
      ** Enable the snippet when it is supported.
      */
#if 0
      if (devCfg->uActiveBamPipePair == 1)
      {


         if (NULL != devCfg->hBamDev)
         {

            if (bam_deinit(devCfg->hBamDev,disable_bam) != BAM_SUCCESS)
            {
               result = IBUSBAM_DEINIT_ERROR;
               devCfg->devBamState = SPIDEVICEPLATBAM_BAMSTATE_ERROR;
               break;
            }
         }
         devCfg->hBamDev = NULL;
         devCfg->devBamState = SPIDEVICEPLATBAM_BAMSTATE_UNINITIALIZED;

         if (devCfg->devBamPipeConfig != NULL)
         {
            result = IBUSOS_Free(devCfg->devBamPipeConfig);
            
            if (IBUSOS_RESULT_OK != result)
            {
               break;
            }
         }
  
         result = SpiDevicePlatBam_RemoveDevCfg(devCfg->uBamDevId);

         SPIDEVICE_PLATBAM_CHK_RES_BREAK(result);         
      }
#endif         

      devCfg->uActiveBamPipePair--; /* Reduce active pipe pair count */
      
   }while(0);
  
   return result;
}


/**
 * @brief Reset the BAM hardware
 *
 * This function resets the BAM hardware
 * 
 * @param[in] hBAM  BAM device handle
 * 
 * @return SPI_DEVICE_SUCCESS on sucess, error code on error 
 */
int32 SpiDevice_ResetPlatBam(SPIDEVICE_PLATBAM_HANDLE hBAM)
{
   int32 result = SPIDEVICE_RES_SUCCESS;
   SpiDevicePlatBam_DevCfgType *devCfg = (SpiDevicePlatBam_DevCfgType *)(hBAM);

   do
   {
      if (NULL == devCfg)
      {
         result = IBUSBAM_NULLPTR_ERROR;
         break;
      }

      if (NULL != devCfg->hBamDev)
      {
         if (bam_reset(devCfg->hBamDev) != BAM_SUCCESS)
         {
            result = IBUSBAM_RESET_ERROR;
            break;
         }
      }
     
   }while(0);

   return result;
}

/**
 * @brief Schedule a single BAM transfer 
 *
 * This function schedules a single BAM transfer.
 * 
 * @param hPlatBam Platform BAM handle
 * @param uDir  Producer or Consumer transfer
 * @param uAddr  Physical start address for the transfer
 * @param uSize  Size of the transfer in bytes
 * @param pUser  User-defined pointer associated with the 
 *               transfer
 * @param uFlags  Flags for the transfer
 * 
 * @return SPI_DEVICE_SUCCESS  on sucess, error code on error
 */
uint32 SpiDevice_PlatBamTransfer
(
   SPIDEVICE_PLATBAM_HANDLE hPlatBam,  
   uint32 uDir, 
   uint32 uAddr, 
   uint32 uSize, 
   void *pArg,
   uint32 uFlags
)
{
   int32 result = SPIDEVICE_RES_SUCCESS;
   SpiDevicePlatBam_DevCfgType *devCfg = (SpiDevicePlatBam_DevCfgType *)hPlatBam;
   SpiDevicePlatBam_PipeCtxt *pipeCtxt = NULL;
   uint32 uQupCoreNum = 0;
   do
   {
      if (NULL == devCfg)
      {
         result = IBUSBAM_NULLPTR_ERROR;
         break;
      }
	  uQupCoreNum = IBUSPLATCFG_PlatInternal_GetQupCoreNum(devCfg->hPlat);

      if (uDir == SPIDEVICE_BAM_OUT)
      {
         pipeCtxt = &(devCfg->devBamPipeConfig[uQupCoreNum].devBamTxPipeCtxt);
      }
      else
      {
         pipeCtxt = &(devCfg->devBamPipeConfig[uQupCoreNum].devBamRxPipeCtxt);
      }

      if (NULL != pipeCtxt->hBamPipeDev) 
      {

         pipeCtxt->devBamPipeState = SPIDEVICEPLATBAM_BAMPIPESTATE_INACTIVE;
         pipeCtxt->devBamPipeCallbackCfg.data = pArg ;
            
         if (bam_pipe_transfer(pipeCtxt->hBamPipeDev,
                              uAddr, 
                              (uint16) uSize, 
                              (uint16) uFlags, 
                              pipeCtxt) != BAM_SUCCESS)
         {
            result = IBUSBAM_TRANSFER_REQUEST_ERROR;
            break;
         }

         pipeCtxt->devBamPipeState = SPIDEVICEPLATBAM_BAMPIPESTATE_ACTIVE;
      }
      else
      {
         result = IBUSBAM_TRANSFER_REQUEST_ERROR;
         break;       
      }
     
   }while(0);

   return result;
}


/**
 * @brief Find Available free space in a BAM pipe
 *
 * This function schedules a single BAM transfer.
 * 
 * @param hPlatBam Platform BAM handle
 * @param uDir  Producer or Consumer transfer
 * @param count  Available free space for descriptors on the 
 *                       platform pipe.
 * 
 * @return SPI_DEVICE_SUCCESS on sucess, error code on error 
 */
uint32 SpiDevice_PlatBamGetPipeFreeSpace
(
   SPIDEVICE_PLATBAM_HANDLE hPlatBam,
   uint32 uDir,
   uint32 *count
)
{
   int32 result = SPIDEVICE_RES_SUCCESS;
   SpiDevicePlatBam_DevCfgType *devCfg = (SpiDevicePlatBam_DevCfgType *)(hPlatBam);
   bam_handle pipehandle;
   uint32 uQupCoreNum = 0; 

   do
   {
      if ((NULL == devCfg) ||(count == NULL) )
      {
         result = IBUSBAM_NULLPTR_ERROR;
         break;
      }
	  uQupCoreNum = IBUSPLATCFG_PlatInternal_GetQupCoreNum(devCfg->hPlat);

      if (uDir == SPIDEVICE_BAM_OUT)
      {
         pipehandle = devCfg->devBamPipeConfig[uQupCoreNum].devBamTxPipeCtxt.hBamPipeDev;
      }
      else
      {
         pipehandle = devCfg->devBamPipeConfig[uQupCoreNum].devBamRxPipeCtxt.hBamPipeDev;
      }


      if (NULL != pipehandle)
      {
         *count = bam_pipe_getfreecount(pipehandle);        
      }    
   }while(0);

   return result;
}

