/*==============================================================================

                        ds_ASSort.h

GENERAL DESCRIPTION
  General sort function to sort objects in ASVector. Comparison function must be
  provided by the caller.

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
04/29/14    ml     Created file/Initial version.
==============================================================================*/

#ifndef DS_ASSORT_H
#define DS_ASSORT_H

#include "ds_ASVector.h"

namespace Appsrv
{
namespace Utils
{


// Compare two pointers (T is a pointer type) using *T's operator<
template<typename T>
struct Less_than_ptr {
  bool operator()(const T p1, const T p2) const {return *p1 < *p2;}
};

// Compare two objects using T's operator<
template<typename T>
struct Less_than_obj {
  bool operator()(const T& p1, const T& p2) const {return p1 < p2;}
};

// Compare two pointers (T is a pointer type) using *T's operator>
template<typename T>
struct More_than_ptr {
  bool operator()(const T p1, const T p2) const {return *p1 > *p2;}
};

// Compare two objects using T's operator>
template<typename T>
struct More_than_obj {
  bool operator()(const T& p1, const T& p2) const {return p1 > p2;}
};


/*===========================================================================
FUNCTION ds_merge_sort

DESCRIPTION
  Sorts the ASVector based on the compare function/functor given using merge
  sort algorithm.

PARAMETERS
  [In/Out] vec - The vector to sort
  [In]     cmp - The method used to compare two values. It can be either a
                 functor or function pointer that takes in two parameters
                 (T1, T2) and returns a bool value.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
template<typename T, typename Comparer>
void ds_merge_sort(ASVector<T> &vec, Comparer &cmp);


/*===========================================================================
FUNCTION ds_insert_sort

DESCRIPTION
  Sorts the ASVector based on the compare function/functor given using insert
  sort algorithm.

PARAMETERS
  [In/Out] vec - The vector to sort
  [In]     cmp - The method used to compare two values. It can be either a
                 functor or function pointer that takes in two parameters
                 (T1, T2) and returns a bool value.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
template<typename T, typename Comparer>
void ds_insert_sort(ASVector<T> &vec, Comparer &cmp);








/*==============================================================================
                         Merge Sort Implementation
==============================================================================*/
/*
  Merge sort helper function. Sorts the vector between [left, right)
*/
template<class T, typename Comparer>
void ds_merge_sort_helper(ASVector<T> &vec, Comparer &cmp, const uint32 left, const uint32 right);





template<class T, typename Comparer>
void ds_merge_sort(ASVector<T> &vec, Comparer &cmp)
{
  ds_merge_sort_helper(vec, cmp, 0, vec.size());
}



template<class T, typename Comparer>
void ds_merge_sort_helper(ASVector<T> &vec, Comparer &cmp, const uint32 left, const uint32 right)
{
  uint32 vec_len = right - left;
  // right not after left || default case vector size 1.
  if(left > right || vec_len <= 1)
    return;

  uint32 half_point = left + (vec_len / 2);

  ds_merge_sort_helper(vec, cmp, left, half_point);
  ds_merge_sort_helper(vec, cmp, half_point, right);

  ASVector<T> temp_vec(vec_len);
  uint32 left_index  = left;
  uint32 right_index = half_point;

  for(uint32 i = 0; i < vec_len; ++i)
  {
    if(
       left_index < half_point &&
       (right_index == right ||
        cmp(vec[left_index], vec[right_index]))
       )
    {
      temp_vec[i] = vec[left_index++];
    }
    else
    {
      temp_vec[i] = vec[right_index++];
    }
  }

  for(uint32 i = 0, vec_index = left; i < vec_len; ++i, ++vec_index)
  {
    vec[vec_index] = temp_vec[i];
  }
}






template<typename T, typename Comparer>
void ds_insert_sort(ASVector<T> &vec, Comparer &cmp)
{
  for(uint32 i = 1; i < vec.size(); ++i)
  {
    uint32 j = i;
    while(j > 0 && cmp(vec[j], vec[j-1]))
    {
      T tmp = vec[j];
      vec[j] = vec[j-1];
      vec[j-1] = tmp;
      --j;
    }
  }
}




} /* namespace Utils */
} /* namespace Appsrv */


#endif /* DS_ASSORT_H */
