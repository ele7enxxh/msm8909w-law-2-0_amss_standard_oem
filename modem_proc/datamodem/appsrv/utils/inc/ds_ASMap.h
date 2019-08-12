/*==============================================================================

                              ds_ASMap.h

GENERAL DESCRIPTION
  Map container. Internal structure is a red-black tree
  TODO: Hashmap, Multimap, Set, MultiSet

  Insert O(log(n))
  Remove O(log(n))
  Access O(log(n))

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
#ifndef DS_AS_MAP_H
#define DS_AS_MAP_H

#include "comdef.h"
#include "ds_appsrv_mem.h"

#include "data_msg.h"


namespace Appsrv
{
namespace Utils
{


template <typename FirstType, typename SecondType>
struct ASPair
{
  FirstType  first;
  SecondType second;

  ASPair()
  { }

  // ASPair(const ASPair& other)
  // : first(other.first), second(other.second)
  // { }

  ASPair(const FirstType& val1, const SecondType& val2)
  : first(val1), second(val2)
  { }
};


/*==============================================================================
CLASS ASMap

DESCRIPTION
  Class for a map container. Similar to STL map but with less features.
==============================================================================*/
template <typename KeyType, typename MappedType>
class ASMap
{
private:
  // ASMap internal types
  typedef ASPair<KeyType, MappedType> ValueType;

  enum node_color
  {
    ASMAP_NODE_BLACK,
    ASMAP_NODE_RED
  };

  struct node
  {
    ValueType  obj;
    ValueType* obj_ptr;
    node*      parent_node;
    node*      left_node;
    node*      right_node;
    node_color color;

    node()
    : parent_node(NULL), left_node(NULL), right_node(NULL), color(ASMAP_NODE_RED)
    { }

    node(const ValueType& object)
    : obj(object), obj_ptr(NULL), parent_node(NULL), left_node(NULL), right_node(NULL), color(ASMAP_NODE_RED)
    { }

    ~node()
    { }
  };

public:
  template <typename ObjType, typename NodeType>
  class Iterator
  {
  public:
    // default initialize to nullptr
    Iterator()
    : node_ptr(NULL)
    { }

    // Copy constructor & operator
    Iterator(Iterator& other)
    : node_ptr(other.node_ptr)
    { }

    Iterator(const Iterator& other)
    : node_ptr(other.node_ptr)
    { }

    Iterator& operator= (Iterator& other)
    {
      node_ptr = other.node_ptr;
      return *this;
    }

    Iterator& operator= (const Iterator& other)
    {
      node_ptr = other.node_ptr;
      return *this;
    }

    // Overloaded dereferencing operators
    // * returns a reference to the datum in the pointed-to node
    ObjType& operator* ()
    { return node_ptr->obj; }

    const ObjType& operator* () const
    { return node_ptr->obj; }

    ObjType* operator-> ()
    { return &node_ptr->obj; }

    const ObjType* operator-> () const
    { return &node_ptr->obj; }

    // ++ operator moves the iterator forward to point to the next node; return *this by reference
    Iterator& operator++ () // prefix
    {
      if(NULL == node_ptr)
        return *this;

      if(NULL != node_ptr->right_node)
      {
        node_ptr = node_ptr->right_node;
        while(NULL != node_ptr->left_node)
        {
          node_ptr = node_ptr->left_node;
        }
        return *this;
      }

      while(NULL != node_ptr->parent_node)
      {
        if(node_ptr->parent_node->right_node == node_ptr)
        {
          node_ptr = node_ptr->parent_node;
          continue;
        }
        else
        {
          node_ptr = node_ptr->parent_node;
          return *this;
        }
      }

      // end() reached
      node_ptr = NULL;
      return *this;
    }


    Iterator& operator++ (int) // postfix
    {
      if(NULL == node_ptr)
        return *this;

      if(NULL != node_ptr->right_node)
      {
        node_ptr = node_ptr->right_node;
        while(NULL != node_ptr->left_node)
        {
          node_ptr = node_ptr->left_node;
        }
        return *this;
      }

      while(NULL != node_ptr->parent_node)
      {
        if(node_ptr->parent_node->right_node == node_ptr)
        {
          node_ptr = node_ptr->parent_node;
          continue;
        }
        else
        {
          node_ptr = node_ptr->parent_node;
          return *this;
        }
      }

      // end() reached
      node_ptr = NULL;
      return *this;
    }

