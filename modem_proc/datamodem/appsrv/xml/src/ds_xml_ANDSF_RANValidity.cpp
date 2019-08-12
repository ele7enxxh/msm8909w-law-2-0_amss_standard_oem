/*==============================================================================

                      ds_xml_ANDSF_RANValidity.cpp

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
#include "ds_xml_ANDSF_RANValidity.h"

#include "ds_xml_ANDSF_constants.h"
#include "ds_XMLDecoderException.h"
#include "ds_XMLElement.h"
#include "ds_XMLSchemaValidator.h"
#include "ds_XMLUtility.h"
#include "ds_appsrv_mem.h"
#include <cstdlib> // strtol strtoul

namespace ASXML = Appsrv::XML;



/* ==========================================================================
 *                            ANDSF_WLANThreshold
 * ========================================================================== */
void ASXML::ANDSF_WLANThreshold::parse(const XMLElement* elem, bool& flag, const char* thresh_name)
{
  typedef void (ANDSF_WLANThreshold::*elem_parser)(const XMLElement*, bool&);

  ASString              thresh_name_andsf(thresh_name);
  ASString              thresh_name_ran(thresh_name);
  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem);

  thresh_name_andsf.append("ANDSF");
  thresh_name_ran.append("RAN");

  // Add schema rules
  element_validator.addRule(XMLElementCondition(thresh_name_andsf.c_str(), false, false));
  parse_fcn_ptrs.push_back(&ANDSF_WLANThreshold::parse_andsf);

  element_validator.addRule(XMLElementCondition(thresh_name_ran.c_str(), false, false));
  parse_fcn_ptrs.push_back(&ANDSF_WLANThreshold::parse_ran);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


void ASXML::ANDSF_WLANThreshold::parse_andsf(const XMLElement* elem, bool& flag)
{
  andsf.set(strtol(elem->get_text().c_str(), NULL, 10));
}


void ASXML::ANDSF_WLANThreshold::parse_ran(const XMLElement* elem, bool& flag)
{
  ran.set(strtol(elem->get_text().c_str(), NULL, 10));
}







/* ==========================================================================
 *                        ANDSF_RANThresholdCondition
 * ========================================================================== */
ASXML::ANDSF_RANThresholdCondition::ANDSF_RANThresholdCondition(const XMLElement* elem, bool& flag)
: evaluation_policy(false)
{
  typedef void (ANDSF_RANThresholdCondition::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem);


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_THRESHOLD, false, true, 0, 12));
  parse_fcn_ptrs.push_back(&ANDSF_RANThresholdCondition::parse_threshold);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_THRESHOLD_COND_EVAL, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_RANThresholdCondition::parse_eval_policy);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


void ASXML::ANDSF_RANThresholdCondition::parse_threshold(const XMLElement* elem, bool& flag)
{
  typedef void (ANDSF_RANThresholdCondition::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem);

  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_THRESH_WLAN_LOW_P, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_RANThresholdCondition::parse_thresh_wlan_low_p);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_THRESH_WLAN_HIGH_P, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_RANThresholdCondition::parse_thresh_wlan_high_p);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_THRESH_WLAN_LOW_Q, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_RANThresholdCondition::parse_thresh_wlan_low_q);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_THRESH_WLAN_HIGH_Q, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_RANThresholdCondition::parse_thresh_wlan_high_q);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_THRESH_CH_WLAN_LOW, false, false, 0, 2));
  parse_fcn_ptrs.push_back(&ANDSF_RANThresholdCondition::parse_thresh_ch_wlan_low);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_THRESH_CH_WLAN_HIGH, false, false, 0, 2));
  parse_fcn_ptrs.push_back(&ANDSF_RANThresholdCondition::parse_thresh_ch_wlan_high);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_THRESH_DL_WLAN_LOW, false, false, 0, 2));
  parse_fcn_ptrs.push_back(&ANDSF_RANThresholdCondition::parse_thresh_ul_wlan_low);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_THRESH_DL_WLAN_HIGH, false, false, 0, 2));
  parse_fcn_ptrs.push_back(&ANDSF_RANThresholdCondition::parse_thresh_ul_wlan_high);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_THRESH_UL_WLAN_LOW, false, false, 0, 2));
  parse_fcn_ptrs.push_back(&ANDSF_RANThresholdCondition::parse_thresh_dl_wlan_low);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_THRESH_UL_WLAN_HIGH, false, false, 0, 2));
  parse_fcn_ptrs.push_back(&ANDSF_RANThresholdCondition::parse_thresh_dl_wlan_high);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}

