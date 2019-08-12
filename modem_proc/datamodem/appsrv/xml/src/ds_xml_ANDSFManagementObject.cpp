/*==============================================================================

                      ds_xml_ANDSFManagementObject.cpp

GENERAL DESCRIPTION
  ANDSF structure

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
05/20/15    ml     Use memory from modem heap
04/21/14    ml     Created file/Initial version.
==============================================================================*/
#include "ds_xml_ANDSFManagementObject.h"

#include "ds_xml_ANDSF_FlowDistribution.h"
#include "ds_xml_ANDSF_Policy_WLANSP.h"
#include "ds_xml_ANDSF_RANValidity.h"
#include "ds_xml_ANDSF_RoutingCriteria.h"
#include "ds_xml_ANDSF_ValidityArea.h"


#include "ds_XMLDecoderException.h"
#include "ds_xml_ANDSF_constants.h"
#include "ds_XMLSchemaValidator.h"
#include "ds_XMLElement.h"
#include "ds_XMLUtility.h"
#include "ds_appsrv_mem.h"
#include "data_msg.h"
#include <cstdlib> // strtoul

namespace ASXML = Appsrv::XML;



/* ==========================================================================
 *                            ANDSF_DiscoveryInfo
 * ========================================================================== */
ASXML::ANDSF_DiscoveryInfo::ANDSF_DiscoveryInfo(const XMLElement* discovery_elem, bool& flag)
: access_network_area(NULL)
{
  typedef void (ANDSF_DiscoveryInfo::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(discovery_elem, flag, XMLElementCondition(NULL, true, true, 0, 5));

  // Flag will be set if current element is invalid.
  if(flag) return;


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_ACCESS_NETWORK_TYPE, true, false));
  parse_fcn_ptrs.push_back(&ANDSF_DiscoveryInfo::parse_access_network_type);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_ACCESS_NETWORK_AREA, true, true, 0, 5));
  parse_fcn_ptrs.push_back(&ANDSF_DiscoveryInfo::parse_access_network_area);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_VALIDITY_AREA_REF, false, true, 0));
  parse_fcn_ptrs.push_back(&ANDSF_DiscoveryInfo::parse_validity_area_ref);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_ACCESS_NETWORK_REF, false, true, 0));
  parse_fcn_ptrs.push_back(&ANDSF_DiscoveryInfo::parse_access_network_ref);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_PLMN, true, false));
  parse_fcn_ptrs.push_back(&ANDSF_DiscoveryInfo::parse_plmn);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


void ASXML::ANDSF_DiscoveryInfo::parse_access_network_type(const XMLElement* elem, bool& flag)
{
  access_network_type = strtoul(elem->get_text().c_str(), NULL, 10);
}


void ASXML::ANDSF_DiscoveryInfo::parse_access_network_area(const XMLElement* elem, bool& flag)
{
  access_network_area = new(ds_appsrv_alloc<ANDSF_ValidityArea>(), APPSRV_MEM) ANDSF_ValidityArea(elem, flag);
  if(access_network_area.is_null())
  {
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC, ELEM_NAME_ACCESS_NETWORK_AREA);
  }
}


void ASXML::ANDSF_DiscoveryInfo::parse_validity_area_ref(const XMLElement* elem, bool& flag)
{
  // Do Nothing
}


void ASXML::ANDSF_DiscoveryInfo::parse_access_network_ref(const XMLElement* elem, bool& flag)
{
    access_network_info_ref = elem->get_text();
}


void ASXML::ANDSF_DiscoveryInfo::parse_plmn(const XMLElement* elem, bool& flag)
{
  plmn = elem->get_text();
}




/* ==========================================================================
 *                              ANDSF_UEProfile
 * ========================================================================== */
ASXML::ANDSF_UEProfile::ANDSF_UEProfile(const XMLElement* profile_elem, bool& flag)
{
  XMLElementCondition filler_condition(NULL, true, true, 1);
  XMLElementCondition osid_condition(ELEM_NAME_OSID, true, false);
  XMLElementCondition dev_cap_condition(ELEM_NAME_DEV_CAPABILITY, false, false);
  const XMLElement*   child     = NULL;
  uint32              num_child = profile_elem->children_size();

  for(uint32 i = 0; i < num_child; ++i)
  {
    child = profile_elem->get_child(i);
    if(dev_cap_condition.validateElement(child, flag))
    {
      dev_capability = child->get_text();
      if(i + 1 != num_child)
        XMLDecoderException::get_instance().raise(flag, ERR_MSG_PARSE_EXCESS);

      break;
    }
    else if(filler_condition.validateElement(child, flag))
    {
      const XMLElement* osid_elem = child->get_child(0);
      if(osid_condition.validateElement(osid_elem, flag))
      {
        if(!ds_xml_add_obj_to_vector(
                                     new(ds_appsrv_alloc<ASString>(), APPSRV_MEM) ASString(osid_elem->get_text()),
                                     osid,
                                     flag,
                                     ELEM_NAME_OSID
                                     ))
        {
          return;
        }
      }
    }
    else
    {
      XMLDecoderException::get_instance().raise(flag, ERR_MSG_PARSE_INCOMPLETE);
    }

    if(flag)
    {
      return;
    }
  }
}



