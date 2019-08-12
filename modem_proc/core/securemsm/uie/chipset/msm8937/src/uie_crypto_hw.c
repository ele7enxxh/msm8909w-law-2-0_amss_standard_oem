/*===========================================================================
            Unified Image Encryption (UIE) Crypto Engine Interface

GENERAL DESCRIPTION
 Provide an interface for cryptodriver configuration for UIE algorithms 

Copyright (c) 2014 QUALCOMM Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                                 INCLUDE FILES
===========================================================================*/

#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <comdef.h>
#include "IxErrno.h"
#include "CeML.h"
#include "uie.h"
#include "uie_priv.h"
#include "uie_env.h"


/*===========================================================================
                          MACROS AND TYPE DECLARATIONS
===========================================================================*/

#define BREAKIF(cond) if (cond) break
#define RESIDUE_POWER_2(addr, size) ((addr) & ((size) - 1))
#define ROUND_UP_POWER_2(addr, size)                  \
  RESIDUE_POWER_2(addr, size)                         \
    ? ((addr) + (size) - RESIDUE_POWER_2(addr, size)) \
    : (addr)
#define MAX_ADATA_LEN_ENCODING_LENGTH 6 // Excluding the 10 octet case

/*===========================================================================
                   UIE CRYPTOGRAPHIC FUNCTION IMPLEMENTATIONS
===========================================================================*/

static uint32 format_adata_len(uint32 adata_len, uint8 *buffer_adata_len)
{
  uint32 formatted_len;

  /* Add associated data length formatting
   * NOTE: in order to maintain compatibility with the Crypto 5 driver for CCM,
   *       do not populate the adata length field with the actual length.
   *       Instead, fill in with dummy zero bytes.
   */
  if (adata_len == 0)
  {
    formatted_len = 0;
  }
  else if (adata_len < ((1 << 16) - (1 << 8))) // 0 < a < 2**16 - 2**8
  {
    formatted_len = 2;
    buffer_adata_len[0] = 0;
    buffer_adata_len[1] = 0;
  }
  else // a < 2**32, we don't worry about the case where adata_len is too big for a uint32
  {
    formatted_len = 6;
    buffer_adata_len[0] = 0;
    buffer_adata_len[1] = 0;
    buffer_adata_len[2] = 0;
    buffer_adata_len[3] = 0;
    buffer_adata_len[4] = 0;
    buffer_adata_len[5] = 0;
  }

  return formatted_len;
}

int uie_decrypt_ccm_priv(uie_key_src_t key_src,
            uint8 *key,     uint32 key_len,
            uint8 *nonce,   uint32 nonce_len,
            uint8 *payload, uint32 payload_len,
            uint8 *adata,   uint32 adata_len,
            uint8 *mac,     uint32 mac_len,
            uint8 *decrypted_payload,
            uint32 decrypted_payload_len) 
{

  CeMLCipherModeType  mode = CEML_CIPHER_MODE_CCM;
  CeMLCipherDir       dir  = CEML_CIPHER_DECRYPT;
  CeMLCntxHandle*     cntx = NULL;
  CEMLIovecListType   ioVecIn;
  CEMLIovecListType   ioVecOut;
  CEMLIovecType       IovecIn;
  CEMLIovecType       IovecOut;
  uint8               *ct = NULL;
  uint32              ct_len;
  uint8               *pt = NULL;
  uint32              pt_len;
  uint32              adjusted_adata_len = adata_len;
  uint32              buffer_a_len;
  uint8               buffer_a[MAX_ADATA_LEN_ENCODING_LENGTH];
  CeMLErrorType       retval = CEML_ERROR_INVALID_PARAM;
  CeMLErrorType       retval_teardown;
  bool                crypto_init = false;

  do {
    BREAKIF(nonce == NULL || payload == NULL || adata == NULL ||
            mac == NULL || decrypted_payload == NULL ||
            key_src != UIE_KEY_SRC_PROVIDED);

    buffer_a_len = format_adata_len(adata_len, buffer_a);
    adjusted_adata_len += buffer_a_len;
    adjusted_adata_len = ROUND_UP_POWER_2(adjusted_adata_len, CEML_AES_BLOCK_SIZE);

    ct_len = adjusted_adata_len + payload_len + mac_len;
    pt_len = ct_len;

    // Setup
    pt = UIE_MALLOC(pt_len); 
    ct = UIE_MALLOC(ct_len);
    BREAKIF(pt == NULL || ct == NULL);
    memset(pt, 0, pt_len);
    memset(ct, 0, ct_len);
    UIE_MEMSCPY(ct, ct_len, buffer_a, buffer_a_len);
    UIE_MEMSCPY(ct + buffer_a_len, ct_len, adata, adata_len);
    UIE_MEMSCPY(ct + adjusted_adata_len, ct_len, payload, payload_len);
    UIE_MEMSCPY(ct + adjusted_adata_len + payload_len, ct_len, mac, mac_len);

    ioVecIn.size = 1;
    ioVecIn.iov = &IovecIn;
    ioVecIn.iov[0].dwLen = ct_len;
    ioVecIn.iov[0].pvBase = ct; 

    ioVecOut.size = 1;
    ioVecOut.iov = &IovecOut;
    ioVecOut.iov[0].dwLen = pt_len;
    ioVecOut.iov[0].pvBase = pt;

    retval = CeMLInit();
    crypto_init = true;
    BREAKIF(retval != CEML_ERROR_SUCCESS);

    if (key_len == CEML_AES128_IV_SIZE)
    {
      retval = CeMLCipherInit(&cntx, CEML_CIPHER_ALG_AES128);
    }
    else
    {
      retval = CeMLCipherInit(&cntx, CEML_CIPHER_ALG_AES256);
    }
    BREAKIF(retval != CEML_ERROR_SUCCESS);

    retval |= CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_MODE,
                                 &mode, sizeof(CeMLCipherModeType));
    retval |= CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_DIRECTION,
                                 &dir, sizeof(CeMLCipherDir));
    retval |= CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_KEY,
                                 key, key_len);
    retval |= CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_CCM_HDR_LEN,
                                 &adata_len, sizeof(uint32));
    retval |= CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_CCM_PAYLOAD_LEN,
                                 &payload_len, sizeof(uint32));
    retval |= CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_CCM_MAC_LEN,
                                 &mac_len, sizeof(uint32));
    retval |= CeMLCipherSetParam(cntx, CEML_CIPHER_PARAM_NONCE,
                                 nonce, nonce_len);
    BREAKIF(retval != CEML_ERROR_SUCCESS);

    // Decryption
    retval = CeMLCipherData(cntx, ioVecIn, &ioVecOut);
    BREAKIF(retval != CEML_ERROR_SUCCESS);

    UIE_MEMSCPY(decrypted_payload, decrypted_payload_len,
                pt + adjusted_adata_len, payload_len);

    retval = CEML_ERROR_SUCCESS;
  } while(0);

  // Teardown
  if (cntx != NULL)
  {
    retval_teardown = CeMLCipherDeInit(&cntx);
    if (retval == CEML_ERROR_SUCCESS)
    {
      retval = retval_teardown;
    }
  }
  if (crypto_init)
  {
    retval_teardown = CeMLDeInit();
    if (retval == CEML_ERROR_SUCCESS)
    {
      retval = retval_teardown;
    }
  }

  if (pt != NULL)
  {
    UIE_FREE(pt);
  }
  if (ct != NULL)
  {
    UIE_FREE(ct);
  }

  return retval;
}
