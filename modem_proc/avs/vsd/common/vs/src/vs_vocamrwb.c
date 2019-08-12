/**
  @file vs_vocamrwb.c
  @brief This file contains the definitions of the interfaces for AMR-WB
         vocoder.

*/

/*
  ============================================================================

   Copyright (C) 2013-2015 QUALCOMM Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/vs/src/vs_vocamrwb.c#1 $
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  05/22/14  aim   Fixed a bug in applying vocoder properties.
  07/30/13  sud   Updated software to report MSG_FATAL when fatal error
                  conditions occurs in client context.
  06/26/13  sud   Updated debug messages and rearranged some of the functions
                  from vs_voc.c
  06/16/13  sud   Added debug messages.
  06/13/13  hl    Added support for handling ADSP SSR.
  06/12/13  sud   Updated VS to handle errors and status propagation to client.
                  Updated VS to handle memory leak and memory corruption
                  issues.
                  Updated VS to handle command priorities and proper sequencing.
                  Updated VS to add proper debug messages
  04/16/13  sud   VS refactored code and fixed bugs.
  01/10/13  sg    Added the funtions for AMR_WB vocoder
  01/02/13  sud   Initial revision

  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/
#include <stdlib.h>

#include "msg.h"
#include "vs.h"
#include "vs_i.h"
#include "amrsup.h"


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
 * Sets the codec mode of the AMRWB vocoder.
 *
 * VS_EOK (0) when successful.
 */
VS_INTERNAL uint32_t vs_vocamrwb_cmd_set_codec_mode (
  vs_vocamrwb_cmd_set_codec_mode_t* params
)
{
  uint32_t rc = VS_EOK;
  vs_vocamrwb_cmd_set_codec_mode_t mode_args;
  vs_session_object_t* p_session_obj = NULL;

  for ( ;; )
  {
    if ( params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_vocamrwb_cmd_set_codec_mode() - NULL params" );
      rc = VS_EBADPARAM;
      break;
    }

    /* get the session object based on handle*/
    rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc != VS_EOK )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_vocamrwb_cmd_set_codec_mode() - Invalid handle=0x%08x",
             params->handle );
      rc = VS_EBADPARAM;
      break;
    }

    mode_args.handle = params->handle;
    mode_args.codec_mode = params->codec_mode;

    rc = vs_prepare_and_dispatch_cmd_packet( VS_THREAD_PRIORITY_ENUM_MED,
                                             VS_VOCAMRWB_CMD_SET_CODEC_MODE,
                                             &mode_args, sizeof( mode_args ) );
    break;
  }  /* end of for ( ;; ) */

  return rc;
}  /* end of vs_vocamrwb_cmd_set_codec_mode () */

