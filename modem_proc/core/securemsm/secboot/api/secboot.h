#ifndef SECBOOT_H
#define SECBOOT_H

/*****************************************************************************
*
* @file  secboot.h (Secboot Authentication API)
*
* @brief This header files contains the types and API to authenticate an
*        image based on certificates and signature. 
* 
* @details
* 
* @note  Example usage:
*          secboot_handle_type            secboot_handle; //try and put this as a static global, as it is
*                                                         //is 512 bytes
* 
*          secboot_image_info_type        image_info;
*          uint32 auth_enabled = 0;
* 
*          if (E_SECBOOT_HW_SUCCESS != secboot_hw_is_auth_enabled(SECBOOT_HW_APPS_CODE_SEGMENT, &auth_enabled))
*             return FAILURE 
* 
*          if (auth_enabled == 1) //Authentication is enabled for this image. Image MUST be authenticated successfully
*          {
*              image_info.header_ptr_1    = header_buf;
*              image_info.header_len_1    = header_len;
*              image_info.code_ptr_1      = code_buf;
*              image_info.code_len_1      = code_len;
*              image_info.x509_chain_ptr  = chain_buf;
*              image_info.x509_chain_len  = chain_len;
*              image_info.signature_ptr   = sig_buf;
*              image_info.signature_len   = sig_len;
*              image_info.sw_type         = TZ_SW_SIGN_TYPE; //Type identifier for TZ images during signing
*              image_info.sw_version      = NumOfOneBitsIn(TZ_ANTI_REPLAY_FUSES);
* 
*              if (E_SECBOOT_SUCCESS != secboot_init(&crypto_hash_ftbl (or NULL if PBL), &secboot_handle))
*                  return FAILURE
* 
*              if (E_SECBOOT_SUCCESS != secboot_authenticate(&secboot_handle,
*                                                            SECBOOT_HW_APPS_CODE_SEGMENT,
*                                                            &image_info, verified_info))
*              {
*                  secboot_deinit(secboot_handle);
*                  return FAILURE;
*              }
*  
*              secboot_deinit(&secboot_handle);
*              return SUCCESS;
*          }
* 
* Copyright (c) 2010-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*
*****************************************************************************/


/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:$
$Header: //components/rel/core.mpss/3.5.c12.3/securemsm/secboot/api/secboot.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/28/2013 hw       Added retail unlock feature code
03/06/2013 vg       Added a function to take the fuse values.
03/02/2013 hw,mm    Added addition return code for Root Cert Selection feature
08/25/2011 sm       Initial API - based on old api with some changes
============================================================================*/

/** @ingroup SecureMSM
 *  @{
 */

/** @defgroup SecbootAuthentication Secboot Authentication Library */

/**
 * @addtogroup SecbootAuthentication
 *
 * @{
 */


/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "CeML.h"
/*===========================================================================
                      PUBLIC DATA DECLARATIONS
===========================================================================*/
/* Maximum number of cert levels in a cert chain */
#define SECBOOT_MAX_NUM_CERTS    3

/* Minimum number of cert levels in a cert chain*/
#define SECBOOT_MIN_NUM_CERTS    2

/* Maximum number of Root Certs */
#define SECBOOT_MAX_ROOT_CERTS   16

/* Total number of certs including max root certs */
#define SECBOOT_TOTAL_MAX_CERTS  (SECBOOT_MAX_ROOT_CERTS + SECBOOT_MAX_NUM_CERTS - 1)

/* Maximum Cert Chain Size */
#define SECBOOT_MAX_CERT_SIZE       (3072)
#define SECBOOT_MAX_CERT_CHAIN_SIZE (SECBOOT_TOTAL_MAX_CERTS * SECBOOT_MAX_CERT_SIZE)

/**
 * @brief Error codes specific to secboot
 */
