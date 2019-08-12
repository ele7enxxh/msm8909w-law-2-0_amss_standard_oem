/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: slcore.h
** DESC: sensor language - slang - builtin functions
**                                 interface with sensor1 APIs
** Revision History
** ================================================================ */
#ifndef SLCORE_H
#define SLCORE_H

#include "qmgr.h"
#include "sli.h"
#include "array.h"
#include "sensor1.h"
#include "sns_common_v01.h"

#include <pthread.h>

#define MAX_SERVICE_NUM 4               // stub should be set from where ?
#define MAX_CLIENTS 3
#define BASE_CLIENT 100                 // client numbers in the range of 100 - 102

typedef struct activity {
    uint8_t  instance_id;               // holds instance_id for service
    bool     active;                    // service enabled active
} activity;

typedef struct client {
    bool                active;                 // client slot active
    sensor1_handle_s    *handle;                // client handle assigned by sensor1_open
    activity            work[ MAX_SERVICE_NUM]; // index is service_num
} client;

/*
** respind - carries response | indication messages presented to callback function
** ---------------------------------------------------------------------------- */
typedef struct respind {
   queue                qlw;
   int                  client_index;       // cb_data - BASE_CLIENT
   sensor1_msg_header_s header;
   sensor1_msg_type_e   msg_type;
   void                 *msg_ptr;
} respind;

typedef struct rendez {
    bool sync_wait;
    bool async_wait;
    pthread_mutex_t mutx;
    pthread_cond_t  cond;
    struct timespec ts;
} rendezvous;

void slsend_req( const array *parmlist, token *retval);
void slsend_recv( const array *parmlist, token *retval);
void slrecv( const array *parmlist, token *retval);
void slcore_init( void);
void slopen( const array *parmlist, token *retval);
void slclose( const array *parmlist, token *retval);
void slcore_cleanup( void);


#endif  // SLCORE_H
