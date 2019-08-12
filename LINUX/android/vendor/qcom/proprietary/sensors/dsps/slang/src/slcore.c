/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: slcore.c
** DESC: sensor language - slang - interface to sensor1 API
** Revision History
** 11-09-2015       df      Fix static analysis errorss
** ================================================================ */
#include "literals.h"
#include "qmgr.h"
#include "sli.h"
#include "slcore.h"
#include "slmalloc.h"
#include "slnew_req.h"

#include "sensor1.h"
#include "sns_common_v01.h"
#include "sns_sam_amd_v01.h"

#include <pthread.h>
#include <stdio.h>                      // printf
#include <stdlib.h>                     // exit
#include <signal.h>

extern bool option_script_handles_indications;
extern bool option_script_handles_errors;

bool slcore_initialized = false;        // one time initialization

sigset_t set;

qhead ri_Queue;

/*
** clients - anchor for client activity
*/
client  clients[ MAX_CLIENTS];                  // index :: cb_data = index

uint8_t txn_id = 0;                             // auto incrementing transaction_id

/*
** rv - inter-thread rendevous
*/
rendezvous rv;

intptr_t await_client_index;
sensor1_msg_header_s await_reqhdr;              // header for request. we're waiting for response.
respind *await_pMsg = NULL;

void InitRV( rendezvous *rv)
{
    pthread_mutex_init( &rv->mutx, NULL );
    pthread_cond_init( &rv->cond, NULL );
    rv->sync_wait = false;
    rv->async_wait = false;
}

/**
 * @name    PostRv
 * @brief   post completion of synchronous or asynchronous event
 * @param rv
 * @param type  'S' or 'A'  - sync or asynch completion
 */
void PostRV( rendezvous *rv, char type)
{
    pthread_mutex_lock( &rv->mutx);
    if ( rv->sync_wait && type == 'S') {
        rv->sync_wait = false;
    }
    else if ( rv->async_wait && type == 'A') {
        rv->async_wait = false;
    }
    pthread_cond_signal( &rv->cond);
    pthread_mutex_unlock( &rv->mutx);
}

int WaitRV( rendezvous *rv, int timeout)
{
    pthread_mutex_lock( &rv->mutx);

    clock_gettime( CLOCK_REALTIME, &rv->ts);
    rv->ts.tv_sec += timeout;

    int rc = 0;
    while ( rc == 0 && ( rv->sync_wait || rv->async_wait)) {
        rc = pthread_cond_timedwait( &rv->cond, &rv->mutx, &rv->ts);
    }
    pthread_mutex_unlock( &rv->mutx);

    return rc;
}

/**
 * @name notify_data_cb
 * @brief this function is called by the sensor1 runtime
 * @param cb_data
 * @param msg_hdr_ptr
 * @param msg_type
 * @param msg_ptr       - pointer to payload body
 */
