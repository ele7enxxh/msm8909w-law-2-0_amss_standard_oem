/*============================================================================
  @file sns_sam_io.c

  @brief
  Manages all input/output data storage for algorithms and clients within
  the SAM Framework.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include "sns_sam_io.h"
#include "sns_batchbuff.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/
/* When dropping unneeded data for non-wakeup clients, will drop this many more
 * bins past the first low memory threshold. */
#define SNS_SAM_DROP_BUFFER 15

/*============================================================================
  Static Data
  ===========================================================================*/

/* Low memory events received from the batchbuff module. */
sns_q_s samLowMemQ SNS_SAM_UIMAGE_DATA;
OS_EVENT *samLowMemQMutex SNS_SAM_UIMAGE_DATA;

/*===========================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Check all input and output data associated with a given algorithm instance.
 * If all data is in Island mark accordingly, otherwise leave markings alone.
 *
 * @param[io] algoInst The algorithm instance to update
 */
STATIC SNS_SAM_UIMAGE_CODE void
sns_sam_update_uimage_status( sns_sam_algo_inst *algoInst )
{
  sns_q_link_s *qCurr;

  if( SNS_SAM_ALGOINST_NOMEM_UIMAGE == algoInst->imageMode )
  {
    for( qCurr = sns_q_check( &algoInst->clientRequests ); NULL != qCurr;
         qCurr = sns_q_next( &algoInst->clientRequests, qCurr ) )
    {
      sam_client_req const *clientReq = (sam_client_req*)qCurr;
      sns_q_link_s *qCurrBin = sns_q_last_check_probe( &clientReq->outputDataQ );

      if( 0 != sns_q_cnt( &clientReq->outputDataQ )
          && !SNS_OS_IS_UHEAP_PTR(qCurrBin) )
      {
        // Every client must either: have an empty queue; last entry in uImage
        return ;
      }
    }

    for( qCurr = sns_q_check( &algoInst->algoInputQ ); NULL != qCurr;
         qCurr = sns_q_next( &algoInst->algoInputQ, qCurr ) )
    {
      sns_q_link_s *qCurrBin;
      sns_sam_algo_input_q *inputQ = (sns_sam_algo_input_q*)qCurr;

      if( !SNS_OS_IS_UHEAP_PTR(inputQ) )
      {
        return ;
      }

      for( qCurrBin = sns_q_check( &inputQ->inputDataQ ); NULL != qCurrBin;
           qCurrBin = sns_q_next( &inputQ->inputDataQ, qCurrBin ) )
      {
        sns_sam_algo_bin *inputBin = (sns_sam_algo_bin*)qCurrBin;

        if( !SNS_OS_IS_UHEAP_PTR(inputBin) )
        {
          return ;
        }
      }
    }
    SNS_UMSG_MEDIUM_1( samModule, "Moving %x back to uImage",
        *algoInst->algorithm->sensor.sensorUID );
    algoInst->imageMode = SNS_SAM_ALGOINST_UIMAGE;
  }
}

/**
 * Push an input or output data object to end of the given queue.  If
 * necessary, allocate additional data bins, and update the algorithm instance
 * image mode.
 *
 * @param[io] ioQueue The input/output queue to add the buffer (sns_sam_algo_bin)
 * @param[io] algoInst The associated algorithm instance
 * @param[i] ioData The data to be added
 * @param[i] dataSize The length of the data field within ioData
 */
STATIC SNS_SAM_UIMAGE_CODE void
sns_sam_io_push( sns_q_s *ioQueue, sns_sam_algo_inst *algoInst,
                 struct sns_sam_algo_io const *ioData, uint32_t dataSize )
{
  sns_sam_algo_bin *ioBin = (sns_sam_algo_bin*)sns_q_last_check( ioQueue );