VS_INTERNAL uint32_t vs_vocamrwb_cmd_set_codec_mode_proc (
  uint32_t handle,
  vs_vocamrwb_codec_mode_t codec_mode
)
{

  uint32_t rc = VS_EOK;
  vss_istream_cmd_voc_amrwb_set_enc_rate_t amrwb_mode;
  vs_session_object_t* p_session_obj;
  vs_vocamrwb_codec_mode_t current_voc_codec_mode;
  bool_t stream_ready;
  uint32_t media_id;
  uint32_t pending_media_id;

  for ( ;; )
  {
    /* get the session object based on handle*/
    rc = vs_get_typed_object( handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    if ( codec_mode > VS_VOCAMRWB_CODEC_MODE_2385 )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_vocamrwb_cmd_set_codec_mode(): Invalid AMRWB codec_mode %d "
             "input, setting mode to 06.60", codec_mode );

      codec_mode = VS_VOCAMRWB_CODEC_MODE_0660;
    }
    VS_ACQUIRE_LOCK( p_session_obj->common_lock );
    current_voc_codec_mode = p_session_obj->current_voc.amrwb.codec_mode;
    stream_ready = p_session_obj->stream_ready;
    media_id = p_session_obj->media_id;
    pending_media_id = p_session_obj->pending_media_id;
    VS_RELEASE_LOCK( p_session_obj->common_lock );

    /* send to CVD only if the codec mode requested is different from the
     * current codec mode
     */
    if ( current_voc_codec_mode == ( uint32_t )codec_mode )
    {
      /* Invalidate pending_voc since newly set voc properties
       * are identical to current_voc properties.
       */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.amrwb.codec_mode = VS_VOCAMRWB_CODEC_MODE_NONE_V;
      VS_RELEASE_LOCK( p_session_obj->common_lock );

      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_vocamrwb_cmd_set_codec_mode_proc(): no change in "
             "codec_mode %d", codec_mode );
      break;
    }

    /* Cache and send to CVD immediately when in RUN state and media_id is
     * VS_COMMON_MEDIA_ID_AMRWB.
     * If both conditions are NOT true, cache the value and apply while                    .
     * enabling the stream.
     */
    if ( ( stream_ready ==  TRUE ) &&
         ( media_id == VS_COMMON_MEDIA_ID_AMRWB ) &&
         ( pending_media_id == VS_COMMON_MEDIA_ID_AMRWB ) )
    {

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_vocamrwb_cmd_set_codec_mode_proc()-  codec_mode %d set, "
             "previous mode %d",
             codec_mode, current_voc_codec_mode );

      /* update pending_voc and current_voc */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.amrwb.codec_mode = VS_VOCAMRWB_CODEC_MODE_NONE_V;
      p_session_obj->current_voc.amrwb.codec_mode = ( uint32_t )codec_mode;
      VS_RELEASE_LOCK( p_session_obj->common_lock );

      /* apply the codec_mode */
      amrwb_mode.mode = p_session_obj->current_voc.amrwb.codec_mode;

      rc = __aprv2_cmd_alloc_send(
             vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             vs_my_addr, ( (uint16_t) p_session_obj->header.handle ),
             vs_cvs_addr, p_session_obj->cvs_port,
             p_session_obj->header.handle,
             VSS_ISTREAM_CMD_VOC_AMRWB_SET_ENC_RATE,
             &amrwb_mode, sizeof( amrwb_mode ) );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_vocamrwb_cmd_set_codec_mode_proc(): Failed to communicate "
               "with CVS with rc=0x%08x", rc );
      }

      break;
    }
    else
    {
      /* Cache the codec_mode now.
       * Apply the codec_mode while enabling the stream
       */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.amrwb.codec_mode = ( uint32_t )codec_mode;
      VS_RELEASE_LOCK( p_session_obj->common_lock );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_vocamrwb_cmd_set_codec_mode_proc() - codec_mode saved=%d "
             "current codec_mode=%d", codec_mode,
             current_voc_codec_mode );
      break;
    }  /* end of if ( session_obj->state == VS_VOC_STATE_ENUM_RUN ) */

  }  /* end of for ( ;; ) */

  return rc;

}  /* end of vs_vocamrwb_cmd_set_codec_mode_proc () */

/**
 * Sets the DTX mode of the AMRWB vocoder.
 *
 * VS_EOK (0) when successful.
 */
VS_INTERNAL uint32_t vs_vocamrwb_cmd_set_dtx_mode (
  vs_vocamrwb_cmd_set_dtx_mode_t* params
)
{
  uint32_t rc = VS_EOK;
  vs_vocamrwb_cmd_set_dtx_mode_t dtx_args;
  vs_session_object_t* p_session_obj = NULL;

  for ( ;; )
  {
    if ( params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_vocamrwb_cmd_set_dtx_mode() - NULL params" );
      rc = VS_EBADPARAM;
      break;
    }

    /* get the session object based on handle*/
    rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc != VS_EOK )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_vocamrwb_cmd_set_dtx_mode() - Invalid handle=0x%08x",
             params->handle );
      rc = VS_EBADPARAM;
      break;
    }

    dtx_args.handle = params->handle;
    dtx_args.enable_flag = params->enable_flag;

    rc = vs_prepare_and_dispatch_cmd_packet( VS_THREAD_PRIORITY_ENUM_MED,
                                             VS_VOCAMRWB_CMD_SET_DTX_MODE,
                                             &dtx_args, sizeof( dtx_args ) );
    break;
  }  /* end of for ( ;; ) */

  return rc;
}  /* end of vs_vocamrwb_cmd_set_dtx_mode () */

