/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: diagnotics.h
** DESC: sensor language - slang - diagnostic error reporting
** Revision History
** ================================================================ */
#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include "literals.h"

void BadFieldName( const char *fieldname);
void FatalError( const char *pMsg);
void InvalidFieldName( const char *fieldname);
void ErrReasonForField( literals_e literal, const char *fieldname);
void ErrReasonValueForFieldName( literals_e reason, const char *p_symbol, const char *fieldname);
void UninitializedVariable( const char *pSymbol);


#endif // DIAGNOSTICS_H




