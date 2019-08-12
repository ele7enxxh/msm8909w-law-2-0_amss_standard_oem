/* =================================================================================
 *  Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 * =================================================================================
 */
#ifndef KM_UTILS_H_
#define KM_UTILS_H_

/*
 * Included Files
 */
#include "km_main.h"
#include "keymaster_defs.h"
#include "km_platform_utils.h"

#include "gk_main.h"
/*
 * Pre-processor Definitions
 */
#define KM_APP_VERSION_MAJOR 0x0
#define KM_APP_VERSION_MINOR 0x1
#define KM_RPMB_PARTITION_ID_0  0x4b4d5441 
#define KM_RPMB_NUM_SECTOR_MAX_0 8 //i.e 4k
#define KEYMASTER_NUM_SECTOR 40
#define KM_RPMB_SECTOR_SIZE 512
#define MAX_TA_NAME_LEN 64

#define SUCCESS 							 0
#define GENERIC_ERROR 						-1
#define FAIL_CMD_BUF_TOO_SMALL              -2
#define FAIL_CMD_RSP_BUF_TOO_SMALL          -3
#define FAIL_REGISTER_SHARED_BUF            -4
#define FAIL_PREP_SHARED_BUF_SEC_READ       -5
#define FAIL_PREP_SHARED_BUF_NONSEC_READ    -6
#define FAIL_DEREGISTER_SHARED_BUF          -7
#define FAIL_QSTOR_API                      -8
#define FAIL_KM_END_MILESTONE_DONE          -9
#define FAIL_SW_FUSE_API                    -10
#define FAIL_READ_SW_FUSE_API               -11
#define FAIL_INVALID_RPMB_BUF               -12
#define FAIL_RPMB_BUF_TOO_SMALL             -13
#define FAIL_RPMB_PARITION_NOT_OPEN         -14
#define FAIL_MILESTONE_ALREADY_CALLED       -15

#define UINT_MAX 0xffffffffU
#define KM_UINT64_MAX  0xffffffffffffffffU

#define MAX_AEAD_CHUNK_LENGTH (4 * 1024)

#define CHECK_INTEGER_OVERFLOW(a,b) \
	if((uint32_t)a > (UINT_MAX - (uint32_t)b)) {\
		KM_LOG_ERROR("Integer Overflow: a=%u, b=%u", (uint32_t)a, (uint32_t)b); \
		ret = KM_ERROR_INSUFFICIENT_BUFFER_SPACE; \
		goto ret_handle; \
	}

#define CHECK_INTEGER_OVERFLOW64(a,b) \
	if((uint64_t)a > (KM_UINT64_MAX - (uint64_t)b)) {\
		KM_LOG_ERROR("Integer Overflow: a=%llu, b=%llu", (uint64_t)a, (uint64_t)b); \
		ret = KM_ERROR_INSUFFICIENT_BUFFER_SPACE; \
		goto ret_handle; \
	}

#define CHECK_INTEGER_UNDERFLOW(a,b) \
	if((uint32_t)a < (uint32_t)b) {\
		KM_LOG_ERROR("Integer Underflow: a=%u, b=%u", (uint32_t)a, (uint32_t)b); \
		ret = KM_ERROR_INSUFFICIENT_BUFFER_SPACE; \
		goto ret_handle; \
	}

#define CHECK_INTEGER_OVERFLOW_MULT(a,b) \
		if ((uint32_t)b && ((uint32_t)a > UINT_MAX / (uint32_t)b)) {\
			KM_LOG_ERROR("Integer Overflow: a=%u, b=%u", (uint32_t)a, (uint32_t)b); \
			ret = KM_ERROR_INSUFFICIENT_BUFFER_SPACE; \
			goto ret_handle; \
		}

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*(a)))

#define KEY_MATERIAL_SZ (4 * 1024)
#define KEY_MATERIAL_INDIRECT_SZ (512)
#define HW_KEY_PARAM_SZ (512)
#define SW_KEY_PARAM_SZ (512)
#define HW_KEY_PARAM_INDIECT_SZ (256)
#define SW_KEY_PARAM_INDIECT_SZ (256)
#define KEY_OP_SZ (512)

#define KM_CIPHER_KEY    0
#define KM_HMAC_KEY      1
#define GK_CIPHER_KEY    2
#define GK_HMAC_KEY      3
#define KM_AUTH_TOKEN_KEY      4

/*
 * Public Types
 */
typedef boolean bool;

#define KM_HMAC_SHA256_KEY_SIZE   32
/*
 * Public Types
 */

#define KM_OLD_KEY_SIZE_MAX  (512)           /* 4096 bits */
#define KM_OLD_IV_LENGTH     (16)            /* AES128 CBC IV */
#define KM_OLD_HMAC_LENGTH   (32)            /* SHA2 will be used for HMAC  */

/* Key blob data structure */
typedef struct
{
    uint32 magic_num; /* Validate the integrity of keyblob content */
    uint32 version; /* API version number */
    uint8 modulus[KM_OLD_KEY_SIZE_MAX]; /* Modulus (N) array [MSB...LSB] */
    uint32 modulus_size; /* Modulus array length */
    uint8 public_exponent[KM_OLD_KEY_SIZE_MAX]; /* public exponent (e) array [MSB...LSB] */
    uint32 public_exponent_size; /* public exponent array length */
    uint8 iv[KM_OLD_IV_LENGTH]; /* Initial vector */
    uint8 encrypted_private_exponent[KM_OLD_KEY_SIZE_MAX]; /* Encrypted Private Exponent (d) array [MSB...LSB] */
    uint32 encrypted_private_exponent_size; /* Encrypted Private Exponent array length */
    uint8 hmac[KM_OLD_HMAC_LENGTH]; /* HMAC array */
} km_old_key_blob_type;

