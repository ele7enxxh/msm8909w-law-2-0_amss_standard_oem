#ifndef SECCRYPT_H
#define SECCRYPT_H

/** 
  @file seccrypt.h
  @brief Cryptographic Algorithm support API in DSM chain ARM-QDSP2.
  This file contains the definitions of the constants, data structures, and 
  interfaces of the ARM-QDSP2 Cryptographic Library. It defines the 
  external interfaces for key exchange, message digest, and encrypt/decrypt 
  for various algorithms. 
*/

/*===========================================================================

                      S E C U R I T Y    S E R V I C E S
                
                 C R Y P T O G R A P H I C    I N T E R F A C E

                        H E A D E R    F I L E

DESCRIPTION
  This is the header file for the ARM-QDSP2 Cryptographic Interface. It
  defines the external interfaces for key exchange, message digest, and
  encrypt/decrypt for various algorithms. Those applications who want to
  use CRYPTO services should include this header file.

  NOTE: Applications outside of DMSS shall use the API defined
        by secapi.h !


EXTERNALIZED FUNCTIONS

    seccrypt_new()               - create a new CRYPTO handle
    seccrypt_delete()            - delete a CRYPTO handle
    seccrypt_pkx_dh_set_param()  - set the parameters for DH handle
    seccrypt_pkx_dh_create_key() - create the keys using the DH algorithm
    seccrypt_pkx_rsa_set_param() - set the parameters for an RSA handle
    seccrypt_pkx_rsa_public_encrypt()   - encrypt data using RSA
    seccrypt_pkx_rsa_verify_signature() - decrypt data using RSA
    seccrypt_pkx_abort()         - abort the current key generation process
    seccrypt_hsh_set_param()     - set the parameters for a hash handle
    seccrypt_hsh_create_digest() - create a message digest
    seccrypt_hsh_create_hmac()   - create an HMAC digest
    seccrypt_enc_set_param()     - set the parameters for a cipher
    seccrypt_enc_get_param()     - get the parameters for a cipher
    seccrypt_enc_encrypt()       - encrypt using a cipher
    seccrypt_enc_decrypt()       - decrypt using a cipher

INITIALIZATION AND SEQUENCING REQUIREMENTS

  seccrypt_init() has to be called before using any of the other routines.
  This is done at task startup automatically.

===========================================================================*/

/*===========================================================================
  Copyright (c) 2003 - 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/crypto/seccrypt.h#1 $ 
  $DateTime: 2016/12/13 07:59:23 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/10   dm      Crypto API files rearchitecture
06/24/10   sd      (Tech Pubs) Edited Doxygen markup and comments.
06/06/10   qxu     Added mainpage, dscriptions for functions, constants and data types
                   to doxygenate API document.
08/28/09   ejt     Removed #include "customer.h" and FEATURE flags for CMI compliance
08/26/09   ejt     Removed #include "seccrypt_v.h" for CMI compliance
06/10/09   ejt     Removed FEATURE_SEC_IPC
01/07/09   ejt     Added PLATFORM_LTK conditional compile
05/17/07   rv      Added support for IPubKey using CS implementatiion
04/17/07   rv      Added support for SHA256 using CS implementation
08/31/05   df      Changed iovec filename to seciovec
                   Changes to support IOVEC
03/21/05   sb      Added support for Key Store API, Change to pkx_authenticate API
02/25/05   sb      Added support for IPC based SEC, Rsa signing, MSM device key based DSP commands
06/08/04   jay     Added RFC2630_PADDING
02/18/04   rwh     Added modmath parameter size definitions.
11/19/03   yhw     Added AES128 definitions
11/26/03   rwh     Added seccrypt_enc_get_padded_length().
10/21/03   jay     Removed private key parameter for DH.
09/04/03   om      Added private key parameter for DH.
06/04/03   om      Added private key length for DH.
04/04/03   om      Created module.

===========================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "rex.h"
#include "dsm.h"
#include "secerrno.h"
#include "secapi.h"


/*===========================================================================

                        PUBLIC DATA DECLARATIONS

===========================================================================*/
/** @addtogroup crypto_secalgoapi
  @{ */

/** Handle used by DMSS-internal users.
*/
typedef void* seccrypt_handle_type;

/*===========================================================================

                     ALGORITHM PARAMETER DECLARATIONS

===========================================================================*/

/**
  @name Common Parameter Definitions
  @{
  The most significant byte contains the common parameters. 
*/

#define SECCRYPT_EXEC_MODE_PARAM_MASK       0x01000000
#define SECCRYPT_EXEC_TIMEOUT_PARAM_MASK    0x02000000
#define SECCRYPT_EXEC_PLATFORM_PARAM_MASK   0x04000000
#define SECCRYPT_ASYNC_CALLBACK_PARAM_MASK  0x08000000
/** @} */  /* end_name_Common Parameter Definitions */

