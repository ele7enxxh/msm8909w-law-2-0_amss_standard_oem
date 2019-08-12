#ifndef _POLICYMAN_STATE_H_
#define _POLICYMAN_STATE_H_

/**
  @file policyman_state.h

  @brief
*/

/*
    Copyright (c) 2013-2016 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_state.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/

#include "policyman_i.h"
#include "policyman_call_events.h"
#include "policyman_diag.h"
#include "policyman_cfgitem.h"
#include "policyman_network_events.h"
#include "policyman_phone_events.h"
#include "policyman_rf.h"
#include "policyman_serving_system.h"
#include "policyman_subs.h"
#include "policyman_thermal_state.h"
#include "policyman_uim.h"



#define NO_MCC 1665              // 1665 is 0xFFF in BCD encoding and considered invalid MCC

typedef struct
{
  policyman_subs_info_t     *pSubsInfo;
  policyman_thermal_level_t  thermalLevel;
} policyman_device_info_t;

typedef struct
{
  sys_modem_as_id_e_type    asubs_id;
  sys_mcc_type              current_mcc;
  uint32                    preconditions_met;
  boolean                   simLockVerified;
  policyman_ss_info_t      *pSsInfo;
  policyman_cmph_info_t    *pCmPhInfo;
  policyman_uim_info_t     *pUimInfo;
  policyman_cmcall_info_t  *pCmCallInfo;
  policyman_network_info_t *pNetworkInfo;
} policyman_subs_state_t;

typedef struct
{
  sys_mcc_type         mcc;
  size_t               confidenceLevel;
  size_t               status;
  boolean              immediate_fullrat;
} policyman_global_info_t;

struct policyman_state_t
{
  policyman_device_info_t     *pDeviceInfo;
  policyman_subs_state_t      *pSubsState[POLICYMAN_NUM_SUBS];
  policyman_global_info_t     *pGlobalInfo;
  policyman_cmcall_history_t  *pCmcallHistory;
  policyman_cmss_history_t    *pCmssHistory;
  policyman_cmph_history_t    *pCmphHistory;
  policyman_notify_history_t  *pNotifyHistory;
};


/*=============================================================================
  General APIs
=============================================================================*/


/*-------- policyman_state_init --------*/
/**
@brief

@param

@return

*/
void policyman_state_init(
  void
  );


/*-------- policyman_state_static_init --------*/
/**
@brief

@param

@return

*/
void policyman_state_static_init(
  void
);


/*-------- policyman_state_deinit --------*/
/**
@brief

@param

@return

*/
void policyman_state_deinit(
  void
  );


/*-------- policyman_state_static_deinit --------*/
/**
@brief

@param

@return

*/
void policyman_state_static_deinit(
  void
);


/*-------- policyman_state_enter_crit_section --------*/
/**
@brief

@param

@return

*/
void policyman_state_enter_crit_section(
  void
);


/*-------- policyman_state_leave_crit_section --------*/
/**
@brief

@param

@return

*/
void policyman_state_leave_crit_section(
  void
);


/*-------- policyman_state_clone --------*/
/**
@brief

@param

@return

*/
policyman_state_t * policyman_state_clone(
  void
);


/*-------- policyman_state_delete_clone --------*/
/**
@brief

@param

@return

*/
void policyman_state_delete_clone(
  policyman_state_t *pState
);


/*-------- policyman_state_get_state --------*/
/**
@brief  Get a pointer to the policyman state.  This will in general only be
        called by unit tests - most use of policyman_state will be through
        parameters passed to functions.

@param

@return

*/
policyman_state_t * policyman_state_get_state(
  void
  );


/*-------- policyman_state_get_subs_state --------*/
/**
@brief

@param

@return

*/
policyman_subs_state_t * policyman_state_get_subs_state(
  policyman_state_t       *pState,
  sys_modem_as_id_e_type  asubs_id
);


/*-------- policyman_state_get_subs_precond --------*/
/**
@brief

@param

@return

*/
uint32 policyman_state_get_subs_precond(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
  );


/*-------- policyman_state_get_device_info --------*/
policyman_device_info_t * policyman_state_get_device_info(
  policyman_state_t *pState
);


/*-------- policyman_state_get_subs_info --------*/
/**
@brief  Get a pointer to the policyman subs state.

@param

@return

*/
policyman_subs_info_t * policyman_state_get_subs_info(
  policyman_state_t *pState
  );



/*-------- policyman_state_handle_update --------*/
/**
@brief  The policyman state has been udpated; policies should be processed to
        determine whether there is a policy update.

@return
  Nothing.
*/
void policyman_state_handle_update(
  policyman_state_t *pState
);