/*
 * RPMB layout
 *
 * Verified boot partition (1 sector)
 * Gatekeeper partition (1 sector)
 * Keymaster partitions(38 sectors)
 */
typedef enum km_rpmb_partitions_sz
{
    KM_RPMB_VERIFIED_BOOT_SZ = 8,
    KM_RPMB_GATEKEEPER_SZ = 8,
    KM_RPMB_KEYMASTER_SZ = 24,
    KM_RPMB_MAX_SZ,
} km_rpmb_partitions_sz_t;

typedef enum km_rpmb_partitions
{
    KM_RPMB_VERIFIED_BOOT,
    KM_RPMB_GATEKEEPER,
    KM_RPMB_KEYMASTER,
    KM_RPMB_MAX,
} km_rpmb_partitions_t;

/*
 * key_offset, if aes or hmac key, the key is present.
 * 				if asymmetric key - key ctxt is stored.
 */

typedef enum
{
    KM_GENERATE_KEY,
    KM_IMPORT_KEY,
    KM_EXPORT_KEY,
    KM_GET_CHAR,
    KM_GET_SUPPORTED,
    KM_KEY_OP,
} km_functions_t;

/*
 * This is an overall internal buffer.
 * Each call will allocate the appropriate
 * buffer. Use only allocate_km_internal_buf
 * and deallocate_km_internal_buf.
 *
 * ptr - This is the ptr to the buffer.
 * size - total size of the buffer
 * offset - the offset in the buffer that
 * 			data needs to be filled. Client
 * 			Checks if offset > size before writing.
 */
typedef struct _km_internal_buf_t
{
    uint8 *key_material;
    uint32_t key_material_size;
    uint32_t key_material_offset;
    uint8 *key_material_indirect;
    uint32_t key_material_indirect_size;
    uint32_t key_material_indirect_offset;
    uint8 *hw_key_param;
    uint32_t hw_key_param_size;
    uint32_t hw_key_param_offset;
    uint8 *hw_key_param_indirect;
    uint32_t hw_key_param_indirect_size;
    uint32_t hw_key_param_indirect_offset;
    uint8 *sw_key_param;
    uint32_t sw_key_param_size;
    uint32_t sw_key_param_offset;
    uint8 *sw_key_param_indirect;
    uint32_t sw_key_param_indirect_size;
    uint32_t sw_key_param_indirect_offset;
    uint8 *get_supported;
    uint32_t get_supported_size;
    uint32_t get_supported_offset;
    uint8 *key_op;
    uint32_t key_op_size;
    uint32_t key_op_offset;
    uint8 *key_op_indirect;
    uint32_t key_op_indirect_size;
    uint32_t key_op_indirect_offset;
} km_internal_buf_t;

/*
 * Key operation structs
 */
typedef struct _km_key_op_algorithm_t
{
    keymaster_algorithm_t alg;
    keymaster_purpose_t purpose;
    keymaster_block_mode_t mode;
    keymaster_padding_t pad;
    keymaster_digest_t digest;
    uint32_t keysize;
    uint8 *key;
    uint32_t iv_nonce;
    uint32_t iv_size;
    uint8 *IV;
    uint32_t auth_data_len;
    uint8 *auth_data;
    uint32_t taglen;
    uint8 *tag;
    bool aead_input_processed;
    uint8 *overflow_buf;
    uint32_t overflow_buf_len;
    uint32_t overflow_buf_offset;
    uint8 *signature;
    uint32_t signature_len;
    uint8 *hash;
    uint32_t hash_len;
    void *hmac_ctx;
    void *cipher_ctx;
    void *hash_ctx;
    uint8 IV_key_blob[KM_AES_IV_SIZE];
    uint16_t min_taglen;
} km_key_op_algorithm_t;

typedef struct _km_key_op_algorithm_v0_t
{
    keymaster_algorithm_t alg;
    keymaster_purpose_t purpose;
    keymaster_block_mode_t mode;
    keymaster_padding_t pad;
    keymaster_digest_t digest;
    uint32_t keysize;
    uint8 *key;
    uint32_t iv_nonce;
    uint32_t iv_size;
    uint8 *IV;
    uint32_t auth_data_len;
    uint8 *auth_data;
    uint32_t taglen;
    uint8 *tag;
    bool aead_input_processed;
    uint8 *overflow_buf;
    uint32_t overflow_buf_len;
    uint32_t overflow_buf_offset;
    uint8 *signature;
    uint32_t signature_len;
    uint8 *hash;
    uint32_t hash_len;
    void *hmac_ctx;
    void *cipher_ctx;
    void *hash_ctx;
    uint8 *asymm_buf_data;
    uint32_t asymm_buf_data_length;
    uint32_t asymm_buf_data_offset;
    uint8 IV_key_blob[KM_AES_IV_SIZE];
} km_key_op_algorithm_v0_t;
/*
 * Generate key structs
 */
typedef struct _km_generate_header_t km_generate_header_t;
typedef struct _km_gen_key_crypto_t km_gen_key_crypto_t;

typedef int32_t
(*gen_key_create)(uint8 *, uint32_t *, km_gen_key_crypto_t *);
typedef int32_t
(*begin_op)(km_gen_key_crypto_t*, km_key_op_algorithm_t *);
typedef int32_t
(*update_op)(km_key_op_algorithm_t *, uint8 *, uint32_t, uint8 *, uint32_t *,
                uint32_t *);
