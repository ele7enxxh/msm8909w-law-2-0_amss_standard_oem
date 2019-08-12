#include "task_list.h"
#include "rpm_task.h"

extern "C"
{
  #include "err.h"
}

void TaskList::push_front(Task &t)
{
    // Link element to the current front; ensure prev pointer is cleared
    t.prev_task = 0;
    t.next_task = head_;

    if(head_)
    {
        // There was a head, so redirect it to point at the new guy
        head_->prev_task = &t;
    }
    else
    {
        // No previous head means this will be the tail as well
        tail_ = &t;
    }

    head_ = &t;
}

void TaskList::push_back(Task &t)
{
    // Link element to the current back; ensure next pointer is cleared
    t.prev_task = tail_;
    t.next_task = 0;

    if(tail_)
    {
        // There was a tail, so redirect it to point at the new guy
        tail_->next_task = &t;
    }
    else
    {
        // No previous tail means this will be the head as well
        head_ = &t;
    }

    tail_ = &t;
}

void TaskList::insert(Task &pos, Task &t)
{
    if(&pos == head_)
        push_front(t);
    else
    {
        // point the new guy at the others
        t.prev_task = pos.prev_task;
        t.next_task = &pos;

        // point the others at the new guy
        pos.prev_task->next_task = &t;
        pos.prev_task = &t;
    }
}

void TaskList::remove(Task &t)
{
  if(t.prev_task)
    t.prev_task->next_task = t.next_task;
  else
  {
    if(&t != head_)
      ERR_FATAL("Attempt to remove node that does not exist in the list.", 0, 0, 0);
    head_ = t.next_task;
  }

  if(t.next_task)
    t.next_task->prev_task = t.prev_task;
  else
  {
    if(&t != tail_)
      ERR_FATAL("Attempt to remove node that does not exist in the list.", 0, 0, 0);
    tail_ = t.prev_task;
  }

  t.prev_task = 0;
  t.next_task = 0;
}

void TaskList::pop_front()
{
  Task *old_task = head_;

    head_ = head_->next_task;
    if(head_)
        head_->prev_task = 0;
    else
        tail_ = 0;

  old_task->next_task = 0;
}

void TaskList::pop_back()
{
  Task *old_task = tail_;

    tail_ = tail_->prev_task;
    if(tail_)
        tail_->next_task = 0;
    else
        head_ = 0;

  old_task->prev_task = 0;
}

