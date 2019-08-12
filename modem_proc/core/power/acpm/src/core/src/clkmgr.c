/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * clkmgr.c
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */


#include "adsppm.h"
#include "ULog.h"
#include "ULogFront.h"
#include "adsppm_utils.h"
#include "requestmgr.h"
#include "asic.h"
#include "clkmgr.h"
#include "core_internal.h"
#include "hal_clkrgm.h"

//Enumeration for core clock index
//Has to be in exactly the same order as core clocks arranged in the AdsppmClkIdType
typedef enum {
   HwRsp_Core = 0,
   Midi_Core,
   AvSync_Xo,
   AvSync_Bt,
   AvSync_Fm,
   Slimbus_Core,
   Avtimer_Core,
   Atime_Core,
   Atime2_Core,
   BSTC_core
}ReqArrayIndexType;

typedef struct{
   AsicClkTypeType ClkType;
   AdsppmClkIdType CoreId;
   uint32 value;  /* current freq */
   uint32 last;   /* last setting freq */
   AdsppmClkDomainSrcIdType      clkDomainSrc; /* which source */
   AdsppmClkDomainSrcIdType      clkDomainSrc_last; /* which source */
}CLKAggaregateType;


static CLKAggaregateType gCLKReqArray[] =
{
   {.CoreId = AdsppmClk_HwRsp_Core,.value=0,.last=0,.clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL},
   {.CoreId = AdsppmClk_Midi_Core,.value=0,.last=0,.clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL},
   {.CoreId = AdsppmClk_AvSync_Xo,.value=0,.last=0,.clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL},
   {.CoreId = AdsppmClk_AvSync_Bt,.value=0,.last=0,.clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL},
   {.CoreId = AdsppmClk_AvSync_Fm,.value=0,.last=0,.clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL},
   {.CoreId = AdsppmClk_Slimbus_Core,.value=0,.last=0,.clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL},
   {.CoreId = AdsppmClk_Avtimer_core,.value=0,.last=0,.clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL},
   {.CoreId = AdsppmClk_Atime_core,.value=0,.last=0,.clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL},
   {.CoreId = AdsppmClk_Atime2_core,.value=0,.last=0,.clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL},
   {.CoreId = AdsppmClk_BSTC_core,.value=0,.last=0,.clkDomainSrc.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL, .clkDomainSrc_last.clkDomainSrcIdLpass = Adsppm_Clk_Domain_Src_Id_Q6PLL},
   
};


Adsppm_Status CLK_Init(void)
{
   uint32 i;
   uint32 j = 0;
   Adsppm_Status sts = Adsppm_Status_Success;
   ADSPPM_LOG_FUNC_ENTER;

   for(i = AdsppmClk_HwRsp_Core; i < AdsppmClk_EnumMax; i++)
   {
      gCLKReqArray[j].ClkType = ACMClk_GetClockType(i);
	  j++;
   }
   ADSPPM_LOG_FUNC_EXIT(sts);
   return sts;
}

