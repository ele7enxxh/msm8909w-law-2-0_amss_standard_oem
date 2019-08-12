#ifndef __QSEE_EXEC_H_
#define __QSEE_EXEC_H_

/*===========================================================================
  Copyright (c) 2011 - 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/kernel/inc/tzos_exec.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/

#include "lktypes.h"

typedef struct tzbsp_memblock_s
{
  uint32_t start;
  uint32_t end;
  uint32_t attributes;
} tzexec_memblock_t, *tzexec_memblock_ptr;


typedef struct 
{
  uint32_t id;
  void*  fntbl_group;
}__attribute__ ((packed)) tzexecFuncTable_t;


typedef struct
{
  uint32_t     id;
  const void *group;
} __attribute__ ((packed)) tzexecFuncGroups_t;

/*----------------------------------------------------------------------------
                      TZEXEC Function Group Boot
 -----------------------------------------------------------------------------*/

/**
 * Notifies TZEXEC client of SMC call by HLOS directed at client
 *
 * @param [in]  req      Opaque structure passed by HLOS
 * @param [in]  req_len  Length of req in bytes
 * @param [out] rsp      Return structure for TZEXEC client to fill
 * @param [in]  rsp_len  Length of rsp in bytes
 *
 * @return Zero on success, otherwise an error code.
 */
typedef int (*tzapp_smc_func_t)(uint32_t app_id, void *req, uint32_t req_len,
                                 void *rsp, uint32_t rsp_len);

/**
 * Called to notify TZEXEC client that a FIQ has occurred
 */
typedef void (*tzexec_fiq_notifier_func_t)(uint32_t);

typedef int (*qsee_smc_func_t)(void *req, uint32_t req_len,
                                 void *rsp, uint32_t rsp_len);

/**
 * Called to notify QSEE client that warmboot has occurred
 */
typedef void (*qsee_warmboot_notifier_func_t)(void);

/**
 * Called to notify QSEE client that power collapse has occurred
 */
typedef void (*qsee_power_collapse_notifier_func_t)(void);

typedef struct
{
  qsee_smc_func_t             qsee_callback_smc_handler;
  tzexec_fiq_notifier_func_t  tzexec_callback_fiq_handler;
  tzapp_smc_func_t            tzapp_callback_smc_handler;
  qsee_warmboot_notifier_func_t         qsee_callback_warmboot_handler;
  qsee_power_collapse_notifier_func_t   qsee_callbacl_power_collapse_handler;
} tzexec_monitor_handler_table_t, *tzexec_monitor_handler_table_ptr;

/**
 * Called to populate optional external handlers for communication to
 * TZ
 *
 * @param [in] pTable pointer to packed table of function pointers
 *                    appropriate for each communication type
 */
typedef void (*tzexec_register_monitor_handlers_func_t)(tzexec_monitor_handler_table_ptr  pTable);


/**
 * Logs all TZEXEC client related debugging under the same priority as
 * TZEXEC
 *
 * @param [in] fmt String of characters to print
 * @param [in] ap  List of values to substitute in string
 */
typedef void (*tzexec_t32_printf)(const char* fmt, va_list ap);

/**
 * Fetch the function pointer table address for use by an external OS
 *
 * Returns the address of the function pointer table
 */
typedef uint32_t (*tzexec_get_fgroups_ptr_ext_func_t)(void);

/**
 * Fetch the address of the structure describing the external DDR
 * available to an external OS
 *
 * Returns the address of the structure
 */
typedef uint32_t (*tzexec_get_ddr_ext_func_t)(void);

typedef int (*tzexec_ssd_decrypt_img_func_t)(uint32_t** img_ptr, uint32_t *img_len_ptr);


/*----------------------------------------------------------------------------
                      TZEXEC Function Group: Boot
 -----------------------------------------------------------------------------*/
typedef struct
{
  uint32_t                                        register_id;
  tzexec_register_monitor_handlers_func_t         tzexec_register_monitor_handlers;

  uint32_t                                        printf_id;
  tzexec_t32_printf                               tzexec_t32_printf;

  uint32_t                                        function_ptr_id;
  tzexec_get_fgroups_ptr_ext_func_t               tzexec_get_fgroups_ptr_ext;
  
  uint32_t                                        get_ddr_id;
  tzexec_get_ddr_ext_func_t                       tzexec_get_ddr_ext;
  
  uint32_t                                        ssd_decrypt_img;
  tzexec_ssd_decrypt_img_func_t                   tzexec_ssd_decrypt_img;
      
  uint32_t                                        end_of_table_id;
  void*                                           null_ptr;
} tzexec_func_group_boot_t, *tzexec_func_group_boot_ptr;


#define TZEXEC_FUNC_BOOT_REGISTER_MONITOR_HANDLERS_ID            1
#define TZEXEC_FUNC_BOOT_T32_PRINTF_ID                           2
#define TZEXEC_FUNC_BOOT_GET_FGROUPS_PTR_ID                      3
#define TZEXEC_FUNC_BOOT_GET_DDR_EXT_LAYOUT_ID                   4
#define TZEXEC_FUNC_BOOT_SSD_DECRYPT_IMG_ID                      5

/*----------------------------------------------------------------------------
                      TZEXEC Function Group: Interrupt
 -----------------------------------------------------------------------------*/

/**
 * Called to tell TZEXEC to register a FIQ in the Qualcomm Generic
 * Interrupt Controller
 *
 * @param [in] intnum The interrupt to enable
 * @param [in] flags  Either 0 for triggering on level or
 *                           1 for triggering on edge
 * @param [in] enable Initial interrupt enable/disable status on the
 *                    interrupt controller.
 *
 * @return int 0-31 bit position of newly registered interrupt in the
 *             bitmask, negative value indicates failure
 */
typedef uint32_t (*tzexec_reg_int_func_t)(
                                       uint32_t      intNum,
                                       uint32_t      flags,
                                       boolean     isEnabled);

/**
 * Called to tell TZEXEC to deregister a FIQ in the Qualcomm Generic
 * Interrupt Controller
 *
 * @param [in] intnum The interrupt to enable
 */
