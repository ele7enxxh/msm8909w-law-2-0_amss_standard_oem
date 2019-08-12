#ifndef SECAPI_H
#define SECAPI_H


/** 
  @file secapi.h
  @brief General Cryptographic Algorithm support API.
  This file contains the definitions of the constants, data structures
  and interfaces that provide Cryptographic Alogrithms support. It
  defines the external interfaces for key exchange, message digest, and
  encrypt/decrypt for various algorithms.
 * 
 */
/*===========================================================================
  Copyright (c) 2009 - 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/crypto/secapi.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/12/13   nk      Optimised kasumi stream cipher
08/26/13   nk      AES 256 secapi interface support 
12/12/12   sm      Updated secapi_secure_message() and secapi_authenticate_decrypt_message()
                   to support passing subsystem ID and client ID
04/30/12   nk      ZUC algorithm ported. 
09/21/11   sm      Added secapi_enc_encrypt_iov_mac() & secapi_enc_decrypt_iov_mac()
08/28/10   dm      Crypto API files rearchitecture
06/06/10   qxu     added mainpage, dscriptions for functions, constants and data types
                   to doxygenate API document.
02/11/10   ejt     Added SNOW3G LFSR & FSM constants
08/28/09   ejt     Removed #include "customer.h" and FEATURE flags for CMI compliance
06/10/09   ejt     Added secapi_hsh_create_hmac_dsm() & secapi_hsh_create_digest_dsm()
                   secapi_enc_encrypt_dsm() & secapi_enc_decrypt_dsm() routines to secapi.
05/06/09   ejt     Added secapi_hsh_create_hmac_iov() & secapi_hsh_create_digest_iov()
                   secapi_enc_encrypt_iov() & secapi_enc_decrypt_iov() routines to secapi.
                   These routines take msg in/out based on a IOVEC_T pointer.
11/25/08    rv     Added SECAPI_GET_RANDOM_SET_CB
09/24/08   EJT     Added SECAPI_ENC_DEVKEY_SFS & SECAPI_HSH_DEVKEY_SFS defines  
02/19/08   pc      Added ONCRPC meta comments.
04/10/07   rv      added support for SHA256 on dsp
11/22/06   df      Add support for enc input mode
08/02/06   rv      support CTR mode encryption
08/31/05   df      changes to support IOVEC
03/21/05   sb      Added support for Key Store API, Change to pkx_authenticate API
02/25/05   sb      Added support for Rsa signing, MSM device key based DSP commands
06/08/04   jay     Added RFC2630_PADDING
04/22/04   rwh     Minor formatting change.
03/08/04   rwh     Added RFC-2104-style HMAC.
03/02/04   rwh     Removed endianess params in encrypt(), decrypt(), digest().
                   Added return status in get_padded_length().
02/19/04   rwh     Increased modmath param size to 256 bytes.
09/25/03   yhw     Added SECAPI_AES128.
10/30/03   rwh     Added secapi_pkx_dh_private_key_len_type
09/04/03   om      Added SECPKX_NO_PADDING enum value.
07/22/03   rwh     Added some authentication #define's.
05/28/03   jm      Cleanup for new seccrypt interface.
12/12/02   rwh     Corrected comment.
12/11/02   rwh     Removed DH and RSA callback param types; instead use 
                   general event handler callback.
11/18/02   rwh     Corrected DH parameter name.
10/21/02   om      Updates for new Brew requirements.
08/01/02   om      Created file.

=============================================================================*/

#include "comdef.h"
#include "rex.h"
#include "dsm.h"
#include "secerrno.h"

/*===========================================================================

                        DEFINITIONS AND TYPES

===========================================================================*/
// Define secapi random fill buffer timer 
#define SECAPI_RANDOM_FILL_BUFFER_TIMER (5 * 1000)   // 5 seconds 

#define SECRND_INIT_DONE "secrnd_init_done" 

/*===========================================================================
  General CRYPTO API types and definitions
===========================================================================*/
/** @addtogroup crypto_secalgoapi 
  @{ */

/**
  CRYPTO session external handle definition.
*/
typedef void * secapi_handle_type;

/**
  Asynchronous execution callback registration.
 */
typedef struct
{
  secerrno_cb_fptr_type  cb_fptr;  /**< Callback function pointer. */
  void*                  usr_ptr;  /**< User function pointer. */
} secapi_async_exec_cb_param_data_type; 

/**
  IOVEC structure.
*/
typedef struct {
    void   *pvBase;             /**< Starting address of the I/O vector. */
    uint32  dwLen;              /**< Length of the I/O vector in bytes. */
}IOVEC;

/**
  IOVEC_T structure.
*/
typedef struct {
  IOVEC  *iov;                  /**< Pointer to the iovec table. */
  uint32 size;                  /**< Number of iovecs; size of the iovec 
                                     table. */
  uint32 max_size;              /**< Maximum number of iovecs that can be 
                                     allocated. */
  uint32 tot_byte_cnt;          /**< Total byte count. */
  uint32 offset;                /**< Starting data byte. */
  uint32 ref_cnt;               /**< Future use of the reference count. */
}IOVEC_T;

/**
  Data Get function pointer.
 */
typedef secerrno_enum_type (*secapi_data_get_fptr_type)(void*, void *, int *);

/**
  Function pointer for getting sensitive data.
 */
typedef struct
{
  secapi_data_get_fptr_type  fptr;  /**< Function pointer for getting parameter 
                                         data. */
  void*                  usr_ptr;   /**< User data pointer. */
} secapi_data_get_func_param_data_type; 

/**
  Possible Crypographic algorithms: Key exchanges, hash functions, and 
  encrypt-decrypt functions.
*/
typedef enum
{
  SECAPI_NULL_KEY_X          = 0,  /**< No key exchange protocol used. */
  SECAPI_MODEXP              = 1,  /**< Generic modulo exponentiation. */
  SECAPI_DIFFIE_HELLMAN      = 2,  /**< Diffie-Hellman key exchange protocol. */
  SECAPI_RSA                 = 3,  /**< RSA Encrypt/Key exchange protocol. */
  SECAPI_NULL_HASH           = 4,  /**< No simultaneous hash with encryption-decryption. */
  SECAPI_SHA                 = 5,  /**< SHA-1 hash function. */
  SECAPI_SHA256              = 6,  /**< SHA256 hash function. */
  SECAPI_MD5                 = 7,  /**< MD5 hash function. */
  SECAPI_NULL_CIPHER         = 8,  /**< No encryption selected. */
  SECAPI_DES                 = 9,  /**< DES encryption/decryption function. */
  SECAPI_3DES                = 10, /**< 3DES encryption/decryption function. */
  SECAPI_ARC4                = 11, /**< ARC4 encryption/decryption function. */
  SECAPI_AES128              = 12, /**< AES cipher; 128-bit key. */
  SECAPI_C2                  = 13, /**< C2 cipher. */
  SECAPI_AES128_CMAC	     = 14, /**< 128-bit AES in CMAC mode. */
  SECAPI_SNOW3G_UEA2 	     = 15, /**< UEA2 SNOW 3G Encryption Algorithm. */
  SECAPI_SNOW3G_UIA2 	     = 16, /**< UIA2 SNOW 3G Integrity Algorithm. */
  SECAPI_ZUC_CIPHER          = 17, /**< UEA3 ZUC Encryption Algorithm. */
  SECAPI_ZUC_INTEGRITY       = 18, /**< UIA3 ZUC Integrity Algorithm. */
  SECAPI_AES256              = 19, /**< AES cipher; 256-bit key. */
  SECAPI_ECDSA               = 20, /**< ECDSA Integirty Algorithm */
  SECAPI_KASUMI_CIPHER       = 21, /**< UEA1 Kasumi Encryption Algorithm. */
  SECAPI_KASUMI_INTEGRITY    = 22, /**< UIA1 Kasumi Integrity Algorithm. */
  SECAPI_ALGO_MAX,                 /**< Must be last in list; sizes the array. */
  SECAPI_RESERVED        = 0x7FFFFFFF /**< Reserved. */
} secapi_algo_enum_type;

/**
  Parameter type for SECAPI_EXEC_MODE commands.
*/
typedef enum
{
  SECAPI_ASYNC_EXEC  = 0,     /**< Asynchronous execution (non-blocking). */
  SECAPI_SYNC_EXEC   = 1      /**< Synchronous execution (blocking). */
} secapi_exec_mode_enum_type;

/**
  Parameter type for the endianess of the data format.
*/
typedef enum
{
  SECAPI_NETWORK_ORDER    = 0, /**< Big endian data format. */
  SECAPI_WORD_HOST_ORDER  = 1  /**< Small endian word (16-bit) format. */
} secapi_endian_type;

/**
  Execution platforms. These are used as indices into the state change function 
  pointer tables. Do not change. 
*/
typedef enum
{
  SECAPI_EXEC_PLATFORM_ARM = 0,  /**< ARM platform. */ 
  SECAPI_EXEC_PLATFORM_DSP = 1,  /**< DSP platform. */
  SECAPI_EXEC_PLATFORM_HW  = 2,  /**< HW CRYPTO Engine platform. */
  SECAPI_EXEC_PLATFORM_SIM = 3,  /**< SIM platform. */
  SECAPI_EXEC_NUM_PLATFORM = 4,  /**< Number of execution platform types. */

 /* //depot/Living_Documents/WIP_for_writers/Sherry/crypto/inc/secutil_v.h */
  SECAPI_EXEC_PLATFORM_ACCELERATOR_PREFERRED = SECAPI_EXEC_NUM_PLATFORM,
    /**<  Default setting for all instances. */
  SECAPI_EXEC_UNKNOWN_PLATFORM,  /**< Unknown platform. */

  SECAPI_EXEC_MAX_PLATFORM       /**< Maximum number of platform types. */
} secapi_platform_enum_type;

/**
  Execution platform data type.
 */
typedef secapi_platform_enum_type secapi_exec_platform_param_data_type; 

