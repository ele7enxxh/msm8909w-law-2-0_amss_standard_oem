/*==========================================================================
 * FILE:         dlpager_aux.c
 *
 * DESCRIPTION:  dlpager aux task
 *
 * Copyright (c) 2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved. QUALCOMM Proprietary and Confidential.
 ===========================================================================*/

/*==========================================================================

  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/dlpager/src/dlpager_aux.c#1 $

  when       who     what, where, why
  --------   ---     --------------------------------------------------------
  01/01/15   ao      Initial revision
  ===========================================================================*/
#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <qurt.h>

#include "dlpager_constants.h"
#include "dlpager_log.h"
#include "memload_handler.h"

#define DLPAGER_AUX_ALL_SIGNALS 0xFFFFFFFF
#define DLPAGER_AUX_PLRU        1

static qurt_anysignal_t dlpager_aux_signal;

unsigned dlpager_aux_total_plru;

static void dlpager_aux_task(void* pArgs)
{
    unsigned signal;
    (void)pArgs;

    qurt_anysignal_init(&dlpager_aux_signal);

    while (1)
    {
        qurt_anysignal_wait(&dlpager_aux_signal, DLPAGER_AUX_ALL_SIGNALS);
        signal = qurt_anysignal_clear(&dlpager_aux_signal, DLPAGER_AUX_ALL_SIGNALS);

        if (signal & DLPAGER_AUX_PLRU)
        {
            /* TODO add ticks in to log here */
            unsigned log_idx = dlpager_log_start(DLP_PLRU, 0, 0, 0);
            dlpager_memload_loaded_pages_update(0);
            dlpager_aux_total_plru++;
            dlpager_log_end(DLP_PLRU, log_idx, 0, 0);
        }
    }
}

void dlpager_aux_check_plru(void)
{
    qurt_anysignal_set(&dlpager_aux_signal, DLPAGER_AUX_PLRU);
}


void dlpager_aux_init(void)
{
    int status;
    void *pStack;
    qurt_thread_t tid;
    qurt_thread_attr_t thread_attr;

    qurt_thread_attr_init (&thread_attr);
#define STACK_SIZE (4*1024)
    pStack = malloc (STACK_SIZE);

    if (pStack != NULL)
    {
        qurt_thread_attr_set_name (&thread_attr, "DLPager_aux");
        qurt_thread_attr_set_stack_addr (&thread_attr, pStack);
        qurt_thread_attr_set_stack_size (&thread_attr, STACK_SIZE);
        qurt_thread_attr_set_priority (&thread_attr, 200);
        status = qurt_thread_create (&tid, &thread_attr,
                                 dlpager_aux_task, (void *)0);
        assert(status == 0);
        DL_DEBUG("Starting DLPager aux thread , id=%d \n", tid);
    }
    else
    {
        assert(0);
    }
}