void notify_data_cb( intptr_t cb_data,
                     sensor1_msg_header_s *msg_hdr_ptr,
                     sensor1_msg_type_e msg_type,
                     void *msg_ptr )
{
    if( msg_hdr_ptr == NULL) {
        fprintf( stderr, "notify_data_cb: msg_hdr_ptr == NULL\n");
        return;
    }
    else {
        /*
        printf("notify_data_cb: hdr.service_number: %u\n\thdr.msg_id: %d\n\t"
               "hdr.msg_type: %d\n\thdr.msg_size: %d\n\thdr.txn_id: %d\n",
               msg_hdr_ptr->service_number,
               msg_hdr_ptr->msg_id,
               msg_type,
               msg_hdr_ptr->msg_size,
               msg_hdr_ptr->txn_id );
        */
    }

    if ( cb_data < 0 || cb_data >= MAX_CLIENTS) {
        char errbuf[ 64];
        snprintf( errbuf, sizeof(errbuf), "notify_data_cb: invalid cb_data %p", (void *)cb_data);
        FatalError( errbuf);
    }

//    int client_index = (int)cb_data;

    if ( msg_ptr == NULL) {
        printf("notify_data_cb: msg_ptr == NULL\n");
        return;
    }

    if ( msg_type == SENSOR1_MSG_TYPE_IND
         && option_script_handles_indications == false) {
        sensor1_handle_s *handle = clients[ cb_data].handle;
        sensor1_free_msg_buf( handle, msg_ptr);                 // free sensor1 buffer
        return;
    }

    /*
    ** queue response or indication message for further
    ** processing by the script
    ** --------------------------------------------- */
    if ( msg_type == SENSOR1_MSG_TYPE_RESP
         || msg_type == SENSOR1_MSG_TYPE_IND ) {

        respind *pMsg = (respind *)slmalloc( sizeof( respind) + msg_hdr_ptr->msg_size, "slcore.1");
        if ( pMsg == NULL) {
            fprintf( stderr, "notify_data_cb: %s. discarding message\n", get_literal( OOM));
            return;
        }

        pMsg->qlw.pforward          = NULL;
        pMsg->qlw.pbackward         = NULL;
        pMsg->client_index          = (int)cb_data;
        pMsg->header.service_number = msg_hdr_ptr->service_number;
        pMsg->header.msg_id         = msg_hdr_ptr->msg_id;
        pMsg->header.msg_size       = msg_hdr_ptr->msg_size;
        pMsg->header.txn_id         = msg_hdr_ptr->txn_id;
        pMsg->msg_type              = msg_type;
        pMsg->msg_ptr               = NULL;
        if ( pMsg->header.msg_size != 0) {
            pMsg->msg_ptr = ((uint8_t *)pMsg) + sizeof(respind);
            memcpy( pMsg->msg_ptr, msg_ptr, msg_hdr_ptr->msg_size); // stage payload to local memory

            sensor1_handle_s *handle = clients[ cb_data].handle;
            sensor1_free_msg_buf( handle, msg_ptr);                 // free sensor1 buffer
            msg_ptr = NULL;
        }

        /*
        ** if slsend_recv() is waiting for this response
        ** ------------------------------------------ */
        pthread_mutex_lock( &rv.mutx);
        bool sync_wait = rv.sync_wait;
        pthread_mutex_unlock( &rv.mutx);

        if ( msg_type == SENSOR1_MSG_TYPE_RESP
             && sync_wait
             && await_client_index == (int)cb_data
             && await_reqhdr.service_number == pMsg->header.service_number
             && await_reqhdr.msg_id == pMsg->header.msg_id
             && await_reqhdr.txn_id == pMsg->header.txn_id ) {
                await_pMsg = pMsg;
                PostRV( &rv, 'S');          // post synchronous completion
        }
        else {
            int rc = PutAtTail( &ri_Queue, &pMsg->qlw);
            PostRV( &rv, 'A');              // post asynchronous event
        }
    }
    else {
        if( SENSOR1_MSG_TYPE_BROKEN_PIPE == msg_type  ) {
            fprintf( stderr, "notify_data_cb: Got broken pipe\n" );
        }
        else {
            fprintf( stderr, "notify_data_cb: received INVALID MSG type\n" );
        }
        if( msg_ptr != NULL ) {
            sensor1_handle_s *handle = clients[ cb_data].handle;
            sensor1_free_msg_buf( handle, msg_ptr);
        }
    }
}

/**
 * @name    slisend
 * @brief   perform sensor1_write(), conditionally wait for
 *          response
 * @param   parmlist
 * @param   retval
 * @param   blocking - optional
 */
