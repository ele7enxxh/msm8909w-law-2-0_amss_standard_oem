/*============================================================================
  rpm_ulogcompatibility.c

  This file implements the front end interfaces for ULog compatibility.  
  To keep memory requirements to a minimum there is no real log structure 
  created for the logs.  All writes are directed out QDSS once QDSS has 
  been enabled by ULogCore_SetTransportToStm for any log. 
  
  This file could be fairly easily enhanced to write to a ram buffer someday
  if rpm has enough memory to provided it. 

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/rpm.bf/2.1.1/core/power/ulog/src/rpm_ulogcompatibility.c#1 $ */
#include "ULog.h"
#include "rpm_ulog.h"
#include <stdarg.h>
#include "DALStdErr.h"

//Share one STM port across all ULogCompatibility users.
uint32 ulog_global_stmport;


/*----------------------------------------------------------------------------
ULogFront_RealTimeInit
Called by ULog users to create a log
----------------------------------------------------------------------------*/
ULogResult ULogFront_RealTimeInit(ULogHandle * h, const char * name, uint32 logBufSize, uint32 logBufMemType, ULOG_LOCK_TYPE logLockType)
{
  *h = (ULogHandle *)0; 
  return (RPM_ULOG_SUCCESS);            
}


/*----------------------------------------------------------------------------
ULogFront_RealTimePrintf
This function does the most work for ULog logging.
This is a very special version of ULogFront_RealTimePrintf.  For our 
compatibility layer we only direct the writes to QDSS.
----------------------------------------------------------------------------*/
ULogResult ULogFront_RealTimePrintf(ULogHandle h, uint32 dataCount, const char * formatStr, ...)
{
  uint32 total_size;
  uint32 dataArray[15];
  uint64 timestamp;
  uint32 * dataPtr = dataArray;
  uint32 i;
  va_list    ap;


  if ((dataCount>10)){
    return(RPM_ULOG_ERROR);
  }

  //if output to QDSS has been turned on, use it. 
  if(ulog_global_stmport!=0)
  {
    // msgSize is len/fmt + TS + format string + data
    total_size = 8 + ULOG_TIMESTAMP_SIZE + dataCount*4; 

    va_start(ap, formatStr);  
	 
    *dataPtr++ = ULOG_REALTIME_SUBTYPE_PRINTF;
    timestamp = RPM_ULOG_GETTIME64;
    *dataPtr++ = (uint32)timestamp;
    *dataPtr++ = (uint32)((uint64)timestamp>>32);
    *dataPtr++ = (uint32)formatStr;

     for(i = 0; i < dataCount; i++)
     {
       *dataPtr++ = va_arg(ap, uint32);
     }
     //(Using 2 for the protocol, This is differnt from normal ULog where the person setting the transport can choose the protocol.  It saves a little ram to just set it to 1. )
     QDSSWriteOSTPacket(ulog_global_stmport, QDSSOSTHeader(QDSS_OST_ENTITY_ULOG, 2), (byte*)dataArray, total_size);
 
     va_end(ap);
  }
  
  return RPM_ULOG_SUCCESS;  
}



/*----------------------------------------------------------------------------
 ULogCore_AttributeSet
----------------------------------------------------------------------------*/
void ULogCore_AttributeSet(ULogHandle h, uint32 attribute)
{
  return;
}



/*----------------------------------------------------------------------------
 ULogCore_HeaderSet
----------------------------------------------------------------------------*/
ULogResult ULogCore_HeaderSet(ULogHandle h, char * headerText)
{
  return(RPM_ULOG_SUCCESS);
}


/*----------------------------------------------------------------------------
ULogCore_SetTransportToSTM
----------------------------------------------------------------------------*/
ULogResult ULogCore_SetTransportToStm(ULogHandle h){
  
  if (ulog_global_stmport == 0){
    RPM_ULOG_QDSS_ALLOC(&ulog_global_stmport);
  }

  return RPM_ULOG_SUCCESS;
}



