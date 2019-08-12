#ifndef __VSS_ITTY_PUBLIC_IF_H__
#define __VSS_ITTY_PUBLIC_IF_H__

/**
  @file vss_itty_public_if.h
  @brief This file contains the APR API definitions for Teletypewriter (TTY).
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/cvd/vss_itty_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/27/14     sw    (Tech Pubs) Updated with Doxygen comments from 2.0.
04/01/11    leo    (Tech pubs) Minor updates for new template.
01/12/11    SCD    (Tech Pubs) Edited Doxygen markup and comments.
01/11/11     AT    Initial version.

===========================================================================*/

#include "apr_comdef.h"

/****************************************************************************
 * TELETYPEWRITER (TTY) APIS                                                *
 ****************************************************************************/

/** @ingroup cvd_tty_inb_cmd_set_tty
  Sets the TTY mode. The client must sent this command to the MVM session in
  the CVD module.
 
  @apr_msgpayload{vss_itty_cmd_set_tty_mode_t}
  @wktable{weak__vss__itty__cmd__set__tty__mode__t}
 
  @return 
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/ /* Q2 = Async Packet Router API for MDM9x15 Interface Spec (80-N1463-2)
*/
#define VSS_ITTY_CMD_SET_TTY_MODE ( 0x00011196 )
  
/* Type definition for vss_itty_cmd_set_tty_mode_t. */
typedef struct vss_itty_cmd_set_tty_mode_t vss_itty_cmd_set_tty_mode_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_itty_cmd_set_tty_mode_t
@{ */
/* Payload structure for the VSS_ITTY_CMD_SET_TTY_MODE command.
*/
struct vss_itty_cmd_set_tty_mode_t
{
  uint32_t mode;
  /**< Sets the TTY mode.
    
       @values
       - 0 -- Disable TTY
         - 1 -- Hearing Carry Over (HCO)
         - 2 -- Voice Carry Over (VCO)
       - 3 -- FULL
       
       Currently, this command does not fail if the mode value is not
       supported. */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_itty_cmd_set_tty_mode_t */


#endif /* __VSS_ITTY_PUBLIC_IF_H__ */

