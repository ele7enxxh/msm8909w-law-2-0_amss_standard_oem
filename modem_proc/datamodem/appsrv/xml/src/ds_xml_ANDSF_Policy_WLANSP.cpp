/*==============================================================================

                      ds_xml_ANDSF_Policy_WLANSP.cpp

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
05/20/15    ml     Use memory from modem heap
03/02/15    ml     Created file/Initial version.
==============================================================================*/
#include "ds_xml_ANDSF_Policy_WLANSP.h"

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
#include <cstdlib> // strtoul

namespace ASXML = Appsrv::XML;



/* ==========================================================================
 *                            ANDSF_PreferredSSID
 * ========================================================================== */
ASXML::ANDSF_PreferredSSID::ANDSF_PreferredSSID(const XMLElement* elem, bool& flag)
{
  typedef void (ANDSF_PreferredSSID::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem, flag, XMLElementCondition(NULL, true, true, 0, 2));

  // Flag will be set if current element is invalid.
  if(flag) return;


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_SSID, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_PreferredSSID::parse_ssid);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_HESSID, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_PreferredSSID::parse_hessid);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


void ASXML::ANDSF_PreferredSSID::parse_ssid(const XMLElement* elem, bool& flag)
{
  ssid = elem->get_text();
}


void ASXML::ANDSF_PreferredSSID::parse_hessid(const XMLElement* elem, bool& flag)
{
  hessid = elem->get_text();
}



/* ==========================================================================
 *                            ANDSF_SelectionCriteria
 * ========================================================================== */
ASXML::ANDSF_SelectionCriteria::ANDSF_SelectionCriteria(const XMLElement* elem, bool& flag)
: preferred_roaming_partner_list(NULL),
  min_backhaul_threshold(NULL),
  required_proto_port_tuple(NULL),
  sp_exclusion_list(NULL)
{
  typedef void (ANDSF_SelectionCriteria::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem, flag, XMLElementCondition(NULL, true, true, 1, 8));

  // Flag will be set if current element is invalid.
  if(flag) return;


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_CRITERIA_PRIORITY, true, false));
  parse_fcn_ptrs.push_back(&ANDSF_SelectionCriteria::parse_criteria_priority);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_HOME_NETWORK_INDICATION, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_SelectionCriteria::parse_home_network_indication);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_PREFERRED_ROAMING_PARTNER_LIST, false, true, 0));
  parse_fcn_ptrs.push_back(&ANDSF_SelectionCriteria::parse_preferred_roaming_partner_list);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_MIN_BACKHAUL_THRESHOLD, false, true, 0));
  parse_fcn_ptrs.push_back(&ANDSF_SelectionCriteria::parse_min_backhaul_threshold);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_MAXIMUM_BSS_LOAD_VALUE, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_SelectionCriteria::parse_max_bss_load_value);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_REQUIRED_PROTO_PORT_TUPLE, false, true, 0));
  parse_fcn_ptrs.push_back(&ANDSF_SelectionCriteria::parse_required_proto_port_tuple);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_SP_EXCLUSION_LIST, false, true, 0));
  parse_fcn_ptrs.push_back(&ANDSF_SelectionCriteria::parse_sp_exclusion_list);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_PREFERRED_SSID_LIST, false, true, 0, 2));
  parse_fcn_ptrs.push_back(&ANDSF_SelectionCriteria::parse_preferred_ssid_list);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


ASXML::ANDSF_SelectionCriteria::~ANDSF_SelectionCriteria()
{
  clear_ptr_vector(preferred_ssid_list);
}


void ASXML::ANDSF_SelectionCriteria::parse_criteria_priority(const XMLElement* elem, bool& flag)
{
  criteria_priority = strtoul(elem->get_text().c_str(), NULL, 10);
}


void ASXML::ANDSF_SelectionCriteria::parse_home_network_indication(const XMLElement* elem, bool& flag)
{
   home_network_indication = str_to_bool(elem->get_text());
}


void ASXML::ANDSF_SelectionCriteria::parse_preferred_roaming_partner_list(const XMLElement* elem, bool& flag)
{
  preferred_roaming_partner_list = elem;
}


void ASXML::ANDSF_SelectionCriteria::parse_min_backhaul_threshold(const XMLElement* elem, bool& flag)
{
  min_backhaul_threshold = elem;
}


void ASXML::ANDSF_SelectionCriteria::parse_max_bss_load_value(const XMLElement* elem, bool& flag)
{
   max_bss_load_value = elem->get_text();
}


void ASXML::ANDSF_SelectionCriteria::parse_required_proto_port_tuple(const XMLElement* elem, bool& flag)
{
  required_proto_port_tuple = elem;
}


void ASXML::ANDSF_SelectionCriteria::parse_sp_exclusion_list(const XMLElement* elem, bool& flag)
{
  sp_exclusion_list = elem;
}


void ASXML::ANDSF_SelectionCriteria::parse_preferred_ssid_list(const XMLElement* elem, bool& flag)
{
  for(uint32 i = 0; i < elem->children_size(); ++i)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<ANDSF_PreferredSSID>(), APPSRV_MEM) ANDSF_PreferredSSID(elem->get_child(i), flag),
                                 preferred_ssid_list,
                                 flag,
                                 ELEM_NAME_PREFERRED_SSID_LIST
                                 ))
    {
      return;
    }
  }
}


/* ==========================================================================
 *                            ANDSF_Policy_Wlansp_base
 * ========================================================================== */
ASXML::ANDSF_Policy_Wlansp_base::ANDSF_Policy_Wlansp_base()
: validity_area(NULL), update_policy(false)
{ }


ASXML::ANDSF_Policy_Wlansp_base::~ANDSF_Policy_Wlansp_base()
{
  clear_ptr_vector(prioritized_access);
  clear_ptr_vector(selection_criteria);
  clear_ptr_vector(time_of_day);
}


