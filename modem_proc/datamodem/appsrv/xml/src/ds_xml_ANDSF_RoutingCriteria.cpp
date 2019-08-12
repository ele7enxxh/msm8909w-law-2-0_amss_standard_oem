/*==============================================================================

                      ds_xml_ANDSF_RoutingCriteria.cpp

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
#include "ds_xml_ANDSF_RoutingCriteria.h"

#include "ds_xml_ANDSF_ValidityArea.h"
#include "ds_xml_ANDSF_RANValidity.h"
#include "ds_xml_ANDSF_OPRANValidity.h"

#include "ds_xml_ANDSF_constants.h"
#include "ds_XMLDecoderException.h"
#include "ds_XMLElement.h"
#include "ds_XMLSchemaValidator.h"
#include "ds_XMLUtility.h"
#include "ds_appsrv_mem.h"
#include "data_msg.h"
#include <cstdlib> // strtoul

namespace ASXML = Appsrv::XML;



namespace
{

/* Static functions */
bool decode_time(const ASString& time_str, uint32 &hour, uint32 &minute, uint32 &second);
bool decode_date(const ASString& date_str, uint32 &year, uint32 &month, uint32 &day);


/* Const variables */
const uint32 DATE_FORMAT_YYYY_MM    = 7;
const uint32 DATE_FORMAT_YYYYMMDD   = 8;
const uint32 DATE_FORMAT_YYYY_MM_DD = 10;

const char   DATE_DELIM             = '-';
const uint32 DATE_DELIM_LOC[2]      = {4, 7};

const uint32 TIME_FORMAT_HH         = 2;
const uint32 TIME_FORMAT_HHMM       = 4;
const uint32 TIME_FORMAT_HH_MM      = 5;
const uint32 TIME_FORMAT_HHMMDD     = 6;
const uint32 TIME_FORMAT_HH_MM_DD   = 8;

const char   TIME_DELIM             = ':';
const uint32 TIME_DELIM_LOC[2]      = {2, 5};

}



/* ==========================================================================
 *                                 ANDSF_Date
 * ========================================================================== */
ASXML::ANDSF_Date::ANDSF_Date()
: year(0), month(0), day(0), set(false)
{ }



ASXML::ANDSF_Date::ANDSF_Date(const uint32 yr, const uint32 mon, const uint32 d)
: year(yr), month(mon), day(d), set(true)
{ }


bool ASXML::ANDSF_Date::operator==(const ANDSF_Date& rhs) const
{
  uint32 lhs_date = (year*10000) + (month*100) + day;
  uint32 rhs_date = (rhs.year*10000) + (rhs.month*100) + rhs.day;
  return lhs_date == rhs_date;
}


bool ASXML::ANDSF_Date::operator!=(const ANDSF_Date& rhs) const
{
  uint32 lhs_date = (year*10000) + (month*100) + day;
  uint32 rhs_date = (rhs.year*10000) + (rhs.month*100) + rhs.day;
  return lhs_date != rhs_date;
}


bool ASXML::ANDSF_Date::operator<(const ANDSF_Date& rhs) const
{
  uint32 lhs_date = (year*10000) + (month*100) + day;
  uint32 rhs_date = (rhs.year*10000) + (rhs.month*100) + rhs.day;
  return lhs_date < rhs_date;
}



bool ASXML::ANDSF_Date::operator<=(const ANDSF_Date& rhs) const
{
  uint32 lhs_date = (year * 10000) + (month * 100) + day;
  uint32 rhs_date = (rhs.year * 10000) + (rhs.month * 100) + rhs.day;
  return lhs_date <= rhs_date;
}



/* ==========================================================================
 *                                 ANDSF_Time
 * ========================================================================== */
ASXML::ANDSF_Time::ANDSF_Time()
: hour(0), minute(0), second(0), set(false)
{ }



ASXML::ANDSF_Time::ANDSF_Time(const uint32 hr, const uint32 min, const uint32 sec)
: hour(hr), minute(min), second(sec), set(true)
{ }


bool ASXML::ANDSF_Time::operator==(const ANDSF_Time& rhs) const
{
  return (hour == rhs.hour && minute == rhs.minute && second == rhs.second);
}


bool ASXML::ANDSF_Time::operator!=(const ANDSF_Time& rhs) const
{
  return (hour != rhs.hour || minute != rhs.minute || second != rhs.second);
}


