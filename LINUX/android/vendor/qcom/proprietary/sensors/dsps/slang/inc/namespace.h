/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: namespace.c
** DESC: sensor language - slang - block structured name space
** Revision History
** ================================================================ */
#ifndef NAMESPACE_H
#define NAMESPACE_H

#include "sli.h"
#include "array.h"

array *AddNameSpaceLayer( void);
void RemoveNameSpaceLayer( void);
void InitNameSpace( void);
void  AppendNewSymbol( token *lhs);
token *OneLayerLookup( const array *pLayer, const token *nu);
token *ThisLayerLookup( const token *nu);
token *AllLayerLookup( const token *nu);

#endif // NAMESPACE_H

