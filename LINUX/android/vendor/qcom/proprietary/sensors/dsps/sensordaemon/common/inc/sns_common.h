#ifndef SNS_COMMON_H
#define SNS_COMMON_H

/*============================================================================

  @file sns_common.h

  @brief
  This file contains common definitions for Sensors framework

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================

                                INCLUDE FILES

============================================================================*/
#include "sensor1.h"
#include "sns_osa.h"
#include <stdlib.h>
#include <stdbool.h>
#if defined(SNS_DSPS_BUILD)
# include <err.h>
# include "customer.h"
#endif
#if defined(SNS_LA) || defined(SNS_LA_SIM)
# include "cutils/log.h"
# include <common_log.h>
#endif /* defined(SNS_LA) || defined(SNS_LA_SIM) */

#if defined(SNS_DSPS_BUILD)
# include <qmi_csi.h>
#endif /* defined(SNS_DSPS_BUILD) */

/*============================================================================

                                  Constants

============================================================================*/
/* The version definition */
#define SNS_MAJOR_VER             0x00
#define SNS_MINOR_VER             0x01

#ifdef QDSP6
#undef SNS_UNIT_TEST            /* For testing PM, undefine if not required */
#undef SNS_SAM_QMI_UNIT_TEST    /* For testing SAM, undefine if not required */
#undef SNS_SMGR_QMI_UNIT_TEST   /* For testing SMGR, undefine if not required */
#undef ADSP_STANDALONE
#endif //QDSP6

/* -----------------------------------------------------------
 *  Module ID definition
 * ---------------------------------------------------------*/
/* Processor ID definition */
#define SNS_MODULE_GRP_MASK       0xE0    /* 3 MSBs for Processor identification field */
#if defined(SNS_LA_SIM) || defined(SNS_PCSIM)
/* For simulation environments, "flatten" all of the module IDs into one list starting
 * from 0 */
# define SNS_MODULE_APPS           00
# define SNS_MODULE_DSPS           (SNS_MODULE_APPS + SNS_APPS_MODULE_CNT)
# define SNS_MODULE_UNDEFINED      0xFF
#else
# define SNS_MODULE_APPS           0x00
# define SNS_MODULE_DSPS           0x20
# define SNS_MODULE_UNDEFINED      0xFF
#endif

/* Module definition - For APPS */
/* NOTE : PRIO 1 to 9 ARE RESERVED !!!!
 * DALSys UCOS Shim allocates these priorities to mutexes at run time
 * during creation of DALSys Sync objects.
 * MAKE SURE THESE PRIORITIES ARE NOT USED IN SEONSORS CODE !!!
 */
#define SNS_APPS_MODULE_CNT       13

#define SNS_APPS_PRI_MUTEX_BASE   13
#define SNS_APPS_MODULE_PRI_BASE  20

#define SNS_MODULE_APPS_PP        (SNS_MODULE_APPS + 00)    /* Power Proxy */
#define SNS_MODULE_APPS_SAM       (SNS_MODULE_APPS + 01)    /* Legacy Sensor Algorithm Manager */
#define SNS_MODULE_APPS_SMR       (SNS_MODULE_APPS + 02)    /* Sensors Message Router */
#define SNS_MODULE_APPS_DIAG      (SNS_MODULE_APPS + 03)    /* Sensors diag agent module */
#define SNS_MODULE_APPS_ACM       (SNS_MODULE_APPS + 04)    /* Sensors application client module */
#define SNS_MODULE_APPS_REG       (SNS_MODULE_APPS + 05)    /* Sensors Registry Module */
#define SNS_MODULE_APPS_EM        (SNS_MODULE_APPS + 06)    /* Sensors Event Manager Module */
#define SNS_MODULE_APPS_INIT      (SNS_MODULE_APPS + 07)    /* Initialization module */
#define SNS_MODULE_APPS_PWR       (SNS_MODULE_APPS + 8)     /* Power module */
#define SNS_MODULE_APPS_TIME      (SNS_MODULE_APPS + 9)     /* Sensors Time Module */
#define SNS_MODULE_APPS_SCM       (SNS_MODULE_APPS + 10)    /* Sensors Calibration Module on Apps */
#define SNS_MODULE_APPS_SAM2      (SNS_MODULE_APPS + 11)    /* Updated SAM Framework */

