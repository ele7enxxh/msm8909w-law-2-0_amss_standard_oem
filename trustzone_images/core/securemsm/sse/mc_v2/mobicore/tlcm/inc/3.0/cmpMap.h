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

#ifndef CMP_MAP_H_
#define CMP_MAP_H_

#include "TlCm/3.0/cmp.h"

/** Map CMP GetVersion message. */
typedef union {
    /** Map CMP command. */
    cmpCmdGetVersion_t cmpCmdGetVersion;
    /** Map CMP response. */
    cmpRspGetVersion_t cmpRspGetVersion;
} cmpMapGetVersion_t;

/** Map CMP GetSuid message. */
typedef union {
    /** Map CMP command. */
    cmpCmdGetSuid_t cmpCmdGetSuid;
    /** Map CMP response. */
    cmpRspGetSuid_t cmpRspGetSuid;
} cmpMapGetSuid_t;

/** Map CMP GenAuthToken message. */
typedef union {
    /** Map CMP command. */
    cmpCmdGenAuthToken_t cmpCmdGenAuthToken;
    /** Map CMP response. */
    cmpRspGenAuthToken_t cmpRspGenAuthToken;
} cmpMapGenAuthToken_t;

/** Map CMP command header. */
typedef struct {
    /** Command id. */
    cmpCommandId_t commandId;
    /** Size of command. */
    uint32_t len;
    /** Reserved. */
    uint32_t reserved;
} cmpCommandHeaderMap_t;

/** Map CMP response header. */
typedef struct {
    /** Return id. */
    cmpResponseId_t responseId;
    /** Return code. */
    cmpReturnCode_t returnCode;
    /** Size of response. */
    uint32_t len;
    /** Reserved. */
    uint32_t reserved;
} cmpResponseHeaderMap_t;

/** Map CMP message header. */
typedef union {
    /** Map CMP command. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Map CMP response. */
    cmpResponseHeaderMap_t rspHeader;
} cmpMapHeader_t;

/** CMP error signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Signature. */
    cmpMac_t mac;
} cmpErrorRsp_t;

/** Map CMP error response. */
typedef struct {
    /** Response. */
    cmpErrorRsp_t rsp;
} cmpMapError_t;

/** @defgroup MC_CMP_CMD_BEGIN_SOC_AUTHENTICATION \
  MC_CMP_CMD_BEGIN_SOC_AUTHENTICATION
 * @{ */

/** @defgroup MC_CMP_CMD_BEGIN_SOC_AUTHENTICATION_CMD Command
 * @{ */

/** BeginSocAuthentication non signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
} cmpBeginSocAuthenticationCmd_t;

/** BeginSocAuthentication command. */
typedef struct {
    /** Command. */
    cmpBeginSocAuthenticationCmd_t cmd;
} cmpCmdBeginSocAuthentication_t;

/** @} */

/** @defgroup MC_CMP_CMD_BEGIN_SOC_AUTHENTICATION_RSP Response
 * @{ */

/** BeginSocAuthentication signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Suid. */
    mcSuid_t suid;
    /** SWd authentication random number. */
    cmpRnd8_t rnd1;
} cmpBeginSocAuthenticationRspSdata_t;

/** BeginSocAuthentication signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpBeginSocAuthenticationRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpBeginSocAuthenticationRsp_t;

/** BeginSocAuthentication response. */
typedef struct {
    /** Response. */
    cmpBeginSocAuthenticationRsp_t rsp;
} cmpRspBeginSocAuthentication_t;

/** @} */

/** Map CMP BeginSocAuthentication message. */
typedef union {
    /** Map CMP command. */
    cmpCmdBeginSocAuthentication_t cmpCmdBeginSocAuthentication;
    /** Map CMP response. */
    cmpRspBeginSocAuthentication_t cmpRspBeginSocAuthentication;
} cmpMapBeginSocAuthentication_t;

/** @} */

/** @defgroup MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION \
  MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION
 * @{ */

/** @defgroup MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION_CMD Command
 * @{ */

/** BeginRootAuthentication non signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
} cmpBeginRootAuthenticationCmd_t;

/** BeginRootAuthentication command. */
typedef struct {
    /** Command. */
    cmpBeginRootAuthenticationCmd_t cmd;
} cmpCmdBeginRootAuthentication_t;

/** @} */

/** @defgroup MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION_RSP Response
 * @{ */

/** BeginRootAuthentication signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Suid. */
    mcSuid_t suid;
    /** SWd authentication random number. */
    cmpRnd8_t rnd1;
} cmpBeginRootAuthenticationRspSdata_t;

/** BeginRootAuthentication signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpBeginRootAuthenticationRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpBeginRootAuthenticationRsp_t;

/** BeginRootAuthentication response. */
typedef struct {
    /** Response. */
    cmpBeginRootAuthenticationRsp_t rsp;
} cmpRspBeginRootAuthentication_t;

/** @} */

/** Map CMP BeginRootAuthentication message. */
typedef union {
    /** Map CMP command. */
    cmpCmdBeginRootAuthentication_t cmpCmdBeginRootAuthentication;
    /** Map CMP response. */
    cmpRspBeginRootAuthentication_t cmpRspBeginRootAuthentication;
} cmpMapBeginRootAuthentication_t;

/** @} */

/** @defgroup MC_CMP_CMD_BEGIN_SP_AUTHENTICATION \
  MC_CMP_CMD_BEGIN_SP_AUTHENTICATION
 * @{ */

/** @defgroup MC_CMP_CMD_BEGIN_SP_AUTHENTICATION_CMD Command
 * @{ */

