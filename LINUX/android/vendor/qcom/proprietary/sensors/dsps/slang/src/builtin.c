/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: builtin.c
** DESC: sensor language - slang - interface to built in functions
** Revision History
** ================================================================ */
#include "sli.h"
#include "array.h"
#include "builtin.h"
#include "literals.h"
#include "namespace.h"
#include "slcore.h"
#include "slnew_req.h"

#include <stdio.h>
#include <unistd.h>     // sleep
#include <sys/time.h>

extern bool debugging;

bool option_script_handles_indications = false;
bool option_script_handles_errors = false;

#define MAX_PRINTF_BUFFER   256

typedef struct name_addr {
    char    *name;
    void    *addr;
} name_addr;

name_addr builtin[ 17];              // built-in functions

/**
 * @name wrapDefined    - wrapper for builtin 'defined' function
 * @brief               = Expression() has already set input
 *                      symbol's value
 * @param[i]            - array * parmlist->element[0] is token
 *       for symbol under question
 * @param[i]            - token *retval gets set
 *
 */
void wrapDefined( const array *parmlist, token *retval)
{
    retval->type = VALUE;
    retval->subtype = NUMBER;
    retval->datatype = INTEGER;
    retval->data.i = false;                         // not defined

    if ( parmlist != NULL) {
        token *t = ( token *)get_array_element( parmlist, parmlist->high_water);
        if ( t != NULL && t->datatype != UNDEFINED) {
            if ( t->datatype == INTEGER || t->datatype == FLOAT) {
                retval->data.i = true;              // defined - its value is not relevant
            }
            else if ( t->datatype == POINTER && t->pSymbol != NULL) {
                retval->data.i = true;              // defined - as not a NULL string; but defined as "" is true :-)
            }
        }
    }
}

void wrapSleep( const array *parmlist, token *retval)
{
    UNREFERENCED_PARAMETER( retval);

    unsigned int seconds = 10;      // default

    token *t = ( token *)get_array_element( parmlist, 0);
    if ( t != NULL) {
        if ( t->datatype == INTEGER) {
            seconds = ( unsigned int)t->data.i;
        }
    }
    if ( debugging) {
        printf( "sleep( %d)\n", seconds);
    }
    sleep( seconds);
}

void sloption( const array *parmlist, token *retval)
{
    retval->type = VALUE;
    retval->subtype = SCALAR;
    retval->datatype = INTEGER;
    retval->data.i = 0;

    token *t = ( token *)get_array_element( parmlist, 0);
    if ( t != NULL) {
        if ( t->subtype == STRING) {
            char *p_string = ( char *)t->data.p;
            if ( strcmp( "script_handles_indications", p_string) == 0) {
                option_script_handles_indications = true;
                return;
            }
            else if ( strcmp( "script_handles_errors", p_string) == 0) {
                    option_script_handles_errors = true;
                    return;
            }
        }
    }
    retval->data.i = -1;
    // stub setup get last error
    RTErrorMsg( get_literal( BAD_ARGUMENT));
}

/**
 * script usage: ( $seconds, $microsecs) = gettimeofday();
 */
void wrapGettimeofday( array *parmlist, token *retval)
{
    struct timeval tv;

    UNREFERENCED_PARAMETER( parmlist);
    retval->datatype = UNDEFINED;

    if ( gettimeofday( &tv, NULL) == 0) {
        time_t      tv_sec  = tv.tv_sec;
        suseconds_t tv_used = tv.tv_usec;

        array *output_array = new_array( 2);

        token *p_value = newToken();
        p_value->type    = VALUE;
        p_value->subtype = SCALAR;
        p_value->memtype = T_MALLOCED;      // token eligible for garbage collection
        p_value->datatype = INTEGER;
        p_value->data.i   = tv.tv_sec;
        output_array = append_array_element( output_array, p_value);

        p_value = newToken();
        p_value->type    = VALUE;
        p_value->subtype = SCALAR;
        p_value->memtype = T_MALLOCED;      // token eligible for garbage collection
        p_value->datatype = INTEGER;
        p_value->data.i   = tv.tv_usec;
        output_array = append_array_element( output_array, p_value);

        retval->type = PARMLIST;
        retval->memtype |= P_MALLOCED;              // token data.p eligible for garbage collection
        retval->subtype = ARRAY;
        retval->datatype = POINTER;
        retval->data.p = output_array;
    }
}

/**
 * @name IsBuiltInFunction
 * @param[i] pSymbol - pointer to builtin function name
 * @returns     NULL or address of builtin function.
 * ----------------------------------------------------------- */
void *IsBuiltInFunction( const char *pSymbol)
{
    void *function_addr = NULL;

    unsigned int i;
    for ( i = 0; i < sizeof( builtin) / sizeof( name_addr); i++) {

        if ( strcmp( pSymbol, builtin[ i].name) == 0) {
            function_addr = builtin[ i].addr;
            break;
        }
    }
    return function_addr;
}

void InitBuiltIn( void)
{
    builtin[ 0].name = "print";
    builtin[ 0].addr = &wrapPrint;
    builtin[ 1].name = "defined";
    builtin[ 1].addr = &wrapDefined;
    builtin[ 2].name = "slopen";
    builtin[ 2].addr = &slopen;
    builtin[ 3].name = "slclose";
    builtin[ 3].addr = &slclose;
    builtin[ 4].name = "slnew_req";
    builtin[ 4].addr = &slnew_req;
    builtin[ 5].name = "slsend_req";
    builtin[ 5].addr = &slsend_req;
    builtin[ 6].name = "slsend_recv";
    builtin[ 6].addr = &slsend_recv;
    builtin[ 7].name = "sleep";
    builtin[ 7].addr = &wrapSleep;
    builtin[ 8].name = "slget";
    builtin[ 8].addr = &slget;
    builtin[ 9].name = "slrecv";
    builtin[ 9].addr = &slrecv;
    builtin[ 10].name = "sloption";
    builtin[ 10].addr = &sloption;
    builtin[ 11].name = "fopen";
    builtin[ 11].addr = &wrapFopen;
    builtin[ 12].name = "fclose";
    builtin[ 12].addr = &wrapFclose;
    builtin[ 13].name = "fwrite";
    builtin[ 13].addr = &wrapFwrite;
    builtin[ 14].name = "sprint";
    builtin[ 14].addr = &wrapSprint;
    builtin[ 15].name = "slset_req";
    builtin[ 15].addr = &slset_req;
    builtin[ 16].name = "gettimeofday";
    builtin[ 16].addr = &wrapGettimeofday;
    // caution: if adding entries then also update builtin[ array size]
}

