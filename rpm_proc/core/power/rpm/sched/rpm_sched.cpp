/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#define __STDC_LIMIT_MACROS
#include <limits.h>

#include "rpm_sched.h"
#include "rpm_task.h"
#include "time_service.h"
#include "dog.h"
#include "../server/rpm_estimator.h"
#include "../server/rpm_hash.h"
#include "svs.h"

#include "swevent.h"


#define START_TIME_HAIRY_THRESHOLD (2000)

Sched::Sched(unsigned hint_num_immed_tasks, unsigned hint_num_deadline_tasks)
    : cpuFreq_(0),
      immediateQ_(hint_num_immed_tasks),
      currentTask_ (NULL),
      currentImmediateTask_ (NULL)
{
}

Sched::~Sched()
{
}

void Sched::schedule_task(Task &new_task, ScheduleType schedule_type)
{
    int sav;
    INTLOCK_SAV(sav);

    uint64_t deadline;

    // Get the deadline.  If there is none (or if we're supposed to ignore it),
    // simply add this task to the immediate queue and be done.
    if(IGNORE_DEADLINE == schedule_type || !new_task.get_deadline(deadline))
    {
        immediateQ_.insert(&new_task);
        new_task.inImmediateQ_ = true;

        if ((currentTask_ != &new_task) && (currentTask_ != NULL) && (currentImmediateTask_ != NULL))
        {
            // If this new task is at the top of the heap, we may need to preempt
            // whatever was running. First, it has to be the highest priority task amongst all tasks
            // currently in the queue. Secondly, it has to be higher priority than the current task.
            // Note: higher priority means lower number.
            Task *min;
            immediateQ_.findMin(&min);
            if((min == &new_task) && (min->get_priority () < currentImmediateTask_->get_priority ()))
            {
                preempt_ = true;
            }
          }

          INTFREE_SAV(sav);
          return;
      }

      // Skip down the list until we find a task with a deadline later than ours.
      for(Task *list_pos = scheduledQ_.head(); list_pos; list_pos = list_pos->next_task)
      {
          uint64_t list_deadline;
          list_pos->get_deadline(list_deadline);

          if((list_deadline > deadline) ||
             ((list_deadline == deadline) && (new_task.get_length() > list_pos->get_length())))
          {
              // Insert just before the task that is supposed to finish after us.
              scheduledQ_.insert(*list_pos, new_task);
              new_task.inScheduledQ_ = true;

              // Make sure the schedule start times are all self consistent with
              // respect to overlap and expected execution duration / state
              // prediction.
              fixup_schedule(*scheduledQ_.head(), *scheduledQ_.tail(), rpm_system_hash);

              // If this task is now the first on the schedule we may need to
              // preempt something else to allow it to run.
              if(&new_task == scheduledQ_.head())
                  preempt_ = true;

            INTFREE_SAV(sav);
            return;
        }
    }

    // We reached the end of our task list and didn't find a task with a deadline later than this one.
    // So, simply insert at the end of the list.
    scheduledQ_.push_back(new_task);
    new_task.inScheduledQ_ = true;
    fixup_schedule(*scheduledQ_.head(), *scheduledQ_.tail(), rpm_system_hash);

    // If this task is now the first on the schedule we may need to
    // preempt something else to allow it to run.
    if(&new_task == scheduledQ_.head())
        preempt_ = true;

    INTFREE_SAV(sav);
}

