#include <algorithm>

#include "task_heap.h"
#include "rpm_task.h"
#include "rpm_hash.h"

//
// Utility functions for implementing a simple binary heap in an array
//
static inline unsigned heap_parent(unsigned idx)
{
    return (idx - 1) / 2;
};

static inline unsigned heap_lchild(unsigned idx)
{
    return 2 * idx + 1;
};

static inline unsigned heap_rchild(unsigned idx)
{
    return 2 * idx + 2;
};

//
// The actual APIs
//
TaskHeap::TaskHeap(unsigned size_hint)
    : totalWork_(0),
      insertionCount_(0),
      heapReserved_(size_hint),
      heapSize_(0),
      heap_(new HeapItem[size_hint])
{
}

void TaskHeap::expandHeap()
{
    HeapItem *new_heap = new HeapItem[2*heapReserved_];

    for(unsigned i = 0; i < heapReserved_; ++i)
        new_heap[i] = heap_[i];

    delete [] heap_;
    heap_ = new_heap;
    heapReserved_ *= 2;
}

void TaskHeap::insert(Task *new_task)
{
    if(++heapSize_ > heapReserved_)
        expandHeap();

    unsigned idx            = heapSize_ - 1;
    heap_[idx].insertID     = insertionCount_++;
    heap_[idx].workEstimate = new_task->get_length();
    heap_[idx].task         = new_task;

    // add the work this task represents to our accumulator
    totalWork_ += heap_[idx].workEstimate;

    // now, bubble the item up until the heap property is satisfied
    while((idx != 0) && less(heap_[idx], heap_[heap_parent(idx)]))
    {
        std::swap(heap_[idx], heap_[heap_parent(idx)]);
        idx = heap_parent(idx);
    }
}

bool TaskHeap::findMin(Task **top)
{
    if(!empty())
    {
        *top = heap_[0].task;
        return true;
    }

    return false;
}

void TaskHeap::deleteIdx(unsigned idx)
{
    // remove the work that we accounted for this task
    totalWork_ -= heap_[idx].workEstimate;

    // replace object[idx] with bottom-most element
    heap_[idx] = heap_[--heapSize_];

    // now, bubble the item back down until the heap property is satisfied
    while(true)
    {
        unsigned lchild   = heap_lchild(idx);
        unsigned rchild   = heap_rchild(idx);
        unsigned swap_idx = idx;

        // check to see if we should swap with the left child
        if((lchild < heapSize_) && less(heap_[lchild], heap_[idx]))
        {
            swap_idx = lchild;
        }

        // check to see if we should swap with the right child; this has the
        // additional complication that we should only swap with this if it's
        // also smaller than the left child as well
        if(((rchild < heapSize_) && less(heap_[rchild], heap_[idx])) && // we should swap with rchild
           (less(heap_[rchild], heap_[swap_idx])))                      // and the currently planned swap is worse than this one
        {
            swap_idx = rchild;
        }

        if(swap_idx == idx)
            break; // no swap was required--we have restored a valid heap

        std::swap(heap_[idx], heap_[swap_idx]);
        idx = swap_idx;
    }
}

void TaskHeap::deleteMin()
{
    if(empty())
        return;

    deleteIdx(0);
}

void TaskHeap::deleteTask(Task *t)
{
  // Look for the task in the queue--this is annoying as it's O(n)
  for(unsigned i = 0; i < heapSize_; ++i)
  {
    if(heap_[i].task == t)
    {
      deleteIdx(i);
      return;
    }
  }
}

void TaskHeap::recalculateWork()
{
    uint32_t system_out = rpm_system_hash;
    uint64_t work = 0;
    totalWork_ = 0;
    for(unsigned i = 0; i < heapSize_; ++i)
    {
        uint32_t temp_system_state = system_out;
        heap_[i].task->reestimate();
        // Use get_length_given to check for set transition cache hits.
        // Update the new system state for subsequent tasks.
        work = heap_[i].task->get_length_given(temp_system_state, system_out);
        heap_[i].workEstimate = work;
        totalWork_ += work;
    }
}

bool TaskHeap::less(const HeapItem &a, const HeapItem &b)
{
    uint8_t a_flex = a.task->get_priority();
    uint8_t b_flex = b.task->get_priority();
    return ((a_flex < b_flex) || ((a_flex == b_flex) && 
                ((insertionCount_ - a.insertID) > (insertionCount_ - b.insertID))));
}

