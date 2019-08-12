/******************************************************************************
  @file    ActiveRequestList.cpp
  @brief   Implementation of active request lists

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <cstdio>
#include <cstring>

#define LOG_TAG "ANDR-PERF-ACTIVEREQLIST"
#include <cutils/log.h>
#include <stdlib.h>

#include "Request.h"
#include "ActiveRequestList.h"
#include "MpctlUtils.h"

void ActiveRequestList::Reset() {
    RequestListNode *current = NULL;
    RequestListNode *next = NULL;

    current = mActiveListHead;

    if (NULL == current) {
        QLOGI("No activity present");
        return;
    }

    while (NULL != current) {
        next = current->mNext;
        if (NULL != current->mHandle) {
            delete current->mHandle;
        }
        free(current);
        current = next;
    }
    mActiveListHead = NULL;
    mActiveReqs = 0;
}

/* Always add new request to head of active list */
int ActiveRequestList::Add(Request *req, int req_handle) {
    RequestListNode *newnode;

    newnode = (struct RequestListNode *)calloc(1, sizeof(struct RequestListNode));
    if (NULL == newnode) {
        QLOGE("Failed to create active request");
        return FAILED;
    } else {
        newnode->mHandle = req;
        newnode->mRetHandle = req_handle;
        if (mActiveListHead == NULL) {
            newnode->mNext = NULL;
        } else {
            newnode->mNext = mActiveListHead;
        }
        mActiveListHead = newnode;
    }
    mActiveReqs++;
    return SUCCESS;
}

void ActiveRequestList::Remove(Request *req) {
    struct RequestListNode *del = NULL;
    struct RequestListNode *iter = NULL;

    if (mActiveListHead == NULL) {
        QLOGI("No activity to remove");
        return;
    }

    if (mActiveListHead->mHandle == req) {
        del = mActiveListHead;
        mActiveListHead = mActiveListHead->mNext;
    } else {
        iter = mActiveListHead;
        while ((iter->mNext != NULL) && (iter->mNext->mHandle != req)) {
            iter = iter->mNext;
        }
        del = iter->mNext;
        if (del != NULL)
            iter->mNext = del->mNext;
    }

    /* Free list_node */
    if (del) {
        mActiveReqs--;
        free(del);
    }
}

Request * ActiveRequestList::DoesExists(int handle) {
    RequestListNode *iter;
    iter = mActiveListHead;

    if (iter == NULL || handle < 1) {
        return NULL;
    }

    while ((iter->mNext != NULL) && (iter->mRetHandle != handle)) {
        iter = iter->mNext;
    }
    if (iter->mRetHandle == handle) {
        return iter->mHandle;
    }
    return NULL;
}

/* Search for request in active list */
Request * ActiveRequestList::IsActive( int handle, Request *req) {
    int i;
    RequestListNode *iter;
    iter = mActiveListHead;

    if ((iter == NULL) || (handle < 1) || (NULL == iter->mHandle) ||
        (NULL == req) || (req->GetNumLocks() < 0)) {
        return NULL;
    }

    while ((iter->mNext != NULL) && (iter->mRetHandle != handle)) {
        iter = iter->mNext;
    }
    if (iter->mRetHandle == handle) {
        if (*req != *(iter->mHandle)) {
            return NULL;
        }
        return iter->mHandle;
    }
    return NULL;
}

int ActiveRequestList::GetReturnHandle(RequestListNode *node) {
    int ret = 0;
    if (NULL != node) {
        ret = node->mRetHandle;
    }
    return ret;
}

Request *ActiveRequestList::GetRequestHandle(RequestListNode *node) {
    Request *request = NULL;
    if (NULL != node) {
        request = node->mHandle;
    }
    return request;
}

void ActiveRequestList::StartIterator() {
    mIter = mActiveListHead;
}

RequestListNode *ActiveRequestList::GetNext() {
    RequestListNode *tmp = mIter;
    if (NULL != mIter) {
        mIter = mIter->mNext;
    }
    return tmp;
}
