/*==============================================================================

                        ds_andsf_priority_list_manager.h

GENERAL DESCRIPTION
  ANDSF Priority List Manager

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
#ifndef DS_ANDSF_PLIST_MANAGER_H
#define DS_ANDSF_PLIST_MANAGER_H

#include "datamodem_variation.h"
#ifdef FEATURE_DATA_ANDSF_RULE_MGR

#include "ds_andsf_ext_i.h"
#include "ds_ASVector.h"
#include "ds_ASString.h"


namespace Appsrv
{
namespace XML
{

/* Forward declaration */
class ANDSF_FlowDistribution;
class ANDSF_RoutingRule;

} /* namespace XML */


namespace ANDSF
{

class APNPriorityListInfo;


/* ==========================================================================
 * Class ANDSFPriorityListManager
 *
 * DESCRIPTION
 *  This class manages all APN priority list information.
 * ========================================================================== */
class ANDSFPriorityListManager
{
public:
  /* ========================================================================
   * ANDSFPriorityListManager Constructor / Destructor
   *
   * DESCRIPTION
   *  ANDSFPriorityListManager constructor / destructor.
   *
   * PARAMETERS
   *  None
   *
   * RETURN VALUE
   *  None
   *
   * DEPENDENCIES
   *  None
   * ======================================================================== */
  ANDSFPriorityListManager();
  ~ANDSFPriorityListManager();

  /* ========================================================================
   * ANDSFPriorityListManager clear_apn_list
   *
   * DESCRIPTION
   *  Clear the current APN list
   *
   * PARAMETERS
   *  None
   *
   * RETURN VALUE
   *  None
   *
   * DEPENDENCIES
   *  None
   * ======================================================================== */
  void clear_apn_list();

  /* ========================================================================
   * ANDSFPriorityListManager update_apn_list
   *
   * DESCRIPTION
   *  Updates its APN list with the given APN list. APN name not found in the
   *    its APN list will be added to the list. Missing APNs in the given APN
   *    list will be ignored.
   *
   * PARAMETERS
   *  [In] apn_list - APN list to use to update the current APN list.
   *
   * RETURN VALUE
   *  True if APN list changes, else false.
   *
   * DEPENDENCIES
   *  None
   * ======================================================================== */
  bool update_apn_list(const ASVector<ASString>& apn_list);

  /* ========================================================================
   * ANDSFPriorityListManager update_priority_list
   *
   * DESCRIPTION
   *  Updates the priority list of the specified APN using the given valid
   *    rule list.
   *
   * PARAMETERS
   *  [In] apn_name        - The name of APN to update
   *  [In] valid_rule_list - List of valid rules for the APN
   *
   * RETURN VALUE
   *  True if the APN priority list changes, else false.
   *
   * DEPENDENCIES
   *  None
   * ======================================================================== */
  bool update_priority_list(
                            const ASString&                                     apn_name,
                            const ASVector<const XML::ANDSF_FlowDistribution*>& valid_rule_list
                            );

  /* ========================================================================
   * ANDSFPriorityListManager get_plist
   *
   * DESCRIPTION
   *  Fills the input object with the current priority list info of each APN.
   *
   * PARAMETERS
   *  [Out] out_list - The object to fill APN priority info list.
   *
   * RETURN VALUE
   *  None
   *
   * DEPENDENCIES
   *  None
   * ======================================================================== */
  void get_plist(ds_andsf_priority_info_s_type* out_list) const;

private:
  struct WLANMacAddrInfo
  {
    ASString ssid;
    uint8    hessid[DS_SYS_MAC_ADDR_LEN];

    WLANMacAddrInfo();
    WLANMacAddrInfo(const ASString& ssid_in, uint8* hessid_in);
  };


  void add_tech_to_list(
                        const XML::ANDSF_RoutingRule*               routing_rule,
                        bool                                        tech_set[DS_ANDSF_TECH_MAX],
                        ASVector<WLANMacAddrInfo>&                  wlan_accessid_vector,
                        ASVector<ds_andsf_access_tech_info_s_type>& priority_list
                        );
  APNPriorityListInfo* get_apn_plist(const ASString& apn_name);


  /* Class Member */
  ASVector<APNPriorityListInfo*> apn_priority_list;


public:
  /* Debug */
  void print();
};



} /* namespace ANDSF */
} /* namespace Appsrv */

#endif /* FEATURE_DATA_ANDSF_RULE_MGR */
#endif /* DS_ANDSF_PLIST_MANAGER_H */
