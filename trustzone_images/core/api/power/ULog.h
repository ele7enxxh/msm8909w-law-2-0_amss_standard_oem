#ifndef ULOG_H
#define ULOG_H

/*============================================================================
  @file ULog.h

  Mainnterface for ULog.

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/power/ULog.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "DALStdDef.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

//#define ULOG_HANDLE_FROM_DAL_HANDLE(x)    (x)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef  void *   ULogHandle;
typedef  int32    ULogResult;

// Error Codes
// 
// These Error codes are return values from ULog functions.  ULog 
// functions additionally use the standard DAL return codes.
//
typedef enum
{
  ULOG_ERR_INVALIDNAME = 1,       // The name provided is invalid.
  ULOG_ERR_INVALIDPARAMETER,      // A parameter provided is invalid.
  ULOG_ERR_ALREADYCONNECTED,      // The DAL Handle is already 
                                  // connected to a log.
  ULOG_ERR_ALREADYCREATED,        // The log has already been created.
  ULOG_ERR_NOTCONNECTED,          // The DAL Handle is not connected 
                                  // to a log.
  ULOG_ERR_ALREADYENABLED,
  ULOG_ERR_INITINCOMPLETE,
  ULOG_ERR_READONLY,
  ULOG_ERR_INVALIDHANDLE,
} ULOG_ERRORS;

// Buffer Sizes that can be queried in order to manually
// allocate memory for a log.
// 
// See: ULogCore_ValueQuery
//
typedef enum
{
  // These queries do not require a log handle
  ULOG_VALUE_BUFFER,
  ULOG_VALUE_SHARED_HEADER,
  ULOG_VALUE_READCORE,
  ULOG_VALUE_WRITECORE,
  ULOG_VALUE_LOCKCORE,

  // Below here the queries require a log handle
  //
  ULOG_VALUE_LOG_BASE = 0x100,
  ULOG_VALUE_LOG_READFLAGS = ULOG_VALUE_LOG_BASE,
} ULOG_VALUE_ID;

// Lock Types
//
// Each log can select the locking mechanism used to protect data 
// accesses.  It is up to the log definer to select the appropriate 
// lock type depending on the use cases.
// 
typedef enum
{
  ULOG_LOCK_NONE = 0,              // No locking is provided for this 
                                   // log.  The user is responsible for 
                                   // providing mutual exclusion if logging
                                   // from multiple threads.

  ULOG_LOCK_OS,                    // An OS lock is provided.  The log can
                                   // be written from more than one thread.
                                   // OS locks are assumed to work across 
                                   // cores in the case where one OS is 
                                   // managing all cores.

  ULOG_LOCK_SPIN,                  // Spinlocks are NOT currently supported
                                   // and an ERR_FATAL will result if a log
                                   // is created with this type of locking.
                                   // 
                                   // They are intended to be supported in 
                                   // future releases.
                                   
  ULOG_LOCK_INT,                   // An interrupt lock is provided.  The log can
                                   // be written from more than one thread.
                                   // Interrupt locks are NOT multi-core safe.

  ULOG_LOCK_UINT32 = 0x7FFFFFFF    // force enum to 32-bits 
} ULOG_LOCK_TYPE;

// Memory Type
//
// Control how memory is provided for the log.
// 
typedef enum
{
  ULOG_ALLOC_LOCAL = 0x01,         // Memory will be allocated using
                                   // DALSYS_Malloc.

  ULOG_ALLOC_MANUAL = 0x04,        // Memory allocation is handled 
                                   // manually, and is provided to ULog
                                   // through the DalULog_MemoryAssign
                                   // function.

  ULOG_ALLOC_TYPE_MASK = 0xFF,     // Mask for all possible allocation bit 
                                   // types.

  ULOG_ALLOC_UINT32 = 0x7FFFFFFF    // force enum to 32-bits 
} ULOG_ALLOC_TYPE;

// Memory Usage
//
// Control the amount and type of memory access available to the log.
// 
typedef enum
{
  ULOG_MEMORY_USAGE_FULLACCESS = 0x0300,    // The processor is expected to read and
                                            // write from this log.

  ULOG_MEMORY_USAGE_READABLE = 0x0100,      // The processor will only read from this
                                            // log.

  ULOG_MEMORY_USAGE_WRITEABLE = 0x0200,     // The processor will only write to this 
                                            // log.

  ULOG_MEMORY_USAGE_UINT32 = 0x7FFFFFFF    // force enum to 32-bits 
} ULOG_MEMORY_USAGE_TYPE;

// Memory Configuration
// 
// Control if the memory is configured and used as shared
//
typedef enum
{
  ULOG_MEMORY_CONFIG_LOCAL = 0x010000,        // Header information is stored in local
                                              // memory.  Because the log is used only
                                              // by one processor (or a symmetrical 
                                              // multi-processing system), there is no
                                              // need to store the log in any other
                                              // location.

  ULOG_MEMORY_CONFIG_SHARED = 0x020000,       // Header information is stored in the
                                              // allocated buffer memory.  Because the log
                                              // is shared across multiple processors,
                                              // it is necessary to share this basic 
                                              // information.

  ULOG_MEMORY_CONFIG_UINT32 = 0x7FFFFFFF    // force enum to 32-bits 
} ULOG_MEMORY_CONFIG_TYPE;


// Memory type
//
// Memory type is a parameter specifying the allocation, usage, and configuration
// of the memory used by the log.  Each log must specify one of each of the 
// ULOG_ALLOC_TYPE, ULOG_MEMORY_USAGE_TYPE, and ULOG_MEMORY_CONFIG_TYPE values.
//
// It is up to the log creator to select the appropriate memory type.
// 
// Standard memory allocation types are provided below.
//
// Most logs will be created as local logs.
#define ULOG_MEMORY_LOCAL      (ULOG_MEMORY_CONFIG_LOCAL | ULOG_MEMORY_USAGE_FULLACCESS | ULOG_ALLOC_LOCAL)
#define ULOG_MEMORY_SHARED     (ULOG_MEMORY_CONFIG_SHARED | ULOG_MEMORY_USAGE_FULLACCESS | ULOG_ALLOC_MANUAL)


// Interface Types
//
// Interface Types are used by the logging core to allow only 
// like-formatted data into a given log buffer.  It is recommended 
// that data is only logged using the Frontends.
// 
typedef enum
{
  ULOG_INTERFACE_INVALID = 0,      // Invalid Log Type

  ULOG_INTERFACE_RAW,              // Raw data logging provided in
                                   // ULogFront.h

  ULOG_INTERFACE_REALTIME,         // Real-Time data logging provided in
                                   // ULogFront.h.

  ULOG_INTERFACE_UINT32 = 0x7FFFFFFF    // force enum to 32-bits 
} ULOG_INTERFACE_TYPE;

// Log Status Indicators
//
// Log status is returned as the first word from the DalULog_Read function.
//  
typedef enum
{
  ULOG_ERR_OVERFLOW = 0x0001,        // An overflow occurred.

  ULOG_ERR_LARGEMSG = 0x0002,        // A message too large for the buffer 
                                     // was logged, and therefore was dropped.
                                     // This indicates the message was too 
                                     // large for the entire buffer.

  ULOG_ERR_LARGEMSGOUT = 0x0004,     // A message too large for the output 
                                     // buffer was dropped because it could
                                     // not fit.

  ULOG_ERR_UINT32 = 0x7FFFFFFF    // force enum to 32-bits 
} ULOG_ERR_TYPE;

// ReadFlags Indicators
//
// ReadFlags is returned from a query.
//  
typedef enum
{
  ULOG_RD_FLAG_REWIND = 0x0001,        // The read pointer was rewound
                                       // by the Autorewind feature.

  ULOG_RD_FLAG_UINT32 = 0x7FFFFFFF    // force enum to 32-bits 
} ULOG_RD_FLAG_TYPE;


// Attribute Types
//
// Logs can be customized with the following attributes.
// 
typedef enum
{
  ULOG_ATTRIBUTE_READ_AUTOREWIND = 0x00000001, // Rewind the read pointer for this log
                                               // to the earliest message after the entire
                                               // log has been read, and zero bytes have 
                                               // been transferred.

  ULOG_ATTRIBUTE_UINT32 = 0x7FFFFFFF    // force enum to 32-bits 
} ULOG_ATTRIBUTE_TYPE;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/* Small System Interface */

