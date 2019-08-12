/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#pragma once

#include <stdint.h>

#define MAX_FINGER_NUM  10

#define TLAPI_TOUCH_EVENT_NONE  0
#define TLAPI_TOUCH_EVENT_DOWN  1
#define TLAPI_TOUCH_EVENT_MOVE  2
#define TLAPI_TOUCH_EVENT_UP    4

struct tsTouchData {
  uint32_t event;
  uint32_t x;
  uint32_t y;
  uint32_t timestamp; // unused
  uint32_t dirty; // changed since last read from TL
};

struct tsFingerData {
  struct tsTouchData finger[MAX_FINGER_NUM];
};

/** Invalid session id. Returned in case of an error. */
#define DR_SID_INVALID      0xffffffff



