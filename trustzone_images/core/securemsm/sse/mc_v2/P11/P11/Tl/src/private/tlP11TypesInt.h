/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _tlP11TypesINT_H
#define _tlP11TypesINT_H

#include "TlApi/TlApiError.h"

#define CKR_STUBBED (tlApiResult_t)0x00100074
#if defined(MOBICORE_CRYPTO)
/*lint -emacro(506,IS_STUBBED) -emacro(774,IS_STUBBED)*/
#define IS_STUBBED(x) (1==0)
#else
#define IS_STUBBED(x) (x == CKR_STUBBED)
#endif
#define IS_OK(x) ((x == TLAPI_OK)/* || IS_STUBBED(x)*/)
#define STUB(x) (CKR_STUBBED)

#define LENGTH(x) sizeof(x)/sizeof((x)[0])

#if defined(OPENSSL_CRYPTO_LIBRARY) || defined(MOBICORE_CRYPTO)
  #define CRYPTO_OP(x) (x)
#else
  #define CRYPTO_OP(x) (CKR_STUBBED)
#endif

#define MAGIC_TOKEN   0x4E4B4F54 //TOKN
#define MAGIC_SESSION 0x50474E53 //SNGP
#define MAGIC_OBJECT  0x544A424F //OBJT

#endif
