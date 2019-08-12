#ifndef LMTSMGR_TASK_H
#define LMTSMGR_TASK_H
/*!
  @file lmtsmgr_task.h

  @brief
   Provides a number of basic utilities to the Limits Manager Layer

*/

/*=============================================================================

  Copyright (c) 2014-2016 Qualcomm Technologies Incorporated.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/limitsmgr/utils/inc/lmtsmgr_task.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/26/16   jm      F3 Reduction
06/23/14   sg      desense indication mechanism
04/28/14   jm      L+G Band Avoidance support
01/13/14   sk      Changes to release TRM resource for CM client
12/09/13   jm      DDR management phase 2 support (diversity power)
11/21/13   jm      Adding DDR management support
02/20/13   rj      Add prototype for lmtsmgr_set_sigs & lmtsmgr_clr_sigs
09/20/12   btl     Add DIAG & UART signals
07/16/12   ag      Using LMTSMGR SSID
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

/*=============================================================================

                   INTERNAL DEFINITIONS AND TYPES

=============================================================================*/
/*! @brief LMTSMGR task rex control block
 * On target these declarations reside in service/task/task.c */
extern rex_tcb_type lmtsmgr_tcb;

/*=============================================================================

                         OS Abstraction Layer.

=============================================================================*/
typedef rex_sigs_type lmtsmgr_signal_mask_t;

typedef rex_sigs_type lmtsmgr_signal_result_t;

/*=============================================================================

  MACRO: LMTSMGR_ASSERT

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
     macros are expected to be changed to LMTSMGR_ERROR macro.

    @note
     This is not good for error validation at a module's top level fns which 
     are probably seeing the input arguents from a different module. There 
     proper error checks are recommended and a grecful recovery is desirable.
     There use LMTSMGR_ERROR macros to catch error right where it is seen.

*/
/*===========================================================================*/
#define LMTSMGR_ASSERT(exp) ASSERT(exp)

/*===========================================================================*/
/*  The signals that can be sent to the lmtsmgr task */
/*---------------------------------------------------------------------------*/
/*  to be set for LMTSMGR's MSGR REX client & associated queue */
#define LMTSMGR_MSGR_Q_SIG      0x00000001
/*---------------------------------------------------------------------------*/
/*  LMTSMGR's watchdog signal */
#define LMTSMGR_WDOG_SIG        0x00000002
/*---------------------------------------------------------------------------*/
/*  to be set when the task is stopped during power down */
#define LMTSMGR_TASK_STOP_SIG   0x00000004
/*---------------------------------------------------------------------------*/
/*  to be set when the QMI interface gets a message for LMTSMGRs SAR service */
#define LMTSMGR_SAR_QMI_SIG   0x00000008

/*---------------------------------------------------------------------------*/
/*  to be set when the QMI interface gets a message for LMTSMGRs SAR service */
#define LMTSMGR_SAR_CALL_BACK_SIG   0x00000010

/*---------------------------------------------------------------------------*/
/* to be set by sub prio module whenever limitsmgr task need to do a release*/
#define LMTSMGR_TRM_REL_SIG   0x00000020

/*---------------------------------------------------------------------------*/
/* to be set by sub prio module whenever limitsmgr task need to do a release*/
#define LMTSMGR_DESENSE_CALC_SIG   0x00000040

/*===========================================================================*/
/* All the signals related to LMTSMGR task */
#define LMTSMGR_SIGS ( LMTSMGR_MSGR_Q_SIG             |\
                         LMTSMGR_WDOG_SIG             |\
                         LMTSMGR_TASK_STOP_SIG        |\
                         LMTSMGR_SAR_QMI_SIG          |\
                         LMTSMGR_SAR_CALL_BACK_SIG    |\
                         LMTSMGR_TRM_REL_SIG          |\
                         LMTSMGR_DESENSE_CALC_SIG )

/*===========================================================================*/


/*=============================================================================
                         Message Macros
=============================================================================*/