typedef void (*tzexec_dereg_int_func_t)(uint32_t  intNum);

/**
 * Read the TZEXEC client interrupt mask
 *
 * @returns bitmask of active int bits
 */
typedef uint32_t (*tzexec_read_int_msk_func_t)(void);

/**
 * Clear a bit or bits in the interrupt mask
 *
 * @param [in] msk bitmask to clear from the status register
 *
 * @returns uint32_t bitmask of active interrupt bits after clearing
 *
 * Note: This fn can be interrupted and return a stale mask value if
 * not called from the ISR context
 *
 * Note: The physical interrupt line will be cleared regardless of the
 * status bit here
 */
typedef uint32_t (*tzexec_clr_int_func_t)(uint32_t mask);


/**
 * Disables all interrupts.
 *
 * @return The original interrupt bits from CPSR.
 */
typedef uint32_t (*tzexec_disable_all_int_func_t)(void);

/**
 * Restores previously disabled interrupts.
 *
 * @param [in] flags The interrupt bits to restore to CPSR.
 */
typedef void (*tzexec_restore_int_func_t)(uint32_t flags);


/**
 * Overrides interrupt status on spare shared peripheral interrupts
 * (SPIs) to provide IRQs to Android that they can register for.  This
 * functionality needs to be approved and perhaps the IRQs should be
 * reserved and explicitly set to nonsecure.  The spare SPIs are
 * 218-223.  The argument given is the offest from the first spare SPI.
 *
 * @param intnum Interrupt number to raise, should be 0-6
 *
 * @retval -1 If intnum is outside range of allowable IRQs
 *         0 If success
 */
typedef int (*tzexec_gen_irq_func_t)(uint32_t intNum);

/**
 * Configures given intnum as secure and generates a secure software
 * generated interrupt (SGI) to the currently running processor
 *
 * @param intnum Interrupt number to raise, should be 0-15
 *
 * @retval -1 If intnum is outside range of allowable SGIs
 *         0 If success
 */
typedef int (*tzexec_gen_fiq_func_t)(uint32_t intNum);


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
typedef struct
{
  uint32_t                              register_int_id;
  tzexec_reg_int_func_t               tzexec_reg_int;

  uint32_t                              dereg_int_id;
  tzexec_dereg_int_func_t             tzexec_dereg_int;

  uint32_t                              read_int_msk_id;
  tzexec_read_int_msk_func_t          tzexec_read_int_mask;

  uint32_t                              clr_int_id;
  tzexec_clr_int_func_t               tzexec_clr_int;

  uint32_t                              disable_all_int_id;
  tzexec_disable_all_int_func_t       tzexec_disable_all_int;

  uint32_t                              restore_int_id;
  tzexec_restore_int_func_t           tzexec_restore_int;

  uint32_t                              gen_irq_id;
  tzexec_gen_irq_func_t               tzexec_gen_irq;

  uint32_t                              gen_fiq_id;
  tzexec_gen_fiq_func_t               tzexec_gen_fiq;

  uint32_t                              end_of_table_id;
  void*                               null_ptr;
} tzexec_func_group_int_t, *tzexec_func_group_int_ptr;


#define TZEXEC_FUNC_INT_REGISTER_ID                       1
#define TZEXEC_FUNC_INT_DEREGSITER_ID                     2
#define TZEXEC_FUNC_INT_READ_MASK_ID                      3
#define TZEXEC_FUNC_INT_CLEAR_MASK_ID                     4
#define TZEXEC_FUNC_INT_DISABLE_ALL_ID                    5
#define TZEXEC_FUNC_INT_ENABLE_ID                         6
#define TZEXEC_FUNC_INT_GENERATE_IRQ_ID                   7
#define TZEXEC_FUNC_INT_GENERATE_FIQ_ID                   8


/*----------------------------------------------------------------------------
                      TZEXEC Function Group: Timer
 -----------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
typedef int (*tzexec_gpt_timer_start_func_t)(uint32_t timeout_ms,boolean   periodic);

//------------------------------------------------------------------------------
typedef int (*tzexec_gpt_timer_stop_func_t)(void);

//------------------------------------------------------------------------------
typedef int (*tzexec_timer_enable_int_func_t)(void);

//------------------------------------------------------------------------------
typedef int (*tzexec_timer_disable_int_func_t)(void);

typedef uint64_t (*tzexec_sec_ctrl_timer_start_func_t)(uint64_t timeout_ms);
                                  
//------------------------------------------------------------------------------
typedef uint64_t (*tzexec_sec_ctrl_timer_count_func_t)(void);

typedef void (*tzbsp_exec_sec_ctrl_wdt_start_t)(uint64_t bark_timeout, uint64_t bite_timeout);
typedef void (*tzbsp_exec_sec_ctrl_wdt_stop_t)(void);
typedef void (*tzbsp_exec_sec_ctrl_wdt_pet_t)(void);

typedef struct
{
  uint32_t                              timer_gpt_start_id;
  tzexec_gpt_timer_start_func_t           tzexec_gpt_timer_start;

  uint32_t                              timer_gpt_stop_id;
  tzexec_gpt_timer_stop_func_t            tzexec_gpt_timer_stop;

  uint32_t                              timer_enable_int_id;
  tzexec_timer_enable_int_func_t      tzexec_timer_enable_int;

  uint32_t                              timer_disable_int_id;
  tzexec_timer_disable_int_func_t     tzexec_timer_disable_int;

  uint32_t                              timer_sec_ctrl_start_id;
  tzexec_sec_ctrl_timer_start_func_t    tzexec_sec_ctrl_timer_start;

  uint32_t                              timer_sec_ctrl_count_id;
  tzexec_sec_ctrl_timer_count_func_t    tzexec_sec_ctrl_timer_count;

  uint32_t                              timer_sec_ctrl_wdt_start_id;
  tzbsp_exec_sec_ctrl_wdt_start_t       tzbsp_exec_sec_ctrl_wdt_start;

  uint32_t                              timer_sec_ctrl_wdt_stop_id;
  tzbsp_exec_sec_ctrl_wdt_stop_t        tzbsp_exec_sec_ctrl_wdt_stop;

  uint32_t                              timer_sec_ctrl_wdt_pet_id;
  tzbsp_exec_sec_ctrl_wdt_pet_t         tzbsp_exec_sec_ctrl_wdt_pet;

  uint32_t                              end_of_table_id;
  void*                               null_ptr;
} tzexec_func_group_timer_t, *tzexec_func_group_timer_ptr;


#define TZEXEC_FUNC_TIMER_START_ID                        1
#define TZEXEC_FUNC_TIMER_STOP_ID                         2
#define TZEXEC_FUNC_TIMER_ENBALE_INT_ID                   3
#define TZEXEC_FUNC_TIMER_DISBALE_INT_ID                  4

/*----------------------------------------------------------------------------
                      TZEXEC Function Group: Crypto
 -----------------------------------------------------------------------------*/

