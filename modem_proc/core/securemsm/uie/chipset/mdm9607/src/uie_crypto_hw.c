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

int uie_decrypt_ccm_priv(uie_key_src_t key_src,
            uint8 *key,     uint32 key_len,
            uint8 *nonce,   uint32 nonce_len,
            uint8 *payload, uint32 payload_len,
            uint8 *adata,   uint32 adata_len,
            uint8 *mac,     uint32 mac_len,
            uint8 *decrypted_payload,
            uint32 decrypted_payload_len) 
{
  return CEML_ERROR_INVALID_PARAM;
}
