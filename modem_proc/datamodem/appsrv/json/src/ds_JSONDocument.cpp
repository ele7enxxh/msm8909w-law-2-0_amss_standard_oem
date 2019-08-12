/*==============================================================================

                              ds_JSONDocument.cpp

GENERAL DESCRIPTION



  Copyright (c) 2016 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/21/16    ml     Created file/Initial version.
==============================================================================*/
#include "ds_JSONDocument.h"

#include "ds_json_utils.h"
#include "ds_ASFStream.h"
#include "ds_ASString.h"

#include "data_msg.h"
#include "ds_appsrv_mem.h"

#include <cstdlib>
#include <cctype>
using namespace Appsrv::JSON;
using namespace Appsrv::Utils;

namespace
{

enum ds_json_parse_state
{
  DS_JSON_PARSE_STATE_OBJ = 0,
  DS_JSON_PARSE_STATE_ARRAY
};

struct ds_json_parser_stack
{
  ds_json_parse_state parse_state;
  JSONValueType*      active_root;

  ds_json_parser_stack();
  ds_json_parser_stack(ds_json_parse_state state, JSONValueType* root);
};

bool parse_value(Appsrv::Utils::ASIStream& json_iss, JSONValueType* root_value, ASList<ds_json_parser_stack>& stack);
bool parse_object(Appsrv::Utils::ASIStream& json_iss, JSONValueType* root_value, ASList<ds_json_parser_stack>& stack);
bool parse_array(Appsrv::Utils::ASIStream& json_iss, JSONValueType* root_value, ASList<ds_json_parser_stack>& stack);

} /* namespace */




namespace Appsrv
{
namespace JSON
{

JSONDocument::JSONDocument()
: root(JSON_TYPE_NULL)
{ }



JSONDocument::JSONDocument(const ASString& json_content)
: root(JSON_TYPE_NULL)
{
  parse(json_content.c_str(), json_content.size());
}


JSONDocument::JSONDocument(const char* json_content, uint32 content_size)
: root(JSON_TYPE_NULL)
{
  parse(json_content, content_size);
}



JSONDocument::~JSONDocument()
{
  clear();
}



void JSONDocument::clear()
{
  root.clear();
  serialize_oss.str( ASString("") );
}



JSONValueType* JSONDocument::get_root()
{
  return &root;
}



const JSONValueType* JSONDocument::get_root() const
{
  return &root;
}



const JSONValueType& JSONDocument::operator[](const ASString& name) const
{
  return root[name];
}



const JSONValueType& JSONDocument::operator[](uint32 offset) const
{
  return root[offset];
}



bool JSONDocument::parse(const char* file_path)
{
  clear();
  Utils::ASIFStream ifstream(file_path, O_RDONLY, 0);

  if(!ifstream.is_open())
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "JSONDocument::parse - Failed to open file");
    return false;
  }

  if( !parse_helper(ifstream) )
  {
    clear();
    return false;
  }
  return true;
}



bool JSONDocument::parse(const char* json_content, uint32 content_len)
{
  clear();

  if(NULL == json_content || 0 == content_len)
  {
    DATA_APPSRV_MSG2(MSG_LEGACY_ERROR, "parse - Invalid input %d %d", json_content, content_len);
    return false;
  }

  Utils::ASIStringStream json_iss(json_content, content_len);
  if( !parse_helper(json_iss) )
  {
    clear();
    return false;
  }
  return true;
}



