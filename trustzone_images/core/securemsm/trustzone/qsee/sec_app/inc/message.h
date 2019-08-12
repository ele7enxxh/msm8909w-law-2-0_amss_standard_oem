#ifndef __QSEE_MESSAGE_H
#define __QSEE_MESSAGE_H

/**
@file message.h
@brief Type definitions and constants for qsee message passing.  
*/

/*===========================================================================
   Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      ------------------------------------
03/02/13   nbeier      Initial version.

===========================================================================*/
#include "lkthread.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define QSEE_MESSAGE_HEADER_LEN sizeof(qsee_message_header_t)
#define QSEE_MAX_ENTRIES_IN_MESSAGE_COUNTER_TABLE 20
#define QSEE_MESSAGE_APP_NAME_MAX_LEN 32
#define QSEE_MESSAGE_IV_LEN 16
#define QSEE_MESSAGE_MAC_LEN 32
#define QSEE_MESSAGE_MAX_COUNTER_VAL 0xFFFFFFFF //Max uint32

/*----------------------------------------------------------------------------
 * Error codes
 * -------------------------------------------------------------------------*/
#define QSEE_MESSAGE_SUCESS                      0
#define QSEE_MESSAGE_ERROR                       0xFF000FFF
#define QSEE_MESSAGE_ERROR_ZERO_BUFFER_LEN       0xFF000FFE
#define QSEE_MESSAGE_ERROR_INSUFFCIENT_OUT_LEN   0xFF000FFD
#define QSEE_MESSAGE_ERROR_BAD_DEST_APPNAME      0xFF000FFC
#define QSEE_MESSAGE_ERROR_NO_DEST_APP_THREAD    0xFF000FFB
#define QSEE_MESSAGE_ERROR_NO_DEST_APP_KEYS      0xFF000FFA
#define QSEE_MESSAGE_ERROR_NO_SPACE_IN_TABLE     0xFF000FF9
#define QSEE_MESSAGE_ERROR_CIPHER_INIT           0xFF000FF8
#define QSEE_MESSAGE_ERROR_CIPHER_SET_PARAM      0xFF000FF7
#define QSEE_MESSAGE_ERROR_CIPHER_ENCRYPT        0xFF000FF6
#define QSEE_MESSAGE_ERROR_HMAC_FAIL             0xFF000FF5
#define QSEE_MESSAGE_ERROR_PRNG_FAIL             0xFF000FF4
#define QSEE_MESSAGE_ERROR_BAD_SOURCE_NAME       0xFF000FF3
#define QSEE_MESSAGE_ERROR_MESSAGE_NOT_FOR_YOU   0xFF000FF2
#define QSEE_MESSAGE_ERROR_NO_COUNTER_VALS       0xFF000FF1
#define QSEE_MESSAGE_ERROR_MAC_MISMATCH          0xFF000FF0
#define QSEE_MESSAGE_ERROR_BAD_COUNTER_VALS      0xFF000FEF
#define QSEE_MESSAGE_ERROR_MAXED_OUT_COUNTER_VAL 0xFF000FEE
#define QSEE_MESSAGE_ERROR_MALLOC_INTERNAL_BUF   0xFF000FED
#define QSEE_MESSAGE_ERROR_INTEGER_OVERFLOW      0xFF000FEC
#define QSEE_MESSAGE_ERROR_INTEGER_UNDERFLOW     0xFF000FEB
#define QSEE_MESSAGE_ERROR_MALLOC_HMAC_BUF       0xFF000FEA
#define QSEE_MESSAGE_ERROR_STRLCPY_FAIL          0xFF000FE9


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct qsee_message_header {
  char source_app_name[QSEE_MESSAGE_APP_NAME_MAX_LEN];
  char dest_app_name[QSEE_MESSAGE_APP_NAME_MAX_LEN];
  uint8 iv[QSEE_MESSAGE_IV_LEN];
  uint32 counter_s; 
}qsee_message_header_t;


/*---------------------------------------------------------------------------
 *  Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief 
 *    API for a secure application to prepare a message that can be sent to 
 *    another secure application. This API writes the encryped message into 
 *    the supplied output buffer, prepended with a header and appended with   
 *    a MAC. This output buffer can be given to the receiving app and then 
 *    passed into qsee_decapsulate_inter_app_message to be authenticated and 
 *    decrypted.  The actual data exchange (passing the encrypted bufer) 
 *    between the secure applications can be done between their non secure 
 *    clients running in the HLOS. 
 *
 * @param[in] dest_app_name 
 * @param[in] in_buf 
 * @param[in] in_len 
 * @param[out] out_buf 
 * @param[in, out] out_len 
 *
 * @requires 
 *   in_len should indicate the exact size of in_buf.
 *   out_len should indicate the exact size of out_buf, and this should be
 *     greater than in_len by 144 bytes to allow room for the header and MAC.
 *     out_len is modified to reflect the exact length of the data 
 *     written into out_buf.   
 *
 * @return 0 indicates sucess, all other values indicate failure and
 *     correspond to a specific error code. 
 */
int qsee_encapsulate_inter_app_message(char* dest_app_name, uint8* in_buf, 
  uint32 in_len, uint8* out_buf, uint32 out_len, uint32* out_len_ptr);


/**
 * @brief 
 *    API for a secure application to decapsulate (ie decrypt) a message 
 *  from another secure application.  This API authenticates the message, 
 *  decrypts the input buffer, and writes the plaintext message into the 
 *  supplied output buffer.  The input buffer must have been prepared by 
 *  qsee_encapsulate_inter_app_message(), containing a header and MAC. 
 *
 * @param[out] source_app_name 
 * @param[in] in_buf 
 * @param[in] in_len 
 * @param[out] out_buf 
 * @param[in, out] out_len 
 *
 * @requires 
 *   source_app_name is modified to return the sending application's identity
 *      to the caller.  
 *   in_len should indicate the exact size of in_buf. 
 *   out_len should indicate the exact size of out_buf, and this should be 
 *      greater than or equal to in_len. 
 *
 * @return 0 indicates sucess, all other values indicate failure and 
 *     correspond to a specific error code. 
 */
int qsee_decapsulate_inter_app_message(char* source_app_name, uint8* in_buf, 
  uint32 in_len, uint8* out_buf, uint32 out_len, uint32* out_len_ptr);


#endif
