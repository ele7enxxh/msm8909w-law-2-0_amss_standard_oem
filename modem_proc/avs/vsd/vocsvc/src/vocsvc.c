/*
  Copyright (C) 2013-2014 QUALCOMM Technologies, Inc.
  All rights reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/vocsvc/src/vocsvc.c#1 $
  $Author: mplcsds1 $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "msg.h"

#include "apr_errcodes.h"
#include "apr_event.h"
#include "apr_thread.h"
#include "apr_misc.h"

#include "mvs_module_api.h"
#include "vocsvc_nv_api.h"
#include "drv_api.h"
#include "voicecfg_api.h"
#include "mvs_diag_api.h"

#include "vocsvc.h"
#include "vs_api.h"
#include "vs_common.h"
#include "voicemem.h"
#include "mva_api.h"

/****************************************************************************
 * GLOBALS                                                                  *
 ****************************************************************************/

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

#define VOCSVC_SERVICE_TASK_NAME ( "VOCSVC" )
#define VOCSVC_SERVICE_TASK_PRIORITY ( 166 )
  /* Voice Services is created to run services each time its awoken. The
   * services should avoid blocking.
   */

/*****************************************************************************
 * DEFINITIONS                                                               *
 ****************************************************************************/

typedef enum vocsvc_thread_state_enum_t
{
  VOCSVC_THREAD_STATE_ENUM_INIT,
  VOCSVC_THREAD_STATE_ENUM_READY,
  VOCSVC_THREAD_STATE_ENUM_EXIT
}
  vocsvc_thread_state_enum_t;

/****************************************************************************
 * VARIABLE DECLARATIONS                                                    *
 ****************************************************************************/

static apr_event_t vocsvc_event_handle;
static apr_thread_t vocsvc_task_handle;
static vocsvc_thread_state_enum_t vocsvc_task_state = VOCSVC_THREAD_STATE_ENUM_INIT;

/****************************************************************************
 * VOICE SERVICES TASK                                                      *
 ****************************************************************************/

APR_INTERNAL void vocsvc_signal_task ( void )
{
  ( void ) apr_event_signal( vocsvc_event_handle );
}

static int32_t vocsvc_service_task ( void* param )
{
  int32_t rc;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "Entering voice services task" );

  rc = apr_event_create( &vocsvc_event_handle );
  if ( rc )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "Exiting voice services task due " \
          "to failure" );
    return APR_EFAILED;
  }

  vocsvc_task_state = VOCSVC_THREAD_STATE_ENUM_READY;

  do
  {
    rc = apr_event_wait( vocsvc_event_handle );

    /* Run services. */
  }
  while ( rc == APR_EOK );

  ( void ) apr_event_destroy( vocsvc_event_handle );
  vocsvc_task_state = VOCSVC_THREAD_STATE_ENUM_EXIT;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "Exiting voice services task" );

  return APR_EOK;
}

/****************************************************************************
 * VOICE SERVICES INITIALIZATION ROUTINES                                   *
 ****************************************************************************/

static int32_t vocsvc_init ( void )
{
  { /* Bring up the service task. */
    ( void ) apr_thread_create(
               &vocsvc_task_handle, VOCSVC_SERVICE_TASK_NAME,
               VOCSVC_SERVICE_TASK_PRIORITY, NULL, 0, vocsvc_service_task,
               NULL );

    while ( vocsvc_task_state != VOCSVC_THREAD_STATE_ENUM_READY )
    {
      ( void ) apr_misc_sleep( 1000000 ); /* Sleep for 1ms. */
    }
  }

  { /* Initialize the drivers and services. */
    ( void ) voicecfg_call( DRV_CMDID_INIT, NULL, 0 );
    ( void ) vocsvc_nv_call( VOCSVC_NV_CALLINDEX_ENUM_INIT, NULL, 0 );
    ( void ) voicemem_call( VOICEMEM_CMD_INIT, NULL, 0 );
    ( void ) vs_call( VS_CALLINDEX_ENUM_INIT, NULL, 0 );
    ( void ) mvs_call( MVS_CALLINDEX_ENUM_INIT, NULL, 0 );
    ( void ) mvs_diag_call( MVS_DIAG_CALLINDEX_ENUM_INIT, NULL, 0 );
    ( void ) vocsvc_cm_call( VOCSVC_CALLINDEX_ENUM_INIT, NULL, 0 );
    ( void ) mva_call( DRV_CMDID_INIT, NULL, 0 );
  }

  return APR_EOK;
}

