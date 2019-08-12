/*==============================================================================

                      ds_xml_ANDSF_FlowDistribution.cpp

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
#include "ds_xml_ANDSF_FlowDistribution.h"

#include "ds_xml_ANDSF_RoutingCriteria.h"
#include "ds_xml_ANDSF_constants.h"
#include "ds_XMLDecoderException.h"
#include "ds_XMLElement.h"
#include "ds_XMLUtility.h"
#include "ds_appsrv_mem.h"

#include <cstdlib> // strtoul

namespace ASXML = Appsrv::XML;


/* ==========================================================================
 *                                IPFlow_AppID
 * ========================================================================== */
ASXML::IPFlow_AppID::IPFlow_AppID(const XMLElement* elem, bool& flag)
{
  typedef void (IPFlow_AppID::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem, flag, XMLElementCondition(NULL, true, true, 2, 2));

  // Flag will be set if current element is invalid.
  if(flag) return;


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_OSID, true, false));
  parse_fcn_ptrs.push_back(&IPFlow_AppID::parse_osid);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_OS_APPS, true, true, 1));
  parse_fcn_ptrs.push_back(&IPFlow_AppID::parse_os_apps);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}



ASXML::IPFlow_AppID::~IPFlow_AppID()
{
  clear_ptr_vector(os_apps);
}


void ASXML::IPFlow_AppID::parse_osid(const XMLElement* elem, bool& flag)
{
  osid = elem->get_text();
}


void ASXML::IPFlow_AppID::parse_os_apps(const XMLElement* elem, bool& flag)
{
  XMLElementCondition filler_condition(NULL, true, true, 1);
  XMLElementCondition os_apps_condition(ELEM_NAME_OS_APP_ID, true, false);

  for(uint32 i = 0; i < elem->children_size(); ++i)
  {
    const XMLElement* child = elem->get_child(i);
    if(!filler_condition.validateElement(child, flag))
      return;

    child = child->get_child(0);
    if(!os_apps_condition.validateElement(child, flag))
      return;

    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<ASString>(), APPSRV_MEM) ASString(child->get_text()),
                                 os_apps,
                                 flag,
                                 ELEM_NAME_OS_APP_ID
                                 ))
    {
      return;
    }
  }
}




/* ==========================================================================
 *                                ANDSF_IPFlow
 * ========================================================================== */
ASXML::ANDSF_IPFlow::ANDSF_IPFlow(const XMLElement* elem, const flow_distribution_type type, bool& flag)
{ }

ASXML::ANDSF_IPFlow::ANDSF_IPFlow(const XMLElement* elem, bool& flag)
{
  typedef void (ANDSF_IPFlow::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem, flag, XMLElementCondition(NULL, true, true, 0, 12));

  // Flag will be set if current element is invalid.
  if(flag) return;


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_APP_ID, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_IPFlow::parse_app_id);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_ADDRESS_TYPE, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_IPFlow::parse_address_type);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_START_SOURCE_IP_ADDRESS, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_IPFlow::parse_start_source_ip_address);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_END_SOURCE_IP_ADDRESS, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_IPFlow::parse_end_source_ip_address);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_PROTOCOL_TYPE, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_IPFlow::parse_protocol_type);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_START_SOURCE_PORT_NUMBER, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_IPFlow::parse_start_source_port_number);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_END_SOURCE_PORT_NUMBER, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_IPFlow::parse_end_source_port_number);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_START_DEST_PORT_NUMBER, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_IPFlow::parse_start_dest_port_number);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_END_DEST_PORT_NUMBER, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_IPFlow::parse_end_dest_port_number);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_QOS, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_IPFlow::parse_qos);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_DOMAIN_NAME, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_IPFlow::parse_domain_name);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_APN, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_IPFlow::parse_apn);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}

ASXML::ANDSF_IPFlow::~ANDSF_IPFlow()
{
  clear_ptr_vector(app_id);
}