typedef void qsee_cipher_ctx;

/** Cipher supported algorithms */
typedef enum
{
  QSEE_CIPHER_ALGO_AES_128    = 0,
  QSEE_CIPHER_ALGO_AES_256,
  QSEE_CIPHER_ALGO_TRIPLE_DES,
  QSEE_CMAC_ALGO_INVALID      =0x7FFFFFFF,
} QSEE_CIPHER_ALGO_ET;

/** Support for SHA1 and SHA256 for hash */
typedef enum
{
  QSEE_HASH_NULL         = 1, ///< Do not perform any hashing
  QSEE_HASH_SHA1         = 2,
  QSEE_HASH_SHA256       = 3
} QSEE_HASH_ALGO_ET;

/** Supported HMAC algorithms   */
typedef enum
{
  QSEE_HMAC_SHA1          = 1,
  QSEE_HMAC_SHA256        = 2,
  QSEE_HMAC_INVALID       = 0x7FFFFFFF,
} QSEE_HMAC_ALGO_ET;

typedef enum
{
  QSEE_CIPHER_PARAM_KEY        = 0,
  QSEE_CIPHER_PARAM_IV,
  QSEE_CIPHER_PARAM_MODE,
  QSEE_CIPHER_PARAM_PAD,
  QSEE_CIPHER_PARAM_NONCE,
  QSEE_CIPHER_PARAM_XTS_KEY,
  QSEE_CIPHER_PARAM_XTS_DU_SIZE,
  QSEE_CIPHER_PARAM_CCM_PAYLOAD_LEN,
  QSEE_CIPHER_PARAM_CCM_MAC_LEN,
  QSEE_CIPHER_PARAM_CCM_HDR_LEN,
  QSEE_CIPHER_PARAM_INVALID    = 0x7FFFFFFF,
} QSEE_CIPHER_PARAM_ET;

/** Supported CMAC algorithms   */
typedef enum
{
   QSEE_CMAC_ALGO_AES_128          = 1,
   QSEE_CMAC_ALGO_AES_256          = 2,
   QSEE_CMAC_INVALID               = 0x7FFFFFFF,
}  QSEE_CMAC_ALGO_ET;

typedef void qsee_hash_ctx;

typedef void qsee_hmac_ctx;

typedef enum
{
  QSEE_HASH_PARAM_MODE              = 0x00,
  QSEE_HASH_PARAM_HMAC_KEY          = 0x01,
  QSEE_HASH_PARAM_SEQ               = 0x02
} QSEE_HASH_PARAM_ET;

/**
 * @brief  This function will create a message digest hash using the
 *         algorithm specified.
 *
 * @param hash[in]         The hash algorithm
 * @param msg[in]          The message to hash
 * @param msg_len[in]      The length of the message
 * @param digest[in,out]   The digest to store
 * @param digest_len[in]   Length of the output message digest hash
 *                         buffer in bytes. Must be 20 bytes for SHA1 and
 *                         32 bytes for SHA256
 *
 * @return 0 on success, negative on failure
 */
typedef int (*tzexec_hash_func_t)
(
  QSEE_HASH_ALGO_ET     alg,
  const uint8_t             *msg,
  uint32_t                  msg_len,
  uint8_t                   *digest,
  uint32_t                  digest_len
);

/**
 * @brief Intialize a hash context for update and final functions.
 *
 * @param alg[in] The algorithm standard to use
 * @param hash_ctx[out] The hash context
 *
 * @return 0 on success, negative on failure
 *
 */
typedef int (*tzexec_hash_init_func_t)
(
  QSEE_HASH_ALGO_ET   alg,
  qsee_hash_ctx       **hash_ctx
);


/**
 * @brief  This function will hash some data into the hash context
 *         structure, which Must have been initialized by
 *         tzexec_hash_init().
 *
 * @param hash_ctx[in]    The hash context
 * @param[in]             Pointer to the msg to hash
 * @param[in]             Length of the msg to hash
 *
 * @return 0 on success, negative on failure
 */
typedef int (*tzexec_hash_update_func_t)
(
  const qsee_hash_ctx   *hash_ctx,
  const uint8_t             *msg,
  uint32_t                  msg_len
);

/**
 * @brief  Compute the digest hash value
 *
 * @param hash_ctx[in]     The hash context
 * @param digest[in]       Pointer to output message digest hash
 * @param digest_len[in]   Length of the output message digest hash
 *                         buffer in bytes. Must be 20 bytes for SHA1 and
 *                         32 bytes for SHA256
 *
 * @return 0 on success, negative on failure
 */
typedef int (*tzexec_hash_final_func_t)
(
  const qsee_hash_ctx   *hash_ctx,
  uint8_t                   *digest,
  uint32_t                  digest_len
);

/**
 * @brief Release all resources with a given hash context.
 *
 * @param hash_ctx[in] Hash context to be deleted
 *
 * @return 0 on success, negative on failure
 *
 */
typedef int (*tzexec_hash_free_ctx_func_t) (qsee_hash_ctx *hash_ctx);

