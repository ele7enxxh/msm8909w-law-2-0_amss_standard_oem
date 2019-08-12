/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include <stdint.h>

int32_t i2cReadReg(const uint16_t address, void *buf, const uint32_t bufLen);

int32_t i2cOpen(void);

int32_t i2cClose(void);