  if( NULL == ioBin ||
      ioBin->writeOffset + dataSize + sizeof(sns_sam_timestamp) >= ioBin->bufferSize )
  {
    ioBin = NULL;

    // Always first attempt to allocate in Island for all non-bigImage algos
    if( SNS_SAM_ALGOINST_UIMAGE == algoInst->imageMode ||
        SNS_SAM_ALGOINST_NOMEM_UIMAGE == algoInst->imageMode )
    {
      ioBin = sns_batchbuff_malloc(
          SNS_BATCHBUFF_CLIENT_SAM, SNS_BATCHBUFF_UIMG_MEM );

      if( NULL == ioBin && SNS_SAM_ALGOINST_UIMAGE == algoInst->imageMode )
      {
        sns_sam_uimage_exit();
        SNS_PRINTF_STRING_MEDIUM_1( samModule,
            "Exiting uImage: Exhausted TCM bins (%x)",
            *algoInst->algorithm->sensor.sensorUID );
        algoInst->imageMode = SNS_SAM_ALGOINST_NOMEM_UIMAGE;
      }
      else if( NULL != ioBin)
      {
        if(SNS_SAM_ALGOINST_NOMEM_UIMAGE == algoInst->imageMode )
        {
          /* If a NOMEM algo inst, and we were just able to allocate an output
           * bin in island, we might be able to transition into uImage; check now */
          sns_sam_update_uimage_status( algoInst );
        }
      }
    }

    if( NULL == ioBin )
    {
      ioBin = sns_batchbuff_malloc(
          SNS_BATCHBUFF_CLIENT_SAM, SNS_BATCHBUFF_BIMG_MEM );
    }
    SNS_ASSERT(NULL != ioBin);

    ioBin->writeOffset = 0;
    ioBin->readOffset = 0;
    ioBin->bufferSize = sns_sam_io_size();
    sns_q_link( ioBin, &ioBin->qLink );
    sns_q_put( ioQueue, &ioBin->qLink );
  }

  SNS_OS_MEMSCPY((void*)&ioBin->buffer[ioBin->writeOffset], dataSize,
    (void*)ioData->data, dataSize);
  ioBin->writeOffset += dataSize;

  SNS_OS_MEMSCPY((void*)&ioBin->buffer[ioBin->writeOffset], sizeof(sns_sam_timestamp),
    (void*)&ioData->timestamp, sizeof(sns_sam_timestamp));
  ioBin->writeOffset += sizeof(sns_sam_timestamp);
}

/**
 * Remove the next input/output data object from the given queue.  May free
 * empty bins as appropriate, and upon each freed bin, check whether the
 * algorithm instance can again be run in uImage.
 *
 * @param[io] algoInst Algorithm instance associated with the input/output data
 * @param[i] ioQueue Input/Output data bin queue
 * @param[i] dataSize The size of the input/output data buffer
 * @param[i] releaseEmpty Whether to free bins as they become empty
 *
 * @return An input/output object with set timestamp and pointer to data
 */
STATIC SNS_SAM_UIMAGE_CODE struct sns_sam_algo_io
sns_sam_io_pop( sns_sam_algo_inst *algoInst, sns_q_s *ioQueue, uint32_t dataSize,
    bool releaseEmpty )
{
  sns_sam_algo_bin *ioBin = (sns_sam_algo_bin*)sns_q_check( ioQueue );
  struct sns_sam_algo_io rv = (struct sns_sam_algo_io)
    { .timestamp = -1, .data = (intptr_t)NULL };

  // If the bin we were using is empty, free it
  while( NULL != ioBin && ioBin->readOffset == ioBin->writeOffset )
  {
    if( releaseEmpty )
    {
      bool isUHeap = SNS_OS_IS_UHEAP_PTR( ioBin );
      SNS_UMSG_LOW_0( samModule, "Freeing newly empty IO bin" );
      sns_q_delete( &ioBin->qLink );
      sns_batchbuff_free( SNS_BATCHBUFF_CLIENT_SAM, ioBin );

      if( SNS_SAM_ALGOINST_NOMEM_UIMAGE == algoInst->imageMode && !isUHeap )
      {
        sns_sam_update_uimage_status( algoInst );
      }
      ioBin = (sns_sam_algo_bin*)sns_q_check( ioQueue );
    }
    else
    {
      ioBin = (sns_sam_algo_bin*)sns_q_next( ioQueue, &ioBin->qLink );
    }
  }

  if( NULL != ioBin )
  {
    rv.data = (intptr_t)&ioBin->buffer[ioBin->readOffset];
    ioBin->readOffset += dataSize;

    SNS_OS_MEMSCPY( &rv.timestamp, sizeof(sns_sam_timestamp),
        (void*)&ioBin->buffer[ioBin->readOffset],
        sizeof(sns_sam_timestamp) );
    ioBin->readOffset += sizeof(sns_sam_timestamp);
  }

  return rv;
}

