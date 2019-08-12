/******************************************************************************
  @file    ActiveRequestList.h
  @brief   Implementation of active request lists

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __ACTIVEREQUESTLIST__H_
#define __ACTIVEREQUESTLIST__H_

class Request;

typedef struct RequestListNode {
    int mRetHandle;
    Request *mHandle;
    RequestListNode *mNext;
}RequestListNode;

class ActiveRequestList {
    private:
        RequestListNode *mActiveListHead;
        RequestListNode *mIter;
        int mActiveReqs;

    public:
        static const int ACTIVE_REQS_MAX = 30;

        ActiveRequestList() {
            mActiveListHead = NULL;
            mIter = NULL;
            mActiveReqs = 0;
        };
        ~ActiveRequestList() {
        };
        int Add(Request *req, int req_handle);
        void Remove(Request *req);
        void Reset();
        Request * DoesExists(int handle);
        Request * IsActive( int handle, Request *req);

        int GetReturnHandle(RequestListNode *node);
        Request *GetRequestHandle(RequestListNode *node);

        int GetNumActiveRequests() {
            return mActiveReqs;
        }

        void StartIterator();
        RequestListNode *GetNext();
};

#endif