void Sched::pop_next_task(uint64_t &stopping_time)
{
    uint64_t next_scheduled_start;

    INTLOCK();

    do
    {
        // Clear the preemption flag.  We're about to look up the highest
        // priority task right now, so it's only if this information changes in
        // the future that we'll need to preempt whatever we selected.
        preempt_ = false;

        // try to find a scheduled task to run first
        currentTask_ = scheduledQ_.head();
        uint64_t now = time_service_now();
        if(currentTask_)
        {
            // Check for speed changes, update schedule if it changed
            if(svs_update_speed(0))
                stopping_time = fixup_schedule(*scheduledQ_.head(), *scheduledQ_.tail(), rpm_system_hash);

            next_scheduled_start = currentTask_->get_start();
            // Add a bit of a fudge factor here. With all timestamps being at XO speed, we're getting too fine-grained.
            if((now + START_TIME_HAIRY_THRESHOLD) >= next_scheduled_start)
            {
                stopping_time = UINT64_MAX;
                scheduledQ_.pop_front();
                currentTask_->inScheduledQ_ = false;
                break; // found task to run
            }
        }
        else
            next_scheduled_start = UINT64_MAX;

        // no scheduled task is ready, so try to run an immediate one
        currentTask_ = NULL;
        currentImmediateTask_ = NULL;

        if(immediateQ_.findMin(&currentTask_))
        {
            // Check for speed changes
            svs_update_speed(0);

            if(UINT64_MAX == next_scheduled_start)
            {
                stopping_time = next_scheduled_start;
            }
            else
            {
                stopping_time = fixup_schedule(*scheduledQ_.head(), *scheduledQ_.tail(), rpm_system_hash);
                if (now >= stopping_time)
                {
                    // Ended up in the past - no way we can run the current immediate task
                    // We know there's a scheduled task waiting to be run, so start the scheduled work
                    // now (ahead of schedule) in the hope that we'll get to the immediate work sooner
                    currentTask_ = scheduledQ_.head ();
                    stopping_time = UINT64_MAX;
                    scheduledQ_.pop_front ();
                    currentTask_->inScheduledQ_ = false;
                    break;
                }
            }

            // Update our notion of the immediate task
            currentImmediateTask_ = currentTask_;
            immediateQ_.deleteMin();
            currentTask_->inImmediateQ_ = false;
        }
    }
    while(false);

    INTFREE();
}

void Sched::run()
{
    while(true)
    {
        dog_kick();

        uint64_t stopping_time;
        bool schedule_changed = false;
        pop_next_task(stopping_time);

        if(currentTask_)
        {
            uint32_t nextState, preState = rpm_system_hash;

            currentTask_->get_length_given(preState, nextState);

            currentTask_->execute(preempt_, stopping_time);

            INTLOCK();

            // update the status of this task.  it could have gained / lost
            // immediate or scheduled status while running.

            // 1) remove from immediate queue if no longer immediate
            if(currentTask_->inImmediateQ_ && !currentTask_->hasImmediateWork())
            {
                immediateQ_.deleteTask(currentTask_);
                currentTask_->inImmediateQ_ = false;
            }
            // 2) add to immediate queue if now/still immediate
            else if(!currentTask_->inImmediateQ_ && currentTask_->hasImmediateWork())
                schedule_task(*currentTask_, IGNORE_DEADLINE);
            // 3) remove from schedule if no longer scheduled
            if(currentTask_->inScheduledQ_ && !currentTask_->hasScheduledWork())
            {
                schedule_changed = true;
                scheduledQ_.remove(*currentTask_);
                currentTask_->inScheduledQ_ = false;
                if(!scheduledQ_.empty())
                {
                    fixup_schedule(*scheduledQ_.head(), *scheduledQ_.tail(), rpm_system_hash);
                }
            }
            // 4) add to schedule if now scheduled
            else if(!currentTask_->inScheduledQ_ && currentTask_->hasScheduledWork())
            {
                schedule_changed = true;
                schedule_task(*currentTask_);
            }

            // Make sure our schedule is in sync.
            if(!nextState || (rpm_system_hash != nextState) || schedule_changed)
            {
                schedule_changed = false;
                if(!scheduledQ_.empty())
                    fixup_schedule(*scheduledQ_.head(), *scheduledQ_.tail(), rpm_system_hash);
                if(nextState)
                    SWEVENT(RPM_HASH_MISMATCH, currentTask_, preState, nextState, rpm_system_hash);
            }

            // Reset the current task
            currentTask_ = NULL;
            currentImmediateTask_ = NULL;

            INTFREE();

            continue;
        }

        break; // nothing to do at the moment
    }
}

void Sched::preempt()
{
    preempt_ = true;
}


