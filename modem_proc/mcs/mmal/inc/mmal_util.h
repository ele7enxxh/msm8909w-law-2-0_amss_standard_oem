#ifndef MMAL_UTIL_H
#define MMAL_UTIL_H
/*!
  @file mmal_util.h

  @brief
   Provides a number of basic utilities to the Modem Metrics Abstraction Layer

*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mmal/inc/mmal_util.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/26/11   ars     Code review comments. 
03/25/11   ars     Initial Checkin

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "mcs_variation.h"
#include <mmal_cfg.h>
#include <msgcfg.h>
#include <msg.h>
#include <err.h>
#include <assert.h>
#include <comdef.h>
#ifdef FEATURE_MMAL_POSIX
#include <pthread.h>
#endif
#include <rex.h>

/*=============================================================================

                         OS Abstraction Layer.

=============================================================================*/
#ifdef FEATURE_MMAL_POSIX
typedef pthread_mutex_t mmal_crit_sect_s;
#else
typedef rex_crit_sect_type mmal_crit_sect_s;
#endif

typedef struct
{
  rex_tcb_type * tcb;
  rex_sigs_type  signal_id;
} mmal_signal_s;

typedef rex_sigs_type mmal_signal_mask_s;

typedef rex_sigs_type mmal_signal_result_t;

/*=============================================================================

  MACRO: MMAL_ASSERT

=============================================================================*/
/*!
    @brief
     Given a boolean expression, verify that the input expression is TRUE.
     If the input expression is FALSE, flags an error and resets SW stack.

    @detail
     Use this in all places where you are NOT handling or otherwise checking 
     for this error gracefully. This can be used for  out of range
     function parameters, NULL pointers and unintentional fall to the default
     case of a switch statement. The assumption is that no additional error
     recovery mechanism is present in the code at the time these macros are
     placed. If and when error recovery code is developed, default class 
     macros are expected to be changed to MMAL_ERROR macro.

    @note
     This is not good for error validation at a module's top level fns which 
     are probably seeing the inpur arguents from a different module. There 
     proper error checks are recommended and a grecful recovery is desirable.
     There use MMAL_ERROR macros to catch error right where it is seen.

    @return
     None
*/
/*===========================================================================*/
#define MMAL_ASSERT(exp) ASSERT(exp)

typedef enum
{
  MMAL_CIQ_LT01_METRIC                         = 0x1,
  MMAL_CIQ_GS01_METRIC                         = 0x2,
  MMAL_CIQ_GS02_METRIC                         = 0x4,
  MMAL_CIQ_GS03_METRIC                         = 0x8,
  MMAL_CIQ_GS30_METRIC                         = 0x10,
  MMAL_CIQ_GS31_METRIC                         = 0x20,
  MMAL_CIQ_GS34_METRIC                         = 0x40,
  MMAL_CIQ_GS35_METRIC                         = 0x80,
  MMAL_CIQ_LT02_METRIC                         = 0x100,
  MMAL_CIQ_LT03_METRIC                         = 0x200,
  MMAL_CIQ_LT04_METRIC                         = 0x400,
  MMAL_CIQ_LT05_METRIC                         = 0x800,
  MMAL_CIQ_LT06_METRIC                         = 0x1000,
  MMAL_CIQ_LT07_METRIC                         = 0x2000,
  MMAL_CIQ_LT08_METRIC                         = 0x4000,
  MMAL_CIQ_LT09_METRIC                         = 0x8000,
  MMAL_CIQ_LT10_METRIC                         = 0x10000,
  MMAL_CIQ_LT11_METRIC                         = 0x20000,
  MMAL_CIQ_LT12_METRIC                         = 0x40000,
  MMAL_CIQ_LT13_METRIC                         = 0x80000,
  MMAL_CIQ_GS15_METRIC                         = 0x100000,
  MMAL_CIQ_GS18_METRIC                         = 0x200000,
  MMAL_CIQ_GS19_METRIC                         = 0x400000,
  MMAL_CIQ_GS46_METRIC                         = 0x800000,
  MMAL_CIQ_GS47_METRIC                         = 0x1000000,
  MMAL_CIQ_GS6E_METRIC                         = 0x2000000,
  MMAL_CIQ_SS25_METRIC                         = 0x4000000,
  MMAL_CIQ_SS2B_METRIC                         = 0x8000000,
} mmal_ciq_metric_e;

