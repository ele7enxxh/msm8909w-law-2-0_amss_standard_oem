/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include "TlApiTsDefs.h"

#ifdef MAIN_C
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN struct tsFingerData *g_fingers_ptr;
EXTERN uint32_t driver_version;
