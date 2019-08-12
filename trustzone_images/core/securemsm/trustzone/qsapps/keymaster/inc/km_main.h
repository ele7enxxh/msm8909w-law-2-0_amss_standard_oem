#ifndef KM_MAIN_H
#define KM_MAIN_H

#include "keymaster_defs.h"

/**
 * Commands supported
 * Since Gatekeeper is supported int he same TA, if adding or
 * changing this, keep in sync in Gatekeeper(gk_main.h).
 */
#define KEYMASTER_CMD_ID_OLD  0UL
#define KEYMASTER_CMD_ID  0x100UL
#define KEYMASTER_UTILS_CMD_ID  0x200UL
#define GK_CMD_ID  0x1000UL

typedef enum
{
    /*
     * List the commands supportedin by the hardware.
     */
    KEYMASTER_GET_SUPPORTED_ALGORITHMS = (KEYMASTER_CMD_ID + 1UL),
    KEYMASTER_GET_SUPPORTED_BLOCK_MODES = (KEYMASTER_CMD_ID + 2UL),
    KEYMASTER_GET_SUPPORTED_PADDING_MODES = (KEYMASTER_CMD_ID + 3UL),
    KEYMASTER_GET_SUPPORTED_DIGESTS = (KEYMASTER_CMD_ID + 4UL),
    KEYMASTER_GET_SUPPORTED_IMPORT_FORMATS = (KEYMASTER_CMD_ID + 5UL),
    KEYMASTER_GET_SUPPORTED_EXPORT_FORMATS = (KEYMASTER_CMD_ID + 6UL),
    KEYMASTER_ADD_RNG_ENTROPY = (KEYMASTER_CMD_ID + 7UL),
    KEYMASTER_GENERATE_KEY = (KEYMASTER_CMD_ID + 8UL),
    KEYMASTER_GET_KEY_CHARACTERISTICS = (KEYMASTER_CMD_ID + 9UL),
    KEYMASTER_RESCOPE = (KEYMASTER_CMD_ID + 10UL),
    KEYMASTER_IMPORT_KEY = (KEYMASTER_CMD_ID + 11UL),
    KEYMASTER_EXPORT_KEY = (KEYMASTER_CMD_ID + 12UL),
    KEYMASTER_DELETE_KEY = (KEYMASTER_CMD_ID + 13UL),
    KEYMASTER_DELETE_ALL_KEYS = (KEYMASTER_CMD_ID + 14UL),
    KEYMASTER_BEGIN = (KEYMASTER_CMD_ID + 15UL),
    KEYMASTER_UPDATE = (KEYMASTER_CMD_ID + 17UL),
    KEYMASTER_FINISH = (KEYMASTER_CMD_ID + 18UL),
    KEYMASTER_ABORT = (KEYMASTER_CMD_ID + 19UL),

    KEYMASTER_GET_VERSION = (KEYMASTER_UTILS_CMD_ID + 0UL),
    KEYMASTER_SET_ROT = (KEYMASTER_UTILS_CMD_ID + 1UL),
    KEYMASTER_READ_KM_DEVICE_STATE = (KEYMASTER_UTILS_CMD_ID + 2UL),
    KEYMASTER_WRITE_KM_DEVICE_STATE = (KEYMASTER_UTILS_CMD_ID + 3UL),
    KEYMASTER_MILESTONE_CALL = (KEYMASTER_UTILS_CMD_ID + 4UL),
    KEYMASTER_GET_AUTH_TOKEN_KEY = (KEYMASTER_UTILS_CMD_ID + 5UL),
    KEYMASTER_SECURE_WRITE_PROTECT = (KEYMASTER_UTILS_CMD_ID + 6UL), 
    KEYMASTER_SET_VERSION = (KEYMASTER_UTILS_CMD_ID + 7UL), 

    GK_ENROLL = (GK_CMD_ID + 1UL),
    GK_VERIFY = (GK_CMD_ID + 2UL),
    GK_DELETE_USER = (GK_CMD_ID + 3UL),
    GK_DELETE_ALL_USERS = (GK_CMD_ID + 4UL),

    KEYMASTER_GENERATE_KEY_OLD = (KEYMASTER_CMD_ID_OLD + 1UL),
    KEYMASTER_IMPORT_KEY_OLD = (KEYMASTER_CMD_ID_OLD + 2UL),
    KEYMASTER_SIGN_DATA_OLD = (KEYMASTER_CMD_ID_OLD + 3UL),
    KEYMASTER_VERIFY_DATA_OLD = (KEYMASTER_CMD_ID_OLD + 4UL),

    KEYMASTER_LAST_CMD_ENTRY = (int) 0xFFFFFFFFULL
} keymaster_cmd_t;

