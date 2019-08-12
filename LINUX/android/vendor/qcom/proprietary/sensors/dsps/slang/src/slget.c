/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: slget.c
** DESC: sensor language - slang - slget() function
** Revision History
** 11-09-2015       df      Fix static analysis errorss
** ================================================================ */
#include "sli.h"
#include "array.h"
#include "literals.h"
#include "slcore.h"
#include "slmalloc.h"
#include "slnew_req.h"

#include <stdio.h>

/**
 * @name slget
 * @brief   get field values from response or indication message
 * @param[ i]   parmlist
 * @param[ io]  retval
 * script usage examples:
 *
 * my ( $count) = slget( $all_resp, {info});
 * my ( $sensor_id, $sensor_name) = slget( $all_resp,
 *  {"info" => 0,sensor_id,sensor_name} );
 *
 * ---------------------------------------------- */
static void set_int_type( token *p_element)
{
    p_element->type = VALUE;
    p_element->subtype = SCALAR;
    p_element->datatype = INTEGER;
}

void slget( const array *parmlist, token *retval)
{
//    printf( "slget\n");
//    printf( "slget parmlist %p, retval %p\n", (void *)parmlist, (void *)retval);

    token *t_message = ( token *)get_array_element( parmlist, 0);
    if ( t_message == NULL) {
        fprintf( stderr, "slget t_message NULL\n");
        return;
    }

    respind *p_respind = t_message->data.p;
    if ( p_respind == NULL) {
        fprintf( stderr, "slget p_respind NULL\n");
        return;
    }

    uint32_t service_number = p_respind->header.service_number;
//    sprintf( "slget service_number %d\n", (int)service_number);

    if ( service_toc[ service_number].name == NULL) {
        RTErrorMsg( get_literal( UNSUPPORTED_SERVICE));
        return;
    }

    token *t_field_list = ( token *)get_array_element( parmlist, 1);
    if ( t_field_list == NULL) {
        RTErrorMsg( "missing { fields, ...} argument\n");
        return;
    }

    array *output_array = new_array( 2);

    array *a_field_list = t_field_list->data.p;
    int i = 0;
    for ( ; i <= a_field_list->high_water; i++  ) {

        token *t_field = get_array_element( a_field_list, i);
        if ( t_field == NULL) {
            continue;                       // should not occur
        }
        if ( !( t_field->subtype == KEYVALUE || t_field->type == SYMBOL)) {
            RTErrorMsg( "slget bad field type or subtype\n");
            return;
        }
        char *fieldname = t_field->pSymbol;

        token *p_value = newToken();
        p_value->type    = VALUE;
        p_value->subtype = SCALAR;
        p_value->memtype = T_MALLOCED;      // token eligible for garbage collection

        /*
        ** satisfy client, service_number, msg_id, msg_type, msg_size, txn_id
        ** --------------------------------------------------------------- */
        if ( strcasecmp( fieldname, get_literal( CLIENT)) == 0) {
            set_int_type( p_value);
            p_value->data.i = p_respind->client_index;
        }
        else if ( strcasecmp( fieldname, get_literal( SRVC_NUM)) == 0) {
            set_int_type( p_value);
            p_value->data.i = p_respind->header.service_number;
        }
        else if ( strcasecmp( fieldname, get_literal( MSG_ID)) == 0) {
            set_int_type( p_value);
            p_value->data.i = p_respind->header.msg_id;
        }
        else if ( strcasecmp( fieldname, get_literal( MSG_TYPE)) == 0) {
            set_int_type( p_value);
            p_value->data.i = p_respind->msg_type;
        }
        else if ( strcasecmp( fieldname, get_literal( MSG_SIZE)) == 0) {
            set_int_type( p_value);
            p_value->data.i = p_respind->header.msg_size;
        }
        else if ( strcasecmp( fieldname, get_literal( TXN_ID)) == 0) {
            set_int_type( p_value);
            p_value->data.i = p_respind->header.txn_id;
        }
        else if ( p_respind->msg_type == SENSOR1_MSG_TYPE_RESP
                  && strcasecmp( fieldname, get_literal( RESULT)) == 0) {
            set_int_type( p_value);
            sns_common_resp_s_v01 *pResp = (sns_common_resp_s_v01 *)p_respind->msg_ptr;
            p_value->data.i = pResp->sns_result_t;
        }
        else if ( p_respind->msg_type == SENSOR1_MSG_TYPE_RESP
                  && strcasecmp( fieldname, get_literal( ERR)) == 0) {
            set_int_type( p_value);
            sns_common_resp_s_v01 *pResp = (sns_common_resp_s_v01 *)p_respind->msg_ptr;
            p_value->data.i = pResp->sns_err_t;
        }
        else if ( p_respind->msg_type == SENSOR1_MSG_TYPE_RESP
                  && strcasecmp( fieldname, get_literal( VERSION)) == 0) {
            set_int_type( p_value);
            sns_common_version_resp_msg_v01 *pResp = (sns_common_version_resp_msg_v01 *)p_respind->msg_ptr;
            p_value->data.i = pResp->interface_version_number;
        }
        else if ( p_respind->msg_type == SENSOR1_MSG_TYPE_RESP
                  && strcasecmp( fieldname, get_literal( MAX_MSG_ID)) == 0) {
            set_int_type( p_value);
            sns_common_version_resp_msg_v01 *pResp = (sns_common_version_resp_msg_v01 *)p_respind->msg_ptr;
            p_value->data.i = pResp->max_message_id;
        }
        else {
            /*
            ** invoke service dependent getter function
            ** ------------------------------------- */
            get_func getter = service_toc[ service_number].getter_func;
            if ( getter == NULL) {
                RTErrorMsg( "get_func is NULL");
                continue;
            }
            ( getter)( p_respind, t_field, p_value);
        }

        if ( t_field->subtype == KEYVALUE) {
            slfree( p_value, "slget.1");
            continue;
        }
        output_array = append_array_element( output_array, p_value);
    }

    if ( output_array->high_water >= 0) {
        retval->type = PARMLIST;
        retval->memtype |= P_MALLOCED;              // token data.p eligible for garbage collection
        retval->subtype = ARRAY;
        retval->datatype = POINTER;
        retval->data.p = output_array;
    }
    else {
        slfree( output_array, "slget");
        retval->type = UNDEFINED;
        retval->subtype = UNDEFINED;
        retval->datatype = UNDEFINED;
    }
}