static int32_t vocsvc_postinit ( void )
{
  { /* Post-initialize drivers and services. */
    ( void ) voicecfg_call( DRV_CMDID_POSTINIT, NULL, 0 );
    ( void ) vocsvc_nv_call( VOCSVC_NV_CALLINDEX_ENUM_POSTINIT, NULL, 0 );
    ( void ) vs_call( VS_CALLINDEX_ENUM_POSTINIT, NULL, 0 );
    ( void ) mvs_call( MVS_CALLINDEX_ENUM_POSTINIT, NULL, 0 );
    ( void ) mvs_diag_call( MVS_DIAG_CALLINDEX_ENUM_POSTINIT, NULL, 0 );
    ( void ) mva_call( DRV_CMDID_POSTINIT, NULL, 0 );
  }

  return APR_EOK;
}

static int32_t vocsvc_predeinit ( void )
{
  { /* Pre-deinitialize drivers and services (in reverse order of initialization). */
    ( void ) mva_call( DRV_CMDID_PREDEINIT, NULL, 0 );
    ( void ) mvs_diag_call( MVS_DIAG_CALLINDEX_ENUM_PREDEINIT, NULL, 0 );
    ( void ) mvs_call( MVS_CALLINDEX_ENUM_PREDEINIT, NULL, 0 );
    ( void ) vocsvc_nv_call( VOCSVC_NV_CALLINDEX_ENUM_PREDEINIT, NULL, 0 );
    ( void ) vs_call( VS_CALLINDEX_ENUM_PREDEINIT, NULL, 0 );
    ( void ) voicecfg_call( DRV_CMDID_PREDEINIT, NULL, 0 );
  }

  return APR_EOK;
}

static int32_t vocsvc_deinit ( void )
{
  { /* De-initialize drivers and services (in reverse order of initialization). */
    ( void ) mva_call( DRV_CMDID_DEINIT, NULL, 0 );
    ( void ) vocsvc_cm_call( VOCSVC_CALLINDEX_ENUM_DEINIT, NULL, 0 );
    ( void ) mvs_diag_call( MVS_DIAG_CALLINDEX_ENUM_DEINIT, NULL, 0 );
    ( void ) mvs_call( MVS_CALLINDEX_ENUM_DEINIT, NULL, 0 );
    ( void ) voicemem_call( VOICEMEM_CMD_DEINIT, NULL, 0 );
    ( void ) vocsvc_nv_call( VOCSVC_NV_CALLINDEX_ENUM_DEINIT, NULL, 0 );
    ( void ) vs_call( VS_CALLINDEX_ENUM_DEINIT, NULL, 0 );
    ( void ) voicecfg_call( DRV_CMDID_DEINIT, NULL, 0 );
  }

  { /* Trigger the service task to shut down. */
    ( void ) apr_event_signal_abortall( vocsvc_event_handle );

    while ( vocsvc_task_state != VOCSVC_THREAD_STATE_ENUM_EXIT )
    {
      ( void ) apr_misc_sleep( 1000000 ); /* Sleep for 1ms. */
    }

    ( void ) apr_thread_destroy( vocsvc_task_handle );
  }

  return APR_EOK;
}

/*===========================================================================

FUNCTION vocsvc_call

DESCRIPTION
  This procedure provides a single entry-point into the voice services.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
APR_EXTERNAL int32_t vocsvc_call (
  vocsvc_callindex_enum_t index,
  void* params,
  uint32_t size
)
{
  int32_t rc;

  switch ( index )
  {
  case VOCSVC_CALLINDEX_ENUM_INIT:
    rc = vocsvc_init( );
    break;

  case VOCSVC_CALLINDEX_ENUM_POSTINIT:
    rc = vocsvc_postinit( );
    break;

  case VOCSVC_CALLINDEX_ENUM_PREDEINIT:
    rc = vocsvc_predeinit( );
    break;

  case VOCSVC_CALLINDEX_ENUM_DEINIT:
    rc = vocsvc_deinit( );
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "Unsupported callindex (%d)",
            index );
    rc = APR_EUNSUPPORTED;
    break;
  }

  return rc;
}

