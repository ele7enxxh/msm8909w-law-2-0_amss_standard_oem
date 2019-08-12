/*==============================================================================

                      ds_xml_ANDSF_ValidityArea.cpp

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
03/06/15    ml     Created file/Initial version.
==============================================================================*/
#include "ds_xml_ANDSF_ValidityArea.h"

#include "ds_xml_ANDSF_constants.h"
#include "ds_XMLDecoderException.h"
#include "ds_XMLElement.h"
#include "ds_XMLSchemaValidator.h"
#include "ds_XMLUtility.h"
#include "ds_appsrv_mem.h"
#include <cstdlib> // strtoul

namespace ASXML = Appsrv::XML;



/* ==========================================================================
 *                                 VA_3gppLocation
 * ========================================================================== */
ASXML::VA_3gppLocation::VA_3gppLocation()
: plmn(0)
{ }



ASXML::VA_3gppLocation::VA_3gppLocation(const XMLElement* elem, bool& flag)
: plmn(0)
{
  typedef void (VA_3gppLocation::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem, flag, XMLElementCondition(NULL, true, true, 1, 6));

  // Flag will be set if current element is invalid.
  if(flag) return;


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_PLMN, false, false));
  parse_fcn_ptrs.push_back(&VA_3gppLocation::parse_plmn);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_TAC, true, false));
  parse_fcn_ptrs.push_back(&VA_3gppLocation::parse_tac);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_LAC, true, false));
  parse_fcn_ptrs.push_back(&VA_3gppLocation::parse_lac);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_GERAN_CI, true, false));
  parse_fcn_ptrs.push_back(&VA_3gppLocation::parse_geran_ci);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_UTRAN_CI, true, false));
  parse_fcn_ptrs.push_back(&VA_3gppLocation::parse_utran_ci);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_EUTRA_CI, true, false));
  parse_fcn_ptrs.push_back(&VA_3gppLocation::parse_eutra_ci);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}

void ASXML::VA_3gppLocation::clear()
{
  plmn = 0;
  tac.unset();
  lac.unset();
  geran_ci.unset(); // 16 bit binary
  utran_ci.unset(); // 24 bit binary
  eutra_ci.unset(); // 28 bit binary
}


bool ASXML::VA_3gppLocation::is_ue_loc_valid(const VA_3gppLocation& ue_loc_data) const
{
  /* Check required value(s). */
  if(ue_loc_data.plmn != plmn)
    return false;

  /* Check if UE does not have location data that ANDSF has. Viceversa is valid and ignored. */
  if((tac.is_set()      && !ue_loc_data.tac.is_set())      ||
     (lac.is_set()      && !ue_loc_data.lac.is_set())      ||
     (geran_ci.is_set() && !ue_loc_data.geran_ci.is_set()) ||
     (utran_ci.is_set() && !ue_loc_data.utran_ci.is_set()) ||
     (eutra_ci.is_set() && !ue_loc_data.eutra_ci.is_set()))
  {
    return false;
  }

  /* Compare location data. */
  if((tac.is_set()      &&      ue_loc_data.tac != tac)      ||
     (lac.is_set()      &&      ue_loc_data.lac != lac)      ||
     (geran_ci.is_set() && ue_loc_data.geran_ci != geran_ci) ||
     (utran_ci.is_set() && ue_loc_data.utran_ci != utran_ci) ||
     (eutra_ci.is_set() && ue_loc_data.eutra_ci != eutra_ci))
  {
    return false;
  }

  return true;
}


void ASXML::VA_3gppLocation::parse_plmn(const XMLElement* elem, bool& flag)
{
  plmn = strtoul(elem->get_text().c_str(), NULL, 10);
}


void ASXML::VA_3gppLocation::parse_tac(const XMLElement* elem, bool& flag)
{
  tac = strtoul(elem->get_text().c_str(), NULL, 10);
}


void ASXML::VA_3gppLocation::parse_lac(const XMLElement* elem, bool& flag)
{
  lac = strtoul(elem->get_text().c_str(), NULL, 10);
}


