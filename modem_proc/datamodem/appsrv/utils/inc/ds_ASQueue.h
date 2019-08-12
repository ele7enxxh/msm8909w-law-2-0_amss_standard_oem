/*==============================================================================

                              ds_ASList.h

GENERAL DESCRIPTION
  Queue container
  Insert O(1)
  Remove O(1)
  Access O(1) *** first and last element access only ***

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/20/15    ml     Use memory from modem heap
06/10/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_AS_QUEUE_H
#define DS_AS_QUEUE_H

#include "comdef.h"
#include "ds_appsrv_mem.h"

/*==============================================================================
                              ASQueue Declaration
==============================================================================*/

/*==============================================================================
CLASS ASVector

DESCRIPTION
  Class for a queue container. Similar to STL Queue but with less features.
==============================================================================*/
template <typename T>
class ASQueue
{
public:
  ASQueue();
  ~ASQueue();

  /*===========================================================================
  FUNCTION ASQueue::front

  DESCRIPTION
    Returns the object at the front of the queue.
    WARNING!!! - Undefined behavior if this function is called on an empty queue.

  PARAMETERS
    None

  DEPENDENCIES
    Non-empty queue

  SIDE EFFECTS
    None
  ===========================================================================*/
  T& front();
  const T& front() const;


  /*===========================================================================
  FUNCTION ASQueue::back

  DESCRIPTION
    Returns the object at the end of the queue.
    WARNING!!! - Undefined behavior if this function is called on an empty queue.

  PARAMETERS
    None

  DEPENDENCIES
    Non-empty queue

  SIDE EFFECTS
    None
  ===========================================================================*/
  T& back();
  const T& back() const;


  /*===========================================================================
  FUNCTION ASQueue::push

  DESCRIPTION
    Adds the object the the end of the queue.

  PARAMETERS
    [In] object - The object to add to the container

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  bool push(const T& obj);


  /*===========================================================================
  FUNCTION ASQueue::pop

  DESCRIPTION
    Removes the top object in the queue. No effect if the queue is empty.

  PARAMETERS
    None

  DEPENDENCIES
    Non-empty queue

  SIDE EFFECTS
    None
  ===========================================================================*/
  void pop();


  /*===========================================================================
  FUNCTION ASQueue::empty

  DESCRIPTION
    Returns true if the queue is empty, otherwise false

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  bool empty() const;

private:
  struct node
  {
    T     obj;
    node* next_ptr;

    node(const T& object)
    : obj(object), next_ptr(NULL) { }
  };

  node* front_node; // pointer for top access
  node* back_node;  // pointer to add to
};



/*===========================================================================
FUNCTION clear_ptr_queue

DESCRIPTION
  Clears the queue and releases all the memory that the objects in the queue
  holds as well.

PARAMETERS
  vec - The vector to clear.

DEPENDENCIES
  ASQueue with pointer objects

SIDE EFFECTS
  None
===========================================================================*/
template <typename T>
void clear_ptr_queue(ASQueue<T*>& queue);









/*==============================================================================
                        ASQueue Implementation
==============================================================================*/
template <typename T>
ASQueue<T>::ASQueue()
: front_node(NULL), back_node(NULL) { }

template <typename T>
ASQueue<T>::~ASQueue()
{
  while(!empty())
  {
    node* tmp  = front_node;
    front_node = front_node->next_ptr;
    ds_appsrv_free(tmp);
  }
}


template <typename T>
T& ASQueue<T>::front()
{
  return front_node->obj;
}


template <typename T>
const T& ASQueue<T>::front() const
{
  return front_node->obj;
}



template <typename T>
T& ASQueue<T>::back()
{
  return back_node->obj;
}


template <typename T>
const T& ASQueue<T>::back() const
{
  return back_node->obj;
}



template <typename T>
bool ASQueue<T>::push(const T& obj)
{
  node* newnode = new(ds_appsrv_alloc<node>(), APPSRV_MEM) node(obj);
  if(NULL == newnode)
    return false;

  if(NULL == front_node)
  {
    front_node = newnode;
  }
  else
  {
    back_node->next_ptr = newnode;
  }
  back_node = newnode;

  return true;
}



template <typename T>
void ASQueue<T>::pop()
{
  if(NULL == front_node)
    return;

  node* tmp  = front_node;
  front_node = front_node->next_ptr;
  ds_appsrv_free(tmp);

  if(NULL == front_node)
    back_node = NULL;
}



template <typename T>
bool ASQueue<T>::empty() const
{
  return (NULL == front_node);
}





template <typename T>
void clear_ptr_queue(ASQueue<T*>& queue)
{
  while(!queue.empty())
  {
    ds_appsrv_free(queue.front());
    queue.pop();
  }
}



#endif /* DS_AS_QUEUE_H */