/** BeginSpAuthentication non signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
} cmpBeginSpAuthenticationCmd_t;

/** BeginSpAuthentication command. */
typedef struct {
    /** Command. */
    cmpBeginSpAuthenticationCmd_t cmd;
} cmpCmdBeginSpAuthentication_t;

/** @} */

/** @defgroup MC_CMP_CMD_BEGIN_SP_AUTHENTICATION_RSP Response
 * @{ */

/** BeginSpAuthentication signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Suid. */
    mcSuid_t suid;
    /** Spid. */
    mcSpid_t spid;
    /** SWd authentication random number. */
    cmpRnd8_t rnd1;
} cmpBeginSpAuthenticationRspSdata_t;

/** BeginSpAuthentication signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpBeginSpAuthenticationRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpBeginSpAuthenticationRsp_t;

/** BeginSpAuthentication response. */
typedef struct {
    /** Response. */
    cmpBeginSpAuthenticationRsp_t rsp;
} cmpRspBeginSpAuthentication_t;

/** @} */

/** Map CMP BeginSpAuthentication message. */
typedef union {
    /** Map CMP command. */
    cmpCmdBeginSpAuthentication_t cmpCmdBeginSpAuthentication;
    /** Map CMP response. */
    cmpRspBeginSpAuthentication_t cmpRspBeginSpAuthentication;
} cmpMapBeginSpAuthentication_t;

/** @} */

/** @defgroup MC_CMP_CMD_AUTHENTICATE MC_CMP_CMD_AUTHENTICATE
 * @{ */

/** @defgroup MC_CMP_CMD_AUTHENTICATE_CMD Command
 * @{ */

/** Authenticate encrypted data command. */
typedef struct {
    /** NWd authentication random number. */
    cmpRnd8_t rnd2;
    /** Suid. */
    mcSuid_t suid;
    /** EntityId. */
    uint32_t entityId;
    /** SWd authentication random number. */
    cmpRnd8_t rnd1;
    /** NWd authentication random seed. */
    cmpRnd32_t k2;
} cmpAuthMsgEdata_t;

/** Authenticate encrypted and padded data command. */
typedef struct {
    /** Encrypted data. */
    cmpAuthMsgEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpAuthMsgEdata_t))];
} cmpAuthCmdEd_t;

/** Authenticate signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Encrypted and padded data size. */
    uint32_t edLen;
    /** Encrypted and padded data. */
    cmpAuthCmdEd_t ed;
} cmpAuthCmdSdata_t;

/** Authenticate signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpAuthCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpAuthenticateCmd_t;

/** Authenticate command. */
typedef struct {
    /** Command. */
    cmpAuthenticateCmd_t cmd;
} cmpCmdAuthenticate_t;

/** @} */

/** @defgroup MC_CMP_CMD_AUTHENTICATE_RSP Response
 * @{ */

/** Authenticate encrypted data response. */
typedef struct {
    /** NWd authentication random number. */
    cmpRnd8_t rnd2;
    /** Suid. */
    mcSuid_t suid;
    /** EntityId. */
    uint32_t entityId;
    /** SWd authentication random number. */
    cmpRnd8_t rnd1;
    /** SWd authentication random seed. */
    cmpRnd32_t k1;
} cmpAuthRspEdata_t;

/** Authenticate encrypted and padded data response. */
typedef struct {
    /** Encrypted data. */
    cmpAuthRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpAuthRspEdata_t))];
} cmpAuthRspEd_t;

/** Authenticate signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Encrypted and padded data size. */
    uint32_t edLen;
    /** Encrypted and padded data. */
    cmpAuthRspEd_t ed;
} cmpAuthRspSdata_t;

/** Authenticate signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpAuthRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpAuthenticateRsp_t;

/** Authenticate response. */
typedef struct {
    /** Response. */
    cmpAuthenticateRsp_t rsp;
} cmpRspAuthenticate_t;

/** @} */

/** Map CMP Authenticate message. */
typedef union {
    /** Map CMP command. */
    cmpCmdAuthenticate_t cmpCmdAuthenticate;
    /** Map CMP response. */
    cmpRspAuthenticate_t cmpRspAuthenticate;
} cmpMapAuthenticate_t;

/** @} */

/** @defgroup MC_CMP_CMD_AUTHENTICATE_TERMINATE \
  MC_CMP_CMD_AUTHENTICATE_TERMINATE
 * @{ */

/** @defgroup MC_CMP_CMD_AUTHENTICATE_TERMINATE_CMD Command
 * @{ */

/** AuthenticateTerminate signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
} cmpAuthenticateTerminateCmdSdata_t;

/** AuthenticateTerminate signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpAuthenticateTerminateCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpAuthenticateTerminateCmd_t;

/** AuthenticateTerminate command. */
typedef struct {
    /** Command. */
    cmpAuthenticateTerminateCmd_t cmd;
} cmpCmdAuthenticateTerminate_t;

/** @} */

/** @defgroup MC_CMP_CMD_AUTHENTICATE_TERMINATE_RSP Response
 * @{ */

/** AuthenticateTerminate signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
} cmpAuthenticateTerminateRspSdata_t;

/** AuthenticateTerminate signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpAuthenticateTerminateRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpAutenticateTerminateRsp_t;

/** AuthenticateTerminate response. */
typedef struct {
    /** Response. */
    cmpAutenticateTerminateRsp_t rsp;
} cmpRspAuthenticateTerminate_t;

/** @} */

/** Map CMP AuthenticateTerminate message. */
typedef union {
    /** Map CMP command. */
    cmpCmdAuthenticateTerminate_t cmpCmdAuthenticateTerminate;
    /** Map CMP response. */
    cmpRspAuthenticateTerminate_t cmpRspAuthenticateTerminate;
} cmpMapAuthenticateTerminate_t;

