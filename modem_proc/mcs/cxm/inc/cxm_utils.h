#ifndef CXM_UTILS_H
#define CXM_UTILS_H
/*!
  @file cxm_utils.h

  @brief
   Provides a number of basic utilities to the Co-Existence Manager Layer

*/

/*=============================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/inc/cxm_utils.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/14/14   ckk     Added WCI2 type4 & type7[pwr_bit] support
06/10/13   btl     Add COEX power limit timer signal
09/20/12   btl     Add DIAG & UART signals
07/16/12   ag      Using CXM SSID
07/10/12   cab     changing assert.h to amssassert.h
06/26/12   btl     Wdog support
04/08/12   ckk     Initial Checkin

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include <msgcfg.h>
#include <msg.h>
#include <err.h>
#include <amssassert.h>
#include <comdef.h>
#include <rex.h>
#include "modem_mem.h"
#include <IxErrno.h>
#include "cxm_intf_types.h"

/*=============================================================================

                   INTERNAL DEFINITIONS AND TYPES

=============================================================================*/
/*! @brief CXM task rex control block
 * On target these declarations reside in service/task/task.c */
extern rex_tcb_type cxm_tcb;

/* defined in utils.c */
extern timer_group_type cxm_nondeferrable_timer_group;

/* macro for checking if a COEX behavior is enabled */
#define COEX_SYS_ENABLED(b) ((b)&coex_sys_behavior_mask)

/*behavior of the system*/
extern cxm_sys_bhvr_t coex_sys_behavior_mask;
/*=============================================================================

                         OS Abstraction Layer.

=============================================================================*/
typedef rex_sigs_type cxm_signal_mask_t;

typedef rex_sigs_type cxm_signal_result_t;

#define CXM_ERR_FATAL(format, exp1, exp2, exp3) \
          ERR_FATAL(format, exp1, exp2, exp3)

/*=============================================================================

  MACRO: CXM_ASSERT

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
     macros are expected to be changed to CXM_ERROR macro.

    @note
     This is not good for error validation at a module's top level fns which 
     are probably seeing the input arguents from a different module. There 
     proper error checks are recommended and a grecful recovery is desirable.
     There use CXM_ERROR macros to catch error right where it is seen.

*/
/*===========================================================================*/
#define CXM_ASSERT(exp) \
  if( !(exp) ) \
  { \
     CXM_ERR_FATAL( "MCS_CXM assertion " #exp " failed", 0, 0, 0 ); \
  }

/*===========================================================================*/
/*  The signals that can be sent to the cxm task */
/*---------------------------------------------------------------------------*/
/*  to be set for CXM's MSGR REX client & associated queue */
#define CXM_MSGR_Q_SIG                 0x00000001
/*---------------------------------------------------------------------------*/
/* to be set new QMI interface gets a message (req) for CXM's COEX service */
#define CXM_COEX_QMI_SIG               0x00000002
/*---------------------------------------------------------------------------*/
/*  CXM's watchdog signal */
#define CXM_WDOG_SIG                   0x00000004
/*---------------------------------------------------------------------------*/
/* signal for timer handlers used by CXM Diag */
#define CXM_DIAG_SEND_SIG              0x00000008
/*---------------------------------------------------------------------------*/
/* signal that COEX has a message from QMB to process */
#define CXM_COEX_QMB_SIG               0x00000020
/*---------------------------------------------------------------------------*/
/* signal that COEX power limit timed out */
#define CXM_COEX_PWR_LMT_WDOG_SIG      0x00000040
/*---------------------------------------------------------------------------*/
/* signal used to call oem handler */
#define CXM_OEM_SIG                    0x00000080
/*---------------------------------------------------------------------------*/
/* signal that QMB response timed out */
#define CXM_COEX_QMB_TIMER_SIG         0x00000100
/*---------------------------------------------------------------------------*/
/* signal that CXM has been granted the antenna */
#define CXM_ANTENNA_GRANT_SIG          0x00000200
/*---------------------------------------------------------------------------*/
/* signal that CXM has to release the antenna */
#define CXM_ANTENNA_RELEASE_SIG        0x00000400
/*---------------------------------------------------------------------------*/
/* signal used to call oem handler */
#define CXM_COEX_SMDL_SIG              0x00000800
/*---------------------------------------------------------------------------*/
/* signal used to indicate type 7[conn_state] holdoff timer has fired */
#define CXM_COEX_T7_CONN_ST_TMR_SIG    0x00001000
/*---------------------------------------------------------------------------*/
/* signal used to indicate type 7[pwr_state] holdoff timer has fired */
#define CXM_COEX_T7_PWR_ST_TMR_SIG     0x00002000
/*---------------------------------------------------------------------------*/
/* signal that UART RX has direct read data available */
#define CXM_UART_RX_DIRECT_SIG         0x00004000
/*---------------------------------------------------------------------------*/
/* signal that UART RX has direct read data available */
#define CXM_COEX_QMB_NV_TIMER_SIG      0x00008000
/*---------------------------------------------------------------------------*/
/* signal used to flush cxm log packets on a periodic timer */
#define CXM_LOG_PKT_FLUSH_SIG          0x00010000
/*---------------------------------------------------------------------------*/
/*  to be set when the task is stopped during power down */
#define CXM_TASK_STOP_SIG              0x10000000
/*---------------------------------------------------------------------------*/

/*===========================================================================*/
/* All the signals related to CXM task */
#define CXM_SIGS ( CXM_MSGR_Q_SIG            |\
                   CXM_COEX_QMI_SIG          |\
                   CXM_DIAG_SEND_SIG         |\
                   CXM_WDOG_SIG              |\
                   CXM_UART_RX_DIRECT_SIG    |\
                   CXM_COEX_QMB_SIG          |\
                   CXM_COEX_PWR_LMT_WDOG_SIG |\
                   CXM_OEM_SIG               |\
                   CXM_COEX_QMB_TIMER_SIG    |\
                   CXM_ANTENNA_GRANT_SIG     |\
                   CXM_ANTENNA_RELEASE_SIG   |\
                   CXM_COEX_SMDL_SIG         |\
                   CXM_COEX_T7_CONN_ST_TMR_SIG |\
                   CXM_COEX_T7_PWR_ST_TMR_SIG  |\
                   CXM_COEX_QMB_NV_TIMER_SIG |\
                   CXM_LOG_PKT_FLUSH_SIG |\
                   CXM_TASK_STOP_SIG )

