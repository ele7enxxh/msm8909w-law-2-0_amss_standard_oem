/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: qmgr.h
** DESC: sensor language - slang - generic queue manager
** Revision History
** 09-24-2015       df  Eliminate extraneous features
** ================================================================ */
#ifndef QMGR_H
#define QMGR_H

#include <pthread.h>
#include <stdint.h>

typedef   struct queue {
   struct queue  *pforward;         /* forward pointer also the head pointer */
   struct queue  *pbackward;        /* backward pointer also the tail pointer */
}  queue;

typedef  struct qhead {
   pthread_mutex_t  my_mutex;
   queue            qlw;
}  qhead;

int OpenQ( qhead *qh);
int CloseQ( qhead *qh);
int PutAtTail( qhead *qh,  queue  *pe);
int GetFromHead( qhead *qh,  queue  **pe);

int SDOpenSem( pthread_mutex_t *mutex );
int SDLockSem( pthread_mutex_t *mutex );
int SDUnLockSem( pthread_mutex_t *mutex );
int SDCloseSem( pthread_mutex_t *mutex );
#endif