void ASXML::VA_3gppLocation::parse_geran_ci(const XMLElement* elem, bool& flag)
{
  geran_ci = strtoul(elem->get_text().c_str(), NULL, 16);
}


void ASXML::VA_3gppLocation::parse_utran_ci(const XMLElement* elem, bool& flag)
{
  utran_ci = strtoul(elem->get_text().c_str(), NULL, 16);
}


void ASXML::VA_3gppLocation::parse_eutra_ci(const XMLElement* elem, bool& flag)
{
  eutra_ci = strtoul(elem->get_text().c_str(), NULL, 16);
}




/* ==========================================================================
 *                              VA_3gpp2Location_1x
 * ========================================================================== */
ASXML::VA_3gpp2Location_1x::VA_3gpp2Location_1x()
: sid(0)
{ }



ASXML::VA_3gpp2Location_1x::VA_3gpp2Location_1x(const XMLElement* elem, bool& flag)
: sid(0)
{
  typedef void (VA_3gpp2Location_1x::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem, flag, XMLElementCondition(NULL, true, true, 1, 3));

  // Flag will be set if current element is invalid.
  if(flag) return;


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_SID, true, false));
  parse_fcn_ptrs.push_back(&VA_3gpp2Location_1x::parse_sid);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_NID, false, false));
  parse_fcn_ptrs.push_back(&VA_3gpp2Location_1x::parse_nid);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_BASE_ID, false, false));
  parse_fcn_ptrs.push_back(&VA_3gpp2Location_1x::parse_base_id);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}



bool ASXML::VA_3gpp2Location_1x::is_ue_loc_valid(const VA_3gpp2Location_1x& ue_loc_data) const
{
  /* Check required value(s). */
  if(ue_loc_data.sid != sid)
    return false;

  /* Check if UE does not have location data that ANDSF has. Viceversa is valid and ignored. */
  if((nid.is_set()     && !ue_loc_data.nid.is_set()) ||
     (base_id.is_set() && !ue_loc_data.base_id.is_set()))
  {
    return false;
  }

  /* Compare location data. */
  if((nid.is_set()     &&     ue_loc_data.nid != nid) ||
     (base_id.is_set() && ue_loc_data.base_id != base_id))
  {
    return false;
  }

  return true;
}

void ASXML::VA_3gpp2Location_1x::clear()
{
  sid = 0;
  nid.unset();
  base_id.unset();
}


void ASXML::VA_3gpp2Location_1x::parse_sid(const XMLElement* elem, bool& flag)
{
  sid = strtoul(elem->get_text().c_str(), NULL, 10);
}


void ASXML::VA_3gpp2Location_1x::parse_nid(const XMLElement* elem, bool& flag)
{
  nid = strtoul(elem->get_text().c_str(), NULL, 10);
}


void ASXML::VA_3gpp2Location_1x::parse_base_id(const XMLElement* elem, bool& flag)
{
  base_id = strtoul(elem->get_text().c_str(), NULL, 10);
}




/* ==========================================================================
 *                              VA_3gpp2Location_Hrpd
 * ========================================================================== */
ASXML::VA_3gpp2Location_Hrpd::VA_3gpp2Location_Hrpd()
: netmask(0)
{
  memset(sector_id, 0, sizeof(uint8) * 16);
}



ASXML::VA_3gpp2Location_Hrpd::VA_3gpp2Location_Hrpd(const XMLElement* elem, bool& flag)
{
  typedef void (VA_3gpp2Location_Hrpd::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem, flag, XMLElementCondition(NULL, true, true, 2, 2));

  // Flag will be set if current element is invalid.
  if(flag) return;


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_SECTOR_ID, true, false));
  parse_fcn_ptrs.push_back(&VA_3gpp2Location_Hrpd::parse_sector_id);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_NETMASK, true, false));
  parse_fcn_ptrs.push_back(&VA_3gpp2Location_Hrpd::parse_netmask);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


