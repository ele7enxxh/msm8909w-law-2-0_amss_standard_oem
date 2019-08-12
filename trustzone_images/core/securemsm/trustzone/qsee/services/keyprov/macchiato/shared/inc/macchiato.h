#ifndef MACCHIATO_H
#define MACCHIATO_H

/** @file macchiato.h
 * @brief
 * This file contains the definitions of the constants, data structures
 * and interfaces to the QSEE Macchiato key provisioning lib.
 */

/*===========================================================================
 Copyright (c) 2014 Qualcomm Technologies, Inc.
 All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/10/15   cz      Added a new parameter: utc seconds
01/21/15   cz      Added macchiato_is_secure_state() function
11/06/14   cz      Moved error codes define to qsee_macchiato.h
09/22/14   cz      Added an error code: MACCHIATO_ERROR_FEATURE_NOT_SUPPORTED
08/14/14   qxu     Added return errors
05/26/14   sn      Initial Version.

===========================================================================*/
#include <stdint.h>
#include "macchiato_service.h"
#include "macchiato_env.h"
#include "macchiato_keygen.h"
#include "macchiato_wrapper.h"
#include "qsee_macchiato.h" // error code reference



#define MACCHIATO_SIZEOF_OPAQUE_DATA_LEN           4 /*32 bits*/

#define MACCHIATO_ECC_SIG_LEN                     64 /*512 bits*/
#define MACCHIATO_PUB_KEY_LEN                     64 /*512 bits*/

#define MACCHIATO_SIGN_PUBKEY_LEN                 64 /*512 bits*/
#define MACCHIATO_ENC_PUBKEY_LEN                  64 /*512 bits*/

#define MACCHIATO_MSG_TA_HASH_LEN                 32 /*256 bits*/
#define MACCHIATO_MSG_NUM_SVCID_LEN                4 /*32 bits*/
#define MACCHIATO_MSG_SVCID_LEN                    8 /*64 bits*/

#define MACCHIATO_MSG_HW_VERSION_LEN               2 /*16 bits*/
#define MACCHIATO_MSG_SERIAL_NUM_LEN               4 /*32 bits*/
#define MACCHIATO_MSG_OEM_ID_LEN                   4 /*32 bits*/
#define MACCHIATO_MSG_OEM_PK_HASH_LEN             32 /*256 bits*/
#define MACCHIATO_MSG_OPAQUE_DATA_LEN              4 /*32 bits*/

#define MACCHIATO_MSG_SIGNATURE_LEN               64 /*512 bits*/

#define MACCHIATO_ERROR_BASE QSEE_MACC_KEYGEN_MAX_ERROR

#define TZBSP_SANDBOX_RELOCATE_OFFSET 0x04800000


#pragma pack (push, macchiato, 1)

/* This struct represents the common header fields in every Macchiato message. */
struct macchiato_msg_hdr_common_fields{
	uint32_t u32ProtocolVersion;
	uint32_t u32MsgId;
	uint32_t u32MsgLength;
	uint32_t u32ErrorCode;
};

struct macchiato_signed_header_format{
	uint8_t uccSignature[MACCHIATO_ECC_SIG_LEN];
	uint8_t devicePublicSigningKey[MACCHIATO_PUB_KEY_LEN];
	uint8_t devicePublicEncryptionKey[MACCHIATO_PUB_KEY_LEN];
};

struct macchiato_TA_firewalling_fields{
	uint8_t TaHash[MACCHIATO_MSG_TA_HASH_LEN];
	uint32_t u32NumSvcIDs;
	uint8_t u8SvcID[1];
};

/* This struct represents the fields that identify the device in the Macchiato signed message*/
struct macchiato_device_identifier{
	uint16_t u16HwVersion;
	uint32_t u32ChipSerialNum;
	uint32_t u32OEMId;
	uint8_t u8OEMPKHash[MACCHIATO_MSG_OEM_PK_HASH_LEN];
};

struct macciato_msg_data_additional_fields{
	uint8_t serverNonce[MACCHIATO_SERVER_NONCE_LEN];
	uint8_t deviceNonce[MACCHIATO_DEVICE_NONCE_LEN];
	uint32_t u32OpaqueDataLen;
	uint8_t u8OpaqueData[1];
};

