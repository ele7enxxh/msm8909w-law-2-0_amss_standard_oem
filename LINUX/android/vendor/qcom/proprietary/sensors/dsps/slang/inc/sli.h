/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: sli.h
** DESC: sensor language - slang - topmost slang include
** Revision History
** ================================================================ */
#ifndef SLI_H
#define SLI_H

#include "array.h"
#include "diagnostics.h"

#include <stdbool.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(x) (void)x;
#endif /* UNREFERENCED_PARAMETER */

/* -----------
** token.type
** ----------*/
enum {
    UNDEFINED,   //  0
    LPAREN,      //  1
    RPAREN,      //  2
    LBRACE,      //  3
    RBRACE,      //  4
    SEMICOLON,   //  5
    COMMA,       //  6
    OPERATOR,    //  7 - type
    CONSTANT,    //  8 - type
    SYMBOL,      //  9 - type
    PARMLIST,    // 10 - ProcCall ( parm, ... )
    SUBLIST,     // 11 - ProcCall '{' key => value, ...'}'
    VALUE,       // 12 - type e.g <boolean value> = defined( symbol)
    NUMBER,      // 13 - subtype
    STRING,      // 14 - subtype
    SCALAR,      // 15 - subtype
    ARRAY,       // 16 - subtype
    SUBROUTINE,  // 17 - subtype
    KEYVALUE,    // 18 - subtype
    RESPONSE,    // 19 - subtype
    BAREWORD,    // 20 - subtype    proccall bareword -> builtin || subroutine
    BUILTIN,     // 21   subtype
    INTEGER,     // 22 - datatype
    FLOAT,       // 23 - datatype
    POINTER,     // 24 - datatype
    ISEOF,       // 25
};

// memtype bits
// token is malloced - eligible for garbage collection
// data.p is malloced - eligible for garbage collection
#define T_MALLOCED  0x01        // bits
#define V_MALLOCED  0x02        // + 'my' VarDecl token, freed by ReleaseNameSpaceLayer
#define P_MALLOCED  0x04        // +

typedef struct token {
    uint8_t     type;           // OPERATOR | CONSTANT | SYMBOL | VALUE
    uint8_t     memtype;        // memory type
    uint8_t     subtype;        // NUMBER | STRING | SCALAR | ARRAY | SUBROUTINE
    uint8_t     datatype;       // INTEGER, FLOAT, POINTER,
    int32_t     pc_index;       // index of this token in p_code[]
    char        *pSymbol;
    union {
        long i;
        double d;
        void *p;
    } data;
} token;

typedef void ( *func_addr) ( array *parmlist, token *retval);


/**
 * @name    typedef struct jump_stat
 * @brief
 */
typedef struct jump_stat {
    bool    iteration_stat;
    bool    continue_stat;
    bool    break_stat;
    bool    return_stat;
    token   ret_token;
} jump_stat;

int  isalphanumeric( char c);
int  isRelOp( const char *op);
void InitToken( token *t);
token *newToken( void);
void RTErrorMsg( const char *pMsg);
void SetTypes( token *t_value, uint16_t type, uint8_t subtype, uint8_t datatype);


#endif // SLI_H
