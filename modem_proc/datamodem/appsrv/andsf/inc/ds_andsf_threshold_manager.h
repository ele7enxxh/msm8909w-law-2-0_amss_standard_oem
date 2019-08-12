/*==============================================================================

                          ds_andsf_threshold_manager.h

GENERAL DESCRIPTION
  ANDSF Threshold Manager

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
#ifndef DS_ANDSF_THRESHOLD_MANAGER_H
#define DS_ANDSF_THRESHOLD_MANAGER_H

#include "datamodem_variation.h"
#ifdef FEATURE_DATA_ANDSF_RULE_MGR

// #include "ds_andsf_ext_i.h"
#include "ds_ASVector.h"
#include "ds_ASAssignType.h"

class ASBuffer;

namespace Appsrv
{
/* Forward declaration */
namespace XML
{
class ANDSF_Isrp;
} /* namespace XML */



namespace ANDSF
{

struct ANDSFOPRANThresholdValues
{
  Utils::ASAssignType<int32> rsrp_low;
  Utils::ASAssignType<int32> rsrp_med;
  Utils::ASAssignType<int32> rsrp_high;
  Utils::ASAssignType<int32> rssi_low;
  Utils::ASAssignType<int32> rssi_high;
  Utils::ASAssignType<int32> sinr_low;
  Utils::ASAssignType<int32> sinr_high;

  void clear();
  // Returns true if a value is set
  bool is_set() const;
  void update(const ANDSFOPRANThresholdValues& rhs);
  void print() const; // Debug
};

struct ANDSFThresholdValues
{
  ANDSFOPRANThresholdValues in_call;
  ANDSFOPRANThresholdValues idle_call;

  Utils::ASAssignType<int32> ecio_low;
  Utils::ASAssignType<int32> ecio_high;


  void clear();
  // Returns true if a value is set
  bool is_set() const;
  void update(const ANDSFThresholdValues& rhs);
  void print() const; // Debug
};



class ANDSFThresholdManager
{
public:
  void update_cache(const ASVector<XML::ANDSF_Isrp*>& isrp_list);

  uint16 update_threshold_values(ANDSFThresholdValues& chg_values, const char* file_path);


private:
  void clear();
  void update_cache(const ANDSFThresholdValues& chg_values);

  // Validates change values. If change value == cache value, it will unset change flag for that value
  bool validate_threshold_chg_values(ANDSFThresholdValues& chg_values);
  bool validate_threshold_chg_values_helper(
                                            const Utils::ASAssignType<int32>& cache_low,
                                            const Utils::ASAssignType<int32>& cache_high,
                                            Utils::ASAssignType<int32>&       chg_low,
                                            Utils::ASAssignType<int32>&       chg_high
                                            );

  bool validate_threshold_chg_values_helper(
                                            const Utils::ASAssignType<int32>& cache_low,
                                            const Utils::ASAssignType<int32>& cache_med,
                                            const Utils::ASAssignType<int32>& cache_high,
                                            Utils::ASAssignType<int32>&       chg_low,
                                            Utils::ASAssignType<int32>&       chg_med,
                                            Utils::ASAssignType<int32>&       chg_high
                                            );

  bool validate_ecio_chg_values(
                                const Utils::ASAssignType<int32>& cache_low,
                                const Utils::ASAssignType<int32>& cache_high,
                                Utils::ASAssignType<int32>&       chg_low,
                                Utils::ASAssignType<int32>&       chg_high
                                );

  void add_to_list(ASVector<int32>& list, const Utils::ASAssignType<int32>& value);

  void update_value(
                    const Utils::ASAssignType<int32>& cache_value,
                    Utils::ASAssignType<int32>&       chg_value,
                    Utils::ASAssignType<int32>&       result_value
                    );

  void update_value_from_list(
                              ASVector<int32>&            value_list,
                              Utils::ASAssignType<int32>& low_val,
                              Utils::ASAssignType<int32>& high_val
                              );
  void update_value_from_list(
                              ASVector<int32>&            value_list,
                              Utils::ASAssignType<int32>& low_val,
                              Utils::ASAssignType<int32>& med_val,
                              Utils::ASAssignType<int32>& high_val
                              );
  void update_ecio_value_from_list(
                                   ASVector<int32>&            low_value_list,
                                   ASVector<int32>&            high_value_list,
                                   Utils::ASAssignType<int32>& low_val,
                                   Utils::ASAssignType<int32>& high_val
                                   );

private:
  ANDSFThresholdValues cache_values;
};




} /* namespace ANDSF */
} /* namespace Appsrv */

#endif /* FEATURE_DATA_ANDSF_RULE_MGR */
#endif /* DS_ANDSF_THRESHOLD_MANAGER_H */
