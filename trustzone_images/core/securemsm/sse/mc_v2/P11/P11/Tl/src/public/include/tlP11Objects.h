/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TLP11_OBJECTS_P11_H
#define TLP11_OBJECTS_P11_H


#include "tlP11Types.h"
#include "tlP11Lengths.h"
#include "tlP11So.h"
#include "mcSo.h"
#include <stddef.h>

#pragma pack(push, obj, 1)

typedef char UTF8CHAR;

#define SO_OVERHEAD_MAX MC_SO_SIZE(0,0) // 92

/*lint -emacro(737,IS_TYPE)*/
#define IS_TYPE(obj,type) \
  (((obj)->header.object_type & (type)) == (type))

/*lint -emacro(737,BELONGS_TO_TYPES)*/
#define BELONGS_TO_TYPES(obj,type) \
  (((obj)->header.object_type & (type)) == (obj)->header.object_type)

/** Start of encrypted portion.
 * Dummy object used to declare the start of the encrypted portion of the derived secure object.
 */
#define DECLARE_ENC_START() \
    uint32_t so_enc_start

typedef enum {
  tlp11_keypd_event_started  = 0,
  tlp11_keypd_event_char     = 1,
  tlp11_keypd_event_password_done   = 2,
  tlp11_keypd_event_cancel   = 3,
  tlp11_keypd_event_confirmation_done   = 4,
  tlp11_keypd_event_spacer   = ENUM_32BIT_SPACER
} tlp11_keypad_event_t;

typedef enum {
  tlp11_type_undefined =          0x00000000,
  tlp11_type_storage =            0x00000001,  // leave 0 unassigned
  tlp11_type_certificate =       (0x00000002 | tlp11_type_storage),
  tlp11_type_certificate_x509 =  (0x00000004 | tlp11_type_certificate),
  tlp11_type_data =              (0x00000008 | tlp11_type_storage),
  tlp11_type_key =               (0x00000010 | tlp11_type_storage),
  tlp11_type_secret_key =        (0x00000100 | tlp11_type_key),
  tlp11_type_aes_key =           (0x00000200 | tlp11_type_secret_key),
  tlp11_type_des3_key =          (0x00000400 | tlp11_type_secret_key),
  tlp11_type_des_key =           (0x00000800 | tlp11_type_secret_key),
  tlp11_type_generic_key =       (0x00001000 | tlp11_type_secret_key),
  tlp11_type_rc4_key =           (0x00002000 | tlp11_type_secret_key),
  tlp11_type_otp_key =           (0x00004000 | tlp11_type_secret_key),
  tlp11_type_public_key =        (0x00010000 | tlp11_type_key),
  tlp11_type_rsa_public_key =    (0x00020000 | tlp11_type_public_key),
  tlp11_type_private_key =       (0x00100000 | tlp11_type_key),
  tlp11_type_rsa_private_key =   (0x00200000 | tlp11_type_private_key),
  tlp11_type_reserved_otp_params=(0x10000000),
  tlp11_type_reserved_otp_info = (0x20000000),
  tlp11_type_unused_spacer = ENUM_32BIT_SPACER
} tlp11_object_type_enum_t;

typedef struct {
  CK_ATTRIBUTE_TYPE type;
  CK_ULONG value;
} tlp11_ulong_attribute_t;

typedef struct {
  CK_ATTRIBUTE_TYPE type;
  CK_BBOOL value;
} tlp11_bbool_attribute_t;

typedef struct {
  CK_ATTRIBUTE_TYPE type;
  uint8_t value[SUBJECT_MAX_LEN];
  uint32_t len;
} tlp11_array_attribute_t;


 /** Template for CKA_WRAP_TEMPLATE and CKA_UNWRAP_TEMPLATE */
typedef struct {
  tlp11_array_attribute_t array_attribute;
  tlp11_ulong_attribute_t ulong_attributes[MAX_ULONG_ATTRIBUTES_IN_TEMPLATE];
  tlp11_bbool_attribute_t bbool_attributes[MAX_BBOOL_ATTRIBUTES_IN_TEMPLATE];
} tlp11_attribute_template_t;

