/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * core_internal.h
 *
 *  Created on: Mar 23, 2012
 *      Author: yrusakov
 */

#ifndef CORE_INTERNAL_H_
#define CORE_INTERNAL_H_

#include "adsppm.h"

/**
*@typedef
*@struct AdsppmCBData - call back data for call back function
*/

typedef struct AdsppmCBData {

    int                         eventId;     // callback type
    int                         adsppmId;    // client id
    struct AdsppmCBData           *pNext;
} AdsppmCBDataType;



/**
*@define MAX_EVENT_NUM - max event num for callback
*/

#define MAX_EVENT_NUM     32

/** 
*@typedef
*@struct AdsppmCtxType  - ADSPPM Global context to hold global information for adsppm.
*/

typedef struct {
    //number of active clients
    uint32                      numOfRegisteredClients;
	//clientNum used to assign clientId
	uint32                      clientIdNum;
    DALSYSSyncHandle            adsppmCtxLock;
    DALSYSSyncHandle            RMCtxLock[Adsppm_Rsc_Id_Max-1];
    coreUtils_Q_Type            AdsppmHandleQ;
    coreUtils_Q_Type            AdsppmReqQArray[Adsppm_Rsc_Id_Max];
    //callback event queue, headEvt and tailEvt points to the head and tail in the eventQue
    AdsppmCBDataType            cbEventQue[MAX_EVENT_NUM];
    //signal event for callback
    DALSYSEventHandle           signalCbEvent;
    uint32                      periodicUseCase;//!< Indicates if the system running only periodic clients (1) or there are non-periodic clients as well (0)
} AdsppmCtxType;


/**
 * @fn GetGlobalContext - retrieves pointer to the global ADSPPM context. Inline
 * @return pointer to the global ADSPPM context
 */
extern inline AdsppmCtxType* GetGlobalContext(void);

/**
 * @fn findClientHandle - find the client handle in linklist
 */
int findClientHandle(AdsppmHandleType *pLHandleItem,  uint32 *clientId);

/**
 * Get periodic usecase flag
 * @return 1 if only periodic lients are running
 *         0 in all other cases
 */
extern inline uint32 IsPeriodicUseCase(void);

/**
 * Indicate whether the current usecase is periodic
 * @param periodic - 1 if only periodic lients are running
 *                   0 in all other cases
 */
extern inline void SetPeriodicUseCase(uint32 periodic);

#endif /* CORE_INTERNAL_H_ */