typedef int32_t
(*finish_op)(km_key_op_algorithm_t *, uint8_t *, uint32_t, uint8_t *,
                uint32_t *);
typedef int32_t
(*abort_op)(km_key_op_algorithm_t *);
typedef int32_t
(*import_key)(keymaster_key_format_t, uint8 *, uint32_t, uint8 *, uint32_t *,
                km_gen_key_crypto_t *);
typedef int32_t
(*export_key)(keymaster_key_format_t, uint8 *, uint32_t *,
                km_gen_key_crypto_t *);

typedef struct _km_func_ptrs
{
    keymaster_algorithm_t alg;
    gen_key_create genkey_func_ptr_create;
    begin_op begin_func;
    update_op update_func;
    finish_op finish_func;
    abort_op abort_func;
    import_key import_key_func;
    export_key export_key_func;
} km_func_ptrs_t;

typedef struct _km_key_op_user_auth_t
{
    uint32_t all_users;
    uint32_t android_user_id;
    uint64_t secure_user_id[5];
    uint32_t secure_user_id_count;
    uint32_t no_auth_required;
    hw_authenticator_type_t auth_type;
    uint64_t auth_token_timestamp;
    uint64_t auth_timeout;
    uint64_t active_date;
    uint64_t origination_expire;
    uint64_t usage_expire;
    uint32_t seconds_between_ops;
    uint32_t uses_per_boot;
    uint32_t handle;
    uint32_t authenticated;
} km_key_op_user_auth_t;

typedef struct _km_key_op_checks_user_auth_t
{
   uint64_t key_id;
    uint64_t time_last_op;
    uint32_t num_uses;
} km_key_op_checks_user_auth_t;

typedef struct _km_key_op_appn_ac_t
{
    uint32_t dummy;
} km_key_op_appn_ac_t;

typedef struct _km_key_op_rot_t
{
    uint32_t dummy;
} km_key_op_rot_t;

typedef struct _km_operation_ctx_t
{
    void *alg_ctx;
    uint32_t alg_size;
    void *user_auth_ctx;
    uint32_t user_auth_size;
    void *appn_ac_ctx;
    uint32_t appn_ac_size;
    void *rot_ctx;
    uint32_t rot_size;
    void *info_tags_ctx;
    uint32_t info_tags_size;
    uint32_t pubkeyop;
    uint32_t verify_user_auth;
    keymaster_operation_handle_t handle;
    uint64_t key_id;
    uint8_t  old_key_blob;
} km_operation_ctx_t;

typedef struct _km_operation_handle_t
{
    keymaster_operation_handle_t handle;
   uint64_t key_id;
   void *ctx;
} km_operation_handle_t;

/*
 * Generate key structs
 */
typedef struct _km_generate_header_t
{
    keymaster_purpose_t purpose[KM_PURPOSE_MAX];
    uint32_t purpose_count;
    keymaster_algorithm_t alg;
    keymaster_padding_t pad[KM_PAD_MAX];
    uint32_t pad_count;
    keymaster_digest_t digest[KM_DIGEST_MAX];
    uint32_t digest_count;
    keymaster_block_mode_t mode[KM_MODE_MAX];
    uint32_t mode_count;
    uint32_t key_size;
    uint8_t rsa_pub_exponent[5];
    uint16_t iv_nonce;
    uint16_t min_tag_size;
} km_generate_header_t;

typedef struct _km_gen_key_crypto_t
{
    km_generate_header_t header;
    uint32_t paramsize;
    uint32_t param_offset;
    uint32_t keysize;
    uint32_t key_offset;
} km_gen_key_crypto_t;

typedef struct _km_gen_key_user_auth_header_t
{
    uint32_t handle;
    uint32_t all_users;
    uint32_t android_user_id;
    uint64_t secure_user_id[5];
    uint32_t secure_user_id_count;
    uint32_t no_auth_required;
    hw_authenticator_type_t auth_type;
    uint32_t auth_timeout;
    uint64_t active_date;
    uint64_t creation_date;
    uint64_t origination_expire;
    uint64_t usage_expire;
    uint32_t seconds_between_ops;
    uint32_t uses_per_boot;
} km_gen_key_user_auth_header_t;

typedef struct km_gen_key_user_auth_t
{
    km_gen_key_user_auth_header_t header;
    uint32_t paramsize;
    uint32_t param_offset;
} km_gen_key_user_auth_t;

typedef struct _km_gen_key_rot_t
{
    uint8_t rot[(KM_SHA256_HASH_SZ / 8)];
} km_gen_key_rot_t;

typedef struct _km_gen_key_appn_ac_header_t
{
    uint32_t all_applications;
    uint32_t application_id;
    uint32_t application_id_size;
    uint32_t application_data;
    uint32_t application_data_size;
} km_gen_key_appn_ac_header_t;

typedef struct _km_gen_key_appn_ac_t
{
    km_gen_key_appn_ac_header_t header;
    uint32_t paramsize;
    uint32_t param_offset;
} km_gen_key_appn_ac_t;

typedef struct _km_gen_key_info_tags_header_t
{
    keymaster_key_origin_t origin;
    uint32_t rollback_resistant;
    keymaster_key_blob_usage_requirements_t keyblob_usage;
} km_gen_key_info_tags_header_t;

