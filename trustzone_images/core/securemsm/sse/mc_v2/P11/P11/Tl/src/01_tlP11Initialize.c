/*
 * Copyright (c) 2012 Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

//#define TL_LOG_FILE
#include "tlStd.h"
#include "TlApi/TlApi.h"
#include "tlP11Api.h"
#include "tlLog.h"
#include "tlP11Glob.h"

/** Initialize
 * Used to trigger container activation
 *
 * @param shared buffer containing the command and our working area
 *
 * @return RET_OK if operation has been successfully completed.
 */
tciReturnCode_t processCmdInitialize(tciMessage_t* tci_message)
{
  return CKR_OK;
} //lint !e715