    // TODO: Reverse iterator
    // Iterator& operator-- (); // prefix
    // Iterator& operator-- (int); // postfix

    // Iterators are equal if they point to the same node
    bool operator== (const Iterator &rhs) const
    { return (node_ptr == rhs.node_ptr); }
    bool operator!= (const Iterator &rhs) const
    { return (node_ptr != rhs.node_ptr); }

    friend class ASMap;

  private:
    // Constructor only accessible by ASMap
    Iterator(NodeType* node)
    : node_ptr(node)
    { }

    NodeType* node_ptr;
  };

  typedef Iterator<ValueType, node> iterator;
  typedef Iterator<const ValueType, const node> const_iterator;



public:
  // ASMap member functions
  ASMap();
  ~ASMap();

  // Accessor
  MappedType& operator[](const KeyType& key);
  ValueType at(const KeyType& key);


  // Modifiers
  // pair<iterator, bool> insert(const KeyType& key, const MappedType& value);
  // ASPair<iterator, bool> insert(const ValueType& pair);
  bool insert(const ValueType& pair);
  void erase(const KeyType& key);
  void erase(iterator& position);
  void clear();


  uint32 size() const
  { return container_size; }

  bool empty() const;


  // Iterator functions
  iterator begin();
  const_iterator begin() const;

  iterator end();
  const_iterator end() const;

  iterator find(const KeyType& key);
  const_iterator find(const KeyType& key) const;



private:
  // Tree utils
  node* get_grandparent_node(node* current);
  node* get_uncle_node(node* current);
  node* get_sibling_node(node* current);
  void rotate_left(node*& root_node);
  void rotate_right(node*& current_node);

  void insert_rebalance(node* current_node);
  void remove_rebalance(node* current_node);


  bool is_black(const node* n) const;
  bool is_red(const node* n) const;

  void erase_helper(node* base_node, node* parent_node, node* sibling_node);
  void erase(node* erase_node);
  void swap_values(node* lhs, node* rhs);
  void insert(node*& current_node, node* new_node);

  void clear_helper(node* erase_node);
  node* find_node(const KeyType& key) const;


private:
  node*  root_node; // root node
  uint32 container_size;
};




// template <typename T>
// void clear_ptr_map(ASMap<Key, Value*>& list);













/*==============================================================================
                           ASMap Implementation
==============================================================================*/
template <typename KeyType, typename MappedType>
ASMap<KeyType, MappedType>::ASMap()
: root_node(NULL), container_size(0)
{ }


template <typename KeyType, typename MappedType>
ASMap<KeyType, MappedType>::~ASMap()
{
  clear();
}




template <typename KeyType, typename MappedType>
void ASMap<KeyType, MappedType>::clear()
{
  clear_helper(root_node);
  root_node      = NULL;
  container_size = 0;
}



template <typename KeyType, typename MappedType>
void ASMap<KeyType, MappedType>::clear_helper(node* erase_node)
{
  if(NULL == erase_node)
    return;

  clear_helper(erase_node->left_node);
  clear_helper(erase_node->right_node);
  ds_appsrv_free(erase_node);
}



template <typename KeyType, typename MappedType>
bool ASMap<KeyType, MappedType>::insert(const ValueType& pair)
{
  // Check if key already is used. If so, reject insert.
  iterator it = find(pair.first);
  if(end() != it)
  {
    // return ASPair(it, false);
    return false;
  }


  node* new_node = new(ds_appsrv_alloc<node>(), APPSRV_MEM) node(pair);
  if(NULL == new_node)
  {
    // return ASPair(end(), false);
    return false;
  }

  // ASPair<iterator, bool> result(iterator(new_node), true);
  insert(root_node, new_node);
  insert_rebalance(new_node);

  // return ASPair<bool, bool>(true, true);
  // return ASPair<iterator, bool>(iterator(new_node), true);

  ++container_size;
  return true;
}