bool ASXML::ANDSF_Time::operator<(const ANDSF_Time& rhs) const
{
  uint32 lhs_time = (hour * 10000) + (minute * 100) + second;
  uint32 rhs_time = (rhs.hour * 10000) + (rhs.minute * 100) + rhs.second;
  return lhs_time < rhs_time;
}



bool ASXML::ANDSF_Time::operator<=(const ANDSF_Time& rhs) const
{
  uint32 lhs_time = (hour * 10000) + (minute * 100) + second;
  uint32 rhs_time = (rhs.hour * 10000) + (rhs.minute * 100) + rhs.second;
  return lhs_time <= rhs_time;
}




/* ==========================================================================
 *                               ANDSF_TimeOfDay
 * ========================================================================== */
ASXML::ANDSF_TimeOfDay::ANDSF_TimeOfDay(const XMLElement* elem, bool& flag)
: day_of_week(0xFF)
{
  typedef void (ANDSF_TimeOfDay::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(elem, flag, XMLElementCondition(NULL, true, true, 0, 5));

  // Flag will be set if current element is invalid.
  if(flag) return;


  element_validator.addRule(XMLElementCondition(ELEM_NAME_TIME_START, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_TimeOfDay::parse_time_start);
  element_validator.addRule(XMLElementCondition(ELEM_NAME_TIME_STOP, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_TimeOfDay::parse_time_stop);
  element_validator.addRule(XMLElementCondition(ELEM_NAME_DATE_START, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_TimeOfDay::parse_date_start);
  element_validator.addRule(XMLElementCondition(ELEM_NAME_DATE_STOP, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_TimeOfDay::parse_date_stop);
  element_validator.addRule(XMLElementCondition(ELEM_NAME_DAY_OF_WEEK, false, false));
  parse_fcn_ptrs.push_back(&ANDSF_TimeOfDay::parse_day_of_week);

  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);

  if((time_start.set && !time_stop.set  && !date_start.set) ||
     (time_stop.set  && !time_start.set && !date_stop.set))
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ANDSF_TimeOfDay - invalid pattern");
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_PARSE_INVALID, ELEM_NAME_TIME_OF_DAY);
    return;
  }
}


void ASXML::ANDSF_TimeOfDay::parse_time_start(const XMLElement* elem, bool& flag)
{
  uint32 hour, minute, second;
  hour = minute = second = 0;

  if(decode_time(elem->get_text(), hour, minute, second))
  {
    time_start = ANDSF_Time(hour, minute, second);
  }
  else
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ANDSF_TimeOfDay - invalid time format (start)");
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_PARSE_INVALID, ELEM_NAME_TIME_START);
  }
}


void ASXML::ANDSF_TimeOfDay::parse_time_stop(const XMLElement* elem, bool& flag)
{
  uint32 hour, minute, second;
  hour = minute = second = 0;

  if(decode_time(elem->get_text(), hour, minute, second))
  {
    time_stop = ANDSF_Time(hour, minute, second);
  }
  else
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ANDSF_TimeOfDay - invalid time format (stop)");
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_PARSE_INVALID, ELEM_NAME_TIME_STOP);
  }
}


void ASXML::ANDSF_TimeOfDay::parse_date_start(const XMLElement* elem, bool& flag)
{
  uint32 year, month, day;
  year = month = day = 0;

  if(decode_date(elem->get_text(), year, month, day))
  {
    date_start = ANDSF_Date(year, month, day);
  }
  else
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ANDSF_TimeOfDay - invalid date format (start)");
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_PARSE_INVALID, ELEM_NAME_TIME_STOP);
  }
}


void ASXML::ANDSF_TimeOfDay::parse_date_stop(const XMLElement* elem, bool& flag)
{
  uint32 year, month, day;
  year = month = day = 0;

  if(decode_date(elem->get_text(), year, month, day))
  {
    date_stop = ANDSF_Date(year, month, day);

    if(date_start.set && date_stop < date_start)
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ANDSF_TimeOfDay - stop date before start");
      XMLDecoderException::get_instance().raise(flag, ERR_MSG_PARSE_INVALID, ELEM_NAME_TIME_OF_DAY);
    }
  }
  else
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ANDSF_TimeOfDay - invalid date format (stop)");
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_PARSE_INVALID, ELEM_NAME_TIME_STOP);
  }
}


void ASXML::ANDSF_TimeOfDay::parse_day_of_week(const XMLElement* elem, bool& flag)
{
  day_of_week = strtoul(elem->get_text().c_str(), NULL, 10);
  if(128 >= day_of_week)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "ANDSF_TimeOfDay - invalid day of week value");
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_PARSE_INVALID, ELEM_NAME_DAY_OF_WEEK);
  }
}