/**
* @brief Intialize a cipher context for encrypt/decrypt operation
*
* @param[in] alg  The algorithm standard to use
* @param[out] cipher_ctx The cipher ctx
*
* @return 0 on success, negative on failure
*
*/
typedef int (*tzexec_cipher_init_func_t)
(
  QSEE_CIPHER_ALGO_ET alg,
  qsee_cipher_ctx     **cipher_ctx
);


typedef int (*tzexec_cipher_reset_func_t)
(
  qsee_cipher_ctx     *cipher_ctx
);

typedef int (*tzexec_hash_reset_func_t)
(
  qsee_hash_ctx *hash_ctx
);

typedef int (*tzexec_hash_set_param_func_t)
(
  qsee_hash_ctx *hash_ctx,
  QSEE_HASH_PARAM_ET param_id,
  const void * param,
  uint32_t       param_len
);

typedef int (*tzexec_hmac_init_func_t)
(
  QSEE_HMAC_ALGO_ET alg,
  qsee_hmac_ctx **hmac_ctx
);

typedef int (*tzexec_hmac_update_func_t)
(
  const qsee_hmac_ctx *hmac_ctx,
  uint8_t *msg,
  uint32_t msg_len
);

typedef int (*tzexec_hmac_final_func_t)
(
  const qsee_hmac_ctx *hmac_ctx,
  uint8_t *digest,
  uint32_t digest_len
);

typedef int (*tzexec_hmac_free_ctx_func_t)
(
	qsee_hmac_ctx *hmac_ctx
);

/**
 * @brief Release all resources with a given cipher context.
 *
 * @param[in] cipher_ctx Cipher context to be deleted
 *
 * @return 0 on success, negative on failure
 *
 */
typedef int (*tzexec_cipher_free_ctx_func_t) (qsee_cipher_ctx *cipher_ctx);


/**
 * @brief Modify the parameters for a given cipher operation.
 *
 * @param[in] cipher_ctx  Cipher context
 * @param[in] param_id    The parameter to modify
 * @param[in] param       The parameter value to set.
 * @param[in] param_len   The length of the param (in bytes).
 *
 * @return 0 on success,
 * negative on failure,
 * -E_NOT_SUPPORTED if an alogirthm or parameter is not currently supported.
 *
 */
typedef int (*tzexec_cipher_set_param_func_t)
(
  qsee_cipher_ctx *cipher_ctx,
  QSEE_CIPHER_PARAM_ET param_id,
  const void *param,
  uint32_t param_len
);

/**
 * @brief Retrieve the parameters for a given cipher context.
 *
 * @param[in] cipher_ctx  Cipher context
 * @param[in] param_id    The parameter to retrieve
 * @param[in] param       The memory location to store the parameter.
 * @param[in] param_len   The length of the param (in bytes).
 *
 * @return 0 on success, negative on failure
 *
 */
typedef int (*tzexec_cipher_get_param_func_t)
(
  const qsee_cipher_ctx *cipher_ctx,
  QSEE_CIPHER_PARAM_ET  param_id,
  void *param,
  uint32_t *param_len
);

/**
 * @brief This function encrypts the passed plaintext message using
 *        the specified algorithm. The memory allocated for the
 *        ciphertext must be large enough to hold the plaintext
 *        equivalent. If the output buffer is not large enough to
 *        hold the encrypted results, an error is returned.
 *
 * @param[in] cipher_ctx         The cipher context to create
 * @param[in] pt                 The input plaintext buffer
 * @param[in] pt_len             The input plaintext buffer length (in bytes)
 * @param[in,out] ct             The output ciphertext buffer
 * @param[in,out] ct_len         The output ciphertext buffer length. This
 *                               is modified to the actual ct bytes written.
 *
 * @return E_SUCCESS if successful
 *         E_INVALID_ARG if not multiple of block length
 *         E_FAILURE otherwise
 */
typedef int (*tzexec_cipher_encrypt_func_t) 
(
const qsee_cipher_ctx *cipher_ctx,
const uint8_t *pt,
uint32_t pt_len,
uint8_t   *ct,
uint32_t  *ct_len
);

/**
 * @brief This function decrypts the passed ciphertext message using
 *        the specified algorithm. The memory allocated for the
 *        plaintext must be large enough to hold the ciphertext
 *        equivalent. If the output buffer is not large enough to
 *        hold the decrypted results, an error is returned.
 *
 * @param[in] cipher_ctx         The cipher context to create
 * @param[in] ct                 The input ciphertext buffer
 * @param[in] ct_len             The input ciphertext buffer length (in bytes)
 * @param[in,out] pt             The output plaintext buffer
 * @param[in,out] pt_len         The output plaintext buffer length. This
 *                               is modified to the actual pt bytes written.
 *
 * @return E_SUCCESS if successful
 *         E_INVALID_ARG if not multiple of block length
 *         E_FAILURE otherwise
 */
typedef int (*tzexec_cipher_decrypt_func_t)
(
  const qsee_cipher_ctx* cipher_ctx,
  const uint8_t* ct,
  uint32_t ct_len,
  uint8_t* pt,
  uint32_t* pt_len
);



/**
 * @brief Create a hash MAC per FIPS pub 198 using the specified hash algorithm.
 *
 * @param[in] msg             - Pointer to message to be authenticated
 * @param[in] msg_len         - Length of message in bytes
 * @param[in] key             - Pointer to input key to HMAC algorithm
 * @param[in] key_len         - Length of input key in bytes
 * @param[out] msg_digest     - Pointer to message digest (memory provided by caller)
 *
 * @return 0 on success, negative on failure
 */
typedef int (*tzexec_hmac_func_t) (
                                  QSEE_HMAC_ALGO_ET alg,
                                  const uint8_t *msg,
                                  uint32_t msg_len,
                                  const uint8_t *key,
                                  uint16_t key_len,
                                  uint8_t *msg_digest);

