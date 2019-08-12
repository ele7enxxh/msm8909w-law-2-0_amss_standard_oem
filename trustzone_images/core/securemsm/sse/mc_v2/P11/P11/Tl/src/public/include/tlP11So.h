/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _SO_H
#define _SO_H

#include "tlP11Lengths.h"

/**
 * Format used for Secure Objects pushed by TSM
 */

#define CKF_SO_INIT       0x00000001

#define CK_OTP_PARAM_NOT_SUPPORTED 8 // parameter cannot be used for OTP with this key

#pragma pack(push, so, 1)

typedef struct {
  uint32_t version;
  uint32_t cls; // can be either CKO_SECRET_KEY, CKO_CERTIFICATE, CKO_PUBLIC_KEY, CKO_PRIVATE_KEY
  uint8_t id[ID_MAX_LEN]; /**< CKA_ID */
  uint32_t id_len;
} tlp11_so_object_t;

typedef struct {
  tlp11_so_object_t object;
  uint32_t flags; // CKF which will be turned into CKA wherever appropriate
  uint32_t keyType; /**< now ignored, only AES supported */
} tlp11_so_key_t;

typedef struct {
  tlp11_so_key_t key;
  uint32_t len; /**< CKA_VALUE_LEN */
  uint8_t value[SECRET_KEY_MAX_LEN]; /**< CKA_VALUE */
} tlp11_so_secret_key_t;

typedef struct {
  tlp11_so_secret_key_t secret;
  uint8_t counter[OTP_COUNTER_LEN]; /**< CKA_OTP_COUNTER */
  char user_identifier[OTP_USER_ID_LEN]; /**< CKA_OTP_USER_IDENTIFIER */
  uint32_t user_identifier_len;
  char service_identifier[OTP_SERVICE_ID_LEN];
  uint32_t service_identifier_len;
  uint8_t service_logo[OTP_SERVICE_LOGO_LEN]; /**< CKA_OTP_SERVICE_LOGO */
  uint32_t service_logo_len;
  char service_logo_type[OTP_SERVICE_LOGO_TYPE_LEN]; /**< CKA_OTP_SERVICE_LOGO_TYPE */
  uint32_t service_logo_type_len;
} tlp11_so_otp_key_t;

typedef struct {
  tlp11_so_key_t key;
  uint32_t derDataLen; /**< CKA_VALUE_LEN */
  uint8_t derData[WRAPPED_DATA_MAX_LEN]; /**< CKA_VALUE */
} tlp11_so_public_key_t, tlp11_so_private_key_t;

typedef struct {
  tlp11_so_object_t object;
  uint8_t check_value[CHECK_VALUE_LEN]; /**< CKA_CHECK_VALUE */
  uint8_t spacer;
  struct {
    unsigned char       year[4];   /* the year ("1900" - "9999") */
    unsigned char       month[2];  /* the month ("01" - "12") */
    unsigned char       day[2];    /* the day   ("01" - "31") */
  } start_date; /**< CKA_START_DATE */
  struct {
    unsigned char       year[4];   /* the year ("1900" - "9999") */
    unsigned char       month[2];  /* the month ("01" - "12") */
    unsigned char       day[2];    /* the day   ("01" - "31") */
  } end_date; /**< CKA_END_DATE */
  uint8_t subject[SUBJECT_MAX_LEN]; /**< CKA_SUBJECT */
  uint32_t subject_len;
  uint8_t issuer[ISSUER_MAX_LEN]; /**< CKA_ISSUER */
  uint32_t issuer_len;
  uint8_t serial_number[SERIAL_NUMBER_MAX_LEN]; /**< CKA_SERIAL_NUMBER */
  uint32_t serial_number_len;
  uint8_t value[X509_VALUE_MAX_LEN]; /**< CKA_VALUE */
  uint32_t value_len;
} tlp11_so_x509_t;

typedef union {
  tlp11_so_object_t object;
  tlp11_so_key_t key;
  tlp11_so_secret_key_t secret;
  tlp11_so_otp_key_t otp;
  tlp11_so_public_key_t pub;
  tlp11_so_private_key_t prv;
  tlp11_so_x509_t x509;
} tlp11_so_container_t;

#pragma pack(pop, so)

#endif // _SO_H