/**
  Parameter type for SECAPI_CALC_MAX_TIME command.
*/
typedef uint32 secapi_exec_timeout_param_data_type;

/**
  Union for all common parameters.
*/
typedef union
{
  secapi_exec_mode_enum_type            exec_mode;
    /**< Execution mode. */
  secapi_exec_timeout_param_data_type   exec_timeout;
    /**< Execution timeout. */
  secapi_exec_platform_param_data_type  exec_platform;
    /**< Execution platform. */
  secapi_async_exec_cb_param_data_type  async_cb;
    /**< Callback function when in asynchronous mode. */
  secapi_data_get_func_param_data_type  data_get;
    /**< Gets the data. */
} secapi_param_type;

/**
  Parameter types for all instances.
*/
typedef enum
{
  
  SECAPI_EXEC_MODE                = 0,
    /**< Common parameter type: Execution mode.  */
  SECAPI_EXEC_TIMEOUT             = 1,
    /**< Common parameter type: Timeout value. */
  SECAPI_EXEC_PLATFORM            = 2,
    /**< Common parameter type: Platform setting. */
  SECAPI_ASYNC_CALLBACK           = 3,
    /**< Common parameter type: Call function setting in asynchronous mode. */
  SECAPI_DATA_GET_FUNC            = 4,
    /**< Common parameter type: Get data function. */
  SECAPI_MAX_COMMON_PARAM         = 5,
    /**< Common parameter type: Maximum common parameter number. */


  SECAPI_PKX_PARAM_START          = 0x1000,
    /**< PKX-specific parameter: Start value. */
  SECAPI_PKX_MODEXP               = SECAPI_PKX_PARAM_START,
    /**< PKX-specific parameter: Modulus/exponent value setting. */
  SECAPI_PKX_DH_GROUP             = SECAPI_PKX_PARAM_START + 1,
    /**< PKX-specific parameter: DH group. */
  SECAPI_PKX_DH_PRIVATE_KEY_LEN   = SECAPI_PKX_PARAM_START + 2,
    /**< PKX-specific parameter: DH private key length. */
  SECAPI_PKX_DH_PEER_PUBLIC_KEY   = SECAPI_PKX_PARAM_START + 3,
    /**< PKX-specific parameter: DH peer public key. */
  SECAPI_PKX_RSA_MODULUS          = SECAPI_PKX_PARAM_START + 4,
    /**< PKX-specific parameter: RSA modulus value. */
  SECAPI_PKX_RSA_PRIVATE_KEY      = SECAPI_PKX_PARAM_START + 5,
    /**< PKX-specific parameter: RSA private key. */
  SECAPI_PKX_RSA_PUBLIC_KEY       = SECAPI_PKX_PARAM_START + 6,
    /**< PKX-specific parameter: RSA public key. */
  SECAPI_PKX_RSA_PRIVATE_KEY_IV   = SECAPI_PKX_PARAM_START + 7,
    /**< PKX-specific parameter: RSA private key and IV setting. */
  SECAPI_PKX_MODEXP_PRIV_KEY_IV   = SECAPI_PKX_PARAM_START + 8,
    /**< PKX-specific parameter: Modulus exponent private key and IV setting. */
  SECAPI_PKX_RSA_KS_PARAMS        = SECAPI_PKX_PARAM_START + 9,
    /**< PKX-specific parameter: RSA KS parameter. */
  SECAPI_PKX_RSA_ISIGNAL          = SECAPI_PKX_PARAM_START + 10,
    /**< PKX-specific parameter: RSA is signal, or not.  */
  SECAPI_PKX_ECC_CURVE_PARAMS     = SECAPI_PKX_PARAM_START + 11,
    /**< PKX-specific parameter: ECC curve parameters.  */
  SECAPI_PKX_ECC_PUBLIC_KEY       = SECAPI_PKX_PARAM_START + 12,
    /**< PKX-specific parameter: ECC Public Key.  */
  SECAPI_PKX_ECC_PRIVATE_KEY      = SECAPI_PKX_PARAM_START + 13,
    /**< PKX-specific parameter: ECC Private Key.  */
  SECAPI_PKX_MAX_PARAM            = SECAPI_PKX_PARAM_START + 14,
    /**< PKX-specific parameter: Maximum PKX parameter number. */ 

  SECAPI_ENC_PARAM_START          = 0x2000,
    /**< Cipher-specific parameter: Start number. */
  SECAPI_ENC_INIT_DATA            = SECAPI_ENC_PARAM_START,
    /**< Cipher-specific parameter: Key and IV setting. */
  SECAPI_ENC_OP_MODE              = SECAPI_ENC_PARAM_START + 1,
    /**< Cipher-specific parameter: Operation mode. */ 
  SECAPI_ENC_CBC_STATE            = SECAPI_ENC_PARAM_START + 2,
    /**< Cipher-specific parameter: CBC state. */
  SECAPI_ENC_INPUT_MODE           = SECAPI_ENC_PARAM_START + 3,
    /**< Cipher-specific parameter: Input mode. */
  SECAPI_ENC_IV_SYNC              = SECAPI_ENC_PARAM_START + 4,
    /**< Cipher-specific parameter: IV synchronization. */
  SECAPI_ENC_MAC_INFO             = SECAPI_ENC_PARAM_START + 5,
    /**< Cipher-specific parameter: MAC information. */
  SECAPI_ENC_PADDING_TYPE         = SECAPI_ENC_PARAM_START + 6,
    /**< Cipher-specific parameter: Padding type. */
  SECAPI_ENC_DEVKEY_SFS           = SECAPI_ENC_PARAM_START + 7,  
    /**< Cipher-specific parameter: SFS devkey or not. */  
  SECAPI_ENC_COUNTER_WIDTH        = SECAPI_ENC_PARAM_START + 8,  	
    /**< Cipher-specific parameter: Counter width if cipher is in AES CTR mode. */  	
  SECAPI_ENC_INPUT_BIT_LENGTH     = SECAPI_ENC_PARAM_START + 9,
    /**< Cipher-specific parameter: Input data buffer bit length. */
  SECAPI_ENC_MAX_PARAM            = SECAPI_ENC_PARAM_START + 10,
    /**< Cipher-specific parameter: Maximum cipher parameter number. */ 


  SECAPI_HSH_PARAM_START          = 0x3000,
    /**< Hash-specific parameters: Starting number for the hash parameter. */ 
  SECAPI_HSH_INPUT_MODE           = SECAPI_HSH_PARAM_START,
    /**< Hash-specific parameters: Input mode. */
  SECAPI_HSH_DEVKEY_SFS,                                        
    /**< Hash-specific parameters: SFS devkey, or not. */        
  SECAPI_HSH_HIPRI,
    /**< Hash-specific parameters: High priority, or not.  */
  SECAPI_HSH_INIT_DATA,
    /**< Hash-specific parameters: Initialization data. */
  SECAPI_HSH_INPUT_BIT_LENGTH,
    /**< Hash-specific parameters: Input data buffer bit length. */
  SECAPI_HSH_MAX_PARAM,
    /**< Hash-specific parameters. Maximum parameter value. */
  SECAPI_PARAM_ENUM_RESERVED      = 0x7FFFFFFF
    /**< Hash-specific parameters: Reserved. */
} secapi_param_enum_type; 

/**
  @name Key Exchange Interface Types and Definitions
  @{
*/

/**
  Enumeration for the key exchange command parameter types.
*/
typedef secapi_param_enum_type secapi_pkx_param_enum_type;

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

/*
  Supported operand byte length for PKX ECC operations.
*/
#define SECAPI_PKX_ECC_MAX_PUBLIC_KEY_LEN    32
#define SECAPI_PKX_ECC_MAX_PRIVATE_KEY_LEN   32
#define SECAPI_PKX_ECC_MAX_CURVE_PARAM_LEN   66

/**
  Generic modulo exponentiation. 
*/
typedef struct
{
  uint8*  base_ptr;             /**< PKX base value. */
  uint16  base_size;            /**< PKX base size. */
  uint8*  modulus_ptr;          /**< PKX modulus value. */
  uint16  modulus_size;         /**< PKX modulus size. */
  uint8*  exponent_ptr;         /**< PKX exponent value. */
  uint16  exponent_size;        /**< PKX exponent size. */
  uint8*  priv_key_iv_ptr;      /**< Private key IV value. */
  uint8*  priv_key_devkey_ptr;  /**< Private key devkey value. */
  uint16  priv_key_iv_size;     /**< Private key IV size. */
} secapi_pkx_modexp_param_type;

/**
  Diffie-Hellman key generation command parameter type.
*/
typedef enum
{
  SECAPI_PKX_DH_PRIVATE_KEY    = 0,   /**< Private key. */
  SECAPI_PKX_DH_PUBLIC_KEY     = 1,   /**< Public key. */
  SECAPI_PKX_DH_SECRET_KEY     = 2,   /**< Secret key. */
  SECAPI_PKX_KEY_ENUM_RESERVED = 0x7FFFFFFF /**< Reserved.  */
} secapi_pkx_key_enum_type;

/**
   PKX Diffie-Hellman group parameter type.
 */
typedef struct
{
  uint8*  base_ptr;     /**< Base value pointer. */
  uint16  base_size;    /**< Base value size. */
  uint8*  modulus_ptr;  /**< Modulus value pointer. */
  uint16  modulus_size; /**< Modulus value size. */
} secapi_pkx_dh_group_param_type;

/**
   PKX Diffie-Hellman key length type.
 */
typedef uint16 secapi_pkx_dh_private_key_len_type;

/**
   PKX Diffie-Hellman peer public key type.
 */
typedef uint8* secapi_pkx_dh_peer_public_key_type;

/**
   PKX Diffie-Hellman parameter type.
 */
typedef union
{
  secapi_pkx_dh_group_param_type      group;
    /**< PKX DH group parameter. */
  secapi_pkx_dh_peer_public_key_type  peer_public_key_ptr;
    /**< PKX DH peer public key. */
  secapi_pkx_dh_private_key_len_type  private_key_len;
    /**< PKX DH private key length. */
} secapi_pkx_dh_param_type;