/**
 * @brief Create a hash MAC per FIPS pub 198 using the specified hash algorithm.
 *
 * @param[in] msg             - Pointer to message to be authenticated
 * @param[in] msg_len         - Length of message in bytes
 * @param[in] key             - Pointer to input key to HMAC algorithm
 * @param[in] key_len         - Length of input key in bytes
 * @param[out] msg_digest     - Pointer to message digest (memory provided by caller)
 * @param[in] cmac_len        - Length of CMAC in bytes
 *  
 * @return 0 on success, negative on failure
 */
typedef int (*tzexec_cmac_func_t) (
                                  QSEE_CMAC_ALGO_ET alg,
                                  const uint8_t *msg,
                                  uint32_t msg_len,
                                  const uint8_t *key,
                                  uint32_t key_len,
                                  uint8_t *cmac_digest,
                                  uint32_t cmac_len); 

/**
 * @brief Release all resources with a given prng context.
 *
 * @param[in] out     The output data buffer
 * @param[in] out_len The output data length. The out_len
 *                    must be at most QSEE_MAX_PRNG bytes.
 *
 * @return number of bytes read
 *
 */
typedef uint32_t (*tzexec_prng_get_data_func_t) (
                                              uint8_t *out,
                                              uint32_t out_len);

/**
  Derives a Key using known label, context strings and using either 
  the Hardware Key [Primary or Secondary](for unsigned image) 
  and application derived key (signed image) or a user provided 
  Key. 

  This is an implementation of the key derivation algorithm based on
  AES_CMAC from FIPS Special Publication 800-38B.

  @param[in]  input_key        Pointer to the input Key.
                               This should be NULL if Primary or Secondary
                               HW Key are to be used for Ciphering.
			       
  @param[in]  input_key_len    Length of the 'input_key' in bytes.
                               The value of this is not acted on if the
                               'input_key' is NULL.

  @param[in]  label            Pointer to a string constant.

  @param[in]  label_len        Length of the string constant 'label'.

  @param[in]  context          Pointer to a string constant.

  @param[in]  context_len      Length of the string constant 'context'.

  @param[out] output_key       Pointer to the generated Key.
                               The Memory for this should be provided by the caller.
  @param[in]  output_key_len   Length of the Key to be generated
                               in bytes. This is size of the
                               buffer that the 'output_key'
                               points to.
       
  @return
  zero on success, failure otherwise
 
  @dependencies
  The memory for the 'output_key' should be allocated by the caller
  according to the 'output_key_len'.
*/

typedef int (*tzexec_kdf_func_t)(
                                 const void  *input_key,
                                 uint32_t    input_key_len,
                                 const void  *label,
                                 uint32_t    label_len,
                                 const void  *context,
                                 uint32_t    context_len,
                                 void        *output_key,
                                 uint32_t    output_key_len);

/**
 * @brief Lock or unlock the crypto hardware by XPU protecting the resource.
 *        This function may do nothing based if the chipset supports dedicated
 *        TZ crypto engine.
 *
 * @param[in] lock  Set to TRUE to lock and FALSE to unlock
 *
 * @return TRUE if hardware can be locked; otherwise FALSE is returned.
 */
typedef boolean (*tzexec_lock_crypto_engine_func_t)(boolean lock);
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
typedef struct
{
  uint32_t                            hash_id;
  tzexec_hash_func_t                tzexec_hash;

  uint32_t                            hash_init_id;
  tzexec_hash_init_func_t           tzexec_hash_init;

  uint32_t                            hash_update_id;
  tzexec_hash_update_func_t         tzexec_hash_update;

  uint32_t                            hash_final_id;
  tzexec_hash_final_func_t          tzexec_hash_final;

  uint32_t                            hash_free_ctx_id;
  tzexec_hash_free_ctx_func_t       tzexec_hash_free_ctx;

  uint32_t                            cipher_init_id;
  tzexec_cipher_init_func_t         tzexec_cipher_init;

  uint32_t                            cipher_free_ctx_id;
  tzexec_cipher_free_ctx_func_t     tzexec_cipher_free_ctx;

  uint32_t                            cipher_set_param_id;
  tzexec_cipher_set_param_func_t    tzexec_cipher_set_param;

  uint32_t                            cipher_get_param_id;
  tzexec_cipher_get_param_func_t    tzexec_cipher_get_param;

  uint32_t                            cipher_encrypt_id;
  tzexec_cipher_encrypt_func_t      tzexec_cipher_encrypt;

  uint32_t                            cipher_decrypt_id;
  tzexec_cipher_decrypt_func_t      tzexec_cipher_decrypt;

  uint32_t                            hmac_id;
  tzexec_hmac_func_t                tzexec_hmac;

  uint32_t                            prng_get_data_id;
  tzexec_prng_get_data_func_t       tzexec_prng_get_data;

  uint32_t                            lock_crypto_engine;
  tzexec_lock_crypto_engine_func_t  tzexec_lock_crypto_engine;

  uint32_t                            kdf_id;
  tzexec_kdf_func_t                 tzexec_kdf;

  uint32_t                           cmac_id;
  tzexec_cmac_func_t                 tzexec_cmac;

  uint32_t                           cipher_reset_id;
  tzexec_cipher_reset_func_t         tzexec_cipher_reset;

  uint32_t                           hash_reset_id;
  tzexec_hash_reset_func_t           tzexec_hash_reset;
  
  uint32_t                           hash_set_param_id;
  tzexec_hash_set_param_func_t       tzexec_hash_set_param;
  
  uint32_t                           hmac_init_id;
  tzexec_hmac_init_func_t            tzexec_hmac_init;

  uint32_t                           hmac_update_id;
  tzexec_hmac_update_func_t          tzexec_hmac_update;

  uint32_t                           hmac_final_id;
  tzexec_hmac_final_func_t           tzexec_hmac_final;

  uint32_t                           hmac_free_ctx_id;
  tzexec_hmac_free_ctx_func_t        tzexec_hmac_free_ctx;


  uint32_t                            end_of_table_id;
  void*                             null_ptr;
} tzexec_func_group_crypto_t, *tzexec_func_group_crypto_ptr;


