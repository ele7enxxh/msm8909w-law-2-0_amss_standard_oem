#ifndef PR_DECRYPT_SVC_H
#define PR_DECRYPT_SVC_H

/** @file pr_decrypt_svc.h
 * @brief
 * This file contains the definitions of the constants, data structures and
 * and functions for playready's decryption implementation on TZ Service Side
 */
/*===========================================================================
  Copyright © 2012-14 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/14   tp     Modified by running "atyle --style=allman" command.
10/30/12   hw     Initial Version, to support non-contiguous output buffer.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/** @addtogroup OEMCrypto_Svc
  @} */
#include "comdef.h"
#include "tzbuffer.h"
#include "qsee_cipher.h"
#include "qsee_core.h"
#include "playready_entry.h"
#include "oemcipher.h"

typedef enum
{
    PRCIP_SUCCESS = 0,
    PRCIP_FAIL = 1
} PRCIP_STATUS;

struct PRCipher
{
    qsee_cipher_ctx       * ctx;
    uint8                 * pNonce;
    unsigned long long    * pBlockOffset;
    uint32                  byteOffset;
};

const void * PRCipher();

void prcipher_int(struct PRCipherContext * ctx,
                  qsee_cipher_ctx * p_ctx,
                  uint8           * p_nonce,
                  uint8           * p_blockOffset,
                  uint32          byteOffset);


/**************************************************************
  This function calls TzBSP to decrypt the content in AES CTR mode.

  @return
  Success - 0
  Failure - 1

  @param[in]	  *pPKCipherContext       cipher context.
  @param[in]	  *encryptionMetaData     List of encrypted regions.
  @param[in]	   numEncryptedUnits      Number of encrypted regions.
  @param[in]	   in_data_ptr            Content Buffer Pointer.
  @param[in]	   out_data_buf           Output Buffer Pointer.
  @param[in]	   out_offset             Offset to Output Buffer Pointer.


  @dependencies
  Assumes that the ciphercontext is initialized with Algo and AES key has been set.

  @sideeffects
  None
****************************************************************/
uint8 decryptAESCTRContent(DRM_CIPHER_CONTEXT             *pPKCipherContext,
                           tzEncryptionMetaData_t         *encryptionMetaData,
                           uint8			                numEncryptedUnits,
                           uint8                          *in_data_ptr,
                           TzBuffer_t                     *out_data_buf,
                           uint32                          out_offset);

uint8 decryptCocktailContent(DRM_CIPHER_CONTEXT             *pPKCipherContext,
                             tzEncryptionMetaData_t         *encryptionMetaData,
                             uint8			                numEncryptedUnits,
                             uint8                          *in_data_ptr,
                             TzBuffer_t                     *out_data_buf,
                             uint32                          out_offset);

#endif //PR_DECRYPT_SVC_H
