/*
 * Copyright (c) 2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary.
 */

#ifndef TCI_H_
#define TCI_H_

typedef uint32_t tciCommandId_t;
typedef uint32_t tciResponseId_t;
typedef uint32_t tciReturnCode_t;

#define RET_OK            0
#define E_GENERAL_ERROR   1
#define E_INVALID_INPUT   2
#define E_INVALID_OUTPUT  3
#define E_UNKNOWN_COMMAND 4

#define LENGTH(x) sizeof(x)/sizeof((x)[0])

/**< Responses have bit 31 set */
#define RSP_ID_MASK (1U << 31)
#define RSP_ID(cmdId) (((uint32_t)(cmdId)) | RSP_ID_MASK)
#define IS_CMD(cmdId) ((((uint32_t)(cmdId)) & RSP_ID_MASK) == 0)
#define IS_RSP(cmdId) ((((uint32_t)(cmdId)) & RSP_ID_MASK) == RSP_ID_MASK)

/**
 * TCI command header.
 */
typedef struct{
    tciCommandId_t commandId; /**< Command ID */
    tciReturnCode_t reserved; /**< Reserved */
} tciCommandHeader_t;

/**
 * TCI response header.
 */
typedef struct{
    tciResponseId_t     responseId; /**< Response ID (must be command ID | RSP_ID_MASK )*/
    tciReturnCode_t     returnCode; /**< Return code of command */
} tciResponseHeader_t;

#endif // TCI_H_