/** 
Parameter types for all instances.
*/
typedef enum {

    SECCRYPT_EXEC_MODE = 0,            
      /**< Common parameter type: Execution mode.  */
    SECCRYPT_EXEC_TIMEOUT = 1,         
      /**< Common parameter type: Timeout value. */
    SECCRYPT_EXEC_PLATFORM = 2,        
      /**< Common parameter type: Platform setting. */
    SECCRYPT_ASYNC_CALLBACK = 3,       
      /**< Common parameter type: Call function setting in asynchronized mode. */
    SECCRYPT_DATA_GET_FUNC = 4,        
      /**< Common parameter type: Get data function. */
    SECCRYPT_MAX_COMMON_PARAM = 5,     
      /**< Common parameter type: maximum common parameter type number. */
    SECCRYPT_PKX_PARAM_START = 0x1000, 
      /**< PKX-specific parameter: Start value. */
	SECCRYPT_ENC_PARAM_START = 0x2000, 
      /**< ENC-specific parameter: Start value. */
	SECCRYPT_HSH_PARAM_START = 0x3000, 
      /**< HSH-specific parameter: Start value. */

    SECPKX_MODEXP = SECCRYPT_PKX_PARAM_START, 
      /**< PKX-specific parameter: Modulus/exponent value setting. */
	SECPKX_DH_GROUP,                          
      /**< PKX-specific parameter: DH group. */
    SECPKX_DH_PRIVATE_KEY_LEN,                
      /**< PKX-specific parameter: DH private key length. */
    SECPKX_DH_PEER_PUBLIC_KEY,                
      /**< PKX-specific parameter: DH peer public key. */
    SECPKX_RSA_MODULUS,                       
      /**< PKX-specific parameter: RSA modulus value. */
    SECPKX_RSA_PRIVATE_KEY,                   
      /**< PKX-specific parameter: RSA private key. */
	SECPKX_RSA_PUBLIC_KEY,                    
      /**< PKX-specific parameter: RSA public key. */
    SECPKX_RSA_PRIVATE_KEY_IV,                
      /**< PKX-specific parameter: RSA private key&IV setting. */
    SECPKX_MODEXP_PRIV_KEY_IV,                
      /**< PKX-specific parameter: Modulus exponent private key and IV setting. */
    SECPKX_RSA_KS_PARAMS,                     
      /**< PKX-specific parameter: RSA KS parameter. */
    SECPKX_RSA_ISIGNAL,                       
      /**< PKX-specific parameter: RSA is signal, or not.  */
    SECPKX_ECC_CURVE_PARAMS,
      /**< PKX-specific parameter: ECC curve parameters.  */
    SECPKX_ECC_PUBLIC_KEY,
      /**< PKX-specific parameter: ECC Public Key.  */
    SECPKX_ECC_PRIVATE_KEY,
      /**< PKX-specific parameter: ECC Private Key.   */
    SECPKX_MAX_PARAM,                         
      /**< PKX-specific parameter: maximum parameter.  */

    SECENC_INIT_DATA = SECCRYPT_ENC_PARAM_START, 
      /**< Cipher-specific parameter: Key and IV setting. */
    SECENC_OP_MODE,                              
      /**< Cipher-specific parameter: Operation mode. */ 
    SECENC_CBC_STATE,                            
      /**< Cipher-specific parameter: CBC state. */
    SECENC_INPUT_MODE,                           
      /**< Cipher-specific parameter: Input mode. */
    SECENC_IV_SYNC,                              
      /**< Cipher-specific parameter: IV synchronization. */
    SECENC_MAC_INFO,                             
      /**< Cipher-specific parameter: MAC information. */
    SECENC_PADDING_TYPE,                         
      /**< Cipher-specific parameter: Padding type. */
    SECENC_DEVKEY,                               
      /**< Cipher-specific parameter: SFS devkey or not. */  
    SECENC_COUNTER_WIDTH,                        
      /**< Cipher-specific parameter: Counter width if cipher is in AES CTR mode. */  	
    SECENC_INPUT_BIT_LENGTH,                     
      /**< Cipher-specific parameter: Input data buffer bit length. */
    SECENC_MAX_PARAM,                            


    SECHSH_HASH_MODE = SECCRYPT_HSH_PARAM_START, 
      /**< Hash-specific parameters: Input mode. */
    SECHSH_DEVKEY,                               
      /**< Hash-specific parameters: SFS devkey, or not. */        
    SECHSH_HIPRI,                                
      /**< Hash-specific parameters: High priority, or not.  */
    SECHSH_INIT_DATA,                            
      /**< Hash-specific parameters: Initialization data. */
    SECHSH_INPUT_BIT_LENGTH,                                                
      /**< Hash-specific parameters: Input data buffer bit length. */
    SECHSH_MAX_PARAM,                            
      /**< Hash-specific parameters. Maximum parameter value. */
    SECCRYPT_PARAM_ENUM_RESERVED = 0x7FFFFFFF    
      /**< Hash-specific parameters: Reserved. */

} seccrypt_param_enum_type;

/** Maximum execution time definition. 
*/
typedef uint32 seccrypt_exec_timeout_param_data_type;

/** Execution platform definitions.\ Do not change. 
  Used as an index into the state change function pointer tables 
*/
typedef enum {

    SECCRYPT_EXEC_PLATFORM_ARM = 0, 
	  /**< Execution platform: ARM platform. */
    SECCRYPT_EXEC_PLATFORM_DSP = 1, 
	  /**< Execution platform: DSP platform. */
    SECCRYPT_EXEC_PLATFORM_HW  = 2, 
	  /**< Execution platform: HW CRYPTO Engine platform. */
    SECCRYPT_EXEC_PLATFORM_SIM = 3, 
	  /**< Execution platform: SIM platform. */
    SECCRYPT_EXEC_NUM_PLATFORM = 4, 
	  /**< Execution platform: Maximum platform number. */

    SECCRYPT_EXEC_PLATFORM_ACCELERATOR_PREFERRED = SECCRYPT_EXEC_NUM_PLATFORM,
      /**< Default setting for all instances. */
    SECCRYPT_EXEC_UNKNOWN_PLATFORM, 
	  /**< Execution platform: Unknown platform. */

    SECCRYPT_EXEC_MAX_PLATFORM      
	/**< Execution platform: Maximum platform number. */

} seccrypt_platform_enum_type;

/** Execution platform type. 
*/
typedef seccrypt_platform_enum_type seccrypt_exec_platform_param_data_type;

/** Asynchronous execution callback registration. 
*/
typedef struct {
    secerrno_cb_fptr_type  cb_fptr;  /**< Callback function pointer. */
    void*                  usr_ptr;  /**< User data pointer. */
} seccrypt_async_exec_cb_param_data_type;

/** Union for all common parameters. 
*/
typedef union {
    secapi_exec_mode_enum_type              exec_mode;     
      /**< Execution mode. */
    seccrypt_exec_timeout_param_data_type   exec_timeout;  
      /**< Execution timeout. */
    seccrypt_exec_platform_param_data_type  exec_platform; 
      /**< Execution platform. */
    seccrypt_async_exec_cb_param_data_type  async_cb;      
      /**< Callback function when in asynchronous mode. */
    secapi_data_get_func_param_data_type    data_get;      
      /**< Gets the data. */
} seccrypt_param_type;

/*---------------------------------------------------------------------------
  PKX Parameter Definitions
---------------------------------------------------------------------------*/


