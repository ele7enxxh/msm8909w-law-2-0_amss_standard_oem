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

#ifndef CMP_H_
#define CMP_H_

#include "TlCm/cmpCommon.h"

/** Info of the whole mapped memory with NWd. */
typedef struct {
    /** Address of the mapped memory. */
    void* addr;
    /** Size of the mapped memory. */
    uint32_t len;
} cmpMapInfo_t;

/** Info of the mapped element from a CMP message with NWd. */
typedef struct {
    /** Offset of the mapped element in the mapped memory. */
    uint32_t offset;
    /** Size of the mapped element. */
    uint32_t len;
} cmpMapOffsetInfo_t;

/** TCI CMP command header. */
typedef struct {
    /** CMP version. */
    cmpVersionId_t version;
    /** Command id. */
    cmpCommandId_t commandId;
    /** Info of the whole mapped memory with NWd. */
    cmpMapInfo_t mapInfo;
    /** Info of the mapped CMP command with NWd. */
    cmpMapOffsetInfo_t cmpCmdMapOffsetInfo;
} cmpCommandHeaderTci_t;

/** TCI CMP response header. */
typedef struct {
    /** CMP version. */
    cmpVersionId_t version;
    /** Response id (must be command id | RSP_ID_MASK ). */
    cmpResponseId_t responseId;
    /** Required lenght (return 0 if lenght is enough) of the mapped memory
     * with NWd. */
    uint32_t len;
} cmpResponseHeaderTci_t;

/** @defgroup MC_CMP_CMD_GET_VERSION_TCI \
   MC_CMP_CMD_GET_VERSION_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_GET_VERSION_TCI_CMD Command
 * @{ */

/** GetVersion TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdGetVersionTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_GET_VERSION_TCI_RSP Response
 * @{ */

/** GetVersion TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
} cmpRspGetVersionTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_GET_SUID_TCI \
  MC_CMP_CMD_GET_SUID_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_GET_VERSION_TCI_CMD Command
 * @{ */

/** GetSuid TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdGetSuidTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_GET_VERSION_TCI_RSP Response
 * @{ */

/** GetSuid TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
} cmpRspGetSuidTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_GENERATE_AUTH_TOKEN_TCI \
 MC_CMP_CMD_GENERATE_AUTH_TOKEN_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_GENERATE_AUTH_TOKEN_TCI_CMD Command
 * @{ */

/** GenAuthToken TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdGenAuthTokenTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_GENERATE_AUTH_TOKEN_TCI_RSP Response
 * @{ */