typedef struct _km_gen_key_info_tags_t
{
    km_gen_key_info_tags_header_t header;
    uint32_t paramsize;
    uint32_t param_offset;
} km_gen_key_info_tags_t;

typedef struct _km_hmac_ctx
{
    void *platform_hmac_ctx;
    km_key_op_algorithm_t *km_key_op_ctxt;
} km_hmac_ctx;

typedef struct _km_cipher_ctx
{
    void *platform_cipher_ctx;
    km_key_op_algorithm_t *km_key_op_ctxt;
    uint8_t deinit_done;
} km_cipher_ctx;

typedef struct _km_hash_ctx
{
    void *platform_hash_ctx;
    km_key_op_algorithm_t *km_key_op_ctxt;
} km_hash_ctx;

typedef struct _km_gen_key_cipher_main_header_t
{
    uint8 hmac[KM_HMAC_SHA256_KEY_SIZE];
    km_key_op_algorithm_v0_t cipher_ctx;
    km_key_op_algorithm_v0_t hmac_ctx;
} km_gen_key_cipher_main_header_t;

typedef struct _km_gen_key_main_header_t
{
    km_gen_key_cipher_main_header_t metadata;
    uint32_t version;
    uint64_t key_id;
    uint32_t alg_offset;
    uint32_t alg_size;
    uint32_t user_auth_offset;
    uint32_t user_auth_size;
    uint32_t appn_ac_offset;
    uint32_t appn_ac_size;
    uint32_t rot_offset;
    uint32_t rot_size;
    uint32_t info_tags_offset;
    uint32_t info_tags_size;
} km_gen_key_main_header_t;

typedef struct _tag_to_string
{
    keymaster_tag_t tag;
    char tagstring[100];
} tag_to_string_t;

typedef struct _cmdid_to_string_t
{
    keymaster_cmd_t cmdid;
    char cmdidstring[100];
} cmdid_to_string_t;

/*
 * Public Data
 */

/*
 * Inline Functions
 */
/*
 * Inline Functions
 */
extern bool is_rpmb_enabled;
extern bool rpmb_init_done;
/*
 * Public Function Prototypes
 */

/*
 * Caller will pass the correct buffer to copy the data. This function will check
 * the length before copy..
 */

/*
 * Public Function Prototypes
 */
//TODO: CLeanup and arrange funcs in order
int32_t
algorithm_generate_import_key(keymaster_key_param_t *key_params,
                uint32_t params_count, uint8 *provided_key,
                uint32_t provided_key_length, km_internal_buf_t *key_alloc,
                uint32_t generate_import, keymaster_key_format_t format);

int32_t
user_auth_generate_key(keymaster_key_param_t *key_params, uint32_t params_count,
                km_internal_buf_t *key_alloc);

int32_t
appn_ac_generate_key(keymaster_key_param_t *key_params, uint32_t params_count,
                km_internal_buf_t *key_alloc);

int32_t
info_tags_generate_import_key(keymaster_key_param_t *key_params,
                uint32_t params_count, km_internal_buf_t *key_alloc,
                uint32_t generate_import, uint64_t *key_id);

int32_t
info_tags_key_op_delete_handler(km_internal_buf_t *key_alloc, uint64_t key_id);

int32_t
rot_generate_key(keymaster_key_param_t *key_params, uint32_t params_count,
                km_internal_buf_t *key_alloc);

int32_t
algorithm_get_supported_alg(km_internal_buf_t *key_alloc);

int32_t
algorithm_get_supported_block_modes(keymaster_algorithm_t alg,
                keymaster_purpose_t purpose, km_internal_buf_t *key_alloc);

int32_t
algorithm_get_supported_padding_modes(keymaster_algorithm_t alg,
                keymaster_purpose_t purpose, km_internal_buf_t *key_alloc);

int32_t
algorithm_get_supported_digests(keymaster_algorithm_t alg,
                keymaster_purpose_t purpose, km_internal_buf_t *key_alloc);

int32_t
algorithm_get_key_char(km_internal_buf_t *key_alloc);

int32_t
user_auth_get_key_char(km_internal_buf_t *key_alloc);

int32_t
appn_ac_get_key_char(uint8 * client_id, uint32_t client_id_length,
                uint8 *app_data, uint32_t app_data_length,
                km_internal_buf_t *key_alloc);

int32_t
rot_get_key_char(km_internal_buf_t *key_alloc);

int32_t
info_tags_get_key_char(km_internal_buf_t *key_alloc, uint64_t key_id);

int32_t
info_tags_key_op_begin_handler(keymaster_key_param_t *provided_key_params,
                uint32_t provided_params_count, km_internal_buf_t *key_alloc,
                uint64_t key_id);

int32_t
user_auth_key_op_handler(keymaster_key_param_t *key_params,
                uint32_t params_count, km_internal_buf_t *key_alloc);

int32_t
appn_ac_key_op_handler(keymaster_key_param_t *key_params, uint32_t params_count,
                km_internal_buf_t *key_alloc);

int32_t
rot_key_op_handler(keymaster_key_param_t *key_params, uint32_t params_count,
                km_internal_buf_t *key_alloc);

int32_t
key_op_delete_handler(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);
int32_t
key_op_delete_all_handler(void* cmd, uint32_t cmdlen, void* rsp,
                uint32_t rsplen);
int32_t
km_user_auth_cryptoperiod_begin_key_tags(km_internal_buf_t *key_alloc,
                bool pub_operation);