void ASXML::ANDSF_RANThresholdCondition::parse_eval_policy(const XMLElement* elem, bool& flag)
{
  evaluation_policy = str_to_bool(elem->get_text());
}


void ASXML::ANDSF_RANThresholdCondition::parse_thresh_wlan_low_p(const XMLElement* elem, bool& flag)
{
  thresh_wlan_low_p.set(strtol(elem->get_text().c_str(), NULL, 10));
}


void ASXML::ANDSF_RANThresholdCondition::parse_thresh_wlan_high_p(const XMLElement* elem, bool& flag)
{
  thresh_wlan_high_p.set(strtol(elem->get_text().c_str(), NULL, 10));
}


void ASXML::ANDSF_RANThresholdCondition::parse_thresh_wlan_low_q(const XMLElement* elem, bool& flag)
{
  thresh_wlan_low_q.set(strtol(elem->get_text().c_str(), NULL, 10));
}


void ASXML::ANDSF_RANThresholdCondition::parse_thresh_wlan_high_q(const XMLElement* elem, bool& flag)
{
  thresh_wlan_high_q.set(strtol(elem->get_text().c_str(), NULL, 10));
}


void ASXML::ANDSF_RANThresholdCondition::parse_thresh_ch_wlan_low(const XMLElement* elem, bool& flag)
{
  thresh_ch_wlan_low.parse(elem, flag, ELEM_NAME_THRESH_CH_WLAN_LOW);
}


void ASXML::ANDSF_RANThresholdCondition::parse_thresh_ch_wlan_high(const XMLElement* elem, bool& flag)
{
  thresh_ch_wlan_high.parse(elem, flag, ELEM_NAME_THRESH_CH_WLAN_HIGH);
}


void ASXML::ANDSF_RANThresholdCondition::parse_thresh_ul_wlan_low(const XMLElement* elem, bool& flag)
{
  thresh_ul_wlan_low.parse(elem, flag, ELEM_NAME_THRESH_DL_WLAN_LOW);
}


void ASXML::ANDSF_RANThresholdCondition::parse_thresh_ul_wlan_high(const XMLElement* elem, bool& flag)
{
  thresh_ul_wlan_high.parse(elem, flag, ELEM_NAME_THRESH_DL_WLAN_HIGH);
}


void ASXML::ANDSF_RANThresholdCondition::parse_thresh_dl_wlan_low(const XMLElement* elem, bool& flag)
{
  thresh_dl_wlan_low.parse(elem, flag, ELEM_NAME_THRESH_UL_WLAN_LOW);
}


void ASXML::ANDSF_RANThresholdCondition::parse_thresh_dl_wlan_high(const XMLElement* elem, bool& flag)
{
  thresh_dl_wlan_high.parse(elem, flag, ELEM_NAME_THRESH_UL_WLAN_HIGH);
}


void ASXML::ANDSF_RANThresholdCondition::parse_thresh_rssi_wlan_low(const XMLElement* elem, bool& flag)
{
  thresh_rssi_wlan_low.parse(elem, flag, ELEM_NAME_THRESH_RSSI_WLAN_LOW);
}


void ASXML::ANDSF_RANThresholdCondition::parse_thresh_rssi_wlan_high(const XMLElement* elem, bool& flag)
{
  thresh_rssi_wlan_high.parse(elem, flag, ELEM_NAME_THRESH_RSSI_WLAN_HIGH);
}




/* ==========================================================================
 *                            ANDSF_RANValidity
 * ========================================================================== */
ASXML::ANDSF_RANValidity::ANDSF_RANValidity(const XMLElement* elem, bool& flag)
{
  typedef void (ANDSF_RANValidity::*elem_parser)(const XMLElement*, bool&);
  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem);


  // Add schema rules for current tree
  element_validator.addRule(XMLElementCondition(ELEM_NAME_THRESHOLD_COND, false, true, 1, 2));
  parse_fcn_ptrs.push_back(&ANDSF_RANValidity::parse_threshold_cond);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_OPI, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_RANValidity::parse_opi);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


void ASXML::ANDSF_RANValidity::parse_threshold_cond(const XMLElement* elem, bool& flag)
{
  threshold_condition
    = new(ds_appsrv_alloc<ANDSF_RANThresholdCondition>(), APPSRV_MEM) ANDSF_RANThresholdCondition(elem, flag);

  if(threshold_condition.is_null())
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC, ELEM_NAME_THRESHOLD_COND);
}


void ASXML::ANDSF_RANValidity::parse_opi(const XMLElement* elem, bool& flag)
{
  opi = strtol(elem->get_text().c_str(), NULL, 10);
}



