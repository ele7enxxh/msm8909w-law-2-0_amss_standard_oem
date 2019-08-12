/*============================================================================
  @file sns_buffer.c

  Circular buffer utility source file

  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

#include "sns_buffer.h"
/*
#ifdef SNS_LA
#include <cutils/log.h>
#define LOG_TAG "BUFFER"
//#define LOG_NDDEBUG 0
//#define LOG_NIDEBUG 0
#else
#include <stdio.h>
#define LOGE printf("\n");printf
#define LOGD LOGE
#endif
*/
/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/

#ifdef SNS_USES_ISLAND
#define SNS_SAM_UIMAGE_CODE __attribute__((section (".text.uSAM")))
#define SNS_SAM_UIMAGE_DATA __attribute__((section (".data.uSAM")))
#else
#define SNS_SAM_UIMAGE_CODE
#define SNS_SAM_UIMAGE_DATA
#endif /* USES_ISLAND */


/*=========================================================================
  FUNCTION:  sns_buffer_size_sns
  =======================================================================*/
/*!
    @brief Returns size round up to the nearest power of 2

    @param[i] windowSize: size of window (number of data elements)

    @return size of window round up to nearest power of 2
*/
/*=======================================================================*/
SNS_SAM_UIMAGE_CODE uint32_t sns_buffer_size_sns(
   uint32_t windowSize)
{
   windowSize |= windowSize >> 1;
   windowSize |= windowSize >> 2;
   windowSize |= windowSize >> 4;
   windowSize |= windowSize >> 8;
   windowSize |= windowSize >> 16;
   windowSize++;

   return windowSize;
}

/*=========================================================================
  FUNCTION:  sns_buffer_mem_req_sns
  =======================================================================*/
/*!
    @brief Returns memory needed for buffer with specified params

    @param[i] windowSize: size of window (number of data elements)
    @param[i] numDataAxes: number of axes for each data element

    @return Size of memory required
*/
/*=======================================================================*/
SNS_SAM_UIMAGE_CODE uint32_t sns_buffer_mem_req_sns(
   uint32_t windowSize,
   uint32_t numDataAxes)
{
   return(sizeof(float) * sns_buffer_size_sns(windowSize) * numDataAxes);
}

/*=========================================================================
  FUNCTION:  sns_buffer_reset_sns
  =======================================================================*/
/*!
    @brief Reset the buffer

    @param[io] buffer: pointer to buffer
    @param[i] windowSize: size of window (number of data elements)
    @param[i] numDataAxes: number of axes for each data element
    @param[i] mem: memory provided for data buffer

    @return None
*/
/*=======================================================================*/
SNS_SAM_UIMAGE_CODE void sns_buffer_reset_sns(
   sns_buffer_type* buffer,
   uint32_t windowSize,
   uint32_t numDataAxes,
   void *mem)
{
   uint32_t i, j;

   buffer->data = mem;
   buffer->numDataAxes = numDataAxes;
   buffer->numDataValues = sns_buffer_size_sns(windowSize);
   buffer->windowSize = windowSize;
   buffer->end   = 0;
   buffer->count = 0;

   for (i=0; i< buffer->numDataValues; i++)
   {
      for (j=0; j< buffer->numDataAxes; j++)
      {
         SNS_BUF_ELEM(buffer, i, j) = 0;
      }
   }

   //LOGD("%s: windowSize=%d, numDataValues=%d, numDataAxes=%d",
   //     __FUNCTION__, buffer->windowSize, buffer->numDataValues, buffer->numDataAxes);
}

/*=========================================================================
  FUNCTION:  sns_buffer_full_sns
  =======================================================================*/
/*!
    @brief Indicate if buffer is full

    @param[i] buffer: pointer to buffer

    @return true - if buffer full, false - otherwise
*/
/*=======================================================================*/
SNS_SAM_UIMAGE_CODE bool sns_buffer_full_sns(
   const sns_buffer_type* buffer)
{
   return(buffer->count >= buffer->windowSize);
}

/*=========================================================================
  FUNCTION:  sns_buffer_insert_sns
  =======================================================================*/
/*!
    @brief Inserts an element into the the buffer

    @param[i] buffer: pointer to buffer
    @param[i] newData: pointer to data to be inserted in buffer

    @return None
*/
/*=======================================================================*/
SNS_SAM_UIMAGE_CODE void sns_buffer_insert_sns(
   sns_buffer_type* buffer,
   const float* newData)
{
   uint32_t i;

   for (i=0; i < buffer->numDataAxes; i++)
   {
      SNS_BUF_ELEM(buffer, buffer->end, i) = newData[i];
   }

   buffer->end = SNS_BUF_INDEX(buffer->end+1, buffer->numDataValues);

   if (buffer->count < buffer->numDataValues)
   {
      buffer->count++;
   }

   //LOGD("%s: buffer count %d, end %d",
   //     __FUNCTION__, buffer->count, buffer->end);
}

