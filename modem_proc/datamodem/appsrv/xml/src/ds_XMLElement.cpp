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
05/20/15    ml     Use memory from modem heap
04/21/14    ml     Created file/Initial version.
==============================================================================*/
#include "ds_XMLElement.h"

#include "ds_ASIOStream.h"
#include "ds_appsrv_mem.h"


namespace Appsrv
{
namespace XML
{

XMLAttribute::XMLAttribute(const XMLAttribute& other)
: name(other.name), value(other.value)
{ }


XMLAttribute::XMLAttribute(const char* n, const char* v)
{
  name  = ASString(n);
  value = ASString(v);
}


XMLAttribute::XMLAttribute(const char* n, const char* v, uint32 n_length, uint32 v_length)
{
  name  = ASString(n, n_length);
  value = ASString(v, v_length);
}


XMLAttribute::XMLAttribute(const ASString& n, const ASString& v)
: name(n), value(v) { }



XMLElement::XMLElement(const XMLElement& other, bool& flag)
: tag(other.tag), text(other.text)
{
  // Copy attributes
  for(uint32 i = 0; i < other.attributes.size(); ++i)
  {
    const XMLAttribute* attr = other.attributes[i];
    if(NULL == attr)
    {
      continue;
    }
    else if(!add_attribute(attr->name, attr->value))
    {
      flag = true;
      return;
    }
  }


  // Copy children
  for(uint32 i = 0; i < other.children.size(); ++i)
  {
    const XMLElement* child = other.children[i];
    if(NULL == child)
      continue;

    XMLElement* elem = new(ds_appsrv_alloc<XMLElement>(), APPSRV_MEM) XMLElement(*child, flag);

    if(NULL == elem)
    {
      flag = true;
      return;
    }

    if(flag || !add_child(elem))
    {
      ds_appsrv_free(elem);
      flag = true;
      return;
    }
  }
}


XMLElement::XMLElement(char* tg)
: tag(ASString(tg)) { }


XMLElement::XMLElement(ASString& tg)
: tag(tg) { }


XMLElement::~XMLElement()
{
  clear_ptr_vector(attributes);
  clear_ptr_vector(children);
}


uint32 XMLElement::attributes_size()
{
  return attributes.size();
}

const uint32 XMLElement::attributes_size() const
{
  return attributes.size();
}


uint32 XMLElement::children_size()
{
  return children.size();
}

const uint32 XMLElement::children_size() const
{
  return children.size();
}


/* GETTER */
ASString& XMLElement::get_tag()
{
  return tag;
}

const ASString& XMLElement::get_tag() const
{
  return tag;
}

ASString& XMLElement::get_text()
{
  return text;
}

const ASString& XMLElement::get_text() const
{
  return text;
}


XMLAttribute* XMLElement::get_attribute(uint32 index)
{
  if (index >= attributes.size())
    return NULL;

  return attributes[index];
}

const XMLAttribute* XMLElement::get_attribute(uint32 index) const
{
  if (index >= attributes.size())
    return NULL;

  return attributes[index];
}


XMLElement* XMLElement::get_child(uint32 index)
{
  if (index >= children.size())
    return NULL;

  return children[index];
}

const XMLElement* XMLElement::get_child(uint32 index) const
{
  if (index >= children.size())
    return NULL;

  return children[index];
}



/* SETTER */
void XMLElement::set_tag(const char* tg)
{
  tag = ASString(tg);
}


void XMLElement::set_tag(const ASString& tg)
{
  tag = tg;
}



void XMLElement::set_text(const char* tx)
{
  text = ASString(tx);
}


void XMLElement::set_text(const ASString& tx)
{
  text = tx;
}



bool XMLElement::add_attribute(const ASString& name, const ASString& value)
{
  XMLAttribute* attribute = new(ds_appsrv_alloc<XMLAttribute>(), APPSRV_MEM) XMLAttribute(name, value);
  if(NULL == attribute)
    return false;

  if(!attributes.push_back(attribute))
  {
    ds_appsrv_free(attribute);
    return false;
  }
  return true;
}



bool XMLElement::add_child(XMLElement* child)
{
  if(NULL != child && children.push_back(child))
    return true;
  return false;
}



/*==============================================================================
                OStream overload for XMLElement and XMLAttribute
==============================================================================*/
Utils::ASOStream& operator<<(Utils::ASOStream& os, const XMLAttribute& attr)
{
  os << ' ' << attr.name << "=\"" << attr.value << '"';
  return os;
}


Utils::ASOStream& operator<<(Utils::ASOStream& os, const XMLAttribute* attr)
{
  if(NULL == attr)
    return os;

  os << ' ' << attr->name << "=\"" << attr->value << '"';

  return os;
}


// TODO: Convert entity references in attribute value and element text
Utils::ASOStream& operator<<(Utils::ASOStream& os, const XMLElement& elem)
{
  os << '<' << elem.get_tag();

  // Print attributes
  for(uint32 i = 0; i < elem.attributes_size(); ++i)
  {
    os << elem.get_attribute(i);
  }
  os << '>';

  // Print text
  if(!elem.get_text().empty())
  {
    os << elem.get_text();
  }

  // Print children
  for(uint32 i = 0; i < elem.children_size(); ++i)
  {
    os << elem.get_child(i);
  }

  // close tag
  os << "</" << elem.get_tag() << '>';

  return os;
}


// TODO: Convert entity references in attribute value and element text
Utils::ASOStream& operator<<(Utils::ASOStream& os, const XMLElement* elem)
{
  if(NULL == elem)
    return os;

  os << '<' << elem->get_tag();
  for(uint32 i = 0; i < elem->attributes_size(); ++i)
  {
    os << elem->get_attribute(i);
  }
  os << '>';


  // Print text
  if(!elem->get_text().empty())
  {
    os << elem->get_text();
  }

  // Print children
  for(uint32 i = 0; i < elem->children_size(); ++i)
  {
    os << *(elem->get_child(i));
  }

  // close tag
  os << "</" << elem->get_tag() << '>';

  return os;
}



} /* namespace XML */
} /* namespace Appsrv */
