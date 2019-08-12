/*==============================================================================

                          ds_andsf_location_manager.h

GENERAL DESCRIPTION
  ANDSF Location Manager

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/26/15    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_ANDSF_LOCATION_MANAGER_H
#define DS_ANDSF_LOCATION_MANAGER_H

#include "datamodem_variation.h"
#ifdef FEATURE_DATA_ANDSF_RULE_MGR

#include "ds_ASVector.h"
#include "ds_ASAutoPointer.h"

#include "ds_andsf_location_update.h"
#include "ds_xml_ANDSF_ValidityArea.h"

namespace Appsrv
{
namespace ANDSF
{


/* ==========================================================================
 * Class ANDSFLocationManager
 *
 * DESCRIPTION
 *  This class manages UE location information and supports validation
 *  against ANDSF location criteria.
 * ========================================================================== */
class ANDSFLocationManager
{
public:
  /* ========================================================================
   * ANDSFLocationManager update_wwan_location
   * ANDSFLocationManager update_wlan_location
   *
   * DESCRIPTION
   *  Updates the current cached UE WWAN/WLAN location information.
   *
   * PARAMETERS
   *  [In] wwan_loc_info - UE WWAN location information
   *  [In] wlan_loc_info - UE WLAN location information
   *
   * RETURN VALUE
   *  None
   *
   * DEPENDENCIES
   *  None
   * ======================================================================== */
  void update_wwan_location(const ds_andsf_wwan_loc_info_s_type* wwan_loc_info);
  void update_wlan_location(const ds_andsf_wlan_loc_info_s_type* wlan_loc_info);

  void clear();

  /* ========================================================================
   * ANDSFLocationManager is_location_valid
   *
   * DESCRIPTION
   *  Checks if the current UE location meets the location criteria for ANDSF.
   *
   * PARAMETERS
   *  [In] validity_area - ANDSF location validity criteria
   *
   * RETURN VALUE
   *  True if UE location meets ANDSF location criteria
   *
   * DEPENDENCIES
   *  None
   * ======================================================================== */
  bool is_location_valid(const ASAutoPointer<XML::ANDSF_ValidityArea>& validity_area) const;

private:
  bool compare_wwan(
                    const ASVector<XML::VA_3gppLocation*>& va_3gpp,
                    const ASAutoPointer<XML::VA_3gpp2Location>& va_3gpp2
                    ) const;
  bool compare_3gpp(const ASVector<XML::VA_3gppLocation*>& va_3gpp) const;
  bool compare_3gpp2_1x(const ASVector<XML::VA_3gpp2Location_1x*>& va_3gpp2_1x) const;
  bool compare_3gpp2_hrpd(const ASVector<XML::VA_3gpp2Location_Hrpd*>& va_3gpp2_hrpd) const;
  bool compare_wlan(const ASVector<XML::VA_WlanLocation*>& va_wlan) const;


  bool                       is_3gpp_avail;
  bool                       is_3gpp2_1x_avail;
  bool                       is_3gpp2_hrpd_avail;
  bool                       is_wlan_avail;
  XML::VA_3gppLocation       ue_3gpp_loc_info;
  XML::VA_3gpp2Location_1x   ue_3gpp2_1x_loc_info;
  XML::VA_3gpp2Location_Hrpd ue_3gpp2_hrpd_loc_info;
  XML::VA_WlanLocation       ue_wlan_loc_info;

public:
  /* Debug */
  void print() const;
};


} /* namespace ANDSF */
} /* namespace Appsrv */

#endif /* FEATURE_DATA_ANDSF_RULE_MGR */
#endif /* DS_ANDSF_LOCATION_MANAGER_H */
