/*============================================================================
   FILE:      ref_cnt_obj.h

   OVERVIEW:  Declarations and definitions for reference-counted objects.
              Reference-counted objects are dynamic objects whose lifetime is
              governed by the number of "references" (uses) of the object.  When
              the last reference to the object is released, its destructor is
              called and the memory used by the object is freed.  This allows
              for more straightforward design of the use of dynamic objects.

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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
============================================================================*/

#ifndef _REF_CNT_OBJ_H_
#define _REF_CNT_OBJ_H_

#include "atomic_ops.h"


/*-----------------------------------------------------------------------------
  Object destructors take an object pointer and return nothing.
-----------------------------------------------------------------------------*/
typedef void (*destructor_ptr)(void *object);

/*-----------------------------------------------------------------------------
  The structure that is used to implement reference-counted objects.  This
  should be treated as a black box by anything except code that unit-tests
  reference-counted objects.
-----------------------------------------------------------------------------*/
typedef struct
{
  destructor_ptr  destructor;
  atomic_word_t   ref_count;
} _ref_cnt_info_t;


/*-----------------------------------------------------------------------------
  All reference-counted object structures must begin with the line
    _REF_CNT_OBJ;

  For example

  typedef struct
  {
    _REF_CNT_OBJ;
    int   an_integer;
    char  *a_string;
  } my_refcounted_object_t;
-----------------------------------------------------------------------------*/
#define _REF_CNT_OBJ _ref_cnt_info_t _rcobj_


/*-----------------------------------------------------------------------------
  ref_cnt_obj_init
    This should be called from your object creation function to initialize the
    reference-counting aspects of the object.  It takes a pointer to a function
    to clean up an object (the destructor).  This initializes the reference
    count of the object to 1.
-----------------------------------------------------------------------------*/
void  ref_cnt_obj_init(void *object, destructor_ptr destructor);

/*-----------------------------------------------------------------------------
  ref_cnt_obj_add_ref
    This adds a reference to the object.  Use it when you are storing a pointer
    to the object or returning a pointer to the object to another component.  It
    need not be called when passing a pointer as a parameter in a function call
    - if the called function wants to save a reference to the object, *it* is
    responsible for calling ref_cnt_obj_add_ref().
-----------------------------------------------------------------------------*/
void  ref_cnt_obj_add_ref(void *object);

/*-----------------------------------------------------------------------------
  ref_cnt_obj_release
    This releases a reference to the object and calls the object's destructor
    when the reference count reaches zero.  Call it when the code owning a
    pointer to the object is done with the object.
-----------------------------------------------------------------------------*/
void  ref_cnt_obj_release(void *object);


/*-----------------------------------------------------------------------------
  REF_CNT_OBJ_RELEASE_IF
    A useful utility macro that checks to see whether an object pointer is
    non-NULL, releases it if it is, and then sets the pointer to NULL.
-----------------------------------------------------------------------------*/
#define REF_CNT_OBJ_RELEASE_IF(p) \
  do                              \
  {                               \
    if (NULL != (p))              \
    {                             \
      ref_cnt_obj_release(p);     \
      (p) = NULL;                 \
    }                             \
  /*lint -save -e717*/            \
  } while (0)                     \
  /*lint -restore*/
    

#endif   /* #ifndef _REF_CNT_OBJ_H_ */