bool ASXML::VA_3gpp2Location_Hrpd::is_ue_loc_valid(const VA_3gpp2Location_Hrpd& ue_loc_data) const
{
  /* Check required value(s). */
  if(0 == memcmp(sector_id, ue_loc_data.sector_id, sizeof(uint8) * 16) && ue_loc_data.netmask == netmask)
    return true;
  else
    return false;
}

void ASXML::VA_3gpp2Location_Hrpd::clear()
{
  memset(sector_id, 0, sizeof(uint8) * 16);
  netmask = 0;;   
}

void ASXML::VA_3gpp2Location_Hrpd::parse_sector_id(const XMLElement* elem, bool& flag)
{
  ASString sid_str(elem->get_text());
  if(32 != sid_str.size())
  {
    XMLDecoderException::get_instance().raise(flag, "VA_3gpp2Location_Hrpd - Invalid string size");
    return;
  }
  for(uint32 i = 0; i < 16; ++i)
  {
    ASString sid_bin(sid_str.c_str() + (2*i), 2);
    sector_id[i] = strtoul(sid_bin.c_str(), NULL, 16);
  }
}


void ASXML::VA_3gpp2Location_Hrpd::parse_netmask(const XMLElement* elem, bool& flag)
{
  netmask = strtoul(elem->get_text().c_str(), NULL, 16);
}




/* ==========================================================================
 *                              VA_3gpp2Location
 * ========================================================================== */
ASXML::VA_3gpp2Location::VA_3gpp2Location(const XMLElement* elem, bool& flag)
{
  typedef void (VA_3gpp2Location::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem, flag, XMLElementCondition(NULL, true, true, 0, 2));

  // Flag will be set if current element is invalid.
  if(flag) return;


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_1X, false, true, 1));
  parse_fcn_ptrs.push_back(&VA_3gpp2Location::parse_1x);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_HRPD, false, true, 1));
  parse_fcn_ptrs.push_back(&VA_3gpp2Location::parse_hrpd);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


ASXML::VA_3gpp2Location::~VA_3gpp2Location()
{
  clear_ptr_vector(onex);
  clear_ptr_vector(hrpd);
}


void ASXML::VA_3gpp2Location::parse_1x(const XMLElement* elem, bool& flag)
{
  for(uint32 i = 0; i < elem->children_size(); ++i)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<VA_3gpp2Location_1x>(), APPSRV_MEM) VA_3gpp2Location_1x(elem->get_child(i), flag),
                                 onex,
                                 flag,
                                 ELEM_NAME_1X
                                 ))
    {
      return;
    }
  }
}


void ASXML::VA_3gpp2Location::parse_hrpd(const XMLElement* elem, bool& flag)
{
  for(uint32 i = 0; i < elem->children_size(); ++i)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<VA_3gpp2Location_Hrpd>(), APPSRV_MEM) VA_3gpp2Location_Hrpd(elem->get_child(i), flag),
                                 hrpd,
                                 flag,
                                 ELEM_NAME_HRPD
                                 ))
    {
      return;
    }
  }
}




/* ==========================================================================
 *                                VA_WimaxLocation
 * ========================================================================== */
ASXML::VA_WimaxLocation::VA_WimaxLocation(const XMLElement* elem, bool& flag)
{
  typedef void (VA_WimaxLocation::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem, flag, XMLElementCondition(NULL, true, true, 2, 2));

  // Flag will be set if current element is invalid.
  if(flag) return;


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_NAP_ID, true, false));
  parse_fcn_ptrs.push_back(&VA_WimaxLocation::parse_nap_id);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_BS_ID, true, false));
  parse_fcn_ptrs.push_back(&VA_WimaxLocation::parse_bs_id);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


void ASXML::VA_WimaxLocation::parse_nap_id(const XMLElement* elem, bool& flag)
{
  nap_id = elem->get_text();
}