/**
 * Allocate an input_q object for the given algorithm instance.  Update the
 * imageMode field accordingly.
 *
 * @param[io] algoInst Algorithm Instance for which to allocate
 *
 * @return Allocated input_q object, or NULL if no memory is available
 */
SNS_SAM_UIMAGE_CODE STATIC sns_sam_algo_input_q*
sns_sam_alloc_input_q( sns_sam_algo_inst *algoInst )
{
  sns_sam_algo_input_q *inputQ = NULL;

  if( SNS_SAM_ALGOINST_BIGIMAGE != algoInst->imageMode )
  {
    inputQ = SNS_OS_U_MALLOC( SNS_SAM_DBG_MOD, sizeof(sns_sam_algo_input_q) );

    if( NULL == inputQ )
    {
      sns_sam_uimage_exit();
      SNS_PRINTF_STRING_MEDIUM_1( samModule,
            "Exiting uImage: Unable to alloc input queue (%x)",
            *algoInst->algorithm->sensor.sensorUID );
      algoInst->imageMode = SNS_SAM_ALGOINST_NOMEM_UIMAGE;
    }
  }

  if( NULL == inputQ )
  {
    inputQ = SNS_OS_MALLOC( SNS_SAM_DBG_MOD, sizeof(sns_sam_algo_input_q) );
  }

  return inputQ;
}

/**
 * Calculate the maximum number of output objects that can be fit into a
 * single bin.
 *
 * @param[i] algoInst Algorithm for which to calculate
 *
 * @return Output capacity of one io bin
 */
SNS_SAM_UIMAGE_CODE STATIC uint32_t
sns_sam_output_cap( sns_sam_sensor_algo const *algo )
{
  return sns_sam_io_size() / ( sizeof(sns_sam_timestamp) + algo->bufferSizes.outputDataSize );
}

/**
 * Handle the low memory callback from the batchbuff module.
 *
 * See sns_batchbuff_threshold_cb for further information.
 */
SNS_SAM_UIMAGE_CODE STATIC void
sns_sam_lowmem_cb( sns_batchbuff_mem_type_e mem_type, uint32_t threshold )
{
  uint8_t errOS;
  sns_sam_io_lowmem_data *lowmemData =
     SNS_OS_ANY_MALLOC( SNS_SAM_DBG_MOD, sizeof(sns_sam_io_lowmem_data) );
  SNS_ASSERT( NULL != lowmemData);

  if( SNS_BATCHBUFF_UIMG_MEM == mem_type )
  {
    if( SNS_BATCHBUFF_UIMG_CB_THRESHOLD2 == threshold )
    {
      SNS_PRINTF_STRING_LOW_0( samModule, "CB: Flush Island -> DDR" );
      lowmemData->action = SNS_SAM_IO_FLUSH_DDR;
    }
  }
  else if( SNS_BATCHBUFF_BIMG_MEM == mem_type )
  {
    if( SNS_BATCHBUFF_BIMG_CB_THRESHOLD1 == threshold )
    {
      SNS_PRINTF_STRING_LOW_0( samModule, "CB: Drop unneeded Data" );
      lowmemData->action = SNS_SAM_IO_DROP_DATA;
    }
    else
    {
      SNS_PRINTF_STRING_LOW_0( samModule, "CB: Flush SSC -> AP" );
      lowmemData->action = SNS_SAM_IO_FLUSH_AP;
    }
  }

  sns_q_link( lowmemData, &lowmemData->qLink );
  sns_os_mutex_pend( samLowMemQMutex, 0, &errOS );
  sns_q_put( &samLowMemQ, (sns_q_link_s*)lowmemData );
  sns_os_mutex_post( samLowMemQMutex );
  sns_os_sigs_post( sns_sam_sig_event,
    SNS_SAM_LOWMEM_CB_SIG, OS_FLAG_SET, &errOS );
}

