#ifndef __VSS_IMVM_DEPRECATED_PUBLIC_IF_H__
#define __VSS_IMVM_DEPRECATED_PUBLIC_IF_H__

/**
  @file vss_imvm_deprecated_public_if.h
  @brief This file contains the deprecated APR API definitions for Multimode
  Manager (MVM) module.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The CVD_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the vss_imvm_public_if 
      group description in the CVD_mainpage.dox file. 
=============================================================================*/
/*===========================================================================
Copyright (c) 2012 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_imvm_deprecated_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
03/13/12     AT    Initial version.

===========================================================================*/


#include "apr_comdef.h"

/** @addtogroup vss_imvm_deprecated_public_if
@{ */

/****************************************************************************
 * MULTIMODE VOICE MANAGER CORE APIS                                        *
 ****************************************************************************/

/**
  Sets modem control policy on MVM. In modem control mode, MVM state is controled 
  by both modem and apps. Modem control policy must be set on MVM immediately 
  after session creation and before starting voice. 

  @deprecated
  This command is deprecated. Use #VSS_IMVM_CMD_SET_POLICY_DUAL_CONTROL.

  @par Payload
  #vss_imvm_cmd_set_modem_voice_control_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.
*/

#define VSS_IMVM_CMD_SET_MODEM_VOICE_CONTROL ( 0x000112D8 )

/** @brief Type definition for #vss_imvm_cmd_set_modem_voice_control_t.
*/
typedef struct vss_imvm_cmd_set_modem_voice_control_t vss_imvm_cmd_set_modem_voice_control_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IMVM_CMD_SET_MODEM_VOICE_CONTROL command.
*/
struct vss_imvm_cmd_set_modem_voice_control_t
{
  bool_t enable_flag;
    /**< Set to TRUE to enable modem state machine control. */
}
#include "apr_pack_end.h"
;

/** @} */ /* end_addtogroup vss_imvm_derepcated_public_if */

#endif /* __VSS_IMVM_DEPRECATED_PUBLIC_IF_H__ */

