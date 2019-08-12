/*==============================================================================

                          ds_xml_ANDSFManagementObject.h

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
#ifndef DS_XML_ANDSF_MANAGEMENT_OBJECT_H
#define DS_XML_ANDSF_MANAGEMENT_OBJECT_H

#include "ds_ASAutoPointer.h"
#include "ds_ASString.h"
#include "ds_ASVector.h"


namespace Appsrv
{
namespace XML
{

class XMLElement;

class ANDSF_UELocation;
class ANDSF_Isrp;
class ANDSF_Iarp;
class ANDSF_ValidityArea;
class ANDSF_TimeOfDay;
class ANDSF_RANValidity;
class ANDSF_Policy;
class ANDSF_Wlansp;
class ANDSF_ValidityCriteria;


/* ==========================================================================
 *                             ANDSF_DiscoveryInfo
 * ========================================================================== */
class ANDSF_DiscoveryInfo
{
public:
  ANDSF_DiscoveryInfo(const XMLElement* discovery_elem, bool& flag);

private:
  void parse_access_network_type(const XMLElement* elem, bool& flag);
  void parse_access_network_area(const XMLElement* elem, bool& flag);
  void parse_validity_area_ref(const XMLElement* elem, bool& flag);
  void parse_access_network_ref(const XMLElement* elem, bool& flag);
  void parse_plmn(const XMLElement* elem, bool& flag);


public:
  uint32                            access_network_type;  // Req // 0-255
  ASAutoPointer<ANDSF_ValidityArea> access_network_area;
  ASString                          access_network_info_ref;
  ASString                          plmn;                 // Req
};



/* ==========================================================================
 *                               ANDSF_UEProfile
 * ========================================================================== */
class ANDSF_UEProfile
{
public:
  ANDSF_UEProfile(const XMLElement* profile_elem, bool& flag);
  ~ANDSF_UEProfile();


  ASVector<ASString*> osid;
  ASString            dev_capability;
};



/* ==========================================================================
 *                           ANDSF_RuleSelectionInfo
 * ========================================================================== */
class ANDSF_RuleSelectionInfo
{
public:
  ANDSF_RuleSelectionInfo(const XMLElement* elem, bool& flag);
  ~ANDSF_RuleSelectionInfo();

private:
  void parse_vplmns(const XMLElement* elem, bool& flag);
  void parse_plmn(const XMLElement* elem, bool& flag);


public:
  ASVector<ASString*> vplmns;
  ASString            plmn;
};



/* ==========================================================================
 *                                HOP_PrefSPs
 * ========================================================================== */
class HOP_PrefSPs
{
public:
  HOP_PrefSPs(const XMLElement* hop_elem, bool& flag);

private:
  void parse_priority(const XMLElement* elem, bool& flag);
  void parse_service_provider(const XMLElement* elem, bool& flag);


public:
  uint32   priority;
  ASString srv_provider;
};



/* ==========================================================================
 *                           ANDSF_HomeOperatorPref
 * ========================================================================== */
class ANDSF_HomeOperatorPref
{
public:
  ANDSF_HomeOperatorPref(const XMLElement* profile_elem, bool& flag);
  ~ANDSF_HomeOperatorPref();

private:
  void parse_pspl(const XMLElement* elem, bool& flag);
  void parse_s2a_connectivity_preference(const XMLElement* elem, bool& flag);
  void parse_plmn(const XMLElement* elem, bool& flag);
  void parse_update_policy(const XMLElement* elem, bool& flag);

  // PSPL
  void parse_rplmn_pref(const XMLElement* elem, bool& flag);
  void parse_equiv_home_sps(const XMLElement* elem, bool& flag);
  void parse_preferred_sps(const XMLElement* elem, bool& flag);


public:
  // HomeOperatorPreference
  ASString s2a_conn_pref;
  ASString plmn;                 // Req
  bool     update_policy;

  // PSPL
  ASString               rplmn_pref;
  ASVector<ASString*>    equiv_home_sps;
  ASVector<HOP_PrefSPs*> preferred_sps;
};



/* ==========================================================================
 *                           ANDSFManagementObject
 * ========================================================================== */
class ANDSFManagementObject
{
public:
  ANDSFManagementObject(const XMLElement* elem, bool& flag);
  ~ANDSFManagementObject();

private:
  /* Root child decoders */
  void parse_name(const XMLElement* policies, bool& flag);
  void parse_policy(const XMLElement* policies, bool& flag);
  void parse_discovery_info(const XMLElement* discovery_infos, bool& flag);
  void parse_ue_location(const XMLElement* ue_loc, bool& flag);
  void parse_isrp(const XMLElement* isrps, bool& flag);
  void parse_iarp(const XMLElement* iarps, bool& flag);
  void parse_ue_profile(const XMLElement* profile, bool& flag);
  void parse_wlansp(const XMLElement* wlansps, bool& flag);
  void parse_rule_selection_info(const XMLElement* elem, bool& flag);
  void parse_home_operator_pref(const XMLElement* elem, bool& flag);
  void parse_validity_criteria(const XMLElement* elem, bool& flag);
  void parse_ext(const XMLElement* ext_elem, bool& flag);


public:
  ASString                               name;
  ASVector<ANDSF_Policy*>                policy;
  ASVector<ANDSF_DiscoveryInfo*>         discovery_information;
  ASAutoPointer<ANDSF_UELocation>        ue_location;
  ASVector<ANDSF_Isrp*>                  isrp;
  ASAutoPointer<ANDSF_UEProfile>         ue_profile;
  ASVector<ANDSF_Wlansp*>                wlansp;
  ASVector<ANDSF_Iarp*>                  iarp;
  ASAutoPointer<ANDSF_RuleSelectionInfo> rule_selection_info;
  ASAutoPointer<ANDSF_HomeOperatorPref>  home_operator_pref;
  ASVector<ANDSF_ValidityCriteria*>      validity_criteria;
  const XMLElement*                      ext;
};



} /* namespace ASXML */
} /* namespace ds */


#endif /* DS_XML_ANDSF_MANAGEMENT_OBJECT_H */
