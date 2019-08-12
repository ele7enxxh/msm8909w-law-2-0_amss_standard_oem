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

#ifndef TL_CM_API_COMMON_H_
#define TL_CM_API_COMMON_H_

#include "tlCmError.h"

/** TlCm command ids supported.
 * @note All command ids must be in range 0 to 0x1D.
 */
//lint -esym(756, cmpCommands_t) cmpCommands_t type by itself not used.
typedef enum cmpCommands_t {
    MC_CMP_CMD_AUTHENTICATE = 0,
    MC_CMP_CMD_BEGIN_ROOT_AUTHENTICATION = 1,
    MC_CMP_CMD_BEGIN_SOC_AUTHENTICATION = 2,
    MC_CMP_CMD_BEGIN_SP_AUTHENTICATION = 3,
    MC_CMP_CMD_GENERATE_AUTH_TOKEN = 4,
    MC_CMP_CMD_GET_VERSION = 5,
    //MC_CMP_CMD_ROOT_CONT_ACTIVATE = 6,
    MC_CMP_CMD_ROOT_CONT_LOCK_BY_ROOT = 7,
    //MC_CMP_CMD_ROOT_CONT_REGISTER = 8,
    MC_CMP_CMD_ROOT_CONT_REGISTER_ACTIVATE = 9,
    MC_CMP_CMD_ROOT_CONT_UNLOCK_BY_ROOT = 10,
    MC_CMP_CMD_ROOT_CONT_UNREGISTER = 11,
    MC_CMP_CMD_SP_CONT_ACTIVATE = 12,
    MC_CMP_CMD_SP_CONT_LOCK_BY_ROOT = 13,
    MC_CMP_CMD_SP_CONT_LOCK_BY_SP = 14,
    MC_CMP_CMD_SP_CONT_REGISTER = 15,
    MC_CMP_CMD_SP_CONT_REGISTER_ACTIVATE = 16,
    MC_CMP_CMD_SP_CONT_UNLOCK_BY_ROOT = 17,
    MC_CMP_CMD_SP_CONT_UNLOCK_BY_SP = 18,
    MC_CMP_CMD_SP_CONT_UNREGISTER = 19,
    MC_CMP_CMD_TLT_CONT_ACTIVATE = 20,
    MC_CMP_CMD_TLT_CONT_LOCK_BY_SP = 21,
    MC_CMP_CMD_TLT_CONT_PERSONALIZE = 22,
    MC_CMP_CMD_TLT_CONT_REGISTER = 23,
    MC_CMP_CMD_TLT_CONT_REGISTER_ACTIVATE = 24,
    MC_CMP_CMD_TLT_CONT_UNLOCK_BY_SP = 25,
    MC_CMP_CMD_TLT_CONT_UNREGISTER = 26,
    MC_CMP_CMD_GET_SUID = 27,
    MC_CMP_CMD_AUTHENTICATE_TERMINATE = 28,
    MC_CMP_CMD_LAST_ = MC_CMP_CMD_AUTHENTICATE_TERMINATE,
} cmpCommands_t;

/** TlCm exit code: TlCm exited with error. */
#define EXIT_ERROR  ((uint32_t)(-1))

#endif // TL_CM_API_COMMON_H_

/** @} */