/*===========================================================================

  FUNCTION:  mmal_ciq_fend_check_if_metric_enabled

===========================================================================*/
/*!
    @brief

    @detail

    @return

    @Note

*/
/*=========================================================================*/
boolean mmal_ciq_fend_check_if_metric_enabled
(
  mmal_ciq_metric_e   metric
);

/*===========================================================================*/
/*  The signals that can be sent to the mmal task */
/*---------------------------------------------------------------------------*/
/* to be set whenever data is placed in the mmal task's message queue */
#define MMAL_TASK_MSG_SIG     0x00000001
/*---------------------------------------------------------------------------*/
/*  to be set for MMAL's MSGR REX client & associated queue */
#define MMAL_MSGR_Q_SIG       0x00000002
/*---------------------------------------------------------------------------*/
/* NOTE: TASK_STOP_SIG = 0x00004000 comes from task.h */
/*===========================================================================*/

INLINE void mmal_crit_sect_init( mmal_crit_sect_s * lock_ptr )
{
#ifdef FEATURE_MMAL_POSIX
   pthread_mutexattr_t attr;
   MMAL_ASSERT( lock_ptr != NULL );

   if( pthread_mutexattr_init(&attr) !=0 )
   {
     ERR_FATAL("failed on pthread_mutexattr_init",0,0,0);
   }
   if( pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) !=0 )
   {
     ERR_FATAL("failed on pthread_mutexattr_settype",0,0,0);
   }
   if( pthread_mutex_init(lock_ptr, &attr) != 0 )
   {
     ERR_FATAL("failed on thread_mutex_init",0,0,0);
   }
   if(pthread_mutexattr_destroy(&attr)!=0)
   {
     MSG_HIGH(
      "WARNING: Potential memory leak. Could not destroy pthread_mutex_attr",
      0,0,0);
   }

#else
  rex_init_crit_sect( (rex_crit_sect_type*) lock_ptr );
#endif
}

INLINE void mmal_crit_sect_deinit( mmal_crit_sect_s * lock_ptr )
{
#ifdef FEATURE_MMAL_POSIX

  MMAL_ASSERT( lock_ptr != NULL );

  if( pthread_mutex_destroy(lock_ptr) !=0 )
  {
    ERR_FATAL("Couldn't destroy pthread mutex",0,0,0);
  }

#endif /* FEATURE_MMAL_POSIX */
}

INLINE void mmal_crit_sect_enter( mmal_crit_sect_s * crit_sect )
{
  MMAL_ASSERT( crit_sect != NULL );
#ifdef FEATURE_MMAL_POSIX
  if( pthread_mutex_lock(crit_sect) != 0 )
  {
    ERR_FATAL("mmal_crit_sect_enter: failed on pthread_mutex_lock",0,0,0);
  }
#else
  rex_enter_crit_sect ( (rex_crit_sect_type*) crit_sect );
#endif
}

INLINE void mmal_crit_sect_leave( mmal_crit_sect_s * crit_sect )
{
  MMAL_ASSERT( crit_sect != NULL );
#ifdef FEATURE_MMAL_POSIX
  if(pthread_mutex_unlock(crit_sect)!=0)
  {
    ERR_FATAL("mmal_crit_sect_leave: Failed pthread_mutex_unlock",0,0,0);
  }
#else
  rex_leave_crit_sect ( (rex_crit_sect_type*) crit_sect );
#endif
}

/*=============================================================================
                         Message Macros
=============================================================================*/


#define MMAL_MSG(msg) \
  MSG( MSG_SSID_LTE, MSG_LEGACY_HIGH, msg );

