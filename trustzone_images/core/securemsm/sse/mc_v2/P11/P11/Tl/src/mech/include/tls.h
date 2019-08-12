/*
 * Copyright (c) 2012-2013 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TLS_H_
#define TLS_H_

#include "tlP11Mechanisms.h"

DECLARE_DERIVE_FUNC(tls_pseudorandom_function);
DECLARE_GENERATE_KEY_FUNC(tls_pre_master_key);
DECLARE_DERIVE_FUNC(tls_derive_master_secret);
DECLARE_DERIVE_FUNC(tls_derive_key_and_mac);

#endif /* TLS_H_ */