VS_INTERNAL uint32_t vs_vocamrwb_cmd_set_dtx_mode_proc (
  uint32_t handle,
  bool_t dtx_enable
)
{
  uint32_t rc = VS_EOK;
  vss_istream_cmd_set_enc_dtx_mode_t amrwb_dtx;
  vs_session_object_t* p_session_obj;
  uint32_t current_voc_dtx_enable;
  bool_t stream_ready;
  uint32_t media_id;
  uint32_t pending_media_id;

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
    
    VS_ACQUIRE_LOCK( p_session_obj->common_lock );
    current_voc_dtx_enable = p_session_obj->current_voc.amrwb.dtx_enable;
    stream_ready = p_session_obj->stream_ready;
    media_id = p_session_obj->media_id;
    pending_media_id = p_session_obj->pending_media_id;
    VS_RELEASE_LOCK( p_session_obj->common_lock );

    /* send to CVD only if the dtx mode requested is different from the
     * current dtx mode
     */
    if ( current_voc_dtx_enable == ( uint32_t ) dtx_enable )
    {
      /* Invalidate pending_voc since newly set voc properties
       * are identical to current_voc properties.
       */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.amrwb.dtx_enable =
                                               VS_VOC_DTX_MODE_UNDEFINED_V;
      VS_RELEASE_LOCK( p_session_obj->common_lock );

      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "vs_vocamrwb_cmd_set_dtx_mode_proc(): no change in dtx_mode %d",
             dtx_enable );
      break;
    }

    /* Cache and send to CVD immediately when in RUN state and media_id is
     * VS_COMMON_MEDIA_ID_AMRWB.
     * If both conditions are NOT true, cache the value and apply while                    .
     * enabling the stream.
     */
    if ( ( stream_ready == TRUE ) &&
         ( media_id == VS_COMMON_MEDIA_ID_AMRWB ) &&
         ( pending_media_id == VS_COMMON_MEDIA_ID_AMRWB ) )
    {

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_vocamrwb_cmd_set_dtx_mode_proc()-  dtx_mode %d set, "
             "previous mode %d", dtx_enable,
             current_voc_dtx_enable );

      /* update pending_voc and current_voc */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.amrwb.dtx_enable =
                                               VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->current_voc.amrwb.dtx_enable = ( uint32_t )dtx_enable;
      VS_RELEASE_LOCK( p_session_obj->common_lock );

      amrwb_dtx.enable = ( uint32_t )dtx_enable;

      /* apply the dtx_mode */
      rc = __aprv2_cmd_alloc_send(
             vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             vs_my_addr, ( (uint16_t) p_session_obj->header.handle ),
             vs_cvs_addr, p_session_obj->cvs_port,
             p_session_obj->header.handle, VSS_ISTREAM_CMD_SET_ENC_DTX_MODE,
             &amrwb_dtx, sizeof( amrwb_dtx ) );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_vocamrwb_cmd_set_dtx_mode_proc(): Failed to communicate "
               "with CVS with rc=0x%08x", rc );
      }

      break;
    }
    else
    {
      /* Cache the dtx_mode now.
       * Apply the dtx_mode while enabling the stream
       */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.amrwb.dtx_enable = ( uint32_t )dtx_enable;
      VS_RELEASE_LOCK( p_session_obj->common_lock );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_vocamrwb_cmd_set_dtx_mode_proc(): dtx_mode saved=%d,"
             "current dtx_mode=%d", dtx_enable,
             current_voc_dtx_enable );
      break;
    }  /* end of if ( session_obj->state == VS_VOC_STATE_ENUM_RUN ) */

  }  /* end of for ( ;; ) */

  return rc;

}  /* end of vs_vocamrwb_cmd_set_dtx_mode_proc () */

