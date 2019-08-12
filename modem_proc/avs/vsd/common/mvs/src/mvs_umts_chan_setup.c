/*
  Copyright (C) 2009 - 2014 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/mvs/src/mvs_umts_chan_setup.c#1 $
  $Author: mplcsds1 $
*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/01/13   sud     Fixed compiler warnings.
07/05/11   db      Initial revision
==========================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include <stddef.h>
#include "err.h"
#include "task.h"
#include "mvs.h"
#include "mvsi.h"
#include "mvs_module.h"
#include "mvs_voc_def.h"
#include "mvsdsm.h"
#include "mvsamr.h"
#include "mvs_umts_chan_setup.h"

#include "log.h"
#include "log_codes_umts.h"

#define USE_WCDMAMVSIF_CMI_NONCOMPLIANCE_HACK
#ifdef USE_WCDMAMVSIF_CMI_NONCOMPLIANCE_HACK
  #include "queue.h"
#endif /* USE_WCDMAMVSIF_CMI_NONCOMPLIANCE_HACK */
#include "wcdmamvsif.h"

#include "tdsl2external.h"
#ifndef WINSIM
#include "wcdma_ext_api.h"
#endif

/* Definitions of MVS client type */
typedef enum {
  MVS_CHAN_CLIENT_TDSCDMA,
  MVS_CHAN_CLIENT_WCDMA,
  MVS_CHAN_CLIENT_NONE
} mvs_channel_setup_client_type;

/* TDSCDMA and WCDMA Channels */
mvssup_umts_chan_state_type mvs_ul_umts_chan_state;
mvssup_umts_chan_state_type mvs_dl_umts_chan_state;

static rlc_lc_id_type m_ul_lc_class_a,m_ul_lc_class_b, m_ul_lc_class_c;
static mvssup_umts_chan_class_type m_ul_chan_class;

static rlc_lc_id_type m_dl_lc_class_a,m_dl_lc_class_b, m_dl_lc_class_c;
static mvssup_umts_chan_class_type m_dl_chan_class;
static mvs_channel_setup_client_type mvs_chan_setup_ul_client = MVS_CHAN_CLIENT_NONE;
static mvs_channel_setup_client_type mvs_chan_setup_dl_client = MVS_CHAN_CLIENT_NONE;

/* Determine channel composition based on channel class. */
static mvs_amr_chan_state_type mvs_mod_umts_chan_class_to_state (
  mvssup_umts_chan_class_type chan_class
)
{
  mvs_amr_chan_state_type chan_state = { FALSE, FALSE, FALSE };

  switch ( chan_class )
  {
  case MVSSUP_UMTS_CHAN_CLASS_ABC:
    chan_state.has_chan_c = TRUE;
    /*-fallthru */

  case MVSSUP_UMTS_CHAN_CLASS_AB:
    chan_state.has_chan_b = TRUE;
    /*-fallthru */

  case MVSSUP_UMTS_CHAN_CLASS_A:
    chan_state.has_chan_a = TRUE;
    /*-fallthru */

  case MVSSUP_UMTS_CHAN_CLASS_NONE:
    break;

  default:
    MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvs_mod_umts_chan_class_to_state(): invalid channel class %d",
                                            chan_class );
    return chan_state;
  }

  return chan_state;
}

