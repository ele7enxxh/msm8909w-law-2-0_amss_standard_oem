#ifndef RRCENHSTATEPROC_H
#define RRCENHSTATEPROC_H

/*========================================================================================

                                    R R C E N H S T A T E P R O C.H

DESCRIPTION
  This header file contains the declarations of the functions that handles the configuration of Enhanced FACH state.

  Copyright (c) 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.
========================================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcenhstateproc.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
  
 when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/13/14   sp      Removed code under FEATURE_WCDMA_HS_PCH feature flag
10/26/12   db      EFACH Redesign changes
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/20/11   as      Made changes to handle cell update confirm with dhrnti and epch state
03/28/11   rl      Added the file in Osys line due to HS-FACH/PCH changes. 
01/12/11   ad      Added changes for RRC SW decoupling
11/18/10   prk     Added function prototype.
11/12/10   prk     Modified function prototype.
10/04/10   prk     Added code to support FEATURE_WCDMA_HS_PCH.
10/01/10   su      Fixed compiler warnings
06/07/10   dm/gkg  Support for FEATURE_WCDMA_HS_FACH

===========================================================================*/

#include "wcdma_variation.h"
#include "rrcllc.h"
#include "rrcllci.h"
#include "rrclcm.h"
#include "rrcllcoc.h"
#include "rrcdata_v.h"
#include "l1rrcif.h"
#include "uecomdef.h"
#include "rrcscmgr.h"

#ifdef FEATURE_WCDMA_HS_FACH

/*====================================================================
FUNCTION: rrcenhstate_process_add_ccch()

DESCRIPTION:

DEPENDENCIES:
  None

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type  rrcenhstate_process_add_ccch
(
  void
);

/*====================================================================
FUNCTION: rrcenhstate_process_reconfig_ccch()

DESCRIPTION:

DEPENDENCIES:
  None

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type  rrcenhstate_process_reconfig_ccch
(
  void
);

/*====================================================================
FUNCTION: rrcenhstate_process_add_bcch_on_hsdsch()

DESCRIPTION:

DEPENDENCIES:
  None

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type  rrcenhstate_process_add_bcch_on_hsdsch
(
  void
);

/*====================================================================
FUNCTION: rrcenhstate_config_phychan_for_resel()

DESCRIPTION:

DEPENDENCIES:
  None

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcenhstate_config_phychan_for_resel
(
  rrc_state_e_type  curr_rrc_state
);

/*====================================================================
FUNCTION: rrcenhstate_add_cell_trans_info()

DESCRIPTION:

DEPENDENCIES:
  None

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/
void rrcenhstate_add_cell_trans_info
(
  l1_cell_transition_cmd_type *cell_trans
);

/*====================================================================
FUNCTION: rrcenhstate_setup_phychan()

DESCRIPTION:

DEPENDENCIES:
  None

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type  rrcenhstate_setup_phychan
(
  void
);


/*====================================================================
FUNCTION: rrcenhstate_reconfig_phychan()

DESCRIPTION:

DEPENDENCIES:
  None

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type  rrcenhstate_reconfig_phychan
(
  rrc_e_fach_operation_mode_e_type mode
);

/*====================================================================
FUNCTION: rrcenhstate_release_phychan()

DESCRIPTION:

DEPENDENCIES:
  None

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type  rrcenhstate_release_phychan
(
  rrc_e_fach_operation_mode_e_type mode
);

/*====================================================================
FUNCTION: rrcenhstate_add_hs_pdsch()

DESCRIPTION:

DEPENDENCIES:
  None

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcenhstate_add_hs_pdsch
(
  void
);

/*====================================================================
FUNCTION: rrcenhstate_reconfig_hs_pdsch()

DESCRIPTION:

DEPENDENCIES:
  None

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcenhstate_reconfig_hs_pdsch
(
  void
);


/*====================================================================
FUNCTION: rrcenhstate_release_hs_pdsch()

DESCRIPTION:

DEPENDENCIES:
  None

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcenhstate_release_hs_pdsch
(
  void
);

/*====================================================================
FUNCTION: rrcenhstate_process_add_bcch_on_hsdsch()

DESCRIPTION:

DEPENDENCIES:
  None

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type  rrcenhstate_process_reconfig_bcch_on_hsdsch
(
  void
);

/*====================================================================
FUNCTION: rrcenhstate_set_hsdpa_l1_info_in_cmd_under_process()

DESCRIPTION:
  This function sets HSDPA Information in L1 struct in rrcllc_cmd_under_process

DEPENDENCIES:
  None.

RETURN VALUE:
  Success/Failure.

SIDE EFFECTS:
  None.
====================================================================*/

uecomdef_status_e_type rrcenhstate_set_hsdpa_l1_info_in_cmd_under_process
(
  ordered_config_type *config_ptr
);

#endif /* FEATURE_WCDMA_HS_FACH*/

#endif /*RRCENHSTATEPROC_H*/