typedef enum secboot_error_type
{
  E_SECBOOT_SUCCESS                = 0,   /**< Operation was successful. */
  E_SECBOOT_FAILURE                = 1,   /**< General failure. */
  E_SECBOOT_INVALID_PARAM          = 2,   /**< Invalid parameter passed into function. */
  E_SECBOOT_INVALID_DATA           = 3,   /**< Data is invalid. */
  E_SECBOOT_UNSUPPORTED            = 4,   /**< Option not supported. */
  E_SECBOOT_RSA_FAIL               = 5,   /**< Failure occured for RSA. */
  E_SECBOOT_HASH_FAIL              = 6,   /**< Failure occured for hash. */
  E_SECBOOT_HW_FAIL                = 7,   /**< Failure occured for HW. */
  E_SECBOOT_X509_FAIL              = 8,   /**< Failure occured during cert chain parsing. */
  E_SECBOOT_INVALID_CERT           = 9,   /**< Cert chain validation checks failed. */
  E_SECBOOT_INVALID_CERT_SIG       = 10,  /**< Cert chain signature validation failed. */
  E_SECBOOT_UNTRUSTED_ROOT         = 11,   /**< Root certificate is not the root of trust. */
  E_SECBOOT_INVALID_IMAGE_SIG      = 12,  /**< Invalid image signature. */
  E_SECBOOT_INVALID_SW_TYPE        = 13,  /**< Unexpected software type. */
  E_SECBOOT_INVALID_SW_VERSION     = 14,  /**< Image has been signed with an older version */
  E_SECBOOT_INVALID_MSM_HW_ID      = 15,  /**< Image has been signed for a different hw id */
  E_SECBOOT_INVALID_DEBUG          = 16,  /**< Invalid debug cert */
  E_SECBOOT_INIT                   = 17,  /**< Initialization failed */
  E_SECBOOT_DEINIT                 = 18,  /**< De-initialization failed */
  E_SECBOOT_INVALID_ROOT_SEL       = 19,  /**< Root selection failed */
  E_SECBOOT_INVALID_CRASH_DUMP     = 20,  /**< Invalid crash dump cert */
  E_SECBOOT_MAX                    = 0x7FFFFFFF /**< Force to 32 bits */
} secboot_error_type;

/**
 * @brief Function table containing pointers to the crypto driver hash API
 */
typedef struct secboot_crypto_hash_ftbl_type
{
   CeMLErrorType (*CeMLInit)       ( void );
   CeMLErrorType (*CeMLDeInit)     ( void );
   CeMLErrorType (*CeMLHashInit)   ( CeMLCntxHandle **h,  CeMLHashAlgoType);
   CeMLErrorType (*CeMLHashDeInit) ( CeMLCntxHandle ** h);
   CeMLErrorType (*CeMLHashUpdate) ( CeMLCntxHandle * h,  CEMLIovecListType);
   CeMLErrorType (*CeMLHashFinal)  ( CeMLCntxHandle * h,  CEMLIovecListType*);
   CeMLErrorType (*CeMLHashAtomic) ( CeMLCntxHandle * h,  CEMLIovecListType, CEMLIovecListType*);
}secboot_crypto_hash_ftbl_type;

/**
 * @brief Information about the image to be authenticated
 */
typedef struct secboot_image_info_type
{
  const uint8* header_ptr_1;   /**< Pointer to the header */
  uint32       header_len_1;   /**< Length in bytes of the image header */
  const uint8* code_ptr_1;     /**< Pointer to the code */
  uint32       code_len_1;     /**< Length in bytes of the image */
  const uint8* x509_chain_ptr; /**< Pointer to the certificate chain */
  uint32       x509_chain_len; /**< Length in bytes of the certificate chain */
  const uint8* signature_ptr;  /**< Pointer to the signature */
  uint32       signature_len;  /**< Length in bytes of the  signature */
  uint32       sw_type;        /**< Type of the image being authenticated - SBL1, TZ etc */
  uint32       sw_version;     /**< Minimum version of the image that can be executed (for rollback prevention) */
} secboot_image_info_type;

/**
 * @brief Hash information of the image
 */
typedef struct secboot_code_hash_info_type
{
  uint32   code_address;       /**< Address (pointer value) of the code that was hashed */
  uint32   code_length;        /**< the code length */
  uint32   image_hash_length;  /**< hash length - e.g 20 for SHA1, 32 for SHA256 */
  uint8    image_hash[CEML_HASH_DIGEST_SIZE_SHA256]; /**< hash of HEADER + CODE */
}secboot_image_hash_info_type;