void ASXML::ANDSF_Policy_Wlansp_base::parse_rule_priority(const XMLElement* elem, bool& flag)
{
  rule_priority = strtoul(elem->get_text().c_str(), NULL, 10);
}


void ASXML::ANDSF_Policy_Wlansp_base::parse_prioritized_access(const XMLElement* elem, bool& flag)
{
  for(uint32 i = 0; i < elem->children_size(); ++i)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<ANDSF_PrioritizedAccess>(), APPSRV_MEM) ANDSF_PrioritizedAccess(elem->get_child(i), flag),
                                 prioritized_access,
                                 flag,
                                 ELEM_NAME_PRIORITIZED_ACCESS
                                 ))
    {
      return;
    }
  }
}


void ASXML::ANDSF_Policy_Wlansp_base::parse_selection_criteria(const XMLElement* elem, bool& flag)
{
  for(uint32 i = 0; i < elem->children_size(); ++i)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<ANDSF_SelectionCriteria>(), APPSRV_MEM) ANDSF_SelectionCriteria(elem->get_child(i), flag),
                                 selection_criteria,
                                 flag,
                                 ELEM_NAME_SELECTION_CRITERIA
                                 ))
    {
      return;
    }
  }
}


void ASXML::ANDSF_Policy_Wlansp_base::parse_validity_area(const XMLElement* elem, bool& flag)
{
  validity_area = new(ds_appsrv_alloc<ANDSF_ValidityArea>(), APPSRV_MEM) ANDSF_ValidityArea(elem, flag);
  if(validity_area.is_null())
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC, ELEM_NAME_VALIDITY_AREA);
}


void ASXML::ANDSF_Policy_Wlansp_base::parse_validity_area_ref(const XMLElement* elem, bool& flag)
{
  // Do nothing
}


void ASXML::ANDSF_Policy_Wlansp_base::parse_roaming(const XMLElement* elem, bool& flag)
{
  roaming = str_to_bool(elem->get_text());
}


void ASXML::ANDSF_Policy_Wlansp_base::parse_plmn(const XMLElement* elem, bool& flag)
{
  plmn = elem->get_text();
}


void ASXML::ANDSF_Policy_Wlansp_base::parse_time_of_day(const XMLElement* elem, bool& flag)
{
  for(uint32 i = 0; i < elem->children_size(); ++i)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<ANDSF_TimeOfDay>(), APPSRV_MEM) ANDSF_TimeOfDay(elem->get_child(i), flag),
                                 time_of_day,
                                 flag,
                                 ELEM_NAME_TIME_OF_DAY
                                 ))
    {
      return;
    }
  }
}


void ASXML::ANDSF_Policy_Wlansp_base::parse_time_of_day_ref(const XMLElement* elem, bool& flag)
{
  // Do nothing
}


void ASXML::ANDSF_Policy_Wlansp_base::parse_update_policy(const XMLElement* elem, bool& flag)
{
  update_policy = str_to_bool(elem->get_text());
}






ASXML::ANDSF_Policy::ANDSF_Policy(const XMLElement* elem, bool& flag)
{
  typedef void (ANDSF_Policy::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem, flag, XMLElementCondition(NULL, true, true, 3, 9)); // <X>+

  // Flag will be set if current element is invalid.
  if(flag) return;


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_RULE_PRIORITY, true, false));
  parse_fcn_ptrs.push_back(&ANDSF_Policy::parse_rule_priority);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_PRIORITIZED_ACCESS, true, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_Policy::parse_prioritized_access);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_VALIDITY_AREA, false, true, 0, 5));
  parse_fcn_ptrs.push_back(&ANDSF_Policy::parse_validity_area);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_VALIDITY_AREA_REF, false, true, 0));
  parse_fcn_ptrs.push_back(&ANDSF_Policy::parse_validity_area_ref);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_ROAMING, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_Policy::parse_roaming);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_PLMN, true, false));
  parse_fcn_ptrs.push_back(&ANDSF_Policy::parse_plmn);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_TIME_OF_DAY, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_Policy::parse_time_of_day);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_TIME_OF_DAY_REF, false, true, 0));
  parse_fcn_ptrs.push_back(&ANDSF_Policy::parse_time_of_day_ref);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_UPDATE_POLICY, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_Policy::parse_update_policy);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}



ASXML::ANDSF_Wlansp::ANDSF_Wlansp(const XMLElement* elem, bool& flag)
{
  typedef void (ANDSF_Wlansp::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem, flag, XMLElementCondition(NULL, true, true, 3, 9)); // <X>+

  // Flag will be set if current element is invalid.
  if(flag) return;


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_RULE_PRIORITY, true, false));
  parse_fcn_ptrs.push_back(&ANDSF_Wlansp::parse_rule_priority);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_SELECTION_CRITERIA, true, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_Wlansp::parse_selection_criteria);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_VALIDITY_AREA, false, true, 0, 5));
  parse_fcn_ptrs.push_back(&ANDSF_Wlansp::parse_validity_area);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_VALIDITY_AREA_REF, false, true, 0));
  parse_fcn_ptrs.push_back(&ANDSF_Wlansp::parse_validity_area_ref);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_ROAMING, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_Wlansp::parse_roaming);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_PLMN, true, false));
  parse_fcn_ptrs.push_back(&ANDSF_Wlansp::parse_plmn);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_TIME_OF_DAY, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_Wlansp::parse_time_of_day);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_TIME_OF_DAY_REF, false, true, 0));
  parse_fcn_ptrs.push_back(&ANDSF_Wlansp::parse_time_of_day_ref);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_UPDATE_POLICY, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_Wlansp::parse_update_policy);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}




