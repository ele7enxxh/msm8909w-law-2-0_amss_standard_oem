/*===========================================================================
  FILE: List.cpp

  OVERVIEW: This file implements the ds::Utils::List class.

  DEPENDENCIES: None

  Copyright (c) 2007-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ds_Utils_List.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-05-10 hm  Added support for cloning the list before traversal.
  2008-08-29 hm  Added Iterator implementation.
  2008-05-02 hm  Created module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Include Files
---------------------------------------------------------------------------*/
#include "comdef.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_StdErr.h"
#include "ds_Errors_Def.h"
#include "ds_Utils_List.h"

#include "ds_Utils_CSSupport.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_CCritSect.h"
#include "list.h"

#include "ps_system_heap.h"

using namespace ds::Error;
using namespace ds::Utils;

/*===========================================================================

                     INTERNAL CLASSES AND FUNCTION DEFINITIONS

===========================================================================*/

typedef struct _list_node
{
  list_link_type        _list_link;
  ds::Utils::INode*     _item_ptr;
} _list_node_type;

namespace ds
{
namespace Utils
{

class ListInternal
{
public:
  ListInternal() throw();
  virtual ~ListInternal() throw();

  list_type     _list;
  int           _current;

  static void* operator new
  (
    unsigned int numBytes
  )
  throw();

  static void operator delete
  (
    void* bufPtr
  )
  throw();

}; /* class ListInternal */

}  // namespace Utils
}  // namespace ds

/*---------------------------------------------------------------------------
  Namespace declarations. This is done after ListInternal class is
  defined to avoid lint error 578.
---------------------------------------------------------------------------*/
using namespace ds::Error;
using namespace ds::Utils;

ListInternal::ListInternal
(
  void
)
throw()
: _current(0)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset (&_list, 0, sizeof(_list));
  list_init (&_list);

} /* ListInternal() */

ListInternal::~ListInternal
(
  void
)
throw()
{
  _list_node_type *  _list_node_ptr;
  INode *            _item_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  while (NULL !=
         (_list_node_ptr = (_list_node_type *) list_pop_front(&_list)))
  {
    _item_ptr = _list_node_ptr->_item_ptr;
    (void) _item_ptr->ReleaseWeak();
    PS_SYSTEM_HEAP_MEM_FREE (_list_node_ptr);
  }

  list_destroy(&_list);

} /* ~ListInternal() */

void* ListInternal::operator new
(
  unsigned int numBytes
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   void * buf = NULL;
   PS_SYSTEM_HEAP_MEM_ALLOC(buf, numBytes, void *);
   return buf; 
} /* ListInternal::operator new() */


void ListInternal::operator delete
(
  void* bufPtr
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == bufPtr)
  {
    DATA_ERR_FATAL("delete(): NULL ptr");
    return;
  }

  PS_SYSTEM_HEAP_MEM_FREE(bufPtr);

} /* ListInternal::operator delete() */

/*===========================================================================

                       PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
List::List
(
  void
)
throw()
{
/*-------------------------------------------------------------------------*/

  LOG_MSG_INFO3_1 ("List::List(): "
                   "obj 0x%p", this);

  pListInternal = (ds::Utils::ListInternal *) new ListInternal();
  if (0 == pListInternal)
  {
    DATA_ERR_FATAL("List::List(): No mem for List");
  }

  if (AEE_SUCCESS != DS_Utils_CreateInstance (NULL,
                                              AEECLSID_CCritSect,
                                              (void **) &pCritSect))
  {
    DATA_ERR_FATAL("List::List(): Cannot create crit sect");
  }
} /* List() */


List::~List
(
  void
)
throw()
{
/*-------------------------------------------------------------------------*/

  LOG_MSG_INFO3_1 ("List::~List(): "
                   "obj 0x%p", this);

  DS_UTILS_RELEASEIF(pCritSect);
  delete (ds::Utils::ListInternal *) pListInternal;

} /* ~List() */

/* Prepares a list node for pushing to front or to back, returns errors.
   Refactors the common part out of PushBack and PushFront.

   Must be called from within critical section.
*/
int32 List::PreparePush
(
  INode *item,
  void **listNodePtr
)
throw()
{
  _list_node_type    * _node_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  *listNodePtr = 0;
  LOG_MSG_INFO3_3 ("List::PreparePush(): "
                   "list 0x%p, node 0x%p, size %d", this, item, Count());

  if (0 == item)
  {
    LOG_MSG_ERROR_0 ("List::PreparePush(): "
                     "Can't push a NULL item");
    return QDS_EFAULT;
  }

  /* Avoid multiple enqueue of the same item */
  for (_node_ptr  = (_list_node_type *) list_peek_front (&pListInternal->_list);
       _node_ptr != 0;
       _node_ptr  = (_list_node_type *)
                    list_peek_next (&pListInternal->_list, &(_node_ptr->_list_link)))
  {
    if (_node_ptr->_item_ptr == item)
    {
      return AEE_SUCCESS;
    }
  }

  /*lint -save -e774 Reason: Mem allocation can fail and NULL check a must*/
  PS_SYSTEM_HEAP_MEM_ALLOC_ASSERT(_node_ptr, sizeof(_list_node_type),
                                  _list_node_type*);
  if (0 == _node_ptr)
  {
    return AEE_ENOMEMORY;
  }
  /*lint -restore Restore lint error 774*/

  /* Store INode item, increment weak ref-count of item */
  _node_ptr->_item_ptr = item;
  (void) item->AddRefWeak();

  *listNodePtr = _node_ptr;
  return AEE_SUCCESS;

} /* List::PreparePush() */

