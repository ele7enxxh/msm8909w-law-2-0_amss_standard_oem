/******************************************************************************
  @file    ds_profile_util.c
  @brief

  DESCRIPTION
  This file contains:
  1. Utility functions for DS PROFILE

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2008, 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsprofile/src/ds_util.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/30/10   ars     Made modem_mem_alloc/free and str fns platform independent
09/30/09   mg      Created the module. First version of the file.
===========================================================================*/

#include "ds_util.h"
#include "ds_profile_os.h"
#include "comdef.h"
#include <string.h>
#include <stdarg.h>

#define IS_ITR_HNDL_VALID( itr_hndl ) \
          ( (uint32)itr_hndl <= ITR_MAX_NUMBER_HANDLES )

#define MAGIC_EMPTY 0xFFFFFFFF

/* removes the particular node stored in list, rather then searching for info 
   Internal API */
int ds_util_list_remove_list_node(
  ds_util_list_hndl_type  hndl,
  ds_util_list_node_type  *node_ptr
);

/*---------------------------------------------------------------------------
               Implementation of List iterator fn
---------------------------------------------------------------------------*/
void list_first(
  void *t
)
{
  ds_util_list_type *l  = ( ds_util_list_type *) t;

  /* Get what is the current node */
  ds_util_list_node_type *first = (ds_util_list_node_type *)(l->lst.front_ptr);

  /* Validation */
  if ( ( first == NULL) || ( (uint32)first == MAGIC_EMPTY ) )
  {
    /* This cannot be handled, stop here */
    return;
  }

  l->itr.curr = ( void * )first;
  return;
}

int list_next(
  void *t
)
{
  ds_util_list_type *lst  = ( ds_util_list_type *) t;

  /* Get what is the current node */
  ds_util_list_node_type *curr = lst->itr.curr;

  /* Validation */
  if ( ( curr == NULL) || ( (uint32)curr == MAGIC_EMPTY ) )
  {
    /* This cannot be handled, stop here */
    return DS_FAILURE;
  }

  /* Advance by 1 and return */
  lst->itr.curr           = ( void * ) ( ( ( list_link_type * ) curr )->next_ptr );
  if ( lst->itr.curr == NULL )
  {
    return DS_FAILURE;
  }
  return DS_SUCCESS;
}

/* TO DO make get_data and get_size to 1 function, (ret type error code) */
void * list_get_data(
  void *t
)
{
  ds_util_list_type *l  = ( ds_util_list_type *) t;

  /* Get what is the current node */
  ds_util_list_node_type *curr = l->itr.curr;

  /* Validation */
  if ( ( curr == NULL) || ( (uint32)curr == MAGIC_EMPTY ) )
  {
    return NULL;
  }

  /* Return data from node */
  return ( void *) curr->data;
}

int list_get_size(
  void *t
)
{
  ds_util_list_type *l  = ( ds_util_list_type *) t;

  /* Get what is the current node */
  ds_util_list_node_type *curr = l->itr.curr;

  /* Validation */
  if ( ( curr == NULL) || ( (uint32)curr == MAGIC_EMPTY ) )
  {
    return 0;
  }

  /* Return data from node */
  return curr->size;
}

void list_dstr(
  void *t
)
{
  ds_util_list_type *l  = ( ds_util_list_type *) t;
  l->itr.f( t );
  return;
}

/*---------------------------------------------------------------------------
               Implementation of List Fn
---------------------------------------------------------------------------*/
int ds_util_list_get_hndl
(
  ds_util_list_hndl_type *hndl /* <- */
)
{
  ds_util_list_type *l = NULL;

  /* Allocate and init list blob a */

  l = ( ds_util_list_type * )DS_PROFILE_MEM_ALLOC( sizeof( ds_util_list_type ),
                                              MODEM_MEM_CLIENT_DATA );

  if ( l == NULL )
  {
    return DS_FAILURE;
  }
  memset( l, 0, sizeof( ds_util_list_type ) );
  list_init( &l->lst );
  l->itr.f      = list_first;
  l->itr.n      = list_next;
  l->itr.i      = list_get_data;
  l->itr.s      = list_get_size;
  l->itr.d      = list_dstr;
  l->itr.curr   = (void *)MAGIC_EMPTY;
  *hndl         = (ds_util_list_hndl_type)l;

  return DS_SUCCESS;
}/* ds_util_list_get_hndl() */