/**
 * Drop data until either the reserved FIFO size is reached, or the number
 * of free batchbuff bins exceeds the first low memory threshold.
 *
 * @param[io] clientReq Client whose output to drop
 * @param[i] reservedFifoSize At least how many samples to leave behind
 */
STATIC void
sns_sam_drop_oldest_output( sam_client_req *clientReq,
    uint32_t reservedFifoSize )
{
  sns_sam_sensor_algo const *algo = clientReq->algoInstance->algorithm;
  uint32_t outputCnt = sns_sam_output_cnt( clientReq );
  uint32_t extraBinCnt;
  uint32_t freedSampleCnt = 0;
  uint32_t i;

  extraBinCnt = ( outputCnt - reservedFifoSize ) /
      sns_sam_output_cap( algo );
  for( i = 0; i < extraBinCnt &&
        SNS_BATCHBUFF_BIMG_CB_THRESHOLD1 + SNS_SAM_DROP_BUFFER
          > sns_batchbuff_available_block_cnt( SNS_BATCHBUFF_BIMG_MEM );
       i++)
  {
    sns_sam_algo_bin *outputBin =
      (sns_sam_algo_bin*)sns_q_get( &clientReq->outputDataQ );
    SNS_ASSERT( NULL != outputBin );

    freedSampleCnt += (outputBin->writeOffset - outputBin->readOffset) /
      ( clientReq->algoInstance->algorithm->bufferSizes.outputDataSize
        + sizeof(sns_sam_timestamp) );

    sns_batchbuff_free( SNS_BATCHBUFF_CLIENT_SAM, outputBin );
  }
  clientReq->outputSampleCnt -= freedSampleCnt;

  SNS_PRINTF_STRING_MEDIUM_2( samModule,
      "Dropped %i outputs from %x due to low memory",
      freedSampleCnt, *clientReq->algorithm->sensor.sensorUID );
}

/*============================================================================
  Public Function Definitions
  ===========================================================================*/

SNS_SAM_UIMAGE_CODE uint32_t
sns_sam_output_cnt( sam_client_req const *clientReq )
{
  return clientReq->outputSampleCnt;
}

SNS_SAM_UIMAGE_CODE bool
sns_sam_output_bimg( sam_client_req const *clientReq )
{
  sns_q_link_s *qCurr;

  if( 0 == sns_q_cnt( &clientReq->outputDataQ ) )
  {
    return false;
  }

  for( qCurr = sns_q_check_probe( &clientReq->outputDataQ ); NULL != qCurr;
       qCurr = sns_q_next( &clientReq->outputDataQ, qCurr ) )
  {
    if( !SNS_OS_IS_UHEAP_PTR(qCurr) )
    {
      return true;
    }
  }

  return false;
}

SNS_SAM_UIMAGE_CODE void
sns_sam_output_push( sam_client_req *clientReq,
    sns_sam_algo_output const *output )
{
  uint32_t outputDataSize =
    clientReq->algoInstance->algorithm->bufferSizes.outputDataSize;

  sns_sam_io_push( &clientReq->outputDataQ, clientReq->algoInstance,
                   output, outputDataSize );
  clientReq->outputSampleCnt++;
}