#pragma pack (pop, macchiato)

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
 


 
  /**
  @brief macchiato_sign_service_data
  Use this function to sign service data with the device Machhiato private key.
  
  @param[in]    serviceData               - buffer that contains the service data to be signed.
  @param[in/out]   signedServiceData         - buffer (also allocated by the calling app) to fill the signature output in the following format:
	                                                                         <----------------------------------------------------------------- ECC signature covers this area -------------------------------------------------------------------->
	 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	|      32 bits     | 32 bits |   32 bits  |   32 bits  |   512 bits    |          512 bits         |           512 bits           | 256 bits |   32 bits   | 64 bits each |  16 bits   |    32 bits    |  128 bits | 128 bits | variable length |
	|------------------|---------|------------|------------|---------------|---------------------------|------------------------------|----------|-------------|--------------|------------|---------------|-----------|----------|-----------------|
	| Protocol Version | MSG ID  | MSG length | Error Code | ECC Signature | Device Public Signing Key | Device Public Encryption Key | TA Hash  | Num Svc IDs |    Svc IDs   | HW Version | Serial Number | Challenge | Response |   Opaque Data   |
	 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
												  
	Note: The byte order is LITTLE ENDIAN !!! (except the keys and the signature)
	Svc IDs are the identifiers of the calling QSEE application.
	All the buffers should be allocated by the caller.
	The Nonce field is zeroed.
												  
  @param[out]  pu32SignedServiceDataLen  - the length of the signature output in bytes.
  
  @return
  MACCHIATO_SUCCESS – success
  MACCHIATO_GOT_NULL_INPUT_ERROR – received null pointer
  MACCHIATO_TOO_SHORT_OUT_BUFFER_ERR - the signature output overflows the given buffer (signedServiceData), the required buffer size is returned in pu32SignedServiceDataLen
  MACCHIATO_FAILED_GET_SECURE_STATE - failed to get the error code for the message
  MACCHIATO_FAILED_SIGN_DATA - failed signing the data using Macchiato private key
  

  @dependencies
  None.

  @sideeffects
  None.
*/
 macchiato_err_t macchiato_sign_service_data(buffer_t serviceData, 
                                             buffer_t signedServiceData, 
                                             uint32_t* pu32SignedServiceDataLen);
											
  /**
  @brief macchiato_authenticate_device
  Use this function to authenticate the device using challenge response with Machhiato private key.

  This function also outputs the device id and Macchiato public key to enable signature verification.
  
  @param[in]   serviceNonce                 - buffer that contains the authentication challenge.

  @param[in]   opaqueData                   - buffer that contains additional opaque data to be signed (optional).
  @param[in/out]  signedChallengeResponse   - buffer (also allocated by the calling app) to fill the challenge response output in the following format:
                                                                            <----------------------------------------------------------------- ECC signature covers this area -------------------------------------------------------------------->
	 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	|      32 bits     | 32 bits |   32 bits  |   32 bits  |   2048 bits   |          2048 bits        |           2048 bits          | 256 bits |   32 bits   | 32 bits each |  16 bits   |    32 bits    |  128 bits | 128 bits | variable length |
	|------------------|---------|------------|------------|---------------|---------------------------|------------------------------|----------|-------------|--------------|------------|---------------|-----------|----------|-----------------|
	| Protocol Version | MSG ID  | MSG length | Error Code | RSA Signature | Device Public Signing Key | Device Public Encryption Key | TA Hash  | Num Svc IDs |    Svc IDs   | HW Version | Serial Number | Challenge | Response |   Opaque Data   |
	 -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
										  
	Note: The byte order is LITTLE ENDIAN !!!(except the keys and the signature)
	Svc IDs are the identifiers of the calling QSEE application.
	All the buffers should be allocated by the caller.
  
  @param[out]  pu32SignedChallengeResponseLen  - the length of the challenge response output in bytes.
  
  @return
  MACCHIATO_SUCCESS – success
  MACCHIATO_GOT_NULL_INPUT_ERROR – received null pointer
  MACCHIATO_TOO_SHORT_OUT_BUFFER_ERR - the signature output overflows the given buffer (signedChallengeResponse), the required buffer size is returned in pu32SignedChallengeResponseLen
  MACCHIATO_FAILED_GET_SECURE_STATE - failed to get the error code for the message
  MACCHIATO_FAILED_SIGN_DATA - failed signing the data using Macchiato private key
  MACCHIATO_ALLOCATION_ERROR - failed to allocate helper buffer
  MACCHIATO_FAILED_GENERATE_RANDOM_RESPONSE - failed to generate random response
  @dependencies
  None.

  @sideeffects
  None.
*/
 
 macchiato_err_t macchiato_authenticate_device(buffer_t serviceNonce, 
                                               buffer_t deviceNonce, 
                                               buffer_t opaqueData, 
                                               buffer_t signedChallengeResponse, 
                                               uint32_t* pu32SignedChallengeResponseLen);

 /**
 @brief macchiato_provision_service_key
 Use this function to get the unwrapped service key from the Macchiato provisioning message.

 @param[in]   provisionMsg          - buffer that contains the provisioning message of the following format:
  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	   <------------------------ RSA signature covers this area ------------------------>
 	 --------------------------------------------------------------------------------------------------------------------------------------------------------
	|      32 bits     | 32 bits |   32 bits  |   32 bits  |   2048 bits   |  variable length | 128 bits |          2048 bits         |    variable length   |
	|------------------|---------|------------|------------|---------------|------------------|----------|----------------------------|----------------------|
	| Protocol Version | MSG ID  | MSG length | Error Code | RSA Signature | KPSP Certificate | Response | ECC Encrypted Wrapping Key | Wrapped Service Key  |
	 --------------------------------------------------------------------------------------------------------------------------------------------------------


 @param[in/out]   unwrappedKey - A buffer to contain the unwrapped service key, the maximum size should be known to the calling application.
 @param[out]      pu32UnwarppedKeyLen
 @param[in]       deviceNonce - deviceNonce passsed in to compare with the nonce embedded in provision message.

 @return
 MACCHIATO_SUCCESS – success
 MACCHIATO_GOT_NULL_INPUT_ERROR – received null pointer
 @dependencies
 None.

 @sideeffects
 None.
*/

macchiato_err_t macchiato_provision_service_key(buffer_t provisionMsg,
                                                buffer_t unwrappedKey,
                                                uint32_t* pu32UnwarppedKeyLen,
                                                buffer_t deviceNonce,
                                                uint32_t utcSeconds);
												

 /**
 @brief return if it is a secure device or non-secure device
 
 @return
 MACCHIATO_SUCCESS – success
 MACCHIATO_FAILED_GET_SECURE_STATE – Unable to read secure states
 
 @param[in]       is_secure_state - ture: secure device; false: non-secure device

 
 @dependencies
 None.

 @sideeffects
 None.
*/
macchiato_err_t macchiato_is_secure_state(boolean *is_secure_state);

#endif /* MACCHIATO_H */
