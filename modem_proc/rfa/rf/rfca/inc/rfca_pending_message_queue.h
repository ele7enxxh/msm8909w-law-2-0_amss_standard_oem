/*!
  @file
  rfca_pending_message_queue.h
 
  @brief
  Types and functions to manage pending RFCA respose queue.

*/


/*==============================================================================

  Copyright (c) 2011-2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/inc/rfca_pending_message_queue.h#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/19/13   aki     Removed IterateRfcaCommandQueue
06/10/13   aki     Mainline RFCA
09/04/12   aki     Added StatusOverride to HAL response
03/01/12   aki     <> to "", doc updates, PopFromRfcaResponseQueue public again
02/08/12   aki     Removed PopFromFtmIdQueue
01/25/12   aki     Diag V2 delayed response implementation
10/28/11   aki     Initial revision

==============================================================================*/

#ifndef _RFCA_PENDING_MESSAGE_QUEUE_H_
#define _RFCA_PENDING_MESSAGE_QUEUE_H_


#include "rfa_variation.h"
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "rfca_hal_msg_def.h"
#include "rfca_diag_msg_def.h"
#include "rfca_packet_helper.h"

#ifdef _WIN32
  #pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

/*----------------------------------------------------------------------------*/

/*!
 @brief FTM packet queue item structure. 

 */
struct FtmPacketQueueItemStruct{  
  unsigned int SequenceID;                    //!< Unique FTM sequencer message id
  bool ResponseReceived;                      //!< Indicates that we have received a response
  struct FtmPacketQueueItemStruct *NextItem;
  RfcaPacketHelper Packet;                    //!< FTM message request and response
};

/*----------------------------------------------------------------------------*/
/*!
 @brief Pending ftm request/response

 */
typedef struct FtmPacketQueueItemStruct FtmPacketQueueItem;

/*----------------------------------------------------------------------------*/
/*!
 @brief Defines callback function which is called when all ftm responses have been received

 @details The reponse of callback function is to fill the DIAG RFCA response with correct data
          based on the information in the received FTM reponses.
 */
typedef void (*FtmResponseCallback)(unsigned int halId);

/*----------------------------------------------------------------------------*/
/*!
 @brief Holds formation about peding RFCA requests

 @details When RFCA requests are recevied tehy are translated into
          multiple FTM messages which are sent to the FTM task.
          This structure holds the pending DIAG RFCA response and
          the list of all FTM messages that were sent. When response 
          for all FTM messages has been received the callback function is
          called.

 */
PACK(struct) PendingRfcaResponseStruct{
  unsigned int HalID;                         //!< Unique ID for this message
  
  PACK(struct) {
    HAL_MESSAGE_ID HalMessageID;                //!< ID of the response to be sent back, see HAL_MESSAGE_ID
    void* HalRequest;                           //!< HAL response to be sent back 
    unsigned int SizeOfHalRequest;              //!< Size of data pointed by HalResponse
    void* HalResponse;                          //!< HAL response to be sent back 
    unsigned int SizeOfHalResponse;             //!< Size of data pointed by HalResponse
    uint16 DelayedDiagResponseId;               //!< Contains ID of delayed response
    uint16 StatusOverride;                      //!< Override status with this value before sending back
  }HAL;

  PACK(struct) {
    FtmPacketQueueItem * FtmItems;              //!< Linked list of FTM messages that were sent
    FtmPacketQueueItem * TailOfFtmItems;        //!< Last item in the FTM message list 
    unsigned int ResponseCountdown;             //!< Number of FTM messages sent and expect back
    FtmResponseCallback CallbackFunction;       //!< Function to call when all FTM messages have been received
  }FTM;

  struct PendingRfcaResponseStruct *NextItem;  //!< Next item in the list
};

/*----------------------------------------------------------------------------*/
/*!
 @brief Type of pending RFCA response
 */
typedef struct PendingRfcaResponseStruct PendingRfcaResponse;

/*----------------------------------------------------------------------------*/
/*!
 @brief Clears the RFCA response queue.

 */
void ClearRfcaCommandQueue();

/*----------------------------------------------------------------------------*/
/*!
 @brief Adds new item to the RFCA response queue. 

 @param rfcaID             Unique identifier for RFCA response 
 @param halRequest        HAL request messsage
 @param sizeOfHalRequest  Size or HAL request messages
 @param sizeOfHalResponse Size or HAL response messages
 @param rfcaCommandCode    Message ID from HAL_MESSAGE_ID 
 @param callback          Pointer to the callback function to be called
                          when all FTM responses have been received

 @return Pointer to the new queue item
 */