int32 List::PushBack
(
  INode* item
)
throw()
{
  _list_node_type    * _node_ptr;
  ds::ErrorType        result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pCritSect->Enter();

  result = PreparePush(item, (void **)&_node_ptr);
  if (0 != _node_ptr)
  {
    list_push_back (&pListInternal->_list, &(_node_ptr->_list_link));
  }

  pCritSect->Leave();

  return result;

} /* PushBack() */

/**
  @brief      This function inserts an object at the front of the list.

  @param[in]  item - Pointer to the item to be inserted.

  @return     AEE_SUCCESS on success.
  @return     Error code on error.
*/

int32 List::PushFront
(
  INode* item
)
throw()
{
  _list_node_type    * _node_ptr;
  ds::ErrorType        result = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pCritSect->Enter();

  result = PreparePush(item, (void **)&_node_ptr);
  if (0 != _node_ptr)
  {
    list_push_front (&pListInternal->_list, &(_node_ptr->_list_link));
  }

  pCritSect->Leave();

  return result;

} /* PushFront() */

/**
  @brief      This function removes an item from the front of the list.

  @param      None.

  @return     AEE_SUCCESS on success.
  @return     Error code on error.
*/

INode* List::PopFront
(
  void
)
throw()
{
  _list_node_type* _node_ptr;
  INode*           _item_ptr = NULL;
  boolean          _have_strong_ref = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO3_2 ("List::PopFront(): "
                   "list 0x%p, size %d", this, Count());

  pCritSect->Enter();

  _node_ptr = (_list_node_type *) list_pop_front (&pListInternal->_list);
  while (NULL != _node_ptr)
  {
    _item_ptr = _node_ptr->_item_ptr;
    _have_strong_ref = _item_ptr->GetStrongRef();
    (void) _item_ptr->ReleaseWeak();
    PS_SYSTEM_HEAP_MEM_FREE (_node_ptr);
    if (_have_strong_ref)
    {
      pCritSect->Leave();

      return _item_ptr;
    }
  }

  pCritSect->Leave();

  return NULL;

} /* PopFront() */

void List::RemoveItem
(
  INode*   item
)
throw()
{
  _list_node_type* _node_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO3_3 ("List::RemoveItem(): "
                   "list 0x%p, node 0x%p, size %d", this, item, Count());

  pCritSect->Enter();

  for (_node_ptr  = (_list_node_type *) list_peek_front (&pListInternal->_list);
       _node_ptr != NULL;
       _node_ptr  = (_list_node_type *) list_peek_next (&pListInternal->_list, &(_node_ptr->_list_link)))
  {
    if (_node_ptr->_item_ptr == item)
    {
      list_pop_item (&pListInternal->_list, &(_node_ptr->_list_link));
      (void) item->ReleaseWeak();
      PS_SYSTEM_HEAP_MEM_FREE (_node_ptr);
      break;
    }
  }

  pCritSect->Leave();

} /* PopFront() */

int32 List::Count
(
  void
)
throw()
{
  int32 result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pCritSect->Enter();

  result = list_size (&pListInternal->_list);

  pCritSect->Leave();

  return result;

} /* Count() */



INode* List::Get
(
  int32 argIndex
)
throw()
{
  int32 index;
  _list_node_type* _node_ptr;
  INode* result = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 > argIndex || Count() <= argIndex)
  {
    return NULL;
  }

  pCritSect->Enter();

  index = 0;
  for (_node_ptr  = (_list_node_type *) list_peek_front (&pListInternal->_list);
       _node_ptr != NULL;
       _node_ptr  = (_list_node_type *) list_peek_next (&pListInternal->_list, &(_node_ptr->_list_link)))
  {
    if (index == argIndex)
    {
      result = _node_ptr->_item_ptr;
      break;
    }

    index ++;
  }

  pCritSect->Leave();

  return result;

} /* Get() */

