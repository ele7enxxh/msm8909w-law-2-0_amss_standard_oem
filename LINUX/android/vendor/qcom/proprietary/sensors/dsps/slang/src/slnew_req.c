/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: slnew_req.c
** DESC: sensor language - slang - slnew_req() function
** Revision History
** 11-09-2015       df      Fix static analysis errors
** ================================================================ */
#include "sensor1.h"
#include "sli.h"
#include "alu.h"
#include "array.h"
#include "builtin.h"
#include "diagnostics.h"
#include "literals.h"
#include "namespace.h"
#include "slmalloc.h"
#include "slnew_req.h"

#include "sns_common_v01.h"

#include <stdio.h>

/**
 * @name service2num
 */
name2num service2num[] = {
    { "smgr", SNS_SMGR_SVC_ID_V01},                     // 0
    { "amd",  SNS_SAM_AMD_SVC_ID_V01},                  // 4
    { "rmd",  SNS_SAM_RMD_SVC_ID_V01},                  // 5
    { "reg",  SNS_REG2_SVC_ID_V01},                     // 15
    { "rotv", SNS_SAM_ROTATION_VECTOR_SVC_ID_V01},      // 18
    { "grav", SNS_SAM_GRAVITY_VECTOR_SVC_ID_V01},       // 20
    { "ornt", SNS_SAM_ORIENTATION_SVC_ID_V01},          // 23
    { "time", SNS_TIME2_SVC_ID_V01},                    // 24
    { "ped",  SNS_SAM_PED_SVC_ID_V01},                  // 37
    { "grv",  SNS_SAM_GAME_ROTATION_VECTOR_SVC_ID_V01}, // 43
};

#define NUM_SERVICES 56         // IOD == 55

name2num    message2enum[] = {
  {  "cancel", SAM_CANCEL            },
  {  "version", SAM_VERSION          },
  {  "enable", SAM_ENABLE            },
  {  "single_read", SAM_ENABLE       },
  {  "disable", SAM_DISABLE          },
  {  "single_write", SAM_DISABLE     },
  {  "get_report", SAM_GET_REPORT    },
  {  "reset", SAM_ALGO_SPECIFIC      },
  {  "batch", SAM_BATCH              },
  {  "flush", SAM_FLUSH              },
  {  "attributes", SAM_ALGO_ATTRIB   },

  {  "report", SMGR_REPORT           },
  {  "all_info", SMGR_ALL_INFO       },
  {  "sngl_info", SMGR_SNGL_INFO     },
  {  "selftest", SMGR_SELFTEST       },
  {  "buffered", SMGR_BUFFERED       },
  {  "buf_query", SMGR_BUF_QUERY     },

};

/**
 * @name    service_toc     service table of contents
 * @brief   index is service number
 */
service_entry   service_toc[ NUM_SERVICES];

/**
 * @name        get_service_id
 * @brief       get numberic service number from lc service name
 * @param[i]    char *lower case service name
 * @return      UNKNOWN or int service_number
 */
int get_service_id( const char *name)
{
    unsigned int i;
    // stub could use service_toc instead, skipping name == NULL. don't lookup service by name very often.
    for ( i = 0; i < sizeof( service2num) / sizeof( name2num); i++) {
        if ( strcmp( service2num[ i].name, name) == 0) {
            return service2num[ i].num;
        }
    }
    return UNKNOWN;
}

/**
 * @name        get_message_enum
 * @brief       get the enum corresponding to the input name
 * @param[i]    char *lower case message name
 * @return      UNKNOWN or sl_msg_type_e
 *
 */
sl_msg_type_e get_message_enum( const char *name)
{
    unsigned int i;
    for ( i = 0; i < sizeof( message2enum) / sizeof( name2num); i++) {
        if ( strcmp( message2enum[ i].name, name) == 0) {
            return ( sl_msg_type_e)message2enum[ i].num;
        }
    }
    return BAD_MSG_ENUM;;
}

// void InvalidMsgId( int32_t msg_id)
// {
//     char errbuf[ 64];
//
//     snprintf( errbuf, sizeof(errbuf), "invalid msg_id %d", msg_id);
//     RTErrorMsg( errbuf);
// }

/*
** my $request = slnew_req( "service", "message", [ { field => value, ...} ]);
*/

/**
 * @name    slnew_req
 * @param[i]   array *parmlist
 * @param[io]  token *retval - data.p <- req_msg
 */
