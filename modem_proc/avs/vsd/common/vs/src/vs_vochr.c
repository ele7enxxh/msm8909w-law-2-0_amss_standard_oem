/**
  @file vs_vochr.c
  @brief This file contains the definitions of the interfaces for GSM-HR
         vocoder.

*/

/*
  ============================================================================

   Copyright (C) 2013-2015 QUALCOMM Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/vs/src/vs_vochr.c#1 $
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  05/22/14  aim   Fixed a bug in applying vocoder properties.
  07/30/13  sud   Updated software to report MSG_FATAL when fatal error
                  conditions occurs in client context.
  06/26/13  sud   Updated debug messages.
  06/16/13  sud   Addded debug messages.
  06/13/13  hl    Added support for handling ADSP SSR.
  06/12/13  sud   Updated VS to handle errors and status propagation to client.
                  Updated VS to handle memory leak and memory corruption
                  issues.
                  Updated VS to handle command priorities and proper sequencing.
                  Updated VS to add proper debug messages
  04/16/13  sud   VS refactored code and fixed bugs.
  01/03/13  sg    Added functions for HR Vocoder proprties
  01/02/13  sud   Initial revision

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
 * Sets the DTX mode of the hr vocoder.
 *
 * VS_EOK (0) when successful.
 */
VS_INTERNAL uint32_t vs_vochr_cmd_set_dtx_mode (
  vs_vochr_cmd_set_dtx_mode_t* params
)
{

  uint32_t rc = VS_EOK;
  vs_vochr_cmd_set_dtx_mode_t dtx_args;
  vs_session_object_t* p_session_obj = NULL;

  for ( ;; )
  {
    if ( params == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_vochr_cmd_set_dtx_mode() - NULL params" );
      rc = VS_EBADPARAM;
      break;
    }

    /* get the session object based on handle*/
    rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &p_session_obj ) );
    if ( rc != VS_EOK )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_vochr_cmd_set_dtx_mode() -Invalid handle=0x%08x",
             params->handle );
      rc = VS_EBADPARAM;
      break;
    }

    dtx_args.handle = params->handle;
    dtx_args.enable_flag = params->enable_flag;

    rc =  vs_prepare_and_dispatch_cmd_packet( VS_THREAD_PRIORITY_ENUM_MED,
                                              VS_VOCHR_CMD_SET_DTX_MODE,
                                              &dtx_args, sizeof( dtx_args ) );
    break;
  }  /* end of for ( ;; ) */

  return rc;
}  /* end of vs_vochr_cmd_set_dtx_mode () */

/**
 * Sets the DTX mode of the hr vocoder.
 *
 * VS_EOK (0) when successful.
 */
VS_INTERNAL uint32_t vs_vochr_cmd_set_dtx_mode_proc (
  uint32_t handle,
  bool_t dtx_enable
)
{
  uint32_t rc = VS_EOK;
  vss_istream_cmd_set_enc_dtx_mode_t hr_dtx;
  vs_session_object_t* p_session_obj = NULL;
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
    current_voc_dtx_enable = p_session_obj->current_voc.hr.dtx_enable;
    stream_ready = p_session_obj->stream_ready;
    media_id = p_session_obj->media_id;
    pending_media_id = p_session_obj->pending_media_id;
    VS_RELEASE_LOCK( p_session_obj->common_lock );

    /**
     * Send to CVD only if the dtx mode requested is different hrom
     * the current dtx mode.
     */
    if ( current_voc_dtx_enable == ( uint32_t ) dtx_enable )
    {
      /* Invalidate pending_voc since newly set voc properties
       * are identical to current_voc properties.
       */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.hr.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      VS_RELEASE_LOCK( p_session_obj->common_lock );

      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
            "vs_vochr_cmd_set_dtx_mode_proc(): no change in dtx_mode %d",
            dtx_enable );
      break;
    }

    /**
     * Cache and send to CVD immediately when in RUN state and
     * media_id is VS_COMMON_MEDIA_ID_HR.
     * If both conditions are NOT true, cache the value and apply while                    .
     * enabling the stream.
     */
    if ( ( stream_ready == TRUE ) &&
         ( media_id == VS_COMMON_MEDIA_ID_HR ) &&
         ( pending_media_id == VS_COMMON_MEDIA_ID_HR ) )
    {

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_vochr_cmd_set_dtx_mode_proc()-  dtx_mode %d set, previous "
             "mode %d", dtx_enable, current_voc_dtx_enable );

      /* update pending_voc and current_voc */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.hr.dtx_enable = VS_VOC_DTX_MODE_UNDEFINED_V;
      p_session_obj->current_voc.hr.dtx_enable = ( uint32_t )dtx_enable;

      hr_dtx.enable = p_session_obj->current_voc.hr.dtx_enable;
      VS_RELEASE_LOCK( p_session_obj->common_lock );

      /* apply the dtx_mode */
      rc = __aprv2_cmd_alloc_send(
             vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
             vs_my_addr, ( ( uint16_t ) p_session_obj->header.handle ),
             vs_cvs_addr, p_session_obj->cvs_port,
             p_session_obj->header.handle, VSS_ISTREAM_CMD_SET_ENC_DTX_MODE,
             &hr_dtx, sizeof( hr_dtx ) );
      if ( rc )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "vs_vochr_cmd_set_dtx_mode_proc(): Failed to communicate with "
               "CVS with rc=0x%08x", rc );
      }
    }
    else
    {
      /* Cache the dtx_mode now.
       * Apply the dtx_mode while enabling the stream
       */
      VS_ACQUIRE_LOCK( p_session_obj->common_lock );
      p_session_obj->pending_voc.hr.dtx_enable = ( uint32_t )dtx_enable;
      VS_RELEASE_LOCK( p_session_obj->common_lock );
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_vochr_cmd_set_dtx_mode_proc(): dtx_mode saved=%d, current "
             "dtx_mode=%d", dtx_enable, current_voc_dtx_enable );
    }  /* end of if ( session_obj->state == VS_VOC_STATE_ENUM_RUN ) */

    break;
  }  /* end of for ( ;; ) */

  return rc;
}  /* end of vs_vochr_cmd_set_dtx_mode_proc () */

