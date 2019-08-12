/* ===================================================================
** Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: parse.c
** DESC: sensor language - slang - parser
** Revision History
** 11-09-2015       df      Fix static analysis errorss
** ================================================================ */
// sensor language
//
// dfarrell@DFARRELL /cygdrive/c/Projects/study
// $ gcc -Iinc src/parse.c -o parse.exe
/*
    BNF
    [ optional ]   optional elements enclosed in brackets
    ( group )      group of elements enclosed in prentheses
    'literal'      literal data enclosed in single quotes
    +              1 or more
    *              0 or more

    <program>      :: <body>
    <body>         :: ( <data-decl> | < proc_decl> | <null> )*

    <data-decl>    ::= 'my' var | '(' <var-list> ')' ';'
    <var-list>     ::= <var> (',' <var> )*
    <var>          ::= <symbol> [ '=' <number> | <string> ]

    <proc-decl>    :: 'sub' <symbol> '(' [ <parm_list> ] ')' '{' <statement> '}'
    <parm_list>    :: <parm> (',' <parm>)*
    <parm>         :: <symbol> | <number> | <string>

    <proc_call>    :: <symbol> '(' [ <arg_list>) ] ')'
    <arg_list>     :: <arg> (',' <arg> | <arg_sub_list>)*
    <arg>          :: <expression>
    <arg_sub_list> :: '{' <sub_arg> | ( ',' <sub_arg>)* '}'
    <sub_arg>      :: <symbol> [ ('=>' | '.') <expression> ]*


    <block>        :: '{' (<statement>)* '}'
    <statement>    :: <block> | <expression> |
                      <if> | <while> | <data-decl> |
                      <return> | <continue> | <break> | <null> |
                      '(' <sym_list> ')' '=' <expression>

    <if>           :: 'if' '(' <condition> ')' <statement> [ 'else' <statement> ]
    <while>        :: 'while' '(' <condition> ')' <statement>
    <continue>     :: 'continue' ';'
    <break>        :: 'break' ';'
    <return>       :: 'return' <expression>* ';'

    <sym>          :: <symbol> | <bareword>
    <sym_list>     :: <sym> (',' <sym>)*

    <condition>    :: (<expression> [ <relop> < expression>] )*
    <assignment>   ::= <symbol> <assignop> <expression>

    <not_factor>   ::= [!] <factor>
    <factor>       ::= <variable> | <constant> | <proc_call> | '(' <expression> ')'
    <signed_term>  ::= [<addop>] <term>
    <term>         ::= <not_factor> (<mulop> <not_factor>)*
    <numeric_expr> ::= <signed_term> (<addop> <term>)*

    <expression>   ::= <numeric_expr>
                       [ (<relop> <term>) | (<assignop> <expression>) ]
                       [ <logicop> <expression> ]

    <addop>        ::= '+' | '-' | '&' | '|' | '^'          // caret is bitwise xor
    <mulop>        ::= '*' | '/' |
    <relop>        ::= '==' | '<>' |  '!=' | '<' | '>' | '<=' | '>=' |
    <logicop>      ::= '&&' | '||'
    <assignop>     ::= '=' | '+=' | '-=' | '*=' | '/=' | '|=' | '&=' | '^=' | '<<=' | '>>='

    <variable>     :: = <scalar> | <array>
    <scalar>       ::= '$' <symbol>
    <array>        ::= '@' <symbol>

    <symbol>       ::= <letter> [ <letter> | <digit> | '_']*
    <number>       ::= [<digit>]+ [ '.' <digit>* ]

*/

#include "sensor1.h"
#include "sns_common_v01.h"
#include "sns_smgr_api_v01.h"
#include "sli.h"
#include "alu.h"
#include "array.h"
#include "builtin.h"
#include "literals.h"
#include "namespace.h"
#include "slmalloc.h"
#include "slcore.h"
#include "slnew_req.h"

#include <ctype.h>      // isalpha() ...
#include <stdio.h>
#include <stdlib.h>

/*
** forward declarations
*/
void  ArrayAssignment( array *parmlist, token *rhs);
void  IfStatement( jump_stat *jumps, token *if_stmt);
void  WhileStatement( jump_stat *jumps, token *while_stmt);
void  ForStatement( jump_stat *jumps, token *for_stmt);
void  ProcDecl( array *parmlist, token *retval);
void  ProcCall( token *proc_token, token *retval);

const   char *pcurrent_subroutine = NULL;

typedef struct parse_info {
    char    *pLine;                 // start of line
    int     line_num;               // 0 relative
    int     column_num;             // +
} parse_info;

typedef struct loc_map {
    char    *pLine;                 // addresss of this line in the source code
    int32_t line_num;               // source code line number - 0 relative
    int32_t column_num;             // source code column number - 0 relative
} loc_map;

typedef struct memory_map {
    const   char *script_name;
    char    *pSource;               // pointer to source code
    array   *ip_loc;                // array of loc_maps. index is pc_ip
} memory_map;

/**
 * global variables
 */

bool        debugging_spin = false;
bool        debugging = false;
int         runtime = false;        // e.g. compile_time
char        *pCursor;               // pointer into script. null terminated string.
token       *tlook;                 // lookahead token

array       *p_code;                // pointer to array of pointers to tokens.
int32_t     pc_ip;                  // p_code instruction pointer: p_code->element[ pc_index]

parse_info  parseinfo;
memory_map  mem_map     = { NULL, NULL, NULL};

char *lit_semicolon = ";";
char *lit_comma     = ",";
char *lit_lparen    = "(";
char *lit_rparen    = ")";
char *lit_lbrace    = "{";
char *lit_rbrace    = "}";

#define MAX_ERR_MSG_LEN 128
char errbuf[ MAX_ERR_MSG_LEN];

void InitMemoryMap( void)
{
    mem_map.ip_loc = new_array( 128);
}

void InitToken( token *rhs)
{
    rhs->type = UNDEFINED;
    rhs->memtype = UNDEFINED;
    rhs->subtype = UNDEFINED;
    rhs->datatype = UNDEFINED;
    rhs->pc_index = 0;
    rhs->pSymbol = NULL;
    rhs->data.p = NULL;
}

void InitJumps( jump_stat *jumps)
{
    jumps->continue_stat = false;
    jumps->break_stat = false;
    jumps->return_stat = false;
    jumps->iteration_stat  = false;
    InitToken( &jumps->ret_token);
}

void RTErrorMsg ( const char *msg)
{
    // source code location for current token p-code ip
    loc_map *pSrcLoc = get_array_element( mem_map.ip_loc, tlook->pc_index);
    if ( pSrcLoc == NULL) {
        fprintf( stderr, "FatalError can\'t locate pSrcLoc for pc_ip %d\n", pc_ip);
        fprintf( stderr, "%s\n", msg);
        exit( 4);
    }

    if ( pcurrent_subroutine) {
        fprintf( stderr, "module %s: In Function \'%s\'\n", mem_map.script_name, pcurrent_subroutine);
    }

    int32_t line_num = pSrcLoc->line_num;                       // 0 relative
    int16_t column_num = pSrcLoc->column_num;                   // +

    fprintf( stderr, "module %s:%d:%d %s\n", mem_map.script_name, line_num + 1, column_num + 1, msg);

    column_num     = ( column_num < (int16_t)sizeof( errbuf) - 4) ? column_num : (int16_t)sizeof( errbuf) - 4;

    const char *psourceLine = pSrcLoc->pLine;

    int nl_offset = 0;
    while ( psourceLine[ nl_offset] && ( psourceLine[ nl_offset] != '\n')) {
        nl_offset++;
    }
    snprintf( errbuf, nl_offset + 1, "%s", psourceLine);       // show offending source line
    fprintf( stderr, "%s\n", errbuf);
    fprintf( stderr, "\n");

    // point at offending symbol ala gcc ^ at line_offset
    memset( errbuf, ' ', sizeof( errbuf));
    errbuf[ column_num++] = '^';
    errbuf[ column_num++] = '\n';
    errbuf[ column_num++] = '\0';
    fprintf( stderr, "%s\n", errbuf);
}