/**
 @brief
 Handles key_op_begin_handler

 @param[in]   cmd         pointer to command structure
 @param[in]   cmdlen      command message size
 @param[in]   rsp         pointer to response structure
 @param[in]   rsplen      response message size
 */

int32_t
key_op_begin_handler(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);

/**
 @brief
 Handles key_op_update_handler

 @param[in]   cmd         pointer to command structure
 @param[in]   cmdlen      command message size
 @param[in]   rsp         pointer to response structure
 @param[in]   rsplen      response message size
 */

int32_t
key_op_update_handler(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);

/**
 @brief
 Handles key_op_final_handler

 @param[in]   cmd         pointer to command structure
 @param[in]   cmdlen      command message size
 @param[in]   rsp         pointer to response structure
 @param[in]   rsplen      response message size
 */

int32_t
key_op_finish_handler(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);

/**
 @brief
 Handles key_op_abort_handler

 @param[in]   cmd         pointer to command structure
 @param[in]   cmdlen      command message size
 @param[in]   rsp         pointer to response structure
 @param[in]   rsplen      response message size
 */

int32_t
key_op_abort_handler(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);

int32_t
encrypt_key_material(km_key_op_algorithm_v0_t *km_cipher_op_ctxt, uint8 *msg,
                uint32_t msglen, uint8 *ct, uint32_t *ct_len, uint8 *key,
                uint32 keysize);
int32_t
hmac_key_material(km_key_op_algorithm_v0_t *km_hmac_op_ctxt, uint8 *ct,
                uint32_t *ct_len, uint8 *digest, uint32_t digestlen, uint8 *key,
                uint32 keysize);
int32_t
auth_key_material(km_key_op_algorithm_v0_t *km_hmac_op_ctxt, uint8 *ct,
                uint32_t ct_len, uint8 *digest, uint32_t digestlen, uint8 *key,
                uint32 keysize);
int32_t
decrypt_key_material(km_key_op_algorithm_v0_t *km_cipher_op_ctxt, uint8 *ct,
                uint32_t ct_len, uint8 *msg, uint32_t *msg_len, uint8 *key,
                uint32_t keysize);

int32_t
km_encrypt_hmac_key_material(km_key_op_algorithm_v0_t *km_cipher_op_ctxt,
                km_key_op_algorithm_v0_t *km_hmac_op_ctxt, uint8 *msg,
                uint32_t msglen, uint8 *ct, uint32_t *ct_len, uint8 *digest,
                uint32_t digestlen);
int32_t
km_decrypt_auth_key_material(km_key_op_algorithm_v0_t *km_cipher_op_ctxt,
                km_key_op_algorithm_v0_t *km_hmac_op_ctxt, uint8 *ct,
                uint32_t ct_len, uint8 *msg, uint32_t *msg_len, uint8 *digest,
                uint32_t digestlen);

int
add_tag_to_key_buf(keymaster_key_param_t* param,
                keymaster_key_param_t *param_add, km_internal_buf_t *key_alloc);
int
add_tag_to_characteristics(keymaster_key_param_t* param,
                keymaster_key_param_t *param_add, km_internal_buf_t *key_alloc,
                km_char_override_t override_hw_sw);
int32_t
km_free_key_op_algorithm_header(km_key_op_algorithm_t *km_key_op_ctxt);

keymaster_key_param_t*
check_tag_present(keymaster_tag_t tag, keymaster_key_param_t *key_params,
                uint32_t params_count);

bool
is_blob_tag(keymaster_tag_t tag);
void
reset_rsp_buf_offset(keymaster_key_param_t *param, uint32_t count);
int32_t
copy_to_rsp_buf(uint8 *rsp, uint32_t rsplen, km_internal_buf_t *gen_key_alloc);

int32_t
allocate_km_internal_buf(km_internal_buf_t *gen_key_alloc, km_functions_t func);
void
deallocate_km_internal_buf(km_internal_buf_t *gen_key_alloc);
void
deallocate_km_internal_buf_key_op_begin(km_internal_buf_t *gen_key_alloc);

keymaster_operation_handle_t
km_add_handle(uint64_t key_id, void *ctx);
keymaster_operation_handle_t
km_add_handle_velocity(uint64_t key_id, void *ctx);
void
km_remove_expired_entries_velocity(uint64_t uptime);
uint32_t
km_get_used_handle_count(void);
void*
km_get_ctx(keymaster_operation_handle_t handle);
void*
km_get_ctx_key_id(uint64_t key_id);
int32_t
km_delete_handle(keymaster_operation_handle_t handle);
int32_t
km_delete_handle_all(void);
int32_t
km_delete_key_id(uint64_t key_id);

int32_t
get_supported_algorithms(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);
int32_t
get_supported_block_modes(void* cmd, uint32_t cmdlen, void* rsp,
                uint32_t rsplen);
int32_t
get_supported_padding_modes(void* cmd, uint32_t cmdlen, void* rsp,
                uint32_t rsplen);
int32_t
get_supported_digests(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);
int32_t
get_supported_import_formats(void* cmd, uint32_t cmdlen, void* rsp,
                uint32_t rsplen);
int32_t
get_supported_export_formats(void* cmd, uint32_t cmdlen, void* rsp,
                uint32_t rsplen);

int32_t
algorithm_get_supported_import_formats(keymaster_algorithm_t alg,
                km_internal_buf_t *key_alloc);
int32_t
algorithm_get_supported_export_formats(keymaster_algorithm_t alg,
                km_internal_buf_t *key_alloc);

int32_t
get_key_charac_handler(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);