void ASXML::VA_WimaxLocation::parse_bs_id(const XMLElement* elem, bool& flag)
{
  bs_id = elem->get_text();
}





/* ==========================================================================
 *                                VA_WlanLocation
 * ========================================================================== */
ASXML::VA_WlanLocation::VA_WlanLocation()
: ssid_set(false), hessid_set(false), bssid_set(false)
{
  memset(hessid, 0, sizeof(uint8) * 6);
  memset(bssid, 0, sizeof(uint8) * 6);
}



ASXML::VA_WlanLocation::VA_WlanLocation(const XMLElement* elem, bool& flag)
: ssid_set(false), hessid_set(false), bssid_set(false)
{
  typedef void (VA_WlanLocation::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem, flag, XMLElementCondition(NULL, true, true, 0, 6));

  // Flag will be set if current element is invalid.
  if(flag) return;


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_HESSID, false, false));
  parse_fcn_ptrs.push_back(&VA_WlanLocation::parse_hessid);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_SSID, false, false));
  parse_fcn_ptrs.push_back(&VA_WlanLocation::parse_ssid);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_BSSID, false, false));
  parse_fcn_ptrs.push_back(&VA_WlanLocation::parse_bssid);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


bool ASXML::VA_WlanLocation::is_ue_loc_valid(const VA_WlanLocation& ue_loc_data) const
{
  /* Nothing to compare. Error? */
  if(!ssid_set && !hessid_set && !bssid_set)
    return false;

  /* Check if UE does not have location data that ANDSF has. Viceversa is valid and ignored. */
  if((ssid_set   && !ue_loc_data.ssid_set)   ||
     (hessid_set && !ue_loc_data.hessid_set) ||
     (bssid_set  && !ue_loc_data.bssid_set))
  {
    return false;
  }

  /* Compare location data. */
  if((ssid_set   && ue_loc_data.ssid != ssid) ||
     (hessid_set && 0 == memcmp(ue_loc_data.hessid, hessid, sizeof(uint8) * 6)) ||
     (bssid_set  && 0 == memcmp(ue_loc_data.bssid,  bssid,  sizeof(uint8) * 6)))
  {
    return false;
  }

  return true;
}

void ASXML::VA_WlanLocation::clear()
{
  ssid = "";
  memset(hessid, 0, sizeof(uint8)*6);
  memset(bssid, 0, sizeof(uint8)*6);
  
  ssid_set = false;
  hessid_set = false;
  bssid_set = false;
}


void ASXML::VA_WlanLocation::parse_hessid(const XMLElement* elem, bool& flag)
{
  if(Utils::mac_addr_to_bin(elem->get_text(), hessid))
  {
    XMLDecoderException::get_instance().raise(flag, "VA_WlanLocation - Failed to decode hessid mac address");
    return;
  }
  hessid_set = true;
}


void ASXML::VA_WlanLocation::parse_ssid(const XMLElement* elem, bool& flag)
{
  ssid = elem->get_text();
  ssid_set = true;
}


void ASXML::VA_WlanLocation::parse_bssid(const XMLElement* elem, bool& flag)
{
  if(Utils::mac_addr_to_bin(elem->get_text(), bssid))
  {
    XMLDecoderException::get_instance().raise(flag, "VA_WlanLocation - Failed to decode bssid mac address");
    return;
  }
  bssid_set = true;
}




/* ==========================================================================
 *                                  GeoLoc_Info
 * ========================================================================== */
ASXML::GeoLoc_Info::GeoLoc_Info(const XMLElement* elem, bool& flag)
{
  typedef void (GeoLoc_Info::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem, flag, XMLElementCondition(NULL, true, true, 0, 6));

  // Flag will be set if current element is invalid.
  if(flag) return;


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_ANCHOR_LATITUDE, true, false));
  parse_fcn_ptrs.push_back(&GeoLoc_Info::parse_anchor_latitude);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_ANCHOR_LONGITUDE, true, false));
  parse_fcn_ptrs.push_back(&GeoLoc_Info::parse_anchor_longitude);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_RADIUS, true, false));
  parse_fcn_ptrs.push_back(&GeoLoc_Info::parse_radius);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