void ASXML::ANDSF_IPFlow::parse_app_id(const XMLElement* elem, bool& flag)
{
  for(uint32 i = 0; i < elem->children_size(); i++)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<IPFlow_AppID>(), APPSRV_MEM) IPFlow_AppID(elem->get_child(i), flag),
                                 app_id,
                                 flag,
                                 ELEM_NAME_APP_ID
                                 ))
    {
      return;
    }
  }
}


void ASXML::ANDSF_IPFlow::parse_address_type(const XMLElement* elem, bool& flag)
{
  address_type = elem->get_text();
}


void ASXML::ANDSF_IPFlow::parse_start_source_ip_address(const XMLElement* elem, bool& flag)
{
  start_src_ip = elem->get_text();
}


void ASXML::ANDSF_IPFlow::parse_end_source_ip_address(const XMLElement* elem, bool& flag)
{
  end_src_ip = elem->get_text();
}


void ASXML::ANDSF_IPFlow::parse_protocol_type(const XMLElement* elem, bool& flag)
{
  protocol_type = elem->get_text();
}


void ASXML::ANDSF_IPFlow::parse_start_source_port_number(const XMLElement* elem, bool& flag)
{
  start_src_port = elem->get_text();
}


void ASXML::ANDSF_IPFlow::parse_end_source_port_number(const XMLElement* elem, bool& flag)
{
  end_src_port = elem->get_text();
}


void ASXML::ANDSF_IPFlow::parse_start_dest_port_number(const XMLElement* elem, bool& flag)
{
  start_dest_port = elem->get_text();
}


void ASXML::ANDSF_IPFlow::parse_end_dest_port_number(const XMLElement* elem, bool& flag)
{
  end_dest_port = elem->get_text();
}


void ASXML::ANDSF_IPFlow::parse_qos(const XMLElement* elem, bool& flag)
{
  qos = elem->get_text();
}


void ASXML::ANDSF_IPFlow::parse_domain_name(const XMLElement* elem, bool& flag)
{
  domain_name = elem->get_text();
}


void ASXML::ANDSF_IPFlow::parse_apn(const XMLElement* elem, bool& flag)
{
  apn = elem->get_text();
}




/* ==========================================================================
 *                              ANDSF_RoutingRule
 * ========================================================================== */
