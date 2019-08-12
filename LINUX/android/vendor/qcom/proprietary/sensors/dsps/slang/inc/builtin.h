/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: builtin.h
** DESC: sensor language - slang - builtin functions
** Revision History
** ================================================================ */
#ifndef BUILTIN_H
#define BUILTIN_H

#include "sli.h"

void  InitBuiltIn( void);
void *IsBuiltInFunction( const char *pSymbol);

// slfile.c
void wrapFopen( const array *parmlist, token *retval);
void wrapFclose( const array *parmlist, token *retval);
void wrapFwrite( const array *parmlist, token *retval);

// slget.c
void slget( const array *parmlist, token *retval);

// slnew_req.c
void slnew_req( const array *parmlist, token *retval);

// slprintf.c
void wrapPrint( const array *parmlist, token *retval);
void wrapSprint( const array *parmlist, token *retval);

#endif // BUILTIN

