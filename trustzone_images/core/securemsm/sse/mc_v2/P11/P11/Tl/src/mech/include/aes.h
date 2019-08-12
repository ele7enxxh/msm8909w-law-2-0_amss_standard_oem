/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "tlP11Mechanisms.h"

DECLARE_CIPHER_INIT_FUNC(aes_cipher_init);
DECLARE_GENERATE_KEY_FUNC(aes_generate);
DECLARE_WRAP_FUNC(aes_wrap);
DECLARE_UNWRAP_FUNC(aes_unwrap);
DECLARE_DERIVE_FUNC(aes_encrypt_data);
DECLARE_WRAP_FUNC(wrapkey_aes_cbc_pad_wrap);
DECLARE_UNWRAP_FUNC(wrapkey_aes_cbc_pad_unwrap);