#define SECCRYPT_PKX_MAX_MOD_SIZE   SECAPI_PKX_MAX_KEY_SIZE   
  /**< Maximum supported operand byte size for modular math. */
#define SECCRYPT_PKX_MAX_EXP_SIZE   SECAPI_PKX_MAX_KEY_SIZE   
  /**< Maximum supported operand byte size for modular math. */
#define SECCRYPT_PKX_MAX_BASE_SIZE  SECCRYPT_PKX_MAX_MOD_SIZE 
  /**< Maximum supported operand byte size for modular math. */

/** Parameter types for the PKX module (modulo exponentiation). 
*/
typedef seccrypt_param_enum_type secpkx_param_enum_type;

/** Generic modulo exponentiation. 
*/
typedef struct {
    uint8*  base_ptr;            /**< PKX base value. */
    uint16  base_size;           /**< PKX base size. */
    uint8*  modulus_ptr;         /**< PKX modulus value. */
    uint16  modulus_size;        /**< PKX modulus size. */
    uint8*  exponent_ptr;        /**< PKX exponent value. */
    uint16  exponent_size;       /**< PKX exponent size. */
    uint8*  priv_key_iv_ptr;     /**< Private key IV value. */
    uint8*  priv_key_devkey_ptr; /**< Private key devkey value. */
    uint16  priv_key_iv_size;    /**< Private key IV size. */
} secpkx_modexp_param_type;

/** Diffie-Hellman group information. 
*/
typedef struct {
    uint8*  base_ptr;           /**< Base value pointer. */
    uint16  base_size;          /**< Base value size. */
    uint8*  modulus_ptr;        /**< Modulus value pointer. */
    uint16  modulus_size;       /**< Modulus value size. */
} secpkx_dh_group_param_type; 

/** PKX Diffie-Helman private key length type. 
*/
typedef uint16 secpkx_dh_private_key_len_type; 

/** PKX Diffie-Helman peer private key type. 
*/
typedef uint8* secpkx_dh_private_key_type;     

/** PKX Diffie-Helman peer public key type. 
*/
typedef uint8* secpkx_dh_peer_public_key_type; 

/**
  PKX Diffie-Helman parameter type.
*/
typedef union {
    secpkx_dh_group_param_type      group;               
      /**< PKX DH group parameter. */
	secpkx_dh_private_key_len_type  private_key_len;     
      /**< PKX DH peer private key length. */
    secpkx_dh_private_key_type      private_key_ptr;     
      /**< PKX DH peer private key type. */
    secpkx_dh_peer_public_key_type  peer_public_key_ptr; 
      /**< PKX DH peer public key. */
} secpkx_dh_param_type;

/** Key store parameters. 
*/
typedef struct secpkx_rsa_ks_params_s {
    void*   handle;          /**< Key store handle. */
    uint16  key_size;        /**< Size of the key in key store. */
    boolean sign;            /**< Will perform a sign operation. */
} secpkx_rsa_ks_params_type;

/** RSA information. 
*/
typedef struct {
    uint8*  modulus_ptr;         /**< Modulus value. */
    uint16  modulus_size;        /**< Modulus size. */
    uint8*  private_key_ptr;     /**< Private key. */
    uint8*  public_key_ptr;      /**< Public key. */
    uint16  key_size;            /**< Key size. */
    uint8*  priv_key_iv_ptr;     /**< IV setting. */
    uint8*  priv_key_devkey_ptr; /**< Devkey as private key. */
    uint16  priv_key_iv_size;    /**< IV size. */
} secpkx_rsa_key_param_type;   

/** RSA signal user.
*/
typedef struct {
    secerrno_enum_type      *_errno;         /**< Error value. */
    uint8                   *sig_output;     /**< Output signal. */
    uint16                  *sig_output_len; /**< Output signal length. */
}secpkx_rsa_isignal_usr_ptr;

/** RSA signal parameter.
*/
typedef struct secpkx_rsa_isignal_param_s {
    void                          *piSignal; /**< RSA signal pointer. */
    secpkx_rsa_isignal_usr_ptr    *pusr_ptr; /**< User self-defined data. */
}secpkx_rsa_isignal_param_type;

/** RSA parameter type.
*/
typedef union {
    secpkx_rsa_key_param_type      key;       
      /**< PKX RSA key. */
    secpkx_rsa_ks_params_type      ks_params; 
      /**< PKX RSA KS parameter type. */
    secpkx_rsa_isignal_param_type  isignal;   
      /**< PKX RSA is the signal type or not. */
} secpkx_rsa_param_type;

/** ECC key information. 
*/
typedef struct {
    uint8*  public_key_x_ptr;      /**< Public x key. */
    uint8*  public_key_y_ptr;      /**< Public y key. */ 
    uint32  public_key_size;       /**< Public Key size. */
    uint8*  private_key_ptr;       /**< Private key. */
    uint32  private_key_size;      /**< Private key size. */
} secpkx_ecc_key_param_type;
  
/** ECC curve parameters. 
*/
typedef struct secpkx_ecc_curve_param_s {
    char*   m_ptr;  
    char*   a_ptr;  
    char*   b_ptr;
    char*   G_x_ptr;  
    char*   G_y_ptr;  
    char*   n_ptr;  
} secpkx_ecc_curve_param_type;

/** ECC parameter type.
*/
typedef union {
    secpkx_ecc_key_param_type      key;       
      /**< PKX ECC key. */
    secpkx_ecc_curve_param_type    c_params; 
      /**< PKX ECC curve parameter type. */
} secpkx_ecc_param_type;

/** PKX parameter type. 
*/
typedef union {
    seccrypt_param_type         common; /**< Common parameter for PKX. */
    secpkx_modexp_param_type    modexp; /**< Modulus and exponent value type. */
    secpkx_dh_param_type        dh;     /**< DH parameter. */
    secpkx_rsa_param_type       rsa;    /**< RSA parameter. */
    secpkx_ecc_param_type       ecc;    /**< ECC parameter. */
} secpkx_param_data_type; 

/*---------------------------------------------------------------------------
  Hash Parameter Definitions
---------------------------------------------------------------------------*/
/** Enum of parameters (same as common one). 
*/
typedef seccrypt_param_enum_type sechsh_param_enum_type;