void Expected ( char *msg)
{
    char buffer[ 64] = { '\0' };
    char before[ 32] = { '\0' };

    if ( tlook->pSymbol != NULL) {
        snprintf( before, sizeof( before), "before \'%s\'", tlook->pSymbol);
    }
    snprintf( buffer, sizeof( buffer), "expected \'%s\' %s", msg, before);
    FatalError ( buffer) ;
}

/* -----------------------------------------------------------------------------------------
** write_once chunk of memory is intended to hold null terminated strings and numbers copied
** from the source code scripts. Everthing stored in this chunk of  ** memory is "const".
* ------------------------------------------------------------------------------------------*/
#define CHUNK_SIZE 2048

typedef struct write_once {
    int     chunk_size;
    char    *pFirst;
    char    *pNext;
    char    *pLast;
} write_once;

write_once storage;

char *MallocWriteOnly( int length)
{
    char    *pAppend = NULL;

    if ( length <= 0) {
        FatalError( "MallocWriteOnly usage error");
    }
    if ( storage.chunk_size == 0) {
        char *pChunk = (char *)slmalloc( CHUNK_SIZE, "parse.1");
        if ( pChunk == NULL) {
            FatalError( "out of memory");
        }
        storage.chunk_size = CHUNK_SIZE;
        storage.pFirst     = pChunk;
        storage.pNext      = pChunk;
        storage.pLast      = pChunk + CHUNK_SIZE;
    }

    if ( length <= storage.pLast - storage.pNext) {
        pAppend  = storage.pNext;
        storage.pNext += length;
    }
    else {
        storage.chunk_size = 0;
        pAppend = MallocWriteOnly( length);
    }

    return  pAppend;
}

/* ------------------------------------------------------
** wish list:
** backslash '\' as the "address of" operator
** hash variables and accessors, e.g. '->' or '{' <key> }'
** array element accessors, e.g. '[' <index> ']'
** exists each keys values
** et. al. builtin functions: int, sizeof, strlen
** ------------------------------------------- */
const char *keywords[] = { "my", "sub", "if", "else", "for", "while", "next", "last", "defined", "return", "scalar", "exit"};

/**
 * @name        LatchTokenLiteral
 * @param[i]    token for this Literal
 * @param[i]    address of this number/text n the source code
 * @param[i]    number of text characters
 * @retval      pointer to null terminated text
 */
const char *LatchTokenLiteral( token* t, const char *pString, size_t length)
{
    if ( length) {
        char *pStorage = (char *)MallocWriteOnly( length + 1);  // space for trailing \0
        t->pSymbol = pStorage;
        strlcpy( pStorage, pString, length);
        pStorage[ length] = '\0';
        return pStorage;            /* pointer to null terminated string */
    }
    return NULL;
}

// /* ---------
// ** iskeyword
// ** returns 0 - pSymbol is not a keyword
// **         1 - pSymbol is a keyword, and *index get's
// ** ------------------------------------------------- */
// int iskeyword( char *pSymbol, int *index)
// {
//     int i;
//
//     for ( i = 0; i < sizeof(keywords) / sizeof(char *); i++ ) {
//         if ( strcmp( keywords[ i], pSymbol) == 0) {
//             *index = i;
//             return TRUE;
//         }
//     }
//     return FALSE;
// }

int isoperator( char c)
{
    if ( c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '=' ||
         c == '>' || c == '<' || c == '!' || c == '&' || c == '|' || c == '.') {
        return TRUE;
    }
    return FALSE;
}

int isalphanumeric( char c) {
    return ( isalpha( c) || isdigit(c) || c == '_');
}

// ------------------------------
// get next source code characher
// ------------------------------
char peekChar( void)
{
    return pCursor[ 0];
}

void eatChar( void)
{
    pCursor++;
    parseinfo.column_num++;
}

char skipWhiteSpace( char c)
{
    while ( isspace( c)) {
        if ( c == '\n') {
            parseinfo.pLine = pCursor + 1;
            parseinfo.line_num++;
            parseinfo.column_num = -1;      // eatChar increments
        }
        eatChar();                          // make this an inline macro
        c = peekChar();
    }
    return c;
}

/**
 * @name    skipSlashStarCommment
 * @brief   skip C-style comments
 */
void skipSlashStarCommment( void)
{
    char c;
    while (( c = peekChar()) != '\0') {
        if ( c == ISEOF) {
            break;
        }
        else {
            c = skipWhiteSpace( c);                 // need this to manage line and column numbers
        }

        if ( c == '/' && pCursor[ 1] == '*') {    // handle nested comments
            eatChar();
            eatChar();
            skipSlashStarCommment();
            continue;
        }
        else if ( c == '*' && pCursor[ 1] == '/') {      // end of comment
            eatChar();
            eatChar();
            break;
        }
        else {
            eatChar();
        }
    }
}

/**
 * @name skipComments
 * @param[ i]   t->pSymbol -> slash asterick or slash slash
 */
void skipComments( token *t)
{
    if ( strcmp( t->pSymbol, "//") == 0) {          // discard comment thru end of line
        int line_no = parseinfo.line_num;
        while ( line_no == parseinfo.line_num) {    // go til line number changes
            char c = peekChar();
            if ( c == ISEOF) {
                break;
            }
            else if ( !isspace( c)) {
                eatChar();                          // need this to manage column numbers
            }
            else {
                c = skipWhiteSpace( c);             // need this to manage line and column numbers
            }
        }
    }
    else if ( strcmp( t->pSymbol, "/*") == 0) {     // C-style comments
        skipSlashStarCommment();
    }
}

/* ------------------------------------------------------
** cursor positioned at character after the leading quote
** string :: '"' <character>* '"'
** input:   pLength - address of length field
** output:  length  - gets number of characters in the string
** ---------------------------------------------------------- */
void getStringToken( size_t *pLength)
{
    char c;
    *pLength = 0;

    int column_num = parseinfo.column_num;

    while (( c = peekChar()) != '\0') {
        eatChar();
        if ( c == '\\') {                   // if backslash, i.e. escape
            eatChar();                      // then eat the following character too
            *pLength += 2 ;                 // +
            continue;
        }
        if ( c == '"') {                    /* does not support embedded quote */
            break;
        }
        *pLength += 1 ;
    }

    if ( c == '\0') {
        parseinfo.column_num = column_num;
        FatalError( "runaway string. missing trailing quote");
    }
}

/* --------------------------------
** cursor positioned at first digit
** <number ::= [<digit>]+ [ '.' [<digit>]+ ]
** return: INTEGER, or FLOAT
** ----------------------------------------------- */
uint8_t getNumberToken( size_t *pLength)
{
    char c;

    uint8_t datatype = INTEGER;

    eatChar();
    *pLength = 1;

    while (( c = peekChar()) != '\0') {
        if ( !isdigit( c)) {
            break;
        }
        eatChar();
        *pLength += 1 ;
    }

    if ( c == '.') {
        eatChar();
        *pLength += 1 ;
        datatype   = FLOAT;
        while (( c = peekChar()) != '\0') {
            if ( !isdigit( c)) {
                break;
            }
            eatChar();
            *pLength += 1 ;
        }
    }

    return datatype;
}

/* -----------------------------------------------------------
** cursor positioned at first letter of symbol
** <ident> ::= <letter> [ <letter> | <digit> | <underscore> ]*
** ============-------------------------------------------- */
void getSymbolToken( size_t *pLength)
{
    char c;

    eatChar();
    *pLength = 1;

    while (( c = peekChar()) != '\0') {
        if ( !isalphanumeric( c) ) {
            break;
        }
        eatChar();
        *pLength += 1 ;
    }
}

/* ------------------------------------------
** cursor positioned at first operator symbol
** --------------------------------------- */
void getOperatorToken( size_t *pLength)
{
    char c;

    eatChar();
    *pLength = 1;

    while (( c = peekChar()) != '\0') {
        if ( !isoperator( c)) {
            break;
        }
        eatChar();
        *pLength += 1 ;
    }
}

/**
 * @name newToken
 * @return token*
 */