/*===========================================================================*/
/*handler type for oem signal*/
typedef void (*coex_oem_handler_type)(void);

/*===========================================================================*/


/*=============================================================================
                         Message Macros
=============================================================================*/

#define CXM_MSG_0(LVL, TEXT) \
  MSG( MSG_SSID_CXM, MSG_LEGACY_ ## LVL, "MCS_CXM: " TEXT );

#define CXM_MSG_1(LVL, TEXT, A) \
  MSG_1( MSG_SSID_CXM, MSG_LEGACY_ ## LVL, "MCS_CXM: " TEXT, A );

#define CXM_MSG_2(LVL, TEXT, A, B) \
  MSG_2( MSG_SSID_CXM, MSG_LEGACY_ ## LVL, "MCS_CXM: " TEXT, A, B );

#define CXM_MSG_3(LVL, TEXT, A, B, C) \
  MSG_3( MSG_SSID_CXM, MSG_LEGACY_ ## LVL, "MCS_CXM: " TEXT, A, B, C );

#define CXM_MSG_4(LVL, TEXT, A, B, C, D) \
  MSG_4( MSG_SSID_CXM, MSG_LEGACY_ ## LVL, "MCS_CXM: " TEXT, A, B, C, D );

#define CXM_MSG_5(LVL, TEXT, A, B, C, D, E) \
  MSG_5( MSG_SSID_CXM, MSG_LEGACY_ ## LVL, "MCS_CXM: " TEXT, A, B, C, D, E );

#define CXM_MSG_6(LVL, TEXT, A, B, C, D, E, F) \
  MSG_6( MSG_SSID_CXM, MSG_LEGACY_ ## LVL, "MCS_CXM: " TEXT, A, B, C, D, E, F );

#define CXM_MSG_7(LVL, TEXT, A, B, C, D, E, F, G) \
  MSG_7( MSG_SSID_CXM, MSG_LEGACY_ ## LVL, "MCS_CXM: " TEXT, A, B, C, D, E, F, G );

#define CXM_MSG_8(LVL, TEXT, A, B, C, D, E, F, G, H ) \
  MSG_8( MSG_SSID_CXM, MSG_LEGACY_ ## LVL, "MCS_CXM: " TEXT, A, B, C, D, E, F, G, H );

#define CXM_MSG_9(LVL, TEXT, A, B, C, D, E, F, G, H, I ) \
  MSG_9( MSG_SSID_CXM, MSG_LEGACY_ ## LVL, "MCS_CXM: " TEXT, A, B, C, D, E, F, G, H, I );

#define CXM_MSG_SPRINTF_1(msg, a1 ) \
  MSG_SPRINTF_1( MSG_SSID_CXM, MSG_LEGACY_HIGH, msg, (a1))

#define CXM_MSG_SPRINTF_2(msg, a1, a2) \
  MSG_SPRINTF_2( MSG_SSID_CXM, MSG_LEGACY_HIGH, msg, (a1), (a2))

#define CXM_MSG_SPRINTF_3(msg, a1, a2, a3 ) \
  MSG_SPRINTF_3( MSG_SSID_CXM, MSG_LEGACY_HIGH, msg, (a1), (a2), (a3))

#define CXM_MSG_SPRINTF_4(msg, a1, a2, a3, a4 ) \
  MSG_SPRINTF_4( MSG_SSID_CXM, MSG_LEGACY_HIGH, msg, (a1), (a2), (a3), (a4))

/*=============================================================================
                         MEM alloc/free Macros
=============================================================================*/
#define CXM_MEM_ALLOC( size ) modem_mem_alloc( size, MODEM_MEM_CLIENT_MCS )
#define CXM_MEM_CALLOC( cnt, size ) modem_mem_calloc( cnt, size, MODEM_MEM_CLIENT_MCS )
#define CXM_MEM_FREE( ptr ) modem_mem_free( ptr, MODEM_MEM_CLIENT_MCS )

/*=============================================================================
                         EXTERNAL FUNCTION DEFINITIONS
=============================================================================*/
/*===========================================================================

  FUNCTION:  cxm_timer_group_init

===========================================================================*/
/*!
  @brief
    Initialize non-deferrable timer group for cxm

  @return
*/
/*=========================================================================*/
void cxm_timer_group_init (void);

/*===========================================================================

  FUNCTION:  cxm_float_to_unsigned_Q16

===========================================================================*/
/*!
  @brief
    Convert from a float to a Q16.16 (fixed point, 16 integer bits and 16 
    fractional bits) format

  @return
    errno_enum_type
      E_SUCCESS
      E_FAILURE if overflow
    uint32 Q8 number
*/
/*=========================================================================*/
errno_enum_type cxm_float_to_unsigned_Q16(float f_in, uint32 *q_out);

/*===========================================================================

  FUNCTION:  cxm_float_to_unsigned_Q8

===========================================================================*/
/*!
  @brief
    Convert from a float to a Q24.8 (fixed point, 24 integer bits and 8 
    fractional bits) format

  @return
    errno_enum_type
      E_SUCCESS
      E_FAILURE if overflow
    uint32 Q8 number
*/
/*=========================================================================*/
errno_enum_type cxm_float_to_unsigned_Q8(float f_in, uint32 *q_out);

/*===========================================================================

  FUNCTION:  cxm_float_to_signed_Q8

===========================================================================*/
/*!
  @brief
    Convert from a float to a signed Q24.8 (fixed point, 24 integer 
    bits and 8 fractional bits) format

  @return
    errno_enum_type
      E_SUCCESS
      E_FAILURE if overflow
    int32 Q8 number
*/
/*=========================================================================*/
errno_enum_type cxm_float_to_signed_Q8(float f_in, int32 *q_out);

/*===========================================================================

  FUNCTION:  cxm_unsigned_Q8_to_float

===========================================================================*/
/*!
  @brief
    Convert from a Q24.8 (fixed point, 24 integer bits and 8 
    fractional bits) format to a float

  @return
    errno_enum_type
    uint32 Q8 number
*/
/*=========================================================================*/
errno_enum_type cxm_unsigned_Q8_to_float(uint32 q_in, float *f_out);

/*===========================================================================*/

/*===========================================================================

  FUNCTION:  cxm_signed_Q8_to_float

===========================================================================*/
/*!
  @brief
    Convert from a Q24.8 (fixed point, 24 integer bits and 8 
    fractional bits) format to a float

  @return
    errno_enum_type
    int32 Q8 number
*/
/*=========================================================================*/
errno_enum_type cxm_signed_Q8_to_float(int32 q_in, float *f_out);

/*=============================================================================

  FUNCTION:  cxm_register_oem_handler

=============================================================================*/
/*!
    @brief
    Registers an OEM handler that will be called when CXM_OEM_SIG is received
 
    @return
    errno_enum_type
*/
/*===========================================================================*/

errno_enum_type cxm_register_oem_handler(coex_oem_handler_type oem_handler);

/*=============================================================================

  FUNCTION:  cxm_terminate_task

=============================================================================*/
/*!
    @brief
    Terminates the CXM task by sending signal CXM_TASK_STOP_SIG
 
    @return
    errno_enum_type
*/
/*===========================================================================*/

void cxm_terminate_task(void);

#endif /* CXM_UTILS_H */
