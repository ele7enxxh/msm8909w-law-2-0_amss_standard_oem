/*!
  @file
  rfca_to_ftm_adapter_embedded.cpp
 
  @brief
  Implementes RFCA to FTM adapation for embedded implementation.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/src/rfca_to_ftm_adapter_embedded.cpp#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/27/14   aki     Added msg include
08/09/13   aki     Cleanup
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
06/10/13   aki     Mainline RFCA 
02/29/12   aki     Renamed Qmls to Rfca and used HalLogWrite instead of MSG macro
02/08/12   aki     Clear rex signal before sync msg sending
10/28/11   aki     Initial revision

==============================================================================*/


#include "rfa_variation.h"
#include "comdef.h"
#include "stringl.h" /* for memscpy */

#include "rfca.h"
#include "rfca_to_ftm_adapter.h"
#include "rfca_dispatcher.h"

#include "rfca_defs.h"

#include "ftm.h"
#include "ftm_common_control.h"
#include "ftmdiag.h"
#include "msg.h"

#include "rex.h"

#include "ftm_sequencer_definitions.h"
#include "ftm_sequencer.h"
#include "modem_mem.h"

#include "rfca_task.h"
#include "rfca_helper.h"
#include "rfca_mem_helper.h"

static bool RfcaInitDone = false;

/*----------------------------------------------------------------------------*/
int RFCA_FtmSequencerInit()
{

  unsigned char clearSeqQueueReq[] = {0x4B, 0x0B, 0x25, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0C, 0x00};
  unsigned char clearSeqQueueResp[256] = {0};
  uint16 clearSeqQueueRespSize = 256;
  // TODO: Check response
  if(0 != RFCA_SendToFtmTaskSync(clearSeqQueueReq, sizeof(clearSeqQueueReq), clearSeqQueueResp, &clearSeqQueueRespSize))
  {
    MSG_ERROR("Failed to init FTM sequencer", 0, 0, 0);
    return RFCA_FUNCTION_FAIL;
  }
  return RFCA_FUNCTION_SUCCESS;
}

/*----------------------------------------------------------------------------*/
int RFCA_FtmSequencerExecute()
{

  unsigned char executeSeqQueueReq[] = {0x4B, 0x0B, 0x25, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0C, 0x00};
  unsigned char executeSeqQueueResp[256] = {0};
  uint16 executeSeqQueueRespSize = 256;
  if(0 != RFCA_SendToFtmTaskSync(executeSeqQueueReq, sizeof(executeSeqQueueReq), executeSeqQueueResp, &executeSeqQueueRespSize))
  {
    MSG_ERROR("Failed to execute FTM sequencer", 0 ,0 ,0);
    return RFCA_FUNCTION_FAIL;
  }
  // Check if seqeuncer returned an error
  if(0 != executeSeqQueueResp[10])
  {
    return RFCA_FUNCTION_FAIL;
  }
  return RFCA_FUNCTION_SUCCESS;
}


/*----------------------------------------------------------------------------*/
int RFCA_FtmLogsEnable()
{
  // *********************************
if(false == RfcaInitDone)
{

    // Enable FTM logs
    unsigned char enableLogReq[] = {0x4B, 0x0B, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x14, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};
    unsigned char enableLogResp[256] = {0};
    uint16 enableLogRespSize = 256;

    // TODO: Check response
    if(0 != RFCA_SendToFtmTaskSync(enableLogReq, sizeof(enableLogReq), enableLogResp, &enableLogRespSize))
    {
      MSG_ERROR("Failed to enable FTM logs", 0 ,0 ,0);
      return RFCA_FUNCTION_FAIL;
    }
    RfcaInitDone = true;
  }
  return RFCA_FUNCTION_SUCCESS;
  // *********************************
}


/*----------------------------------------------------------------------------*/
void RFCA_FtmSequencerDeinit()
{
  RfcaInitDone = false;
}

typedef PACK(struct)
{
  byte* req_data;
  word  req_data_size;
  byte* resp_data;
  word*  resp_data_size;
  FTM_Sequencer_ErrorCode error;
} rfca_idle_processing_cb_param_type;

extern "C" {
  void FTM_RFCA_TEST_FUNC(void);
}

/*----------------------------------------------------------------------------*/
volatile int sendLock = 0;

/*----------------------------------------------------------------------------*/
int RFCA_SendToFtmTaskSync(uint8 *req_data, uint16 req_data_size, uint8 *resp_data, uint16 *resp_data_size)
{
  // Tamper first byte to indicate FTM that message should be sent back to RFCA
  req_data[0] = 0xFF;

  // Clear signal to be sure we actually wait message to complete
  (void) rex_clr_sigs(rex_self(), RFCA_FTM_ASYNC_RESP_SIG );
  ftm_common_send_radio_command(req_data, req_data_size);
  rex_wait( RFCA_FTM_ASYNC_RESP_SIG);

  (void) rex_clr_sigs(rex_self(), RFCA_FTM_ASYNC_RESP_SIG );
  if( (NULL == ftm_async_response.data) || (0 == ftm_async_response.size) || (ftm_async_response.size > *resp_data_size) )
  {
    MSG_ERROR("Failed to receive response from FTM.", 0, 0, 0);      
    if(NULL != ftm_async_response.data)
    {
      RFCA_Free(ftm_async_response.data);
    }
    ftm_async_response.data = NULL;
    ftm_async_response.size = 0;
    return RFCA_FUNCTION_FAIL;
  }

  memscpy(resp_data, ftm_async_response.size, ftm_async_response.data, ftm_async_response.size);
  *resp_data_size = ftm_async_response.size;
  if(NULL != ftm_async_response.data)
  {
    RFCA_Free(ftm_async_response.data);
  }
  ftm_async_response.data = NULL;
  ftm_async_response.size = 0;
  return RFCA_FUNCTION_SUCCESS;
}
