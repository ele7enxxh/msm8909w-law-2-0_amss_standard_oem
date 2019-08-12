/*==============================================================================

                              ds_ASList.h

GENERAL DESCRIPTION
  Linked list container.
  Insert O(1)
  Remove O(n)
  Access O(n)

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
#ifndef DS_AS_LIST_H
#define DS_AS_LIST_H

#include "comdef.h"
#include "ds_appsrv_mem.h"

/*==============================================================================
CLASS ASVector

DESCRIPTION
  Class for a list container. Similar to STL List but with less features.
==============================================================================*/
template <typename T>
class ASList
{
public:
  ASList();
  ~ASList();

  T& operator[](uint32 index);
  const T& operator[](uint32 index) const;

  T& front();
  const T& front() const;
  T& back();
  const T& back() const;

  // bool push_front(const T& obj); // TODO
  void pop_front();
  bool push_back(const T& obj);
  void pop_back();
  void erase(uint32 index);

  uint32 size() const
  { return container_size; }

  bool empty() const;

private:
  struct node
  {
    T     obj;
    node* next_ptr;

    node()
    : next_ptr(NULL)
    { }

    node(const T& object)
    : obj(object), next_ptr(NULL)
    { }
  };

public:
  template <typename ObjType, typename NodeType>
  class Iterator
  {
  public:
    // default initialize to nullptr
    Iterator()
    : node_ptr(NULL), prev_ptr(NULL)
    { }

    // Copy constructor & operator
    Iterator(Iterator& other)
    : node_ptr(other.node_ptr), prev_ptr(other.prev_ptr)
    { }

    // Copy constructor & operator
    Iterator(const Iterator& other)
    : node_ptr(other.node_ptr), prev_ptr(other.prev_ptr)
    { }

    Iterator& operator= (Iterator& other)
    {
      node_ptr = other.node_ptr;
      prev_ptr = other.prev_ptr;
      return *this;
    }

    Iterator& operator= (const Iterator& other)
    {
      node_ptr = other.node_ptr;
      prev_ptr = other.prev_ptr;
      return *this;
    }

    // Overloaded dereferencing operators
    // * returns a reference to the datum in the pointed-to node
    ObjType& operator* ()
    { return node_ptr->obj; }

    const ObjType& operator* () const
    { return node_ptr->obj; }

    // ++ operator moves the iterator forward to point to the next node; return *this by reference
    Iterator& operator++ () // prefix
    {
      if(NULL != node_ptr)
      {
        prev_ptr = node_ptr;
        node_ptr = node_ptr->next_ptr;
      }
      return *this;
    }
    Iterator& operator++ (int) // postfix
    {
      if(NULL != node_ptr)
      {
        prev_ptr = node_ptr;
        node_ptr = node_ptr->next_ptr;
      }
      return *this;
    }
    // Iterators are equal if they point to the same node
    bool operator== (const Iterator &rhs) const
    { return (node_ptr == rhs.node_ptr); }
    bool operator!= (const Iterator &rhs) const
    { return (node_ptr != rhs.node_ptr); }

    friend class ASList;

  private:
    // Constructor only accessible by ASList
    Iterator(NodeType* head)
    : node_ptr(head->next_ptr), prev_ptr(head)
    { }

    NodeType* node_ptr;
    NodeType* prev_ptr;
  };

  typedef Iterator<T, node> iterator;
  typedef Iterator<const T, const node> const_iterator;

  // Iterator functions
  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  iterator erase(iterator& position);

private:
  node head_node; // head node
  node* back_node; // pointer to add
  uint32 container_size;
};


template <typename T>
void clear_ptr_list(ASList<T*>& list);





/*==============================================================================
                           ASList Implementation
==============================================================================*/
template <typename T>
ASList<T>::ASList()
: back_node(NULL), container_size(0)
{
  back_node = &head_node;
}



template <typename T>
ASList<T>::~ASList()
{
  node* tmp;

  while(NULL != head_node.next_ptr)
  {
    tmp = head_node.next_ptr;
    head_node.next_ptr = head_node.next_ptr->next_ptr;
    ds_appsrv_free(tmp);
  }
}


template <typename T>
T& ASList<T>::operator[](uint32 index)
{
  node* tmp = head_node.next_ptr;

  for(uint32 i = 0; i < index; i++)
  {
    tmp = tmp->next_ptr;
  }
  return tmp->obj;
}


template <typename T>
const T& ASList<T>::operator[](uint32 index) const
{
  node* tmp = head_node.next_ptr;

  for(uint32 i = 0; i < index; i++)
  {
    tmp = tmp->next_ptr;
  }
  return tmp->obj;
}



template <typename T>
T& ASList<T>::front()
{
  return head_node.next_ptr->obj;
}


template <typename T>
const T& ASList<T>::front() const
{
  return head_node.next_ptr->obj;
}



template <typename T>
T& ASList<T>::back()
{
  return back_node->obj;
}


template <typename T>
const T& ASList<T>::back() const
{
  return back_node->obj;
}


// template <typename T>
// bool ASList<T>::push_front(const T& obj)
// {
// }


template <typename T>
void ASList<T>::pop_front()
{
  if(0 != container_size)
  {
    erase(0);
  }
}


template <typename T>
bool ASList<T>::push_back(const T& obj)
{
  back_node->next_ptr = new(ds_appsrv_alloc<node>(), APPSRV_MEM) node(obj);
  if(NULL == back_node->next_ptr)
    return false;

  back_node = back_node->next_ptr;
  container_size++;
  return true;
}


template <typename T>
void ASList<T>::pop_back()
{
  if(0 != container_size)
  {
    erase(container_size - 1);
  }
}



template <typename T>
void ASList<T>::erase(const uint32 index)
{
  if(index >= container_size)
    return;

  node* tmp       = head_node.next_ptr;
  node* prev_node = &head_node;

  for(uint32 i = 0; i < index; i++)
  {
    prev_node = tmp;
    tmp = tmp->next_ptr;
  }

  prev_node->next_ptr = tmp->next_ptr;
  ds_appsrv_free(tmp);

  if(NULL == prev_node->next_ptr) // end_node
    back_node = prev_node;

  container_size--;
}



template <typename T>
typename ASList<T>::iterator ASList<T>::erase(iterator& position)
{
  if(position == end() || NULL == position.node_ptr || NULL == position.prev_ptr)
    return position;

  node* tmp                   = position.node_ptr;
  position.prev_ptr->next_ptr = position.node_ptr->next_ptr;
  position.node_ptr           = position.node_ptr->next_ptr;

  if(NULL == position.node_ptr) // end_node
    back_node = position.prev_ptr;
  ds_appsrv_free(tmp);
  container_size--;

  return position;
}


// Iterator functions
template <typename T>
typename ASList<T>::iterator ASList<T>::begin()
{ return iterator(&head_node); }

template <typename T>
typename ASList<T>::iterator ASList<T>::end()
{ return iterator(); }



template <typename T>
typename ASList<T>::const_iterator ASList<T>::begin() const
{ return const_iterator(&head_node); }

template <typename T>
typename ASList<T>::const_iterator ASList<T>::end() const
{ return const_iterator(); }



template <typename T>
bool ASList<T>::empty() const
{
  return (NULL == head_node.next_ptr);
}



template <typename T>
void clear_ptr_list(ASList<T*>& list)
{
  typename ASList<T*>::iterator it = list.begin();
  while(it != list.end())
  {
    ds_appsrv_free(*it);
    it = list.erase(it);
  }
}



#endif /* DS_AS_LIST_H */
