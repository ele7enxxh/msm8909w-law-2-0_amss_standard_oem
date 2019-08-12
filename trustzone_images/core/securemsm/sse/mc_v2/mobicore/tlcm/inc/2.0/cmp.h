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

/** Key size of encryption algorithm used for secure messaging. */
#define CMP_MSG_KEY_SIZE 32

/** @defgroup MC_CMP_2_0_CMD_BEGIN_SOC_AUTHENTICATION \
  MC_CMP_2_0_CMD_BEGIN_SOC_AUTHENTICATION
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_BEGIN_SOC_AUTHENTICATION_CMD Command_2_0
 * @{ */

/** BeginSocAuthentication non signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeader_t cmdHeader;
} cmpBeginSocAuthenticationCmd_t;

/** BeginSocAuthentication command. */
typedef struct {
    /** Command. */
    cmpBeginSocAuthenticationCmd_t cmd;
    /** AuthToken container secure object. */
    mcSoAuthTokenCont_t soAuthTokenCont;
} cmpCmdBeginSocAuthentication_t;

/** @} */

/** @defgroup MC_CMP_2_0_CMD_BEGIN_SOC_AUTHENTICATION_RSP Response_2_0
 * @{ */

/** BeginSocAuthentication signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
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

/** @} */

/** @defgroup MC_CMP_2_0_CMD_BEGIN_ROOT_AUTHENTICATION \
  MC_CMP_2_0_CMD_BEGIN_ROOT_AUTHENTICATION
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_BEGIN_ROOT_AUTHENTICATION_CMD Command_2_0
 * @{ */

/** BeginRootAuthentication non signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeader_t cmdHeader;
} cmpBeginRootAuthenticationCmd_t;

/** BeginRootAuthentication command. */
typedef struct {
    /** Command. */
    cmpBeginRootAuthenticationCmd_t cmd;
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
} cmpCmdBeginRootAuthentication_t;

/** @} */

/** @defgroup MC_CMP_2_0_CMD_BEGIN_ROOT_AUTHENTICATION_RSP Response_2_0
 * @{ */

/** BeginRootAuthentication signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
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

/** @} */

/** @defgroup MC_CMP_2_0_CMD_BEGIN_SP_AUTHENTICATION \
  MC_CMP_2_0_CMD_BEGIN_SP_AUTHENTICATION
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_BEGIN_SP_AUTHENTICATION_CMD Command_2_0
 * @{ */

/** BeginSpAuthentication non signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeader_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
} cmpBeginSpAuthenticationCmdSdata_t;

/** BeginSpAuthentication non signed command and no signature. */
typedef struct {
    /** Non signed command. */
    cmpBeginSpAuthenticationCmdSdata_t sdata;
} cmpBeginSpAuthenticationCmd_t;

