/*
 * Copyright(c) 2013 Qualcomm Technologies, Inc.All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include <stdint.h>

#include "sse_defs.h"

#define MAX_INPUT_FIELDS  2
#define MAX_INPUT_FIELD_LEN 64

typedef enum {
  SECURETOUCH_EVENT_DATA          = 0,
  SECURETOUCH_EVENT_CMD_CANCELLED = 1,
  SECURETOUCH_EVENT_CMD_COMPLETED = 2,
  SECURETOUCH_EVENT_CMD_MAX       = 0x7FFFFFFF,
} SecureTouch_Event_t;

typedef enum {
  SECURETOUCH_COMPLETION_STATUS_INPROGRESS = 0,
  SECURETOUCH_COMPLETION_STATUS_COMPLETED  = 1,
  SECURETOUCH_COMPLETION_STATUS_CANCELLED  = 2,
  SECURETOUCH_COMPLETION_STATUS_ABORTED    = 3,
  SECURETOUCH_COMPLETION_STATUS_MAX        = 0x7FFFFFFF,
} SecureTouch_CompletionStatus_t;

/**
 * Maintain the status of the input fields, to allow the
 * backspace function to remove the last, multibyte, character
 * entered, without adding runtime UTF-8 capabilities to the TL.
 */
typedef struct {
  uint32_t focus;  /**< index of the input field with focus */
  uint32_t num_input; /**< number of input fields in the active layout */
  SecureTouch_CompletionStatus_t status; /**< whether the user has pressed enter and completed the entry */
  uint32_t num_chars[MAX_INPUT_FIELDS]; /**< number of (possibly multibytes) characters in the input field */
  uint8_t num_bytes_in_char[MAX_INPUT_FIELDS][MAX_INPUT_FIELD_LEN]; /**< */
} SecureTouch_InputStatus_t;

typedef struct {
  DECLARE_ENC_START();
  uint32_t nonce;
  SecureTouch_CompletionStatus_t status; /**< will never be TL_SECURETOUCH_COMPLETION_STATUS_INPROGRESS */
  uint32_t length[MAX_INPUT_FIELDS]; /**< length in bytes of the content of the input field */
  uint8_t input_fields[MAX_INPUT_FIELDS][MAX_INPUT_FIELD_LEN]; /**< input fields, in UTF-8 chars */
} SecureTouchResponse_t;
