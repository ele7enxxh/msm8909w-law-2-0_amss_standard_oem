/*===========================================================================
                        mcfg_setting.h

DESCRIPTION

   MCFG setting for the MCFG module
 
Copyright (c) 2012-2015 by QUALCOMM Technologies Incorporated.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/api/mcfg_setting.h#1 $ $DateTime: 2016/12/13 08:00:27 $ $Author: mplcsds1 $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
11/2/15      cys   Create

===========================================================================*/

#ifndef MCFG_SETTING_H
#define MCFG_SETTING_H

#include "comdef.h"
#include "mcfg_common.h"
#include "persistent_device_configuration_v01.h"

/*===========================================================================

  FUNCTION mcfg_set_feature

  DESCRIPTION
    Process the change request of feature setting for specified slot index

  DEPENDENCIES
    None

  PARAMETERS
    feature      [in]  feature setting for specified slot index

  RETURN VALUE
    mcfg_error_e_type mcfg error code

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_set_feature
(
  pdc_set_feature_req_msg_v01 *feature
);

/*===========================================================================

  FUNCTION mcfg_set_feature

  DESCRIPTION
    Queries the change request of feature setting for specified slot index

  DEPENDENCIES
    None

  PARAMETERS
    sloti        [in]  specified slot index
    feature      [in]  pointer of feature settings

  RETURN VALUE
    mcfg_error_e_type mcfg error code

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_get_feature
(
  mcfg_slot_index_type_e_type sloti, 
  pdc_get_feature_ind_msg_v01 *feature
);

#endif /* MCFG_SETTING_H */