/** GenAuthToken TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeaderTci;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of the mapped AuthTokenCont with NWd. */
    cmpMapOffsetInfo_t cmpAuthTokenContMapOffsetInfo;
} cmpRspGenAuthTokenTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_BEGIN_SOC_AUTHENTICATION_TCI \
  MC_CMP_CMD_BEGIN_SOC_AUTHENTICATION_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_BEGIN_SOC_AUTHENTICATION_TCI_CMD Command
 * @{ */

/** BeginSocAuthentication TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
    /** Offset of the mapped AuthTokenCont with NWd. */
    cmpMapOffsetInfo_t cmpAuthTokenContMapOffsetInfo;
} cmpCmdBeginSocAuthenticationTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_BEGIN_SOC_AUTHENTICATION_TCI_RSP Response
 * @{ */

/** BeginSocAuthentication TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
} cmpRspBeginSocAuthenticationTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION_TCI \
  MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION_TCI_CMD Command
 * @{ */

/** BeginRootAuthentication TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
    /** Offset of the mapped RootCont with NWd. */
    cmpMapOffsetInfo_t cmpRootContMapOffsetInfo;
} cmpCmdBeginRootAuthenticationTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION_TCI_RSP Response
 * @{ */

/** BeginRootAuthentication TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
} cmpRspBeginRootAuthenticationTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_BEGIN_SP_AUTHENTICATION_TCI \
  MC_CMP_CMD_BEGIN_SP_AUTHENTICATION_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_BEGIN_SP_AUTHENTICATION_TCI_CMD Command
 * @{ */

/** BeginSpAuthentication TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
    /** Offset of the mapped RootCont with NWd. */
    cmpMapOffsetInfo_t cmpRootContMapOffsetInfo;
    /** Offset of the mapped SpCont with NWd. */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpCmdBeginSpAuthenticationTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_BEGIN_SP_AUTHENTICATION_TCI_RSP Response
 * @{ */

/** BeginSpAuthentication TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
} cmpRspBeginSpAuthenticationTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_AUTHENTICATE_TCI \
  MC_CMP_CMD_AUTHENTICATE_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_AUTHENTICATE_TCI_CMD Command
 * @{ */

/** Authenticate TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdAuthenticateTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_AUTHENTICATE_TCI_RSP Response
 * @{ */

/** Authenticate TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
} cmpRspAuthenticateTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_AUTHENTICATE_TERMINATE_TCI \
  MC_CMP_CMD_AUTHENTICATE_TERMINATE_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_AUTHENTICATE_TERMINATE_TCI_CMD Command
 * @{ */

/** AuthenticateTerminate TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdAuthenticateTerminateTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_AUTHENTICATE_TERMINATE_TCI_RSP Response
 * @{ */

/** AuthenticateTerminate TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
} cmpRspAuthenticateTerminateTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE_TCI \
  MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE_TCI_CMD Command
 * @{ */

/** RootContRegisterActivate TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdRootContRegisterActivateTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE_TCI_RSP Response
 * @{ */

/** RootContRegisterActivate TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of the mapped RootCont with NWd. */
    cmpMapOffsetInfo_t cmpRootContMapOffsetInfo;
} cmpRspRootContRegisterActivateTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNREGISTER_TCI \
  MC_CMP_CMD_ROOT_CONT_UNREGISTER_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNREGISTER_TCI_CMD Command
 * @{ */

/** RootContUnregister TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdRootContUnregisterTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNREGISTER_TCI_RSP Response
 * @{ */

/** RootContUnregister TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
} cmpRspRootContUnregisterTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_ROOT_CONT_LOCK_BY_ROOT_TCI \
  MC_CMP_CMD_ROOT_CONT_LOCK_BY_ROOT_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_ROOT_CONT_LOCK_BY_ROOT_TCI_CMD Command
 * @{ */

/** RootContLockByRoot TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdRootContLockByRootTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_ROOT_CONT_LOCK_BY_ROOT_TCI_RSP Response
 * @{ */

/** RootContLockByRoot TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of the mapped RootCont with NWd. */
    cmpMapOffsetInfo_t cmpRootContMapOffsetInfo;
} cmpRspRootContLockByRootTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNLOCK_BY_ROOT_TCI \
  MC_CMP_CMD_ROOT_CONT_UNLOCK_BY_ROOT_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNLOCK_BY_ROOT_TCI_CMD Command
 * @{ */

/** RootContUnlockByRoot TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdRootContUnlockByRootTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNLOCK_BY_ROOT_TCI_RSP Response
 * @{ */

/** RootContUnlockByRoot TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of the mapped RootCont with NWd. */
    cmpMapOffsetInfo_t cmpRootContMapOffsetInfo;
} cmpRspRootContUnlockByRootTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_ACTIVATE_TCI \
  MC_CMP_CMD_SP_CONT_REGISTER_ACTIVATE_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_ACTIVATE_TCI_CMD Command
 * @{ */

/** SpContRegisterActivate TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdSpContRegisterActivateTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_ACTIVATE_TCI_RSP Response
 * @{ */

/** SpContRegisterActivate TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of the mapped RootCont with NWd. */
    cmpMapOffsetInfo_t cmpRootContMapOffsetInfo;
    /** Offset of the mapped SpCont with NWd. */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpRspSpContRegisterActivateTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_UNREGISTER_TCI \
  MC_CMP_CMD_SP_CONT_UNREGISTER_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_UNREGISTER_TCI_CMD Command
 * @{ */

/** SpContUnregister TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdSpContUnregisterTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_UNREGISTER_TCI_RSP Response
 * @{ */

/** SpContUnregister TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of the mapped RootCont with NWd. */
    cmpMapOffsetInfo_t cmpRootContMapOffsetInfo;
} cmpRspSpContUnregisterTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_TCI \
  MC_CMP_CMD_SP_CONT_REGISTER_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_TCI_CMD Command
 * @{ */

/** SpContRegister TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdSpContRegisterTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_TCI_RSP Response
 * @{ */

/** SpContRegister TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of the mapped RootCont with NWd. */
    cmpMapOffsetInfo_t cmpRootContMapOffsetInfo;
    /** Offset of the mapped SpCont with NWd. */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpRspSpContRegisterTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_ROOT_TCI \
  MC_CMP_CMD_SP_CONT_LOCK_BY_ROOT_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_ROOT_TCI_CMD Command
 * @{ */

/** SpContLockByRoot TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
    /** Offset of the mapped SpCont with NWd. */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpCmdSpContLockByRootTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_ROOT_TCI_RSP Response
 * @{ */

/** SpContLockByRoot TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of the mapped SpCont with NWd. */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpRspSpContLockByRootTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_ROOT_TCI \
  MC_CMP_CMD_SP_CONT_UNLOCK_BY_ROOT_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_ROOT_TCI_CMD Command
 * @{ */

/** SpContUnlockByRoot TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
    /** Offset of the mapped SpCont with NWd. */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpCmdSpContUnlockByRootTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_ROOT_TCI_RSP Response
 * @{ */

/** SpContUnlockByRoot TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of the mapped SpCont with NWd. */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpRspSpContUnlockByRootTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_ACTIVATE_TCI \
  MC_CMP_CMD_SP_CONT_ACTIVATE_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_ACTIVATE_TCI_CMD Command
 * @{ */

/** SpContActivate TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdSpContActivateTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_ACTIVATE_TCI_RSP Response
 * @{ */

/** SpContActivate TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of the mapped SpCont with NWd. */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpRspSpContActivateTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_SP_TCI \
  MC_CMP_CMD_SP_CONT_LOCK_BY_SP_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_SP_TCI_CMD Command
 * @{ */

/** SpContLockBySp TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdSpContLockBySpTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_SP_TCI_RSP Response
 * @{ */

/** SpContLockBySp TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of the mapped SpCont with NWd. */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpRspSpContLockBySpTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_SP_TCI \
  MC_CMP_CMD_SP_CONT_UNLOCK_BY_SP_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_SP_TCI_CMD Command
 * @{ */

/** SpContUnlockBySp TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdSpContUnlockBySpTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_SP_TCI_RSP Response
 * @{ */

/** SpContUnlockBySp TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of the mapped SpCont with NWd. */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpRspSpContUnlockBySpTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE_TCI \
  MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE_TCI_CMD Command
 * @{ */

/** TltContRegisterActivate TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdTltContRegisterActivateTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE_TCI_RSP Response
 * @{ */

/** TltContRegisterActivate TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of the mapped SpCont with NWd. */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
    /** Offset of the mapped TltCont with NWd. */
    cmpMapOffsetInfo_t cmpTltContMapOffsetInfo;
} cmpRspTltContRegisterActivateTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_UNREGISTER_TCI \
  MC_CMP_CMD_TLT_CONT_UNREGISTER_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_UNREGISTER_TCI_CMD Command
 * @{ */

/** TltContUnregister TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdTltContUnregisterTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_UNREGISTER_TCI_RSP Response
 * @{ */

/** TltContUnregister TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of the mapped SpCont with NWd. */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
} cmpRspTltContUnregisterTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_TCI \
  MC_CMP_CMD_TLT_CONT_REGISTER_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_TCI_CMD Command
 * @{ */

/** TltContRegister TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
} cmpCmdTltContRegisterTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_TCI_RSP Response
 * @{ */

/** TltContRegister TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of the mapped SpCont with NWd. */
    cmpMapOffsetInfo_t cmpSpContMapOffsetInfo;
    /** Offset of the mapped TltCont with NWd. */
    cmpMapOffsetInfo_t cmpTltContMapOffsetInfo;
} cmpRspTltContRegisterTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_ACTIVATE_TCI \
  MC_CMP_CMD_TLT_CONT_ACTIVATE_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_ACTIVATE_TCI_CMD Command
 * @{ */

/** TltContActivate TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
    /** Offset of the mapped TltCont with NWd. */
    cmpMapOffsetInfo_t cmpTltContMapOffsetInfo;
} cmpCmdTltContActivateTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_ACTIVATE_TCI_RSP Response
 * @{ */

/** TltContActivate TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of the mapped TltCont with NWd. */
    cmpMapOffsetInfo_t cmpTltContMapOffsetInfo;
} cmpRspTltContActivateTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_LOCK_BY_SP_TCI \
  MC_CMP_CMD_TLT_CONT_LOCK_BY_SP_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_LOCK_BY_SP_TCI_CMD Command
 * @{ */

/** TltContLockBySp TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
    /** Offset of the mapped TltCont with NWd. */
    cmpMapOffsetInfo_t cmpTltContMapOffsetInfo;
} cmpCmdTltContLockBySpTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_LOCK_BY_SP_TCI_RSP Response
 * @{ */

/** TltContLockBySp TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of the mapped TltCont with NWd. */
    cmpMapOffsetInfo_t cmpTltContMapOffsetInfo;
} cmpRspTltContLockBySpTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_UNLOCK_BY_SP_TCI \
  MC_CMP_CMD_TLT_CONT_UNLOCK_BY_SP_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_UNLOCK_BY_SP_TCI_CMD Command
 * @{ */

/** TltContUnlockBySp TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
    /** Offset of the mapped TltCont with NWd. */
    cmpMapOffsetInfo_t cmpTltContMapOffsetInfo;
} cmpCmdTltContUnlockBySpTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_UNLOCK_BY_SP_TCI_RSP Response
 * @{ */

/** TltContUnlockBySp TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of the mapped TltCont with NWd. */
    cmpMapOffsetInfo_t cmpTltContMapOffsetInfo;
} cmpRspTltContUnlockBySpTci_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_PERSONALIZE_TCI \
  MC_CMP_CMD_TLT_CONT_PERSONALIZE_TCI
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_PERSONALIZE_TCI_CMD Command
 * @{ */

/** TltContPersonalize TCI command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderTci_t cmdHeader;
    /** Offset of the mapped TltCont with NWd. */
    cmpMapOffsetInfo_t cmpTltContMapOffsetInfo;
} cmpCmdTltContPersonalizeTci_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_PERSONALIZE_TCI_RSP Response
 * @{ */

/** TltContPersonalize TCI response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderTci_t rspHeader;
    /** Offset of the mapped CMP response with NWd. */
    cmpMapOffsetInfo_t cmpRspMapOffsetInfo;
    /** Offset of the mapped PersonalizeData with NWd. */
    cmpMapOffsetInfo_t cmpTltContPersoMapOffsetInfo;
} cmpRspTltContPersonalizeTci_t;

/** @} */

/** @} */

#endif // CMP_H_

/** @} */