int ds_util_list_add(
  ds_util_list_hndl_type  hndl,
  void                   *info,
  uint32                  info_size
)
{
  ds_util_list_type *l = ( ds_util_list_type * )hndl;
  ds_util_list_node_type *node;

  node = ( ds_util_list_node_type * )
           DS_PROFILE_MEM_ALLOC( sizeof( ds_util_list_node_type ),
                                 MODEM_MEM_CLIENT_DATA );
  /* TO DO check size */

  if ( node == NULL)
  {
    return DS_FAILURE;
  }

  memset( (void *)node, 0, sizeof( ds_util_list_node_type ) );

  node->data = (void *)DS_PROFILE_MEM_ALLOC( info_size, MODEM_MEM_CLIENT_DATA );
  if ( node->data == NULL)
  {
    DS_PROFILE_MEM_FREE( (void *)node, MODEM_MEM_CLIENT_DATA );
    return DS_FAILURE;
  }

  memset( (void *)node->data, 0, info_size );
  node->size = info_size;
  memscpy( node->data, info_size, info, info_size );
  node->self = node;

  /* first node is added, subsequent updates to curr is thru (*next)(), frst is unchanged */
  if ( ( (uint32)l->itr.curr == MAGIC_EMPTY ) )
  {
     //DS_ASSERT( l->itr.curr == NULL );
     //DS_ASSERT( l->itr.frst == NULL );
     l->itr.curr = ( void * )node;
  }

  list_push_back( &l->lst, ( list_link_type *)node );

  return DS_SUCCESS;
}/*ds_util_list_add()*/

/* the same as above, but returns the pointer to data copy in the list, 
   NULL in case of failure */
void* ds_util_list_add_node(
  ds_util_list_hndl_type  hndl,
  void                   *info,
  uint32                  info_size
)
{
  ds_util_list_type *l = ( ds_util_list_type * )hndl;

  if (DS_SUCCESS != ds_util_list_add(hndl, info, info_size))
  {
    return NULL;
  }

  return ((ds_util_list_node_type*)(list_peek_back(&l->lst)))->data;
}

static int func_find_element(void* data, va_list args) 
{  
  void** element;
  void*  info;
  uint32  info_size;  
  int res = -1;
  
  element = va_arg(args, void**);
  info  = va_arg(args, void*);
  info_size = va_arg(args, uint32);

  if (0 == memcmp(data, info, info_size))
  {
    *element = data;               
    res = 0;    
  }

  return res;
}

/* returns pointer to the data copy in the list if found,
   NULL - if not found 
   if several copies are present, the first one is returned */
/* !!! WARNING !!! 
   the size of all elements in the list should be at least as info_size */
void* ds_util_list_find_node(
  ds_util_list_hndl_type  hndl,
  void                   *info,
  uint32                  info_size
)
{
  int res;  
  void* element = NULL;
  ds_util_itr_hndl_type itr_hndl; 
  uint32 real_size;

  if (0 == ds_util_list_get_size(hndl))
  {
    return 0;
  }

  if (ds_util_itr_get_hndl((ds_util_iterable_type *)hndl, &itr_hndl) != DS_SUCCESS)
  {
    return 0; 
  }

  if (ds_util_itr_first(itr_hndl) != DS_SUCCESS)
  {
    ds_util_itr_rel_hndl(itr_hndl);
    return 0;         
  }  

  real_size = ((ds_util_iterable_type*)itr_hndl)->s((void*)itr_hndl);      
  ds_util_itr_rel_hndl(itr_hndl);  

  if (info_size > real_size)
  {
    return 0;
  }
  
  res = ds_util_list_do_for_every_node(hndl, 
                                       func_find_element, 
                                       TRUE,
                                       NULL,
                                       &element,
                                       info,
                                       info_size);                                       

  if (DS_SUCCESS != res)
  {
    return 0;
  }

  return element;
}


