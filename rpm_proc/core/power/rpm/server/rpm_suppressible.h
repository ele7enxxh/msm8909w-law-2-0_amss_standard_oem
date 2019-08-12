#ifndef RPM_SUPPRESSIBLE_H
#define RPM_SUPPRESSIBLE_H

/*
===========================================================================

FILE:         rpm_suppressible.h

DESCRIPTION:


===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/power/rpm/server/rpm_suppressible.h#1 $
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

#include <stdint.h>

#include "rpm_task.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

typedef void (*suppressible_cb)(void *);

class suppressible : public Task
{
public:
    virtual uint64_t get_length() const;
    virtual uint64_t get_length_given(uint32_t systemStateIn, uint32_t &systemStateOut);
    virtual void reestimate();
    virtual bool hasImmediateWork() const;

    inline void register_cb(suppressible_cb);
    inline void deregister_cb();

    void enqueue(uint64_t deadline = 0);
    enum state_t
    {
        IDLE,
        ACTIVE,
    };

    suppressible(unsigned timeout_ms, suppressible_cb cb, uint8_t priority);

private:
    virtual ~suppressible ();

    virtual void execute_until(volatile bool &preempt, uint64_t stop_time);

    state_t               state_;
    uint64_t              interval_;
    uint64_t              estimate_;
    suppressible_cb       callback_;
};

inline void suppressible::register_cb(suppressible_cb callback)
{
    callback_ = callback;
}

inline void suppressible::deregister_cb()
{
    callback_ = NULL;
}

#endif /* RPM_SUPPRESSIBLE_H */

