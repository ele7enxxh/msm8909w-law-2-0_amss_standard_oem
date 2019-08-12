/*
 * Copyright (c) 2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary.
 */

#ifndef TLWRAPPER_API_H_
#define TLWRAPPER_API_H_

#include "tci.h"
#include "tlWrapperApiParams.h"

#define LOG_SIZE 8192

/**
 * @cond
 * Command ID's for communication Trustlet Connector -> Trustlet.
 */

#define TLWRAPPER_CMD_WRAP                  1
#define TLWRAPPER_CMD_UNWRAP                2

/**
 * Termination codes
 */
#define EXIT_ERROR                      ((uint32_t)(-1))

/*
 * @endcond
 */


/**
 * TCI message data.
 */
typedef union {
  tciCommandHeader_t        commandHeader;
  tciResponseHeader_t       responseHeader;

  cmdWrap_t                 cmdWrap;
  rspWrap_t                 rspWrap;

  cmdUnwrap_t               cmdUnwrap;
  rspUnwrap_t               rspUnwrap;

} tciMessage_t;

/**
 * Overall TCI structure.
 */
typedef struct {
    tciMessage_t    message;                    /**< TCI message */
    uint8_t         log[LOG_SIZE];       /**< size = len + log_len*/
}tci_t;

 
/**
 * Trustlet UUID.
 */
#define TL_WRAPPER_UUID { { 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 } }

#endif // TLWRAPPER_API_H_
