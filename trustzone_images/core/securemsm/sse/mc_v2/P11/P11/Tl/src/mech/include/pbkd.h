/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include "tlP11Mechanisms.h"

#define MIN_PBKDF2_ITERATIONS 1000 // following standard

DECLARE_GENERATE_KEY_FUNC(pkcs5_pbkd2);
DECLARE_GENERATE_KEY_FUNC(pkcs5_pbkd2_generic_secret);