ASXML::ANDSF_RoutingRule::ANDSF_RoutingRule(const XMLElement* elem, const flow_distribution_type type, bool& flag)
{
  typedef void (ANDSF_RoutingRule::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem);


  // Add schema rules
  switch(type)
  {
    case FD_PRIORITIZED_ACCESS: // cascade down
    case FD_FLOW_BASED:         // cascade down
    case FD_SERVICE_BASED:
    {
      XMLElementCondition(NULL, true, true, 2, 4).validateElement(elem, flag);

      // Flag will be set if current element is invalid.
      if(flag) return;

      element_validator.addRule(XMLElementCondition(ELEM_NAME_ACCESS_TECHNOLOGY, true, false));
      parse_fcn_ptrs.push_back(&ANDSF_RoutingRule::parse_access_technology);

      element_validator.addRule(XMLElementCondition(ELEM_NAME_ACCESS_ID, false, false));
      parse_fcn_ptrs.push_back(&ANDSF_RoutingRule::parse_access_id);

      element_validator.addRule(XMLElementCondition(ELEM_NAME_SECONDARY_ACCESS_ID, false, false));
      parse_fcn_ptrs.push_back(&ANDSF_RoutingRule::parse_secondary_access_id);

      element_validator.addRule(XMLElementCondition(ELEM_NAME_ACCESS_NETWORK_PRIORITY, true, false));
      parse_fcn_ptrs.push_back(&ANDSF_RoutingRule::parse_access_network_priority);
    }
    break;

    case FD_NON_SEAMLESS_OFFLOAD:
    {
      XMLElementCondition(NULL, true, true, 1, 3).validateElement(elem, flag);

      // Flag will be set if current element is invalid.
      if(flag) return;

      element_validator.addRule(XMLElementCondition(ELEM_NAME_ACCESS_ID, false, false));
      parse_fcn_ptrs.push_back(&ANDSF_RoutingRule::parse_access_id);

      element_validator.addRule(XMLElementCondition(ELEM_NAME_SECONDARY_ACCESS_ID, false, false));
      parse_fcn_ptrs.push_back(&ANDSF_RoutingRule::parse_secondary_access_id);

      element_validator.addRule(XMLElementCondition(ELEM_NAME_ACCESS_NETWORK_PRIORITY, true, false));
      parse_fcn_ptrs.push_back(&ANDSF_RoutingRule::parse_access_network_priority);
    }
    break;

    case FD_APN_ROUTING:
    {
      XMLElementCondition(NULL, true, true, 2, 2).validateElement(elem, flag);

      // Flag will be set if current element is invalid.
      if(flag) return;

      element_validator.addRule(XMLElementCondition(ELEM_NAME_APN, true, false));
      parse_fcn_ptrs.push_back(&ANDSF_RoutingRule::parse_apn);

      element_validator.addRule(XMLElementCondition(ELEM_NAME_APN_PRIORITY, true, false));
      parse_fcn_ptrs.push_back(&ANDSF_RoutingRule::parse_apn_priority);
    }
    break;

    default:
    {
      // Should never reach here.
      XMLDecoderException::get_instance().raise(flag, ERR_MSG_UNEXPECTED);
      return;
    }
    break;
  }


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


void ASXML::ANDSF_RoutingRule::parse_access_technology(const XMLElement* elem, bool& flag)
{
  access_technology = strtoul(elem->get_text().c_str(), NULL, 10);
}


void ASXML::ANDSF_RoutingRule::parse_access_id(const XMLElement* elem, bool& flag)
{
  access_id = elem->get_text();
}


void ASXML::ANDSF_RoutingRule::parse_secondary_access_id(const XMLElement* elem, bool& flag)
{
  secondary_access_id = elem->get_text();
}


void ASXML::ANDSF_RoutingRule::parse_access_network_priority(const XMLElement* elem, bool& flag)
{
  access_network_priority = strtoul(elem->get_text().c_str(), NULL, 10);
}


void ASXML::ANDSF_RoutingRule::parse_apn(const XMLElement* elem, bool& flag)
{
  apn = elem->get_text();
}


void ASXML::ANDSF_RoutingRule::parse_apn_priority(const XMLElement* elem, bool& flag)
{
  apn_priority = strtoul(elem->get_text().c_str(), NULL, 10);
}



/* ==========================================================================
 *                           ANDSF_PrioritizedAccess
 * ========================================================================== */
 ASXML::ANDSF_PrioritizedAccess::ANDSF_PrioritizedAccess(const XMLElement* elem, bool& flag)
 : ANDSF_RoutingRule(elem, FD_PRIORITIZED_ACCESS, flag)
 { }



/* ==========================================================================
 *                            ANDSF_FlowDistribution
 * ========================================================================== */
ASXML::ANDSF_FlowDistribution::ANDSF_FlowDistribution(const XMLElement* elem, const flow_distribution_type type, bool& flag)
: is_api_override(false), flow_type(type)
{
  typedef void (ANDSF_FlowDistribution::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem);

  switch(flow_type)
  {
    case FD_SERVICE_BASED:
    {
      XMLElementCondition(NULL, true, true, 3, 5).validateElement(elem, flag);

      element_validator.addRule(XMLElementCondition(ELEM_NAME_APN, false, false));
      parse_fcn_ptrs.push_back(&ANDSF_FlowDistribution::parse_apn);
    }
    break;

    case FD_FLOW_BASED:           // cascade down
    case FD_APN_ROUTING:          // cascade down
    case FD_NON_SEAMLESS_OFFLOAD:
    {
      XMLElementCondition(NULL, true, true, 4, 5).validateElement(elem, flag);

      // Add schema rules
      element_validator.addRule(XMLElementCondition(ELEM_NAME_IP_FLOW, true, true, 1));
      parse_fcn_ptrs.push_back(&ANDSF_FlowDistribution::parse_ip_flow);
    }
    break;

    default:
    {
      XMLDecoderException::get_instance().raise(flag, ERR_MSG_UNEXPECTED);
      return;
    }
    break;
  }

  // Flag will be set if current element is invalid.
  if(flag) return;

  element_validator.addRule(XMLElementCondition(ELEM_NAME_ROUTING_CRITERIA, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_FlowDistribution::parse_routing_criteria);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_ROUTING_RULE, true, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_FlowDistribution::parse_routing_rule);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_API_OVERRIDE, false, true));
  parse_fcn_ptrs.push_back(&ANDSF_FlowDistribution::parse_api_override);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_RULE_PRIORITY, true, false));
  parse_fcn_ptrs.push_back(&ANDSF_FlowDistribution::parse_rule_priority);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}



