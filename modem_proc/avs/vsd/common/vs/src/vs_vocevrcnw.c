/**
  @file vs_vocevrcnw.c
  @brief This file contains the definitions of the interfaces for EVRC-NW vocoder.

*/

/*
  ============================================================================

   Copyright (C) 2013-2015 QUALCOMM Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/vs/src/vs_vocevrcnw.c#1 $
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
 * Maps the EVRC-NW Average Encoder Rate ( VS to CVD )
 *
 */
VS_INTERNAL uint32_t vs_to_cvd_map_evrcnw_avg_enc_rate (
  uint32_t rate
)
{
  uint32_t mapped_rate;

  switch ( rate )
  {
  case VS_VOCEVRCNW_AVG_ENC_RATE_4_8_KBPS:
    mapped_rate = 0x0007;
    break;

  case VS_VOCEVRCNW_AVG_ENC_RATE_5_8_KBPS:
    mapped_rate = 0x0006;
    break;

  case VS_VOCEVRCNW_AVG_ENC_RATE_6_2_KBPS:
    mapped_rate = 0x0005;
    break;

  case VS_VOCEVRCNW_AVG_ENC_RATE_6_6_KBPS:
    mapped_rate = 0x0004;
    break;

  case VS_VOCEVRCNW_AVG_ENC_RATE_7_0_KBPS:
    mapped_rate = 0x0003;
    break;

  case VS_VOCEVRCNW_AVG_ENC_RATE_7_5_KBPS:
    mapped_rate = 0x0002;
    break;

  case VS_VOCEVRCNW_AVG_ENC_RATE_8_5_KBPS:
    mapped_rate = 0x0000;
    break;

  case VS_VOCEVRCNW_AVG_ENC_RATE_9_3_KBPS:
    mapped_rate = 0x0001;
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_to_cvd_map_evrcnw_avg_enc_rate(): \
           Invalid rate (%d)", rate );
    mapped_rate = -1;
    break;
  }

  return mapped_rate;

}

/**
 * Sets the average encoder rate of the EVRC-NW vocoder.
 *
 * VS_EOK (0) when successful.
 */
VS_INTERNAL uint32_t vs_vocevrcnw_cmd_set_avg_enc_rate (
  vs_vocevrcnw_cmd_set_avg_enc_rate_t* params
)
{
  uint32_t rc = VS_EOK;
  vs_vocevrcnw_cmd_set_avg_enc_rate_t args;
  vs_session_object_t* p_session_obj = NULL;

  for ( ;; )
  {
    if ( params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_vocevrcnw_cmd_set_avg_enc_rate() - NULL params" );
      rc = VS_EBADPARAM;
      break;
    }

    /* get the session object based on handle*/
    rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc != VS_EOK )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_vocevrcnw_cmd_set_avg_enc_rate() - Invalid handle=0x%08x",
             params->handle );
      rc = VS_EBADPARAM;
      break;
    }

    args.handle = params->handle;
    args.rate = params->rate;

    rc = vs_prepare_and_dispatch_cmd_packet ( VS_THREAD_PRIORITY_ENUM_MED,
                                              VS_VOCEVRCNW_CMD_SET_AVG_ENC_RATE,
                                              &args, sizeof( args ) );
    break;
  }  /* end of for ( ;; ) */

  return rc;
}    /* end of vs_vocevrcnw_cmd_set_avg_enc_rate () */

