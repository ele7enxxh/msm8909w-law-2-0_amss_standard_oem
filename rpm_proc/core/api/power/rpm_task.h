#ifndef RPM_TASK_H
#define RPM_TASK_H

#include <stdint.h>

enum ScheduleType { DEFAULT = 0, IGNORE_DEADLINE = 1 };

class Task
{
public:
    // Smaller number == higher priority
    Task(uint8_t priority = 127, uint8_t suppressible = 0);
    virtual ~Task() {};

    inline uint8_t get_priority() const;

    // Set the execution deadline--that is, the time that execute() should
    // complete by.  If this task has not completed by 'deadline', it is
    // officialy late.
    inline void set_deadline(uint64_t deadline);

    // Get the currently set deadline, or return false if there is none.
    inline bool get_deadline(uint64_t& deadline) const;

    // Overridable function for returning an estimate of how long this task
    // will take to execute.
    virtual uint64_t get_length() const = 0;

    // Return an estimate of how long this task will take if performed upon
    // the system state "systemStateIn."  If the task does not make use of
    // systemStateIn or doesn't have any specific knowledge, this should
    // return the same as get_length() and set systemStateOut = 0.
    //
    // If the task does have specific knowledge, it should return the expected
    // length and set systemStateOut to the expected system state following
    // the completion of this task.
    virtual uint64_t get_length_given(uint32_t systemStateIn, uint32_t &systemStateOut);

    // Overridable function for updating estimates, as in the case of CPU speed
    // change.
    virtual void reestimate() {};

    // Get the time this task is slated to start by, in order to finish on time
    // as per the current schedule's requirements.
    inline uint64_t get_start() const;
    inline uint64_t get_required_start() const;

    // Set the time this task needs to start by in order to finish on time as
    // per the current schedule's requirements.
    inline void set_start(uint64_t start_time);
    inline void set_end(uint64_t end_time);

    // Run this task, completing as much work as it can until the preempt flag becomes nonzero or the task completes.
    void execute(volatile bool &preempt, uint64_t stop_time);

    virtual bool hasImmediateWork() const = 0;
    inline bool hasScheduledWork() const;

protected:
    // Overridable function for performing the actual work requested when
    // execute() is called.  execute() acts as a thin wrapper for this actual
    // implmenetation of the function, where the wrapper checks for stop_time
    // violations and performs logging, etc.
    virtual void execute_until(volatile bool &preempt, uint64_t stop_time) = 0;

    void clear_deadline();

    void schedule_me(ScheduleType schedule_type = DEFAULT);
private:
    const uint8_t priority_;

    bool          haveDeadline_;
    uint64_t      deadline_;
    uint64_t      start_;
    uint64_t      end_;

    // A handful of debugging statistics / things that can be watchpointed.
    uint8_t       lateStarts_;
    uint8_t       lateStops_;
    uint8_t       violatedDeadlines_;
    uint8_t       suppressible_;

    // intrusive data structure hooks
    // FIXME: in the future, make these hooks work in a less hacky, more C++-like fashion
    Task *next_task;
    Task *prev_task;
    bool inImmediateQ_;
    bool inScheduledQ_;
    friend class Sched;
    friend class TaskList;
};

/////////////////////////////////////
//Implementation of inline funcitons.
/////////////////////////////////////

inline uint8_t Task::get_priority() const
{
    return priority_;
}

inline void Task::set_deadline(uint64_t deadline)
{
    haveDeadline_ = true;
    deadline_ = deadline;
}

inline bool Task::get_deadline(uint64_t &deadline) const
{
    if(haveDeadline_)
        deadline = deadline_;
    return haveDeadline_;
}

inline uint64_t Task::get_start() const
{
    return start_;
}

inline uint64_t Task::get_required_start() const
{
    if(!suppressible_)
    {
        return start_;
    }
    return 0;
}

inline void Task::set_start(uint64_t start)
{
    start_ = start;
}

inline bool Task::hasScheduledWork() const
{
    return haveDeadline_;
}

inline void Task::set_end (uint64_t end)
{
    end_ = end;
}

#endif // RPM_TASK_H