/** @} */

/** @defgroup MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE \
  MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE
 * @{ */

/** @defgroup MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE_CMD Command
 * @{ */

/** RootContRegisterActivate encrypted data command. */
typedef struct {
    /** Root authentication key. */
    mcSymmetricKey_t kRootAuth;
} cmpRootRegActMsgEdata_t;

/** RootContRegisterActivate encrypted and padded data command. */
typedef struct {
    /** Encrypted data. */
    cmpRootRegActMsgEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpRootRegActMsgEdata_t))];
} cmpRootRegActCmdEd_t;

/** RootContRegisterActivate signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Rootid. */
    mcRootid_t rootid;
    /** Encrypted and padded data size. */
    uint32_t edLen;
    /** Encrypted and padded data. */
    cmpRootRegActCmdEd_t ed;
} cmpRootRegActCmdSdata_t;

/** RootContRegisterActivate signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpRootRegActCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpRootContRegisterActivateCmd_t;

/** RootContRegisterActivate command. */
typedef struct {
    /** Command. */
    cmpRootContRegisterActivateCmd_t cmd;
} cmpCmdRootContRegisterActivate_t;

/** @} */

/** @defgroup MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE_RSP Response
 * @{ */

/** RootContRegisterActivate signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Root container secure object size. */
    uint32_t soRootContLen;
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
} cmpRootRegActRspSdata_t;

/** RootContRegisterActivate signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpRootRegActRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpRootContRegisterActivateRsp_t;

/** RooContRegisterActivate response. */
typedef struct {
    /** Response. */
    cmpRootContRegisterActivateRsp_t rsp;
} cmpRspRootContRegisterActivate_t;

/** @} */

/** Map CMP RootContRegisterActivate message. */
typedef union {
    /** Map CMP command. */
    cmpCmdRootContRegisterActivate_t cmpCmdRootContRegisterActivate;
    /** Map CMP response. */
    cmpRspRootContRegisterActivate_t cmpRspRootContRegisterActivate;
} cmpMapRootContRegisterActivate_t;

/** @} */

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNREGISTER \
  MC_CMP_CMD_ROOT_CONT_UNREGISTER
 * @{ */

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNREGISTER_CMD Command
 * @{ */

/** RootContUnregister encrypted data command. */
typedef struct {
    /** Suid. */
    mcSuid_t suid;
} cmpRootUnregMsgEdata_t;

/** RootContUnregister encrypted and padded data command. */
typedef struct {
    /** Encrypted data. */
    cmpRootUnregMsgEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpRootUnregMsgEdata_t))];
} cmpRootUnregCmdEd_t;

/** RootContUnregister signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Encrypted and padded data size. */
    uint32_t edLen;
    /** Encrypted and padded data. */
    cmpRootUnregCmdEd_t ed;
} cmpRootUnregCmdSdata_t;

/** RootContUnregister signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpRootUnregCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpRootContUnregisterCmd_t;

/** RootContUnregister command. */
typedef struct {
    /** Command. */
    cmpRootContUnregisterCmd_t cmd;
} cmpCmdRootContUnregister_t;

/** @} */

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNREGISTER_RSP Response
 * @{ */

/** RootContUnregister encrypted data response. */
typedef struct {
    /** Suid. */
    mcSuid_t suid;
} cmpRootUnregRspEdata_t;

/** RootContUnregister encrypted and padded data response. */
typedef struct {
    /** Encrypted data. */
    cmpRootUnregRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpRootUnregRspEdata_t))];
} cmpRootUnregRspEd_t;

/** RootContUnregister signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Encrypted and padded data size. */
    uint32_t edLen;
    /** Encrypted and padded data. */
    cmpRootUnregRspEd_t ed;
} cmpRootUnregRspSdata_t;

/** RootContUnregister signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpRootUnregRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpRootContUnregisterRsp_t;

/** RootContUnregister response. */
typedef struct {
    /** Response. */
    cmpRootContUnregisterRsp_t rsp;
} cmpRspRootContUnregister_t;

/** @} */

/** Map CMP RootContUnregister message. */
typedef union {
    /** Map CMP command. */
    cmpCmdRootContUnregister_t cmpCmdRootContUnregister;
    /** Map CMP response. */
    cmpRspRootContUnregister_t cmpRspRootContUnregister;
} cmpMapRootContUnregister_t;

/** @} */

/** @defgroup MC_CMP_CMD_ROOT_CONT_LOCK_BY_ROOT \
  MC_CMP_CMD_ROOT_CONT_LOCK_BY_ROOT
 * @{ */

/** @defgroup MC_CMP_CMD_ROOT_CONT_LOCK_BY_ROOT_CMD Command
 * @{ */

/** RootContLockByRoot signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
} cmpRootLockByRootCmdSdata_t;

/** RootContLockByRoot signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpRootLockByRootCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpRootContLockByRootCmd_t;

/** RootContLockByRoot command. */
typedef struct {
    /** Command. */
    cmpRootContLockByRootCmd_t cmd;
} cmpCmdRootContLockByRoot_t;

/** @} */

/** @defgroup MC_CMP_CMD_ROOT_CONT_LOCK_BY_ROOT_RSP Response
 * @{ */

/** RootContLockByRoot signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Root container secure object size. */
    uint32_t soRootContLen;
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
} cmpRootLockByRootRspSdata_t;

/** RootContLockByRoot signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpRootLockByRootRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpRootContLockByRootRsp_t;

/** RootContLockByRoot response. */
typedef struct {
    cmpRootContLockByRootRsp_t rsp;
} cmpRspRootContLockByRoot_t;