/** @} */ /* end_name_Key Exchange Interface Types and Definitions */

/**
  @name RSA Types and Definitions
  @{
*/

/**
 Key store parameters.
 */
typedef struct secapi_pkx_rsa_ks_params_s
{
  void*   handle;          /**< Key store handle. */
  uint16  key_size;        /**< Size of the key in key store. */
  boolean sign;            /**< Performs a sign operation. */
} secapi_pkx_rsa_ks_params_type;

/**
 Key store parameters.
 */
typedef struct
{
  uint8*  modulus_ptr;           /**< Modulus value. */
  uint16  modulus_size;          /**< Modulus size. */
  uint8*  private_key_ptr;       /**< Private key. */
  uint8*  public_key_ptr;        /**< Public key. */
  uint16  key_size;              /**< Key size. */
  uint8*  priv_key_iv_ptr;       /**< IV setting. */
  uint8*  priv_key_devkey_ptr;   /**< Devkey as private key. */
  uint16  priv_key_iv_size;      /**< IV size. */
} secapi_pkx_rsa_key_param_type;

/**
   RSA signal user.
 */
typedef struct
{
    secerrno_enum_type        *_errno;         /**< Error value. */
    uint8                     *sig_output;     /**< Output signal. */
    uint16                    *sig_output_len; /**< Output signal length. */
}secapi_pkx_rsa_isignal_usr_ptr;

/**
   RSA signal parameter.
 */
typedef struct secapi_pkx_rsa_isignal_param_s
{
    void                              *piSignal; /**< Signal pointer. */
    secapi_pkx_rsa_isignal_usr_ptr    *pusr_ptr; /**< User data pointer. */
}secapi_pkx_rsa_isignal_param_type;

/**
  RSA Encryption Encoding types. 
*/
typedef enum
{
  SECAPI_PKX_RSA_PADDING_ENC_PKCS1_V15 = 0,
    /**< Encoding method for encrypt/decrypt, using Public Key Cryptography 
         Standards 1 (PKCS1) method. */
  SECAPI_PKX_RSA_PADDING_ENC_OAEP,
    /**< Encoding method for encrypt/decrypt, using Optimal Asymmetric 
         Encryption Padding (OAEP) method. */
  SECAPI_PKX_RSA_PADDING_SIG_PKCS1_V15,
    /**< Encoding method for signature sign/verification, using PKCS1 method. */
  SECAPI_PKX_RSA_PADDING_SIG_PSS,
    /**< Encoding method for signature sign/verification, using Probabilistic 
         Signature Scheme(PSS) method. */
  SECAPI_PKX_NO_PADDING,
    /**< No padding for PKX. */
  SECAPI_PKX_PADDING_ENUM_RESERVED /**< Reserved. */
} secapi_pkx_padding_enum_type;


/**
   RSA parameter type.
 */
typedef union
{
  secapi_pkx_rsa_key_param_type      key;
    /**< PKX RSA key. */
  secapi_pkx_rsa_ks_params_type      ks_params;
    /**< PKX RSA KS parameter type. */
  secapi_pkx_rsa_isignal_param_type  isignal;
    /**< PKX RSA is signal type or not. */
} secapi_pkx_rsa_param_type;

/** ECC key information. 
*/
typedef struct {
    uint8*  public_key_x_ptr;      /**< Public x key. */
    uint8*  public_key_y_ptr;      /**< Public y key. */ 
    uint32  public_key_size;       /**< Public Key size. */
    uint8*  private_key_ptr;       /**< Private key. */
    uint32  private_key_size;      /**< Private key size. */
} secapi_ecc_key_param_type;
  
/** ECC curve parameters. 
*/
typedef struct secapi_ecc_curve_param_s {
    char*   m_ptr;  
    char*   a_ptr;  
    char*   b_ptr;
    char*   G_x_ptr;  
    char*   G_y_ptr;  
    char*   n_ptr;  
} secapi_ecc_curve_param_type;

/** ECC parameter type.
*/
typedef union {
    secapi_ecc_key_param_type      key;       
      /**< PKX ECC key type. */
    secapi_ecc_curve_param_type    c_params; 
      /**< PKX ECC curve parameter type. */
} secapi_ecc_param_type;

/** PKX parameter union data type.
 */
typedef union
{
  secapi_param_type             common; /**< Common parameter for PKX. */
  secapi_pkx_modexp_param_type  modexp; /**< Modulus and exponent value type. */
  secapi_pkx_dh_param_type      dh;     /**< DH parameter. */
  secapi_pkx_rsa_param_type     rsa;    /**< RSA parameter. */
  secapi_ecc_param_type         ecc;    /**< ECC parameter. */
} secapi_pkx_param_data_type;
/** @} */ /* end_name_RSA Types and Definitions */

/**
  @name Cipher Key Length and Block Length
  @{
 */
#define SECAPI_ENC_ARC4_KEY_BYTE_LEN   16 /**< ARC4 key length. */

#define SECAPI_ENC_AES_BLOCK_BYTE_LEN   16 /**< AES block length. */

#define SECAPI_ENC_DES_UNPACKED_KEY_BYTE_LEN    8 /**< DES unpacked key length. */
 
#define SECAPI_ENC_MAX_KEY_BYTE_LEN     (3*SECAPI_ENC_DES_UNPACKED_KEY_BYTE_LEN)  /**< Cipher maximum key length is 3 keys for 3DES. */

#define SECAPI_ENC_MAX_BLOCK_BYTE_LEN   (SECAPI_ENC_AES_BLOCK_BYTE_LEN)  /**< Cipher maximum block byte length. */

#define SECAPI_ENC_PAD_BLOCK_BYTE_LEN      255

#define SECAPI_ENC_DES_BLOCK_BYTE_LEN    8   /**< DES cipher block length. */
#define SECAPI_ENC_3DES_BLOCK_BYTE_LEN   8   /**< 3DES cipher block length. */
#define SECAPI_ENC_3DES_KEY_BYTE_LEN     24  /**< 3DES cipher key length. */
#define SECAPI_ENC_DES_KEY_BYTE_LEN      8   /**< DES cipher key length. */

#define SECAPI_ENC_AES128_BLOCK_BYTE_LEN  16 /**< AES128 block length and key length. */
#define SECAPI_ENC_AES128_KEY_BYTE_LEN    16 /**< AES128 key length. */
#define SECAPI_ENC_AES128_KEY_BYTE_LEN    16 /**< AES128 key length. */
#define SECAPI_ENC_AES256_KEY_BYTE_LEN    32 /**< AES256 key length. */

#define SECAPI_ENC_SNOW3G_KEY_SIZE        16  /**< SNOW3G cipher key size. */
#define SECAPI_ENC_SNOW3G_IV_SIZE         16  /**< SNOW3G cipher IV size. */
#define SECAPI_ENC_SNOW3G_LFSR_SIZE       16  /**< SNOW3G cipher linear feedback shift register size. */
#define SECAPI_ENC_SNOW3G_FSM_SIZE        3   /**< SNOW3G cipher finite state machine size. */

#define SECAPI_ENC_ZUC_KEY_SIZE           16  /**< ZUC cipher key size. */
#define SECAPI_ENC_ZUC_IV_SIZE            16  /**< ZUC cipher IV size. */

#define SECAPI_ENC_KASUMI_KEY_SIZE        16  /**< KASUMI cipher key size. */
#define SECAPI_ENC_KASUMI_IV_SIZE         16  /**< KASUMI cipher IV size. */

#define SECAPI_ENC_MAX_INPUT_BYTE_LENGTH  80000  /**< Maximum input byte for encryption. */

#define SECAPI_ENC_AES_IV_BLOCK_BYTE_LEN   (SECAPI_ENC_AES_BLOCK_BYTE_LEN)
  /**< Cipher maximum block length. */
#define SECAPI_ENC_DES_IV_BLOCK_BYTE_LEN   (SECAPI_ENC_DES_BLOCK_BYTE_LEN)
  /**< Cipher maximum block length. */
#define SECAPI_ENC_3DES_IV_BLOCK_BYTE_LEN  (SECAPI_ENC_DES_BLOCK_BYTE_LEN)
  /**< Cipher maximum block length. */

/** @} */  /* end_name_Cipher Key Length and Block Length */

/**
  Encryption/decryption operation enumerator.
 */
typedef enum
{
  SECAPI_ENC_CIPHER_ENCRYPT  = 0,   /**< Encryption operation, path, etc. */
  SECAPI_ENC_CIPHER_DECRYPT  = 1    /**< Decryption operation, path, etc. */
} secapi_enc_cipher_xform_type;

/**
  Cipher initialization vector parameter type.
*/
typedef secapi_param_enum_type secapi_enc_param_enum_type;

/**
  Cipher encryption intialization data parameter type.
 */
typedef struct
{
  struct
  {
    uint8*  key_ptr; /**< Key setting for encryption. */
    uint8*  iv_ptr;  /**< IV setting for encryption. */
  } enc;

  struct
  {
    uint8*  key_ptr;  /**< Key setting for decryption. */
    uint8*  iv_ptr;   /**< IV setting for decryption. */
  } dec;

} secapi_enc_init_data_param_type;

/**
 Cipher operation mode parameters type. 
*/
typedef enum
{
  SECAPI_ENC_OP_MODE_CBC         = 0,
    /**< Cipher Block Chaining (CBC) mode: The input depends on the output of the 
         proceeding block, or the initial vector. */
  SECAPI_ENC_OP_MODE_ECB         = 0x1,
    /**< Electronic Codebook (ECB) mode: Each block is encoded independently using 
         the same key. */
  SECAPI_ENC_OP_MODE_CTR         = 0x2,
    /**< Counter (CTR) mode: The application of the forward cipher to a set of 
         input blocks called counters to produce a sequence of output blocks 
         that are exclusive-ORed with the plaintext to produce the ciphertext. 
         The same process is applied to produce plaintext from ciphertext. */
  SECAPI_ENC_OP_MODE_CMAC        = 0x3    /**< AES CMAC mode authentication. */
} secapi_enc_op_mode_enum_type;

