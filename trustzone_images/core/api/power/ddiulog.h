#ifndef __DALIULOG_H__
#define __DALIULOG_H__

/*============================================================================
  @file ddiulog.h

  Interface file to the ULog DAL.

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/power/ddiulog.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "DalDevice.h"
#include "ULog.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define DALULOG_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

//
// DAL created types and structures.
// 
typedef struct DalULog DalULog;
struct DalULog
{
  struct DalDevice DalDevice;
  DALResult (*Connect)(DalDeviceHandle * _h,  const char *  logName);
  DALResult (*Create)(DalDeviceHandle * _h,  const char *  logName, uint32  bufferSize, uint32  memoryType, ULOG_LOCK_TYPE  lockType, ULOG_INTERFACE_TYPE  interfaceType);
  DALResult (*Log)(DalDeviceHandle * _h,  uint32  msgCount,  char *  msgContent, uint32  reserved);
  DALResult (*Read)(DalDeviceHandle * _h,  uint32  outputSize,  char *  outputMem,  uint32 *  outputCount);
  DALResult (*Status)(DalDeviceHandle * _h, uint32 *  bufStatus);
  DALResult (*Allocate)(DalDeviceHandle * _h, uint32  bufferSize);
  DALResult (*Enable)(DalDeviceHandle * _h);
  DALResult (*Disable)(DalDeviceHandle * _h);
  DALResult (*List)(DalDeviceHandle * _h,  uint32 *  registeredCount,  uint32  nameArraySize,  uint32 *  namesReadCount,  char *  nameArray);
  DALResult (*FormatMsg)(DalDeviceHandle * _h,  char *  msg,  char *  msgString, uint32 msgStringSize, uint32 * msgConsumed); 
  DALResult (*FormatStatus)(DalDeviceHandle * _h,  uint32 status,  char * statusString, uint32 statusStringSize); 
  DALResult (*ValueQuery)(DalDeviceHandle * _h, ULOG_VALUE_ID id, uint32 * value);
  DALResult (*MemoryAssign)(DalDeviceHandle * _h, ULOG_VALUE_ID id, void * bufferPtr, uint32  bufferSize);
  DALResult (*EnabledCheck)( DalDeviceHandle * _h, uint32 * enabled);
};

typedef struct DalULogHandle DalULogHandle; 
struct DalULogHandle 
{
  uint32 dwDalHandleId;
  const DalULog * pVtbl;
  void * pClientCtxt;
};



/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/


#define DAL_ULogDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALULOG_INTERFACE_VERSION,hDalDevice)

/**
 @brief DalULog_Connect - Connect to a log.

 Connect to a log.  This function should be used to connect to a
 log that is / will be created elsewhere.
 
 
 @param _h  : A log DAL handle.
 @param name : The name of the log to connect with.  Log
               names are 24 characters maximum, including the
               null terminator.

 @return DAL_SUCCESS if connection was made.
 */
static __inline DALResult DalULog_Connect(DalDeviceHandle * _h,  
                                          const char *  name)
{
  return((DalULogHandle *)_h)->pVtbl->Connect( _h, 
                                               name);
}

/**
 @brief DalULog_Create - Create a log.
 
 Create a log with the provided information.  When finished, the
 log will exist and be fully defined.  Memory will only be
 allocated per the buffer size comment below.
 
 @param _h  : A log DAL handle (retrieved with DAL_DeviceAttach)
 @param name : The name of the log to define.  Log
               names are 24 characters maximum, including the
               null terminator.
 @param bufferSize : The size of memory to allocate bytes.  If
                   this field is set to zero, the log will not
                   be enabled and no memory will be allocated.
                   Memory is always allocated in powers of 2,
                   and the value provided will be rounded down
                   to the nearest value.
 @param memoryType : The type of memory to allocate.
 @param lockType : The type of lock required.
 @param interfaceType : The interface type this log supports.

 @return DAL_SUCCESS if the log is now defined.
 */
static __inline DALResult DalULog_Create(DalDeviceHandle * _h,
                                         const char *  name,
                                         uint32  bufferSize, 
                                         uint32  memoryType, 
                                         ULOG_LOCK_TYPE  lockType, 
                                         ULOG_INTERFACE_TYPE  interfaceType)
{
  return((DalULogHandle *)_h)->pVtbl->Create( _h, 
                                              name,
                                              bufferSize, 
                                              memoryType, 
                                              lockType, 
                                              interfaceType);
}