/** BeginSpAuthentication command. */
typedef struct {
    /** Command. */
    cmpBeginSpAuthenticationCmd_t cmd;
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpCmdBeginSpAuthentication_t;

/** @} */

/** @defgroup MC_CMP_2_0_CMD_BEGIN_SP_AUTHENTICATION_RSP Response_2_0
 * @{ */

/** BeginSpAuthentication signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
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

/** @} */

/** @defgroup MC_CMP_2_0_CMD_AUTHENTICATE MC_CMP_2_0_CMD_AUTHENTICATE
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_AUTHENTICATE_CMD Command_2_0
 * @{ */

/** Authenticate encrypted data command. */
typedef struct {
    /** Suid. */
    mcSuid_t suid;
    /** EntityId. */
    uint32_t entityId;
    /** NWd authentication random number. */
    cmpRnd8_t rnd2;
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
    cmpCommandHeader_t cmdHeader;
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

/** @defgroup MC_CMP_2_0_CMD_AUTHENTICATE_RSP Response_2_0
 * @{ */

/** Authenticate encrypted data response. */
typedef struct {
    /** Suid. */
    mcSuid_t suid;
    /** EntityId. */
    uint32_t entityId;
    /** SWd authentication random number. */
    cmpRnd8_t rnd1;
    /** NWd authentication random number. */
    cmpRnd8_t rnd2;
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
    cmpResponseHeader_t rspHeader;
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

/** @} */

/** @defgroup MC_CMP_2_0_CMD_AUTHENTICATE_TERMINATE \
  MC_CMP_2_0_CMD_AUTHENTICATE_TERMINATE
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_AUTHENTICATE_TERMINATE_CMD Command_2_0
 * @{ */

/** AuthenticateTerminate signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeader_t cmdHeader;
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

/** @defgroup MC_CMP_2_0_CMD_AUTHENTICATE_TERMINATE_RSP Response_2_0
 * @{ */

/** AuthenticateTerminate signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
} cmpAuthenticateTerminateRspSdata_t;

/** AuthenticateTerminate signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpAuthenticateTerminateRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpTerminateAutenticateRsp_t;

/** AuthenticateTerminate response. */
typedef struct {
    /** Response. */
    cmpTerminateAutenticateRsp_t rsp;
} cmpRspAuthenticateTerminate_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_2_0_CMD_ROOT_CONT_REGISTER_ACTIVATE \
  MC_CMP_2_0_CMD_ROOT_CONT_REGISTER_ACTIVATE
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_ROOT_CONT_REGISTER_ACTIVATE_CMD Command_2_0
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
    cmpCommandHeader_t cmdHeader;
    /** Rootid. */
    mcRootid_t rootid;
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

/** @defgroup MC_CMP_2_0_CMD_ROOT_CONT_REGISTER_ACTIVATE_RSP Response_2_0
 * @{ */

/** RootContRegisterActivate encrypted data response. */
typedef struct {
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
} cmpRootRegActRspEdata_t;

/** RootContRegisterActivate encrypted and padded data response. */
typedef struct {
    /** Encrypted data. */
    cmpRootRegActRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpRootRegActRspEdata_t))];
} cmpRootRegActRspEd_t;

/** RootContRegisterActivate signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Encrypted and padded data. */
    cmpRootRegActRspEd_t ed;
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
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
} cmpRspRootContRegisterActivate_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_2_0_CMD_ROOT_CONT_UNREGISTER \
  MC_CMP_2_0_CMD_ROOT_CONT_UNREGISTER
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_ROOT_CONT_UNREGISTER_CMD Command_2_0
 * @{ */

/** RootContUnregister encrypted data command. */
typedef struct {
    /** Suid. */
    mcSuid_t suid;
    /** AuthToken container secure object. */
    mcSoAuthTokenCont_t soAuthTokenCont;
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
    cmpCommandHeader_t cmdHeader;
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

/** @defgroup MC_CMP_2_0_CMD_ROOT_CONT_UNREGISTER_RSP Response_2_0
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
    cmpResponseHeader_t rspHeader;
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
    /** AuthToken container secure object. */
    mcSoAuthTokenCont_t soAuthTokenCont;
} cmpRspRootContUnregister_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_2_0_CMD_ROOT_CONT_LOCK_BY_ROOT \
  MC_CMP_2_0_CMD_ROOT_CONT_LOCK_BY_ROOT
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_ROOT_CONT_LOCK_BY_ROOT_CMD Command_2_0
 * @{ */

/** RootContLockByRoot signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeader_t cmdHeader;
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

/** @defgroup MC_CMP_2_0_CMD_ROOT_CONT_LOCK_BY_ROOT_RSP Response_2_0
 * @{ */

/** RootContLockByRoot encrypted data response. */
typedef struct {
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
} cmpRootLockByRootRspEdata_t;

/** RootContLockByRoot encrypted and padded data response. */
typedef struct {
    /** Encrypted data. */
    cmpRootLockByRootRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpRootLockByRootRspEdata_t))];
} cmpRootLockByRootRspEd_t;

/** RootContLockByRoot signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Encrypted and padded data. */
    cmpRootLockByRootRspEd_t ed;
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
    /** Response. */
    cmpRootContLockByRootRsp_t rsp;
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
} cmpRspRootContLockByRoot_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_2_0_CMD_ROOT_CONT_UNLOCK_BY_ROOT \
  MC_CMP_2_0_CMD_ROOT_CONT_UNLOCK_BY_ROOT
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_ROOT_CONT_UNLOCK_BY_ROOT_CMD Command_2_0
 * @{ */

/** RootContUnlockByRoot signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeader_t cmdHeader;
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

/** @defgroup MC_CMP_2_0_CMD_ROOT_CONT_UNLOCK_BY_ROOT_RSP Response_2_0
 * @{ */

/** RootContUnlockByRoot encrypted data response. */
typedef struct {
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
} cmpRootUnlockByRootRspEdata_t;

/** RootContUnlockByRoot encrypted and padded data response. */
typedef struct {
    /** Encrypted data. */
    cmpRootUnlockByRootRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpRootUnlockByRootRspEdata_t))];
} cmpRootUnlockByRootRspEd_t;

