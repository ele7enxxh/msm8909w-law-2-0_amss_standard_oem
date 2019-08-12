/*==============================================================================

                        ds_XMLUtility.cpp

GENERAL DESCRIPTION
  Utility functions for XML decoding

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

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
#include "ds_XMLUtility.h"

#include "ds_XMLElement.h"
#include "data_msg.h"

namespace ASXML = Appsrv::XML;


namespace
{


bool check_element(
                   const ASXML::XMLElement* element,
                   const char*              element_name,
                   const uint32             expected_attr_count,
                   bool&                    flag
                   );
}


bool ASXML::get_next_child(XMLElement* parent, XMLElement** child, uint32& counter)
{
  if(NULL == parent || parent->children_size() <= counter)
    return false;

  *child = parent->get_child(counter);

  if(NULL == *child)
    return false;

  counter++;
  return true;
}


bool ASXML::get_next_child(const XMLElement* parent, const XMLElement** child, uint32& counter)
{
  if(NULL == parent || parent->children_size() <= counter)
    return false;

  *child = parent->get_child(counter);

  if(NULL == *child)
    return false;

  counter++;
  return true;
}


bool ASXML::check_element_with_content(
                                       const XMLElement* element,
                                       const char*       element_name,
                                       const uint32      expected_attr_count,
                                       const bool        empty_content_is_valid,
                                       bool&             flag
                                       )
{
  if(!check_element(element, element_name, expected_attr_count, flag))
    return false;

  if(!empty_content_is_valid && element->get_text().empty())
  {
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_EMPTY_CONTENT);
  }

  if(0 != element->children_size())
  {
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_CHILD_HIGH);
  }

  return !flag;
}


bool ASXML::check_element_with_child(
                                     const XMLElement* element,
                                     const char*       element_name,
                                     const uint32      expected_attr_count,
                                     const uint32      min_expected_children,
                                     bool&             flag
                                     )
{
  if(!check_element(element, element_name, expected_attr_count, flag))
    return false;

  if(min_expected_children > element->children_size())
  {
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_CHILD_LOW);
  }

  return !flag;
}



bool ASXML::check_element_with_child(
                                     const XMLElement* element,
                                     const char*       element_name,
                                     const uint32      expected_attr_count,
                                     const uint32      min_expected_children,
                                     const uint32      max_expected_children,
                                     bool&             flag
                                     )
{
  if(!check_element_with_child(element, element_name, expected_attr_count, min_expected_children, flag))
    return false;

  if(max_expected_children < element->children_size())
  {
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_CHILD_HIGH);
  }

  return !flag;
}



bool ASXML::check_attribute(const XMLAttribute* attr, const char* attribute_name, bool& flag)
{
  if(NULL == attr)
  {
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_NULL_ATTRIBUTE);
    return false;
  }

  if(attribute_name != attr->name)
  {
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_INVALID_ATTR_NAME);
    return false;
  }

  return true;
}



bool ASXML::str_to_bool(const ASString& str)
{
  return str == "1";
}



/* ============================ Static functions =============================*/
namespace
{

bool check_element(
                   const ASXML::XMLElement* element,
                   const char*              element_name,
                   const uint32             expected_attr_count,
                   bool&                    flag
                   )
{
  if(NULL == element)
  {
    ASXML::XMLDecoderException::get_instance().raise(flag, ASXML::ERR_MSG_NULL_ELEMENT, element_name);
    return false;
  }

  if(NULL != element_name && element_name != element->get_tag())
  {
    DATA_APPSRV_MSG2(MSG_LEGACY_LOW, "check_element : Elem - %s; Val - %s", element_name, element->get_tag().c_str());
    ASXML::XMLDecoderException::get_instance().raise(flag, ASXML::ERR_MSG_INVALID_TAG);
    return false;
  }

  if(expected_attr_count != element->attributes_size())
  {
    ASXML::XMLDecoderException::get_instance().raise(flag, ASXML::ERR_MSG_ATTR_COUNT);
    return false;
  }

  return true;
}

} /* namespace */
