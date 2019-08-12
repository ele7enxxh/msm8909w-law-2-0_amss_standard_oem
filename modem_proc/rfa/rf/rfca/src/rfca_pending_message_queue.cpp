/*!
  @file
  rfca_pending_message_queue.cpp
 
  @brief
  Implementation to manage pending RFCA respose queue.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/src/rfca_pending_message_queue.cpp#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
02/06/14   aki     Free request packet when RFCA packet gets deleted
01/23/14   aki     Only log FTM as hex if debug flag is on
01/14/14   aki     Shrink FTM message buffer and do not print '%' to hex dump
01/03/14   aki     Removed RFCA_ASSERT and added memset for queue items
11/19/13   aki     Removed IterateRfcaCommandQueue and fixed error messages
08/12/13   aki     Fixed KW errors
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
06/10/13   aki     Mainline RFCA 
06/06/13   aki     Updates to remove KW warnings and errors
06/06/13   aki     Update to first byte check from strlen in hex_dump debug helper
05/30/13   aki     Print FTM messages as hex to F3 if RFCA_DEBUG is on
04/30/13   aki     Add F3 message with HAL ID and status when command has been done
04/03/13   aki     Fixed memory leaks
12/03/12   aki     Convert volatile mutex to REX critical section
10/02/12   aki     Remove RFCA_QMSL ifdefs
09/04/12   aki     Defaulting HAL response status override to OK
08/16/12   aki     Added log message just before calling diagpkt_commit
08/01/12   jr      Replaced diag_free with diagpkt_free as per diag interface changes
07/09/12   aki     Truncate immediate DIAG response if it is too long
05/25/12   aki     Cleanup
05/17/12   aki     Support for immediate diag response allocation in RFCA_diag 
02/24/12   aki     Removed RFCA_DEBUG around HalLogWrite
02/08/12   aki     Memory management improvements
01/25/12   aki     Diag V2 delayed response implementation
10/28/11   aki     Initial revision

==============================================================================*/


#include "rfa_variation.h"
#include "comdef.h"
#include "stringl.h" /* for memscpy */

#include "rfca_custom_types.h"
#include "rfca_helper.h"

#include "rfca_dispatcher.h"
#include "rfca_handler.h"
#include "rfca_mem_helper.h"
#include "rfca_pending_message_queue.h"

#include "rfca_hal_msg_def.h"
#include "stdio.h"
#include "rfca_defs.h"

#include "ftm.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "rfca.h"

// Local function implemented at the end of this file
static void SendDelayedFromRfcaResponseQueue(unsigned int rfcaID);

/*----------------------------------------------------------------------------*/
/*!
 @brief Critical section to initialize 

 */
extern "C"
{
  rex_crit_sect_type RfcaCommandQueueLock;
}

/*----------------------------------------------------------------------------*/
/*!
 @brief Retrieves lock for RfcaCommandQueue access

 @details Lock is used to limit access into RfcaCommandQueue for single client at
          a time. Each client must request and get the lock before accessing
          RfcaCommandQueue.

 @param whoAreYou Name of caller. This is used for debug purposes only.
 */
void GetRfcaCommandQueueLock(const char *whoAreYou)
{
  rex_enter_crit_sect(&RfcaCommandQueueLock);
}

/*----------------------------------------------------------------------------*/
/*!
 @brief Releases lock for RfcaCommandQueue access

 @param whoAreYou Name of caller. This is used for debug purposes only.
 */
void ReleaseRfcaCommandQueueLock(const char *whoAreYou)
{
  rex_leave_crit_sect(&RfcaCommandQueueLock);
}

PendingRfcaResponse * RfcaCommandQueue = NULL;        ///< Global queue to store RFCA messages which are currenlty in progress
PendingRfcaResponse * TailOfRfcaCommandQueue = NULL;