token *newToken( void)
{
    token *t = ( token *)slmalloc( sizeof( token), "newToken");
    if ( t == NULL) {
        FatalError( "out of memory");
    }
    t->type = UNDEFINED;
    t->memtype = T_MALLOCED;
    t->subtype = UNDEFINED;
    t->datatype = UNDEFINED;
    t->pc_index = 0;
    t->pSymbol = NULL;
    t->data.p  = NULL;
    return t;
}

token *getNextToken( loc_map *src_loc)
{
    size_t  length;

    token *t = newToken();

    char c = peekChar();

    if ( isspace( c)) {
        c = skipWhiteSpace( c);
    }

    src_loc->pLine       = parseinfo.pLine;
    src_loc->line_num    = parseinfo.line_num;
    src_loc->column_num  = parseinfo.column_num;

    if ( c == ';') {
        t->type = SEMICOLON;
        t->pSymbol = lit_semicolon;
        eatChar();
    }
    else if ( c == ',') {
        t->type = COMMA;
        t->pSymbol = lit_comma;
        eatChar();
    }
    else if ( c == '(') {
        t->type = LPAREN;
        t->pSymbol = lit_lparen;
        eatChar();
    }
    else if ( c == ')') {
        t->type = RPAREN;
        t->pSymbol = lit_rparen;
        eatChar();
    }
    else if ( c == '{') {
        t->type = LBRACE;
        t->pSymbol = lit_lbrace;
        eatChar();
    }
    else if ( c == '}') {
        t->type = RBRACE;
        t->pSymbol = lit_rbrace;
        eatChar();
    }
    else if ( c == '"') {       // STRING
        t->type = CONSTANT;
        t->subtype = STRING;
        eatChar();
        const char *pString = pCursor;
        getStringToken( &length);                   // eats up to and including trailing quote
        t->data.p = (void *)LatchTokenLiteral( t, pString, length);
        t->datatype = POINTER;
    }
    else if ( isdigit( c)) {    // NUMBER
        t->type    = CONSTANT;
        t->subtype = NUMBER;
        const char *pString = pCursor;              // address of first digit in number
        t->datatype = getNumberToken( &length);     // INTEGER or FLOAT
        LatchTokenLiteral( t, pString, length);
        if ( t->datatype == INTEGER) {
            t->data.i = atol( t->pSymbol);
        }
        else if ( t->datatype == FLOAT) {
            t->data.d = atof( t->pSymbol);
        }
        else {
            fprintf( stderr, "getNextToken bad NUMBER datatype\n");
        }
    }
    else if ( c == '$') {       // SCALAR variable
        t->type    = SYMBOL;
        t->subtype = SCALAR;
        const char *pString = pCursor;              // address of $symbol
        getSymbolToken( &length);
        LatchTokenLiteral( t, pString, length);
    }
    else if ( c == '@') {       // ARRAY variable
        t->type    = SYMBOL;
        t->subtype = ARRAY;
        const char *pString = pCursor;              // address of @symbol
        getSymbolToken( &length);
        LatchTokenLiteral( t, pString, length);
    }
    else if ( isalpha( c)) {    // SYMBOL
        t->type    = SYMBOL;
        t->subtype = BAREWORD;
        const char *pString = pCursor;              // address of first character in symbol
        getSymbolToken( &length);
        LatchTokenLiteral( t, pString, length);

//        int keyword_index;
//        if ( iskeyword( t->pSymbol, &keyword_index)) {
//            t->subtype = KEYWORD;                 // not used
//        }
    }
    else if ( isoperator( c)) {
        t->type    = OPERATOR;

        const char *pString = pCursor;              // address of first character of operator
        getOperatorToken( &length);
        LatchTokenLiteral( t, pString, length);

        if ( strcmp( t->pSymbol, "/*") == 0       // skip comments
             || strcmp( t->pSymbol, "//") == 0) {
            skipComments( t);
            slfree( t, "parse.f1");
            return getNextToken( src_loc);
        }

    }
    else if ( c == '\0') {
        t->type = ISEOF;
    }
    else {
        char buffer[ 40];
        snprintf( buffer, sizeof(buffer), "token ***%c*** 0x%x not recognized", c, c);
        FatalError( buffer);
    }

    return t;
}

// # #### ## ######## ##### #######
// # #### ## ######## ##### #######
// # #### ## ######## ##### #######

/* ---------------
** top of compiler
** --------------- */

/**
 * @name        AppendPCode
 * @brief       append input token to the p_code array
 *              setting token->pc_index to its location in the
 *              array
 * @param[i]    token *t
 */
void AppendPCode( token *t, loc_map *src_loc)
{
    if ( p_code == NULL) {
        p_code = new_array( 128);                   // initially provide for 128 token
        if ( p_code == NULL) {
            FatalError( "out of memory");
        }
    }

    p_code = append_array_element( p_code, t);

    t->pc_index = p_code->high_water;               // set token->pc_index to its p_code[ index]

    // append location of token in source code to the memory map

    loc_map *pSrcLoc = slmalloc( sizeof(loc_map), "parse.3");
    if ( pSrcLoc == NULL) {
        FatalError( "out of memory");
    }
    memcpy( pSrcLoc, src_loc, sizeof( loc_map));
    mem_map.ip_loc = append_array_element( mem_map.ip_loc, pSrcLoc);
}

void NextToken( void)
{
    if ( tlook == NULL || tlook->type == ISEOF) {
        FatalError( "runaway syntax error");
    }
    if ( !runtime) {                                // if compiling
        loc_map src_loc;                            // get location of token in source code
        tlook = getNextToken( &src_loc);
        AppendPCode( tlook, &src_loc);
    }
    else {                                           // run_time
        pc_ip++;                                     // p-code instruction pointer
        tlook = get_array_element( p_code, pc_ip);
    }
}

/**   google grep bnf for arithmetic expression
 * <not_factor> 	::= [!] <factor>
 * <factor>      	::= <variable> | <constant> | '(' <expression> ')'
 * <signed_term> 	::= [<addop>] <term>
 * <term>        	::= <not_factor> (<mulop> <not_factor>)*
 * <expression>  	::= <signed_term> (<addop> <term>)*

 * precedence   see also C language operator precedence
 * 1.	!
 * 2.	- (unary)
 * 3.	*, /, &
 * 4.	+, -, |, ~
 */


int IsAddOp( void)
{
    char *$op = tlook->pSymbol;
    if ( strcmp( $op, "+") == 0
         || strcmp( $op, "-") == 0 ) {
        return true;
    }
    return false;
}

int IsMulOp( void)
{
    char *$op = tlook->pSymbol;
    if ( strcmp( $op, "*") == 0
         || strcmp( $op, "/") == 0 ) {
        return true;
    }
    return false;
}

char *relational_operator[] = { "==", "!=", ">=", ">", "<=", "<",
};

int isRelOp( const char *op)
{
    unsigned int i;
    for ( i = 0; i < sizeof( relational_operator) / sizeof( char *); i++) {
        if ( strcmp( op, relational_operator[ i]) == 0) {
            return true;
        }
    }
    return false;
}

int IsRelOp( void)
{
    char *op = tlook->pSymbol;
    return isRelOp( op);
}

char *assignment_operator[] = { "=", "+=", "-=", "*=",
                               "/=", "%=", "&=", "|=",
                               "^=", "<<=", ">>",
};
int IsAssignOp( void)
{
    char *op = tlook->pSymbol;
    unsigned int i;
    for ( i = 0; i < sizeof( assignment_operator) / sizeof( char *); i++) {
        if ( strcmp( op, assignment_operator[ i]) == 0) {
            return true;
        }
    }
    return false;
}

void MatchOp( char *target)
{
    if ( strcmp( tlook->pSymbol, target) != 0) {
        Expected( target);
    }
    NextToken();
}

char *TypeToLit( uint16_t type)
{
    char    *lit = "unknown type";
    if ( type == SEMICOLON) {
        lit = lit_semicolon;
    }
    else if ( type == LPAREN) {
        lit = lit_lparen;
    }
    else if ( type == RPAREN) {
        lit = lit_rparen;
    }
    else if ( type == LBRACE) {
        lit = lit_lbrace;
    }
    else if ( type == RBRACE) {
        lit = lit_rbrace;
    }
    else if ( type == COMMA) {
        lit = lit_comma;
    }

    return lit;
}