void ASXML::GeoLoc_Info::parse_anchor_latitude(const XMLElement* elem, bool& flag)
{
  anchor_latitude = strtoul(elem->get_text().c_str(), NULL, 16);
}


void ASXML::GeoLoc_Info::parse_anchor_longitude(const XMLElement* elem, bool& flag)
{
  anchor_longitude = strtoul(elem->get_text().c_str(), NULL, 16);
}


void ASXML::GeoLoc_Info::parse_radius(const XMLElement* elem, bool& flag)
{
  radius = strtoul(elem->get_text().c_str(), NULL, 10);
}




/* ==========================================================================
 *                              ANDSF_GeoLocation
 * ========================================================================== */
ASXML::ANDSF_GeoLocation::~ANDSF_GeoLocation()
{
  clear_ptr_vector(circular);
}



/* ==========================================================================
 *                            ANDSF_VAGeoLocation
 * ========================================================================== */
ASXML::ANDSF_VAGeoLocation::ANDSF_VAGeoLocation(const XMLElement* elem, bool& flag)
{
  XMLElementCondition circular_condition(ELEM_NAME_CIRCULAR, false, true, 1);
  XMLElementCondition filler_condition(NULL, true, true, 3, 3);
  const XMLElement*   circ_elem = elem->get_child(0);

  if(circular_condition.validateElement(circ_elem, flag))
    return;

  for(uint32 i = 0; i < circ_elem->children_size(); ++i)
  {
    const XMLElement* child = circ_elem->get_child(i);
    if(filler_condition.validateElement(child, flag))
      return;

    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<GeoLoc_Info>(), APPSRV_MEM) GeoLoc_Info(child, flag),
                                 circular,
                                 flag,
                                 ELEM_NAME_CIRCULAR
                                 ))
    {
      return;
    }
  }
}



/* ==========================================================================
 *                            ANDSF_UEGeoLocation
 * ========================================================================== */
ASXML::ANDSF_UEGeoLocation::ANDSF_UEGeoLocation(const XMLElement* elem, bool& flag)
{
  typedef void (ANDSF_UEGeoLocation::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem);


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_LATITUDE, true, false));
  parse_fcn_ptrs.push_back(&ANDSF_UEGeoLocation::parse_latitude);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_LONGITUDE, true, false));
  parse_fcn_ptrs.push_back(&ANDSF_UEGeoLocation::parse_longitude);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


void ASXML::ANDSF_UEGeoLocation::parse_latitude(const XMLElement* elem, bool& flag)
{
  latitude = strtoul(elem->get_text().c_str(), NULL, 16);
}


void ASXML::ANDSF_UEGeoLocation::parse_longitude(const XMLElement* elem, bool& flag)
{
  longitude = strtoul(elem->get_text().c_str(), NULL, 16);
}




/* ==========================================================================
 *                              ANDSF_VA_base
 * ========================================================================== */
ASXML::ANDSF_VA_base::ANDSF_VA_base()
: va_3gpp2(NULL), va_geoloc(NULL)
{ }


ASXML::ANDSF_VA_base::~ANDSF_VA_base()
{
  clear_ptr_vector(va_3gpp);
  clear_ptr_vector(va_wimax);
  clear_ptr_vector(va_wlan);
}


bool ASXML::ANDSF_VA_base::empty() const
{
  return (
          0 == va_3gpp.size()  &&
          va_3gpp2.is_null()   &&
          0 == va_wimax.size() &&
          0 == va_wlan.size()  &&
          va_geoloc.is_null()
          );
}


void ASXML::ANDSF_VA_base::parse_3gpp(const XMLElement* elem, bool& flag)
{
  for(uint32 i = 0; i < elem->children_size(); ++i)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<VA_3gppLocation>(), APPSRV_MEM) VA_3gppLocation(elem->get_child(i), flag),
                                 va_3gpp,
                                 flag,
                                 ELEM_NAME_3GPP_LOCATION
                                 ))
    {
      return;
    }
  }
}


