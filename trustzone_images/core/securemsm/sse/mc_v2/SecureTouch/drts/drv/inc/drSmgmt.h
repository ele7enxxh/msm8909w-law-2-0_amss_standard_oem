/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#ifndef __DRSMGMT_H__
#define __DRSMGMT_H__

#include "DrApi/DrApiThread.h"
#include "DrApi/DrApiMm.h"

#include "drApiMarshal.h"
#include "drCommon.h"


/**
 * Driver session management error codes
 */
typedef enum {
    E_DR_SMGMT_OK        = 0, /** Success */
    E_DR_SMGMT_INVALID   = 1, /** Invalid parameter */
    /* ... add more error codes when required */
} drSmgmtError_t;

/**
 * Maximum driver session number
 * TODO: update maximum session number accordingly
 */
#define MAX_DR_SESSIONS  1


/**
 * Session state
 */
typedef enum {
    SESSION_STATE_UNUSED          = 0, /** Session object is not inuse */
    SESSION_STATE_ACTIVE          = 1, /** Session is active */
} sessionState_t;

/**
 * Callback used to close a session
 */
typedef void (*drSessionClose_f)(void *);

/**
 * Session registry
 * TODO: update maximum session number accordingly
 */
typedef struct {
    sessionState_t      state;    /** Session state */
    threadid_t          threadid; /** threadid initiated the session */
    drMarshalingData_t  mData;    /** Marshalling data */
    uint32_t            tsInUse;  /** This session is using the Touch Controller */
    drSessionClose_f    close;
} drSessionReg_t, *drSessionReg_ptr;

/** Driver session context */
typedef struct {
    /* Session registry array */
    drSessionReg_t    sessionReg[MAX_DR_SESSIONS];
} drSessionCtx_t, *drSessionCtx_ptr;

/**
 * Open session and initializes data
 *
 * @param threadid  caller thread identifier
 *
 * @retval session id or DR_SID_INVALID
 */
uint32_t drSmgmtOpenSession( threadid_t threadid, drSessionClose_f close );

/**
 * Close session based on session id
 *
 * @param sid  session id
 *
 * @retval none
 */
void drSmgmtCloseSession( uint32_t sid );


/**
 * Close session opened by specific thread
 *
 * @param threadid  caller thread identifier
 *
 * @retval none
 */
void drSmgmtCloseSessionForThread( threadid_t threadid );


/**
 * Retrieves session data based on session id
 *
 * @param sid  session id
 *
 * @retval pointer to the session data
 */
drSessionReg_ptr drSmgmtGetSessionData( uint32_t sid );


/**
 * Set session data
 *
 * @param sid  session id
 * @param threadid  caller thread identifier
 * @param pData  pointer to data
 *
 * @retval E_DR_SMGMT_OK or E_DR_SMGMT_INVALID
 */
drSmgmtError_t drSmgmtSetSessionData(
        uint32_t sid,
        threadid_t threadid,
        drMarshalingData_t *pData);


/**
 * Init all session data
 *
 * @retval none
 */
void drSmgmtInit( void );


/**
 * Closes all active sessions
 *
 * @retval none
 */
void drSmgmtCloseAllSessions( void );


#endif // __DRSMGMT_H__
