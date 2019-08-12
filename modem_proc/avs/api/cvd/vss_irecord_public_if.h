#ifndef __VSS_IRECORD_PUBLIC_IF_H__
#define __VSS_IRECORD_PUBLIC_IF_H__

/**
  @file vss_irecord_public_if.h
  @brief This file contains the APR API definitions for In-Call conversation
  recording related functions.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/cvd/vss_irecord_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/27/14     sw    (Tech Pubs) Updated with Doxygen comments from 2.0.
07/20/11    nrc    Initial revision.

===========================================================================*/


#include "apr_comdef.h"

/****************************************************************************
 * RECORD DEFINES                                                          *
 ****************************************************************************/

/** @addtogroup cvd_incall_rec_constants
@{ */
/** Default AFE port ID. */
#define VSS_IRECORD_PORT_ID_DEFAULT ( 0xFFFF )

/** Indicates no tapping for the specified path. */
#define VSS_IRECORD_TAP_POINT_NONE ( 0x00010F78 )

/** Indicates that the specified path is to be tapped at the end of 
 the stream. */
#define VSS_IRECORD_TAP_POINT_STREAM_END ( 0x00010F79 )

/** Selects Tx on the left channel and Rx on the right channel. */
#define VSS_IRECORD_MODE_TX_RX_STEREO ( 0x00010F7A )

/** Selects mixed Tx and Rx paths. */
#define VSS_IRECORD_MODE_TX_RX_MIXING ( 0x00010F7B )
/** @} */ /* end_addtogroup cvd_incall_rec_constants */

/****************************************************************************
 * IN-CALL CONVERSATION RECORDING APIS                                      *
 ****************************************************************************/

/** @ingroup cvd_incall_rec_cmd_start
  Starts the in-call conversation recording by forwarding the
  in-call conversation stream to the specified AFE port.

  @apr_msgpayload{vss_irecord_cmd_start_t}
  @wktable{weak__vss__irecord__cmd__start__t}

  @detdesc
  The audio path is used to record the voice streams. If recording silence
  before a call or during handovers is desired, the AFE port must be 
  configured to live mode.
  @par
  Only one recording session can be active on a particular module instance at
  any given time.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/ /* Q2 = Async Packet Router API for MDM9x15 Interface Spec (80-N1463-2)
*/
#define VSS_IRECORD_CMD_START ( 0x000112BE )

/* Type definition for vss_irecord_cmd_start_t. */
typedef struct vss_irecord_cmd_start_t vss_irecord_cmd_start_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_irecord_cmd_start_t
@{ */
/* Payload structure for the VSS_IRECORD_CMD_START command.
*/
struct vss_irecord_cmd_start_t
{
  uint32_t rx_tap_point;
  /**< Tap point to use on the Rx path.

       @values
       - #VSS_IRECORD_TAP_POINT_NONE
       - #VSS_IRECORD_TAP_POINT_STREAM_END @tablebulletend */

  uint32_t tx_tap_point;
  /**< Tap point to use on the Tx path.

       @values
       - #VSS_IRECORD_TAP_POINT_NONE
       - #VSS_IRECORD_TAP_POINT_STREAM_END @tablebulletend */

  uint16_t port_id;
  /**< AFE port ID to which the conversation recording stream must be sent.

       Set this to #VSS_IRECORD_PORT_ID_DEFAULT to use default AFE pseudoports:
       - 0x8003 for Rx
       - 0x8004 for Tx @tablebulletend */

  uint32_t mode;
  /**< Recording mode.

       @values
       - #VSS_IRECORD_MODE_TX_RX_STEREO
       - #VSS_IRECORD_MODE_TX_RX_MIXING

       The mode parameter value is ignored if the port_id is set to
       #VSS_IRECORD_PORT_ID_DEFAULT. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup vss_irecord_cmd_start_t */


/** @ingroup cvd_incall_rec_cmd_stop
  Stops the in-call conversation recording.

  @apr_msgpayload
  None.

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_IRECORD_CMD_STOP ( 0x00011237 )


#endif /* __VSS_IRECORD_PUBLIC_IF_H__ */