ASXML::ANDSF_FlowDistribution::~ANDSF_FlowDistribution()
{
  clear_ptr_vector(ip_flow);
  clear_ptr_vector(routing_criteria);
  clear_ptr_vector(routing_rule);
}


void ASXML::ANDSF_FlowDistribution::parse_apn(const XMLElement* elem, bool& flag)
{
  apn = elem->get_text();
}


void ASXML::ANDSF_FlowDistribution::parse_routing_rule(const XMLElement* elem, bool& flag)
{
  for(uint32 i = 0; i < elem->children_size(); i++)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<ANDSF_RoutingRule>(), APPSRV_MEM) ANDSF_RoutingRule(elem->get_child(i), flow_type, flag),
                                 routing_rule,
                                 flag,
                                 ELEM_NAME_ROUTING_RULE
                                 ))
    {
      return;
    }
  }
}


void ASXML::ANDSF_FlowDistribution::parse_ip_flow(const XMLElement* elem, bool& flag)
{
  for(uint32 i = 0; i < elem->children_size(); i++)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<ANDSF_IPFlow>(), APPSRV_MEM) ANDSF_IPFlow(elem->get_child(i), flag),
                                 ip_flow,
                                 flag,
                                 ELEM_NAME_IP_FLOW
                                 ))
    {
      return;
    }
  }
}


void ASXML::ANDSF_FlowDistribution::parse_routing_criteria(const XMLElement* elem, bool& flag)
{
  for(uint32 i = 0; i < elem->children_size(); i++)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<ANDSF_RoutingCriteria>(), APPSRV_MEM) ANDSF_RoutingCriteria(elem->get_child(i), flag),
                                 routing_criteria,
                                 flag,
                                 ELEM_NAME_ROUTING_CRITERIA
                                 ))
    {
      return;
    }
  }
}


void ASXML::ANDSF_FlowDistribution::parse_api_override(const XMLElement* elem, bool& flag)
{
  is_api_override = str_to_bool(elem->get_text());
}


void ASXML::ANDSF_FlowDistribution::parse_rule_priority(const XMLElement* elem, bool& flag)
{
  rule_priority = strtoul(elem->get_text().c_str(), NULL, 10);
}




/* ==========================================================================
 *                            ANDSF_FlowDistribution
 * ========================================================================== */
ASXML::ANDSF_ForCSBased::ANDSF_ForCSBased(const XMLElement* elem, bool& flag)
: evaluation_policy(false), cs_1x_available(false), rule_priority(0)
{
  typedef void (ANDSF_ForCSBased::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem, flag, XMLElementCondition(NULL, true, true, 2, 3));

  // Flag will be set if current element is invalid.
  if(flag) return;


  element_validator.addRule(XMLElementCondition(ELEM_NAME_CDMA_1X_CONDITION, false, true, 0, 1));
  parse_fcn_ptrs.push_back(&ANDSF_ForCSBased::parse_1x_condition);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_IS_1X_CS_AVAILABLE, true, false));
  parse_fcn_ptrs.push_back(&ANDSF_ForCSBased::parse_cs_1x_available);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_RULE_PRIORITY, true, false));
  parse_fcn_ptrs.push_back(&ANDSF_ForCSBased::parse_rule_priority);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


