/*!
  @file
  cfcmi.h

  @brief
  CFCM internal header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

  @author
  rohitj

*/

/*==============================================================================

  Copyright (c) 2014-15 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/src/cfcmi.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/22/15   rj      Incorrectly using assert.h which breaks QTF testing
11/20/14   rj      Log Packet support added in CFCM
10/09/14   rj      CFCM changes to Process monitor's input in its task context
08/20/14   mm      Adding macro CFCM_IS_DSM_MONITOR 
08/11/14   rj      Adding support for QSH in CFCM
07/04/14   rj      Update A2 client with generic name for IPA
07/03/14   rj      BUS BW changes
04/07/14   rj      initial version
==============================================================================*/

#ifndef CFCMI_H
#define CFCMI_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include "rcevt_rex.h"        /* RCEVT Signal Delivery by REX */
#include <comdef.h>
#include <err.h>
#include <amssassert.h>
#include <msg.h>
#include <msgr.h>
#include <modem_mem.h>
#include "stringl.h"
#include "cfcm.h"
#include "cfcm_cfg.h"
#include <DDITimetick.h>
#include "cfcm_msg_internal.h"

/* QMI Files */
#include "common_flow_control_management_impl_v01.h"

/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/
#define CFCM_CMD_MAX 8

extern const char * cfcm_cmd_str[CFCM_CMD_MAX];

extern const char * cfcm_monitor_str[CFCM_MONITOR_MAX];

extern cfcm_cfg_s cfcm_cfg;

typedef uint32 cfcm_timetick_t;

typedef uint32 cfcm_timetick_ms_t;

/*! @brief Global structure for CFCM task
*/
typedef struct
{
  boolean                     active;             /*!< the task is running? */
  boolean                     cfcm_disable;       /*!< CFCM is disable? */
  boolean                     diag_test_enabled;  /*!< CFCM testing via Diag is enabled? */
#ifdef CFCM_ON_TARGET
  DalDeviceHandle *           timetick_handle;  /*!< timetick handle */
#endif /* CFCM_ON_TARGET */
  /* Critical section */
  rex_crit_sect_type           crit_sect;
} cfcm_s;


extern cfcm_s cfcm;

/* Atomic section macros for initialization of critical section */
#define CFCM_ENTER_ATOMIC_SECT()     REX_DISABLE_PREMPTION()
#define CFCM_LEAVE_ATOMIC_SECT()     REX_ENABLE_PREMPTION()

/* Critical section macros */
#define CFCM_INIT_CRIT_SECT(lock)    rex_init_crit_sect(&(lock))
#define CFCM_ENTER_CRIT_SECT(lock)   rex_enter_crit_sect(&(lock))
#define CFCM_LEAVE_CRIT_SECT(lock)   rex_leave_crit_sect(&(lock))

#define IS_CFCM_DISABLED(is_disable) ((is_disable)== TRUE)

/*! @brief whether client is RLC client
*/
#define CFCM_IS_RLC_CLIENT(c)    (((c) == CFCM_CLIENT_WCDMA_RLC_DL) || \
                                  ((c) == CFCM_CLIENT_WCDMA_RLC_UL))

/*! @brief whether client is non-RLC client
*/
#define CFCM_IS_LTE_CLIENT(c)    (((c) == CFCM_CLIENT_LTE_UL)     || \
                                  ((c) == CFCM_CLIENT_LTE_RLC_DL) || \
                                  ((c) == CFCM_CLIENT_A2_UL_PER)  || \
                                  ((c) == CFCM_CLIENT_UL_PER)     || \
                                  ((c) == CFCM_CLIENT_LTE_ML1)     || \
                                  ((c) == CFCM_CLIENT_LTE_PDCP_DL) || \
                                  ((c) == CFCM_CLIENT_LTE_PDCP_UL) || \
                                  ((c) == CFCM_CLIENT_LTE_ML1_CPU))

/*! @brief whether monitor is Large Pool DSM
*/
#define CFCM_IS_DSM_LARGE_POOL_MONITOR(m)   (((m) == CFCM_MONITOR_DSM_LARGE_POOL_UL)  || \
                                             ((m) == CFCM_MONITOR_DSM_LARGE_POOL_DL))

/*! @brief whether monitor is Dup Pool DSM
*/
#define CFCM_IS_DSM_DUP_POOL_MONITOR(m)   (((m) == CFCM_MONITOR_DSM_DUP_POOL_UL)  || \
                                           ((m) == CFCM_MONITOR_DSM_DUP_POOL_DL))