void MatchType( uint16_t type)
{
    if ( tlook->type != type) {
        char *lit_type = TypeToLit( type);
        Expected( lit_type);
    }
    NextToken();
}

void  Factor( token *retval);            /** forward declaration */
void  Term( token *retval);
void  Expression( token *retval);
void  FreeTokenParmList( array *parmlist);

void Factor( token *retval) {
    // stub TODO array
    if ( tlook->type == SYMBOL && tlook->subtype == SCALAR) {       // variable - lookup its svalue
        if ( runtime) {
            token *t_real = AllLayerLookup( tlook);
            if ( t_real == NULL) {
                char errBuf[ 48];
                snprintf( errBuf, sizeof( errBuf), "Factor - unknown symbol %s", tlook->pSymbol);
                FatalError( errBuf);
            }
            else {
                retval->type = t_real->type;
                retval->subtype = t_real->subtype;
                retval->datatype = t_real->datatype;
                retval->pc_index = UNDEFINED;
                retval->pSymbol = t_real->pSymbol;
                retval->data.p = t_real->data.p;
            }
        }
        NextToken();
    }
    else if ( tlook->type == CONSTANT) {                            // constant - number or string
        retval->type = tlook->type;
        retval->subtype = tlook->subtype;
        retval->datatype = tlook->datatype;
        retval->pc_index = UNDEFINED;
        retval->pSymbol = NULL;
        retval->data.p = tlook->data.p;
        NextToken();
    }
    else if ( tlook->type == SYMBOL &&
              ( tlook->subtype == BUILTIN
                || tlook->subtype == SUBROUTINE
                || tlook->subtype == BAREWORD )) {
        token *callers_token = tlook;
        NextToken();
        ProcCall( callers_token, retval);                 // call subroutine
    }
    else if ( tlook->type == LPAREN) {
        NextToken();                                        // probably  '(' expr && expr || expr ... ')'
        Expression( retval);
        if ( runtime == false || tlook->type == RPAREN) {   // at compile time must end in ')'
            MatchType( RPAREN);                             // at runtime maybe '&&' but done: already false
        }
    }
}

void NotFactor( token *retval)
{
    char *not = NULL;
    if ( strcmp( tlook->pSymbol, "!") == 0) {
        not = tlook->pSymbol;
        NextToken();
    }
    Factor( retval);
    if ( not) {
        Alu( retval, not, NULL);    // handle ! not
    }
}

void Term( token *retval)
{
    NotFactor( retval);
    while( IsMulOp()) {
        char *op = tlook->pSymbol;
        NextToken();

        token rhs;     InitToken( &rhs);
        NotFactor( &rhs);
        Alu( retval, op, &rhs);     // perform: not_factor op notfactor
    }
}

void SignedTerm( token *retval)
{
    char *sign = NULL;
    if ( IsAddOp()) {
        sign = tlook->pSymbol;
        NextToken();
    }
    Term( retval);
    if ( sign && strcmp( sign, "-") == 0) {
        Alu( retval, sign, NULL);      // TODO handle unary minus for sign == '-'
    }
}

void Expression( token *retval)
{
    SignedTerm( retval);
    while( IsAddOp()) {
        char *op = tlook->pSymbol;
        NextToken();
        token rhs;     InitToken( &rhs);
        Term( &rhs);
        Alu( retval, op, &rhs);     // perform: signed_term op term
    }

    if ( IsRelOp()) {
        char *op = tlook->pSymbol;
        NextToken();
        token rhs;     InitToken( &rhs);
        Term( &rhs);
        Alu( retval, op, &rhs);     // perform comparison
    }
    else if ( IsAssignOp()) {
        char *op = tlook->pSymbol;
        token *lhs = NULL;
        if ( retval->subtype == SCALAR || retval->subtype == ARRAY) {
            lhs = AllLayerLookup( retval);
            if ( lhs == NULL) {
                char    buffer[64];
                snprintf( buffer, sizeof(buffer), "%s symbol not found", retval->pSymbol);
                FatalError( buffer);
            }
        }
        NextToken();
        token rhs;     InitToken( &rhs);
        Expression( &rhs);

        if ( runtime) {
            if ( lhs) { 
                Alu( lhs, op, &rhs);                // perform assignments like: $j += 1;
                                                    // the following 2 lines probably don't matter at all
                retval->datatype = lhs->datatype;   // because lhs is the real auto-variable token
                retval->data.p = lhs->data.p;       // retval is object code's pSymbol $j
            }
        }
    }

    if ( !runtime) {                            // compile time?
        if ( tlook->type == OPERATOR
             && ( strcmp( tlook->pSymbol, "&&") == 0 || strcmp( tlook->pSymbol, "||") == 0 )) {
            NextToken();
            Expression( retval);                // continue evaluation
        }
        return;
    }
    /*
    ** <expression> '&&' <expression>
    ** --------------------------- */
    if ( tlook->type == OPERATOR && strcmp( tlook->pSymbol, "&&") == 0) {
        NextToken();
        if ( retval->datatype == INTEGER && retval->data.i == true) {
            Expression( retval);            // continue evaluation
        }
        else if ( debugging) {
            printf( "\"&&\" but done: already false\n");                    // debug
        }
    }
    /*
    ** <expression> '||' <expression>
    ** --------------------------- */
    else if ( tlook->type == OPERATOR && strcmp( tlook->pSymbol, "||") == 0) {
        NextToken();
        if ( retval->datatype == INTEGER && retval->data.i == true) {
            if ( debugging) {
                printf( "\"||\" done early: already true\n");                   // debug
            }
        }
        else {
            Expression( retval);            // continue evaluation
        }
    }
}

/**
 * @name Arg    Individual Procedure Calls Arguments
 * @param[i]    NULL or pointer to array of arg tokens
 * @returns     NULL or pointer to array of arg tokens
 */
array *Arg( array *parmlist)
{
    token expr;     InitToken( &expr);
    Expression( &expr);

    if ( expr.type == SYMBOL || expr.type == CONSTANT) {
        if ( runtime) {
            if ( parmlist == NULL) {
                parmlist = new_array( 1);
            }
            token *arg    = newToken();
            arg->type     = expr.type;
            arg->memtype  = T_MALLOCED;
            arg->subtype  = expr.subtype;
            arg->datatype = expr.datatype;
            arg->pSymbol  = expr.pSymbol;
            arg->data.p   = expr.data.p;

            parmlist = append_array_element( parmlist, arg);
        }
    }
    else if ( expr.type != UNDEFINED) {
        Expected( "Argument");
    }

      return parmlist;
}

// tlook points to ProcCall's( ..., { "name" => value, and/or "name", ....
// get 'name' token, get '=>' token, evaluate value expression,
// init new parmlist token as type = KEYVALUE
//                            pSymbol -> "name'
//                            subtype = KEYVALUE
//                            datatype/data set from expression.

array *KeyValueArg( array *sublist)
{
    if ( !( tlook->type == CONSTANT || tlook->type == SYMBOL)) {    // if not ( number | string | symbol)
        Expected( "key or field_name argument");
    }

    token *arg = NULL;

    if ( runtime) {
        arg = newToken();
        arg->type     = tlook->type;
        arg->memtype  = T_MALLOCED;
        arg->subtype  = tlook->subtype;         // stub if VARIABLE, need to substitute value for the $symbol ?
        arg->datatype = tlook->datatype;
        arg->pc_index = UNDEFINED;
        arg->pSymbol  = tlook->pSymbol;
        arg->data.p   = tlook->data.p;
    }
    NextToken();

    if (( tlook->type == OPERATOR)
          && ( strcmp( tlook->pSymbol, "=>") == 0 || strcmp( tlook->pSymbol, ".") == 0 ))  {
        NextToken();

        token value;     InitToken( &value);
        Expression( &value);

        if ( runtime) {
            arg->subtype  = KEYVALUE;           // pSymbol -> key portion
            arg->datatype = value.datatype;
            arg->data.p   = value.data.p;
//            printf("KeyValueArg arg %p\n", arg);    // debugging
        }
    }

    if ( arg != NULL) {
        if ( sublist == NULL) {
            sublist = new_array( 2);
//            printf("KeyValueArg new_array sublist %p\n", sublist);    // debugging
        }
        sublist = append_array_element( sublist, arg);
    }

    return sublist;
}

