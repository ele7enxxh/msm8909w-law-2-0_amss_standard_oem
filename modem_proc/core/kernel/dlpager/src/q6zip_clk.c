/*==========================================================================
 * FILE:         q6zip_clk.c
 *
 * DESCRIPTION:  Q6zip clock voting management
 *
 * Copyright (c) 2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved. QUALCOMM Proprietary and Confidential.
 ===========================================================================*/

/*==========================================================================

  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.mpss/3.5.c12.3/kernel/dlpager/src/q6zip_clk.c#1 $

  when       who     what, where, why
  --------   ---     --------------------------------------------------------
  10/28/14   ao      Initial revision
  ===========================================================================*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <qurt.h>
#include <npa.h>
#include <timer.h>

#include "dlpager_main.h"
#include "q6zip_clk.h"
#include "dlpager_log.h"

//#define Q6ZIP_CLK_ASYNC_VOTE

#define Q6ZIP_CLK_EXPIRED_TIMER_MASK       1
#define Q6ZIP_CLK_RESTART_BOOST_TIMER_MASK 2
#define Q6ZIP_CLK_FIRST_VOTE_MASK          4
#define Q6ZIP_CLK_MAX_VOTE_MASK            8
#define Q6ZIP_CLK_ALL_SIGNALS              (Q6ZIP_CLK_EXPIRED_TIMER_MASK | Q6ZIP_CLK_RESTART_BOOST_TIMER_MASK | Q6ZIP_CLK_FIRST_VOTE_MASK | Q6ZIP_CLK_MAX_VOTE_MASK)

static npa_client_handle q6zip_clk_npa_client;
static npa_client_handle q6zip_clk_npa_client_latency;
static npa_query_handle  q6zip_clk_npa_query;
static qurt_anysignal_t  q6zip_clk_signal;
static timer_type        q6zip_clk_boost_timer;
static qurt_thread_t     q6zip_clk_tid;
static qurt_mutex_t      q6zip_clk_vote_mutex;

#ifdef Q6ZIP_DCVS_PCYCLES_BACK2BACK
unsigned int q6zip_clk_pcycles_back2back      = Q6ZIP_DCVS_PCYCLES_BACK2BACK;
#else
unsigned int q6zip_clk_pcycles_back2back      = 120000;
#endif
#ifdef Q6ZIP_DCVS_BACK2BACK_CPU_BOOST
unsigned int q6zip_clk_back2back_cpu_boost    = Q6ZIP_DCVS_BACK2BACK_CPU_BOOST;
#else
unsigned int q6zip_clk_back2back_cpu_boost    = 10;
#endif
#ifdef Q6ZIP_DCVS_USECS_CANCEL
unsigned int q6zip_clk_usecs_cancel           = Q6ZIP_DCVS_USECS_CANCEL;
#else
unsigned int q6zip_clk_usecs_cancel           = 5000;
#endif
#ifdef Q6ZIP_DCVS_MIPS_FIRST_LEVEL_BOOST
unsigned int q6zip_clk_mips_first_level_boost = Q6ZIP_DCVS_MIPS_FIRST_LEVEL_BOOST*1000;
#else
unsigned int q6zip_clk_mips_first_level_boost = 576*1000;
#endif
#ifdef Q6ZIP_DCVS_MIPS_MAX_LEVEL_BOOST
unsigned int q6zip_clk_mips_max_level_boost   = Q6ZIP_DCVS_MIPS_MAX_LEVEL_BOOST*1000;
#else
unsigned int q6zip_clk_mips_max_level_boost   = 614*1000;
#endif

static void q6zip_clk_task(void* pArgs);
static void q6zip_clk_remove_vote(unsigned clk);
static void q6zip_clk_avail_cb(void *context, unsigned event_type,
                               void *data, unsigned data_size);
static void q6zip_clk_latency_avail_cb(void *context, unsigned event_type,
                                       void *data, unsigned data_size);
static void q6zip_clk_set_timer_signal(unsigned clk);
static void q6zip_clk_vote(unsigned value, unsigned clk);
static void q6zip_clk_thread_safe_vote(unsigned vote, unsigned  clk);
static void q6zip_clk_restart_boost_timer(unsigned clk);

void q6zip_clk_init(void)
{
    int status;
    void *pStack;
    qurt_thread_t q6zip_tid;
    qurt_thread_attr_t thread_attr;

    npa_resource_available_cb("/mcpm/nrat/q6freq", q6zip_clk_avail_cb, NULL);
    npa_resource_available_cb("/core/cpu/latency", q6zip_clk_latency_avail_cb, NULL);

    qurt_thread_attr_init (&thread_attr);
#define Q6ZIP_CLK_STACK_SIZE (4*1024)
    pStack = malloc (Q6ZIP_CLK_STACK_SIZE);
    if (pStack != NULL)
    {
        qurt_thread_attr_set_name (&thread_attr, "Q6zip_Clk");
        qurt_thread_attr_set_stack_addr (&thread_attr, pStack);
        qurt_thread_attr_set_stack_size (&thread_attr, Q6ZIP_CLK_STACK_SIZE);
        qurt_thread_attr_set_priority (&thread_attr, DLPAGER_MAIN_PRIO-1);
        status = qurt_thread_create (&q6zip_tid, &thread_attr,
                                 q6zip_clk_task, (void *)0);
        assert(status == 0);
        DL_DEBUG("Starting Q6zip Clk Thread , id=%d \n", q6zip_tid);

        qurt_mutex_init( &q6zip_clk_vote_mutex );
    }
    else
    {
       assert(0);
    }
}

void q6zip_clk_check_vote(void)
{
    static int back2back_count = 0;
    static unsigned long long prev_timer_kick_pcycles = 0;
    unsigned long long current_pcycles;
    npa_query_type npa_clk;

    if (0 == q6zip_clk_npa_client) return;

    current_pcycles = dlpager_log_last_pcycles;
    if ((current_pcycles - prev_timer_kick_pcycles) < q6zip_clk_pcycles_back2back )
    {
        back2back_count++;
    }
    else
    {
        back2back_count = 0;
    }
    npa_query(q6zip_clk_npa_query, NPA_QUERY_CURRENT_STATE, &npa_clk);

    if (npa_clk.data.value < q6zip_clk_mips_first_level_boost)
    {
        /* adds separation between boosts to space-out delays */
        back2back_count = 0;
        q6zip_clk_set_timer_signal(npa_clk.data.value);
        prev_timer_kick_pcycles = current_pcycles;
        q6zip_clk_thread_safe_vote(q6zip_clk_mips_first_level_boost, npa_clk.data.value);
    }
    else if((back2back_count > q6zip_clk_back2back_cpu_boost) &&
            (npa_clk.data.value < q6zip_clk_mips_max_level_boost))
    {
        q6zip_clk_set_timer_signal(npa_clk.data.value);
        prev_timer_kick_pcycles = current_pcycles;
        q6zip_clk_thread_safe_vote(q6zip_clk_mips_max_level_boost, npa_clk.data.value);
    }
    else
    {
        /* Optimization to avoid resources usage
           (timer restarting) and signals (generating task switching).
           If the timer was restarted recently then avoid doing it again.
           If more than 576000 pcycles (~1ms at 576MHz) then restart */
        if ((current_pcycles - prev_timer_kick_pcycles) > 576000)
        {
            q6zip_clk_set_timer_signal(npa_clk.data.value);
            prev_timer_kick_pcycles = current_pcycles;
        }
    }
}

