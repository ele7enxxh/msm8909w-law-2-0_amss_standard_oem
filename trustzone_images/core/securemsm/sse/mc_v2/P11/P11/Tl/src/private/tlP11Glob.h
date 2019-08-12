/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _tlP11Glob_H
#define _tlP11Glob_H

#include "tlP11Objects.h"
#include "tlP11Token.h"
#include "tlP11Init.h"
#include "tlP11So.h"
#include "tlP11Utils.h"


#ifdef MAIN_TL_C
#define EXTERN
#else
#define EXTERN extern
#endif

// we use it to unwrap:
// - a token
// - a session
// - an object
// - an SO object (from TSM)
#define UNWRAP_BUFFER_SIZE max(SO_TOKEN_MAX_LEN,max(SO_SESSION_GROUP_MAX_LEN,max(SO_CONTAINER_OBJECT_MAX_LEN,SO_CONTAINER_MAX_LEN)))

// global convenience objects
EXTERN tsSource_t g_tsSource;
EXTERN tlp11_session_group_t g_session_group_0;
EXTERN tlp11_token_t *g_token_0_ptr;

#ifdef SK_ENABLED
/* Variables used for Secure Keypad */
EXTERN UTF8CHAR pin[PASSWORD_MAX_LEN]; // placeholder for PIN/PASSWORD as it is entered
EXTERN uint32_t  pinLen;  // length of the PIN entered so far
EXTERN UTF8CHAR pinVerification[PASSWORD_MAX_LEN]; // placeholder for PIN/PASSWORD verification as it is entered
EXTERN uint32_t  pinVerLen;  // length of the PIN entered so far

// Maximum length of the PIN as expected by the calling app
// capped with the max supported length
EXTERN uint32_t pinMaxLen;
EXTERN uint32_t pinFailed;
EXTERN uint32_t pinEntered;
#endif

EXTERN uint32_t loginFailed;

extern tlp11_config_t __attribute__((section("gconfig"))) g_config;

#endif