/**
 * @name    SubArgList
 * @brief   parse embedded '{' array element, ... '}'
 * @param   parmlist[ io] - gets new element of
 *                  token->sub_array_parmlist
 */
array *SubArgList( array *parmlist)
{
    array *sublist = NULL;

    while ( tlook->type != RBRACE) {
        sublist = KeyValueArg( sublist);
        if ( tlook->type == COMMA) {
            NextToken();                        // eat COMMA
            continue;
        }
        if ( tlook->type == RBRACE) {           // ok to have '=>'
            break;
        }
        MatchType( COMMA);
    }

    if ( runtime) {
        if ( parmlist == NULL) {
            parmlist = new_array( 2);
        }
        token *arg    = newToken();
        arg->type     = SUBLIST;
        arg->memtype  = T_MALLOCED;
        arg->subtype  = ARRAY;
        arg->datatype = POINTER;
        arg->data.p   = sublist;

        parmlist = append_array_element( parmlist, arg);
    }

    return parmlist;
}

/**
 * @name ArgList    Procedure Call's Argument List
 * @param[i]        NULL or pointer to array of arg tokens
 * @returns         NULL or pointer to array of arg tokens
 */
array *ArgList( array *parmlist)
{
    while( tlook->type != RPAREN) {
        if ( tlook->type == LBRACE) {
            NextToken();
            parmlist = SubArgList( parmlist);   // parse embedded { array_elements, ... }
            MatchType( RBRACE);
        }
        else {
            parmlist = Arg( parmlist);
        }
        if ( tlook->type == COMMA) {
            NextToken();
        }
        else {                                  // 06-09-2015
            break;                              // +
        }                                       // +
    }
    return parmlist;
}

/**
 * @name    ProcCall    '(' [ <arg-list>] ')'
 *
 * @param[i]    proc_tok subroutine name token
 */
void ProcCall( token *callers_token, token *retval)
{
    array *parmlist = NULL;
    void  *function_addr;

    if ( tlook->type != LPAREN) {                               //  MatchType( LPAREN);
        Expected( lit_lparen);
    }
    NextToken();                                                // eat LPAREN
    parmlist = ArgList( parmlist);
    MatchType( RPAREN);                                         // eat RPAREN

    if ( runtime){
        /*
        ** during first runtime call, resolve the caller's token
        ** as user supplied or builtin subroutine reference. call it.
        ** ------------------------------------------------------- */
        int32_t resume_ip     = pc_ip;                          // save p-code return address
        token   *resume_token = tlook;                          // e.g. token after ( parmlist)
        if ( callers_token->subtype == BAREWORD) {
            token *proc_token = AllLayerLookup( callers_token); // caller defined sub ?
            if ( proc_token) {
                callers_token->data.p  = proc_token;
                callers_token->subtype = SUBROUTINE;
            }
            else if (( function_addr = IsBuiltInFunction( callers_token->pSymbol)) != NULL  ) {
                callers_token->data.p  = function_addr;
                callers_token->subtype = BUILTIN;
            }
            else {
                char buffer[ 64] = { '\0' };
                snprintf( buffer, sizeof( buffer), "%s function not found", callers_token->pSymbol);
                FatalError ( buffer) ;
            }
        }
        if ( callers_token->subtype == BUILTIN) {
            func_addr addr = (func_addr)callers_token->data.p;
            (addr)( parmlist, retval);                          // call builtin function by reference
        }
        else if ( callers_token->subtype == SUBROUTINE) {
            tlook    = callers_token->data.p;
            pc_ip    = tlook->pc_index;                         // set p-code instruction pointer
            ProcDecl( parmlist, retval);                        // call (*target)( parmlist, &retval);
        }
        else {
            FatalError ( "ProcCall: should not occur");
        }

        tlook = resume_token;
        pc_ip = resume_ip;                                      // pop p-code resume address

        FreeTokenParmList( parmlist);                           // 06-30-2015
    }
}

void FreeTokenParmList( array *pArray)
{
    if ( pArray) {
        int i = pArray->high_water;
        for ( ; i >= 0; i--) {
            token *parm = get_array_element( pArray, i);
            if ( parm == NULL) {                                // 07-10-2105
                continue;                                       // + parm moved into namespace
            }                                                   // +
            if ( parm->type == SUBLIST) {
                FreeTokenParmList( parm->data.p);               // free parmlist's { fieldlist ...}
            }
            if ( !( parm->memtype & T_MALLOCED)) {
                fprintf( stderr, "FreeTokenParmList but not T_MALLOCED\n");
            }
//            if ( debugging) {
//                printf( "ftpl %s token %p\n", parm->pSymbol, parm);
//            }
            slfree( parm, "parm_token.f2");                     // free this token
            pArray->element[ i] = NULL;
        }
        slfree( pArray, "parm_list.f3");
    }
}

/**
 * @name Var    var [= <expression>]
 * @param[io]   address of pointer to parmlist array
 */
void VarDecl( array **pparmlist)
{
    if ( tlook->type == SYMBOL) {
        token *pExisting = ThisLayerLookup( tlook);
        if ( pExisting
             && pExisting->pc_index != tlook->pc_index) {
            char    buffer[ 64];
            snprintf( buffer, sizeof( buffer), "symbol %s already defined", tlook->pSymbol);
            FatalError( buffer);
        }

        token *lhs = pExisting;
        if ( pExisting == NULL) {
            /*
            ** make this token a local variable under recursion
            ** --------------------------------------------- */
            token *local = newToken();
            local->type = tlook->type;
            local->subtype = tlook->subtype;
            local->datatype = tlook->datatype;
            local->memtype = T_MALLOCED | V_MALLOCED;   // token subject to garbage collection
            local->pSymbol = tlook->pSymbol;
            local->pc_index = tlook->pc_index;
            local->data.i = tlook->data.i;
            AppendNewSymbol( local);
            lhs = local;
        }

        *pparmlist = append_array_element( *pparmlist, lhs);

        NextToken();
        if ( strcmp( tlook->pSymbol, "=") == 0) {
            char *op = tlook->pSymbol;
            NextToken();                            // my $var = < value>

            token value;     InitToken( &value);
            Expression( &value);

            Alu( lhs, op, &value);                  // Alu sets lhs datatype and data.i/f/p
        }
    }
}

/**
 * @name VarList   <var> | ( ',' <var>)*
 * @param[io]   address of pointer to parmlist array
 */
void VarList( array **pparmlist)
{
    while ( tlook->type != RPAREN) {
        VarDecl( pparmlist);
        if ( tlook->type != COMMA) {
            break;
        }
        NextToken();
    }
}

/**
 * @name DataDecl   my var | '(' <var-list>+ ')'
 *
 * @author dfarrell (4/21/2015)
 */
