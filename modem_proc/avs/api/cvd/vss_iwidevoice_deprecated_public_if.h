#ifndef __VSS_IWIDEVOICE_DEPRECATED_PUBLIC_IF_H__
#define __VSS_IWIDEVOICE_DEPRECATED_PUBLIC_IF_H__

/**
  @file vss_iwidevoice_deprecated_public_if.h
  @brief This file contains the APR API definitions for WideVoice.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/cvd/vss_iwidevoice_deprecated_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/27/14     sw    (Tech Pubs) Updated with Doxygen comments from 2.0.
04/01/11    leo    (Tech pubs) Minor updates for new template.
01/12/11    SCD    (Tech Pubs) Edited Doxygen markup and comments.
01/11/11     AT    Initial revision.

===========================================================================*/

#include "apr_comdef.h"

/****************************************************************************
 * WIDEVOICE APIS                                                           *
 ****************************************************************************/

/** @addtogroup cvd_widevoice_deprecated
@{ */
/** Deprecated. Use #VSS_IVOCPROC_CMD_REGISTER_DEVICE_CONFIG to register the
  device configuration table containing the WideVoice sample rate with the
  vocproc.

  Enables/disables WideVoice.

  @apr_msgpayload{vss_iwidevoice_cmd_set_widevoice_t}
  @wktable{weak__vss__iwidevoice__cmd__set__widevoice__t}
 
  @return 
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_IWIDEVOICE_CMD_SET_WIDEVOICE ( 0x00011243 )

/* Type definition for vss_iwidevoice_cmd_set_widevoice_t. */
typedef struct vss_iwidevoice_cmd_set_widevoice_t vss_iwidevoice_cmd_set_widevoice_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_iwidevoice_cmd_set_widevoice_t 
@{ */
/* Payload of the VSS_IWIDEVOICE_CMD_SET_WIDEVOICE command.
*/
struct vss_iwidevoice_cmd_set_widevoice_t
{
  uint32_t enable;
  /**< Specifies whether WideVoice is enabled.

       @values
       - 0 -- Disable WideVoice
       - 1 -- Enable WideVoice @tablebulletend */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_iwidevoice_cmd_set_widevoice_t */
/** @} */ /* end_addtogroup cvd_mvm_deprecated */


#endif /* __VSS_IWIDEVOICE_DEPRECATED_PUBLIC_IF_H__ */