VS_INTERNAL uint32_t vs_vocevrcnw_cmd_set_avg_enc_rate_proc (
  uint32_t handle,
  uint32_t rate
)
{
  uint32_t rc = APR_EOK;
  uint32_t mapped_rate;
  vss_istream_cmd_voc_4gvnw_set_rate_t evrcnw_rate;
  vs_session_object_t* p_session_obj;
  uint32_t current_rate;
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

    mapped_rate = vs_to_cvd_map_evrcnw_avg_enc_rate ( rate );
    if ( mapped_rate == -1 )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_vocevrcnw_cmd_set_avg_enc_rate_proc(): "
             "Invalid frame rate, rate = (%d)", mapped_rate );
      rc = APR_EBADPARAM;
      break;
    }
    VS_ACQUIRE_LOCK( p_session_obj->common_lock );
    current_rate = p_session_obj->current_voc.evrcnw.rate;
    stream_ready = p_session_obj->stream_ready;
    media_id = p_session_obj->media_id;
    pending_media_id = p_session_obj->pending_media_id;
    VS_RELEASE_LOCK( p_session_obj->common_lock );

    if( current_rate ==
        (uint32_t) mapped_rate )
    {
      /* Invalidate pending_voc since newly set voc properties
       * are identical to current_voc properties.
       */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.evrcnw.rate = VS_VOCEVRCNW_RATE_UNDEFINED_V;
      VS_RELEASE_LOCK( p_session_obj->common_lock );

      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_vocevrcnw_cmd_set_avg_enc_rate_proc(): "
             "no change in rate %d",
             mapped_rate );
      break;
    }

    /* Cache and send to CVD immediately when in RUN state and media_id is
     * VS_COMMON_MEDIA_ID_EVRCNW.
     * If both conditions are NOT true, cache the value and apply while
     * enabling the stream.
     */
    if ( ( stream_ready == TRUE ) &&
         ( media_id == VS_COMMON_MEDIA_ID_EVRCNW ) &&
         ( pending_media_id == VS_COMMON_MEDIA_ID_EVRCNW ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_vocevrcnw_cmd_set_avg_enc_rate_proc() - "
             "rate %d set "
             "Current rate %d",
             mapped_rate,
             current_rate );

      /* update pending_voc and current_voc */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.evrcnw.rate = VS_VOCEVRCNW_RATE_UNDEFINED_V;
      p_session_obj->current_voc.evrcnw.rate = ( uint32_t ) mapped_rate;

      evrcnw_rate.rate = p_session_obj->current_voc.evrcnw.rate;
      VS_RELEASE_LOCK( p_session_obj->common_lock );

      /* apply the minmax rate */
      rc = __aprv2_cmd_alloc_send(
             vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             vs_my_addr, ( (uint16_t) p_session_obj->header.handle ),
             vs_cvs_addr, p_session_obj->cvs_port,
             p_session_obj->header.handle, VSS_ISTREAM_CMD_VOC_4GVNW_SET_RATE,
             &evrcnw_rate, sizeof( evrcnw_rate ) );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                "vs_vocevrcnw_cmd_set_avg_enc_rate_proc(): \
                Failed to communicate with "
                "CVS with rc=0x%08x", rc );
      }
    }
    else
    {
      /* Cache the rate now.
       * Apply the rate while enabling the stream
       */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.evrcnw.rate = ( uint32_t ) mapped_rate;
      VS_RELEASE_LOCK( p_session_obj->common_lock );

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_vocevrcnw_cmd_set_avg_enc_rate_proc()- "
             "rate %d set, "
             "Cached rate %d",
             mapped_rate, mapped_rate );
    }

    break;

  } /* end of for ( ;; ) */

  return rc;

} /* end of vs_vocevrcnw_cmd_set_avg_enc_rate_proc () */

/**
 * Sets the minimum and maximum encoder rate of the EVRC-NW vocoder.
 *
 * VS_EOK (0) when successful.
 */