/**
 @brief DalULog_Log - Log data.

 Add a log message to the selected log.  If log data is not
 stored for a normal reason such as the log being disabled, it
 is considered a success.
 
 It is recommended that the frontend library be used for all
 logging activity.
 
 @param _h  : A log DAL handle.
 @param msgContent : An array of bytes to log.
 @param msgCount : The number of bytes provided in
                  msgContent.
 @param interfaceType : Indicates the internal formatting of
                      this message.
 @return DAL_SUCCESS if the message was parsed correctly.
 */
static __inline DALResult DalULog_Log(DalDeviceHandle * _h,  
                                      char * msgContent, 
                                      uint32  msgCount,  
                                      uint32  interfaceType)
{
  return((DalULogHandle *)_h)->pVtbl->Log( _h, 
                                           msgCount, 
                                           msgContent, 
                                           interfaceType);
}

/**
 @brief DalULog_Read - Read data from a log.

 Read data from the selected log.  In order to read from a log
 you must first connect to it.  Only complete messages are put
 into the read buffer.  All messages are padded so the message
 ends on a word boundary.
 
 The format of outputMem will always be:
 <Log Stat(32)> <Msg Len(32)> <MsgData1(8)> ... <MsgDataN(8)>
                <Msg Len(32)> <MsgData1(8)> ... <MsgDataN(8)>
                <Msg Len(32)> <MsgData1(8)> ... <MsgDataN(8)>
 
 Each message is formatted individually as per the
 ULOG_OUTPUT_FORMATS enum.
 
 @param _h  : A log DAL handle.
 @param outputMem : An array to put log data into.
 @param outputSize : The size of the read output array in
                     bytes.
 @param outputCount : The number of bytes written into
                      the outputMem.

 @return DAL_SUCCESS if the read completed.
 */
// bjs how do I make sure the outputMem is aligned?  Or must I handle it
// internally?
static __inline DALResult DalULog_Read(DalDeviceHandle * _h,  
                                       char *  outputMem,  
                                       uint32  outputSize,  
                                       uint32 *  outputCount)
{
  return((DalULogHandle *)_h)->pVtbl->Read( _h, 
                                            outputSize, 
                                            outputMem, 
                                            outputCount);
}

/**
 @brief DalULog_Allocate - Allocate memory for a log.

 Allocate memory for the selected log.  

 If memory has been previously allocated, this function will 
 free that memory first.  The new memory will be allocated 
 and assigned to the buffer.

 If the new buffer size is zero, no memory will be allocated,
 and the log will be disabled.
 
 @param _h  : A log DAL handle.
 @param bufferSize : The amount of memory to allocate in bytes.
                   See DalULog_Create for more details on memory
                   allocation.

 @return DAL_SUCCESS if the log is successfully enabled.
 */
static __inline DALResult DalULog_Allocate(DalDeviceHandle * _h, 
                                           uint32  bufferSize)
{
  return((DalULogHandle *)_h)->pVtbl->Allocate( _h, 
                                                bufferSize);
}

/**
 @brief DalULog_Enable - Enable a log.

 Enable the selected log.  The log must be previously created
 with the DalULog_Create function, and memory must be allocated.
 
 If it is already enabled this function will have no effect.
 
 @param _h  : A log DAL handle.

 @return DAL_SUCCESS if the log is successfully enabled.
 */
static __inline DALResult DalULog_Enable(DalDeviceHandle * _h)
{
  return((DalULogHandle *)_h)->pVtbl->Enable( _h);
}

/**
 @brief DalULog_Disable - Disable a log.

 Disable a log.  If it is already disabled this function will
 have no effect.  If the log is enabled, it will be disabled.
 
 This function will not free log memory, but will stop all write
 access to the log buffer.
 
 @param _h  : A log DAL handle.

 @return DAL_SUCCESS if the log is successfully disabled.
 */
static __inline DALResult DalULog_Disable(DalDeviceHandle * _h)
{
  return((DalULogHandle *)_h)->pVtbl->Disable( _h);
}

/**
 @brief DalULog_List - List the available logs.

 Retrieve a list of the available logs.  It is not necessary to
 be connected to a specific log in order to issue this command.
 
 nameArray is filled with as many full log name strings as will
 fit.  Each log name in the nameArray output is 24 characters
 long, including the null termination.
 
 @param _h  : A log DAL handle.
 @param nameArray : Log name data as an array of 24 character
                     entries.
 @param nameArraySize : The size of the output data array in
                      bytes.
 @param registeredCount : The number of logs registered in
                          the system.
 @param namesReturnedCount : The actual number of log names
                       placed in nameArray.

 @return DAL_SUCCESS if the read completed.
 */