void ASXML::ANDSF_ForCSBased::parse_1x_condition(const XMLElement* elem, bool& flag)
{
  if(0 == elem->children_size())
    return;

  typedef void (ANDSF_ForCSBased::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem->get_child(0), flag, XMLElementCondition(ELEM_NAME_THRESHOLD_COND, true, true, 0, 4));

  // Flag will be set if current element is invalid.
  if(flag) return;


  element_validator.addRule(XMLElementCondition(ELEM_NAME_CDMA_1X_ECIO_LOW, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_ForCSBased::parse_thresh_1x_ecio_low);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_CDMA_1X_ECIO_HIGH, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_ForCSBased::parse_thresh_1x_ecio_high);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_CDMA_1X_AVAILABLE, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_ForCSBased::parse_thresh_1x_avail);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_CONDITION_EVAL_POLICY, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_ForCSBased::parse_thresh_evaluation_policy);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


void ASXML::ANDSF_ForCSBased::parse_cs_1x_available(const XMLElement* elem, bool& flag)
{
  cs_1x_available = str_to_bool(elem->get_text());
}


void ASXML::ANDSF_ForCSBased::parse_rule_priority(const XMLElement* elem, bool& flag)
{
  rule_priority = strtoul(elem->get_text().c_str(), NULL, 10);
}


void ASXML::ANDSF_ForCSBased::parse_thresh_1x_ecio_low(const XMLElement* elem, bool& flag)
{
  thresh_cdma_1x_ecio_low = strtol(elem->get_text().c_str(), NULL, 10);
}


void ASXML::ANDSF_ForCSBased::parse_thresh_1x_ecio_high(const XMLElement* elem, bool& flag)
{
  thresh_cdma_1x_ecio_high = strtol(elem->get_text().c_str(), NULL, 10);
}


void ASXML::ANDSF_ForCSBased::parse_thresh_1x_avail(const XMLElement* elem, bool& flag)
{
  thresh_cdma_1x_avail = str_to_bool(elem->get_text());
}


void ASXML::ANDSF_ForCSBased::parse_thresh_evaluation_policy(const XMLElement* elem, bool& flag)
{
  evaluation_policy = str_to_bool(elem->get_text());
}




/* ==========================================================================
 *                              ANDSF_Isrp_Iarp_base
 * ========================================================================== */
ASXML::ANDSF_Isrp_Iarp_base::ANDSF_Isrp_Iarp_base(const XMLElement* elem, const bool is_isrp, bool& flag)
: roaming(false), update_policy(false)
{
  typedef void (ANDSF_Isrp_Iarp_base::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem);

  // Flag will be set if current element is invalid.
  if(flag) return;


  // Add schema rules
  if(is_isrp)
  {
    XMLElementCondition(NULL, true, true, 1, 7).validateElement(elem, flag);

    element_validator.addRule(XMLElementCondition(ELEM_NAME_FLOW_BASED, false, true, 1));
    parse_fcn_ptrs.push_back(&ANDSF_Isrp_Iarp_base::parse_flow_based);

    element_validator.addRule(XMLElementCondition(ELEM_NAME_SERVICE_BASED, false, true, 1));
    parse_fcn_ptrs.push_back(&ANDSF_Isrp_Iarp_base::parse_service_based);
  }
  else
  {
    XMLElementCondition(NULL, true, true, 1, 5).validateElement(elem, flag);

    element_validator.addRule(XMLElementCondition(ELEM_NAME_INTER_APN_ROUTING, false, true, 1));
    parse_fcn_ptrs.push_back(&ANDSF_Isrp_Iarp_base::parse_inter_apn_routing);
  }

  element_validator.addRule(XMLElementCondition(ELEM_NAME_NON_SEAMLESS_OFFLOAD, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_Isrp_Iarp_base::parse_non_seamless_offload);

  if(is_isrp)
  {
    element_validator.addRule(XMLElementCondition(ELEM_NAME_FOR_CS_BASED, false, true, 1));
    parse_fcn_ptrs.push_back(&ANDSF_Isrp_Iarp_base::parse_cs_based);
  }

  element_validator.addRule(XMLElementCondition(ELEM_NAME_ROAMING, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_Isrp_Iarp_base::parse_roaming);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_PLMN, true, false));
  parse_fcn_ptrs.push_back(&ANDSF_Isrp_Iarp_base::parse_plmn);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_UPDATE_POLICY, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_Isrp_Iarp_base::parse_update_policy);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}