/* ==========================================================================
 *                            ANDSF_RoutingCriteria
 * ========================================================================== */
ASXML::ANDSF_RoutingCriteria::ANDSF_RoutingCriteria()
: validity_area(NULL), ran_validity(NULL)
{ }


ASXML::ANDSF_RoutingCriteria::ANDSF_RoutingCriteria(const XMLElement* rc_elem, bool& flag)
: validity_area(NULL), ran_validity(NULL)
{
  typedef void (ANDSF_RoutingCriteria::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(rc_elem, flag, XMLElementCondition(NULL, true, true, 0, 7));

  // Check current if element is valid
  if(flag) return;


  // Add schema rules for current tree
  element_validator.addRule(XMLElementCondition(ELEM_NAME_VALIDITY_AREA, false, true, 0, 5));
  parse_fcn_ptrs.push_back(&ANDSF_RoutingCriteria::parse_validity_area);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_VALIDITY_AREA_REF, false, true, 0));
  parse_fcn_ptrs.push_back(&ANDSF_RoutingCriteria::parse_ref);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_TIME_OF_DAY, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_RoutingCriteria::parse_time_of_day);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_TIME_OF_DAY_REF, false, true, 0));
  parse_fcn_ptrs.push_back(&ANDSF_RoutingCriteria::parse_ref);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_RAN_VALIDITY, false, true, 0, 2));
  parse_fcn_ptrs.push_back(&ANDSF_RoutingCriteria::parse_ran_validity);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_RAN_VALIDITY_REF, false, true, 0));
  parse_fcn_ptrs.push_back(&ANDSF_RoutingCriteria::parse_ref);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_OP_SPECIFIC_RAN_VALIDITY, false, true, 0, 2));
  parse_fcn_ptrs.push_back(&ANDSF_RoutingCriteria::parse_op_ran_validity);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


ASXML::ANDSF_RoutingCriteria::~ANDSF_RoutingCriteria()
{
  clear_ptr_vector(time_of_day);
}


void ASXML::ANDSF_RoutingCriteria::parse_validity_area(const XMLElement* elem, bool& flag)
{
  validity_area = new(ds_appsrv_alloc<ANDSF_ValidityArea>(), APPSRV_MEM) ANDSF_ValidityArea(elem, flag);

  if(validity_area.is_null())
  {
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC_OBJ, ELEM_NAME_VALIDITY_AREA);
  }
}


void ASXML::ANDSF_RoutingCriteria::parse_time_of_day(const XMLElement* elem, bool& flag)
{
  uint32 num_tod = elem->children_size();
  for(uint32 i = 0; i < num_tod; i++)
  {
    ANDSF_TimeOfDay* new_timeofday = new(ds_appsrv_alloc<ANDSF_TimeOfDay>(), APPSRV_MEM) ANDSF_TimeOfDay(elem->get_child(i), flag);
    if(!ds_xml_add_obj_to_vector(new_timeofday, time_of_day, flag, ELEM_NAME_TIME_OF_DAY))
    {
      return;
    }
  }
}


void ASXML::ANDSF_RoutingCriteria::parse_ran_validity(const XMLElement* elem, bool& flag)
{
  ran_validity = new(ds_appsrv_alloc<ANDSF_RANValidity>(), APPSRV_MEM) ANDSF_RANValidity(elem, flag);

  if(ran_validity.is_null())
  {
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC_OBJ, ELEM_NAME_RAN_VALIDITY);
  }
}


void ASXML::ANDSF_RoutingCriteria::parse_ref(const XMLElement* elem, bool& flag)
{
  // Do nothing
}


void ASXML::ANDSF_RoutingCriteria::parse_op_ran_validity(const XMLElement* elem, bool& flag)
{
  operator_ran_validity = new(ds_appsrv_alloc<ANDSF_OPRANValidity>(), APPSRV_MEM) ANDSF_OPRANValidity(elem, flag);

  if(operator_ran_validity.is_null())
  {
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC_OBJ, ELEM_NAME_OP_SPECIFIC_RAN_VALIDITY);
  }
}





/* ==========================================================================
 *                         ANDSF_ValidityCriteria
 * ========================================================================== */
