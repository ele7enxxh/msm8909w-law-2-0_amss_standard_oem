#ifndef __VSS_ITTY_PUBLIC_IF_H__
#define __VSS_ITTY_PUBLIC_IF_H__

/**
  @file vss_itty_public_if.h
  @brief This file contains the APR API definitions for Teletypewriter (TTY).
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The CVD_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the vss_itty_public_if 
      group description in the CVD_mainpage.dox file. 
=============================================================================*/
/*===========================================================================
Copyright (c) 2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_itty_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
04/01/11    leo    (Tech pubs) Minor updates for new template.
01/12/11    SCD    (Tech Pubs) Edited Doxygen markup and comments.
01/11/11     AT    Initial version.

===========================================================================*/

#include "apr_comdef.h"

/** @addtogroup vss_itty_public_if
@{ */

/****************************************************************************
 * TELETYPEWRITER (TTY) APIS                                                *
 ****************************************************************************/

/**
  Sets the TTY mode.
 
  @par Payload
  #vss_itty_cmd_set_tty_mode_t
 
  @return 
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_ITTY_CMD_SET_TTY_MODE ( 0x00011196 )
  
/** @brief Type definition for #vss_itty_cmd_set_tty_mode_t.
*/
typedef struct vss_itty_cmd_set_tty_mode_t vss_itty_cmd_set_tty_mode_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ITTY_CMD_SET_TTY_MODE command.
*/
struct vss_itty_cmd_set_tty_mode_t
{
  uint32_t mode;
    /**< Sets the TTY mode; possible values:
         - 0 -- TTY disabled
         - 1 -- Hearing Carry Over (HCO)
         - 2 -- Voice Carry Over (VCO)
         - 3 -- FULL */
}
#include "apr_pack_end.h"
;

/** @} */ /* end_addtogroup vss_itty_public_if */

#endif /* __VSS_ITTY_PUBLIC_IF_H__ */

