/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S

DESCRIPTION
  This file defines function interfaces and corresponding data
  structures for invoking multi-mode voice services.

REFERENCES
  None.

Copyright (C) 2007-2014 QUALCOMM Technologies Incorporated
All rights reserved.
Qualcomm Confidential and Proprietary

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/mvs/src/mvs.c#1 $
$Author: mplcsds1 $

===========================================================================*/

/*========================================================================
Edit History

when       who     what, where, why
--------   ---     ------------------------------------------------------
06/26/13   sud     Updated Copyright.
11/09/09    jk     g711a enum type fix
11/05/09    jk     added g711 and g711a functionality
11/10/08    sr     Removed reference to CClockAdieControl.h
08/15/08    sr     Fixed g_pMVSLegacyTest going to NULL.
07/25/08    at     Removed AudioHwInit (has moved to a new init file).
06/29/08   laks    Added AudioHwInit hack.
04/15/08    at     Added MVS test functionality.
08/30/08    at     Added debug messages for all of the funcions.
08/17/07   ash     Created file.

========================================================================== */

/* =======================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#include "msg.h"
#include "mvs.h"
#include "mvs_module.h"

/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

/* Map g711 mode to g711A mode. */
mvs_g711a_mode_type map_g711_mode_to_g711a (
  mvs_g711_mode_type g711_mode
)
{
  mvs_g711a_mode_type g711a_mode;

  switch ( g711_mode )
  {
  case MVS_G711_MODE_MULAW:
    g711a_mode = MVS_G711A_MODE_MULAW;
    break;
  case MVS_G711_MODE_ALAW:
    g711a_mode = MVS_G711A_MODE_ALAW;
    break;
  default:
    g711a_mode = MVS_G711A_MODE_MAX;
    break;
  }

  return g711a_mode;
}

/* Map g711a mode to g711 mode. */
mvs_g711_mode_type map_g711a_mode_to_g711 (
  mvs_g711a_mode_type g711a_mode
)
{
  mvs_g711_mode_type g711_mode;

  switch ( g711a_mode )
  {
  case MVS_G711A_MODE_MULAW:
    g711_mode = MVS_G711_MODE_MULAW;
    break;
  case MVS_G711A_MODE_ALAW:
    g711_mode = MVS_G711_MODE_ALAW;
    break;
  default:
    g711_mode = MVS_G711_MODE_MAX;
    break;
  }

  return g711_mode;
}

void mvs_init ( void )
{
  /* Do nothing. */
}

boolean mvs_mode_supported (
  mvs_mode_type mode
)
{
  return mvs_mod_mode_supported( mode );
}

void mvs_set_loopback (
  mvs_loopback_type lb_mode
)
{
  MSG_1(MSG_SSID_DFLT, MSG_LEGACY_MED, "mvs_set_loopback(): mode %d",
                                        lb_mode );
  /* Not supported. */
}

void mvs_acquire (
  mvs_client_type client,
  mvs_event_cb_type cb_func
)
{
  ( void ) mvs_mod_acquire( client, cb_func );
}

void mvs_enable (
  mvs_client_type client,
  mvs_mode_type mode,
  mvs_packet_ul_fn_type ul_func,
  mvs_packet_dl_fn_type dl_func,
  mvs_pkt_context_type context
)
{
  ( void ) mvs_mod_enable( client, mode, ul_func, dl_func, context );
}

void mvs_standby (
  mvs_client_type client,
  mvs_mode_type mode
)
{
  ( void ) mvs_mod_standby( client, mode );
}

void mvs_listen (
  mvs_client_type client,
  mvs_event_cb_type cb_func,
  mvs_listen_fn_type ul_func,
  mvs_listen_fn_type dl_func
)
{
  /* Not supported. */
}

void mvs_release (
  mvs_client_type client
)
{
  ( void ) mvs_mod_release( client );
}

void mvs_amr_set_amr_mode (
  mvs_amr_mode_type amr_ul_mode
)
{
  ( void ) mvs_mod_amr_set_amr_mode( amr_ul_mode );
}

void mvs_amr_set_awb_mode (
  mvs_amr_mode_type amr_ul_mode
)
{
  ( void ) mvs_mod_amr_set_awb_mode( amr_ul_mode );
}



void mvs_amr_set_scr_mode (
  mvs_scr_mode_type scr_mode
)
{
  ( void ) mvs_mod_amr_set_scr_mode( scr_mode );
}

void mvs_voc_set_frame_rate (
  mvs_voc_rate_type max_rate,
  mvs_voc_rate_type min_rate
)
{
  ( void ) mvs_mod_voc_set_frame_rate( max_rate, min_rate );
}

void mvs_gsm_set_dtx_mode (
  mvs_dtx_mode_type dtx_mode
)
{
  ( void ) mvs_mod_set_dtx_mode( dtx_mode );
}

void mvs_voc_tx_rate_limit (
  word rate_factor
)
{
  ( void ) mvs_mod_voc_tx_rate_limit( rate_factor );
}

mvs_g711_mode_type mvs_get_g711_mode ( void )
{
  return mvs_mod_get_g711_mode( );
}

mvs_g711a_mode_type mvs_get_g711a_mode ( void )
{
  return map_g711_mode_to_g711a( mvs_mod_get_g711_mode( ) );
}

void mvs_set_g711_mode (
  mvs_g711_mode_type mvs_g711_mode
)
{
  mvs_mod_set_g711_mode( mvs_g711_mode );
}

void mvs_g711a_set_dtx_mode (
  mvs_dtx_mode_type dtx_mode
)
{
  mvs_mod_set_dtx_mode( dtx_mode );
}

void mvs_g729a_set_dtx_mode(
  mvs_dtx_mode_type dtx_mode
)
{
  mvs_mod_set_dtx_mode( dtx_mode );
}

void mvs_set_g711a_mode (
  mvs_g711a_mode_type mvs_g711a_mode
)
{
  mvs_mod_set_g711_mode( map_g711a_mode_to_g711( mvs_g711a_mode ) );
}

void mvs_set_g722_mode (
  mvs_g722_mode_type mvs_g722_mode
)
{
  mvs_mod_set_g722_mode(mvs_g722_mode);
}

mvs_g722_mode_type mvs_get_g722_mode ( void )
{
  return ( mvs_mod_get_g722_mode() );
}



void mvs_client_send_frames (
  mvs_client_type client,
  mvs_frame_type* frame_ptr,
  uint16 mvs_frame_count
)
{
  /* Not supported. */
}

