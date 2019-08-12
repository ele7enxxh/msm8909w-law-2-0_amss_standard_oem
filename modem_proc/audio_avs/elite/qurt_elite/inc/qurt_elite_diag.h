/*========================================================================
Copyright (c) 2010-2011, 2013 Qualcomm Technologies, Inc. All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
======================================================================== */
/**
@file qurt_elite_diag.h

@brief This file contains a utility for generating diagnostic messages.
 This file defines macros for printing debug messages on the target or
 in simulation.
*/
/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      qurt_elite_mainpage.dox file.
===========================================================================*/

/*========================================================================
Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/elite/qurt_elite/inc/qurt_elite_diag.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
03/25/13   sw       (Tech Pubs) Edited Doxygen comments/markup for 2.0.
05/03/11   leo      (Tech Pubs) Edited doxygen comments and markup.
02/04/10   mwc      Created file.
03/09/10   brs      Edited for doxygen-to-latex process.
========================================================================== */

#ifndef QURT_ELITE_DIAG_H
#define QURT_ELITE_DIAG_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */

#include "mmdefs.h"
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


//***************************************************************************
// Debug message features
//***************************************************************************
// change the following to be variables for runtime (TBD)
// hence defined in lower case

/** @addtogroup qurt_elite_diag
@{ */

/** Enable or disable the debug message. */
#define qurt_elite_debugmsg_enable    1

/** Lowest debug message level to be shown. This is only valid in SIM mode. */
#define qurt_elite_debugmsg_lowest_prio DBG_MED_PRIO


#if !defined(SIM)


#include "msg.h"
#include "msgcfg.h"

/* TODO: request our own ssids by emailing asw.diag.request
   currently all ssid are mapped to MSG_SSID_QDSP6
 */
// common priorities

/** @name Legacy Debug Priority Messages
Use diag-defined MSG_N macros and legacy debug priorities.
@{ */

#define DBG_LOW_PRIO   MSG_LEGACY_HIGH    /**< Low priority debug message. */
#define DBG_MED_PRIO   MSG_LEGACY_HIGH    /**< Medium priority debug message. */
#define DBG_HIGH_PRIO  MSG_LEGACY_HIGH   /**< High priority debug message. */
#define DBG_ERROR_PRIO MSG_LEGACY_ERROR  /**< Error priority debug message. */
#define DBG_FATAL_PRIO MSG_LEGACY_FATAL  /**< Fatal priority debug message. */

/** @} */ /* end_namegroup Legacy Debug Priority Messages */

#else /* SIM only */
/*=====================================================*/
/* Original implementation cut&paste from AudCmnUtil.h */
/*=====================================================*/

/*
DBG_MSG for debugging. Currently compile time as we are supposed
to have a single build. Prints the file and line number and the
message. Message is printf format and takes a variable number of
arguments.
*/
#include "hexagon_sim_timer.h"
/** @name Common Debug Priorities
@{ */

#define DBG_FATAL_PRIO 0
#define DBG_ERROR_PRIO 1
#define DBG_HIGH_PRIO  2
#define DBG_MED_PRIO   3
#define DBG_LOW_PRIO   4

/** @} */ /* end_namegroup Common Debug Priorities */
#undef  MSG_SSID_QDSP6
#undef MSG_SSID_APR_ADSP
#define MSG_SSID_QDSP6  0
#define MSG_SSID_APR_ADSP  1

//if __FILENAME macro is not available, just leave that field blank
#ifndef __FILENAME__
#define __FILENAME__ ""
#endif

// assert is for priority == 0 (i.e on ERROR)
// disable warning "conditional expression is constant" for qurt_elite_debugmsg_enable
#if (__QDSP6_ARCH__ <= 3)
#define DBG_MSG(ssid, priority, message, ...) \
  do { \
    if (qurt_elite_debugmsg_enable && (priority <= qurt_elite_debugmsg_lowest_prio)) \
    { \
      uint64_t cycles = hexagon_sim_read_pcycles(); \
      int tms = ((cycles >> 10)*1790)>>20; \
      int tsec = ((cycles >> 10)*1833)>>30; \
      int tmin = ((cycles >> 10)*31)>>30; \
      qurt_printf("%dm:%ds:%dms @ %d of %s : " message "\n", tmin, tsec-60*tmin, tms - 1000*tsec, __LINE__, __FILENAME__, ##__VA_ARGS__); \
      QURT_ELITE_ASSERT(priority);\
    } \
  } while(0) \

#elif (__QDSP6_ARCH__>=4)

#define DBG_MSG(ssid, priority, message, ...) \
  do { \
    if (qurt_elite_debugmsg_enable && (priority <= qurt_elite_debugmsg_lowest_prio)) \
    { \
      uint64_t cycles = hexagon_sim_read_pcycles(); \
      int tms = ((cycles >> 10)*2148)>>20; \
      int tsec = ((cycles >> 10)*2200)>>30; \
      int tmin = ((cycles >> 10)*37)>>30; \
      qurt_printf("%dm:%ds:%dms @ %d of %s : " message "\n", tmin, tsec-60*tmin, tms - 1000*tsec, __LINE__, __FILENAME__, ##__VA_ARGS__); \
      QURT_ELITE_ASSERT(priority);\
    } \
  } while(0) \

#else
#error "Unknown Q6 Arch Version"
#endif
#undef MSG
#undef MSG_1
#undef MSG_2
#undef MSG_3
#undef MSG_4
#undef MSG_5
#undef MSG_6
#undef MSG_7
#undef MSG_8
#undef MSG_9
#undef MSG_FATAL
#define MSG(xx_ss_id, xx_ss_mask, xx_fmt)                       DBG_MSG(0,xx_ss_mask,xx_fmt)
#define MSG_1(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1)            DBG_MSG(0,xx_ss_mask,xx_fmt,xx_arg1)
#define MSG_2(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)   DBG_MSG(0,xx_ss_mask,xx_fmt,xx_arg1,xx_arg2)
#define MSG_3(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
                 DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define MSG_4(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
                 DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define MSG_5(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
                 DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define MSG_6(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
                 DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

#define MSG_7(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7) \
                 DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7)

#define MSG_8(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8) \
                 DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8)


#define MSG_9(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8, xx_arg9) \
    DBG_MSG(0,xx_ss_mask,xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6, xx_arg7, xx_arg8, xx_arg9)


#define MSG_FATAL(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_QDSP6, DBG_FATAL_PRIO, x_fmt, a, b, c)

#endif /* SIMULATION */

/* Original macro cut&paste from AudCmnUtil.h, leave it outside SURF for kernel debugging */

/** Print macro for debugging. Prints to the QuRT buffer. */
#define PRINT()   qurt_printf("%s %d\n",__FILENAME__, __LINE__);

#if defined DEBUG || defined SIM
   /** Qurt_elite version of assert. */
   #define QURT_ELITE_ASSERT(x) assert(x)
#else
   /** Qurt_elite version of assert. */
   #define QURT_ELITE_ASSERT(x)
#endif

/**
  Displays the contents of a buffer byte-wise in hexadecimal format on the
  diagnostic service. The buffer size must be a multiple of 8.

  @param[in] buffer  Pointer to the start address of the buffer to be printed.
  @param[in] size    Size of the buffer to be printed.

  @return
  None.

  @dependencies
  None. @newpage
*/
void qurt_elite_hexdump(const uint8_t* buffer, int32_t size);

/** @} */ /* end_addtogroup qurt_elite_diag */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef QURT_ELITE_DIAG_H