#define TZEXEC_FUNC_CRYPTO_HASH_ID                        1
#define TZEXEC_FUNC_CRYPTO_HASH_INIT_ID                   2
#define TZEXEC_FUNC_CRYPTO_HASH_UPDATE_ID                 3
#define TZEXEC_FUNC_CRYPTO_HASH_FINAL_ID                  4
#define TZEXEC_FUNC_CRYPTO_HASH_FREE_CTX_ID               5
#define TZEXEC_FUNC_CRYPTO_CIPHER_INIT_ID                 6
#define TZEXEC_FUNC_CRYPTO_CIPHER_FREE_CTX_ID             7
#define TZEXEC_FUNC_CRYPTO_CIPHER_SET_PARAM_ID            8
#define TZEXEC_FUNC_CRYPTO_CIPHER_GET_PARAM_ID            9
#define TZEXEC_FUNC_CRYPTO_CIPHER_ENCRYPT_ID             10
#define TZEXEC_FUNC_CRYPTO_CIPHER_DECRYPT_ID             11
#define TZEXEC_FUNC_CRYPTO_HMAC_ID                       12
#define TZEXEC_FUNC_CRYPTO_PRNG_GET_DATA_ID              13
#define TZEXEC_FUNC_CRYPTO_LOCK_CRYPTO_ENGINE_ID         14
#define TZEXEC_FUNC_CRYPTO_KDF                           15
#define TZEXEC_FUNC_CRYPTO_CMAC_ID                       16

/*----------------------------------------------------------------------------
                      QSEE Function Group: Core
 -----------------------------------------------------------------------------*/

/**
 * Given an area of memory, this function will add XPU protection to that area
 * so that only the given region (subsystem) may have rights to access it
 *
 * @param[in] region Named memory area associated with a subsystem
 * @param[in] start  Beginning of memory region to alter permssions on
 * @param[in] end    End of memory region to alter permssions on
 * @param[in] lock   Whether to enable XPU protection or disable XPU protection
 */
typedef int (*qsee_protect_memory_func_t)
(
   uint32_t region,
   uint32_t start,
   uint32_t end,
   boolean lock
);

/**
 * @brief      Query whether the given SW fuse has been blown
 *
 * @param[in]  fuse_num    the SW fuse to query
 * @param[out] is_blown    whether the given fuse has been blown
 * @param[in]  is_blown_sz size of the return pointer
 *
 * @return     0 on success, negative on failure
 * @warning    function is not thread-safe
 */
typedef int (*qsee_is_sw_fuse_blown_t)
(
  uint32_t      fuse_num,
  boolean*    is_blown,
  uint32_t      is_blown_sz
);

/**
 * @brief     Change the state of the SW fuse
 * @param[in] fuse_num the SW fuse to query
 * @return    0 on success, negative on failure
 */
typedef int (*qsee_blow_sw_fuse_t)(uint32_t fuse_num);

/**
 * @brief     Reads the JTAG ID
 * @return    JTAG ID value
 */
typedef int (*qsee_read_jtag_id_t)(void);

/**
 * @brief     Reads the serial number from PTE chain
 * @return    Serial number
 */
typedef int (*qsee_read_serial_num_t)(void);

/**
 * @brief     QFPROM write row
 * @param[in] raw_row_address
 * @param[in] row_data
 * @param[in] bus_clk_khz
 * @param[in] qfprom_api_status
 * @return    0 on success, negative on failure
 */
typedef int (*qsee_fuse_write_t)
(
  uint32_t    raw_row_address,
  uint32_t*   row_data,
  uint32_t    bus_clk_khz,
  uint32_t*   qfprom_api_status
);

/**
 * @brief     QFPROM read row
 * @param[in] row_address
 * @param[in] addr_type
 * @param[in] row_data
 * @param[in] qfprom_api_status
 * @return    0 on success, negative on failure
 */
typedef int (*qsee_fuse_read_t)
(
  uint32_t   row_address,
  int32_t    addr_type,
  uint32_t*  row_data,
  uint32_t*  qfprom_api_status
);

/**
 * @brief     Enable JTAG debug fuse
 * @param[in] debug boolean value dictating enablement of fuse
 */
typedef void (*qsee_jtag_override_t)(uint32_t debug);

/**
 * Tests whether all of the range [\c start, \c start + \c len] is in
 * non-secure memory. This is a convenience function to access \c
 * tzbsp_is_ns_area. \c NULL is a valid value for \c start, because
 * physical addressing is used.
 *
 * @param [in] start Start of the memory range, physical address,
 * included in the range.
 *
 * @param [in] len Length of the memory range in bytes.
 *
 * @return \c TRUE if the entire area is in non-secure memory. \c
 * FALSE if the area contains secure memory.
 */
typedef boolean (*qsee_is_ns_range_t)(const void* start, uint32_t len);

/**
 * Tests whether all of the range [\c start, \c end] is in secure
 * memory and also tagged for a particular use case.
 *  
 * @param [in] tag  Tag ID of the memory it should be tagged with
 *  
 * @param [in] start Start of the memory range, physical address,
 * included in the range.
 *
 * @param [in] end End of the memory range, physical address,
 * included  in the range.
 *
 * @return \c TRUE if the entire area is in secure memory. \c FALSE
 *  if the area contains non-secure memory.
 */
