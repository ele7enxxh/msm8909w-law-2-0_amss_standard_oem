/*!
@file
  rfca_dispatcher.cpp

*/


/*==============================================================================

  Copyright (c) 2011-2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/src/rfca_dispatcher.cpp#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/27/14   aki     Allow sleep after sequence has finished
01/24/14   aki     Using new debug flag
01/06/14   aki     LTETDD TX align
01/03/14   aki     Removed RFCA_ASSERT
11/19/13   aki     LTEFDD, WCDMA and TDS TX align
07/31/13   aki     Add IQ functions
07/31/13   aki     Remove old LTE APIs
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
06/10/13   aki     Mainline RFCA 
06/06/13   aki     Updates to remove KW warnings and errors
05/02/13   aki     Added new WAIT_V2 functions
04/26/13   aki     New LTE functions
10/02/12   aki     Remove RFCA_QMSL ifdefs
09/04/12   aki     Added status override for HAL response
07/23/12   aki     More TDSCDMA functions
07/13/12   aki     Added couple LTEFDD and TDSCDMA functions, fixed typo in msg,
                   removed gsm_tx_dac
05/25/12   aki     Added HAL_RF_INFORM
04/25/12   aki     Removed WCDMA TX ALIGN
04/23/12   aki     Stop new as very last response, 
03/01/12   aki     Changed <> to "", MSG macros to use HalLogWrite
02/17/12   aki     Added GSM_TX_ALIGN
02/08/12   aki     Allocate memory for response message
01/25/12   aki     Diag V2 delayed response implementation
10/28/11   aki     Initial revision

==============================================================================*/


#include "rfa_variation.h"
#include "comdef.h"
#include "stringl.h" /* for memscpy */

#include "rfca.h"
#include "rfca_custom_types.h"

#include "rfca_dispatcher.h"
#include "rfca_handler.h"
#include "rfca_helper.h"
#include "rfca_pending_message_queue.h"
#include "rfca_to_ftm_adapter.h"

#include "rfca_hal_msg_def.h"
#include "rfca_defs.h"

#include "ftm.h"
#include "msg.h"

