/*===========================================================================
  Copyright (c) 2011-2014 Qualcomm Technologies, Incorporated.
  All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsapps/sampleapp/src/app_content_protection.c#1 $

when       who    what, where, why
--------   ---    ----------------------------------------------------------
12/20/13   wt     Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/** @addtogroup Content_Protection
  @} */

#include "app_content_protection.h"

SampleClientResult mem_seg_copy(uint8* outBuff, uint32 outBuffLen, uint8* inBuff, uint32 inBuffLen)
{
  SampleClientResult status = SAMPLE_CLIENT_SUCCESS;
  int                ret    = 0;

  TZ_APPS_LOG( TZ_LOG_MSG_DEBUG,"TZ mem_seg_copy is starting...");

  do
  {
    //sanity check
    if(NULL == outBuff || NULL == inBuff || 0 >= outBuffLen || 0 >= inBuffLen)
    {
      TZ_APPS_LOG( TZ_LOG_MSG_ERROR, "%s Error: Invalid parameters!", __FUNCTION__);
      status = SAMPLE_CLIENT_ERROR_INVALID_PARAMS;
      break;
    }

    //check lengths
    if(inBuffLen > outBuffLen)
    {
      TZ_APPS_LOG( TZ_LOG_MSG_ERROR, "%s Error: output buffer is too small.", __FUNCTION__);
      status = SAMPLE_CLIENT_ERROR_MEM_SEG_COPY_FAILED;
      break;
    }

    //register buffers for copying
    ret = qsee_register_shared_buffer(inBuff, inBuffLen);
    if(ret)
    {
      TZ_APPS_LOG( TZ_LOG_MSG_ERROR, "%s Error: qsee_register_shared_buffer for input buffer failed.", __FUNCTION__);
      status = SAMPLE_CLIENT_ERROR_MEM_SEG_COPY_FAILED;
      break;
    }
    ret = qsee_register_shared_buffer(outBuff, outBuffLen);
    if(ret)
    {
      TZ_APPS_LOG( TZ_LOG_MSG_ERROR, "%s Error: qsee_register_shared_buffer for output buffer failed.", __FUNCTION__);
      status = SAMPLE_CLIENT_ERROR_MEM_SEG_COPY_FAILED;
      break;
    }

    //copy
    ret = memscpy(outBuff, outBuffLen, inBuff, inBuffLen);
    if(ret < inBuffLen)
    {
      TZ_APPS_LOG( TZ_LOG_MSG_ERROR, "%s Error: memscpy truncated the copy.", __FUNCTION__);
      status = SAMPLE_CLIENT_ERROR_MEM_SEG_COPY_FAILED;
      break;
    }

  }while(0);

  //deregister buffers
  ret = qsee_deregister_shared_buffer(inBuff);
  if(ret)
  {
    TZ_APPS_LOG( TZ_LOG_MSG_ERROR, "%s Error: qsee_deregister_shared_buffer for input buffer failed.", __FUNCTION__);
    status = SAMPLE_CLIENT_ERROR_MEM_SEG_COPY_FAILED;
  }
  ret = qsee_deregister_shared_buffer(outBuff);
  if(ret)
  {
    TZ_APPS_LOG( TZ_LOG_MSG_ERROR, "%s Error: qsee_deregister_shared_buffer for output buffer failed.", __FUNCTION__);
    status = SAMPLE_CLIENT_ERROR_MEM_SEG_COPY_FAILED;
  }

  if(status != SAMPLE_CLIENT_SUCCESS)
  {
    TZ_APPS_LOG(TZ_LOG_MSG_ERROR, "%s failed and returns %d!", __FUNCTION__, (int)status);
  }

  TZ_APPS_LOG(TZ_LOG_MSG_DEBUG,"%s Returns %d", __FUNCTION__, status);
  return status;

}

