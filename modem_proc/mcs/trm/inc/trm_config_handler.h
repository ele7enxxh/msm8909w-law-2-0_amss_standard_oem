#ifndef TRM_CONFIG_HANDLER_H
#define TRM_CONFIG_HANDLER_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

           T R A N S C E I V E R   R E S O U R C E   M A N A G E R

              Transceiver Resource Manager Wakeup Header File

GENERAL DESCRIPTION

  This file provides API for the clients to register for wakeup information
  with TRM. With the registration on every TRM call the wakeup information
  will be communicated to the client.


EXTERNALIZED FUNCTIONS

  trm_wmgr_reg_wakeup_cb() - Wakup Mgr registers for notification of future wakeups
  and chain owners from TRM

REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  trm_init() must be called before any other function.



  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/trm/inc/trm_config_handler.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
06/08/2015   sp      Add tech bitmask array to distinguish between sub capabilities (825586)
01/12/2014   mn      Map second config chain to chain2 when DR capability is
                      detected. (CR: 778424).
12/07/2014   sk      Use active sub info updated by MMOC(767564)
10/30/2014   sk      Change 8-bit modes to 32 -bit 
09/17/2014   sk      Changes to update active sub mask from MMOC(713328) 
08/18/2014   sk      support for fallback full RAT DSDA mode in SV+G DSDA(702293)
08/01/2014   mn      Set second preferred chain for 1x same as first preferred in 
                      non-SHDR, non-SLTE cases (CR: 698522).
07/11/2014   sk      Ignore Single Sim case in legacy DSDA (690913)    
07/08/2014   mn      VCO coupling logic should use the any_bp_bookkeeper.
                      (CR: 690472).
07/07/2014   mn      Changes to support single SIM scenario for TRM
                      mode changes(CR: 679494)     
06/18/2014   mn      Changes for DR-DSDS (CR: 651800)
04/14/2014   mn      TRM should look at feature_mode provided by CM for single SIM
                      cases. (CR: 646746).
03/12/2014   mn      Adding support for connected mode WTR hopping.
01/09/2014   mn      Only change mode at TRM when there is an actual mode 
                     change (CR:597982).
08/12/2013   mn      Initial version.

============================================================================*/

/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#include "trmi.h"
#include "subs_prio.h"

/*============================================================================

                                   MACROS

============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

============================================================================*/

/*----------------------------------------------------------------------------
  TRM config handler hopping config
----------------------------------------------------------------------------*/
typedef struct
{
  /* Flag to signify whether connected mode hopping is enabled */
  boolean                connected_mode_hop_enabled;

  /* Flag to signify whether connected mode hopping is enabled */
  boolean                idle_mode_hop_enabled;
} trm_config_handler_hopping_config_info_type;

/*----------------------------------------------------------------------------
  TRM config information
----------------------------------------------------------------------------*/
typedef struct
{
  /* This is a combined tech bitmask for all subscriptions indicating the RATs
     that can be activated. */
  uint32                                      tech_supported_bitmask;

  /* This is an array for tech bitmasks (per subscription) indicating the RATs
     that can be activated. */
  uint32                                      tech_supported_bitmask_arr[MAX_NUM_OF_SUBSCRIPTIONS];

  /* Default data tech */
  uint32                                      num_active_simul_mode_stacks;

  /* Default data sub  */
  sys_modem_as_id_e_type                      default_data_subs;

  /* This is a bitmask denoting the modes used to set compatibilities.
     The bit positions are similar to trm.modes_enabled */
  uint32                                      compatibility_modes;
  
  /* This is a bitmask denoting the modes used to set WTR/chain mappings.
     The bit positions are similar to trm.modes_enabled */
  uint32                                      chain_mapping_modes;

    /* num active subscriptions */
  uint32                                      num_concurr_active_subs;

  /* num standby subscriptions */
  uint32                                      num_standby_subs;

  /* Active number of subscription */
  uint32                                      num_of_active_subs;

  /* active feature mode mask */
  uint32                                      feature_mode_active;

    /* All feature mode mask, includes feature from inactive subs as well */
  uint32                                      feature_mode_all;

  /* Service scan mode */
  uint8                                       svc_scan_mode;

  /* updated indication, set when config data is updated at least once
     from CM */
  boolean                                     cm_updated;


} trm_config_handler_config_info_type;

/*----------------------------------------------------------------------------
  Data for Mode Callback. To be used with trm_config_handler_mode_cb_type
----------------------------------------------------------------------------*/
typedef struct 
{
  /* This is a bitmask denoting the modes.
     The bit positions are similar to trm.modes_enabled */
  uint32                                       modes;

  /* TRM client and chain mapping information. */
  trm_client_chain_info_type                  chain_info;
  
  /* Config for hopping */
  trm_config_handler_hopping_config_info_type hop_config;

  /* Tech supported bitmask only for sub0 */
  uint32                                      tech_supported_bitmask_sub0;
} trm_config_handler_config_cb_data_type;

/*----------------------------------------------------------------------------
  The prototype for for receiving callback when the mode changes 
  at TRM config handler.
----------------------------------------------------------------------------*/
typedef void (*trm_config_handler_config_cb_fn_type)
(
  trm_config_handler_config_cb_data_type* config_data
);