/** @} */

/** Map CMP RootContLockByRoot message. */
typedef union {
    /** Map CMP command. */
    cmpCmdRootContLockByRoot_t cmpCmdRootContLockByRoot;
    /** Map CMP response. */
    cmpRspRootContLockByRoot_t cmpRspRootContLockByRoot;
} cmpMapRootContLockByRoot_t;

/** @} */

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNLOCK_BY_ROOT \
  MC_CMP_CMD_ROOT_CONT_UNLOCK_BY_ROOT
 * @{ */

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNLOCK_BY_ROOT_CMD Command
 * @{ */

/** RootContUnlockByRoot signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
} cmpRootUnlockByRootCmdSdata_t;

/** RootContUnlockByRoot signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpRootUnlockByRootCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpRootContUnlockByRootCmd_t;

/** RootContUnlockByRoot command. */
typedef struct {
    /** Command. */
    cmpRootContUnlockByRootCmd_t cmd;
} cmpCmdRootContUnlockByRoot_t;

/** @} */

/** @defgroup MC_CMP_CMD_ROOT_CONT_UNLOCK_BY_ROOT_RSP Response
 * @{ */

/** RootContUnlockByRoot signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Root container secure object size. */
    uint32_t soRootContLen;
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
} cmpRootUnlockByRootRspSdata_t;

/** RootContUnlockByRoot signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpRootUnlockByRootRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpRootContUnlockByRootRsp_t;

/** RootContUnlockByRoot response. */
typedef struct {
    /** Response. */
    cmpRootContUnlockByRootRsp_t rsp;
} cmpRspRootContUnlockByRoot_t;

/** @} */

/** Map CMP RootContUnlockByRoot message. */
typedef union {
    /** Map CMP command. */
    cmpCmdRootContUnlockByRoot_t cmpCmdRootContUnlockByRoot;
    /** Map CMP response. */
    cmpRspRootContUnlockByRoot_t cmpRspRootContUnlockByRoot;
} cmpMapRootContUnlockByRoot_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_ACTIVATE \
  MC_CMP_CMD_SP_CONT_REGISTER_ACTIVATE
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_ACTIVATE_CMD Command
 * @{ */

/** SpContRegisterActivate encrypted data command. */
typedef struct {
    /** Sp authentication key. */
    mcSymmetricKey_t kSpAuth;
} cmpSpRegActMsgEdata_t;

/** SpContRegisterActivate encrypted and padded data command. */
typedef struct {
    /** Encrypted data. */
    cmpSpRegActMsgEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpSpRegActMsgEdata_t))];
} cmpSpRegActCmdEd_t;

/** SpContRegisterActivate signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
    /** Encrypted and padded data size. */
    uint32_t edLen;
    /** Encrypted and padded data. */
    cmpSpRegActCmdEd_t ed;
} cmpSpRegActCmdSdata_t;

/** SpContRegisterActivate signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpSpRegActCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpSpContRegisterActivateCmd_t;

/** SpContRegisterActivate command. */
typedef struct {
    /** Command. */
    cmpSpContRegisterActivateCmd_t cmd;
} cmpCmdSpContRegisterActivate_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_ACTIVATE_RSP Response
 * @{ */

/** SpContRegisterActivate signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Root container secure object size. */
    uint32_t soRootContLen;
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
    /** Sp container secure object size. */
    uint32_t soSpContLen;
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpSpRegActRspSdata_t;

/** SpContRegisterActivate signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpSpRegActRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpSpContRegisterActivateRsp_t;

/** SpContRegisterActivate response. */
typedef struct {
    /** Response. */
    cmpSpContRegisterActivateRsp_t rsp;
} cmpRspSpContRegisterActivate_t;

/** @} */

/** Map CMP SpContRegisterActivate message. */
typedef union {
    /** Map CMP command. */
    cmpCmdSpContRegisterActivate_t cmpCmdSpContRegisterActivate;
    /** Map CMP response. */
    cmpRspSpContRegisterActivate_t cmpRspSpContRegisterActivate;
} cmpMapSpContRegisterActivate_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_UNREGISTER \
  MC_CMP_CMD_SP_CONT_UNREGISTER
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_UNREGISTER_CMD Command
 * @{ */

/** SpContUnregister signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
} cmpSpContUnregCmdSdata_t;

/** SpContUnregister signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpSpContUnregCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpSpContUnregisterCmd_t;

/** SpContUnregister command. */
typedef struct {
    /** Command. */
    cmpSpContUnregisterCmd_t cmd;
} cmpCmdSpContUnregister_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_UNREGISTER_RSP Response
 * @{ */

/** SpContUnregister signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Root container secure object size. */
    uint32_t soRootContLen;
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
} cmpSpContUnregRspSdata_t;

/** SpContUnregister signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpSpContUnregRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpSpContUnregisterRsp_t;

/** SpContUnregister response. */
typedef struct {
    /** Response. */
    cmpSpContUnregisterRsp_t rsp;
} cmpRspSpContUnregister_t;

/** @} */

/** Map CMP SpContUnregister message. */
typedef union {
    /** Map CMP command. */
    cmpCmdSpContUnregister_t cmpCmdSpContUnregister;
    /** Map CMP response. */
    cmpRspSpContUnregister_t cmpRspSpContUnregister;
} cmpMapSpContUnregister_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER \
  MC_CMP_CMD_SP_CONT_REGISTER
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_CMD Command
 * @{ */

/** SpContRegister encrypted data command. */
typedef struct {
    /** Sp authentication key. */
    mcSymmetricKey_t kSpAuth;
} cmpSpRegisterMsgEdata_t;

/** SpContRegister encrypted and padded data command. */
typedef struct {
    /** Encrypted data. */
    cmpSpRegisterMsgEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpSpRegisterMsgEdata_t))];
} cmpSpRegisterCmdEd_t;

