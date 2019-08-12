#ifndef ULOGRAW_H
#define ULOGRAW_H

/*============================================================================
  @file ULogRaw.h

  Raw Data frontend for ULog DAL.  Simplified access to the logging mechanism.

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/power/ULogRaw.h#1 $ */


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "ddiulog.h"


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 @brief ULog_RawInit - Initializes a Log structure and prepares
        it for use logging raw data.

 This function should be called once to initialize a log
 structure.  The log will not be enabled at this point unless
 memory is provided, but the structure is ready to be accessed.

 @param h  : A handle to retrieve.
 @param name : The name for the buffer. A maximum of 24
             characters are allowed for the strings, including
             the null terminator.
 @param bufferSize : The size in bytes of the log buffer.
                   Set this value to 0 to keep the log disabled,
                   and allocate the memory later.
 @param memoryType : Where should this memory be allocated:
                      local or shared memory.
 @param lockType : Which type of locking will be used on this
                    log.
 
 @return DAL_SUCCESS if log was initialized.
 */
DALResult ULog_RawInit(DalDeviceHandle ** h, 
                       const char * name,
                       uint32 bufferSize,
                       uint32 memoryType,
                       ULOG_LOCK_TYPE lockType);

/**
 @brief Ulog_RawConnect - Connect to a log structure.

 Connect and get a handle without creating the log. Use 
 RawInit to create a log, or use this interface
 to simply connect to a log that has been created elsewhere.

 @param h  : A handle to retrieve.
 @param name : The name for the buffer. A maximum of 24
             characters are allowed for the strings, including
             the null terminator.

 @return DAL_SUCCESS if connection was made.
 */
DALResult ULog_RawConnect(DalDeviceHandle ** h, 
                          const char * name);

/**
 @brief ULog_RawData - Call this function to log data.

 This function is called to log data.

 @param h : A handle indicating where to log.
 @param dataArray : The data to log.
 @param dataCount : The number of data elements to log.
 */
DALResult ULog_RawLog(DalDeviceHandle * h, 
                      char * dataArray,
                      uint32 dataCount);

#endif // ULOGRAW_H