typedef boolean (*qsee_is_s_tag_area_t)
( 
  uint32_t  tag,
  uint32_t  start,
  uint32_t  end
);


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
typedef struct
{
  uint32_t                                  protect_memory_id;
  qsee_protect_memory_func_t              qsee_protect_memory;

  uint32_t                                  is_sw_fuse_blown_id;
  qsee_is_sw_fuse_blown_t                 qsee_is_sw_fuse_blown;

  uint32_t                                  blow_sw_fuse_id;
  qsee_blow_sw_fuse_t                     qsee_blow_sw_fuse;

  uint32_t                                  read_jtag_id_id;
  qsee_read_jtag_id_t                     qsee_read_jtag_id;

  uint32_t                                  read_serial_num_id;
  qsee_read_serial_num_t                  qsee_read_serial_num;

  uint32_t                                  fuse_write_id;
  qsee_fuse_write_t                       qsee_fuse_write;

  uint32_t                                  fuse_read_id;
  qsee_fuse_read_t                        qsee_fuse_read;

  uint32_t                                  jtag_override_id;
  qsee_jtag_override_t                    qsee_jtag_override;

  uint32_t                                  is_ns_range;
  qsee_is_ns_range_t                      qsee_is_ns_range;
  
  uint32_t                                  is_s_tag_area_id;
  qsee_is_s_tag_area_t                    qsee_is_s_tag_area;

  uint32_t                                  end_of_table_id;
  void*                                   null_ptr;
} qsee_func_group_core_t, *qsee_func_group_core_ptr;

#define TZEXEC_FUNC_CORE_PROTECT_MEM_ID                   1

/*----------------------------------------------------------------------------
                      QSEE Function Group: SecureChannel
 -----------------------------------------------------------------------------*/

/** 
  @brief 
  This is used to secure the the input message
   
  @details
  The output buffer should be large enough to hold the encrypted message 
  and some internal headers and possible padding. Recommended output buffer size 
  is atleast input_msg_len + 100 bytes. The memory has to be managed by the caller.
   
  @param[in] ssid                    subsystem ID
  @param[in] cid                     Client ID
  @param[in] input_msg_ptr           Pointer to the plaintext data.
  @param[in] input_msg_len           Length of the plaintext in bytes.
  @param[in, out] output_msg_ptr     Pointer to the buffer to hold the secure blob 
                                     (memory provided by the caller)
  @param[in,out] output_msg_len_ptr  Size of the above buffer in bytes, set to 
                                     the length of the secure blob by the function.

  @return
  E_SUCCESS - Successful.  \n 
  E_FAILURE - Operation failed. 
 
  @dependencies
  Secure Channel has to be established successfully.

  @sideeffects
  None
*/
typedef int (*qsee_secure_message_func_t)
(
  uint8_t                  ssid,
  uint8_t                  cid,
  const uint8_t*           input_msg_ptr,
  uint32_t                 input_msg_len,
  uint8_t*                 output_msg_ptr,
  uint32_t*                output_msg_len_ptr
);

/** 
  @brief  
  This is used to authenticate and decrypt the secure blob
   
  @details 
  The output buffer should be large enough to hold the decrypted message 
  Recommended output buffer size is atleast input_msg_len. 
  The memory has to be managed by the caller.
   
  @param[in] ssid                    subsystem ID
  @param[in] cid                     Client ID
  @param[in] input_msg_ptr           Pointer to the secure blob.
  @param[in] input_msg_len           Length of the secure blob in bytes.
  @param[in, out] output_msg_ptr     Pointer to the buffer to hold the decrypted data 
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
typedef int (*qsee_authenticate_decrypt_message_func_t)
(
  uint8_t                  ssid,
  uint8_t                  cid,
  const uint8_t*           input_msg_ptr,
  uint32_t                 input_msg_len,
  uint8_t*                 output_msg_ptr,
  uint32_t*                output_msg_len_ptr
);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
typedef struct
{
  uint32_t                                    secure_message_id;
  qsee_secure_message_func_t                qsee_secure_message;

  uint32_t                                    authenticate_decrypt_message_id;
  qsee_authenticate_decrypt_message_func_t  qsee_authenticate_decrypt_message;

  uint32_t                                    end_of_table_id;
  void*                                     null_ptr;
} qsee_func_group_securechannel_t, *qsee_func_group_securechannel_ptr;

#define QSEE_FUNC_SECURECHANNEL_SECURE_MESSAGE_ID                1
#define QSEE_FUNC_SECURECHANNEL_AUTHENTICATE_DECRYPT_MESSAGE_ID  2

/*----------------------------------------------------------------------------
                      TZEXEC Function Group: MMU Cache
 -----------------------------------------------------------------------------*/

/**
 * Invalidate the entire unified TLB.
 */
typedef void (*tzexec_tbl_inval_all_func_t)(void);


/**
 * Invalidate the entire instruction TLB.
 */
typedef void (*tzexec_itbl_inval_all_func_t)(void);

/**
 * Invalidate the entire data TLB.
 */
typedef void (*tzexec_dtbl_inval_all_func_t)(void);

/**
 * Flushes the entire cache system.  The data cache flush is achieved
 * using atomic clean / invalidates working outwards from L1
 * cache. This is done using Set/Way based cache maintainance
 * instructions.  The instruction cache can still be invalidated back
 * to the point of unification in a single instruction.  This also
 * flushes the branch target cache.
 */
typedef void (*tzexec_flush_cache_func_t)(void);

/**
 * Inval the data cache system.  The invalidate is achieved using
 * invalidate operations working outwards from L1 cache. This is done
 * using Set/Way based cache maintainance instructions.
 */
typedef void (*tzexec_dcache_inval_all_func_t)(void);

/**
 * Flush the data cache system.  The flush is achieved using atomic
 * clean / invalidates working outwards from L1 cache. This is done
 * using Set/Way based cache maintainance instructions.
 */
typedef void (*tzexec_dcache_flush_all_func_t)(void);

/**
 * Clean the data cache system.  The clean is achieved using clean
 * operations working outwards from L1 cache. This is done using
 * Set/Way based cache maintainance instructions.
 */
typedef void (*tzexec_dcache_clean_all_func_t)(void);

/**
 * Invalidate a memory region in the cache. Please note that the data
 * in the cache will not be written back to the main memory. However,
 * any further accesses to data in this region would result in a
 * cache-miss.
 *
 * @param addr Start address of memory region
 * @param length Length of memory region
 */
typedef void (*tzexec_dcache_inval_region_func_t)(void *addr, unsigned int length);

/**
 * Cleans and invalidates a memory region in the cache. Please note
 * that the data in the cache would be written back to main memory if
 * dirty and the region invalidated. So any further access to the data
 * would result in a cache-miss.
 *
 * @param addr Start address of memory region
 * @param length Length of memory region
 */