/* Priority definition for mutex and modules for APPS*/
#define SNS_MEMMGR_APPS_MUTEX           (SNS_APPS_PRI_MUTEX_BASE + 00)
#define SNS_MODULE_PRI_APPS_EM_MUTEX    (SNS_APPS_PRI_MUTEX_BASE + 01)
#define SNS_MODULE_PRI_APPS_ACM_MUTEX   (SNS_APPS_PRI_MUTEX_BASE + 02)
#define SNS_SMR_APPS_SMDL_MUTEX         (SNS_APPS_PRI_MUTEX_BASE + 03)
#define SNS_SMR_APPS_QUE_MUTEX          (SNS_APPS_PRI_MUTEX_BASE + 04)
#define SNS_PWR_APPS_MUTEX              (SNS_APPS_PRI_MUTEX_BASE + 05)
#define SNS_REG_APPS_MUTEX              (SNS_APPS_PRI_MUTEX_BASE + 06)
#define SNS_TIME_APPS_MUTEX             (SNS_APPS_PRI_MUTEX_BASE + 07)
#define SNS_SAM_APPS_MSG_QUE_MUTEX      (SNS_APPS_PRI_MUTEX_BASE + 8)
#define SNS_SAM_APPS_CLI_QUE_MUTEX      (SNS_APPS_PRI_MUTEX_BASE + 9)

#define SNS_MODULE_PRI_APPS_EM     (SNS_APPS_MODULE_PRI_BASE + 0)
#define SNS_MODULE_PRI_APPS_PP     (SNS_APPS_MODULE_PRI_BASE + 1)
#define SNS_MODULE_PRI_APPS_SCM    (SNS_APPS_MODULE_PRI_BASE + 2)
#define SNS_MODULE_PRI_APPS_SAM    (SNS_APPS_MODULE_PRI_BASE + 3)
#define SNS_MODULE_PRI_APPS_ACM    (SNS_APPS_MODULE_PRI_BASE + 4)
#define SNS_MODULE_PRI_APPS_SMR    (SNS_APPS_MODULE_PRI_BASE + 5)
#define SNS_MODULE_PRI_APPS_DIAG   (SNS_APPS_MODULE_PRI_BASE + 6)
#define SNS_MODULE_PRI_APPS_REG    (SNS_APPS_MODULE_PRI_BASE + 7)
#define SNS_MODULE_PRI_APPS_TIME   (SNS_APPS_MODULE_PRI_BASE + 8)
#define SNS_MODULE_PRI_APPS_TIME2  (SNS_APPS_MODULE_PRI_BASE + 9)
#define SNS_MODULE_PRI_APPS_SAM_MR (SNS_APPS_MODULE_PRI_BASE + 10)

/* Stack size definition */
#define SNS_MODULE_STK_SIZE_APPS_PP     (0x200)
#define SNS_MODULE_STK_SIZE_APPS_SAM    (0x200)
#define SNS_MODULE_STK_SIZE_APPS_SMR    (0x200)
#define SNS_MODULE_STK_SIZE_APPS_DIAG   (0x200)
#define SNS_MODULE_STK_SIZE_APPS_ACM    (0x200)
#define SNS_MODULE_STK_SIZE_APPS_REG    (0x200)
#define SNS_MODULE_STK_SIZE_APPS_TIME   (0x200)
#define SNS_MODULE_STK_SIZE_APPS_SCM    (0x200)

#if defined(_WIN32)
/* Win32 doesn't use SAM or SMR */
#undef SNS_MODULE_STK_SIZE_APPS_SAM
#define SNS_MODULE_STK_SIZE_APPS_SAM    (1)
#undef SNS_MODULE_STK_SIZE_APPS_SMR
#define SNS_MODULE_STK_SIZE_APPS_SMR    (1)
#endif /* _WIN32 */

/* Module definition - For DSPS */
#ifdef  SNS_UNIT_TEST
#ifdef SNS_PM_TEST
  #define SNS_DSPS_MODULE_CNT       9
#else
  #define SNS_DSPS_MODULE_CNT       8
#endif
#elif defined(SNS_REG_TEST)
#ifdef SNS_PM_TEST
  #define SNS_DSPS_MODULE_CNT       8
#else
  #define SNS_DSPS_MODULE_CNT       7