/** RootContUnlockByRoot signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Encrypted and padded data. */
    cmpRootUnlockByRootRspEd_t ed;
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
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
} cmpRspRootContUnlockByRoot_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_REGISTER_ACTIVATE \
  MC_CMP_2_0_CMD_SP_CONT_REGISTER_ACTIVATE
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_REGISTER_ACTIVATE_CMD Command_2_0
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
    cmpCommandHeader_t cmdHeader;
    mcSpid_t spid;
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

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_REGISTER_ACTIVATE_RSP Response_2_0
 * @{ */

/** SpContRegisterActivate encrypted data response. */
typedef struct {
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpSpRegActRspEdata_t;

/** SpContRegisterActivate encrypted and padded data response. */
typedef struct {
    /** Encrypted data. */
    cmpSpRegActRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpSpRegActRspEdata_t))];
} cmpSpRegActRspEd_t;

/** SpContRegisterActivate signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Encrypted and padded data. */
    cmpSpRegActRspEd_t ed;
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
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpRspSpContRegisterActivate_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_UNREGISTER \
 MC_CMP_2_0_CMD_SP_CONT_UNREGISTER
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_UNREGISTER_CMD Command_2_0
 * @{ */

/** SpContUnregister signed response. */
typedef struct {
    /** Command header. */
    cmpCommandHeader_t cmdHeader;
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

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_UNREGISTER_RSP Response_2_0
 * @{ */

/** SpContUnregister encrypted data response. */
typedef struct {
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
} cmpSpUnregRspEdata_t;

/** SpContUnregister encrypted and padded data response. */
typedef struct {
    /** Encrypted data. */
    cmpSpUnregRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpSpUnregRspEdata_t))];
} cmpSpUnregRspEd_t;

/** SpContUnregister signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Encrypted and padded data. */
    cmpSpUnregRspEd_t ed;
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
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
} cmpRspSpContUnregister_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_REGISTER \
  MC_CMP_2_0_CMD_SP_CONT_REGISTER
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_REGISTER_CMD Command_2_0
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
    cmpCommandHeader_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
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

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_REGISTER_RSP Response_2_0
 * @{ */

/** SpContRegister encrypted data response. */
typedef struct {
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpSpRegisterRspEdata_t;

/** SpContRegister encrypted and padded data response. */
typedef struct {
    /** Encrypted data. */
    cmpSpRegisterRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpSpRegisterRspEdata_t))];
} cmpSpRegisterRspEd_t;

/** SpContRegister signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Encrypted and padded data. */
    cmpSpRegisterRspEd_t ed;
} cmpSpRegisterRspSdata_t;

/** SpContRegister signed response and signature. */
typedef struct {
    cmpSpRegisterRspSdata_t sdata;
    /** Signature. */
    cmpMac_t mac;
} cmpSpContRegisterRsp_t;

