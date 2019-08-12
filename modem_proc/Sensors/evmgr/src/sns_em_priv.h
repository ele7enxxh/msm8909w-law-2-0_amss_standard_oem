#ifndef _SNS_EM_PRIV_H
#define _SNS_EM_PRIV_H

/*============================================================================
  @file sns_em_priv.h

  The sensors subsystem event manager private header.

  This header file contains the private interface for the sensors event manager.
  The Event Manager provides timer functionality to the various modules in the
  sensor subsystem.

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-01-22  dk   Added the SNS_EM_UPRINTF macros
  2014-04-22  sc   Initial version

============================================================================*/


/*---------------------------------------------------------------------------
* Include Files
* -------------------------------------------------------------------------*/
#include "sns_debug_str.h"
#include "sns_debug_api.h"

// ATS timer
#include "timer.h"


/*---------------------------------------------------------------------------
* Definitions and Type Declarations
* -------------------------------------------------------------------------*/

#define USEC_PER_SEC 1000000

typedef struct sns_em_tmr_s{
  uint8_t                bTimerDefined;
  uint8_t                bTimerIsUTimer;
  timer_type             sTimer;
  timer_ptr_type         pTimer;
  timer_group_ptr        pTimerGroup;
  sns_em_timer_type_e    tmr_type;
} sns_em_tmr_s;

#define SNS_EM_PRINTF0(level,msg)           SNS_PRINTF_STRING_##level##_0(SNS_DBG_MOD_EM,msg)
#define SNS_EM_PRINTF1(level,msg,p1)        SNS_PRINTF_STRING_##level##_1(SNS_DBG_MOD_EM,msg,p1)
#define SNS_EM_PRINTF2(level,msg,p1,p2)     SNS_PRINTF_STRING_##level##_2(SNS_DBG_MOD_EM,msg,p1,p2)
#define SNS_EM_PRINTF3(level,msg,p1,p2,p3)  SNS_PRINTF_STRING_##level##_3(SNS_DBG_MOD_EM,msg,p1,p2,p3)

#define SNS_EM_UPRINTF0(level,msg)           SNS_UMSG_##level##_0(SNS_DBG_MOD_EM,msg)
#define SNS_EM_UPRINTF1(level,msg,p1)        SNS_UMSG_##level##_1(SNS_DBG_MOD_EM,msg,p1)
#define SNS_EM_UPRINTF2(level,msg,p1,p2)     SNS_UMSG_##level##_2(SNS_DBG_MOD_EM,msg,p1,p2)
#define SNS_EM_UPRINTF3(level,msg,p1,p2,p3)  SNS_UMSG_##level##_3(SNS_DBG_MOD_EM,msg,p1,p2,p3)


#endif /*#ifndef _SNS_EM_PRIV_H*/