#endif
#elif defined(SNS_PCSIM) || defined(SNS_LA_SIM) || defined(QDSP6)
#ifdef SNS_PM_TEST
  #define SNS_DSPS_MODULE_CNT       9
#else
  #define SNS_DSPS_MODULE_CNT       8
#endif
#else
#ifdef SNS_PM_TEST
/* Module count is made as Power test priority + 1
 * This is done so that SMR register does not fail for the
 * power test thread. SMR register can fail because
 * SMR asserts if module count <= module id
 * In this case pm test thread priority is 8 and module count has
 * to be greater than 8 for the check to pass.
 * SMR checks need to be fixed later.
 */
  #define SNS_DSPS_MODULE_CNT       9
#else
  #define SNS_DSPS_MODULE_CNT       6
#endif
#endif
#if defined(SNS_PCSIM) || defined(SNS_LA_SIM)
# define SNS_DSPS_MUTEX_PRI_BASE  (SNS_APPS_PRI_MUTEX_BASE+10)
# define SNS_DSPS_MODULE_PRI_BASE (SNS_APPS_MODULE_PRI_BASE+20)
#else
# define SNS_DSPS_MUTEX_PRI_BASE  (OS_MAX_DAL_RESERVED_PRIO + 2)
# define SNS_DSPS_MODULE_PRI_BASE (SNS_DSPS_MUTEX_PRI_BASE + 10)
#endif
#define SNS_MODULE_DSPS_IST       (SNS_MODULE_DSPS + 00)    /* Sensors Interrupt Service Task */
#define SNS_MODULE_DSPS_SMGR      (SNS_MODULE_DSPS + 01)    /* Sensor Manager */
#define SNS_MODULE_DSPS_PM        (SNS_MODULE_DSPS + 02)    /* Power Manager */
#define SNS_MODULE_DSPS_SAM       (SNS_MODULE_DSPS + 03)    /* Sensors Algorithm Manager */
#define SNS_MODULE_DSPS_SMR       (SNS_MODULE_DSPS + 04)    /* Sensors Message Router */
#define SNS_MODULE_DSPS_SCM       (SNS_MODULE_DSPS + 05)    /* Sensors Calibration Manager */
#define SNS_MODULE_DSPS_DEBUG     (SNS_MODULE_DSPS + 06)    /* Diag/debug */

/* Priority definition for mutex and modules for DSPS */
#define SNS_MEMMGR_DSPS_MUTEX     (SNS_DSPS_MUTEX_PRI_BASE  + 00)
#define SNS_SMR_SMDL_MUTEX        (SNS_DSPS_MUTEX_PRI_BASE  + 01)
#define SNS_SMR_QUE_MUTEX         (SNS_DSPS_MUTEX_PRI_BASE  + 02)
#define SNS_SMGR_QUE_MUTEX        (SNS_DSPS_MUTEX_PRI_BASE  + 03)
#define SNS_SAM_QUE_MUTEX         (SNS_DSPS_MUTEX_PRI_BASE  + 04)
#define SNS_PM_QUE_MUTEX          (SNS_DSPS_MUTEX_PRI_BASE  + 05)

#define SNS_MODULE_PRI_DSPS_SMR   (SNS_DSPS_MODULE_PRI_BASE + 03)

// TODO: decide the thread priorities
#ifndef QDSP6
#define SNS_MODULE_PRI_DSPS_IST   (SNS_DSPS_MODULE_PRI_BASE + 00)
#define SNS_MODULE_PRI_DSPS_PM    (SNS_DSPS_MODULE_PRI_BASE + 01)
#define SNS_MODULE_PRI_DSPS_SMGR  (SNS_DSPS_MODULE_PRI_BASE + 02)
#define SNS_MODULE_PRI_DSPS_SAM   (SNS_DSPS_MODULE_PRI_BASE + 04)
#define SNS_MODULE_PRI_DSPS_SCM   (SNS_DSPS_MODULE_PRI_BASE + 05)
#define SNS_MODULE_PRI_DSPS_DEBUG (SNS_DSPS_MODULE_PRI_BASE + 06)

