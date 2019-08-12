/* ===================================================================
** Copyright (c) 2015 Qualcomm Technologies, Inc.
** All Rights Reserved.
** Confidential and Proprietary - Qualcomm Technologies, Inc.
**
** FILE: qmgr.c
** DESC: sensor language - slang - generic queue manager
** Revision History
** 09-24-2015       df  Eliminate extraneous features
** ================================================================ */
#include "qmgr.h"
#include <stdio.h>

#define QM_SEMAPHORE_FAILED     8

/**
 * @name      OpenQ
 * @brief     open a thread safe queue
 * @param[io] pointer a queue head
 * @returns   0 == successful or QM_SEMAPHORE_FAILED
 */
int  OpenQ( qhead *pq)
{
    pq->qlw.pforward  = NULL;
    pq->qlw.pbackward = NULL;
    return SDOpenSem( &pq->my_mutex );
}

/**
 * @name      CloseQ
 * @brief     close a thread safe queue
 * @param[io] pointer a queue head
 * @returns   0 == successful or QM_SEMAPHORE_FAILED
 */
int  CloseQ( qhead *pq)
{
    return SDCloseSem( &pq->my_mutex);
}

/**
 * @name      PutAtTail
 * @brief     Places the element at the tail of the queue
 * @param[io] pq - pointer a queue head
 * @param[io] pe - pointer to a queue element
 * @returns   0 == successful or QM_SEMAPHORE_FAILED
 */
int  PutAtTail( qhead *pq, queue *pe)
{
    int rc = SDLockSem( &pq->my_mutex);
    if ( rc == 0) {
        if ( pq->qlw.pbackward != NULL) {
            pq->qlw.pbackward->pforward = pe;
            pe->pbackward = pq->qlw.pbackward;
        }
        else {
            pe->pbackward = NULL;
            pq->qlw.pforward  = pe;
        }
        pq->qlw.pbackward = pe;
        pe->pforward  = NULL;

        rc = SDUnLockSem( &pq->my_mutex);
    }
    return rc;
}

/**
 * @name      GetFromHead
 * @brief     Places the element at the tail of the queue
 * @param[io] pq - pointer a queue head
 * @param[i]  ppe - pointer to pointer
 * @param[o]  ppe - gets NULL or queue element
 * @returns   0 == successful or QM_SEMAPHORE_FAILED
 */
int GetFromHead( qhead *pq, queue **ppe)
{
    int rc = SDLockSem( &pq->my_mutex);
    if ( rc == 0) {
        if ((*ppe = pq->qlw.pforward) != NULL) {
            pq->qlw.pforward = (*ppe)->pforward;
            (*ppe)->pforward = (*ppe)->pbackward = NULL;
        }

        if (pq->qlw.pforward != NULL) {
            pq->qlw.pforward->pbackward = NULL;
        }
        else {
            pq->qlw.pbackward = NULL;
        }

        rc = SDUnLockSem( &pq->my_mutex);
    }
    return rc;
}

int SDOpenSem( pthread_mutex_t *mutex)
{
    int rc = pthread_mutex_init( mutex, NULL);
    if ( rc) {
        rc = QM_SEMAPHORE_FAILED;
    }
    return rc;
}

int SDLockSem( pthread_mutex_t *mutex)
{
    int rc = pthread_mutex_lock( mutex);
    if ( rc) {
        rc = QM_SEMAPHORE_FAILED;
    }
    return rc;
}

int SDUnLockSem( pthread_mutex_t *mutex)
{
    int rc = pthread_mutex_unlock( mutex);
    if ( rc) {
        rc = QM_SEMAPHORE_FAILED;
    }
    return rc;
}

int SDCloseSem( pthread_mutex_t *mutex)
{
    int rc = pthread_mutex_destroy( mutex);
    if ( rc) {
        rc = QM_SEMAPHORE_FAILED;
    }
    return rc;
}
