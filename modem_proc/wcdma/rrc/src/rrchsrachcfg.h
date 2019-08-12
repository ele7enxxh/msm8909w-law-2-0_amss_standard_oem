#ifndef RRCHSRACHCFG_H
#define RRCHSRACHCFG_H
/*========================================================================================

                                    R R C H S R A C H C F G . H

DESCRIPTION
  This file contains the definitions of the functions that handles the configuration of HS RACH.

  Copyright (c) 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
========================================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrchsrachcfg.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
  
 when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/17/14    sg      Made changes to move all RRC nv macros to wnv_rrc.h
06/20/14   db      Mainlined FEATURE_WCDMA_HS_RACH_DELAY_CU and introduced NV control
05/20/14   rmn     Made changes to save DS Tuneaway Status Change indication in connected state based on the concurrency NV
10/07/13   db      Made changes to configure downlink properly upon PCH to FACH thorugh MR
07/01/13   vg      Remove Unused macro COMMON_EDCH_SYS_INFO_WITH_2MS_TTI_IN_SIB5.
06/10/13   vg      HSRACH: Ignore HSRACH in SIB5 if 2ms EDCH-TTI is configured for UE HSUPA/EDCH CAT 1,3,5
03/22/13   mp      Removed tmc.h include file
01/03/12   ad/as  Support for FEATURE_WCDMA_HS_RACH
===========================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "wcdma_variation.h"
#ifdef FEATURE_WCDMA_HS_RACH
#include "uecomdef.h"
#include "rex.h"
#include "err.h"
#include "rrcllc.h"
#include "rrcdata.h"
#include "rrcllcoc.h"


#define RRC_IS_HSRACH_TURNED_ON ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8) && \
                                 (rrc_feature_nv_item_value & RRC_EFACH_ENABLED) && \
                                 (rrc_feature_nv_item_value & RRC_MAC_I_ENABLED) && \
                                 (rrc_feature_nv_item_value & RRC_HSRACH_ENABLED) && \
                                 (!(rrc_disable_cpc_fdpch_nv & (NV_FDPCH_DISABLE |NV_EFDPCH_DISABLE))))

#define RRC_IS_SIB5_DELAY_CU_ENABLED ((RRC_IS_HSRACH_TURNED_ON) && \
                                      (rrc_feature_nv_item_value & RRC_SIB5_DELAY_CELL_UPDATE_ENABLED))


/*This macro checks whether HSRACH support is present in SIB5 or not*/

#define COMMON_EDCH_SYS_INFO_PRESENT_IN_SIB5(sib5_ptr) \
  ((sib5_ptr->m.v4b0NonCriticalExtensionsPresent) &&\
    (sib5_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent) &&\
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v650NonCriticalExtensionsPresent) && \
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.m.v680NonCriticalExtensionsPresent) &&\
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.\
    m.v690NonCriticalExtensionsPresent) &&\
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.\
      v690NonCriticalExtensions.m.v770NonCriticalExtensionsPresent) &&\
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.\
      v690NonCriticalExtensions.v770NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) &&\
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.\
      v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.m.v890NonCriticalExtensionsPresent) &&\
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.\
      v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.v890NonCriticalExtensions.m.v8b0NonCriticalExtensionsPresent) &&\
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.\
      v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.v890NonCriticalExtensions.v8b0NonCriticalExtensions.m.\
      v8d0NonCriticalExtensionsPresent) &&\
    (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.\
      v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.v890NonCriticalExtensions.v8b0NonCriticalExtensions.\
      v8d0NonCriticalExtensions.sysInfoType5_v8d0ext.m.commonEDCHSystemInfoFDDPresent))


/*====================================================================
FUNCTION: rrchsrach_get_edch_rach_info_from_sib5()

DESCRIPTION:
  Update config pointer with HSRACH info from SIB5
  
DEPENDENCIES:


RETURN VALUE:
  SUCCESS/FAILURE

SIDE EFFECTS:
  ====================================================================*/
uecomdef_status_e_type rrchsrach_get_edch_rach_info_from_sib5
(
  ordered_config_type *config_ptr,
  rrc_state_e_type next_rrc_state
);

/*====================================================================
FUNCTION: rrchsrach_camped_cell_supports_hsrach()

DESCRIPTION:
This function checks whether the currently camped cell supports HSRACH or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrchsrach_camped_cell_supports_hsrach
(
  void
);
/*====================================================================
FUNCTION: rrchsrach_find_e_mac_d_flow_idx()

DESCRIPTION:
  Find a EDCH MAC-d flow Id.

DEPENDENCIES:
  None.

RETURN VALUE:
  uint8 -> A EDCH MAC-d flow Index.
  UE_MAX_MAC_D_FLOW -> Given EDCH Mac-d flow does not
           have an allocated DCH transport channel index.

SIDE EFFECTS:
====================================================================*/
uint8  rrchsrach_find_e_mac_d_flow_idx
(
  ordered_config_type *config_ptr,
  uint8 e_mac_d_flow_id
);
/*====================================================================
FUNCTION: rrchsrach_nv_supports_hs_rach()

DESCRIPTION:
This function checks whether the NV supports eFACH operation or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrchsrach_nv_supports_hs_rach
(
  void
);
void rrchsrach_print_cphy_setup(l1_setup_cmd_type *l1_cmd_ptr);
void rrchsrach_print_cell_transition_req(l1_cell_transition_cmd_type *l1_cmd_ptr);

void rrchsrach_print_cmac_ul_config(mac_ul_config_type *l2_cmd_ptr);
void rrchsrach_print_crlc_am_config(rlc_am_config_req_type *l2_cmd_ptr);
void rrchsrach_print_crlc_dl_tm_config(rlc_dl_tm_config_type *l2_cmd_ptr);
void rrchsrach_print_crlc_ul_tm_config(rlc_ul_tm_config_type *l2_cmd_ptr);
void rrchsrach_print_crlc_dl_um_config(rlc_dl_um_config_type *l2_cmd_ptr);
void rrchsrach_print_crlc_ul_um_config(rlc_ul_um_config_type *l2_cmd_ptr);
/*====================================================================
FUNCTION: rrchsrach_get_previous_cell_supports_hsrach()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrchsrach_get_previous_cell_supports_hsrach
(
  void
);

/*===========================================================================
FUNCTION rrchsrach_check_for_edch_flow_in_sib5

DESCRIPTION
Checks for flow-ID presense in SIB5

DEPENDENCIES

RETURN VALUE
TRUE if if present
SIDE EFFECTS
  None
===========================================================================*/

boolean rrchsrach_check_for_edch_flow_in_sib5
(
  uint8 mac_e_flow_id
);





#endif


#endif /*RRCHSRACHCFG_H*/