/*----------------------------------------------------------------------------
  Data for Initialization. To be used with trm_config_handler_init
----------------------------------------------------------------------------*/
typedef struct
{
  /* Initial modes that need to be enabled */
  uint32            modes;

  /* DR mode */
  uint32           dr_mask;

  /* RF capability. Should be set to trm.rf_hw_capability */
  uint32            rf_capability;

  /* Flag for SHDR */
  boolean           shdr_enabled;

  /* Dual receive capability 
      bit 0: SLTE_1X
      bit 1: SLTE_GSM
      bit 2: DR_DSDS 
  */
  uint32            rf_dual_receive_capability;

  /* Mode callback that needs to be called when
     there is any config change */
  trm_config_handler_config_cb_fn_type mode_cb;
} tcxomgr_config_handler_init_data_type;

/*----------------------------------------------------------------------------
  Data for active sub info. Updated By MMOC
----------------------------------------------------------------------------*/
typedef struct
{
  /* Mask with active subs */
  uint8 active_sub_mask;

  /* Indication if data has been updated by MMOC once:
     Till mmoc updates, TRM will need to look at CM provided info
     once updated, TRM will alwasys need to look at MMOC provided info */
  boolean mmoc_updated;

}trm_active_sub_info_type;

/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/*============================================================================

FUNCTION TRM_CONFIG_HANDLER_SAVE_NEW_CONFIG

DESCRIPTION
  This function can be used by Subs Prio module save new configuration. This 
  should be followed by a call to trm_config_handler_commit_new_config for
  the new configuration to actually take effect.

DEPENDENCIES
  Subscription priority should be holding lock on TRM.

RETURN VALUE
  TRUE if the save was successful.
  FALSE if the save was unsuccessful. Cause could be that hardware does not 
  support the configuration.

SIDE EFFECTS
  None.

============================================================================*/
boolean trm_config_handler_save_new_config
(
  trm_config_handler_config_info_type* config
);

/*============================================================================

FUNCTION TRM_CONFIG_HANDLER_PROCESS_MODE_CHANGE

DESCRIPTION
  This function can be used by Subs Prio module to process the mode change
  request. This function will determine the new mode and save the new
  mode

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Result will be modified based on whether mode change was successful.

============================================================================*/
void trm_config_handler_process_mode_change
(
   sp_subscription_info*            subs_info,
   sp_mode_change_result_data_t*          result_info,
   trm_config_handler_config_info_type*   config_info
);

/*============================================================================

FUNCTION TRM_CONFIG_HANDLER_COMMIT_NEW_CONFIG

DESCRIPTION
  This function can be used by Subs Prio module set the new configuration.

DEPENDENCIES
  Subscription priority should be holding lock on TRM.

RETURN VALUE
  TRUE if the update was successful.
  FALSE if the update was unsuccessful.

SIDE EFFECTS
  None.

============================================================================*/
boolean trm_config_handler_commit_new_config(void);

/*============================================================================

FUNCTION TRM_CONFIG_HANDLER_GET_SIMULT_CAP_BITMASK

DESCRIPTION
  This function can be used to get the simultaneos capability bitmask from
  the mode bitmask.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/
void trm_config_handler_get_simult_cap_bitmask
(
  uint32* simult_cap,
  uint8   modes
);


/*============================================================================

FUNCTION TRM_CONFIG_HANDLER_GET_NUM_SIMUL_STACK

DESCRIPTION
  This function can be used to get the number of simultaneous stack on SxLTE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/
uint32 trm_config_handler_get_num_simul_stack(void);

/*============================================================================

FUNCTION TRM_CONFIG_HANDLER_INIT

DESCRIPTION
  This API needs to be called at TRM initialization.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/
void trm_config_handler_init
(
  tcxomgr_config_handler_init_data_type* init_data
);

/*============================================================================

FUNCTION TRM_CONFIG_HANDLER_GET_SRLTE_DSDS_DUAL_MULTI_MODE

DESCRIPTION
  This function checks if the current mode is SRLTE+DSDS or SRLTE only and
  returns it in.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: SRLTE+DSDS Mode.
  FALSE: Not in SRLTE+DSDS Mode.

SIDE EFFECTS
  None.

============================================================================*/
uint8 trm_config_handler_get_srlte_dsds_dual_multi_mode(void);

/*============================================================================

FUNCTION TRM_CONFIG_HANDLER_UPDATE_ACTIVE_SUB_INFO

DESCRIPTION
  This function is used to update the active sub mask whenever MMOC updates
  the info to subs_prio.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/
boolean trm_config_handler_update_active_sub_info( uint8 active_sub_mask );

/*============================================================================

FUNCTION TRM_CONFIG_HANDLER_GET_ACTIVE_SUB_INFO

DESCRIPTION
  This function is used to get the active sub mask that MMOC updates.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/
uint8 trm_config_handler_get_active_sub_info
( 
  sp_subscription_info*            subs_info
);

#ifdef __cplusplus
}
#endif

#endif /* TRM_CONFIG_HANDLER_H */

