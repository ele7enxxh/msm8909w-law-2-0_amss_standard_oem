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

#ifndef TL_CM_ERROR_H_
#define TL_CM_ERROR_H_

/** No error. */
#define  SUCCESSFUL                                 0x00000000

/** Error unknown command. */
#define RET_ERR_EXT_UNKNOWN_COMMAND                 0xE0000000
/** Error security status not satified. */
#define RET_ERR_EXT_SECURITY_STATUS_NOT_SATISFIED   0xE0000010
/** Error secure messaging failed. */
#define RET_ERR_EXT_SECURE_MESSAGING_FAILED         0xE0000020
/** Error incorrect parameters. */
#define RET_ERR_EXT_INCORRECT_PARAMETERS            0xE0000030
/** Error referenced data invalid. */
#define RET_ERR_EXT_REFERENCED_DATA_INVALID         0xE0000040
/** Error referenced data not found. */
#define RET_ERR_EXT_REFERENCED_DATA_NOT_FOUND       0xE0000050
/** Error method blocked. */
#define RET_ERR_EXT_METHOD_BLOCKED                  0xE0000060
/** Error conditions of use not satified. */
#define RET_ERR_EXT_CONDITIONS_OF_USE_NOT_SATISFIED 0xE0000070
/** Error container already registred error. */
#define RET_ERR_EXT_ALREADY_REGISTERED              0xE0000090
/** Error container already activated. */
#define RET_ERR_EXT_ALREADY_ACTIVATED               0xE00000A0
/** Error container not registred. */
#define RET_ERR_EXT_NOT_REGISTERED                  0xE00000B0
/** Error container not activated. */
#define RET_ERR_EXT_NOT_ACTIVATED                   0xE00000C0
/** Error container full. */
#define RET_ERR_EXT_CONTAINER_FULL                  0xE00000D0
/** Error container not locked. */
#define RET_ERR_EXT_NOT_LOCKED                      0xE00000E0
/** Error container locked. */
#define RET_ERR_EXT_LOCKED                          0xE00000F0
/** Error container already locked. */
#define RET_ERR_EXT_ALREADY_LOCKED                  0xE0000100

/** Internal error. */
#define RET_ERR_EXT_INTERNAL_ERROR                  0xE0001000
/** Mapped allocation size error. */
#define RET_ERR_EXT_SIZE                            0xE0002000

/** Unspecified error. */
#define RET_ERR_EXT_UNSPECIFIED                     0xEEEEEEEE

#endif // TL_CM_ERROR_H_

/** @} */