// Get the total work load at this point of time
uint64_t Sched::get_next_workload() const
{
  
  uint64_t total_workload = 0;
  uint64_t earliest_sleep = 0;
  uint32_t system_out = rpm_system_hash;
  uint32_t temp_system_state = system_out;
  
  if(!immediateQ_.empty())
    return 0;
		
  Task *t = (Task*)scheduledQ_.head();

  //Add the first task to the duration since thats the one we are waking up  
  total_workload = t->get_length_given(temp_system_state, system_out);
  
  //Earliest sleep would be deadline of first task
  t->get_deadline(earliest_sleep);
  
  t = t->next_task;
  
  for(; t; t = t->next_task)
  {
    temp_system_state = system_out;
    // Use get_length_given to check for set transition cache hits.
    // Update the new system state for subsequent tasks.
	if((t->get_start()) <= (earliest_sleep + START_TIME_HAIRY_THRESHOLD))
	{
        total_workload += t->get_length_given(temp_system_state, system_out);
		// move to next earliest sleep which is deadline of this task
		t->get_deadline(earliest_sleep);
	}
  }
  
  return total_workload;
}



uint64_t Sched::get_next_start() const
{
    // If there's immediate work to be done, we should be running it already.
    if(!immediateQ_.empty())
        return 0;

    // Otherwise, the next start is the next scheduled start.
    const Task *next_task = scheduledQ_.head();
    if(next_task)
        return next_task->get_start();
    else
        return UINT64_MAX;
}

uint64_t Sched::get_next_required_start() const
{
    uint64_t task_start = 0;
    // If there's immediate work to be done, we should be running it already.
    if(!immediateQ_.empty())
        return 0;

    // Otherwise, the next start is the next scheduled start.
    const Task *sched_task = scheduledQ_.head();
    while(sched_task)
    {
       task_start = sched_task->get_required_start();
       if(!task_start)
       {
         sched_task = sched_task->next_task;
       }
       else
       {
         return task_start;
       }
    }

    return UINT64_MAX;
}

void Sched::cpu_speed_for_fast(unsigned int cpu_freq_khz)
{
    if(cpu_freq_khz == cpuFreq_)
      return;

    INTLOCK();

    // re-estimate work load for svs_fast mode
    cpu_speed_change(cpu_freq_khz, 1);

    INTFREE();

}


// FIXME: can we make this smarter so we don't have to "break it" and then
// "correct it" just for a hypothetical?  does it take that much time?
uint64_t Sched::get_next_duration(unsigned int cpu_freq_khz)
{
  uint64_t duration;

  unsigned int currentFreq = cpuFreq_;
  bool         restoreFreq = (cpu_freq_khz != cpuFreq_);

  INTLOCK();
  if(restoreFreq)
  {
    cpu_speed_change(cpu_freq_khz, 0);
  }
  else
  {
    //if we aren't changing speeds, check if any of the immediate set transitions have cache hits.
    //  this is already done in cpu_speed_change
    immediateQ_.recalculateWork();
  }

  uint64_t now = time_service_now();

  // Add up all the work in the immediate queue.  We'll have to do this work
  // before going to sleep no matter what.
  uint64_t immediate_work = immediateQ_.totalWork();

  // Figure out the earliest time we could possibly go to sleep.
  uint64_t earliest_sleep = now + immediate_work;

  // Crawl the scheduled queue.
  uint32_t system_out = rpm_system_hash;
  for(Task *t = scheduledQ_.head(); t; t = t->next_task)
  {
    uint32_t temp_system_state = system_out;
    if((t->get_start()) <= (earliest_sleep + START_TIME_HAIRY_THRESHOLD))
    {
      // Use get_length_given to check for set transition cache hits.
      // Update the new system state for subsequent tasks.
      earliest_sleep += t->get_length_given(temp_system_state, system_out);
    }
    else
    {
      // The task starts after 'earliest_sleep', so we'll actually go to sleep before this one.
      break;
    }
  }

  // 'earliest_sleep' is now the projected endpoint of our activity
  duration = earliest_sleep - now;

  if(restoreFreq)
  {
    cpu_speed_change(currentFreq, 0);
  }
  INTFREE();

  return duration;
}

