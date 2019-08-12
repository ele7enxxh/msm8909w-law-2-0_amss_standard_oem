/*==============================================================================

                      ds_xml_ANDSF_FlowDistribution.h

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
04/21/14    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_XML_ANDSF_FLOW_DISTRIBUTION_H
#define DS_XML_ANDSF_FLOW_DISTRIBUTION_H

#include "ds_ASAssignType.h"
#include "ds_ASString.h"
#include "ds_ASVector.h"


namespace Appsrv
{
namespace XML
{

class XMLElement;
class ANDSF_RoutingCriteria;


enum flow_distribution_type
{
  FD_FLOW_BASED,
  FD_SERVICE_BASED,
  FD_APN_ROUTING,
  FD_NON_SEAMLESS_OFFLOAD,
  FD_PRIORITIZED_ACCESS // for ANDSF_PrioritizedAccess use ONLY
};


/* ==========================================================================
 *                              IPFlow_AppID
 * ========================================================================== */
class IPFlow_AppID
{
public:
  IPFlow_AppID(const XMLElement* elem, bool& flag);
  ~IPFlow_AppID();

private:
  void parse_osid(const XMLElement* elem, bool& flag);
  void parse_os_apps(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  ASString            osid; // req
  ASVector<ASString*> os_apps; // req min 1
};



/* ==========================================================================
 *                              ANDSF_IPFlow
 * ========================================================================== */
class ANDSF_IPFlow
{
public:
  ANDSF_IPFlow(const XMLElement* elem, bool& flag);
  ANDSF_IPFlow(const XMLElement* elem, const flow_distribution_type type, bool& flag);
  ~ANDSF_IPFlow();

private:
  void parse_app_id(const XMLElement* elem, bool& flag);
  void parse_address_type(const XMLElement* elem, bool& flag);
  void parse_start_source_ip_address(const XMLElement* elem, bool& flag);
  void parse_end_source_ip_address(const XMLElement* elem, bool& flag);
  void parse_protocol_type(const XMLElement* elem, bool& flag);
  void parse_start_source_port_number(const XMLElement* elem, bool& flag);
  void parse_end_source_port_number(const XMLElement* elem, bool& flag);
  void parse_start_dest_port_number(const XMLElement* elem, bool& flag);
  void parse_end_dest_port_number(const XMLElement* elem, bool& flag);
  void parse_qos(const XMLElement* elem, bool& flag);
  void parse_domain_name(const XMLElement* elem, bool& flag);
  void parse_apn(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  ASVector<IPFlow_AppID*> app_id;
  ASString                address_type;
  ASString                start_src_ip;
  ASString                end_src_ip;
  ASString                protocol_type;
  ASString                start_src_port;
  ASString                end_src_port;
  ASString                start_dest_port; // Flow based only
  ASString                end_dest_port;   // Flow based only
  ASString                qos;             // 0-63
  ASString                domain_name;
  ASString                apn;
};




/* ==========================================================================
 *                              ANDSF_RoutingRule
 * ========================================================================== */
class ANDSF_RoutingRule
{
public:
  ANDSF_RoutingRule(const XMLElement* elem, const flow_distribution_type type, bool& flag);

  bool operator<(const ANDSF_RoutingRule &rhs)
  { return access_network_priority < rhs.access_network_priority; }

private:
  void parse_access_technology(const XMLElement* elem, bool& flag);
  void parse_access_id(const XMLElement* elem, bool& flag);
  void parse_secondary_access_id(const XMLElement* elem, bool& flag);
  void parse_access_network_priority(const XMLElement* elem, bool& flag);
  void parse_apn(const XMLElement* elem, bool& flag);
  void parse_apn_priority(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  uint32   access_technology;        // Req // 0-255
  ASString access_id;
  ASString secondary_access_id;
  uint32   access_network_priority;  // Req
  uint32   apn_priority;             // Req2
  ASString apn;                      // Req2
};




/* ==========================================================================
 *                           ANDSF_PrioritizedAccess
 * ========================================================================== */
class ANDSF_PrioritizedAccess : public ANDSF_RoutingRule
{
public:
  ANDSF_PrioritizedAccess(const XMLElement* elem, bool& flag);
};




/* ==========================================================================
 *                            ANDSF_FlowDistribution
 * ========================================================================== */
class ANDSF_FlowDistribution
{
public:
  ANDSF_FlowDistribution(const XMLElement* elem, const flow_distribution_type type, bool& flag);
  ~ANDSF_FlowDistribution();