/**
 @brief ULogCore_Init - Initialize the ULog System.
 
 It is not strictly necessary to call this function since it
 will be executed any time an attempt is made to connect to, or
 create a log.
 
 @return DAL_SUCCESS if the initialization is successful.
 */
ULogResult ULogCore_Init (void);

/**
 @brief ULogCore_LogCreate - Create a new log.
 
 Create a new log, and add it to the logs managed by the ULog
 subsystem.
 
 @param h  : A ULog handle.
 @param logName : The log name (24 chars max, including NULL).
 @param bufferSize : The size in bytes of the log
                          buffer.
 @param memoryType : The memory configuration.
 @param lockType : The type of locking required.
 @param interfaceType : Which style of data will be logged.

 @return DAL_SUCCESS if the create completed successfully.
         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_LogCreate (ULogHandle * h,  
                               const char *  logName, 
                               uint32  bufferSize, 
                               uint32  memoryType, 
                               ULOG_LOCK_TYPE  lockType, 
                               ULOG_INTERFACE_TYPE  interfaceType);

/**
 @brief ULogCore_HeaderSet - Set the header for the log.
 
 Apply the supplied string to a given log header.  The header
 is stored with a log, and will not be overwritten unless this
 function is called again.
 
 There are no real limitations to this string.  The system
 determines the length of the supplied string, allocates that
 amount of memory, and copies the string to it.
 
 @param h  : A ULog handle.
 @param headerText : The string to store in the log.
 
 @return DAL_SUCCESS if the header was sucessfully stored.
 */
