/* camera_dbg.h
 *
 * Copyright (c) 2012 Qualcomm Technologies, Inc. All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

#ifndef __CAMERA_DBG_H__
#define __CAMERA_DBG_H__

#define LOG_DEBUG 1
volatile unsigned int gcam_mct_loglevel;
#undef CDBG
#undef LOGE
#ifndef LOG_DEBUG
  #define CDBG(fmt, args...) do{}while(0)
  #ifdef _ANDROID_
    #undef LOG_NIDEBUG
    #undef LOG_TAG
    #define LOG_NIDEBUG 0
    #define LOG_TAG "mm-camera"
    #include <utils/Log.h>
  #else
    #include <stdio.h>
    #define LOGE(fmt, args...) do {} while (0)
  #endif
#else
  #ifdef _ANDROID_
    #undef LOG_NIDEBUG
    #undef LOG_TAG
    #define LOG_NIDEBUG 0
    #define LOG_TAG "mm-camera"
    #include <utils/Log.h>
    #define CDBG(fmt, args...) ALOGD_IF(gcam_mct_loglevel >= 2, fmt, ##args)
  #else
    #include <stdio.h>
    #define CDBG(fmt, args...) fprintf(stderr, fmt, ##args)
    #define LOGE(fmt, args...) fprintf(stderr, fmt, ##args)
  #endif
#endif

#ifdef _ANDROID_
  #ifndef ATRACE_TAG
  #define ATRACE_TAG ATRACE_TAG_CAMERA
  #endif
  #include <cutils/trace.h>
  #define KPI_ONLY 1
  #define KPI_DBG 2
  #define ATRACE_BEGIN(name) atrace_begin(ATRACE_TAG, name)
  #define ATRACE_END() atrace_end(ATRACE_TAG)
  #define CDBG_HIGH(fmt, args...)  ALOGD_IF(gcam_mct_loglevel >= 1, fmt, ##args)
  #define CDBG_ERROR(fmt, args...)  ALOGE(fmt, ##args)
  #define CDBG_LOW(fmt, args...) do {} while (0)
  /* Q3A_TRACE*/
  #define Q3A_TRACE_INT(kpi_dbg_level,name, val) ({\
  if (kpi_dbg_level >= KPI_ONLY) { \
     atrace_int(ATRACE_TAG_ALWAYS, name, val); \
}\
})
#else
  #define ATRACE_BEGIN(name) do {} while (0)
  #define ATRACE_END() do {} while (0)
  #define Q3A_TRACE_INT(kpi_dbg_level, name, val) do {} while (0)
  #define CDBG_HIGH(fmt, args...) fprintf(stderr, fmt, ##args)
  #define CDBG_ERROR(fmt, args...) fprintf(stderr, fmt, ##args)
  #define CDBG_LOW(fmt, args...) fprintf(stderr, fmt, ##args)
#endif

#define PRINT_2D_MATRIX(X,Y,M) ({ \
  int i, j; \
  for(i=0; i<X; i++) { \
    CDBG_HIGH("\n%s: ", __func__); \
    for(j=0; j<Y; j++) {\
      CDBG_HIGH("%lf ", M[i][j]); \
    } \
  } \
})

#define PRINT_1D_MATRIX(X,Y,M) ({ \
  for(i=0; i<X; i++) { \
    CDBG_HIGH("\n%s: ", __func__); \
    for(j=0; j<Y; j++) {\
      CDBG_HIGH("%lf ", M[j+(i*X)]); \
    } \
  } \
})

#undef STATS_DISABLE_HAF_ALGO_MASK
#ifdef _ANDROID_
#include <utils/Log.h>
#include <cutils/properties.h>
#define STATS_DISABLE_HAF_ALGO_MASK(mask) {\
                                  char prop[PROPERTY_VALUE_MAX];\
                                  property_get("persist.camera.stats.disablehaf", prop, "0"); \
                                  mask = atoi (prop);};

#define STATS_KPI_MASK(mask) {\
                      char prop[PROPERTY_VALUE_MAX];\
                      property_get("debug.camera.kpi.enable", prop, "0"); \
                      mask = atoi(prop);};

#else
#define STATS_DISABLE_HAF_ALGO_MASK(mask)  {mask = 0;}
#define STATS_KPI_MASK(mask) {mask = 0;}

#endif

#endif /* __CAMERA_DBG_H__ */
