#ifndef TRACER_MUTEX_H
#define TRACER_MUTEX_H
/*===========================================================================
  @file tracer_mutex.h

   tracer_lock mutex
   To be included by only one file unless modified.

               Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/debugtrace/tracer/src/qurt/tracer_mutex.h#1 $ */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include <qurt_mutex.h>

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
extern qurt_mutex_t tracer_lock;

/**
  @brief Translator to target specific mutex init function.
*/
#define tracer_mutex_init() qurt_mutex_init(&tracer_lock)

/**
  @brief Translator to target specific mutex destroy function.
*/
#define tracer_mutex_destroy() qurt_mutex_destroy(&tracer_lock)

/**
  @brief Translator to target specific mutex lock function.
*/
#define tracer_mutex_lock() qurt_mutex_lock(&tracer_lock)

/**
  @brief Translator to target specific mutex unlock function.
*/
#define tracer_mutex_unlock() qurt_mutex_unlock(&tracer_lock)

#endif /* #ifndef TRACER_MUTEX_H */

