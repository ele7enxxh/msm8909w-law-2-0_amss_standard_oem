#ifndef QTIMER_H
#define QTIMER_H
/*=============================================================================

                 qtimer.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   Prototypes of qurt_timer API 

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2010
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qube/qtimer.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file, Fix warning when running doxygen 
=============================================================================*/
#include "qurt_timer.h"
#include "qurt_sclk.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QTIMER_MAX_GROUPS     QURT_TIMER_MAX_GROUPS
#define QTIMER_ONESHOT        QURT_TIMER_ONESHOT
#define QTIMER_PERIODIC       QURT_TIMER_PERIODIC

#ifdef QTIMER_TXCO
/* Conversion from TCXO clock ticks to microseconds at the nominal frequency */
#define QTIMER_TIMETICK_TO_US(ticks) (((unsigned long long)(ticks)*(1000000))/TIMETICK_NOMINAL_FREQ_HZ)

/*---------------------------------------------------------------------------*/
/* MINIMUM Microseconds Value = 125 Microseconds                              */
/*---------------------------------------------------------------------------*/
#define QTIMER_MIN_DURATION             125
/*---------------------------------------------------------------------------*/
/* MAXIMUM Microseconds Value = 3 Minutes                                    */
/*---------------------------------------------------------------------------*/
#define QTIMER_MAX_DURATION             (3 * 60 * 1000 * 1000)

#else

/* Conversion from sleep clock ticks to microseconds at the nominal frequency */
// 1000*1000/32768 = 30.517578125
#define QTIMER_TIMETICK_TO_US(ticks) (((unsigned long long)(ticks)*30ul)+((unsigned long long)(ticks)>>1)+((unsigned long long)(ticks)>>6)+((unsigned long long)(ticks)>>9))

/*---------------------------------------------------------------------------*/
/* MINIMUM Microseconds Value = 100 Microseconds (Sleep timer)               */
/*---------------------------------------------------------------------------*/
#define QTIMER_MIN_DURATION             100uL
/*---------------------------------------------------------------------------*/
/* MAXIMUM Microseconds Value = 36 hours                                     */
/*---------------------------------------------------------------------------*/
#define QTIMER_MAX_DURATION             ((unsigned long long)(36ul * 3600ul * 1000ul * 1000uLL))

#endif /* endif QTIMER_TXCO */  

#define qtimer_attr_setgroup        qurt_timer_attr_set_group
#define qtimer_attr_getremaining        qurt_timer_attr_get_remaining
#define qtimer_cmd_free_t        qurt_timer_cmd_free_t
#define qtimer_attr_setexpiry        qurt_timer_attr_set_expiry
#define qtimer_cmd_getattr        qurt_timer_cmd_getattr
#define qtimer_attr_getgroup        qurt_timer_attr_get_group
#define qtimer_attr_t        qurt_timer_attr_t
#define qtimer_cmd_t        qurt_timer_cmd_t
#define qtimer_recover_pc        qurt_timer_recover_pc
#define qtimer_cmd_create        qurt_timer_cmd_create
#define qtimer_type_t        qurt_timer_type_t
#define qtimer_is_init        qurt_timer_is_init
#define qtimer_attr_settype        qurt_timer_attr_set_type
#define qtimer_cmd_group        qurt_timer_cmd_group
#define qtimer_cmd_group_t        qurt_timer_cmd_group_t
#define qtimer_t        qurt_timer_t
#define sclk_timer_restart        qurt_timer_restart
#define qtimer_restart        qurt_timer_restart
#define qtimer_callback        qurt_timer_callback
#define qtimer_attr_gettype        qurt_timer_attr_get_type
#define qtimer_cmd_create_t        qurt_timer_cmd_create_t
#define qtimer_type        qurt_timer_type
#define qtimer_attr_init        qurt_timer_attr_init
#define qtimer_attr_getduration        qurt_timer_attr_get_duration
#define qtimer_delete        qurt_timer_delete
#define qtimer_group_disable        qurt_timer_group_disable
#define qtimer_group_enable        qurt_timer_group_enable
#define qtimer_attr_setduration        qurt_timer_attr_set_duration
#define qtimer_duration_t        qurt_timer_duration_t
#define qtimer_sleep        qurt_timer_sleep
#define qtimer_get_attr        qurt_timer_get_attr
#define qtimer_cmd_getticks_t        qurt_timer_cmd_getticks_t
#define qtimer_time_t        qurt_timer_time_t
#define qtimer_cmd_getattr_t        qurt_timer_cmd_getattr_t
#define sclk_timer_create        qurt_timer_create
#define qtimer_cmd_getticks        qurt_timer_cmd_getticks
#define qtimer_cmd_free        qurt_timer_cmd_free
#define sclk_timer_stop        qurt_timer_stop
#define qtimer_stop        qurt_timer_stop
#define qtimer_callback_t        qurt_timer_callback_t
#define qsystem_sclk_attr_getticks       qurt_timer_get_ticks 
#define qsystem_sclk_attr_getexpiry   qurt_sysclock_get_expiry
#define qsystem_sclk_attr_gethwticks  qurt_sysclock_get_hw_ticks

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QTIMER_H */