/*! @brief whether monitor is Small Pool DSM
*/
#define CFCM_IS_DSM_SMALL_POOL_MONITOR(m) ((m) == CFCM_MONITOR_DSM_SMALL_POOL_DL)

/*! @brief whether monitor is any DSM pool
*/
#define CFCM_IS_DSM_MONITOR(m) (CFCM_IS_DSM_LARGE_POOL_MONITOR(m) || \
  CFCM_IS_DSM_DUP_POOL_MONITOR(m) || CFCM_IS_DSM_SMALL_POOL_MONITOR(m))

#define CFCM_IS_MONITOR_ID_VALID(monitor) ((monitor >= CFCM_MONITOR_THERMAL_PA) && (monitor < CFCM_MONITOR_MAX))

#define CFCM_MONITOR_ID_PRESENT_IN_MASK(mask, monitor) ((mask & (1 << monitor)) != 0)

/*=============================================================================

                         OS Abstraction Layer.

=============================================================================*/
typedef rex_sigs_type cfcm_signal_mask_t;

typedef rex_sigs_type cfcm_signal_result_t;

/*! @brief CFCM task rex control block
 * On target these declarations reside in service/task/task.c */
extern rex_tcb_type cfcm_tcb;

/*===========================================================================*/
/*  The signals that can be sent to the lmtsmgr task */
/*---------------------------------------------------------------------------*/
/*  to be set for CFCM's MSGR REX client & associated queue */
#define CFCM_MSGR_Q_SIG      0x00000001
/*---------------------------------------------------------------------------*/
/*  CFCM's watchdog signal */
#define CFCM_WDOG_SIG        0x00000002
/*---------------------------------------------------------------------------*/
/*  to be set when the task is stopped during power down */
#define CFCM_TASK_STOP_SIG   0x00000004
/*---------------------------------------------------------------------------*/
/*  to be set when the QMI interface gets a message for CFCM Bus BW service */
#define CFCM_TASK_QMI_SIG    0x00000008

/*===========================================================================*/

/* All the signals related to CFCM task */
#define CFCM_SIGS    ( CFCM_MSGR_Q_SIG           |\
                       CFCM_WDOG_SIG             |\
                       CFCM_TASK_STOP_SIG        |\
                       CFCM_TASK_QMI_SIG )

