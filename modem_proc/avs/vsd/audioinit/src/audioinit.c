/*===========================================================================

                    A U D I O   I N I T   S E R V I C E S

DESCRIPTION
  This header file defines APIs for initializing all audio-related modules.

REFERENCES
  None.

Copyright (C) 2008 - 2015 QUALCOMM Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/audioinit/src/audioinit.c#1 $
$DateTime: 2016/12/13 07:59:59 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/02/14   aim     Moved apr_init to RCinit framework.
12/10/13    qc     Removed audio deinitialization (CR584966).
06/01/13   sud     Fixed compiler warnings.
04/30/12    qc     Register audio initialization to RCINIT framework.
07/28/11    hl     Clear received rex signals in audioinit_task. Otherwise, audio_init task may get the same signal
                   multiple times. Remove the wait for TASK_OFFLINE_SIG because audio_init task does not need to 
                   process any modem offline changes.
12/14/10    lg     Moved audioinit_rpt_timer initialization under #if 0
12/13/10    lg     Included tmc.h to fix compilation error on MDM9k.
01/29/10   act     Adding ftm_audio_init().
07/25/08   ymc     Initial revision.

===========================================================================*/

#include "rcinit.h"
#include "rcevt_sigosal.h"
#include "msg.h"

#include "DALSys.h"

#include "apr_api.h"
#include "apr_errcodes.h"

#include "vocsvc.h"
#include "audioinit.h"

#include "toyserver_api.h"

/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

int32_t audio_init ( void )
{
  { /* Initialize DAL. */
    DALSYSConfig dalsyscfg = { NULL, DALREMOTING_CLIENTS_SERVERS, NULL };
    DALSYS_InitMod( &dalsyscfg );
  }
  { /* Initialize the drivers and services. */
    ( void ) vocsvc_call( VOCSVC_CALLINDEX_ENUM_INIT, NULL, 0 );
  }

  return APR_EOK;
}

int32_t audio_postinit ( void )
{
  { /* Post-initialize the drivers and services. */
    ( void ) vocsvc_call( VOCSVC_CALLINDEX_ENUM_POSTINIT, NULL, 0 );
  }

  return APR_EOK;
}

int32_t audio_pre_deinit ( void )
{
  { /* Pre-deinitialize drivers and services (in reverse order of initialization). */
    ( void ) vocsvc_call( VOCSVC_CALLINDEX_ENUM_PREDEINIT, NULL, 0 );
  }

  return APR_EOK;
}

int32_t audio_deinit ( void )
{
  { /* De-initialize drivers and services (in reverse order of initialization). */
    ( void ) vocsvc_call( VOCSVC_CALLINDEX_ENUM_DEINIT, NULL, 0 );
  }
  { /* De-initialize APR. */
    ( void ) apr_deinit( );
  }

  return APR_EOK;
}

void audioinit_task
(
  dword ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
  ( void ) audioinit_call( AUDIOINIT_CALLINDEX_ENUM_INIT_LIBS_AFTER_SERVICES_STARTED, NULL, 0 );

  return;
}

int32_t audioinit_call (
  audioinit_callindex_enum_t index,
  void* params,
  uint32_t size
)
{
  int32_t rc = APR_EOK;

  switch ( index )
  {
  case AUDIOINIT_CALLINDEX_ENUM_INIT_LIBS_BEFORE_TASKS_START:
    rc = APR_EOK;
    break;

  case AUDIOINIT_CALLINDEX_ENUM_INIT_LIBS_BEFORE_SERVICES_START:
    rc = APR_EOK;
    break;

  case AUDIOINIT_CALLINDEX_ENUM_INIT_LIBS_AFTER_SERVICES_STARTED:
    ( void ) audio_init( );
    ( void ) audio_postinit( );
    rc = APR_EOK;
    break;

  case AUDIOINIT_CALLINDEX_ENUM_INIT_LIBS_AFTER_TASKS_STARTED:
    break;

  case AUDIOINIT_CALLINDEX_ENUM_PREDEINIT:
    rc = audio_pre_deinit( );
    break;

  case AUDIOINIT_CALLINDEX_ENUM_DEINIT:
    rc = audio_deinit();
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "Unsupported callindex (%d)",
            index );
    rc = APR_EUNSUPPORTED;
    break;
  }

  return rc;
}

