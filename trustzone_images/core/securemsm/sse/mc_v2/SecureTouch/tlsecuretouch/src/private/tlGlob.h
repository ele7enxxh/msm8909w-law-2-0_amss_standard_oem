/*
 * Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#include "tlSecureTouchApi.h"
#include "tlInit.h"

#ifdef MAIN_C
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN tlSecureTouchCommandPayload_t g_command;
EXTERN tlSecureTouchRequest_t g_request; /**< need this one to keep track who we have to respond to */
EXTERN tlSecureTouchResponse_t g_response;
EXTERN uint32_t g_sid;                /**< session with the touch driver */
EXTERN SecureTouch_InputStatus_t g_inputStatus; /**< status of the input fields */
EXTERN LayoutPage_t g_page;
extern const tl_config_t g_config;
