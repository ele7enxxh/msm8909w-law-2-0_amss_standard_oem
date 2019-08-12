/*==========================================================================
 * FILE:         dlpager_log.c
 *
 * DESCRIPTION:  Dlpager logging
 *
 * Copyright (c) 2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved. QUALCOMM Proprietary and Confidential.
 ===========================================================================*/

/*==========================================================================

  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/dlpager/src/dlpager_log.c#1 $

  when       who     what, where, why
  --------   ---     --------------------------------------------------------
  10/30/14   ao      Initial revision
  ===========================================================================*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <atomic_ops.h>
#include <qurt.h>

#include "dlpager_main.h"

#ifndef assert
#define assert ASSERT
#endif

//#define DLP_DEBUG

#include "dlpager_log.h"

#define DLPAGER_LOG_ENTRIES_BITS  11
#define DLPAGER_LOG_ENTRIES_MASK  ((1<<DLPAGER_LOG_ENTRIES_BITS)-1)
#define DLPAGER_LOG_ENTRIES_MAX   (1<<DLPAGER_LOG_ENTRIES_BITS)

#define DLPAGER_LOG_TID_BITS   9
#define DLPAGER_LOG_DELTA_BITS 15
#define DLPAGER_LOG_CAUSE_BITS 8
#define DLPAGER_LOG_DELTA_MAX_VAL ((1 << DLPAGER_LOG_DELTA_BITS) - 1)

/** Log entry that logs the pager activity. The entries are logged when
    when event_t happens. */
typedef struct dlpager_log_entry {
    uint32_t pcycles32;
    dlpager_log_event_t event:5;
    uint32_t              clk:11;          /* MIPS from DCVS */
    uint32_t          evicted:16;          /* evicted page swap pool idx or evicted clk */

    uint32_t thread_id: DLPAGER_LOG_TID_BITS;   /* thread id for event */
    uint32_t dPcycles128: DLPAGER_LOG_DELTA_BITS; /* amount of time spent on this operation in K pcycles */
    uint32_t ssr_cause: DLPAGER_LOG_CAUSE_BITS; /* SSR Cause */

    void* fault_addr;            /* faulting address */
} dlpager_log_entry_t;

static unsigned dlpager_log_idx;
static dlpager_log_entry_t dlpager_log[DLPAGER_LOG_ENTRIES_MAX];

uint64_t dlpager_log_last_pcycles; /* Last 32 bits of pcycles logged */

void dlpager_log_init(void)
{
    assert((DLPAGER_LOG_TID_BITS +
            DLPAGER_LOG_DELTA_BITS +
            DLPAGER_LOG_CAUSE_BITS) <= (sizeof(uint32_t) * 8));
}

unsigned dlpager_log_start(dlpager_log_event_t event, void* fault_addr, unsigned int thread_id, unsigned int ssr)
{
    static atomic_word_t dlpager_log_atomic_idx = ATOMIC_INIT(0);
    unsigned log_idx;

#ifndef DLP_DEBUG
    if (event >= DLP_DEBUG_TYPE) return 0;
#endif

    log_idx = atomic_inc_return(&dlpager_log_atomic_idx) & DLPAGER_LOG_ENTRIES_MASK;
    dlpager_log_idx = log_idx;

    dlpager_log[log_idx].fault_addr    = fault_addr;
    dlpager_log[log_idx].event         = event;
    dlpager_log[log_idx].clk           = 0;
    dlpager_log[log_idx].evicted       = 0;
    dlpager_log[log_idx].thread_id     = thread_id;
    dlpager_log[log_idx].dPcycles128   = 0;
    dlpager_log[log_idx].ssr_cause     = ssr;

    uint64_t pcycles = qurt_get_core_pcycles();
    // TODO enable direct read asm volatile ("%0=UPCYCLELO" : "=r" (pcycles32));

    dlpager_log_last_pcycles           = pcycles;
    dlpager_log[log_idx].pcycles32     = (uint32_t)pcycles; /* last 32 bits of pcycles */


    DL_DEBUG("dlpager_log start %d %x %x\n", event, fault_addr, thread_id);

    return log_idx;
}

void dlpager_log_end(dlpager_log_event_t event, unsigned log_idx, unsigned int evicted, unsigned clk)
{
#ifndef DLP_DEBUG
    if (event >= DLP_DEBUG_TYPE) return;
#endif
    dlpager_log[log_idx].evicted = evicted;
    dlpager_log[log_idx].clk     = clk;

    unsigned end_pcycles = qurt_get_core_pcycles();
    // TODO enable direct read asm volatile ("%0=UPCYCLELO" : "=r" (end_pcycles));

    unsigned int delta_pcycles128 = (end_pcycles - dlpager_log[log_idx].pcycles32) >> 7;
    dlpager_log[log_idx].dPcycles128 = (delta_pcycles128 > DLPAGER_LOG_DELTA_MAX_VAL) ?
        DLPAGER_LOG_DELTA_MAX_VAL : delta_pcycles128;
#ifdef DLP_DEBUG
    /* for debug use more accurate pcycles delta */
    dlpager_log[log_idx].pcycles32 = end_pcycles - dlpager_log[log_idx].pcycles32;
#endif

    DL_DEBUG("dlpager_log end %d %x %x\n", event, evicted, clk);
}

/*
 * dump_log_table
 *
 * Dump the logs that indicate the activity of the pager.
 */
void dump_log_table (void)
{
    int i;
    char empty_string[] = "           ";

    printf ("Log index is at: %d\n\n", dlpager_log_idx);
    printf ("Log table: fault addr -- loaded page  --- thread ID\n");

    i = ((dlpager_log_idx + 1) < DLPAGER_LOG_ENTRIES_MAX) ? dlpager_log_idx + 1 : 0;
    do {
        if (dlpager_log[i].fault_addr != 0)
            printf ("%s0x%x  --  0x%03x  -- 0x%03x\n", empty_string, (unsigned int)(dlpager_log[i].fault_addr), dlpager_log[i].evicted, dlpager_log[i].thread_id);

        i = ((i + 1) < DLPAGER_LOG_ENTRIES_MAX) ? i + 1 : 0;
    } while (i != dlpager_log_idx);

}