/*----------------------------------------------------------------------------*/
void RfcaReqDispatcher( DIAG_RFCA_REQ *req ){  

  if(NULL == req)
  {
    MSG_ERROR("Request pointer is NULL!", 0, 0, 0);
    return;
  }

  HalLogWrite("Start processing RFCA package %s\n", HalGetMgsName(req->rfca_msg_id));

  uint16 opId = ((HAL_RF_COMMON_HEADER*)(req->data))->operation_id;
  if(NULL != FindFromRfcaResponseQueue(opId))
  {
    HalLogWrite("Item with same operation ID = %d is already in the queue, ignoring this one.\n", opId);
    return;
  }

  switch(req->rfca_msg_id)  {

  case HAL_RF_ARB_TX_REQ_ID:
    RFCA_arb_tx_send((HAL_RF_ARB_TX_REQ*)(req->data));
    break;

  case HAL_RF_CALIBRATION_VALUES_LOAD_REQ_ID:
    RFCA_calibration_values_load_send((HAL_RF_CALIBRATION_VALUES_LOAD_REQ*)(req->data));
    break;

  case HAL_RF_CALIBRATION_VALUES_STORE_REQ_ID:
    RFCA_calibration_values_store_send((HAL_RF_CALIBRATION_VALUES_STORE_REQ*)(req->data));
    break;

  case HAL_RF_CDMA2K_RX_ALIGN_REQ_ID:
    RFCA_cdma2k_rx_align_send((HAL_RF_CDMA2K_RX_ALIGN_REQ*)(req->data));
    break;

  case HAL_RF_CDMA2K_RX_IQ_REQ_ID:
    RFCA_cdma2k_rx_iq_send((HAL_RF_CDMA2K_RX_IQ_REQ*)(req->data));
    break;

  case HAL_RF_CDMA2K_RX_RSSI_REQ_ID:
    RFCA_cdma2k_rx_rssi_send((HAL_RF_CDMA2K_RX_RSSI_REQ*)(req->data));
    break;

  case HAL_RF_CDMA2K_RX_WAIT_REQ_ID:
    RFCA_cdma2k_rx_wait_send((HAL_RF_CDMA2K_RX_WAIT_REQ*)(req->data));
    break;

  case HAL_RF_CDMA2K_RX_WAIT_V2_REQ_ID:
    RFCA_cdma2k_rx_wait_v2_send((HAL_RF_CDMA2K_RX_WAIT_V2_REQ*)(req->data));
    break;	

  case HAL_RF_CDMA2K_TX_DAC_REQ_ID:
    RFCA_cdma2k_tx_dac_send((HAL_RF_CDMA2K_TX_DAC_REQ*)(req->data));
    break;
  
  case HAL_RF_CDMA2K_TX_DBM_REQ_ID:
    RFCA_cdma2k_tx_dbm_send((HAL_RF_CDMA2K_TX_DBM_REQ*)(req->data));
    break;

  case HAL_RF_GSM_RX_ALIGN_REQ_ID:
    RFCA_gsm_rx_align_send((HAL_RF_GSM_RX_ALIGN_REQ*)(req->data));
    break;

  case HAL_RF_GSM_RX_WAIT_REQ_ID:
    RFCA_gsm_rx_wait_send((HAL_RF_GSM_RX_WAIT_REQ*)(req->data));
    break;

  case HAL_RF_GSM_RX_WAIT_V2_REQ_ID:
    RFCA_gsm_rx_wait_v2_send((HAL_RF_GSM_RX_WAIT_V2_REQ*)(req->data));
    break;

  case HAL_RF_GSM_RX_IQ_REQ_ID:
    RFCA_gsm_rx_iq_send((HAL_RF_GSM_RX_IQ_REQ*)(req->data));
    break;

  case HAL_RF_GSM_RX_RSSI_REQ_ID:
    RFCA_gsm_rx_rssi_send((HAL_RF_GSM_RX_RSSI_REQ*)(req->data));
    break;

  case HAL_RF_GSM_TX_ALIGN_REQ_ID:
    RFCA_gsm_tx_align_send((HAL_RF_GSM_TX_ALIGN_REQ*)(req->data));
    break;

  case HAL_RF_GSM_TX_PCL_REQ_ID:
    RFCA_gsm_tx_pcl_send((HAL_RF_GSM_TX_PCL_REQ*)(req->data));
    break;

  case HAL_RF_INIT_REQ_ID:
    RFCA_init_send((HAL_RF_INIT_REQ*)(req->data));
    break;

  case HAL_RF_LTEFDD_RX_ALIGN_REQ_ID:
    RFCA_ltefdd_rx_align_send((HAL_RF_LTEFDD_RX_ALIGN_REQ*)(req->data));
    break;

  case HAL_RF_LTEFDD_RX_IQ_REQ_ID:
    RFCA_ltefdd_rx_iq_send((HAL_RF_LTEFDD_RX_IQ_REQ*)(req->data));
    break;

  case HAL_RF_LTEFDD_RX_RSSI_REQ_ID:
    RFCA_ltefdd_rx_rssi_send((HAL_RF_LTEFDD_RX_RSSI_REQ*)(req->data));
    break;

  case HAL_RF_LTEFDD_RX_WAIT_REQ_ID:
    RFCA_ltefdd_rx_wait_send((HAL_RF_LTEFDD_RX_WAIT_REQ*)(req->data));
    break;

  case HAL_RF_LTEFDD_RX_WAIT_V2_REQ_ID:
    RFCA_ltefdd_rx_wait_v2_send((HAL_RF_LTEFDD_RX_WAIT_V2_REQ*)(req->data));
    break;

  case HAL_RF_LTEFDD_TX_DAC_REQ_ID:
    RFCA_ltefdd_tx_dac_send((HAL_RF_LTEFDD_TX_DAC_REQ*)(req->data));
    break;

  case HAL_RF_LTEFDD_TX_DBM_REQ_ID:
    RFCA_ltefdd_tx_dbm_send((HAL_RF_LTEFDD_TX_DBM_REQ*)(req->data));
    break;

  case HAL_RF_LTEFDD_TX_ALIGN_REQ_ID:
    RFCA_ltefdd_tx_align_send((HAL_RF_LTEFDD_TX_ALIGN_REQ*)(req->data));
    break;

  case HAL_RF_LTETDD_RX_ALIGN_REQ_ID:
    RFCA_ltetdd_rx_align_send((HAL_RF_LTETDD_RX_ALIGN_REQ*)(req->data));
    break;

  case HAL_RF_LTETDD_RX_IQ_REQ_ID:
    RFCA_ltetdd_rx_iq_send((HAL_RF_LTETDD_RX_IQ_REQ*)(req->data));
    break;

  case HAL_RF_LTETDD_RX_RSSI_REQ_ID:
    RFCA_ltetdd_rx_rssi_send((HAL_RF_LTETDD_RX_RSSI_REQ*)(req->data));
    break;

  case HAL_RF_LTETDD_RX_WAIT_REQ_ID:
    RFCA_ltetdd_rx_wait_send((HAL_RF_LTETDD_RX_WAIT_REQ*)(req->data));
    break;

  case HAL_RF_LTETDD_TX_DAC_REQ_ID:
    RFCA_ltetdd_tx_dac_send((HAL_RF_LTETDD_TX_DAC_REQ*)(req->data));
    break;

  case HAL_RF_LTETDD_TX_DBM_REQ_ID:
    RFCA_ltetdd_tx_dbm_send((HAL_RF_LTETDD_TX_DBM_REQ*)(req->data));
    break;

  case HAL_RF_LTETDD_TX_ALIGN_REQ_ID:
    RFCA_ltetdd_tx_align_send((HAL_RF_LTETDD_TX_ALIGN_REQ*)(req->data));
    break;

  case HAL_RF_RFIC_READ_REQ_ID:
    RFCA_rfic_read_send((HAL_RF_RFIC_READ_REQ*)(req->data));
    break;

  case HAL_RF_RFIC_WRITE_REQ_ID:
    RFCA_rfic_write_send((HAL_RF_RFIC_WRITE_REQ*)(req->data));
    break;

  case HAL_RF_STOP_REQ_ID:
    RFCA_stop_send((HAL_RF_STOP_REQ*)(req->data));
    break;

  case HAL_RF_WCDMA_RX_ALIGN_REQ_ID:
    RFCA_wcdma_rx_align_send((HAL_RF_WCDMA_RX_ALIGN_REQ*)(req->data));
    break;

  case HAL_RF_WCDMA_RX_IQ_REQ_ID:
    RFCA_wcdma_rx_iq_send((HAL_RF_WCDMA_RX_IQ_REQ*)(req->data));
    break;

  case HAL_RF_WCDMA_RX_RSSI_REQ_ID:
    RFCA_wcdma_rx_rssi_send((HAL_RF_WCDMA_RX_RSSI_REQ*)(req->data));
    break;

  case HAL_RF_WCDMA_RX_WAIT_REQ_ID:
    RFCA_wcdma_rx_wait_send((HAL_RF_WCDMA_RX_WAIT_REQ*)(req->data));
    break;

  case HAL_RF_WCDMA_RX_WAIT_V2_REQ_ID:
    RFCA_wcdma_rx_wait_v2_send((HAL_RF_WCDMA_RX_WAIT_V2_REQ*)(req->data));
    break;

  case HAL_RF_WCDMA_TX_ALIGN_REQ_ID:
    RFCA_wcdma_tx_align_send((HAL_RF_WCDMA_TX_ALIGN_REQ*)(req->data));
    break;

  case HAL_RF_WCDMA_TX_DAC_REQ_ID:
    RFCA_wcdma_tx_dac_send((HAL_RF_WCDMA_TX_DAC_REQ*)(req->data));
    break;

  case HAL_RF_WCDMA_TX_DBM_REQ_ID:
    RFCA_wcdma_tx_dbm_send((HAL_RF_WCDMA_TX_DBM_REQ*)(req->data));
    break;

  case HAL_RF_INFORM_REQ_ID:
    RFCA_inform_send((HAL_RF_INFORM_REQ*)(req->data));
    break;

  case HAL_RF_TDSCDMA_RX_ALIGN_REQ_ID:
    RFCA_tdscdma_rx_align_send((HAL_RF_TDSCDMA_RX_ALIGN_REQ*)(req->data));
    break;

  case HAL_RF_TDSCDMA_RX_WAIT_REQ_ID:
    RFCA_tdscdma_rx_wait_send((HAL_RF_TDSCDMA_RX_WAIT_REQ*)(req->data));
    break;

  case HAL_RF_TDSCDMA_RX_WAIT_V2_REQ_ID:
    RFCA_tdscdma_rx_wait_v2_send((HAL_RF_TDSCDMA_RX_WAIT_V2_REQ*)(req->data));
    break;

  case HAL_RF_TDSCDMA_RX_IQ_REQ_ID:
    RFCA_tdscdma_rx_iq_send((HAL_RF_TDSCDMA_RX_IQ_REQ*)(req->data));
    break;

  case HAL_RF_TDSCDMA_RX_RSSI_REQ_ID:
    RFCA_tdscdma_rx_rssi_send((HAL_RF_TDSCDMA_RX_RSSI_REQ*)(req->data));
    break;

  case HAL_RF_TDSCDMA_TX_ALIGN_REQ_ID:
    RFCA_tdscdma_tx_align_send((HAL_RF_TDSCDMA_TX_ALIGN_REQ*)(req->data));
    break;

  case HAL_RF_TDSCDMA_TX_DAC_REQ_ID:
    RFCA_tdscdma_tx_dac_send((HAL_RF_TDSCDMA_TX_DAC_REQ*)(req->data));
    break;

  case HAL_RF_TDSCDMA_TX_DBM_REQ_ID:
    RFCA_tdscdma_tx_dbm_send((HAL_RF_TDSCDMA_TX_DBM_REQ*)(req->data));
    break;

  default:
    MSG_ERROR("INVALID OR UNIMPLEMENTED RFCA REQUEST, HAL MSG ID: %d \n", req->rfca_msg_id, 0, 0);
    break;
  }

  HalLogWrite("Stop processing RFCA package: %s\n", HalGetMgsName(req->rfca_msg_id));

  return;
}