int32_t mvs_wcdma_set_ul_channel (
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_umts_chan_class_type chan_class
)
{

#ifndef WINSIM
  uint32_t rc;
  uint8 nchan = 0;
  mvs_amr_chan_state_type chan_state;
  mvs_dsm_amr_queue_type* ul_queues_ptr = NULL;
  wcdma_cmd_l2_ul_cmd_buffer_t ul_reg_cmd_buf;
  wcdma_cmd_l2_ul_cmd_buffer_t ul_dereg_cmd_buf;

  m_ul_lc_class_a = lc_class_a;
  m_ul_lc_class_b = lc_class_b;
  m_ul_lc_class_c = lc_class_c;
  m_ul_chan_class = chan_class;

  MSG(MSG_SSID_DFLT, MSG_LEGACY_LOW, "mvssup_wcdma_set_ul_channel()" );

  chan_state = mvs_mod_umts_chan_class_to_state( chan_class );

  switch ( mvs_chan_setup_ul_client )
  {
  case MVS_CHAN_CLIENT_WCDMA:
  case MVS_CHAN_CLIENT_NONE:
    {
      switch ( chan_class )
      {
      case MVSSUP_UMTS_CHAN_CLASS_NONE:
        mvs_chan_setup_ul_client = MVS_CHAN_CLIENT_NONE;
        break;

      default:
        mvs_chan_setup_ul_client = MVS_CHAN_CLIENT_WCDMA;
        break;
      }
    }
    break;

  default:
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvs_wcdma_set_ul_channel():UL DSM queues currently "
                                          "in use by another client. Operation failed" );
    return APR_EFAILED;
  }

  /* If there are valid channels then get the ptr of upload DSM queue */
  if ( chan_class != MVSSUP_UMTS_CHAN_CLASS_NONE )
  {
#ifndef WINSIM
    ul_queues_ptr = mvs_dsm_amr_get_ul_q( );
#endif /* !WINSIM */
    if ( ul_queues_ptr == NULL )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Failed in getting uplink DSM watermarks." );
      return APR_EFAILED ;
    }
  }

  /* Now we get the uplink channels */
  /* Now De-register the channels which are not in use any more */
  /* and re-register new channels */
  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "Begin Register/Deregister of WCDMA L2 uplink channels." );
  {
  /* ========== De-register logical channels if necessary ==========
     L2 Layer works on command buffer
     Get the command packet using WCDMA_GET_L2_UL_CMD_BUFFER ;
     Fill it with Command header (cmd_hdr.cmd_id) =  RLC_UL_DEREGISTER_SRVC_REQ
     fill appropriate cmd_data.(ul_dereg) members
     and dispatch it WCDMA_PUT_L2_UL_CMD */
    ul_dereg_cmd_buf.service = NULL;
    rc  = wcdma_ext_api( WCDMA_GET_L2_UL_CMD_BUFFER, &ul_dereg_cmd_buf,
                         sizeof( ul_dereg_cmd_buf ) );
    if( ( rc != WCDMA_STATUS_GOOD ) || ( ul_dereg_cmd_buf.service == NULL ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "Couldn't get WCMDA L2 uplink command buffer for dereg. rc = (0x%08X)",
       	     rc );
      return APR_EFAILED;
    }

  /*<- command id for deregister these channels */
    ul_dereg_cmd_buf.service->cmd_hdr.cmd_id = RLC_UL_DEREGISTER_SRVC_REQ;

  /* deregister class A channel if necessary */
    if ( mvs_ul_umts_chan_state.chan_state.has_chan_a == TRUE )
    {
      if ( ( chan_state.has_chan_a == FALSE ) ||
           ( mvs_ul_umts_chan_state.lca != lc_class_a ) )
      {
        ul_dereg_cmd_buf.service->cmd_data.ul_dereg.rlc_id[nchan] =
                                                     mvs_ul_umts_chan_state.lca;
        nchan++;
        mvs_ul_umts_chan_state.chan_state.has_chan_a = FALSE;
      }
    }

  /* deregister class B channel if necessary */
    if ( mvs_ul_umts_chan_state.chan_state.has_chan_b == TRUE )
    {
      if ( ( chan_state.has_chan_b == FALSE ) ||
           ( mvs_ul_umts_chan_state.lcb != lc_class_b ) )
      {
        ul_dereg_cmd_buf.service->cmd_data.ul_dereg.rlc_id[nchan] =
                                                    mvs_ul_umts_chan_state.lcb;
        nchan++;
        mvs_ul_umts_chan_state.chan_state.has_chan_b = FALSE;
      }
    }

  /* deregister class C channel if necessary */
    if ( mvs_ul_umts_chan_state.chan_state.has_chan_c == TRUE )
    {
      if ( ( chan_state.has_chan_c == FALSE ) ||
           ( mvs_ul_umts_chan_state.lcc != lc_class_c ) )
      {
        ul_dereg_cmd_buf.service->cmd_data.ul_dereg.rlc_id[nchan] =
                                                     mvs_ul_umts_chan_state.lcc;
        nchan++;
        mvs_ul_umts_chan_state.chan_state.has_chan_c = FALSE;
      }
    }

    ul_dereg_cmd_buf.service->cmd_data.ul_dereg.nchan = nchan;

    rc  = wcdma_ext_api( WCDMA_PUT_L2_UL_CMD, &ul_dereg_cmd_buf,
                         sizeof( ul_dereg_cmd_buf ) );
    if( rc == WCDMA_STATUS_GOOD )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "WCDMA L2 UL De-registration successful. Number of WCDMA L2 uplink " 
             "logical channels, nchan = (%d).", nchan );
    }
    else
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "WCDMA L2 UL De-registration failed, rc = (0x%08X). Number of "
             "WCDMA L2 uplink logical channels, nchan = (%d).", rc, nchan );
    }
  }

  /* ========== Register logical channels if necessary =============
     L2 Layer works on command buffer
     Get the command packet using WCDMA_GET_L2_UL_CMD_BUFFER ;
     Fill it with Command header (cmd_hdr.cmd_id) =  RLC_UL_REGISTER_SRVC_REQ
     fill appropriate cmd_data.(ul_reg) members
     and dispatch it WCDMA_PUT_L2_UL_CMD */
  {
    nchan = 0;
    ul_reg_cmd_buf.service = NULL;
    rc  = wcdma_ext_api( WCDMA_GET_L2_UL_CMD_BUFFER, &ul_reg_cmd_buf,
                         sizeof( ul_reg_cmd_buf ) );
    if( ( rc != WCDMA_STATUS_GOOD ) || ( ul_reg_cmd_buf.service == NULL ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "Couldn't get WCMDA L2 uplink command buffer for reg. rc = (0x%08X)",
             rc );
      return APR_EFAILED;
    }

    ul_reg_cmd_buf.service->cmd_hdr.cmd_id = RLC_UL_REGISTER_SRVC_REQ;

  /* Register class A channel if necessary */
    if ( ( chan_state.has_chan_a == TRUE )
         && ( mvs_ul_umts_chan_state.chan_state.has_chan_a == FALSE ) )
    {
      ul_reg_cmd_buf.service->cmd_data.ul_reg.rlc_data[nchan].lc_id = lc_class_a;
      ul_reg_cmd_buf.service->cmd_data.ul_reg.rlc_data[nchan].ul_wm_ptr =
                                                              &ul_queues_ptr->wm_a;
      nchan++;
      mvs_ul_umts_chan_state.lca = lc_class_a;
      mvs_ul_umts_chan_state.chan_state.has_chan_a = TRUE;
    }

  /* Register class B channel if necessary */
    if ( ( chan_state.has_chan_b == TRUE )
         && ( mvs_ul_umts_chan_state.chan_state.has_chan_b == FALSE ) )
    {
      ul_reg_cmd_buf.service->cmd_data.ul_reg.rlc_data[nchan].lc_id = lc_class_b;
      ul_reg_cmd_buf.service->cmd_data.ul_reg.rlc_data[nchan].ul_wm_ptr =
                                                              &ul_queues_ptr->wm_b;
      nchan++;
      mvs_ul_umts_chan_state.lcb = lc_class_b;
      mvs_ul_umts_chan_state.chan_state.has_chan_b = TRUE;
    }

  /* Register class C channel if necessary */
    if ( ( chan_state.has_chan_c == TRUE )
         && ( mvs_ul_umts_chan_state.chan_state.has_chan_c == FALSE ) )
    {
      ul_reg_cmd_buf.service->cmd_data.ul_reg.rlc_data[nchan].lc_id = lc_class_c;
      ul_reg_cmd_buf.service->cmd_data.ul_reg.rlc_data[nchan].ul_wm_ptr =
                                                              &ul_queues_ptr->wm_c;
      nchan++;
      mvs_ul_umts_chan_state.lcc = lc_class_c;
      mvs_ul_umts_chan_state.chan_state.has_chan_c = TRUE;
    }

    ul_reg_cmd_buf.service->cmd_data.ul_reg.nchan = nchan;

    rc  = wcdma_ext_api( WCDMA_PUT_L2_UL_CMD, &ul_reg_cmd_buf,
                         sizeof( ul_reg_cmd_buf ) );
    if( rc == WCDMA_STATUS_GOOD )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "WCDMA L2 UL Registration successful. Number of WCDMA L2 uplink " 
             "logical channels, nchan = (%d).", nchan );
    }
    else
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "WCDMA L2 UL Registration failed, rc = (0x%08X). Number of "
             "WCDMA L2 uplink logical channels, nchan = (%d).", rc, nchan );
    }

    MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "De-register/Register of WCDMA L2 uplink channels completed" );
  }

  /* verification */
  if ( mvs_amr_cmp_chan_state( &chan_state, &mvs_ul_umts_chan_state.chan_state ) != TRUE )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "WCDMA UL channels setup errors!" );
    return APR_EFAILED;
  }

  MSG(MSG_SSID_DFLT, MSG_LEGACY_LOW, "mvssup_wcdma_set_ul_channel(): complete" );