static __inline DALResult DalULog_List(DalDeviceHandle * _h,  
                                       char *  nameArray,
                                       uint32  nameArraySize,  
                                       uint32 *  registeredCount,  
                                       uint32 *  namesReturnedCount)
{
  return((DalULogHandle *)_h)->pVtbl->List( _h, 
                                            registeredCount, 
                                            nameArraySize, 
                                            namesReturnedCount, 
                                            nameArray);
}

/**
 @brief DalULog_FormatMsg - Format a message for ASCII output.
 
 This is a helper function designed to make a log
 reader more portable.  It will format a single message
 retrieved with the DalULog_Read function for ASCII output,
 allowing the reader to not know how to parse output messages.
 
 A read may return several messages in the buffer.  This
 function should be called once for each message.  The words
 consumed indicates how much to advance the message pointer
 within the read buffer.
 
 If msgString is too short for the ASCII output, the
 string will simply be truncated at the maximum length.  In all
 cases the entire message is parsed, and msgConsumed always
 returns the length of the entire message.
 
 @param _h  : A log DAL handle.
 @param msg : A pointer to the start of the raw message to
                parse.
 @param msgString : The ASCII output message.
 @param msgStringSize : The size in bytes of the output
                          msgString.
 @param msgConsumed : The number of bytes used from the
                      message buffer.

 @return DAL_SUCCESS if the read completed.
 */
static __inline DALResult DalULog_FormatMsg( DalDeviceHandle * _h,  
                                             char *  msg,  
                                             char *  msgString, 
                                             uint32 msgStringSize, 
                                             uint32 * msgConsumed) 
{
  return((DalULogHandle *)_h)->pVtbl->FormatMsg( _h, 
                                                 msg, 
                                                 msgString, 
                                                 msgStringSize, 
                                                 msgConsumed);
}

/**
 @brief DalULog_FormatStatus - Format the status for ASCII
        output.
 
 This is a helper function designed to make a log reader more
 portable.  It will format the status of a log for ASCII output.
 
 The status value is the first word of the buffer after each
 DalULog_Read invocation.
 
 If statusString is too short for the ASCII output, the string
 will simply be truncated at the maximum length.
 
 @param _h  : A log DAL handle.
 @param status : The status value to parse.
 @param statusString : The output status.
 @param statusStringSize : The size in bytes of the output
                          statusString.

 @return DAL_SUCCESS if the read completed.
 */
static __inline DALResult DalULog_FormatStatus( DalDeviceHandle * _h,  
                                                uint32 status,  
                                                char * statusString, 
                                                uint32 statusStringSize) 
{
  return((DalULogHandle *)_h)->pVtbl->FormatStatus( _h, 
                                                    status, 
                                                    statusString, 
                                                    statusStringSize);
}

/**
 @brief DalULog_ValueQuery - Retrieve a parameter from the log.
 
 
 @param _h  : A log DAL handle.
 @param id: The parameter ID to retrieve.
 @param value: The return value of the parameter.

 @return DAL_SUCCESS if the assign completed.
 */
static __inline DALResult DalULog_ValueQuery( DalDeviceHandle * _h,  
                                              ULOG_VALUE_ID id,
                                              uint32 * value) 
{
  return((DalULogHandle *)_h)->pVtbl->ValueQuery( _h, 
                                                  id,
                                                  value);
}

/**
 @brief DalULog_MemoryAssign - Assign memory to a log manually.
 
 
 @param _h  : A log DAL handle.
 @param bufferPtr : The log buffer.
 @param bufferSize : The size in bytes of the log
                          buffer.

 @return DAL_SUCCESS if the assign completed.
 */
static __inline DALResult DalULog_MemoryAssign( DalDeviceHandle * _h,  
                                                ULOG_VALUE_ID id,
                                                void *  bufferPtr, 
                                                uint32 bufferSize) 
{
  return((DalULogHandle *)_h)->pVtbl->MemoryAssign( _h, 
                                                    id,
                                                    bufferPtr,
                                                    bufferSize);
}

static __inline DALResult DalULog_EnabledCheck( DalDeviceHandle * _h, uint32 * enabled) 
{
  return((DalULogHandle *)_h)->pVtbl->EnabledCheck( _h, enabled);
}

#endif

