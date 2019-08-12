#ifndef PS_DPM_UL_LEGACY_H
#define PS_DPM_UL_LEGACY_H
/*===========================================================================

                     PS_DPM . H
DESCRIPTION
Header file describing global DPM config definitions.

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dpm/src/ps_dpm_ul_legacy.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/10/13    bvd     Initial Version
=========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ps_iface_defs.h"
#include "dsm.h"


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
/**
  @brief  This API sets up legacy wmks.

  @code
  @endcode
*/
void ps_dpm_ul_legacy_init
(
  void
);


//MSR TODO Make it doxygen style. Fix description
/*===========================================================================
FUNCTION PS_DPM_UL_LEGACY_INPUT

DESCRIPTION

  This function processes the packet in legacy path.

PARAMETERS
  user_data : pointer to user data
  rx_pkt    : dsm_item_type rx pkt

RETURN VALUE
  TRUE  : if legacy watermark is flow disabled
  FALSE : if legacy watermark is flow enabled

DEPENDENCIES
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean ps_dpm_ul_legacy_input
(
  ps_iface_type  * rm_iface_ptr,
  dsm_item_type  * rx_pkt,
  void           * user_data
);

#endif /* PS_DPM_UL_LEGACY_H */