VS_INTERNAL uint32_t vs_vochr_set_cached_voc_property (
  uint32_t handle
)
{

  uint32_t rc = VS_EOK;
  vs_session_object_t* p_session_obj = NULL;
  vss_istream_cmd_set_enc_dtx_mode_t hr_dtx;

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
    if ( ( p_session_obj->pending_voc.hr.dtx_enable ==
                                               VS_VOC_DTX_MODE_UNDEFINED_V ) &&
         ( p_session_obj->current_voc.hr.dtx_enable ==
                                               VS_VOC_DTX_MODE_UNDEFINED_V ) )
    {
      VS_RELEASE_LOCK( p_session_obj->common_lock );
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "HR dtx_mode is not yet set by client - ADSP default "
           "value would be used" );
      break;
    }
    else
    {
      if ( p_session_obj->pending_voc.hr.dtx_enable !=
                                            VS_VOC_DTX_MODE_UNDEFINED_V )
      {
        p_session_obj->current_voc.hr.dtx_enable =
                                    p_session_obj->pending_voc.hr.dtx_enable;
        p_session_obj->pending_voc.hr.dtx_enable =
                                    VS_VOC_DTX_MODE_UNDEFINED_V;
      }
      else
      {
        /* we reach here if current_voc.hr.codec_mode is valid (may be
         * after ADSP SSR)
         * in this case, just send current_voc.hr.codec_mode to ADSP */
      }
    }

    /* apply the codec_mode */
    hr_dtx.enable = p_session_obj->current_voc.hr.dtx_enable;
    VS_RELEASE_LOCK( p_session_obj->common_lock );

    rc = __aprv2_cmd_alloc_send(
           vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           vs_my_addr, ( ( uint16_t ) p_session_obj->header.handle ),
           vs_cvs_addr, p_session_obj->cvs_port,
           p_session_obj->header.handle, VSS_ISTREAM_CMD_SET_ENC_DTX_MODE,
           &hr_dtx, sizeof( hr_dtx ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_vochr_set_cached_voc_property(): Failed to communicate with "
             "CVS with rc=0x%08x", rc );
      break;
    }

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
          "vs_vochr_set_cached_voc_property(): dtx_mode applied %d",
          hr_dtx.enable );
    break;
  }  /* end of for ( ;; ) */

  return rc;
}  /* end of vs_vochr_set_cached_voc_property () */

/**
 * Gets the DTX mode of the hr vocoder.
 *
 * VS_EOK (0) when successful.
 */
VS_INTERNAL uint32_t vs_vochr_cmd_get_dtx_mode (
  vs_vochr_cmd_get_dtx_mode_t* params
)
{
  uint32_t rc = VS_EOK;
  vs_session_object_t *p_session_obj = NULL;
  apr_lock_t* client_session_lock_p = NULL;

  if ( params == NULL )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "vs_vochr_cmd_get_dtx_mode() - NULL params" );
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

  /*get the session object based on handle*/
  rc = vs_get_typed_object( params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                            ( ( vs_object_t** ) &p_session_obj ) );
  if ( rc != VS_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_vochr_cmd_get_dtx_mode() - Invalid handle=0x%08x",
            params->handle );
    rc = VS_EBADPARAM;
    return rc;
  }

  /* read the current hr dtx mode and return */
  VS_ACQUIRE_LOCK( p_session_obj->common_lock );
  *( params->ret_enable_flag ) =
                      ( bool_t )p_session_obj->current_voc.hr.dtx_enable;
  VS_RELEASE_LOCK( p_session_obj->common_lock );
  VS_RELEASE_LOCK( *client_session_lock_p );

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "vs_vochr_cmd_get_dtx_mode() - dtx_enable=%d",
        *( params->ret_enable_flag ) );

  return VS_EOK;
}  /* end of vs_vochr_cmd_get_dtx_mode () */