#ifdef _WIN32
  #pragma pack(push,1) // Save previous, and turn on 1 byte alignment
  #pragma warning( push )
  #pragma warning( disable : 4200 ) // warning C4200: nonstandard extension used : zero-sized array in struct/union
#endif
/*----------------------------------------------------------------------------*/

/*!
@brief Async reponse message we expect to get from FTM sequencer after each 
command has been executed.

*/
typedef PACK(struct){
  uint8 CMD_CODE;
  uint8 MORE;
  uint16 LENGTH;                
  uint16 LENGTH_LRS;            //!< Same as LENGTH above
  uint16 LOG_CODE;
  uint32 TIMESTAMP_H;
  uint32 TIMESTAMP_L;
  uint16 FTM_LOG_ID;
  uint16 SEQUENCE_STATUS;       //!< 0 = Executing, 1 = Complete, seems that the "80-N0528-4 A" has this wrong
  uint32 COMMAND_STATUS;
  uint32 ID_NUMBER;             //!< ID_NUMBER assigned during the FTM_SEQ_QUEUE_COMMANDS command
  uint32 EXPECTED_START_TIME;
  uint32 ACTUAL_START_TIME;
  uint32 TIME_OVERRUN;
  uint32 EXECUTION_DURATION;
  uint32 FTM_RESPONSE_SIZE;     //!< Size of the FTM_RESPONSE_DATA field. Either 80-N0528-4 A defines this incorrectly as uint16 or duplicate ID_NUMBER is missing from Table 2-2
  uint8  FTM_RESPONSE_DATA[];
}ASYNC_FTM_SEQ_RESP;