#endif /* !WINSIM*/

  return APR_EOK;

}

int32_t mvs_wcdma_set_dl_channel (
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_umts_chan_class_type chan_class,
  mvssup_umts_blk_size_cb_set_type *blk_size_cb_set
)
{

#ifndef WINSIM
  uint32_t rc;
  uint8 nchan = 0;
  mvs_amr_chan_state_type chan_state;
  mvs_dsm_amr_queue_type* dl_queues_ptr = NULL;
  wcdma_cmd_l2_dl_cmd_buffer_t dl_reg_cmd_buf;
  wcdma_cmd_l2_dl_cmd_buffer_t dl_dereg_cmd_buf;

  m_dl_lc_class_a = lc_class_a;
  m_dl_lc_class_b = lc_class_b;
  m_dl_lc_class_c = lc_class_c;
  m_dl_chan_class = chan_class;

  MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "mvs_wcdma_set_dl_channel()" );

  chan_state = mvs_mod_umts_chan_class_to_state( chan_class );

  switch ( mvs_chan_setup_dl_client )
  {
  case MVS_CHAN_CLIENT_WCDMA:
  case MVS_CHAN_CLIENT_NONE:
    {
      switch ( chan_class )
      {
      case MVSSUP_UMTS_CHAN_CLASS_NONE:
        mvs_chan_setup_dl_client = MVS_CHAN_CLIENT_NONE;
        break;

      default:
        mvs_chan_setup_dl_client = MVS_CHAN_CLIENT_WCDMA;
        break;
      }
    }
    break;

  default:
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvs_wcdma_set_dl_channel():UL DSM queues currently "
                                          "in use by another client. Operation failed" );
    return APR_EFAILED;
  }

  if ( chan_class != MVSSUP_UMTS_CHAN_CLASS_NONE )
  {
#ifndef WINSIM
    dl_queues_ptr = mvs_dsm_amr_get_dl_q( );
#endif /* !WINSIM */
    if ( dl_queues_ptr == NULL )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Failed in getting downlink DSM watermarks." );
      return APR_EFAILED;
    }
  }

  /* Now we get the downlink channels */
  /* Now De-register the channels which are not in use any more */
  /* and re-register new channels */
  MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
       "Begin Register/Deregister of WCDMA L2 downlink channels." );
  {
    /* ========== De-register logical channels if necessary ==========
    L2 Layer works on command buffer
    Get the command packet using WCDMA_GET_L2_DL_CMD_BUFFER ;
    Fill it with Command header (cmd_hdr.cmd_id) =  RLC_DL_DEREGISTER_SRVC_REQ
    fill appropriate cmd_data.(dl_dereg) members
    and dispatch it WCDMA_PUT_L2_DL_CMD */
    dl_dereg_cmd_buf.service = NULL;
    rc  = wcdma_ext_api( WCDMA_GET_L2_DL_CMD_BUFFER, &dl_dereg_cmd_buf,
                         sizeof( dl_dereg_cmd_buf ) );
    if( ( rc != WCDMA_STATUS_GOOD ) || ( dl_dereg_cmd_buf.service == NULL ) )
    {
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
              "Couldn't get WCMDA L2 downlink command buffer for dereg. rc = (0x%08X)",
              rc );
    return APR_EFAILED;
  }

    /*<- command id for deregister these channels */
    dl_dereg_cmd_buf.service->cmd_hdr.cmd_id = RLC_DL_DEREGISTER_SRVC_REQ;

  /* deregister class A channel if necessary */
    if ( mvs_dl_umts_chan_state.chan_state.has_chan_a == TRUE )
    {
      if ( ( chan_state.has_chan_a == FALSE ) ||
           ( mvs_dl_umts_chan_state.lca != lc_class_a ) )
      {
        dl_dereg_cmd_buf.service->cmd_data.dl_dereg.rlc_id[nchan] =
                                                    mvs_dl_umts_chan_state.lca;
        nchan++;
        mvs_dl_umts_chan_state.chan_state.has_chan_a = FALSE;
      }
    }

  /* deregister class B channel if necessary */
    if ( mvs_dl_umts_chan_state.chan_state.has_chan_b == TRUE )
    {
      if ( ( chan_state.has_chan_b == FALSE )
           || ( mvs_dl_umts_chan_state.lcb != lc_class_b ) )
      {
        dl_dereg_cmd_buf.service->cmd_data.dl_dereg.rlc_id[nchan] =
                                                    mvs_dl_umts_chan_state.lcb;
        nchan++;
        mvs_dl_umts_chan_state.chan_state.has_chan_b = FALSE;
      }
    }

  /* deregister class C channel if necessary */
    if ( mvs_dl_umts_chan_state.chan_state.has_chan_c == TRUE )
    {
      if ( ( chan_state.has_chan_c == FALSE )
           || ( mvs_dl_umts_chan_state.lcc != lc_class_c ) )
      {
        dl_dereg_cmd_buf.service->cmd_data.dl_dereg.rlc_id[nchan] =
                                                    mvs_dl_umts_chan_state.lcc;
        nchan++;
        mvs_dl_umts_chan_state.chan_state.has_chan_c = FALSE;
      }
    }

    dl_dereg_cmd_buf.service->cmd_data.dl_dereg.nchan = nchan;

    rc  = wcdma_ext_api( WCDMA_PUT_L2_DL_CMD, &dl_dereg_cmd_buf,
                         sizeof( dl_dereg_cmd_buf ) );
    if( rc == WCDMA_STATUS_GOOD )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "WCDMA L2 DL De-registration successful. Number of WCDMA L2 downlink " 
             "logical channels, nchan = (%d).", nchan );
    }
    else
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "WCDMA L2 DL De-registration failed, rc = (0x%08X). Number of "
             "WCDMA L2 downlink logical channels, nchan = (%d).", rc, nchan );
    }
  }
  /* ========== Register logical channels if necessary ============= 
     L2 Layer works on command buffer
     Get the command packet using WCDMA_GET_L2_DL_CMD_BUFFER ;
     Fill it with Command header (cmd_hdr.cmd_id) =  RLC_DL_REGISTER_SRVC_REQ
     fill appropriate cmd_data.(dl_reg) members
     and dispatch it WCDMA_PUT_L2_DL_CMD */
  {
    nchan = 0;
    dl_reg_cmd_buf.service = NULL;
    rc  = wcdma_ext_api( WCDMA_GET_L2_DL_CMD_BUFFER, &dl_reg_cmd_buf,
                         sizeof( dl_reg_cmd_buf ) );
    if( ( rc != WCDMA_STATUS_GOOD ) || ( dl_reg_cmd_buf.service == NULL ) )
    {
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
              "Couldn't get WCMDA L2 downlink command buffer for reg. rc = (0x%08X)",
              rc );
       return APR_EFAILED;
    }

    dl_reg_cmd_buf.service->cmd_hdr.cmd_id = RLC_DL_REGISTER_SRVC_REQ;

  /* Register class A channel if necessary */
    if ( ( chan_state.has_chan_a == TRUE )
         && ( mvs_dl_umts_chan_state.chan_state.has_chan_a == FALSE ) )
    {
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].lc_id = lc_class_a;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].dl_wm_ptr = &dl_queues_ptr->wm_a;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].context = TRUE; /* in isr */
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].rlc_post_rx_proc_func_ptr = NULL;
      nchan++;
      mvs_dl_umts_chan_state.lca = lc_class_a;
      mvs_dl_umts_chan_state.chan_state.has_chan_a = TRUE;
    }

  /* Register class B channel if necessary */
    if ( ( chan_state.has_chan_b == TRUE )
         && ( mvs_dl_umts_chan_state.chan_state.has_chan_b == FALSE ) )
    {
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].lc_id = lc_class_b;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].dl_wm_ptr = &dl_queues_ptr->wm_b;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].context = TRUE; /* in isr */
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].rlc_post_rx_proc_func_ptr = NULL;
      nchan++;
      mvs_dl_umts_chan_state.lcb = lc_class_b;
      mvs_dl_umts_chan_state.chan_state.has_chan_b = TRUE;
    }

  /* Register class C channel if necessary */
    if ( ( chan_state.has_chan_c == TRUE )
         && ( mvs_dl_umts_chan_state.chan_state.has_chan_c == FALSE ) )
    {
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].lc_id = lc_class_c;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].dl_wm_ptr = &dl_queues_ptr->wm_c;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].context = TRUE; /* in isr */
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].rlc_post_rx_proc_func_ptr = NULL;
      nchan++;
      mvs_dl_umts_chan_state.lcc = lc_class_c;
      mvs_dl_umts_chan_state.chan_state.has_chan_c = TRUE;
    }

    dl_reg_cmd_buf.service->cmd_data.dl_reg.nchan = nchan;

    rc  = wcdma_ext_api( WCDMA_PUT_L2_DL_CMD, &dl_reg_cmd_buf,
                            sizeof( dl_reg_cmd_buf ) );
					 
    if( rc == WCDMA_STATUS_GOOD )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "WCDMA L2 DL Registration successful. Number of WCDMA L2 downlink " 
             "logical channels, nchan = (%d).", nchan );
    }
    else
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "WCDMA L2 DL Registration failed, rc = (0x%08X). Number of "
             "WCDMA L2 downlink logical channels, nchan = (%d).", rc, nchan );
    }

    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "De-register/Register of WCDMA L2 downlink channels completed" );
  }

  /* verification */
  if ( mvs_amr_cmp_chan_state( &chan_state, &mvs_dl_umts_chan_state.chan_state ) != TRUE )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "WCDMA DL channels setup errors!" );
  }

  MSG(MSG_SSID_DFLT, MSG_LEGACY_LOW, "mvssup_wcdma_set_dl_channel() complete" );