VS_INTERNAL uint32_t vs_vocevrcnw_cmd_set_frame_minmax_rate (
  vs_vocevrcnw_cmd_set_frame_minmax_rate_t* params
)
{
  uint32_t rc = VS_EOK;
  vs_vocevrcnw_cmd_set_frame_minmax_rate_t args;
  vs_session_object_t* p_session_obj = NULL;

  for ( ;; )
  {
    if ( params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_vocevrcnw_cmd_set_frame_minmax_rate() - NULL params" );
      rc = VS_EBADPARAM;
      break;
    }

    /* get the session object based on handle*/
    rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc != VS_EOK )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_vocevrcnw_cmd_set_frame_minmax_rate() - Invalid handle=0x%08x",
             params->handle );
      rc = VS_EBADPARAM;
      break;
    }

    args.handle = params->handle;
    args.min_rate = params->min_rate;
    args.max_rate = params->max_rate;

    rc = vs_prepare_and_dispatch_cmd_packet( VS_THREAD_PRIORITY_ENUM_MED,
                                             VS_VOCEVRCNW_CMD_SET_FRAME_MINMAX_RATE,
                                             &args, sizeof( args ) );
    break;
  }  /* end of for ( ;; ) */

  return rc;
}    /* end of vs_vocevrcnw_cmd_set_frame_minmax_rate () */

VS_INTERNAL uint32_t vs_vocevrcnw_cmd_set_frame_minmax_rate_proc (
  uint32_t handle,
  vs_vocevrcnw_frame_rate_t min_rate,
  vs_vocevrcnw_frame_rate_t max_rate
)
{
  uint32_t rc = APR_EOK;
  uint16_t evrcnw_min_rate;
  uint16_t evrcnw_max_rate;
  vss_istream_cmd_cdma_set_enc_minmax_rate_t evrcnw_minmax_rate;
  vs_session_object_t* p_session_obj = NULL;
  uint32_t current_min_rate;
  uint32_t current_max_rate;
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

    evrcnw_min_rate = ( uint16_t ) vs_to_cvd_map_evrcnw_frame_rate ( min_rate );
    evrcnw_max_rate = ( uint16_t ) vs_to_cvd_map_evrcnw_frame_rate ( max_rate );

    if ( ( evrcnw_min_rate == VS_VOC_INVALID_RATE ) || ( evrcnw_max_rate == VS_VOC_INVALID_RATE ) )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_vocevrcnw_cmd_set_frame_minmax_rate_proc(): "
             "Invalid frame rate, min = (%d), max = (%d)",
             evrcnw_min_rate, evrcnw_max_rate );
      rc = APR_EBADPARAM;
      break;
    }
    VS_ACQUIRE_LOCK( p_session_obj->common_lock );
    current_min_rate = p_session_obj->current_voc.evrcnw.min_rate;
    current_max_rate = p_session_obj->current_voc.evrcnw.max_rate;
    stream_ready = p_session_obj->stream_ready;
    media_id = p_session_obj->media_id;
    pending_media_id = p_session_obj->pending_media_id;
    VS_RELEASE_LOCK( p_session_obj->common_lock );

    if( ( current_min_rate ==
        (vs_vocevrcnw_frame_rate_t) evrcnw_min_rate ) &&
        ( current_max_rate ==
        (vs_vocevrcnw_frame_rate_t) evrcnw_max_rate ) )
    {
      /* Invalidate pending_voc since newly set voc properties
       * are identical to current_voc properties.
       */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.evrcnw.min_rate =
                             ( uint16_t ) VS_VOCEVRCNW_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->pending_voc.evrcnw.max_rate =
                             ( uint16_t ) VS_VOCEVRCNW_MINMAX_RATE_UNDEFINED_V;
      VS_RELEASE_LOCK( p_session_obj->common_lock );

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_vocevrcnw_cmd_set_frame_minmax_rate_proc(): "
             "no change in min_rate and max_rate 0x%08x 0x%08x",
             evrcnw_min_rate, evrcnw_max_rate );
      break;
    }

    /* Cache and send to CVD immediately when in RUN state and media_id is
     * VS_COMMON_MEDIA_ID_EVRCNW.
     * If both conditions are NOT true, cache the value and apply while
     * enabling the stream.
     */
    if ( ( stream_ready == TRUE ) &&
         ( media_id == VS_COMMON_MEDIA_ID_EVRCNW ) &&
         ( pending_media_id == VS_COMMON_MEDIA_ID_EVRCNW ) )
    {
      MSG_4( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_vocevrcnw_cmd_set_frame_minmax_rate_proc()- "
             "evrcnw_min_rate 0x%08x evrcnw_max_rate 0x%08x set, "
             "current min_rate 0x%08x max_rate 0x%08x",
             evrcnw_min_rate, evrcnw_max_rate,
             current_min_rate,
             current_max_rate );

      /* update pending_voc and current_voc */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.evrcnw.min_rate =
                             ( uint16_t ) VS_VOCEVRCNW_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->pending_voc.evrcnw.max_rate =
                             ( uint16_t ) VS_VOCEVRCNW_MINMAX_RATE_UNDEFINED_V;
      p_session_obj->current_voc.evrcnw.min_rate =
                             ( uint16_t ) evrcnw_min_rate;
      p_session_obj->current_voc.evrcnw.max_rate =
                             ( uint16_t ) evrcnw_max_rate;

      evrcnw_minmax_rate.min_rate = p_session_obj->current_voc.evrcnw.min_rate;
      evrcnw_minmax_rate.max_rate = p_session_obj->current_voc.evrcnw.max_rate;
      VS_RELEASE_LOCK( p_session_obj->common_lock );

      /* apply the minmax rate */
      rc = __aprv2_cmd_alloc_send(
             vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             vs_my_addr, ( (uint16_t) p_session_obj->header.handle ),
             vs_cvs_addr, p_session_obj->cvs_port,
             p_session_obj->header.handle,
             VSS_ISTREAM_CMD_CDMA_SET_ENC_MINMAX_RATE,
             &evrcnw_minmax_rate, sizeof( evrcnw_minmax_rate ) );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_vocevrcnw_cmd_set_frame_minmax_rate_proc(): \
               Failed to communicate with "
               "CVS with rc=0x%08x", rc );
      }
    }
    else
    {
      /* Cache the dtx_mode now.
       * Apply the dtx_mode while enabling the stream
       */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.evrcnw.min_rate = ( uint16_t ) evrcnw_min_rate;
      p_session_obj->pending_voc.evrcnw.max_rate = ( uint16_t ) evrcnw_max_rate;
      VS_RELEASE_LOCK( p_session_obj->common_lock );
      MSG_4( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_vocevrcnw_cmd_set_frame_minmax_rate_proc()- "
             "evrcnw_min_rate 0x%08x evrcnw_max_rate 0x%08x set, "
             "Cached min_rate 0x%08x max_rate 0x%08x",
              evrcnw_min_rate, evrcnw_max_rate,
              evrcnw_min_rate,
              evrcnw_max_rate );
    }

    break;

  }  /* end of for ( ;; ) */

  return rc;
} /* end of vs_vocevrcnw_cmd_set_frame_minmax_rate_proc () */

