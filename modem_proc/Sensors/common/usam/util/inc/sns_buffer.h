#ifndef SNS_BUFFER_H
#define SNS_BUFFER_H

/*============================================================================
  @file sns_buffer.h

  Circular buffer utility header file

  Copyright (c) 2012-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

#include <stdint.h>
#include <stdbool.h>

/*
 * since BUF_SIZE is a power of 2,
 * (x & (BUF_SIZE-1)) == (x % BUF_SIZE)
 */
#define SNS_BUF_INDEX(index,bufSize) ((index) & (bufSize-1))

#define SNS_BUF_ELEM(buf,row,column) (*((buf)->data + row*(buf)->numDataAxes + column))
#define SNS_1D_BUF_ELEM(buf, offset) (*((buf)->data + offset))

#define sns_buffer_size sns_buffer_size_sns
#define sns_buffer_mem_req sns_buffer_mem_req_sns
#define sns_buffer_reset sns_buffer_reset_sns
#define sns_buffer_full sns_buffer_full_sns
#define sns_buffer_insert sns_buffer_insert_sns
#define sns_buffer_compute_variance sns_buffer_compute_variance_sns
#define sns_buffer_sprd sns_buffer_sprd_sns
#define sns_buffer_get_one_column_variance sns_buffer_get_one_column_variance_sns

/* Structure for circular buffer of data stored as floating point */
typedef struct
{
   float *data;
   uint32_t numDataAxes;
   uint32_t numDataValues;
   uint32_t windowSize;
   uint32_t end;
   uint32_t count;
} sns_buffer_type;

/*=======================================================================*/
/*!
    @brief Returns size round up to the nearest power of 2

    @param[i] windowSize: size of window (number of data elements)

    @return size of window round up to nearest power of 2
*/
/*=======================================================================*/
uint32_t sns_buffer_size_sns(
   uint32_t windowSize);

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
uint32_t sns_buffer_mem_req_sns(
   uint32_t windowSize,
   uint32_t numDataAxes);

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
void sns_buffer_reset_sns(
   sns_buffer_type* buffer,
   uint32_t windowSize,
   uint32_t numDataAxes,
   void* mem);

/*=========================================================================
  FUNCTION:  sns_buffer_full_sns
  =======================================================================*/
/*!
    @brief Indicate if buffer is full

    @param[i] buffer: pointer to buffer

    @return true - if buffer full, false - otherwise
*/
/*=======================================================================*/
bool sns_buffer_full_sns(
   const sns_buffer_type* buffer);

/*=========================================================================
  FUNCTION:  sns_buffer_insert_sns
  =======================================================================*/
/*!
    @brief Reset the buffer

    @param[io] buffer: pointer to buffer
    @param[i] data: pointer to data to be inserted in buffer

    @return None
*/
/*=======================================================================*/
void sns_buffer_insert_sns(
   sns_buffer_type* buffer,
   const float* data);

/*=========================================================================
  FUNCTION:  sns_buffer_compute_variance_sns
  =======================================================================*/
/*!
    @brief Compute the variance

    @param[i] buffer: pointer to buffer
    @param[o] sumData: sum of data elements for variance computation
    @param[o] sumSqData: sum of square data elements for variance computation

    @return variance
*/
/*=======================================================================*/
double sns_buffer_compute_variance_sns(
   const sns_buffer_type* buffer,
   double* sumData,
   double* sumSqData);

/*=========================================================================
  FUNCTION:  sns_buffer_sprd
  =======================================================================*/
/*!
    @brief Calculate the spread of the buffer
«
    @param[i] buffer: pointer to buffer
    @param[o] sprd_data: pointer to the spread value of buffer

    @return None
*/
/*=======================================================================*/

void sns_buffer_sprd_sns(sns_buffer_type *buffer, float *sprd_data);

/*=========================================================================
  FUNCTION:  sns_buffer_get_one_column_variance_hr
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
double sns_buffer_get_one_column_variance_sns(
   const sns_buffer_type* buffer,
   const double* sumData,
   const double* sumSquareData,
   const uint32_t column);

#endif /* SNS_BUFFER_H */
