/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: slget.c
** DESC: sensor language - slang - slget_req() function
**       building block for mark/sweep garbage collector
**       need to be made thread safe
** Revision History
** ================================================================ */
#include "sli.h"
#include "slmalloc.h"
#include <stdio.h>
#include <stdlib.h>     // malloc

extern int runtime;     // boolean
extern bool debugging;  // boolean

typedef struct {
    void    *p_memory;
    char    *loc;
    size_t  size;
} memories;

#define MAX_ENTRIES 2048
int first_free = 0;
int high_water = 0;
memories memory[ 2048];     // eventually want to use array so this can be expanded dynamically

/* forward declaration */
void HexPrint(uint32_t len, uint8_t *pByteArray);

/**
 * @name mem_latch
 * @brief record address, size, and caller_id of slmalloc'ed ram
 * @param p_memory
 * @param loc
 * @param size
 */
void mem_latch( void *p_memory, char *loc, size_t size )
{
    if ( runtime) {
        int i = first_free;
        if ( first_free >= 0 && first_free < MAX_ENTRIES) {
            memory[ i].p_memory = p_memory;
            memory[ i].loc = loc;
            memory[ i].size = size;

            i++;
            while ( i < MAX_ENTRIES) {
                if ( memory[ i].p_memory == NULL) {
                    first_free = i;
                    if ( first_free >= high_water) {
                        high_water = i;
                    }
                    return;
                }
                i++;
            }
        }
        printf("mem_latch no more slots\n");
    }
}

/**
 * @name mem_forget
 * @brief forget memory recorded by mem_latch
 * @param p_memory
 */
void mem_forget( void *p_memory)
{
    if ( runtime) {
        int i = high_water;
        while ( i >= 0 ) {
            if ( p_memory == memory[ i].p_memory) {
                memory[ i].p_memory = NULL;
                memory[ i].loc = NULL;
                memory[ i].size = 0;
                first_free = i;
                free( p_memory);
                return;
            }
            i--;
        }
        if ( debugging) {
            fprintf( stderr, " mem_forget %p not found\n", p_memory);
        }
    }
}

/**
 * @name slmem_list
 * @brief list address, size, and id of slmalloc'ed ram
 */
void slmem_list( void)
{
    printf( "mem_list\n");
    int i;
    for ( i = 0; i < MAX_ENTRIES; i++) {
        if ( memory[ i].p_memory != NULL) {
            printf( "==> %d: %p %s %lu\n",
                   i,
                   memory[ i].p_memory,
                   memory[ i].loc,
                   memory[ i].size);
            HexPrint( memory[ i].size, memory[ i].p_memory);
            if ( strcmp( memory[ i].loc, "newToken") == 0) {
                token *t = (token *)memory[ i].p_memory;
                char *pSymbol = t->pSymbol;
                if ( pSymbol) {
                    if ( debugging) {
                        printf( "\t\t%s\n", pSymbol);
                    }
                }
            }
        }
    }
    printf( "end mem_list\n");
}

/**
 * @name slmalloc
 * @brief allocate ram and record p_memory, loc, and size
 * @param size
 * @param loc - caller id
 * @return void* - NULL or pointer to allocated ram
 */
void *slmalloc( size_t size, char *loc)
{
    void *p_memory = malloc( size);
    if ( runtime) {
//        if ( debugging) {
//            printf( "slmalloc %s %p %lu\n", loc, p_memory, size);
//        }
    }
    mem_latch( p_memory, loc, size);
    return p_memory;
}


/**
 * @name slfree
 * @brief free ram obtained by slmalloc
 * @param p_memory
 */
void slfree( void *p_memory, char *loc)
{
    if ( runtime) {
        UNREFERENCED_PARAMETER( loc);
//        if ( debugging) {
//            printf( "slfree     %s %p\n", loc, p_memory);
//        }
    }

    mem_forget( p_memory);
}

/**
 * @name    LatchLiteral
 * @brief   relocate a literal char[] to the heap
 * @param[i] pFrom
 * @param[i] len    - number of characters excluding traling Z
 * @return void*    - pointer to null terminated char[] in heap
 *         memory
 */
void *LatchLiteral( const char *p_from, int len)
{
    char *p_to = slmalloc( len + 1, "LatchLiteral");
    if ( p_to == NULL) {
        FatalError( get_literal(OOM));
    }
    memcpy( p_to, p_from, len);
    p_to[ len] = '\0';
    return( p_to);
}

void HexPrint(uint32_t len, uint8_t *pByteArray) {

    uint32_t i;

    printf("%p  ", pByteArray);
    for (i = 0; i < len; i++) {
        if 	(i && i % 4 == 0) {
            printf(" ");
        }
        if (i && i % 32 == 0) {
           printf("\n%p  ", ( pByteArray + i ));
        }
        printf("%2.2x", pByteArray[i]);
    }
    printf("\n");
 }