ASXML::ANDSF_Isrp_Iarp_base::~ANDSF_Isrp_Iarp_base()
{
  clear_ptr_vector(for_flow_based);
  clear_ptr_vector(for_service_based);
  clear_ptr_vector(for_inter_apn_routing);
  clear_ptr_vector(for_non_seamless_offload);
  clear_ptr_vector(for_cs_based);
}


void ASXML::ANDSF_Isrp_Iarp_base::parse_flow_distribution(
                                                    const XMLElement*                  elem,
                                                    bool&                              flag,
                                                    ASVector<ANDSF_FlowDistribution*>& fd_vector,
                                                    const flow_distribution_type       flow_type,
                                                    const char*                        name
                                                    )
{
  for(uint32 i = 0; i < elem->children_size(); i++)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<ANDSF_FlowDistribution>(), APPSRV_MEM) ANDSF_FlowDistribution(elem->get_child(i), flow_type, flag),
                                 fd_vector,
                                 flag,
                                 name
                                 ))
    {
      return;
    }
  }
}


void ASXML::ANDSF_Isrp_Iarp_base::parse_flow_based(const XMLElement* elem, bool& flag)
{
  parse_flow_distribution(
                          elem,
                          flag,
                          for_flow_based,
                          FD_FLOW_BASED,
                          ELEM_NAME_FLOW_BASED
                          );
}


void ASXML::ANDSF_Isrp_Iarp_base::parse_service_based(const XMLElement* elem, bool& flag)
{
  parse_flow_distribution(
                          elem,
                          flag,
                          for_service_based,
                          FD_SERVICE_BASED,
                          ELEM_NAME_SERVICE_BASED
                          );
}


void ASXML::ANDSF_Isrp_Iarp_base::parse_inter_apn_routing(const XMLElement* elem, bool& flag)
{
  parse_flow_distribution(
                          elem,
                          flag,
                          for_inter_apn_routing,
                          FD_APN_ROUTING,
                          ELEM_NAME_INTER_APN_ROUTING
                          );
}


void ASXML::ANDSF_Isrp_Iarp_base::parse_non_seamless_offload(const XMLElement* elem, bool& flag)
{
  parse_flow_distribution(
                          elem,
                          flag,
                          for_non_seamless_offload,
                          FD_NON_SEAMLESS_OFFLOAD,
                          ELEM_NAME_NON_SEAMLESS_OFFLOAD
                          );
}


void ASXML::ANDSF_Isrp_Iarp_base::parse_cs_based(const XMLElement* elem, bool& flag)
{
  for(uint32 i = 0; i < elem->children_size(); i++)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<ANDSF_ForCSBased>(), APPSRV_MEM) ANDSF_ForCSBased(elem->get_child(i), flag),
                                 for_cs_based,
                                 flag,
                                 ELEM_NAME_FOR_CS_BASED
                                 ))
    {
      return;
    }
  }
}


void ASXML::ANDSF_Isrp_Iarp_base::parse_roaming(const XMLElement* elem, bool& flag)
{
  roaming = str_to_bool(elem->get_text());
}


void ASXML::ANDSF_Isrp_Iarp_base::parse_plmn(const XMLElement* elem, bool& flag)
{
  plmn = elem->get_text();
}


void ASXML::ANDSF_Isrp_Iarp_base::parse_update_policy(const XMLElement* elem, bool& flag)
{
  update_policy = str_to_bool(elem->get_text());
}




ASXML::ANDSF_Isrp::ANDSF_Isrp(const XMLElement* elem, bool& flag)
: ANDSF_Isrp_Iarp_base(elem, true, flag)
{ }



ASXML::ANDSF_Iarp::ANDSF_Iarp(const XMLElement* elem, bool& flag)
: ANDSF_Isrp_Iarp_base(elem, false, flag)
{ }