typedef void (*tzexec_dcache_flush_region_func_t)(void *addr, uint32_t  len);

/**
 * Cleans a memory region in the cache. Please note that this will
 * write back any data that is dirty but it will not invalidate the
 * cache region.  So any further accesses to data in this region would
 * result in a cache-hit.
 *
 * @param addr Start address of memory region
 * @param length Length of memory region
 */
typedef void (*tzexec_dcache_clean_region_func_t)(void *addr, unsigned int length);


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
typedef struct
{
  uint32_t                                  invalidate_tbl_id;
  tzexec_tbl_inval_all_func_t             tzexecInvalidateTlb;

  uint32_t                                  itbl_inval_all_id;
  tzexec_itbl_inval_all_func_t            tzexec_itbl_inval_all;

  uint32_t                                  dtbl_inval_all_id;
  tzexec_dtbl_inval_all_func_t            tzexec_dtbl_inval_all;

  uint32_t                                  flush_cache_id;
  tzexec_flush_cache_func_t               tzexec_flush_cache;

  uint32_t                                  dcache_inval_all_id;
  tzexec_dcache_inval_all_func_t          tzexec_dcache_inval_all;

  uint32_t                                  dcache_flush_all_id;
  tzexec_dcache_flush_all_func_t          tzexec_dcache_flush_all;

  uint32_t                                  dcache_clean_all_id;
  tzexec_dcache_clean_all_func_t          tzexec_dcache_clean_all;

  uint32_t                                  dcache_inval_region_id;
  tzexec_dcache_inval_region_func_t       tzexec_dcache_inval_region;

  uint32_t                                  dcache_flush_region_id;
  tzexec_dcache_flush_region_func_t       tzexec_dcache_flush_region;

  uint32_t                                  dcache_clean_region_id;
  tzexec_dcache_clean_region_func_t       tzexec_dcache_clean_region;

  uint32_t                                  end_of_table_id;
  void*                                   null_ptr;
} tzexec_func_group_mmu_cache_t, *tzexec_func_group_mmu_cache_ptr;


#define TZEXEC_FUNC_MMU_CACHE_TBL_INVAL_ALL_ID              1
#define TZEXEC_FUNC_MMU_CACHE_ITBL_INVAL_ALL_ID             2
#define TZEXEC_FUNC_MMU_CACHE_DTBL_INVAL_ALL_ID             3
#define TZEXEC_FUNC_MMU_CACHE_FLUSH_CACHE_ID                4
#define TZEXEC_FUNC_MMU_CACHE_DCACHE_INVAL_ALL_ID           5
#define TZEXEC_FUNC_MMU_CACHE_DCACHE_FLUSH_ALL_ID           6
#define TZEXEC_FUNC_MMU_CACHE_DCACHE_CLEAN_ALL_ID           7
#define TZEXEC_FUNC_MMU_CACHE_DCACHE_INVAL_REGION_ID        8
#define TZEXEC_FUNC_MMU_CACHE_DCACHE_FLUSH_REGION_ID        9
#define TZEXEC_FUNC_MMU_CACHE_DCACHE_CLEAN_REGION_ID        10

/*----------------------------------------------------------------------------
                      TZEXEC Function Groups
 -----------------------------------------------------------------------------*/


typedef struct
{
  uint32_t                            func_group_boot_id;
  tzexec_func_group_boot_ptr        tzexec_func_group_boot;

  uint32_t                            func_group_int_id;
  tzexec_func_group_int_ptr         tzexec_func_group_int;

  uint32_t                            func_group_timer_id;
  tzexec_func_group_timer_ptr       tzexec_func_group_timer;

  uint32_t                            func_group_crypto_id;
  tzexec_func_group_crypto_ptr      tzexec_func_group_crypto;

  uint32_t                            func_group_core_id;
  qsee_func_group_core_ptr           qsee_func_group_core;

  uint32_t                            func_group_cache_id;
  tzexec_func_group_mmu_cache_ptr   tzexec_func_group_mmu_cache;

  uint32_t                             func_group_securechannel_id;
  qsee_func_group_securechannel_ptr  qsee_func_group_securechannel;

  uint32_t                            end_of_table_id;
  void*                             null_ptr;
} tzexec_func_groups_t, *tzexec_func_groups_ptr;

#define TZEXEC_GROUP_BOOT_ID          1
#define TZEXEC_GROUP_INT_ID           2
#define TZEXEC_GROUP_TIMER_ID         3
#define TZEXEC_GROUP_CRYPTO_ID        4
#define TZEXEC_GROUP_CORE_ID          5
#define TZEXEC_GROUP_MMU_CACHE_ID     6
#define QSEE_GROUP_SECURECHANNEL_ID   7

/*Function types for passting the table of func ptrs to apps*/
typedef int (*qsee_timer_start_type)(uint32_t timeout, boolean periodic);
typedef void (*qsee_timer_stop_type)(void);
typedef void (*qsee_timer_enable_int_type)(void);
typedef void (*qsee_timer_disable_int_type)(void);

typedef uint32_t (*qsee_register_int_type)(uint32_t intNum, uint32_t flags, boolean isEnabled);
typedef void (*qsee_deregister_int_type)(uint32_t intNum);
typedef uint32_t (*qsee_read_int_mask_type)(void);
typedef uint32_t (*qsee_clear_int_mask_type)(uint32_t mask);

int qsee_timer_start(uint32_t timeout, boolean periodic);
void qsee_timer_stop(void);
void qsee_timer_enable_int(void);
void qsee_timer_disable_int(void);

uint32_t qsee_register_int(uint32_t intNum, uint32_t flags, boolean isEnabled);
void qsee_deregister_int(uint32_t intNum);
uint32_t qsee_read_int_mask(void);
uint32_t qsee_clear_int_mask(uint32_t mask);

uint32_t qsee_get_ddr_ext(void);

#endif /* __TZAPPS_EXEC_H_ */
