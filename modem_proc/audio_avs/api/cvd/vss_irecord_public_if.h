#ifndef __VSS_IRECORD_PUBLIC_IF_H__
#define __VSS_IRECORD_PUBLIC_IF_H__

/**
  @file vss_irecord_public_if.h
  @brief This file contains the APR API definitions for In-Call conversation
  recording related functions.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The CVD_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the vss_irecord_public_if
      group description in the CVD_mainpage.dox file.
=============================================================================*/
/*===========================================================================
Copyright (C) 2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_irecord_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
07/20/11    nrc    Initial revision.

===========================================================================*/


#include "apr_comdef.h"

/** @addtogroup vss_irecord_public_if
@{ */

/****************************************************************************
 * RECORD DEFINES                                                          *
 ****************************************************************************/

/** Default AFE port ID. */
#define VSS_IRECORD_PORT_ID_DEFAULT ( 0xFFFF )

/** Indicates no tapping for the specified path. */
#define VSS_IRECORD_TAP_POINT_NONE ( 0x00010F78 )

/** Indicates that the specified path is to be tapped at the end of 
 the stream. */
#define VSS_IRECORD_TAP_POINT_STREAM_END ( 0x00010F79 )

/** Select Tx on left channel and Rx on right channel. */
#define VSS_IRECORD_MODE_TX_RX_STEREO ( 0x00010F7A )

/** Select mixed Tx and Rx paths. */
#define VSS_IRECORD_MODE_TX_RX_MIXING ( 0x00010F7B )


/****************************************************************************
 * IN-CALL CONVERSATION RECORDING APIS                                      *
 ****************************************************************************/

/**
  This command starts the in-call conversation recording by forwarding the
  in-call conversation stream to the specified AFE port.

  @par Payload
  #vss_irecord_cmd_start_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.

  @comments
  The audio path is used to record the voice streams. If recording silence
  before a call or during handovers is desired, AFE port must be 
  configured to live mode.
  
  Only one recording session may be active on a particular module instance at
  any given time.
*/
#define VSS_IRECORD_CMD_START ( 0x000112BE )

/** @brief Type definition for #vss_irecord_cmd_start_t.
*/
typedef struct vss_irecord_cmd_start_t vss_irecord_cmd_start_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IRECORD_CMD_START command.
*/
struct vss_irecord_cmd_start_t
{
  uint32_t rx_tap_point;
      /**< Tap point to use on the Rx path. The supported values:\n
           #VSS_IRECORD_TAP_POINT_NONE -- Do not record Rx path.\n
           #VSS_IRECORD_TAP_POINT_STREAM_END -- Rx tap point is at the end of
           the stream. */
  uint32_t tx_tap_point;
      /**< Tap point to use on the Tx path. The supported values:\n
           #VSS_IRECORD_TAP_POINT_NONE -- Do not record Tx path.\n
           #VSS_IRECORD_TAP_POINT_STREAM_END -- Tx tap point is at the end of
           the stream. */
  uint16_t port_id;
      /**< AFE Port ID to which the conversation recording stream needs to be
           sent. Set this to #VSS_IRECORD_PORT_ID_DEFAULT to use default AFE
           pseudo ports (0x8003 for Rx and 0x8004 for Tx). */
  uint32_t mode;
      /**< Recording mode. The mode parameter value is ignored if the port_id
           is set to #VSS_IRECORD_PORT_ID_DEFAULT.
           The supported values: \n
           #VSS_IRECORD_MODE_TX_RX_STEREO \n
           #VSS_IRECORD_MODE_TX_RX_MIXING */
}
#include "apr_pack_end.h"
;


/**
  This command stops the in-call conversation recording.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IRECORD_CMD_STOP ( 0x00011237 )

/** @} */ /* end_addtogroup vss_irecord_public_if */

#endif /* __VSS_IRECORD_PUBLIC_IF_H__ */

