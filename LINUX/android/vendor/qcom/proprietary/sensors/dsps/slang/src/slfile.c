/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: slfile.c
** DESC: sensor language - slang - builtin file io functions
** Revision History
** ================================================================ */
#include "sli.h"
#include "array.h"
#include "literals.h"
#include "slmalloc.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

char last_error[ 96];

/**
 * @name wrapFopen
 * @brief
 * @param[ i]   parmlist
 * @param[ io]  retval
 * script usage examples:
 * my $fd = fopen( "path", "mode");
 */

void wrapFopen( const array *parmlist, token *retval)
{
    retval->type = VALUE;
    retval->subtype = SCALAR;
    retval->datatype = UNDEFINED;       // default fopen failed
    retval->data.p = NULL;

    token *t_path = ( token *)get_array_element( parmlist, 0);
    if ( !( t_path && ( t_path->subtype == SCALAR || t_path->subtype == STRING))) {
        RTErrorMsg( get_literal( BAD_ARGUMENT));
        return;
    }
    const char *p_path = t_path->data.p;

    token *t_mode = ( token *)get_array_element( parmlist, 1);
    if ( !( t_mode && ( t_mode->subtype == SCALAR || t_mode->subtype == STRING))) {
        RTErrorMsg( get_literal( BAD_ARGUMENT));
        return;
    }
    const char *p_mode = t_mode->data.p;

    FILE *fd = fopen( p_path, p_mode);
    if ( fd) {
        retval->datatype = POINTER;
        retval->data.p = fd;
    }
    else {
        int errnum = errno;
        strerror_r( errnum, last_error, sizeof(last_error));
    }
}

/**
 * @name wrapFclose
 * @brief
 * @param[ i]   parmlist
 * @param[ io]  retval
 * script usage examples:
 * my $rc = fclose( $fd);
 */
void wrapFclose( const array *parmlist, token *retval)
{
    retval->type = VALUE;
    retval->subtype = SCALAR;
    retval->datatype = INTEGER;

    token *t_fd = ( token *)get_array_element( parmlist, 0);
    if ( t_fd == NULL || t_fd->datatype != POINTER) {
        RTErrorMsg( get_literal( BAD_ARGUMENT));
        return;
    }

    FILE *fd = (FILE *)t_fd->data.p;

    int rc = fclose( fd);
    retval->data.i = rc;

    if ( rc) {
        int errnum = errno;
        strerror_r( errnum, last_error, sizeof(last_error));
    }
}

/**
 * @name wrapFwrite
 * @brief
 * @param[ i]   parmlist
 * @param[ io]  retval
 * script usage examples:
 * my $length = fwrite( $fd, $string);
 */
void wrapFwrite( const array *parmlist, token *retval)
{
    retval->type = VALUE;
    retval->subtype = SCALAR;
    retval->datatype = INTEGER;

    token *t_fd = ( token *)get_array_element( parmlist, 0);
    if ( t_fd == NULL || t_fd->datatype != POINTER) {
        RTErrorMsg( get_literal( BAD_ARGUMENT));
        return;
    }

    FILE *fd = (FILE *)t_fd->data.p;

    token *t_string = ( token *)get_array_element( parmlist, 1);
    if ( !( t_string && ( t_string->subtype == SCALAR || t_string->subtype == STRING))) {
        RTErrorMsg( get_literal( BAD_ARGUMENT));
        return;
    }

    char *p_string = t_string->data.p;

    size_t length = strlen( p_string);

    size_t bytes_written = fwrite( p_string, length, 1, fd);
    if ( bytes_written != length) {
        int errnum = errno;
        strerror_r( errnum, last_error, sizeof(last_error));
    }

    retval->data.i = bytes_written;
}