/**
 * Sets the Discontinuous Transmission (DTX) mode of the EVRC-NW vocoder.
 *
 * VS_EOK (0) when successful.
 */
VS_INTERNAL uint32_t vs_vocevrcnw_cmd_set_dtx_mode (
  vs_vocevrcnw_cmd_set_dtx_mode_t* params
)
{
  uint32_t rc = VS_EOK;
  vs_vocevrcnw_cmd_set_dtx_mode_t args;
  vs_session_object_t* p_session_obj = NULL;

  for ( ;; )
  {
    if ( params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_vocevrcnw_cmd_set_dtx_mode() - NULL params" );
      rc = VS_EBADPARAM;
      break;
    }

    /* get the session object based on handle*/
    rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc != VS_EOK )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_vocevrcnw_cmd_set_dtx_mode() - Invalid handle=0x%08x",
             params->handle );
      rc = VS_EBADPARAM;
      break;;
    }

    args.handle = params->handle;
    args.enable_flag = params->enable_flag;

    rc = vs_prepare_and_dispatch_cmd_packet( VS_THREAD_PRIORITY_ENUM_MED,
                                             VS_VOCEVRCNW_CMD_SET_DTX_MODE,
                                             &args, sizeof( args ) );
    break;
  }  /* end of for ( ;; ) */

  return rc;
}    /* end of vs_vocevrcnw_cmd_set_dtx_mode () */

