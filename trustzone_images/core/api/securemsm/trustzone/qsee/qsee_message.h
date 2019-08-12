#ifndef QSEE_MESSAGE_API_H
#define QSEE_MESSAGE_API_H

/**
@file qsee_message.h
@brief Provide APIs for sending messages between secure applications. 
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
  uint32 in_len, uint8* out_buf, uint32* out_len);


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
  uint32 in_len, uint8* out_buf, uint32* out_len);



#endif