#if 0
#define CFCM_MSG_0(LVL, TEXT) \
  MSG( MSG_SSID_DFLT, MSG_LEGACY_ ## LVL, TEXT );

#define CFCM_MSG_1(LVL, TEXT, A) \
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ ## LVL, TEXT, A );

#define CFCM_MSG_2(LVL, TEXT, A, B) \
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ ## LVL, TEXT, A, B );

#define CFCM_MSG_3(LVL, TEXT, A, B, C) \
  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ ## LVL, TEXT, A, B, C );

#define CFCM_MSG_4(LVL, TEXT, A, B, C, D) \
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_ ## LVL, TEXT, A, B, C, D );

#define CFCM_MSG_5(LVL, TEXT, A, B, C, D, E) \
  MSG_5( MSG_SSID_DFLT, MSG_LEGACY_ ## LVL, TEXT, A, B, C, D, E );

#define CFCM_MSG_6(LVL, TEXT, A, B, C, D, E, F) \
  MSG_6( MSG_SSID_DFLT, MSG_LEGACY_ ## LVL, TEXT, A, B, C, D, E, F );

#define CFCM_MSG_8(LVL, TEXT, A, B, C, D, E, F, G, H) \
  MSG_8( MSG_SSID_DFLT, MSG_LEGACY_ ## LVL, TEXT, A, B, C, D, E, F, G, H );
#else
#define CFCM_MSG_0(LVL, TEXT) \
  MSG( MSG_SSID_CFCM, MSG_LEGACY_ ## LVL, TEXT );

#define CFCM_MSG_1(LVL, TEXT, A) \
  MSG_1( MSG_SSID_CFCM, MSG_LEGACY_ ## LVL, TEXT, A );

#define CFCM_MSG_2(LVL, TEXT, A, B) \
  MSG_2( MSG_SSID_CFCM, MSG_LEGACY_ ## LVL, TEXT, A, B );

#define CFCM_MSG_3(LVL, TEXT, A, B, C) \
  MSG_3( MSG_SSID_CFCM, MSG_LEGACY_ ## LVL, TEXT, A, B, C );

#define CFCM_MSG_4(LVL, TEXT, A, B, C, D) \
  MSG_4( MSG_SSID_CFCM, MSG_LEGACY_ ## LVL, TEXT, A, B, C, D );

#define CFCM_MSG_5(LVL, TEXT, A, B, C, D, E) \
  MSG_5( MSG_SSID_CFCM, MSG_LEGACY_ ## LVL, TEXT, A, B, C, D, E );

#define CFCM_MSG_6(LVL, TEXT, A, B, C, D, E, F) \
  MSG_6( MSG_SSID_CFCM, MSG_LEGACY_ ## LVL, TEXT, A, B, C, D, E, F );

#define CFCM_MSG_8(LVL, TEXT, A, B, C, D, E, F, G, H) \
  MSG_8( MSG_SSID_CFCM, MSG_LEGACY_ ## LVL, TEXT, A, B, C, D, E, F, G, H );


#define CFCM_MSG_SPRINTF_1(msg, a1 ) \
  MSG_SPRINTF_1( MSG_SSID_CFCM, MSG_LEGACY_HIGH, msg, (a1))

#define CFCM_MSG_SPRINTF_2(msg, a1, a2) \
  MSG_SPRINTF_2( MSG_SSID_CFCM, MSG_LEGACY_HIGH, msg, (a1), (a2))

#define CFCM_MSG_SPRINTF_3(msg, a1, a2, a3 ) \
  MSG_SPRINTF_3( MSG_SSID_CFCM, MSG_LEGACY_HIGH, msg, (a1), (a2), (a3))

#define CFCM_MSG_SPRINTF_4(msg, a1, a2, a3, a4 ) \
  MSG_SPRINTF_4( MSG_SSID_CFCM, MSG_LEGACY_HIGH, msg, (a1), (a2), (a3), (a4))

#ifdef TEST_FRAMEWORK 
#error code not present
#else

#define CFCM_DBG_MSG_0(LVL, TEXT) 
#define CFCM_DBG_MSG_1(LVL, TEXT, A) 
#define CFCM_DBG_MSG_2(LVL, TEXT, A, B) 
#define CFCM_DBG_MSG_3(LVL, TEXT, A, B, C) 
#define CFCM_DBG_MSG_4(LVL, TEXT, A, B, C, D) 
#define CFCM_DBG_MSG_5(LVL, TEXT, A, B, C, D, E) 

#endif
#endif

/*=============================================================================

  MACRO: CFCM_ASSERT

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
     macros are expected to be changed to CFCM_ERROR macro.

    @note
     This is not good for error validation at a module's top level fns which 
     are probably seeing the input arguents from a different module. There 
     proper error checks are recommended and a grecful recovery is desirable.
     There use CFCM_ERROR macros to catch error right where it is seen.

*/
/*===========================================================================*/
#define CFCM_ASSERT(exp) ASSERT(exp)


/* macro to prevent compiler warnings when the payload parameter is not used
   when a variable is only used in MSG_XXXX calls that may be compiled out */
#define CFCM_UNUSED(x) if(x){}

/*=============================================================================

  MACRO: CFCM_ERR_FATAL

=============================================================================*/
/*!
    @brief
     Does ERR_FATAL & print F3, flags an error and resets SW stack.
*/
/*===========================================================================*/

#define CFCM_ERR_FATAL(TEXT, A, B, C) \
  ERR_FATAL(TEXT, A, B, C )

/*==============================================================================

                    INTERNAL FUNCTION PROTOTYPES

==============================================================================*/
/*===========================================================================

FUNCTION CFCM_SET_SIGS

DESCRIPTION
  This function sets the signals for CFCM task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cfcm_set_sigs ( cfcm_signal_mask_t sigs );

/*===========================================================================

FUNCTION CFCM_CLR_SIGS

DESCRIPTION
  This function Clears the required signals for CFCM task.

DEPENDENCIES
  CFCM Task should be intialized

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cfcm_clr_sigs ( cfcm_signal_mask_t sigs );

/*===========================================================================

  FUNCTION:  cfcm_timetick_get

===========================================================================*/
/*!
  @brief
  Returns the current timetick value in 32-bit timeticks.
*/
/*=========================================================================*/
cfcm_timetick_t cfcm_timetick_get
(
  void
);

/*===========================================================================

  FUNCTION:  cfcm_timetick_get_ms

===========================================================================*/
/*!
  @brief
  Returns the current timetick value in msec and stored in 32 bit.
  This number is monotonically increasing and wraps around in 1193 hours
*/
/*=========================================================================*/
cfcm_timetick_ms_t cfcm_timetick_get_ms
(
  void
);

#endif /* CFCMI_H */