Adsppm_Status CLK_ProcessRequest(coreUtils_Q_Type *pClkReqQ)
{
   Adsppm_Status sts = Adsppm_Status_Success;
   AdsppmClkRequestType *pClkReqData;
   RMCtxType *pLRMCtxItem = NULL;
   uint32 index_a;
   uint32 index_b;
   uint32 index_c;

   ADSPPM_LOG_FUNC_ENTER;
   if(NULL == pClkReqQ)
   {
      sts = Adsppm_Status_BadParm;
   }
   else
   {
      ADSPPM_LOCK_RESOURCE(Adsppm_Rsc_Id_Core_Clk);
      gCLKReqArray[HwRsp_Core].value = 0;
      gCLKReqArray[Midi_Core].value = 0;
      gCLKReqArray[AvSync_Xo].value = 0;
      gCLKReqArray[AvSync_Bt].value = 0;
      gCLKReqArray[AvSync_Fm].value = 0;
      gCLKReqArray[Slimbus_Core].value = 0;
	  gCLKReqArray[Avtimer_Core].value = 0;
	  gCLKReqArray[Atime_Core].value = 0;
	  gCLKReqArray[Atime2_Core].value = 0;
	  gCLKReqArray[BSTC_core].value = 0;
      //also set hw resampler and midi clk to 0
       
      //checks with all clients
      pLRMCtxItem = (RMCtxType *)coreUtils_Q_Check(pClkReqQ);
      for(index_a = 0; index_a < pClkReqQ->nCnt; index_a++)
      {
          if(NULL != pLRMCtxItem)
          {
              if(RM_Valid == pLRMCtxItem->validFlag)
              {
                  if(NULL == ((AdsppmClkRequestType *)(pLRMCtxItem->pRequestData)))
                  {
                      ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "gpRMCtx[index_a].pRequestData =NULL\n");
                      sts = Adsppm_Status_BadParm;
                      break;
                  }
                  else
                  {
                      pClkReqData = (AdsppmClkRequestType *)(pLRMCtxItem->pRequestData);
                      //checks array of clock settings
                      for(index_b = 0; index_b < pClkReqData->numOfClk; index_b++)
                      {
                          ReqArrayIndexType clkIdx = pClkReqData->pClkArray[index_b].clkId-AdsppmClk_HwRsp_Core;
                          gCLKReqArray[clkIdx].value = MAX(gCLKReqArray[clkIdx].value,pClkReqData->pClkArray[index_b].clkFreqHz);
                      }
                  }
              }
              pLRMCtxItem = (RMCtxType *)coreUtils_Q_Next(pClkReqQ, &(pLRMCtxItem->RMCtxLink));
          }
      }
      for(index_c = AdsppmClk_HwRsp_Core; index_c < AdsppmClk_EnumMax; index_c++)
      {
          ReqArrayIndexType clkIdx = index_c-AdsppmClk_HwRsp_Core;
          //Adjust based on feature parameters
          gCLKReqArray[clkIdx].value = ACM_AdjustParamValue (AsicFeatureId_CoreClk_Scaling, gCLKReqArray[clkIdx].value);

          if (gCLKReqArray[clkIdx].last != gCLKReqArray[clkIdx].value)
          {
        	  if(Adsppm_Status_Success !=
        			  ClkRgm_SetClock(index_c,gCLKReqArray[clkIdx].value))
        	  {
        		  sts = Adsppm_Status_Failed;
        		  break;
        	  }
        	  else
        	  {
        		  gCLKReqArray[clkIdx].last = gCLKReqArray[clkIdx].value;
        	  }
          }
         /*
          switch(gCLKReqArray[clkIdx].ClkType)
          {
          case AsicClk_TypeNpa:
          case AsicClk_TypeDalFreqSet:

              if (gCLKReqArray[clkIdx].last != gCLKReqArray[clkIdx].value)
              {
                  if(Adsppm_Status_Success !=
                      ClkRgm_SetClock(index_c,gCLKReqArray[clkIdx].value))
                  {
                      sts = Adsppm_Status_Failed;
                      break;
                  }
                  else
                  {
                      gCLKReqArray[clkIdx].last = gCLKReqArray[clkIdx].value;
                  }
              }

              break;
          case AsicClk_TypeDalEnable:
		  case AsicClk_TypeInternalCGC:

              if (gCLKReqArray[clkIdx].last != gCLKReqArray[clkIdx].value)
              {
                  Hal_ClkRgmEnableDisableType enable = (gCLKReqArray[clkIdx].value > 0)?Hal_ClkRgmEnable:Hal_ClkRgmDisable;
                  if(Adsppm_Status_Success !=
                        ClkRgm_EnableClock(index_c,enable))
                  {
                      sts = Adsppm_Status_Failed;
                      break;
                  }
                  else
                  {
                      gCLKReqArray[clkIdx].last = gCLKReqArray[clkIdx].value;
                  }
              }
              break;
          default:
              break;
          }
          */
      }
      ADSPPM_UNLOCK_RESOURCE(Adsppm_Rsc_Id_Core_Clk);
   }
   ADSPPM_LOG_FUNC_EXIT(sts);
   return sts;
}


