/*
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All rights reserved
 *
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 */

#ifndef TL_CM_API_H_
#define TL_CM_API_H_

#include "TlCm/tlCmApiCommon.h"
#include "TlCm/2.0/cmp.h"

/** TCI CMP 2.0 messages. */
typedef union {
    /** Command header. */
    cmpCommandHeader_t commandHeader;
    /** Response header. */
    cmpResponseHeader_t responseHeader;

    /** System command GetVersion. */
    cmpCmdGetVersion_t cmpCmdGetVersion;
    /** System response GetVersion. */
    cmpRspGetVersion_t cmpRspGetVersion;
    /** System command GetSuid. */
    cmpCmdGetSuid_t cmpCmdGetSuid;
    /** System response GetSuid. */
    cmpRspGetSuid_t cmpRspGetSuid;
    /** System command GenAuthToken. */
    cmpCmdGenAuthToken_t cmpCmdGenAuthToken;
    /** System response GenAuthToken. */
    cmpRspGenAuthToken_t cmpRspGenAuthToken;

    /** Authentication command BeginSocAuthentication. */
    cmpCmdBeginSocAuthentication_t cmpCmdBeginSocAuthentication;
    /** Authentication response BeginSocAuthentication. */
    cmpRspBeginSocAuthentication_t cmpRspBeginSocAuthentication;
    /** Authentication command BeginRootAuthentication. */
    cmpCmdBeginRootAuthentication_t cmpCmdBeginRootAuthentication;
    /** Authentication response BeginRootAuthentication. */
    cmpRspBeginRootAuthentication_t cmpRspBeginRootAuthentication;
    /** Authentication command BeginSpAuthentication. */
    cmpCmdBeginSpAuthentication_t cmpCmdBeginSpAuthentication;
    /** Authentication response BeginSpAuthentication. */
    cmpRspBeginSpAuthentication_t cmpRspBeginSpAuthentication;
    /** Authentication command Authenticate. */
    cmpCmdAuthenticate_t cmpCmdAuthenticate;
    /** Authentication response Authenticate. */
    cmpRspAuthenticate_t cmpRspAuthenticate;
    /** Authentication command AuthenticateTerminate. */
    cmpCmdAuthenticateTerminate_t cmpCmdAuthenticateTerminate;
    /** Authentication response AuthenticateTerminate. */
    cmpRspAuthenticateTerminate_t cmpRspAuthenticateTerminate;

    /** SoC administrative command RootContRegisterActivate. */
    cmpCmdRootContRegisterActivate_t cmpCmdRootContRegisterActivate;
    /** SoC administrative response RootContRegisterActivate. */
    cmpRspRootContRegisterActivate_t cmpRspRootContRegisterActivate;

    /** Root administrative command RootContUnregister. */
    cmpCmdRootContUnregister_t cmpCmdRootContUnregister;
    /** Root administrative response RootContUnregister. */
    cmpRspRootContUnregister_t cmpRspRootContUnregister;
    /** Root administrative command RootContLockByRoot. */
    cmpCmdRootContLockByRoot_t cmpCmdRootContLockByRoot;
    /** Root administrative response RootContLockByRoot. */
    cmpRspRootContLockByRoot_t cmpRspRootContLockByRoot;
    /** Root administrative command RootContUnlockByRoot. */
    cmpCmdRootContUnlockByRoot_t cmpCmdRootContUnlockByRoot;
    /** Root administrative command RootContUnlockByRoot. */
    cmpRspRootContUnlockByRoot_t cmpRspRootContUnlockByRoot;
    /** Root administrative command SpContRegisterActivate. */
    cmpCmdSpContRegisterActivate_t cmpCmdSpContRegisterActivate;
    /** Root administrative response SpContRegisterActivate. */
    cmpRspSpContRegisterActivate_t cmpRspSpContRegisterActivate;
    /** Root administrative command SpContUnregister. */
    cmpCmdSpContUnregister_t cmpCmdSpContUnregister;
    /** Root administrative response SpContUnregister. */
    cmpRspSpContUnregister_t cmpRspSpContUnregister;
    /** Root administrative command SpContRegister. */
    cmpCmdSpContRegister_t cmpCmdSpContRegister;
    /** Root administrative response SpContRegister. */
    cmpRspSpContRegister_t cmpRspSpContRegister;
    /** Root administrative command SpContLockByRoot. */
    cmpCmdSpContLockByRoot_t cmpCmdSpContLockByRoot;
    /** Root administrative response SpContLockByRoot. */
    cmpRspSpContLockByRoot_t cmpRspSpContLockByRoot;
    /** Root administrative command SpContUnlockByRoot. */
    cmpCmdSpContUnlockByRoot_t cmpCmdSpContUnlockByRoot;
    /** Root administrative response SpContUnlockByRoot. */
    cmpRspSpContUnlockByRoot_t cmpRspSpContUnlockByRoot;

    /** Sp administrative command SpContActivate. */
    cmpCmdSpContActivate_t cmpCmdSpContActivate;
    /** Sp administrative response SpContActivate. */
    cmpRspSpContActivate_t cmpRspSpContActivate;
    /** Sp administrative command SpContLockBySp. */
    cmpCmdSpContLockBySp_t cmpCmdSpContLockBySp;
    /** Sp administrative response SpContLockBySp. */
    cmpRspSpContLockBySp_t cmpRspSpContLockBySp;
    /** Sp administrative command SpContUnlockBySp. */
    cmpCmdSpContUnlockBySp_t cmpCmdSpContUnlockBySp;
    /** Sp administrative command SpContUnlockBySp. */
    cmpRspSpContUnlockBySp_t cmpRspSpContUnlockBySp;
    /** Sp administrative command TltContRegisterActivate. */
    cmpCmdTltContRegisterActivate_t cmpCmdTltContRegisterActivate;
    /** Sp administrative response TltContRegisterActivate. */
    cmpRspTltContRegisterActivate_t cmpRspTltContRegisterActivate;
    /** Sp administrative command TltContUnregister. */
    cmpCmdTltContUnregister_t cmpCmdTltContUnregister;
    /** Sp administrative response TltContUnregister. */
    cmpRspTltContUnregister_t cmpRspTltContUnregister;
    /** Sp administrative command TltContRegister. */
    cmpCmdTltContRegister_t cmpCmdTltContRegister;
    /** Sp administrative response TltContRegister. */
    cmpRspTltContRegister_t cmpRspTltContRegister;
    /** Sp administrative command TltContActivate. */
    cmpCmdTltContActivate_t cmpCmdTltContActivate;
    /** Sp administrative response TltContActivate. */
    cmpRspTltContActivate_t cmpRspTltContActivate;
    /** Sp administrative command TltContLockBySp. */
    cmpCmdTltContLockBySp_t cmpCmdTltContLockBySp;
    /** Sp administrative response TltContLockBySp. */
    cmpRspTltContLockBySp_t cmpRspTltContLockBySp;
    /** Sp administrative command TltContUnlockBySp. */
    cmpCmdTltContUnlockBySp_t cmpCmdTltContUnlockBySp;
    /** Sp administrative response TltContUnlockBySp. */
    cmpRspTltContUnlockBySp_t cmpRspTltContUnlockBySp;
    /** Sp administrative command TltContPersonalize. */
    cmpCmdTltContPersonalize_t cmpCmdTltContPersonalize;
    /** Sp administrative response TltContPersonalize. */
    cmpRspTltContPersonalize_t cmpRspTltContPersonalize;
} cmpMessage_t;

/** TCI CMP 2.0. */
typedef struct {
    /** TCI CMP 2.0 messages. */
    cmpMessage_t msg;
} cmp_t;

#endif // TL_CM_API_H_

/** @} */
