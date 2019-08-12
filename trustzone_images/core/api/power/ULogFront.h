#ifndef ULOGFRONT_H
#define ULOGFRONT_H

/*============================================================================
  @file ULogFront.h

  Frontend interfaces for ULog Core.  Simplified access to the logging mechanism.

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/power/ULogFront.h#1 $ */


/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "ULog.h"
#include "stdarg.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/* Documentation for these functions is forthcoming. */

/*----------------------------------------------------------------------------
 * ULOG_INTERFACE_RAW
 * -------------------------------------------------------------------------*/

/**
 @brief ULogFront_RawInit - Create a new log in the RAW format.
 
 Create a new raw log, See ULogCore_LogCreate for details.
 
 @param h  : See ULogCore_LogCreate.
 @param logName : See ULogCore_LogCreate.
 @param bufferSize : See ULogCore_LogCreate.
 @param memoryType : See ULogCore_LogCreate.
 @param lockType : See ULogCore_LogCreate.

 @return See ULogCore_LogCreate.
*/
ULogResult ULogFront_RawInit(ULogHandle * h, 
                             const char * name,
                             uint32 logBufSize,
                             uint32 logBufMemType,
                             ULOG_LOCK_TYPE logLockType);

/**
 @brief ULogFront_RawLog - Log raw data.
 
 Log data to a raw log.
 
 @param h  : A ULog handle.
 @param dataArray : A pointer to the data to log.
 @param dataCount : The amount of data to log.

 @return DAL_SUCCESS if the message was logged successfully.
         Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RawLog(ULogHandle h, 
                            char * dataArray,
                            uint32 dataCount);

/*----------------------------------------------------------------------------
 * ULOG_INTERFACE_REALTIME
 * -------------------------------------------------------------------------*/

#define ULOG_RT_FLOAT(x) (*((uint32 *)((void *)&x)))

/**
 @brief ULogFront_RealTimeInit - Create a new log in the
        RealTime format.
 
 Create a new RealTime log, See ULogCore_LogCreate for details.
 
 @param h  : See ULogCore_LogCreate.
 @param logName : See ULogCore_LogCreate.
 @param bufferSize : See ULogCore_LogCreate.
 @param memoryType : See ULogCore_LogCreate.
 @param lockType : See ULogCore_LogCreate.

 @return See ULogCore_LogCreate.
*/
ULogResult ULogFront_RealTimeInit(ULogHandle * h, 
                             const char * name,
                             uint32 logBufSize,
                             uint32 logBufMemType,
                             ULOG_LOCK_TYPE logLockType);

/**
 @brief ULogFront_RealTimePrintf - Log data in the printf
        format.
 
 Log printf data to a RealTime log.  The printf is not executed
 until the log is pulled.  This makes a very performant call,
 but also means all strings must be present when the log is
 read.
 
 @param h  : A ULog handle.
 @param dataArray : the number of parameters being printed (Not
                  including the formatStr).  Limited to 10
                  parameters.
 @param formatStr : The format string for the printf.
 @param ... : The parameters to print.

 @return DAL_SUCCESS if the message was logged successfully.
         Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimePrintf(ULogHandle h, 
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
             ULogFront_RealTimePrintf(h, 0, formatStr)
#define ULOG_RT_PRINTF_1(h, formatStr, p1)    \
             ULogFront_RealTimePrintf(h, 1, formatStr, p1)
#define ULOG_RT_PRINTF_2(h, formatStr, p1, p2)    \
             ULogFront_RealTimePrintf(h, 2, formatStr, p1, p2)
#define ULOG_RT_PRINTF_3(h, formatStr, p1, p2, p3)    \
             ULogFront_RealTimePrintf(h, 3, formatStr, p1, p2, p3)
#define ULOG_RT_PRINTF_4(h, formatStr, p1, p2, p3, p4)    \
             ULogFront_RealTimePrintf(h, 4, formatStr, p1, p2, p3, p4)
#define ULOG_RT_PRINTF_5(h, formatStr, p1, p2, p3, p4, p5)    \
             ULogFront_RealTimePrintf(h, 5, formatStr, p1, p2, p3, p4, p5)
#define ULOG_RT_PRINTF_6(h, formatStr, p1, p2, p3, p4, p5, p6)    \
             ULogFront_RealTimePrintf(h, 6, formatStr, p1, p2, p3, p4, p5, p6)
#define ULOG_RT_PRINTF_7(h, formatStr, p1, p2, p3, p4, p5, p6, p7)    \
             ULogFront_RealTimePrintf(h, 7, formatStr, p1, p2, p3, p4, p5, p6, p7)
#define ULOG_RT_PRINTF_8(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)    \
             ULogFront_RealTimePrintf(h, 8, formatStr, p1, p2, p3, p4, p5, p6, p7, p8)
#define ULOG_RT_PRINTF_9(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)    \
             ULogFront_RealTimePrintf(h, 9, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define ULOG_RT_PRINTF_10(h, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)    \
             ULogFront_RealTimePrintf(h, 10, formatStr, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)

/**
 @brief ULogFront_RealTimeVprintf - Log data in the vprintf
        format.
 
 Log printf data to a RealTime log.  The printf is not executed
 until the log is pulled.  This makes a very performant call,
 but also means all strings must be present when the log is
 read.

 @note This function serves identical purpose to ULogFront_RealTimePrintf,
       except it takes a va_list argument instead of being variadic.
 
 @param h         : A ULog handle.
 @param dataCount : the number of parameters being printed (Not
                    including the formatStr).  Limited to 10
                    parameters.
 @param formatStr : The format string for the printf.
 @param ap        : A va_list of the parameters being printed.

 @return DAL_SUCCESS if the message was logged successfully.
         Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeVprintf(ULogHandle h, 
                                     uint32 dataCount, 
                                     const char * formatStr,
                                     va_list ap);

/**
 @brief ULogFront_RealTimeData - Log uint32 parameters.
 
 Log uint32 data to a RealTime log.  Data is provided as
 variatic arguments to this function.
 
 @param h  : A ULog handle.
 @param dataCount : the number of parameters being logged.  Can
                  be a maximum of 10.
 @param ... : The parameters to log.

 @return DAL_SUCCESS if the message was logged successfully.
         Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeData(ULogHandle h, 
                                  uint32 dataCount, 
                                  ...);


/**
 @brief ULOG_RT_DATA_N - Macros to provide some parameter count
        safety to the ULogFront_RealTimeData function.

 These macros provide parameter count safety to the
 ULogFront_RealTimeData function.  They can be used instead of
 calling the function directly.

 @param h : A handle indicating where to log.
 @param p1 - pN : The data to log.  These must be (u)int32 in
            length.  Smaller data types will be automatically
            promoted.
 */
