/*!
  @file
  rflm_dm_buf_mgr_t.h

  @brief
  The function defines the core class template forlist manager and buffer array. 
  Most of the DM buffers are derived from these two classes. 

  @detail
  OPTIONAL detailed description of this C header file.

*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/services/dm/inc/rflm_dm_buf_mgr_t.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/18/13   qwu     Changed Char to uint32
09/06/13   JYW     Added init support
09/03/13   JJ      DM hack, allocate DM handle ID to 0
===========================================================================*/

#ifndef RFLM_DM_BUF_TEMPLATE_H
#define RFLM_DM_BUF_TEMPLATE_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
#include "qurt_mutex.h"
#ifdef __cplusplus
}
#endif


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*! @brief Template for the list managment
  - mNumFreeItems inidcate how many free items are available
  - Two arrays, mUsed, mFree serve as a basic list for active items and  
    free items.
*/
template<int32 MAX_NUM_ITEM> 
class MyList
{
public:
  MyList(void);
  int32 newItem(void);
  boolean deleteItem(int32 idx);
  void  list_init(void);
  boolean is_idx_used(int32 idx);
  int32 free_cnt();

private:
  /*!< The constant numbe of free item */
  const int32 mTotalItems;
  int32 mNumFreeItems; 
  int32 mUsed[MAX_NUM_ITEM];
  int32 mFree[MAX_NUM_ITEM];
  /*!< mutex to serialize the allocation /deallocation  */
  qurt_mutex_t mMutex;
};

/*! @brief define the constructor outside the class body to avoid code increase
    by inlining the function */
template<int32 MAX_NUM_ITEM> 
MyList<MAX_NUM_ITEM>::MyList(void):mTotalItems(MAX_NUM_ITEM) 
{
  list_init();
  qurt_mutex_init (&mMutex);
}

/*! @brief return the private free count
*/
template<int32 MAX_NUM_ITEM> 
inline int32 MyList<MAX_NUM_ITEM>::free_cnt(void)
{
  return mNumFreeItems;
}

/*! @brief check if the idx is currently in use
*/
template<int32 MAX_NUM_ITEM> 
inline boolean MyList<MAX_NUM_ITEM>::is_idx_used(int32 idx)
{
  /* Loop index and return value */
  boolean rv=TRUE; int32 i;

  /* Loop through the Used list to check if the idx is currently in use */
  for(i=0; i<(MAX_NUM_ITEM-mNumFreeItems);i++) 
  {
    if(mUsed[i]==idx) { break;}
  }

  /* If not found, return FALSE */
  if(i==(MAX_NUM_ITEM-mNumFreeItems)) { rv=FALSE; }
  return rv;
}


/*! @brief Get a new item from the manangment list 
*/
template<int32 MAX_NUM_ITEM> 
int32 MyList<MAX_NUM_ITEM>::newItem(void)
{
  qurt_mutex_lock(&mMutex);
  int32 new_item=-1;
  if(mNumFreeItems > 0) 
  {
    /* Get a new index from the tail of the list */
    new_item=mFree[mNumFreeItems-1];
    //mfree[mNumFreeItems-1]=-1;
    mNumFreeItems--;
    mUsed[MAX_NUM_ITEM-mNumFreeItems-1]=new_item;
  }
  qurt_mutex_unlock(&mMutex);
  return new_item;
};

/*! @brief The function initialize the list and set all items as free.
 */
template<int32 MAX_NUM_ITEM> 
inline void MyList<MAX_NUM_ITEM>::list_init(void)
{
  /* loop index */
  int32 i;
  mNumFreeItems=MAX_NUM_ITEM;

  /* All index are in the free list */
  for(i=0; i<MAX_NUM_ITEM; i++) 
  {
    mFree[i]=MAX_NUM_ITEM-i-1; //mUsed[i]=-1;
  }
};

/*! @brief The function remove a index from the used list and instert it back
    into the free list. */
template<int32 MAX_NUM_ITEM> 
boolean MyList<MAX_NUM_ITEM>::deleteItem(int32 idx)
{
  qurt_mutex_lock(&mMutex);
  /* Loop index and return value */
  int32 i; boolean rv= FALSE;

  for(i=0; i<(MAX_NUM_ITEM-mNumFreeItems);i++) 
  {
    if(mUsed[i]==idx) { break; }
  }

  if(i<(MAX_NUM_ITEM-mNumFreeItems)) {
    mUsed[i]=mUsed[MAX_NUM_ITEM-mNumFreeItems-1];
    mFree[mNumFreeItems]=idx;
    mNumFreeItems++;
    rv=TRUE;
  }
  qurt_mutex_unlock(&mMutex);
  return rv;
};


