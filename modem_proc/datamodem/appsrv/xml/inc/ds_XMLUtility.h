/*==============================================================================

                        ds_XMLUtility.h

GENERAL DESCRIPTION
  Utility functions for XML decoding

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
#ifndef DS_XML_UTILITY_H
#define DS_XML_UTILITY_H

#include "comdef.h"

#include "ds_ASUtility.h"
#include "ds_ASVector.h"
#include "ds_XMLDecoderException.h"
#include "ds_XMLSchemaValidator.h"
#include "ds_appsrv_mem.h"

class ASString;

namespace Appsrv
{
namespace XML
{

class XMLElement;
class XMLAttribute;

/*===========================================================================
FUNCTION get_next_child

DESCRIPTION
  Get the next child in the parent element. Returns true on success, else false.

PARAMETERS
  [In]     parent  - The parent XMLElement
  [Out]    child   - The child to return
  [In/Out] counter - The current location counter; increments by one if success

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
bool get_next_child(XMLElement* parent, XMLElement** child, uint32& counter);
bool get_next_child(const XMLElement* parent, const XMLElement** child, uint32& counter);


/*===========================================================================
FUNCTION check_element_with_content

DESCRIPTION
  Element validator for an element with content expected. Any validator failure
  will set the flag to true.

PARAMETERS
  [In]  element                - The element to check
  [In]  element_name           - The expected name of the element; unchecked if NULL
  [In]  expected_attr_count    - The expected number of attributes
  [In]  empty_content_is_valid - True if empty content is ok, else false
  [Out] flag                   - This will be set to true if validation fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
bool check_element_with_content(
                                const XMLElement* element,
                                const char*       element_name,
                                const uint32      expected_attr_count,
                                const bool        empty_content_is_valid,
                                bool&             flag
                                );


/*===========================================================================
FUNCTION check_element_with_child

DESCRIPTION
  Element validator for an element with content expected. Any validator failure
  will set the flag to true.

PARAMETERS
  [In]  element               - The element to check
  [In]  element_name          - The expected name of the element; unchecked if NULL
  [In]  expected_attr_count   - The expected number of attributes
  [In]  min_expected_children - The minimum number children the element must contain
  [In]  max_expected_children - The maximum number children the element must contain
  [Out] flag                  - This will be set to true if validation fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
bool check_element_with_child(
                              const XMLElement* element,
                              const char*       element_name,
                              const uint32      expected_attr_count,
                              const uint32      min_expected_children,
                              const uint32      max_expected_children,
                              bool&             flag
                              );

bool check_element_with_child(
                              const XMLElement* element,
                              const char*       element_name,
                              const uint32      expected_attr_count,
                              const uint32      min_expected_children,
                              bool&             flag
                              );


/*===========================================================================
FUNCTION check_attribute

DESCRIPTION
  Check is the attribute name is correct

PARAMETERS
  [In]  attr           - Attribute to check.
  [In]  attribute_name - Expected attribute name.
  [Out] flag           - This will be set to true if validation fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
bool check_attribute(const XMLAttribute* attr, const char* attribute_name, bool& flag);


/*===========================================================================
FUNCTION str_to_bool

DESCRIPTION
  Converts the given string to a bool value. Expected values are "0" and "1"
    only. Note that invalid input will return false.

PARAMETERS
  [In] str - String to convert to bool.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
bool str_to_bool(const ASString& str);


/*===========================================================================
FUNCTION add_obj_to_vector

DESCRIPTION
  Add the given object pointer to the vector.
  Flag is set to true if
    - object is NULL
    - flag is already set
    - failed to add object to vector
  Returns true if object is successfully added to vector

PARAMETERS
  [In]  object   - The object to add to vector
  [In]  vec      - The vector to add the object to
  [Out] flag     - This will be set to true if it fails to add object to vector fails
  [In]  obj_type - Object type name. Doesn't need to be exact; log purposes only.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
template <typename T>
bool ds_xml_add_obj_to_vector(T* object, ASVector<T*>& vec, bool& flag, const char* obj_type);



/*===========================================================================
FUNCTION ds_xml_iterate_elements

DESCRIPTION
  Iterate the child elements aganst the rule validator. Flag will be set if the
    element or its child elements is invalid.

PARAMETERS
  [In]  object            - The caller class' this pointer.
  [In]  element_validator - The rule validator to iterate through.
  [In]  parse_fcn_ptrs     - Function list to parse the corresponding element.
  [Out] flag             - This will be set to true if rule validation fails

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
template <typename T, typename fcn>
void ds_xml_iterate_elements(
                             T*                   object,
                             XMLElementValidator& element_validator,
                             ASVector<fcn>&       parse_fcn_ptrs,
                             bool&                flag
                             );








template <typename T>
bool ds_xml_add_obj_to_vector(T* object, ASVector<T*>& vec, bool& flag, const char* obj_type)
{
  if(NULL == object)
  {
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC_OBJ, obj_type);
  }
  else if(flag)
  {
    ds_appsrv_free<T>(object);
  }
  else if(!vec.push_back(object))
  {
    ds_appsrv_free<T>(object);
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC_VEC, obj_type);
  }
  return !flag; // true if flag is not set.
}



template <typename T, typename fcn>
void ds_xml_iterate_elements(
                             T*                   object,
                             XMLElementValidator& element_validator,
                             ASVector<fcn>&       parse_fcn_ptrs,
                             bool&                flag
                             )
{
  // Iterate child elements against the schema rules
  while(element_validator.parseIncomplete())
  {
    const XMLElement* child = NULL;
    uint32            loc   = 0;

    if(element_validator.getNextChild(child, loc, flag))
      (object->*(parse_fcn_ptrs[loc]))(child, flag);

    if(flag) return;
  }
}


} /* namespace ASXML */
} /* namespace ds */


#endif /* DS_XML_UTILITY_H */