static int func_remove_element(void* data, va_list args) 
{
  ds_util_list_hndl_type hndl;
  void    *info;
  uint32  info_size;  
  int res = -1;  

  hndl = va_arg(args, ds_util_list_hndl_type);
  info  = va_arg(args, void*);
  info_size = va_arg(args, uint32);

  if (0 == memcmp(data, info, info_size))
  {
    ds_util_list_remove_list_node(hndl, 
                                  ((ds_util_list_node_type*)((ds_util_iterable_type*)hndl)->curr));                             
    res = 0;    
  }
  
  return res;
}

/* !!! WARNING !!! 
   the size of all elements in the list should be at least as info_size */
int ds_util_list_remove(
  ds_util_list_hndl_type  hndl,
  void                   *info, 
  uint32                  info_size
)
{ 
  int res;  
  boolean was_removed;
  ds_util_itr_hndl_type itr_hndl;  
  uint32 real_size;

  if (0 == ds_util_list_get_size(hndl))
  {
    return DS_FAILURE;
  }

  if (ds_util_itr_get_hndl((ds_util_iterable_type *)hndl, &itr_hndl) != DS_SUCCESS)
  {
    return DS_FAILURE; 
  }

  if (ds_util_itr_first(itr_hndl) != DS_SUCCESS)
  {
    ds_util_itr_rel_hndl(itr_hndl);
    return DS_FAILURE;         
  }  

  real_size = ((ds_util_iterable_type*)itr_hndl)->s((void*)itr_hndl);      
  ds_util_itr_rel_hndl(itr_hndl);  

  if (info_size > real_size)
  {
    return DS_FAILURE;
  }
  
  res = ds_util_list_do_for_every_node(hndl, 
                                       func_remove_element, 
                                       TRUE,
                                       &was_removed,
                                       hndl, 
                                       info,
                                       info_size);

  if (DS_SUCCESS != res)
  {
    return DS_FAILURE;
  }

  if (!was_removed)
  {
    return DS_FAILURE;
  }
  
  return DS_SUCCESS;
} /* ds_util_list_remove() */

/* removes the particular node stored in list, rather then searching for info */
int ds_util_list_remove_list_node(
  ds_util_list_hndl_type  hndl,
  ds_util_list_node_type  *node_ptr
)
{
  list_pop_item( &(((ds_util_list_type *)hndl)->lst), 
                 &(node_ptr->link));

  if (node_ptr) 
  {
    if (node_ptr->data)
    {
      DS_PROFILE_MEM_FREE( (void *) node_ptr->data, MODEM_MEM_CLIENT_DATA );
      node_ptr->data = NULL;
    }
    DS_PROFILE_MEM_FREE( (void *) node_ptr, MODEM_MEM_CLIENT_DATA );
    node_ptr = NULL;
  }

  return 0;
}

/* creates a list in destination output param and copies source list to it */
/* !!! WARNING !!! 
   all elements in the list should be the same size */
int ds_util_list_copy(
  ds_util_list_hndl_type  source,
  ds_util_list_hndl_type*  destination /* <- */
) /* ds_util_list_copy() */ 
{
  int res;
  ds_util_itr_hndl_type itr_hndl = NULL;
  uint32 tmp_sz;  
  void* tmp_data = NULL;

  if ( ds_util_list_get_hndl( destination ) != DS_SUCCESS )
  {   
    return DS_FAILURE;
  }

  if (0 == ds_util_list_get_size(source))
  {
    return DS_SUCCESS;    
  }

  if (ds_util_itr_get_hndl((ds_util_iterable_type *)source, &itr_hndl) != DS_SUCCESS)
  {
    res = DS_FAILURE; 
    goto bail;
  }

  if (ds_util_itr_first(itr_hndl) != DS_SUCCESS)
  {
    res = DS_FAILURE; 
    goto bail;
  }  

  tmp_sz =((ds_util_iterable_type*)itr_hndl)->s((void*)itr_hndl); 
  tmp_data = (void *)DS_PROFILE_MEM_ALLOC( tmp_sz, MODEM_MEM_CLIENT_DATA );
  if (NULL == tmp_data)
  {
    res = DS_FAILURE;
    goto bail;
  }

  res = DS_SUCCESS;

  do
  {
    if (ds_util_itr_get_data(itr_hndl, tmp_data, &tmp_sz) != DS_SUCCESS)
    {
      res = DS_FAILURE;
      goto bail;
    }

    if (ds_util_list_add(*destination, tmp_data, tmp_sz) != DS_SUCCESS)
    {
      res = DS_FAILURE; 
      goto bail;
    }   
  }
  while (ds_util_itr_next(itr_hndl) == DS_SUCCESS);

  /* fall through */

bail:

  if (tmp_data)
  {
    DS_PROFILE_MEM_FREE( (void *)tmp_data, MODEM_MEM_CLIENT_DATA );
  }

  if (itr_hndl)
  {
    ds_util_itr_rel_hndl(itr_hndl);  
  }

  if (DS_SUCCESS != res)
  {
    ds_util_list_rel_hndl(*destination);
  }

  return res;
}

