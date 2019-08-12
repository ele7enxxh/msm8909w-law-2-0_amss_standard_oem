/*
 * Copyright (c) 2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary.
 */

#ifndef TLWRAPPER_API_PARAMS_H_
#define TLWRAPPER_API_PARAMS_H_

#include "tci.h"
#include <mcSo.h>

/**
 * Termination codes
 */
#define EXIT_ERROR                      ((uint32_t)(-1))

/******************************************************************************/
/* (1) Wrap */
/******************************************************************************/
/** Wrap command */
typedef struct {
  mcSpid_t spid;
  mcUuid_t uuid;
  mcSoLifeTime_t lifetime;
  mcSoContext_t context;
  size_t plaintext;
  uint8_t *data;
  size_t   dataLen;
} params_cmdWrap_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdWrap_t params;
} cmdWrap_t;
/** Wrap response */
typedef struct {
  uint8_t *data;
  size_t   dataLen;
} params_rspWrap_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdWrap_t)];
  params_rspWrap_t    params;
} rspWrap_t;

/******************************************************************************/
/* (2) Unwrap */
/******************************************************************************/
/** Unwrap command */
typedef struct {
  uint8_t *data;
  size_t   dataLen;
} params_cmdUnwrap_t;
typedef struct {
  tciCommandHeader_t  header;         /**< Command header */
  params_cmdWrap_t params;
} cmdUnwrap_t;
/** Unwrap response */
typedef struct {
  uint8_t *data;
  size_t   dataLen;
} params_rspUnwrap_t;
typedef struct {
  tciResponseHeader_t  header;            /**< Command header */
  char empty[sizeof(params_cmdUnwrap_t)];
  params_rspUnwrap_t    params;
} rspUnwrap_t;


#endif // TLWRAPPER_API_PARAMS_H_
