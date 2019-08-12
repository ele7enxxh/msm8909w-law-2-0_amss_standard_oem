/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "tlP11Mechanisms.h"

DECLARE_CIPHER_INIT_FUNC(rsa_cipher_init);
DECLARE_SIGN_VERIFY_INIT_FUNC(rsa_sign_verify_init);
DECLARE_SIGN_VERIFY_FINAL_FUNC(rsa_sign_verify_final);
DECLARE_GENERATE_KEY_PAIR_FUNC(rsa_generate);
DECLARE_WRAP_FUNC(rsa_wrap);
DECLARE_UNWRAP_FUNC(rsa_unwrap);