/**
 * @name    slset_req
 * @brief   script usage:
 *          slset_req( $request, { field => value, ...});
 * @param[i]   array *parmlist
 * @param[io]  token *retval    - returns nothing
 */
void slset_req( const array *parmlist, token *retval)
{
    sl_msg_type_e   msg_enum = BAD_MSG_ENUM;

    retval->datatype = UNDEFINED;
    retval->data.p   = NULL;

    token *t_request = ( token *)get_array_element( parmlist, 0);
    if ( t_request == NULL) {
        RTErrorMsg( "missing \'request\' argument");
        return;
    }
    if ( t_request->datatype != POINTER) {
        RTErrorMsg( "invalid \'reqyest\' argument");
        return;
    }
    request *p_request = t_request->data.p;

    if ( p_request->header.msg_size == 0) {
        RTErrorMsg( "invalid fields argument");
        return;
    }

    uint32_t srvc_id = p_request->header.service_number;

    /* cloned from slnew_req.c
    ** when optional { field => value, ... } supplied, invoke ServiceSetReq()
    ** ------------------------------------------------------------------- */
    token *t_fields = ( token *)get_array_element( parmlist, 1);
    if ( t_fields != NULL) {
        if ( t_fields->subtype != ARRAY) {
            RTErrorMsg( "slset_req invalid fields argument: expected \'{\' \"field\" => value, ...\'}\'");
            return;
        }
        array *fieldlist      = (array *)t_fields->data.p;

        int i;
        for ( i = 0; i <= fieldlist->high_water; i++) {
            token *t_keyvalue = get_array_element( fieldlist, i);
            set_req set_req_func  = service_toc[ srvc_id].set_req_func;
            ( set_req_func)( p_request, t_keyvalue);                  // call ServiceSetReq()
        }
    }
}