void slisend( const array *parmlist, token *retval, bool blocking)
{
    sensor1_error_e rc = 0;

    // todo problem of return rc when blocking == false, return response when blocking == true
    retval->type = VALUE;
    retval->subtype = SCALAR;
    retval->datatype = INTEGER;
    retval->data.i = rc;

    token *t_client = ( token *)get_array_element( parmlist, 0);
    if ( t_client == NULL || t_client->datatype != INTEGER) {
        RTErrorMsg( get_literal( BAD_CLIENT));
        retval->data.i = 512;                    // stub need some rc mapping
        return;
        // stub want/need something like $error_message = slget_last_error();
        // e.g set return value to -1 means use slget_last_error();
    }

    int client_index = t_client->data.i;
    if ( client_index < 0 || client_index >= MAX_CLIENTS) {
        RTErrorMsg( get_literal( BAD_CLIENT));
        retval->data.i = 512;                    // stub need some rc mapping
        return;
    }

    token *t_request = ( token *)get_array_element( parmlist, 1);
    if ( t_request == NULL) {
        RTErrorMsg( "missing \'request\' argument");
        return;
    }
    else if ( t_request->datatype == UNDEFINED) {
        if ( t_request->pSymbol) {
            UninitializedVariable( t_request->pSymbol);
        }
        else {
            RTErrorMsg( get_literal( BAD_ARGUMENT));
            retval->data.i = 512;                    // stub need some rc mapping
        }
        return;
    }

    request *message = ( request *)t_request->data.p;

    void *tempMsg    = NULL;
    int32_t msg_size = message->header.msg_size;
    message->header.txn_id = txn_id++;           // auto increment transaction_id
    sensor1_handle_s *handle  = clients[ client_index].handle;

    if ( msg_size > 0) {
        rc = sensor1_alloc_msg_buf( handle,
                                    msg_size,
                                    &tempMsg );
        if( rc != SENSOR1_SUCCESS) {
            char errbuf[ 64];
            snprintf( errbuf, sizeof( errbuf), "sensor1_alloc_msg_buf() rc %d\n", rc );
            RTErrorMsg( errbuf);
            retval->data.i = rc;
            return;
        }
        memcpy( tempMsg, message->payload, msg_size);
    }

    /*
    ** setup for slsend_recv()
    */
    if ( blocking) {
        pthread_mutex_lock( &rv.mutx);
        await_client_index = client_index;
        await_reqhdr.service_number = message->header.service_number;
        await_reqhdr.msg_id = message->header.msg_id;
        await_reqhdr.txn_id = message->header.txn_id;
        await_pMsg = NULL;
        rv.sync_wait = true;
        pthread_mutex_unlock( &rv.mutx);
    }

    rc = sensor1_write( handle, &message->header, tempMsg );

    if ( rc != SENSOR1_SUCCESS ) {
        char errbuf[ 64];
        fprintf( stderr, "slisend: sensor1_write rc %d\n", rc );
        RTErrorMsg( errbuf);
        retval->data.i = rc;    // stub map rc to sensor1 error messages ...
        return;
    }

    if ( blocking) {
        retval->type = VALUE;                               // setup bad_timeout value of timedout
        retval->subtype = SCALAR;
        retval->memtype = UNDEFINED;
        retval->datatype = UNDEFINED;

        int timeout = 6;                                    // default 6 seconds
        token *t_timeout = ( token *)get_array_element( parmlist, 2);
        if ( t_timeout != NULL) {
            if ( t_timeout->datatype != INTEGER) {
                RTErrorMsg( get_literal( BAD_TIMEOUT));
                retval->data.i = 512;                       // stub need some rc mapping
                retval->datatype = INTEGER;
                return;
            }
            timeout = ( int)t_timeout->data.i;              // timeout value in seconds
        }
        rc = WaitRV( &rv, timeout);
        if ( rc == 0) {
            retval->type = VALUE;
            retval->subtype = RESPONSE;
            retval->memtype  = P_MALLOCED;
            retval->datatype = POINTER;
            retval->data.p = await_pMsg;
        }
        else {
            char *msg = get_literal(TIMEDOUT);
            RTErrorMsg( msg);
            // RTErrorMsg or need setup slget_last_error() or something...
        }
    }
}

/**
 * @name    slsend_req - builtin function
 * @brief   send request message to sensor1
 * @usage   script: $rc = slsend_req( $client, $message);
 * @param   parmlist
 * @param   retval
 */
void slsend_req( const array *parmlist, token *retval)
{
    slisend( parmlist, retval, false);
}

/**
 * @name    slsend_recv
 * @brief   blocking send request and wait for response
 * @usage   script: $response = slsend_recv( $client, $request,
 *          [$timeout]);
 * @param   parmlist
 * @param   retval
 */
void slsend_recv( const array *parmlist, token *retval)
{
    slisend( parmlist, retval, true);
}

/**
 * @name    slrecv
 * @brief   retrieve a response or indication message
 * @usage   script: $respind = slrecv( [ $timeout]);
 * @param[i]   parmlist NULL or timeout element
 * @param[io]   retval gets NULL or UNDEFINED (timeout)
 */
