#ifndef TASK_LIST_H
#define TASK_LIST_H

// TaskList is an intrusively-implemented doubly-linked list type.  At some
// point in the future the mechanism for implementing the hooks will be more
// typesafe and portable, however in the interest of expediency the location of
// the hooks is passed as an offset from the base of the task structure.

class Task;

class TaskList
{
public:
    inline TaskList();

    // Returns true if there are no elements in the list
    inline bool empty();

    // Returns a pointer to the first element of the list
    inline Task *head();
    inline const Task *head() const;

    // Returns a pointer to the last element of the list
    inline Task *tail();
    inline const Task *tail() const;

    // Inserts new element 't' at the front of the list
    void push_front(Task &t);

    // Inserts new element 't' at the back of the list
    void push_back(Task &t);

    // Inserts new element 't' just before the task 'pos' -- O(1)
    void insert(Task &pos, Task &t);

	// Removes a specific task from the list -- O(1)
	void remove(Task &t);

    // Removes the first element from the list.
    void pop_front();

    // Removes the last element from the list.
    void pop_back();

private:
    Task *head_;
    Task *tail_;
};

/////////////////////////////////////
//Implementation of inline funcitons.
/////////////////////////////////////

inline TaskList::TaskList()
    : head_(0), tail_(0)
{
}

inline bool TaskList::empty()
{
    return (0 == head_);
}

inline Task *TaskList::head()
{
    return head_;
}

inline const Task *TaskList::head() const
{
    return head_;
}

inline Task *TaskList::tail()
{
    return tail_;
}

inline const Task *TaskList::tail() const
{
    return tail_;
}

#endif // TASK_HEAP_H