SNS_SAM_UIMAGE_CODE sns_sam_algo_output
sns_sam_output_pop( sam_client_req *clientReq )
{
  uint32_t outputDataSize =
    clientReq->algoInstance->algorithm->bufferSizes.outputDataSize;
  sns_sam_algo_output output = sns_sam_io_pop( clientReq->algoInstance,
    &clientReq->outputDataQ, outputDataSize, false );

  if( NULL != (void*)output.data )
  {
    clientReq->outputSampleCnt--;
  }
  return output;
}

SNS_SAM_UIMAGE_CODE void
sns_sam_output_release( sam_client_req *clientReq, uint32_t cnt )
{
  sns_sam_algo_bin *ioBin;
  uint32_t outputDataSize = sizeof(sns_sam_timestamp) +
    clientReq->algoInstance->algorithm->bufferSizes.outputDataSize;
  bool isBHeap = false;

  // If samples need to be restored, do one one bin at a time
  while( 0 != cnt && 0 != sns_q_cnt( &clientReq->outputDataQ ) )
  {
    uint32_t offset, restoreCnt;
    sns_sam_algo_bin *next =
      (sns_sam_algo_bin*)sns_q_check( &clientReq->outputDataQ );
    // Find the most recent bin with that that can be restored
    do
    {
      ioBin = next;
      next = (sns_sam_algo_bin*)sns_q_next(
          &clientReq->outputDataQ, &next->qLink );
    } while( NULL != next && 0 != next->readOffset );

    offset = ioBin->readOffset;
    restoreCnt = offset / outputDataSize;

    if( restoreCnt > cnt )
    {
      restoreCnt = cnt;
      offset = restoreCnt * outputDataSize;
    }

    // Update the bin meta data to "restore" these samples
    ioBin->readOffset -= offset;
    clientReq->outputSampleCnt += restoreCnt;
    cnt -= restoreCnt;
  }

  // Now free and release any bins that remain consumed
  ioBin = (sns_sam_algo_bin*)sns_q_check( &clientReq->outputDataQ );
  while( NULL != ioBin && ioBin->readOffset == ioBin->writeOffset )
  {
    isBHeap |= !SNS_OS_IS_UHEAP_PTR( ioBin );
    SNS_UMSG_LOW_0( samModule, "Freeing empty IO bin" );
    sns_q_delete( &ioBin->qLink );
    sns_batchbuff_free( SNS_BATCHBUFF_CLIENT_SAM, ioBin );

    ioBin = (sns_sam_algo_bin*)sns_q_check( &clientReq->outputDataQ );
  }

  if( SNS_SAM_ALGOINST_NOMEM_UIMAGE == clientReq->algoInstance->imageMode && isBHeap )
  {
    sns_sam_update_uimage_status( clientReq->algoInstance );
  }
}

SNS_SAM_UIMAGE_CODE void
sns_sam_output_clear( sam_client_req *clientReq )
{
  sns_sam_algo_bin *outputBin;

  SNS_UMSG_LOW_2( samModule,
      "Clearing output queue for algo %x, queue cnt %i",
      *clientReq->algoInstance->algorithm->sensor.sensorUID,
      sns_sam_output_cnt( clientReq ) );

  while( NULL != ( outputBin =
        (sns_sam_algo_bin*)sns_q_get( &clientReq->outputDataQ ) ) )
  {
    sns_batchbuff_free( SNS_BATCHBUFF_CLIENT_SAM, outputBin );
  }
  clientReq->outputSampleCnt = 0;
}

SNS_SAM_UIMAGE_CODE uint32_t
sns_sam_input_cnt( sns_sam_algo_inst const *algoInst,
     sns_sam_sensor_uid const *sensorUID )
{
  sns_q_link_s *qCurr;
  uint32_t inputCnt = 0;

  for( qCurr = sns_q_check( &algoInst->algoInputQ ); NULL != qCurr;
       qCurr = sns_q_next( &algoInst->algoInputQ, qCurr ) )
  {
    sns_sam_algo_input_q *inputQ = (sns_sam_algo_input_q*)qCurr;

    if( NULL == sensorUID || sensorUID == inputQ->sensorUID )
    {
      inputCnt += inputQ->inputCnt;
    }
  }

  return inputCnt;
}

