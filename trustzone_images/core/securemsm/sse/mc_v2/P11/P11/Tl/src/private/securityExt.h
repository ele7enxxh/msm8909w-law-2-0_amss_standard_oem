/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _SECURITY_EXT_H
#define _SECURITY_EXT_H

#include "tlStd.h"
#include "tlP11Init.h"

uint32_t sticky(const tlp11_token_t *token);

uint32_t conflictingAttributes(const tlp11_token_t *token);

uint32_t secureTemplates(const tlp11_token_t *token);

uint32_t minIterationsPbkdf2SK(const tlp11_token_t *token);

uint32_t weakerKey(const tlp11_token_t *token);

uint32_t getKeyStrength(const tlp11_container_object_t *object);

uint32_t mechanismIsSecure(const CK_MECHANISM_TYPE mech);

uint32_t secureWrapOnly(const tlp11_token_t *token);

uint32_t trustedWrapOnly(const tlp11_token_t *token);

#endif