/* store the root cert selection information */
typedef struct secboot_root_cert_fuse_info_type
{
  uint32   is_root_sel_enabled;  /* Is Root Cert Selection enabled */
  uint32   root_cert_sel;        /* valid only if root cert selection is enabled */
  uint32   num_root_certs;       /* valid only if root cert selection is enabled */
} secboot_root_cert_fuse_info_type;

/**
 * @brief: This structure contains the fuse information used by secboot authentication process.
 * Caller can optionally supply these fuse value by calling secboot_init_fuses() api.
 *
 * Caller has option to supply either all the fuse values or only root_of_trust. Which will
 * be determined by structure parameter "use_root_of_trust_only".
 *  @ param[in]    use_root_of_trust_only    TRUE  - Caller supply only root_of_trust
 *                                           FALSE - Caller supply all the fuses.
 * 
 * If caller didn't pass any fuse value (don't call secboot_init_fuses() api), secboot authentication 
 * process read all these fuse parameters from h/w.
 *
 * If the caller passes only root_of_trust (caller sets use_root_of_trust_only = TRUE), secboot  
 * authentication process read remaining fuse parameters from h/w.
 *
 */

typedef struct secboot_fuse_info_type
{
  uint8        root_of_trust[CEML_HASH_DIGEST_SIZE_SHA256]; /**< sha256 hash of the root certificate */
  uint64       msm_hw_id;             
  uint32       auth_use_serial_num;
  uint32       serial_num;
  boolean      use_root_of_trust_only; /**< Caller sets this variable to TRUE if 
                                            secboot needs to use only root of trust from the 
                                            supplied fuses */
  secboot_root_cert_fuse_info_type    root_sel_info;
} secboot_fuse_info_type;


/**
 * @brief Debug settings which are specified in the Debug OU field of the 
 *        attestation certificate to indicate what to do with the one-time
 *        override registers. The Debug OU field is 64 bits, lower 32 bits
 *        denote what debug settings the attestation certifiate has been
 *        signed with. Bits 0 and 1 of the debug settings denote if/what
 *        needs to be written to the one-time override registers.
 *        Images which need to act on these settings is architecture specific
 */
#define SECBOOT_DEBUG_NOP     0x0 /**< Bit value 00 - No operation needs to be performed */
#define SECBOOT_DEBUG_DISABLE 0x2 /**< Bit value 10-  Write 0 to one-time override registers */                                     
#define SECBOOT_DEBUG_ENABLE  0x3 /**< Bit value 11 - Write 1 to one-time override registers*/

/**
 * @brief Crash Dump settings which are specified in the Crash Dump OU field of the 
 *        attestation certificate to indicate whether the crash report is dumped for
 *        the specific phone. The Crash Dump OU field is 64 bits, lower 32 bits
 *        denote what Crash Dump settings the attestation certifiate has been
 *        signed with. Bits 0 of the Crash Dump settings denote the boolean
 *        flag. 0 means disabled and 1 means enabled.
  */
#define SECBOOT_CRASH_DUMP_DISABLE 0x0 /**< Bit value 0 - crash dump is turned off */                                     
#define SECBOOT_CRASH_DUMP_ENABLE  0x1 /**< Bit value 1 - crash dump is turned on */

/**
 * @brief The Root Cert Selection feature can be disabled
 * by blowing 0xF0 in the ROOT_CERT_PK_HASH_INDEX. If this is blown then 
 * switching to a different cert is not possible. By default the feature is
 * enabled.
 */
#define SECBOOT_ROOT_CERT_SEL_ENABLED  1 /**< Root Cert Selection is enabled */
#define SECBOOT_ROOT_CERT_SEL_DISABLED 0 /**< Root Cert Selection  is disabled */

/**
 * @brief Data returned from a successful authentication.
 */
