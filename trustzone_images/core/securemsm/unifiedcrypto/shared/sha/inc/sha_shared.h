#ifndef SHA_SHARED_H
#define SHA_SHARED_H
/*===========================================================================

               A S W   S E C U R I T Y   S E R V I C E S

                    C O M M O N   U T I L I T I E S

GENERAL DESCRIPTION


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/unifiedcrypto/shared/sha/inc/sha_shared.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/10   yk     initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uc_comdef.h"
#include "uc_env.h"
#include "sha_core.h"

/**
 * SHA-2 Context definition
 */


typedef enum
{
  SW_AUTH_ALG_SHA1               = 0x1,
  SW_AUTH_ALG_SHA256,             
  SW_AUTH_ALG_INVALID            = 0x7FFFFFFF
} SW_Auth_Alg_Type;

typedef struct 
{
	uint32  counter[2];
	uint32  iv[16];
	uint32  auth_iv[8]; 
	uint8   leftover[SW_SHA_BLOCK_SIZE];
	uint32  leftover_size;
	SW_Auth_Alg_Type auth_alg;
} SW_SHA_Ctx;

#ifndef CRYPTOCNTXHANDLE
#define CRYPTOCNTXHANDLE
typedef void CryptoCntxHandle;
#endif

#ifndef IOVECDEF
typedef struct __attribute__((packed)){
  void                              *pvBase; 
  uint32                            dwLen;  
} IovecType;

typedef  struct __attribute__((packed)){
  IovecType                     *iov;                 
  uint32                            size;                 
} IovecListType;
#define IOVECDEF
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/**
 * @brief Main function for sha1/sha256 and hmac
 *
 * @param  void
 *
 * @return void
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type SW_Hash_Init(CryptoCntxHandle **handle, SW_Auth_Alg_Type auth_alg); 

/**
 * @brief Main function for sha1/sha256 and hmac
 *
 * @param  void
 *
 * @return void
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type SW_Hash_Update(CryptoCntxHandle *handle,IovecListType ioVecIn);

/**
 * @brief Main function for sha1/sha256 and hmac
 *
 * @param  void
 *
 * @return void
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type SW_Hash_Final (CryptoCntxHandle *handle,IovecListType *ioVecOut);

/**
 * @brief Main function for sha1/sha256 and hmac
 *
 * @param  void
 *
 * @return void
 *
 * @see 
 *
 */
sw_crypto_errno_enum_type SW_Hmac (uint8 * key_ptr, uint32 keylen, IovecListType ioVecIn, IovecListType * ioVecOut, SW_Auth_Alg_Type  pAlgo);

#endif /* AES_SHARED */