void CheckQueuePointers()
{
  if( ((NULL == RfcaCommandQueue) && (NULL != TailOfRfcaCommandQueue)) || ((NULL != RfcaCommandQueue) && (NULL == TailOfRfcaCommandQueue)) )
  {
    if(NULL != TailOfRfcaCommandQueue)
    {
      HalLogWrite("RfcaCommandQueue and TailOfRfcaCommandQueue do not match, one is NULL. Don't just ignore this error, fix it instead!\n");
    }
  }
}
/*----------------------------------------------------------------------------*/
PendingRfcaResponse * AddToRfcaResponseQueue(unsigned int rfcaID, void* halRequest, uint16 sizeOfHalRequest, uint16 sizeOfHalResponse, HAL_MESSAGE_ID rfcaCommandCode, FtmResponseCallback callback)
{
  GetRfcaCommandQueueLock(__FUNCTION__);
  CheckQueuePointers();
  PendingRfcaResponse *newItem = (PendingRfcaResponse *)RFCA_Malloc(sizeof(PendingRfcaResponse));
  if(NULL == newItem)
  {
    MSG_ERROR("Failed to allocate memory for RFCA reponse queue item.", 0, 0, 0);
    return NULL;
  }

  memset(newItem, 0, sizeof(PendingRfcaResponse));
  newItem->HalID = rfcaID;
  newItem->HAL.HalMessageID = rfcaCommandCode;
  newItem->HAL.DelayedDiagResponseId = 0;
  newItem->HAL.StatusOverride = HAL_RF_STATUS_OK;
  
  newItem->HAL.HalResponse = RFCA_Malloc(sizeOfHalResponse);
  if(NULL == newItem->HAL.HalResponse)
  {
    MSG_ERROR("Failed to allocate memory for RFCA reponse.", 0, 0, 0);
    RFCA_Free(newItem);
    newItem = NULL;
    return NULL;
  }

  memset(newItem->HAL.HalResponse , 0, sizeOfHalResponse);
  // Knowing that all messages have ID as the first field we can fill this already
  ((HAL_RF_COMMON_HEADER*)newItem->HAL.HalResponse)->operation_id = rfcaID;
  // Also knowing that all response messages have status field we can fill this already
  ((HAL_RF_INIT_RESP*)newItem->HAL.HalResponse)->status = HAL_RF_STATUS_FAILED;
  newItem->HAL.SizeOfHalResponse = sizeOfHalResponse;

  newItem->HAL.HalRequest = RFCA_Malloc(sizeOfHalRequest);
  if(NULL == newItem->HAL.HalRequest)
  {
    MSG_ERROR("Failed to allocate memory for RFCA request.", 0, 0, 0);
    RFCA_Free(newItem->HAL.HalResponse);
    newItem->HAL.HalResponse = NULL;
	
    RFCA_Free(newItem);
    newItem = NULL;
    return NULL;
  }

  memscpy(newItem->HAL.HalRequest, sizeOfHalRequest, halRequest, sizeOfHalRequest);
  newItem->HAL.SizeOfHalRequest = sizeOfHalRequest;

  newItem->FTM.CallbackFunction = callback;
  newItem->FTM.FtmItems = NULL;
  newItem->FTM.TailOfFtmItems = NULL;
  newItem->NextItem = NULL;
  newItem->FTM.ResponseCountdown = 0;
  
  if(NULL == RfcaCommandQueue)
  {
    RfcaCommandQueue = newItem;
  }
  else
  {
    TailOfRfcaCommandQueue->NextItem = newItem;
  }
  TailOfRfcaCommandQueue = newItem;
  ReleaseRfcaCommandQueueLock(__FUNCTION__);

  return newItem;
}


/*----------------------------------------------------------------------------*/
PendingRfcaResponse *FindFromRfcaResponseQueue(unsigned int rfcaID)
{
  GetRfcaCommandQueueLock(__FUNCTION__);
  CheckQueuePointers();
  PendingRfcaResponse *rfcaCommand = RfcaCommandQueue;
  if(NULL == RfcaCommandQueue)
  {
    ReleaseRfcaCommandQueueLock(__FUNCTION__);
    return NULL;  
  }
  {
    while(NULL != rfcaCommand )
    {
      if(rfcaCommand->HalID == rfcaID)
      {
        ReleaseRfcaCommandQueueLock(__FUNCTION__);
        return rfcaCommand;
      }
      rfcaCommand = rfcaCommand->NextItem;
    }   
  }
  ReleaseRfcaCommandQueueLock(__FUNCTION__);

  return rfcaCommand;
}