typedef struct {
  uint32_t version;
  uint32_t stamp; /**< Token or session stamp */
  tlp11_object_type_enum_t object_type; /**< final object type */
  uint32_t changed; /**< volatile, for internal use only */
  uint32_t magic;
  uint32_t reserved;
} tlp11_object_header_t;

/** CKO base class. */
typedef struct {
  tlp11_object_header_t header;
  /* From here, proper PKCS#11 attributes */
  CK_OBJECT_CLASS cls; /**< CKA_CLASS */
} tlp11_object_t;

/** CKO storage class. */
typedef struct {
  tlp11_object_t object;
  CK_BBOOL token; /**< CKA_TOKEN */
  CK_BBOOL prvt; /**< CKA_PRIVATE */
  CK_BBOOL modifiable; /**< CKA_MODIFIABLE */
  uint8_t spacer;
  UTF8CHAR label[LABEL_LEN]; /**< CKA_LABEL */
  uint32_t label_len;
} tlp11_storage_t;

/** CKO_CERTIFICATE. */
typedef struct {
  tlp11_storage_t storage;
  CK_CERTIFICATE_TYPE type; /**< CKA_CERTIFICATE_TYPE */
  uint8_t check_value[CHECK_VALUE_LEN]; /**< CKA_CHECK_VALUE */
  CK_BBOOL trusted; /**< CKA_TRUSTED */
  CK_ULONG category; /**< CKA_CERTIFICATE_CATEGORY */
  CK_DATE start_date; /**< CKA_START_DATE */
  CK_DATE end_date; /**< CKA_END_DATE */
} tlp11_certificate_t;


/** X.509 CERTIFICATE. */
typedef struct {
  tlp11_certificate_t certificate; /**< CKO_CERTIFICATE */
  uint8_t subject[SUBJECT_MAX_LEN]; /**< CKA_SUBJECT */
  uint32_t subject_len;
  uint8_t id[ID_MAX_LEN]; /**< CKA_ID */
  uint32_t id_len;
  uint8_t issuer[ISSUER_MAX_LEN]; /**< CKA_ISSUER */
  uint32_t issuer_len;
  uint8_t serial_number[SERIAL_NUMBER_MAX_LEN]; /**< CKA_SERIAL_NUMBER */
  uint32_t serial_number_len;
  uint8_t hash_of_subject_public_key[PUBLIC_KEY_HASH_LEN]; /**< CKA_HASH_OF_SUBJECT_PUBLIC_KEY*/
  uint8_t hash_of_issuer_public_key[PUBLIC_KEY_HASH_LEN]; /**< CKA_HASH_OF_ISSUER_PUBLIC_KEY*/
  CK_ULONG java_midp_security_domain; /**< CKA_JAVA_MIDP_SECURITY_DOMAIN */
  uint8_t value[X509_VALUE_MAX_LEN]; /**< CKA_VALUE */
  uint32_t value_len;
  UTF8CHAR url[URL_MAX_LEN];
  uint32_t url_len;
} tlp11_certificate_x509_t;

/** CKO_DATA. */
typedef struct {
  tlp11_storage_t storage;
  UTF8CHAR application[APPLICATION_LEN]; /**< CKA_APPLICATION */
  uint32_t application_len;
  uint8_t object_id[OBJECT_ID_MAX_LEN]; /**< CKA_OBJECT_ID */
  uint32_t object_id_len;
  DECLARE_ENC_START();
  uint8_t value[DATA_MAX_LEN]; /**< CKA_VALUE */
  uint32_t value_len;
} tlp11_data_t;

/** Key. */
typedef struct {
  tlp11_storage_t storage;
  CK_KEY_TYPE key_type; /**< CKA_KEY_TYPE */
  uint8_t id[ID_MAX_LEN]; /**< CKA_ID */
  uint32_t id_len;
  CK_DATE start_date; /**< CKA_START_DATE */
  CK_DATE end_date; /**< CKA_END_DATE */
  CK_BBOOL derive; /**< CKA_DERIVE */
  CK_BBOOL local; /**< CKA_LOCAL */
  uint16_t spacer;
  CK_MECHANISM_TYPE key_gen_mechanism; /**< CKA_KEY_GEN_MECHANISM */
  CK_MECHANISM_TYPE allowed_mechanisms[MAX_ALLOWED_MECHANISMS]; /**< CKA_ALLOWED_MECHANISM */
} tlp11_key_t;

