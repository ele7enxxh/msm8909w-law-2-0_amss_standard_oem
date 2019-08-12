/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          Diag Stub.

General Description

  This file contains the stub routines of diag bring up.

Copyright (c) 2013 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                               Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diag_bringup_stub.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.

===========================================================================*/

#include "msg.h"
#include "osal_rex_types.h"
#include "err.h"
#include "diag_bringup_stub.h"


static void diag_bringup_stub_test(void);

#if 0

//sleep
void sleep_task (dword argv)
{
    diag_bringup_stub_test();
}
//Picked from rcinit_autogen.c
//extern void sleep_task(dword argv);

/*static*/ rex_tcb_type sleep_tcb;
static rcinit_handle sleep_rcinit_handle = { RCINIT_TASK_REXTASK, { &sleep_tcb} };
/*static*/ /*const*/ rcinit_info rcinit_info_sleep = {
    "sleep",
    (rcinit_entry_p)sleep_task, TASK_PRIORITY(SLEEP_PRI_ORDER), &sleep_rcinit_handle,
    2048, RCINIT_NULL,
    REX_ANY_CPU_AFFINITY_MASK
};

#include "rcinit.h"
#include "task.h"
/*static*/ rex_tcb_type sleep_tcb;
//SLEEP_PRI_ORDER = 238;
static rcinit_handle sleep_rcinit_handle = { RCINIT_TASK_REXTASK, { &sleep_tcb} };
/*static*/ /*const*/ rcinit_info rcinit_info_sleep = {
    "sleep",
    (rcinit_entry_p)sleep_task, TASK_PRIORITY(/*SLEEP_PRI_ORDER*/ 238), &sleep_rcinit_handle,
    2048, RCINIT_NULL,
    REX_ANY_CPU_AFFINITY_MASK
};
#endif

//Err
void err_put_log (word line, const char *file_ptr)
{
    diag_bringup_stub_test();
}

void err_set_auto_action(err_action_type action)
{
    diag_bringup_stub_test();
}

static void diag_bringup_stub_test(void)
{
    uint32 i=0;
    i=i+1;
    MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Diag bring up stub test function");
}