/*
 * Structures for generate key cmd
 */
typedef struct _km_get_version_req_t
{
    keymaster_cmd_t cmd_id;
}__attribute__ ((packed)) km_get_version_req_t;

typedef struct _km_get_version_rsp_t
{
    int status;
    uint32_t major_version;
    uint32_t minor_version;
    uint32_t ta_major_version;
    uint32_t ta_minor_version;
}__attribute__ ((packed)) km_get_version_rsp_t;
typedef struct _km_set_version_req_t 
{ 
   keymaster_cmd_t cmd_id; 
   uint32_t major_version; 
   uint32_t minor_version; 
   uint32_t hal_major_version; 
   uint32_t hal_minor_version; 
   uint32_t req_rsp_buf_size; 
}__attribute__((packed)) km_set_version_req_t; 
typedef struct _km_set_version_rsp_t 
{ 
  int status; 
}__attribute__((packed)) km_set_version_rsp_t; 

/**
 @brief
 Data structure

 @param[in]   cmd_id       Requested command
 @param[in]   params   	params offset from the top of the struct.
 @param[in]   params_count The number of params present.

 params will be keymaster_key_param_t
 */
typedef struct _generate_key_req
{
    keymaster_cmd_t cmd_id;
    uint32_t params;
    uint32_t params_count;
}__attribute__ ((packed)) generate_key_req_t;

/**
 @brief
 Data structure

 @param[out]   status       	 Status of the request
 @param[out]   required_length  If status is KM_ERROR_INSUFFICIENT_BUFFER_SPACE,
 required_length points to the buffer size required.
 @param[out]   key_blob		 This is the struct that contains the keyblob.
 uint32_t key_material - This is the offset from
 the top of the buffer that
 points to the key material.
 uint32_t key_material_size - This is the size of the
 key material data.

 @param[out]   characteristics   This is the struct that contains the characteristics
 (both hw and sw).
 For each characteristics the below parameters,
 uint32_t params - This is the offset from
 the top of the buffer that
 points to the characteristics.
 uint32_t length - This is the buffer length.
 params will be keymaster_key_param_t
 */
typedef struct _generate_key_rsp
{
    int status;
    uint32_t required_length;
    keymaster_key_blob_t key_blob;
    keymaster_key_characteristics_t characteristics;
}__attribute__ ((packed)) generate_key_rsp_t;

/*
 * Structures for get_key_char cmd
 */
/**
 @brief
 Data structure

 @param[in]  cmd_id       		Requested command
 @param[in]  key_blob		 	This is the struct that contains the keyblob.
 uint32_t key_material - This is the offset from
 the top of the buffer that
 points to the key material.
 uint32_t key_material_size - This is the size of the
 key material data.

 @param[in]  client_id		 	This is the struct that contains the client_id (opaque blob to KM).
 uint32_t data		  - This is the offset from
 the top of the buffer that
 points to the client_id.
 uint32_t data_length  - This is the size of the
 client_id.

 @param[in]  application_data	This is the struct that contains the application_data (opaque blob to KM).
 uint32_t data		  - This is the offset from
 the top of the buffer that
 points to the application_data.
 uint32_t data_length  - This is the size of the
 application_data.
 params will be keymaster_key_param_t
 */
typedef struct _get_key_char_req_t
{
    keymaster_cmd_t cmd_id;
    keymaster_key_blob_t keyblob;
    keymaster_blob_t client_id;
    keymaster_blob_t application_data;
}__attribute__ ((packed)) get_key_char_req_t;

/**
 @brief
 Data structure

 @param[out]   status       	 Status of the request
 @param[out]   required_length  If status is KM_ERROR_INSUFFICIENT_BUFFER_SPACE,
 required_length points to the buffer size required.

 @param[out]   characteristics   This is the struct that contains the characteristics
 (both hw and sw).
 For each characteristics the below parameters,
 uint32_t params - This is the offset from
 the top of the buffer that
 points to the characteristics.
 uint32_t length - This is the buffer length.
 params will be keymaster_key_param_t
 */
typedef struct _get_key_char_rsp_t
{
    int status;
    uint32_t required_length;
    keymaster_key_characteristics_t characteristics;
}__attribute__ ((packed)) get_key_char_rsp_t;

