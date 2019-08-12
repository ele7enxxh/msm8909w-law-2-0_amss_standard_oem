#ifndef SSD_BOOT_H
#define SSD_BOOT_H
/** 
 * @file ssd_boot.h
 * @brief This module only has one API that can be used to 
 *        1. Validate & authenticate
 *        2. Parse the metadata
 *        3. Decrypt the encrypted binary image.
 * This file contains the definitions of the constants, data structures, 
 * and interfaces that comprise the Secure Software Download 
 * module. 
 * Only the interfaces declared shall be used by the client for accessing 
 * the Secure Software Download module. 
 *  
 *  
 * ===========================================================================
 *                           Edit History 
 * when       who   what, where, why 
 * --------   ---   -------------------------------------------- 
 * 02/02/10   rv     Create Module 
 * ===========================================================================
*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "boot_comdef.h"
#include "IxErrno.h"


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

/**
@ingroup ssd_data_structures
@{ */

/*@{*/
#define SSD_MAX_VERSION_LEN           4                   /**< -- Max length of SSD version */
#define SSD_MAX_MFG_ID_LEN            20                  /**< -- Max lenght of manufacturers ID */
#define SSD_SHA256_BLOCK_SIZE         32                  /**< --  */
#define SSD_RSA_KEY_SIZE              128                 /**< -- We will use RSA 1024 */
#define SSD_IEK_CIPHER_SIZE           SSD_RSA_KEY_SIZE    /**< -- Size of Image Encryption Key */
#define SSD_MD_SIGNATURE_SIZE         SSD_RSA_KEY_SIZE    /**< -- Size of MD tag signature */
#define SSD_MAX_SIG_LEN               SSD_RSA_KEY_SIZE    /**< -- Max Signature length supported */
#define SSD_PUB_KEY_ID_SIZE           20                  /**< --  */
#define SSD_AES_BLOCK_SIZE            16                  /**< --  */

#define SSD_MAX_RSA_KEY_FILE_SIZE     1024                /**< -- Size of file in bits */
#define SSD_AES_128_KEY_SIZE          16
/*@}*/

/** 
 * Various Crypto Algorithms supported by SSD
 */
typedef enum
{
   SSD_CRYPTO_ALGO_NULL,              /**< -- No Crypto algorithm  */
   SSD_CRYPTO_ALGO_AES128,            /**< -- AES with 128 bit key */
   SSD_CRYPTO_ALGO_SHA256,            /**< -- SHA with 256 bit hash */
   SSD_CRYPTO_ALGO_RSA_1024,          /**< -- RAS with 1024 bit key */
}ssd_crypto_algo_etype;

/** 
 * Various Padding types supported by SSD
 */
typedef enum
{
   SSD_CRYPTO_PAD_NULL,               /**< -- No padding */
   SSD_CRYPTO_PAD_RFC2630,            /**< -- Padding as per RFC 2360 */
   SSD_CRYPTO_PAD_PKCSV15             /**< -- Padding as per PKCS V1.5 specification */
}ssd_crypto_pad_etype;

/** 
 * Various feedback modes supported by SSD
 */
typedef enum
{
   SSD_CRYPTO_FB_MODE_NULL,           /**< -- No Feedback mode */
   SSD_CRYPTO_FB_MODE_CBC             /**< -- CBC Feedback mode */
}ssd_crypto_fb_mode_etype;

/** 
 * Structure that holds the parsed Metadata
 */
typedef struct
{
   uint8                      md_ver[SSD_MAX_VERSION_LEN+1];          /**< -- Metadata Version */
   char                       mfg_id[SSD_MAX_MFG_ID_LEN+1];           /**< -- Manufacturers ID */
   uint8                      sw_ver[SSD_MAX_VERSION_LEN+1];          /**< -- AMSS Software version */

   ssd_crypto_algo_etype      iek_crypt_algo;                         /**< -- Encryption Algo used to encrypt IEK */
   ssd_crypto_pad_etype       iek_pad_type;                           /**< -- Padding type for IEK encryption */
   uint8                      iek_pub_key_id[SSD_SHA256_BLOCK_SIZE];  /**< -- Hash of public key used to encrypt IEK */
   uint8                      iek_cipher_val[SSD_IEK_CIPHER_SIZE];    /**< -- Encrypted IEK cipher text */
   ssd_crypto_algo_etype      img_enc_crypt_algo;                     /**< -- Encryption Algo used to encrypt image */
   uint8                      img_enc_iv[SSD_AES_BLOCK_SIZE];         /**< -- Image encryption IV */
   ssd_crypto_fb_mode_etype   img_fb_mode;                            /**< -- Feedback mode for Image encryption */
   ssd_crypto_pad_etype       img_pad_type;                           /**< -- Padding type for image encryption */
   ssd_crypto_algo_etype      img_hash_algo;                          /**< -- Image Hash method */
   uint8                      img_hash_val[SSD_SHA256_BLOCK_SIZE];    /**< -- Hash of the encrypted Image */
   ssd_crypto_algo_etype      md_sig_dgst_algo;                       /**< -- MD_SIGN digest method */
   ssd_crypto_algo_etype      md_sig_sign_algo;                       /**< -- MD_SIGN signature method */
   ssd_crypto_pad_etype       md_sig_pad_type;                        /**< -- Padding type for MD_SIGNATURE */
   uint8                      md_sign_val[SSD_RSA_KEY_SIZE];          /**< -- Hash of the encrypted Image */
   uint8                      oem_pub_key_id[SSD_SHA256_BLOCK_SIZE];  /**< -- Hash of public key used to sign */
   uint32                     img_enc_img_size;                       /**< -- Size of encrypted image */
   uint32                     bytes_read;                             /**< -- Number of bytes read */
   uint8                      *enc_img_ptr;                           /**< -- Pointer to encrypted Data */
}ssd_metadata_type;

