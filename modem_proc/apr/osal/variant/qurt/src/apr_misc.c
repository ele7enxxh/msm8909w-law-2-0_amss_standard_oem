/*
  Copyright (C) 2009-2010 Qualcomm Technologies Incorporated.
  All rights reserved.
  QUALCOMM Proprietary/GTDR.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/apr/osal/variant/qurt/src/apr_misc.c#1 $
  $Author: mplcsds1 $
*/

#include "qube.h"
#include "timer.h"
#include "apr_errcodes.h"
#include "apr_misc.h"

APR_INTERNAL int32_t apr_misc_sleep (
  uint64_t time_ns
)
{
  timer_sleep( (time_ns / 1000), T_USEC, FALSE );
  return APR_EOK;
}

