/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#pragma once

#include "TlApi/TlApiError.h"
#include "TlApiTsDefs.h"

/** Each function must have its own ID.
 * Extend this list if you add a new function.
 */
enum tsFuncID {
  FID_DRV_TS_OPEN = 1,    /**< Open a session with the secure touch driver */
  FID_DRV_TS_CLOSE = 2,   /**< Close the current session with the secure touch driver */
  FID_DRV_TS_GETDATA = 3 /**< Get the data associated with active fingers */
};

/** Marshaled function parameters.
 * structs and union of marshaling parameters via TlApi.
 *
 * @note The structs can NEVER be packed !
 * @note The structs can NOT used via sizeof(..) !
 */
struct drGetDataParams {
  struct tsFingerData *fingers;
};

typedef union {
  struct drGetDataParams getData;   /**< Marshalled parameters */
} drMarshalingData_t;
 
/** Union of marshaling parameters. */
typedef struct {
  uint32_t     functionId;      /**< Function identifier. */
  struct {
    tlApiResult_t retVal;       /**< Return value */
    drMarshalingData_t params;
  } payload;
  uint32_t sid;                 /**< Session Identifier */
} drMarshalingParam_t;

/** Driver ID. */
#define DRV_TS_ID 0x8101
