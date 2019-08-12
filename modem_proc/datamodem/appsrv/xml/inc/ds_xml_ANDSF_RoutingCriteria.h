/*==============================================================================

                          ds_xml_ANDSF_RoutingCriteria.h

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
#ifndef DS_XML_ANDSF_ROUTING_CRITERIA_H
#define DS_XML_ANDSF_ROUTING_CRITERIA_H

#include "ds_ASAutoPointer.h"
#include "ds_ASVector.h"


namespace Appsrv
{
namespace XML
{

class XMLElement;
class ANDSF_ValidityArea;
class ANDSF_RANValidity;
class ANDSF_OPRANValidity;


/* ==========================================================================
 *                                 ANDSF_Date
 * ========================================================================== */
struct ANDSF_Date
{
  uint32 year;
  uint32 month;
  uint32 day;
  bool   set;


  ANDSF_Date();
  ANDSF_Date(const uint32 yr, const uint32 mon, const uint32 d);

  bool operator==(const ANDSF_Date& rhs) const;
  bool operator!=(const ANDSF_Date& rhs) const;
  bool operator<(const ANDSF_Date& rhs) const;
  bool operator<=(const ANDSF_Date& rhs) const;
};



/* ==========================================================================
 *                                 ANDSF_Time
 * ========================================================================== */
struct ANDSF_Time
{
  uint32 hour;
  uint32 minute;
  uint32 second;
  bool   set;


  ANDSF_Time();
  ANDSF_Time(const uint32 hr, const uint32 min, const uint32 sec);

  bool operator==(const ANDSF_Time& rhs) const;
  bool operator!=(const ANDSF_Time& rhs) const;
  bool operator<(const ANDSF_Time& rhs) const;
  bool operator<=(const ANDSF_Time& rhs) const;
};



/* ==========================================================================
 *                               ANDSF_TimeOfDay
 * ========================================================================== */
class ANDSF_TimeOfDay
{
public:
  ANDSF_TimeOfDay(const XMLElement* elem, bool& flag);

private:
  void parse_time_start(const XMLElement* elem, bool& flag);
  void parse_time_stop(const XMLElement* elem, bool& flag);
  void parse_date_start(const XMLElement* elem, bool& flag);
  void parse_date_stop(const XMLElement* elem, bool& flag);
  void parse_day_of_week(const XMLElement* elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  ANDSF_Time time_start;
  ANDSF_Time time_stop;
  ANDSF_Date date_start;
  ANDSF_Date date_stop;
  uint8      day_of_week; // 8 bit w/ 1st bit set to 1
};



/* ==========================================================================
 *                            ANDSF_RoutingCriteria
 * ========================================================================== */
class ANDSF_RoutingCriteria
{
public:
  ANDSF_RoutingCriteria();
  ANDSF_RoutingCriteria(const XMLElement* rc_elem, bool& flag);
  virtual ~ANDSF_RoutingCriteria();

protected:
  void parse_validity_area(const XMLElement* name_elem, bool& flag);
  void parse_time_of_day(const XMLElement* name_elem, bool& flag);
  void parse_ran_validity(const XMLElement* name_elem, bool& flag);
  void parse_ref(const XMLElement* name_elem, bool& flag);
  void parse_op_ran_validity(const XMLElement* name_elem, bool& flag);


  /* ============================ Class Members ============================= */
public:
  ASAutoPointer<ANDSF_ValidityArea> validity_area;
  ASVector<ANDSF_TimeOfDay*>         time_of_day;
  ASAutoPointer<ANDSF_RANValidity>   ran_validity;
  ASAutoPointer<ANDSF_OPRANValidity> operator_ran_validity;
};

/* ==========================================================================
 *                           ANDSF_ValidityCriteria
 * ========================================================================== */
class ANDSF_ValidityCriteria : public ANDSF_RoutingCriteria
{
public:
  ANDSF_ValidityCriteria(const XMLElement* profile_elem, bool& flag);
  ~ANDSF_ValidityCriteria();

private:
  using ANDSF_RoutingCriteria::operator_ran_validity;
};




} /* namespace XML */
} /* namespace Appsrv */

#endif /* DS_XML_ANDSF_ROUTING_CRITERIA_H */
