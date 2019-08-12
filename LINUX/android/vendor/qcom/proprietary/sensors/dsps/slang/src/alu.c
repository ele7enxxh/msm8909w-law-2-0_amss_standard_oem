/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: alu.c
** DESC: sensor language - slang - arithmetic logic unit
** Revision History
** 11-09-2015       df      Fix static analysis errors
** ================================================================ */
#include "sli.h"
#include "slmalloc.h"

#include <stdbool.h>    // true, false
#include <stdint.h>
#include <string.h>     // NULL
#include <stdio.h>

extern int      runtime;


void ToFloat( token *retval, token *rhs)
{
    if ( retval->datatype == INTEGER) {
        retval->data.d = (double)retval->data.i;
        retval->datatype = FLOAT;
    }
    if ( rhs->datatype == INTEGER) {
        rhs->data.d = (double)rhs->data.i;
        rhs->datatype = FLOAT;
    }
}

/**
 * @name        Alu
 * @brief       arithmetic logic unit
 * @param[io]   retval - left hand argument gets output result
 * @param[i]    op
 * @param[i]    rhs
 *
 * @notes:      for now INTEGERs only
 *
 */
void Alu( token *retval, const char *op, token *rhs)
{
    if ( !runtime) {
        return;
    }
    if ( retval == NULL) {
        return;
    }

    if ( op == NULL) {
        fprintf( stderr, "Alu op == NULL\n");
        retval->datatype = UNDEFINED;
        retval->data.p = NULL;
        return;
    }

    if ( rhs == NULL) {
        if ( strcmp( op, "-") == 0) {                           // handle unary minus
            retval->data.i = 0L - retval->data.i;
        }                                                       // these are probably both identical in function
        else if ( strcmp( op, "!") == 0) {                      // negation !
            retval->data.i = !retval->data.i;
        }
        return;
    }

    if ( rhs->type == UNDEFINED || rhs->datatype == UNDEFINED) {
        retval->datatype = UNDEFINED;
        retval->data.p = NULL;
        return;
    }

    if ( retval->subtype == ARRAY || rhs->subtype == ARRAY) {
        fprintf( stderr, "Alu unsupported subtype ARRAY\n");
        retval->datatype = UNDEFINED;
        retval->data.p = NULL;
        return;
    }

    /*
    ** assignment
    ** ------- */
    if ( strcmp( op, "=") == 0) {
        if ( retval->datatype == POINTER                        // free dynamically allocated memory
             && ( retval->memtype & P_MALLOCED)) {              // + when lhs symbol's pointer value
            slfree( retval->data.p, "alu.1");                   // + is getting re-assigned
            retval->data.p = NULL;
            retval->memtype ^= P_MALLOCED;
        }

        retval->memtype   |= ( rhs->memtype & P_MALLOCED);      // propogate rhs memory malloced bit.
        retval->datatype   = rhs->datatype;
        retval->data.p     = rhs->data.p;
        return;
    }

    /*
    ** string comparison
    ** -------------- */
    if ( retval->datatype == POINTER && rhs->datatype == POINTER
         && ( retval->type == SYMBOL || retval->type == CONSTANT)
         && ( rhs->type == SYMBOL || rhs->type == CONSTANT)
         && isRelOp( op)) {

        int result = strcmp( (char *)retval->data.p, (char *)rhs->data.p);
        retval->datatype = INTEGER;
        retval->data.i = false;

        if ( strcmp( op, "==") == 0) {
            if ( result == 0) {
                retval->data.i = true;
            }
        }
        else if ( strcmp( op, ">=") == 0) {
            if ( result >= 0) {
                retval->data.i = true;
            }
        }
        else if ( strcmp( op, ">") == 0) {
            if ( result > 0) {
                retval->data.i = true;
            }
        }
        else if ( strcmp( op, "<=") == 0) {
            if ( result <= 0) {
                retval->data.i = true;
            }
        }
        else if ( strcmp( op, "<") == 0) {
            if ( result < 0) {
                retval->data.i = true;
            }
        }
        else if (( strcmp( op, "!=") == 0) || ( strcmp( op, "<>") == 0)) {
            if ( result != 0) {
                retval->data.i = true;
            }
        }
        return;
    }

    /*
    ** either operand POINTER
    ** ------------------- */
    if ( retval->datatype == POINTER || rhs->datatype == POINTER) {
        if ( retval->pSymbol) {
            fprintf( stderr, "retval->pSymbol %s\n",retval->pSymbol);
        }
        else {
            fprintf( stderr, "retval->pSymbol is NULL\n");
        }
        fprintf( stderr, "op %s\n", op);
        if ( rhs->pSymbol) {
            fprintf( stderr, "rhs->pSymbol %s\n",rhs->pSymbol);
        }
        else {
            fprintf( stderr, "rhs->pSymbol is NULL\n");
        }
        fprintf( stderr, "Alu unsupported pointer operation\n");
        retval->datatype = UNDEFINED;
        retval->data.p = NULL;
        return;
    }

    /*
    ** both operands INTEGER
    ** ------------------ */
    if ( retval->datatype == INTEGER && rhs->datatype == INTEGER) {
        if ( strcmp( op, "+") == 0 || strcmp( op, "+=") == 0) {
            retval->data.i += rhs->data.i;
        }
        else if ( strcmp( op, "-") == 0 || strcmp( op, "-=") == 0) {
            retval->data.i -= rhs->data.i;
        }
        else if ( strcmp( op, "*") == 0 || strcmp( op, "*=") == 0) {
            retval->data.i *= rhs->data.i;
        }
        else if ( strcmp( op, "/") == 0 || strcmp( op, "/=") == 0) {
            ToFloat( retval, rhs);                  // expect fractions after divide
            retval->data.d /= rhs->data.d;
        }
        /* comparison */
        else if ( strcmp( op, "==") == 0) {
            retval->data.i = ( retval->data.i == rhs->data.i);
        }
        else if ( strcmp( op, ">=") == 0) {
            retval->data.i = ( retval->data.i >= rhs->data.i);
        }
        else if ( strcmp( op, ">") == 0) {
            retval->data.i = ( retval->data.i > rhs->data.i);
        }
        else if ( strcmp( op, "<=") == 0) {
            retval->data.i = ( retval->data.i <= rhs->data.i);
        }
        else if ( strcmp( op, "<") == 0) {
            retval->data.i = ( retval->data.i < rhs->data.i);
        }
        else if ( strcmp( op, "!=") == 0) {
            retval->data.i = ( retval->data.i != rhs->data.i);
        }
        /* assignment */
        else if ( strcmp( op, "=") == 0) {
            retval->datatype   = rhs->datatype;
            retval->data.p     = rhs->data.p;
        }
        else if ( strcmp( op, "%=") == 0) {
            retval->data.i     %= rhs->data.i;
        }
        else if ( strcmp( op, "&=") == 0) {
            retval->data.i     &= rhs->data.i;
        }
        else if ( strcmp( op, "|=") == 0) {
            retval->data.i     |= rhs->data.i;
        }
        else if ( strcmp( op, "^=") == 0) {
            retval->data.i     ^= rhs->data.i;
        }
        else if ( strcmp( op, "<<=") == 0) {
            retval->data.i     <<= rhs->data.i;
        }
        else if ( strcmp( op, ">>=") == 0) {
            retval->data.i     >>= rhs->data.i;
        }
        /* logical */
        else if ( strcmp( op, "&&") == 0) {
            retval->data.i = ( retval->data.i && rhs->data.i);
        }
        else if ( strcmp( op, "||") == 0) {
            retval->data.i = ( retval->data.i || rhs->data.i);
        }
        /* bitwise */
        else if ( strcmp( op, "&") == 0) {
            retval->data.i &= rhs->data.i;
        }
        else if ( strcmp( op, "|") == 0) {
            retval->data.i |= rhs->data.i;
        }
        /* shift */
        else if ( strcmp( op, "<<") == 0) {
            retval->data.i <<= rhs->data.i;
        }
        else if ( strcmp( op, ">>") == 0) {
            retval->data.i >>= rhs->data.i;
        }
        else {
            fprintf( stderr, "Alu unsupported integer op %s\n", op);
            retval->datatype = UNDEFINED;
        }
        return;
    }

    /*
    ** FLOATING POINT
    ** ------------------ */
    ToFloat( retval, rhs);

    if ( strcmp( op, "+") == 0 || strcmp( op, "+=") == 0) {
        retval->data.d += rhs->data.d;
    }
    else if ( strcmp( op, "-") == 0 || strcmp( op, "-=") == 0) {
        retval->data.d -= rhs->data.d;
    }
    else if ( strcmp( op, "*") == 0 || strcmp( op, "*=") == 0) {
        retval->data.d *= rhs->data.d;
    }
    else if ( strcmp( op, "/") == 0 || strcmp( op, "/=") == 0) {
        retval->data.d /= rhs->data.d;
    }
    /* comparison */
    else if ( strcmp( op, "==") == 0) {
        retval->data.i   = ( retval->data.d == rhs->data.d);
        retval->datatype = INTEGER;
    }
    else if ( strcmp( op, ">=") == 0) {
        retval->data.i   = ( retval->data.d >= rhs->data.d);
        retval->datatype = INTEGER;
    }
    else if ( strcmp( op, ">") == 0) {
        retval->data.i   = ( retval->data.d > rhs->data.d);
        retval->datatype = INTEGER;
    }
    else if ( strcmp( op, "<=") == 0) {
        retval->data.i   = ( retval->data.d <= rhs->data.d);
        retval->datatype = INTEGER;
    }
    else if ( strcmp( op, "<") == 0) {
        retval->data.i   = ( retval->data.d < rhs->data.d);
        retval->datatype = INTEGER;
    }
    else if ( strcmp( op, "!=") == 0) {
        retval->data.d   = ( retval->data.d != rhs->data.d);
        retval->datatype = INTEGER;
    }
    /* assignment */
    else if ( strcmp( op, "=") == 0) {
        retval->data.d     = rhs->data.d;
    }
    /* logical */
    else if ( strcmp( op, "&&") == 0) {
        retval->data.i   = ( retval->data.d && rhs->data.d);
        retval->datatype = INTEGER;
    }
    else if ( strcmp( op, "||") == 0) {
        retval->data.i   = ( retval->data.d || rhs->data.d);
        retval->datatype = INTEGER;
    }
    else {
        retval->datatype = UNDEFINED;
        fprintf( stderr, "Alu unsupported float op %s\n", op);
    }
}