#define MMAL_MSG_ERROR(msg) \
  MSG( MSG_SSID_LTE, MSG_LEGACY_ERROR, msg );

#define MMAL_MSG_SPRINTF_1(msg, a1 ) \
  MSG_SPRINTF_1( MSG_SSID_LTE, MSG_LEGACY_HIGH, msg, (a1))

#define MMAL_MSG_SPRINTF_2(msg, a1, a2) \
  MSG_SPRINTF_2( MSG_SSID_LTE, MSG_LEGACY_HIGH, msg, (a1), (a2))

#define MMAL_MSG_SPRINTF_3(msg, a1, a2, a3 ) \
  MSG_SPRINTF_3( MSG_SSID_LTE, MSG_LEGACY_HIGH, msg, (a1), (a2), (a3))

#define MMAL_MSG_SPRINTF_4(msg, a1, a2, a3, a4 ) \
  MSG_SPRINTF_4( MSG_SSID_LTE, MSG_LEGACY_HIGH, msg, (a1), (a2), (a3), (a4))

/*=============================================================================
                         MEM alloc/free Macros
=============================================================================*/
#define MMAL_MEM_ALLOC( size, client ) modem_mem_alloc( size, client )
#define MMAL_MEM_FREE( ptr, client ) modem_mem_free( ptr, client )

/*===========================================================================*/

/*=============================================================================

  FUNCTION:  mmal_signal_init

=============================================================================*/
/*!
  @brief
  Initializes the signal

*/
/*===========================================================================*/
INLINE void mmal_signal_init
(
  mmal_signal_s* signal,
  rex_tcb_type* tcb,
  rex_sigs_type signal_id
)
{
  signal->tcb = tcb;
  signal->signal_id = signal_id;
}

/*=============================================================================

  FUNCTION:  mmal_signal_set

=============================================================================*/
/*!
  @brief
  Sets the signal

*/
/*===========================================================================*/
INLINE void mmal_signal_set( mmal_signal_s* signal )
{
  (void)rex_set_sigs( signal->tcb, signal->signal_id);
}

/*=============================================================================

  FUNCTION:  mmal_signal_mask_init

=============================================================================*/
/*!
  @brief
  Adds a signal to the mask

*/
/*===========================================================================*/
INLINE void mmal_signal_mask_init ( mmal_signal_mask_s* mask )
{
  *mask = 0;
}

/*=============================================================================

  FUNCTION:  mmal_signal_mask_add

=============================================================================*/
/*!
  @brief
  Adds a signal to the mask

*/
/*===========================================================================*/
INLINE void mmal_signal_mask_add ( mmal_signal_mask_s* mask, 
                                   mmal_signal_s* signal )
{
  *mask |= signal->signal_id;
}

/*=============================================================================

  FUNCTION:  mmal_signal_mask_wait

=============================================================================*/
/*!
  @brief
  Waits on a signal

*/
/*===========================================================================*/
INLINE mmal_signal_result_t mmal_signal_mask_wait( mmal_signal_mask_s* mask )
{
  return (mmal_signal_result_t)rex_wait( (rex_sigs_type)*mask );
}

/*=============================================================================

  FUNCTION:  mmal_signal_is_set

=============================================================================*/
/*!
  @brief
  Checks to see if a signal is set in the signal result.

  @return true if the signal was set, false otherwise.
*/
/*===========================================================================*/
INLINE boolean mmal_signal_is_set
(
  const mmal_signal_result_t* result,
  const mmal_signal_s* signal
)
{
  return ((*result & signal->signal_id) != 0);
}

/*=============================================================================

  FUNCTION:  mmal_signal_clear

=============================================================================*/
/*!
  @brief
  Clears the signal

*/
/*===========================================================================*/
INLINE void mmal_signal_clear( mmal_signal_s* signal )
{
  (void) rex_clr_sigs( signal->tcb, signal->signal_id );
}

#endif /* MMAL_UTIL_H */
