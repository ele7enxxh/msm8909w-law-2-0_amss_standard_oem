/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: namespace.c
** DESC: sensor language - slang - block structured namespace support
** Revision History
** ================================================================ */
#include "sli.h"
#include "array.h"
#include "namespace.h"
#include "slmalloc.h"
#include <stdio.h>          // printf

/*
** The namespace is intended to hold layers of nested namespaces.
** Each time a LBRACE is encountered, a new inner layer
** Each time a RBRACE is encounter, inner layer is removed
** pNamespace -> array :: element [i] -> array of inodes for layer i
*/
array *pNamespace;

#define NUM_ELEMENTS 7

extern bool debugging;

void DisplayNameSpace( void);

/**
 * @name AddNameSpaceLayer
 * @param[i] none
 * @return array* to new layer
 */
array *AddNameSpaceLayer( void) {

    /** as necessary, create namespace array */
    array *pLayer = new_array( NUM_ELEMENTS);           /* array for new layer*/

    pNamespace = append_array_element( pNamespace, pLayer);
//    if ( debugging) {
//        printf( "added_nsl layer %d, %p\n", pNamespace->high_water, pLayer); // debugging
//    }

    return pLayer;
}

/**
 * @name ReleaseNameSpaceLayer
 * @brief   free the layer's tokens and token.data.p memory
 *          and free the pLayer array
 * @param pLayer
 */
void ReleaseNameSpaceLayer( array *pLayer)
{
    int i;
    if ( pLayer) {
        for ( i = pLayer->high_water; i >= 0; i--) {
            token *t = pLayer->element[ i];
//            if ( debugging) {
//                printf( "release_nss %s element  %d\n", t->pSymbol, i);
//            }
            if ( t->memtype & P_MALLOCED) {
                t->memtype ^= P_MALLOCED;
                slfree( t->data.p, "namespace.token data P");
                t->data.p = NULL;
            }
            if ( t->memtype & V_MALLOCED) {         // 'my' VarDecl's token?
                if ( t->memtype & T_MALLOCED) {
                    t->memtype ^= T_MALLOCED;
                    slfree( t, "namespace.token V");
                }
                pLayer->element[ i] = NULL;
            }
        }
        slfree( pLayer, "namespace.layer");
    }
}

/**
 * @name RemoveNameSpaceLayer
 * @param[i] none
 * @return array* to new layer
 */
void RemoveNameSpaceLayer( void)
{

    int32_t ns_index = pNamespace->high_water;

    if ( ns_index > 0) {
        array *pLayer = pNamespace->element[ ns_index];

//        if ( debugging) {
//            printf( "b4 release_ns layer %d %p\n", ns_index, pLayer);
//            // DisplayNameSpace();
//        }

        ReleaseNameSpaceLayer( pLayer);
        pNamespace->element[ ns_index] = NULL;
        pNamespace->high_water--;                          /* remove current layer */
    }
}


/**
 * @name  InitNameSpace
 */
void InitNameSpace( void)
{
    if ( pNamespace == NULL) {
        array *pArray = new_array( NUM_ELEMENTS);     /* array for namespace */
        pNamespace = pArray;
    }
    AddNameSpaceLayer();                              /* add layer for global symbols */
}

/**
 * @name AppendNewSymbol
 * @param[i] pointer to token defining the symbol
 */
void  AppendNewSymbol( token *lhs)
{
    int32_t ns_index = pNamespace->high_water;
//    if ( debugging) {
//        printf( "b4 append token %p, symbol %s to layer %d\n", lhs, lhs->pSymbol, ns_index);
//    }

    array *pLayer = pNamespace->element[ ns_index];
    array *pExpanded = append_array_element( pLayer, lhs);
    pNamespace->element[ ns_index] = pExpanded;
}

/**
 * @name        OneLayerLookup - check for already existing nu->pSymbol
 *              literal in current namespace layer.
 * @param[i]    pLayer - pointer to layer array
 * @param[i]    nu - pointer to token.
 * @retval      NULL or pointer to token using same symbol.
 */
token *OneLayerLookup( const array *pLayer, const token *nu) {

    token *pExisting = NULL;

    int index = pLayer->high_water;
    for ( ; index >= 0; index--) {
        token *t = pLayer->element[ index];
        if ( strcmp( nu->pSymbol, t->pSymbol) == 0) {
            pExisting = t;
            break;
        }
    }

    return pExisting;
}

/**
 * @name ThisLayerLookup - check for already existing
 *       nu->pSymbol
 * @param nu
 * @return NULL or already existing token*
 */
token *ThisLayerLookup( const token *nu) {

    int32_t ns_index = pNamespace->high_water;
    if ( ns_index == -1) {
        return NULL;
    }
    array *pLayer = pNamespace->element[ ns_index];
    return OneLayerLookup( pLayer, nu);
}

/**
 * @name        AllLayerLookup - lookup existing nu->pSymbol in
 *              namespace.
 * @param[i]    token *nu
 * @retval      NULL or pointer to token using same symbol.
 */
token *AllLayerLookup( const token *nu)
{
    token *pExisting = NULL;

    int ns_layer = pNamespace->high_water;                  // array index of current namespace layer
    while ( ns_layer >= 0) {
        array *pLayer = pNamespace->element[ ns_layer];    // pointer to current namespace layer
        pExisting = OneLayerLookup( pLayer, nu);
        if ( pExisting) {
            break;
        }
        ns_layer--;

    }
    return pExisting;
}

/* -->
void DisplayNameSpace( void)
{
    // for all layers
    int ns_layer = pNamespace->high_water;                  // array index of outer most namespace layer
    while ( ns_layer >= 0) {
        array *pLayer = pNamespace->element[ ns_layer];    // pointer to current namespace layer
        printf( "DisplayNameSpace layer %d\n", ns_layer);

        // display one layer
        int i_layer = pLayer->high_water;
        while ( i_layer >= 0) {
            token *pElement = pLayer->element[ i_layer];
            if ( pElement == NULL) {
                printf( " + element %d NULL\n", i_layer);
            }
            else {
                printf( " + element %d, token %p\n", i_layer, pElement);
                if ( pElement->pSymbol) {
                    printf( " + element %d, symbol %s\n", i_layer, pElement->pSymbol);
                }
                printf( " + element %d, subtype %d, datatype %d\n", i_layer, pElement->subtype, pElement->datatype);
            }

            printf( " + ----------\n");
            i_layer--;
        }
        ns_layer--;
    }

    printf( " ===\n");
}

---> */

