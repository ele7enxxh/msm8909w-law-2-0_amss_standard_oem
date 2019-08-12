/*==============================================================================

                          ds_xml_ANDSF_OPRANValidity.h

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
#ifndef DS_XML_ANDSF_OP_RAN_VALIDITY_H
#define DS_XML_ANDSF_OP_RAN_VALIDITY_H

#include "ds_ASAssignType.h"
#include "ds_ASAutoPointer.h"
#include "ds_ASVector.h"

#include "comdef.h"


namespace Appsrv
{
namespace XML
{

class XMLElement;



/* ==========================================================================
 *                            ANDSF_OPLTEThreshold
 * ========================================================================== */
class ANDSF_OPLTEThreshold
{
public:
  ANDSF_OPLTEThreshold(const XMLElement* rc_elem, bool& flag);

private:
  void parse_rsrp_low(const XMLElement* elem, bool& flag);
  void parse_rsrp_high(const XMLElement* elem, bool& flag);
  void parse_rsrq_low(const XMLElement* elem, bool& flag);
  void parse_rsrq_high(const XMLElement* elem, bool& flag);

  void parse_available(const XMLElement* elem, bool& flag);
  void parse_eval_policy(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  Utils::ASAssignType<int32> rsrp_low;
  Utils::ASAssignType<int32> rsrp_high;
  Utils::ASAssignType<int32> rsrq_low;
  Utils::ASAssignType<int32> rsrq_high;
  Utils::ASAssignType<bool>  available;
  bool                       evaluation_policy;
};



/* ==========================================================================
 *                            ANDSF_OPWiFiThreshold
 * ========================================================================== */
class ANDSF_OPWiFiThreshold
{
public:
  ANDSF_OPWiFiThreshold(const XMLElement* rc_elem, bool& flag);

private:
  void parse_rssi_low(const XMLElement* elem, bool& flag);
  void parse_rssi_high(const XMLElement* elem, bool& flag);
  void parse_sinr_low(const XMLElement* elem, bool& flag);
  void parse_sinr_high(const XMLElement* elem, bool& flag);
  void parse_bss_load_low(const XMLElement* elem, bool& flag);
  void parse_bss_load_high(const XMLElement* elem, bool& flag);

  void parse_available(const XMLElement* elem, bool& flag);
  void parse_eval_policy(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  Utils::ASAssignType<int32>  rssi_low;
  Utils::ASAssignType<int32>  rssi_high;
  Utils::ASAssignType<int32>  sinr_low;
  Utils::ASAssignType<int32>  sinr_high;
  Utils::ASAssignType<uint32> bss_load_low;
  Utils::ASAssignType<uint32> bss_load_high;
  Utils::ASAssignType<bool>   available;
  bool                        evaluation_policy;
};



/* ==========================================================================
 *                          ANDSF_OPeHRPDThreshold
 * ========================================================================== */
class ANDSF_OPeHRPDThreshold
{
public:
  ANDSF_OPeHRPDThreshold(const XMLElement* rc_elem, bool& flag);

private:
  void parse_available(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  Utils::ASAssignType<bool> available;
};



/* ==========================================================================
 *                          ANDSF_OPThresholdConditions
 * ========================================================================== */
class ANDSF_OPThresholdConditions
{
public:
  ANDSF_OPThresholdConditions(const XMLElement* rc_elem, bool& flag);

private:
  void parse_lte_threshold(const XMLElement* elem, bool& flag);
  void parse_wifi_threshold(const XMLElement* elem, bool& flag);
  void parse_ehrpd_threshold(const XMLElement* elem, bool& flag);
  void parse_eval_policy(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  ASAutoPointer<ANDSF_OPLTEThreshold>   lte_threshold;
  ASAutoPointer<ANDSF_OPWiFiThreshold>  wifi_threshold;
  ASAutoPointer<ANDSF_OPeHRPDThreshold> ehrpd_threshold;
  bool                                  evaluation_policy;
};



/* ==========================================================================
 *                            ANDSF_OPRANValidity
 * ========================================================================== */
class ANDSF_OPRANValidity
{
public:
  ANDSF_OPRANValidity(const XMLElement* rc_elem, bool& flag);

private:
  void parse_threshold_conditions(const XMLElement* elem, bool& flag);
  void parse_call_conditions(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  ASAutoPointer<ANDSF_OPThresholdConditions> threshold_condition;
  ASVector<uint8>                            calltypes;
};



} /* namespace XML */
} /* namespace Appsrv */

#endif /* DS_XML_ANDSF_OP_RAN_VALIDITY_H */
