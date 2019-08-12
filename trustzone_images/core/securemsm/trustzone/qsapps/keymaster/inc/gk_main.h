/* =================================================================================
 *  Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 * =================================================================================
 */

#ifndef GK_MAIN_H
#define GK_MAIN_H

#define GK_HMAC_SHA256_KEY_SIZE 32
/**
 * Commands supported
 * Since Keymaster is supported int he same TA, if adding or
 * changing this, keep in sync in Keymaster(km_main.h).
 */

typedef uint64_t secure_id_t;
typedef uint64_t salt_t;
typedef boolean bool;
typedef enum
{
    GK_UNTRUSTED_ENROLL,
    GK_TRUSTED_ENROLL,
} gk_enroll_type;

typedef struct _gk_rpmb_data_t
{
    uint32_t num_failures;
    uint64_t last_checked_uptime;
    secure_id_t user_id;
    uint32_t uid;
}__attribute__ ((packed)) gk_rpmb_data_t;

/**
 * structure for easy serialization
 * and deserialization of password handles.
 */
static const uint8_t HANDLE_VERSION = 2;
typedef struct __password_handle_t
{
    // fields included in signature
    uint8_t version;
    secure_id_t user_id;
    uint64_t flags;

    // fields not included in signature
    salt_t salt;
    uint8_t signature[GK_HMAC_SHA256_KEY_SIZE];

    bool hardware_backed;
}__attribute__ ((__packed__)) password_handle_t;

/**
 * Enrolls desired_password, which should be derived from a user selected pin or password,
 * with the authentication factor private key used only for enrolling authentication
 * factor data.
 *
 * If there was already a password enrolled, it should be provided in
 * current_password_handle, along with the current password in current_password
 * that should validate against current_password_handle.
 *
 * Parameters:
 * - dev: pointer to gatekeeper_device acquired via calls to gatekeeper_open
 * - uid: the Android user identifier
 *
 * - current_password_handle: the currently enrolled password handle the user
 *   wants to replace. May be null if there's no currently enrolled password.
 * - current_password_handle_length: the length in bytes of the buffer pointed
 *   at by current_password_handle. Must be 0 if current_password_handle is NULL.
 *
 * - current_password: the user's current password in plain text. If presented,
 *   it MUST verify against current_password_handle.
 * - current_password_length: the size in bytes of the buffer pointed at by
 *   current_password. Must be 0 if the current_password is NULL.
 *
 * - desired_password: the new password the user wishes to enroll in plain-text.
 *   Cannot be NULL.
 * - desired_password_length: the length in bytes of the buffer pointed at by
 *   desired_password.
 *
 * - enrolled_password_handle: on success, a buffer will be allocated with the
 *   new password handle referencing the password provided in desired_password.
 *   This buffer can be used on subsequent calls to enroll or verify.
 *   The caller is responsible for deallocating this buffer via a call to free()
 * - enrolled_password_handle_length: pointer to the length in bytes of the buffer allocated
 *   by this function and pointed to by *enrolled_password_handle_length.
 *
 * Returns: 0 on success or an error code less than 0 on error.
 * On error, enrolled_password_handle will not be allocated.
 */

/*
 * current_password_handle, current_password, desired_password, enrolled_password_handle
 * are all offsets from the top of the struct.
 */
typedef struct _gk_enroll_req_t
{
    keymaster_cmd_t cmd_id;
    uint32_t uid;
    uint32_t current_password_handle;
    uint32_t current_password_handle_length;
    uint32_t current_password;
    uint32_t current_password_length;
    uint32_t desired_password;
    uint32_t desired_password_length;
}__attribute__ ((packed)) gk_enroll_req_t;

typedef struct _gk_enroll_rsp_t
{
    int status;
    uint32_t enrolled_password_handle;
    uint32_t enrolled_password_handle_length;
}__attribute__ ((packed)) gk_enroll_rsp_t;

int
enroll(void *cmd, uint32_t cmdlen, void *rsp, uint32_t rsplen);

/**
 * Verifies provided_password matches enrolled_password_handle.
 *
 * Implementations of this module may retain the result of this call
 * to attest to the recency of authentication.
 *
 * On success, writes the address of a verification token to auth_token,
 * usable to attest password verification to other trusted services. Clients
 * may pass NULL for this value.
 *
 * Parameters:
 * - dev: pointer to gatekeeper_device acquired via calls to gatekeeper_open
 * - uid: the Android user identifier
 *
 * - challenge: An optional challenge to authenticate against, or 0. Used when a separate
 *              authenticator requests password verification, or for transactional
 *              password authentication.
 *
 * - enrolled_password_handle: the currently enrolled password handle that the
 *   user wishes to verify against.
 * - enrolled_password_handle_length: the length in bytes of the buffer pointed
 *   to by enrolled_password_handle
 *
 * - provided_password: the plaintext password to be verified against the
 *   enrolled_password_handle
 * - provided_password_length: the length in bytes of the buffer pointed to by
 *   provided_password
 *
 * - auth_token: on success, a buffer containing the authentication token
 *   resulting from this verification is assigned to *auth_token. The caller
 *   is responsible for deallocating this memory via a call to free()
 * - auth_token_length: on success, the length in bytes of the authentication
 *   token assigned to *auth_token will be assigned to *auth_token_length
 *
 * Returns: 0 on success or an error code less than 0 on error
 * On error, auth token will not be allocated
 */
typedef struct _gk_verify_req_t
{
    keymaster_cmd_t cmd_id;
    uint32_t uid;
    uint64_t challenge;
    uint32_t enrolled_password_handle;
    uint32_t enrolled_password_handle_length;
    uint32_t provided_password;
    uint32_t provided_password_length;
}__attribute__ ((packed)) gk_verify_req_t;

typedef struct _gk_verify_rsp_t
{
    int status;
    uint32_t auth_token;
    uint32_t auth_token_length;
}__attribute__ ((packed)) gk_verify_rsp_t;

int
verify(void *cmd, uint32_t cmdlen, void *rsp, uint32_t rsplen);

typedef struct _gk_delete_user_req_t
{
    keymaster_cmd_t cmd_id;
    uint32_t uid;
}__attribute__ ((packed)) gk_delete_user_req_t;

typedef struct _gk_delete_user_rsp_t
{
    int status;
}__attribute__ ((packed)) gk_delete_user_rsp_t;

int
delete_user(void *cmd, uint32_t cmdlen, void *rsp, uint32_t rsplen);

typedef struct _gk_delete_all_users_req_t
{
    keymaster_cmd_t cmd_id;
}__attribute__ ((packed)) gk_delete_all_users_req_t;

typedef struct _gk_delete_all_users_rsp_t
{
    int status;
}__attribute__ ((packed)) gk_delete_all_users_rsp_t;

int
delete_all_users(void *cmd, uint32_t cmdlen, void *rsp, uint32_t rsplen);
#endif // GK_MAIN_H
