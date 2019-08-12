#ifndef __VSS_IWIDEVOICE_DEPRECATED_PUBLIC_IF_H__
#define __VSS_IWIDEVOICE_DEPRECATED_PUBLIC_IF_H__

/**
  @file vss_iwidevoice_deprecated_public_if.h
  @brief This file contains the APR API definitions for WideVoice.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The CVD_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the 
      vss_iwidevoice_deprecatred_public_if group description in the
      CVD_mainpage.dox file. 
=============================================================================*/
/*===========================================================================
Copyright (c) 2012 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_iwidevoice_deprecated_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
04/01/11    leo    (Tech pubs) Minor updates for new template.
01/12/11    SCD    (Tech Pubs) Edited Doxygen markup and comments.
01/11/11     AT    Initial revision.

===========================================================================*/

#include "apr_comdef.h"

/** @addtogroup vss_iwidevoice_deprecated_public_if
@{ */

/****************************************************************************
 * WIDEVOICE APIS                                                           *
 ****************************************************************************/

/**
  Enables/disables WideVoice.

  @deprecated
  This command is deprecated. Use #VSS_IVOCPROC_CMD_REGISTER_DEVICE_CONFIG
  to register the device config table containing the WideVoice sample rate
  with the vocproc.
 
  @par Payload
  #vss_iwidevoice_cmd_set_widevoice_t
 
  @return 
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/
#define VSS_IWIDEVOICE_CMD_SET_WIDEVOICE ( 0x00011243 )

/** @brief Type definition for #vss_iwidevoice_cmd_set_widevoice_t.
*/
typedef struct vss_iwidevoice_cmd_set_widevoice_t vss_iwidevoice_cmd_set_widevoice_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IWIDEVOICE_CMD_SET_WIDEVOICE command.
*/
struct vss_iwidevoice_cmd_set_widevoice_t
{
  uint32_t enable;
    /**< WideVoice enable/disable; possible values:
         - 0 -- WideVoice disabled
         - 1 -- WideVoice enabled */
}
#include "apr_pack_end.h"
;

/** @} */ /* end_addtogroup vss_iwidevoice_deprecated_public_if */

#endif /* __VSS_IWIDEVOICE_DEPRECATED_PUBLIC_IF_H__ */

