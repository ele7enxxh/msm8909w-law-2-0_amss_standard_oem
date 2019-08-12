#ifndef __VSS_IPLAYBACK_PUBLIC_IF_H__
#define __VSS_IPLAYBACK_PUBLIC_IF_H__

/**
  @file vss_iplayback_public_if.h
  @brief This file contains the APR API definitions for In-Call Music Delivery
  related functions.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The CVD_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the 
      vss_iplayback_public_if group description in the CVD_mainpage.dox file.
=============================================================================*/
/*===========================================================================
Copyright (C) 2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_iplayback_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
07/19/11    nrc    Initial revision.

===========================================================================*/


#include "apr_comdef.h"

/** @addtogroup vss_iplayback_public_if
@{ */

/****************************************************************************
 * PLAYBACK DEFINES                                                         *
 ****************************************************************************/

/** Default AFE port ID. */
#define VSS_IPLAYBACK_PORT_ID_DEFAULT ( 0xFFFF )


/****************************************************************************
 * IN-CALL MUSIC DELIVERY APIS                                              *
 ****************************************************************************/

/**
  This command starts the in-call music delivery on Tx voice path by
  establishing a connection between TX voice stream and the specified AFE
  port, allowing the voice stream to receive music from the audio path.

  @par Payload
  #vss_iplayback_cmd_start_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.

  @comments
  The audio path is used to inject music to the Tx voice path. Configuring
  AFE port to non-live mode allows the voice modules to throttle transfer
  jitters. 
  
  Only one playback session may be active on a particular module instance at
  any given time.
*/

#define VSS_IPLAYBACK_CMD_START ( 0x000112BD )

/** @brief Type definition for #vss_iplayback_cmd_start_t.
*/
typedef struct vss_iplayback_cmd_start_t vss_iplayback_cmd_start_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IPLAYBACK_CMD_START command.
*/
struct vss_iplayback_cmd_start_t
{
  uint16_t port_id;
      /**< AFE Port ID from which the audio samples are available. To use the
           default AFE pseudo port (0x8005), set this value to
           #VSS_IPLAYBACK_PORT_ID_DEFAULT. */
}
#include "apr_pack_end.h"
;


/**
  This command stops the in-call music delivery on the Tx voice path.

  @par Payload
  None.

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IPLAYBACK_CMD_STOP ( 0x00011239 )

/** @} */ /* end_addtogroup vss_iplayback_public_if */

#endif /* __VSS_IPLAYBACK_PUBLIC_IF_H__ */

