#ifndef RRCENHSTATECFG_H
#define RRCENHSTATECFG_H

/*========================================================================================

                                    R R C E N H S T A T E CFG.H

DESCRIPTION
  This header file contains the declarations of the functions that handles the configuration of Enhanced FACH state.

  Copyright (c) 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.
========================================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcenhstatecfg.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
  
 when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/20/15   sr      Made changes for FR29345 EFS/NV Settings to be Read per Sub
09/17/14    sg      Made changes to move all RRC nv macros to wnv_rrc.h
06/04/14   sp      Made changes to init efach mappings for SRB0/1 after reselection
05/26/14   db      Made changes to include the correct diverstiy value when the optiona IE 
                   'Primary PCCPCH info'is omitted
05/20/14   rmn     Made changes to save DS Tuneaway Status Change indication in connected state based on the concurrency NV
03/13/14   sp      Removed code under FEATURE_WCDMA_HS_PCH feature flag
10/07/13   db      Made changes to configure downlink properly upon PCH to FACH thorugh MR
06/12/13   as      Made Changes to set the sttd indicator to FALSE when NW doesn't support
10/26/12   db      EFACH Redesign changes
10/23/12   pm      Made changes to correct enable HSRACH NV only when MACI and HSFACH
                   NVs are also ON
02/17/12   db      Fixed Compiler errors and warnings
12/20/11   db      Made changes to restore dedicated queue after moving to DCH
11/20/11   db      Made changes to backup and restore dedicated queue info
                   if queue id is same as in common queue
09/08/11   as      Added E-DRX changes for R8
08/02/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.103.00
20/07/11   as      Made changes to fill correct L1 params for EFACH-DRX
04/26/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.96.50
04/18/11   as      Added code to support feature EFACH-DRX 
04/07/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.52
03/30/11   as      Made changes to update rb mapping info for re-enter in epch cell
03/29/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.94.50
03/14/11   kp      Fixed compilation warning
03/28/11   rl      Added the file in Osys line due to HS-FACH/PCH changes. 
01/12/11   ad      Added changes for RRC SW decoupling
01/06/11   as      Made changes to delete commonn queue info while moving
                   to non EFACH cell or DCH state
10/04/10   prk     Added code to support FEATURE_WCDMA_HS_PCH.
06/07/10   dm/gkg  Support for FEATURE_WCDMA_HS_FACH

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "wcdma_variation.h"
#include "rrclcm.h"
#include "rrcllcoc.h"
#include "rrcdata_v.h"
#include "rrcccm.h"
#include "rrcsibdb.h"
#include "l1rrcif.h"
#include "rrcllc.h"
#include "uecomdef.h"

#ifdef FEATURE_WCDMA_HS_FACH

#define  RRC_IS_EFACH_NV_TURNED_ON ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL7) && \
                                    (rrc_feature_nv_item_value & RRC_EFACH_ENABLED))

#ifdef  FEATURE_WCDMA_HS_FACH_DRX
#define RRC_IS_EDRX_NV_TURNED_ON ((rrc_nv_rel_indicator & RRC_NV_REL_INDICATOR_REL8) && \
                                  (rrc_feature_nv_item_value & RRC_EFACH_DRX_ENABLED) && \
                                  (rrc_feature_nv_item_value & RRC_EFACH_ENABLED))
#endif

extern uint16 rrc_feature_nv_item_value;


/*===================================================================
                        DATA DECLARATIONS
=====================================================================*/



/*This macro checks whether EFACH support is present in SIB5 or not*/

#define HSDSCH_COMMON_SYS_INFO_PRESENT_IN_SIB5(sib5_ptr) \
((sib5_ptr->m.v4b0NonCriticalExtensionsPresent) &&\
   (sib5_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent) &&\
   (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v650NonCriticalExtensionsPresent) && \
   (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.m.v680NonCriticalExtensionsPresent) &&\
   (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.\
    m.v690NonCriticalExtensionsPresent) &&\
   (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.\
      v690NonCriticalExtensions.m.v770NonCriticalExtensionsPresent) &&\
   (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.\
      v690NonCriticalExtensions.v770NonCriticalExtensions.sysInfoType5_v770ext.m.modeSpecificInfoPresent) &&\
   (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.\
      v690NonCriticalExtensions.v770NonCriticalExtensions.sysInfoType5_v770ext.modeSpecificInfo.t == \
      T_rrc_SysInfoType5_v770ext_IEs_modeSpecificInfo_fdd))


