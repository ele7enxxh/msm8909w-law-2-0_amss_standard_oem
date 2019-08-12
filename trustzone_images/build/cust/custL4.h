#ifndef CUSTL4_H
#define CUSTL4_H
/*===========================================================================

            " C u s t - L 4 "   H E A D E R   F I L E

DESCRIPTION
  Configuration for L4 Feature.

  Copyright (c) 2005       by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header:
$DateTime: 2016/06/17 14:31:11 $ $Author:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/06   hn      Disabled FEATURE_L4_CPUSLEEP for 7200
12/07/05   ddh     Enabled FEATURE_L4_CPUSLEEP
11/17/05   dlb'    Enabling Rex Profiling for modem processor..
10/27/05   AMW     Added BUILD_JFLASH exception
09/21/05   AMW     Added test for building boot and tools

===========================================================================*/


#if !defined(BUILD_BOOT_CHAIN) && \
    !defined(BUILD_JNAND) && \
    !defined(BUILD_JFLASH) && \
    !defined(BUILD_HOSTDL) && \
    defined(FEATURE_L4)

  #define FEATURE_MEMORY_VIRTUAL
  #define FEATURE_L4_PROFILE
  #define FEATURE_L4_PAGE_IN_AMSS_DURING_BOOT
  #define FEATURE_DOG_IGNORE_SLEEP_TASK
  /* #define FEATURE_USB_DIAG_NMEA */
  #define FEATURE_NO_INLINE_SYSCALLS
  #define FEATURE_L4_SRCH_OPTIM
  #define FEATURE_QMUTEX
  #define FEATURE_QUARTZ_135
  #define FEATURE_QUARTZ_20
  #define CONFIG_HYBRID_MUTEXES
  #define ARCH_ARM
#ifdef IMAGE_APPS_PROC
  #define ARCH_VER 6 
#else
  #define ARCH_VER 5 
#endif

  /* Output security hole violation using QXDM */
  #define FEATURE_QDMS_DBG

  #ifdef FEATURE_L4_PERF_OPTIM
    #undef FEATURE_L4_PERF_OPTIM
  #endif

  #ifdef FEATURE_FLASH_INIT_ECC
    #undef FEATURE_FLASH_INIT_ECC
  #endif

  #ifdef FEATURE_L4_MSMSLEEP_BOOT_SRAM
    #undef FEATURE_L4_MSMSLEEP_BOOT_SRAM
  #endif

  #ifdef FEATURE_L4_KNIGHTRIDER
    #undef FEATURE_L4_KNIGHTRIDER
  #endif

  #ifdef FEATURE_L4_KERNEL
    /* Fix problem of having multiple targets defined when compiling the kernel */
    #undef T_MSM6275 
    #undef T_MSM6280
  #endif

  #ifdef FEATURE_L4_MSMSLEEP
    #undef FEATURE_L4_MSMSLEEP
  #endif

  #ifdef FEATURE_L4_CPUSLEEP
    #undef FEATURE_L4_CPUSLEEP
  #endif
/* Use KxInterrupt in tramp.c for AMSS build */
/* This is to avoid using KxInterrupt when building the sbl files */
  #define KXINTERRUPT_ENABLE
  #define FEATURE_KXMUTEX_ENHANCED_PRIORITY_INHERITANCE 

  #ifndef QC_MODIFIED
    #define QC_MODIFIED
  #endif
#else

  #undef FEATURE_L4
  #define FEATURE_QUARTZ_20

#endif /* !BUILD_BOOT_CHAIN & !BUILD_JNAND & FEATURE_L4 */

#endif /* CUSTL4_H */

