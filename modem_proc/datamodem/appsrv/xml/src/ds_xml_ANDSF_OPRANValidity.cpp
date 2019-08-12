/*==============================================================================

                      ds_xml_ANDSF_OPRANValidity.cpp

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
03/02/15    ml     Created file/Initial version.
==============================================================================*/
#include "ds_xml_ANDSF_OPRANValidity.h"

#include "ds_xml_ANDSF_constants.h"
#include "ds_XMLDecoderException.h"
#include "ds_XMLElement.h"
#include "ds_XMLSchemaValidator.h"
#include "ds_XMLUtility.h"
#include "ds_appsrv_mem.h"
#include <cstdlib> // strtol strtoul

namespace ASXML = Appsrv::XML;


/* ==========================================================================
 *                            ANDSF_OPLTEThreshold
 * ========================================================================== */
ASXML::ANDSF_OPLTEThreshold::ANDSF_OPLTEThreshold(const XMLElement* elem, bool& flag)
{
  typedef void (ANDSF_OPLTEThreshold::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem);


  // Add schema rules for current element
  element_validator.addRule(XMLElementCondition(ELEM_NAME_LTE_RSRP_LOW, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_OPLTEThreshold::parse_rsrp_low);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_LTE_RSRP_HIGH, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_OPLTEThreshold::parse_rsrp_high);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_LTE_RSRQ_LOW, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_OPLTEThreshold::parse_rsrq_low);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_LTE_RSRQ_HIGH, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_OPLTEThreshold::parse_rsrq_high);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_LTE_AVAILABLE, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_OPLTEThreshold::parse_available);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_CONDITION_EVAL_POLICY, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_OPLTEThreshold::parse_eval_policy);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


void ASXML::ANDSF_OPLTEThreshold::parse_rsrp_low(const XMLElement* elem, bool& flag)
{
  rsrp_low = strtol(elem->get_text().c_str(), NULL, 10);
}


void ASXML::ANDSF_OPLTEThreshold::parse_rsrp_high(const XMLElement* elem, bool& flag)
{
  rsrp_high = strtol(elem->get_text().c_str(), NULL, 10);
}


void ASXML::ANDSF_OPLTEThreshold::parse_rsrq_low(const XMLElement* elem, bool& flag)
{
  rsrq_low = strtol(elem->get_text().c_str(), NULL, 10);
}


void ASXML::ANDSF_OPLTEThreshold::parse_rsrq_high(const XMLElement* elem, bool& flag)
{
  rsrq_high = strtol(elem->get_text().c_str(), NULL, 10);
}


void ASXML::ANDSF_OPLTEThreshold::parse_available(const XMLElement* elem, bool& flag)
{
  available = str_to_bool(elem->get_text());
}


void ASXML::ANDSF_OPLTEThreshold::parse_eval_policy(const XMLElement* elem, bool& flag)
{
  evaluation_policy = str_to_bool(elem->get_text());
}




/* ==========================================================================
 *                            ANDSF_OPWiFiThreshold
 * ========================================================================== */
ASXML::ANDSF_OPWiFiThreshold::ANDSF_OPWiFiThreshold(const XMLElement* elem, bool& flag)
{
  typedef void (ANDSF_OPWiFiThreshold::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem);

  // Add schema rules for current element
  element_validator.addRule(XMLElementCondition(ELEM_NAME_WIFI_RSSI_LOW, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_OPWiFiThreshold::parse_rssi_low);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_WIFI_RSSI_HIGH, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_OPWiFiThreshold::parse_rssi_high);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_WIFI_SINR_LOW, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_OPWiFiThreshold::parse_sinr_low);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_WIFI_SINR_HIGH, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_OPWiFiThreshold::parse_sinr_high);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_WIFI_BSS_LOW, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_OPWiFiThreshold::parse_bss_load_low);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_WIFI_BSS_HIGH, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_OPWiFiThreshold::parse_bss_load_high);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_WIFI_AVAILABLE, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_OPWiFiThreshold::parse_available);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_CONDITION_EVAL_POLICY, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_OPWiFiThreshold::parse_eval_policy);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


void ASXML::ANDSF_OPWiFiThreshold::parse_rssi_low(const XMLElement* elem, bool& flag)
{
  rssi_low = strtol(elem->get_text().c_str(), NULL, 10);
}


void ASXML::ANDSF_OPWiFiThreshold::parse_rssi_high(const XMLElement* elem, bool& flag)
{
  rssi_high = strtol(elem->get_text().c_str(), NULL, 10);
}


void ASXML::ANDSF_OPWiFiThreshold::parse_sinr_low(const XMLElement* elem, bool& flag)
{
  sinr_low = strtol(elem->get_text().c_str(), NULL, 10);
}


void ASXML::ANDSF_OPWiFiThreshold::parse_sinr_high(const XMLElement* elem, bool& flag)
{
  sinr_high = strtol(elem->get_text().c_str(), NULL, 10);
}


void ASXML::ANDSF_OPWiFiThreshold::parse_bss_load_low(const XMLElement* elem, bool& flag)
{
  bss_load_low = strtoul(elem->get_text().c_str(), NULL, 10);
}


void ASXML::ANDSF_OPWiFiThreshold::parse_bss_load_high(const XMLElement* elem, bool& flag)
{
  bss_load_high = strtoul(elem->get_text().c_str(), NULL, 10);
}