// FIXME: can we make this smarter so we don't have to "break it" and then
// "correct it" just for a hypothetical?  does it take that much time?
bool Sched::cpu_speed_change_ok(unsigned int cpu_freq_khz)
{
    bool would_be_late;

    if(cpu_freq_khz == cpuFreq_)
      return true;

    INTLOCK();

    unsigned int currentFreq = cpuFreq_;

    // Update the schedule for the slower speed.
    cpu_speed_change(cpu_freq_khz, 0);

    // Check to make sure this didn't make us late.
    would_be_late = (get_next_start() < time_service_now());

    // Undo the work we just did.
    cpu_speed_change(currentFreq, 0);

    INTFREE();
    return !would_be_late;
}

void Sched::cpu_speed_change(unsigned int cpu_freq_khz, unsigned int passive)
{
    if(cpu_freq_khz == cpuFreq_)
      return;

    INTLOCK();

    theEstimator().updateClockRate(cpu_freq_khz);

    // Update the system hash with the new cpu clock speed.  Technically we
    // pass a resource data pointer off the end of the array so that the code
    // uses a unique rotation for it.
    rpm_system_hash = rpm_resource_hash(rpm_system_hash, &rpm->resources[rpm->supported_resources], sizeof(cpu_freq_khz), &cpuFreq_, &cpu_freq_khz);

    // This calls re-estimate on its work for us.
    immediateQ_.recalculateWork();

    // We need to do this for the scheduled queue ourselves because we fail at
    // making good abstractions.
    for(Task *t = scheduledQ_.head(); t; t = t->next_task)
        t->reestimate();

    // Now we need to correct the schedule, since stuff might have moved
    // around.
    if(scheduledQ_.tail())
        fixup_schedule(*scheduledQ_.head(), *scheduledQ_.tail(), rpm_system_hash);

    if (!passive)
        cpuFreq_ = cpu_freq_khz;

    INTFREE();
}

// Fix start times for tasks between start_task and last_task (inclusive).
// Return value is the start time of the start_task (used in recursion).
// systemState is updated by each level of recursion to the output state after that task completes.
uint64_t Sched::fixup_schedule(Task &start_task, Task &last_task, uint32_t systemState)
{
    uint32_t nextState;

    // Figure out how long start_task will take given systemState, and how it affects system state.
    uint64_t length = start_task.get_length_given(systemState, nextState);

    // Figure out how early start_task can be run.
    uint64_t lastStart;
    if((&start_task == &last_task) || !start_task.next_task)
        lastStart = UINT64_MAX;
    else
        lastStart = fixup_schedule(*start_task.next_task, last_task, nextState);

    // Finally, figure out when we need to be done by.
    uint64_t deadline;
    start_task.get_deadline(deadline);

    // We have all the information we need to determine start time.
    uint64_t start_time;
    if(lastStart > deadline)
    {
        if(length > deadline)
        {
            start_time = 0;
        }
        else
        {
            start_time = deadline - length;
        }
    }
    else
    {
        start_time = lastStart - length;
    }

    start_task.set_start(start_time);
    return start_time;
}

Sched &theSchedule()
{
  static Sched s;
  return s;
}

extern "C" void sched_run()
{
  theSchedule().run();
}

extern "C" uint64_t sched_get_next_start()
{
  return theSchedule().get_next_start();
}

extern "C" uint64_t sched_get_next_required_start()
{
  return theSchedule().get_next_required_start();
}

extern "C" uint64_t sched_get_next_duration(unsigned int cpu_freq_khz)
{
  return theSchedule().get_next_duration(cpu_freq_khz);
}

extern "C" bool sched_cpu_speed_change_ok(unsigned int cpu_freq_khz)
{
  return theSchedule().cpu_speed_change_ok(cpu_freq_khz);
}

extern "C" void sched_cpu_speed_change(unsigned int cpu_freq_khz)
{
  theSchedule().cpu_speed_change(cpu_freq_khz, 0);
}

extern "C" void sched_cpu_speed_for_fast(unsigned int cpu_freq_khz)
{
  theSchedule().cpu_speed_for_fast(cpu_freq_khz);
}

extern "C" uint64_t sched_get_next_workload()
{
  return theSchedule().get_next_workload();
}