#endif /* !WINSIM */

  return APR_EOK;
}

int32_t mvs_tdscdma_set_ul_channel (
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_umts_chan_class_type chan_class
)
{

#ifndef WINSIM
  uint32_t rc;
  uint8 nchan = 0;
  mvs_amr_chan_state_type chan_state;
  mvs_dsm_amr_queue_type* ul_queues_ptr = NULL;
  tdsl2_ul_service_register_type ul_service_dereg;
  tdsl2_ul_service_register_type ul_service_reg;
  tdsl2_cmd_ul_service_register_t tds_ul_service_register_param;
  m_ul_lc_class_a = lc_class_a;
  m_ul_lc_class_b = lc_class_b;
  m_ul_lc_class_c = lc_class_c;
  m_ul_chan_class = chan_class;

  MSG(MSG_SSID_DFLT, MSG_LEGACY_LOW, "mvssup_tdscdma_set_ul_channel()" );

  chan_state = mvs_mod_umts_chan_class_to_state( chan_class );

  switch ( mvs_chan_setup_ul_client )
  {
  case MVS_CHAN_CLIENT_TDSCDMA:
  case MVS_CHAN_CLIENT_NONE:
    {
      switch ( chan_class )
      {
      case MVSSUP_UMTS_CHAN_CLASS_NONE:
        mvs_chan_setup_ul_client = MVS_CHAN_CLIENT_NONE;
        break;

      default:
        mvs_chan_setup_ul_client = MVS_CHAN_CLIENT_TDSCDMA;
        break;
      }
    }
    break;

  default:
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvs_tdscdma_set_ul_channel():UL DSM queues currently "
                                          "in use by another client. Operation failed" );
    return APR_EFAILED;
  }

  /* If there are valid channels then get the ptr of upload DSM queue. */
  if ( chan_class != MVSSUP_UMTS_CHAN_CLASS_NONE )
  {
#ifndef WINSIM
    ul_queues_ptr = mvs_dsm_amr_get_ul_q( );
#endif /* !WINSIM */
    if ( ul_queues_ptr == NULL )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Failed in getting uplink DSM watermarks." );
      return APR_EFAILED;
    }
  }

  /* Now we get the uplink channels */
  /* Now De-register the channels which are not in use any more */
  /* and re-register new channels */

  MSG(MSG_SSID_DFLT, MSG_LEGACY_LOW, "Begin Register/Deregister of UL TDSCDMA channels." );

  /* ========== De-register logical channels if necessary ========= */
  ul_service_dereg.service = FALSE;

  /* deregister class A channel if necessary */
  if ( mvs_ul_umts_chan_state.chan_state.has_chan_a == TRUE )
  {
    if ( ( chan_state.has_chan_a == FALSE )
         || ( mvs_ul_umts_chan_state.lca != lc_class_a ) )
    {
      ul_service_dereg.rlc_id[nchan] = mvs_ul_umts_chan_state.lca;
      nchan++;
      mvs_ul_umts_chan_state.chan_state.has_chan_a = FALSE;
    }
  }

  /* deregister class B channel if necessary */
  if ( mvs_ul_umts_chan_state.chan_state.has_chan_b == TRUE )
  {
    if ( ( chan_state.has_chan_b == FALSE )
         || ( mvs_ul_umts_chan_state.lcb != lc_class_b ) )
    {
      ul_service_dereg.rlc_id[nchan] = mvs_ul_umts_chan_state.lcb;
      nchan++;
      mvs_ul_umts_chan_state.chan_state.has_chan_b = FALSE;
    }
  }

  /* deregister class C channel if necessary */
  if ( mvs_ul_umts_chan_state.chan_state.has_chan_c == TRUE )
  {
    if ( ( chan_state.has_chan_c == FALSE )
         || ( mvs_ul_umts_chan_state.lcc != lc_class_c ) )
    {
      ul_service_dereg.rlc_id[nchan] = mvs_ul_umts_chan_state.lcc;
      nchan++;
      mvs_ul_umts_chan_state.chan_state.has_chan_c = FALSE;
    }
  }

  ul_service_dereg.nchan = nchan;

  if ( nchan > 0 )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_LOW, "De-registering old UL TDSCDMA channels." );
  }