ULogResult ULogCore_HeaderSet(ULogHandle h, 
                              char * headerText);

/**
 @brief ULogCore_AttributeSet - Set the log attributes

 Or the provided attribute bits into the logs attribute mask.

 @param h : The log to set attributes for
 @param attribute : The attributes to set
 @return A handle to the named log
 */
void ULogCore_AttributeSet (ULogHandle h, uint32 attribute);

/**
 @brief ULogCore_MemoryAssign - Assign memory to specific log
        buffers.
 
 When a log is created with the allocation type
 ULOG_ALLOC_MANUAL, the buffers are and must be manually
 assigned with this function.  All supplied buffers must be word
 aligned.  If a buffer is not word aligned, this function will
 fail.
 
 @param h  : A ULog handle.
 @param id : Which buffer to assign.
 @param bufferPtr : A pointer to the buffer memory to assign.
                  Note that ALL buffers must be word aligned.
 @param bufferSize : The size in bytes of the buffer.

 @return DAL_SUCCESS if the memory was assigned successfully.
         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_MemoryAssign (ULogHandle h, 
                                  ULOG_VALUE_ID id, 
                                  void * bufferPtr, 
                                  uint32  bufferSize);

/**
 @brief ULogCore_Enable - Enable a log that is already defined.
 
 Enable a log.  A log can need manual enabling if it has manual
 allocation, or if buffer memory was never assigned.
 
 @param h  : A ULog handle.

 @return DAL_SUCCESS if the log was enabled successfully.
         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_Enable (ULogHandle h);

/**
 @brief ULogCore_IsEnabled - Check if a log is enabled.
 
 Determine if a log is enabled.
 
 @param h  : A ULog handle.

 @return TRUE if the log is enabled.
 */
DALBOOL ULogCore_IsEnabled (ULogHandle h);

