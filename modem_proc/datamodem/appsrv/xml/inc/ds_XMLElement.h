/*==============================================================================

                              ds_XMLElement.h

GENERAL DESCRIPTION
  XML element structure

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
#ifndef DS_XML_ELEMENT_H
#define DS_XML_ELEMENT_H

#include "ds_ASVector.h"
#include "ds_ASString.h"


namespace Appsrv
{
namespace Utils
{
  // forward declaration
  class ASOStream;
}


namespace XML
{

/*==============================================================================
CLASS XMLAttribute

DESCRIPTION
  XML attribute name-value pair
==============================================================================*/
class XMLAttribute
{
public:
  ASString name;
  ASString value;

  XMLAttribute() {}
  XMLAttribute(const XMLAttribute& other);
  XMLAttribute(const char* n, const char* v);
  XMLAttribute(const char* n, const char* v, uint32 n_length, uint32 v_length);
  XMLAttribute(const ASString& n, const ASString& v);
};



/*==============================================================================
CLASS XMLElement

DESCRIPTION
  XML element contents
==============================================================================*/
class XMLElement
{
public:
  XMLElement(const XMLElement& other, bool& flag);
  XMLElement(char* tg);
  XMLElement(ASString& tg);
  ~XMLElement();

  /* Getters */
  /*===========================================================================
  FUNCTION XMLElement::attributes_size

  DESCRIPTION
    Get the number of attributes the element has

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  uint32 attributes_size();
  const uint32 attributes_size() const;

  /*===========================================================================
  FUNCTION XMLElement::children_size

  DESCRIPTION
    Get the number of children the element has

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  uint32 children_size();
  const uint32 children_size() const;

  /*===========================================================================
  FUNCTION XMLElement::get_tag

  DESCRIPTION
    Get the element tag name

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  ASString& get_tag();
  const ASString& get_tag() const;

  /*===========================================================================
  FUNCTION XMLElement::get_text

  DESCRIPTION
    Get the text content of the element

  PARAMETERS
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  ASString& get_text();
  const ASString& get_text() const;

  /*===========================================================================
  FUNCTION XMLElement::get_attribute

  DESCRIPTION
    Get the attribute at index location. NULL will be returned if index is
    bigger than the number of attributes

  PARAMETERS
    [In] index - The index of which attribute to retrieve

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  XMLAttribute* get_attribute(uint32 index);
  const XMLAttribute* get_attribute(uint32 index) const;

  /*===========================================================================
  FUNCTION XMLElement::get_child

  DESCRIPTION
    Get the child at index location. NULL will be returned if index is bigger
    than the number of children

  PARAMETERS
    [In] index - The index of which child to retrieve

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  XMLElement* get_child(uint32 index);
  const XMLElement* get_child(uint32 index) const;


  /* Adders / Setters */
  /*===========================================================================
  FUNCTION XMLElement::set_tag

  DESCRIPTION
    Set the tag value

  PARAMETERS
    [In] tg - The tag string to set

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  void set_tag(const char* tg);
  void set_tag(const ASString& tg);

  /*===========================================================================
  FUNCTION XMLElement::set_text

  DESCRIPTION
    Set the text value

  PARAMETERS
    [In] tx - The text string to set

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  void set_text(const char* tx);
  void set_text(const ASString& tx);

  /*===========================================================================
  FUNCTION XMLElement::add_attribute

  DESCRIPTION
    Adds a new attribute to the attributes vector

  PARAMETERS
    [In] name  - The name of the attribute
    [In] value - The value of the attribute

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  bool add_attribute(const ASString& name, const ASString& value);

  /*===========================================================================
  FUNCTION XMLElement::get the child at index location

  DESCRIPTION
    Adds a new child to the children vector

  PARAMETERS
    [In] child - The child element

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
  ===========================================================================*/
  bool add_child(XMLElement* child);


private:
  // <tag attribute="val">text</tag>
  ASString                tag;        // Element tag (name)
  ASString                text;       // Text content
  ASVector<XMLAttribute*> attributes; // Attribute vector
  ASVector<XMLElement*>   children;   // Children vector
};



// OStream overload for XMLElement and XMLAttribute
Utils::ASOStream& operator<<(Utils::ASOStream& os, const XMLAttribute& attr);
Utils::ASOStream& operator<<(Utils::ASOStream& os, const XMLAttribute* attr);
Utils::ASOStream& operator<<(Utils::ASOStream& os, const XMLElement& elem);
Utils::ASOStream& operator<<(Utils::ASOStream& os, const XMLElement* elem);


} /* namespace XML */
} /* namespace Appsrv */

#endif /* DS_XML_ELEMENT_H */