/*
 * Structures for get_supported_alg cmd
 */
/**
 @brief
 Data structure

 @param[in]  cmd_id       		Requested command
 */
typedef struct _get_supported_alg_req_t
{
    keymaster_cmd_t cmd_id;
}__attribute__ ((packed)) get_supported_alg_req_t;

/**
 @brief
 Data structure

 @param[out]   status       	 Status of the request
 @param[out]   required_length  required_length points to the buffer size
 of supported_alg.
 @param[out]   supported_alg    Points to the supported_alg, this will grow
 based on required_length
 */
typedef struct _get_supported_alg_rsp_t
{
    int status;
    uint32_t required_length;
    keymaster_algorithm_t supported_alg[1];
}__attribute__ ((packed)) get_supported_alg_rsp_t;

/*
 * Structures for get_supported_block_modes cmd
 */
/**
 @brief
 Data structure

 @param[in]  cmd_id       		Requested command
 */
typedef struct _get_supported_block_modes_req_t
{
    keymaster_cmd_t cmd_id;
    keymaster_algorithm_t alg;
    keymaster_purpose_t purpose;
}__attribute__ ((packed)) get_supported_block_modes_req_t;

/**
 @brief
 Data structure

 @param[out]   status       	 		Status of the request
 @param[out]   required_length  		Points to the buffer size
 of supported_block_modes.
 @param[out]   supported_block_modes   Points to the supported_block_modes, this will grow
 based on required_length
 */
typedef struct _get_supported_block_modes_rsp_t
{
    int status;
    uint32_t required_length;
    keymaster_block_mode_t supported_block_modes[1];
}__attribute__ ((packed)) get_supported_block_modes_rsp_t;

/*
 * Structures for get_supported_padding_modes cmd
 */
/**
 @brief
 Data structure

 @param[in]  cmd_id       		Requested command
 */
typedef struct _get_supported_padding_modes_req_t
{
    keymaster_cmd_t cmd_id;
    keymaster_algorithm_t alg;
    keymaster_purpose_t purpose;
}__attribute__ ((packed)) get_supported_padding_modes_req_t;

/**
 @brief
 Data structure

 @param[out]   status       	 		Status of the request
 @param[out]   required_length  		Points to the buffer size of supported_padding_modes.
 @param[out]   supported_padding_modes Points to the supported_padding_modes, this will grow
 based on required_length
 */
typedef struct _get_supported_padding_modes_rsp_t
{
    int status;
    uint32_t required_length;
    keymaster_padding_t supported_padding_modes[1];
}__attribute__ ((packed)) get_supported_padding_modes_rsp_t;

/*
 * Structures for get_supported_digests cmd
 */
/**
 @brief
 Data structure

 @param[in]  cmd_id       		Requested command
 */
typedef struct _get_supported_digests_req_t
{
    keymaster_cmd_t cmd_id;
    keymaster_algorithm_t alg;
    keymaster_purpose_t purpose;
}__attribute__ ((packed)) get_supported_digests_req_t;

/**
 @brief
 Data structure

 @param[out]   status       	  Status of the request
 @param[out]   required_length   required_length points to the buffer size
 of supported_digests.
 @param[out]   supported_digests Points to the supported_digests, this will grow
 based on required_length
 */
typedef struct _get_supported_digests_rsp_t
{
    int status;
    uint32_t required_length;
    keymaster_digest_t supported_digests[1];
}__attribute__ ((packed)) get_supported_digests_rsp_t;

/*
 * Structures for get_supported_import_formats cmd
 */
/**
 @brief
 Data structure

 @param[in]  cmd_id       		Requested command
 */
typedef struct _get_supported_import_formats_req_t
{
    keymaster_cmd_t cmd_id;
    keymaster_algorithm_t alg;
}__attribute__ ((packed)) get_supported_import_formats_req_t;

/**
 @brief
 Data structure

 @param[out]   status       	  Status of the request
 @param[out]   required_length   required_length points to the buffer size
 of supported_import_formats.
 @param[out]   supported_import_formats Points to the supported_import_formats, this will grow
 based on required_length
 */
typedef struct _get_supported_import_formats_rsp_t
{
    int status;
    uint32_t required_length;
    keymaster_key_format_t supported_import_formats[1];
}__attribute__ ((packed)) get_supported_import_formats_rsp_t;

/*
 * Structures for get_supported_export_formats cmd
 */