/** Public key */
typedef struct {
  tlp11_key_t key;
  char subject[SUBJECT_MAX_LEN]; /**< CKA_SUBJECT */
  uint32_t subject_len;
  CK_BBOOL encrypt; /**< CKA_DECRYPT */
  CK_BBOOL verify; /**< CKA_VERIFY */
  CK_BBOOL verify_recovery; /**< CKA_VERIFY_RECOVERY */
  CK_BBOOL wrap; /**< CKA_WRAP */
  CK_BBOOL trusted; /**< CKA_TRUSTED */
  uint8_t spacer[3];
  tlp11_attribute_template_t wrap_template; /**< CKA_WRAP_TEMPLATE */
} tlp11_public_key_t;

/** Private Key */
typedef struct {
  tlp11_key_t key;
  char subject[SUBJECT_MAX_LEN]; /**< CKA_SUBJECT */
  uint32_t subject_len;
  CK_BBOOL sensitive; /**< CKA_SENSITIVE */
  CK_BBOOL decrypt; /**< CKA_DECRYPT */
  CK_BBOOL sign; /**< CKA_SIGN */
  CK_BBOOL sign_recovery; /**< CKA_SIGN_RECOVERY */
  CK_BBOOL unwrap; /**< CKA_UNWRAP */
  CK_BBOOL extractable; /**< CKA_EXTRACTABLE */
  CK_BBOOL always_sensitive; /**< CKA_ALWAYS_SENSITIVE */
  CK_BBOOL never_extractable; /**< CKA_NEVER_EXTRACTABLE */
  CK_BBOOL wrap_with_trusted; /**< CKA_WRAP_WITH_TRUSTED */
  CK_BBOOL always_authenticate; /**< CKA_ALWAYS_AUTHENTICATE */
  uint16_t spacer;
  tlp11_attribute_template_t unwrap_template; /**< CKA_UNWRAP_TEMPLATE */
} tlp11_private_key_t;

/** RSA Private Key. */
typedef struct {
  tlp11_private_key_t private_key;
  uint8_t modulus[MODULUS_MAX_LEN]; /**< CKA_MODULUS */
  uint32_t modulus_len;
  uint8_t public_exponent[PUBLIC_EXPONENT_LEN]; /**< CKA_PUBLIC_EXPONENT */
  uint32_t public_exponent_len;
  DECLARE_ENC_START();
  uint8_t exponent[PRIVATE_EXPONENT_MAX_LEN]; /**< CKA_PRIVATE_EXPONENT */
  uint32_t exponent_len;
  uint8_t prime1[PRIME1_MAX_LEN];
  uint32_t prime1_len;
  uint8_t prime2[PRIME2_MAX_LEN];
  uint32_t prime2_len;
  uint8_t exponent1[EXPONENT1_MAX_LEN];
  uint32_t exponent1_len;
  uint8_t exponent2[EXPONENT2_MAX_LEN];
  uint32_t exponent2_len;
  uint8_t coefficient[COEFFICIENT_MAX_LEN];
  uint32_t coefficient_len;
} tlp11_rsa_private_key_t;

/** RSA Public Key. */
typedef struct {
  tlp11_public_key_t public_key;
  uint8_t modulus[MODULUS_MAX_LEN]; /**< CKA_MODULUS */
  uint32_t modulus_len;
  uint32_t modulus_bits;
  uint8_t public_exponent[PUBLIC_EXPONENT_LEN]; /**< CKA_PUBLIC_EXPONENT */
  uint32_t public_exponent_len;
} tlp11_rsa_public_key_t;