SNS_SAM_UIMAGE_CODE void
sns_sam_input_clear( sns_sam_algo_inst *algoInst )
{
  sns_sam_algo_input_q *intputQ;

  SNS_UMSG_LOW_2( samModule,
      "Clearing input queue for algo %x, queue cnt %i",
      *algoInst->algorithm->sensor.sensorUID,
      sns_sam_input_cnt( algoInst, NULL ) );

  while( NULL != ( intputQ =
        (sns_sam_algo_input_q*)sns_q_get( &algoInst->algoInputQ ) ) )
  {
    sns_sam_algo_bin *inputBin;
    while( NULL != ( inputBin =
          (sns_sam_algo_bin*)sns_q_get( &intputQ->inputDataQ ) ) )
    {
      sns_batchbuff_free( SNS_BATCHBUFF_CLIENT_SAM, inputBin );
    }

    SNS_OS_ANY_FREE( intputQ );
  }
}

SNS_SAM_UIMAGE_CODE void
sns_sam_input_push( sns_sam_algo_inst *algoInst,
    sns_sam_sensor_uid const *sensorUID, sns_sam_algo_input const *input )
{
  sns_q_link_s *qCurr;
  sns_sam_algo_input_q *inputQ = NULL;
  uint32_t inputDataSize =
    algoInst->algorithm->bufferSizes.inputDataSize;
  bool inputValid = true;

  for( qCurr = sns_q_check( &algoInst->algoInputQ ); NULL != qCurr;
       qCurr = sns_q_next( &algoInst->algoInputQ, qCurr ) )
  {
    inputQ = (sns_sam_algo_input_q*)qCurr;

    if( sensorUID == inputQ->sensorUID )
    {
      break;
    }
    inputQ = NULL;
  }

  if( NULL == inputQ )
  {
    SNS_UMSG_LOW_1( samModule,
      "Creating new input queue for %x", *sensorUID );

    inputQ = sns_sam_alloc_input_q( algoInst );
    SNS_ASSERT(NULL != inputQ);

    inputQ->inputCnt = 0;
    inputQ->sensorUID = sensorUID;
    sns_q_init( &inputQ->inputDataQ );
    sns_q_link( inputQ, &inputQ->qLink );
    sns_q_put( &algoInst->algoInputQ, &inputQ->qLink );
  }
  else
  {
    /* if lastInputTs is later or equal to current input ts, mark the input
       as invalid */
    if ( sns_sam_compare_ts(inputQ->lastInputTs, input->timestamp) >= 0 )
    {
      SNS_UMSG_MEDIUM_2( samModule,
        "Warning: Invalid input detected: inputSUID=0x%x, algoSUID=0x%x",
        *sensorUID, *algoInst->algorithm->sensor.sensorUID);
      SNS_UMSG_MEDIUM_2( samModule,
        "Warning: Invalid input detected: last_input_ts=%lu, cur_input_ts=%lu",
         inputQ->lastInputTs, input->timestamp);
      inputValid = false;
    }
  }

  if ( inputValid == true )
  {
  sns_sam_io_push( &inputQ->inputDataQ, algoInst, input, inputDataSize );
  inputQ->inputCnt++;
    inputQ->lastInputTs = input->timestamp;
  }
}