/** SpContRegister response. */
typedef struct {
    /** Response. */
    cmpSpContRegisterRsp_t rsp;
    /** Root container secure object. */
    mcSoRootCont_t soRootCont;
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpRspSpContRegister_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_LOCK_BY_ROOT \
  MC_CMP_2_0_CMD_SP_CONT_LOCK_BY_ROOT
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_LOCK_BY_ROOT_CMD Command_2_0
 * @{ */

/** SpContLockByRoot signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeader_t cmdHeader;
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
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpCmdSpContLockByRoot_t;

/** @} */

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_LOCK_BY_ROOT_RSP Response_2_0
 * @{ */

/** SpContLockByRoot encrypted data response. */
typedef struct {
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpSpLockByRootRspEdata_t;

/** SpContLockByRoot encrypted and padded data response. */
typedef struct {
    /** Encrypted data. */
    cmpSpLockByRootRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpSpLockByRootRspEdata_t))];
} cmpSpLockByRootRspEd_t;

/** SpContLockByRoot signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Encrypted and padded data. */
    cmpSpLockByRootRspEd_t ed;
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
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpRspSpContLockByRoot_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_UNLOCK_BY_ROOT \
  MC_CMP_2_0_CMD_SP_CONT_UNLOCK_BY_ROOT
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_UNLOCK_BY_ROOT_CMD Command_2_0
 * @{ */

/** SpContUnlockByRoot signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeader_t cmdHeader;
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
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpCmdSpContUnlockByRoot_t;

/** @} */

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_UNLOCK_BY_ROOT_RSP Response_2_0
 * @{ */

/** SpContUnlockByRoot encrypted data response. */
typedef struct {
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpSpUnlockByRootRspEdata_t;

/** SpContUnlockByRoot encrypted and padded data response. */
typedef struct {
    /** Encrypted data. */
    cmpSpUnlockByRootRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpSpUnlockByRootRspEdata_t))];
} cmpSpUnlockByRootRspEd_t;

/** SpContUnlockByRoot signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Encrypted and padded data. */
    cmpSpUnlockByRootRspEd_t ed;
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
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpRspSpContUnlockByRoot_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_ACTIVATE \
  MC_CMP_2_0_CMD_SP_CONT_ACTIVATE
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_ACTIVATE_CMD Command_2_0
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
    cmpCommandHeader_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
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

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_ACTIVATE_RSP Response_2_0
 * @{ */

/** SpContActivate encrypted data response. */
typedef struct {
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpSpActivateRspEdata_t;

/** SpContActivate encrypted and padded data response. */
typedef struct {
    /** Encrypted data. */
    cmpSpActivateRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpSpActivateRspEdata_t))];
} cmpSpActivateRspEd_t;

/** SpContActivate signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Encrypted and padded data. */
    cmpSpActivateRspEd_t ed;
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
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpRspSpContActivate_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_LOCK_BY_SP \
  MC_CMP_2_0_CMD_SP_CONT_LOCK_BY_SP
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_LOCK_BY_SP_CMD Command_2_0
 * @{ */

/** SpContLockBySp signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeader_t cmdHeader;
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

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_LOCK_BY_SP_RSP Response_2_0
 * @{ */

/** SpContLockBySp encrypted data response. */
typedef struct {
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpSpLockBySpRspEdata_t;

/** SpContLockBySp encrypted and padded data command. */
typedef struct {
    /** Encrypted data. */
    cmpSpLockBySpRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpSpLockBySpRspEdata_t))];
} cmpSpLockBySpRspEd_t;

/** SpContLockBySp signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Encrypted and padded data. */
    cmpSpLockBySpRspEd_t ed;
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
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpRspSpContLockBySp_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_UNLOCK_BY_SP \
  MC_CMP_2_0_CMD_SP_CONT_UNLOCK_BY_SP
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_UNLOCK_BY_SP_CMD Command_2_0
 * @{ */

/** SpContUnlockBySp signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeader_t cmdHeader;
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

/** @defgroup MC_CMP_2_0_CMD_SP_CONT_UNLOCK_BY_SP_RSP Response_2_0
 * @{ */

/** SpContUnlockBySp encrypted data response. */
typedef struct {
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpSpUnlockBySpRspEdata_t;

/** SpContUnlockBySp encrypted and padded data command. */
typedef struct {
    /** Encrypted data. */
    cmpSpUnlockBySpRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpSpUnlockBySpRspEdata_t))];
} cmpSpUnlockBySpRspEd_t;

/** SpContUnlockBySp signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Encrypted and padded data. */
    cmpSpUnlockBySpRspEd_t ed;
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
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpRspSpContUnlockBySp_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_REGISTER_ACTIVATE \
  MC_CMP_2_0_CMD_TLT_CONT_REGISTER_ACTIVATE
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_REGISTER_ACTIVATE_CMD Command_2_0
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
    cmpCommandHeader_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
    /** Uuid. */
    mcUuid_t uuid;
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

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_REGISTER_ACTIVATE_RSP Response_2_0
 * @{ */

/** TltContRegisterActivate encrypted data response. */
typedef struct {
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
    /** Tlt container secure object. */
    mcSoTltCont_2_0_t soTltCont;
} cmpTltRegActRspEdata_t;

/** TltContRegisterActivate encrypted and padded data response. */
typedef struct {
    /** Encrypted data. */
    cmpTltRegActRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpTltRegActRspEdata_t))];
} cmpTltRegActRspEd_t;

