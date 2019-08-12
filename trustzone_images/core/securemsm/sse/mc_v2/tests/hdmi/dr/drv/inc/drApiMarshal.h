/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */
#pragma once

#include "TlApi/TlApiError.h"

/** Each function must have its own ID.
 * Extend this list if you add a new function.
 */
typedef enum {
  FID_DRV_HDMI_GETSTATUS = 1,    /**< Get the status of the HDMI/HDCP link */
} tsFuncID_t;

/** Marshaled function parameters.
 * structs and union of marshaling parameters via TlApi.
 *
 * @note The structs can NEVER be packed !
 * @note The structs can NOT used via sizeof(..) !
 */
typedef struct {
  uint32_t hdmi_enable;
  uint32_t hdmi_sense;
  uint32_t hdcp_auth;
} drGetStatusParams_t;

typedef union {
  drGetStatusParams_t getStatus;   /**< Marshalled parameters */
} drMarshalingData_t, *drMarshalingData_ptr;
 
/** Union of marshaling parameters. */
typedef struct {
  uint32_t     functionId;      /**< Function identifier. */
  struct {
    tlApiResult_t retVal;       /**< Return value */
    drMarshalingData_t params;
  } payload;
} drMarshalingParam_t, *drMarshalingParam_ptr;

/** Driver ID. */
#define DRV_HDMI_ID 0x8102



