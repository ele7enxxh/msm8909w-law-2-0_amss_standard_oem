/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: sam_common.h
** DESC: sensor language - slang - common sam functions
** Revision History
** ================================================================ */
#ifndef SAM_COMMON_H
#define SAM_COMMON_H

#include "sli.h"
#include "literals.h"
#include "slnew_req.h"
#include "slmalloc.h"

void CommonSetManT1InstanceId( void *p_body, const token *t_keyvalue);
void CommonGetManT3InstanceId( const respind *p_respind, const token *t_field, token *t_value);
void CommonGetOptT3InstanceId( const respind *p_respind, const token *t_field, token *t_value);
void CommonGetAttributesResp(  const respind *p_respind, const token *t_field, token *t_value);

#endif // SAM_COMMON_H