template <typename KeyType, typename MappedType>
void ASMap<KeyType, MappedType>::insert(node*& current_node, node* new_node)
{
  if(NULL == current_node)
  {
    current_node = new_node;
  }
  else if(current_node->obj.first > new_node->obj.first)
  {
    new_node->parent_node = current_node;
    insert(current_node->left_node, new_node);
  }
  else
  {
    new_node->parent_node = current_node;
    insert(current_node->right_node, new_node);
  }
}



template <typename KeyType, typename MappedType>
void ASMap<KeyType, MappedType>::insert_rebalance(node* current_node)
{
  // Node is root
  if(NULL == current_node->parent_node)
  {
    current_node->color = ASMAP_NODE_BLACK;
    return;
  }

  if(ASMAP_NODE_BLACK == current_node->parent_node->color)
    return;

  node* uncle       = get_uncle_node(current_node);
  node* grandparent = get_grandparent_node(current_node); // if parent is red, grandparent can be assumed to not be NULL
  node* parent_node = current_node->parent_node;

  if(NULL == grandparent)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "insert_rebalance - Unexpected NULL grandparent node for a red parent node");
    return;
  }

  // case 3
  if(NULL != uncle && uncle->color == ASMAP_NODE_RED)
  {
    parent_node->color = ASMAP_NODE_BLACK;
    uncle->color       = ASMAP_NODE_BLACK;
    grandparent->color = ASMAP_NODE_RED;
    insert_rebalance(grandparent);
    return;
  }

  // case 4
  if(current_node == parent_node->right_node && parent_node == grandparent->left_node)
  {
    rotate_left(parent_node);
    current_node = current_node->left_node;
  }
  else if(current_node == parent_node->left_node && parent_node == grandparent->right_node)
  {
    rotate_right(parent_node);
    current_node = current_node->right_node;
  }
  // case 5
  parent_node->color = ASMAP_NODE_BLACK;
  grandparent->color = ASMAP_NODE_RED;

  if(current_node == parent_node->left_node)
  {
    rotate_right(grandparent);
  }
  else
  {
    rotate_left(grandparent);
  }
}







/// TODO TODO TODO

template <typename KeyType, typename MappedType>
void ASMap<KeyType, MappedType>::erase(const KeyType& key)
{
  erase( find(key) );
}


template <typename KeyType, typename MappedType>
void ASMap<KeyType, MappedType>::erase(iterator& it)
{
  if(it == end() || NULL == it.node_ptr)
    return;

  erase(it.node_ptr);
  it.node_ptr = NULL;
  --container_size;
}


template <typename KeyType, typename MappedType>
void ASMap<KeyType, MappedType>::erase(node* erase_node)
{
  // if erase node has 2, find in-order predecessor and swap locations with it
  if(NULL != erase_node->left_node && NULL != erase_node->right_node)
  {
    node* replace_node = erase_node->left_node;

    while(NULL != replace_node->right_node)
      replace_node = replace_node->right_node;

    swap_values(erase_node, replace_node);
    erase_node = replace_node;
  }

  // erase_node will have at most 1 child now

  // Replace erase node with its child node; child will be NULL if it has no child
  node* child = (NULL != erase_node->left_node) ? erase_node->left_node : erase_node->right_node;
  node* sibling_node = get_sibling_node(erase_node);

  if(NULL != erase_node->parent_node)
  {
    if(erase_node == erase_node->parent_node->left_node)
    {
      erase_node->parent_node->left_node = child;
    }
    else
    {
      erase_node->parent_node->right_node = child;
    }

    if(NULL != child)
    {
      child->parent_node = erase_node->parent_node;
    }
  }
  else
  {
    // erase node is root node, mark child as black and finish
    if(NULL != child)
    {
      child->color       = ASMAP_NODE_BLACK;
      child->parent_node = NULL;
    }

    root_node = child;
    return;
  }

  if( is_black(erase_node) )
  {
    if( is_red(child) )
    {
      child->color = ASMAP_NODE_BLACK;
    }
    else
    {
      erase_helper(child, erase_node->parent_node, sibling_node);
    }
  }

  ds_appsrv_free(erase_node);
}



