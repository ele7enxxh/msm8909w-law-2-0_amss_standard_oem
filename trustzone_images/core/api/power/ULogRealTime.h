#ifndef ULOGREALTIME_H
#define ULOGREALTIME_H

/*============================================================================
  @file DalULogRealTime.h

  Realtime Data frontend for ULog DAL.  Simplified access to the logging 
  mechanism.

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/power/ULogRealTime.h#1 $ */

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
 @brief ULog_RealTimeInit - Initializes a Log structure and
        prepares it for use.
 
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
 @param memoryType : Where memory for this log should be
                      allocated. 
 @param lockType : Which type of locking will be used on this
                    log.
 
 @return DAL_SUCCESS if log was initialized.
 */
DALResult ULog_RealTimeInit(DalDeviceHandle ** h, 
                            const char * name,
                            uint32 bufferSize,
                            uint32 memoryType,
                            ULOG_LOCK_TYPE lockType);

/**
 @brief ULog_RealTimeConnect - Connect to a log structure.

 Connect and get a handle without creating the log. Use 
 RealTimeInit to connect and define a log, or use this interface
 to simply connect to a log that has been defined elsewhere.

 @param h  : A handle to retrieve.
 @param name : The name for the buffer. A maximum of 24
             characters are allowed for the strings, including
             the null terminator.

 @return DAL_SUCCESS if connection was made.
 */
DALResult ULog_RealTimeConnect(DalDeviceHandle ** h, 
                               const char * name);

/**
 @brief ULog_RealTimePrintf - Call this function to log data.

 This function is called to log data using a printf style
 interface.

 @param h : A handle indicating where to log.
 @param dataCount : The number of parameters not including the
                  format string with a maximum value of 10.
 @param formatStr : The format string for the printf.
 @param ... : The data to log.  These must be (u)int32 in
        length.
 */
DALResult ULog_RealTimePrintf(DalDeviceHandle * h, 
                              uint32 dataCount, 
                              const char * formatStr,
                              ...);


/**
 @brief ULOG_RT_PRINTF_N - Macros to provide some parameter
        count safety to the ULog_RealTimePrintf function.

 These macros provide parameter count safety to the
 ULog_RealTimePrintf function.  They can be used instead of
 calling the function directly.

 @param h : A handle indicating where to log.
 @param formatStr : The format string for the printf.
 @param p1 - pN : The data to log.  These must be (u)int32 in
            length.  Smaller data types will be automatically
            promoted.
 */
#define ULOG_RT_PRINTF_0(h, formatStr)    \
             ULog_RealTimePrintf(h, 0, formatStr)
#define ULOG_RT_PRINTF_1(h, formatStr, p1)    \
             ULog_RealTimePrintf(h, 1, formatStr, p1)
#define ULOG_RT_PRINTF_2(h, formatStr, p1, p2)    \
             ULog_RealTimePrintf(h, 2, formatStr, p1, p2)
#define ULOG_RT_PRINTF_3(h, formatStr, p1, p2, p3)    \
             ULog_RealTimePrintf(h, 3, formatStr, p1, p2, p3)
#define ULOG_RT_PRINTF_4(h, formatStr, p1, p2, p3, p4)    \
             ULog_RealTimePrintf(h, 4, formatStr, p1, p2, p3, p4)
#define ULOG_RT_PRINTF_5(h, formatStr, p1, p2, p3, p4, p5)    \
             ULog_RealTimePrintf(h, 5, formatStr, p1, p2, p3, p4, p5)
#define ULOG_RT_PRINTF_6(h, formatStr, p1, p2, p3, p4, p5, p6)    \
             ULog_RealTimePrintf(h, 6, formatStr, p1, p2, p3, p4, p5, p6)
#define ULOG_RT_PRINTF_7(h, formatStr, p1, p2, p3, p4, p5, p6, p7)    \
             ULog_RealTimePrintf(h, 7, formatStr, p1, p2, p3, p4, p5, p6, p7)
#define ULOG_RT_PRINTF_8(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)    \
             ULog_RealTimePrintf(h, 8, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)
#define ULOG_RT_PRINTF_9(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)    \
             ULog_RealTimePrintf(h, 9, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define ULOG_RT_PRINTF_10(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)    \
             ULog_RealTimePrintf(h, 10, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)


/**
 @brief ULog_RealTimeData - Call this function to log data.

 This function is called to log data.

 @param h  : A handle indicating where to log.
 @param dataCount : The number of data parameters to log.
 @param ... : The data to log.  These must be no more than
        (u)int32 in length.
 */
DALResult ULog_RealTimeData(DalDeviceHandle * h, 
                            uint32 dataCount, 
                            ...);

/**
 @brief ULOG_RT_DATA_N - Macros to provide some parameter count
        safety to the ULog_RealTimeData function.

 These macros provide parameter count safety to the
 ULog_RealTimeData function.  They can be used instead of
 calling the function directly.

 @param h : A handle indicating where to log.
 @param p1 - pN : The data to log.  These must be (u)int32 in
            length.  Smaller data types will be automatically
            promoted.
 */
#define ULOG_RT_DATA_1(h, p1)    \
             ULog_RealTimeData(h, 1, p1)
#define ULOG_RT_DATA_2(h, p1, p2)    \
             ULog_RealTimeData(h, 2, p1, p2)
#define ULOG_RT_DATA_3(h, p1, p2, p3)    \
             ULog_RealTimeData(h, 3, p1, p2, p3)
#define ULOG_RT_DATA_4(h, p1, p2, p3, p4)    \
             ULog_RealTimeData(h, 4, p1, p2, p3, p4)
#define ULOG_RT_DATA_5(h, p1, p2, p3, p4, p5)    \
             ULog_RealTimeData(h, 5, p1, p2, p3, p4, p5)
#define ULOG_RT_DATA_6(h, p1, p2, p3, p4, p5, p6)    \
             ULog_RealTimeData(h, 6, p1, p2, p3, p4, p5, p6)
#define ULOG_RT_DATA_7(h, p1, p2, p3, p4, p5, p6, p7)    \
             ULog_RealTimeData(h, 7, p1, p2, p3, p4, p5, p6, p7)
#define ULOG_RT_DATA_8(h, p1, p2, p3, p4, p5, p6, p7, p8)    \
             ULog_RealTimeData(h, 8, p1, p2, p3, p4, p5, p6, p7, p8)
#define ULOG_RT_DATA_9(h, p1, p2, p3, p4, p5, p6, p7, p8, p9)    \
             ULog_RealTimeData(h, 9, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define ULOG_RT_DATA_10(h, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)    \
             ULog_RealTimeData(h, 10, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)

#endif // ULOGREALTIME_H
