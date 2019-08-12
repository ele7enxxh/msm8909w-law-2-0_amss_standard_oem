/*==============================================================================

                            ds_XMLDecoder.cpp

GENERAL DESCRIPTION
  Base XML decoder

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
#include "ds_XMLDecoder.h"

#include "ds_XMLElement.h"
#include "ds_appsrv_mem.h"
#include "data_msg.h"
#include <cctype> // isdigit()

namespace ASXML = Appsrv::XML;



namespace
{

/* Constatnts for XML parsing */
const char XML_DELIM_OPEN_ANGLE_BRACKET              = '<';
const char XML_DELIM_CLOSE_ANGLE_BRACKET             = '>';
const char XML_DELIM_ENDSTRING                       = '\0';
const char XML_DELIM_ATTR_NAME                       = '=';
const char XML_DELIM_ATTR_VAL                        = '"';

const char XML_SPECIAL_ELEMENT_MARK_END_SLASH        = '/';
const char XML_SPECIAL_ELEMENT_MARK_QUESTION_MARK    = '?';
const char XML_SPECIAL_ELEMENT_MARK_EXCLAMATION_MARK = '!';

const char XML_COMMENT_MARK[]                        = "--";
const char XML_DOCTYPE_MARK[]                        = "!DOCTYPE";

}



/*==============================================================================
                                  XMLDecoder
==============================================================================*/
ASXML::XMLDecoder::XMLDecoder(const char* xml_content)
: root(NULL), flag(false)
{
  ASString xml(xml_content);
  DATA_APPSRV_MSG1(MSG_LEGACY_HIGH, "XMLDecoder::XMLDecoder - decoding xml file of size %d", xml.size());
  decode(xml);

  if(flag)
  {
    DATA_APPSRV_MSG_SPRINTF_2(
                              MSG_LEGACY_ERROR,
                              "XMLDecoder::XMLDecoder - XMLDecoderException : %s; %s",
                              XMLDecoderException::get_instance().what(),
                              XMLDecoderException::get_instance().get_details()
                              );
    clean();
  }
}


ASXML::XMLDecoder::XMLDecoder(const char* xml_content, uint32 len)
: root(NULL), flag(false)
{
  ASString xml(xml_content, len);
  DATA_APPSRV_MSG2(MSG_LEGACY_HIGH, "XMLDecoder::XMLDecoder - decoding xml file of size %d, len %d", xml.size(), len);
  decode(xml);

  if(flag)
  {
    DATA_APPSRV_MSG_SPRINTF_2(
                              MSG_LEGACY_ERROR,
                              "XMLDecoder::XMLDecoder - XMLDecoderException : %s; %s",
                              XMLDecoderException::get_instance().what(),
                              XMLDecoderException::get_instance().get_details()
                              );
    clean();
  }
}

ASXML::XMLDecoder::~XMLDecoder()
{
  clean();
}

void ASXML::XMLDecoder::clean()
{
  if (NULL != root)
    ds_appsrv_free(root);

  root = NULL;
}


void ASXML::XMLDecoder::decode(ASString& raw_bytes)
{
  uint32   skip_count = 0;
  ASString token;

  XMLDecoderException::get_instance().reset();

  if(raw_bytes.empty())
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "XMLDecoderException: Empty input given");
    return;
  }

  iss = ASStringStream(raw_bytes.c_str(), true);

  // check xml declaration
  token = get_initial_content();
  if(flag) return;

  if(XML_SPECIAL_ELEMENT_MARK_QUESTION_MARK == token[0])
  {
    if (XML_SPECIAL_ELEMENT_MARK_QUESTION_MARK != token[token.size()-1])
    {
      XMLDecoderException::get_instance().raise(flag, ERR_MSG_INIT); // invalid xml declaration
      return;
    }
    skip_count++;
  }
  else
  {
    iss.reset();
  }

  // check xml doctype
  token = get_initial_content();
  if(flag) return;

  if(token.size() >= 8 && ASString(token.c_str(), 8) == XML_DOCTYPE_MARK)
  {
    skip_count++;
  }

  iss.reset();
  for(uint32 i=0; i<skip_count; i++)
  {
    get_next_content(iss, XML_DELIM_OPEN_ANGLE_BRACKET, NULL);
    if(flag) return;
    get_next_content(iss, XML_DELIM_CLOSE_ANGLE_BRACKET, NULL);
    if(flag) return;
  }

  // Start parsing XML
  decode_loop();
}




