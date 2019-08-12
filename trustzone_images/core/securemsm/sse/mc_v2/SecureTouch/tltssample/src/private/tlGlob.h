/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include "touchSampleApi.h"

#ifdef MAIN_C
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN touchSampleCommandPayload_t g_command;