VS_INTERNAL uint32_t vs_vocamrwb_set_cached_voc_property (
  uint32_t handle
)
{

  uint32_t rc = VS_EOK;
  vs_session_object_t* p_session_obj = NULL;
  vss_istream_cmd_voc_amrwb_set_enc_rate_t amrwb_mode;
  vss_istream_cmd_set_enc_dtx_mode_t amrwb_dtx;

  /* set cached amrwb codec_mode */
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

    /* if client hasn't set codec_mode yet, then do not update CVD */
    VS_ACQUIRE_LOCK( p_session_obj->common_lock );
    if ( ( p_session_obj->pending_voc.amrwb.codec_mode ==
                                    VS_VOCAMRWB_CODEC_MODE_NONE_V ) &&
         ( p_session_obj->current_voc.amrwb.codec_mode ==
                                    VS_VOCAMRWB_CODEC_MODE_NONE_V ) )
    {
      VS_RELEASE_LOCK( p_session_obj->common_lock );
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "AMRWB codec_mode NOT yet set by client - ADSP default "
           "value would be used" );
      break;
    }
    else
    {
      if ( p_session_obj->pending_voc.amrwb.codec_mode !=
                                       VS_VOCAMR_CODEC_MODE_NONE_V )
      {
        p_session_obj->current_voc.amrwb.codec_mode =
                                  p_session_obj->pending_voc.amrwb.codec_mode;
        p_session_obj->pending_voc.amrwb.codec_mode =
                                            VS_VOCAMRWB_CODEC_MODE_NONE_V;
      }
      else
      {
        /* we reach here if current_voc.amrwb.codec_mode is valid (may be
         * after ADSP SSR)
         * in this case, just send current_voc.amrwb.codec_mode to ADSP */
      }
    }

    /* apply the codec_mode */
    amrwb_mode.mode = p_session_obj->current_voc.amrwb.codec_mode;
    VS_RELEASE_LOCK( p_session_obj->common_lock );

    rc = __aprv2_cmd_alloc_send(
           vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           vs_my_addr, ( (uint16_t) p_session_obj->header.handle ),
           vs_cvs_addr, p_session_obj->cvs_port,
           p_session_obj->header.handle,
           VSS_ISTREAM_CMD_VOC_AMRWB_SET_ENC_RATE,
           &amrwb_mode, sizeof( amrwb_mode ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_vocamrwb_set_cached_voc_property(): Failed to communicate "
             "with CVS with rc=0x%08x", rc );
      break;
    }

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vs_vocamrwb_set_cached_voc_property(): codec_mode applied %d",
           amrwb_mode.mode );
    break;
  }  /* end of for ( ;; ) */

  /* set cached dtx_mode */
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

    /* if client hasn't set dtx_mode yet, then do not update CVD.
     * if we are NOT here because of ADSP SSR, then do not update CVD.
     */
    VS_ACQUIRE_LOCK( p_session_obj->common_lock );
    if ( ( p_session_obj->pending_voc.amrwb.dtx_enable ==
                                               VS_VOC_DTX_MODE_UNDEFINED_V ) &&
         ( p_session_obj->current_voc.amrwb.dtx_enable ==
                                               VS_VOC_DTX_MODE_UNDEFINED_V ) )
    {
      VS_RELEASE_LOCK( p_session_obj->common_lock );
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "AMRWB dtx_mode is not yet set by client - ADSP default value "
           "would be used" );
      break;
    }
    else
    {
      if ( p_session_obj->pending_voc.amrwb.dtx_enable !=
                                            VS_VOC_DTX_MODE_UNDEFINED_V )
      {
        p_session_obj->current_voc.amrwb.dtx_enable =
                                    p_session_obj->pending_voc.amrwb.dtx_enable;
        p_session_obj->pending_voc.amrwb.dtx_enable =
                                    VS_VOC_DTX_MODE_UNDEFINED_V;
      }
      else
      {
        /* we reach here if current_voc.amrwb.codec_mode is valid (may be
         * after ADSP SSR)
         * in this case, just send current_voc.amrwb.codec_mode to ADSP */
      }
    }

    /* apply the codec_mode */
    amrwb_dtx.enable = p_session_obj->current_voc.amrwb.dtx_enable;
    VS_RELEASE_LOCK( p_session_obj->common_lock );

    rc = __aprv2_cmd_alloc_send(
           vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           vs_my_addr, ( (uint16_t) p_session_obj->header.handle ),
           vs_cvs_addr, p_session_obj->cvs_port,
           p_session_obj->header.handle, VSS_ISTREAM_CMD_SET_ENC_DTX_MODE,
           &amrwb_dtx, sizeof( amrwb_dtx ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_vocamrwb_set_cached_voc_property(): Faield to communicate "
             "with CVS with rc=0x%08x", rc );
      break;
    }

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vs_vocamrwb_set_cached_voc_property(): dtx_mode applied %d",
           amrwb_dtx.enable );
    break;
  }  /* end of for ( ;; ) */

  return rc;

}  /* end of vs_vocamrwb_set_cached_values () */