/**
  Cipher CBC state parameter type.
 */
typedef struct
{
  uint8*  enc_ptr; /**< Encryption state. */
  uint8*  dec_ptr; /**< Decryption state. */
} secapi_enc_cbc_state_param_type;

/**
  Cipher input mode enum type.
 */
typedef enum
{
  SECAPI_ENC_INPUT_MODE_CONTINUE  = 0,      /**< More data are coming. */
  SECAPI_ENC_INPUT_MODE_FINAL     = 1       /**< Last chunk of data. */
} secapi_enc_input_mode_enum_type;

/**
Padding type.
 */
typedef enum
{
  SECAPI_ENC_NO_PADDING = 0,
	/**< No padding is needed. */
  SECAPI_ENC_ZERO_PADDING,
	/**< Padding is filled with zero. */
  SECAPI_ENC_SEQUENTIAL_PADDING,
	/**< Padding byte starts from 1 and increased by 1. */
  SECAPI_ENC_RANDOM_PADDING,
	/**< Padded with random bytes, and last byte is the padding length. */
  SECAPI_ENC_STATIC_PADDING,
    /**< Padded with the value user sets, but the last byte is the padding 
	     length. */  
  SECAPI_ENC_LENGTH_VALUE_PADDING,
    /**< Padded all the bytes with (padding length - 1). */
  SECAPI_ENC_RFC2630_PADDING,
    /**< Padded all the bytes with the padding length. */
  SECAPI_ENC_MAX_PADDING_TYPE
	/**< Maximum padding type. */
} secapi_enc_padding_enum_type;

/**
Cipher padding parameter type.
 */
typedef struct
{
  secapi_enc_padding_enum_type  type;  /**< Cipher padding enum type. */
  uint8                         value; /**< Padding value if it is required by 
                                            a padding type. */
} secapi_enc_padding_param_type;

/**
  HMAC types, used per invocation.
 */
typedef enum
{
  SECAPI_HMAC_NONE = 0,          /**< HMAC type initial value. */
  SECAPI_HMAC_SSL_V3_TYPE,       /**< HMAC defined in SSL v3. */
  SECAPI_HMAC_TLS_10_TYPE,       /**< HMAC defined in TLS 1.0. */
  SECAPI_HMAC_TLS_11_TYPE,       /**< HMAC defined in TLS 1.1. */
  SECAPI_HMAC_TLS_12_TYPE,       /**< HMAC defined in TLS 1.2. */  
  SECAPI_HMAC_RFC2104_TYPE,      /**< HMAC defined in RFC2104. */
  SECAPI_HMAC_SNOW3G_TYPE,       /**< HMAC with snow3g algorithm. */ 
  SECAPI_CMAC_AES_TYPE,          /**< CMAC AES type. */
  SECAPI_HMAC_ZUC_TYPE,          /**< HMAC with ZUC algorithm. */ 
  SECAPI_HMAC_KASUMI_TYPE,       /**< HMAC with Kasumi algorithm. */ 
  SECAPI_HMAC_MAX_TYPE,          /**< Maximum HMAC value. */  
  SECAPI_HMAC_ENUM_RESERVED = 0x7FFFFFFF /**< Reserved enumerator. */
} secapi_hsh_hmac_enum_type;

/**
  Cipher IV synchronization parameter type.
 */
typedef enum
{
  SECAPI_ENC_IV_SYNC_OFF = 0, /**< &nbsp */
  SECAPI_ENC_IV_SYNC_ON       /**< &nbsp */
} secapi_enc_iv_sync_param_type;

/**
MAC information passed in per packet.
 */
typedef struct
{
  union
  {
    struct
    {
      uint8   record_type; /* Record type. */
      uint64  seq_no;      /* Sequence number. */
    } ssl;

  } mac_type; /* MAC type. */

} secapi_mac_pkt_info_type;  /* MAC type for SSL. */

/** MAC information parameter type.   
*/
typedef struct
{
  
  secapi_hsh_hmac_enum_type mode;       /**< MAC mode executing the hashing. */
  secapi_handle_type        mac_handle; /**< MAC handle executing the hashing. */

  union
  {
    struct
    {
      struct
      {
        uint8* mac_secret_ptr; /* Encryption secret key. */
      } enc;  /* MAC configuration information for SSL encryption. */                 

      struct
      {
        uint8* mac_secret_ptr; /* Decryption secret key. */
      } dec;  /* MAC configuration information for SSL decryption. */                 

    } ssl;  /* MAC configuration information for SSL. */

  } config; /**< MAC configuration. */

} secapi_mac_info_param_type;

/** Parameter type for SECAPI_ENC_CNT_WIDTH command.
*/
typedef uint32 secapi_enc_counter_width_type;

/** Bit length of the encryption input message. 
*/
typedef uint32 secapi_enc_input_bit_length_type;

/** Cipher parameter data type. 
*/
typedef union
{
  secapi_param_type                common;        /**< Common parameters for encryption. */
  secapi_enc_init_data_param_type  init_data;     /**< Initial data for encryption. */
  secapi_enc_op_mode_enum_type     op_mode;       /**< Cipher operation mode. */
  secapi_enc_cbc_state_param_type  cbc_state;     /**< Cipher CBC state parameter type. */
  secapi_enc_input_mode_enum_type  input_mode;    /**< Cipher input mode enum type. */
  secapi_enc_iv_sync_param_type    iv_sync;       /**< Cipher IV syncronization parameter. */
  secapi_enc_padding_param_type    padding;       /**< Padding type. */
  secapi_mac_info_param_type       mac_info;      /**< MAC information. */
  secapi_enc_counter_width_type	   counter_width; /**< Counter width in Counter mode. */
  secapi_enc_input_bit_length_type input_bit_len; /**< Cipher input data bit length. */
} secapi_enc_param_data_type;


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

/**
  Hash parameter enum type.
*/
typedef secapi_param_enum_type secapi_hsh_param_enum_type;

/**
  Hash mode enum type. If a hash is to be calculated over each packet of data 
  passed to the hashing routine, the application must specify the Atomic mode.
  If the hash is to be calculated over a sequence of n packets of data, the 
  application must specify the Continue mode for the first n-1 packets and 
  Final mode for the nth packet.
 */
typedef enum
{
  SECAPI_HSH_INPUT_MODE_ATOMIC    = 0, /**< Atomic mode for hash calculation. */
  SECAPI_HSH_INPUT_MODE_CONTINUE  = 1, /**< Continue mode for hash calculation. */
  SECAPI_HSH_INPUT_MODE_FINAL     = 2  /**< Final input mode for hash calculation. */
} secapi_hsh_mode_enum_type;

/**
  Hash initialization data parameter type.
 */
typedef struct
{
  uint8*  key_ptr;  /**< Key value setting. */
  uint8*  iv_ptr;   /**< IV value setting. */
} secapi_hsh_init_data_param_type;

/**
  Hash input bit length type.
 */
typedef uint32 secapi_hsh_input_bit_length_type;

/**
  Hash parameter data type.
 */
typedef union
{
  secapi_param_type          common;              /**< Common parameter for the hash algorithm. */
  secapi_hsh_mode_enum_type  mode;                /**< Hash input mode. */
  secapi_hsh_init_data_param_type  init_data;     /**< Hash initialization data. */
  secapi_hsh_input_bit_length_type input_bit_len; /**< Input data bit length for hash algorithm. */
} secapi_hsh_param_data_type;

/**
  @name Authentication Interface Types and Defintions
  @{
*/

#define SECAPI_ASN1_MAX_HASH_OBJ_LEN       19  /**< ASN1 maxmium hash object length. */
#define SECAPI_MAX_VRFY_SIG_LEN      \
        ( SECAPI_ASN1_MAX_HASH_OBJ_LEN + SECAPI_HSH_LARGEST_DIGEST_SIZE )
   /**< Maximum verification signature length. */
        
/** @} */ /* end_name_Authentication Interface Types and Defintions */
        
/**
  Specifies the random data generator entropy.
*/
typedef enum
{
  SECAPI_PSEUDO_RANDOM         = 0,  /**< Get pseudo random number from pool which is periodically filled with random number generated by PRNG HW */
  SECAPI_SECURE_RANDOM         = 1,  /**< Currently behavior is same as ECAPI_PSEUDO_RANDOM */
  SECAPI_SECURE_DIRECT_RANDOM  = 2,  /**< Get pseudo random number directly from PRNG HW  */
  SECAPI_PKCS_RANDOM           = 3,  /**< As above, with no zero bytes in stream. */
  SECAPI_TIME_CRITICAL_RANDOM  = 4   /**< Get pseudo random number from pool only if available otherwise returns an error */
} secapi_rnd_enum_type;

/**
  Defines the secutil random number buffer.
*/
typedef struct
{
  boolean  done;       /**< For compatibility with 1x. */
}secapi_rnd_type;

extern secapi_rnd_type secapi_rnd;


/**
Callback function pointer for SECAPI_GET_RANDOM.
*/
typedef void (*secapi_get_random_cb_fptr_type)( void* );

/** @} */  /** end_addtogroup crypto_secalgoapi */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/**
   @addtogroup crypto_secapi_config 
   @{
*/
/** @name Stream Functions
  @{ */

/**
  Creates a new crypto instance and returns the instance handle back to the 
  calling task.

  There is no callback function invocation upon the completion of this 
  operation.
 
  @param[out] handle_ptr  Pointer to the handle, set by the function.
  @param[in] algorithm    Algorithm of the handle.

  @return
  E_SUCCESS - Successful. \n
  E_NOT_ALLOWED - Running out of free space for allocating the handle. \n
  E_INVALID_ARG - Algorithm is not supported.

  @dependencies
  Any application using crypto handles must first use this function to get
  a handle.
*/
extern secerrno_enum_type secapi_new
(
  secapi_handle_type     *handle_ptr,
  secapi_algo_enum_type  algorithm
);

