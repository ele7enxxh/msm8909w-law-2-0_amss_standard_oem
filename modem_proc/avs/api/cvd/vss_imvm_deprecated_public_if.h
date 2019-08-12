#ifndef __VSS_IMVM_DEPRECATED_PUBLIC_IF_H__
#define __VSS_IMVM_DEPRECATED_PUBLIC_IF_H__

/**
  @file vss_imvm_deprecated_public_if.h
  @brief This file contains the deprecated APR API definitions for Multimode
  Manager (MVM) module.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2012, 2014 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/cvd/vss_imvm_deprecated_public_if.h#1 $
  $Author: mplcsds1 $

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/28/14     sw    (Tech Pubs) Updated with Doxygen comments from 2.0.
03/13/12     AT    Initial version.

===========================================================================*/


#include "apr_comdef.h"

/** @addtogroup cvd_mvm_deprecated
@{ */

/****************************************************************************
 * MULTIMODE VOICE MANAGER CORE APIS                                        *
 ****************************************************************************/

/** Deprecated. Use #VSS_IMVM_CMD_SET_POLICY_DUAL_CONTROL.

  Sets modem control policy on the MVM. In Modem Control mode, the MVM state
  is controlled by both the modem and the applications. Modem control policy
  must be set on the MVM immediately after session creation and before starting
  voice. 

  @apr_msgpayload{vss_imvm_cmd_set_modem_voice_control_t}
  @wktable{weak__vss__imvm__cmd__set__modem__voice__control__t}

  @return
  APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).

  @dependencies
  None.
*/
#define VSS_IMVM_CMD_SET_MODEM_VOICE_CONTROL ( 0x000112D8 )

/* Type definition for vss_imvm_cmd_set_modem_voice_control_t. */
typedef struct vss_imvm_cmd_set_modem_voice_control_t vss_imvm_cmd_set_modem_voice_control_t;

#include "apr_pack_begin.h"

/** @weakgroup weak_vss_imvm_cmd_set_modem_voice_control_t 
@{ */
/* Payload structure for the VSS_IMVM_CMD_SET_MODEM_VOICE_CONTROL command.
*/
struct vss_imvm_cmd_set_modem_voice_control_t
{
  bool_t enable_flag;
  /**< Specifies whether modem state machine control is enabled (TRUE). */
}
#include "apr_pack_end.h"
;
/** @} */ /* end_weakgroup weak_vss_imvm_cmd_set_modem_voice_control_t */
/** @} */ /* end_addtogroup cvd_mvm_deprecated */


#endif /* __VSS_IMVM_DEPRECATED_PUBLIC_IF_H__ */