typedef struct secboot_verified_info_type
{
  uint32                       version_id;   /**< The version id (define the secboot lib version) */
  uint64                       sw_id;        /**< The software id (upper 32 bits:version, lower 32 bits:type)
                                                  the image was signed with */
  uint64                       msm_hw_id;    /**< The constructed MSM HW ID value used to authenticate
                                                  the image */
  uint32                       enable_debug; /**< Value of the debug settings from the attestation cert i.e
                                                  SECBOOT_DEBUG_NOP, SECBOOT_DEBUG_DISABLE, SECBOOT_DEBUG_ENABLE */
  secboot_image_hash_info_type image_hash_info; /** Hash of the header + code */

  uint32                       enable_crash_dump; /**< Value of the crash dump settings from the attestation cert i.e
                                                       SECBOOT_CRASH_DUMP_DISABLE, SECBOOT_CRASH_DUMP_ENABLE */
} secboot_verified_info_type;

/**
 * @brief Secboot handle
 */
typedef struct secboot_handle_type
{
    uint32 handle[128]; /**< 512 byte buffer needed by secboot for it's operations */
}secboot_handle_type;

/**
 * @brief Secboot Function table
 */
typedef struct secboot_ftbl_type
{
  secboot_error_type (*secboot_init)
                      (const secboot_crypto_hash_ftbl_type*  crypto_hash_ftbl_ptr,
                       secboot_handle_type*                 secboot_handle_ptr);
  secboot_error_type (*secboot_deinit)
                       (secboot_handle_type* secboot_handle_ptr);
  secboot_error_type (*secboot_authenticate)
                      (secboot_handle_type*                secboot_handle_ptr,
                       uint32                              code_segment,
                       const secboot_image_info_type*      image_info_ptr,
                       secboot_verified_info_type*         verified_info_ptr);
  secboot_error_type (*secboot_init_fuses)
                      (secboot_handle_type* secboot_handle_ptr,
                       secboot_fuse_info_type fuses);
  secboot_error_type (*secboot_read_fuses)
                      (secboot_handle_type*       secboot_handle_ptr,
                       uint32 code_segment,
                       secboot_fuse_info_type*    fuse_info);
}secboot_ftbl_type;

/**
 * @brief This function initializes the secboot handle and sets up the crypto hw engine
 *
 * @param[in]     crypto_hash_ftbl_ptr  Pointer to the crypto hash driver functions 
 *                                      that secboot should use for hashing. If
 *                                      NULL the crypto driver linked into the
 *                                      image that this routine resides in will be
 *                                      used for hashing.
 *  
 * @param[in,out] secboot_handle_ptr    Pointer to the secboot handle. The handle
 *                                      must be allocated in protected memory
 * 
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. If 
 *         this function returns an error, callers need not call secboot_deinit()
 *         as this will be called from within the error handling routine of this
 *         function 
 *
 * @dependencies This function must be called before any other function in this
 *               API is called. Caller must not deallocate the secboot handle
 *               until after secboot_deinit().
 *
 * @sideeffects  Calls the CeMLInit() to initialize the Crypto HW engine. 
 
 *
 * @see secboot_deinit()
 *
 */
secboot_error_type secboot_init
(
  const secboot_crypto_hash_ftbl_type*  crypto_hash_ftbl_ptr,
  secboot_handle_type*                  secboot_handle_ptr
);


/**
 * @brief This function stores the supplied fuses in secboot context.
 *  
 * @param[in,out] secboot_handle_ptr    Pointer to the secboot handle returned
 *                                      from secboot_init()
 *
 * @param[in]     fuses                 Contains the fuse related information.
 * 
 * @return E_SECBOOT_SUCCESS on success. Appropriate error code on failure. 
 *
 * @dependencies secboot_init() needs to have been called..
 *
 * @sideeffects  none.  
 *
 * @see 
 *
 */
secboot_error_type secboot_init_fuses
(
  secboot_handle_type* secboot_handle_ptr,
  secboot_fuse_info_type fuses  
);

/**
 * @brief This function stores the supplied fuses in secboot context.
 *
 * @param[in,out] secboot_handle_ptr    Pointer to the secboot handle returned
 *                                      from secboot_init()
 *
 * @param[in]     fuse_info                 Contains the fuse related information.
 *
 * @return E_SECBOOT_SUCCESS on success. Appropriate error code on failure.
 *
 * @dependencies secboot_init() needs to have been called..
 *
 * @sideeffects  none.
 *
 * @see
 *
 */