/**
  Deletes the crypto instance specified by the passed handle pointer and stops 
  any DSP operation associated with the passed handle. 

  There is no callback function invocation upon completion of this operation. 

  @param[in] handle  Handle for the instance to be freed.

  @return
  E_SUCCESS - Successful.
 
  @dependencies
  None.

  @sideeffects
  If the QDSP CRYPTO or Modular Math applet is performing an operation for
  the passed handle instance, the QDSP operation will be aborted.
*/
extern secerrno_enum_type secapi_delete
(
  secapi_handle_type*  handle
);  

/**
  Allows an application to register wait functions for all TCBs from which it 
  might be calling the CRYPTO API.

  A registration of a NULL wait function pointer will clear the entry for this 
  task, subsequently causing rex_wait() to be used.

  @param[in] tcb_ptr     Pointer to the TCB from which the function might 
                         be calling the CRYPTO API.  
  @param[in] (*wait_fptr)( rex_sigs_type ) Wait functions that call the CRYPTO 
                                           API.

  @return
  E_SUCCESS - Registration successful. \n
  E_ALREADY_DONE - Wait function is already registered for this TCB. \n
  E_INVALID_ARG - Called with a NULL fptr, and this TCB is not registered. \n
  E_NOT_ALLOWED - Registry is full. 

  @dependencies
  Only one wait function can be registered per TCB.
*/
secerrno_enum_type secapi_register_wait_func
(
  rex_tcb_type*  tcb_ptr,
  rex_sigs_type  (*wait_fptr)( rex_sigs_type )
);
/** @} */  /* end_name Stream Functions */
/** @} */  /* end_addtogroup_crypto_secapi_config */

/**
   @addtogroup crypto_secapi_pkx 
   @{
*/
/** @name Stream Functions
  @{ */

/**
  Sets specific Diffie-Hellman parameters for the DH instance specified by the 
  passed instance handle.

  @param[in] handle          Handle to the crypto instance.
  @param[in] algo            Algorithm.
  @param[in] param           Parameter to be set.
  @param[in] param_data_ptr  Pointer to the parameter-specific data.

  @return
  E_SUCCESS - Success.
  
  @dependencies
  None.
*/
extern secerrno_enum_type secapi_pkx_set_param
(
  secapi_handle_type             handle,
  secapi_algo_enum_type          algo,
  secapi_pkx_param_enum_type     param,
  secapi_pkx_param_data_type*    param_data_ptr
);

/**
  Creates a key using the Diffie-Hellman algorithm.

  The key created is dependent on the passed key type: Private, Public, or 
  Secret. The key is written to the passed address.

  This function uses previously stored values for the base, modulus, and key 
  information needed by the Diffie-Hellman algorithm. 

  @note 
  If none of these values have been set (via secapi_pkx_dh_set_param), 
  default values are used.

  @param[in] handle        handle to the crypto instance.
  @param[in] algo          Algorithm.
  @param[in] key           Type of key to create.
  @param[in] key_data_ptr  Pointer to the memory for the key generation result.
        
  @return
  Status of the request.
 
  @dependencies
  - The passed crypto instance must be initialized to perform the Diffie-Hellman
  algorithm. 
  - The maximum calculation time stored for Diffie-Hellman must have a non-zero
  value.

  @sideeffects
  If a private key is requested, the generated key will be stored as the 
  Diffie-Hellman key for the passed instance of Diffie-Hellman.
*/
extern secerrno_enum_type secapi_pkx_create_key
(
  secapi_handle_type           handle,
  secapi_algo_enum_type        algo,
  secapi_pkx_key_enum_type     key,
  uint8*                       key_data_ptr
);

/**
  Computes a basic modular math exponentiation.

  This function uses previously stored values for the base, exponent, and 
  modulus to compute a basic modular math exponentiation: \n
  @verbatim 
     (base ^ exponent) % modulus
  @endverbatim

  The result is the size of the modulus and is stored at the result_ptr
  parameter.

  @param[in] handle       Handle to the crypto instance.
  @param[in] result_ptr   Pointer to the memory for result.

  @return
  E_SUCCESS       - Successful. \n
  E_NOT_SUPPORTED - Option is currently not supported. \n
  E_FALIURE       - Failed to process the request.

  @dependencies
  The crypto instance must be properly initialized by calling secapi_new(), and 
  all the parameters must be set using secapi_pkx_set_param(). 
*/
extern secerrno_enum_type secapi_pkx_modexp
(
  secapi_handle_type           handle,
  uint8*                       result_ptr
);

/**
  Encrypts a message using the configured public RSA key. 

  @param[in] handle                  Handle to the crypto instance.
  @param[in] algo                    Algorithm.
  @param[in] padding_type            PKX cipher padding type.
  @param[in] plaintext_ptr           Pointer to the plaintext data (memory 
                                     provided by the caller).
  @param[in] plaintext_len           Pointer to the plaintext data length.
  @param[in] ciphertext_ptr          Pointer to the plaintext text data.
  @param[in,out] ciphertext_len_ptr  Length of the encrypted data (ciphertext).

  @return
  E_SUCCESS     - Function executes successfully. \n
  E_NOT_ALLOWED - One or more Diffie-Hellman parameters have not been set. \n
  E_IN_PROGRESS - CRYPTO-DSP interface was already busy. \n
  E_FAILURE     - Any other failures.

  @dependencies
  None.
*/
extern secerrno_enum_type secapi_pkx_encrypt
(
  secapi_handle_type             handle,
  secapi_algo_enum_type          algo,
  secapi_pkx_padding_enum_type   padding_type,
  uint8*                         plaintext_ptr,
  uint16                         plaintext_len,
  uint8*                         ciphertext_ptr,
  uint16*                        ciphertext_len_ptr
);

/**
  Decrypts a message using the configured private RSA key. 

  @param[in] handle                Handle to the crypto instance.
  @param[in] algo                  Algorithm. 
  @param[in] padding_type          PKX cipher padding type.
  @param[in] ciphertext_ptr        Pointer to the ciphertext data.
  @param[in] ciphertext_len        Length of the ciphertext data buffer.
  @param[in] plaintext_ptr         Pointer to the plaintext data (memory 
                                   provided by the caller).
  @param[in,out] plaintext_len_ptr Pointer to the plaintext data length.

  @return
  E_SUCCESS     - Function executes successfully. \n
  E_NOT_ALLOWED - One or more Diffie-Hellman parameters have not been set. \n
  E_IN_PROGRESS - CRYPTO-DSP interface was already busy. \n
  E_FAILURE     - Any other failures.

  @dependencies
  None.
*/
extern secerrno_enum_type secapi_pkx_decrypt
(
  secapi_handle_type            handle,
  secapi_algo_enum_type         algo,
  secapi_pkx_padding_enum_type  padding_type,
  uint8*                        ciphertext_ptr,
  uint16                        ciphertext_len,
  uint8*                        plaintext_ptr,
  uint16*                       plaintext_len_ptr
);

/**
  Signs a message using the configured private RSA key. 

  @param[in] handle                Handle to the crypto instance.
  @param[in] algo                  Algorithm. 
  @param[in] padding_type          PKX cipher padding type.
  @param[in] msg_hash_ptr          Pointer to the ciphertext data.
  @param[in] msg_hash_len          Length of the encrypted signature 
                                   (ciphertext).
  @param[out] signature_ptr        Pointer to the signature data
                                   (memory provided by the
                                   caller). For ECDSA mode ptr
                                   to sig r followed by sig s
  @param[out] signature_len_ptr    Pointer to the sig data 
                                   length. For ECDSA mode len of
                                   sig r + sig s

  @return
  E_SUCCESS     - Function executes successfully. \n
  E_NOT_ALLOWED - One or more Diffie-Hellman parameters have not been set. \n
  E_IN_PROGRESS - CRYPTO-DSP interface was already busy. \n
  E_FAILURE     - Any other failures.

  @dependencies
  None.
*/
extern secerrno_enum_type secapi_pkx_sign
(
  secapi_handle_type            handle,
  secapi_algo_enum_type         algo,
  secapi_pkx_padding_enum_type  padding_type,
  uint8*                        msg_hash_ptr,
  uint16                        msg_hash_len,
  uint8*                        signature_ptr,
  uint16*                       signature_len_ptr
);

/**
  Verifies an RSA signature by decrypting and decoding the signature. 

  @note 
  A comparison of the hash data is not done here. It should be done by the 
  calling function. \n
  The signature to be verified must conform to certain restrictions; 
  otherwise, an E_FAILURE is returned. 

  @param[in] handle               handle to the crypto instance. 
  @param[in] algo                 Algorithm. 
  @param[in] padding_type         PKX cipher padding type.
  @param[in] signature_ptr        Pointer to the signature data.
                                  For ECDSA mode ptr to sig r
                                  followed by sig s
  @param[in] signature_len        Length of the signature.
                                  For ECDSA mode len of sig r +
                                  sig s
  @param[in] msg_hash_ptr         Pointer to the hash (memory provided by the 
                                  caller).
  @param[in,out] msg_hash_len_ptr Pointer to the hash data length.

  @return
  E_SUCCESS     - Function executes successfully. \n
  E_NOT_ALLOWED - One or more Diffie-Hellman parameters have not been set. \n
  E_IN_PROGRESS - CRYPTO-DSP interface was already busy. \n
  E_FAILURE     - Any other failures.

  @dependencies
  None.
*/
extern secerrno_enum_type secapi_pkx_authenticate
(
  secapi_handle_type            handle,
  secapi_algo_enum_type         algo,
  secapi_pkx_padding_enum_type  padding_type,
  uint8*                        signature_ptr,
  uint16                        signature_len,
  uint8*                        msg_hash_ptr,
  uint16*                       msg_hash_len_ptr
);

/** @} */  /* end_name Stream Functions */
/** @} */  /* end_addtogroup_crypto_secapi_pkx */

/**
   @addtogroup crypto_secapi_hash 
   @{
*/
/** @name Stream Functions
  @{ 
*/