/** TltContRegisterActivate signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Encrypted and padded data. */
    cmpTltRegActRspEd_t ed;
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
    /** Response. */
    cmpTltContRegisterActivateRsp_t rsp;
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
    /** Tlt container secure object. */
    mcSoTltCont_2_0_t soTltCont;
} cmpRspTltContRegisterActivate_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_UNREGISTER \
  MC_CMP_2_0_CMD_TLT_CONT_UNREGISTER
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_UNREGISTER_CMD Command_2_0
 * @{ */

/** TltContUnregister signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeader_t cmdHeader;
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

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_UNREGISTER_RSP Response_2_0
 * @{ */

/** TltContUnregister encrypted data response. */
typedef struct {
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpTltUnregRspEdata_t;

/** TltContUnregister encrypted and padded data response. */
typedef struct {
    /** Encrypted data. */
    cmpTltUnregRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpTltUnregRspEdata_t))];
} cmpTltUnregRspEd_t;

/** TltContUnregister signed command. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Encrypted and padded data. */
    cmpTltUnregRspEd_t ed;
} cmpTltUnregRspSdata_t;

/** TltContUnregister signed command and signature. */
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
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
} cmpRspTltContUnregister_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_REGISTER \
  MC_CMP_2_0_CMD_TLT_CONT_REGISTER
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_REGISTER_CMD Command_2_0
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
    cmpCommandHeader_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
    /** Uuid. */
    mcUuid_t uuid;
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

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_REGISTER_RSP Response_2_0
 * @{ */

/** TltContRegister encrypted data response. */
typedef struct {
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
    /** Tlt container secure object. */
    mcSoTltCont_2_0_t soTltCont;
} cmpTltRegRspEdata_t;

/** TltContRegister encrypted and padded data response. */
typedef struct {
    /** Encrypted data. */
    cmpTltRegRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpTltRegRspEdata_t))];
} cmpTltRegRspEd_t;

/** TltContRegister signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Encrypted and padded data. */
    cmpTltRegRspEd_t ed;
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
    /** Sp container secure object. */
    mcSoSpCont_t soSpCont;
    /** Tlt container secure object. */
    mcSoTltCont_2_0_t soTltCont;
} cmpRspTltContRegister_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_ACTIVATE \
  MC_CMP_2_0_CMD_TLT_CONT_ACTIVATE
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_ACTIVATE_CMD Command_2_0
 * @{ */

/** TltContActivate signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeader_t cmdHeader;
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
    /** Tlt container secure object. */
    mcSoTltCont_2_0_t soTltCont;
} cmpCmdTltContActivate_t;

/** @} */

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_ACTIVATE_RSP Response_2_0
 * @{ */

/** TltContActivate encrypted data response. */
typedef struct {
    /** Tlt container secure object. */
    mcSoTltCont_2_0_t soTltCont;
} cmpTltActRspEdata_t;

/** TltContActivate encrypted and padded data response. */
typedef struct {
    /** Encrypted data. */
    cmpTltActRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpTltActRspEdata_t))];
} cmpTltActRspEd_t;

/** TltContActivate signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Encrypted and padded data. */
    cmpTltActRspEd_t ed;
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
    /** Tlt container secure object. */
    mcSoTltCont_2_0_t soTltCont;
} cmpRspTltContActivate_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_LOCK_BY_SP \
  MC_CMP_2_0_CMD_TLT_CONT_LOCK_BY_SP
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_LOCK_BY_SP_CMD Command_2_0
 * @{ */

/** TltContLockBySp signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeader_t cmdHeader;
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
    /** Tlt container secure object. */
    mcSoTltCont_2_0_t soTltCont;
} cmpCmdTltContLockBySp_t;

/** @} */

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_LOCK_BY_SP_RSP Response_2_0
 * @{ */

/** TltContLockBySp encrypted data response. */
typedef struct {
    /** Tlt container secure object. */
    mcSoTltCont_2_0_t soTltCont;
} cmpTltLockBySpRspEdata_t;

/** TltContLockBySp encrypted and padded data response. */
typedef struct {
    /** Encrypted data. */
    cmpTltLockBySpRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpTltLockBySpRspEdata_t))];
} cmpTltLockBySpRspEd_t;