template <typename KeyType, typename MappedType>
void ASMap<KeyType, MappedType>::erase_helper(node* base_node, node* parent_node, node* sibling_node)
{
  if(NULL == parent_node)
  {
    // base node is root node, mark it as black and finish
    if(NULL != base_node)
    {
      base_node->color       = ASMAP_NODE_BLACK;
      base_node->parent_node = NULL;
    }

    root_node = base_node;
    return;
  }

  if(NULL == sibling_node)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "erase_helper - Unexpected NULL sibling");
    return;
  }

  if( is_red(sibling_node) )
  {
    parent_node->color  = ASMAP_NODE_RED;
    sibling_node->color = ASMAP_NODE_BLACK;

    if (sibling_node == parent_node->right_node)
    {
      rotate_left(parent_node);
    }
    else
    {
      rotate_right(parent_node);
    }
  }


  // case 3,4
  if( is_black(sibling_node) && is_black(sibling_node->left_node) && is_black(sibling_node->right_node) )
  {
    // case 3
    if(is_black(parent_node))
    {
      sibling_node->color = ASMAP_NODE_RED;
      erase_helper( parent_node, parent_node->parent_node, get_sibling_node(parent_node) );
    }
    else // case 4
    {
      sibling_node->color = ASMAP_NODE_RED;
      parent_node->color  = ASMAP_NODE_BLACK;
    }
    return;
  }


  // case 5
  if(sibling_node->color == ASMAP_NODE_BLACK)
  {
    if( sibling_node == parent_node->right_node && is_black(sibling_node->right_node) && is_red(sibling_node->left_node) )
    {
      sibling_node->color            = ASMAP_NODE_RED;
      sibling_node->left_node->color = ASMAP_NODE_BLACK;
      rotate_right(sibling_node);
    }
    else if( sibling_node == parent_node->left_node && is_black(sibling_node->left_node) && is_red(sibling_node->right_node) )
    {
      sibling_node->color             = ASMAP_NODE_RED;
      sibling_node->right_node->color = ASMAP_NODE_BLACK;
      rotate_left(sibling_node);
    }
  }


  // case 6
  sibling_node->color = parent_node->color;
  parent_node->color  = ASMAP_NODE_BLACK;

  if (sibling_node == parent_node->right_node)
  {
    if(NULL != sibling_node->right_node)
    {
      sibling_node->right_node->color = ASMAP_NODE_BLACK;
    }

    rotate_left(parent_node);
  }
  else
  {
    if(NULL != sibling_node->left_node)
    {
      sibling_node->left_node->color = ASMAP_NODE_BLACK;
    }

    rotate_right(parent_node);
  }
}

















/*==============================================================================
                           ASMap Node Manipulators
==============================================================================*/
template <typename KeyType, typename MappedType>
void ASMap<KeyType, MappedType>::swap_values(node* lhs, node* rhs)
{
  if(NULL == lhs || NULL == rhs)
    return;

  ASPair<KeyType, MappedType> tmp_obj = lhs->obj;
  lhs->obj = rhs->obj;
  rhs->obj = tmp_obj;
}


template <typename KeyType, typename MappedType>
typename ASMap<KeyType, MappedType>::node* ASMap<KeyType, MappedType>::get_grandparent_node(node* current)
{
  if(NULL != current && NULL != current->parent_node)
    return current->parent_node->parent_node;

  return NULL;
}



template <typename KeyType, typename MappedType>
typename ASMap<KeyType, MappedType>::node* ASMap<KeyType, MappedType>::get_uncle_node(node* current)
{
  node* gp = get_grandparent_node(current);
  if(NULL == gp)
    return NULL;

  if(current->parent_node == gp->left_node)
    return gp->right_node;
  else
    return gp->left_node;
}



template <typename KeyType, typename MappedType>
typename ASMap<KeyType, MappedType>::node* ASMap<KeyType, MappedType>::get_sibling_node(node* current)
{
  if(NULL == current->parent_node)
    return NULL;

  if (current == current->parent_node->left_node)
    return current->parent_node->right_node;
  else
    return current->parent_node->left_node;
}



template <typename KeyType, typename MappedType>
bool ASMap<KeyType, MappedType>::is_black(const node* n) const
{
  return (NULL == n || ASMAP_NODE_BLACK == n->color);
}