/** SpContRegister signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
    /** Encrypted and padded data size. */
    uint32_t edLen;
    /** Encrypted and padded data. */
    cmpSpRegisterCmdEd_t ed;
} cmpSpRegisterCmdSdata_t;

/** SpContRegister signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpSpRegisterCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpSpContRegisterCmd_t;

/** SpContRegister command. */
typedef struct {
    /** Command. */
    cmpSpContRegisterCmd_t cmd;
} cmpCmdSpContRegister_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_REGISTER_RSP Response
 * @{ */

/** SpContRegister signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Root container secure object size. */
    uint32_t soRootContLen;
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
    /** Sp container secure object size. */
    uint32_t soSpContLen;
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpSpRegisterRspSdata_t;

/** SpContRegister signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpSpRegisterRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpSpContRegisterRsp_t;

/** SpContRegister response. */
typedef struct {
    /** Response. */
    cmpSpContRegisterRsp_t rsp;
} cmpRspSpContRegister_t;

/** @} */

/** Map CMP SpContRegister message. */
typedef union {
    /** Map CMP command. */
    cmpCmdSpContRegister_t cmpCmdSpContRegister;
    /** Map CMP response. */
    cmpRspSpContRegister_t cmpRspSpContRegister;
} cmpMapSpContRegister_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_ROOT \
  MC_CMP_CMD_SP_CONT_LOCK_BY_ROOT
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_ROOT_CMD Command
 * @{ */

/** SpContLockByRoot signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
} cmpSpLockByRootCmdSdata_t;

/** SpContLockByRoot signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpSpLockByRootCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpSpContLockByRootCmd_t;

/** SpContLockByRoot command. */
typedef struct {
    /** Command. */
    cmpSpContLockByRootCmd_t cmd;
} cmpCmdSpContLockByRoot_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_ROOT_RSP Response
 * @{ */

/** SpContLockByRoot signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Sp container secure object size. */
    uint32_t soSpContLen;
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpSpLockByRootRspSdata_t;

/** SpContLockByRoot signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpSpLockByRootRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpSpContLockByRootRsp_t;

/** SpContLockByRoot response. */
typedef struct {
    /** Response. */
    cmpSpContLockByRootRsp_t rsp;
} cmpRspSpContLockByRoot_t;

/** @} */

/** Map CMP SpContLockByRoot message. */
typedef union {
    /** Map CMP command. */
    cmpCmdSpContLockByRoot_t cmpCmdSpContLockByRoot;
    /** Map CMP response. */
    cmpRspSpContLockByRoot_t cmpRspSpContLockByRoot;
} cmpMapSpContLockByRoot_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_ROOT \
  MC_CMP_CMD_SP_CONT_UNLOCK_BY_ROOT
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_ROOT_CMD Command
 * @{ */

/** SpContUnlockByRoot signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
} cmpSpUnlockByRootCmdSdata_t;

/** SpContUnlockByRoot signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpSpUnlockByRootCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpSpContUnlockByRootCmd_t;

/** SpContUnlockByRoot command. */
typedef struct {
    /** Command. */
    cmpSpContUnlockByRootCmd_t cmd;
} cmpCmdSpContUnlockByRoot_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_ROOT_RSP Response
 * @{ */

/** SpContUnlockByRoot signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Sp container secure object size. */
    uint32_t soSpContLen;
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpSpUnlockByRootRspSdata_t;

/** SpContUnlockByRoot signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpSpUnlockByRootRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpSpContUnlockByRootRsp_t;

/** SpContUnlockByRoot response. */
typedef struct {
    /** Response. */
    cmpSpContUnlockByRootRsp_t rsp;
} cmpRspSpContUnlockByRoot_t;

/** @} */

/** Map CMP SpContUnlockByRoot message. */
typedef union {
    /** Map CMP command. */
    cmpCmdSpContUnlockByRoot_t cmpCmdSpContUnlockByRoot;
    /** Map CMP response. */
    cmpRspSpContUnlockByRoot_t cmpRspSpContUnlockByRoot;
} cmpMapSpContUnlockByRoot_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_ACTIVATE \
  MC_CMP_CMD_SP_CONT_ACTIVATE
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_ACTIVATE_CMD Command
 * @{ */

/** SpContActivate encrypted data command. */
typedef struct {
    /** Sp authentication key. */
    mcSymmetricKey_t kSpAuth;
} cmpSpActivateMsgEdata_t;

/** SpContActivate encrypted and padded data command. */
typedef struct {
    /** Encrypted data. */
    cmpSpActivateMsgEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpSpActivateMsgEdata_t))];
} cmpSpActivateCmdEd_t;

/** SpContActivate signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
    /** Encrypted and padded data size. */
    uint32_t edLen;
    /** Encrypted and padded data. */
    cmpSpActivateCmdEd_t ed;
} cmpSpActivateCmdSdata_t;

/** SpContActivate signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpSpActivateCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpSpContActivateCmd_t;

/** SpContActivate command. */
typedef struct {
    /** Command. */
    cmpSpContActivateCmd_t cmd;
} cmpCmdSpContActivate_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_ACTIVATE_RSP Response
 * @{ */

/** SpContActivate signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Sp container secure object size. */
    uint32_t soSpContLen;
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpSpActivateRspSdata_t;

/** SpContActivate signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpSpActivateRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpSpContActivateRsp_t;

/** SpContActivate response. */
typedef struct {
    /** Response. */
    cmpSpContActivateRsp_t rsp;
} cmpRspSpContActivate_t;

/** @} */

/** Map CMP SpContActivate message. */
typedef union {
    /** Map CMP command. */
    cmpCmdSpContActivate_t cmpCmdSpContActivate;
    /** Map CMP response. */
    cmpRspSpContActivate_t cmpRspSpContActivate;
} cmpMapSpContActivate_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_SP \
  MC_CMP_CMD_SP_CONT_LOCK_BY_SP
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_SP_CMD Command
 * @{ */

/** SpContLockBySp signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
} cmpSpLockBySpCmdSdata_t;

/** SpContLockBySp signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpSpLockBySpCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpSpContLockBySpCmd_t;

/** SpContLockBySp command. */
typedef struct {
    /** Command. */
    cmpSpContLockBySpCmd_t cmd;
} cmpCmdSpContLockBySp_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_LOCK_BY_SP_RSP Response
 * @{ */

/** SpContLockBySp signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Sp container secure object size. */
    uint32_t soSpContLen;
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpSpLockBySpRspSdata_t;

/** SpContLockBySp signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpSpLockBySpRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpSpContLockBySpRsp_t;

/** SpContLockBySp response. */
typedef struct {
    /** Response. */
    cmpSpContLockBySpRsp_t rsp;
} cmpRspSpContLockBySp_t;

/** @} */

/** Map CMP SpContLockBySp message. */
typedef union {
    /** Map CMP command. */
    cmpCmdSpContLockBySp_t cmpCmdSpContLockBySp;
    /** Map CMP response. */
    cmpRspSpContLockBySp_t cmpRspSpContLockBySp;
} cmpMapSpContLockBySp_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_SP \
  MC_CMP_CMD_SP_CONT_UNLOCK_BY_SP
 * @{ */

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_SP_CMD Command
 * @{ */

/** SpContUnlockBySp signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
} cmpSpUnlockBySpCmdSdata_t;

/** SpContUnlockBySp signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpSpUnlockBySpCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpSpContUnlockBySpCmd_t;

/** SpContUnlockBySp command. */
typedef struct {
    /** Command. */
    cmpSpContUnlockBySpCmd_t cmd;
} cmpCmdSpContUnlockBySp_t;

/** @} */

/** @defgroup MC_CMP_CMD_SP_CONT_UNLOCK_BY_SP_RSP Response
 * @{ */

/** SpContUnlockBySp signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Sp container secure object size. */
    uint32_t soSpContLen;
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpSpUnlockBySpRspSdata_t;

/** SpContUnlockBySp signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpSpUnlockBySpRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpSpContUnlockBySpRsp_t;

/** SpContUnlockBySp response. */
typedef struct {
    /** Response. */
    cmpSpContUnlockBySpRsp_t rsp;
} cmpRspSpContUnlockBySp_t;

/** @} */

/** Map CMP SpContUnlockBySp message. */
typedef union {
    /** Map CMP command. */
    cmpCmdSpContUnlockBySp_t cmpCmdSpContUnlockBySp;
    /** Map CMP response. */
    cmpRspSpContUnlockBySp_t cmpRspSpContUnlockBySp;
} cmpMapSpContUnlockBySp_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE \
  MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE_CMD Command
 * @{ */

/** TltContRegisterActivate encrypted data command. */
typedef struct {
    /** Tlt authentication key. */
    mcSymmetricKey_t kSpTltEnc;
} cmpTltRegActMsgEdata_t;

/** TltContRegisterActivate encrypted and padded data command. */
typedef struct {
    /** Encrypted data. */
    cmpTltRegActMsgEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpTltRegActMsgEdata_t))];
} cmpTltRegActCmdEd_t;

/** TltContRegisterActivate signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
    /** Uuid. */
    mcUuid_t uuid;
    /** Trustlet public key hash. */
    mcSha256_t skSpTltEnc;
    /** Minimum trustlet version supported. */
    mcContVersion_t tltVersion;
    /** Encrypted and padded data size. */
    uint32_t edLen;
    /** Encrypted and padded data. */
    cmpTltRegActCmdEd_t ed;
} cmpTltRegActCmdSdata_t;

/** TltContRegisterActivate signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpTltRegActCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpTltContRegisterActivateCmd_t;

/** TltContRegisterActivate command. */
typedef struct {
    /** Command. */
    cmpTltContRegisterActivateCmd_t cmd;
} cmpCmdTltContRegisterActivate_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE_RSP Response
 * @{ */

/** TltContRegisterActivate signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Sp container secure object size. */
    uint32_t soSpContLen;
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
    /* TltCont mapping info is in TCI cmpRspTltContRegisterActivateTci */
    uint32_t soTltContLen;
    mcSoTltCont_2_1_t soTltCont;
} cmpTltRegActRspSdata_t;

/** TltContRegisterActivate signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpTltRegActRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpTltContRegisterActivateRsp_t;

/** TltContRegisterActivate response. */
typedef struct {
    cmpTltContRegisterActivateRsp_t rsp;
} cmpRspTltContRegisterActivate_t;

/** @} */

/** Map CMP TltContRegisterActivate message. */
typedef union {
    /** Map CMP command. */
    cmpCmdTltContRegisterActivate_t cmpCmdTltContRegisterActivate;
    /** Map CMP response. */
    cmpRspTltContRegisterActivate_t cmpRspTltContRegisterActivate;
} cmpMapTltContRegisterActivate_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_UNREGISTER \
  MC_CMP_CMD_TLT_CONT_UNREGISTER
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_UNREGISTER_CMD Command
 * @{ */

/** TltContUnregister signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
    /** Uuid. */
    mcUuid_t uuid;
} cmpTltUnregCmdSdata_t;

/** TltContUnregister signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpTltUnregCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpTltContUnregisterCmd_t;

/** TltContUnregister command. */
typedef struct {
    /** Command. */
    cmpTltContUnregisterCmd_t cmd;
} cmpCmdTltContUnregister_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_UNREGISTER_RSP Response
 * @{ */

/** TltContUnregister signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Sp container secure object size. */
    uint32_t soSpContLen;
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpTltUnregRspSdata_t;

/** TltContUnregister signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpTltUnregRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpTltContUnregisterRsp_t;

/** TltContUnregister response. */
typedef struct {
    /** Response. */
    cmpTltContUnregisterRsp_t rsp;
} cmpRspTltContUnregister_t;

/** @} */

/** Map CMP TltContUnregister message. */
typedef union {
    /** Map CMP command. */
    cmpCmdTltContUnregister_t cmpCmdTltContUnregister;
    /** Map CMP response. */
    cmpRspTltContUnregister_t cmpRspTltContUnregister;
} cmpMapTltContUnregister_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER \
  MC_CMP_CMD_TLT_CONT_REGISTER
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_CMD Command
 * @{ */

/** TltContRegister encrypted data command. */
typedef struct {
    /** Tlt authentication key. */
    mcSymmetricKey_t kSpTltEnc;
} cmpTltRegMsgEdata_t;

/** TltContRegister encrypted and padded data command. */
typedef struct {
    /** Encrypted data. */
    cmpTltRegMsgEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpTltRegMsgEdata_t))];
} cmpTltRegCmdEd_t;

/** TltContRegister signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
    /** Uuid. */
    mcUuid_t uuid;
    /** Trustlet public key hash. */
    mcSha256_t skSpTltEnc;
    /** Minimum trustlet version supported. */
    mcContVersion_t tltVersion;
    /** Encrypted and padded data size. */
    uint32_t edLen;
    /** Encrypted and padded data. */
    cmpTltRegCmdEd_t ed;
} cmpTltRegCmdSdata_t;

/** TltContRegister signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpTltRegCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpTltContRegisterCmd_t;

/** TltContRegister command. */
typedef struct {
    /** Command. */
    cmpTltContRegisterCmd_t cmd;
} cmpCmdTltContRegister_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_REGISTER_RSP Response
 * @{ */

/** TltContRegister signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Sp container secure object size. */
    uint32_t soSpContLen;
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
    /** Tlt container secure object size. */
    uint32_t soTltContLen;
    /** Tlt container secure object. */
    mcSoTltCont_2_1_t soTltCont;
} cmpTltRegRspSdata_t;

/** TltContRegister signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpTltRegRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpTltContRegisterRsp_t;

/** TltContRegister response. */
typedef struct {
    /** Response. */
    cmpTltContRegisterRsp_t rsp;
} cmpRspTltContRegister_t;

/** @} */

/** Map CMP TltContRegister message. */
typedef union {
    /** Map CMP command. */
    cmpCmdTltContRegister_t cmpCmdTltContRegister;
    /** Map CMP response. */
    cmpRspTltContRegister_t cmpRspTltContRegister;
} cmpMapTltContRegister_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_ACTIVATE \
  MC_CMP_CMD_TLT_CONT_ACTIVATE
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_ACTIVATE_CMD Command
 * @{ */

/** TltContActivate signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
    /** Uuid. */
    mcUuid_t uuid;
} cmpTltActCmdSdata_t;

/** TltContActivate signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpTltActCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpTltContActivateCmd_t;

/** TltContActivate command. */
typedef struct {
    /** Command. */
    cmpTltContActivateCmd_t cmd;
} cmpCmdTltContActivate_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_ACTIVATE_RSP Response
 * @{ */

/** TltContActivate signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Tlt container secure object size. */
    uint32_t soTltContLen;
    /** Tlt container secure object. */
    mcSoTltCont_2_1_t soTltCont;
} cmpTltActRspSdata_t;

/** TltContActivate signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpTltActRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpTltContActivateRsp_t;

/** TltContActivate response. */
typedef struct {
    /** Response. */
    cmpTltContActivateRsp_t rsp;
} cmpRspTltContActivate_t;

/** @} */

/** Map CMP TltContActivate message. */
typedef union {
    /** Map CMP command. */
    cmpCmdTltContActivate_t cmpCmdTltContActivate;
    /** Map CMP response. */
    cmpRspTltContActivate_t cmpRspTltContActivate;
} cmpMapTltContActivate_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_LOCK_BY_SP \
  MC_CMP_CMD_TLT_CONT_LOCK_BY_SP
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_LOCK_BY_SP_CMD Command
 * @{ */

/** TltContLockBySp signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
    /** Uuid. */
    mcUuid_t uuid;
} cmpTltLockBySpCmdSdata_t;

/** TltContLockBySp signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpTltLockBySpCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpTltContLockBySpCmd_t;

/** TltContLockBySp command. */
typedef struct {
    /** Command. */
    cmpTltContLockBySpCmd_t cmd;
} cmpCmdTltContLockBySp_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_LOCK_BY_SP_RSP Response
 * @{ */

/** TltContLockBySp signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Tlt container secure object size. */
    uint32_t soTltContLen;
    /** Tlt container secure object. */
    mcSoTltCont_2_1_t soTltCont;
} cmpTltLockBySpRspSdata_t;

/** TltContLockBySp signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpTltLockBySpRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpTltContLockBySpRsp_t;

/** TltContLockBySp response. */
typedef struct {
    /** Response. */
    cmpTltContLockBySpRsp_t rsp;
} cmpRspTltContLockBySp_t;

/** @} */

/** Map CMP TltContLockBySp message. */
typedef union {
    /** Map CMP command. */
    cmpCmdTltContLockBySp_t cmpCmdTltContLockBySp;
    /** Map CMP response. */
    cmpRspTltContLockBySp_t cmpRspTltContLockBySp;
} cmpMapTltContLockBySp_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_UNLOCK_BY_SP \
  MC_CMP_CMD_TLT_CONT_UNLOCK_BY_SP
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_UNLOCK_BY_SP_CMD Command
 * @{ */

/** TltContLockBySp signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
    /** Uuid. */
    mcUuid_t uuid;
} cmpTltUnlockBySpCmdSdata_t;

/** TltContLockBySp signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpTltUnlockBySpCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpTltContUnlockBySpCmd_t;

/** TltContUnlockBySp command. */
typedef struct {
    /** Command. */
    cmpTltContUnlockBySpCmd_t cmd;
} cmpCmdTltContUnlockBySp_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_UNLOCK_BY_SP_RSP Response
 * @{ */

/** TltContUnlockBySp signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Tlt container secure object size. */
    uint32_t soTltContLen;
    /** Tlt container secure object. */
    mcSoTltCont_2_1_t soTltCont;
} cmpTltUnlockBySpRspSdata_t;

/** TltContUnlockBySp signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpTltUnlockBySpRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpTltContUnlockBySpRsp_t;

/** TltContUnlockBySp response. */
typedef struct {
    /** Response. */
    cmpTltContUnlockBySpRsp_t rsp;
} cmpRspTltContUnlockBySp_t;

/** @} */

/** Map CMP TltContUnlockBySp message. */
typedef union {
    /** Map CMP command. */
    cmpCmdTltContUnlockBySp_t cmpCmdTltContUnlockBySp;
    /** Map CMP response. */
    cmpRspTltContUnlockBySp_t cmpRspTltContUnlockBySp;
} cmpMapTltContUnlockBySp_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_PERSONALIZE \
  MC_CMP_CMD_TLT_CONT_PERSONALIZE
 * @{ */

/** @defgroup MC_CMP_CMD_TLT_CONT_PERSONALIZE_CMD Command
 * @{ */

/** TltContPersonalize encrypted data command. */
typedef struct {
    /** Data. */
    mcCoDataCont_t dataCont;
} cmpTltContPersonalizeCmdEdata_t;

/** TltContPersonalize encrypted and padded data command. */
typedef struct {
    /** Encrypted data. */
    cmpTltContPersonalizeCmdEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpTltContPersonalizeCmdEdata_t))];
} cmpTltContPersonalizeCmdEd_t;

