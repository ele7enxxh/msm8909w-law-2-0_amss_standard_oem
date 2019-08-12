/*===========================================================================
  FILE: Factory.cpp

  OVERVIEW: This file implements the ds::Utils::Factory class.

  DEPENDENCIES: None

  Copyright (c) 2007 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ds_Utils_Factory.cpp#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-05-02 hm  Created module.

===========================================================================*/

/*---------------------------------------------------------------------------
  Include Files
---------------------------------------------------------------------------*/
#include "comdef.h"
#include "ds_Utils_DebugMsg.h"
#include "ds_Utils_Factory.h"

using namespace ds::Utils;


/*===========================================================================

                       CONSTRUCTOR/DESTRUCTOR

===========================================================================*/
Factory::Factory
(
  void
)
throw()
: _list()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_1 ("_list(): "
                   "Creating Factory 0x%p", this);

} /* Factory() */

Factory::~Factory
(
  void
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_1 ("_list(): "
                   "Deleting Factory 0x%p", this);

  DeleteAllItems();

} /* ~Factory() */

/*===========================================================================

                       PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
int32 Factory::AddItem
(
  INode* item
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_2 ("Factory::AddItem(): "
                   "Fac 0x%p, item 0x%p", this, item);

  return _list.PushBack(item);

} /* PushBack() */

void Factory::RemoveItem
(
  INode*   item
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_1 ("Factory::RemoveItem(): "
                   "Fac 0x%p, item 0x%p", this);

  _list.RemoveItem(item);

} /* RemoveItem() */


void Factory::DeleteAllItems
(
  void
)
throw()
{
  INode *pItem;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_1 ("Factory::DeleteAllItems(): "
                   "Factory 0x%p", this);

  while (NULL != (pItem = _list.PopFront()))
  {
    (void) pItem->Release();
  }

} /* DeleteAllItems() */


boolean Factory::Traverse
(
  void* userDataPtr
)
throw()
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOG_MSG_INFO3_1 ("Factory::Traverse(): "
                   "Fac 0x%p", this);

  return _list.Traverse (userDataPtr);

} /* Traverse() */

//Harsh: This is very rough and ugly implementation for iterator.
//It is currently very inefficient. We need to clean this up.
void Factory::First 
(
  void 
)
throw()
{
  _list.First();
}

void Factory::Next 
(
  void 
)
throw()
{
  _list.Next();
}

INode * Factory::CurrentItem 
(
  void 
)
throw()
{
  return _list.CurrentItem();
}

boolean Factory::IsDone 
(
  void 
)
throw()
{
  return _list.IsDone();
}