unsigned q6zip_clk_query(void)
{
    npa_query_type npa_clk;
	
    npa_clk.data.value = 0;
	
    if ( q6zip_clk_npa_client != 0 )
    {
        npa_query( q6zip_clk_npa_query, NPA_QUERY_CURRENT_STATE, &npa_clk );
    }
    return npa_clk.data.value;
}

static void q6zip_clk_avail_cb(void *context,
                               unsigned event_type,
                               void *data,
                               unsigned data_size)
{
    (void)context;
    (void)event_type;
    (void)data;
    (void)data_size;
    q6zip_clk_npa_client = npa_create_sync_client("/mcpm/nrat/q6freq", "q6zip_via_mcpm", NPA_CLIENT_REQUIRED);
    q6zip_clk_npa_query = npa_create_query_handle("/mcpm/nrat/q6freq");
}

static void q6zip_clk_latency_avail_cb(void *context,
                                       unsigned event_type,
                                       void *data,
                                       unsigned data_size)
{
    (void)context;
    (void)event_type;
    (void)data;
    (void)data_size;
    q6zip_clk_npa_client_latency = npa_create_sync_client(
        "/core/cpu/latency", "q6zip_latency", NPA_CLIENT_REQUIRED);
}

static void q6zip_clk_thread_safe_vote(unsigned vote, unsigned clk)
{
#ifdef Q6ZIP_CLK_ASYNC_VOTE
    int is_max = vote == q6zip_clk_mips_max_level_boost;
    unsigned idx = dlpager_log_start(is_max ? DLP_MAX_CLK_REQ : DLP_FIRST_CLK_REQ, 0, 0, 0);
    qurt_anysignal_set(&q6zip_clk_signal, is_max ? Q6ZIP_CLK_MAX_VOTE_MASK : Q6ZIP_CLK_FIRST_VOTE_MASK);
    dlpager_log_end(is_max ? DLP_MAX_CLK_REQ : DLP_FIRST_CLK_REQ, idx, clk, vote);
#else
    qurt_mutex_lock(&q6zip_clk_vote_mutex);
    if (q6zip_clk_npa_client_latency != 0)
        npa_issue_required_request(q6zip_clk_npa_client_latency, 1); /* prevent PC */
    q6zip_clk_vote(vote, clk);
    qurt_mutex_unlock(&q6zip_clk_vote_mutex);
#endif
}

static void q6zip_clk_set_timer_signal(unsigned clk)
{
    unsigned idx = dlpager_log_start(DLP_TIMER_REQ, 0, 0, 0);
    qurt_anysignal_set(&q6zip_clk_signal, Q6ZIP_CLK_RESTART_BOOST_TIMER_MASK);
    dlpager_log_end(DLP_TIMER_REQ, idx, 0, clk);
}

