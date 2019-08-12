#ifndef __VSS_IPLAYBACK_PUBLIC_IF_H__
#define __VSS_IPLAYBACK_PUBLIC_IF_H__

/**
  @file vss_iplayback_public_if.h
  @brief This file contains the APR API definitions for In-Call Music Delivery
  related functions.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2011, 2014 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/cvd/vss_iplayback_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/28/14     sw    (Tech Pubs) Updated with Doxygen comments from 2.0.
07/19/11    nrc    Initial revision.

===========================================================================*/


#include "apr_comdef.h"

/****************************************************************************
 * PLAYBACK DEFINES                                                         *
 ****************************************************************************/

/** @ingroup cvd_incall_playback_constants
  Default AFE port ID. */
#define VSS_IPLAYBACK_PORT_ID_DEFAULT ( 0xFFFF )


/****************************************************************************
 * IN-CALL MUSIC DELIVERY APIS                                              *
 ****************************************************************************/

/** @ingroup cvd_incall_playback_cmd_start
  Starts the in-call music delivery on the Tx voice path by establishing a
  connection between the Tx voice stream and the specified AFE port, allowing
  the voice stream to receive music from the audio path.

  @apr_msgpayload{vss_iplayback_cmd_start_t}
  @wktable{weak__vss__iplayback__cmd__start__t}

  @detdesc
  The audio path is used to inject music to the Tx voice path. Configuring
  the AFE port to nonlive mode allows the voice modules to throttle transfer
  jitters. 
  @par
  Only one playback session can be active on a particular module instance at
  any given time.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/ /* Q2 = Async Packet Router API for MDM9x15 Interface Spec (80-N1463-2)
*/
#define VSS_IPLAYBACK_CMD_START ( 0x000112BD )

/* Type definition for vss_iplayback_cmd_start_t. */
typedef struct vss_iplayback_cmd_start_t vss_iplayback_cmd_start_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_iplayback_cmd_start_t
@{ */
/* Payload structure for the VSS_IPLAYBACK_CMD_START command.
*/
struct vss_iplayback_cmd_start_t
{
  uint16_t port_id;
  /**< AFE port ID from which the audio samples are available.

       To use the default AFE pseudoport (0x8005), set this value to
           #VSS_IPLAYBACK_PORT_ID_DEFAULT. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_iplayback_cmd_start_t */


/** @ingroup cvd_incall_playback_cmd_stop
  Stops the in-call music delivery on the Tx voice path.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_IPLAYBACK_CMD_STOP ( 0x00011239 )


#endif /* __VSS_IPLAYBACK_PUBLIC_IF_H__ */

