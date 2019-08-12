/*==============================================================================

                          ds_xml_ANDSF_RANValidity.h

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
03/02/15    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_XML_ANDSF_RAN_VALIDITY_H
#define DS_XML_ANDSF_RAN_VALIDITY_H

#include "ds_ASAssignType.h"
#include "ds_ASAutoPointer.h"


namespace Appsrv
{
namespace XML
{

class XMLElement;



/* ==========================================================================
 *                            ANDSF_WLANThreshold
 * ========================================================================== */
class ANDSF_WLANThreshold
{
public:
  void parse(const XMLElement* elem, bool& flag, const char* thresh_name);

private:
  void parse_andsf(const XMLElement* elem, bool& flag);
  void parse_ran(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  Utils::ASAssignType<int32> andsf;
  Utils::ASAssignType<int32> ran;
};



/* ==========================================================================
 *                        ANDSF_RANThresholdCondition
 * ========================================================================== */
class ANDSF_RANThresholdCondition
{
public:
  ANDSF_RANThresholdCondition(const XMLElement* elem, bool& flag);

private:
  void parse_threshold(const XMLElement* elem, bool& flag);
  void parse_eval_policy(const XMLElement* elem, bool& flag);

  void parse_thresh_wlan_low_p(const XMLElement* elem, bool& flag);
  void parse_thresh_wlan_high_p(const XMLElement* elem, bool& flag);
  void parse_thresh_wlan_low_q(const XMLElement* elem, bool& flag);
  void parse_thresh_wlan_high_q(const XMLElement* elem, bool& flag);

  void parse_thresh_ch_wlan_low(const XMLElement* elem, bool& flag);
  void parse_thresh_ch_wlan_high(const XMLElement* elem, bool& flag);

  void parse_thresh_ul_wlan_low(const XMLElement* elem, bool& flag);
  void parse_thresh_ul_wlan_high(const XMLElement* elem, bool& flag);

  void parse_thresh_dl_wlan_low(const XMLElement* elem, bool& flag);
  void parse_thresh_dl_wlan_high(const XMLElement* elem, bool& flag);

  void parse_thresh_rssi_wlan_low(const XMLElement* elem, bool& flag);
  void parse_thresh_rssi_wlan_high(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  bool evaluation_policy; // 0 - match 1, 1 - match all, default 0

  Utils::ASAssignType<int32> thresh_wlan_low_p;
  Utils::ASAssignType<int32> thresh_wlan_high_p;
  Utils::ASAssignType<int32> thresh_wlan_low_q;
  Utils::ASAssignType<int32> thresh_wlan_high_q;

  ANDSF_WLANThreshold thresh_ch_wlan_low;
  ANDSF_WLANThreshold thresh_ch_wlan_high;

  ANDSF_WLANThreshold thresh_ul_wlan_low;
  ANDSF_WLANThreshold thresh_ul_wlan_high;

  ANDSF_WLANThreshold thresh_dl_wlan_low;
  ANDSF_WLANThreshold thresh_dl_wlan_high;

  ANDSF_WLANThreshold thresh_rssi_wlan_low;
  ANDSF_WLANThreshold thresh_rssi_wlan_high;
};



/* ==========================================================================
 *                            ANDSF_RANValidity
 * ========================================================================== */
class ANDSF_RANValidity
{
public:
  ANDSF_RANValidity(const XMLElement* elem, bool& flag);

private:
  void parse_threshold_cond(const XMLElement* elem, bool& flag);
  void parse_opi(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  ASAutoPointer<ANDSF_RANThresholdCondition> threshold_condition;
  uint16                                     opi;
};



} /* namespace XML */
} /* namespace Appsrv */

#endif /* DS_XML_ANDSF_RAN_VALIDITY_H */
