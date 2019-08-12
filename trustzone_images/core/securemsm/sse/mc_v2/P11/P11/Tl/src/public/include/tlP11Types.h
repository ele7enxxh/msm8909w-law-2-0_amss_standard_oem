/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef TLP11_TYPES_H
#define TLP11_TYPES_H

#include "cryptoki.h"

#define CKR_OBJECT_NOT_BOUND                  (CKR_VENDOR_DEFINED+1)
#define CKR_OBJECT_USED                       (CKR_VENDOR_DEFINED+2)
#define CKR_OBJECT_NOT_USED                   (CKR_VENDOR_DEFINED+3)
#define CKR_UNKNOWN_COMMAND                   (CKR_VENDOR_DEFINED+4)
#define CKR_SK_CONTINUE                    (CKR_VENDOR_DEFINED+5)
#define CKR_SK_CANCELLED                   (CKR_VENDOR_DEFINED+6)
#define CKR_INVALID_COMMAND                   (CKR_VENDOR_DEFINED+7)

#define ENUM_32BIT_SPACER           ((int32_t)-1)

// temporary, till it's in MC
#define CR_SID_INVALID      0xffffffff


#endif /* TLP11_TYPES_H */