/**
  Sets the hashing parameters for the passed instance handle.

  There is no callback function invocation upon the completion this
  operation.

  @param[in] handle          Handle to the crypto instance.
  @param[in] algo            Hashing algorithm specifier.
  @param[in] param           Parameter to be set.
  @param[out] param_data_ptr Pointer to the parameter-specific data.

  @return
  E_SUCCESS - Request was successful. \n
  E_FAILURE - Failed to process the request.

  @dependencies
  None.
*/
extern secerrno_enum_type secapi_hsh_set_param
(
  secapi_handle_type           handle,
  secapi_algo_enum_type        algo,
  secapi_hsh_param_enum_type   param,
  secapi_hsh_param_data_type*  param_data_ptr
);
/*~ FUNCTION secapi_hsh_set_param */
/*~ PARAM IN param_data_ptr POINTER DISC param */

/**
  Creates a message digest using the algorithm specified.

  @param[in] handle_ptr      Handle to the crypto instance.
  @param[in] hash            Hashing algorithm specifier.
  @param[in] msg_data_ptr    Pointer to the message to be authenticated.
  @param[in] msg_byte_len    Length of the message in bytes.
  @param[out] msg_digest_ptr Pointer to the message digest (memory provided by 
                             the caller).

  @return
  E_SUCCESS    - Request was successful. \n
  E_IN_PROGRES - QDSP is in use. \n
  E_FAILURE    - Failed to process the request.

  @dependencies
  The hash mode must be properly set by calling secapi_hsh_set_param.
*/
extern secerrno_enum_type secapi_hsh_create_digest
(
  secapi_handle_type      handle_ptr,
  secapi_algo_enum_type   hash,
  uint8*                  msg_data_ptr,
  uint32                  msg_byte_len,
  uint8*                  msg_digest_ptr
);

/**
  Creates a message digest using the algorithm specified. 

  @param[in] handle            Handle to the crypto instance.
  @param[in] algo              Hashing algorithm specifier.
  @param[in] in_msg_vect       Pointer to the iovec struct containing the 
                               message to be authenticated.
  @param[out] *msg_digest_ptr  Pointer to the message digest (memory provided 
                               by the caller).

  @return 
  E_SUCCESS    - Request was successful. \n 
  E_IN_PROGRES - QDSP is in use. \n 
  E_FAILURE    - Failed to process the request.

  @dependencies
  The hash mode must be properly set by calling secapi_hsh_set_param. 
*/
extern secerrno_enum_type secapi_hsh_create_digest_iov
(
  secapi_handle_type      handle,
  secapi_algo_enum_type   algo,
  IOVEC_T*                in_msg_vect,
  uint8*                  msg_digest_ptr
);

/**
  Creates a message digest using the algorithm specified. 

  @param[in] handle          Handle to the crypto instance.
  @param[in] algo            Hashing algorithm specifier.
  @param[in] msg_ptr         Pointer to the DSM struct containing the 
                             message to be authenticated.
  @param[in] data_ptr        Pointer to the message to be authenticated 
                             (use msg_ptr or data_ptr).
  @param[in] msg_byte_len    Length of the message in bytes.
  @param[out] msg_digest_ptr Pointer to the DSM struct containing the message 
                             digest (memory provided by the caller).

  @return
  E_SUCCESS    - Request was successful. \n
  E_IN_PROGRES - QDSP is in use. \n
  E_FAILURE    - Failed to process the request.

  @dependencies
  The hash mode must be properly set by calling secapi_hsh_set_param.
*/
secerrno_enum_type secapi_hsh_create_digest_dsm
(
  secapi_handle_type    handle,
  secapi_algo_enum_type algo,
  dsm_item_type*        msg_ptr,
  uint8*                data_ptr,
  uint16                msg_byte_len,
  dsm_item_type**       msg_digest_ptr
);

/**
  Creates a hash MAC per RFC 2104 using the specified hash algorithm. 

  @param[in] handle           Handle to the crypto instance.
  @param[in] algo             Hashing algorithm specifier.
  @param[in] msg_ptr          Pointer to the message to be authenticated.
  @param[in] msg_len          Length of the message in bytes.
  @param[in] key_ptr          Pointer to the input key to the HMAC algorithm.
  @param[in] key_len          Length of the input key in bytes.
  @param[out] msg_digest_ptr  Pointer to the message digest (memory provided 
                              by the caller).

  @return
  E_SUCCESS    - Request was successful. \n
  E_IN_PROGRES - QDSP is in use. \n
  E_FAILURE    - Failed to process the request.

  @dependencies
  None.
*/
extern secerrno_enum_type secapi_hsh_create_hmac
(
  secapi_handle_type      handle,
  secapi_algo_enum_type   algo,
  uint8*                  msg_ptr,
  uint16                  msg_len,
  uint8*                  key_ptr,
  uint16                  key_len,
  uint8*                  msg_digest_ptr
);

/**
  Creates a hash MAC per RFC 2104 using the specified hash algorithm. 

  @param[in] handle          Handle to the crypto instance.
  @param[in] algo            Hashing algorithm specifier.
  @param[in] in_msg_vect     Pointer to the iovec struct containing the message 
                             to be authenticated.
  @param[in] in_key_vect     Pointer to the iovec struct containing the key for 
                             the HMAC algorithm.
  @param[out] msg_digest_ptr Pointer to the message digest (memory provided by 
                             the caller).

  @return 
  E_SUCCESS    - Request was successful. \n
  E_IN_PROGRES - QDSP is in use. \n
  E_FAILURE    - Failed to process the request.

  @dependencies
  None.
*/
extern secerrno_enum_type secapi_hsh_create_hmac_iov
(
  secapi_handle_type      handle,
  secapi_algo_enum_type   algo,
  IOVEC_T*                in_msg_vect,
  IOVEC_T*                in_key_vect,
  uint8*                  msg_digest_ptr
);

/**
  Creates a hash MAC per RFC 2104 using the specified hash algorithm.

  @param[in] handle          Handle to the crypto instance.
  @param[in] hmac_type       HMAC type.
  @param[in] algo            Hashing algorithm specifier.
  @param[in] inner_msg_ptr   Pointer to the DSM struct containing the inner 
                             message data.
  @param[in] inner_data_ptr  Pointer to the inner message data (use msg_ptr or 
                             data_ptr).
  @param[in] inner_byte_len  Byte length of the inner data.
  @param[in] outer_msg_ptr   Pointer to the DSM struct containing the outer 
                             message data.
  @param[in] outer_data_ptr  Pointer to the outer message data (use 
                             msg_ptr or data_ptr).
  @param[in] outer_byte_len  Byte length of the outer data.
  @param[in,out] hmac_ptr    Pointer to the DSM struct containing the HMAC data.

  @return
  E_SUCCESS    - Request was successful. \n
  E_IN_PROGRES - QDSP is in use. \n 
  E_FAILURE    - Failed to process the request.

  @dependencies
  None.
*/
secerrno_enum_type secapi_hsh_create_hmac_dsm
(
  secapi_handle_type         handle,
  secapi_hsh_hmac_enum_type  hmac_type,
  secapi_algo_enum_type      algo,
  dsm_item_type*             inner_msg_ptr,
  uint8*                     inner_data_ptr,
  uint16                     inner_byte_len,
  dsm_item_type*             outer_msg_ptr,
  uint8*                     outer_data_ptr,
  uint16                     outer_byte_len,
  dsm_item_type**            hmac_ptr
);
/** @} */  /* end_name Stream Functions */
/** @} */  /* end_addtogroup_crypto_secapi_hash*/

/**
   @addtogroup crypto_secapi_cipher 
   @{
*/
/** @name Stream Functions
   @{
*/

/**
  Sets the parameters for the specified encryption/decryption algorithm.

  @param[in] handle           Handle to the crypto instance.
  @param[in] algo             Cipher algorithm specifier.
  @param[in] param            Parameter to be set.
  @param[out] param_data_ptr  Pointer to the parameter-specific data.

  @return
  E_SUCCESS - Request was successful. \n
  E_FAILURE - Failed to process the request.

  @dependencies
  None.
*/
extern secerrno_enum_type secapi_enc_set_param
(
  secapi_handle_type           handle,
  secapi_algo_enum_type        algo,
  secapi_enc_param_enum_type   param,
  secapi_enc_param_data_type*  param_data_ptr
);

/**
  Sets the parameters for the DES encryption/decryption algorithm.

  There is no callback function invocation upon the completion of this 
  operation. 

  @param[in] handle           Handle to the crypto instance.
  @param[in] algo             Cipher algorithm specifier.
  @param[in] param            Parameter to get.
  @param[out] param_data_ptr  Pointer to the parameter-specific data.

  @return
  E_SUCCESS - Request was successful. \n
  E_FAILURE - Failed to process the request.
  
  @dependencies
  None.
*/
extern secerrno_enum_type secapi_enc_get_param
(
  secapi_handle_type           handle,
  secapi_algo_enum_type        algo,
  secapi_enc_param_enum_type   param,
  secapi_enc_param_data_type*  param_data_ptr
);

/**
  Encrypts the passed message using the specified algorithm.

  @param[in] handle                Handle to the crypto instance.
  @param[in] cipher                Cipher algorithm specifier.
  @param[in] in_msg_ptr            Pointer to the plaintext data.
  @param[in] in_msg_byte_len       Length of the plaintext in bytes.
  @param[in] out_msg_ptr           Pointer to the ciphertext data (memory 
                                   provided by the caller).
  @param[out] out_msg_byte_len_ptr Length of the ciphertext in bytes, set to 
                                   the length of the ciphertext by the function.

  @return
  E_SUCCESS - Successful completion. \n
  E_BAD_DATA - Data verification failed. \n
  E_NOT_SUPPORTED - Option is currently not supported. \n
  E_FAILURE - Any other failure to process the request.

  @dependencies
  - The memory allocated for the ciphertext must be large enough to hold the 
  plaintext equivalent and any cipher-specific padding. 
  - The cipher instance pointer must be properly initialized by secapi_new(). 
  - Cipher algorithm parameters must be properly set by secapi_enc_set_param(). 
*/
extern secerrno_enum_type secapi_enc_encrypt
(
  secapi_handle_type        handle,
  secapi_algo_enum_type     cipher,
  uint8*                    in_msg_ptr,
  uint16                    in_msg_byte_len,
  uint8*                    out_msg_ptr,
  uint16*                   out_msg_byte_len_ptr
);

