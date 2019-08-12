/*==============================================================================

                          ds_xml_ANDSF_ValidityArea.h

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
#ifndef DS_XML_ANDSF_VALIDITY_AREA_H
#define DS_XML_ANDSF_VALIDITY_AREA_H

#include "ds_ASAssignType.h"
#include "ds_ASAutoPointer.h"
#include "ds_ASString.h"
#include "ds_ASVector.h"


namespace Appsrv
{
namespace XML
{

class XMLElement;



/* ==========================================================================
 *                      ANDSF Error Condition Values
 * ========================================================================== */
// #define DS_ANDSF_EOK            0   /* No error */
// #define DS_ANDSF_EEMPTYVALUE    1   /* Empty string */


/* ============================= VALIDITY AREA ============================= */

/* ==========================================================================
 *                                 VA_3gppLocation
 * ========================================================================== */
class VA_3gppLocation
{
public:
  VA_3gppLocation();
  VA_3gppLocation(const XMLElement* elem, bool& flag);
  bool is_ue_loc_valid(const VA_3gppLocation& ue_loc_data) const;
  void clear();
  


private:
  void parse_plmn(const XMLElement* elem, bool& flag);
  void parse_tac(const XMLElement* elem, bool& flag);
  void parse_lac(const XMLElement* elem, bool& flag);
  void parse_geran_ci(const XMLElement* elem, bool& flag);
  void parse_utran_ci(const XMLElement* elem, bool& flag);
  void parse_eutra_ci(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  uint32                      plmn; // 24 bit value
  Utils::ASAssignType<uint16> tac;
  Utils::ASAssignType<uint16> lac;
  Utils::ASAssignType<uint32> geran_ci; // 16 bit binary
  Utils::ASAssignType<uint32> utran_ci; // 24 bit binary
  Utils::ASAssignType<uint32> eutra_ci; // 28 bit binary
};



/* ==========================================================================
 *                              VA_3gpp2Location_1x
 * ========================================================================== */
class VA_3gpp2Location_1x
{
public:
  VA_3gpp2Location_1x();
  VA_3gpp2Location_1x(const XMLElement* elem, bool& flag);
  bool is_ue_loc_valid(const VA_3gpp2Location_1x& ue_loc_data) const;
  void clear();

private:
  void parse_sid(const XMLElement* elem, bool& flag);
  void parse_nid(const XMLElement* elem, bool& flag);
  void parse_base_id(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  uint16                      sid;
  Utils::ASAssignType<uint16> nid;
  Utils::ASAssignType<uint16> base_id;
};



/* ==========================================================================
 *                              VA_3gpp2Location_Hrpd
 * ========================================================================== */
class VA_3gpp2Location_Hrpd
{
public:
  VA_3gpp2Location_Hrpd();
  VA_3gpp2Location_Hrpd(const XMLElement* elem, bool& flag);
  bool is_ue_loc_valid(const VA_3gpp2Location_Hrpd& ue_loc_data) const;
  void clear();

private:
  void parse_sector_id(const XMLElement* elem, bool& flag);
  void parse_netmask(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  uint8 sector_id[16];
  uint8 netmask;   // required bin 8
};



/* ==========================================================================
 *                              VA_3gpp2Location
 * ========================================================================== */
class VA_3gpp2Location
{
public:
  VA_3gpp2Location(const XMLElement* elem, bool& flag);
  ~VA_3gpp2Location();

private:
  void parse_1x(const XMLElement* elem, bool& flag);
  void parse_hrpd(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  ASVector<VA_3gpp2Location_1x*>   onex;
  ASVector<VA_3gpp2Location_Hrpd*> hrpd;
};



/* ==========================================================================
 *                                VA_WimaxLocation
 * ========================================================================== */
class VA_WimaxLocation
{
public:
  VA_WimaxLocation(const XMLElement* elem, bool& flag);

private:
  void parse_nap_id(const XMLElement* elem, bool& flag);
  void parse_bs_id(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  ASString nap_id;
  ASString bs_id;
};



/* ==========================================================================
 *                                VA_WlanLocation
 * ========================================================================== */
class VA_WlanLocation
{
public:
  VA_WlanLocation();
  VA_WlanLocation(const XMLElement* elem, bool& flag);
  bool is_ue_loc_valid(const VA_WlanLocation& ue_loc_data) const;
  void clear();

private:
  void parse_hessid(const XMLElement* elem, bool& flag);
  void parse_ssid(const XMLElement* elem, bool& flag);
  void parse_bssid(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  ASString ssid;
  uint8    hessid[6];
  uint8    bssid[6];
  bool     ssid_set;
  bool     hessid_set;
  bool     bssid_set;
};



/* ==========================================================================
 *                                  GeoLoc_Info
 * ========================================================================== */
class GeoLoc_Info
{
public:
  GeoLoc_Info(const XMLElement* elem, bool& flag);

private:
  void parse_anchor_latitude(const XMLElement* elem, bool& flag);
  void parse_anchor_longitude(const XMLElement* elem, bool& flag);
  void parse_radius(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  uint32 anchor_latitude;
  uint32 anchor_longitude;
  uint32 radius;
};



/* ==========================================================================
 *                              ANDSF_GeoLocation
 * ========================================================================== */
class ANDSF_GeoLocation
{
public:
  ~ANDSF_GeoLocation();