typedef struct {
  tlp11_key_t key;
  CK_BBOOL encrypt; /**< CKA_DECRYPT */
  CK_BBOOL verify; /**< CKA_VERIFY */
  CK_BBOOL wrap; /**< CKA_WRAP */
  CK_BBOOL sensitive; /**< CKA_SENSITIVE */
  CK_BBOOL decrypt; /**< CKA_DECRYPT */
  CK_BBOOL sign; /**< CKA_SIGN */
  CK_BBOOL unwrap; /**< CKA_UNWRAP */
  CK_BBOOL extractable; /**< CKA_EXTRACTABLE */
  CK_BBOOL always_sensitive; /**< CKA_ALWAYS_SENSITIVE */
  CK_BBOOL never_extractable; /**< CKA_NEVER_EXTRACTABLE */
  CK_BBOOL wrap_with_trusted; /**< CKA_WRAP_WITH_TRUSTED */
  CK_BBOOL trusted; /**< CKA_TRUSTED */
  uint8_t check_value[CHECK_VALUE_LEN]; /**< CKA_CHECK_VALUE */
  uint8_t spacer;
  tlp11_attribute_template_t wrap_template; /**< CKA_WRAP_TEMPLATE */
  tlp11_attribute_template_t unwrap_template; /**< CKA_UNWRAP_TEMPLATE */
} tlp11_secret_key_t;

/** Secret Key. */
typedef struct {
  tlp11_secret_key_t secret;
  DECLARE_ENC_START();
  uint32_t len; /**< CKA_VALUE_LEN */
  uint8_t value[SECRET_KEY_MAX_LEN]; /**< CKA_VALUE */
} tlp11_secret_key_simple_t;

/** Secret Key. */
typedef struct {
  tlp11_secret_key_t secret;
  CK_ULONG format; /**< CKA_OTP_FORMAT */
  CK_ULONG length; /**< CKA_OTP_LENGTH */
  CK_BBOOL user_friendly_mode; /**< CKA_OTP_USER_FRIENDLY_MODE */
  uint8_t spacer[3];
  CK_ULONG challenge_requirement; /**< CKA_OTP_CHALLENGE_REQUIREMENT */
  CK_ULONG time_requirement; /**< CKA_OTP_TIME_REQUIREMENT */
  CK_ULONG counter_requirement; /**< CKA_OTP_COUNTER_REQUIREMENT */
  CK_ULONG pin_requirement; /**< CKA_OTP_PIN_REQUIREMENT */
  DECLARE_ENC_START();
  uint8_t counter[OTP_COUNTER_LEN]; /**< CKA_OTP_COUNTER */
  CK_ULONG time; /**< CKA_OTP_TIME */
  UTF8CHAR user_identifier[OTP_USER_ID_LEN]; /**< CKA_OTP_USER_IDENTIFIER */
  uint32_t user_identifier_len;
  UTF8CHAR service_identifier[OTP_SERVICE_ID_LEN]; /**< CKA_OTP_SERVICE_IDENTIFIER */
  uint32_t service_identifier_len;
  uint8_t service_logo[OTP_SERVICE_LOGO_LEN]; /**< CKA_OTP_SERVICE_LOGO */
  uint32_t service_logo_len;
  UTF8CHAR service_logo_type[OTP_SERVICE_LOGO_TYPE_LEN]; /**< CKA_OTP_SERVICE_LOGO_TYPE */
  uint32_t service_logo_type_len;
  uint32_t len; /**< CKA_VALUE_LEN */
  uint8_t value[OTP_KEY_MAX_LEN]; /**< CKA_VALUE */
} tlp11_otp_secret_key_t;

typedef union {
  tlp11_object_header_t header;
  tlp11_object_t object;
  tlp11_storage_t storage;
  tlp11_key_t key;
  tlp11_public_key_t public_key;
  tlp11_private_key_t private_key;
  tlp11_secret_key_t secret_key;
  tlp11_secret_key_simple_t secret_key_simple;
  tlp11_otp_secret_key_t otp;
  tlp11_rsa_public_key_t rsa_public_key;
  tlp11_rsa_private_key_t rsa_private_key;
  tlp11_data_t data;
  tlp11_certificate_t certificate;
  tlp11_certificate_x509_t x509;
} tlp11_container_object_t;

#define SO_CONTAINER_OBJECT_MAX_LEN (sizeof(tlp11_container_object_t) + SO_OVERHEAD_MAX)

typedef struct {
  uint32_t changed;
  size_t len;
  uint8_t value[SO_CONTAINER_OBJECT_MAX_LEN];
} tlp11_container_object_so_t;

#define SO_CONTAINER_MAX_LEN (sizeof(tlp11_so_container_t) + SO_OVERHEAD_MAX)

#pragma pack(pop, obj)

#endif /* TLP11_OBJECTS_P11_H */