/*! @brief Template for buffer
  - list is the managmenet unit
  - buf[MAX_NUM_ITEM] is actually the buffer storage
*/
template<class T, int32 MAX_NUM_ITEM> 
class MyBufMgrTemplate 
{
public:
  MyBufMgrTemplate():mTotalBufs(MAX_NUM_ITEM), max_usage(0), totalNewBuffs(0), totalDeleteBuffs(0) {}
  T* newItem(int32 *rv_idx);
  boolean deleteItem(int32 idx);
  boolean deleteItem(uint32* idx);
  void init();
  int32 get_array_idx(uint32* ptr);
  boolean is_buf_in_use(uint32* ptr);
  boolean is_buf_in_use(int32 idx);
  int32 get_pool_size(void);
  int32 get_free_cnt(void);
  boolean is_buff_full(void);
  /*lint -save -esym(1401,*MyBufMgrTemplate*buf*) */
  T buf[MAX_NUM_ITEM];
  /*lint -restore */
private:
  MyList<MAX_NUM_ITEM> list;
  const int32 mTotalBufs;
  uint32 max_usage;
  uint32 totalNewBuffs;
  uint32 totalDeleteBuffs;
};


/*! @brief The function returns the total number of free buffers 
*/
template<class T, int32 MAX_NUM_ITEM> 
inline int32 MyBufMgrTemplate<T, MAX_NUM_ITEM>::get_free_cnt(void)
{
  return list.free_cnt();
}

/*! @brief The function returns if the buff is full 
*/
template<class T, int32 MAX_NUM_ITEM> 
inline boolean MyBufMgrTemplate<T, MAX_NUM_ITEM>::is_buff_full(void)
{
  return (get_free_cnt() == 0)? TRUE: FALSE;
}


/*! @brief The function returns the total number of buffers
*/
template<class T, int32 MAX_NUM_ITEM> 
inline int32 MyBufMgrTemplate<T, MAX_NUM_ITEM>::get_pool_size(void)
{
  return mTotalBufs;
}


/*! @brief The function returns a new buffer ptr and its buffer index 
*/
template<class T, int32 MAX_NUM_ITEM> 
inline T* MyBufMgrTemplate<T, MAX_NUM_ITEM>::newItem(int32 *rv_idx)
{
  T* rv_ptr=NULL;
  int32 idx=list.newItem();
  *rv_idx = idx;
  uint32 total_used = 0;
   
  if((idx>=0) && (idx<MAX_NUM_ITEM))
  {
	  rv_ptr=(T*)&buf[idx];
	  total_used = mTotalBufs - list.free_cnt();
	  max_usage  = (max_usage >= total_used)? max_usage:total_used;
	  totalNewBuffs++;
  }
  return rv_ptr;
}


/*! @brief The function deallocates an used buffer and return the buffer back
   into the free list
*/
template<class T, int32 MAX_NUM_ITEM> 
inline boolean MyBufMgrTemplate<T, MAX_NUM_ITEM>::deleteItem(int32 idx)
{
  boolean rv=list.deleteItem(idx);
  totalDeleteBuffs++;
  return rv;
}

/*! @brief The function deallocates an used buffer and return the buffer back
   into the free list; the function uses buf ptr to find the array index
*/
template<class T, int32 MAX_NUM_ITEM> 
inline boolean MyBufMgrTemplate<T, MAX_NUM_ITEM>::deleteItem(uint32* buf_ptr)
{
  boolean rv=FALSE;
  int32 idx=get_array_idx(buf_ptr);
  if((idx!=-1) && (idx<MAX_NUM_ITEM)) {
    rv=list.deleteItem(idx);
    //memset(buf_ptr, 0, sizeof(T));
  }
  return rv;
}

/*! @brief The function memset the buffer area
*/
template<class T, int32 MAX_NUM_ITEM> 
inline void MyBufMgrTemplate<T, MAX_NUM_ITEM>::init(void)
{
  //Reset the list 
  list.list_init();
  //Memset the buff to zero
  //memset(buf,0, MAX_NUM_ITEM*sizeof(T));
}


/*! @brief The function translates a buffer ptr to an array index
*/
template<class T, int32 MAX_NUM_ITEM> 
inline int32 MyBufMgrTemplate<T, MAX_NUM_ITEM>::get_array_idx(uint32* ptr)
{
  int32 rv=-1;

  // This assumes the buf[MAX_NUM_ITEM]
  int64 offset=(uint32)ptr-(uint32)buf;
  if(( offset%sizeof(T)==0)&&(offset>=0)&&(offset/sizeof(T)<MAX_NUM_ITEM)) 
  {
    rv= offset/sizeof(T);
  }
  
  return rv;
}

/*! @brief The function check if the buffer is linked in the mUsed list; it takes
    the buffer ptr as the input.
*/
template<class T, int32 MAX_NUM_ITEM> 
inline boolean MyBufMgrTemplate<T, MAX_NUM_ITEM>::is_buf_in_use(uint32* ptr)
{
  int32 idx=get_array_idx(ptr);
  return list.is_idx_used(idx);
}


/*! @brief The function check if the buffer is linked in the mUsed list. It takes
    the buffer index number
*/
template<class T, int32 MAX_NUM_ITEM> 
inline boolean MyBufMgrTemplate<T, MAX_NUM_ITEM>::is_buf_in_use(int32 idx)
{
  return list.is_idx_used(idx);
}

#endif /* RFLM_DM_BUF_TEMPLATE_H */