#if defined(SNS_LA_SIM) || defined(SNS_PCSIM)
#define SNS_MODULE_DSPS_PLAYBACK     (SNS_MODULE_DSPS + 07)    /* Playback Module */
#define SNS_MODULE_PRI_DSPS_PLAYBACK (SNS_DSPS_MODULE_PRI_BASE + 07)
#define SNS_MODULE_STK_SIZE_DSPS_PLAYBACK (0x200)
#endif
#else //QDSP6
#define SNS_MODULE_STK_SIZE_DSPS_EVMGR    (0x100)
#define SNS_MODULE_PRI_DSPS_EVMGR (SNS_DSPS_MODULE_PRI_BASE + 07)
#define SNS_MODULE_PRI_DSPS_SMGR  (SNS_DSPS_MODULE_PRI_BASE + 06)
#define SNS_MODULE_PRI_DSPS_SAM   (SNS_DSPS_MODULE_PRI_BASE + 05)
#define SNS_MODULE_PRI_DSPS_SCM   (SNS_DSPS_MODULE_PRI_BASE + 04)
#define SNS_MODULE_PRI_DSPS_PM    (SNS_DSPS_MODULE_PRI_BASE + 03)
#define SNS_MODULE_PRI_DSPS_DEBUG (SNS_DSPS_MODULE_PRI_BASE + 02)
#define SNS_MODULE_PRI_DSPS_PLAYBACK (SNS_DSPS_MODULE_PRI_BASE + 01)
#define SNS_MODULE_DSPS_PLAYBACK     (SNS_MODULE_DSPS + 07)    /* Playback Module */
#define SNS_MODULE_STK_SIZE_DSPS_PLAYBACK (0x300)
#endif //QDSP6

#ifdef  SNS_UNIT_TEST
#define SNS_MODULE_DSPS_TEST     (SNS_MODULE_DSPS + 07)
#define SNS_MODULE_PRI_DSPS_TEST (SNS_DSPS_MODULE_PRI_BASE + 07)
#define SNS_MODULE_STK_SIZE_DSPS_TEST 0x100      /* Minimum stack size required for a thread to create */
#endif

#ifdef SNS_REG_TEST /* Sensors registry unit test code */
#define SNS_MODULE_DSPS_REG_TEST     (SNS_MODULE_DSPS + 06)
#define SNS_MODULE_PRI_DSPS_REG_TEST (SNS_DSPS_MODULE_PRI_BASE + 06)
#endif /* SNS_REG_TEST */

#ifdef SNS_PM_TEST
/* Sensors Power manager test thread code used for target bringup */
/* Note change PM Test module number for use on PCSIM. Reason is that
 * on PC SIM playaback module uses same module number
 */
#define SNS_MODULE_DSPS_POWER_TEST     (SNS_MODULE_DSPS + 8)
#define SNS_MODULE_PRI_DSPS_POWER_TEST (SNS_DSPS_MODULE_PRI_BASE + 8)
/* Define a smaller stack for the test code */
#define SNS_MODULE_STK_SIZE_DSPS_PM_TEST     (0x180)
#endif /* SNS_PM_TEST */

/* Stack size difinition */
#define SNS_MODULE_STK_SIZE_DSPS_IST    (0x300)
#define SNS_MODULE_STK_SIZE_DSPS_SMGR   (0x600)
#define SNS_MODULE_STK_SIZE_DSPS_PM     (0x300)
#define SNS_MODULE_STK_SIZE_DSPS_SAM    (0x300)
#define SNS_MODULE_STK_SIZE_DSPS_SMR    (0x300)
#define SNS_MODULE_STK_SIZE_DSPS_SCM    (0x300)
#define SNS_MODULE_STK_SIZE_DSPS_DEBUG    (0x300)

#if defined(SNS_LA) || defined (SNS_LA_SIM) || defined(_WIN32)
#define SNS_MEMMGR_MUTEX SNS_MEMMGR_APPS_MUTEX
#else
#define SNS_MEMMGR_MUTEX SNS_MEMMGR_DSPS_MUTEX
#endif

#if defined(SNS_LA_SIM) || defined(SNS_PCSIM)
# define SNS_MODULE_CNT       (SNS_APPS_MODULE_CNT + SNS_DSPS_MODULE_CNT)
# define SNS_THIS_MODULE_GRP  0x00
#elif defined (SNS_LA) || defined(_WIN32)
# define SNS_MODULE_CNT       SNS_APPS_MODULE_CNT
# define SNS_THIS_MODULE_GRP  SNS_MODULE_APPS
#elif defined (SNS_DSPS_BUILD)
# define SNS_MODULE_CNT       SNS_DSPS_MODULE_CNT
# define SNS_THIS_MODULE_GRP  SNS_MODULE_DSPS
#endif