SNS_SAM_UIMAGE_CODE sns_sam_algo_input_q*
sns_sam_input_earliest( sns_sam_algo_inst *algoInst,
    sns_sam_sensor_uid const *sensorUID, sns_sam_timestamp *earliestTS )
{
  sns_sam_algo_input_q *rv = NULL;
  sns_q_link_s *qCurr;
  uint32_t inputDataSize = algoInst->algorithm->bufferSizes.inputDataSize;

  qCurr = sns_q_check( &algoInst->algoInputQ );
  while( NULL != qCurr )
  {
    sns_sam_algo_input_q *inputQ = (sns_sam_algo_input_q*)qCurr;
    qCurr = sns_q_next( &algoInst->algoInputQ, qCurr );

    if( 0 == inputQ->inputCnt )
    {
      // It's unlikely to cost us much memory, and we'll avoid a lot of
      // repeated malloc/free calls if we don't free inputQ objects here
      if( 0 != sns_q_cnt( &inputQ->inputDataQ ) )
      {
        sns_sam_io_pop( algoInst, &inputQ->inputDataQ, inputDataSize, true );
      }
      continue;
    }

    if( NULL == sensorUID )
    {
      // It's possible that the first bin on the queue is empty
      sns_sam_algo_bin *ioBin = (sns_sam_algo_bin*)sns_q_check( &inputQ->inputDataQ );
      while( NULL != ioBin )
      {
        if( ioBin->readOffset != ioBin->writeOffset )
        {
          sns_sam_timestamp timestamp;

          SNS_OS_MEMSCPY( &timestamp, sizeof(sns_sam_timestamp),
            (void*)&ioBin->buffer[ioBin->readOffset + inputDataSize],
            sizeof(sns_sam_timestamp) );

          if( NULL == rv || sns_sam_compare_ts( *earliestTS, timestamp ) >= 0 )
          {
            rv = inputQ;
            *earliestTS = timestamp;
          }
          break;
        }
        ioBin = (sns_sam_algo_bin*)sns_q_next( &inputQ->inputDataQ, &ioBin->qLink );
      }
    }
    else if( sensorUID == inputQ->sensorUID )
    {
      rv = inputQ;
      break;
    }
  }

  return rv;
}

SNS_SAM_UIMAGE_CODE sns_sam_algo_input
sns_sam_input_pop( sns_sam_algo_inst *algoInst,
    sns_sam_sensor_uid const *sensorUID, sns_sam_timestamp maxInputTS )
{
  sns_sam_algo_input_q *earliestQ;
  sns_sam_timestamp earliestTS;
  sns_sam_algo_input rv = (sns_sam_algo_input)
    { .timestamp = -1, .data = (intptr_t)NULL };
  uint32_t inputDataSize = algoInst->algorithm->bufferSizes.inputDataSize;

  earliestQ = sns_sam_input_earliest( algoInst, sensorUID, &earliestTS );
  if( NULL != earliestQ && ( SNS_SAM_TIMESTAMP_MAX == maxInputTS ||
        sns_sam_compare_ts( earliestTS, maxInputTS ) <= 0 ) )
  {
    earliestQ->inputCnt--;
    rv = sns_sam_io_pop( algoInst, &earliestQ->inputDataQ, inputDataSize, true );
  }

  return rv;
}

void
sns_sam_move_output( sam_client_req *clientReq )
{
  sns_q_link_s *qCurr = sns_q_check( &clientReq->outputDataQ );

  while( NULL != qCurr )
  {
    sns_sam_algo_bin *outputBin = (sns_sam_algo_bin*)qCurr;
    qCurr = sns_q_next( &clientReq->outputDataQ, qCurr );

    // Move Island bin to DDR, unless this is the last bin
    if( SNS_OS_IS_UHEAP_PTR(outputBin) && NULL != qCurr )
    {
      sns_sam_algo_bin *ddrBin = sns_batchbuff_malloc(
          SNS_BATCHBUFF_CLIENT_SAM, SNS_BATCHBUFF_BIMG_MEM );
      SNS_ASSERT(NULL != ddrBin);

      SNS_OS_MEMSCPY( ddrBin, sns_batchbuff_blocksize(),
        outputBin, sns_batchbuff_blocksize() );
      sns_q_link( ddrBin, &ddrBin->qLink );

      sns_q_insert( &ddrBin->qLink, &outputBin->qLink );
      sns_q_delete( &outputBin->qLink );
      sns_batchbuff_free( SNS_BATCHBUFF_CLIENT_SAM, outputBin );
    }
  }

  sns_sam_update_uimage_status( clientReq->algoInstance );
}

