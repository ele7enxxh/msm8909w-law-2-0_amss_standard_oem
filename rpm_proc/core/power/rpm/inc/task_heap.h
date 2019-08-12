#ifndef TASK_HEAP_H
#define TASK_HEAP_H

#include <stdint.h>

class Task;

class TaskHeap
{
public:
    TaskHeap(unsigned size_hint = 10);
    inline ~TaskHeap();

    inline bool empty() const;

    void insert(Task *new_task);

    bool findMin(Task **top);

    void deleteMin();

    void deleteTask(Task *t);

    // Retrieve an eagerly-calculated sum-of-work for all tasks in this queue.
    inline uint64_t totalWork() const;

    // Re-calculate said eagerly-calculated sum-of-work (as in cpu speed change).
    void recalculateWork();

private:
    struct HeapItem
    {
        unsigned insertID;
        uint64_t workEstimate;
        Task    *task;
    };

    void expandHeap();
    void deleteIdx(unsigned idx);

    bool less(const HeapItem &a, const HeapItem &b);

    uint64_t  totalWork_;

    unsigned  insertionCount_;

    unsigned  heapReserved_;
    unsigned  heapSize_;
    HeapItem *heap_;
};

/////////////////////////////////////
//Implementation of inline funcitons.
/////////////////////////////////////

inline TaskHeap::~TaskHeap()
{
    delete [] heap_;
}

inline bool TaskHeap::empty() const
{
    return (0 == heapSize_);
}

inline uint64_t TaskHeap::totalWork() const
{
    return totalWork_;
}

#endif // TASK_HEAP_H