static void q6zip_clk_restart_boost_timer(unsigned clk)
{
    unsigned idx = dlpager_log_start(DLP_RESTART_TIMER, 0, 0, 0);
    timer_stop(&q6zip_clk_boost_timer, T_USEC, NULL);
    timer_set_64(&q6zip_clk_boost_timer, q6zip_clk_usecs_cancel, 0, T_USEC);
    dlpager_log_end(DLP_RESTART_TIMER, idx, 0, clk);
}

static void q6zip_clk_vote(unsigned value, unsigned clk)
{
    dlpager_log_event_t event = (value == q6zip_clk_mips_max_level_boost) ?
        DLP_MAX_CLK_VOTE : DLP_FIRST_CLK_VOTE;
    unsigned idx = dlpager_log_start(event, 0, 0, 0);
    npa_set_request_attribute(q6zip_clk_npa_client,
                              NPA_REQUEST_FIRE_AND_FORGET | NPA_REQUEST_BEST_EFFORT);
    npa_issue_required_request(q6zip_clk_npa_client, value);
    dlpager_log_end(event, idx, clk, value);
}

static void q6zip_clk_remove_vote(unsigned clk)
{
    unsigned idx = dlpager_log_start(DLP_REMOVE_CLK_VOTE, 0, 0, 0);
    npa_set_request_attribute(q6zip_clk_npa_client,
                              NPA_REQUEST_FIRE_AND_FORGET | NPA_REQUEST_BEST_EFFORT);
    npa_issue_required_request(q6zip_clk_npa_client, 0);
    dlpager_log_end(DLP_REMOVE_CLK_VOTE, idx, clk, 0);
}

static void q6zip_clk_task(void* pArgs)
{
    unsigned signal;
    npa_query_type npa_clk;

    (void)pArgs;

    qurt_anysignal_init(&q6zip_clk_signal);

    timer_def_osal(&q6zip_clk_boost_timer, NULL, TIMER_NATIVE_OS_SIGNAL_TYPE,
                   (time_osal_notify_obj_ptr)&q6zip_clk_signal,
                   Q6ZIP_CLK_EXPIRED_TIMER_MASK);

    q6zip_clk_tid = qurt_thread_get_id();

    while (1)
    {
        qurt_anysignal_wait(&q6zip_clk_signal, Q6ZIP_CLK_ALL_SIGNALS);
        signal = qurt_anysignal_clear(&q6zip_clk_signal, Q6ZIP_CLK_ALL_SIGNALS);

        npa_query(q6zip_clk_npa_query, NPA_QUERY_CURRENT_STATE, &npa_clk);

        /* restart boost timer must happen before voting */
        if (signal & Q6ZIP_CLK_RESTART_BOOST_TIMER_MASK)
        {
            q6zip_clk_restart_boost_timer(npa_clk.data.value);
        }

#ifdef Q6ZIP_CLK_ASYNC_VOTE
        if (signal & (Q6ZIP_CLK_FIRST_VOTE_MASK | Q6ZIP_CLK_MAX_VOTE_MASK))
        {
            if (q6zip_clk_npa_client_latency != 0)
                npa_issue_required_request(q6zip_clk_npa_client_latency, 1); /* prevent PC */

            q6zip_clk_vote((signal & Q6ZIP_CLK_MAX_VOTE_MASK) ?
                           q6zip_clk_mips_max_level_boost :
                           q6zip_clk_mips_first_level_boost,
                           npa_clk.data.value);
            if (signal & Q6ZIP_CLK_EXPIRED_TIMER_MASK)
            {
                /* don't want to vote and remove vote
                   back to back, skipping vote removal and re-start the timer
                   to not loose the vote removal */
                q6zip_clk_restart_boost_timer(npa_clk.data.value);
                signal = signal & ~Q6ZIP_CLK_EXPIRED_TIMER_MASK;
            }
        }
#endif

        if (signal & Q6ZIP_CLK_EXPIRED_TIMER_MASK)
        {
#ifndef Q6ZIP_CLK_ASYNC_VOTE
            qurt_mutex_lock(&q6zip_clk_vote_mutex);
#endif
            q6zip_clk_remove_vote(npa_clk.data.value);
#ifndef Q6ZIP_CLK_ASYNC_VOTE
            qurt_mutex_unlock(&q6zip_clk_vote_mutex);
#endif

            if (NPA_QUERY_SUCCESS == npa_query_by_client(
                    q6zip_clk_npa_client,
                    NPA_QUERY_CLIENT_ACTIVE_REQUEST,
                    &npa_clk) &&
                NPA_QUERY_TYPE_STATE == npa_clk.type &&
                0 != npa_clk.data.state)
            {
                /* re-start the timer to try to remove the vote again */
                q6zip_clk_restart_boost_timer(npa_clk.data.value);
            }
            else
            {
                if (q6zip_clk_npa_client_latency != 0)
                    npa_complete_request(q6zip_clk_npa_client_latency); /* allow PC */
            }
        }
    }
}