bool JSONDocument::parse_helper(Utils::ASIStream& json_iss)
{
  ASList<ds_json_parser_stack> stack;
  int16 error_value = 0;

  if( !json_iss.good() )
  {
    return false;
  }

  // Get first value. If first value is an object or array, start loop.
  if( !parse_value(json_iss, &root, stack) )
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "parse - Failed to parse root");
    return false;
  }

  while( !stack.empty() )
  {
    if( !json_iss.good() )
    {
      error_value = 1;
      break;
    }

    /*
    peek next char;
      if } or ] pop and continue;
      else if , check object/array size;
        if != 0 fail
        else pop next_char and parse

      parse obj / array value
    */

    if(DS_JSON_PARSE_STATE_OBJ == stack.back().parse_state)
    {
      if(NULL == stack.back().active_root)
      {
        error_value = 10;
        break;
      }

      char next_char;
      if( !ds_json_peek_next_char(json_iss, next_char) )
      {
        error_value = 11;
        break;
      }

      switch(next_char)
      {
        case DS_JSON_DELIM_END_OBJECT:
        {
          // JSON object closure found; pop stack
          json_iss >> next_char;
          stack.pop_back();
        }
        break;

        case DS_JSON_DELIM_VALUE:
        {
          if( 0 == stack.back().active_root->get_object_count() )
          {
            // unexpected separator for an empty object
            error_value = 12;
            break;
          }

          json_iss >> next_char;
        } // fallthrough

        default:
        {
          if( !parse_object(json_iss, stack.back().active_root, stack) )
          {
            error_value = 13;
          }
        }
        break;
      } // switch

      if(0 != error_value)
      {
        break;
      }
    } // if
    else // if(DS_JSON_PARSE_STATE_ARRAY == stack.back().parse_state)
    {
      if(NULL == stack.back().active_root)
      {
        error_value = 20;
        break;
      }

      char next_char;
      if( !ds_json_peek_next_char(json_iss, next_char) )
      {
        error_value = 21;
        break;
      }

      switch(next_char)
      {
        case DS_JSON_DELIM_END_ARRAY:
        {
          // JSON array closure found; pop stack
          json_iss >> next_char;
          stack.pop_back();
        }
        break;

        case DS_JSON_DELIM_VALUE:
        {
          if( 0 == stack.back().active_root->get_array_size() )
          {
            // unexpected separator for an empty array
            error_value = 22;
            break;
          }

          json_iss >> next_char;
        } // fallthrough

        default:
        {
          if( !parse_array(json_iss, stack.back().active_root, stack) )
          {
            error_value = 23;
            break;
          }
        }
        break;
      } // switch

      if(0 != error_value)
      {
        break;
      }
    } // else
  } // while

  if((stack.size() != 0) || error_value != 0)
  {
    DATA_APPSRV_MSG2(MSG_LEGACY_ERROR, "parse - failed to parse; remaining stack:%d error:%d", stack.size(), error_value);
    return false;
  }

  // check if stream reached end of buffer
  char tmp;
  json_iss >> tmp;
  if( !json_iss.eof() )
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "parse - eof not reached");
    return false;
  }

  return true;
}



bool JSONDocument::serialize(const char* file_path)
{
  Utils::ASOFStream ofstream(file_path, O_CREAT | O_TRUNC | O_WRONLY, DEFFILEMODE);

  if(!ofstream.is_open())
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "JSONDocument::serialize - Failed to open file");
    return false;
  }

  ofstream << root;
  flush(ofstream);

  return ofstream.good();
}



bool JSONDocument::serialize(const char*& content_ptr, uint32& content_size)
{
  serialize_oss.str(ASString(""));
  content_ptr   = NULL;
  content_size  = 0;

  serialize_oss << root;

  if(serialize_oss.good())
  {
    flush(serialize_oss);
    content_ptr   = serialize_oss.str();
    content_size  = serialize_oss.size();
  }
  return serialize_oss.good();
}


} /* namespace JSON */
} /* namespace Appsrv */




