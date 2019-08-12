/* ===================================================================
** Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: slprintf.c
** DESC: sensor language - slang - builtin print, sprint functions
** Revision History
** 11-09-2015       df      Fix static analysis errors
** ================================================================ */
#include "sli.h"
#include "array.h"
#include "literals.h"
#include "namespace.h"
#include "slmalloc.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_PRINTF_BUFFER   256

typedef struct  {
    char    *pair;
    char    c;
} escaped_chars;

// note: missing \a alarm, \b backspace, \f formfeed, \v vertical tab, \? question mark, \octal_digits.., \xhh.. hexadecimal digits
escaped_chars   esc_table[] =
{
    { "\\n",   '\n'},    // newline
    { "\\r",   '\r'},    // carriage return
    { "\\t",   '\t'},    // tab
    { "\\\\",  '\\'},    // backslash
    { "\\'",   '\''},    // single quote
    { "\\\"",  '\"'},    // double quote
    { "\\$",   '$'},     // \$ e.g. literal dollar sign
    { "\\@",   '@'},     // \@      literal @ sign
};

char *lit_undef = "undef";

/**
 * @name        LookupEscapedChar
 * @param[i]    pFrom pointer to backslash
 * @param[io]   gets single byte escape character value
 * @return      true | false
 */

int    LookupEscapedChar( const char *pFrom, char *c)
{
    unsigned int  i;
    for ( i = 0; i < sizeof( esc_table) / sizeof( escaped_chars); i++) {
        if ( strncmp( pFrom, esc_table[i].pair, 2) == 0) {
            *c = esc_table[i].c;
            return true;
        }
    }
    return false;           // probably \$ or \@.
}


/**
 * @name        rtGetSymbol
 * @brief       <symbol> ::= < '$' | '@'> <letter> [ <letter> |
 *              <digit> ]*
 * @param[i]    pFrom pointer to '$' or '@' sign
 * @param[io]   pLength gets number of characters in symbol
 */
static int rtGetSymbol( const char *pFrom, size_t *pLength)
{
    *pLength = 0;

    char c = *pFrom;
    if ( c == '$' || c == '@') {
        pFrom++;
        *pLength = 1;

        c = *pFrom;
        if ( isalphanumeric( c) ) {
            pFrom++;
            *pLength += 1;

            while (( c = *pFrom) != '\0') {
                if ( !isalphanumeric( c) ) {
                    break;
                }
                pFrom++;
                *pLength += 1 ;
            }
            return true;        // symbol is $scalar or @array
        }
    }
    return false;
}

/**
 * @name wrapSprintf  - wrapper for builtin 'sprintf' function
 * @param             - sameas real perl print function
 * @returns
 */
void wrapSprint( const array *parmlist, token *retval)
{
    retval->type = VALUE;
    retval->subtype = SCALAR;
    retval->datatype = UNDEFINED;
    int i;

//    for ( i = 0; i <= parmlist->high_water; i++) {

    char buffer[ MAX_PRINTF_BUFFER];
    char *pTo = buffer;

    // first string is format string. subsequent  are arguments for '%format' ...
    token *t = ( token *)get_array_element( parmlist, 0);
    if ( t == NULL) {
        RTErrorMsg( "missing argument\n");
        return;
    }

    if (( t->subtype == STRING || t->subtype == SCALAR)
          && t->datatype == POINTER) {

        char *pFrom = (char *)t->data.p;

        char c;
        while (( c = *pFrom) != '\0') {
            if ( c == '\\') {
                 if ( LookupEscapedChar( pFrom, &c)) {
                     *pTo++  = c;                       // substitute special ff, lf, cr, ....
                     pFrom  += 2;
                    continue;
                 }
                 else {
                     printf( "\nwrapPrint() unknown escape %s\n", pFrom);
                     pFrom  += 2;
                 }
            }
            else if ( c == '$' || c == '@') {
                size_t length;
                if ( rtGetSymbol( pFrom, &length)) {

                    char symbuf[ 32];
                    length = ( length < sizeof( symbuf) - 1) ? length : sizeof( symbuf) - 1;
                    strlcpy( symbuf, pFrom, length);
                    symbuf[ length] = '\0';

                    token lookup;    InitToken( &lookup);
                    lookup.pSymbol = symbuf;

                    token *pVar = AllLayerLookup( &lookup);
                    if ( pVar) {
                        if ( pVar->type == UNDEFINED || pVar->datatype == UNDEFINED) {
                            strcpy( pTo, lit_undef);
                            pTo += strlen( lit_undef);
                            pFrom += length;
                        }
                        else if ( pVar->datatype == INTEGER) {
                            int count = snprintf( symbuf, sizeof( symbuf), "%ld", pVar->data.i);
                            strlcpy( pTo, symbuf, count);
                            pTo += count;
                            pFrom += length;
                        }
                        else if ( pVar->datatype == FLOAT) {
                            int count = snprintf( symbuf, sizeof( symbuf), "%f", pVar->data.d);
                            strlcpy( pTo, symbuf, count);
                            pTo += count;
                            pFrom += length;
                        }
                        else if ( pVar->datatype == POINTER) {
                            // stub TODO assumes string
                            int count = strlen( pVar->data.p);
                            strlcpy( pTo, pVar->data.p, count);
                            pTo += count;
                            pFrom += length;
                        }
                        else {                          // stub now redundent
                            fprintf( stderr, "wrapPrint type %d, subtype %d, datatype %d\n",
                                    pVar->type, pVar->subtype, pVar->datatype);
                            pFrom += length;
                        }
                        continue;
                    }

                    strlcpy( pTo, pFrom, length);       // symbol not found !
                    pTo += length;
                    pFrom += length;
                    continue;
                }
                else {
                    FatalError( "malformed symbol");
                }
            }
            else {
                *pTo++ = c;
                *pFrom++;
            }
        }
        *pTo = '\0';        // append null

        int length = (int)( pTo - buffer) + 1;
        char *p_memory = slmalloc( length, "slprintf.1");
        if ( p_memory == NULL) {
            RTErrorMsg( get_literal( OOM));
        }
        else {
            memcpy( p_memory, buffer, length);
            retval->memtype = P_MALLOCED;
            retval->subtype = STRING;
            retval->datatype = POINTER;
            retval->data.p = p_memory;
        }
    }
    else {
        fprintf( stderr, "not yet implemented print( t->pSymbol)\n");
    }
}

/**
 * @name wrapPrint  - wrapper for builtin 'sprintf' function
 * @param           - sameas real perl print function
 * @returns         - none
 */
void wrapPrint( const array *parmlist, token *retval)
{
    wrapSprint( parmlist, retval);
    if ( retval->datatype == POINTER) {
        char *p_buffer = retval->data.p;

        printf( "%s", p_buffer);

        slfree( p_buffer, "sprintf.f1");
        retval->datatype == UNDEFINED;
        retval->data.p = NULL;
    }
}