/*----------------------------------------------------------------------------*/
void AddToFtmIdQueue(unsigned int rfcaID, FtmPacketQueueItem *newItem)
{

  PendingRfcaResponse *queueItem = FindFromRfcaResponseQueue(rfcaID);
  if(NULL == queueItem)
  {
    return ;
  }
  GetRfcaCommandQueueLock(__FUNCTION__);
  
  // At this point we want to make sure the FTM request is only using 
  // space for the payload thus we need to resize the buffer
  newItem->Packet.ResizeRequest();
  {    
    newItem->NextItem = NULL;

    if(NULL == queueItem->FTM.FtmItems)
    {
      queueItem->FTM.FtmItems = newItem;
    }
    else
    {
      queueItem->FTM.TailOfFtmItems->NextItem = newItem;
    }
    queueItem->FTM.TailOfFtmItems = newItem;
  }
  queueItem->FTM.ResponseCountdown++;
  ReleaseRfcaCommandQueueLock(__FUNCTION__);

  if(true == rfcaDebugMode)
  {
    hex_dump(newItem->Packet.GetRequestDataPtr(), newItem->Packet.GetRequestLength());
  }
}

/*----------------------------------------------------------------------------*/
FtmPacketQueueItem * FindFromFtmIdQueue( unsigned int rfcaID, unsigned int sequenceID )
{
  PendingRfcaResponse *queueItem = FindFromRfcaResponseQueue(rfcaID);  
  if(NULL == queueItem)
  {    
    return NULL;
  }

  GetRfcaCommandQueueLock(__FUNCTION__);
  
  if(NULL == queueItem->FTM.FtmItems)
  {
    ReleaseRfcaCommandQueueLock(__FUNCTION__);
    return NULL;
  }
    
  FtmPacketQueueItem * ftmItem = queueItem->FTM.FtmItems;
  while (NULL != ftmItem)
  {
    if(sequenceID == ftmItem->SequenceID)
    {
      ReleaseRfcaCommandQueueLock(__FUNCTION__);
      return ftmItem;
    }
    ftmItem = ftmItem->NextItem;
  }
  ReleaseRfcaCommandQueueLock(__FUNCTION__);
  return NULL;
}

/*----------------------------------------------------------------------------*/
PendingRfcaResponse * FindFromRfcaResponseQueueByFtmSequenceId( unsigned int sequenceID )
{
  GetRfcaCommandQueueLock(__FUNCTION__);
  CheckQueuePointers();

  PendingRfcaResponse *rfcaCommand = RfcaCommandQueue;
  if(NULL == RfcaCommandQueue)
  {
    ReleaseRfcaCommandQueueLock(__FUNCTION__);
    return NULL;  
  }
  // Loop RFCA commands
  while(NULL != rfcaCommand )
  {
    FtmPacketQueueItem * ftmItem = rfcaCommand->FTM.FtmItems;
    // Loop FTM commands
    while (NULL != ftmItem)
    {
      if(sequenceID == ftmItem->SequenceID)
      {
        ReleaseRfcaCommandQueueLock(__FUNCTION__);
        return rfcaCommand;
      }
      ftmItem = ftmItem->NextItem;
    }
    rfcaCommand = rfcaCommand->NextItem;
  }   
  ReleaseRfcaCommandQueueLock(__FUNCTION__);
  return NULL;
}