void slrecv( const array *parmlist, token *retval)
{
    retval->type = VALUE;
    retval->subtype = SCALAR;
    retval->datatype = UNDEFINED;
    retval->data.p = NULL;

    int timeout = 6;                                    // default omitted seconds

    if ( parmlist != NULL) {
        token *t_timeout = ( token *)get_array_element( parmlist, 0);
        if ( t_timeout != NULL) {
            if ( t_timeout->datatype != INTEGER) {
                RTErrorMsg( get_literal( BAD_TIMEOUT));
                // RTErrorMsg or need setup slget_last_error() or something...
                return;
            }
            timeout = ( int)t_timeout->data.i;          // timeout value in seconds
        }
    }

    pthread_mutex_lock( &rv.mutx);
    rv.async_wait = true;
    pthread_mutex_unlock( &rv.mutx);

    queue *p_element = NULL;
    int rc = GetFromHead( &ri_Queue, &p_element);
    if ( p_element == NULL) {
        rc = WaitRV( &rv, timeout);
        if ( rc == 0) {
            GetFromHead( &ri_Queue, &p_element);
        }
        else {
            RTErrorMsg( get_literal( TIMEDOUT));
            // RTErrorMsg or need setup slget_last_error() or something...
        }
    }

    pthread_mutex_lock( &rv.mutx);
    rv.async_wait = false;
    pthread_mutex_unlock( &rv.mutx);

    if ( p_element != NULL) {
        retval->type = VALUE;
        retval->subtype = RESPONSE;
        retval->memtype |= P_MALLOCED;
        retval->datatype = POINTER;
        retval->data.p = p_element;
    }
}

/**
 * @name    slcore_init
 * @brief   one time initialization of sensor interface
 */

void slcore_init( void)
{
    if ( slcore_initialized == false) {
        sensor1_error_e rc = sensor1_init();
        if( SENSOR1_SUCCESS != rc ) {
            fprintf( stderr, "slcore_init: sensor1_init rc %d\n", rc );
            exit( rc);
        }

        sigemptyset( &set );
        sigaddset( &set, SIGALRM );
        sigprocmask( SIG_SETMASK, &set, NULL );

        OpenQ( &ri_Queue);  // stub check rc

        InitRV( &rv);       // for inter-thread wait/post rendevous

        slcore_initialized = true;
    }
}

/**
 * @name        slopen
 * @param[i]    array *parmlist
 * @param[io]   token *retval gets UNDEFINED or CLIENT
 */
void slopen( const array *parmlist, token *retval)
{
    sensor1_error_e rc = SENSOR1_SUCCESS;
    int client_index = 0;                               // stub dynamically assign client index

    UNREFERENCED_PARAMETER( parmlist);

    slcore_init();

    clients[ 0].active = true;
    intptr_t cb_data = client_index;
    rc = sensor1_open( &clients[ client_index].handle,
                       notify_data_cb,
                       cb_data );
    if( SENSOR1_SUCCESS != rc ) {
        fprintf( stderr, "slopen: sensor1_open rc %d\n", rc );   // stub need setup slget_last_error()
        exit( rc);
    }

//    printf( "slopen: sensor1_open rc %d, handle %lu\n", rc, (long unsigned int)clients[0].handle );

    retval->type = VALUE;
    retval->subtype = SCALAR;
    retval->datatype = INTEGER;
    retval->data.i = client_index;
}

/**
 * @name        slclose
 * @param[i]    array *parmlist
 * @param[io]   token *retval gets UNDEFINED or CLIENT
 */
void slclose( const array *parmlist, token *retval)
{
    if ( parmlist->high_water != 0) {
        RTErrorMsg( "slclose missing client handle\n");
        return;
    }

    token *t_client = ( token *)get_array_element( parmlist, 0);
    if ( t_client == NULL || t_client->datatype != INTEGER) {
        RTErrorMsg( get_literal( BAD_CLIENT));
        return;
    }

    int client_index = t_client->data.i;

    sensor1_error_e rc = sensor1_close( clients[ client_index].handle );
    if( SENSOR1_SUCCESS != rc ) {
        fprintf( stderr, "slclose: sensor1_close rc %d\n", rc );
    }

    retval->type = VALUE;
    retval->subtype = SCALAR;
    retval->datatype = INTEGER;
    retval->data.i   = rc;
}

void slcore_cleanup( void)
{
    fprintf( stderr, "core_cleanup\n");
    queue *p_element = NULL;

    if ( slcore_initialized) {
        while ( 1) {
            int rc = GetFromHead( &ri_Queue, &p_element);
            if ( p_element == NULL) {
                break;
            }
            slfree( p_element, "core_cleanup");
        }
    }
}
