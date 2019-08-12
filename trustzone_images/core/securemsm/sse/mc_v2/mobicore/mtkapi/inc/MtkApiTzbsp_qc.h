/**
 * Copyright (C) 2012, Giesecke&Devrient GmbH
 * Axel Heider (axh)
 *
 * Architecture: ARM
 * Platform:     Qualcomm MSM8960
 *
 * This file hold the whole TZBSP interface, including the definition, types
 * etc. It largely based on the contents of the file tzbsp.h from Qualcomm's
 * TZBSP package.
 *
 */


#ifndef __MTK_API_TZBSP_QC__
#define __MTK_API_TZBSP_QC__

//------------------------------------------------------------------------------
// Error Codes from TZBSP API Functions
//------------------------------------------------------------------------------

// from modem_proc/core/api/services/IxErrno.h
typedef enum {
    /*** Generic outcomes of operations ***/
    E_SUCCESS            =  0,    /* Operation successful                  */
    E_FAILURE            =  1,    /* Operation failed due to unknown err   */
    E_NOT_ALLOWED        =  2,    /* Operation currently not allowed       */
    E_NOT_AVAILABLE      =  3,    /* Operation currently not available     */
    E_NOT_SUPPORTED      =  4,    /* Operation not yet implemented         */
    E_CANCELED           =  5,    /* Operation was scheduled but canceled  */
    E_ABORTED            =  6,    /* Operation was started but interrp'd   */
    E_INTERRUPTED        =  7,    /* Operation was started but interrp'd   */
    E_DEADLOCK           =  8,    /* Operation would cause a deadlock      */
    E_AGAIN              =  9,    /* Caller should retry operation         */
    E_RESET              = 10,    /* Executing module was reset            */
    E_WOULD_BLOCK        = 11,    /* nonblocking IO operation would block  */
    E_IN_PROGRESS        = 12,    /* Operation in progress                 */
    E_ALREADY_DONE       = 13,    /* Operation already completed           */

    /*** Memory management related error conditions ***/
    E_NO_MEMORY          = 15,    /* Allocation from a memory pool failed  */

    /*** Parameter or data parsing related error conditions ***/
    E_INVALID_ARG        = 16,    /* Arg is not recognized                 */
    E_OUT_OF_RANGE       = 17,    /* Arg value is out of range             */
    E_BAD_ADDRESS        = 18,    /* Ptr arg is bad address                */
    E_NO_DATA            = 19,    /* Expected data, received none          */
    E_BAD_DATA           = 20,    /* Data failed sanity check, e.g. CRC    */
    E_DATA_INVALID       = 21,    /* Data is correct, but contents invalid */
    E_DATA_EXPIRED       = 22,    /* Data is not yet or not any more valid */
    E_DATA_TOO_LARGE     = 23,    /* Data is too large to process          */

    E_RESERVED           = 0x7FFFFFFF

} IxErrnoType;

/*----------------------------------------------------------------------------
                      TZBSP Function Groups Crypto
 -----------------------------------------------------------------------------*/

#define TZBSP_AES128_IV_SIZE         16
#define TZBSP_AES128_KEY_SIZE        16
#define TZBSP_AES256_IV_SIZE         16
#define TZBSP_AES256_KEY_SIZE        32

/** Cipher supported algorithms */
typedef enum
{
   TZBSP_CIPHER_ALGO_AES_128    = 0,
   TZBSP_CIPHER_ALGO_AES_256    = 1,
} TZBSP_CIPHER_ALGO_ET;

/** Support for SHA1 and SHA256 for hash */
typedef enum
{
    TZBSP_HASH_NULL         = 1, ///< Do not perform any hashing
    TZBSP_HASH_SHA1         = 2,
    TZBSP_HASH_SHA256       = 3
} TZBSP_HASH_ALGO_ET;

typedef enum
{
    TZBSP_CIPHER_PARAM_KEY      = 0,
    TZBSP_CIPHER_PARAM_IV       = 1,
    TZBSP_CIPHER_PARAM_MODE     = 2,
    TZBSP_CIPHER_PARAM_PAD      = 3
} TZBSP_CIPHER_PARAM_ET;

/** Supported modes of operation */
typedef enum
{
    TZBSP_CIPHER_MODE_ECB       = 0,
    TZBSP_CIPHER_MODE_CBC       = 1,
    TZBSP_CIPHER_MODE_CTR       = 2,
} TZBSP_CIPHER_MODE_ET;