void ASXML::XMLDecoder::decode_loop()
{
  bool        self_ending = false;
  bool        root_closed = false;
  XMLElement* cur_element = NULL;
  XMLElement* new_element = NULL;
  ASString    token;
  ASString    input;

  ASVector<XMLElement*> parent_vector;


  while(iss.good())
  {
    get_next_content(iss, XML_DELIM_OPEN_ANGLE_BRACKET, ERR_MSG_NO_BRACKET);
    if(flag) return;
    token = get_next_content(iss, XML_DELIM_CLOSE_ANGLE_BRACKET, ERR_MSG_NO_BRACKET);
    if(flag) return;
    token.remove_trailing_spaces();

    if(!iss.good() || 0 == token.size())
    {
      DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "XMLDecoder::decode_loop - no token");
      XMLDecoderException::get_instance().raise(flag, ERR_MSG_NO_BRACKET);
      return;
    }



    if(XML_SPECIAL_ELEMENT_MARK_EXCLAMATION_MARK == token[0])
    {
      // if comment
      if(token.size() >= 3 && ASString(token.c_str()+1, 2) == XML_COMMENT_MARK)
      {
        // check if last two char in string is end comment indicator
        if(token.size() < 5 || ASString(token.c_str()+token.size()-2, 2) != XML_COMMENT_MARK)
        {
          find_end_comment();
          if(flag) return;
        }
      }
      else
      {
        XMLDecoderException::get_instance().raise(flag, ERR_MSG_OTHER);
        return;
      }

      continue;
    }

    else if(XML_SPECIAL_ELEMENT_MARK_END_SLASH == token[0])
    {
      if (NULL == root)
      {
        XMLDecoderException::get_instance().raise(flag, ERR_MSG_OTHER);
        return;
      }

      ASStringStream tok_iss(token, true);
      tok_iss.next_ptr(); // skip /
      tok_iss.get_next(input, XML_DELIM_ENDSTRING);

      if(0 == input.length() || cur_element->get_tag() != input)
      {
        XMLDecoderException::get_instance().raise(flag, ERR_MSG_INVALID_CLOSURE);
        return;
      }

      if(cur_element == root)
      {
        root_closed = true;
        break; // done with parsing.
      }

      cur_element = parent_vector.back();
      parent_vector.pop_back();

      continue;
    }

    else if(isdigit(token[0]))// || ispunct(token[0]))
    {
      XMLDecoderException::get_instance().raise(flag, ERR_MSG_INVALID_TAG);
      return;
    }

    else
    {
      uint32 endloc = token.size()-1;
      if(XML_SPECIAL_ELEMENT_MARK_END_SLASH == token[endloc])
      {
        self_ending = true;
        token[endloc] = '\0';
      }

      ASStringStream tok_iss(token, true);

      tok_iss.get_next(input);

      new_element = new(ds_appsrv_alloc<XMLElement>(), APPSRV_MEM) XMLElement(input);

      if(NULL == new_element)
      {
        XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC, "new element");
        return;
      }

      if(NULL == root)
      {
        root        = new_element;
        cur_element = new_element;
      }
      else
      {
        if(!cur_element->add_child(new_element))
        {
          ds_appsrv_free(new_element);
          XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC, "add child element");
          return;
        }
      }


      // get attribute
      decode_attr(new_element, tok_iss);
      if(flag) return;


      if(self_ending)
      {
        self_ending = false;

        if(new_element == root)
        {
          root_closed = true;
          break;
        }
        else
        {
          get_next_content(iss, XML_DELIM_OPEN_ANGLE_BRACKET, ERR_MSG_EOF);
          iss.back(); // get back to < for next loop
          continue;
        }
      }
      else
      {
        // get content
        token = get_next_content(iss, XML_DELIM_OPEN_ANGLE_BRACKET, ERR_MSG_EOF);
        if(flag) return;
        token.remove_trailing_spaces();

        if(0 != token.length())
        {
          if(!token.resolve_xml_escapes())
          {
            XMLDecoderException::get_instance().raise(flag, ERR_MSG_ESCAPE_DECODE);
            return;
          }

          new_element->set_text(token);
        }
        iss.back(); // get back to < for next loop
      }


      if(new_element != root && !parent_vector.push_back(cur_element))
      {
        XMLDecoderException::get_instance().raise(flag, ERR_MSG_MALLOC, "parent_vector");
        return;
      }
      cur_element = new_element;
    }
  }


  if(!root_closed)
  {
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_INVALID_CLOSURE);
    return;
  }
}





