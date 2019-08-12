/*
===========================================================================

FILE:         rpm_suppressible.cpp

DESCRIPTION:


===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/rpm/server/rpm_suppressible.cpp#1 $
$Date: 2015/02/25 $

when       who     what, where, why
--------   ---     --------------------------------------------------------

===========================================================================
             Copyright (c) 2013 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
              QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */

#include "comdef.h"
#include "CoreVerify.h"
#include "rpm_suppressible.h"
#include "time_service.h"

#ifdef __cplusplus
extern "C" {
#endif
void busywait (uint32 pause_time_us);
#ifdef __cplusplus
} // extern "C"
#endif

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

#define ZQCAL_INTERVAL_MS     25        // in milliseconds
#define ZQCAL_INTERVAL_MAX_MS 32        // in milliseconds

static suppressible          *zqcal_task = NULL;
static suppressible_cb        zqcal_task_cb = NULL;
static volatile unsigned      zqcal_counter;
static volatile uint64_t      zqcal_time;
static volatile uint64_t      zqcal_interval_min;
// This interval may not be correct after XO SHUTDOWN/VDD MIN
static volatile uint64_t      zqcal_interval_max;

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

#if 0 // for test
static void zqcal_task_dummy_cb(void *not_used)
{
    uint64_t now      = time_service_now();
    uint64_t interval = now - zqcal_time;

    zqcal_counter++;
    if (zqcal_interval_max < interval)
    {
        zqcal_interval_max = interval;
    }
    if ((!zqcal_interval_min) || (zqcal_interval_min > interval))
    {
        zqcal_interval_min = interval;
    }
    //CORE_VERIFY(zqcal_interval_max < (ZQCAL_INTERVAL_MAX_MS * 19200));
    zqcal_time = now;

    busywait(500);
}
#endif

suppressible::suppressible(unsigned interval_ms, suppressible_cb cb, uint8_t priority) :
    Task(priority, 1),
    state_(IDLE),
    interval_(0),
    estimate_(9600),
    callback_(cb)
{
    if (interval_ms)
    {
        interval_ = (interval_ms * 19200);
    }
}

suppressible::~suppressible()
{
}

uint64_t suppressible::get_length() const
{
    return estimate_;
}

uint64_t suppressible::get_length_given(uint32_t systemStateIn, uint32_t &systemStateOut)
{
    systemStateOut = systemStateIn;
    return get_length();
}

void suppressible::reestimate()
{
}

bool suppressible::hasImmediateWork() const
{
    uint64_t deadline;

    if (get_deadline(deadline))
    {
        return ((time_service_now() + get_length()) > deadline);
    }

    return false;
}

void suppressible::execute_until(volatile bool &preempt, uint64_t stop_time)
{
    if (preempt)
    {
        return;
    }

    if (state_ == ACTIVE)
    {
#if 0
        if ((start + get_length()) > stop_time)
        {
            return;
        }
#endif

        uint64_t start = time_service_now();
        uint64_t end;

        if (callback_)
            callback_(NULL);

        // Cleanup
        end = time_service_now();
        set_end(end);
        clear_deadline();
        // Update estimate
        estimate_ = end - start;

        // Enqueue
        this->enqueue();
    }
    //state_ = IDLE;
}

void suppressible::enqueue(uint64_t deadline)
{
    if (interval_)
    {
        state_ = ACTIVE;
        set_deadline(interval_ + time_service_now());
        schedule_me();
    }
}

extern "C" void zqcal_task_register_cb(void *callback)
{
    zqcal_task_cb = (suppressible_cb)callback;
    if (zqcal_task)
    {
        zqcal_task->register_cb(zqcal_task_cb);
    }
}

extern "C" void zqcal_task_deregister_cb(void)
{
    zqcal_task_cb = NULL;
    if (zqcal_task)
    {
        zqcal_task->deregister_cb();
    }
}

extern "C" void zqcal_task_init(void)
{
    //zqcal_task_register_cb(zqcal_task_dummy_cb); // for test

    if (zqcal_task_cb)
    {
        zqcal_task = new suppressible(ZQCAL_INTERVAL_MS, zqcal_task_cb, 0);
        zqcal_time = time_service_now();
        zqcal_task->enqueue();
    }
}

#if 0
extern "C" void zqcal_task_enqueue(void)
{
   zqcal_task->enqueue();
}
#endif