void DataDecl( void)
{
    array *parmlist = new_array( 2);        // array with 1 element
    if ( parmlist == NULL) {
        FatalError( get_literal( OOM));     // out of memory
    }

    if ( tlook->type == SYMBOL) {
        VarDecl( &parmlist);
    }
    else if ( tlook->type != LPAREN) {
        Expected( "symbol or '('");
    }
    else {
        NextToken();                        // eat '('

        VarList( &parmlist);
        MatchType( RPAREN);

        if ( tlook->type == OPERATOR && strcmp( tlook->pSymbol, "=") == 0) {
            NextToken();                    // eat '='

            token rhs;  InitToken( &rhs);

            if ( tlook->type == SYMBOL && tlook->subtype == ARRAY) {
                rhs.type = tlook->type;
                rhs.subtype = tlook->subtype;
                rhs.datatype = tlook->datatype;
                rhs.pc_index = tlook->pc_index;
                rhs.pSymbol = tlook->pSymbol;
                rhs.data.p = tlook->data.p;
                NextToken();                // eat @symbol
            }
            else {
                Expression( &rhs);
            }

            ArrayAssignment( parmlist, &rhs);

            // == free rhs == rhs itself is autovariable
            // rhs.memtype & P_MALLOCED means rhs.data.p gets freed after
            // array of token p_value. each p_value->memtype & T_MALLOCED | P_MALLOCED
            if ( rhs.memtype & P_MALLOCED) {
                FreeTokenParmList( rhs.data.p);
            }
        }
    }
    if ( parmlist) {
        slfree( parmlist, "parse.f4");      // note this leaves 'local' token(s) malloced
                                            // RemoveNameSpaceLayer can free memtype = T_MALLOCED tokens
    }
}

void ArrayAssignment( array *parmlist, token *rhs)
{
    if ( !runtime || parmlist == NULL || rhs == NULL) {
        return;
    }
    if ( rhs->subtype != ARRAY) {                 // my ( $list, ...) = @array
        RTErrorMsg( get_literal( BAD_ASSIGNMENT));
        return;
    }
    if ( rhs->datatype == UNDEFINED || rhs->data.p == UNDEFINED) {
        token *t_real = AllLayerLookup( rhs);
        if ( t_real == NULL) {
            UninitializedVariable( rhs->pSymbol);
            return;
        }
        rhs = t_real;
    }

    int i;
    array *valuelist = (array *)rhs->data.p;
    for ( i = 0; i <= parmlist->high_water; i++) {
        token *lhs = get_array_element( parmlist, i);
        if ( lhs == NULL) {
            continue;                                   // should not occur
        }
        if ( lhs->pSymbol != NULL) {                    // name to location lookup...
            token *p_real = AllLayerLookup( lhs);
            if ( p_real != NULL) {
                lhs = p_real;
            }
        }

        token *value = get_array_element( valuelist, i);
        if ( value == NULL || value->type == UNDEFINED || value->datatype == UNDEFINED) {
            lhs->datatype = UNDEFINED;
            lhs->data.p   = NULL;
            continue;
        }
        if ( lhs->subtype == ARRAY && value->subtype == ARRAY) {
            lhs->data.p = rhs->data.p;
            continue;
        }
        if ( lhs->subtype == ARRAY || value->subtype == ARRAY) {
            FatalError( "DataDecl only scalar list elements supported\n");
        }
        Alu( lhs, "=", value);
    }
}

/**
 * @name ReturnStatement
 */
void ReturnStatement( jump_stat *jumps)
{
    token rhs;     InitToken( &rhs);

    if ( tlook->type == LPAREN) {
        NextToken();
        Expression( &rhs);
        MatchType( RPAREN);
    }
    else {
        Expression( &rhs);
    }

    memcpy( &jumps->ret_token, &rhs, sizeof(token));
    jumps->return_stat = true;
}


/**
 * @name SymElement
 * @param[io]   address of pointer to parmlist array
 */
void SymElement( array **pparmlist)
{
    if ( tlook->type == SYMBOL) {

        token *pExisting = AllLayerLookup( tlook);
        if ( pExisting == NULL) {
            char    buffer[ 64];
            snprintf( buffer, sizeof( buffer), "symbol %s not defined", tlook->pSymbol);
            FatalError( buffer);
        }

        if ( *pparmlist) {                          // parmlist allocated?
            token   *t_symbol = tlook;
            *pparmlist = append_array_element( *pparmlist, t_symbol);
        }
        NextToken();                                // eat symbol
    }
}

/**
 * @name Symlist   <var> | ( ',' <var>)*
 * @param[io]   address of pointer to parmlist array
 * @brief       statement is:   '(' <symbol | (',' <symbol)* ')'
 *              usually lhs of assignment statement.
 */
void SymList( array **pparmlist)
{
    while ( tlook->type != RPAREN) {

        SymElement( pparmlist);
        if ( tlook->type != COMMA) {
            break;
        }
        NextToken();                    // eat comma
    }
}

/**
 * @name    Statement ::  <assignment> | <proc-call> | <if> |
 *       <while> | <for> | <data_decl> | <continue> |
 *       <break> | <return> | null
 */
void Statement( jump_stat *jumps)
{
    if ( tlook->type == LBRACE) {               // '{' (statement)* '}'
        MatchType( LBRACE);
        AddNameSpaceLayer();

        while ( tlook->type != RBRACE) {
            Statement( jumps);
            if ( runtime) {
                if ( jumps->continue_stat) {
                    return;
                }
                if ( jumps->break_stat || jumps->return_stat) {
                    RemoveNameSpaceLayer();     // 07-14-2015    this is needed for return.
                                                // + but what about break ?
                    return;                    // recognizing continue/break/return
                }
            }
        }
        MatchType( RBRACE);
        RemoveNameSpaceLayer();
    }
    else if ( tlook->type == SYMBOL) {
        if ( strcmp( tlook->pSymbol, "if") == 0) {
            token *if_stmt = tlook;
            NextToken();
            IfStatement( jumps, if_stmt);
        }
        else if ( strcmp( tlook->pSymbol, "while") == 0) {
            token *while_stmt = tlook;
            NextToken();
            WhileStatement( jumps, while_stmt);
        }
        else if ( strcmp( tlook->pSymbol, "for") == 0) {
            token *for_stmt = tlook;
            NextToken();
            ForStatement( jumps, for_stmt);
        }
        else if ( strcmp( tlook->pSymbol, "my") == 0) {
            NextToken();
            DataDecl();
            MatchType( SEMICOLON);
        }
        else if ( strcmp( tlook->pSymbol, "return") == 0) {
            NextToken();
            ReturnStatement( jumps);
            MatchType( SEMICOLON);
        }
        else if ( strcmp( tlook->pSymbol, "continue") == 0) {
            NextToken();
            if ( runtime) {
                if ( jumps->iteration_stat == false) {
                    FatalError( "continue statement not within loop");
                }
                else {
                    jumps->continue_stat = true;
                }
            }
            MatchType( SEMICOLON);
        }
        else if ( strcmp( tlook->pSymbol, "break") == 0) {
            NextToken();
            if ( runtime) {
                if ( jumps->iteration_stat == false) {
                    FatalError( "break statement not within loop");
                }
                else {
                    jumps->break_stat = true;
                }
            }
            MatchType( SEMICOLON);
        }
        else {
            token retval;     InitToken( &retval);   // <symbol> [ = <expression>]* ';'
            Expression( &retval);
            MatchType( SEMICOLON);
        }
    }
    else if ( tlook->type == LPAREN) {               // '(' (symlist)* ')' [ = <expression> ] ';'
        MatchType( LPAREN);

        array *parmlist = new_array( 2);             // array with 1 element
        if ( parmlist == NULL) {
            FatalError( get_literal( OOM));          // out of memory
        }

        while ( tlook->type != RPAREN) {
            SymList( &parmlist);
        }
        MatchType( RPAREN);

        if ( tlook->type == OPERATOR && strcmp( tlook->pSymbol, "=") == 0) {
            NextToken();                            // ( <symlist> ) = < rhs>

            token rhs;     InitToken( &rhs);
            if ( tlook->type == SYMBOL && tlook->subtype == ARRAY) {
                rhs.type = tlook->type;
                rhs.subtype = tlook->subtype;
                rhs.datatype = tlook->datatype;
                rhs.pc_index = tlook->pc_index;
                rhs.pSymbol = tlook->pSymbol;
                rhs.data.p = tlook->data.p;
                NextToken();                                    // eat @symbol
            }
            else {
                Expression( &rhs);
            }

            ArrayAssignment( parmlist, &rhs);
            if ( rhs.memtype & P_MALLOCED) {
                FreeTokenParmList( rhs.data.p);                 // 6-26-2015
            }
            slfree( parmlist, "parse.f5");                      // 6-17 parmlist is array of char *pSymbol
        }
        MatchType( SEMICOLON);
    }
    else {
        MatchType( SEMICOLON);                  // eat null statement
    }

}