/** TltContPersonalize signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
    /** Uuid. */
    mcUuid_t uuid;
    /** Pid. */
    mcPid_t pid;
    /** Encrypted and padded data size. */
    uint32_t edLen;
    /** Encrypted and padded data. */
    cmpTltContPersonalizeCmdEd_t ed;
} cmpTltContPersonalizeCmdSdata_t;

/** TltContPersonalize signed command and signature. */
typedef struct {
    /** Signed command. */
    cmpTltContPersonalizeCmdSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpTltContPersonalizeCmd_t;

/** TltContPersonalize command. */
typedef struct {
    cmpTltContPersonalizeCmd_t cmd;
} cmpCmdTltContPersonalize_t;

/** @} */

/** @defgroup MC_CMP_CMD_TLT_CONT_PERSONALIZE_RSP Response
 * @{ */

/** TltContPersonalize signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;
    /** Data container secure object size. */
    uint32_t soDataContLen;
    /** Data container secure object. */
    mcSoDataCont_t soDataCont;
} cmpTltContPersonalizeRspSdata_t;

/** TltContPersonalize signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpTltContPersonalizeRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpTltContPersonalizeRsp_t;

/** TltContPersonalize response. */
typedef struct {
    /** Response. */
    cmpTltContPersonalizeRsp_t rsp;
} cmpRspTltContPersonalize_t;