/**
 @brief
 Data structure

 @param[in]  cmd_id       		Requested command
 */
typedef struct _get_supported_export_formats_req_t
{
    keymaster_cmd_t cmd_id;
    keymaster_algorithm_t alg;
}__attribute__ ((packed)) get_supported_export_formats_req_t;

/**
 @brief
 Data structure

 @param[out]   status       	  Status of the request
 @param[out]   required_length   required_length points to the buffer size
 of supported_export_formats.
 @param[out]   supported_export_formats Points to the supported_export_formats, this will grow
 based on required_length
 */
typedef struct _get_supported_export_formats_rsp_t
{
    int status;
    uint32_t required_length;
    keymaster_key_format_t supported_export_formats[1];
}__attribute__ ((packed)) get_supported_export_formats_rsp_t;

/*
 * Structures for import key cmd
 */

/**
 @brief
 Data structure

 @param[in]   cmd_id       Requested command
 @param[in]   params   	params offset from the top of the struct.
 @param[in]   params_count The number of params present.

 params will be keymaster_key_param_t
 */
typedef struct _import_key_req
{
    keymaster_cmd_t cmd_id;
    keymaster_key_format_t key_format;
    uint32_t params;
    uint32_t params_count;
    uint32_t key_data;
    uint32_t key_data_length;
}__attribute__ ((packed)) import_key_req_t;

/**
 @brief
 Data structure

 @param[out]   status       	 Status of the request
 @param[out]   required_length  If status is KM_ERROR_INSUFFICIENT_BUFFER_SPACE,
 required_length points to the buffer size required.
 @param[out]   key_blob		 This is the struct that contains the keyblob.
 uint32_t key_material - This is the offset from
 the top of the buffer that
 points to the key material.
 uint32_t key_material_size - This is the size of the
 key material data.

 @param[out]   characteristics   This is the struct that contains the characteristics
 (both hw and sw).
 For each characteristics the below parameters,
 uint32_t params - This is the offset from
 the top of the buffer that
 points to the characteristics.
 uint32_t length - This is the buffer length.
 params will be keymaster_key_param_t
 */
typedef struct _import_key_rsp
{
    int status;
    uint32_t required_length;
    keymaster_key_blob_t key_blob;
    keymaster_key_characteristics_t characteristics;
}__attribute__ ((packed)) import_key_rsp_t;

/*
 * Structures for export key cmd
 */

/**
 @brief
 Data structure

 @param[in]  cmd_id       		Requested command
 @param[in]  key_blob		 	This is the struct that contains the keyblob.
 uint32_t key_material - This is the offset from
 the top of the buffer that
 points to the key material.
 uint32_t key_material_size - This is the size of the
 key material data.

 @param[in]  client_id		 	This is the struct that contains the client_id (opaque blob to KM).
 uint32_t data		  - This is the offset from
 the top of the buffer that
 points to the client_id.
 uint32_t data_length  - This is the size of the
 client_id.

 @param[in]  application_data	This is the struct that contains the application_data (opaque blob to KM).
 uint32_t data		  - This is the offset from
 the top of the buffer that
 points to the application_data.
 uint32_t data_length  - This is the size of the
 application_data.
 params will be keymaster_key_param_t
 */
typedef struct _export_key_req
{
    keymaster_cmd_t cmd_id;
    keymaster_key_blob_t keyblob;
    keymaster_key_format_t key_format;
    keymaster_blob_t client_id;
    keymaster_blob_t application_data;
}__attribute__ ((packed)) export_key_req_t;

/**
 @brief
 Data structure

 @param[out]   status       	 Status of the request
 @param[out]   required_length  If status is KM_ERROR_INSUFFICIENT_BUFFER_SPACE,
 required_length points to the buffer size required.
 If Status is KM_ERROR_OK, length points to the total
 export_data_length

 @param[out]   export_data Points to the export_data, this will grow
 based on required_length
 params will be keymaster_key_param_t
 */
typedef struct _export_key_rsp
{
    int status;
    uint32_t required_length;
    keymaster_algorithm_t alg;
    uint32_t export_data;
    uint32_t export_data_length;
}__attribute__ ((packed)) export_key_rsp_t;

/*
 * Structures for begin cmd
 */
/**
 @brief
 Data structure

 @param[in]  cmd_id       		Requested command
 @param[in]  purpose			Purpose of the operation.
 @param[in]  key_blob		 	This is the struct that contains the keyblob.
 uint32_t key_material - This is the offset from
 the top of the buffer that
 points to the key material.
 uint32_t key_material_size - This is the size of the
 key material data.
 @param[in]   params   		params offset from the top of the struct.
 @param[in]   params_count 	The number of params present.
 */