Adsppm_Status CLKDomain_ProcessRequest(coreUtils_Q_Type *pClkDomainReqQ)
{
   Adsppm_Status sts = Adsppm_Status_Success;
   AdsppmClkDomainReqType *pClkDomainReqData;
   RMCtxType *pLRMCtxItem = NULL;
   uint32 index_a;
   uint32 index_b;
   uint32 index_c;

   ADSPPM_LOG_FUNC_ENTER;
   if(NULL == pClkDomainReqQ)
   {
      sts = Adsppm_Status_BadParm;
   }
   else
   {
      ADSPPM_LOCK_RESOURCE(Adsppm_Rsc_Id_Core_Clk_Domain);
     
      //checks with all clients
      pLRMCtxItem = (RMCtxType *)coreUtils_Q_Check(pClkDomainReqQ);
      for(index_a = 0; index_a < pClkDomainReqQ->nCnt; index_a++)
      {
          if(NULL != pLRMCtxItem)
          {
              if(RM_Valid == pLRMCtxItem->validFlag)
              {
                  if(NULL == ((AdsppmClkDomainReqType *)(pLRMCtxItem->pRequestData)))
                  {
                      ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "gpRMCtx[index_a].pRequestData =NULL\n");
                      sts = Adsppm_Status_BadParm;
                      break;
                  }
                  else
                  {
                      pClkDomainReqData = (AdsppmClkDomainReqType *)(pLRMCtxItem->pRequestData);
                      //checks array of clock settings
                      for(index_b = 0; index_b < pClkDomainReqData->numOfClk; index_b++)
                      {
                          ReqArrayIndexType clkIdx = pClkDomainReqData->pClkDomainArray[index_b].clkId - AdsppmClk_HwRsp_Core;
						  gCLKReqArray[clkIdx].clkDomainSrc.clkDomainSrcIdLpass =  pClkDomainReqData->pClkDomainArray[index_b].clkDomainSrc.clkDomainSrcIdLpass;						  
					
                      }
                  }
              }
              pLRMCtxItem = (RMCtxType *)coreUtils_Q_Next(pClkDomainReqQ, &(pLRMCtxItem->RMCtxLink));
          }
      }
      for(index_c = AdsppmClk_HwRsp_Core; index_c < AdsppmClk_EnumMax; index_c++)
      {
          ReqArrayIndexType clkIdx = index_c-AdsppmClk_HwRsp_Core;
          switch(gCLKReqArray[clkIdx].ClkType)
          {
			case AsicClk_TypeDalDomainSrc: 

              if(gCLKReqArray[clkIdx].clkDomainSrc.clkDomainSrcIdLpass != gCLKReqArray[clkIdx].clkDomainSrc_last.clkDomainSrcIdLpass)
              {
                 /* if(Adsppm_Status_Success !=
                      ClkRgm_SetClock(index_c,gCLKReqArray[clkIdx].value)) //TODO can change source
                  {
                      sts = Adsppm_Status_Failed;
                      break;
                  }
                  else  TODO can change source*/
                  {
			        gCLKReqArray[clkIdx].clkDomainSrc_last.clkDomainSrcIdLpass = gCLKReqArray[clkIdx].clkDomainSrc.clkDomainSrcIdLpass;
					  
                  }
              }
              break;
        
          default:
              break;
          }
      }
      ADSPPM_UNLOCK_RESOURCE(Adsppm_Rsc_Id_Core_Clk_Domain);
   }
   ADSPPM_LOG_FUNC_EXIT(sts);
   return sts;
}


Adsppm_Status CLK_GetInfo(AdsppmInfoClkFreqType * pClockInfo)
{
    Adsppm_Status sts = Adsppm_Status_Success;
     ADSPPM_LOG_FUNC_ENTER;
    if(NULL == pClockInfo )
    {
        sts = Adsppm_Status_BadParm;
        ADSPPM_LOG_PRINTF_0(ADSPPM_LOG_LEVEL_ERROR, "pClockInfo is NULL\n");

    }
    else
    {
        if(pClockInfo->forceMeasure)
        {
            sts = ClkRgm_CalcClockFrequency(pClockInfo->clkId, &pClockInfo->clkFreqHz);
        }
        else
        {
            sts = ClkRgm_GetClockFrequency(pClockInfo->clkId, &pClockInfo->clkFreqHz);
        }
    }
    return sts;
    ADSPPM_LOG_FUNC_EXIT(sts);
}

Adsppm_Status CLK_TurnOnSharedClk(void)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    sts = clkrgm_SharedClkControl(Hal_ClkRgmEnable);
    return sts;
}

Adsppm_Status CLK_TurnOffSharedClk(void)
{
    Adsppm_Status sts = Adsppm_Status_Success;
    sts = clkrgm_SharedClkControl(Hal_ClkRgmDisable);
    return sts;
}
