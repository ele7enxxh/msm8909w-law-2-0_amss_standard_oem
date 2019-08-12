#ifndef ULOGSTREAM_H
#define ULOGSTREAM_H

/*============================================================================
  @file ULogStream.h

  Windows Mobile IOCTL Stream Driver interface for ULog

  This file describes the IOCTL interface for ULog.  This allows application
  space access to read different logs from kernel space.

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/power/ULogStream.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define ULOG_FILE_NAME "ULG1:"

#define IOCTL_ULG_ATTACH   1
#define IOCTL_ULG_DETACH   2
#define IOCTL_ULG_ALLOCATE 3
#define IOCTL_ULG_ENABLE   4
#define IOCTL_ULG_LIST     5
#define IOCTL_ULG_CONNECT  6
#define IOCTL_ULG_READ     7
#define IOCTL_ULG_FORMATMSG  8


/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

#endif // ULOGSTREAM_H
