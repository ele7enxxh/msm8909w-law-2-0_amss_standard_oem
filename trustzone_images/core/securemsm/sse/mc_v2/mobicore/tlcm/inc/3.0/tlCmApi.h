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
#include "TlCm/3.0/cmp.h"
#include "TlCm/3.0/cmpMap.h"

/** TCI CMP messages. */
typedef union {
    /** Command header. */
    cmpCommandHeaderTci_t commandHeader;
    /** Response header. */
    cmpResponseHeaderTci_t responseHeader;

    /** System command GetVersion. */
    cmpCmdGetVersionTci_t cmpCmdGetVersionTci;
    /** System response GetVersion. */
    cmpRspGetVersionTci_t cmpRspGetVersionTci;
    /** System command GetSuid. */
    cmpCmdGetSuidTci_t cmpCmdGetSuidTci;
    /** System response GetSuid. */
    cmpRspGetSuidTci_t cmpRspGetSuidTci;
    /** System command GenAuthToken. */
    cmpCmdGenAuthTokenTci_t cmpCmdGenAuthTokenTci;
    /** System response GenAuthToken. */
    cmpRspGenAuthTokenTci_t cmpRspGenAuthTokenTci;

    /** Authentication command BeginSocAuthentication. */
    cmpCmdBeginSocAuthenticationTci_t cmpCmdBeginSocAuthenticationTci;
    /** Authentication response BeginSocAuthentication. */
    cmpRspBeginSocAuthenticationTci_t cmpRspBeginSocAuthenticationTci;
    /** Authentication command BeginRootAuthentication. */
    cmpCmdBeginRootAuthenticationTci_t cmpCmdBeginRootAuthenticationTci;
    /** Authentication response BeginRootAuthentication. */
    cmpRspBeginRootAuthenticationTci_t cmpRspBeginRootAuthenticationTci;
    /** Authentication command BeginSpAuthentication. */
    cmpCmdBeginSpAuthenticationTci_t cmpCmdBeginSpAuthenticationTci;
    /** Authentication response BeginSpAuthentication. */
    cmpRspBeginSpAuthenticationTci_t cmpRspBeginSpAuthenticationTci;
    /** Authentication command Authenticate. */
    cmpCmdAuthenticateTci_t cmpCmdAuthenticateTci;
    /** Authentication response Authenticate. */
    cmpRspAuthenticateTci_t cmpRspAuthenticateTci;
    /** Authentication command AuthenticateTerminate. */
    cmpCmdAuthenticateTerminateTci_t cmpCmdAuthenticateTerminateTci;
    /** Authentication response AuthenticateTerminate. */
    cmpRspAuthenticateTerminateTci_t cmpRspAuthenticateTerminateTci;

    /** SoC administrative command RootContRegisterActivate. */
    cmpCmdRootContRegisterActivateTci_t cmpCmdRootContRegisterActivateTci;
    /** SoC administrative response RootContRegisterActivate. */
    cmpRspRootContRegisterActivateTci_t cmpRspRootContRegisterActivateTci;

    /** Root administrative command RootContUnregister. */
    cmpCmdRootContUnregisterTci_t cmpCmdRootContUnregisterTci;
    /** Root administrative response RootContUnregister. */
    cmpRspRootContUnregisterTci_t cmpRspRootContUnregisterTci;
    /** Root administrative command RootContLockByRoot. */
    cmpCmdRootContLockByRootTci_t cmpCmdRootContLockByRootTci;
    /** Root administrative response RootContLockByRoot. */
    cmpRspRootContLockByRootTci_t cmpRspRootContLockByRootTci;
    /** Root administrative command RootContUnlockByRoot. */
    cmpCmdRootContUnlockByRootTci_t cmpCmdRootContUnlockByRootTci;
    /** Root administrative command RootContUnlockByRoot. */
    cmpRspRootContUnlockByRootTci_t cmpRspRootContUnlockByRootTci;
    /** Root administrative command SpContRegisterActivate. */
    cmpCmdSpContRegisterActivateTci_t cmpCmdSpContRegisterActivateTci;
    /** Root administrative response SpContRegisterActivate. */
    cmpRspSpContRegisterActivateTci_t cmpRspSpContRegisterActivateTci;
    /** Root administrative command SpContUnregister. */
    cmpCmdSpContUnregisterTci_t cmpCmdSpContUnregisterTci;
    /** Root administrative response SpContUnregister. */
    cmpRspSpContUnregisterTci_t cmpRspSpContUnregisterTci;
    /** Root administrative command SpContRegister. */
    cmpCmdSpContRegisterTci_t cmpCmdSpContRegisterTci;
    /** Root administrative response SpContRegister. */
    cmpRspSpContRegisterTci_t cmpRspSpContRegisterTci;
    /** Root administrative command SpContLockByRoot. */
    cmpCmdSpContLockByRootTci_t cmpCmdSpContLockByRootTci;
    /** Root administrative response SpContLockByRoot. */
    cmpRspSpContLockByRootTci_t cmpRspSpContLockByRootTci;
    /** Root administrative command SpContUnlockByRoot. */
    cmpCmdSpContUnlockByRootTci_t cmpCmdSpContUnlockByRootTci;
    /** Root administrative response SpContUnlockByRoot. */
    cmpRspSpContUnlockByRootTci_t cmpRspSpContUnlockByRootTci;

    /** Sp administrative command SpContActivate. */
    cmpCmdSpContActivateTci_t cmpCmdSpContActivateTci;
    /** Sp administrative response SpContActivate. */
    cmpRspSpContActivateTci_t cmpRspSpContActivateTci;
    /** Sp administrative command SpContLockBySp. */
    cmpCmdSpContLockBySpTci_t cmpCmdSpContLockBySpTci;
    /** Sp administrative response SpContLockBySp. */
    cmpRspSpContLockBySpTci_t cmpRspSpContLockBySpTci;
    /** Sp administrative command SpContUnlockBySp. */
    cmpCmdSpContUnlockBySpTci_t cmpCmdSpContUnlockBySpTci;
    /** Sp administrative command SpContUnlockBySp. */
    cmpRspSpContUnlockBySpTci_t cmpRspSpContUnlockBySpTci;
    /** Sp administrative command TltContRegisterActivate. */
    cmpCmdTltContRegisterActivateTci_t cmpCmdTltContRegisterActivateTci;
    /** Sp administrative response TltContRegisterActivate. */
    cmpRspTltContRegisterActivateTci_t cmpRspTltContRegisterActivateTci;
    /** Sp administrative command TltContUnregister. */
    cmpCmdTltContUnregisterTci_t cmpCmdTltContUnregisterTci;
    /** Sp administrative response TltContUnregister. */
    cmpRspTltContUnregisterTci_t cmpRspTltContUnregisterTci;
    /** Sp administrative command TltContRegister. */
    cmpCmdTltContRegisterTci_t cmpCmdTltContRegisterTci;
    /** Sp administrative command TltContRegister. */
    cmpRspTltContRegisterTci_t cmpRspTltContRegisterTci;
    /** Sp administrative command TltContActivate. */
    cmpCmdTltContActivateTci_t cmpCmdTltContActivateTci;
    /** Sp administrative command TltContActivate. */
    cmpRspTltContActivateTci_t cmpRspTltContActivateTci;
    /** Sp administrative command TltContLockBySp. */
    cmpCmdTltContLockBySpTci_t cmpCmdTltContLockBySpTci;
    /** Sp administrative response TltContLockBySp. */
    cmpRspTltContLockBySpTci_t cmpRspTltContLockBySpTci;
    /** Sp administrative command TltContUnlockBySp. */
    cmpCmdTltContUnlockBySpTci_t cmpCmdTltContUnlockBySpTci;
    /** Sp administrative response TltContUnlockBySp. */
    cmpRspTltContUnlockBySpTci_t cmpRspTltContUnlockBySpTci;
    /** Sp administrative command TltContPersonalize. */
    cmpCmdTltContPersonalizeTci_t cmpCmdTltContPersonalizeTci;
    /** Sp administrative response TltContPersonalize. */
    cmpRspTltContPersonalizeTci_t cmpRspTltContPersonalizeTci;
} cmpMessage_t;

/** TCI CMP. */
typedef struct {
    /** TCI CMP messages. */
    cmpMessage_t msg;
} cmp_t;

#endif // TL_CM_API_H_

/** @} */