void
sns_sam_drop_unneeded_output()
{
  sns_q_link_s *qCurrAI, *qCurrCR;

  for( qCurrAI = sns_q_last_check_probe( &algoInstQ ); NULL != qCurrAI;
       qCurrAI = sns_q_prev( &algoInstQ, qCurrAI ) )
  {
    sns_sam_algo_inst *algoInst = (sns_sam_algo_inst*)qCurrAI;
    uint32_t reservedFifoSize = 
      algoInst->algorithm->sensor.attributes[SAM_ALGO_ATTR_RESERVED_FIFO_SIZE].supported ?
      algoInst->algorithm->sensor.attributes[SAM_ALGO_ATTR_RESERVED_FIFO_SIZE].attrValue:
      SNS_SAM_DEFAULT_RESERVED_FIFO_SIZE;

    for( qCurrCR = sns_q_check( &algoInst->clientRequests ); NULL != qCurrCR;
         qCurrCR = sns_q_next( &algoInst->clientRequests, qCurrCR ) )
    {
      sam_client_req *clientReq = (sam_client_req*)qCurrCR;
      uint32_t outputCnt = sns_sam_output_cnt( clientReq );
      bool nonwakeupClient =
        ( clientReq->clientAttr.notifySuspend.proc_type == SNS_PROC_APPS_V01 &&
        clientReq->clientAttr.notifySuspend.send_indications_during_suspend == FALSE );

      if( SNS_BATCHBUFF_BIMG_CB_THRESHOLD1 + SNS_SAM_DROP_BUFFER
            <= sns_batchbuff_available_block_cnt( SNS_BATCHBUFF_BIMG_MEM ) )
      {
        return;
      }
      if( outputCnt > reservedFifoSize &&
          ( ( nonwakeupClient && sns_sam_pm_ap_suspend() )
            || clientReq->clientBusy ) )
      {
        sns_sam_drop_oldest_output( clientReq, reservedFifoSize );
      }
    }
  }
}

uint32_t
sns_sam_io_size( void )
{
  return sns_batchbuff_blocksize() - sizeof(sns_sam_algo_bin) + sizeof(uint8_t);
}

uint32_t
sns_sam_io_cnt( void )
{
  return sns_batchbuff_available_block_cnt( SNS_BATCHBUFF_UIMG_MEM ) +
    sns_batchbuff_available_block_cnt( SNS_BATCHBUFF_BIMG_MEM );
}

sns_sam_err
sns_sam_io_init_fw( void )
{
  const uint8_t priority0 = 0;
  uint8_t errOS;

  samLowMemQMutex = sns_os_mutex_create_uimg( priority0, &errOS );
  if( 0 != errOS )
  {
    SNS_PRINTF_STRING_FATAL_1( samModule,
        "Cannot create io msg mutex %i", errOS );
    return SAM_EFAILED;
  }

  sns_q_init( &samLowMemQ );

  sns_batchbuff_cb_register( SNS_BATCHBUFF_CLIENT_SAM, SNS_BATCHBUFF_UIMG_MEM,
      sns_sam_lowmem_cb, SNS_BATCHBUFF_UIMG_CB_THRESHOLD2 );
  sns_batchbuff_cb_register( SNS_BATCHBUFF_CLIENT_SAM, SNS_BATCHBUFF_BIMG_MEM,
      sns_sam_lowmem_cb, SNS_BATCHBUFF_BIMG_CB_THRESHOLD2 );
  sns_batchbuff_cb_register( SNS_BATCHBUFF_CLIENT_SAM, SNS_BATCHBUFF_BIMG_MEM,
      sns_sam_lowmem_cb, SNS_BATCHBUFF_BIMG_CB_THRESHOLD1 );

  return SAM_ENONE;
}

