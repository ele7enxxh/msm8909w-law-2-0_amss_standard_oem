/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: slnew_req.h
** DESC: sensor language - slang - sensor1 message database
** Revision History
** ================================================================ */
#ifndef SLNEW_H
#define SLNEW_H

#include "array.h"
#include "slcore.h"
#include "sensor1.h"

#define UNKNOWN -1

/*
** name2enum
** ------- */
typedef struct name2num {
    char    *name;
    int32_t num;
} name2num;

/*
** message2enum
** --------- */
typedef enum {
    BAD_MSG_ENUM = -1,
    SAM_CANCEL = 0,
    SAM_VERSION,
    SAM_ENABLE,
    SAM_DISABLE,
    SAM_GET_REPORT,
    SAM_ALGO_SPECIFIC,
    SAM_BATCH,
    SAM_FLUSH,
    SAM_ALGO_ATTRIB,
    MAX_MSG_NUM,

    SMGR_CANCEL = 0,
    SMGR_VERSION,
    SMGR_REPORT,
    SMGR_ALL_INFO,
    SMGR_SNGL_INFO,
    SMGR_SELFTEST,
    SMGR_BUFFERED,
    SMGR_BUF_QUERY,
} sl_msg_type_e;

/*
** request
** ---- */
typedef struct request {
    sensor1_msg_header_s header;
    void  *payload;
} request;


typedef struct qualifier {
    char    *node;                              // getter: name of array
    int32_t index;                              // gettar: array element's index
} qualifier;

/*
** service_entry
** ---------- */

typedef void (*new_req)( request *req_msg, array *field_list);
typedef void (*set_req)( request *req_msg, token *t_keyvalue);
typedef void (*get_func)( respind *p_respind, token *t_field, token *t_value);

typedef struct service_entry {
    char        *name;                          // service name
    new_req     new_req_func;                   // new request creator function
    set_req     set_req_func;                   // request field setter
    get_func    getter_func;                    // getter
    request     req_msg[ MAX_MSG_NUM];
} service_entry;


extern service_entry   service_toc[];
void slnew_req( const array *parmlist, token *retval);
void slset_req( const array *parmlist, token *retval);

void InitServices( void);
void GravInit( void);
void GrvInit( void);
void OrntInit( void);
void PedInit( void);
void QmdInit( void);
void RegInit( void);
void RotvInit( void);
void SmgrInit( void);
void TimeInit( void);

#endif // SLNEW_H