VS_INTERNAL uint32_t vs_vocevrcnw_cmd_set_dtx_mode_proc (
  uint32_t handle,
  bool_t dtx_enable
)
{
  uint32_t rc = APR_EOK;
  vss_istream_cmd_set_enc_dtx_mode_t evrcnw_dtx;
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
    current_voc_dtx_enable = p_session_obj->current_voc.evrcnw.dtx_enable;
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
      p_session_obj->pending_voc.evrcnw.dtx_enable =
                             VS_VOC_DTX_MODE_UNDEFINED_V;
      VS_RELEASE_LOCK( p_session_obj->common_lock );

      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_vocevrcnw_cmd_set_dtx_mode_proc(): no change in dtx_mode %d",
             dtx_enable );
      break;
    }

    /* Cache and send to CVD immediately when in RUN state and media_id is
     * VS_COMMON_MEDIA_ID_EVRCNW.
     * If both conditions are NOT true, cache the value and apply while
     * enabling the stream.
     */
    if ( ( stream_ready == TRUE ) &&
         ( media_id == VS_COMMON_MEDIA_ID_EVRCNW ) &&
         ( pending_media_id == VS_COMMON_MEDIA_ID_EVRCNW ) )
    {

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_vocevrcnw_cmd_set_dtx_mode_proc()-  dtx_mode %d set, \
             previous mode %d", dtx_enable,
             current_voc_dtx_enable );

      /* update pending_voc and current_voc */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.evrcnw.dtx_enable =
                             VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->current_voc.evrcnw.dtx_enable =
                             ( uint32_t )dtx_enable;

      evrcnw_dtx.enable = p_session_obj->current_voc.evrcnw.dtx_enable;
      VS_RELEASE_LOCK( p_session_obj->common_lock );

      /* apply the dtx_mode */
      rc = __aprv2_cmd_alloc_send(
             vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             vs_my_addr, ( (uint16_t) p_session_obj->header.handle ),
             vs_cvs_addr, p_session_obj->cvs_port,
             p_session_obj->header.handle, VSS_ISTREAM_CMD_SET_ENC_DTX_MODE,
             &evrcnw_dtx, sizeof( evrcnw_dtx ) );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_vocevrcnw_cmd_set_dtx_mode_proc(): \
               Failed to communicate with \
               CVS with rc=0x%08x", rc );
      }
    }
    else
    {
      /* Cache the dtx_mode now.
       * Apply the dtx_mode while enabling the stream
       */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.evrcnw.dtx_enable = ( uint32_t ) dtx_enable;
      VS_RELEASE_LOCK( p_session_obj->common_lock );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_vocevrcnw_cmd_set_dtx_mode_proc(): dtx_mode saved=%d, "
             "current dtx_mode=%d", dtx_enable,
             current_voc_dtx_enable );
    }

    break;

  }  /* end of for ( ;; ) */

  return rc;
} /* end of vs_vocevrcnw_cmd_set_dtx_mode_proc () */

