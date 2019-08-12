/*==============================================================================
@file npa_remote_qdi_drv.c

NPA QDI Remote driver functions

Copyright (c) 2012-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.mpss/3.5.c12.3/power/npa/src/remote/npa_remote_qdi_drv.c#1 $
==============================================================================*/
#include <string.h>
#include <qurt_qdi_driver.h>
#include "npa_internal.h"
#include "npa_remote_publish_resource.h"
#include "npa_remote_qdi_protocol.h"

#ifdef PD_IMAGE_NAME
/* TODO: change this to a QuRT macro, if available */
#define NPA_QDI_PD_NAME_MAX 64
static char npa_qdi_pd_image_name[NPA_QDI_PD_IMAGE_NAME_MAX];
#endif /* PD_IMAGE_NAME*/

typedef struct
{
  qurt_qdi_obj_t base;
  union
  {
  npa_client_handle client;
    npa_query_handle  query;
  } local;
} npa_qdi_object;

void npa_qdi_client_release( npa_qdi_object *obj )
{
  npa_destroy_client( obj->local.client );
  Core_Free( obj );
}

void npa_qdi_query_release( npa_qdi_object *obj )
{
  npa_destroy_query_handle( obj->local.query );
  Core_Free( obj );
}

int npa_qdi_handle_invoke( int client_handle, qurt_qdi_obj_t *ptr, int method,
                          qurt_qdi_arg_t a1, qurt_qdi_arg_t a2,
                          qurt_qdi_arg_t a3, qurt_qdi_arg_t a4,
                          qurt_qdi_arg_t a5, qurt_qdi_arg_t a6,
                          qurt_qdi_arg_t a7, qurt_qdi_arg_t a8,
                          qurt_qdi_arg_t a9)
{
  npa_qdi_object *obj = (npa_qdi_object *)ptr;
  CORE_VERIFY_PTR( obj );
  
  switch ( method )
  {  
  case NPA_QDI_ISSUE_SCALAR_REQUEST:
  {
    npa_issue_scalar_request( obj->local.client, (npa_resource_state)a1.num );
    return 0;
  }
   
  case NPA_QDI_ISSUE_LIMIT_MAX_REQUEST:
  {
    npa_issue_limit_max_request( obj->local.client, (npa_resource_state)a1.num );
    return 0;
  }

  case NPA_QDI_QUERY:
  {
    int err;
    npa_query_type result = { 0 };
    npa_query_status ret = npa_query( obj->local.query, 
                                      (uint32)a1.num, &result );

    err = qurt_qdi_copy_to_user( client_handle, a2.ptr, 
                                 &result, sizeof(npa_query_type) );
    if ( err < 0 ) { return -1; }
    
    return (int)ret;
  }
   
  default:
    return qurt_qdi_method_default( client_handle, ptr, method,
                                    a1, a2, a3, a4, a5, a6, a7, a8, a9 );
  } /* switch */
}