#ifdef FEATURE_WCDMA_HS_FACH_DRX
#define HSDSCH_COMMON_HS_DRX_SYS_INFO_PRESENT_IN_SIB5(sib5_ptr) \
  ((sib5_ptr->m.v4b0NonCriticalExtensionsPresent) && \
     (sib5_ptr->v4b0NonCriticalExtensions.m.v590NonCriticalExtensionsPresent) && \
      (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.m.v650NonCriticalExtensionsPresent) && \
        (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.m.v680NonCriticalExtensionsPresent) && \
         (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.m.v690NonCriticalExtensionsPresent) && \
          (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.m.v770NonCriticalExtensionsPresent) && \
            (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.m.v860NonCriticalExtensionsPresent) && \
                (sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType5_v860ext.modeSpecificInfo.t == T_rrc_SysInfoType5_v860ext_IEs_modeSpecificInfo_fdd) && \
                  ((sib5_ptr->v4b0NonCriticalExtensions.v590NonCriticalExtensions.v650NonCriticalExtensions.v680NonCriticalExtensions.v690NonCriticalExtensions.v770NonCriticalExtensions.v860NonCriticalExtensions.sysInfoType5_v860ext.modeSpecificInfo.u.fdd->m.hs_dsch_DrxCellfach_infoPresent)))

#endif

/*===========================================================================
FUNCTION rrcenhstate_set_rb_mapping_info_for_bcch_on_efach

DESCRIPTION
  
DEPENDENCIES

RETURN VALUE
  Gets the index in Config database
SIDE EFFECTS
  None
===========================================================================*/
uint8 rrcenhstate_get_bcch_mapping_from_sib5
(
  ordered_config_type *config_ptr
);

/*====================================================================
FUNCTION: rrcenhstate_get_hs_dsch_common_info()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:
  None.
====================================================================*/
uecomdef_status_e_type rrcenhstate_get_hs_dsch_common_info
(
  ordered_config_type *config_ptr,
  rrc_state_e_type next_rrc_state,
  rrc_e_fach_operation_mode_e_type rrc_e_fach_operation_mode
);

/*====================================================================
FUNCTION: rrcenhstate_print_cphy_setup()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcenhstate_print_cphy_setup(l1_setup_cmd_type *l1_cmd_ptr);
/*====================================================================
FUNCTION: rrcenhstate_print_cell_transition_req()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcenhstate_print_cell_transition_req(l1_cell_transition_cmd_type *l1_cmd_ptr);
/*====================================================================
FUNCTION: rrcenhstate_print_cmac_dl_setup()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcenhstate_print_cmac_dl_setup(mac_dl_config_type *mac_cmd_ptr);

/*====================================================================
FUNCTION: rrcenhstate_print_cmac_ul_setup()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
void rrcenhstate_print_cmac_ul_config(mac_ul_config_type *mac_cmd_ptr);

/*====================================================================
FUNCTION: rrc_create_efach_nv_item()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/

void rrc_create_opt_feature_nv_item(sys_modem_as_id_e_type wrrc_as_id);

/*====================================================================
FUNCTION: rrc_read_efach_nv_item()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/

void rrc_read_opt_feature_nv_item(sys_modem_as_id_e_type wrrc_as_id);
/*====================================================================
FUNCTION: rrcllc_get_previous_cell_hs_status_in_e_fach()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  None.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_get_previous_cell_hs_status_in_e_fach
(
  void
);

/*====================================================================
FUNCTION: rrcenhstate_previous_camped_cell_supports_e_fach()

DESCRIPTION:

DEPENDENCIES:
  None
  
RETURN VALUE:
  boolean.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcenhstate_previous_camped_cell_supports_e_fach
(
  void
);


#ifdef FEATURE_WCDMA_HS_FACH_DRX

/*====================================================================
FUNCTION      rrcenhstate_get_t_321_value()

DESCRIPTION   This function returns the t_321 value.

DEPENDENCIES  None

RETURN VALUE  t 321 

SIDE EFFECTS  None.
====================================================================*/

uint16 rrcenhstate_get_t_321_value(rrc_T_321 t_321);

/*====================================================================
FUNCTION      rrcenhstate_get_drx_cycle_value()

DESCRIPTION   This function returns drx cycle value.

DEPENDENCIES  None

RETURN VALUE  drx cycle

SIDE EFFECTS  None.
====================================================================*/

uint8 rrcenhstate_get_drx_cycle_value(rrc_HS_DSCH_DrxCycleFach drx_cycle);

/*====================================================================
FUNCTION      rrcenhstate_get_drx_burst_value()

DESCRIPTION   This function returns the drx burst value.

DEPENDENCIES  None

RETURN VALUE  drx burst

SIDE EFFECTS  None.
====================================================================*/

uint8 rrcenhstate_get_drx_burst_value(rrc_HS_DSCH_DrxBurstFach drx_burst);

/*====================================================================
FUNCTION: rrcenhstate_nv_supports_e_fach_drx()

DESCRIPTION:
This function checks whether the NV supports eFACH drx operation or not

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcenhstate_nv_supports_e_fach_drx
(
  void
);

#endif /* FEATURE_WCDMA_HS_FACH_DRX */