PendingRfcaResponse * AddToRfcaResponseQueue(unsigned int rfcaID, void* halRequest, uint16 sizeOfHalRequest, uint16 sizeOfHalResponse, HAL_MESSAGE_ID rfcaCommandCode, FtmResponseCallback callback);

/*----------------------------------------------------------------------------*/
/*!
 @brief Searches for RFCA response basend on the message identifier

 @param rfcaID Identifier for the RFCA response.

 @return Pointer to the pending RFCA queue item, NULL if id not in queue
 */
PendingRfcaResponse * FindFromRfcaResponseQueue(unsigned int rfcaID);

/*----------------------------------------------------------------------------*/
/*!
 @brief Searches for RFCA response basend on the FTM sequence identifier

 @param sequenceID FTM sequence identifier.

 @return Pointer to the pending RFCA queue item, NULL if id not in queue
 */
PendingRfcaResponse * FindFromRfcaResponseQueueByFtmSequenceId( unsigned int sequenceID );

/*----------------------------------------------------------------------------*/
/*!
 @brief Sends DIAG RFCA ack response back to the caller

 @details This function is called to send immediate response to DIAG request.
          In normal operation this happens after all FTM reponses have been
          queued to FTM sequencer.

 @note This function does not remove the item from the queue, SendPopFromRfcaResponseQueue
       needs to be called to send the delayed DIAG response and permanently remove the item
       from the queue.

 @param rfcaID          Identifier for the message.
 @param isLastResponse  Indicates if this is also the last (the only) response
 */
void SendAckFromRfcaResponseQueue(unsigned int rfcaID, boolean isLastResponse);

/*----------------------------------------------------------------------------*/
/*!
 @brief Removes RFCA queue item from the queue.
 
 @param rfcaID Identifier for the RFCA response.
 */
void PopFromRfcaResponseQueue(unsigned int rfcaID);

/*----------------------------------------------------------------------------*/
/*!
 @brief Sends delayed diag reponse and pops given item from RFCA response queue
 
 @param rfcaID Identifier for the message.

 @see SendDelayedFromRfcaResponseQueue
 @see PopFromRfcaResponseQueue
 */
void SendPopDelayedFromRfcaResponseQueue(unsigned int rfcaID);

/*----------------------------------------------------------------------------*/
/*!
 @brief Sends an ack and pops given item from RFCA response queue
 
 @param rfcaID Identifier for the message.
 @param isLastResponse Identifies if this is the last reponse or if a delayed
                       response will be sent later.

 @see SendAckFromRfcaResponseQueue
 @see PopFromRfcaResponseQueue
 */
void SendPopAckFromRfcaResponseQueue(unsigned int rfcaID, boolean isLastResponse);

/*----------------------------------------------------------------------------*/
/*!
 @brief Adds new FTM message item to the RFCA reponse queue.

 @details Given FTM item is associated to the RFCA reposponse it with rfcaID.

 @param rfcaID   Identifier for the RFCA response item
 @param newItem New FTM item to be added 
*/
void AddToFtmIdQueue(unsigned int rfcaID, FtmPacketQueueItem *newItem);

/*----------------------------------------------------------------------------*/
/*!
 @brief Searches for an item with given id from FTM queue.

 @details Searched the FTM message queue of RFCA reponse with rfcaID.
          This function only looks into FTM queue of RFCA response with rfcaID
          to search all FTM queues use function FindFromRfcaResponseQueueByFtmSequenceId.

 @param rfcaID       Identifier for the RFCA response.
 @param sequenceID  Identifier for the FTM message.

 @return Pointer to the FTM item, NULL if item is not found
 */
FtmPacketQueueItem * FindFromFtmIdQueue( unsigned int rfcaID, unsigned int sequenceID );

/*----------------------------------------------------------------------------*/
/*!
 @brief Defines a callback function which is called for each RFCA item in the queue.

 @todo Consider removing this as it might not be needed
 */
typedef void (*pftHalQueueIteratorCallback)(FtmPacketQueueItem *ftmQueueItem);

#ifdef _WIN32
  #pragma pack(pop) // Restore
#endif

#ifdef __cplusplus
  }
#endif

#endif //_RFCA_PENDING_MESSAGE_QUEUE_H_