  /* ============================ Class Members ============================= */

  uint32                 latitude;
  uint32                 longitude;
  ASVector<GeoLoc_Info*> circular;
};



/* ==========================================================================
 *                            ANDSF_VAGeoLocation
 * ========================================================================== */
class ANDSF_VAGeoLocation: public ANDSF_GeoLocation
{
public:
  ANDSF_VAGeoLocation(const XMLElement* elem, bool& flag);
  ~ANDSF_VAGeoLocation();

private:
  using ANDSF_GeoLocation::latitude;
  using ANDSF_GeoLocation::longitude;
};



/* ==========================================================================
 *                            ANDSF_UEGeoLocation
 * ========================================================================== */
class ANDSF_UEGeoLocation: public ANDSF_GeoLocation
{
public:
  ANDSF_UEGeoLocation(const XMLElement* elem, bool& flag);
  ~ANDSF_UEGeoLocation();

private:
  void parse_latitude(const XMLElement* elem, bool& flag);
  void parse_longitude(const XMLElement* elem, bool& flag);

  using ANDSF_GeoLocation::circular;
};



/* ==========================================================================
 *                              ANDSF_VA_base
 * ========================================================================== */
class ANDSF_VA_base
{
public:
  bool empty() const;

protected:
  ANDSF_VA_base();
  virtual ~ANDSF_VA_base();

  void parse_3gpp(const XMLElement* elem, bool& flag);
  void parse_3gpp2(const XMLElement* elem, bool& flag);
  void parse_wimax(const XMLElement* elem, bool& flag);
  void parse_wlan(const XMLElement* elem, bool& flag);
  void parse_rplmn(const XMLElement* elem, bool& flag);
  virtual void parse_geoloc(const XMLElement* elem, bool& flag) = 0;


  /* ============================ Class Members ============================= */
public:
  ASVector<VA_3gppLocation*>       va_3gpp;
  ASAutoPointer<VA_3gpp2Location>  va_3gpp2;
  ASVector<VA_WimaxLocation*>      va_wimax;
  ASVector<VA_WlanLocation*>       va_wlan;
  ASAutoPointer<ANDSF_GeoLocation> va_geoloc;
  ASString                         rplmn;
};



/* ==========================================================================
 *                            ANDSF_ValidityArea
 * ========================================================================== */
class ANDSF_ValidityArea : public ANDSF_VA_base
{
public:
  ANDSF_ValidityArea(const XMLElement* elem, bool& flag);

private:
  virtual void parse_geoloc(const XMLElement* elem, bool& flag);

  using ANDSF_VA_base::rplmn;
};



/* ==========================================================================
 *                            ANDSF_UELocation
 * ========================================================================== */
class ANDSF_UELocation : public ANDSF_VA_base
{
public:
  ANDSF_UELocation(const XMLElement* elem, bool& flag);

private:
  virtual void parse_geoloc(const XMLElement* elem, bool& flag);
};



} /* namespace XML */
} /* namespace Appsrv */

#endif /* DS_XML_ANDSF_VALIDITY_AREA_H */