/**
  Encrypts the passed message using the specified algorithm.

  @param[in] handle         Handle to the crypto instance.
  @param[in] algo           Cipher algorithm specifier.
  @param[in] in_msg_vect    Pointer to the iovec struct containing the 
                            plaintext data.
  @param[out] out_msg_vect  Pointer to the iovec struct containing the 
                            ciphertext data.

  @return 
  E_SUCCESS       - Successful completion. \n
  E_BAD_DATA      - Data verification failed. \n
  E_NOT_SUPPORTED - Option is currently not supported. \n 
  E_FAILURE       - Any other failure to process the request. 

  @dependencies
  - The memory allocated for the ciphertext must be large enough to
  hold the plaintext equivalent and any cipher-specific padding.  
  - Cipher instance ptr must be properly initialized by 
  secapi_new(). 
  - Cipher algorithm parameters must be properly set by 
  secapi_enc_set_param().
*/
extern secerrno_enum_type secapi_enc_encrypt_iov
(
  secapi_handle_type        handle,
  secapi_algo_enum_type     algo,
  IOVEC_T*                  in_msg_vect,
  IOVEC_T*                  out_msg_vect
);

/**
  Encrypts the passed message using the specified algorithm.

  @param[in] handle         Handle to the crypto instance.
  @param[in] algo           Cipher algorithm specifier.
  @param[in] in_msg_vect    Pointer to the iovec struct containing the 
                            plaintext data.
  @param[out] out_msg_vect  Pointer to the iovec struct containing the 
                            ciphertext data.
  @param[in] consume_input  Consume input if set to TRUE.
  @param[in] mac_pkt_info   Pointer to the MAC information passed in per packet.

  @return 
  E_SUCCESS       - Successful completion. \n
  E_BAD_DATA      - Data verification failed. \n
  E_NOT_SUPPORTED - Option is currently not supported. \n 
  E_FAILURE       - Any other failure to process the request. 

  @dependencies
  - The memory allocated for the ciphertext must be large enough to
  hold the plaintext equivalent and any cipher-specific padding.  
  - Cipher instance ptr must be properly initialized by 
  secapi_new(). 
  - Cipher algorithm parameters must be properly set by 
  secapi_enc_set_param().
*/
extern secerrno_enum_type secapi_enc_encrypt_iov_mac

(

  secapi_handle_type        handle,
  secapi_algo_enum_type     algo,
  IOVEC_T*                  in_msg_vect,
  IOVEC_T*                  out_msg_vect,
  boolean                   consume_input,
  secapi_mac_pkt_info_type* mac_pkt_info
);

/**
  Encrypts the passed message in the DSM chains memory using the specified 
  algorithm. 

  @note 
  The output of this function can be larger than the input, depending on the 
  algorithm padding requirements.  

  @param[in] handle         Handle to the crypto instance.
  @param[in] algo           Cipher algorithm specifier.
  @param[in] in_msg_ptr     Pointer to the DSM struct containing the plaintext 
                            data.
  @param[out] out_msg_ptr   Pointer to the DSM struct containing the ciphertext 
                            data (memory provided by the caller).
  @param[in] msg_byte_len   Length of the plaintext in bytes.
  @param[in] consume_input  Consume input if set to TRUE.
  @param[in] mac_pkt_info   Pointer to the MAC information passed in per packet.

  @return
  E_SUCCESS       - Successful completion. \n
  E_BAD_DATA      - Data verification failed. \n
  E_NOT_SUPPORTED - Option is currently not supported. \n 
  E_FAILURE       - Any other failure to process the request. 

  @dependencies
  - The memory allocated for the ciphertext must be large enough to hold the 
  plaintext equivalent and any cipher-specific padding.  
  - The cipher instance pointer must be properly initialized by secapi_new(). 
  - Cipher algorithm parameters must be properly set by secapi_enc_set_param().
*/
extern secerrno_enum_type secapi_enc_encrypt_dsm
(
  secapi_handle_type         handle,
  secapi_algo_enum_type      algo,
  dsm_item_type**            in_msg_ptr,
  dsm_item_type**            out_msg_ptr,
  uint16*                    msg_byte_len,
  boolean                    consume_input,
  secapi_mac_pkt_info_type*  mac_pkt_info
);

/**
  Decrypts the passed message in the DSM chain memory using the specified 
  algorithm. 

  @param[in] handle                   Handle to the crypto instance.
  @param[in] cipher                   Cipher algorithm specifier.
  @param[in] in_msg_ptr               Pointer to the ciphertext data.
  @param[in] in_msg_byte_len          Length of the ciphertext in bytes.
  @param[out] out_msg_ptr             Pointer to the plaintext data (memory 
                                      provided by the caller).
  @param[in,out] out_msg_byte_len_ptr Length of plaintext in bytes, set to the 
                                      length of the plaintext by function.

  @return
  E_SUCCESS       - Successful completion. \n
  E_BAD_DATA      - Data verification failed. \n
  E_NOT_SUPPORTED - Option is currently not supported. \n
  E_FAILURE       - Any other failure to process the request.

  @dependencies
  - This function must be called with a complete block ready for decryption if 
  the algorithm is a block cipher. 
  - The memory allocated for the ciphertext must be large enough to hold the 
  plaintext equivalent (padding is removed by the cipher).  
  - The cipher instance pointer must be properly initialized by secapi_new(). 
  - Cipher algorithm parameters must be properly set by secapi_enc_set_param(). 
*/
extern secerrno_enum_type secapi_enc_decrypt
(
  secapi_handle_type        handle,
  secapi_algo_enum_type     cipher,
  uint8*                    in_msg_ptr,
  uint16                    in_msg_byte_len,
  uint8*                    out_msg_ptr,
  uint16*                   out_msg_byte_len_ptr
);

/**
  Decrypts the passed message using the specified algorithm.

  @param[in] handle        Handle to the crypto instance.
  @param[in] algo          Cipher algorithm specifier.
  @param[in] in_msg_vect   Pointer to the iovec struct containing the 
                           ciphertext data.
  @param[out] out_msg_vect Pointer to the iovec struct containing the plaintext
                           data.

  @return 
  E_SUCCESS       - Successful completion. \n
  E_BAD_DATA      - Data verification failed. \n
  E_NOT_SUPPORTED - Option is currently not supported. \n 
  E_FAILURE       - Any other failure to process the request. 

  @dependencies
  - This function must be called with a complete block ready for decryption if 
  the algorithm is a block cipher. 
  - The memory allocated for the ciphertext must be large enough to hold the 
  plaintext equivalent (padding is removed by the cipher).  
  - The cipher instance pointer must be properly initialized by secapi_new().  
  - Cipher algorithm parameters must be properly set by secapi_enc_set_param(). 
*/
extern secerrno_enum_type secapi_enc_decrypt_iov
(
  secapi_handle_type        handle,
  secapi_algo_enum_type     algo,
  IOVEC_T*                  in_msg_vect,
  IOVEC_T*                  out_msg_vect
);

/**
  Decrypts the passed message using the specified algorithm.

  @param[in] handle          Handle to the crypto instance.
  @param[in] algo            Cipher algorithm specifier.
  @param[in] in_msg_vect   Pointer to the iovec struct containing the 
                           ciphertext data.
  @param[out] out_msg_vect Pointer to the iovec struct containing the plaintext
                           data.
  @param[in] consume_input Consume input if set to TRUE.
  @param[in] mac_pkt_info  Pointer to the MAC information passed in per packet.

  @return 
  E_SUCCESS       - Successful completion. \n
  E_BAD_DATA      - Data verification failed. \n
  E_NOT_SUPPORTED - Option is currently not supported. \n 
  E_FAILURE       - Any other failure to process the request. 
   
  @dependencies
  - This function must be called with a complete block ready for decryption if 
  the algorithm is a block cipher. 
  - The memory allocated for the ciphertext must be large enough to hold the 
  plaintext equivalent (padding is removed by the cipher).  
  - The cipher instance pointer must be properly initialized by secapi_new().  
  - Cipher algorithm parameters must be properly set by secapi_enc_set_param(). 
*/
extern secerrno_enum_type secapi_enc_decrypt_iov_mac
(
  secapi_handle_type        handle,
  secapi_algo_enum_type     algo,
  IOVEC_T*                  in_msg_vect,
  IOVEC_T*                  out_msg_vect,
  boolean                   consume_input,
  secapi_mac_pkt_info_type* mac_pkt_info
);

/**
  Decrypts the passed message using the specified algorithm.

  @param[in] handle          Handle to the crypto instance.
  @param[in] algo            Cipher algorithm specifier.
  @param[in] in_msg_ptr      Pointer to the DSM struct containing the 
                             ciphertext data.
  @param[out] out_msg_ptr    Pointer to the DSM struct containing the 
                             plaintext data (memory provided by the caller).
  @param[out] msg_byte_len   Length of ciphertext in bytes.
  @param[in] consume_input   Consume input if set to TRUE.
  @param[in] mac_pkt_info    Pointer to the MAC information passed in per packet.
    
  @return
  E_SUCCESS       - Successful completion. \n
  E_BAD_DATA      - Data verification failed. \n
  E_NOT_SUPPORTED - Option is currently not supported. \n 
  E_FAILURE       - Any other failure to process the request.

  @dependencies
  - This function must be called with a complete block ready for decryption if 
  the algorithm is a block cipher. 
  - The memory allocated for the ciphertext must be large enough to hold the 
  plaintext equivalent (padding is removed by the cipher).  
  - The cipher instance pointer must be properly initialized by secapi_new(). 
  - Cipher algorithm parameters must be properly set by secapi_enc_set_param(). 
*/
secerrno_enum_type secapi_enc_decrypt_dsm
(
  secapi_handle_type         handle,
  secapi_algo_enum_type      algo,
  dsm_item_type**            in_msg_ptr,
  dsm_item_type**            out_msg_ptr,
  uint16*                    msg_byte_len,
  boolean                    consume_input,
  secapi_mac_pkt_info_type*  mac_pkt_info
);

