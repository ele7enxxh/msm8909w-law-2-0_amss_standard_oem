#include "tzos_exec.h"

uint64_t qsee_get_uptime(void);
void qsee_wdog_stop(void);
void qsee_wdog_pet(void);

void qsee_get_request_info(void* buffer, unsigned int num_elements);
void qsee_save_thread_regs(void* buffer);

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * @brief 
 *    API for a secure application to prepare a message that can be sent to 
 *    another secure application. This API writes the encryped message into 
 *    the supplied output buffer, prepended with a header and appended with   
 *    a MAC. This output buffer can be given to the receiving app and then 
 *    passed into qsee_decapsulate_inter_app_message to be authenticated and 
 *    decrypted.  The actual data exchange (passing the encrypted bufer) 
 *    between the secure applications can be done between their non secure 
 *    clients running in the HLOS. 
 *
 * @param[in] dest_app_name 
 * @param[in] in_buf 
 * @param[in] in_len 
 * @param[out] out_buf 
 * @param[in] out_len 
 * @param[out] out_len_ptr 
 *
 * @requires 
 *   in_len should indicate the exact size of in_buf.
 *   out_len should indicate the exact size of out_buf, and this should be
 *     greater than in_len by 144 bytes to allow room for the header and MAC.
 *     out_len is modified to reflect the exact length of the data 
 *     written into out_buf.   
 *
 * @return 0 indicates sucess, all other values indicate failure and
 *     correspond to a specific error code. 
 */
int qsee_encapsulate_inter_app_message(char* dest_app_name, uint8* in_buf, 
  uint32 in_len, uint8* out_buf, uint32 out_len, uint32* out_len_ptr);


/**
 * @brief 
 *    API for a secure application to decapsulate (ie decrypt) a message 
 *  from another secure application.  This API authenticates the message, 
 *  decrypts the input buffer, and writes the plaintext message into the 
 *  supplied output buffer.  The input buffer must have been prepared by 
 *  qsee_encapsulate_inter_app_message(), containing a header and MAC. 
 *
 * @param[out] source_app_name 
 * @param[in] in_buf 
 * @param[in] in_len 
 * @param[out] out_buf 
 * @param[in] out_len 
 * @param[out] out_len_ptr 
 *
 * @requires 
 *   source_app_name is modified to return the sending application's identity
 *      to the caller.  
 *   in_len should indicate the exact size of in_buf. 
 *   out_len should indicate the exact size of out_buf, and this should be 
 *      greater than or equal to in_len. 
 *
 * @return 0 indicates sucess, all other values indicate failure and 
 *     correspond to a specific error code. 
 */
int qsee_decapsulate_inter_app_message(char* source_app_name, uint8* in_buf, 
  uint32 in_len, uint8* out_buf, uint32 out_len, uint32* out_len_ptr);


 /**
 * @brief Intialize a cipher context for encrypt/decrypt operation
 *
 * @param[in] alg  The algorithm standard to use
 * @param[out] cipher_ctx The cipher ctx
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_cipher_init(QSEE_CIPHER_ALGO_ET alg,
                     qsee_cipher_ctx  **cipher_ctx);

/**
 * @brief Release all resources with a given cipher context.
 *
 * @param[in] cipher_ctx Cipher context to be deleted
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_cipher_free_ctx(qsee_cipher_ctx *cipher_ctx);

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
int qsee_cipher_set_param(qsee_cipher_ctx *cipher_ctx,
                          QSEE_CIPHER_PARAM_ET param_id,
                          const void *param,
                          uint32_t param_len);
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
int qsee_cipher_get_param(const qsee_cipher_ctx *cipher_ctx,
                          QSEE_CIPHER_PARAM_ET param_id,
                          void *param,
                          uint32_t *param_len);

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
int qsee_cipher_encrypt(const qsee_cipher_ctx *cipher_ctx,
                        const uint8_t *pt,
                        uint32_t pt_len,
                        uint8_t *ct,
                        uint32_t *ct_len);

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
int qsee_cipher_decrypt(const qsee_cipher_ctx *cipher_ctx,
                        const uint8_t *ct,
                        uint32_t ct_len,
                        uint8_t* pt,
                        uint32_t *pt_len);

/**
 * @brief Lock or unlock the crypto hardware by XPU protecting the resource.
 *        This function may do nothing based if the chipset supports dedicated
 *        TZ crypto engine.
 *
 * @param[in] lock  Set to TRUE to lock and FALSE to unlock
 *
 * @return TRUE if hardware can be locked; otherwise FALSE is returned.
 */
boolean qsee_crypto_lock_engine(boolean lock);


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
int qsee_hash(QSEE_HASH_ALGO_ET alg,
              const uint8_t *msg,
              uint32_t msg_len,
              uint8_t *digest,
              uint32_t digest_len);

/**
 * @brief Intialize a hash context for update and final functions.
 *
 * @param alg[in] The algorithm standard to use
 * @param hash_ctx[out] The hash context
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_hash_init(QSEE_HASH_ALGO_ET alg,
                   qsee_hash_ctx **hash_ctx);

/**
 * @brief  This function will hash some data into the hash context
 *         structure, which Must have been initialized by
 *         qsee_hash_init().
 *
 * @param hash_ctx[in]    The hash context
 * @param[in]             Pointer to the msg to hash
 * @param[in]             Length of the msg to hash
 *
 * @return 0 on success, negative on failure
 *
 * @see qsee_hash_init
 */