/**
 * Gets the codec mode of the AMRWB vocoder.
 *
 * VS_EOK (0) when successful.
 */
VS_INTERNAL uint32_t vs_vocamrwb_cmd_get_codec_mode (
  vs_vocamrwb_cmd_get_codec_mode_t* params
)
{
  uint32_t rc = VS_EOK;
  vs_session_object_t* p_session_obj = NULL;
  apr_lock_t* client_session_lock_p = NULL;

  if (params == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "vs_vocamrwb_cmd_get_codec_mode() - NULL params" );
    return VS_EBADPARAM;
  }
  /* Lock session object destroy process */
  if( ( client_session_lock_p = vs_get_session_destroy_lock( params->handle ) ) == NULL )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_free_buffer() - Invalid handle=(0x%08x)",
           params->handle );
    return VS_EBADPARAM;
  }
  VS_ACQUIRE_LOCK( *client_session_lock_p );

  /* get the session object based on handle*/
  rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                            ( ( vs_object_t** ) &p_session_obj ) );
  if ( rc != VS_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_vocamrwb_cmd_get_codec_mode() - Invalid handle=0x%08x",
           params->handle );
    rc = VS_EBADPARAM;
    return rc;
  }

  /* read the current amrwb codec mode and return */
  VS_ACQUIRE_LOCK( p_session_obj->common_lock );
  *( params->ret_codec_mode ) =
       ( vs_vocamrwb_codec_mode_t )p_session_obj->current_voc.amrwb.codec_mode;
  VS_RELEASE_LOCK( p_session_obj->common_lock );
  VS_RELEASE_LOCK( *client_session_lock_p );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "vs_vocamrwb_cmd_get_codec_mode() - codec_mode=%d",
         *( params->ret_codec_mode ) );

  return VS_EOK;
}  /* end of vs_vocamrwb_cmd_get_codec_mode () */

/**
 * Gets the DTX mode of the AMRWB vocoder.
 *
 * VS_EOK (0) when successful.
 */
VS_INTERNAL uint32_t vs_vocamrwb_cmd_get_dtx_mode (
  vs_vocamrwb_cmd_get_dtx_mode_t* params
)
{

  uint32_t rc = VS_EOK;
  vs_session_object_t* p_session_obj = NULL;
  apr_lock_t* client_session_lock_p = NULL;

  if ( params == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "vs_vocamrwb_cmd_get_dtx_mode() - NULL params" );
    return VS_EBADPARAM;
  }
  /* Lock session object destroy process */
  if( ( client_session_lock_p = vs_get_session_destroy_lock( params->handle ) ) == NULL )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_cmd_free_buffer() - Invalid handle=(0x%08x)",
           params->handle );
    return VS_EBADPARAM;
  }
  VS_ACQUIRE_LOCK( *client_session_lock_p );

  /* get the session object based on handle*/
  rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                            ( ( vs_object_t** ) &p_session_obj ) );
  if ( rc != VS_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_vocamrwb_cmd_get_dtx_mode() - Invalid handle=0x%08x",
           params->handle );
    rc = VS_EBADPARAM;
    return rc;
  }

  /* read the current amrwb dtx mode and return */
  VS_ACQUIRE_LOCK( p_session_obj->common_lock );
  *( params->ret_enable_flag ) =
            ( bool_t )p_session_obj->current_voc.amrwb.dtx_enable;
  VS_RELEASE_LOCK( p_session_obj->common_lock );
  VS_RELEASE_LOCK( *client_session_lock_p );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "vs_vocamrwb_cmd_get_dtx_mode() - dtx_enable=%d",
         *( params->ret_enable_flag ) );

  return VS_EOK;
}  /* end of vs_vocamrwb_cmd_get_dtx_mode () */