/*-------- policyman_state_set_precondition_met --------*/
/**
@brief  Indicate that a precondition has been met.

This should only be called on the main policyman thread - events that involve
preconditions should be queued to that thread.

@param[in]  pState  Policyman state in which to set the preconditon
@param[in]  precondition  Bitmask of the POLICYMAN_PRECOND_XXXX defines in
                          policyman_policy.h indicating which precondition has
                          now been met.
@param[in]  asubs_id subscription id for which precondiiton is required to be set

@return
  Nothing
*/
void policyman_state_set_precondition_met(
  policyman_state_t      *pState,
  uint32  precondition,
  sys_modem_as_id_e_type asubs_id
  );

/*-------- policyman_state_check_precondition_met --------*/
/**
@brief  Check that a precondition has been met.

@param[in]  pState  Policyman state in which to check the precondition
@param[in]  precondition  Bitmask of the POLICYMAN_PRECOND_XXXX defines in
                          policyman_policy.h indicating which precondition has
                          to be checked.
@param[in] asubs_id Subscription id to check for precondition

@return
  TRUE if precondition is met, FALSE otherwise
*/
boolean policyman_state_check_precondition_met(
  policyman_state_t      *pState,
  uint32                  precondition,
  sys_modem_as_id_e_type  asubs_id
);

/*-------- policyman_state_reset_precondition_met --------*/
/**
@brief  Reset a precondition that has been met.

@param[in]  pState  Policyman state in which to reset the preconditon
@param[in]  precondition  Bitmask of the POLICYMAN_PRECOND_XXXX defines in
                          policyman_policy.h indicating which precondition has
                          to be reset.
@param[in]  asubs_id  Subscription for which precondition has to be reset.

@return
  None
*/
void policyman_state_reset_precondition_met(
  policyman_state_t      *pState,
  uint32                  precondition,
  sys_modem_as_id_e_type  asubs_id
);

/*=============================================================================
  Serving System related APIs
=============================================================================*/

/*-------- policyman_state_get_serving_system_info --------*/
policyman_ss_info_t *policyman_state_get_serving_system_info(
  policyman_state_t       *pState,
  sys_modem_as_id_e_type   subsId
);


/*=============================================================================
  CM Phone Event related APIs
=============================================================================*/

/*===========================================================================
  FUNCTION POLICYMAN_STATE_UPDATE_PH_OPRT_MODE()

  DESCRIPTION
    Update operating mode into PM Phone state

  PARAMETERS
    oprt_mode : operating mode to be set
===========================================================================*/
void policyman_state_update_ph_oprt_mode(
  policyman_state_t      *pState,
  sys_oprt_mode_e_type oprt_mode
);


/*===========================================================================
  FUNCTION POLICYMAN_STATE_CMPH_NV_INIT()

  DESCRIPTION
    Read NVs related to CM PH state for a subscription

  PARAMETERS    
    asubs_id : subscription to read NVs for
===========================================================================*/
void policyman_state_cmph_nv_init(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type asubs_id
);


/*-------- policyman_state_get_cm_ph_state --------*/
policyman_cmph_info_t * policyman_state_get_cm_ph_state(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
);

/*=============================================================================
  UIM related APIs
=============================================================================*/

/*-------- policyman_state_get_uim_info --------*/
policyman_uim_info_t * policyman_state_get_uim_info(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
);


/*-------- policyman_state_get_uim_plmn --------*/
boolean policyman_state_get_uim_plmn(
  policyman_state_t const *pState,
  size_t                  subsId,
  sys_plmn_id_s_type      *pPlmn
  );

uint8 policyman_state_get_trm_cap(
  void
);

/*-------- policyman_state_set_sim_type --------*/
void policyman_state_set_sim_type(
  size_t              subsId,
  mmgsdi_app_enum_type sim_app_type
);

/*-------- policyman_state_set_cdma_sim_type --------*/
void policyman_state_set_cdma_sim_type(
  size_t               subsId,
  mmgsdi_app_enum_type sim_app_type
);

void policyman_state_set_simlock_verified(
  sys_modem_as_id_e_type  subsId,
  boolean                 verified
);

/*-------- policyman_state_get_cm_ph_state --------*/
policyman_cmph_info_t * policyman_state_get_cm_ph_state(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  asubs_id
);

/*===========================================================================
  FUNCTION POLICYMAN_STATE_GET_SIM_TYPE()

  DESCRIPTION
   Get SIM App type from UIM state
===========================================================================*/
void policyman_state_get_sim_type(
  size_t              subsId,
  mmgsdi_app_enum_type *sim_app_type
);