/**
 * @name        IfStatement
 * @oaram[i]    token *if_stmt  - pointer to the 'if' token
 */
typedef struct if_waypoints {
    int32_t then_clause;
    int32_t else_clause;
    int32_t end_if;
} if_waypoints;

void IfStatement( jump_stat *jumps, token *if_stmt)
{
    token retval;     InitToken( &retval);

    if ( !runtime) {                                                // if compile time
        if ( if_stmt->data.p == NULL) {
            if_stmt->data.p = slmalloc( sizeof( if_waypoints), "parse.4");
            if ( if_stmt->data.p == NULL) {
                FatalError( get_literal( OOM));                     // out of memory
            }
            memset( if_stmt->data.p, 0, sizeof(if_waypoints));
        }
        if_waypoints *waypoints = if_stmt->data.p;

        MatchType( LPAREN);
        Expression( &retval);
        MatchType( RPAREN);

        waypoints->then_clause = tlook->pc_index;                   // location of then clause
        Statement( jumps);                                          // then clause

        if ( tlook->type == SYMBOL && strcmp( tlook->pSymbol, "else") == 0) {
            NextToken();
            waypoints->else_clause = tlook->pc_index;               // location of else clause
            Statement( jumps);                                      // else clause
        }
        waypoints->end_if = tlook->pc_index;                        // location of end_if
    }
    else {
        /* -------
        ** runtime
        ** ---- */
        if_waypoints *waypoints = if_stmt->data.p;
        MatchType( LPAREN);
        Expression( &retval);

        if ( retval.datatype == UNDEFINED) {
            fprintf( stderr, "IfStatement retval.datatype %d\n", retval.datatype);
            goto ELSE_CLAUSE;
        }

        if ( retval.datatype == INTEGER && retval.data.i == 0) {    // if ( false) goto else
            goto ELSE_CLAUSE;
        }

        pc_ip = waypoints->then_clause;
        tlook = get_array_element( p_code, pc_ip);
        Statement( jumps);                                          // then clause
        goto END_IF;

ELSE_CLAUSE:
        if ( waypoints->else_clause != 0) {
            pc_ip = waypoints->else_clause;
            tlook = get_array_element( p_code, pc_ip);
            Statement( jumps);                                          // else clause
        }
END_IF:
        pc_ip = waypoints->end_if;
        tlook = get_array_element( p_code, pc_ip);
        ;
    }
}

/**
 * @name WhileStatement
 * @param   token *while_statement - pointer to the 'while'
 *                token
 */
typedef struct while_waypoints {
    int32_t while_stmt;
    int32_t while_body;
    int32_t end_while;
} while_waypoints;

void WhileStatement( jump_stat *jumps, token *while_stmt)
{
    token retval;     InitToken( &retval);

    if ( !runtime) {                                                // if compile time
        while_waypoints *waypoints = slmalloc( sizeof( while_waypoints), "parse.5");
        if ( waypoints == NULL) {
            FatalError( get_literal( OOM));                         // out of memory
        }
        while_stmt->data.p = waypoints;

        MatchType( LPAREN);
        AddNameSpaceLayer();
        waypoints->while_stmt = tlook->pc_index;                    // location of the while expression
        Expression( &retval);
        MatchType( RPAREN);

        MatchType( LBRACE);
        waypoints->while_body = tlook->pc_index;                    // location of the while statements ....
        while ( tlook->type != RBRACE) {
            Statement( jumps);                                      // body
        }
        waypoints->end_while = tlook->pc_index;
        MatchType( RBRACE);
        RemoveNameSpaceLayer();
    }
    else {
        /* -------
        ** Runtime
        ** ---- */
        MatchType( LPAREN);
        AddNameSpaceLayer();
        while_waypoints *waypoints = while_stmt->data.p;
        jump_stat local_jumps; InitJumps( &local_jumps);
        local_jumps.iteration_stat = true;                          // inside iteration

WHILE_STMT:
        pc_ip = waypoints->while_stmt;
        tlook = get_array_element( p_code, pc_ip);
        Expression( &retval);

        if ( retval.datatype == INTEGER && retval.data.i == true) {   // if ( true) goto END_WHILE
            goto WHILE_BODY;
        }
        else {
            goto END_WHILE;
        }

WHILE_BODY:
        pc_ip = waypoints->while_body;
        tlook = get_array_element( p_code, pc_ip);
        if ( tlook) {
            while ( tlook->type != RBRACE) {
                Statement( &local_jumps);                               // while body
                if ( local_jumps.continue_stat) {
                    local_jumps.continue_stat = false;
                    goto WHILE_STMT;
                }
                else if ( local_jumps.break_stat || local_jumps.return_stat) {
                    goto END_WHILE;
                }
            }
        }
        goto WHILE_STMT;

END_WHILE:
        pc_ip = waypoints->end_while;
        tlook = get_array_element( p_code, pc_ip);
        MatchType( RBRACE);
        RemoveNameSpaceLayer();

        jumps->return_stat = local_jumps.return_stat;
        memcpy( &jumps->ret_token, &local_jumps.ret_token, sizeof( token));
    }
}

void ForStatement( jump_stat *jumps, token *for_stmt)
{
    UNREFERENCED_PARAMETER( jumps);
    UNREFERENCED_PARAMETER( for_stmt);
    fprintf( stderr, "TODO for \n");
}

/**
 * @name ParmList
 * @brief           parse subroutine's arguments or implement
 *                  call to this subroutine using caller's
 *                  parameters.
 * @param[i]        NULL or array of callers arguments.
 */
void ParmList( array *parmlist)
{
    int32_t parm_index = 0;

    while ( true) {
        if ( tlook->type == SYMBOL) {
            if ( runtime) {
                // add caller's arguments to symbol table using ProcDecl's argument names.
                token *parmlist_element = (token *)get_array_element( parmlist, parm_index);

                // stub check caller's argument type vs ProcDecl's expectation
                if ( parmlist_element) {
                    parmlist_element->type     = SYMBOL;             // add using local name for argument
                    parmlist_element->pSymbol  = tlook->pSymbol;
                    parmlist_element->subtype  = tlook->subtype;     // 06-09-2015 hoping SCALAR or ARRAY
                    parmlist_element->pc_index = UNDEFINED;          // auto variable

                    AppendNewSymbol( parmlist_element);
                }
                else {
                    // undef for missing arguments
                    // stub todo ?? do we need to caller_element = newToken and copy from tlook.
                    AppendNewSymbol( tlook);
                }
            }

            NextToken();
        }
        else {
            Expected( "scalar or array");
        }

        if ( tlook->type != COMMA) {
            break;
        }

        parm_index++;
        NextToken();                        // eat comma
    }
}

/**
 * @name        ProcDecl define or run subroutine
 * @brief       parse or execute subroutine
 * @param[i]    NULL or callers parmlist: arrray of args
 * @param[i]    retval
 * @ouptut[o]   retval->type, subtype, datatye, data
 */

void ProcDecl( array *parmlist, token *retval)
{
    const char *prev_sub = pcurrent_subroutine;     // preserve name of caller - stub also need run-time

    if ( tlook->type == SYMBOL) {
        pcurrent_subroutine = tlook->pSymbol;       // set this subroutine's name

        if ( runtime == false) {                    // if compile time
            if ( AllLayerLookup( tlook)) {          // subroutine already defined?
                char buffer[ 64];
                snprintf( buffer, sizeof(buffer), "symbol %s already defined.", tlook->pSymbol);
                FatalError( buffer);
            }
            tlook->subtype = SUBROUTINE;
            AppendNewSymbol( tlook);                // add subroutine name
        }

        NextToken();

        AddNameSpaceLayer();

        MatchType( LPAREN);                         // mandatory '(' <parm_list>* ')'
        if ( tlook->type != RPAREN) {
            ParmList( parmlist);
        }
        MatchType( RPAREN);

        jump_stat   jumps;
        InitJumps( &jumps);

        Statement( &jumps);
        if ( jumps.return_stat && retval != NULL) {
            memcpy( retval, &jumps.ret_token, sizeof(token));
        }

        RemoveNameSpaceLayer();
    }
    pcurrent_subroutine = prev_sub;                 // restore name of caller
}