#ifndef WINSIM
  tds_ul_service_register_param.service = &ul_service_dereg;
  rc = tdscdma_l2_api_call( TDSL2_CMD_UL_SERVICE_REGISTER,&tds_ul_service_register_param,
                            sizeof( tds_ul_service_register_param ) );
  if( rc == TDS_EOK )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Deregister L2 uplink" );
  }
  else
#endif /* !WINSIM */
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Couldn't deregister L2 uplink" );
  }

  /* ========== Register logical channels if necessary ============= */
  nchan = 0;

  ul_service_reg.service = TRUE;

  /* Register class A channel if necessary */
  if ( chan_state.has_chan_a == TRUE
       /* && ( mvs_ul_umts_chan_state.chan_state.has_chan_a == FALSE ) */ )
  {
    ul_service_reg.rlc_id[nchan] = lc_class_a;
    ul_service_reg.ul_wm_ptr[nchan] = &ul_queues_ptr->wm_a;
    nchan++;
    mvs_ul_umts_chan_state.lca = lc_class_a;
    mvs_ul_umts_chan_state.chan_state.has_chan_a = TRUE;
  }

  /* Register class B channel if necessary */
  if ( chan_state.has_chan_b == TRUE
       /* && ( mvs_ul_umts_chan_state.chan_state.has_chan_b == FALSE ) */ )
  {
    ul_service_reg.rlc_id[nchan] = lc_class_b;
    ul_service_reg.ul_wm_ptr[nchan] = &ul_queues_ptr->wm_b;
    nchan++;
    mvs_ul_umts_chan_state.lcb = lc_class_b;
    mvs_ul_umts_chan_state.chan_state.has_chan_b = TRUE;
  }

  /* Register class C channel if necessary */
  if ( chan_state.has_chan_c == TRUE
       /* && ( mvs_ul_umts_chan_state.chan_state.has_chan_c == FALSE ) */ )
  {
    ul_service_reg.rlc_id[nchan] = lc_class_c;
    ul_service_reg.ul_wm_ptr[nchan] = &ul_queues_ptr->wm_c;
    nchan++;
    mvs_ul_umts_chan_state.lcc = lc_class_c;
    mvs_ul_umts_chan_state.chan_state.has_chan_c = TRUE;
  }

  ul_service_reg.nchan = nchan;

  MSG(MSG_SSID_DFLT, MSG_LEGACY_LOW, "Sending UL register request" );

