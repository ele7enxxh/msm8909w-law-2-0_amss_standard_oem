#ifndef MCFG_SEL_H
#define MCFG_SEL_H
/*==============================================================================

                M O D E M   C O N F I G   S E L E C T I O N

GENERAL DESCRIPTION
  Contains the logic to automatically select a configuration file based on
  an outside identifying parameter, such as the ICCID read from the UIM.

Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_sel/inc/mcfg_sel.h#1 $
$DateTime: 2016/12/13 08:00:27 $
$Author: mplcsds1 $
$Change: 11985219 $

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
09/07/12 bd   Created module

==============================================================================*/
#include "mcfg_common.h"
#include "mcfg_trl.h"

#include "persistent_device_configuration_v01.h"

/*==============================================================================

                PUBLIC DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/


/*==============================================================================
  Typedefs
==============================================================================*/


/*==============================================================================
  Variables
==============================================================================*/


/*==============================================================================

                    PUBLIC FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

/*===========================================================================

  FUNCTION mcfg_sel_sw_config_store_updated

  DESCRIPTION
    Notifies the automatic configuration selection module that the stored
    SW configuration files have changed, e.g. new config uploaded, config
    deleted, etc. Ensures that automatic selection logic will run again
    at next boot to verify that we are using the best available config.

  DEPENDENCIES
    None

  PARAMETERS
    slot_index

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_sel_sw_config_store_updated
(
  mcfg_slot_index_type_e_type slot_index
);

/*===========================================================================

  FUNCTION mcfg_sel_sw_by_iccid

  DESCRIPTION
    Searches for a MCFG_SW matching the parameters for the given ICCID. If
    a match is found, the MCFG_SW will be selected and activated (if not
    already active).

  DEPENDENCIES
    None

  PARAMETERS
    iccid_ascii [in] Valid ICCID of the UIM currently inserted in slot 1;
                     ASCII encoding. Buffer size must be at least
                     MCFG_UIM_ICCID_STR_BUF_LEN bytes, and string must be
                     NULL-terminated

  RETURN VALUE
    None

  SIDE EFFECTS
    May result in a system reboot if the configuration is to be changed

===========================================================================*/
void mcfg_sel_sw_by_iccid
(
  mcfg_sub_id_type_e_type sub_id,
  const char             *iccid_ascii
);

#ifdef FEATURE_MCFG_SELECTION_BY_IMSI
/*===========================================================================

  FUNCTION mcfg_sel_sw_by_plmn

  DESCRIPTION
    Searches for a MCFG_SW matching the parameters for the given plmn. If
    a match is found, the MCFG_SW will be selected and activated (if not
    already active).

  DEPENDENCIES
    None

  PARAMETERS
    plmn [in] Valid plmn of the UIM currently inserted for specified
              subscription

  RETURN VALUE
    None

  SIDE EFFECTS
    May result in a system reboot if the configuration is to be changed

===========================================================================*/
void mcfg_sel_sw_by_plmn
(
  mcfg_sub_id_type_e_type          sub_id,
  mcfg_trl_carrier_mcc_mnc_s_type *plmn
);
#endif /* FEATURE_MCFG_SELECTION_BY_IMSI */

void mcfg_sel_sw_by_carrier
(
  mcfg_sub_id_type_e_type          sub_id,
  pdc_selection_carrier_enum_v01   carrier
);

#endif /* MCFG_SEL_H */