boolean List::CloneList
(
  List *  listPtr
)
throw()
{
  _list_node_type *                _node_ptr;
  ds::ErrorType                    dsErrno = AEE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_2 ("List::CloneList(): "
                   "list 0x%p, size %d", this, Count());

  if (0 == listPtr)
  {
    DATA_ERR_FATAL("List::CloneList(): NULL list");
    return FALSE;
  }

  /* Copy items from passed List */

  pCritSect->Enter();

  _node_ptr = (_list_node_type *) list_peek_front (&listPtr->pListInternal->_list);

  while (_node_ptr != NULL && _node_ptr->_item_ptr != NULL)
  {
    dsErrno = PushBack( _node_ptr->_item_ptr );
    if (AEE_SUCCESS != dsErrno)
    {
      LOG_MSG_ERROR_2("List::CloneList(): "
                      "PushBack failed, list 0x%x err 0x%x", this, dsErrno);
      pCritSect->Leave();
      return FALSE;
    }

    /* Increment the iterator */
    _node_ptr = (_list_node_type *) list_peek_next (&listPtr->pListInternal->_list,
                                                    &(_node_ptr->_list_link));
  }

  pCritSect->Leave();

  LOG_MSG_INFO3_1("List::CloneList(): "
                  "Success, list 0x%x", this);
  return TRUE;

} /* CloneList() */

/*---------------------------------------------------------------------------
  Methods from the ITraverser interface.
---------------------------------------------------------------------------*/
boolean List::Traverse
(
  void* userDataPtr
)
throw()
{
  boolean                          result;
  INode*                           currentItem;
  _list_node_type *                _node_ptr;
  List *                           newListPtr      = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_2 ("List::Traverse(): "
                   "list 0x%p, size %d", this, Count());

  result = TRUE;

  /* Create a Cloned List */
  newListPtr = new List();
  if (0 == newListPtr)
  {
    DATA_ERR_FATAL("List::Traverse(): No mem for list");
    return FALSE;
  }

  /* Clone the List */
  pCritSect->Enter();
  result = newListPtr->CloneList( this);
  pCritSect->Leave();
  if (TRUE != result)
  {
    LOG_MSG_ERROR_1("List::Traverse(): "
                    "CloneList failed, list 0x%x ", this);
    goto bail;
  }

  /* Traverse on cloned List */
  _node_ptr = (_list_node_type *) list_peek_front (&newListPtr->pListInternal->_list);
  while (_node_ptr != NULL && _node_ptr->_item_ptr != NULL)
  {
    /* Get the current item */
    currentItem = _node_ptr->_item_ptr;

    /* Increment the iterator first, the current item can get deleted */
    _node_ptr = (_list_node_type *) list_peek_next (&newListPtr->pListInternal->_list,
                                                    &(_node_ptr->_list_link));

    /* Try to obtain a strong reference to the item, only if successful
       Perform Process() on current item */
    if (TRUE == currentItem->GetStrongRef())
    {
      result = currentItem->Process (userDataPtr);

      currentItem->Release();

      if (FALSE == result)
      {
        goto bail;
      }
    }

  } /* while */

  delete newListPtr;
  return result;

bail:
  delete newListPtr;
  return result;

} /* Traverse() */

/*---------------------------------------------------------------------------
  Methods from the IIterator interface.
---------------------------------------------------------------------------*/
void List::First
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_1 ("List::First(): "
                   "list 0x%p", this);

  pCritSect->Enter();

  pListInternal->_current = 0;

  pCritSect->Leave();

} /* First() */


void List::Next
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pCritSect->Enter();

  pListInternal->_current++;

  pCritSect->Leave();

  LOG_MSG_INFO3_2 ("List::Next(): "
                   "list 0x%p, pos %d", this, pListInternal->_current);

} /* Next() */

boolean List::IsDone
(
  void
)
throw()
{
  boolean result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_2 ("List::IsDone(): "
                   "list 0x%p, pos %d", this, pListInternal->_current);

  pCritSect->Enter();

  result = (pListInternal->_current >= Count());

  pCritSect->Leave();

  return result;

} /* IsDone() */

INode* List::CurrentItem
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_2 ("List::CurrentItem(): "
                   "list 0x%p, pos %d", this, pListInternal->_current);

  pCritSect->Enter();

  INode* item = Get(pListInternal->_current);

  pCritSect->Leave();

  return item;

} /* CurrentItem() */


/*---------------------------------------------------------------------------
  Overloaded new/delete methods.
---------------------------------------------------------------------------*/
void* List::operator new
(
  unsigned int numBytes
)
throw()
{
  void * buf = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  PS_SYSTEM_HEAP_MEM_ALLOC(buf, numBytes, void*);
  return buf; 
} /* List::operator new() */


void List::operator delete
(
  void* bufPtr
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (0 == bufPtr)
  {
    DATA_ERR_FATAL("delete(): NULL ptr");
    return;
  }

  PS_SYSTEM_HEAP_MEM_FREE(bufPtr);

} /* List::operator delete() */

