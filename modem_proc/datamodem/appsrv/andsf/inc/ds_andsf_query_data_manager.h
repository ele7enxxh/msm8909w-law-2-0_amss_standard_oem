/*==============================================================================

                    ds_andsf_query_data_manager.h

GENERAL DESCRIPTION
  Manager for RAN signal strength

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/20/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_ANDSF_QUERY_DATA_MANAGER_H
#define DS_ANDSF_QUERY_DATA_MANAGER_H

#include "datamodem_variation.h"
#ifdef FEATURE_DATA_ANDSF_RULE_MGR

#include "ds_andsf_ext_i.h"

#include "ds_andsf_location_update.h"
#include "ds_andsf_ran_meas.h"
#include "ds_andsf_cm_if.h"
#include "ds_andsf_wqe_prof_mgr.h"

class ASString;

namespace Appsrv
{
namespace ANDSF
{

/* Query API Internal Request */
bool register_cb(ds_andsf_event_cb_fcn_type cb_fcn, void* user_data_ptr);
bool deregister_cb(ds_andsf_event_cb_fcn_type cb_fcn);
void get_priority_list(uint32 subs_idx, ds_andsf_priority_info_s_type* priority_list);

/* ANDSF/UE Information Update */
void event_timeout(uint32 subs_idx);
void event_update_file(uint32 subs_idx, const char* file_path);
void event_update_location_wwan(
                                uint32                               subs_idx,
                                const ds_andsf_wwan_loc_info_s_type* wwan_loc_info
                                );
void event_update_location_wlan(const ds_andsf_wlan_loc_info_s_type* wlan_loc_info);
void event_update_calltype_info(
                                uint32                           subs_idx,
                                const ds_andsf_call_info_s_type* calltype_info
                                );
void event_update_lte_meas_info(
                                uint32                               subs_idx,
                                const ds_andsf_lte_meas_info_s_type* lte_meas_info
                                );
void event_update_wlan_meas_info(const ds_andsf_wlan_meas_info_s_type* wlan_meas_info);
void event_update_ehrpd_meas_info(
                                  uint32                                 subs_idx,
                                  const ds_andsf_ehrpd_meas_info_s_type* ehrpd_meas_info
                                  );
void event_update_1x_meas_info(
                               uint32                              subs_idx,
                               const ds_andsf_1x_meas_info_s_type* cdma_1x_meas_info
                               );
void event_update_roaming_status(uint32 subs_idx, bool roaming_status);

void event_update_apn_name(
                           uint32          subs_idx,
                           const ASString& old_apn_name,
                           const ASString& new_apn_name
                           );
uint16 event_update_threshold(
                              uint32                                    subs_idx,
                              const ds_andsf_policy_threshold_chg_type* threshold_info_ptr
                              );

// Defined in ds_andsf_query_event_handler.cpp
void ds_andsf_timeout_timer_cb(uint32 user_data);

} /* namespace ANDSF */
} /* namespace Appsrv */

#endif /* FEATURE_DATA_ANDSF_RULE_MGR */




/*==============================================================================
FUNCTION      ds_andsf_rule_data_init

DESCRIPTION   Internal function for initializing ANDSF Query API at powerup.

PARAMETERS    None

DEPENDENCIES  Must be called once only

RETURN VALUE  None

SIDE EFFECTS  None
==============================================================================*/
void ds_andsf_rule_mgr_data_init();

void ds_andsf_event_handler_init();

#endif /* DS_ANDSF_QUERY_DATA_MANAGER_H */