#ifndef WINSIM
  tds_ul_service_register_param.service = &ul_service_reg;
  rc = tdscdma_l2_api_call( TDSL2_CMD_UL_SERVICE_REGISTER,&tds_ul_service_register_param,
                            sizeof( tds_ul_service_register_param ) );
  if( rc == TDS_EOK )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Register TDSCDMA UL channels done successfully" );
  }
  else
#endif /* !WINSIM */
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Couldn't register L2 uplink" );
  }

  /* verification */
  if ( mvs_amr_cmp_chan_state( &chan_state, &mvs_ul_umts_chan_state.chan_state ) != TRUE )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "TDSCDMA UL channels setup errors!" );
  }

  MSG(MSG_SSID_DFLT, MSG_LEGACY_LOW, "mvs_tdscdma_set_ul_channel(): complete" );
#endif  /* !WINSIM */

  return APR_EOK;
}

int32_t mvs_tdscdma_set_dl_channel (
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_umts_chan_class_type chan_class,
  mvssup_umts_blk_size_cb_set_type* blk_size_cb_set
)
{

#ifndef WINSIM
  uint32_t rc;
  uint8 nchan = 0;
  mvs_amr_chan_state_type chan_state;
  mvs_dsm_amr_queue_type* dl_queues_ptr = NULL;
  tdsl2_dl_service_register_type dl_service_dereg;
  tdsl2_dl_service_register_type dl_service_reg;
  tdsl2_cmd_dl_service_register_t tds_dl_service_register_param;

  m_dl_lc_class_a = lc_class_a;
  m_dl_lc_class_b = lc_class_b;
  m_dl_lc_class_c = lc_class_c;
  m_dl_chan_class = chan_class;

  MSG(MSG_SSID_DFLT, MSG_LEGACY_LOW, "mvssup_tdscdma_set_dl_channel()" );

  chan_state = mvs_mod_umts_chan_class_to_state( chan_class );

  switch ( mvs_chan_setup_dl_client )
  {
  case MVS_CHAN_CLIENT_TDSCDMA:
  case MVS_CHAN_CLIENT_NONE:
    {
      switch ( chan_class )
      {
      case MVSSUP_UMTS_CHAN_CLASS_NONE:
        mvs_chan_setup_dl_client = MVS_CHAN_CLIENT_NONE;
        break;

      default:
        mvs_chan_setup_dl_client = MVS_CHAN_CLIENT_TDSCDMA;
        break;
      }
    }
    break;

  default:
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvs_tdscdma_set_dl_channel():UL DSM queues currently "
                                          "in use by another client. Operation failed" );
    return APR_EFAILED;
  }

  if ( chan_class != MVSSUP_UMTS_CHAN_CLASS_NONE )
  {
#ifndef WINSIM
    dl_queues_ptr = mvs_dsm_amr_get_dl_q( );
#endif /* !WINSIM */
    if ( dl_queues_ptr == NULL )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Failed in getting downlink DSM watermarks." );
      return APR_EFAILED;
    }
  }

  dl_service_dereg.service = FALSE;

  /* deregister class A channel if necessary */
  if ( mvs_dl_umts_chan_state.chan_state.has_chan_a == TRUE )
  {
    if ( ( chan_state.has_chan_a == FALSE )
         || ( mvs_dl_umts_chan_state.lca != lc_class_a ) )
    {
      dl_service_dereg.rlc_id[nchan] = mvs_dl_umts_chan_state.lca;
      nchan++;
      mvs_dl_umts_chan_state.chan_state.has_chan_a = FALSE;
    }
  }

  /* deregister class B channel if necessary  */
  if ( mvs_dl_umts_chan_state.chan_state.has_chan_b == TRUE )
  {
    if ( ( chan_state.has_chan_b == FALSE )
         || ( mvs_dl_umts_chan_state.lcb != lc_class_b ) )
    {
      dl_service_dereg.rlc_id[nchan] = mvs_dl_umts_chan_state.lcb;
      nchan++;
      mvs_dl_umts_chan_state.chan_state.has_chan_b = FALSE;
    }
  }

  /* deregister class C channel if necessary */
  if ( mvs_dl_umts_chan_state.chan_state.has_chan_c == TRUE )
  {
    if ( ( chan_state.has_chan_c == FALSE )
         || ( mvs_dl_umts_chan_state.lcc != lc_class_c ) )
    {
      dl_service_dereg.rlc_id[nchan] = mvs_dl_umts_chan_state.lcc;
      nchan++;
      mvs_dl_umts_chan_state.chan_state.has_chan_c = FALSE;
    }
  }

  dl_service_dereg.nchan = nchan;

  if ( nchan > 0 )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_LOW, "De-registering old DL TDSCDMA channels." );
  }