int32_t
generate_key_handler(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);
int32_t
import_key_handler(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);
int32_t
export_key_handler(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);

bool
algorithm_check_pub_key_operation(keymaster_purpose_t purpose,
                km_internal_buf_t *key_alloc);
int32_t
algorithm_key_op_begin_handler(keymaster_purpose_t purpose,
                keymaster_key_param_t *provided_key_params,
                uint32_t provided_params_count, km_internal_buf_t *key_alloc,
                keymaster_algorithm_t *alg);
int32_t
algorithm_key_op_update_handler(keymaster_key_param_t *provided_key_params,
                uint32_t provided_params_count, void *key_op_header_input,
                void *input, uint32_t input_length, void *output,
                uint32_t *output_length, uint32_t *input_consumed);
int32_t
algorithm_key_op_finish_handler(keymaster_key_param_t *provided_key_params,
                uint32_t provided_params_count, void *key_op_header_input,
                void *signature, uint32_t signature_length, void *output,
                uint32_t *output_length);
int32_t
algorithm_key_op_abort_handler(void *key_op_header_input);
int32_t
algorithm_export_handler(keymaster_key_format_t format, uint8 *exp_key,
                uint32_t *exp_key_length, km_internal_buf_t *key_alloc,
                keymaster_algorithm_t *alg);

int32_t
user_auth_key_op_begin_handler(keymaster_key_param_t *provided_key_params,
                               uint32_t provided_params_count, km_internal_buf_t *key_alloc,
                               uint64_t key_id);
int32_t
user_auth_key_op_update_handler(keymaster_key_param_t *provided_key_params,
                uint32_t provided_params_count,
                km_key_op_user_auth_t *key_op_header,
                keymaster_operation_handle_t handle);
int32_t
user_auth_key_op_finish_handler(keymaster_key_param_t *provided_key_params,
                uint32_t provided_params_count,
                km_key_op_user_auth_t *key_op_header,
                keymaster_operation_handle_t handle);
int32_t
user_auth_key_op_abort_handler(km_key_op_user_auth_t *key_op_header);

int32_t
appn_ac_key_op_begin_handler(keymaster_key_param_t *provided_key_params,
                uint32_t provided_params_count, km_internal_buf_t *key_alloc);
int32_t
appn_ac_key_op_update_handler(keymaster_key_param_t *provided_key_params,
                uint32_t provided_params_count,
                km_key_op_appn_ac_t *key_op_header);
int32_t
appn_ac_key_op_finish_handler(keymaster_key_param_t *provided_key_params,
                uint32_t provided_params_count,
                km_key_op_appn_ac_t *key_op_header);
int32_t
appn_ac_key_op_abort_handler(km_key_op_appn_ac_t *key_op_header);
int32_t
appn_ac_key_op_export_handler(uint8 * client_id, uint32_t client_id_length,
                uint8 *app_data, uint32_t app_data_length,
                km_internal_buf_t *key_alloc);

int32_t
rot_key_op_begin_handler(keymaster_key_param_t *provided_key_params,
                uint32_t provided_params_count, km_internal_buf_t *key_alloc);
int32_t
rot_key_op_update_handler(keymaster_key_param_t *provided_key_params,
                uint32_t provided_params_count, km_key_op_rot_t *key_op_header);
int32_t
rot_key_op_finish_handler(keymaster_key_param_t *provided_key_params,
                uint32_t provided_params_count, km_key_op_rot_t *key_op_header);
int32_t
rot_key_op_abort_handler(km_key_op_rot_t *key_op_header);

int
km_register_shared_buffer(void* ptr, uint32_t len);
int
km_deregister_shared_buffer(void* ptr, uint32_t len);

unsigned long long
km_get_uptime(void);

int32_t
km_create_auth_token_key(void);
int32_t
get_auth_token_key(uint8 *key, uint32_t keylen);
uint32_t
keymaster_get_derived_key_size(uint8 key_type);
uint8 *
keymaster_get_derived_key(uint8 key_type);

int32_t
km_aes_generate_key(uint8 *key, uint32_t *key_buf_len,
                km_gen_key_crypto_t *genkey_key_header);
int32_t
km_aes_begin(km_gen_key_crypto_t *genkey_key_header,
                km_key_op_algorithm_t *km_key_op_ctxt);
int32_t
km_aes_update(km_key_op_algorithm_t *km_key_op_ctxt, uint8 *input,
                uint32_t input_length, uint8 *output, uint32_t *output_length,
                uint32_t *input_consumed);
int32_t
km_aes_finish(km_key_op_algorithm_t *km_key_op_ctxt, uint8_t* signature,
                uint32_t signature_length, uint8_t* output,
                uint32_t *output_length);
int32_t
km_aes_abort(km_key_op_algorithm_t *km_key_op_ctxt);
int32_t
km_aes_import_key(keymaster_key_format_t format, uint8 *imp_key,
                uint32_t imp_key_length, uint8 *key, uint32_t *key_buf_len,
                km_gen_key_crypto_t *genkey_key_header);
int32_t
km_aes_export_key(keymaster_key_format_t format, uint8 *exp_key,
                uint32_t *exp_key_length,
                km_gen_key_crypto_t *genkey_key_header);

int32_t
km_rsa_generate_key(uint8 *key, uint32_t *key_buf_len,
                km_gen_key_crypto_t *genkey_key_header);
int32_t
km_rsa_begin(km_gen_key_crypto_t *genkey_key_header,
                km_key_op_algorithm_t *km_key_op_ctxt);
