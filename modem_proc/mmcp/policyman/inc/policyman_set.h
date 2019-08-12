#ifndef _POLICYMAN_SET_H_
#define _POLICYMAN_SET_H_

/**
  @file policyman_set.h

  @brief  APIs to deal with policyman sets.
*/

/*
    Copyright (c) 2013,2014 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_set.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_i.h"
#include "mre_set.h"


/*=============================================================================
  Set APIS (a set is a dynamically sized set of items).
=============================================================================*/


// TODO: Create a new module for named objects.
#define POLICYMAN_NAMED_OBJECT_BASE       MRE_NAMED_OBJECT_BASE
#define policyman_named_object_t        mre_named_object_t
#define policyman_named_object_dtor     mre_named_object_dtor
#define policyman_named_object_init     mre_named_object_init
#define policyman_set_find_named_object mre_set_find_named_object


#define POLICYMAN_SET_NO_MAX_SIZE   MRE_SET_NO_MAX_SIZE

#define policyman_set_t             mre_set_t
#define policyman_set_new           mre_set_new
#define policyman_set_refcnt_new    mre_set_refcnt_new
#define policyman_set_add           mre_set_add
#define policyman_set_append        mre_set_append
#define policyman_set_replace       mre_set_replace
#define policyman_set_remove        mre_set_remove
#define policyman_set_contains      mre_set_contains
#define policyman_set_find          mre_set_find
#define policyman_set_num_elems     mre_set_num_elems
#define policyman_set_is_empty      mre_set_is_empty
#define policyman_set_get_buf_ptr   mre_set_get_buf_ptr
#define policyman_set_iterate       mre_set_iterate
#define policyman_set_elem_ptr      mre_set_elem_ptr


#endif /* _POLICYMAN_SET_H_ */