ASXML::ANDSF_UEProfile::~ANDSF_UEProfile()
{
  clear_ptr_vector(osid);
}




/* ==========================================================================
 *                           ANDSF_RuleSelectionInfo
 * ========================================================================== */
ASXML::ANDSF_RuleSelectionInfo::ANDSF_RuleSelectionInfo(const XMLElement* elem, bool& flag)
{
  typedef void (ANDSF_RuleSelectionInfo::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem);


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_VPLMNS_PREF_RULES, true, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_RuleSelectionInfo::parse_vplmns);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_PLMN, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_RuleSelectionInfo::parse_plmn);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


ASXML::ANDSF_RuleSelectionInfo::~ANDSF_RuleSelectionInfo()
{
  clear_ptr_vector(vplmns);
}


void ASXML::ANDSF_RuleSelectionInfo::parse_vplmns(const XMLElement* elem, bool& flag)
{
  XMLElementCondition filler_condition(NULL, true, true, 1);
  XMLElementCondition vplmn_condition(ELEM_NAME_VPLMN, true, false);

  for(uint32 i = 0; i < elem->children_size(); ++i)
  {
    const XMLElement* child = elem->get_child(i);
    if(filler_condition.validateElement(child, flag))
      return;

    child = child->get_child(0);
    if(vplmn_condition.validateElement(child, flag))
      return;

    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<ASString>(), APPSRV_MEM) ASString(child->get_text()),
                                 vplmns,
                                 flag,
                                 ELEM_NAME_VPLMN
                                 ))
    {
      return;
    }
  }
}


void ASXML::ANDSF_RuleSelectionInfo::parse_plmn(const XMLElement* elem, bool& flag)
{
  plmn = elem->get_text();
}




/* ==========================================================================
 *                                HOP_PrefSPs
 * ========================================================================== */
ASXML::HOP_PrefSPs::HOP_PrefSPs(const XMLElement* elem, bool& flag)
{
  typedef void (HOP_PrefSPs::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem, flag, XMLElementCondition(NULL, true, true, 2, 4));

  // Flag will be set if current element is invalid.
  if(flag) return;


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_PRIORITY, true, false));
  parse_fcn_ptrs.push_back(&HOP_PrefSPs::parse_priority);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_SERVICE_PROVIDER, true, false));
  parse_fcn_ptrs.push_back(&HOP_PrefSPs::parse_service_provider);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


void ASXML::HOP_PrefSPs::parse_priority(const XMLElement* elem, bool& flag)
{
  priority = strtoul(elem->get_text().c_str(), NULL, 10);
}


void ASXML::HOP_PrefSPs::parse_service_provider(const XMLElement* elem, bool& flag)
{
  srv_provider = elem->get_text();
}




/* ==========================================================================
 *                             ANDSF_HomeOperatorPref
 * ========================================================================== */
ASXML::ANDSF_HomeOperatorPref::ANDSF_HomeOperatorPref(const XMLElement* hop_elem, bool& flag)
{
  typedef void (ANDSF_HomeOperatorPref::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(hop_elem, flag, XMLElementCondition(NULL, true, true, 2, 4));

  // Flag will be set if current element is invalid.
  if(flag) return;


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_PSPL, true, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_HomeOperatorPref::parse_pspl);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_S2A_CONNECTIVITY_PREFERENCE, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_HomeOperatorPref::parse_s2a_connectivity_preference);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_PLMN, true, false));
  parse_fcn_ptrs.push_back(&ANDSF_HomeOperatorPref::parse_plmn);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_UPDATE_POLICY, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_HomeOperatorPref::parse_update_policy);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


ASXML::ANDSF_HomeOperatorPref::~ANDSF_HomeOperatorPref()
{
  clear_ptr_vector(equiv_home_sps);
  clear_ptr_vector(preferred_sps);
}