int npa_qdi_base_invoke( int client_handle, qurt_qdi_obj_t *ptr, int method,
                         qurt_qdi_arg_t a1, qurt_qdi_arg_t a2,
                         qurt_qdi_arg_t a3, qurt_qdi_arg_t a4,
                         qurt_qdi_arg_t a5, qurt_qdi_arg_t a6,
                         qurt_qdi_arg_t a7, qurt_qdi_arg_t a8,
                         qurt_qdi_arg_t a9 )
{
  int err;
  
   switch (method)
   {
   case QDI_OPEN:
     {
       return qurt_qdi_new_handle_from_obj_t(client_handle, ptr);
     }

   case NPA_QDI_GET_PUBLISHED_RESOURCE_COUNT:
     {
       npa_remote_domain_id domain_id = (npa_remote_domain_id)a1.num;
       unsigned int count = 0;
    npa_remote_published_resource *pubr = npa_remote_published_resource_list;

    while ( pubr )
         {
      if ( domain_id & pubr->remote_domains ) { ++count; }
      pubr = pubr->next;
         }
     
    err = qurt_qdi_copy_to_user( client_handle, a2.ptr, &count, sizeof(count) );
    if ( err < 0 ) { return -1; }

         return 0;
       }

   case NPA_QDI_GET_PUBLISHED_RESOURCE_DEFINITION:
     {
       npa_remote_domain_id domain_id = (npa_remote_domain_id)a1.num;
    unsigned int i = 0, namelen, count = (unsigned int)a2.num;
    npa_remote_resource_definition *definition = 
                        (npa_remote_resource_definition *)a3.ptr;
    npa_remote_published_resource *pubr = npa_remote_published_resource_list;

    if ( count != 0 )
       {
      while ( pubr )
       {
        if ( domain_id & pubr->remote_domains )
         {
          npa_resource *resource;
          
           if(i >= count)
           {
            /* More resources published to this domain than expected */
            ULOG_RT_PRINTF_2( npa.log, 
                              "WARN: There are more than %d resources "
                              "published to domain %u", count, domain_id);
            return 0;
           }
          
          namelen = strlen( pubr->local_link->name );
          CORE_VERIFY( namelen <= NPA_MAX_STR_LEN );
          err = qurt_qdi_copy_to_user( client_handle, 
                                       definition[i].remote_resource_name,
                                       pubr->local_link->name,
                                       namelen+1 );
          if ( err < 0 ) { return -1; }
                                       
          resource = pubr->local_link->resource;
          
          namelen = strlen( resource->definition->units );
          CORE_VERIFY( namelen <= NPA_QDI_REMOTE_MAX_RESOURCE_UNITS_LEN );
          err = qurt_qdi_copy_to_user( client_handle,
                                       (void *)definition[i].units,
                                       resource->definition->units,
                                       namelen+1 );
          if ( err < 0 ) { return -1; }

          err = qurt_qdi_copy_to_user( client_handle,
                                       &definition[i].max,
                                       &resource->definition->max,
                                       sizeof(npa_resource_state) );
          if ( err < 0 ) { return -1; }
                                       
          err = qurt_qdi_copy_to_user( client_handle,
                                       &definition[i].attributes,
                                       &resource->definition->attributes,
                                       sizeof(unsigned int) );
          if ( err < 0 ) { return -1; }
                                                 
           i++;
         }
        pubr = pubr->next;
       }
       if(i != count)
       {
         ULOG_RT_PRINTF_1(npa.log, 
                          "ERROR: There are less resources published to "
                          "domain %u than expected", domain_id );
         return -1;
       }
    }
       return 0;
     }
   
   case NPA_QDI_CREATE_SYNC_CLIENT_EX:
     {
    npa_client *client = npa_create_sync_client_ex( a1.ptr, a2.ptr, 
                                          (npa_client_type)a3.num,
                                          (unsigned int)a4.num,
                                           a5.ptr);

    if ( client )
    { /* Create a NPA QDI object for the client and return the QDI handle
       * associated with this object to caller. QURT will call the object's
       * invoke routine when the guest PD calls methods on the handle and the
       * release routine when the guest PD shuts down or restarts.
     */
      int qh;
      npa_qdi_object *obj = Core_Malloc( sizeof(npa_qdi_object) );
      CORE_VERIFY_PTR( obj );

      /* Set up */
      obj->base.invoke = (qurt_qdi_pfn_invoke_t) npa_qdi_handle_invoke;
      obj->base.refcnt = QDI_REFCNT_INIT;
      obj->base.release = (qurt_qdi_pfn_release_t) npa_qdi_client_release;
      obj->local.client = client;

      /* Create a QDI handle from the object */
      qh = qurt_qdi_handle_create_from_obj_t( client_handle, &obj->base );
      CORE_VERIFY( qh >= 0 );

      /* Return handle to caller */
      err = qurt_qdi_copy_to_user( client_handle, a6.ptr, &qh, sizeof(qh) );
      if ( err >= 0 )
      {
         return 0;
       }
    }
       return -1;
     }
  
  case NPA_QDI_CREATE_QUERY_HANDLE:
     {
    npa_query_handle query = npa_create_query_handle( a1.ptr );
    
    if ( query )
    { /* Create a NPA QDI object for the query handle and return the QDI handle
       * associated with this object to caller. QURT will call the object's
       * invoke routine when the guest PD calls methods on the handle and the
       * release routine when the guest PD shuts down or restarts.
       */
      int qh;
      npa_qdi_object *obj = Core_Malloc( sizeof(npa_qdi_object) );
      CORE_VERIFY_PTR( obj );

      /* Set up */
      obj->base.invoke = (qurt_qdi_pfn_invoke_t) npa_qdi_handle_invoke;
      obj->base.refcnt = QDI_REFCNT_INIT;
      obj->base.release = (qurt_qdi_pfn_release_t) npa_qdi_query_release;
      obj->local.query = query;
   
      /* Create a QDI handle from the object */
      qh = qurt_qdi_handle_create_from_obj_t( client_handle, &obj->base );
      CORE_VERIFY( qh >= 0 );
   
      /* Return handle to caller */
      err = qurt_qdi_copy_to_user( client_handle, a2.ptr, &qh, sizeof(qh) );
      if ( err >= 0 )
     {
       return 0;
     }
     }
    return -1;
     }
   
   default:
       return qurt_qdi_method_default(client_handle, ptr, method,
                                      a1, a2, a3, a4, a5, a6, a7, a8, a9);
  } /* switch */
}

static const struct
{
   qurt_qdi_obj_t qdiobj;
} npa_qdi =
   {
  { npa_qdi_base_invoke, QDI_REFCNT_PERM, 0 }
};

void npa_qdi_init(void)
{
#ifdef PD_IMAGE_NAME
  strlcpy( npa_qdi_pd_image_name, PD_IMAGE_NAME, NPA_QDI_PD_IMAGE_NAME_MAX );
#endif  

  CORE_VERIFY( 0 == qurt_qdi_register_devname( "/dev/npa", (void *)&npa_qdi ) );
}
