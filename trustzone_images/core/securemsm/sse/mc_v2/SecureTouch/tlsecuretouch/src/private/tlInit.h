/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include <stdint.h>

#define TL_CONFIG_SIZE 0x100 /* 256 bytes */

#define TL_MAGIC_0  0x43545351  // "QSTC"
#define TL_MAGIC_1  0x51535443  // "CTSQ"
#define TL_CONFIG_VERSION_MAJOR 1
#define TL_CONFIG_VERSION_MINOR 0

typedef struct {
  uint32_t w; /**< width of the touch screen, in px */
  uint32_t h; /**< height of the touch screen, in px */
} tl_config_arg_t;

typedef struct {
  uint32_t magic0;
  uint16_t       major;  /* integer portion of version number */
  uint16_t       minor;  /* 1/100ths portion of version number */
  tl_config_arg_t arg;
  uint8_t reserved[TL_CONFIG_SIZE - 12 - sizeof(tl_config_arg_t)];
  uint32_t magic1;
} tl_config_t;