/*----------------------------------------------------------------------------*/
void PopFromRfcaResponseQueue(unsigned int rfcaID)
{
  PendingRfcaResponse *rfcaCommand = RfcaCommandQueue;
  PendingRfcaResponse *previousQueueItem = NULL;
  GetRfcaCommandQueueLock(__FUNCTION__);
  CheckQueuePointers();

  while(NULL != rfcaCommand )
  {

    if(rfcaCommand->HalID == rfcaID)
    {
      // Found it
      if(NULL != rfcaCommand->HAL.HalRequest)
      {
        RFCA_Free(rfcaCommand->HAL.HalRequest);
      }
      rfcaCommand->HAL.HalRequest = NULL;
      rfcaCommand->HAL.SizeOfHalRequest = 0;

      if(NULL != rfcaCommand->HAL.HalResponse)
      {
        RFCA_Free(rfcaCommand->HAL.HalResponse);
      }
      rfcaCommand->HAL.HalResponse = NULL;
      rfcaCommand->HAL.SizeOfHalResponse = 0;

      FtmPacketQueueItem * ftmId = rfcaCommand->FTM.FtmItems;
      while (NULL != ftmId)
      {
        FtmPacketQueueItem * toBeDeleted = ftmId;
        ftmId = ftmId->NextItem;
        toBeDeleted->Packet.FreeResponse();
        toBeDeleted->Packet.FreeRequest();
        RFCA_Free(toBeDeleted);
        toBeDeleted = NULL;
      }

      if(NULL != previousQueueItem)
      {
        // Deleted item was in the middle or was the last
        previousQueueItem->NextItem = rfcaCommand->NextItem;
        if(NULL == rfcaCommand->NextItem)
        {
          // Was the last item
          TailOfRfcaCommandQueue = previousQueueItem;
        }
      }
      else
      {
        // Deleted item was in the first
        RfcaCommandQueue = rfcaCommand->NextItem;
        if(NULL == rfcaCommand->NextItem)
        {
          // Was the first and the last item
          TailOfRfcaCommandQueue = previousQueueItem;
        }
      }      

      RFCA_Free(rfcaCommand);
      rfcaCommand = NULL;
      break;
    }
    previousQueueItem = rfcaCommand;
    rfcaCommand = rfcaCommand->NextItem;
  }   

  ReleaseRfcaCommandQueueLock(__FUNCTION__);
}

/*----------------------------------------------------------------------------*/
void SendPopDelayedFromRfcaResponseQueue(unsigned int rfcaID)
{
  SendDelayedFromRfcaResponseQueue(rfcaID);
  PopFromRfcaResponseQueue(rfcaID);
}

/*----------------------------------------------------------------------------*/
void SendPopAckFromRfcaResponseQueue(unsigned int rfcaID, boolean isLastResponse)
{
  SendAckFromRfcaResponseQueue(rfcaID, isLastResponse);
  PopFromRfcaResponseQueue(rfcaID);  
}

/*----------------------------------------------------------------------------*/
void ClearRfcaCommandQueue()
{
  GetRfcaCommandQueueLock(__FUNCTION__);
  CheckQueuePointers();

  PendingRfcaResponse *rfcaCommand = RfcaCommandQueue;
  if(NULL == RfcaCommandQueue)
  {
    ReleaseRfcaCommandQueueLock(__FUNCTION__);
    return;  
  }
  while(NULL != rfcaCommand )
  {
    PendingRfcaResponse * currentrfcaCommand = rfcaCommand;
    // Point to the next item as the current will be removed
    rfcaCommand = rfcaCommand->NextItem;
    FtmPacketQueueItem *nextFtmItem = currentrfcaCommand->FTM.FtmItems;
    while(NULL != nextFtmItem)
    {
      FtmPacketQueueItem *currentFtmItem = nextFtmItem;
      nextFtmItem = nextFtmItem->NextItem;
      currentFtmItem->Packet.FreeResponse();
      currentFtmItem->Packet.FreeRequest();
      RFCA_Free(currentFtmItem );      
      currentFtmItem = NULL;
    }
    if(NULL != currentrfcaCommand->HAL.HalRequest)
    {
      RFCA_Free(currentrfcaCommand->HAL.HalRequest);
    }
    currentrfcaCommand->HAL.HalRequest = NULL;
    currentrfcaCommand->HAL.SizeOfHalRequest = 0;

    if(NULL != currentrfcaCommand->HAL.HalResponse)
    {
      RFCA_Free(currentrfcaCommand->HAL.HalResponse);
    }
    currentrfcaCommand->HAL.HalResponse = NULL;
    currentrfcaCommand->HAL.SizeOfHalResponse = 0;
    RFCA_Free(currentrfcaCommand);
    currentrfcaCommand = NULL;
  }
  RfcaCommandQueue = NULL;
  TailOfRfcaCommandQueue = NULL;
  CheckQueuePointers();
  ReleaseRfcaCommandQueueLock(__FUNCTION__);
}