/** @} */

/** Map CMP TltContPersonalize message. */
typedef union {
    /** Map CMP command. */
    cmpCmdTltContPersonalize_t cmpCmdTltContPersonalize;
    /** Map CMP response. */
    cmpRspTltContPersonalize_t cmpRspTltContPersonalize;
} cmpMapTltContPersonalize_t;

/** @} */

/** Map CMP messages. */
typedef union {
    /** Backward compatible command header. */
    cmpCommandHeader_t commandHeader;
    /** Backward compatible response header. */
    cmpResponseHeader_t responseHeader;

    /** Backward compatible system command GetVersion. */
    cmpCmdGetVersion_t cmpCmdGetVersion;
    /** Backward compatible system response GetVersion. */
    cmpRspGetVersion_t cmpRspGetVersion;
    /** Backward compatible system command GetSuid. */
    cmpCmdGetSuid_t cmpCmdGetSuid;
    /** Backward compatible system response GetSuid. */
    cmpRspGetSuid_t cmpRspGetSuid;
    /** Backward compatible system command GenAuthToken. */
    cmpCmdGenAuthToken_t cmpCmdGenAuthToken;
    /** Backward compatible system response GenAuthToken. */
    cmpRspGenAuthToken_t cmpRspGenAuthToken;

    /** Command header. */
    cmpCommandHeaderMap_t cmdHeader;
    /** Response header. */
    cmpResponseHeaderMap_t rspHeader;

    /** Error signed response. */
    cmpMapError_t cmpMapError;

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
    /** Root administrative response RootContUnlockByRoot. */
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
    /** Sp administrative response SpContUnlockBySp. */
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
} cmpMap_t;

#endif // CMP_MAP_H_

/** @} */
