#ifndef TZ_SSD_H
#define TZ_SSD_H

/**
   @file tz_ssd.h
   @brief Global type definitions for SSD
*/

/*===========================================================================
   Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/ssd/inc/tz_ssd.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
01/18/13   pre      Initial version.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "IxErrno.h"
#include "ks.h"

/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/
/**
@defgroup ssd_api SSD APIs
   This section explains the APIs provided by Secure Software Download.
   Detailed description information is provided below.

@defgroup ssd_data_structures Data Structures and Constants
   This section defines the various enumerated constants and data structures used by the SSD.
@ingroup ssd_api
*/

#define OVERFLOWS(ptr, len) ((unsigned int) (ptr) >                 \
                             ((unsigned int) (ptr) + len))
#define _SIZEOF_CHK(xx) {extern int see_code[1-2*(xx)];}
#define SIZEOF_CHK(xx) _SIZEOF_CHK(xx)
#define IS_ALIGNED(xx, aln) (0 == ((unsigned int) (xx) & (aln - 1)))
#define IS_WORD_ALIGNED(xx) IS_ALIGNED(xx, sizeof(int))


/**
@ingroup ssd_data_structures
@{ */

/*@{*/
#define SSD_MAX_VERSION_LEN           4         /**< Max length of SSD version */
#define SSD_MAX_MFG_ID_LEN            20        /**< Max lenght of manufacturers ID */
#define SSD_SHA256_BLOCK_SIZE         32        /**<  */
#define SSD_RSA_KEY_SIZE              256       /**< We will use RSA 2048 */
#define SSD_MAX_IEK_CIPHER_SIZE       256       /**< Max size of Image Encryption Key */
#define SSD_MD_SIGNATURE_SIZE         256       /**< Size of MD tag signature */
#define SSD_MAX_SIG_LEN               256       /**< Max Signature length supported */
#define SSD_PUB_KEY_ID_SIZE           20        /**<  */
#define SSD_AES_BLOCK_SIZE            16        /**<  */
#define SSD_AES_128_KEY_SIZE          16        /**< Size of AES key */
#define SSD_AES_IV_SIZE               16        /**< Size of IV */
#define SSD_INVALID_CTX_ID            0
/*@}*/

/**
 * Various Crypto Algorithms supported by SSD
 */
typedef enum
{
   SSD_CRYPTO_ALGO_NULL,              /**< No Crypto algorithm  */
   SSD_CRYPTO_ALGO_AES128,            /**< AES with 128 bit key */
   SSD_CRYPTO_ALGO_SHA256,            /**< SHA with 256 bit hash */
   SSD_CRYPTO_ALGO_RSA_2048,          /**< RSA with 2048 bit key */
} ssd_crypto_algo_etype;

/**
 * Various Padding types supported by SSD
 */
typedef enum
{
   SSD_CRYPTO_PAD_NULL,               /**< No padding */
   SSD_CRYPTO_PAD_PKCSV15             /**< Padding as per PKCS V1.5
                                           specification */
} ssd_crypto_pad_etype;

/**
 * Various feedback modes supported by SSD
 */
typedef enum
{
   SSD_CRYPTO_FB_MODE_NULL,           /**< No Feedback mode */
   SSD_CRYPTO_FB_MODE_CBC             /**< CBC Feedback mode */
} ssd_crypto_fb_mode_etype;

/**
 * Structure that holds the parsed Metadata
 */
typedef struct
{
  ks_ent_id_t                dvc_key_id;      /**< Key ID of key used
                                                   to encrypt IEK */
  ks_ent_id_t                oem_key_id;      /**< Hash of public
                                                   key used to sign */

  ssd_crypto_algo_etype      iek_crypt_algo;  /**< Encryption
                                                   Algo used to
                                                   encrypt IEK */
  ssd_crypto_pad_etype       iek_pad_type;    /**< Padding type for
                                                   IEK encryption */
  uint8                      iek_cipher_val[SSD_MAX_IEK_CIPHER_SIZE];
                                              /**< Encrypted IEK
                                                   cipher text */
  uint8                      iek_enc_iv[SSD_AES_BLOCK_SIZE];
                                              /**< IV used to encrypt
                                                   IEK */
  ssd_crypto_algo_etype      img_enc_crypt_algo;
                                              /**< Encryption
                                                   Algo used to
                                                   encrypt image */
  uint8                      img_enc_iv[SSD_AES_BLOCK_SIZE];
                                              /**< Image encryption IV */
  ssd_crypto_algo_etype      md_sig_sign_algo;/**< MD_SIGN signature
                                                   method */
  uint8                      md_sign_val[SSD_RSA_KEY_SIZE];
                                              /**< Hash of the
                                                   encrypted image */
  uint32                     len;             /**< Length of the parsed
                                                   metadata region in
                                                   memory. */
} ssd_metadata_type;

typedef struct
{
  ssd_metadata_type          md;              /**< parsed metadata info */
  uint32                     ctx_id;          /**< Context id for
                                                   the struct. Used to
                                                   differentiate
                                                   between multiple
                                                   parsed metadatas. */
  uint32                     seg_no;          /**< Current seg number
                                                   0xffffffff for no
                                                   current seg no */
  uint8                      frag_iv[SSD_AES_BLOCK_SIZE];
                                              /**< IV for the fragment */
} ssd_context_type;


#endif /* TZ_SSD_H */
