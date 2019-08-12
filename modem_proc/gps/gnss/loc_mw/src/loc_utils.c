/*============================================================================
  FILE:         loc_utils.c

  OVERVIEW:     Utility functions for loc_slim and loc_sdp modules.

  DEPENDENCIES: None.
 
                Copyright (c) 2014 QUALCOMM Atheros, Inc.
                All Rights Reserved.
                Qualcomm Atheros Confidential and Proprietary
                Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc. 
                All Rights Reserved.
                Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_utils.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  01/11/16    yh   LocMW refactoring
  2014-04-17  lv   Initial revision.

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "loc_api_internal.h"
#include "loc_utils.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/**
 * Create an ipc message and send to LocMW task
 *
 * @param msg_id    ipc message id
 * @param loc_client_handle
 *                  LocAPI client handle
 * @param transaction_id
 *                  will be duplicated into the resulting indication
 * @param p_payload QMI request, usually data structure created by the IDL compiler
 * @param payload_size
 *                  size of the QMI request
 *
 * @return 0 if ipc has been sent successfully
 */
uint32 loc_send_ipc_to_locmw
(
  const uint32 msg_id,
  const int32 loc_client_handle,
  const uint32 transaction_id,
  const void *const p_payload,
  const uint32 payload_size
)
{
  uint32 error_code = 1;
  const uint32 msg_size = sizeof(loc_client_handle) + sizeof(transaction_id) + payload_size;

  if (NULL != p_payload)
  {
    os_IpcMsgType *const ipc_msg_ptr =
      os_IpcCreate(msg_size, IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_LOC_MIDDLEWARE);
    if (NULL != ipc_msg_ptr)
    {
      // casting to void * first to avoid lint error of memory size
      byte *p_insertion_point = (byte *)(void *)ipc_msg_ptr->p_Data;
      int32 *p_client_handle = NULL;
      uint32 *p_transaction_id = NULL;
      void *p_payload_store = NULL;

      // calculate offset
      p_client_handle = (int32 *)p_insertion_point;
      p_insertion_point += sizeof(int32);
      p_transaction_id = (uint32 *)p_insertion_point;
      p_insertion_point += sizeof(uint32);
      p_payload_store = (void *)p_insertion_point;

      // first is client handle
      *p_client_handle = (uint32)loc_client_handle;
      // copy the transaction id
      *p_transaction_id = transaction_id;
      // make a shallow copy of the request
      (void)memscpy(p_payload_store, payload_size, p_payload, payload_size);

      ipc_msg_ptr->q_MsgId = msg_id;
      if (TRUE == os_IpcSend(ipc_msg_ptr, THREAD_ID_LOC_MIDDLEWARE))
      {
        // ipc sent
        error_code = 0;
      }
      else
      {
        // ipc not sent
        (void)os_IpcDelete(ipc_msg_ptr);
        error_code = 2;
      }
    }
    else
    {
      // ipc memory allocation failure, ipc not sent
      error_code = 3;
    }
  }
  else
  {
    // null pointer for payload. this shall never happen
    error_code = 4;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[LOC] loc_send_ipc_to_locmw: "
      "error code: %u", error_code, 0, 0);
  }
  return error_code;
}


/**
 * Unpack an ipc message sent to LocMW.
 *
 * @param p_ipc_msg_in
 *                  [in] ipc message
 * @param p_loc_client_handle
 *                  [out] LocAPI client handle
 * @param p_transaction_id
 *                  [out] transaction id in the request
 * @param p_payload [out] QMI request. structure created by IDL compiler.
 *                  size must be enough to hold payload_size
 * @param payload_size
 *                  [in] size of the QMI request
 *
 * @return 0 if ipc has been unpacked successfully
 */
uint32 loc_unpack_ipc_to_locmw
(
  const os_IpcMsgType *const p_ipc_msg_in,
  int32 *const p_loc_client_handle,
  uint32 *const p_transaction_id,
  void *const p_payload,
  const uint32 payload_size
)
{
  uint32 error_code = 1;
  const uint32 msg_size = sizeof(int32) + sizeof(uint32) + payload_size;

  if (NULL != p_ipc_msg_in &&
      NULL != p_loc_client_handle &&
      NULL != p_transaction_id &&
      NULL != p_payload &&
      msg_size <= p_ipc_msg_in->q_Size)
  {
    const byte *p_extraction_point = (byte *)(void *)p_ipc_msg_in->p_Data;
    const int32 *p_client_handle = NULL;
    const uint32 *p_transaction_id_store = NULL;
    const void *p_payload_store = NULL;

    // calculate offset
    p_client_handle = (const int32 *)p_extraction_point;
    p_extraction_point += sizeof(int32);
    p_transaction_id_store = (const uint32 *)p_extraction_point;
    p_extraction_point += sizeof(uint32);
    p_payload_store = (const void *)p_extraction_point;

    // retrieve client id
    *p_loc_client_handle = *p_client_handle;
    // retrieve transaction id
    *p_transaction_id = *p_transaction_id_store;
    // retrieve payload
    (void)memscpy(p_payload, payload_size, p_payload_store, payload_size);

    error_code = 0;
  }
  else
  {
    // null pointer for something. this shall never happen
    error_code = 2;
  }

  if (0 != error_code)
  {
    LOC_MSG_ERROR("[LOC] loc_unpack_ipc_to_locmw: "
      "error code: %u", error_code, 0, 0);
  }
  return error_code;
}

/**
 * get a somewhat unique transaction id for debugging and message matching purposes
 *
 * @return qmiloc transaction id
 */
uint32 loc_qmiloc_get_transaction_id
(
  void
)
{
  // it might be better if transaction can be globally unique and time varying so we can survive
  // state machine resets and the like
  // however, it's a bit difficult to acquire true time varying source from higher level code
  static uint32 transaction_id = 0;
  ++transaction_id;

  return transaction_id;
}

/*---------------------------------------------------------------------------
@brief
  Function to generate an IPC to locMW task

@param[in] e_MsgId       : LocMW IPC message ID
@param[in] q_PayloadSize : Payload size
@param[in] p_PayloadData : Pointer to the payload data

@retval    TRUE   Called if IPC was sent successfully
@retval    FALSE  Called otherwise
---------------------------------------------------------------------------*/
boolean locMW_SendIpcMsg
(
  loc_middleware_msg_id_e_type e_MsgId,
  uint32                       q_PayloadSize,
  const void * const           p_PayloadData
)
{
  os_IpcMsgType* const p_IpcMsg = os_IpcCreate(q_PayloadSize, IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_LOC_MIDDLEWARE);

  LOC_MSG_LOW("locMW_SendIpcMsg: Sending IPC MsgId=%d to LocMW", e_MsgId, 0, 0);

  if (NULL != p_IpcMsg)
  {
    /* Copy the payload */
    (void)memscpy(p_IpcMsg->p_Data, q_PayloadSize, p_PayloadData, q_PayloadSize);

    p_IpcMsg->q_MsgId = e_MsgId;
    if(FALSE == os_IpcSend(p_IpcMsg, THREAD_ID_LOC_MIDDLEWARE))
    {
      LOC_MSG_ERROR("locMW_SendIpcMsg: Sending IPC MsgId=%d to LocMW failed", e_MsgId, 0, 0);
      os_IpcDelete(p_IpcMsg);
      return FALSE;
    }
  }
  return TRUE;
}