SampleClientResult Content_Protection_Copy_NonsecureToSecure
(
  const uint8       *nonSecBuffer,
  const uint32       nonSecBufferLength,
  tz_mem_array_s_t  *secBufferHandle,
  uint32             secBufferOffset,
  uint32            *secBufferLength
)
{
  SampleClientResult status              = SAMPLE_CLIENT_SUCCESS;
  uint32             size                = 0;
  uint8*             secBufferPtr        = NULL;
  uint8*             nonSecBufferPtr     = (uint8*)nonSecBuffer;
  uint32             segNum              = 0;
  uint32             sizeCopied          = 0;

  TZ_APPS_LOG( TZ_LOG_MSG_DEBUG,"TZ Content_Protection_Copy_NonsecureToSecure is starting...");

  do
  {
    //sanity check
    if(NULL == nonSecBuffer || 0 == nonSecBufferLength || NULL == secBufferHandle || NULL == secBufferLength)
    {
      TZ_APPS_LOG( TZ_LOG_MSG_ERROR, "%s Error: Invalid paramters!", __FUNCTION__);
      status = SAMPLE_CLIENT_ERROR_INVALID_PARAMS;
      break;	
    }
    
    /* check if the nonSecBuffer memory is not protected */
    if(qsee_is_ns_range(nonSecBuffer,nonSecBufferLength) == FALSE)
    {
      TZ_APPS_LOG( TZ_LOG_MSG_ERROR, "Error: trying to access secure memory region ");
      status = SAMPLE_CLIENT_ERROR_NSS_COPY_FAILED;
      break;
    }

    *secBufferLength = nonSecBufferLength;

    //get address and size of memory segment to copy
    secBufferPtr = (uint8*)(((secBufferHandle->_tz_mem_seg)[segNum])._address);
    size = ((secBufferHandle->_tz_mem_seg)[segNum])._size;

    //0 size means only one contiguous buffer
    if(0 == size)
    {
      /* check if the secBuffer memory is protected */
      if(qsee_is_s_tag_area(QSEE_MEM_TAG_USECASE_CP, (uint32)secBufferPtr, (uint32)(secBufferPtr + *secBufferLength)) == FALSE &&
         qsee_is_s_tag_area(QSEE_MEM_TAG_USECASE_AUDIO_CP, (uint32)secBufferPtr, (uint32)(secBufferPtr + *secBufferLength)) == FALSE &&
         qsee_is_s_tag_area(QSEE_MEM_TAG_USECASE_VIDEO_PIXEL_CP, (uint32)secBufferPtr, (uint32)(secBufferPtr + *secBufferLength)) == FALSE)
      {
        TZ_APPS_LOG( TZ_LOG_MSG_ERROR, "Error: Failed is s tag area check.");
        status = SAMPLE_CLIENT_ERROR_NSS_COPY_FAILED;
        break;
      }
  
      //copy entire non-secure buff into secure buff
      status = mem_seg_copy(secBufferPtr, *secBufferLength, (uint8*)nonSecBufferPtr, (uint32)nonSecBufferLength);
      if(status != SAMPLE_CLIENT_SUCCESS)
      {
        TZ_APPS_LOG(TZ_LOG_MSG_ERROR, "%s Error: copy to secure buffer failed! Returned %ld", __FUNCTION__, status);
        status = SAMPLE_CLIENT_ERROR_SNS_COPY_FAILED;
      }
      break;
    }

    //size was not 0. Need to copy to multiple memory segments
    do
    {
      //break when size is 0 or if address is garbage - no more segments to copy to
      if(0 == size || 0 >= secBufferPtr)
      {
        break;
      }

      //make sure we're not copying from past our source buffer's end
      sizeCopied = nonSecBufferPtr - nonSecBuffer;
      if(sizeCopied >= nonSecBufferLength)
      {
        TZ_APPS_LOG(TZ_LOG_MSG_ERROR, "%s Error: Trying to copy from past end of non-secure buffer. Failed!", __FUNCTION__, status);
        status = SAMPLE_CLIENT_ERROR_SNS_COPY_FAILED;
        break;
      }
    
      /* check if the secBuffer memory is protected */
      if(qsee_is_s_tag_area(QSEE_MEM_TAG_USECASE_CP, (uint32)secBufferPtr, (uint32)(secBufferPtr + size)) == FALSE &&
         qsee_is_s_tag_area(QSEE_MEM_TAG_USECASE_AUDIO_CP, (uint32)secBufferPtr, (uint32)(secBufferPtr + size)) == FALSE &&
         qsee_is_s_tag_area(QSEE_MEM_TAG_USECASE_VIDEO_PIXEL_CP, (uint32)secBufferPtr, (uint32)(secBufferPtr + size)) == FALSE)
      {
        TZ_APPS_LOG( TZ_LOG_MSG_ERROR, "Error: Failed is s tag area check.");
        status = SAMPLE_CLIENT_ERROR_NSS_COPY_FAILED;
        break;
      }

      //copy segment of non-secure buff into secure buff segment
      status = mem_seg_copy(secBufferPtr, size, (uint8*)nonSecBufferPtr, size);
      if(status != SAMPLE_CLIENT_SUCCESS)
      {
        TZ_APPS_LOG(TZ_LOG_MSG_ERROR, "%s Error: copy to secure buffer failed! Returned %ld", __FUNCTION__, status);
        status = SAMPLE_CLIENT_ERROR_SNS_COPY_FAILED;
        break;
      }

      //update destination pointer and segment number
      nonSecBufferPtr += size;
      segNum++;

      //don't go past max number of segments
      if(segNum >= TZ_OUT_BUF_MAX)
      {
        TZ_APPS_LOG(TZ_LOG_MSG_DEBUG, "Copied to maximum number of segments!");
        break;
      }

      //get next address and size of memory segment to copy to
      secBufferPtr = (uint8*)(secBufferHandle->_tz_mem_seg[segNum]._address);
      size = secBufferHandle->_tz_mem_seg[segNum]._size;

    }while(1);
    //end of copy segments loop
       
  } while(0);

  if(status != SAMPLE_CLIENT_SUCCESS)
    TZ_APPS_LOG(TZ_LOG_MSG_ERROR, "%s failed and returns %d!", __FUNCTION__, (int) status);
  
  TZ_APPS_LOG(TZ_LOG_MSG_DEBUG,"%s Returns %d", __FUNCTION__, status);
  return status;
}