/**
 @brief ULogCore_Write - Write a message to a log.
 
 This is the base log write function.  It takes complete
 messages only, and writes them to the log if the interface type
 of the message matches the interface type specified at
 creation.
 
 @param h  : A ULog handle.
 @param msgCount : The number of bytes in the message.
 @param msgContent : A pointer to the message data.
 @param interfaceType : The type of interface this message is
                      formatted for.

 @return DAL_SUCCESS if the log was successfully written.
         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_Write (ULogHandle h,  
                           uint32 msgCount,  
                           char * msgContent, 
                           ULOG_INTERFACE_TYPE interfaceType);

/**
 @brief ULogCore_Query - Query information about a specific log,
        or the logging system.
 
 This function will return the expected sizes of internal log
 buffers, in order to manually allocate the buffers.  Use a NULL
 handle for these values.
 
 @param h  : A ULog handle.
 @param id : A log buffer ID.
 @param value : The returned number of bytes needed by the
              selected buffer.

 @return DAL_SUCCESS if the buffer id was known, and supported.
         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_Query(ULogHandle h,
                          ULOG_VALUE_ID id,
                          uint32 * value);

/**
 @brief ULogCore_ValueQuery - Query the expected sizes of
        internal buffers.
 
 This function will return the expected sizes of internal log
 buffers, in order to manually allocate the buffers.
 
 @param id : A log buffer ID.
 @param value : The returned number of bytes needed by the
              selected buffer.

 @return DAL_SUCCESS if the buffer id was known, and supported.
         Errors are defined in the ULOG_ERRORS enum.
 */
#define ULogCore_ValueQuery(id, value)     ULogCore_Query(NULL, id, value)

/* Full System Interface extensions to Small System */

/**
 @brief ULogCore_TransactionStart - Start a message write
        transaction.
 
 Some messages are more convenient to write in multiple blocks.
 This interface starts a message write transaction, to enable
 the feature.  If this function returns FALSE, then the
 transaction is NOT started, and the Write / Commit functions
 should not be called.
 
 Once started, a transaction must be Committed.  
 
 @param h : A ULog handle.
 @param maxMsgSize : The maximum number of bytes that will be
                   used by this transaction.
 @param interfaceType : The type of interface this message is
                      formatted for.

 @return TRUE if the transaction was started.
 */
DALBOOL ULogCore_TransactionStart (ULogHandle h,
                                   uint32 maxMsgSize,
                                   ULOG_INTERFACE_TYPE interfaceType);

/**
 @brief ULogCore_TransactionWrite - Write data to a log assuming
        a valid transaction has been started.
 
 Write one block of data to the open transaction of a log.
 
 @param h : A ULog handle.
 @param msgCount : The amount of data to write.
 @param msgContent : The data to write.
 */
void ULogCore_TransactionWrite (ULogHandle h,  
                                uint32 msgCount,  
                                char * msgContent);

/**
 @brief ULogCore_TransactionCommit - Commit a transaction to the
        log.
 
 Commit the current transaction to the log.  Once a transaction
 is started, it must be committed.
 
 @param h : A ULog handle.
 */
void ULogCore_TransactionCommit (ULogHandle h);

/**
 @brief ULogCore_Read - Read data from the log buffer.
 
 Read data from the log buffer.  This data is unformatted. Call
 ULogCore_MsgFormat to format each message.  Only full messages
 are placed in the output buffer, thus it is incorrect to use
 outputCount != outputSize as an indicator that the log buffer
 is empty.
 
 An empty log buffer will yield a read of only status, and an
 outputCount = 4.
 
 @param h : A ULog handle of the log to read.
 @param outputSize : Size of the output buffer.
 @param outputMem : A pointer to the output buffer.
 @param outputCount : The amount of data placed in the output
                  buffer.

 @return DAL_SUCCESS if the read completed.
 */
ULogResult ULogCore_Read (ULogHandle h,  
                          uint32  outputSize,  
                          char *  outputMem,  
                          uint32 *  outputCount);

/**
 @brief ULogCore_ReadEx - Read data from the log buffer.
 
 Read data from the log buffer.  See ULogCore_Read for
 description.  Allows additional control of limited number of
 messages read.
 
 @param h : A ULog handle of the log to read.
 @param outputSize : Size of the output buffer.
 @param outputMem : A pointer to the output buffer.
 @param outputCount : The amount of data placed in the output
                  buffer.
 @param outputMessageLimit : The maximum number of messages to
                           read from the buffer.

 @return DAL_SUCCESS if the read completed.
 */