/* empties the list */
int ds_util_list_reset(
  ds_util_list_hndl_type  hndl
) /* ds_util_list_reset() */
{
  ds_util_list_type *l = ( ds_util_list_type * )hndl;
  ds_util_list_node_type *node;

  while ( ( node = list_pop_front( &l->lst ) ) != NULL )
  {
    DS_PROFILE_MEM_FREE( (void *) node->data, MODEM_MEM_CLIENT_DATA );
    DS_PROFILE_MEM_FREE( (void *) node, MODEM_MEM_CLIENT_DATA );
  }

  return DS_SUCCESS;
}

/*===========================================================================
FUNCTION ds_util_list_do_for_every_node
 
DESCRIPTION
  This function is used to iterate over the list and perform some action for 
  node(s) in it. 

PARAMETERS 
  hndl                  - list to iterate
  func                  - function to perform, with the first argument of pointer to 
                        - data in node and additional variadic number of argument
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
  int (*func)(void* data, va_list args),  
  boolean stop_on_first_success,  
  boolean* func_success,
  ...
)
{
  int res;
  int tmp_res;  
  ds_util_itr_hndl_type itr_hndl;
  uint32 tmp_sz;
  va_list args;
  void* tmp_data = NULL;
  
  if (func_success)
  {
    *func_success = FALSE;
  }

  if (0 == ds_util_list_get_size(hndl))
  {
    return DS_SUCCESS;    
  }

  if (ds_util_itr_get_hndl((ds_util_iterable_type *)hndl, &itr_hndl) != DS_SUCCESS)
  {
    return DS_FAILURE; 
  }

  if (ds_util_itr_first(itr_hndl) != DS_SUCCESS)
  {
    ds_util_itr_rel_hndl(itr_hndl);
    return DS_FAILURE;         
  }

  va_start(args, func_success);

  tmp_sz =((ds_util_iterable_type*)itr_hndl)->s((void*)itr_hndl);    

  res = DS_SUCCESS;
    
  do
  {
    /* get the data pointer inside the list, rather than copy it, this will allow operations 
       to be done on the list data inside the user function */
    if (ds_util_itr_get_data_in_list(itr_hndl, &tmp_data, &tmp_sz) != DS_SUCCESS)
    {
      res = DS_FAILURE;
      goto bail;
    }
    
    tmp_res = (*func)(tmp_data, args);

    /* function succeeded */
    if (!(tmp_res))
    {
      if (func_success)
      {
        *func_success = TRUE;
      }
      if (stop_on_first_success)
      {
        goto bail;
      }           
    }

  }
  while (ds_util_itr_next(itr_hndl) == DS_SUCCESS);
  
  /* fall through */

bail:

  ds_util_itr_rel_hndl(itr_hndl);
  va_end(args);

  if (DS_SUCCESS != res)
  {
    if (func_success)
    {
      *func_success = FALSE;
    }
  }

  return res;
} /* ds_util_list_do_for_every_node() */

/*==================================================================================*/

uint32 ds_util_list_get_size(
  ds_util_list_hndl_type hndl
)
{
  list_size_type sz;
  ds_util_list_type *l = ( ds_util_list_type * )hndl;

  sz = list_size( &l->lst );

  return (uint32) sz;
}

/*==================================================================================*/

