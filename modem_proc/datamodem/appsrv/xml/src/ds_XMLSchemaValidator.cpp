/*==============================================================================

                        ds_XMLElementValidator.cpp

GENERAL DESCRIPTION
  XML Schema validator for the current element and its children

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
#include "ds_XMLSchemaValidator.h"

#include "ds_XMLDecoderException.h"
#include "ds_XMLElement.h"
#include "ds_XMLUtility.h"
#include "data_msg.h"

namespace ASXML = Appsrv::XML;



ASXML::XMLElementCondition::XMLElementCondition()
: element_name(NULL),
  is_required(false),
  empty_content_is_valid(false),
  expected_num_attributes(0),
  min_expected_children(0),
  max_expected_children(0),
  check_max_children(false)
{ }


ASXML::XMLElementCondition::XMLElementCondition(
                                                const char* name,
                                                bool        required,
                                                bool        empty_text_valid
                                                )
: element_name(name),
  is_required(required),
  empty_content_is_valid(empty_text_valid),
  expected_num_attributes(0),
  min_expected_children(0),
  max_expected_children(0),
  check_max_children(true)
{ }


ASXML::XMLElementCondition::XMLElementCondition(
                                                const char* name,
                                                bool        required,
                                                bool        empty_text_valid,
                                                uint32      min_child
                                                )
: element_name(name),
  is_required(required),
  empty_content_is_valid(empty_text_valid),
  expected_num_attributes(0),
  min_expected_children(min_child),
  max_expected_children(0),
  check_max_children(false)
{ }


ASXML::XMLElementCondition::XMLElementCondition(
                                                const char* name,
                                                bool        required,
                                                bool        empty_text_valid,
                                                uint32      min_child,
                                                uint32      max_child
                                                )
: element_name(name),
  is_required(required),
  empty_content_is_valid(empty_text_valid),
  expected_num_attributes(0),
  min_expected_children(min_child),
  max_expected_children(max_child),
  check_max_children(true)
{ }


void ASXML::XMLElementCondition::addAttribute(const char* attr_name, bool required)
{
  ++expected_num_attributes;
}


void ASXML::XMLElementCondition::addAttribute(const ASString& attr_name, bool required)
{
  addAttribute(attr_name.c_str(), required);
}


bool ASXML::XMLElementCondition::validateElement(const XMLElement* elem, bool& flag) const
{
  if(flag)
    return false;

  if(NULL == elem)
  {
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_NULL_ELEMENT, element_name);
    return false;
  }

  if(NULL != element_name && element_name != elem->get_tag())
  {
    if(is_required)
      XMLDecoderException::get_instance().raise(flag, ERR_MSG_INVALID_TAG);

    DATA_APPSRV_MSG_SPRINTF_2(MSG_LEGACY_LOW, "validateElement : Elem - %s; Val - %s", element_name, elem->get_tag().c_str());
    return false;
  }
  else
  {
    // Element name matches rule. Check remaining conditions.
    if(min_expected_children > elem->children_size())
    {
      XMLDecoderException::get_instance().raise(flag, ERR_MSG_CHILD_LOW);
      return false;
    }

    if(check_max_children && max_expected_children < elem->children_size())
    {
      XMLDecoderException::get_instance().raise(flag, ERR_MSG_CHILD_HIGH);
      return false;
    }

    if(expected_num_attributes != elem->attributes_size())
    {
      XMLDecoderException::get_instance().raise(flag, ERR_MSG_ATTR_COUNT);
      return false;
    }
    // if(!attribute_conditions.validateAttribute(elem, flag))
    // {
      // return false;
    // }

    if(!empty_content_is_valid && elem->get_text().empty())
    {
      XMLDecoderException::get_instance().raise(flag, ERR_MSG_EMPTY_CONTENT);
      return false;
    }
  }

  return true;
}











ASXML::XMLElementValidator::XMLElementValidator(const XMLElement* elem)
: element(elem), condition_counter(0), element_counter(0)
{ }


ASXML::XMLElementValidator::XMLElementValidator(const XMLElement* elem, bool& flag, const XMLElementCondition& condition)
: element(elem), condition_counter(0), element_counter(0)
{
  condition.validateElement(element, flag);
}



void ASXML::XMLElementValidator::addRule(const XMLElementCondition& condition)
{
  conditions.push_back(condition);
}

static const char ERR_MSG_PARSE_NO_SCHEMA[] = "Failed parsing - Element has no matching schema rule";

/*
  Returns true if there is a child to handle. child will contain the child to
    handle and cur_loc specifies which child it is from the given schema
    condition.

  Returns false if 1. there are no more children to handle or
                   2. there is an element that doesn't follow the schema rule
*/
bool ASXML::XMLElementValidator::getNextChild(const XMLElement*& child, uint32& cur_loc, bool& flag)
{
  if(!get_next_child(element, &child, element_counter))
  {
    for(; condition_counter < conditions.size(); ++condition_counter)
    {
      if(conditions[condition_counter].is_required)
        XMLDecoderException::get_instance().raise(flag, ERR_MSG_PARSE_MISSING);
    }

    child = NULL;
    return false;
  }

  for(; condition_counter < conditions.size(); ++condition_counter)
  {
    cur_loc = condition_counter;

    if(conditions[condition_counter].validateElement(child, flag))
    {
      // Increment so the next call to getNextChild doesn't check the condition again.
      ++condition_counter;
      return true;
    }
    else if(flag)
      return false;
  }

  XMLDecoderException::get_instance().raise(flag, ERR_MSG_PARSE_NO_SCHEMA);
  return false;
}


// Returns true if there are any child element or schema conditions remaining to check
bool ASXML::XMLElementValidator::parseIncomplete() const
{
  return (element_counter < element->children_size() || condition_counter < conditions.size());
}