void ASXML::ANDSF_OPWiFiThreshold::parse_available(const XMLElement* elem, bool& flag)
{
  available = str_to_bool(elem->get_text());
}


void ASXML::ANDSF_OPWiFiThreshold::parse_eval_policy(const XMLElement* elem, bool& flag)
{
  evaluation_policy = str_to_bool(elem->get_text());
}




/* ==========================================================================
 *                          ANDSF_OPeHRPDThreshold
 * ========================================================================== */
ASXML::ANDSF_OPeHRPDThreshold::ANDSF_OPeHRPDThreshold(const XMLElement* elem, bool& flag)
{
  typedef void (ANDSF_OPeHRPDThreshold::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem);

  // Add schema rules for current element
  element_validator.addRule(XMLElementCondition(ELEM_NAME_EHRPD_AVAILABLE, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_OPeHRPDThreshold::parse_available);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


void ASXML::ANDSF_OPeHRPDThreshold::parse_available(const XMLElement* elem, bool& flag)
{
  available = str_to_bool(elem->get_text());
}




/* ==========================================================================
 *                          ANDSF_OPThresholdConditions
 * ========================================================================== */
ASXML::ANDSF_OPThresholdConditions::ANDSF_OPThresholdConditions(const XMLElement* elem, bool& flag)
: evaluation_policy(false)
{
  typedef void (ANDSF_OPThresholdConditions::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem);

  // Add schema rules for current element
  element_validator.addRule(XMLElementCondition(ELEM_NAME_LTE_THRESHOLD, false, true, 0, 6));
  parse_fcn_ptrs.push_back(&ANDSF_OPThresholdConditions::parse_lte_threshold);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_WIFI_THRESHOLD, false, true, 0, 8));
  parse_fcn_ptrs.push_back(&ANDSF_OPThresholdConditions::parse_wifi_threshold);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_EHRPD_THRESHOLD, false, true, 0, 1));
  parse_fcn_ptrs.push_back(&ANDSF_OPThresholdConditions::parse_ehrpd_threshold);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_CONDITION_EVAL_POLICY, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_OPThresholdConditions::parse_eval_policy);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


void ASXML::ANDSF_OPThresholdConditions::parse_lte_threshold(const XMLElement* elem, bool& flag)
{
  lte_threshold = new(ds_appsrv_alloc<ANDSF_OPLTEThreshold>(), APPSRV_MEM) ANDSF_OPLTEThreshold(elem, flag);
  if(lte_threshold.is_null())
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC, ELEM_NAME_LTE_THRESHOLD);
}


void ASXML::ANDSF_OPThresholdConditions::parse_wifi_threshold(const XMLElement* elem, bool& flag)
{
  wifi_threshold = new(ds_appsrv_alloc<ANDSF_OPWiFiThreshold>(), APPSRV_MEM) ANDSF_OPWiFiThreshold(elem, flag);
  if(wifi_threshold.is_null())
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC, ELEM_NAME_WIFI_THRESHOLD);
}


void ASXML::ANDSF_OPThresholdConditions::parse_ehrpd_threshold(const XMLElement* elem, bool& flag)
{
  ehrpd_threshold = new(ds_appsrv_alloc<ANDSF_OPeHRPDThreshold>(), APPSRV_MEM) ANDSF_OPeHRPDThreshold(elem, flag);
  if(ehrpd_threshold.is_null())
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC, ELEM_NAME_EHRPD_THRESHOLD);
}


void ASXML::ANDSF_OPThresholdConditions::parse_eval_policy(const XMLElement* elem, bool& flag)
{
  evaluation_policy = str_to_bool(elem->get_text());
}




/* ==========================================================================
 *                            ANDSF_OPRANValidity
 * ========================================================================== */
ASXML::ANDSF_OPRANValidity::ANDSF_OPRANValidity(const XMLElement* elem, bool& flag)
{
  typedef void (ANDSF_OPRANValidity::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem);

  // Add schema rules for current element
  element_validator.addRule(XMLElementCondition(ELEM_NAME_THRESHOLD_COND, false, true, 0, 4));
  parse_fcn_ptrs.push_back(&ANDSF_OPRANValidity::parse_threshold_conditions);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_CALL_CONDITION, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_OPRANValidity::parse_call_conditions);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


void ASXML::ANDSF_OPRANValidity::parse_threshold_conditions(const XMLElement* elem, bool& flag)
{
  threshold_condition
    = new(ds_appsrv_alloc<ANDSF_OPThresholdConditions>(), APPSRV_MEM) ANDSF_OPThresholdConditions(elem, flag);

  if(threshold_condition.is_null())
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC, ELEM_NAME_THRESHOLD_COND);
}



void ASXML::ANDSF_OPRANValidity::parse_call_conditions(const XMLElement* elem, bool& flag)
{
  uint32              num_conditions = elem->children_size();
  XMLElementCondition filler_condition(NULL, true, true, 1, 1);
  XMLElementCondition calltype_condition(ELEM_NAME_CALLTYPE, true, false);


  for(uint32 i = 0; i < num_conditions; ++i)
  {
    const XMLElement* child = elem->get_child(i);
    filler_condition.validateElement(child, flag);
    if(flag) return;

    child = child->get_child(0);
    calltype_condition.validateElement(child, flag);
    if(flag) return;

    calltypes.push_back(strtoul(child->get_text().c_str(), NULL, 16));
  }
}

