/*==============================================================================

                    ds_andsf_ran_measurement_manager.h

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
#ifndef DS_ANDSF_RAN_MEASUREMENT_MANAGER_H
#define DS_ANDSF_RAN_MEASUREMENT_MANAGER_H

#include "datamodem_variation.h"
#ifdef FEATURE_DATA_ANDSF_RULE_MGR

#include "ds_andsf_ran_meas.h"

#include "ds_ASAutoPointer.h"
#include "ds_ASString.h"
#include "ds_ASVector.h"


namespace Appsrv
{
namespace XML
{

/* Forward declaration */
class ANDSF_RANValidity;
class ANDSF_OPRANValidity;
class ANDSF_OPThresholdConditions;
class ANDSF_OPLTEThreshold;
class ANDSF_OPWiFiThreshold;
class ANDSF_OPeHRPDThreshold;

} /* namespace XML */


namespace ANDSF
{


// Manager for RAN signal strength
class ANDSFRANMeasurementManager
{
public:
  ANDSFRANMeasurementManager();

  void update_calltype(const ds_andsf_call_info_s_type* calltype_info);
  void update_lte_meas_info(const ds_andsf_lte_meas_info_s_type* lte_meas_info);
  void update_wlan_meas_info(const ds_andsf_wlan_meas_info_s_type* wlan_meas_info);
  void update_ehrpd_meas_info(const ds_andsf_ehrpd_meas_info_s_type* ehrpd_meas_info);

  void clear();

  bool validate_ran(const ASAutoPointer<XML::ANDSF_RANValidity>& ran_validity, const char* apn_name) const;
  bool validate_ran(const ASAutoPointer<XML::ANDSF_OPRANValidity>& op_ran_validity, const char* apn_name) const;

private:
  // Operator RAN Validation
  bool validate_threshold(const XML::ANDSF_OPThresholdConditions* op_ran_validity) const;
  bool validate_lte_threshold(const XML::ANDSF_OPLTEThreshold* threshold_info) const;
  bool validate_wifi_threshold(const XML::ANDSF_OPWiFiThreshold* threshold_info) const;
  bool validate_ehrpd_threshold(const XML::ANDSF_OPeHRPDThreshold* threshold_info) const;


  struct CalltypeInfo
  {
    ASString apn_name;
    uint8    calltype;

    CalltypeInfo();
    CalltypeInfo(const ds_andsf_call_info_s_type* calltype_info);

    bool operator==(const char* apn) const;
  };


  ASVector<CalltypeInfo>          calltype_info_list;
  ds_andsf_lte_meas_info_s_type   lte_measurement_info;
  ds_andsf_wlan_meas_info_s_type  wlan_measurement_info;
  ds_andsf_ehrpd_meas_info_s_type ehrpd_measurement_info;
};


} /* namespace ANDSF */
} /* namespace Appsrv */


#endif /* FEATURE_DATA_ANDSF_RULE_MGR */
#endif /* DS_ANDSF_RAN_MEASUREMENT_MANAGER_H */