/* Conversion tables & macros from VS_AMR-WB to AMRSUP. */

amrsup_frame_type vs_amrwb_to_amrsup_frame_table[9] =
{
  AMRSUP_SPEECH_GOOD,     /* VS_AMR_SPEECH_GOOD */
  AMRSUP_SPEECH_DEGRADED, /* VS_AMR_SPEECH_DEGRADED */
  AMRSUP_ONSET,           /* VS_AMR_ONSET */
  AMRSUP_SPEECH_BAD,      /* VS_AMR_SPEECH_BAD */
  AMRSUP_SID_FIRST,       /* VS_AMR_SID_FIRST */
  AMRSUP_SID_UPDATE,      /* VS_AMR_SID_UPDATE */
  AMRSUP_SID_BAD,         /* VS_AMR_SID_BAD */
  AMRSUP_NO_DATA,         /* VS_AMR_NO_DATA */
  AMRSUP_SPEECH_LOST
};

amrsup_mode_type vs_amrwb_to_amrsup_mode_table[9] =
{
  AMRSUP_MODE_0660,        /* VS_AMR_MODE_0660 */
  AMRSUP_MODE_0885,        /* VS_AMR_MODE_0885 */
  AMRSUP_MODE_1265,        /* VS_AMR_MODE_1265 */
  AMRSUP_MODE_1425,        /* VS_AMR_MODE_1425 */
  AMRSUP_MODE_1585,        /* VS_AMR_MODE_1585 */
  AMRSUP_MODE_1825,        /* VS_AMR_MODE_1825 */
  AMRSUP_MODE_1985,        /* VS_AMR_MODE_1985 */
  AMRSUP_MODE_2305,        /* VS_AMR_MODE_2305 */
  AMRSUP_MODE_2385         /* VS_AMR_MODE_2385 */
};

#define VS_AMRWB_TO_AMRSUP_FRAME(x) vs_amrwb_to_amrsup_frame_table[(x)]
#define VS_AMRWB_TO_AMRSUP_MODE(x) vs_amrwb_to_amrsup_mode_table[(x)]

/**
 * This function will determine number of bytes of AMR-WB vocoder frame length
 * based on the frame type and frame rate.
 *
 * returns "number of bytes" when successful.
 */
VS_INTERNAL uint32_t vs_vocamrwb_get_frame_len (
  vs_vocamrwb_frame_type_t frame_type,
  vs_vocamrwb_codec_mode_t amrwb_mode
)
{

  if ( ( amrwb_mode > VS_VOCAMRWB_CODEC_MODE_2385 ) ||
       ( frame_type > VS_VOCAMRWB_FRAME_TYPE_NO_DATA ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_vocamrwb_get_frame_len(): wrong inputs. amrwb_mode=%d, "
           "frame_type=%d", amrwb_mode, frame_type );
    return 0;
  }

  return ( uint32_t )( amrsup_frame_len(
                             VS_AMRWB_TO_AMRSUP_FRAME( frame_type ),
                             VS_AMRWB_TO_AMRSUP_MODE( amrwb_mode ) ) );

}  /* end of vs_vocamrwb_get_frame_len () */

