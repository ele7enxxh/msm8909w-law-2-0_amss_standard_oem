/*---------------------------------------------------------------------------
   SMEM_OS.H  - REX Specific Header for SMEM
---------------------------------------------------------------------------*/
/*!
  @file
    smem_os.h

  @brief
    This file contains the REX specific SMSM definitions.
*/

/*--------------------------------------------------------------------------
     Copyright  2008 - 2012 Qualcomm Technologies Incorporated. 
     All rights reserved.
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/mproc/smem/src/smem_os.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/29/12   bt      Add TZ-specific ERR_FATAL and ASSERT wrappers.
05/30/12   bm      Use ARM(cortex) specific implementation of mem barrier for 
                   RPM
01/20/12   bm      Removing unwanted headers and adding RPM support.
02/09/12   bt      Use Timetick OS abstraction, remove timetick.h inclusion.
01/20/12   pa      Added support for bootloader builds by featurizing 
                   header files and ERR_FATAL defn
12/21/11   rs      Redefined SMEM memory barrier as OS specific.
08/19/11   bm      Use std ERR_FATAL for DSPS proc.
06/17/11   hwu     Seperate out SMSM from SMEM.
05/11/11   hwu     Moved assert.h and err.h to os file.
04/29/11   bm      Redefined OS specific macros for DSPS
04/08/11   rs      Use OS abstraction to implement SMSM critical sections 
04/08/10   hwu     Moved interrupt macros to target specific. 
04/20/10   tl      Use critical sections instead of INTLOCK
11/19/09   sa      warning cleanup, sync-up with symbian
10/27/09   tl      Added support for 9600
09/01/09   hwu     Added smsm_mutex_xxx macros
01/05/08   hwu     Initial version
===========================================================================*/
#ifndef SMEM_OS_H
#define SMEM_OS_H

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include <string.h>
#include "DALFramework.h"

#ifdef SMEM_BOOT_IMAGE
#include "boot_api.h" /* for BOOT_ERR_FATAL() */
#elif defined(SMEM_TZ_IMAGE)
#include "tzbsp.h"    /* for tzbsp_err_fatal() */
#else
#include "err.h"
#endif

#if !defined(SMEM_TZ_IMAGE)
#include "assert.h"
#endif

#ifdef IMAGE_RPM_PROC
#include "cortex-m3.h"
#endif

/*--------------------------------------------------------------------------
  Type definitions
---------------------------------------------------------------------------*/

/** OS Macro redefinitions for DSPS target 
 */
#if defined(IMAGE_DSPS_PROC) || defined(IMAGE_RPM_PROC)
#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT( cond ) \
      if( !( cond ) ) \
      {                 \
        while(1);         \
      }
#endif //IMAGE_DSPS_PROC

#ifdef IMAGE_RPM_PROC
#define ERR(...)    { while(0);}
#endif

/* -------------------------------------------------------------
   Memory barrier for REX
------------------------------------------------------------- */
#if defined(IMAGE_DSPS_PROC)
#define SMEM_MEMORY_BARRIER() smem_generic_memory_barrier()
#else
#define SMEM_MEMORY_BARRIER() memory_barrier()
#endif

/* Special ERR_FATAL for bootloader builds */
#ifdef SMEM_BOOT_IMAGE
#define ERR_FATAL(format, code1, code2, code3)  BOOT_ERR_FATAL()
/* ERR_FATAL and ASSERT implementations specific to TZ builds. */
#elif defined(SMEM_TZ_IMAGE)
#define ERR_FATAL(format, code1, code2, code3)          \
  do {                                                  \
    TZBSP_LOG_ERR(format, code1, code2, code3);         \
    tzbsp_err_fatal(TZBSP_ERR_FATAL_SMEM);              \
  } while(0)
#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT( cond )                                  \
  if( !( cond ) )                                       \
  {                                                     \
    ERR_FATAL( "Assertion " #cond " failed", 0, 0, 0 ); \
  }
#endif

#endif /* SMEM_OS_H */
