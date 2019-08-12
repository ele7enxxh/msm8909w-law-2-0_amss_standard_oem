/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: diagnostics.c
** DESC: sensor language - slang - display compile/run-time errors
** Revision History
** ================================================================ */
#include "sli.h"
#include "diagnostics.h"
#include "literals.h"

#include <stdio.h>
#include <stdlib.h>

#define ERRBUF_SIZE 64

void BadFieldName( const char *fieldname)
{
    char errbuf[ ERRBUF_SIZE];
    snprintf( errbuf, sizeof(errbuf), "%s \'%s\'", get_literal( BAD_FIELD), fieldname);
    RTErrorMsg( errbuf);
}

void FatalError( const char *msg)
{
    RTErrorMsg( msg);
    exit( 4);
}

void InvalidFieldName( const char *fieldname)
{
    char errbuf[ ERRBUF_SIZE];
    snprintf( errbuf, sizeof(errbuf), "invalid field name \'%s\'", fieldname);
    RTErrorMsg( errbuf);
}

void UninitializedVariable( const char *pSymbol)
{
    char errbuf[ ERRBUF_SIZE];
    snprintf( errbuf, sizeof(errbuf), "%s \'%s\'", get_literal( UNINITIALIZED_SYMBOL), pSymbol);
    RTErrorMsg( errbuf);
}

/**
 * @name        ErrReasonForField
 * @param[i]    literal - e.g. BAD_VALUE
 * @param[i]    fieldname
 * @brief       e.g. bad value for fieldname
 */
void ErrReasonForField( literals_e literal, const char *fieldname)
{
    char    errbuf[ ERRBUF_SIZE];
    snprintf( errbuf, sizeof(errbuf), "%s for \'%s\'", get_literal( literal), fieldname);
    RTErrorMsg( errbuf);
}

void ErrReasonValueForFieldName( literals_e reason, const char *p_symbol, const char *fieldname)
{
    char    errbuf[ ERRBUF_SIZE];
    snprintf( errbuf, sizeof(errbuf), "%s \'%s\' for \'%s\'",
              get_literal( reason), p_symbol, fieldname);
    RTErrorMsg( errbuf);
}



