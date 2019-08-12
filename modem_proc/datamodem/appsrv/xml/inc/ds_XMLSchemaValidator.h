/*==============================================================================

                        ds_XMLSchemaValidator.h

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
#ifndef DS_XML_ELEMENT_VALIDATOR_H
#define DS_XML_ELEMENT_VALIDATOR_H

#include "ds_ASVector.h"

class ASString;

namespace Appsrv
{
namespace XML
{

class XMLElement;
class XMLAttribute;



// class XMLAttributeConditions
// {
// public:
//   XMLAttributeConditions();

//   XMLAttributeConditions(XMLAttributeConditions& cond);
//   XMLAttributeConditions(const XMLAttributeConditions& cond);
//   void operator=(XMLAttributeConditions attr_cond);

//   void add_attribute(const char* attr_name, bool required);
//   void add_attribute(const ASString& attr_name, bool required);

//   bool validateAttribute(const XMLElement* elem, bool& flag) const;

// private:
//   struct AttributeInfo
//   {
//     ASString name;
//     bool     required;

//     AttributeInfo()
//     : required(true)
//     { }
//   };


//   uint32                  min_attributes;
//   uint32                  max_attributes;
//   ASVector<AttributeInfo> attribute_list;
// };



class XMLElementCondition
{
public:
  XMLElementCondition();
  XMLElementCondition(
                      const char* name,
                      bool        required,
                      bool        empty_text_valid
                      );
  XMLElementCondition(
                      const char* name,
                      bool        required,
                      bool        empty_text_valid,
                      uint32      min_child
                      );
  XMLElementCondition(
                      const char* name,
                      bool        required,
                      bool        empty_text_valid,
                      uint32      min_child,
                      uint32      max_child
                      );

  void addAttribute(const char* attr_name, bool required);
  void addAttribute(const ASString& attr_name, bool required);

  bool validateElement(const XMLElement* elem, bool& flag) const;


private:
  bool checkElement(const XMLElement* elem, bool& flag) const;


  const char* element_name;
  bool        is_required;
  bool        empty_content_is_valid;
  uint32      expected_num_attributes;
  uint32      min_expected_children;
  uint32      max_expected_children;
  bool        check_max_children;

  // XMLAttributeConditions attribute_conditions;

  friend class XMLElementValidator;
};



class XMLElementValidator
{
public:
  XMLElementValidator(const XMLElement* elem);
  XMLElementValidator(const XMLElement* elem, bool& flag, const XMLElementCondition& condition);

  void addRule(const XMLElementCondition& condition);
  /*
    Returns true if there is a child to handle. child will contain the child to
      handle and cur_loc specifies which child it is from the given schema
      condition.

    Returns false if 1. there are no more children to handle or
                     2. there is an element that doesn't follow the schema rule
  */
  bool getNextChild(const XMLElement*& child, uint32& cur_loc, bool& flag);

  // Returns true if there are any child element remaining to check
  bool parseIncomplete() const;

private:
  ASVector<XMLElementCondition> conditions;
  const XMLElement* element;
  uint32 condition_counter;
  uint32 element_counter;
};



// class XMLSchemaValidator
// {
// public:
//   XMLSchemaValidator(const XMLElement* xml_schema);

//   /*
//   */
//   bool validateXMLSchema(const XMLElement* xml_content);

// private:
//   ASVector<XMLSchemaValidator> child_conditions;
//   XMLElementCondition          condition;
// };




} /* namespace XML */
} /* namespace Appsrv */

#endif /* DS_XML_ELEMENT_VALIDATOR_H */
