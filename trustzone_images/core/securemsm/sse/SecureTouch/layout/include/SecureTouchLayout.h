/*
 * Copyright(c) 2013 Qualcomm Technologies, Inc.All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include <stdint.h>

#define UTF8_MAX_CHAR_LEN 6
#define MAX_LAYOUT_OBJECTS_IN_PAGE  48

#ifdef __RVCT__
  #pragma anon_unions
#endif

typedef enum {
  LAYOUT_OBJECT_TYPE_UNUSED  = 0,
  LAYOUT_OBJECT_TYPE_INPUT   = 1,
  LAYOUT_OBJECT_TYPE_BUTTON  = 2,
  LAYOUT_OBJECT_TYPE_OTHER   = 0x800,
  LAYOUT_OBJECT_TYPE_MAX     = 0x7FFFFFFF
} LayoutObjectType_t;

typedef enum {
  LAYOUT_BUTTON_TYPE_DATA      = 0,
  LAYOUT_BUTTON_TYPE_FUNCTION  = 1,
  LAYOUT_BUTTON_TYPE_MAX       = 0x7FFFFFFF,
} LayoutButtonType_t;

typedef enum {
  LAYOUT_BUTTON_FUNCTION_ENTER     = 0,
  LAYOUT_BUTTON_FUNCTION_CANCEL    = 1,
  LAYOUT_BUTTON_FUNCTION_BACKSPACE = 2,
  LAYOUT_BUTTON_FUNCTION_CLEAR     = 3,
  LAYOUT_BUTTON_FUNCTION_MAX       = 0x7FFFFFFF,
} LayoutButtonFunction_t;

typedef struct __attribute__((packed)){
     uint16_t x1;
     uint16_t y1;
     uint16_t x2;
     uint16_t y2;
} Rect_t;

typedef struct __attribute__((packed)){
  uint32_t length;
  uint8_t data[UTF8_MAX_CHAR_LEN]; // 6
  uint8_t empty[2];
} LayoutButtonData_t;

typedef struct {
  LayoutButtonType_t type;
  union {
    LayoutButtonData_t data;
    LayoutButtonFunction_t function;
  };
} LayoutObjectButton_t;

typedef struct {
  uint32_t index;
} LayoutObjectInput_t;

typedef struct {
  LayoutObjectType_t type;
  Rect_t rect;
  union {
    LayoutObjectButton_t button;
    LayoutObjectInput_t input;
  };
} LayoutObject_t;

typedef struct {
  uint32_t number;
  LayoutObject_t objects[MAX_LAYOUT_OBJECTS_IN_PAGE];
} LayoutPage_t;