/*-------- policyman_state_get_cdma_sim_type --------*/
mmgsdi_app_enum_type policyman_state_get_cdma_sim_type(
  size_t                 subsId
);


/*=============================================================================
  Call related APIs
=============================================================================*/

/*-------- policyman_state_get_call_info --------*/
policyman_cmcall_info_t * policyman_state_get_call_info(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId
);


/*=============================================================================
  Hardware related APIs
=============================================================================*/

/*-------- policyman_state_get_imei --------*/
char const * policyman_state_get_imei(
  void
);


/*-------- policyman_state_get_raw_imei --------*/
byte * policyman_state_get_raw_imei(
  void
);


/*-------- policyman_state_get_trm_cap --------*/
uint8 policyman_state_get_trm_cap(
  void
);


/*=============================================================================
  Network Scan related APIs
=============================================================================*/

/*-------- policyman_state_update_network_scan_result --------*/
void policyman_state_update_network_scan_result(
  policyman_state_t *pState,
  cm_ph_info_s_type *pInfo
  );

/*-------- policyman_state_get_network_info --------*/
policyman_network_info_t * policyman_state_get_network_info(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type subsId
  );

/*=============================================================================
  HLOS MCC related APIs
=============================================================================*/

/*-------- policyman_state_update_hlos_mcc --------*/
void policyman_state_update_hlos_mcc(
  policyman_state_t *pState,
  sys_mcc_type hlos_mcc,
  size_t       confidenceLevel,
  size_t       status
);

/*-------- policyman_state_clear_hlos_mcc --------*/
void policyman_state_clear_hlos_mcc(
  policyman_state_t *pState
);

/*-------- policyman_state_get_hlos_mcc --------*/
sys_mcc_type policyman_state_get_hlos_mcc(
  policyman_state_t *pState
);

/*-------- policyman_state_set_subs_mcc --------*/
void policyman_state_set_subs_mcc(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type subsId,
  sys_mcc_type mcc
);

/*-------- policyman_state_update_subs_mcc --------*/
void policyman_state_update_subs_mcc(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId,
  sys_mcc_type mcc
);

/*-------- policyman_state_get_subs_mcc --------*/
sys_mcc_type policyman_state_get_subs_mcc(
  policyman_state_t      *pState,
  sys_modem_as_id_e_type  subsId
);

/*-------- policyman_state_get_simlock_verified --------*/
boolean policyman_state_get_simlock_verified(
  sys_modem_as_id_e_type  subsId
);

/*-------- policyman_state_update_simlock_verified --------*/
void policyman_state_update_simlock_verified(
  sys_modem_as_id_e_type  subsId,
  boolean                 verified
);

/*=============================================================================
  FULL RAT related APIs
=============================================================================*/

/*-------- policyman_state_update_full_rat --------*/
void policyman_state_update_full_rat(
  policyman_state_t *pState,
  boolean enterFullRat
);

/*-------- policyman_state_get_full_rat --------*/
boolean policyman_state_get_full_rat(
  policyman_state_t *pState
);

/*=============================================================================
  Internal functions exported to unit test code.
=============================================================================*/

/*-------- policyman_state_ss_init --------*/
void policyman_state_ss_init(
  policyman_state_t *pState
);

/*-------- policyman_state_uim_init --------*/
boolean policyman_state_uim_init(
  policyman_state_t *pState
  );

/*-------- policyman_state_cmph_init --------*/
boolean policyman_state_cmph_init(
  policyman_state_t *pState
  );

/*-------- policyman_state_subs_init --------*/
boolean policyman_state_subs_init(
  policyman_state_t *pState
  );

/*-------- policyman_state_cmcall_init --------*/
boolean policyman_state_cmcall_init(
  policyman_state_t *pState
  );

/*-------- policyman_state_network_init --------*/
boolean policyman_state_network_init(
  policyman_state_t *pState
  );

/*-------- policyman_state_trm_cap_init --------*/
void policyman_state_trm_cap_init(
  void
);

void policyman_state_update_thermal_level(
  policyman_thermal_level_t  thermallvl
);


/*-------- policyman_state_get_thermal_rat_disable_mask --------*/
uint32 policyman_state_get_thermal_rat_disable_mask(
  void
);

boolean policyman_state_thermal_allow_persistence(
  void
);	


/*-------- policyman_state_fill_diag_subs_info --------*/
void policyman_state_fill_diag_subs_info(
  sys_modem_as_id_e_type  asubs_id,
  pm_dump_subs_info_t    *dump_subs_info
);

#endif /* _POLICYMAN_STATE_H_ */
