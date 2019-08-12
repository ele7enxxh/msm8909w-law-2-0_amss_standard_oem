/**
  @file vs_vocg711mulaw.c
  @brief This file contains the definitions of the interfaces for G.711 MU-LAW vocoder.
*/

/*
  ============================================================================

   Copyright (C) 2013-2015 QUALCOMM Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/vs/src/vs_vocg711mulaw.c#1 $
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  05/22/14  aim   Fixed a bug in applying vocoder properties.
  07/30/14  shr   Initial revision

  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "stdlib.h"
#include "msg.h"
#include "vs.h"
#include "vs_i.h"  /*internal data structures*/


/****************************************************************************
 * EXTERN DECLARATIONS                                                      *
 ****************************************************************************/
extern uint16_t vs_my_addr;
extern uint16_t vs_cvs_addr;
extern uint32_t vs_apr_handle;
extern apr_memmgr_type vs_heapmgr;

/****************************************************************************
 * FUNCTIONS                                                                *
 ****************************************************************************/

/**
 * Sets the Discontinuous Transmission (DTX) mode of the G711MU vocoder.
 *
 * VS_EOK (0) when successful.
 */
VS_INTERNAL uint32_t vs_vocg711mulaw_cmd_set_dtx_mode (
  vs_vocg711mulaw_cmd_set_dtx_mode_t* params
)
{
  uint32_t rc = VS_EOK;
  vs_vocg711mulaw_cmd_set_dtx_mode_t args;
  vs_session_object_t* p_session_obj = NULL;

  for ( ;; )
  {
    if ( params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_vocg711mulaw_cmd_set_dtx_mode() - NULL params" );
      rc = VS_EBADPARAM;
      break;
    }

    /* get the session object based on handle*/
    rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc != VS_EOK )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_vocg711mulaw_cmd_set_dtx_mode() - Invalid handle=0x%08x",
             params->handle );
      rc = VS_EBADPARAM;
      break;
    }

    args.handle = params->handle;
    args.enable_flag = params->enable_flag;

    rc = vs_prepare_and_dispatch_cmd_packet ( VS_THREAD_PRIORITY_ENUM_MED,
                                              VS_VOCG711MULAW_CMD_SET_DTX_MODE,
                                              &args, sizeof( args ) );
    break;
  }  /* end of for ( ;; ) */

  return rc;
}    /* end of vs_vocg711mulaw_cmd_set_dtx_mode () */

VS_INTERNAL uint32_t vs_vocg711mulaw_cmd_set_dtx_mode_proc (
  uint32_t handle,
  bool_t dtx_enable
)
{
  uint32_t rc = APR_EOK;
  vss_istream_cmd_set_enc_dtx_mode_t g711mulaw_dtx;
  vs_session_object_t* p_session_obj = NULL;
  uint32_t current_voc_dtx_enable;
  bool_t stream_ready;
  uint32_t media_id;
  uint32_t pending_media_id;

  for ( ;; )
  {
    /* Get the Session Object based on the handle */
    rc = vs_get_typed_object( handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    VS_ACQUIRE_LOCK( p_session_obj->common_lock );
    current_voc_dtx_enable = p_session_obj->current_voc.g711mulaw.dtx_enable;
    stream_ready = p_session_obj->stream_ready;
    media_id = p_session_obj->media_id;
    pending_media_id = p_session_obj->pending_media_id;
    VS_RELEASE_LOCK( p_session_obj->common_lock );

    if( current_voc_dtx_enable == (uint32_t) dtx_enable )
    {
      /* Invalidate pending_voc since newly set voc properties
       * are identical to current_voc properties.
       */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.g711mulaw.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      VS_RELEASE_LOCK( p_session_obj->common_lock );

      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_vocg711mulaw_cmd_set_dtx_mode_proc(): no change in dtx_mode %d",
             dtx_enable );
    }

    /* Cache and send to CVD immediately when in RUN state and media_id is
     * VS_COMMON_MEDIA_ID_G711MULAW.
     * If both conditions are NOT true, cache the value and apply while                    .
     * enabling the stream.
     */
    if ( ( stream_ready == TRUE ) &&
         ( media_id == VS_COMMON_MEDIA_ID_G711MULAW ) &&
         ( pending_media_id == VS_COMMON_MEDIA_ID_G711MULAW ) )
    {

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_vocg711mulaw_cmd_set_dtx_mode_proc()-  dtx_mode %d set, previous "
             "mode %d", dtx_enable, current_voc_dtx_enable );

      /* update pending_voc and current_voc */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.g711mulaw.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->current_voc.g711mulaw.dtx_enable = ( uint32_t )dtx_enable;

      g711mulaw_dtx.enable = p_session_obj->current_voc.g711mulaw.dtx_enable;
      VS_RELEASE_LOCK( p_session_obj->common_lock );

      /* apply the dtx_mode */
      rc = __aprv2_cmd_alloc_send(
             vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             vs_my_addr, ( (uint16_t) p_session_obj->header.handle ),
             vs_cvs_addr, p_session_obj->cvs_port,
             p_session_obj->header.handle, VSS_ISTREAM_CMD_SET_ENC_DTX_MODE,
             &g711mulaw_dtx, sizeof( g711mulaw_dtx ) );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                "vs_vocg711mulaw_cmd_set_dtx_mode_proc(): Failed to communicate with "
                "CVS with rc=0x%08x", rc );
      }
    }
    else
    {
      /* Cache the dtx_mode now.
       * Apply the dtx_mode while enabling the stream
       */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.g711mulaw.dtx_enable = ( uint32_t )dtx_enable;
      VS_RELEASE_LOCK( p_session_obj->common_lock );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_vocg711mulaw_cmd_set_dtx_mode_proc(): dtx_mode saved=%d, "
             "current dtx_mode=%d", dtx_enable,
             current_voc_dtx_enable );
    }  /* end of if ( session_obj->state == VS_VOC_STATE_ENUM_RUN ) */

    break;

  }  /* end of for ( ;; ) */

  return rc;
}