#ifdef _WIN32
  // Restore warings and alignment
  #pragma warning( pop ) 
  #pragma pack(pop)
#endif

void RfcaRespDispatcher( 	
  unsigned short iMessageSize,
  unsigned char* iMessageBuffer)
{  

  if( (0 == iMessageSize) || (NULL == iMessageBuffer) )
  {    
    MSG_ERROR("Message size (=%d) is 0 empty or message pointer is NULL, nothing to do.", iMessageSize, 0, 0);
    return;
  }

  if(DIAG_BAD_COMMAND_CMD_CODE == iMessageBuffer[0])
  {
    // We received a bad command response....
    MSG_ERROR("Response received from UUT that a message we've sent has been incorrectly formatted.\n", 0, 0, 0);
    return;
  }

  ASYNC_FTM_SEQ_RESP* seqResp = (ASYNC_FTM_SEQ_RESP*)iMessageBuffer;
  if (iMessageSize < 14
    || seqResp->CMD_CODE != FTM_SEQ_DIAG_LOG_F 
    || seqResp->LOG_CODE != FTM_SEQ_LOG_CODE
    || seqResp->FTM_LOG_ID != FTM_SEQ_FTM_LOG_ID)
  {
    // Not interested in this message
    return;
  }

  PendingRfcaResponse *queueItem = FindFromRfcaResponseQueueByFtmSequenceId(seqResp->ID_NUMBER);
  if(NULL == queueItem)
  {
    // Means that we did not send this message
    return;
  }
  
  HalLogWrite("F(%05d): RECV SEQ RESP; EXPECTED_START_TIME = %8d, START = %8d, EXECUTION_DURATION = %8d, OVERRUN = %2d\n", seqResp->ID_NUMBER, seqResp->EXPECTED_START_TIME, seqResp->ACTUAL_START_TIME, seqResp->EXECUTION_DURATION, seqResp->TIME_OVERRUN);

  // Override any other command status
  if(0 != seqResp->TIME_OVERRUN)
  {
    queueItem->HAL.StatusOverride = HAL_RF_STATUS_TIMING_OVERRUN;
  }

  // Need to store this as the cannot look from queue after callback has been executed
  bool SequencerHasFinished = false;
  if(1 == seqResp->SEQUENCE_STATUS)
  {
    SequencerHasFinished =  true;
  }
  // Find FTM request
  FtmPacketQueueItem *ftmPacket = FindFromFtmIdQueue(queueItem->HalID, seqResp->ID_NUMBER);

  if(NULL == ftmPacket)
  {
    MSG_ERROR("Could not find FTM packet from message queue, HAL ID: %d, SEQ ID: %d", queueItem->HalID, seqResp->ID_NUMBER, 0);
    return;
  }

  // Count each response only once, with LTE responses are coming twice
  if(false == ftmPacket->ResponseReceived)
  {
    queueItem->FTM.ResponseCountdown--;
    ftmPacket->ResponseReceived = true;
  }

  if(false == ftmPacket->Packet.AllocateResponse((word)seqResp->FTM_RESPONSE_SIZE))
  {
    HalLogWrite("Failed to allocate memory for reponse buffer.\n");      
    return;
  }
  // Copy response data
  memscpy(ftmPacket->Packet.GetResponseDataPtr(), seqResp->FTM_RESPONSE_SIZE,
          &seqResp->FTM_RESPONSE_DATA, seqResp->FTM_RESPONSE_SIZE); 
  // Set reponse size
  memscpy(ftmPacket->Packet.GetResponseLengthPtr(), sizeof(word), 
          &seqResp->FTM_RESPONSE_SIZE, sizeof(word));
  
  if(true == rfcaDebugMode)
  {
    hex_dump(seqResp->FTM_RESPONSE_DATA, seqResp->FTM_RESPONSE_SIZE);
  }

  if(0 == queueItem->FTM.ResponseCountdown)
  {
    if(NULL != queueItem->FTM.CallbackFunction)
    {
      queueItem->FTM.CallbackFunction(queueItem->HalID);
    }
    // TODO: Send it here or in callback?
    if(GetStopRequestHalId() != queueItem->HalID)
    {
      if(HAL_RF_STATUS_OK != queueItem->HAL.StatusOverride)
      {
        ((HAL_RF_INIT_RESP*)(queueItem->HAL.HalResponse))->status = queueItem->HAL.StatusOverride;
      }
      SendPopDelayedFromRfcaResponseQueue(queueItem->HalID);
    }
  }

  // Update global FTM sequencer status
  // and call function to send response to stop request
  if(true == SequencerHasFinished)
  {
    HalLogWrite("FTM Sequencer has finished!\n");

    RFCA_ftm_sequencer_completed();
    SendPopDelayedFromRfcaResponseQueue(GetStopRequestHalId());
    RFCA_allow_sleep();
  }

  return;
}