typedef enum
{
    TZBSP_CIPHER_PAD_ISO10126   = 0,
    TZBSP_CIPHER_PAD_PKCS7      = 1,
    TZBSP_CIPHER_PAD_NO_PAD     = 2,
} TZBSP_CIPHER_PAD_ET;

// details are unknown, we can use this as a pointer only.
typedef void tzbsp_cipher_ctx;
typedef void tzbsp_hash_ctx;

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
 * @return E_SUCCESS on success, negative on failure
 */
typedef int (*tzbspHash_func_t) (
            TZBSP_HASH_ALGO_ET alg,
            const uint8_t  *msg,
            uint32_t       msg_len,
            uint8_t        *digest,
            uint32_t       digest_len);

/**
 * @brief Intialize a hash context for update and final functions.
 *
 * @param alg[in] The algorithm standard to use
 * @param hash_ctx[out] The hash context
 *
 * @return E_SUCCESS on success, negative on failure
 *
 * NOTE: In the case of failure the returned hash_ctx will not be
 *       valid.
 *
 */
typedef int (*tzbspHashInit_func_t) (
            TZBSP_HASH_ALGO_ET alg,
            tzbsp_hash_ctx **hash_ctx);


/**
 * @brief  This function will hash some data into the hash context
 *         structure, which Must have been initialized by
 *         tzbsp_hash_init().
 *
 * @param hash_ctx[in]    The hash context
 * @param[in]             Pointer to the msg to hash
 * @param[in]             Length of the msg to hash
 *
 * @return E_SUCCESS on success, negative on failure
 */
typedef int (*tzbspHashUpdate_func_t) (
            const tzbsp_hash_ctx  *hash_ctx,
            const uint8_t         *msg,
            uint32_t              msg_len);

/**
 * @brief  Compute the digest hash value
 *
 * @param hash_ctx[in]     The hash context
 * @param digest[in]       Pointer to output message digest hash
 * @param digest_len[in]   Length of the output message digest hash
 *                         buffer in bytes. Must be 20 bytes for SHA1 and
 *                         32 bytes for SHA256
 *
 * @return E_SUCCESS on success, negative on failure
 */
typedef int (*tzbspHashFinal_func_t) (
            const tzbsp_hash_ctx  *hash_ctx,
            uint8_t               *digest,
            uint32_t              digest_len);

/**
 * @brief Release all resources with a given hash context.
 *
 * @param hash_ctx[in] Hash context to be deleted
 *
 * @return E_SUCCESS on success, negative on failure
 *
 */
typedef int (*tzbspHashFreeCtx_func_t) (
            tzbsp_hash_ctx *hash_ctx);


 /**
 * @brief Intialize a cipher context for encrypt/decrypt operation
 *
 * @param[in] alg  The algorithm standard to use
 * @param[out] cipher_ctx The cipher ctx
 *
 * @return E_SUCCESS on success, negative on failure
 *
 * NOTE: In the case of failure the returned cipher_ctx will not be
 *       valid.
 *
 */
typedef int (*tzbspCipherInit_func_t) (
            TZBSP_CIPHER_ALGO_ET alg,
            tzbsp_cipher_ctx  **cipher_ctx);

/**
 * @brief Release all resources with a given cipher context.
 *
 * @param[in] cipher_ctx Cipher context to be deleted
 *
 * @return E_SUCCESS on success, negative on failure
 *
 */
typedef int (*tzbspCipherFreeCtx_func_t) (
            tzbsp_cipher_ctx *cipher_ctx);


/**
 * @brief Modify the parameters for a given cipher operation.
 *
 * @param[in] cipher_ctx  Cipher context
 * @param[in] param_id    The parameter to modify
 * @param[in] param       The parameter value to set.
 * @param[in] param_len   The length of the param (in bytes).
 *
 * @return E_SUCCESS on success,
 *         negative on failure,
 *         -E_NOT_SUPPORTED if an alogirthm or parameter is not currently supported.
 *
 */