typedef struct _key_op_begin_req_t
{
    keymaster_cmd_t cmd_id;
    keymaster_purpose_t purpose;
    keymaster_key_blob_t keyblob;
    uint32_t params;
    uint32_t params_count;
}__attribute__ ((packed)) key_op_begin_req_t;

/**
 @brief
 Data structure

 @param[out]   status       	 Status of the request
 @param[out]   required_length  If status is KM_ERROR_INSUFFICIENT_BUFFER_SPACE,
 required_length points to the buffer size required.
 @param[in]   out_params 		 params offset from the top of the struct.
 @param[in]   out_params_count  The number of params present.
 @param[in]   operation_handle  Operation handle to use for update/ finish/ abort operations.
 */
typedef struct _key_op_begin_rsp_t
{
    int status;
    uint32_t required_length;
    uint32_t out_params;
    uint32_t out_params_count;
    keymaster_operation_handle_t operation_handle;
    keymaster_algorithm_t alg;
}__attribute__ ((packed)) key_op_begin_rsp_t;

/*
 * Structures for update cmd
 */

/**
 @brief
 Data structure

 @param[in]  cmd_id       		Requested command
 @param[in]  operation_handle	Operation handle got from begin.
 @param[in]   params   		params offset from the top of the struct.
 @param[in]   params_count 	The number of params present.
 @param[in]   input_buf		Input buffer ptr - should be allocated by HLOS
 that contains the input data for the operation.
 @param[in]   input_length		Input buffer size.
 */
typedef struct _key_op_update_req_t
{
    keymaster_cmd_t cmd_id;
    keymaster_operation_handle_t operation_handle;
    uint32_t params;
    uint32_t params_count;
    uint32_t input_buf;
    uint32_t input_length;
}__attribute__ ((packed)) key_op_update_req_t;

/**
 @brief
 Data structure

 @param[out]   status       	 Status of the request
 @param[out]   required_length  If status is KM_ERROR_INSUFFICIENT_BUFFER_SPACE,
 required_length points to the buffer size required.
 @param[in]   output_buf		Output buffer ptr - should be allocated by HLOS
 that contains the output data for the operation.
 @param[in]   output_length	Output buffer size.
 @param[in]   input_consumed   The amount of input data that was consumed.
 */
typedef struct _key_op_update_rsp_t
{
    int status;
    uint32_t required_length;
    uint32_t output_buf;
    uint32_t output_length;
    uint32_t input_consumed;
}__attribute__ ((packed)) key_op_update_rsp_t;

/*
 * Structures for finish cmd
 */

/**
 @brief
 Data structure

 @param[in]  cmd_id       		Requested command
 @param[in]  operation_handle	Operation handle got from begin.
 @param[in]   params   		params offset from the top of the struct.
 @param[in]   params_count 	The number of params present.
 @param[in]   signature		Signature buffer ptr - should be allocated by HLOS
 that contains the signature for the operation (if required).
 @param[in]   signature_length	Signature buffer size.
 */
typedef struct _key_op_finish_req_t
{
    keymaster_cmd_t cmd_id;
    keymaster_operation_handle_t operation_handle;
    uint32_t params;
    uint32_t params_count;
    uint32_t signature;
    uint32_t signature_length;
}__attribute__ ((packed)) key_op_finish_req_t;

/**
 @brief
 Data structure

 @param[out]   status       	 Status of the request
 @param[out]   required_length  If status is KM_ERROR_INSUFFICIENT_BUFFER_SPACE,
 required_length points to the buffer size required.
 @param[in]   output_buf		Output buffer ptr - should be allocated by HLOS
 that contains the output data for the operation.
 @param[in]   output_length	Output buffer size.
 */
typedef struct _key_op_finish_rsp_t
{
    int status;
    uint32_t required_length;
    uint32_t output_buf;
    uint32_t output_length;
}__attribute__ ((packed)) key_op_finish_rsp_t;

/*
 * Structures for abort cmd
 */

/**
 @brief
 Data structure

 @param[in]  cmd_id       		Requested command
 @param[in]  operation_handle	Operation handle got from begin.
 */
typedef struct _key_op_abort_req_t
{
    keymaster_cmd_t cmd_id;
    keymaster_operation_handle_t operation_handle;
}__attribute__ ((packed)) key_op_abort_req_t;

