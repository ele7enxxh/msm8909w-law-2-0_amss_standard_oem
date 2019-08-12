#ifndef __VSS_IAVSYNC_PUBLIC_IF_H__
#define __VSS_IAVSYNC_PUBLIC_IF_H__

/**
  @file vss_iavsync_public_if.h
  @brief This file contains the APR API definitions for CVD audio-video
  synchronization information.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2013-2014 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/cvd/vss_iavsync_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/27/14    sw     (Tech Pubs) Updated with Doxygen comments from 2.2.

===========================================================================*/


#include "apr_comdef.h"


/****************************************************************************
 * AV SYNC APIS                                                             *
 ****************************************************************************/

/** @ingroup cvd_avsync_evt_class_rx
  Rx voice path audio-video syncrhonization event class. This class
  comprises one event: #VSS_IAVSYNC_EVT_RX_PATH_DELAY.
*/
#define VSS_IAVSYNC_EVENT_CLASS_RX ( 0x00012EE1 )


/** @ingroup cvd_avsync_evt_rx_path_delay
  Notifies the client of changes in the Rx voice path delay value.
 
  @apr_msgpayload{vss_iavsync_evt_rx_path_delay_t}
  @wktable{weakst__vss__iavsync__evt__rx__path__delay__t}

  @detdesc
  This event is sent to the client under the following circumstances:
    - Every time the module with which the client is registered for this
      event is started.
    - Immediately after registration, if the module is already running.
    - Subsequently whenever the value of the Rx path delay changes.

  @return
  None.

  @dependencies
  To receive this event, the client must first register to listen for the
  #VSS_IAVSYNC_EVENT_CLASS_RX event class with the appropriate module via the
  #VSS_INOTIFY_CMD_LISTEN_FOR_EVENT_CLASS command.
  @par
  To stop receiving this event, the client must cancel the
  VSS_IAVSYNC_EVENT_CLASS_RX event class via the
  #VSS_INOTIFY_CMD_CANCEL_EVENT_CLASS command.
*/
#define VSS_IAVSYNC_EVT_RX_PATH_DELAY ( 0x00012EE2 )

/* Type definition for vss_iavsync_evt_rx_path_delay_t. */
typedef struct vss_iavsync_evt_rx_path_delay_t vss_iavsync_evt_rx_path_delay_t;

#include "apr_pack_begin.h"

/** @weakgroup weakst_vss_iavsync_evt_rx_path_delay_t
@{ */
/* Payload structure for the #VSS_IAVSYNC_EVT_RX_PATH_DELAY event.
*/
struct vss_iavsync_evt_rx_path_delay_t
{
   uint32_t delay_us;     /**< Rx voice path delay in microseconds. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_iavsync_evt_rx_path_delay_t */

#endif /* __VSS_IAVSYNC_PUBLIC_IF_H__ */