void slnew_req( const array *parmlist, token *retval)
{
    int             srvc_id = 0;
    sl_msg_type_e   msg_enum = BAD_MSG_ENUM;

    retval->datatype = UNDEFINED;
    retval->data.p   = NULL;

    if ( parmlist->high_water < 1) {
        RTErrorMsg( "missing service or message_name\n");
        return;
    }

    token *t_service = ( token *)get_array_element( parmlist, 0);
    if ( t_service == NULL) {
        RTErrorMsg( "omitted \'service\' argument");
        return;
    }
    else if ( t_service->subtype == INTEGER) {
        srvc_id = t_service->data.i;
    }
    else if ( t_service->subtype == STRING || t_service->subtype == SCALAR) {
        srvc_id = get_service_id( t_service->data.p);
        if ( srvc_id < 0) {
            char errbuf[ 64];
            snprintf( errbuf, sizeof( errbuf), "unknown service %s\n", (char *)t_service->data.p);
            RTErrorMsg( errbuf);
            return;
        }
    }
    else {
        RTErrorMsg( "bad \'service\' argument");
        return;
    }

    token *t_message = ( token *)get_array_element( parmlist, 1);
    if ( t_message == NULL) {
        RTErrorMsg( "omitted \'message\' argument");
        return;
    }
    else if ( t_message->subtype == STRING || t_message->subtype == SCALAR) {
        msg_enum = get_message_enum( t_message->data.p);
        if ( msg_enum < 0) {
            char errbuf[ 64];
            snprintf( errbuf, sizeof(errbuf), "unknown message %s\n", (char *)t_message->data.p);
            RTErrorMsg( errbuf);
            return;
        }
    }
    else {
        RTErrorMsg( "bad \'message\' argument");
        return;
    }

    request *req_msg  = &service_toc[ srvc_id].req_msg[ msg_enum];
    if ( req_msg->header.msg_id == UNKNOWN) {
        char errbuf[ 64];
        snprintf( errbuf, sizeof( errbuf), "unknown message %s for service %s\n",
                  (char *)t_message->data.p, (char *)t_service->data.p);
        RTErrorMsg( errbuf);
        return;
    }

    /*
    ** allocate room for header plus payload body
    ** --------------------------------------- */
    request *p_request = slmalloc( sizeof( request) + req_msg->header.msg_size, "slnew_req.1");
    if ( p_request == NULL) {
        RTErrorMsg( get_literal( OOM));
        return;
    }
    p_request->header.service_number = req_msg->header.service_number;
    p_request->header.msg_id = req_msg->header.msg_id;
    p_request->header.msg_size = req_msg->header.msg_size;
    p_request->header.txn_id = 0;
    p_request->payload = NULL;
    if ( p_request->header.msg_size != 0) {
        p_request->payload = (( uint8_t *)p_request) + sizeof( request);
        memset( p_request->payload, 0, p_request->header.msg_size);
    }

    retval->type      = VALUE;
    retval->subtype   = SCALAR;
    retval->datatype  = POINTER;
    retval->memtype  |= P_MALLOCED;         // data.p is malloced
    retval->data.p    = p_request;

    /*
    ** when optional { field => value, ... } supplied, invoke ServiceNewReq()
    ** ------------------------------------------------------------------- */
    token *t_fields = ( token *)get_array_element( parmlist, 2);
    if ( t_fields != NULL) {
        if ( req_msg->header.msg_size == 0) {
            RTErrorMsg( "slnew_req invalid fields argument");
            return;
        }
        if ( t_fields->subtype != ARRAY) {
            RTErrorMsg( "slnew_req invalid fields argument: expected \'{\' \"field\" => value, ...\'}\'");
            return;
        }
        array *fieldlist      = (array *)t_fields->data.p;
        new_req new_req_func  = service_toc[ srvc_id].new_req_func;
        ( new_req_func)( p_request, fieldlist);                  // call NewServiceReq()
    }
}

/**
 * @name    InitServices
 * @brief   First set all service messages as -1 :: undefined
 *          Then call Init for each supported service.
 */
void InitServices( void)
{
    uint32_t    svc_id;
    int         msg_enum;
    /*
    ** set all msg_id to UNKNOWN_MSG
    ** ------------------------------ */
    for ( svc_id = 0; svc_id < NUM_SERVICES; svc_id++) {
        for ( msg_enum = 0; msg_enum < MAX_MSG_NUM; msg_enum++) {
            service_toc[ svc_id].req_msg[ msg_enum].header.msg_id = UNKNOWN;
        }
    }
    /*
    ** initialize known services
    ** ---------------------- */
    GravInit();
    GrvInit();
    OrntInit();
    PedInit();
    QmdInit();                      // amd, rmd
    RegInit();
    RotvInit();
    SmgrInit();
    TimeInit();
}

