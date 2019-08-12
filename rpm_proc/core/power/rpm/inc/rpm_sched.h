#ifndef RPM_SCHED_H
#define RPM_SCHED_H

#ifdef __cplusplus

#include <stdint.h>
#include "rpm_task.h"
#include "task_heap.h"
#include "task_list.h"

class Sched
{
public:
    // Retrieve the single schedule instance thusly.
    friend Sched &theSchedule();

    // Adds a new task to the current schedule.  The task won't actually be
    // executed until the scheduler is given an opportunity to run().
    // 'schedule_type' controls whether a task with a deadline is put in the
    // scheduled queue, or the immediate queue.  by default it always places
    // tasks with a deadline in the scheduled queue, but even a task with a
    // deadline can be forced to run immediately using IGNORE_DEADLINE--this is
    // sometimes handy as in the case of "rude wakeups" when a planned wakeup
    // was already in place.
    void schedule_task(Task &new_task, ScheduleType schedule_type = DEFAULT);

    // Runs the next outstanding task, returning only when there is no
    // immediate work to be done.
    void run();

    // Notify the scheduler that something outside its purview (presumably
    // affecting the currently running task) has changed and that it should
    // stop and restart.
    void preempt();

    uint64_t get_next_start() const;
    uint64_t get_next_required_start() const;
    uint64_t get_next_workload() const;


    // Get next wakeup duration.  Can be computed given a specific frequency,
    // or assuming current frequency (pass 0).
    uint64_t get_next_duration(unsigned int cpu_freq_khz = 0);

    bool cpu_speed_change_ok(unsigned int cpu_freq_khz);
    void cpu_speed_change(unsigned int cpu_freq_khz, unsigned int passive);
    void cpu_speed_for_fast(unsigned int cpu_freq_khz);
	
private:
    Sched(unsigned hint_num_immed_tasks = 10, unsigned hint_num_deadline_tasks = 10);
    Sched(Sched &rhs);
    ~Sched();

    void pop_next_task(uint64_t &stopping_time);
    uint64_t fixup_schedule(Task &start_task, Task &last_task, uint32_t systemState);

    unsigned int cpuFreq_;

    TaskHeap immediateQ_;
    TaskList scheduledQ_;
    volatile bool preempt_;

    // The task currently executing
    Task *currentTask_;

    // The immediate task currently executing. Only set if currentTask_ came from the immediate queue.
    Task *currentImmediateTask_;
};

Sched &theSchedule();

extern "C" void     sched_run();
extern "C" uint64_t sched_get_next_start();
extern "C" uint64_t sched_get_next_workload();
extern "C" uint64_t sched_get_next_required_start();
extern "C" uint64_t sched_get_next_duration(unsigned int cpu_freq_khz);
extern "C" bool     sched_cpu_speed_change_ok(unsigned int cpu_freq_khz);
extern "C" void     sched_cpu_speed_change(unsigned int cpu_freq_khz);
extern "C" void     sched_cpu_speed_for_fast(unsigned int cpu_freq_khz);

#else // !__cplusplus

#include <stdint.h>
extern void     sched_run(void);
extern uint64_t sched_get_next_start(void);
extern uint64_t sched_get_next_workload(void);
extern uint64_t sched_get_next_required_start(void);
extern uint64_t sched_get_next_duration(unsigned int cpu_freq_khz);
extern uint32_t sched_cpu_speed_change_ok(unsigned int cpu_freq_khz);
extern void     sched_cpu_speed_change(unsigned int cpu_freq_khz);
extern void     sched_cpu_speed_for_fast(unsigned int cpu_freq_khz);

#endif // (!)__cplusplus

#endif // RPM_SCHED_H

