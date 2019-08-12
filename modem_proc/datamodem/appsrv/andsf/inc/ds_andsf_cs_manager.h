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
#ifndef DS_ANDSF_CS_MANAGER_H
#define DS_ANDSF_CS_MANAGER_H

#include "datamodem_variation.h"
#ifdef FEATURE_DATA_ANDSF_RULE_MGR

#include "ds_andsf_ran_meas.h"

#include "ds_ASVector.h"

namespace Appsrv
{
namespace XML
{

/* Forward declaration */
class ANDSF_Isrp;
class ANDSF_ForCSBased;

} /* namespace XML */


namespace ANDSF
{


// Manager for RAN signal strength
class ANDSFCSManager
{
public:
  ANDSFCSManager();
  void clear();

  void update_1x_meas_info(const ds_andsf_1x_meas_info_s_type* cdma_1x_meas_info);

  bool validate_cs(const ASVector<XML::ANDSF_Isrp*>& isrp_list, bool roaming_status) const;

private:
  bool validate_cs_criteria(const XML::ANDSF_ForCSBased* op_ran_validity) const;

  ds_andsf_1x_meas_info_s_type cdma_1x_measurement_info;
};


} /* namespace ANDSF */
} /* namespace Appsrv */


#endif /* FEATURE_DATA_ANDSF_RULE_MGR */
#endif /* DS_ANDSF_CS_MANAGER_H */
