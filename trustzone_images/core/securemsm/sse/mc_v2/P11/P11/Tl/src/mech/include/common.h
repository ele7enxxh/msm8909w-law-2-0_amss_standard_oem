/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "tlP11Mechanisms.h"

DECLARE_CIPHER_UPDATE_FUNC(common_cipher_update);
DECLARE_CIPHER_FINAL_FUNC(common_cipher_final);
DECLARE_SIGN_VERIFY_UPDATE_FUNC(common_sign_verify_update);
DECLARE_DERIVE_FUNC(derive_data_base);
DECLARE_DERIVE_FUNC(derive_base_key);
DECLARE_DERIVE_FUNC(derive_extract);
