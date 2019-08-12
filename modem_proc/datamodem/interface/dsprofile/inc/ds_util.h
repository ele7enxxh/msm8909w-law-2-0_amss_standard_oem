/******************************************************************************
  @file    ds_util.h
  @brief    

  DESCRIPTION
  This file contains:
  1. Utility functions for DS PROFILE

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsprofile/inc/ds_util.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/09   mg      Created the module. First version of the file.
===========================================================================*/

#ifndef DS_UTIL_H
#define DS_UTIL_H

#include "comdef.h"
#include "customer.h"
#include "list.h"

#define DS_SUCCESS 0
#define DS_FAILURE 1

/*---------------------------------------------------------------------------
                     Iterator Public Data Structures
---------------------------------------------------------------------------*/
typedef void * ds_util_itr_hndl_type;
typedef void   (*frst)( void * );
typedef int   (*next)( void * );
typedef void * (*data)( void * );
typedef int    (*size)( void * );
typedef void   (*dstr)( void * );

typedef struct
{
  frst  f;
  next  n;
  data  i;
  size  s;
  dstr  d;
  void *curr;
} ds_util_iterable_type;

/*---------------------------------------------------------------------------
                          List Public Data Structures
---------------------------------------------------------------------------*/
typedef void *  ds_util_list_hndl_type;

typedef struct
{
  list_link_type  link;
  void           *data;
  int             size;
  void           *self;
} ds_util_list_node_type;

typedef struct
{
  ds_util_iterable_type   itr;
  list_type               lst;
  void                   *priv;
  void                   *self;
} ds_util_list_type;

/*---------------------------------------------------------------------------
                          List Public Fn declaration
---------------------------------------------------------------------------*/
int ds_util_list_get_hndl(
  ds_util_list_hndl_type *hndl
);

int ds_util_list_add(
  ds_util_list_hndl_type  hndl,
  void                   *info,
  uint32                  info_size
);

/* the same as above, but returns the pointer to the data copy in the list, 
   NULL in case of failure */
void* ds_util_list_add_node(
  ds_util_list_hndl_type  hndl,
  void                   *info,
  uint32                  info_size
);

/* returns pointer to the data copy in the list if found,
   NULL - if not found 
   if several copies are present, the first one is returned */
/* !!! WARNING !!! 
   the size of all elements in the list should be at least as info_size */
void* ds_util_list_find_node(
  ds_util_list_hndl_type  hndl,
  void                   *info,
  uint32                  info_size
);

/* !!! WARNING !!! 
   the size of all elements in the list should be at least as info_size */
int ds_util_list_remove(
  ds_util_list_hndl_type  hndl,
  void                   *info, 
  uint32                  info_size
);

/* creates a list in destination output param and copies source list to it */
/* !!! WARNING !!! 
   all elements in the list should be the same size */
int ds_util_list_copy(
  ds_util_list_hndl_type  source,
  ds_util_list_hndl_type*  destination /* <- */
); 

/* empties the list */
int ds_util_list_reset(
  ds_util_list_hndl_type  hndl
);

/*===========================================================================
FUNCTION ds_util_list_do_for_every_node
 
DESCRIPTION
  This function is used to iterate over the list and perform some action for 
  node(s) in it. 

PARAMETERS 
  hndl                  - list to iterate
  func                  - function to perform, with the first argument of data in node
                        - and additional variadic number of argument
                          function should return 0 in case of success, 
                          any other value - otherwise   

                          !!! Important !!!
                          func receives arguments in va_list, which is already initialized

  stop_on_first_success - stop after first successful func invocation
  func_successs         - Output param. Can be NULL if don't care.  
                          Will be true if ds_util_list_do_for_every_node() returned
                          success and also func succeeded at least once. 
                          (Will never be true in case of ds_util_list_do_for_every_node()
                          failure)

DEPENDENCIES 
  !!! All elements in the list should be the same size !!!
  
RETURN VALUE
  DS_SUCCESS - in case of success, also, when the list is empty
  DS_FAILURE - in case of failure

SIDE EFFECTS 
  
===========================================================================*/
int ds_util_list_do_for_every_node(
  ds_util_list_hndl_type  hndl,
  int (*func)(void* data, va_list),
  boolean stop_on_first_success,  
  boolean* func_success, 
  ...
);

uint32 ds_util_list_get_size(
  ds_util_list_hndl_type hndl
);


int ds_util_list_rel_hndl(
  ds_util_list_hndl_type hndl
);

/*---------------------------------------------------------------------------
                      Iterator Public Fn
---------------------------------------------------------------------------*/
int ds_util_itr_get_hndl(
  ds_util_iterable_type *obj, /* -> */
  ds_util_itr_hndl_type *hndl /* <- */
);

int ds_util_itr_get_data(
  ds_util_itr_hndl_type   hndl,
  void                   *obj,  /* <- */
  uint32                 *size  /* <-> */
);

/* same as above but returns the pointer inside the list, rather than copy it */
int ds_util_itr_get_data_in_list(
  ds_util_itr_hndl_type   hndl,
  void                    **obj,  /* <- */
  uint32                 *size  /* <-> */
);

int ds_util_itr_first(
  ds_util_itr_hndl_type   hndl
);

int ds_util_itr_next(
  ds_util_itr_hndl_type  hndl
);

int ds_util_itr_rel_hndl(
   ds_util_itr_hndl_type  hndl
);

#endif /* DS_UTIL_H */