secboot_error_type secboot_read_fuses(
  secboot_handle_type*       secboot_handle_ptr,
  uint32                     code_segment,
  secboot_fuse_info_type*    fuse_info
);

/**
 * @brief This function authenticates an image. 
 *
 * @param[in,out]     secboot_handle_ptr   Pointer to the secboot handle returned
 *                                         from secboot_init()
 *
 * @param[in]         code_segment         Code segment in SECURE_BOOTn fuse register
 *                                         containing authentication settings for
 *                                         the image.
 *
 * @param[in]         image_info           Information on the image to be authenticated
 *                                         
 * @param[in,out]     verified_info_ptr    Pointer to data that was processed during
 *                                         authentication. Caller can use this data
 *                                         if the function returned success
 *
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure. If a failure,
 * was returned the caller SHOULD NOT allow the image to execute
 *
 * @dependencies secboot_init() needs to have been called. 
 *               This function needs 4000 bytes of stack space
 *
 * @sideeffects  none
 *               
 *               
 *               
 *           
 *
 * @see secboot_init(), secboot_deinit()
 *
 */
secboot_error_type secboot_authenticate
(
  secboot_handle_type*                secboot_handle_ptr,
  uint32                              code_segment,
  const secboot_image_info_type*      image_info_ptr,
  secboot_verified_info_type*         verified_info_ptr
);

/**
 * @brief This function clears the secboot handle
 *
 * @param[in,out] secboot_handle_ptr   Pointer to the secboot handle returned
 *                                     from secboot_init()
 *
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
 *
 * @dependencies None
 *
 * @sideeffects  Calls the CeMLDeInit() to de-initialize the Crypto HW engine
 *
 * @see secboot_init()
 *
 */
secboot_error_type secboot_deinit
(
  secboot_handle_type*      secboot_handle_ptr
);

/**
 * @brief This function return pointers to the secboot functions linked into
 *        the image
 *
 * @param[in,out] ftbl_ptr              Pointer to the function table structure
 *                                      to populate. The pointer must be allocated
 *                                      by the caller.
 *
 * @return E_SECBOOT_HW_SUCCESS on success. Appropriate error code on failure.
 *
 * @sideeffects  None
 *
 *
 */
secboot_error_type secboot_get_ftbl
(
  secboot_ftbl_type* ftbl_ptr
);

/**
 * @brief This function calls the crypto software driver's hash API to
 *        hash the data
 *
 * @param[in]         crypto_hash_ftbl    Pointer to the crypto APIs.
 *
 * @param[in]         hash_algo         Hash algorithm to use.
 *
 * @param[in]         data1_in_ptr      Pointer to the data to hash. Cannot be NULL. 
 *                                      as we don't expect boot images/certificates
 *                                      to be loaded at address 0
 *
 * @param[in]         data1_len         Length in bytes of the data pointed to by
 *                                      data1_in_ptr. Cannot be 0 as we don't allow
 *                                      image or certificate size to be 0
 *
 * @param[in]         data2_in_ptr      Pointer to the data to hash. This is an
 *                                      optional field and can be NULL.
 *
 * @param[in]         data2_len         Length in bytes of the data pointed to by
 *                                      data2_in_ptr. 0 if data2_in_ptr is NULL.
 *                                  
 * @param[in,out]     digest_ptr        Pointer to a buffer to store the hash result.
 *
 * @param[in]         digest_len        Length in bytes of buffer pointed to by
 *                                      digest_ptr.
 *
 * @return E_SECBOOT_SUCCESS on success. Appropriate error code on failure.
 *
 * @dependencies Caller should ensure the pointers and lengths passed in are valid
 *
 * @sideeffects  None
 *           
 * @see None
 *
 */
secboot_error_type secboot_hash
(
  secboot_crypto_hash_ftbl_type*    crypto_hash_ftbl,
  CeMLHashAlgoType                  hash_algo,
  const uint8*                      data1_in_ptr,
  uint32                            data1_len,
  const uint8*                      data2_in_ptr,
  uint32                            data2_len,
  uint8*                            digest_ptr,
  uint32                            digest_len
);

/// @}
//
/// @}
#endif /* SECBOOT_H */