/** Hashing modes; these need to be in sync with SECA values. 
*/
typedef enum {
    SECHSH_HASH_MODE_ATOMIC = 0,   /**< &nbsp */
    SECHSH_HASH_MODE_CONTINUE = 1, /**< &nbsp */
    SECHSH_HASH_MODE_FINAL = 2     /**< &nbsp */
} sechsh_hash_mode_param_type;

/** HMAC types, used per invocation. 
*/
typedef enum {
    SECHSH_HMAC_NONE = 0,     /**< HMAC type initial value. */
    SECHSH_HMAC_SSL_V3_TYPE,  /**< HMAC defined in SSL v3. */
    SECHSH_HMAC_TLS_10_TYPE,  /**< HMAC defined in TLS 1.0. */
    SECHSH_HMAC_TLS_11_TYPE,  /**< HMAC defined in TLS 1.1. */
    SECHSH_HMAC_RFC2104_TYPE, /**< HMAC defined in RFC2104. */
    SECHSH_HMAC_SNOW3G_TYPE,  /**< HMAC with snow3g algorithm. */
    SECHSH_CMAC_AES_TYPE,     /**< CMAC AES type. */
    SECHSH_HMAC_MAX_TYPE,     /**< Maximum HMAC value. */  
    SECHSH_HMAC_ENUM_RESERVED = 0x7FFFFFFF /**< Reserved enumerator. */
} seccrypt_hsh_hmac_enum_type;

/** All ciphers use keys and IVs. Unused parameters are set NULL. 
*/
typedef struct {
    uint8*  key_ptr;  
    uint8*  iv_ptr;   
} sechsh_init_data_param_type;

/** HSH input bit length type.  
*/
typedef uint32 sechsh_input_bit_length_type;

/** Hash parameter type.  
*/
typedef union {
    seccrypt_param_type          common;        /**< Common parameter for the hash algorithm. */
    sechsh_hash_mode_param_type  mode;          /**< Hash input mode. */
    sechsh_init_data_param_type  init_data;     /**< Hash initialization data. */
    sechsh_input_bit_length_type input_bit_len; /**< Input data bit length for the hash algorithm. */
} sechsh_param_data_type; 

/**
* @name Cipher parameter definitions
* @{
*/
#define SECCRYPT_ENC_ARC4_KEY_SIZE      16 /**< ARC4 key length. */

#define SECCRYPT_ENC_DES_BLOCK_SIZE     8  /**< DES cipher block length. */
#define SECCRYPT_ENC_3DES_BLOCK_SIZE    8  /**< 3DES cipher block length. */
#define SECCRYPT_ENC_AES_BLOCK_SIZE     16 /**< AES128 cipher block length. */

#define SECCRYPT_ENC_MAX_BLOCK_SIZE     SECCRYPT_ENC_AES_BLOCK_SIZE
                                           /**< Cipher maximum block byte length. */
#define SECCRYPT_ENC_DES_KEY_SIZE       8  /**< DES cipher block length. */
#define SECCRYPT_ENC_3DES_KEY_SIZE      24 /**< 3DES cipher block length. */
#define SECCRYPT_ENC_AES128_KEY_SIZE    16 /**< AES128 key length. */
#define SECCRYPT_ENC_AES256_KEY_SIZE    32 /**< AES256 key length. */

#define SECCRYPT_ENC_DES_IV_SIZE        SECCRYPT_ENC_DES_BLOCK_SIZE
                                           /**< DES IV length. */
#define SECCRYPT_ENC_3DES_IV_SIZE       SECCRYPT_ENC_DES_BLOCK_SIZE
                                           /**< 3DES IV length. */
#define SECCRYPT_ENC_AES_IV_SIZE        SECCRYPT_ENC_AES_BLOCK_SIZE
                                           /**< AES IV length. */
/** @} */  /* end_name_Cipher Parameter Definitions */

/** Cipher algorithm parameter definition.  
*/
typedef seccrypt_param_enum_type secenc_param_enum_type;

/** All ciphers use keys, and unused IV parameters are set to NULL. 
*/
typedef struct {
  struct
  {
    uint8*  key_ptr; 
    uint8*  iv_ptr;  
  } enc; /**< encryption initialization data */

  struct
  {
    uint8*  key_ptr;  
    uint8*  iv_ptr;   
  } dec; /**< decryption initialization data */

} secenc_init_data_param_type;

/** Block cipher mode (CBC or ECB). 
*/
typedef secapi_enc_op_mode_enum_type  secenc_op_mode_param_type;

/** CBC state is used to retrieve the current CBC output. */
typedef struct {
    uint8*  enc; /**< Encryption state. */
    uint8*  dec; /**< Decryption state. */
} secenc_cbc_state_param_type;

typedef secapi_enc_input_mode_enum_type  secenc_input_mode_param_type;

/** Cipher synchronization type.   
*/
typedef enum {
    SECENC_IV_SYNC_OFF = 0,
    SECENC_IV_SYNC_ON
} secenc_iv_sync_param_type;

/** Cipher padding type.   
*/
typedef enum {
    SECENC_NO_PADDING = 0,       
	  /**< No padding is needed. */
    SECENC_ZERO_PADDING,         
	  /**< Padding is filled with zero. */
    SECENC_SEQUENTIAL_PADDING,   
	  /**< Padding byte starts from 1 and is increased by 1. */
    SECENC_RANDOM_PADDING,       
	  /**< Padded with random bytes, and last byte is the padding length. */
    SECENC_STATIC_PADDING,       
      /**< Padded with the value user sets, but the last byte is the padding length */  
	SECENC_LENGTH_VALUE_PADDING, 
      /**< Padded all the bytes with (padding length - 1). */
	SECENC_RFC2630_PADDING,      
      /**< Padded all the bytes with the padding length. */
    SECENC_MAX_PADDING_TYPE      
	  /**< Maximum padding type. */
} secenc_padding_enum_type;

/** Cipher padding parameter type.   
*/
typedef struct {
    secenc_padding_enum_type  type;  /**< Padding type. */
    uint8                     value; /**< Padding value if it is required. */
} secenc_padding_param_type;

