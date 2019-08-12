#ifndef LOC_UTILS_H
#define LOC_UTILS_H
/*============================================================================
  @file loc_utils.h
 
  Utility functions for loc_slim and loc_sdp modules.
 
               Copyright (c) 2014 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary
               Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc. 
               All Rights Reserved.
               Confidential and Proprietary - Qualcomm Technologies, Inc.
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_utils.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  01/12/16     yh  LocMW refactoring
============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "aries_os_api.h"
#include "loc_api_internal.h"
#include "comdef.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define LOC_SEND_IPC_TO_LOCMW(M,C,T,PTR) \
  loc_send_ipc_to_locmw((M),(C),(T),(PTR),sizeof(*(PTR)))

#define LOC_SLIM_UNPACK_IPC_TO_LOCMW(M,C,T,PTR) \
  loc_unpack_ipc_to_locmw((M),(C),(T),(PTR),sizeof(*(PTR)))

#define LOC_UNPACK_IPC_TO_LOCMW(M,C,T,PTR) \
  loc_unpack_ipc_to_locmw((M),(C),(T),(PTR),sizeof(*(PTR)))

// TODO: need to check if LocMW task has been initialized!
#define IND_PROXY(MSG)\
{\
    if(NULL != p_ind)\
    {\
        if(0 == LOC_SEND_IPC_TO_LOCMW(MSG, client_handle, transaction_id, p_ind))\
        {\
            error_code = 0;\
        }\
        else\
        {\
            error_code = 2;\
        }\
    }\
    else\
    {\
        error_code = 3;\
    }\
}

/*============================================================================
 *                    U T I L I T Y    Preprocessor Definitions and Constants FOR SECURE AVAILABLE POSITION
 * =========================================================================*/


#define SECAPI_ENCRYPTION_MSG_HEADER 100
#define DATA_TYPE_LENGTH_ZERO 0
#define DATA_TYPE_LENGTH_ONE 1
#define _SEC_DEBUG_ 0
#if __BIG_ENDIAN
// No translation needed for big endian system
#define Swap2Bytes(val) (val)
#define Swap4Bytes(val) (val)
#define Swap8Bytes(val) (val
#else

// Swap 2 byte, 16 bit values:
#define Swap2Bytes(val) \
((((val) >> 8) & 0x00FF) | (((val) << 8) & 0xFF00))
// Swap 4 byte, 32 bit values:
#define Swap4Bytes(val) \
((((val) >> 24) & 0x000000FF) | (((val) >> 8) & 0x0000FF00) | \
(((val) << 8) & 0x00FF0000) | (((val) << 24) & 0xFF000000))
// Swap 8 byte, 64 bit values:
#define Swap8Bytes(val) \
	((((val) >> 56) & 0x00000000000000FF) | (((val) >> 40) & 0x000000000000FF00) | \
	(((val) >> 24) & 0x0000000000FF0000) | (((val) >> 8) & 0x00000000FF000000) | \
	(((val) << 8) & 0x000000FF00000000) | (((val) << 24) & 0x0000FF0000000000) | \
	(((val) << 40) & 0x00FF000000000000) | (((val) << 56) & 0xFF00000000000000) )

#endif



/* -----------------------------------------------------------------------*//**

@brief
  Utility macros to Unswap the data bytes to match the system Endianess
  
*//* ------------------------------------------------------------------------*/


// Extract 1 byte, 8 bit values:
#define Extract1Bytes(val) \
		(val[0]) 
// Extract 2 byte, 16 bit values:
#define Extract2Bytes(val) \
		(val[1] | ( val[0] << 8 ) )
// Extract 4 byte, 32 bit values:
#define Extract4Bytes(val) \
		(val[3] | ( val[2] << 8 ) | ( val[1] << 16 ) | ( val[0] << 24 ))
// Extract 8 byte, 64 bit values:
#define Extract8Bytes(val) \
	((uint64)val[7] | ( (uint64)val[6] << 8 ) | ( (uint64)val[5] << 16 ) | ( (uint64)val[4] << 24 )| ( (uint64)val[3] << 32 ) |  ((uint64)val[2] << 40 ) | ( (uint64)val[1] << 48 )| ( (uint64)val[0] << 56 ))

/* =========================================================================*/


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct loc_user_data_struct_type
{
  // loc MW client handle
  int32 clientHandle;

  // transaction id
  uint32 transactionId;

  //pointer to self
  struct loc_user_data_struct_type *pMe;

}locUserDataStructT;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
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
);

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
);

/**
 * get a somewhat unique transaction id for debugging and message matching purposes
 *
 * @return qmiloc transaction id
 */
uint32 loc_qmiloc_get_transaction_id
(
  void
);

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
);

#endif /* #ifndef LOC_UTILS_H */

