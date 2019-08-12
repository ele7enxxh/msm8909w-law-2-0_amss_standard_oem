#ifndef QSEE_UF_SHA_H
#define QSEE_UF_SHA_H

/**
@file qsee_uf_sha.h
@brief Provide Software crypto Hash and Hmac API wrappers
*/

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      ------------------------------------
03/05/13   amen      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/** 
 * SHA DIGEST and Block sizes 
  */
#define SW_SHA1_DIGEST_SIZE             20
#define SW_SHA256_DIGEST_SIZE           32
#define SW_SHA_BLOCK_SIZE               64

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
 * Enum for Hash algorithm type
 */
typedef enum
{
  SW_AUTH_ALG_SHA1               = 0x1,
  SW_AUTH_ALG_SHA256,
  SW_AUTH_ALG_SHA_INVALID = 0x7FFFFFFF,
} SW_Auth_Alg_Type;

#ifndef SW_CRYPTO_TYPEDEF
#define SW_CRYPTO_TYPEDEF
typedef uint32 sw_crypto_errno_enum_type; 

typedef struct
{
  void                              *pvBase; 
  uint32                            dwLen;  
} __attribute__((__packed__)) IovecType;

typedef struct
{
  IovecType                     *iov;                 
  uint32                        size;                 
} __attribute__((__packed__)) IovecListType;


typedef void CryptoCntxHandle;
#endif

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


/**
 * @brief Main function for sha1/sha256 and hmac
 *
 * @param handle       [in] Pointer of pointer to hash context
 * @param auth_alg     [in] see SW_Auth_Alg_Type
 *
 * @return errno_enum_type
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type qsee_SW_Hash_Init(CryptoCntxHandle **handle, SW_Auth_Alg_Type auth_alg); 

/**
 * @brief Update function for sha1/sha256 for intermediate data 
 *        blocks hash
 *
 * @param handle       [in] Pointer of pointer to hash context
 * @param ioVecIn      [in] Input to cipher
 *
 * @return errno_enum_type
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type qsee_SW_Hash_Update(CryptoCntxHandle *handle,IovecListType ioVecIn);

/**
 * @brief Sha1/Sha256 last block hash update
 *
 * @param handle       [in] Pointer of pointer to hash context
 * @param ioVecOut     [in] Output from cipher
 *
 * @return errno_enum_type
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type qsee_SW_Hash_Final (CryptoCntxHandle *handle,IovecListType *ioVecOut);

/**
 * @brief Main function for sha1/sha256 hmac
 *
 * @param key_ptr      [in] Pointer of Key for HMAC
 * @param keylen       [in] key length (16bytes for SHA1 / 
 *                          32bytes for SHA256)
 * @param ioVecIn      [in] Input to cipher
 * @param ioVecOut     [in] Output from cipher 
 * @param pAlgo        [in] See enum SW_Auth_Alg_Type  
 *
 * @return errno_enum_type
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type qsee_SW_Hmac (uint8 * key_ptr, uint32 keylen, IovecListType ioVecIn, 
                              IovecListType * ioVecOut, SW_Auth_Alg_Type  pAlgo);

#endif /* QSEE_UF_SHA_H */