/** Cipher counter width type.   
*/
typedef uint32 secenc_counter_width_type;

/** Cipher input bit length type.   
*/
typedef uint32 secenc_input_bit_length_type;

/** Cipher MAC information parameter type.   
*/
typedef struct {

    seccrypt_hsh_hmac_enum_type    mode;       
	  /**< MAC mode executing the hashing. */
    seccrypt_handle_type           mac_handle; 
	  /**< MAC handle executing the hashing. */

    union {
        
        struct {
            struct {
                uint8* mac_secret_ptr; /* Encryption secret. */
            } enc; /* MAC configuration information for SSL encryption. */

            struct {
                uint8* mac_secret_ptr; /* Decryption secret. */
            } dec; /* MAC configuration information for SSL decryption. */

        } ssl; /* MAC configuration information for the SSL. */

    } config; /* MAC configuration information */ 

} secenc_mac_info_param_type; 

/** Cipher parameter data type.   
*/
typedef union {
    seccrypt_param_type          common;         /**< Common parameters for encryption. */
    secenc_init_data_param_type  init_data;      /**< Initial data for encryption. */
    secenc_op_mode_param_type    op_mode;        /**< CIPHER operation mode. */
    secenc_cbc_state_param_type  cbc_state;      /**< CIPHER CBC state parameter type. */
    secenc_input_mode_param_type input_mode;     /**< CIPHER input mode enum type. */
    secenc_iv_sync_param_type    iv_sync;        /**< CIPHER IV sync parameter. */
    secenc_padding_param_type    padding;        /**< Padding type. */
    secenc_counter_width_type    counter_width;  /**< Counter width in Counter mode. */
    secenc_input_bit_length_type input_bit_len;  /**< Cipher input data bit length. */
    secenc_mac_info_param_type   mac_info;       /**< MAC information. */
} secenc_param_data_type;

/** MAC information passed in per packet. 
*/ 
typedef struct {
    union {
        struct {
            uint8   record_type; /* Record type. */
            uint64  seq_no;      /* Sequence number. */
        } ssl;                 /* MAC type for SSL. */

    } mac_type;              /*  MAC type. */

} seccrypt_mac_pkt_info_type;


/** @} */  /** end_addtogroup crypto_secalgoapi */

/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  General crypto library functions
---------------------------------------------------------------------------*/
/**
   @addtogroup crypto_secapi_config 
   @{
*/
/** @name DSM Chain Functions
  @{ 
*/

/**
  Creates a new crypto instance and returns the instance handle back to the 
  calling task.

  There is no callback function invocation upon the completion of this operation.

  @param[out] handle_ptr Pointer to the handle; set by the function.
  @param[in] handle_type Algorithm of the handle.

  @return
  E_SUCCESS     - Request was successful. \n
  E_NOT_ALLOWED - Running out of free space for allocating the handle. \n
  E_INVALID_ARG - Algorithm is not supported.
 
  @dependencies
  Any application using CRYPTO handles must first use this function to get
  a handle.
*/
secerrno_enum_type seccrypt_new
(
seccrypt_handle_type*  handle_ptr,
secapi_algo_enum_type  handle_type
);

/**
  Deletes the crypto instance specified by the passed handle pointer, and stops 
  any DSP operation associated with the passed handle.

  There is no callback function invocation upon the completion of this 
  operation. 

  @param[in] handle_ptr  Handle for the instance to be freed.

  @return
  E_SUCCESS - Delete operation is successful. \n
  E_FAILURE - Handle delete operation failed.

  @dependencies
  None.
  
  @sideeffects
  If the QDSP CRYPTO or Modular Math applet is performing an operation for
  the passed handle instance, the QDSP operation will be aborted. 
*/
secerrno_enum_type seccrypt_delete
(
seccrypt_handle_type*  handle_ptr
);

/**
  Allows an application to register wait functions for all TCBs from which it 
  might be calling the CRYPTO API.

  A registration of a NULL wait function pointer will clear the entry for this 
  task, causing rex_wait() to be used subsequently.

  @return
  E_SUCCESS       - Registration successful. \n
  E_ALREADY_DONE  - Wait function is already registered for this TCB. \n 
  E_INVALID_ARG   - Called with a NULL fptr; this TCB is not registered. \n 
  E_NOT_ALLOWED   - Registry is full. 

  @dependencies
  Only one wait function can be registered per TCB.
*/
secerrno_enum_type seccrypt_register_wait_func
(
rex_tcb_type*  tcb_ptr,
rex_sigs_type  (*wait_fptr)( rex_sigs_type )
);
/** @} */  /* end_name DSM Chain Functions */
/** @} */  /* end_addtogroup_crypto_secapi_config */

/*---------------------------------------------------------------------------
  PKX library functions
---------------------------------------------------------------------------*/
/**
   @addtogroup crypto_secapi_pkx 
   @{
*/
/** @name DSM Chain Functions
  @{ 
*/

/**
  Sets specific Diffie-Hellman parameters for the DH instance specified by the 
  passed instance handle.

  @param[in] handle          Handle to the crypto instance.
  @param[in] algo            Algorithm.
  @param[in] param           Parameter to be set.
  @param[in] param_data_ptr  Pointer to the parameter-specific data.

  @return
  E_SUCCESS - Function executes successfully.

  @dependencies
  None.
*/
secerrno_enum_type seccrypt_pkx_set_param
(
seccrypt_handle_type          handle,
secapi_algo_enum_type         algo,
secpkx_param_enum_type        param,
secpkx_param_data_type*       param_data_ptr
);
/**
  Creates a key using the Diffie-Hellman algorithm. 

  The key created is dependent on the passed key type: Private, Public, or 
  Secret. The key is written to the passed address. 

  This function uses previously stored values for the base, modulus, and key 
  information needed by the Diffie-Hellman algorithm. 
   
  @note 
  If none of these values have been set (via seccrypt_pkx_dh_set_param), 
  default values will be used.

  @param[in] handle        Handle to the crypto instance.
  @param[in] algo          Algorithm.
  @param[in] key           Type of key to create.
  @param[in] key_ptr       Pointer to the memory for the key generation 
                           result.

  @return
  Status of request.

  @dependencies
  - The passed crypto instance must be initialized to perform the Diffie-Hellman
  algorithm.
  - The maximum calculation time stored for Diffie-Hellman must have a non-zero
  value.
        
  @sideeffects
  If a private key is requested, the generated key will be stored as the
  Diffie-Hellman key for the passed instance of Diffie-Hellman.
*/
secerrno_enum_type seccrypt_pkx_create_key
(
seccrypt_handle_type          handle,
secapi_algo_enum_type         algo,
secapi_pkx_key_enum_type      key,
uint8*                        key_ptr
);