typedef int (*tzbspCipherSetParam_func_t) (
            tzbsp_cipher_ctx *cipher_ctx,
            TZBSP_CIPHER_PARAM_ET param_id,
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
 * @return E_SUCCESS on success, negative on failure
 *
 */
typedef int (*tzbspCipherGetParam_func_t) (
            const tzbsp_cipher_ctx *cipher_ctx,
            TZBSP_CIPHER_PARAM_ET param_id,
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
typedef int (*tzbspCipherEncrypt_func_t) (
            const tzbsp_cipher_ctx  *cipher_ctx,
            const uint8_t           *pt,
            uint32_t                pt_len,
            uint8_t                 *ct,
            uint32_t                *ct_len);

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
typedef int (*tzbspCipherDecrypt_func_t) (
            const tzbsp_cipher_ctx  *cipher_ctx,
            const uint8_t           *ct,
            uint32_t                ct_len,
            uint8_t                 *pt,
            uint32_t                *pt_len);

/** Supported HMAC algorithms   */
typedef enum
{
   TZBSP_HMAC_SHA1          = 1,
   TZBSP_HMAC_SHA256        = 2
}  TZBSP_HMAC_ALGO_ET;

/**
 * @brief Create a hash MAC per FIPS pub 198 using the specified hash algorithm.
 *
 * @param[in] msg             - Pointer to message to be authenticated
 * @param[in] msg_len         - Length of message in bytes
 * @param[in] key             - Pointer to input key to HMAC algorithm
 * @param[in] key_len         - Length of input key in bytes
 * @param[out] msg_digest     - Pointer to message digest (memory provided by caller)
 *
 * @return E_SUCCESS on success, negative on failure
 */
typedef int (*tzbspHmac_func_t) (
            TZBSP_HMAC_ALGO_ET  alg,
            const uint8_t       *msg,
            uint32_t            msg_len,
            const uint8_t       *key,
            uint16_t            key_len,
            uint8_t             *msg_digest);

#define TZBSP_MAX_PRNG    512  /* Maximum number of PRNG bytes read */

/**
 * @brief Release all resources with a given prng context.
 *
 * @param[in] out     The output data buffer
 * @param[in] out_len The output data length. The out_len
 *                    must be at most TZBSP_MAX_PRNG bytes.
 *
 * @return number of bytes read
 *
 */
typedef uint32_t (*tzbspPrng_func_t) (
            const uint8_t *out,
            uint32_t      out_len);


//------------------------------------------------------------------------------
#define TZBSP_FUNC_CRYPTO_HASH_ID                        1
#define TZBSP_FUNC_CRYPTO_HASH_INIT_ID                   2
#define TZBSP_FUNC_CRYPTO_HASH_UPDATE_ID                 3
#define TZBSP_FUNC_CRYPTO_HASH_FINAL_ID                  4
#define TZBSP_FUNC_CRYPTO_HASH_FREE_CTX_ID               5
#define TZBSP_FUNC_CRYPTO_CIPHER_INIT_ID                 6
#define TZBSP_FUNC_CRYPTO_CIPHER_FREE_CTX_ID             7
#define TZBSP_FUNC_CRYPTO_CIPHER_SET_PARAM_ID            8
#define TZBSP_FUNC_CRYPTO_CIPHER_GET_PARAM_ID            9
#define TZBSP_FUNC_CRYPTO_CIPHER_ENCRYPT_ID             10
#define TZBSP_FUNC_CRYPTO_CIPHER_DECRYPT_ID             11
#define TZBSP_FUNC_CRYPTO_HMAC_ID                       12
#define TZBSP_FUNC_CRYPTO_PRNG_ID                       13

/*----------------------------------------------------------------------------
                      TZBSP Function Groups Core
 -----------------------------------------------------------------------------*/

#define TZBSP_I2C_DEFAULT_WAIT_TIME -1

typedef enum
{
  TZBSP_I2CPD_DEVICE_1,
  TZBSP_I2CPD_DEVICE_2,
  TZBSP_I2CPD_DEVICE_3,
  TZBSP_I2CPD_DEVICE_4,
  TZBSP_I2CPD_DEVICE_5,
  TZBSP_I2CPD_DEVICE_6,
  TZBSP_I2CPD_DEVICE_7,
  TZBSP_I2CPD_DEVICE_8,
  TZBSP_I2CPD_DEVICE_9,
  TZBSP_I2CPD_DEVICE_10,
  TZBSP_I2CPD_DEVICE_11,
  TZBSP_I2CPD_DEVICE_12
} TZBSP_I2CPD_DEVICE_ET;

typedef enum
{
  TZBSP_I2C_STANDARD_INTERFACE,
  TZBSP_I2C_AUXILIARY_INTERFACE,
  TZBSP_I2C_INTERFACE_COUNT
} TZBSP_I2C_INTERFACE_ET;

typedef enum
{
  TZBSP_I2C_7_BIT_SLAVE_ADDRESS,
  TZBSP_I2C_10_BIT_SLAVE_ADDRESS,
  TZBSP_I2C_SLAVE_ADDR_OPTIONS_COUNT
} TZBSP_I2C_SLAVEADDR_OPTIONS_ET;

typedef enum
{
  TZBSP_I2C_DEFAULT_ADDRESS_DEVICE,
  TZBSP_I2C_MEMORY_ADDRESS_DEVICE,
  TZBSP_I2C_REGISTER_ADDRESS_DEVICE,
  TZBSP_I2C_SLAVE_DEVICE_COUNT
} TZBSP_I2C_SLAVE_DEVICE_ET;

typedef enum
{
  TZBSP_I2C_READ_OPTION_DEFAULT,
  TZBSP_I2C_GEN_START_BEFORE_READ,
  TZBSP_I2C_GEN_STOP_START_BEFORE_READ,
  TZBSP_I2C_READ_OPTIONS_COUNT
} TZBSP_I2C_READ_OPTIONS_ET;

typedef struct
{
  /* Bus frequency in Fast-Speed mode. */
  uint32_t                        uFsBusFreqInKhz;

  /* Bus frequency in High-Speed mode currently is not supported.*/
  uint32_t                        uHsBusFreqInKhz;

  /* Interface to use. */
  TZBSP_I2C_INTERFACE_ET          eInterface;

  /* Bus acquisition timeout time in microseconds.*/
  int32_t                         nMaxBusAcquireWaitTimeMsec;

} tzbspI2cBusConfig_t;

typedef struct
{
  /* I2C slave device address. */
  uint32_t                        uSlaveAddress;

  /* I2C slave device address options. */
  TZBSP_I2C_SLAVEADDR_OPTIONS_ET  eSlaveAddressOption;

  /* I2C slave device type. */
  TZBSP_I2C_SLAVE_DEVICE_ET       eSlaveDeviceType;

  /* I2C slave device read option. */
  TZBSP_I2C_READ_OPTIONS_ET       eReadOption;

  /*
   * Byte transfer timeout time in microseconds. Clock stretching for
   * a slower device can be achieved by increasing this timeout value.
   * Use the TZBSP_I2C_DEFAULT_WAIT_TIME constant for the default timeout time
   * preset by the driver.
   *
   * Set to TZBSP_I2C_DEFAULT_WAIT_TIME for default value
   */
  int32_t                         nByteTransferWaitTimeUSec;

} tzbspI2cSlaveDeviceConfig_t;


typedef struct
{
  /*
   * Pointer to the bus configuration; set this pointer to NULL
   * if the default bus configuration is preferred.
   *
   * Set this to NULL, if want to use default Bus Config
   */
  tzbspI2cBusConfig_t          *pBusConfig;

  /* Pointer to the slave device configuration. */
  tzbspI2cSlaveDeviceConfig_t  *pSlaveConfig;

} tzbspI2cConfig_t;

typedef struct
{
  /*
   * Memory/register address to be written to the slave device
   * before the slave device starts transmitting/receiving bytes.
   * If the slave device is a I2C_DEFAULT_ADDRESS_DEVICE type, this address
   * is ignored and the I2C controller directly starts reading/writing
   * from/to the slave device without any address byte writes.
   */
  uint32_t                      uStartAddr;

  /* Read/write buffer. */
  unsigned char*                pBuf;

  /* Number of bytes to read/write. */
  uint32_t                      uBufLen;

  /* Number of bytes actually read/written. */
  uint32_t                      uTotalBytes;

} tzbspI2cTransactionInfo_t;

/**
 * Enumeration for the SW fues TZ supports
 */
typedef enum
{
  TZBSP_HLOS_IMG_TAMPER_FUSE = 0,
  TZBSP_NUM_SW_FUSES         = 1
} tzbsp_sw_fuse_t;

/**
 * @brief      Query whether the given SW fuse has been blown
 *
 * @param[in]  fuse_num    the SW fuse to query
 * @param[out] is_blown    whether the given fuse has been blown
 * @param[in]  is_blown_sz size of the return pointer
 *
 * @return     E_SUCCESS on success, negative on failure
 *
 * @warning    function is not thread-safe
 */
typedef int (*tzbspIsSwFuseBlown_func_t)(
            tzbsp_sw_fuse_t  fuse_num,
            bool_t*          is_blown,
            uint32_t         is_blown_sz);

/** Memory tags for supported usecases */

#define  TZBSP_MEM_TAG_USECASE_CP       0x01

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
typedef bool_t (*tzbspIsSTagArea_func_t)(
                    uint32_t  tag,
                    uint32_t  start,
                    uint32_t  end);

/**
 * Reads the status of the HDMI link and hardware HDCP
 *
 * @param [out] hdmiEnable  HDMI output enabled
 *
 * @param [out] hdmiSense HDMI sense
 *
 * @param [out] hdcpAuth HDCP authentication success.
 *
 * @return 0 on success, negative on failure
 */
typedef int (*tzbspHdmiStatusRead_func_t)(
                    uint32_t*  hdmiEnable,
                    uint32_t*  hdmiSense,
                    uint32_t*  hdcpAuth);

/**
 * Wrapper function to transfer access of the I2C QUP to TZ
 *
 * This function performs several operations, and only if all
 * operations are successful will success be returned.
 *
 *   1. Establish a communication link with the requested device
 *   2. Open the device
 *   3. Lock the I2C bus for exclusive access
 *   4. Register for the I2C interrupt in TZBSP, thereby
 *      transfering the interrupt from the HLOS to TZ
 *   5. Protect the control block for device
 *   6. Protect the QUP block for the device
 *
 * @param[in] i2cPdDeviceId The I2C device ID to attach to
 * @return 0 on success, negative on failure
 */
typedef int (*tzbspI2COpen_func_t)(
        TZBSP_I2CPD_DEVICE_ET                      i2cPdDeviceId);

/**
 * Wrapper function to read data from a slave device.
 *
 * This function requires a successful call to tzbspI2COpen() as
 * a prerequisite.
 *
 * This function is blocking. It will return when either data
 * has been read, or an error has occurred.
 *
 * @param[in] i2cPdDeviceId I2C device ID being read from
 * @param[in] pConfig       Pointer to the bus and slave configuration
 *                          for this read transaction.
 * @param[in,out] pReadInfo Pointer to the read information, i.e. buffer,
 *                          length, etc.
 *
 * @return 0 on success, negative on failure
 */
typedef int (*tzbspI2CRead_func_t)(
        TZBSP_I2CPD_DEVICE_ET                      i2cPdDeviceId,
                    const tzbspI2cConfig_t*     pConfig,
                    tzbspI2cTransactionInfo_t*  pReadInfo);

/**
 * Wrapper function to write data to a slave device.
 *
 * This function requires a successful call to tzbspI2COpen() as
 * a prerequisite.
 *
 * @param[in] i2cPdDeviceId  I2C device ID being written to
 * @param[in] pConfig        Pointer to the bus and slave configuration
 *                           for this write transaction.
 * @param[in,out] pWriteInfo Pointer to the write information, i.e. buffer,
 *                           length, etc.
 *
 * @return 0 on success, negative on failure
 */
typedef int (*tzbspI2CWrite_func_t)(
        TZBSP_I2CPD_DEVICE_ET                      i2cPdDeviceId,
                    const tzbspI2cConfig_t*     pConfig,
                    tzbspI2cTransactionInfo_t*  pWriteInfo);

/**
 * Wrapper function to transfer access of the I2C QUP back to
 * the HLOS
 *
 * This function requires a successful call to tzbspI2COpen() as
 * a prerequisite.
 *
 * This function performs several operations, and only if all
 * operations are successful will success be returned.
 *
 *   1. Remove the exclusive access lock to the I2C bus
 *   2. Close the device
 *   3. Remove the QUP block protection for the GSBI3 device
 *   4. Remove the control block protection for GSBI3 device
 *   5. Deregister for the I2C interrupt thereby transfering the
 *      I2C interrupt back to the HLOS
 *
 * @param[in] i2cPdDeviceId I2C Device Id to be closed
 *
 * @return 0 on success, negative on failure
 */
typedef int (*tzbspI2CClose_func_t)(
        TZBSP_I2CPD_DEVICE_ET                      i2cPdDeviceId);


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define TZBSP_FUNC_CORE_IS_SW_FUSE_BLOWN_ID              2
#define TZBSP_FUNC_CORE_IS_TAG_AREA_ID                  10
#define TZBSP_FUNC_CORE_HDMI_STATUS_READ_ID             11
#define TZBSP_FUNC_CORE_I2C_OPEN_ID                     12
#define TZBSP_FUNC_CORE_I2C_READ_ID                     13
#define TZBSP_FUNC_CORE_I2C_WRITE_ID                    14
#define TZBSP_FUNC_CORE_I2C_CLOSE_ID                    15



/*----------------------------------------------------------------------------
                      TZBSP Function Groups
 -----------------------------------------------------------------------------*/
#define TZBSP_GROUP_CRYPTO_ID                            4
#define TZBSP_GROUP_CORE_ID                              5

#endif // __MTK_API_TZBSP_QC__