void ASXML::ANDSF_HomeOperatorPref::parse_pspl(const XMLElement* elem, bool& flag)
{
  typedef void (ANDSF_HomeOperatorPref::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem, flag, XMLElementCondition(NULL, true, true, 0, 3));

  // Flag will be set if current element is invalid.
  if(flag) return;


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_3GPP_RPLMN_PREF, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_HomeOperatorPref::parse_rplmn_pref);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_EQUIVALENT_HOME_SPS, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_HomeOperatorPref::parse_equiv_home_sps);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_PREFERRED_SPS, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_HomeOperatorPref::parse_preferred_sps);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


void ASXML::ANDSF_HomeOperatorPref::parse_s2a_connectivity_preference(const XMLElement* elem, bool& flag)
{
  s2a_conn_pref = elem->get_text();
}


void ASXML::ANDSF_HomeOperatorPref::parse_plmn(const XMLElement* elem, bool& flag)
{
  plmn = elem->get_text();
}


void ASXML::ANDSF_HomeOperatorPref::parse_update_policy(const XMLElement* elem, bool& flag)
{
  update_policy = str_to_bool(elem->get_text());
}


void ASXML::ANDSF_HomeOperatorPref::parse_rplmn_pref(const XMLElement* elem, bool& flag)
{
  rplmn_pref = elem->get_text();
}


void ASXML::ANDSF_HomeOperatorPref::parse_equiv_home_sps(const XMLElement* elem, bool& flag)
{
  XMLElementCondition filler_condition(NULL, true, true, 1);
  XMLElementCondition srv_condition(ELEM_NAME_SERVICE_PROVIDER, true, false);

  for(uint32 i = 0; i < elem->children_size(); ++i)
  {
    const XMLElement* child = elem->get_child(i);
    if(filler_condition.validateElement(child, flag))
      return;

    child = child->get_child(0);
    if(srv_condition.validateElement(child, flag))
      return;

    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<ASString>(), APPSRV_MEM) ASString(child->get_text()),
                                 equiv_home_sps,
                                 flag,
                                 ELEM_NAME_SERVICE_PROVIDER
                                 ))
    {
      return;
    }
  }
}


void ASXML::ANDSF_HomeOperatorPref::parse_preferred_sps(const XMLElement* elem, bool& flag)
{
  for(uint32 i = 0; i < elem->children_size(); ++i)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<HOP_PrefSPs>(), APPSRV_MEM) HOP_PrefSPs(elem->get_child(i), flag),
                                 preferred_sps,
                                 flag,
                                 ELEM_NAME_PREFERRED_SPS
                                 ))
    {
      return;
    }
  }
}



/* ==========================================================================
 *                            ANDSFManagementObject
 * ========================================================================== */
ASXML::ANDSFManagementObject::ANDSFManagementObject(const XMLElement* root_elem, bool& flag)
{
  typedef void (ANDSFManagementObject::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(root_elem);


  if(NULL == root_elem)
  {
    XMLDecoderException::get_instance().raise(flag, "Empty root");
    return;
  }

  check_element_with_child(root_elem, NULL, 0, 0, 12, flag);
  if(flag) return;

  if(0 == root_elem->children_size()) // empty is valid, but do nothing.
    return;

  // Add schema rules for ANDSF tree
  element_validator.addRule(XMLElementCondition(ELEM_NAME_NAME, false, true));
  parse_fcn_ptrs.push_back(&ANDSFManagementObject::parse_name);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_POLICY, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSFManagementObject::parse_policy);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_DISCOVERY_INFORMATION, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSFManagementObject::parse_discovery_info);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_UE_LOCATION, false, true, 0, 6));
  parse_fcn_ptrs.push_back(&ANDSFManagementObject::parse_ue_location);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_ISRP, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSFManagementObject::parse_isrp);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_UE_PROFILE, false, true, 0));
  parse_fcn_ptrs.push_back(&ANDSFManagementObject::parse_ue_profile);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_WLANSP, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSFManagementObject::parse_wlansp);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_IARP, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSFManagementObject::parse_iarp);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_RULE_SELECTION_INFO, false, true, 2, 2));
  parse_fcn_ptrs.push_back(&ANDSFManagementObject::parse_rule_selection_info);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_HOME_OPERATOR_PREF, false, true, 2, 4));
  parse_fcn_ptrs.push_back(&ANDSFManagementObject::parse_home_operator_pref);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_VALIDITY_CRITERIA, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSFManagementObject::parse_validity_criteria);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_EXT, false, true, 0));
  parse_fcn_ptrs.push_back(&ANDSFManagementObject::parse_ext);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}



ASXML::ANDSFManagementObject::~ANDSFManagementObject()
{
  clear_ptr_vector(policy);
  clear_ptr_vector(discovery_information);
  clear_ptr_vector(isrp);
  clear_ptr_vector(iarp);
  clear_ptr_vector(wlansp);
  clear_ptr_vector(validity_criteria);
}



