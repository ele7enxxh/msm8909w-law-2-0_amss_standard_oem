/*
 * Copyright(c) 2013 Qualcomm Technologies, Inc.All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include <stdint.h>
#include <stddef.h> // for offsetof

/** Start of encrypted portion.
 * Dummy member used to declare the start of the encrypted
 * portion of the derived secure object.
 */
#define DECLARE_ENC_START() \
    uint32_t so_enc_start

#define PLAINLEN(x) (offsetof(x,so_enc_start)+sizeof(uint32_t))
#define ENCRYPTEDLEN(x) (sizeof(x) - PLAINLEN(x))

