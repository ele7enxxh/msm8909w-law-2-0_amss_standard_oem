/*
  Copyright (C) 2009-2012, 2013 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/apr/osal/variant/qurt/src/apr_thread.c#1 $
  $Author: mplcsds1 $
*/

#include <stddef.h>
#include "mmstd.h"
#include "DALSys.h"
#include "qurt.h"
#include "qurt_error.h"
#include "apr_errcodes.h"
#include "apr_thread.h"
#include "apr_api.h"

#define APR_THREAD_STACK_SIZE ( 8192 )
#define APR_THREAD_NAME_SIZE ( 12 + 1 ) /* Last byte reserved for NULL. */

typedef struct apr_thread_internal_t
{
  unsigned int handle;
  char_t name[ APR_THREAD_NAME_SIZE ];
  uint8_t stack[ APR_THREAD_STACK_SIZE ];
  apr_thread_fn_t fn;
  void* param;
  int32_t result;
}
  apr_thread_internal_t;

static void apr_thread_wrapper (
  void* param
)
{
  apr_thread_internal_t* the_thread = ( ( apr_thread_internal_t* ) param );

  if ( the_thread != NULL )
  {
    the_thread->result = the_thread->fn( the_thread->param );
  }

  qurt_thread_exit( 0 );
}

APR_INTERNAL int32_t apr_thread_create (
  apr_thread_t* ret_thread,
  char_t* name,
  uint32_t priority,
  uint8_t* stack_base, /* TODO */
  uint32_t stack_size, /* TODO */
  apr_thread_fn_t thread_fn,
  void* param
)
{
  typedef void (*thread_handler_func_t) (void *);
  int cleanup_errors = 0;
  apr_thread_internal_t* the_thread;
  DALResult dal_rc;
  void * pStack;
  void * pStackAlligned;
  unsigned int alligned_stack_size;
  qurt_thread_attr_t attr;
  qurt_thread_t tid;
  int rc;

  if ( ret_thread == NULL )
  {
    return APR_EBADPARAM;
  }

  dal_rc = DALSYS_Malloc( sizeof( apr_thread_internal_t ), ( ( void** ) &the_thread ) );
  if ( dal_rc != DAL_SUCCESS )
  {
    return APR_ENORESOURCE;
  }

  for ( ;; )
  {
    ( void ) mmstd_memset( the_thread, 0, sizeof( apr_thread_internal_t ) );

    if ( name )
    {
      ( void ) mmstd_strlcpy( the_thread->name, name, APR_THREAD_NAME_SIZE );
    }
    else
    {
      ( void ) mmstd_strlcpy( the_thread->name, "AUDIO", APR_THREAD_NAME_SIZE );
    }

    the_thread->fn = thread_fn;
    the_thread->param = param;
    the_thread->result = APR_EOK;

    /* Align the stack pointer. */
    pStack = ( void* )( &the_thread->stack );
    pStackAlligned = ( void* )( (uint32_t)( ( ( uint32_t ) pStack ) + 7) & (~7) );
    /* Calculate alligned stack size. */
    alligned_stack_size = ( unsigned int ) ( APR_THREAD_STACK_SIZE - ( ( ( uint32_t ) pStackAlligned ) - ( ( uint32_t ) pStack ) ) );
    alligned_stack_size -= alligned_stack_size % 8;

    qurt_thread_attr_init (&attr);
    qurt_thread_attr_set_stack_size( &attr, alligned_stack_size );
    qurt_thread_attr_set_stack_addr( &attr, pStackAlligned );
    qurt_thread_attr_set_priority ( &attr, ( unsigned short ) priority );
    qurt_thread_attr_set_name( &attr, the_thread->name );

    rc = qurt_thread_create(
            &tid,                                           /* thread ID */
            &attr,                                          /* thread attributes */
            ( thread_handler_func_t ) apr_thread_wrapper,   /* thread entry function */
            (void *) the_thread                             /* thread argument */
          );

    if ( rc == QURT_EFAILED )
    {
      break;
    }

    the_thread->handle = tid;

    *ret_thread = ( ( apr_thread_t* ) the_thread );

    return APR_EOK;
  }

  /* Perform error clean-up. */
  dal_rc = DALSYS_Free( the_thread );
  if ( dal_rc != DAL_SUCCESS ) cleanup_errors |= 1;

  return ( cleanup_errors ? APR_EPANIC : APR_EFAILED );
}

APR_INTERNAL int32_t apr_thread_destroy (
  apr_thread_t thread
)
{
  int rc , thrd_join_status = 0 , cleanup_errors = 0;
  apr_thread_internal_t* the_thread = ( ( apr_thread_internal_t* ) thread );
  DALResult dal_rc;

  if ( the_thread == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = qurt_thread_join( the_thread->handle, &thrd_join_status );

  if ( rc != QURT_ENOTHREAD && rc != QURT_EOK ) cleanup_errors |= 1;
  if ( the_thread->result ) cleanup_errors |= 2;

  dal_rc = DALSYS_Free( the_thread );
  if ( dal_rc != DAL_SUCCESS ) cleanup_errors |= 4;

  return ( cleanup_errors ? APR_EPANIC : rc );
}