/** 
 * Status of the Process Image engine
 */
typedef enum
{
   /* Continue or successful states */
   SSD_PROC_IMG_STAT_CONTINUE,                  /**< -- Continue processing */
   SSD_PROC_IMG_STAT_FINISH_NO_ENCRYPTION,      /**< -- Processing complete, No encrypted data found */
   SSD_PROC_IMG_STAT_AUTH_SUCCESS,              /**< -- Processing Complete, Authentication successful */
   SSD_PROC_IMG_STAT_DECRYPT_FINISH,            /**< -- Processing Complete, Decryption complete */

   /* Failed states */
   SSD_PROC_IMG_STAT_ABORT,                     /**< -- Processing complete, Some unknown failure */
   SSD_PROC_IMG_STAT_BAD_DATA,                  /**< -- Processing complete, Bad input data found */
   SSD_PROC_IMG_STAT_MD_AUTH_FAIL,              /**< -- Processing complete, Metadata authentication failed */
   SSD_PROC_IMG_STAT_IMG_AUTH_FAIL,             /**< -- Processing complete, Image authentication failed */
   SSD_PROC_IMG_STAT_DECRYPT_FAIL               /**< -- Processing complete, Image decryption failed */
}ssd_proc_img_status_etype;
/** 
 * Structure that holds the pointer & length to encrypted data
 */
typedef struct
{
   uint32       offset;             /**< -- offset from the begining of the buffer */
   uint32       len;                /**< -- length of the decrypted data */
}ssd_decrypt_data_type;
 /** 
@} End of ino_dal_data_structures
*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/**

FUNCTION SSD_BOOT_PROC_IMG()

@brief
  The function does the following:
  1. Authenticates the Metadata header
  2. Runs a HASH over the encrypted image if bauthImage is set to TRUE
  3. Decrypts the image if bdecryptImage is set to TRUE
 
  The caller should repeatedly call this function till
  the status is SSD_DECRYPT_CONTINUE
 
  This function will return the status of the decryption engine.
 
PARAMETERS
  @param [in]     bReset        - If TRUE then it resets the engine  
  @param [in]     ip_buf        - Pointer to input buffer
  @param [in]     ip_buf_len    - Length of input buffer
  @param [in,out] pmd           - Pointer to structure that stores parsed
                                  data 
  @param [out]    decrypt_data  - Structure that points to the decrypted
                                  data & length of decrypted data
  @param [in]     bauthImage    - If set to TRUE, then the encrypted image
                                  will be authenticated
  @param [in]     bdecryptImage - If set to TRUE, then the encrypted image
                                  will be decrypted
 

@dependencies
  None
 
@sideeffects
  None 
 
RETURN VALUE 
@return One of the following error codes 
   Status of Process engine
   <ul>
   <li>SSD_BOOT_CONTINUE               - Continue, engine is ready to process next block
                                         Check decrypt data structure if there is any
                                         decrypted data available
 
   <li>SSD_PROC_IMG_DECRYPT_FINISH_NO_ENCRYPTION - Metadata header is authenticated; but the Image is
                                               not encrypted. Check the decrypt data structure
                                               for the begining of image
 
   <li>SSD_PROC_IMG_DECRYPT_FINISH         - Finished decrypting the last block of data
   <li>SSD_PROC_IMG_ABORT_MD_AUTH_FAIL     - Failed to authenticate the Metadata header
   <li>SSD_PROC_IMG_ABORT_IMG_AUTH_FAIL    - Failed to validate Image integrity
   <li>SSD_PROC_IMG_ABORT_BAD_DATA         - The input data is not 16 byte aligned
   <li>SSD_PROC_IMG_ABORT_DECRYPT_FAIL     - Failed to decrypt the image

@sa 
  None 
 
*/ 


extern ssd_proc_img_status_etype ssd_boot_proc_img
(
   boolean                 bReset,
   uint8                  *ip_buf,
   uint32                  ip_buf_len,
   ssd_metadata_type      *pmd,
   ssd_decrypt_data_type  *decrypt_data,
   boolean                 bauthImage,
   boolean                 bdecryptImage
);
#endif /* SSD_BOOT_H */