#define ULOG_RT_DATA_1(h, p1)    \
             ULogFront_RealTimeData(h, 1, p1)
#define ULOG_RT_DATA_2(h, p1, p2)    \
             ULogFront_RealTimeData(h, 2, p1, p2)
#define ULOG_RT_DATA_3(h, p1, p2, p3)    \
             ULogFront_RealTimeData(h, 3, p1, p2, p3)
#define ULOG_RT_DATA_4(h, p1, p2, p3, p4)    \
             ULogFront_RealTimeData(h, 4, p1, p2, p3, p4)
#define ULOG_RT_DATA_5(h, p1, p2, p3, p4, p5)    \
             ULogFront_RealTimeData(h, 5, p1, p2, p3, p4, p5)
#define ULOG_RT_DATA_6(h, p1, p2, p3, p4, p5, p6)    \
             ULogFront_RealTimeData(h, 6, p1, p2, p3, p4, p5, p6)
#define ULOG_RT_DATA_7(h, p1, p2, p3, p4, p5, p6, p7)    \
             ULogFront_RealTimeData(h, 7, p1, p2, p3, p4, p5, p6, p7)
#define ULOG_RT_DATA_8(h, p1, p2, p3, p4, p5, p6, p7, p8)    \
             ULogFront_RealTimeData(h, 8, p1, p2, p3, p4, p5, p6, p7, p8)
#define ULOG_RT_DATA_9(h, p1, p2, p3, p4, p5, p6, p7, p8, p9)    \
             ULogFront_RealTimeData(h, 9, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define ULOG_RT_DATA_10(h, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)    \
             ULogFront_RealTimeData(h, 10, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)

/**
 @brief ULogFront_RealTimeCharArray - Log byte data.
 
 Log byte data to a RealTime log.  
 
 @param h  : A ULog handle.
 @param byteCount : the number of bytes to log.
 @param byteData : A pointer to the data to log.

 @return DAL_SUCCESS if the message was logged successfully.
         Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeCharArray(ULogHandle h, 
                                       uint32 byteCount, 
                                       char * byteData);

/**
 @brief ULogFront_RealTimeString - Log a full string.
 
 Log string data to a RealTime log.  This function copies the
 full string to the log.
 
 @param h  : A ULog handle.
 @param string : The string to log.

 @return DAL_SUCCESS if the message was logged successfully.
         Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeString(ULogHandle h, 
                                    char * cStr);

/**
 @brief ULogFront_RealTimeWordArray - Log word data.
 
 Log word data to a RealTime log.
 
 @param h  : A ULog handle.
 @param wordCount : the number of words to log.
 @param wordData : A pointer to the data to log.

 @return DAL_SUCCESS if the message was logged successfully.
         Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeWordArray(ULogHandle h, 
                                       uint32 wordCount, 
                                       uint32 * wordData);

/**
 @brief ULogFront_RealTimeCsv - Log word data.
 
 Log word data to a RealTime log.  Output will be a CSV
 compatible format.
 
 @param h  : A ULog handle.
 @param wordCount : the number of words to log.
 @param wordData : A pointer to the data to log.

 @return DAL_SUCCESS if the message was logged successfully.
         Errors are defined in the ULOG_ERRORS enum.
*/
ULogResult ULogFront_RealTimeCsv(ULogHandle h, 
                                 uint32 wordCount, 
                                 uint32 * wordData);

#endif // ULOGFRONT_H