namespace
{

ds_json_parser_stack::ds_json_parser_stack()
: parse_state(DS_JSON_PARSE_STATE_OBJ), active_root(NULL)
{ }

ds_json_parser_stack::ds_json_parser_stack(ds_json_parse_state state, JSONValueType* root)
: parse_state(state), active_root(root)
{ }



bool parse_value(Appsrv::Utils::ASIStream& json_iss, JSONValueType* root_value, ASList<ds_json_parser_stack>& stack)
{
  if( NULL == root_value || !json_iss.good() )
    return false;

  char type_char;
  int16 error_value = 0;

  json_iss >> type_char;

  if( !json_iss.good() )
  {
    return false;
  }


  switch( tolower(type_char) )
  {
    case DS_JSON_DELIM_BEGIN_OBJECT:
    {
      root_value->set_type(JSON_TYPE_OBJECT);
      stack.push_back(ds_json_parser_stack(DS_JSON_PARSE_STATE_OBJ, root_value));
      return true;
    }
    break;

    case DS_JSON_DELIM_BEGIN_ARRAY:
    {
      root_value->set_type(JSON_TYPE_ARRAY);
      stack.push_back(ds_json_parser_stack(DS_JSON_PARSE_STATE_ARRAY, root_value));
      return true;
    }
    break;

    case DS_JSON_DELIM_STRING:
    {
      // JSON String
      ASBuffer jsonstr;

      if( !ds_json_parse_string(json_iss, jsonstr) )
      {
        error_value = 1;
        break;
      }

      root_value->set_value(ASString( jsonstr.c_str(), jsonstr.size() ));
      return true;
    }
    break;

    case 't': // true
    case 'T': // true
    {
      // JSON Bool true
      ASBuffer buf_str;
      buf_str.append(type_char);
      if( !ds_json_parse_and_validate_raw_str(json_iss, buf_str, "true") )
      {
        error_value = 2;
        break;
      }

      root_value->set_value(true);
      return true;
    }
    break;

    case 'f': // false
    case 'F': // false
    {
      // JSON Bool false
      ASBuffer buf_str;
      buf_str.append(type_char);
      if( !ds_json_parse_and_validate_raw_str(json_iss, buf_str, "false") )
      {
        error_value = 3;
        break;
      }

      root_value->set_value(false);
      return true;
    }
    break;

    case 'n': // null
    case 'N': // null
    {
      // JSON null
      ASBuffer buf_str;
      buf_str.append(type_char);
      if( !ds_json_parse_and_validate_raw_str(json_iss, buf_str, "null") )
      {
        error_value = 4;
        break;
      }

      root_value->set_type(JSON_TYPE_NULL);
      return true;
    }
    break;

    default:
    {
      // case 0~9
      if( 0 != isdigit(type_char) )
      {
        ASBuffer numstr;
        numstr.append(type_char);

        type_char = json_iss.peek();

        while( 0 != isdigit(type_char) )
        {
          numstr.append(type_char);
          json_iss.get(type_char); // Remove peeked char
          type_char = json_iss.peek();
        }

        root_value->set_value((int32)strtol(numstr.c_str(), NULL, 10));
        return true;
      }

      // else invalid
      error_value = 5;
    }
    break;
  }

  DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "parse_value - value parse failed with err:%d", error_value);
  return false;
}



bool parse_object(Appsrv::Utils::ASIStream& json_iss, JSONValueType* root_value, ASList<ds_json_parser_stack>& stack)
{
  if( NULL == root_value || !json_iss.good() || 0 == stack.size() )
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "parse_object - Invalid input");
    return false;
  }

  char     type_char;
  ASBuffer object_name_buf;

  // Get object name
  json_iss >> type_char;

  if( !json_iss.good()                  ||
      type_char != DS_JSON_DELIM_STRING ||
      !ds_json_parse_string(json_iss, object_name_buf) )
  {
    DATA_APPSRV_MSG2(MSG_LEGACY_ERROR, "parse_object - failed to get obj name %d, %c", json_iss.good(), type_char);
    return false;
  }

  // Get object name value separator
  json_iss >> type_char;

  if( !json_iss.good() || DS_JSON_DELIM_NAME != type_char)
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "parse_object - JSON object name/value separator is missing");
    return false;
  }

  // Get object value
  ASString object_name( object_name_buf.c_str(), object_name_buf.size() );
  root_value->set_object_value( object_name, JSONValueType(JSON_TYPE_NULL) );


  return parse_value(json_iss, root_value->get_object_value(object_name), stack);
}



bool parse_array(Appsrv::Utils::ASIStream& json_iss, JSONValueType* root_value, ASList<ds_json_parser_stack>& stack)
{
  if( NULL == root_value || !json_iss.good() || 0 == stack.size() )
  {
    DATA_APPSRV_MSG0(MSG_LEGACY_ERROR, "parse_array - Invalid input");
    return false;
  }

  root_value->insert_value_to_list(JSONValueType(JSON_TYPE_NULL));
  if(0 == root_value->get_array_size())
    return false; // This should never happen

  return parse_value(json_iss, root_value->get_array_value(root_value->get_array_size() - 1), stack);
}


} /* namespace */
