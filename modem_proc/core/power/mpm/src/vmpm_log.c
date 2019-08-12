/*============================================================================
  FILE:         vmpm_log.c
  
  OVERVIEW:     This file implements the functions for logging mpm messages.

  NOTE:         Right now there are no low memory targets that are using vMPM
                driver. If that changes, we may want to restrict the log 
                usage for it.
 
                Copyright (c) 2012-2014 QUALCOMM Technologies Inc (QTI)
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
=============================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/mpm/src/vmpm_log.c#1 $
$DateTime: 2016/12/13 07:59:23 $
============================================================================*/
#include <stdarg.h>
#include "vmpm_internal.h"
#include "CoreVerify.h"
#include "ULog.h"
#include "ULogFront.h"
#include "err.h"

/*===========================================================================
 *                            INTERNAL DEFINITIONS
 *===========================================================================*/

/**
 * log handle for mpm logs.
 */
static ULogHandle vmpm_log_handle = NULL;

/**
 * Size of the ULog buffer for mpm log.
 *
 * @Note If this value needs to be changes for the same master within a chip
 *       family, it may affect binary compatibility requirement, if enforced
 *       on that master.
 */
static unsigned int vmpm_log_size = 1024;


/*===========================================================================
 *                         GLOBAL FUNCTION DEFINITIONS
 *===========================================================================*/

/*
 * vmpm_LogInit
 */
void vmpm_LogInit( void )
{
  #if defined(USES_QNX) && defined(QNP_POWERMAN_PROC)
  ULOG_LOCK_TYPE log_lock = ULOG_LOCK_NONE;
  #else
  ULOG_LOCK_TYPE log_lock = ULOG_LOCK_OS;
  #endif

  if( !vmpm_log_handle )
  {
    /* Creating a log handle */
    CORE_VERIFY( ULOG_ERR_INITINCOMPLETE ==
                 ULogFront_RealTimeInit(&vmpm_log_handle, "MPM Driver log", 
                                        0, ULOG_MEMORY_LOCAL, log_lock));

    /* Log header setup */
    ULogCore_HeaderSet(vmpm_log_handle, 
                       "Content-Type: text/mpm-driver-1.0; title=MPM Driver");

    /* Allocating required memory */
    ULogCore_Allocate(vmpm_log_handle, vmpm_log_size);

    /* Enabling the log */
    ULogCore_Enable(vmpm_log_handle);
  }
  else
  {
    ERR_FATAL("Potential multiple initialization of mpm log", 0, 0, 0);
  }
}

/*
 * vmpm_LogPrintf
 */
void vmpm_LogPrintf( int arg_count, const char *format, ... )
{
  va_list args;

  va_start(args, format);
  ULogFront_RealTimeVprintf(vmpm_log_handle, arg_count, format, args);
  va_end(args);
}