  bool operator<(const ANDSF_FlowDistribution& flow) const
  { return (rule_priority < flow.rule_priority); }

private:
  void parse_apn(const XMLElement* elem, bool& flag);
  void parse_routing_rule(const XMLElement* elem, bool& flag);
  void parse_ip_flow(const XMLElement* elem, bool& flag);
  void parse_routing_criteria(const XMLElement* elem, bool& flag);
  void parse_api_override(const XMLElement* elem, bool& flag);
  void parse_rule_priority(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  ASString                         apn;
  ASVector<ANDSF_IPFlow*>          ip_flow;
  ASVector<ANDSF_RoutingCriteria*> routing_criteria;
  ASVector<ANDSF_RoutingRule*>     routing_rule;
  uint32                           rule_priority;
  bool                             is_api_override;

private:
  const flow_distribution_type flow_type;
};




/* ==========================================================================
 *                            ANDSF_FlowDistribution
 * ========================================================================== */
class ANDSF_ForCSBased
{
public:
  ANDSF_ForCSBased(const XMLElement* elem, bool& flag);

  bool operator<(const ANDSF_ForCSBased& flow) const
  { return (rule_priority < flow.rule_priority); }

private:
  void parse_1x_condition(const XMLElement* elem, bool& flag);
  void parse_cs_1x_available(const XMLElement* elem, bool& flag);
  void parse_rule_priority(const XMLElement* elem, bool& flag);

  void parse_thresh_1x_ecio_low(const XMLElement* elem, bool& flag);
  void parse_thresh_1x_ecio_high(const XMLElement* elem, bool& flag);
  void parse_thresh_1x_avail(const XMLElement* elem, bool& flag);
  void parse_thresh_evaluation_policy(const XMLElement* elem, bool& flag);


public:
  Utils::ASAssignType<int32>  thresh_cdma_1x_ecio_low;
  Utils::ASAssignType<int32>  thresh_cdma_1x_ecio_high;
  Utils::ASAssignType<bool>   thresh_cdma_1x_avail;
  bool                        evaluation_policy;

  bool                        cs_1x_available;
  uint32                      rule_priority;
};




/* ==========================================================================
 *                              ANDSF_Isrp_Iarp_base
 * ========================================================================== */
class ANDSF_Isrp_Iarp_base
{
protected:
  ANDSF_Isrp_Iarp_base(const XMLElement* elem, const bool is_isrp, bool& flag);
  ~ANDSF_Isrp_Iarp_base();

private:
  void parse_flow_distribution(
                               const XMLElement*                  elem,
                               bool&                              flag,
                               ASVector<ANDSF_FlowDistribution*>& fd_vector,
                               const flow_distribution_type       flow_type,
                               const char*                        name
                               );
  void parse_flow_based(const XMLElement* elem, bool& flag);
  void parse_service_based(const XMLElement* elem, bool& flag);
  void parse_inter_apn_routing(const XMLElement* elem, bool& flag);
  void parse_non_seamless_offload(const XMLElement* elem, bool& flag);
  void parse_cs_based(const XMLElement* elem, bool& flag);
  void parse_roaming(const XMLElement* elem, bool& flag);
  void parse_plmn(const XMLElement* elem, bool& flag);
  void parse_update_policy(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  ASVector<ANDSF_FlowDistribution*> for_flow_based;           // ISRP
  ASVector<ANDSF_FlowDistribution*> for_service_based;        // ISRP
  ASVector<ANDSF_FlowDistribution*> for_inter_apn_routing;    // IARP
  ASVector<ANDSF_FlowDistribution*> for_non_seamless_offload; // ISRP+IARP
  ASVector<ANDSF_ForCSBased*>       for_cs_based;             // ISRP
  bool                              roaming;                  // Default 0
  ASString                          plmn;                     // Req
  bool                              update_policy;            // Default 0
};




/* ==========================================================================
 *                                ANDSF_Isrp
 * ========================================================================== */
class ANDSF_Isrp : public ANDSF_Isrp_Iarp_base
{
public:
  ANDSF_Isrp(const XMLElement* elem, bool& flag);

private:
  using ANDSF_Isrp_Iarp_base::for_inter_apn_routing;
};




/* ==========================================================================
 *                                ANDSF_Iarp
 * ========================================================================== */
class ANDSF_Iarp : public ANDSF_Isrp_Iarp_base
{
public:
  ANDSF_Iarp(const XMLElement* elem, bool& flag);

private:
  using ANDSF_Isrp_Iarp_base::for_flow_based;
  using ANDSF_Isrp_Iarp_base::for_service_based;
};



} /* namespace XML */
} /* namespace Appsrv */

#endif /* DS_XML_ANDSF_FLOW_DISTRIBUTION_H */