VS_INTERNAL uint32_t vs_vocevrcnw_set_cached_voc_property (
  uint32_t handle
)
{
  uint32_t rc = APR_EOK;
  vs_session_object_t* p_session_obj = NULL;
  vss_istream_cmd_voc_4gvnw_set_rate_t evrcnw_rate;
  vss_istream_cmd_cdma_set_enc_minmax_rate_t evrcnw_minmax_rate;
  vss_istream_cmd_set_enc_dtx_mode_t evrcnw_dtx;

  /* set cached evrcnw rate */
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

    /* if client hasn't set rate yet, then do not update CVD */
    VS_ACQUIRE_LOCK( p_session_obj->common_lock );
    if ( ( p_session_obj->pending_voc.evrcnw.rate ==
                                    VS_VOCEVRCNW_RATE_NONE_V ) &&
         ( p_session_obj->current_voc.evrcnw.rate ==
                                    VS_VOCEVRCNW_RATE_NONE_V ) )
    {
      VS_RELEASE_LOCK( p_session_obj->common_lock );
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "EVRC-NW Rate NOT yet set by client - "
           "ADSP default value would be used" );
      break;
    }
    else
    { /* Doubt */
      if ( p_session_obj->pending_voc.evrcnw.rate !=
                                       VS_VOCEVRCNW_RATE_NONE_V )
      {
        p_session_obj->current_voc.evrcnw.rate =
                                  p_session_obj->pending_voc.evrcnw.rate;
        p_session_obj->pending_voc.evrcnw.rate = VS_VOCEVRCNW_RATE_NONE_V;
      }
      else
      {
        /* we reach here if current_voc.evrcnw.rate is valid (may be
         * after ADSP SSR)
         * in this case, just send current_voc.evrcnw.rate to ADSP */
      }
    }

    /* apply the rate */
    evrcnw_rate.rate = p_session_obj->current_voc.evrcnw.rate;
    VS_RELEASE_LOCK( p_session_obj->common_lock );

    rc = __aprv2_cmd_alloc_send(
           vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           vs_my_addr, ( (uint16_t) p_session_obj->header.handle ),
           vs_cvs_addr, p_session_obj->cvs_port,
           p_session_obj->header.handle, VSS_ISTREAM_CMD_VOC_4GVNW_SET_RATE,
           &evrcnw_rate, sizeof( evrcnw_rate ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_vocevrcnw_set_cached_voc_property(): Failed to communicate "
             "with CVS with rc=0x%08x", rc );
      break;
    }

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vs_vocevrcnw_set_cached_voc_property(): rate applied %d",
           evrcnw_rate.rate );
    break;
  }  /* end of for ( ;; ) */

  /* set cached evrcnw min-max rate  */
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

    /* if client hasn't set min max yet, then do not update CVD */
    VS_ACQUIRE_LOCK( p_session_obj->common_lock );
    if ( ( p_session_obj->pending_voc.evrcnw.min_rate ==
                             ( uint16_t ) VS_VOCEVRCNW_MINMAX_RATE_NONE_V ) &&
         ( p_session_obj->current_voc.evrcnw.min_rate ==
                             (uint16_t) VS_VOCEVRCNW_MINMAX_RATE_NONE_V ) &&
         ( p_session_obj->pending_voc.evrcnw.max_rate ==
                             ( uint16_t ) VS_VOCEVRCNW_MINMAX_RATE_NONE_V ) &&
         ( p_session_obj->current_voc.evrcnw.max_rate ==
                             ( uint16_t ) VS_VOCEVRCNW_MINMAX_RATE_NONE_V ) )
    {
      VS_RELEASE_LOCK( p_session_obj->common_lock );
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "EVRC-NW Min Rate and Max Rate NOT yet set by client - "
           "ADSP default value would be used" );
      break;
    }
    else
    { /* Doubt */
      if ( p_session_obj->pending_voc.evrcnw.min_rate !=
                             ( uint16_t ) VS_VOCEVRCNW_MINMAX_RATE_NONE_V &&
           p_session_obj->pending_voc.evrcnw.max_rate !=
                             ( uint16_t ) VS_VOCEVRCNW_MINMAX_RATE_NONE_V )
      {
        p_session_obj->current_voc.evrcnw.min_rate =
                             p_session_obj->pending_voc.evrcnw.min_rate;
        p_session_obj->current_voc.evrcnw.max_rate =
                             p_session_obj->pending_voc.evrcnw.max_rate;
        p_session_obj->pending_voc.evrcnw.min_rate =
                             ( uint16_t ) VS_VOCEVRCNW_MINMAX_RATE_NONE_V;
        p_session_obj->pending_voc.evrcnw.max_rate =
                             ( uint16_t ) VS_VOCEVRCNW_MINMAX_RATE_NONE_V;
      }
      else
      {
        /* we reach here if current_voc.evrcnw.minmax rate is valid (may be
         * after ADSP SSR)
         * in this case, just send current_voc.evrcnw.minmax rate to ADSP */
      }
    }

    /* apply the min max rate */
    evrcnw_minmax_rate.min_rate = p_session_obj->current_voc.evrcnw.min_rate;
    evrcnw_minmax_rate.max_rate = p_session_obj->current_voc.evrcnw.max_rate;
    VS_RELEASE_LOCK( p_session_obj->common_lock );

    rc = __aprv2_cmd_alloc_send(
           vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           vs_my_addr, ( (uint16_t) p_session_obj->header.handle ),
           vs_cvs_addr, p_session_obj->cvs_port,
           p_session_obj->header.handle,
           VSS_ISTREAM_CMD_CDMA_SET_ENC_MINMAX_RATE,
           &evrcnw_minmax_rate, sizeof( evrcnw_minmax_rate ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_vocevrcnw_set_cached_voc_property(): Failed to communicate "
             "with CVS with rc=0x%08x", rc );
      break;
    }

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vs_vocevrcnw_set_cached_voc_property(): \
           min_rate and max_rate applied %d %d",
           evrcnw_minmax_rate.min_rate, evrcnw_minmax_rate.max_rate );
    break;
  }  /* end of for ( ;; ) */

  /* set cached evrcnw dtx mode */
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
    if ( ( p_session_obj->pending_voc.evrcnw.dtx_enable ==
                                    VS_VOC_DTX_MODE_UNDEFINED_V ) &&
         ( p_session_obj->current_voc.evrcnw.dtx_enable ==
                                    VS_VOC_DTX_MODE_UNDEFINED_V ) )
    {
      VS_RELEASE_LOCK( p_session_obj->common_lock );
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "EVRC-NW DTX Mode NOT yet set by client - "
           "ADSP default value would be used" );
      break;
    }
    else
    { /* Doubt */
      if ( p_session_obj->pending_voc.evrcnw.dtx_enable !=
                                       VS_VOC_DTX_MODE_UNDEFINED_V )
      {
        p_session_obj->current_voc.evrcnw.dtx_enable =
                                  p_session_obj->pending_voc.evrcnw.dtx_enable;
        p_session_obj->pending_voc.evrcnw.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      }
      else
      {
        /* we reach here if current_voc.evrcnw.dtx mode is valid (may be
         * after ADSP SSR)
         * in this case, just send current_voc.evrcnw.dtx mode to ADSP */
      }
    }

    /* apply the dtx_mode */
    evrcnw_dtx.enable = p_session_obj->current_voc.evrcnw.dtx_enable;
    VS_RELEASE_LOCK( p_session_obj->common_lock );

    rc = __aprv2_cmd_alloc_send(
           vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           vs_my_addr, ( (uint16_t) p_session_obj->header.handle ),
           vs_cvs_addr, p_session_obj->cvs_port,
           p_session_obj->header.handle, VSS_ISTREAM_CMD_SET_ENC_DTX_MODE,
           &evrcnw_dtx, sizeof( evrcnw_dtx ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_vocevrcnw_set_cached_voc_property(): Failed to communicate "
             "with CVS with rc=0x%08x", rc );
      break;
    }

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vs_vocevrcnw_set_cached_voc_property(): dtx mode applied %d",
           evrcnw_dtx.enable );
    break;
  }  /* end of for ( ;; ) */

  return rc;
} /* end of vs_vocevrcnw_set_cached_voc_property () */