void ASXML::ANDSF_VA_base::parse_3gpp2(const XMLElement* elem, bool& flag)
{
  va_3gpp2 = new(ds_appsrv_alloc<VA_3gpp2Location>(), APPSRV_MEM) VA_3gpp2Location(elem, flag);
}


void ASXML::ANDSF_VA_base::parse_wimax(const XMLElement* elem, bool& flag)
{
  for(uint32 i = 0; i < elem->children_size(); ++i)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<VA_WimaxLocation>(), APPSRV_MEM) VA_WimaxLocation(elem->get_child(i), flag),
                                 va_wimax,
                                 flag,
                                 ELEM_NAME_WIMAX_LOCATION
                                 ))
    {
      return;
    }
  }
}


void ASXML::ANDSF_VA_base::parse_wlan(const XMLElement* elem, bool& flag)
{
  for(uint32 i = 0; i < elem->children_size(); ++i)
  {
    if(!ds_xml_add_obj_to_vector(
                                 new(ds_appsrv_alloc<VA_WlanLocation>(), APPSRV_MEM) VA_WlanLocation(elem->get_child(i), flag),
                                 va_wlan,
                                 flag,
                                 ELEM_NAME_WLAN_LOCATION
                                 ))
    {
      return;
    }
  }
}



void ASXML::ANDSF_VA_base::parse_rplmn(const XMLElement* elem, bool& flag)
{
  rplmn = elem->get_text();
}




/* ==========================================================================
 *                            ANDSF_ValidityArea
 * ========================================================================== */
ASXML::ANDSF_ValidityArea::ANDSF_ValidityArea(const XMLElement* elem, bool& flag)
{
  typedef void (ANDSF_ValidityArea::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem);


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_3GPP_LOCATION, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_ValidityArea::parse_3gpp);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_3GPP2_LOCATION, false, true, 1, 2));
  parse_fcn_ptrs.push_back(&ANDSF_ValidityArea::parse_3gpp2);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_WIMAX_LOCATION, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_ValidityArea::parse_wimax);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_WLAN_LOCATION, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_ValidityArea::parse_wlan);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_GEO_LOCATION, false, true, 0, 1));
  parse_fcn_ptrs.push_back(&ANDSF_ValidityArea::parse_geoloc);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


void ASXML::ANDSF_ValidityArea::parse_geoloc(const XMLElement* elem, bool& flag)
{
  va_geoloc = new(ds_appsrv_alloc<ANDSF_VAGeoLocation>(), APPSRV_MEM) ANDSF_VAGeoLocation(elem, flag);
}


/* ==========================================================================
 *                            ANDSF_UELocation
 * ========================================================================== */
ASXML::ANDSF_UELocation::ANDSF_UELocation(const XMLElement* elem, bool& flag)
{
  typedef void (ANDSF_UELocation::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem);


  // Add schema rules
  element_validator.addRule(XMLElementCondition(ELEM_NAME_3GPP_LOCATION, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_UELocation::parse_3gpp);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_3GPP2_LOCATION, false, true, 1, 2));
  parse_fcn_ptrs.push_back(&ANDSF_UELocation::parse_3gpp2);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_WIMAX_LOCATION, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_UELocation::parse_wimax);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_WLAN_LOCATION, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_UELocation::parse_wlan);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_GEO_LOCATION, false, true, 2, 2));
  parse_fcn_ptrs.push_back(&ANDSF_UELocation::parse_geoloc);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_RPLMN, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_UELocation::parse_rplmn);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}



void ASXML::ANDSF_UELocation::parse_geoloc(const XMLElement* elem, bool& flag)
{
  va_geoloc = new(ds_appsrv_alloc<ANDSF_UEGeoLocation>(), APPSRV_MEM) ANDSF_UEGeoLocation(elem, flag);
}