/*============================================================================

                                  Typedefs

============================================================================*/
/*
 * The error code definition within the sensor framework
 */
typedef enum {
  SNS_SUCCESS   = 0,
  SNS_ERR_BUFFER,       /* Value in the buffer is wrong */
  SNS_ERR_NOMEM,        /* Insufficient memory to process the request */
  SNS_ERR_INVALID_HNDL, /* Invalid handle */
  SNS_ERR_UNKNOWN,      /* Unknown error */
  SNS_ERR_FAILED,       /* Failure in general */
  SNS_ERR_NOTALLOWED,   /* Not allowed operation */
  SNS_ERR_BAD_PARM,     /* One or more parameters have invalid value */
  SNS_ERR_BAD_PTR,      /* Invalid pointer. This may be due to NULL values */
  SNS_ERR_BAD_MSG_ID,   /* The message ID is not supported */
  SNS_ERR_BAD_MSG_SIZE, /* The message size of the message ID does not match */
  SNS_ERR_WOULDBLOCK,   /* Indicating the data is tentatively unavailable */
  SNS_ERR_NOTSUPPORTED  /* Indicating that API is not supported */
} sns_err_code_e;

/*============================================================================

                       MACRO definition

============================================================================*/
/*
 * Define SNS_ASSERT related macro definitions
 */
#if defined(SNS_LA) || defined(SNS_LA_SIM)
# define SNS_ASSERT(value) if((value)!=true)                          \
  {                                                                   \
    while (1)                                                         \
    {                                                                 \
      SNS_PRINTF_STRING_FATAL_1(SNS_DBG_MOD_APPS_SMR,                 \
                                "!!!SNS ASSERT!!! line %d",           \
                                (int)__LINE__);                       \
      LOGE("!!!SNS ASSERT:%s!!!File %s line %d",                      \
           #value, __FILE__, (int)__LINE__);                          \
      abort();                                                        \
    }                                                                 \
  }
#elif defined(QDSP6)
# ifdef SNS_DEBUG
#   define SNS_ASSERT(value) if((value)!=TRUE)                          \
      ERR_FATAL( #value ,0,0,0)
# else /* SNS_DEBUG */
#   define SNS_ASSERT(value) if((value)!=TRUE)                          \
      ERR_FATAL( "" ,0,0,0)
# endif /* SNS_DEBUG */
#elif defined(SNS_DSPS_BUILD)
# ifdef SNS_DEBUG
#   define SNS_ASSERT(value) if((value)!=true)                          \
      ERR_FATAL( #value ,0,0,0)
# else /* SNS_DEBUG */
#   define SNS_ASSERT(value) if((value)!=true)                          \
      ERR_FATAL( "" ,0,0,0)
# endif /* SNS_DEBUG */
#elif defined (_WIN32)
#  ifdef DBG
//WIN32 TODO - every SNS_ASSERT should probably be SNS_ASSERT(FALSE). Linux
//  will abort in them, even on release builds. Some asserts require return,
//  return <value>, break or continue to be gracefully handled, because we
//  cannot use one function like abort().
#    define SNS_ASSERT(value)                                         \
        __pragma(warning(disable:4127)) /* conditional expression is constant */ \
        if((value)!=TRUE) {                                           \
            SNS_PRINTF_STRING_FATAL_1(SNS_DBG_MOD_APPS_SMR,           \
                                      "!!!SNS ASSERT!!! line %d",     \
                                      (int)__LINE__);                 \
            DbgBreakPoint();                                          \
        }
#  else
#    define SNS_ASSERT(value) /* Do nothing */
#  endif /* SNS_DEBUG */
#else
# define SNS_ASSERT(value) if((value)!=true)                          \
    while (1){}
#endif

#define SNS_ASSERT_DBG(value) SNS_ASSERT(value)

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(x) (void)x;
#endif /* UNREFERENCED_PARAMETER */

/*============================================================================

                        External Variable Declarations

============================================================================*/
/*
 * Boolean variable to hold the kernel state information that is notified by SMSM
 */
extern bool linux_kernel_suspend;

#endif /* SNS_COMMON_H */