int32_t
km_rsa_update(km_key_op_algorithm_t *km_key_op_ctxt, uint8 *input,
                uint32_t input_length, uint8 *output, uint32_t *output_length,
                uint32_t *input_consumed);
int32_t
km_rsa_finish(km_key_op_algorithm_t *km_key_op_ctxt, uint8_t* signature,
                uint32_t signature_length, uint8_t* output,
                uint32_t *output_length);
int32_t
km_rsa_abort(km_key_op_algorithm_t *km_key_op_ctxt);
int32_t
km_rsa_import_key(keymaster_key_format_t format, uint8 *imp_key,
                uint32_t imp_key_length, uint8 *key, uint32_t *key_buf_len,
                km_gen_key_crypto_t *genkey_key_header);
int32_t
km_rsa_export_key(keymaster_key_format_t format, uint8 *exp_key,
                uint32_t *exp_key_length,
                km_gen_key_crypto_t *genkey_key_header);

int32_t
km_ecdsa_generate_key(uint8 *key, uint32_t *key_buf_len,
                km_gen_key_crypto_t *genkey_key_header);
int32_t
km_ecdsa_begin(km_gen_key_crypto_t *genkey_key_header,
                km_key_op_algorithm_t *km_key_op_ctxt);
int32_t
km_ecdsa_update(km_key_op_algorithm_t *km_key_op_ctxt, uint8 *input,
                uint32_t input_length, uint8 *output, uint32_t *output_length,
                uint32_t *input_consumed);
int32_t
km_ecdsa_finish(km_key_op_algorithm_t *km_key_op_ctxt, uint8_t* signature,
                uint32_t signature_length, uint8_t* output,
                uint32_t *output_length);
int32_t
km_ecdsa_abort(km_key_op_algorithm_t *km_key_op_ctxt);
int32_t
km_ecdsa_import_key(keymaster_key_format_t format, uint8 *imp_key,
                uint32_t imp_key_length, uint8 *key, uint32_t *key_buf_len,
                km_gen_key_crypto_t *genkey_key_header);
int32_t
km_ecdsa_export_key(keymaster_key_format_t format, uint8 *exp_key,
                uint32_t *exp_key_length,
                km_gen_key_crypto_t *genkey_key_header);

int32_t
km_alg_hmac_generate_key(uint8 *key, uint32_t *key_buf_len,
                km_gen_key_crypto_t *genkey_key_header);
int32_t
km_alg_hmac_begin(km_gen_key_crypto_t *genkey_key_header,
                km_key_op_algorithm_t *km_key_op_ctxt);
int32_t
km_alg_hmac_update(km_key_op_algorithm_t *km_key_op_ctxt, uint8 *input,
                uint32_t input_length, uint8 *output, uint32_t *output_length,
                uint32_t *input_consumed);
int32_t
km_alg_hmac_finish(km_key_op_algorithm_t *km_key_op_ctxt, uint8_t* signature,
                uint32_t signature_length, uint8_t* output,
                uint32_t *output_length);
int32_t
km_alg_hmac_abort(km_key_op_algorithm_t *km_key_op_ctxt);
int32_t
km_alg_hmac_import_key(keymaster_key_format_t format, uint8 *imp_key,
                uint32_t imp_key_length, uint8 *key, uint32_t *key_buf_len,
                km_gen_key_crypto_t *genkey_key_header);
int32_t
km_alg_hmac_export_key(keymaster_key_format_t format, uint8 *exp_key,
                uint32_t *exp_key_length,
                km_gen_key_crypto_t *genkey_key_header);

void
print_params(keymaster_key_param_t* params, size_t params_count);
void
print_param(keymaster_key_param_t* param);
char*
tag_to_string(keymaster_tag_t tag);
char *
cmdid_to_string(keymaster_cmd_t cmdid);

int
km_rsa_key_gen(KM_RSA_KEY *key, int keylen, unsigned char *pub_exp,
                int pub_exp_len);
int
km_secpkcs8_parse(uint8* data_ptr, uint16 data_len,
                KM_PKCS8_PRIVKEY_TYPE* privkey);
int
km_rsa_sign_hash(KM_RSA_KEY *key, keymaster_padding_t padding_type,
                keymaster_digest_t hashidx, uint32_t salt_len,
                unsigned char *hash, int hashlen, unsigned char *signature,
                int *siglen);
int
km_rsa_verify_signature(KM_RSA_KEY *key, keymaster_padding_t padding_type,
                keymaster_digest_t hashidx, uint32_t salt_len,
                unsigned char *hash, int hashlen, unsigned char *signature,
                int siglen);
int
km_rsa_encrypt(KM_RSA_KEY *key, keymaster_padding_t padding_type,
                keymaster_digest_t hashidx, const unsigned char *msg,
                int msglen, unsigned char *cipher, int *cipherlen);

int
km_rsa_decrypt(KM_RSA_KEY *key, keymaster_padding_t padding_type,
                keymaster_digest_t hashidx, unsigned char *cipher,
                int cipherlen, unsigned char *msg, int *msglen);

int
km_BIGINT_read_unsigned_bin(KM_BigInt * a, const uint8 * buf, uint32_t len);

int km_ce_util_cmp(KM_S_BIGINT *s1, KM_S_BIGINT *s2);

int
km_util_init_s_bigint(KM_S_BIGINT **a);

void
km_util_free_s_bigint(KM_S_BIGINT *a);

int km_util_check_msg_gt_modulus(void *input_buf_temp,
        uint32_t input_buf_len_temp, KM_RSA_KEY *key_temp);