VS_INTERNAL uint32_t vs_vocg711mulaw_set_cached_voc_property (
  uint32_t handle
)
{
  uint32_t rc = APR_EOK;
  vs_session_object_t* p_session_obj = NULL;
  vss_istream_cmd_set_enc_dtx_mode_t g711mulaw_dtx;

    /* set cached g711mulaw dtx mode */
  for ( ;; )
  {

    /*get the session object based on handle*/
    rc = vs_get_typed_object( handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    /* if client hasn't set dtx yet, then do not update CVD */
    VS_ACQUIRE_LOCK( p_session_obj->common_lock );
    if ( ( p_session_obj->pending_voc.g711mulaw.dtx_enable ==
                                    VS_VOC_DTX_MODE_UNDEFINED_V ) &&
         ( p_session_obj->current_voc.g711mulaw.dtx_enable ==
                                    VS_VOC_DTX_MODE_UNDEFINED_V ) )
    {
      VS_RELEASE_LOCK( p_session_obj->common_lock );
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "G711 A-LAW DTX Mode NOT yet set by client - "
           "ADSP default value would be used" );
      break;
    }
    else
    { /* Doubt */
      if ( p_session_obj->pending_voc.g711mulaw.dtx_enable !=
                                       VS_VOC_DTX_MODE_UNDEFINED_V )
      {
        p_session_obj->current_voc.g711mulaw.dtx_enable =
                                  p_session_obj->pending_voc.g711mulaw.dtx_enable;
        p_session_obj->pending_voc.g711mulaw.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      }
      else
      {
        /* we reach here if current_voc.g711mulaw.dtx mode is valid (may be
         * after ADSP SSR)
         * in this case, just send current_voc.g711mulaw.dtx mode to ADSP */
      }
    }

    /* apply the dtx_mode */
    g711mulaw_dtx.enable = p_session_obj->current_voc.g711mulaw.dtx_enable;
    VS_RELEASE_LOCK( p_session_obj->common_lock );

    rc = __aprv2_cmd_alloc_send(
           vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           vs_my_addr, ( (uint16_t) p_session_obj->header.handle ),
           vs_cvs_addr, p_session_obj->cvs_port,
           p_session_obj->header.handle, VSS_ISTREAM_CMD_SET_ENC_DTX_MODE,
           &g711mulaw_dtx, sizeof( g711mulaw_dtx ) );
    if ( rc )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vs_vocg711mulaw_set_cached_voc_property(): Failed to communicate "
                                              "with CVS with rc=0x%08x", rc );
      break;
    }

    MSG_1(MSG_SSID_DFLT, MSG_LEGACY_MED, "vs_vocg711mulaw_set_cached_voc_property(): dtx mode applied %d",
                                          g711mulaw_dtx.enable );
    break;
  }  /* end of for ( ;; ) */

  return rc;
}