int qsee_hash_update(const qsee_hash_ctx  *hash_ctx,
                     const uint8_t           *msg,
                     uint32_t                msg_len);

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
 *
 * @see qsee_hash_init
 */
int qsee_hash_final(const qsee_hash_ctx  *hash_ctx,
                    uint8_t                 *digest,
                    uint32_t                digest_len);

/**
 * @brief Release all resources with a given hash context.
 *
 * @param hash_ctx[in] Hash context to be deleted
 *
 * @return 0 on success, negative on failure
 *
 */
int qsee_hash_free_ctx(qsee_hash_ctx *hash_ctx);

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
int qsee_hmac(QSEE_HMAC_ALGO_ET alg, const uint8_t *msg, uint32_t msg_len,
              const uint8_t *key, uint16_t key_len, uint8_t *msg_digest);

/** 
 * @brief Create a cipher MAC per FIPS pub 198 using the specified hash algorithm.
 *
 * @param[in] msg             - Pointer to message to be authenticated
 * @param[in] msg_len         - Length of message in bytes
 * @param[in] key             - Pointer to input key to CMAC algorithm
 * @param[in] key_len         - Length of input key in bytes
 * @param[out] cmac_digest    - Pointer to cmac digest (memory provided by caller)
 * @param[in] cmac_len        - Length of CMAC in bytes
 *
 * @return 0 on success, negative on failure
*/
int qsee_cmac(QSEE_CMAC_ALGO_ET alg, const uint8_t *msg, uint32_t msg_len,
              const uint8_t *key, uint32_t key_len, uint8_t *cmac_digest,
              uint32_t cmac_len);

/** 
  @brief 
  This is used to secure the the input message
   
  @details
  The output buffer should be large enough to hold the encrypted message 
  and some internal headers and possible padding. Recommended output buffer size 
  is atleast input_msg_len + 100 bytes. The memory has to be managed by the caller.
   
  @param[in] ssid                    Subsystem ID
  @param[in] cid                     Client ID
  @param[in] input_msg_ptr           Pointer to the plaintext data.
  @param[in] input_msg_len           Length of the plaintext in bytes.
  @param[out] output_msg_ptr         Pointer to the buffer to hold the secure blob 
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
int qsee_secure_message
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
   
  @param[in] ssid                    Subsystem ID
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
int qsee_authenticate_decrypt_message
(
  uint8_t                  ssid,
  uint8_t                  cid,
  const uint8_t*           input_msg_ptr,
  uint32_t                 input_msg_len,
  uint8_t*                 output_msg_ptr,
  uint32_t*                output_msg_len_ptr
);


int qsee_protect_memory
(
  uint32_t    region, 
  uint32_t    start,
  uint32_t    end, 
  boolean   lock
);

/**
 * @brief     Reads the JTAG ID
 * @return    JTAG ID value
 */
int qsee_read_jtag_id(void);

/**
 * @brief     Reads the serial number from PTE chain
 * @return    Serial number
 */
int qsee_read_serial_num(void);

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
boolean qsee_is_ns_range(const void* start, uint32_t len);

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
boolean qsee_is_s_tag_area
( 
  uint32_t                    tag,
  uint32_t                    start,
  uint32_t                    end
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
int qsee_is_sw_fuse_blown
(
  uint32_t       fuse_num,
  boolean*          is_blown,
  uint32_t            is_blown_sz
);

/**
 * @brief     Change the state of the SW fuse
 * @param[in] fuse_num the SW fuse to query
 * @return    0 on success, negative on failure
 */
int qsee_blow_sw_fuse(uint32_t fuse_num);



/**
 * @brief     QFPROM write row
 * @param[in] raw_row_address
 * @param[in] row_data
 * @param[in] bus_clk_khz
 * @param[in] qfprom_api_status
 * @return    0 on success, negative on failure
 */
int qsee_fuse_write
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
int qsee_fuse_read
(
  uint32_t   row_address,
  int32_t    addr_type,
  uint32_t*  row_data,
  uint32_t*  qfprom_api_status
);

uint32_t qsee_prng_getdata(uint8_t *out, uint32_t out_len);

int qsee_ssd_decrypt_img(uint32_t** img_ptr, uint32_t *img_len_ptr);

/**
 * Reads the status of the HDMI link and hardware HDCP
 * @param [out] hdmi_enable  HDMI output enabled
 * @param [out] hdmi_sense HDMI sense
 * @param [out] hdcp_auth HDCP authentication success.
 * @return 0 on success
 */
int qsee_hdmi_status_read
(
  uint32_t* hdmi_enable,
  uint32_t* hdmi_sense,
  uint32_t* hdcp_auth
);
/**
 * @brief     Get tz app id, the app id is got from qsee app certificate
 * @return    64 bits long
 */
int qsee_get_tz_app_id(uint8_t * tz_app_id, uint32_t id_buf_len);

/**
 * @brief     Reads the hardware version
 * @return    hw version
 */
uint32_t qsee_read_hw_version(void);
