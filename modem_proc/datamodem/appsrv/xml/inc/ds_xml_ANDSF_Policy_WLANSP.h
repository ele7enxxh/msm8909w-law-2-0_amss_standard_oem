/*==============================================================================

                          ds_xml_ANDSF_Policy_WLANSP.h

GENERAL DESCRIPTION
  ANDSF Policy and WLANSP structure

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/02/15    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_XML_ANDSF_POLICY_WLANSP_H
#define DS_XML_ANDSF_POLICY_WLANSP_H

#include "ds_ASAssignType.h"
#include "ds_ASAutoPointer.h"
#include "ds_ASString.h"
#include "ds_ASVector.h"


namespace Appsrv
{
namespace XML
{

class XMLElement;
class ANDSF_PrioritizedAccess;
class ANDSF_ValidityArea;
class ANDSF_TimeOfDay;



/* ==========================================================================
 *                             ANDSF_PreferredSSID
 * ========================================================================== */
class ANDSF_PreferredSSID
{
public:
  ANDSF_PreferredSSID(const XMLElement* elem, bool& flag);

private:
  void parse_ssid(const XMLElement* elem, bool& flag);
  void parse_hessid(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  ASString ssid;
  ASString hessid;
};



/* ==========================================================================
 *                         ANDSF_SelectionCriteria
 * ========================================================================== */
class ANDSF_SelectionCriteria
{
public:
  ANDSF_SelectionCriteria(const XMLElement* elem, bool& flag);
  ~ANDSF_SelectionCriteria();

private:
  void parse_criteria_priority(const XMLElement* elem, bool& flag);
  void parse_home_network_indication(const XMLElement* elem, bool& flag);
  void parse_preferred_roaming_partner_list(const XMLElement* elem, bool& flag);
  void parse_min_backhaul_threshold(const XMLElement* elem, bool& flag);
  void parse_max_bss_load_value(const XMLElement* elem, bool& flag);
  void parse_required_proto_port_tuple(const XMLElement* elem, bool& flag);
  void parse_sp_exclusion_list(const XMLElement* elem, bool& flag);
  void parse_preferred_ssid_list(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  uint32                         criteria_priority; //req
  bool                           home_network_indication;
  const XMLElement*              preferred_roaming_partner_list;
  const XMLElement*              min_backhaul_threshold;
  ASString                       max_bss_load_value;
  const XMLElement*              required_proto_port_tuple;
  const XMLElement*              sp_exclusion_list;
  ASVector<ANDSF_PreferredSSID*> preferred_ssid_list;
};




/* ==========================================================================
 *                           ANDSF_Policy_Wlansp_base
 * ========================================================================== */
class ANDSF_Policy_Wlansp_base
{
public:
  ANDSF_Policy_Wlansp_base();
  ~ANDSF_Policy_Wlansp_base();

protected:
  void parse_rule_priority(const XMLElement* elem, bool& flag);
  void parse_prioritized_access(const XMLElement* elem, bool& flag);
  void parse_selection_criteria(const XMLElement* elem, bool& flag);
  void parse_validity_area(const XMLElement* elem, bool& flag);
  void parse_validity_area_ref(const XMLElement* elem, bool& flag);
  void parse_roaming(const XMLElement* elem, bool& flag);
  void parse_plmn(const XMLElement* elem, bool& flag);
  void parse_time_of_day(const XMLElement* elem, bool& flag);
  void parse_time_of_day_ref(const XMLElement* elem, bool& flag);
  void parse_update_policy(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  uint32                             rule_priority;      // Req
  ASVector<ANDSF_PrioritizedAccess*> prioritized_access; // Req - Policy(ISMP)
  ASVector<ANDSF_SelectionCriteria*> selection_criteria; // Req - WLANSP
  ASAutoPointer<ANDSF_ValidityArea>  validity_area;
  Utils::ASAssignType<bool>          roaming;
  ASString                           plmn;               // Req
  ASVector<ANDSF_TimeOfDay*>         time_of_day;
  bool                               update_policy;      // Default 0
};



/* ==========================================================================
 *                                 ANDSF_Policy
 * ========================================================================== */
class ANDSF_Policy : public ANDSF_Policy_Wlansp_base
{
public:
  ANDSF_Policy(const XMLElement* elem, bool& flag);
};



/* ==========================================================================
 *                                 ANDSF_Wlansp
 * ========================================================================== */
class ANDSF_Wlansp : public ANDSF_Policy_Wlansp_base
{
public:
  ANDSF_Wlansp(const XMLElement* elem, bool& flag);
};




} /* namespace ASXML */
} /* namespace ds */


#endif /* DS_XML_ANDSF_POLICY_WLANSP_H */