/*----------------------------------------------------------------------------*/
DIAG_RFCA_RESP* create_diag_response(uint16 hal_operation_id, HAL_MESSAGE_ID rfca_command_code, void* hal_resp, uint16 size_of_hal_resp )
{
  if(NULL == hal_resp || 0 == size_of_hal_resp)
  {
      MSG_ERROR("HAL reponse pointer or size is 0 for HAL_OP_ID: %d\n", hal_operation_id, 0, 0);
      return NULL;
  }
  DIAG_RFCA_RESP *diag_resp;
  uint16 size_of_diag_rfca_resp = sizeof(DIAG_RFCA_RESP) + size_of_hal_resp; 
  diag_resp = (DIAG_RFCA_RESP*)RFCA_Malloc(size_of_diag_rfca_resp);
  if(NULL == diag_resp)
  {
    MSG_ERROR("Failed to allocate memory for RFCA diag response.", 0, 0 ,0);
    return NULL;
  }  

  diag_resp->command_code = DIAG_SUBSYS_CMD_VER_2_F;
  diag_resp->subsys_id = DIAG_SUBSYS_FTM; 
  diag_resp->rf_mode_id = FTM_RFCA_C;
  diag_resp->status = 0x0000;                         
  diag_resp->delayed_resp_id = 0;                 
  diag_resp->resp_count = 0;
  diag_resp->rfca_msg_id = rfca_command_code;
  diag_resp->ftm_data_len = size_of_hal_resp;
  diag_resp->resp_size = size_of_diag_rfca_resp;
  memscpy(diag_resp->data, size_of_hal_resp, hal_resp, size_of_hal_resp);

  return diag_resp;
}

/*----------------------------------------------------------------------------*/
void SendAckFromRfcaResponseQueue(unsigned int rfcaID, boolean isLastResponse)
{
  PendingRfcaResponse * resp = FindFromRfcaResponseQueue(rfcaID);
  if(NULL == resp)
  {
    MSG_ERROR("RFCA request not found from response queue.", 0, 0, 0);
    return;
  }

  DIAG_RFCA_RESP* diag_resp = create_diag_response(rfcaID, resp->HAL.HalMessageID, resp->HAL.HalResponse, resp->HAL.SizeOfHalResponse);
  if(NULL == diag_resp)
  {
    return;
  }

  //TODO: resp->DiagResponse->resp_size should be correct, if not then we need to store the DIAG resp size separately

  int responseSize = sizeof(DIAG_RFCA_RESP) + resp->HAL.SizeOfHalResponse;
  void *diag_mem_prt = NULL;
  if(NULL == RFCA_immediate_diag_response_buffer)
  {
    RFCA_Free(diag_resp);
    RFCA_immediate_diag_response_buffer_size = 0;	
    MSG_ERROR("RFCA diag response buffer has not been allocated", 0, 0, 0);
    return;
  }
  if(responseSize > RFCA_immediate_diag_response_buffer_size)
  {
    diagpkt_free(RFCA_immediate_diag_response_buffer);
    RFCA_immediate_diag_response_buffer = NULL;
    RFCA_immediate_diag_response_buffer_size = 0;
    RFCA_Free(diag_resp);
    MSG_ERROR("RFCA diag response buffer is too small, cannot send response.", 0, 0, 0);
    return;
  }
  else if(RFCA_immediate_diag_response_buffer_size > responseSize)
  {
    // Need to resize, this should be the defaut case for pretty much all cases
    diagpkt_shorten(RFCA_immediate_diag_response_buffer, responseSize); 
  }
  diag_mem_prt = RFCA_immediate_diag_response_buffer;

  diagpkt_subsys_set_status(diag_resp, 0);  
  diagpkt_subsys_set_rsp_cnt(diag_resp, 0);
  if(TRUE == isLastResponse)
  {
    // This is an ack and data message there won't be more messages to follow
    diagpkt_subsys_reset_delayed_rsp_id(diag_resp);
  }
  else
  {
    // This is just an ack message more messages to follow
    resp->HAL.DelayedDiagResponseId = diagpkt_subsys_get_delayed_rsp_id(diag_mem_prt);
    diag_resp->delayed_resp_id = resp->HAL.DelayedDiagResponseId;      
  }
  memscpy(diag_mem_prt, responseSize, diag_resp, responseSize);
  RFCA_Free(diag_resp);
  diag_resp = NULL;

  /* send packet */
  MSG_3(MSG_SSID_FTM, MSG_LEGACY_HIGH, "Calling diagpkt_commit from SendAckFromRfcaResponseQueue, HAL ID: %d, status: %d, isLastResponse: %d\n", rfcaID, ((HAL_RF_INIT_RESP*)resp->HAL.HalResponse)->status, isLastResponse);
  diagpkt_commit(diag_mem_prt);

  // Set pointer to NULL and size to 0 as diag owns the buffer
  RFCA_immediate_diag_response_buffer = NULL;
  RFCA_immediate_diag_response_buffer_size = 0;

}