ASXML::ANDSF_ValidityCriteria::ANDSF_ValidityCriteria(const XMLElement* validity_elem, bool& flag)
{
  typedef void (ANDSF_ValidityCriteria::*elem_parser)(const XMLElement*, bool&);

  ASVector<elem_parser> parse_fcn_ptrs;
  XMLElementValidator   element_validator(validity_elem, flag, XMLElementCondition(NULL, true, true, 0, 3));

  // Check current if element is valid
  if(flag) return;


  element_validator.addRule(XMLElementCondition(ELEM_NAME_VALIDITY_AREA, false, true, 0, 5));
  parse_fcn_ptrs.push_back(&ANDSF_ValidityCriteria::parse_validity_area);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_TIME_OF_DAY, false, true, 1));
  parse_fcn_ptrs.push_back(&ANDSF_ValidityCriteria::parse_time_of_day);

  element_validator.addRule(XMLElementCondition(ELEM_NAME_RAN_VALIDITY, false, true, 0, 2));
  parse_fcn_ptrs.push_back(&ANDSF_ValidityCriteria::parse_ran_validity);


  // Iterate child elements against the schema rules
  ds_xml_iterate_elements(this, element_validator, parse_fcn_ptrs, flag);
}


ASXML::ANDSF_ValidityCriteria::~ANDSF_ValidityCriteria()
{ }



/* ==========================================================================
 *                            Utility functions
 * ========================================================================== */
namespace
{

bool decode_time(const ASString& time_str, uint32 &hour, uint32 &minute, uint32 &second)
{
  hour   = 0;
  minute = 0;
  second = 0;

  switch(time_str.length())
  {
    case TIME_FORMAT_HH_MM_DD:
    {
      if(TIME_DELIM != (time_str.c_str())[TIME_DELIM_LOC[1]])
      {
        return false;
      }
      second = strtoul(ASString(time_str.c_str()+TIME_DELIM_LOC[1]+1, 2).c_str(), NULL, 10);
    } // fallthrough
    case TIME_FORMAT_HH_MM:
    {
      if(TIME_DELIM != (time_str.c_str())[TIME_DELIM_LOC[0]])
      {
        return false;
      }
      minute = strtoul(ASString(time_str.c_str()+TIME_DELIM_LOC[0]+1, 2).c_str(), NULL, 10);
    } // fallthrough
    case TIME_FORMAT_HH:
    {
      hour   = strtoul(ASString(time_str.c_str(), 2).c_str(), NULL, 10);
    }
    break;

    case TIME_FORMAT_HHMMDD:
    {
      second = strtoul(ASString(time_str.c_str()+4, 2).c_str(), NULL, 10);
    } // fallthrough
    case TIME_FORMAT_HHMM:
    {
      minute = strtoul(ASString(time_str.c_str()+2, 2).c_str(), NULL, 10);
      hour   = strtoul(ASString(time_str.c_str(), 2).c_str(), NULL, 10);
    }
    break;

    default:
    {
      return false;
    }
  }

  /*
    valid value range
      hour   : 00 to 24
      minute : 00 to 59
      second : 00 to 60
  */
  if(24 < hour || 60 <= minute || 60 < second)
  {
    hour = minute = second = 0;
    return false;
  }

  return true;
}


bool decode_date(const ASString& date_str, uint32 &year, uint32 &month, uint32 &day)
{
  year  = 0;
  month = 1;
  day   = 1;

  switch(date_str.length())
  {
    case DATE_FORMAT_YYYY_MM_DD:
    {
      if(DATE_DELIM != (date_str.c_str())[DATE_DELIM_LOC[1]])
      {
        return false;
      }
      day   = strtoul(ASString(date_str.c_str()+DATE_DELIM_LOC[1]+1, 2).c_str(), NULL, 10);
    }
    case DATE_FORMAT_YYYY_MM:
    {
      if(DATE_DELIM != (date_str.c_str())[DATE_DELIM_LOC[0]])
      {
        return false;
      }

      month = strtoul(ASString(date_str.c_str()+DATE_DELIM_LOC[0]+1, 2).c_str(), NULL, 10);
      year  = strtoul(ASString(date_str.c_str(), 4).c_str(), NULL, 10);
    }
    break;

    case DATE_FORMAT_YYYYMMDD:
    {
      year  = strtoul(ASString(date_str.c_str(), 4).c_str(), NULL, 10);
      month = strtoul(ASString(date_str.c_str()+4, 2).c_str(), NULL, 10);
      day   = strtoul(ASString(date_str.c_str()+6, 2).c_str(), NULL, 10);
    }
    break;

    default:
    {
      return false;
    }
  }

  if(0 == month || 12 < month ||
     0 == day   || 31 < day)
  {
    year = month = day = 0;
    return false;
  }

  return true;
}






} // namespace