#define LMTSMGR_MSG_0(LVL, TEXT) \
  MSG( MSG_SSID_CXM, MSG_LEGACY_ ## LVL, TEXT );

#define LMTSMGR_MSG_1(LVL, TEXT, A) \
  MSG_1( MSG_SSID_CXM, MSG_LEGACY_ ## LVL, TEXT, A );

#define LMTSMGR_MSG_2(LVL, TEXT, A, B) \
  MSG_2( MSG_SSID_CXM, MSG_LEGACY_ ## LVL, TEXT, A, B );

#define LMTSMGR_MSG_3(LVL, TEXT, A, B, C) \
  MSG_3( MSG_SSID_CXM, MSG_LEGACY_ ## LVL, TEXT, A, B, C );

#define LMTSMGR_MSG_4(LVL, TEXT, A, B, C, D) \
  MSG_4( MSG_SSID_CXM, MSG_LEGACY_ ## LVL, TEXT, A, B, C, D );

#define LMTSMGR_MSG_5(LVL, TEXT, A, B, C, D, E) \
  MSG_5( MSG_SSID_CXM, MSG_LEGACY_ ## LVL, TEXT, A, B, C, D, E );

#define LMTSMGR_MSG_6(LVL, TEXT, A, B, C, D, E, F) \
  MSG_6( MSG_SSID_CXM, MSG_LEGACY_ ## LVL, TEXT, A, B, C, D, E, F );

#define LMTSMGR_MSG_7(LVL, TEXT, A, B, C, D, E, F, G) \
  MSG_7( MSG_SSID_CXM, MSG_LEGACY_ ## LVL, TEXT, A, B, C, D, E, F, G );

#define LMTSMGR_MSG_8(LVL, TEXT, A, B, C, D, E, F, G, H) \
  MSG_8( MSG_SSID_CXM, MSG_LEGACY_ ## LVL, TEXT, A, B, C, D, E, F, G, H );


#define LMTSMGR_MSG_SPRINTF_1(msg, a1 ) \
  MSG_SPRINTF_1( MSG_SSID_CXM, MSG_LEGACY_HIGH, msg, (a1))

#define LMTSMGR_MSG_SPRINTF_2(msg, a1, a2) \
  MSG_SPRINTF_2( MSG_SSID_CXM, MSG_LEGACY_HIGH, msg, (a1), (a2))

#define LMTSMGR_MSG_SPRINTF_3(msg, a1, a2, a3 ) \
  MSG_SPRINTF_3( MSG_SSID_CXM, MSG_LEGACY_HIGH, msg, (a1), (a2), (a3))

#define LMTSMGR_MSG_SPRINTF_4(msg, a1, a2, a3, a4 ) \
  MSG_SPRINTF_4( MSG_SSID_CXM, MSG_LEGACY_HIGH, msg, (a1), (a2), (a3), (a4))

#ifdef TEST_FRAMEWORK 
#error code not present
#else

#define LMTSMGR_DBG_MSG_0(LVL, TEXT) 
#define LMTSMGR_DBG_MSG_1(LVL, TEXT, A) 
#define LMTSMGR_DBG_MSG_2(LVL, TEXT, A, B) 
#define LMTSMGR_DBG_MSG_3(LVL, TEXT, A, B, C) 
#define LMTSMGR_DBG_MSG_4(LVL, TEXT, A, B, C, D) 
#define LMTSMGR_DBG_MSG_5(LVL, TEXT, A, B, C, D, E) 
#define LMTSMGR_DBG_MSG_6(LVL, TEXT, A, B, C, D, E, F) 
#define LMTSMGR_DBG_MSG_7(LVL, TEXT, A, B, C, D, E, F, G) 

#endif

/*=============================================================================
                         MEM alloc/free Macros
=============================================================================*/
#define LMTSMGR_MEM_ALLOC( size ) modem_mem_alloc( size, MODEM_MEM_CLIENT_MCS )
#define LMTSMGR_MEM_FREE( ptr ) modem_mem_free( ptr, MODEM_MEM_CLIENT_MCS )

/******************************************************************************
                 LimitsMgr Task prototypes.
******************************************************************************/
/*===========================================================================

FUNCTION LMTSMGR_SET_SIGS

DESCRIPTION    This function sets the signals for Lmtsmgr Manager task.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void lmtsmgr_set_sigs ( rex_sigs_type sigs );

/*=============================================================================

  FUNCTION:  lmtsmgr_clr_sigs

=============================================================================*/
/*!
    @brief
    Clears the required signals

    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_clr_sigs ( lmtsmgr_signal_mask_t sigs );

/*=============================================================================

  FUNCTION:  lmtsmgr_task_check_dog

=============================================================================*/
/*!
    @brief
    Checks is wdog signal is set and pets it and clears the wdog signal

    @return
    None
*/
/*===========================================================================*/
void lmtsmgr_task_check_dog (void);

#endif /* LMTSMGR_TASK_H */