int
km_BIGINT_read_radix(KM_BigInt * a, const char *str, uint32_t radix);

int
km_rsa_exptmod(KM_RSA_KEY *key, const unsigned char *in, int inlen,
                unsigned char *out, int *outlen, int which);
int32_t
km_copy_S_BIGINT(uint8 *dst, uint32_t *dst_len, KM_S_BIGINT *src,
                uint32_t max_len);

void*
km_secure_memset(void* ptr, int value, size_t len);
int
km_rsa_init_key(KM_S_BIGINT **key, const char *key_str, int key_len);
void
km_rsa_free_key(KM_S_BIGINT *key);

/**
 @brief
 Handles km_read_km_device_state

 @param[in]   cmd         pointer to command structure
 @param[in]   cmdlen      command message size
 @param[in]   rsp         pointer to response structure
 @param[in]   rsplen      response message size
 */
int32_t
km_read_km_device_state(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);
/**
 @brief
 Handles km_read_km_device_state

 @param[in]   cmd         pointer to command structure
 @param[in]   cmdlen      command message size
 @param[in]   rsp         pointer to response structure
 @param[in]   rsplen      response message size
 */
int32_t
km_write_km_device_state(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);
/**
 @brief
 API used by AppsBl to set the Milestone for each boot.

 @param[in]   cmd         pointer to command structure
 @param[in]   cmdlen      command message size
 @param[in]   rsp         pointer to response structure
 @param[in]   rsplen      response message size
 */
int32_t
km_set_milestone(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);
int32_t
km_check_is_sw_fuse_blown(bool *fuse_blown);
/**
 @brief
 API used by AppsBl to set the ROT for each boot.

 @param[in]   cmd         pointer to command structure
 @param[in]   cmdlen      command message size
 @param[in]   rsp         pointer to response structure
 @param[in]   rsplen      response message size
 */
int32_t
km_set_rot(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);

uint8_t*
km_get_rot(void);

int32_t
keymaster_old_cmd_handler(void* cmd, uint32 cmdlen, void* rsp, uint32 rsplen);

int32_t
open_add_km_partition(void);

int32_t
km_write_rpmb_data(km_rpmb_partitions_t partition, uint8_t *buf,
                uint32_t buflen, uint32_t partition_offset);

int32_t
km_read_rpmb_data(km_rpmb_partitions_t partition, uint8_t *buf, uint32_t buflen,
                uint32_t partition_offset);

int32_t
km_get_auth_token_key(void* cmd, uint32_t cmdlen, void* rsp, uint32_t rsplen);

int32_t
km_add_key_id_rpmb(uint64_t keyid);
int32_t
km_check_key_id_rpmb(uint64_t keyid);
int32_t
km_delete_key_id_rpmb(uint64_t keyid);
int32_t
km_delete_all_keys_rpmb(void);
int
km_query_rpmb_enablement(boolean* b);

gk_rpmb_data_t*
gk_get_rpmb_data(secure_id_t user_id);
int32_t
gk_modify_rpmb_data(gk_rpmb_data_t *rpmb_data_modify);
int32_t
gk_add_rpmb_data(gk_rpmb_data_t *rpmb_data);
int32_t
gk_delete_all_rpmb_data(void);
int32_t
gk_delete_rpmb_data_uid(uint32_t uid);

int32_t
algorithm_key_op_begin_handler_old(keymaster_purpose_t purpose,
		keymaster_key_param_t *provided_key_params, uint32_t provided_params_count,
		km_internal_buf_t *key_alloc);
int32_t
algorithm_key_op_update_handler_old(keymaster_key_param_t *provided_key_params,
		uint32_t provided_params_count, void *key_op_header_input,
		void *input, uint32_t input_length,
		void *output, uint32_t *output_length,
		uint32_t *input_consumed);
int32_t
algorithm_key_op_finish_handler_old(
        keymaster_key_param_t *provided_key_params,
        uint32_t provided_params_count, void *key_op_header_input,
        void *signature, uint32_t signature_length, void *output,
        uint32_t *output_length);
int32_t
algorithm_export_handler_old(
        keymaster_key_format_t format,
        uint8 *exp_key, uint32_t *exp_key_length,
        km_internal_buf_t *key_alloc, keymaster_algorithm_t *alg);
int32_t
km_rsa_begin_old(uint8 *key_material,
		km_key_op_algorithm_t *km_key_op_ctxt);
int32_t
km_rsa_update_old(
        km_key_op_algorithm_t *km_key_op_ctxt,
        uint8 *input, uint32_t input_length,
        uint8 *output, uint32_t *output_length, uint32_t *input_consumed);
int32_t
km_rsa_finish_old(km_key_op_algorithm_t *km_key_op_ctxt,
                        uint8_t* signature, uint32_t signature_length,
                        uint8_t* output, uint32_t *output_length);
int32_t
km_rsa_export_key_old(
        keymaster_key_format_t format, uint8 *exp_key,
        uint32_t *exp_key_length, uint8 *genkey_key_header);

int32_t
keymaster_old_validate_keydata(key_op_begin_req_t *key_op_req,
		km_internal_buf_t  *key_alloc);

int32_t 
keymaster_old_validate_export_keydata(export_key_req_t *key_op_req);

int32_t
keymaster_old_validate(km_old_key_blob_type *key_blob);
int32
get_fpta_name(char *fpta_name_str, uint32 fpta_name_len);
uint32_t 
get_gk_timeout_config(void);
#endif /* KM_UTILS_H_ */