/** TltContLockBySp signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Encrypted and padded data. */
    cmpTltLockBySpRspEd_t ed;
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
    /** Tlt container secure object. */
    mcSoTltCont_2_0_t soTltCont;
} cmpRspTltContLockBySp_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_UNLOCK_BY_SP \
  MC_CMP_2_0_CMD_TLT_CONT_UNLOCK_BY_SP
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_UNLOCK_BY_SP_CMD Command_2_0
 * @{ */

/** TltContUnlockBySp signed command. */
typedef struct {
    /** Command header. */
    cmpCommandHeader_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
    /** Uuid. */
    mcUuid_t uuid;
} cmpTltUnlockBySpCmdSdata_t;

/** TltContUnlockBySp signed command and signature. */
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
    /** Tlt container secure object. */
    mcSoTltCont_2_0_t soTltCont;
} cmpCmdTltContUnlockBySp_t;

/** @} */

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_UNLOCK_BY_SP_RSP Response_2_0
 * @{ */

/** TltContUnlockBySp encrypted data response. */
typedef struct {
    /** Tlt container secure object. */
    mcSoTltCont_2_0_t soTltCont;
} cmpTltUnlockBySpRspEdata_t;

/** TltContUnlockBySp encrypted and padded data response. */
typedef struct {
    /** Encrypted data. */
    cmpTltUnlockBySpRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpTltUnlockBySpRspEdata_t))];
} cmpTltUnlockBySpRspEd_t;

/** TltContUnlockBySp signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Encrypted and padded data. */
    cmpTltUnlockBySpRspEd_t ed;
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
    /** Tlt container secure object. */
    mcSoTltCont_2_0_t soTltCont;
} cmpRspTltContUnlockBySp_t;

/** @} */

/** @} */

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_PERSONALIZE \
  MC_CMP_2_0_CMD_TLT_CONT_PERSONALIZE
 * @{ */

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_PERSONALIZE_CMD Command_2_0
 * @{ */

/** TltContPersonalize encrypted data command. */
typedef struct {
    /** Pid. */
    mcPid_t pid;
    /** Data. */
    mcCoDataCont_t persoData;
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
    cmpCommandHeader_t cmdHeader;
    /** Spid. */
    mcSpid_t spid;
    /** Uuid. */
    mcUuid_t uuid;
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
    /** Command. */
    cmpTltContPersonalizeCmd_t cmd;
    /** Tlt container secure object. */
    mcSoTltCont_2_0_t soTltCont;
} cmpCmdTltContPersonalize_t;

/** @} */

/** @defgroup MC_CMP_2_0_CMD_TLT_CONT_PERSONLIZE_RSP Response_2_0
 * @{ */

/** TltContPersonalize encrypted data response. */
typedef struct {
    /** Data container secure object. */
    mcSoDataCont_t soDataCont;
} cmpTltContPersonalizeRspEdata_t;

/** TltContPersonalize encrypted and padded data response. */
typedef struct {
    /** Encrypted data. */
    cmpTltContPersonalizeRspEdata_t edata;
    /** Padding. */
    uint8_t padding[CMP_ED_PADDING(sizeof(cmpTltContPersonalizeRspEdata_t))];
} cmpTltContPersonalizeRspEd_t;

/** TltContPersonalize signed response. */
typedef struct {
    /** Response header. */
    cmpResponseHeader_t rspHeader;
    /** Encrypted and padded data size. */
    uint32_t edLen;
    /** Encrypted and padded data. */
    cmpTltContPersonalizeRspEd_t ed;
} cmpTltContPersonalizeRspSdata_t;

/** TltContPersonalize signed response and signature. */
typedef struct {
    /** Signed response. */
    cmpTltContPersonalizeRspSdata_t sdata;
    cmpMac_t mac;
} cmpTltContPersonalizeRsp_t;

/** TltContPersonalize response. */
typedef struct {
    /** Response. */
    cmpTltContPersonalizeRsp_t rsp;
    /** Data container secure object. */
    mcSoDataCont_t soDataCont;
} cmpRspTltContPersonalize_t;

/** @} */

/** @} */

#endif // CMP_H_

/** @} */
