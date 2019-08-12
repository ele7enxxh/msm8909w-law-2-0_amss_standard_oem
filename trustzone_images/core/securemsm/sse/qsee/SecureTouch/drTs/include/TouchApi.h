/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#pragma once

#include <stdint.h>
#include "TouchApiDefs.h"

uint32_t touchOpen(uint32_t *sid);

uint32_t touchClose(const uint32_t sid);

uint32_t touchGetData(const uint32_t sid, struct tsFingerData *fingers);