void ASXML::XMLDecoder::decode_attr(XMLElement* current_element, ASStringStream& attr_iss)
{
  ASString name, value;

  while(attr_iss.good())
  {
    attr_iss.get_next(name, XML_DELIM_ATTR_NAME);
    name.remove_trailing_spaces();

    if(0==name.size())
      return;

    if(attr_iss.eof())
    {
      XMLDecoderException::get_instance().raise(flag, ERR_MSG_ATTR_DECODE);
      return;
    }

    value = get_next_content(attr_iss, XML_DELIM_ATTR_VAL, ERR_MSG_ATTR_DECODE);
    if(flag) return;
    value.remove_trailing_spaces();

    if(0 != value.size() || iss.eof()) // = is not followed by a quote
    {
      XMLDecoderException::get_instance().raise(flag, ERR_MSG_ATTR_DECODE);
      return;
    }

    value = get_next_content(attr_iss, XML_DELIM_ATTR_VAL, ERR_MSG_ATTR_DECODE);
    if(flag) return;

    if(!value.resolve_xml_escapes())
    {
      XMLDecoderException::get_instance().raise(flag, ERR_MSG_ESCAPE_DECODE);
      return;
    }

    current_element->add_attribute(name, value);
  }
}





ASString ASXML::XMLDecoder::get_initial_content()
{
  ASString token;

  token = get_next_content(iss, XML_DELIM_OPEN_ANGLE_BRACKET, ERR_MSG_INIT);
  if(flag) return token;
  token.remove_trailing_spaces();
  if(0 != token.length())
  {
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_INIT);
    return token;
  }

  token = get_next_content(iss, XML_DELIM_CLOSE_ANGLE_BRACKET, ERR_MSG_INIT);
  if(flag) return token;

  if(0 == token.length())
  {
    XMLDecoderException::get_instance().raise(flag, ERR_MSG_INIT);
    return token;
  }

  return token;
}


void ASXML::XMLDecoder::find_end_comment()
{
  ASString token;

  do{
    token = get_next_content(iss, XML_DELIM_CLOSE_ANGLE_BRACKET, ERR_MSG_COMMENT);
    if(flag) return;

    if(token.size() >= 2 && ASString(token.c_str()+token.size()-2, 2) == XML_COMMENT_MARK)
    {
      return;
    }
  } while(iss.good());

  XMLDecoderException::get_instance().raise(flag, XML_COMMENT_MARK);
}



ASString ASXML::XMLDecoder::get_next_content(ASStringStream& xss, const char delim, const char* err_msg)
{
  ASString token;

  if(xss.get_next(token, delim) && NULL != err_msg)
  {
    XMLDecoderException::get_instance().raise(flag, err_msg);
    return token;
  }

  return token;
}