int ds_util_list_rel_hndl
(
  ds_util_list_hndl_type hndl
)
{
  ds_util_list_type *l = ( ds_util_list_type * )hndl;
  ds_util_list_node_type *node;

  while ( ( node = list_pop_front( &l->lst ) ) != NULL )
  {
    DS_PROFILE_MEM_FREE( (void *) node->data, MODEM_MEM_CLIENT_DATA );
    DS_PROFILE_MEM_FREE( (void *) node, MODEM_MEM_CLIENT_DATA );
  }

  list_destroy( &l->lst );
  DS_PROFILE_MEM_FREE( (void *) l, MODEM_MEM_CLIENT_DATA );

  return DS_SUCCESS;
}


/*---------------------------------------------------------------------------
               Implementation of Iterator
---------------------------------------------------------------------------*/
int ds_util_itr_get_hndl(
  ds_util_iterable_type *obj, /* -> */
  ds_util_itr_hndl_type *hndl /* <- */
)
{

  //PLM_LOGD( "_itr_get_hndl: ENTRY", 0 );

  if ( ( obj == NULL ) || ( hndl == NULL ) )
  {
    //PLM_LOGE( "invalid arg", 0 );
    return DS_FAILURE;
  }

  if ( ( obj->n == NULL ) || ( obj->d == NULL ) ||
       ( obj->s == NULL ) || ( obj->f == NULL ) )
  {
    return DS_FAILURE;
  }

  *hndl = (ds_util_itr_hndl_type *) obj;
  //PLM_LOGD( "_itr_get_hndl: EXIT", 0 );
  return DS_SUCCESS;

}/* dsi_profile_utils_get_itr() */


int ds_util_itr_next(
  ds_util_itr_hndl_type   hndl
)
{
  ds_util_iterable_type *itr_obj = ( ds_util_iterable_type * )hndl;
  return itr_obj->n( (void *) itr_obj );
}/* ds_util_itr_next() */

int ds_util_itr_first(
  ds_util_itr_hndl_type   hndl
)
{
  ds_util_iterable_type *itr_obj = ( ds_util_iterable_type * )hndl;
  itr_obj->f( (void *) itr_obj );
  return DS_SUCCESS;
}


int ds_util_itr_get_data(
  ds_util_itr_hndl_type   hndl,
  void                   *obj,  /* <- */
  uint32                 *obj_size  /* <-> */
)
{
  uint32 tmp_sz = 0;
  ds_util_iterable_type *itr_obj = ( ds_util_iterable_type * )hndl;

  /* */
  void *data_obj = itr_obj->i( (void *) hndl );
  tmp_sz         = itr_obj->s( (void *) hndl );

  if ( data_obj == NULL )
  {
    return DS_FAILURE;
  }

  if ( tmp_sz > *obj_size || tmp_sz == 0 )
  {
    return DS_FAILURE;
  }
  memscpy( obj, tmp_sz, data_obj, tmp_sz );
  *obj_size = tmp_sz;
  return DS_SUCCESS;
}/* ds_util_itr_get_data() */

/* same as above but returns the pointer inside the list, rather than copy it */
int ds_util_itr_get_data_in_list(
  ds_util_itr_hndl_type   hndl,
  void                    **obj,  /* <- */
  uint32                 *obj_size  /* <-> */
)
{
  uint32 tmp_sz = 0;
  ds_util_iterable_type *itr_obj = ( ds_util_iterable_type * )hndl;

  /* */
  void *data_obj = itr_obj->i( (void *) hndl );
  tmp_sz         = itr_obj->s( (void *) hndl );

  if ( data_obj == NULL )
  {
    return DS_FAILURE;
  }

  if ( tmp_sz > *obj_size || tmp_sz == 0 )
  {
    return DS_FAILURE;
  }

  *obj = data_obj;

  *obj_size = tmp_sz;
  return DS_SUCCESS;
}/* ds_util_itr_get_data_in_list() */

int ds_util_itr_rel_hndl(
   ds_util_itr_hndl_type  hndl
)
{
  ds_util_iterable_type *itr_obj = ( ds_util_iterable_type * )hndl;
  itr_obj->d( (void *) itr_obj );
  return DS_SUCCESS;
}/*ds_util_itr_destroy()*/