void ASXML::ANDSFManagementObject::parse_name(const XMLElement* name_elem, bool& flag)
{
  name = name_elem->get_text();
}


void ASXML::ANDSFManagementObject::parse_policy(const XMLElement* policies, bool& flag)
{
  for (uint32 i = 0; i < policies->children_size(); ++i)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<ANDSF_Policy>(), APPSRV_MEM) ANDSF_Policy(policies->get_child(i), flag),
                                 policy,
                                 flag,
                                 ELEM_NAME_POLICY
                                 ))
    {
      return;
    }
  }
}



void ASXML::ANDSFManagementObject::parse_discovery_info(const XMLElement* discovery_infos, bool& flag)
{
  for (uint32 i = 0; i < discovery_infos->children_size(); ++i)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<ANDSF_DiscoveryInfo>(), APPSRV_MEM) ANDSF_DiscoveryInfo(discovery_infos->get_child(i), flag),
                                 discovery_information,
                                 flag,
                                 ELEM_NAME_DISCOVERY_INFORMATION
                                 ))
    {
      return;
    }
  }
}



void ASXML::ANDSFManagementObject::parse_ue_location(const XMLElement* ue_loc, bool& flag)
{
  ue_location = new(ds_appsrv_alloc<ANDSF_UELocation>(), APPSRV_MEM) ANDSF_UELocation(ue_loc, flag);

  if(ue_location.is_null())
  {
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC, "ANDSF_UELocation");
  }
}



void ASXML::ANDSFManagementObject::parse_isrp(const XMLElement* isrps, bool& flag)
{
  for (uint32 i = 0; i < isrps->children_size(); ++i)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<ANDSF_Isrp>(), APPSRV_MEM) ANDSF_Isrp(isrps->get_child(i), flag),
                                 isrp,
                                 flag,
                                 ELEM_NAME_ISRP
                                 ))
    {
      return;
    }
  }
}



void ASXML::ANDSFManagementObject::parse_ue_profile(const XMLElement* profile, bool& flag)
{
  ue_profile = new(ds_appsrv_alloc<ANDSF_UEProfile>(), APPSRV_MEM) ANDSF_UEProfile(profile, flag);
  if(ue_profile.is_null())
  {
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC, "ANDSF_UEProfile");
  }
}



void ASXML::ANDSFManagementObject::parse_wlansp(const XMLElement* wlansps, bool& flag)
{
  for (uint32 i = 0; i < wlansps->children_size(); ++i)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<ANDSF_Wlansp>(), APPSRV_MEM) ANDSF_Wlansp(wlansps->get_child(i), flag),
                                 wlansp,
                                 flag,
                                 ELEM_NAME_WLANSP
                                 ))
    {
      return;
    }
  }
}



void ASXML::ANDSFManagementObject::parse_iarp(const XMLElement* iarps, bool& flag)
{
  for (uint32 i = 0; i < iarps->children_size(); ++i)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<ANDSF_Iarp>(), APPSRV_MEM) ANDSF_Iarp(iarps->get_child(i), flag),
                                 iarp,
                                 flag,
                                 ELEM_NAME_IARP
                                 ))
    {
      return;
    }
  }
}



void ASXML::ANDSFManagementObject::parse_rule_selection_info(const XMLElement* elem, bool& flag)
{
  rule_selection_info = new(ds_appsrv_alloc<ANDSF_RuleSelectionInfo>(), APPSRV_MEM) ANDSF_RuleSelectionInfo(elem, flag);
  if(rule_selection_info.is_null())
  {
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC, ELEM_NAME_RULE_SELECTION_INFO);
  }
}



void ASXML::ANDSFManagementObject::parse_home_operator_pref(const XMLElement* elem, bool& flag)
{
  home_operator_pref = new(ds_appsrv_alloc<ANDSF_HomeOperatorPref>(), APPSRV_MEM) ANDSF_HomeOperatorPref(elem, flag);
  if(home_operator_pref.is_null())
  {
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC, ELEM_NAME_HOME_OPERATOR_PREF);
  }
}



void ASXML::ANDSFManagementObject::parse_validity_criteria(const XMLElement* elem, bool& flag)
{
  for(uint32 i = 0; i < elem->children_size(); ++i)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<ANDSF_ValidityCriteria>(), APPSRV_MEM) ANDSF_ValidityCriteria(elem->get_child(i), flag),
                                 validity_criteria,
                                 flag,
                                 ELEM_NAME_VALIDITY_CRITERIA
                                 ))
    {
      return;
    }
  }
}



void ASXML::ANDSFManagementObject::parse_ext(const XMLElement* ext_elem, bool& flag)
{
  ext = ext_elem;
}
