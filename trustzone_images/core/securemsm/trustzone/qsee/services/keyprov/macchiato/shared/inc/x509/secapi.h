#ifndef SECAPI_H
#define SECAPI_H

#include "secerrno.h"
/*
  Maximum supported operand byte size for modular math.
*/
#define SECAPI_PKX_MAX_KEY_SIZE     256
  /**< Maximum supported key size (the operand byte size for modular math). */
#define SECAPI_PKX_MAX_MOD_SIZE     SECAPI_PKX_MAX_KEY_SIZE
  /**< Maximum supported modulus size (the operand byte size for modular 
       math). */
#define SECAPI_PKX_MAX_EXP_SIZE     SECAPI_PKX_MAX_KEY_SIZE
  /**< Maximum supported exponent size (the operand byte size for modular 
       math). */
#define SECAPI_PKX_MAX_BASE_SIZE    SECAPI_PKX_MAX_KEY_SIZE
  /**< Maximum supported base size (the operand byte size for modular math). */

 /**
  @name HASH interface types and definitions.
  @{
*/

#define SECAPI_HSH_MD5_DIGEST_SIZE         16  /**< MD5 digest size.*/
#define SECAPI_HSH_MD5_BLOCK_BYTE_LEN      64  /**< MD5 block length in bytes. */


#define SECAPI_HSH_SHA_DIGEST_SIZE         20 /**< SHA-1 digest size. */
#define SECAPI_HSH_SHA_BLOCK_BYTE_LEN      64 /**< SHA-1 block length in bytes. */


#define SECAPI_HSH_SHA256_DIGEST_SIZE         32 /**< SHA-256 digest size. */
#define SECAPI_HSH_SHA256_BLOCK_BYTE_LEN      64 /**< SHA-256 block length in bytes. */

#define SECAPI_HSH_AES128_CMAC_DIGEST_SIZE      16 /**< AES-128 CMAC value size. */
#define SECAPI_HSH_AES128_CMAC_KEY_SIZE         16 /**< AES-128 CMAC key size. */
#define SECAPI_HSH_AES128_CMAC_BYTE_LEN         4  /**< AES-128 returned CMAC value size. */

#define SECAPI_HSH_SNOW3G_DIGEST_SIZE      4   /**< SNOW3G digest value size. */
#define SECAPI_HSH_SNOW3G_KEY_SIZE         16  /**< SNOW3G hash key size. */
#define SECAPI_HSH_SNOW3G_IV_SIZE          16  /**< SNOW3G hash IV size. */
#define SECAPI_HSH_SNOW3G_LFSR_SIZE        16  /**< SNOW3G hash linear feedback shift register size. */
#define SECAPI_HSH_SNOW3G_FSM_SIZE         3   /**< SNOW3G hash finite state machine size. */

#define SECAPI_HSH_ZUC_DIGEST_SIZE         4   /**< ZUC digest value size. */
#define SECAPI_HSH_ZUC_KEY_SIZE            16  /**< ZUC hash key size. */
#define SECAPI_HSH_ZUC_IV_SIZE             16  /**< ZUC hash IV size. */


#define SECAPI_HSH_KASUMI_DIGEST_SIZE      4   /**< Kasumi digest value size. */
#define SECAPI_HSH_KASUMI_KEY_SIZE         16  /**< Kasumi hash key size. */
#define SECAPI_HSH_KASUMI_IV_SIZE          16  /**< Kasumi hash IV size. */

#define SECAPI_HSH_LARGEST_DIGEST_SIZE     SECAPI_HSH_SHA256_DIGEST_SIZE    /**< Maxmium hash digest size. */
#define SECAPI_HSH_LARGEST_BLOCK_BYTE_LEN  SECAPI_HSH_SHA256_BLOCK_BYTE_LEN /**< Maxmium hash block size */

#define SECAPI_HSH_MAX_INPUT_BYTE_LENGTH  80000 /**< Maxmium hash input string byte length. */

/** @} */  /* end_name_HASH interface types and definitions. */

#endif /* SECAPI_H */



