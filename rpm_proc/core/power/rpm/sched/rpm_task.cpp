#include <assert.h>
#include "rpm_task.h"
#include "time_service.h"
#include "rpm_sched.h"

Task::Task(uint8_t flexibility, uint8_t suppressible)
    : priority_(flexibility), haveDeadline_(false), deadline_(0), start_(0), end_ (0),
      lateStarts_(0), lateStops_(0), violatedDeadlines_(0), suppressible_(suppressible),
      next_task(0), prev_task(0), inImmediateQ_(false), inScheduledQ_(false)
{
}

uint64_t Task::get_length_given(uint32_t systemStateIn, uint32_t &systemStateOut)
{
    // By default, tasks don't know how to make use of the systemStateIn.
    systemStateOut = 0;  // unknown effect on system state
    return get_length(); // fall back to worst case estimate
}

void Task::clear_deadline()
{
    if(haveDeadline_ && time_service_now() > deadline_)
        ++violatedDeadlines_;

    haveDeadline_ = false;
}

void Task::execute(volatile bool &preempt, uint64_t stop_time)
{
    if(haveDeadline_ && time_service_now() > get_start())
        ++lateStarts_;

    execute_until(preempt, stop_time);

    if(time_service_now() > stop_time)
        ++lateStops_;
}

void Task::schedule_me(ScheduleType schedule_type)
{
    theSchedule().schedule_task (*this, schedule_type);
}