/**
 @brief
 Data structure

 @param[out]   status       	 Status of the request
 */
typedef struct _key_op_abort_rsp_t
{
    int status;
}__attribute__ ((packed)) key_op_abort_rsp_t;

/*
 * Structures for delete cmd
 */
/**
 @brief
 Data structure

 @param[in]  cmd_id       		Requested command
 @param[in]  key_blob		 	This is the struct that contains the keyblob.
 uint32_t key_material - This is the offset from
 the top of the buffer that
 points to the key material.
 uint32_t key_material_size - This is the size of the
 key material data.
 */
typedef struct _key_op_delete_req_t
{
    keymaster_cmd_t cmd_id;
    keymaster_key_blob_t keyblob;
}__attribute__ ((packed)) key_op_delete_req_t;

/**
 @brief
 Data structure

 @param[out]   status       	 Status of the request
 */
typedef struct _key_op_delete_rsp_t
{
    int status;
}__attribute__ ((packed)) key_op_delete_rsp_t;

/*
 * Structures for delete_all cmd
 */
/**
 @brief
 Data structure

 @param[in]  cmd_id       		Requested command
 */
typedef struct _key_op_delete_all_req_t
{
    keymaster_cmd_t cmd_id;
}__attribute__ ((packed)) key_op_delete_all_req_t;

/**
 @brief
 Data structure

 @param[out]   status       	 Status of the request
 */
typedef struct _key_op_delete_all_rsp_t
{
    int status;
}__attribute__ ((packed)) key_op_delete_all_rsp_t;

/*
 * Utils Api struct
 */
/**
 @brief
 Data structure

 @param[in]   cmd_id      Requested command
 @param[in]   rot_ofset   Offset from the top of the struct.
 @param[in]   rot_size    Size of the ROT
 */
typedef struct _km_set_rot_req_t
{
    keymaster_cmd_t cmd_id;
    uint32_t rot_offset;
    uint32_t rot_size;
}__attribute__ ((packed)) km_set_rot_req_t;

/**
 @brief
 Data structure

 @param[out]   status      Status of the request
 */
typedef struct _km_set_rot_rsp_t
{
    int status;
}__attribute__ ((packed)) km_set_rot_rsp_t;

/**
 @brief
 Data structure

 @param[in]   cmd_id      Requested command
 @param[in]   data        information (could be data or a pointer to the memory that holds the data
 @param[in]   len         if data is ptr to some buffer, len indicates length of the buffer
 */
typedef struct send_cmd
{
    uint32_t cmd_id;
    uint32_t data;
    uint32_t len;
}__attribute__ ((packed)) send_cmd_t;

typedef struct send_cmd_rsp
{
    uint32_t cmd_id;
    uint32_t data;
    int32_t status;
}__attribute__ ((packed)) send_cmd_rsp_t;

/**
 @brief
 Data structure

 @param[in]   cmd_id      Requested command
 */
typedef struct _km_set_milestone_req_t
{
    keymaster_cmd_t cmd_id;
}__attribute__ ((packed)) km_set_milestone_req_t;

/**
 @brief
 Data structure

 @param[out]   status      Status of the request
 */
typedef struct _km_set_milestone_rsp_t
{
    int status;
}__attribute__ ((packed)) km_set_milestone_rsp_t;

/*
 * Utils Api struct
 */
/**
 @brief
 Data structure

 @param[in]   cmd_id                   Requested command
 @param[in]   hw_authenticator_type_t  Authenticator type
 */
typedef struct _km_get_auth_token_req_t
{
    keymaster_cmd_t cmd_id;
    hw_authenticator_type_t auth_type;
}__attribute__ ((packed)) km_get_auth_token_req_t;

/**
 @brief
 Data structure

 @param[out]   status                Status of the request
 @param[out]   auth_token_key_offset The offset from the top of the struct.
 The payload is the encapsulated payload
 for the specific authenticator TA. The
 TA name is hardcoded so only the specific
 authenticator TA can use
 qsee_decapsulate_inter_app_message
 using source app name as "keymaster"
 @param[out]   auth_token_key_len    Length of the payload.
 */
typedef struct _km_get_auth_token_rsp_t
{
    int status;
    uint32_t auth_token_key_offset;
    uint32_t auth_token_key_len;
}__attribute__ ((packed)) km_get_auth_token_rsp_t;

#endif /* KM_MAIN_H */
