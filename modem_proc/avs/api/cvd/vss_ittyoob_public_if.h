#ifndef __VSS_ITTYOOB_PUBLIC_IF_H__
#define __VSS_ITTYOOB_PUBLIC_IF_H__

/**
  @file vss_ittyoob_public_if.h
  @brief This file contains the APR API definitions for Teletypewriter (TTY) 
  out of band (OOB).
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF are located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2013-2014 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/27/14     sw    (Tech Pubs) Edited with Doxygen comments from 2.2.

===========================================================================*/

#include "apr_comdef.h"


/****************************************************************************
 * TELETYPEWRITER (TTY) Out of Band (OOB) APIS                              *
 ****************************************************************************/

/** @ingroup cvd_tty_oob_cmd_reg
  Registers for access to the OOB TTY character data interface.
 
  @apr_msgpayload
  None.
 
  @return 
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  The HLOS is expected to enable a TTY device and set the correct mode (VCO,
  HCO, or FULL). The OOB TTY stream client must call this registration command
  before the MVM is commanded to start voice operations.
*/ /* Q2 = Async Packet Router API for MDM9x15 Interface Spec (80-N1463-2)
*/
#define VSS_ITTYOOB_CMD_REGISTER ( 0x00012E76 )
  
/** @ingroup cvd_tty_oob_cmd_dereg
  Deregisters access from the OOB TTY character data interface.
  This command disables OOB TTY event exchanges.

  @apr_msgpayload
  None.
 
  @return 
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_ITTYOOB_CMD_DEREGISTER ( 0x00012E77 )

/** @ingroup cvd_tty_oob_evt_send_tx_ch
  Sent by the CVS module to the client when a character is detected
  on the Tx path.
 
  @apr_msgpayload{vss_ittyoob_evt_tx_char_t}
  @wktable{weak__vss__ittyoob__evt__tx__char__t}
 
  @return 
  None.

  @dependencies
  The stream must be in the Ready state before it can generate these events.
*/
#define VSS_ITTYOOB_EVT_SEND_TX_CHAR ( 0x00012E78 )

/* Type definition for vss_ittyoob_evt_tx_char_t. */
typedef struct vss_ittyoob_evt_tx_char_t vss_ittyoob_evt_tx_char_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ittyoob_evt_tx_char_t
@{ */
/* Payload structure for the VSS_ITTYOOB_EVT_SEND_TX_CHAR event.
*/
struct vss_ittyoob_evt_tx_char_t
{
  uint16_t tty_char;
  /**< - LSB 8 bits -- Character in UTF-8 format. 
       - MSB 8 bits -- Not used. Set to 0. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ittyoob_evt_tx_char_t */


/** @ingroup cvd_tty_oob_evt_send_rx_ch
  Command from the client to send a character.
 
  @apr_msgpayload{vss_ittyoob_cmd_rx_char_t}
  @wktable{weak__vss__ittyoob__cmd__rx__char__t}
 
  @return 
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  The stream must be in the Ready state before it can accept this command.
  Otherwise, the stream replies with the response, APR_EFAILED.
*/
#define VSS_ITTYOOB_CMD_SEND_RX_CHAR ( 0x00012E79 )

/* Type definition for vss_ittyoob_cmd_rx_char_t. */
typedef struct vss_ittyoob_cmd_rx_char_t vss_ittyoob_cmd_rx_char_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_ittyoob_cmd_rx_char_t
@{ */
/* Payload structure for the VSS_ITTYOOB_CMD_SEND_RX_CHAR event.
*/
struct vss_ittyoob_cmd_rx_char_t
{
  uint16_t tty_char;
  /**< - LSB 8 bits -- Character in UTF-8 format. 
       - MSB 8 bits -- Not used. Set to 0. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_ittyoob_cmd_rx_char_t */

#endif /* __VSS_ITTYOOB_PUBLIC_IF_H__ */