/**
  Encrypts a message using the configured public RSA key. 

  @param[in] handle                 Handle to the crypto instance.
  @param[in] algo                   Algorithm.
  @param[in] padding_type           PKX cipher padding type.
  @param[in] plaintext_ptr          Pointer to the plaintext data (memory 
                                    provided by the caller).
  @param[in] plaintext_len          Pointer to the plaintext data length.
  @param[in] ciphertext_ptr         Pointer to the plaintext text data.
  @param[in,out] ciphertext_len_ptr Length of the encrypted data 
                                    (ciphertext).

  @return
  E_SUCCESS     - Function executes successfully. \n
  E_NOT_ALLOWED - One or more Diffie-Hellman parameters have not been set. \n
  E_IN_PROGRESS - CRYPTO-DSP interface was already busy. \n
  E_FAILURE     - Any other failures.

  @dependencies
  None.
*/
secerrno_enum_type seccrypt_pkx_encrypt
(
seccrypt_handle_type          handle,
secapi_algo_enum_type         algo,
secapi_pkx_padding_enum_type  padding_type,
uint8*                        plaintext_ptr,
uint16                        plaintext_len,
uint8*                        ciphertext_ptr,
uint16*                       ciphertext_len_ptr
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
secerrno_enum_type seccrypt_pkx_decrypt
(
seccrypt_handle_type          handle,
secapi_algo_enum_type         algo,
secapi_pkx_padding_enum_type  padding_type,
uint8*                        ciphertext_ptr,
uint16                        ciphertext_len,
uint8*                        plaintext_ptr,
uint16*                       plaintext_len_ptr
);

/**
  Verifies an RSA signature by decrypting and decoding the signature. 
   
  @note  
  - The comparison of the hash data is not done here. This should be done by 
    the calling function.
  - The signature to be verified should conform to certain restrictions. 
    Otherwise, an E_FAILURE is returned.

  @param[in] handle               Handle to the crypto instance. 
  @param[in] algo                 PKX authentication algorithm. 
  @param[in] padding_type         PKX cipher padding type. 
  @param[in] signature_ptr        Pointer to the signature data.
  @param[in] signature_len        Length of the signature.
  @param[out] msg_hash_ptr        Pointer to the hash (memory provided by 
                                  the caller).
  @param[in,out] msg_hash_len_ptr Pointer to the hash data length.

  @return
  E_SUCCESS     - Function executes successfully. \n
  E_NOT_ALLOWED - One or more Diffie-Hellman parameters have not been set. \n 
  E_IN_PROGRESS - CRYPTO-DSP interface was already busy. \n
  E_FAILURE     - Any other failures. 

  @dependencies
  None.
*/
secerrno_enum_type seccrypt_pkx_authenticate
(
seccrypt_handle_type          handle,
secapi_algo_enum_type         algo,
secapi_pkx_padding_enum_type  padding_type,
uint8*                        signature_ptr,
uint16                        signature_len,
uint8*                        msg_hash_ptr,
uint16*                       msg_hash_len_ptr
);
/** @} */  /* end_name DSM Chain Functions */
/** @} */  /* end_addtogroup-crypto_secapi_pkx */

/**
   @addtogroup crypto_secapi_cipher 
   @{
 */
/** @name DSM Chain Functions
  @{ 
*/

/**
  Sets the parameters for the encryption/decryption algorithm specified.

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
secerrno_enum_type seccrypt_enc_set_param
(
seccrypt_handle_type         handle,
secapi_algo_enum_type        algo,
secenc_param_enum_type   param,
secenc_param_data_type*  param_data_ptr
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

secerrno_enum_type seccrypt_enc_get_param
(
seccrypt_handle_type         handle,
secapi_algo_enum_type        algo,
secenc_param_enum_type   param,
secenc_param_data_type*  param_data_ptr
);

/**
  Encrypts the passed message using the specified algorithm.

  @param[in] handle             - Handle to crypto instance.
  @param[in] algo               - Cipher algorithm specifier.
  @param[in] in_msg_ptr         - Pointer to the DSM plaintext data pointer.
  @param[out] out_msg_ptr       - Pointer to the ciphertext data pointer.
  @param[in,out] msg_byte_len   - Pointer of the plaintext/ciphertext message length.
  @param[in] consume_input      - Use in-place of plaintext buffer or not.
  @param[in,out] mac_pkt_info   - MAC information passed in. 
 
  @return
  E_SUCCESS         - On sucessful completion. \n 
  E_BAD_DATA        - Data verification failed. \n
  E_NOT_SUPPORTED   - Option not currently supported. \n
  E_FAILURE         - Any other failure to process the request.
  
  @dependencies
  - The memory allocated for the ciphertext must be large enough to hold the 
  plaintext equivalent and any cipher-specific padding. 
  - The cipher instance pointer must be properly initialized by seccrypt_new(). 
  - Cipher algorithm parameters must be properly set by 
  seccrypt_enc_set_param().
*/
secerrno_enum_type seccrypt_enc_encrypt
(
seccrypt_handle_type         handle,
secapi_algo_enum_type        algo,
dsm_item_type**              in_msg_ptr,
dsm_item_type**              out_msg_ptr,
uint16*                      msg_byte_len,
boolean                      consume_input,
seccrypt_mac_pkt_info_type*  mac_pkt_info
);

/**
  Decrypts the passed message in the DSM chain memory using the specified 
  algorithm. 

  @param[in] handle            Handle to the crypto instance.
  @param[in] algo              Cipher algorithm specifier.
  @param[in] in_msg_ptr        Pointer to the ciphertext data.
  @param[out] out_msg_ptr      Pointer to the plaintext data (memory provided 
                               by the caller).
  @param[in,out] msg_byte_len  Length of the plaintext in bytes; set to the 
                               length of the plaintext by the function.
  @param[in] consume_input     Using the cipher in place or not.
  @param[in] mac_pkt_info      MAC information passed in the packet.

  @return 
  E_SUCCESS        - Successful completion. \n
  E_BAD_DATA       - Data verification failed. \n
  E_NOT_SUPPORTED  - Option is not currently supported. \n 
  E_FAILURE        - Any other failure to process the request. 
   
  @dependencies
  - This function must be called with a complete block ready for decryption if 
  the algorithm is a block cipher. The memory allocated for the ciphertext 
  must be large enough to hold the plaintext equivalent (padding is removed by
  the cipher).
  - The cipher instance pointer must be properly initialized by seccrypt_new().
  - Cipher algorithm parameters must be properly set by 
  seccrypt_enc_set_param().
*/
secerrno_enum_type seccrypt_enc_decrypt
(
seccrypt_handle_type         handle,
secapi_algo_enum_type        algo,
dsm_item_type**              in_msg_ptr,
dsm_item_type**              out_msg_ptr,
uint16*                      msg_byte_len,
boolean                      consume_input,
seccrypt_mac_pkt_info_type*  mac_pkt_info
);

/**
  Encrypts the passed message in the iovector memory using the specified
  algorithm. 

  @param[in] handle                    Handle to the crypto instance.
  @param[in] algo                      Cipher algorithm specifier.
  @param[in] in_msg_ptr                Pointer to the plaintext data.
  @param[in] out_msg_ptr               Pointer to the ciphertext data.
  @param[in] consume_input             Cipher in place or not.
  @param[in] mac_pkt_info              MAC information passed in the packet.

  @return
  E_SUCCESS        - Sucessful completion. \n
  E_BAD_DATA       - Data verification failed. \n 
  E_NOT_SUPPORTED  - Option is not currently supported. \n 
  E_FAILURE        - Any other failure to process the request.
   
  @dependencies
  - The memory allocated for the ciphertext must be large enough to hold the 
  plaintext equivalent and any cipherspecific padding.
  - The cipher instance pointer must be properly initialized by seccrypt_new().
  - Cipher algorithm parameters must be properly set by 
  seccrypt_enc_set_param().
*/

secerrno_enum_type seccryptv_enc_encrypt
(
seccrypt_handle_type         handle,
secapi_algo_enum_type        algo,
IOVEC_T*                     in_msg_ptr,
IOVEC_T*                     out_msg_ptr,
boolean                      consume_input,
seccrypt_mac_pkt_info_type*  mac_pkt_info
);

/**
  Decrypts the passed message in iovector using the specified algorithm. 

  @param[in] handle         Handle to the crypto instance.
  @param[in] algo           Cipher algorithm specifier.
  @param[in] in_msg_ptr     Pointer to the ciphertext data.
  @param[in] out_msg_ptr    Pointer to the plaintext data (memory provided 
                            by the caller).
  @param[in] consume_input  Cipher in place or not.
  @param[in] mac_pkt_info   MAC information passed in the packet.

  @return 
  E_SUCCESS        - Successful completion. \n
  E_BAD_DATA       - Data verification failed. \n
  E_NOT_SUPPORTED  - Option is currently not supported. \n
  E_FAILURE        - Any other failure to process the request.
   
  @dependencies
  - This function must be called with a complete block ready for decryption if 
  the algorithm is a block cipher. 
  - The memory allocated for the ciphertext must be large enough to
  hold the plaintext equivalent (padding is removed by the cipher).
  - The cipher instance pointer must be properly initialized by seccrypt_new().
  - Cipher algorithm parameters must be properly set by 
  seccrypt_enc_set_param().
*/
secerrno_enum_type seccryptv_enc_decrypt
(
seccrypt_handle_type         handle,
secapi_algo_enum_type        algo,
IOVEC_T*                     in_msg_ptr,
IOVEC_T*                     out_msg_ptr,
boolean                      consume_input,
seccrypt_mac_pkt_info_type*  mac_pkt_info
); 
/** @} */  /* end_name DSM Chain Functions */
/** @} */  /* end_addtogroup crypto_secapi_cipher */

/*---------------------------------------------------------------------------
  Hashing library functions
---------------------------------------------------------------------------*/
/**
   @addtogroup crypto_secapi_hash 
   @{
*/
/** @name DSM Chain Functions
  @{ 
*/

/**
  Sets the hashing parameters for the passed instance handle.

  There is no callback function invocation upon the completion of this
  operation.

  @param[in] handle           Handle to the crypto instance.
  @param[in] algo             Hashing algorithm specifier.
  @param[in] param            Parameter to be set.
  @param[out] param_data_ptr  Pointer to the parameter-specific data.

  @return
  E_SUCCESS - Request was successful. \n
  E_FAILURE - Failed to process the request.

  @dependencies
  None.
*/
secerrno_enum_type seccrypt_hsh_set_param
(
seccrypt_handle_type     handle,
secapi_algo_enum_type    algo,
sechsh_param_enum_type   param,
sechsh_param_data_type*  param_data_ptr
);

/**
  Creates a message digest using the algorithm specified. The message is in the
  DSM chain buffer. 

  @param[in] handle           Handle to the crypto instance.
  @param[in] handle_type      Algorithm type specifier.
  @param[in] msg_ptr          DSM chain pointer to the message to be 
                              authenticated.
  @param[in] data_ptr         Data buffer pointer to the data to be 
                              authenticated.
  @param[in] msg_byte_len     Length of the message in bytes.
  @param[out] msg_digest_ptr  Pointer to the message digest (memory 
                              provided by the caller).

  @return
  E_SUCCESS    - Request was successful. \n
  E_IN_PROGRES - QDSP is in use. \n
  E_FAILURE    - Failed to process the request.

  @dependencies
  The hash mode must be properly set by calling seccrypt_hsh_set_param.
*/
secerrno_enum_type seccrypt_hsh_create_digest
(
seccrypt_handle_type   handle,
secapi_algo_enum_type  handle_type,
dsm_item_type*         msg_ptr,
uint8*                 data_ptr,
uint16                 msg_byte_len,
dsm_item_type**        msg_digest_ptr
);

/**
  Creates a hash MAC per RFC 2104 using the specified hash algorithm. The 
  message is in the DSM chain buffer.

  @param[in] handle            Handle to the crypto instance.
  @param[in] hmac_type         HMAC types, used per invocation. Includes 
                               HMAC_SSL, TLS, RFC2104, etc.
  @param[in] hmac_algo         HMAC algorithm specifier.
  @param[in] msg_dsm_ptr       Pointer to the message in the DSM chain to 
                               be authenticated. If set, dsm_data_ptr 
                               should be NULL.
  @param[in] msg_data_ptr      Pointer to the message in the data buffer. 
                               If used, dsm_msg_ptr should be NULL.
  @param[in] msg_byte_len      Length of the message in bytes.
  @param[in] *key_dsm_ptr      Pointer to the key in the data buffer. If 
                               used, key_dsm_ptr should be NULL.
  @param[in] key_data_ptr      Pointer to the key in the DSM chain. If set,
                               key_data_ptr should be NULL.
  @param[in] key_byte_len      Length of the key bytes either in the DSM 
                               chain or the data buffer.
  @param[out] hmac_ptr         Pointer to the HMAC DSM chain output (memory
                               provided by the SEC API. The caller should 
                               call DSM_free_packet() to release the 
                               memory).
   
  @return
  E_SUCCESS    - Request was successful. \n
  E_IN_PROGRES - QDSP is in use. \n
  E_FAILURE    - Failed to process the request. 

  @dependencies
  None.
*/
secerrno_enum_type seccrypt_hsh_create_hmac
(
seccrypt_handle_type          handle,         
seccrypt_hsh_hmac_enum_type   hmac_type,  
secapi_algo_enum_type         hmac_algo,      
dsm_item_type*                msg_dsm_ptr,    
uint8*                        msg_data_ptr,                                  
uint16                        msg_byte_len,   
dsm_item_type*                key_dsm_ptr,    
uint8*                        key_data_ptr,   
uint16                        key_byte_len,             
dsm_item_type**               hmac_ptr         
);

/**
  Creates a hash MAC per RFC 2104 using the specified hash algorithm.

  The HMAC return value is the pointer of uint8.

  @param[in] handle           Handle to the crypto instance.
  @param[in] hmac_type        HMAC type specifier.
  @param[in] hmac_algo        Hashing algorithm specifier.
  @param[in] inner_msg_ptr    DSM chain pointer to the inner message to be
                              authenticated.
  @param[in] inner_data_ptr   Data buffer pointer to the inner message to 
                              be authenticated.
  @param[in] inner_byte_len   Inner byte length.
  @param[in] outer_msg_ptr    DSM chain pointer to the outer message to be
                              authenticated.
  @param[in] outer_data_ptr   Data buffer pointer to the outer message to 
                              be authenticated.
  @param[in] outer_byte_len   Outer byte length.
  @param[out] hmac_ptr        Pointer to the message digest (memory provided 
                              by the caller).

  @return
  E_SUCCESS    - Request was successful. \n
  E_IN_PROGRES - QDSP is in use. \n
  E_FAILURE    - Failed to process the request.

  @dependencies
  None.
*/
secerrno_enum_type seccrypt_hsh_create_hmac_int
(
seccrypt_handle_type          handle,
seccrypt_hsh_hmac_enum_type   hmac_type,
secapi_algo_enum_type         hmac_algo,
dsm_item_type*                inner_msg_ptr,
uint8*                        inner_data_ptr,
uint16                        inner_byte_len,
dsm_item_type*                outer_msg_ptr,
uint8*                        outer_data_ptr,
uint16                        outer_byte_len,
uint8*                        hmac_ptr
);

/**
  Creates a message digest using the algorithm specified.  

  The message is in the iovector type.

  @param[in] handle              Handle to the crypto instance.
  @param[in] handle_type         Hashing algorithm specifier.
  @param[in] msg_iovec_table_ptr iovect type pointer to the message to be 
                                 authenticated.
  @param[out] msg_digest_ptr     Pointer to the message digest (memory 
                                 provided by the caller).

  @return
  E_SUCCESS    - Request was successful. \n 
  E_IN_PROGRES - QDSP is in use. \n 
  E_FAILURE    - Failed to process the request.

  @dependencies
  The hash mode must be properly set by calling seccrypt_hsh_set_param.
*/
secerrno_enum_type seccryptv_hsh_create_digest
(
seccrypt_handle_type   handle,
secapi_algo_enum_type  handle_type,
IOVEC_T*               msg_iovec_table_ptr,
uint8*                 msg_digest_ptr
);

/**
  Creates a hash MAC per RFC 2104 using the specified hash algorithm. 

  The inner and outer message is the IOVEC type.

  @param[in] handle           Handle to the crypto instance.
  @param[in] hmac_type        HMAC types, used per invocation. Includes 
                              HMAC_SSL, TLS, RFC2104, etc.
  @param[in] hmac_algo        HMAC algorithm specifier.
  @param[in] inner_msg_ptr    Inner message pointer for the HMAC calculation.
  @param[in] outer_msg_ptr    Outer message pointer for the HMAC calculation.
  @param[out] hmac_ptr        Pointer to the calculated HMAC value.

  @return
  E_SUCCESS    - Request was successful. \n
  E_IN_PROGRES - QDSP is in use. \n
  E_FAILURE    - Failed to process the request.

  @dependencies
  None.
*/
secerrno_enum_type seccryptv_hsh_create_hmac
(
seccrypt_handle_type          handle,
seccrypt_hsh_hmac_enum_type   hmac_type,
secapi_algo_enum_type         hmac_algo,
IOVEC_T*                      inner_msg_ptr,
IOVEC_T*                      outer_msg_ptr,
uint8*                        hmac_ptr
);
/** @} */  /* end_name DSM Chain Functions */
/** @} */  /* end_addtogroup_crypto_secapi_hash */

/*  End of API */

#endif /* SECCRYPT_H */

