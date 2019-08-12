/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#pragma once

#include <stdint.h>

#ifndef MAX_FINGER_NUM
#define MAX_FINGER_NUM 10
#endif

typedef struct {
  uint32_t event;
  uint32_t x;
  uint32_t y;
  uint32_t timestamp; // unused
  uint32_t dirty; // changed since last read from TL
} touchData_t;

typedef struct {
  touchData_t finger[MAX_FINGER_NUM];
} fingerData_t;

/******************************************************************************/
/* (1) Start */
/******************************************************************************/
/** Start command */
typedef struct {
  uint32_t reserved;
} cmdStart_t;
/** Start response */
typedef struct {
  uint32_t sid; /**< Session number */
} rspStart_t;

/******************************************************************************/
/* (2) Stop */
/******************************************************************************/
/** Stop command */
typedef struct {
  uint32_t sid; /**< Session number */
} cmdStop_t;
/** Stop response */
typedef struct {
  uint32_t reserved;
} rspStop_t;

/******************************************************************************/
/* (3) GetData */
/******************************************************************************/
/** GetData command */
typedef struct {
  uint32_t sid; /**< Session number */
} cmdGetData_t;
/** GetData response */
typedef struct {
  fingerData_t data;
} rspGetData_t;