#ifndef WINSIM
  tds_dl_service_register_param.service = &dl_service_dereg;
  rc = tdscdma_l2_api_call( TDSL2_CMD_DL_SERVICE_REGISTER,&tds_dl_service_register_param,
                            sizeof( tds_dl_service_register_param ) );

  if ( rc == TDS_EOK  )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "De-Register of TDSCDMA DL channels done successfully" );
  }
  else
#endif /* !WINSIM */
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Couldn't deregister L2 downlink" );
  }

  /* ========== Register logical channels if necessary ============= */
  nchan = 0;

  dl_service_reg.service = TRUE;

  /* Register class A channel if necessary */
  if ( chan_state.has_chan_a == TRUE
       /*&& ( mvs_dl_umts_chan_state.chan_state.has_chan_a == FALSE )*/ )
  {
    dl_service_reg.rlc_id[nchan] = lc_class_a;
    dl_service_reg.dl_wm_ptr[nchan]= &(dl_queues_ptr->wm_a);
    dl_service_reg.context[nchan] = TRUE; /* in isr */
    dl_service_reg.rlc_post_rx_proc_func_ptr[nchan] = NULL;
    nchan++;
    mvs_dl_umts_chan_state.lca = lc_class_a;
    mvs_dl_umts_chan_state.chan_state.has_chan_a = TRUE;
  }

  /* Register class B channel if necessary */
  if ( chan_state.has_chan_b == TRUE
       /* && ( mvs_dl_umts_chan_state.chan_state.has_chan_b == FALSE ) */ )
  {
    dl_service_reg.rlc_id[nchan] = lc_class_b;
    dl_service_reg.dl_wm_ptr[nchan]= &(dl_queues_ptr->wm_b);
    dl_service_reg.context[nchan] = TRUE; /* in isr */
    dl_service_reg.rlc_post_rx_proc_func_ptr[nchan] = NULL;
    nchan++;
    mvs_dl_umts_chan_state.lcb = lc_class_b;
    mvs_dl_umts_chan_state.chan_state.has_chan_b = TRUE;
  }

  /* Register class C channel if necessary */
  if ( chan_state.has_chan_c == TRUE
       /* && ( mvs_dl_umts_chan_state.chan_state.has_chan_c == FALSE ) */ )
  {
    dl_service_reg.rlc_id[nchan] = lc_class_c;
    dl_service_reg.dl_wm_ptr[nchan] = &dl_queues_ptr->wm_c;
    dl_service_reg.context[nchan] = TRUE; /* in isr */
    dl_service_reg.rlc_post_rx_proc_func_ptr[nchan] = NULL;
    nchan++;
    mvs_dl_umts_chan_state.lcc = lc_class_c;
    mvs_dl_umts_chan_state.chan_state.has_chan_c = TRUE;
  }

  dl_service_reg.nchan = nchan;

  MSG(MSG_SSID_DFLT, MSG_LEGACY_LOW, "Sending DL register request" );

#ifndef WINSIM
  tds_dl_service_register_param.service = &dl_service_reg;
  rc = tdscdma_l2_api_call( TDSL2_CMD_DL_SERVICE_REGISTER,&tds_dl_service_register_param,
                            sizeof( tds_dl_service_register_param ) );
  if( rc == TDS_EOK )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_LOW, "Register of TDSCDMA DL channels done successfully" );
  }
  else
#endif /* !WINSIM */
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Couldn't register L2 uplink" );
  }

  /* verification */
  if ( mvs_amr_cmp_chan_state( &chan_state, &mvs_dl_umts_chan_state.chan_state ) != TRUE )
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "TDSCDMA DL channels setup errors!" );
  }

  MSG(MSG_SSID_DFLT, MSG_LEGACY_LOW, "mvs_tdscdma_set_dl_channel() complete" );
#endif /* !WINSIM */

  return APR_EOK;
}