// //// // //////// ///// ///////
// //// // //////// ///// ///////
// //// // //////// ///// ///////

/**
 * @name    Compile
 */
void Compile( void)
{
    while ( 1) {
        if ( tlook->type == SYMBOL) {
            if ( strcmp( tlook->pSymbol, "my") == 0) {
                NextToken();
                DataDecl();
                MatchType( SEMICOLON);                      // DataDecl ends in semicolon
                continue;
            }
            if ( strcmp( tlook->pSymbol, "sub") == 0) {
                NextToken();
                ProcDecl( NULL, NULL);
                continue;                                   // ProcDecl doesnt end in semicolon
            }
            if ( tlook->type == SEMICOLON) {
                NextToken();
                continue;                                   // eat null statements
            }

            Expected( lit_semicolon);
        }
        else if ( tlook->type == ISEOF) {
            break;
        }
        Expected( "data or proc declaration");
    }
}

/**
 * @name        LoadScript
 * @brief       load script file into memory
 * @param[i]    script_fn - script file name
 * @note        initialize parseinfo structure
 * ------------------------------ */
void LoadScript( const char *script_fn) {

    FILE    *fd = NULL;

    if ( script_fn == NULL) {
        fprintf( stderr, "missing script file name\n");
        exit( 4);
    }

    /*
    ** open script file and read into memory
    ** ------------------------------------- */
    fd = fopen( script_fn, "r");
    if ( NULL == fd) {
        fprintf( stderr, "%s\n", script_fn);
        fprintf( stderr, " file not found\n");
        exit( 4);
    }

    // get file size
    fseek( fd, 0L, SEEK_END);
    int sz = ftell( fd);
    fseek( fd, 0L, SEEK_SET);

    // alloc buffer for script source code
    char *pScript = (char *)slmalloc( sz + 1, "parse.6");
    if ( pScript == NULL) {
        fprintf( stderr, "out of memory\n");
        exit( 4);
    }

    int bytes_read = fread( pScript, sizeof(char), sz, fd);
    if ( bytes_read != sz) {
        fprintf( stderr, "bytes read %d, size %d\n", bytes_read, sz);
    }

    pScript[ sz] = '\0';

    fclose( fd);

    pCursor = pScript;
    mem_map.pSource = pScript;
    mem_map.script_name = script_fn;
    parseinfo.pLine = pScript;
    parseinfo.line_num = 0;
    parseinfo.column_num = 0;
}

/**
 * @name        CategorizeSymbol
 * @param[io]   token *t with type == SYMBOL, pSymbol -> literal
 *              token *t gets type, subtype, datatype, and
 *              data.x
 */
void CategorizeSymbol( token *t) {
    char    c;
    size_t  length;

    if ( t->pSymbol == NULL) {
        t->type = UNDEFINED;
        return;
    }

    int32_t saveColumn  = parseinfo.column_num;     // save
    char    *saveCursor = pCursor;

    pCursor = t->pSymbol;       // setup getNumberToken

    c = t->pSymbol[ 0];
    if ( isdigit(c)) {
        t->type = CONSTANT;
        t->subtype = NUMBER;
        t->datatype = getNumberToken( &length);
        if ( t->datatype == INTEGER) {
            t->data.i = atol( t->pSymbol);
        }
        else {
            t->data.d = atof( t->pSymbol);
        }
    }
    else {
        t->type = CONSTANT;
        t->subtype = STRING;
        t->datatype = POINTER;
        t->data.p   = t->pSymbol;
    }

    parseinfo.column_num = saveColumn;      // restore
    pCursor = saveCursor;
}

/**
 * @name        LatchScriptArgs
 * @brief       Setup scripts's main( @args) array
 * @param[i]    pvalue - pointer to parameters,...
 *       comma-separated-values
 * @param[i]    array *pCmdLineArgs[] gets element per parameter
 * @return      original or expanded array
 * ----------------------------------------------------------- */
array *LatchScriptArgs( char *pvalue, array *pCmdLineArgs)
{
    size_t  length = 0;
    char    *pLiteral = pvalue;
    char    c;

    while ( true ) {
        c = *pvalue++;
        if ( c == ','
             || ( c == '\0' && length != 0)) {
            token *t  = newToken();
            t->type    = STRING;
            t->subtype = SCALAR;             // t.pSymbol is pointer to literal text
            LatchTokenLiteral( t, pLiteral, length);
            CategorizeSymbol( t);            // complete token initialization
            pCmdLineArgs = append_array_element( pCmdLineArgs, t);

            pLiteral = pvalue;               // setup start of next argument
            length = -1;
        }
        length++;
        if ( c == '\0'){
            break;
        }
    }
    return pCmdLineArgs;
}

/*
 * @name        main
 * @brief       sensor language interpreter entry point
 * @usage       usage sl.exe -n=path/scriptname.sli
 * ------------------------------------------------------------------------- */
int main( int argc, char *argv[])
{
    int i;
    int rc = 0;
    int compile_only = false;
    const char *script_fn = NULL;
    array *pCmdLineArgs = new_array( 3);            // empty array get command line -args=parameters,...

    printf( "%s v0.3.0", argv[ 0]);
    for ( i = 1; i < argc; i++) {
        printf( " %s", argv[ i]);
    }
    printf( "\n");

    for ( i = 0; i < argc; i++) {
        /** parse -n=scriptname */
        if ( strncmp( "-n=", argv[ i], 3) == 0) {
            char *pvalue = argv[ i];
            pvalue += 3;                            // strip off "-n="
            script_fn = pvalue;                     // set script filename to parse
        }
        else if ( strcmp( "-c", argv[i]) == 0) {    // parse -c i.e. compile only
            compile_only = true;
        }
        else if ( strcmp( "-spin", argv[i]) == 0) { // -spin for debugging
            debugging_spin = true;
        }
        else if ( strcmp( "-debug", argv[i]) == 0) { // -debug for debug printf
            debugging = true;
        }
        else if ( strncmp( "-args=", argv[i], 6) == 0) {
            char *pvalue = argv[ i];
            pvalue  += 6;                           // strip off "-args=
                                                    // parse -args ala native test script
            pCmdLineArgs = LatchScriptArgs( pvalue, pCmdLineArgs);  // arrays dynamically expand...
        }

        /** also in general, unrecognized args go into $args for use by
         *  script */
    }
    LoadScript( script_fn);

    InitMemoryMap();
    InitNameSpace();

    token primer = { UNDEFINED};                    // prime the pump
    tlook = &primer;                                // +
    NextToken();                                    // +
    Compile();

    if ( debugging) {
        printf( "compile complete\n\n");
    }

    InitBuiltIn();
    InitServices();

// TODO: if compile only, also need return code for success / failure

    if ( compile_only == false) {

        while ( debugging_spin ) {
            ;
        }

        token *at_args    = newToken();
        at_args->type     = SYMBOL;
        at_args->subtype  = ARRAY;
        at_args->datatype = POINTER;
        at_args->pSymbol  = "@args";
        at_args->data.p   = pCmdLineArgs;

        array *parmlist    = new_array( 2);
        parmlist           = append_array_element( parmlist, at_args);

        token lookup_main;     InitToken( &lookup_main);
        lookup_main.type     = SYMBOL;
        lookup_main.subtype  = SUBROUTINE;
        lookup_main.pSymbol  = "main";

        token *sub_main  = ThisLayerLookup( &lookup_main);     // lookup sub main
        if ( sub_main) {
            token retval; InitToken( &retval);

            // perform interpreted call main( @args);
            runtime = true;
            tlook    = sub_main;
            pc_ip    = sub_main->pc_index;                     // main is entry point
            ProcDecl( parmlist, &retval);

            if ( retval.type == NUMBER && retval.datatype == INTEGER) {
                rc = retval.data.i;
            }
        }
    }

    if ( debugging) {
        slcore_cleanup();
        slmem_list();
    }

    return rc;
}