/*====================================================================
FUNCTION: rrcllc_allocate_efach_q_index()

DESCRIPTION:


DEPENDENCIES:
  None.

RETURN VALUE:
  uint8 -> A EDCH MAC-d flow Index.
  UE_MAX_MAC_D_FLOW -> Given EDCH Mac-d flow does not
           have an allocated DCH transport channel index.

SIDE EFFECTS:
====================================================================*/
uint8  rrcllc_allocate_efach_q_index
(
  uint8  mac_ehs_q_id
);

/*====================================================================
FUNCTION: rrcllc_find_lc_index_for_efach()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:

====================================================================*/
uint8 rrcllc_find_lc_index_for_efach
( 
  ordered_config_type *config_ptr,
  rrc_LogicalChannelIdentity logicalChannelIdentity,
  uint16 rb_id
);

/*====================================================================
FUNCTION: rrcllc_allocate_lc_index_efach()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:

====================================================================*/
uint8  rrcllc_allocate_lc_index_efach
( 
  ordered_config_type *config_ptr,
  rrc_LogicalChannelIdentity logicalChannelIdentity
);



/*====================================================================
FUNCTION: rrcenhstate_check_for_efach_parms()

DESCRIPTION:
Checks if mapping, HRNTI, queues are present for configuring dedicated mode EFACH

DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:

====================================================================*/
uecomdef_status_e_type  rrcenhstate_check_for_efach_parms
( 
  rrc_RRC_StateIndicator rrc_StateIndicator,
  boolean validate_directed_cell_info,
  boolean validation_check,
  boolean new_hrnti_present
);


/*====================================================================
FUNCTION: rrcenhstate_stop_efach_if_active()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:

====================================================================*/
void  rrcenhstate_stop_efach_if_active
( 
  void
);

/*====================================================================
FUNCTION: rrcllc_check_efach_config_data()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:
  True/False.

SIDE EFFECTS:
  None.
====================================================================*/
boolean rrcllc_check_efach_config_data
(
  ordered_config_type *config_ptr,
  rrc_state_e_type next_rrc_state,
  rrc_e_fach_operation_mode_e_type efach_mode
);

/*====================================================================
FUNCTION: rrcenhstate_init_efach_mapping()

DESCRIPTION: Clears out HSFACH mapping info

DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:

====================================================================*/
void  rrcenhstate_init_efach_mapping
( 
  ordered_config_type *config_ptr
);

/*====================================================================
FUNCTION: rrcenhstate_init_efach_mapping_per_rb()

DESCRIPTION: Clears out HSFACH mapping info per rb

DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:

====================================================================*/

void rrcllc_init_efach_mapping_per_rb
(
 ordered_config_type *config_ptr, 
 uint16 rb_id
);


/*====================================================================
FUNCTION: rrcenhstate_init_efach_queues()

DESCRIPTION: Clears out HSFACH queue info
                    flag -> dediacated or common
DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:

====================================================================*/
void  rrcenhstate_init_efach_queues
( 
  ordered_config_type *config_ptr
);

/*====================================================================
FUNCTION: rrcenhstate_evaluate_efach_and_set_lower_layers_action()

DESCRIPTION:

DEPENDENCIES:
  None.

RETURN VALUE:


SIDE EFFECTS:

====================================================================*/
uecomdef_status_e_type  rrcenhstate_evaluate_efach_and_set_lower_layers_action
( 
  ordered_config_type *config_ptr,
  rrc_state_e_type next_rrc_state,
  rrc_e_fach_operation_mode_e_type rrc_e_fach_operation_mode
);



/*====================================================================
FUNCTION      rrcenhstate_get_queue_id()

DESCRIPTION 

DEPENDENCIES  Before calling this function, you should have done the check whether the cell supports EFACH

RETURN VALUE  
  SUCCESS if the entire operation is successful
  FAILURE otherwise

SIDE EFFECTS  None.
====================================================================*/
uint8 rrcenhstate_get_queue_id
(
  ordered_config_type *config_ptr,
  rb_id_type  rb_id
);


/*====================================================================
FUNCTION: rrcenhstate_sync_efach_queue()

DESCRIPTION: Clears out HSFACH queue info
                    flag -> dediacated or common
DEPENDENCIES:
  None.

RETURN VALUE:

SIDE EFFECTS:

====================================================================*/
void  rrcenhstate_sync_efach_queue
( 
  ordered_config_type *config_ptr
);


/*===========================================================================
FUNCTION   rrcenhstate_check_for_queue_id

DESCRIPTION
  Checks the queue ID presence in TOC.

DEPENDENCIES
  None.

RETURN VALUE
 TRUE if found
 else FALSE

SIDE EFFECTS
  None.
===========================================================================*/

boolean rrcenhstate_check_for_queue_id
(
  ordered_config_type *config_ptr,
  uint8  queue_id
);



#endif /* FEATURE_WCDMA_HS_FACH*/


#endif /*RRCENHSTATECFG_H*/