/**
  Calculates the maximum data size that must be allocated by the caller of 
  an operation involving padding or MAC calculations.

  It returns the maximum number of bytes to allocate for the result (0 if the 
  handle is not completely configured). 
 
  @param[in] handle           Handle to use the instance.
  @param[in] algo             Cipher algorithm specifier.
  @param[in] in_msg_len       Length of data to be processed.
  @param[out] padded_len_ptr  Calculated length for padding and MAC.

  @return
  Security services error code.
  
  @dependencies
  The specified handle must be fully configured for the intended operation.
*/
extern secerrno_enum_type secapi_enc_get_padded_length
(
  secapi_handle_type       handle,
  secapi_algo_enum_type    algo,
  uint16                   in_msg_len,
  uint16                   *padded_len_ptr
);
/** @} */  /* end_name Stream Functions */
/** @} */ /* end_addtogroup_crypto_secapi_cipher */


/**
   @addtogroup crypto_secapi_rng 
   @{
*/
/** @name Stream Functions
  @{ 
*/

/**
  Fetches random data from its internal random buffer.

  The random buffer is either filled with noise-level quantities from searcher 
  measurements or with HMAC pseudo random data.
 
  The required randomness is specified as follows (increasing entropy): \n
  - SECAPI_PSEUDO_RANDOM - Pseudo random data generated by the ARM.
  - SECAPI_SECURE_RANDOM -  Random data enhanced for maximum entropy (even 
    distributions of 0 and 1 bits), suitable for commercial and cryptographic 
    applications.
 
  @param[in] rnd_data_type  Required randomness of random data.
  @param[out] rnd_data_ptr  Pointer to an array that will be filled with the 
                            random data.
  @param[in] rnd_data_len   Length of data requested (in bytes).

  @return
  E_SUCCESS - Successful completion. \n
  E_AGAIN   - Not enough random data has been collected since the last call.
  E_NOT_ALLOWED - if this API used before random task is initialized.

  @dependencies
  Pseudo random data -  None. \n
  Secure random data -  Random data plus access to hash algorithms (runtime on 
                        the application DSP).
*/
extern secerrno_enum_type secapi_get_random
(
  secapi_rnd_enum_type  rnd_data_type,
  uint8*                rnd_data_ptr,
  uint16                rnd_data_len
);

/**
  Registers a callback that allows the SECRND task to notify the calling 
  task when random data is available.

  In several different scenarios, a task may require random numbers before the 
  SECRND task has been initialized. In such cases, the call to 
  secapi_get_random() returns E_AGAIN. It is the calling task's 
  responsibility to register a callback. This allows the SECRND task to 
  notify the calling task when random data is available. Since the callback 
  function is fired from the SECRND task context, users should not execute 
  any blocking operations that can cause the watchdog to time out. 

  The calling task should not call secapi_get_random() in the callback function.
  
  @param[in] get_random_cb User-defined callback function.
  @param[in] usr_data      User-defined data (can be NULL).

  @return
  E_SUCCESS     - Successful. \n
  E_INVALID_ARG - Callback function pointer is null. \n
  E_NO_MEMORY   - Reached the maximum number of callback function pointers that can be registered. \n 
  E_FAILURE     - SECRND task has already been initialized.

  @dependencies
  None.
*/
extern secerrno_enum_type secapi_get_random_set_cb
(
  secapi_get_random_cb_fptr_type  get_random_cb,
  void*                           usr_data
);

/**
  Registers a callback that allows the SECRND task to notify the calling 
  task when random data needs to be generated.

  This callback functon is connected to a SECRND task timer. When this CB
  function is called random data in stored generated and stored 
  in the random_data_buff. 
  
  None 

  @return
  E_SUCCESS     - Successful. \n

  @dependencies
  None.
*/
//Secapi random fill buffer call back 
extern secerrno_enum_type secapi_fill_random_buffer_cb(void);

/** @} */  /* end_name Stream Functions */
/** @} */  /* end_addtogroup_crypto_secapi_rng */

/*!
   @addtogroup crypto_secutilapi 
   @{
*   This section explains the Cryptographic Utility Library
*   provided by secapi_util.h, secutil.h and secutil_v.h.
*/
/*!
  @brief
  This function gets random data from the searcher task and merges it into
  the random data buffer storage area.

  @details
  This function gets random data from the searcher task and merges it into
  the random data buffer storage area.

  @param[in,out] rnd_data_ptr - Pointer to collected random data
  @param[in] rnd_data_len - Length of collected random data in bytes

  @dependencies
  The task calling this function is assumed to have higher priority
  than the tasks using the random data. 
  
  @return    
  none

  @sideeffects    
  Updates the random data buffer storage area.
  
*/

void secapi_fill_rnd_data
(
  uint8*  rnd_data_ptr,
  uint16  rnd_data_len
);

/*!
  @brief
  This function enables the PRNG clock.

  @details
  This function enables the PRNG clock.

  @dependencies
  None
  
  @return    
  E_SUCCESS     - Successful. \n

  @sideeffects    
  None
  
*/

secerrno_enum_type secapi_prng_clk_enable(void);

/*!
  @brief
  This function disables the PRNG clock.

  @details
  This function disables the PRNG clock.

  @dependencies
  None
  
  @return    
  E_SUCCESS     - Successful. \n

  @sideeffects    
  None   

 */

secerrno_enum_type secapi_prng_clk_disable(void);

/** @} */  /* end_addtogroup_crypto_secutilapi */

/*!
   @addtogroup crypto_securechannel
   @{
*   This section explains the secure channel APIs 
*/
/**
  @brief
  This function is used to initialize the keys at modem side.

  @param
  None

  @dependencies
  None
  
  @return    
  E_SUCCESS      - Successful. 
  E_ALREADY_DONE - Secure channel is already established.

  @sideeffects    
  None.
  
*/

secerrno_enum_type secapi_secure_channel_init
(
 void
);  

typedef enum
{
  SC_SSID_TZ=0,                  /* Trustzone.                                  */ 
  SC_SSID_MPSS,                  /* Modem subsystem.                            */
  SC_SSID_LPASS,                 /* Low power audio subsystem.                  */
  SC_SSID_WCNSS,                 /* Wireless conectivity subsystem.             */
  SC_SSID_COUNT,                 /* Number of supported SS for SC.              */
  SC_SSID_UNSPECIFIED,           /* Client dont want to speficy SSID            */
  SC_UNSUPPORTED_SS=0x7F         /* Subsystem not supported.                    */
} secapi_sc_ss_e_type;

typedef enum
{
  SC_CID_SSM=0,                  /* Trustzone.                                  */ 
  SC_CID_UIM,                    /* Modem subsystem.                            */
  SC_CID_DATA_QMI,               /* Qmi client inside modem                     */
  SC_CID_TLOC,                   /* Trusted Location                            */  
  SC_CID_COUNT,                  /* Number of supported CIDs for SC.            */
  SC_UNSUPPORTED_CID=0x7F        /* Subsystem not supported.                    */
} secapi_sc_cid_e_type;

/** 
  @brief 
  This is used to secure the the input message
   
  @details
  The output buffer should be large enough to hold the encrypted message 
  and some internal headers and possible padding. Recommended output buffer size 
  is atleast input_msg_len + 100 bytes. The memory has to be managed by the caller.
   
  @param[in] ssid                    destination Sub-system ID 
  @param[in] cid                     Client ID
  @param[in] input_msg_ptr           Pointer to the plaintext data.
  @param[in] input_msg_len           Length of the plaintext in bytes.
  @param[out] output_msg_ptr         Pointer to the buffer to hold the secure blob 
                                     (memory provided by the caller)
  @param[in,out] output_msg_len_ptr  Size of the above buffer in bytes, set to 
                                     the length of the secure blob on return

  @return
  E_SUCCESS - Successful.  \n 
  E_FAILURE - Operation failed. 
 
  @dependencies
  Secure Channel has to be established successfully.

  @sideeffects
  None
*/
secerrno_enum_type secapi_secure_message
(
  secapi_sc_ss_e_type  ssid,
  secapi_sc_cid_e_type cid,
  const uint8*  input_msg_ptr,
  uint32        input_msg_len,
  uint8*        output_msg_ptr,
  uint32*       output_msg_len_ptr
);  

/** 
  @brief 
  This is used to authenticate and decrypt the secure blob
   
  @details 
  The output buffer should be large enough to hold the decrypted message 
  Recommended output buffer size is atleast input_msg_len. 
  The memory has to be managed by the caller.
   
  @param[in] ssid                    Source Sub-system ID 
  @param[in] cid                     Client ID
  @param[in] input_msg_ptr           Pointer to the secure blob.
  @param[in] input_msg_len           Length of the secure blob in bytes.
  @param[out] output_msg_ptr         Pointer to the buffer to hold the decrypted data 
                                     (memory provided by the caller)
  @param[in,out] output_msg_len_ptr  Size of the above buffer in bytes, set to 
                                     the length of the decrypted data on return.

  @return
  E_SUCCESS - Successful.  \n 
  E_FAILURE - Operation failed. 
 
  @dependencies
  Secure Channel has to be established successfully.

  @sideeffects
  None
*/
secerrno_enum_type secapi_authenticate_decrypt_message
(
  secapi_sc_ss_e_type  ssid,
  secapi_sc_cid_e_type cid,
  const uint8*  input_msg_ptr,
  uint32        input_msg_len,
  uint8*        output_msg_ptr,
  uint32*       output_msg_len_ptr
);  

/** @} */  /* end_addtogroup_crypto_securechannel */

#endif /* SECAPI_H */



