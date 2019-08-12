/*==============================================================================

                          ds_andsf_rule_manager.h

GENERAL DESCRIPTION
  ANDSF Rule Manager

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
#ifndef DS_ANDSF_RULE_MANAGER_H
#define DS_ANDSF_RULE_MANAGER_H

#include "datamodem_variation.h"
#ifdef FEATURE_DATA_ANDSF_RULE_MGR

#include "ds_andsf_location_manager.h"
#include "ds_andsf_ran_measurement_manager.h"
#include "ds_andsf_cs_manager.h"
#include "ds_andsf_time_manager.h"
#include "ds_andsf_priority_list_manager.h"
#include "ds_andsf_threshold_manager.h"
#include "ds_andsf_XMLParser.h"

#include "ds_andsf_ext_i.h"
#include "ds_ASVector.h"
#include "ds_ASMutex.h"

class ASBuffer;

namespace Appsrv
{
namespace XML
{

/* Forward declaration */
class ANDSF_FlowDistribution;
class ANDSF_RoutingRule;
class ANDSF_RoutingCriteria;
} /* namespace XML */


namespace ANDSF
{


class ANDSFRuleManager
{
public:
  ANDSFRuleManager();
  void init(const char* default_file_path, uint32 subs_idx);
  void clear();
  void update_rule_list(const char* file_path);
  void update_wwan_location(const ds_andsf_wwan_loc_info_s_type* wwan_loc_info);
  void update_wlan_location(const ds_andsf_wlan_loc_info_s_type* wlan_loc_info);

  void update_calltype_info(const ds_andsf_call_info_s_type* calltype_info);
  void update_lte_meas_info(const ds_andsf_lte_meas_info_s_type* lte_meas_info);
  void update_wlan_meas_info(const ds_andsf_wlan_meas_info_s_type* wlan_meas_info);
  void update_ehrpd_meas_info(const ds_andsf_ehrpd_meas_info_s_type* ehrpd_meas_info);
  void update_1x_meas_info(const ds_andsf_1x_meas_info_s_type* cdma_1x_meas_info);

  void update_roaming_status(bool roaming_status);

  void update_policy_threshold_cache();
  uint16 update_threshold_values(ANDSFThresholdValues& threshold_change_info, const char* file_path);

  // returns true if plist is changed
  bool update_plist();
  uint32 find_next_rule_timeout();
  void get_priority_list(ds_andsf_priority_info_s_type* plist);

  void get_meas_requirement(uint8& meas_bitmask) const;

  bool update_apn_name(const ASString& prev_apn_name, const ASString& new_apn_name, const char* file_path);
  bool check_apn_name_exist(const ASString& apn_name) const;

private:
  void init_plist(ANDSFPriorityListManager& plist_manager);
  bool update_plist(ANDSFPriorityListManager& plist_manager, uint32& change_mask);
  // Rule validation
  bool check_rule_criteria(const XML::ANDSF_FlowDistribution* rule, const char* apn_name) const;
  // Plist create
  void get_valid_rule_list(
                           const char*                                   apn_name,
                           ASVector<const XML::ANDSF_FlowDistribution*>& rule_list
                           ) const;
  void create_apn_list(const ASVector<XML::ANDSF_Isrp*>& isrp_list, ASVector<ASString>& apn_list) const;

  void update_plist_cache(uint32 change_mask);

  // Signal measurement requirement check
  void get_meas_requirement_helper(
                                   uint8&                            meas_bitmask,
                                   const ASVector<XML::ANDSF_Isrp*>& isrp_list
                                   ) const;
  void get_meas_requirement_helper(
                                   uint8&                            meas_bitmask,
                                   const XML::ANDSF_RoutingCriteria* criteria
                                   ) const;

private:
  // ANDSF cache
  XMLParser_ServiceBased        andsf_cache;
  // Time manager
  ANDSFTimeManager              time_manager;
  // Location manager
  ANDSFLocationManager          location_manager;
  // RAN manager for signal strength validation
  ANDSFRANMeasurementManager    ran_measurement_manager;
  // CS 1X manager
  ANDSFCSManager                cs_manager;
  // Priority list manager
  ANDSFPriorityListManager      active_plist_manager;
  ANDSFPriorityListManager      default_plist_manager;
  ANDSFThresholdManager         threshold_manager;
  // Priority list cache
  ds_andsf_priority_info_s_type priority_list;
  Utils::ASMutex                plist_mutex;

  bool                          cs_1x_available;
  bool                          is_roaming;
  uint32                        subs_idx;
};


} /* namespace ANDSF */
} /* namespace Appsrv */

#endif /* FEATURE_DATA_ANDSF_RULE_MGR */
#endif /* DS_ANDSF_RULE_MANAGER_H */