template <typename KeyType, typename MappedType>
bool ASMap<KeyType, MappedType>::is_red(const node* n) const
{
  return (NULL != n && ASMAP_NODE_RED == n->color);
}



template <typename KeyType, typename MappedType>
void ASMap<KeyType, MappedType>::rotate_left(node*& base_node)
{
  if(NULL == base_node || NULL == base_node->right_node)
    return;

  // Rotate nodes
  node* pivot           = base_node->right_node;
  base_node->right_node = pivot->left_node;
  pivot->left_node      = base_node;

  // Link correct parent nodes
  pivot->parent_node     = base_node->parent_node;
  base_node->parent_node = pivot;
  if(NULL != base_node->right_node)
  {
    base_node->right_node->parent_node = base_node;
  }


  base_node = pivot;

  // Link the new base node to its parent
  if(NULL == base_node->parent_node)
  {
    root_node = base_node;
  }
  else
  {
    if(base_node->parent_node->left_node == base_node->left_node)
      base_node->parent_node->left_node = base_node;
    else
      base_node->parent_node->right_node = base_node;
  }
}



template <typename KeyType, typename MappedType>
void ASMap<KeyType, MappedType>::rotate_right(node*& base_node)
{
  if(NULL == base_node || NULL == base_node->left_node)
    return;

  // Rotate nodes
  node* pivot          = base_node->left_node;
  base_node->left_node = pivot->right_node;
  pivot->right_node    = base_node;

  // Link correct parent nodes
  pivot->parent_node                = base_node->parent_node;
  base_node->parent_node            = pivot;
  if(NULL != base_node->left_node)
  {
    base_node->left_node->parent_node = base_node;
  }

  base_node = pivot;

  if(NULL == base_node->parent_node)
  {
    root_node = base_node;
  }
  else
  {
    if(base_node->parent_node->left_node == base_node->right_node)
      base_node->parent_node->left_node = base_node;
    else
      base_node->parent_node->right_node = base_node;
  }
}











/*==============================================================================
                           ASMap Iterator
==============================================================================*/
template <typename KeyType, typename MappedType>
typename ASMap<KeyType, MappedType>::iterator ASMap<KeyType, MappedType>::begin()
{
  if(NULL == root_node)
    return end();

  node* begin_node = root_node;
  while(NULL != begin_node->left_node)
  {
    begin_node = begin_node->left_node;
  }

  return iterator(begin_node);
}



template <typename KeyType, typename MappedType>
typename ASMap<KeyType, MappedType>::const_iterator ASMap<KeyType, MappedType>::begin() const
{
  if(NULL == root_node)
    return end();

  const node* begin_node = root_node;
  while(NULL != begin_node->left_node)
  {
    begin_node = begin_node->left_node;
  }

  return const_iterator(begin_node);
}



template <typename KeyType, typename MappedType>
typename ASMap<KeyType, MappedType>::iterator ASMap<KeyType, MappedType>::end()
{
  return iterator(NULL);
}



template <typename KeyType, typename MappedType>
typename ASMap<KeyType, MappedType>::const_iterator ASMap<KeyType, MappedType>::end() const
{
  return const_iterator(NULL);
}




template <typename KeyType, typename MappedType>
typename ASMap<KeyType, MappedType>::iterator ASMap<KeyType, MappedType>::find(const KeyType& key)
{
  return iterator( find_node(key) );
}



template <typename KeyType, typename MappedType>
typename ASMap<KeyType, MappedType>::const_iterator ASMap<KeyType, MappedType>::find(const KeyType& key) const
{
  return const_iterator( find_node(key) );
}




template <typename KeyType, typename MappedType>
typename ASMap<KeyType, MappedType>::node* ASMap<KeyType, MappedType>::find_node(const KeyType& key) const
{
  node* result = NULL;
  node* current_node = root_node;

  while(NULL != current_node)
  {
    if(key == current_node->obj.first)
    {
      result = current_node;
      break;
    }

    if(key < current_node->obj.first)
    {
      current_node = current_node->left_node;
    }
    else
    {
      current_node = current_node->right_node;
    }
  }

  return result;
}






} /* namespace Utils */
} /* namespace Appsrv */

#endif /* DS_AS_MAP_H */