ULogResult ULogCore_ReadEx (ULogHandle h,  
                            uint32  outputSize,  
                            char *  outputMem,  
                            uint32 *  outputCount,
                            uint32 outputMessageLimit);

/**
 @brief ULogCore_Allocate - Allocate memory to the log buffer.
 
 When a log is created with no output buffer, this function can
 be used to allocate that memory.  It is intended for use from
 applications enabling and reading logs from the system.
 
 This only works for logs created with ULOG_ALLOC_LOCAL
 allocation type.
 
 @param h : A ULog handle of the log to read.
 @param bufferSize : Size of the circular buffer to allocate.

 @return DAL_SUCCESS if the memory was allocated successfully.
         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_Allocate (ULogHandle h, 
                              uint32  bufferSize);


/**
 @brief ULogCore_Disable - Disable a log.
 
 Disable a log, and cause any and message writes to drop the
 message in the bit bucket.
 
 @param h : A ULog handle.

 @return DAL_SUCCESS if the log was disabled successfully.
         Errors are defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_Disable (ULogHandle h);

/**
 @brief ULogCore_List - List the available logs.
 
 Create a list of all logs available in the system, and output
 it as a text string.
 
 @param registeredCount : The number of logs that are
                        registered.
 @param nameArraySize : The size of the array that receives log
                      names.
 @param namesReadCount : The number of log names that were
                       actually placed in the nameArray.
 @param nameArray : The buffer to receive log name data.

 @return DAL_SUCCESS if the list was successfull. Errors are
         defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_List (uint32 *  registeredCount,  
                          uint32  nameArraySize,  
                          uint32 *  namesReadCount,  
                          char *  nameArray);

/**
 @brief ULogCore_List - List the available logs.
 
 Create a list of all logs available in the system, and output
 it as a text string.  Offset into the list by the provided
 amount.
 
 @param offsetCount : The number of log names to skip before
                   returning log name data.
 @param registeredCount : The number of logs that are
                        registered.
 @param nameArraySize : The size of the array that receives log
                      names.
 @param namesReadCount : The number of log names that were
                       actually placed in the nameArray.
 @param nameArray : The buffer to receive log name data.

 @return DAL_SUCCESS if the list was successfull. Errors are
         defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_ListEx (uint32 offsetCount,
                            uint32 *  registeredCount,  
                            uint32  nameArraySize,  
                            uint32 *  namesReadCount,  
                            char *  nameArray);

/**
 @brief ULogCore_MsgFormat - Format an individual message
        retrieved with the ULogCore_Read function.
 
 Format and output an individual message read with the
 ULogCore_Read function.
 
 @param h : A ULog handle to the log we are formatting.
 @param msg : The input message.
 @param msgString : The output string.
 @param msgStringSize : The size of the output string
 @param msgConsumed : The number of bytes consumed from the
                    original msg buffer.

 @return DAL_SUCCESS if the format was successful. Errors are
         defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_MsgFormat (ULogHandle h,
                               char *  msg,  
                               char *  msgString, 
                               uint32 msgStringSize, 
                               uint32 * msgConsumed);

/**
 @brief ULogCore_StatusFormat - Format the status value from a
        log read.
 
 Format the status value for a log, retrieved through the
 ULogCore_Read function.
 
 @param status : The value to format.
 @param statusString : The output string.
 @param statusStringSize : The size of the output string

 @return DAL_SUCCESS if the format was successful. Errors are
         defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_StatusFormat (uint32 status,  
                                  char * statusString, 
                                  uint32 statusStringSize);

/**
 @brief ULogCore_HeaderRead - Read the log header.
 
 Read the header from the supplied log.
 
 @param h : A handle of the log to access.
 @param statusString : The output string.
 @param statusStringSize : The size of the output string

 @return DAL_SUCCESS if the format was successful. Errors are
         defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_HeaderRead (ULogHandle h,  
                                uint32 headerReadOffset,
                                char * headerString, 
                                uint32 headerStringSize,
                                uint32 * headerActualLength);

/**
 @brief ULogCore_Connect - Connect a log handle to a log defined
        elsewhere.
 
 Connect a log handle to a log that has been, or will be defined
 elsewhere.  A successful return allows logging through the
 handle, although no data will be written to a physical log
 until the log is defined with the ULogCore_Create function.
 
 @param h : A ULog handle.
 @param logName : The name of the log to connect with.

 @return DAL_SUCCESS if the connect was successful. Errors are
         defined in the ULOG_ERRORS enum.
 */