/*=========================================================================
  FUNCTION:  sns_buffer_compute_variance_sns
  =======================================================================*/
/*!
    @brief Reset the buffer

    @param[i] buffer: pointer to buffer
    @param[o] sumData: sum of data elements for variance computation
    @param[o] sumSqData: sum of square data elements for variance computation

    @return variance
*/
/*=======================================================================*/
SNS_SAM_UIMAGE_CODE double sns_buffer_compute_variance_sns(
   const sns_buffer_type* buffer,
   double* sumData,
   double* sumSquareData)
{
   double varTerm1, varTerm2, variance = 0;
   uint32_t i, oldDataIndex, newDataIndex;
   double oldData, newData;

   oldDataIndex = SNS_BUF_INDEX(buffer->end - buffer->windowSize-1, buffer->numDataValues);
   newDataIndex = SNS_BUF_INDEX(buffer->end-1, buffer->numDataValues);

   for (i=0; i<buffer->numDataAxes; i++)
   {
      oldData = SNS_BUF_ELEM(buffer, oldDataIndex, i);
      newData = SNS_BUF_ELEM(buffer, newDataIndex, i);
      sumData[i] += (newData - oldData);
      sumSquareData[i] += (newData * newData - oldData * oldData);
   }

   //LOGD("%s: buffer sumData[0] %f, sumData[1] %f, sumData[2] %f",
   //     __FUNCTION__, sumData[0], sumData[1], sumData[2]);
   //LOGD("%s: buffer sumSquareData[0] %f, sumSquareData[1] %f, sumSquareData[2] %f",
   //     __FUNCTION__, sumSquareData[0], sumSquareData[1], sumSquareData[2]);

   if (sns_buffer_full_sns(buffer))
   {
      varTerm1 = sumSquareData[0] + sumSquareData[1] + sumSquareData[2];

      varTerm2 = (sumData[0]*sumData[0] + sumData[1]*sumData[1] + sumData[2]*sumData[2]);

      variance = (varTerm1 - varTerm2 / buffer->windowSize) / buffer->windowSize;

      //LOGD("%s: buffer varTerm1 %f, varTerm2 %f, variance %f",
      //     __FUNCTION__, varTerm1, varTerm2, variance);
   }

   return variance;
}

/*=========================================================================
  FUNCTION:  sns_buffer_sprd_sns
  =======================================================================*/
/*!
    @brief Compute the buffer spread

    @param[i] buffer: pointer to buffer
    @param[o] sprd_data: Buffer spread

    @return variance
*/
/*=======================================================================*/

SNS_SAM_UIMAGE_CODE void sns_buffer_sprd_sns(sns_buffer_type *buffer, float *sprd_data)
{
  int32_t j, min_index, max_index, start_index, end_index;
  float buf_min, buf_max, cur_val;

  end_index   = buffer->end;
  start_index = end_index - buffer->windowSize;

    max_index = start_index;
    min_index = max_index;

    buf_min = SNS_1D_BUF_ELEM(buffer, SNS_BUF_INDEX(max_index, buffer->numDataValues));
    buf_max = buf_min;

    for (j=start_index+1; j<end_index; j++)
    {
      cur_val = SNS_1D_BUF_ELEM(buffer, SNS_BUF_INDEX(j, buffer->numDataValues));

      if (cur_val < buf_min)
      {
        buf_min = cur_val;
        min_index = j;
      }

      if (cur_val > buf_max)
      {
        buf_max = cur_val;
        max_index = j;
      }
    }
    *sprd_data = (max_index<min_index)?(-(buf_max-buf_min)):(buf_max-buf_min);
}

/*=========================================================================
  FUNCTION:  sns_buffer_get_one_column_variance_sns
  =======================================================================*/
/*!
    @brief Reset the buffer

    @param[i] buffer: pointer to buffer
    @param[i] sumData: sum of data elements for variance computation
    @param[i] sumSqData: sum of square data elements for variance computation
    @param[i] column index 

    @return one column variance
*/
/*=======================================================================*/
SNS_SAM_UIMAGE_CODE double sns_buffer_get_one_column_variance_sns(
   const sns_buffer_type* buffer,
   const double* sumData,
   const double* sumSquareData,
   const uint32_t column)
{
   if (column >= buffer->numDataAxes || buffer->count < 2) 
       return 0;
   else 
   {
       uint32_t num_valid_elements = sns_buffer_full_sns(buffer)? buffer->windowSize:buffer->count;

       double varTerm1 = sumSquareData[column];
       
       double varTerm2 = (sumData[column]*sumData[column]);
       
       double variance = (varTerm1 - varTerm2 / num_valid_elements) / (num_valid_elements-1);

       if (variance < 0) variance = 0;

       return variance;

   }
}