/*!
  @file
  rfca_dispatcher.h
 
  @brief
  Dispatches DIAG messages coming from HAL and FTM to the RFCA layer. Incoming DIAG requests 
  from AP or PC-HAL are sent to appropriate '_send' function to be processed.

  During the FTM sequencer execution FTM log messages are coming back and are captured
  by RfcaRespDispatcher function. Once all responses to all FTM messages sent from '_send'
  function have been received a callback,if one is defined, is called. Once callback returns
  a DIAG response message 

*/


/*==============================================================================

  Copyright (c) 2011-2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/inc/rfca_dispatcher.h#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
06/10/13   aki     Mainline RFCA
03/01/12   aki     Changed <> to ""
10/28/11   aki     Initial revision

==============================================================================*/


#ifndef _RFCA_DISPATCHER_H_
#define _RFCA_DISPATCHER_H_

#include "rfa_variation.h"
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif


#include "rfca_diag_msg_def.h"

/*----------------------------------------------------------------------------*/
/*!
 @brief Dispatcher for messages sent to RFCA via DIAG from HAL or FTM

 @param [in] req DIAG RFCA message
 */
void RfcaReqDispatcher( DIAG_RFCA_REQ *req );

/*----------------------------------------------------------------------------*/
/*!
 @brief Dispatcher for async messages from FTM sequencer

 @details When a valid response has been recevied the response data is copied to 
          FTM package in the RFCA queue item. If all expected FTM responses have
          been recived then the callback function is called. After callback
          returns the DIAG response is sent back to the caller and item is removed
          from the RFCA queue.

 @see RfcaCommandQueue, FtmResponseCallback

 @param [in] iMessageSize   Size of the message.
 @param [in] iMessageBuffer Message data 

 */
void RfcaRespDispatcher(
	unsigned short iMessageSize,
	unsigned char* iMessageBuffer
);


#ifdef __cplusplus
  }
#endif

#endif //_RFCA_DISPATCHER_H_