/*----------------------------------------------------------------------------*/
/*!
 @brief Sends delayed DIAG RFCA response back to the caller

 @details This function is called to send delayed response to DIAG request.
          In normal operation this happens after all FTM reponses have been
          recevied from FTM sequencer or if nothing was queued then this
          function can be called right away. This fucntion is also called
          if there is an error in RFCA handler function for example invalid
          mode was given.

 @note This function does not remove the item from the queue, PopFromRfcaResponseQueue
       needs to be called to send the delayed DIAG response and permanently remove the item
       from the queue.

 @param rfcaID Identifier for the message.
 */
static void SendDelayedFromRfcaResponseQueue(unsigned int rfcaID)
{
  PendingRfcaResponse * resp = FindFromRfcaResponseQueue(rfcaID);
  if(NULL == resp)
  {
    HalLogWrite("RFCA request not found from response queue.\n");
    return;
  }

  DIAG_RFCA_RESP* diag_resp = create_diag_response(rfcaID, resp->HAL.HalMessageID, resp->HAL.HalResponse, resp->HAL.SizeOfHalResponse);
  if(NULL == diag_resp)
  {
    MSG_ERROR("Failed to create DIAG response.", 0, 0, 0);
    return;
  }
  //TODO: resp->DiagResponse->resp_size should be correct, if not then we need to store the DIAG resp size separately

  // This is the delayed response, our diag alloc callback uses correct value but because we memcpy over it we update this
  diag_resp->delayed_resp_id = resp->HAL.DelayedDiagResponseId;  // Must match the initial response

  diagpkt_subsys_set_rsp_cnt(diag_resp, 1);
  diagpkt_subsys_set_status(diag_resp, 0);  

  int responseSize = sizeof(DIAG_RFCA_RESP) + resp->HAL.SizeOfHalResponse;
  // Allocate memory from diag heap
  void *diag_mem_prt = NULL;

  diag_mem_prt = (void*) diagpkt_subsys_alloc_v2_delay((diagpkt_subsys_id_type)DIAG_SUBSYS_FTM, FTM_RFCA_C, resp->HAL.DelayedDiagResponseId, responseSize);

  if(NULL == diag_mem_prt)
  {
    // Memory allocation failed
    MSG_ERROR("Failed to allocate memory for RFCA diag response from DIAG heap.", 0, 0, 0);
    RFCA_Free(diag_resp);
    diag_resp = NULL;
    return; 
  }
  memscpy(diag_mem_prt, responseSize, diag_resp, responseSize);
  RFCA_Free(diag_resp);
  diag_resp = NULL;

  /* send async packet */     
  MSG_2(MSG_SSID_FTM, MSG_LEGACY_HIGH, "Calling diagpkt_delay_commit from SendPopDelayedFromRfcaResponseQueue, HAL ID: %d, status: %d\n", rfcaID, ((HAL_RF_INIT_RESP*)resp->HAL.HalResponse)->status);
  diagpkt_delay_commit(diag_mem_prt);
}