ULogResult ULogCore_Connect (ULogHandle * h,  
                             const char * logName);


/**
 @brief ULogCore_LogLevelSet - Set the log level.
 
 ULog does not filter messages, but this function is
 intended to facilitate filtering without requiring it. This
 simply sets a parameter in a log that can be used to filter log
 messages, along with the provided macros.
 
 @param h : A ULog handle.
 @param level : The new level for the log.

 @return NONE.
 */
void ULogCore_LogLevelSet(ULogHandle h, uint32 level);

/**
 @brief ULogCore_LogLevelGet - Get the log level.
 
 ULog does not filter messages, but this function is
 intended to facilitate filtering without requiring it. This
 simply returns a parameter in a log that can be used to filter
 log messages, along with the provided macros.
 
 @param h : A ULog handle.

 @return The current log value.  0 if the log handle is NULL.
 */
uint32 ULogCore_LogLevelGet(ULogHandle h);

/**
 @brief ULOG_FILTER_CMD - Filter a log command based on the log
        level.
 
 ULog does not filter messages, but this macro is intended to
 facilitate filtering without requiring it. This macro compares
 the provided level against the level of the provided log.  If
 the provided level is <= the log level then the provided
 function is executed.
 
 @param handle : A ULog handle.  The level is retrieved from
               this handle.
 @param level : The level of this command.
 @param function : The function to execute if the levels
                 indicate acceptance.

 @return The current log value.  0 if the log handle is NULL.
 */
#define ULOG_FILTER_CMD(handle, level, function)     if (level <= ULogCore_LogLevelGet(handle)) { function }


/* Reference Buffer Extensions to Full Interface */
//#include "CoreRef.h"

//CoreReference * ULogCore_ReferenceCreate ( ULogHandle * h );

//void ULogCore_ReferenceFree ( CoreReference * ref );

/**
 @brief ULogCore_TeeAdd - Group handles together.

 Group handles together so that any logging information will be copied to 
 multiple logs.

 Note: the first handle may points to an updated object after a call to 
 this function, whereas the second one will not.

 @param h1 : A ULog handle, which may be one to a single or a group of logs
 @param h2 : A ULog handle, which may be one to a single or a group of logs
             to be added to h1
 @return A handle to the grouped logs
 */
ULogHandle ULogCore_TeeAdd (ULogHandle h1,
                            ULogHandle h2);

/**
 @brief ULogCore_TeeRemove - Remove some handle(s)

 Note: the first handle may points to an updated object after a call to 
 this function, whereas the second one will not.

 @param h1 : A ULog handle, which may be one to a single or a group of logs
 @param h2 : A ULog handle, which may be one to a single or a group of logs
             to be removed from h1
 @return A handle to the resulting group of logs
 */
ULogHandle ULogCore_TeeRemove (ULogHandle h1,
                               ULogHandle h2);

/**
 @brief ULogCore_HandleGet - Get a log handle by its name.

 @param logName : name of a ULog
 @return A handle to the named log
 */
ULogHandle ULogCore_HandleGet (char *logName);

/**
 @brief ULogCore_IsLogPresent - check if a named log is present in a log set

 @param h : A ULog handle, which may be a handle to a single or a group of logs
 @param logName : name of a ULog
 @return TRUE if the named log is present in the log(s) indicated by the handle
         h, and FALSE otherwise
 */
DALBOOL ULogCore_IsLogPresent(ULogHandle h, char *logName);

#endif // ULOG_H
