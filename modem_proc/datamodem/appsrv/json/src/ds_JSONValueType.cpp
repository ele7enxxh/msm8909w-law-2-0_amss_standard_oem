/*==============================================================================

                              ds_json_value.cpp

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
#include "ds_JSONValueType.h"
#include "ds_json_utils.h"

#include "ds_ASIOStream.h"

#include "data_msg.h"
#include "ds_appsrv_mem.h"

using namespace Appsrv::Utils;

namespace Appsrv
{
namespace JSON
{


typedef ASPair<ASString, JSONValueType*>                 JSONObjectPair;
typedef ASMap<ASString, JSONValueType*>::iterator        json_object_iterator;
typedef ASMap<ASString, JSONValueType*>::const_iterator  json_object_const_iterator;


static const JSONValueType INVALID_VALUE(JSON_TYPE_INVALID);



JSONValueType::JSONValueType()
: value_type(JSON_TYPE_NULL)
{ }



JSONValueType::JSONValueType(JSONType type)
: value_type(type),
  number_value(0),
  bool_value(false)
{ }



JSONValueType::JSONValueType(int32 value)
: value_type(JSON_TYPE_NUMBER),
  number_value(value),
  bool_value(false)
{ }



JSONValueType::JSONValueType(const ASString& value)
: value_type(JSON_TYPE_STRING),
  number_value(0),
  string_value(value),
  bool_value(false)
{ }



JSONValueType::JSONValueType(bool value)
: value_type(JSON_TYPE_BOOLEAN),
  number_value(0),
  bool_value(value)
{ }



JSONValueType::~JSONValueType()
{
  clear();
}



void JSONValueType::clear()
{
  switch(value_type)
  {
    case JSON_TYPE_NUMBER:
    {
      number_value = 0;
    }
    break;

    case JSON_TYPE_STRING:
    {
      string_value = "";
    }
    break;

    case JSON_TYPE_BOOLEAN:
    {
      bool_value = false;
    }
    break;

    case JSON_TYPE_ARRAY:
    {
      clear_ptr_vector(array_value);
      array_value.clear();
    }
    break;

    case JSON_TYPE_OBJECT:
    {
      for(json_object_iterator it = object_value.begin(); it != object_value.end(); ++it)
      {
        if(NULL != (*it).second)
        {
          ds_appsrv_free( (*it).second );
        }
      }

      object_value.clear();
    }
    break;

    case JSON_TYPE_NULL:
    default:
      break;
  }

  value_type = JSON_TYPE_NULL;
}



JSONValueType::JSONValueType(const JSONValueType& orig)
{
  copy(orig);
}



JSONValueType& JSONValueType::operator=(const JSONValueType& rhs)
{
  copy(rhs);
  return *this;
}



void JSONValueType::copy(const JSONValueType& orig)
{
  clear();
  value_type = orig.value_type;

  switch(value_type)
  {
    case JSON_TYPE_NUMBER:
    {
      number_value = orig.number_value;
    }
    break;

    case JSON_TYPE_STRING:
    {
      string_value = orig.string_value;
    }
    break;

    case JSON_TYPE_BOOLEAN:
    {
      bool_value = orig.bool_value;
    }
    break;

    case JSON_TYPE_ARRAY:
    {
      for(uint32 i = 0; i < orig.array_value.size(); ++i)
      {
        JSONValueType* new_value = NULL;

        if(NULL != orig.array_value[i])
        {
          new_value = new(ds_appsrv_alloc<JSONValueType>(), APPSRV_MEM) JSONValueType( *(orig.array_value[i]) );
        }

        array_value.push_back(new_value);
      }
    }
    break;

    case JSON_TYPE_OBJECT:
    {
      for(json_object_const_iterator it = orig.object_value.begin(); it != orig.object_value.end(); ++it)
      {
        JSONObjectPair copy_obj(ASString(""), NULL);

        copy_obj.first = (*it).first;
        if(NULL != (*it).second)
        {
          copy_obj.second = new(ds_appsrv_alloc<JSONValueType>(), APPSRV_MEM) JSONValueType( *( (*it).second ) );
        }

        object_value.insert(copy_obj);
      }
    }
    break;

    case JSON_TYPE_NULL:
    default:
      break;
  }
}



const JSONValueType& JSONValueType::operator[](const ASString& name) const
{
  const JSONValueType* val = get_object_value(name);

  if(NULL == val)
    return INVALID_VALUE;

  return *val;
}



const JSONValueType& JSONValueType::operator[](uint32 offset) const
{
  const JSONValueType* val = get_array_value(offset);

  if(NULL == val)
    return INVALID_VALUE;

  return *val;
}



bool JSONValueType::is_null() const
{
  return (value_type == JSON_TYPE_NULL);
}



void JSONValueType::set_type(JSONType new_type)
{
  if(value_type != new_type)
  {
    clear();
    value_type = new_type;
  }
}



JSONType JSONValueType::get_type() const
{
  return value_type;
}



uint32 JSONValueType::get_array_size() const
{
  if(value_type != JSON_TYPE_ARRAY)
    return 0;

  return array_value.size();
}



uint32 JSONValueType::get_object_count() const
{
  return object_value.size();
}




/*==============================================================================
/                            JSON Value Getters
==============================================================================*/
int32 JSONValueType::get_number() const
{
  if(value_type != JSON_TYPE_NUMBER)
    return 0;

  return number_value;
}



const ASString& JSONValueType::get_string() const
{
  if(value_type != JSON_TYPE_STRING)
  {
    // Return empty string
    static ASString empty_string;
    return empty_string;
  }

  return string_value;
}



bool JSONValueType::get_bool() const
{
  if(value_type != JSON_TYPE_BOOLEAN)
    return false;

  return bool_value;
}



JSONValueType* JSONValueType::get_array_value(uint32 offset)
{
  if(value_type != JSON_TYPE_ARRAY || array_value.size() <= offset)
    return NULL;

  return array_value[offset];
}



const JSONValueType* JSONValueType::get_array_value(uint32 offset) const
{
  if(value_type != JSON_TYPE_ARRAY || array_value.size() <= offset)
    return NULL;

  return array_value[offset];
}



JSONValueType* JSONValueType::get_object_value(const ASString& name)
{
  return find_obj_value(name);
}



const JSONValueType* JSONValueType::get_object_value(const ASString& name) const
{
  return find_obj_value(name);
}



JSONValueType* JSONValueType::find_obj_value(const ASString& name) const
{
  if(value_type != JSON_TYPE_OBJECT)
    return NULL;

  for(json_object_const_iterator it = object_value.begin(); it != object_value.end(); ++it)
  {
    if(name == (*it).first)
    {
      return (*it).second;
    }
  }

  return NULL;
}



/*==============================================================================
/                            JSON Value Setters
==============================================================================*/
void JSONValueType::set_value(const JSONValueType& value)
{
  copy(value);
}



void JSONValueType::set_value(int32 value)
{
  if(JSON_TYPE_INVALID == value_type) return;
  set_type(JSON_TYPE_NUMBER);
  number_value = value;
}



void JSONValueType::set_value(const ASString& value)
{
  if(JSON_TYPE_INVALID == value_type) return;
  set_type(JSON_TYPE_STRING);
  string_value = value;
}


void JSONValueType::set_value(const char* value, uint32 value_len)
{
  if(JSON_TYPE_INVALID == value_type) return;
  set_type(JSON_TYPE_STRING);
  string_value = ASString(value, value_len);
}




void JSONValueType::set_value(bool value)
{
  if(JSON_TYPE_INVALID == value_type) return;
  set_type(JSON_TYPE_BOOLEAN);
  bool_value = value;
}


void JSONValueType::set_object_value(const ASString& name, const JSONValueType& value)
{
  if(JSON_TYPE_INVALID == value_type) return;
  set_type(JSON_TYPE_OBJECT);

  JSONValueType* value_obj = find_obj_value(name);

  if(NULL == value_obj)
  {
    value_obj = new(ds_appsrv_alloc<JSONValueType>(), APPSRV_MEM) JSONValueType(value);
    object_value.insert( JSONObjectPair(name, value_obj) );
  }
  else
  {
    // Erase/overwrite existing value if key-value already exists
    value_obj->set_value(value);
  }
}


void JSONValueType::set_array_value(uint32 offset, const JSONValueType& value)
{
  if(JSON_TYPE_INVALID == value_type) return;
  set_type(JSON_TYPE_ARRAY);

  // Append null objects if array is smaller than offset size
  while(offset >= array_value.size())
  {
    array_value.push_back(NULL);
  }

  if(NULL == array_value[offset])
  {
    array_value[offset] = new(ds_appsrv_alloc<JSONValueType>(), APPSRV_MEM) JSONValueType(value);
  }
  else
  {
    // If value already exists, overwrite
    array_value[offset]->set_value(value);
  }
}



void JSONValueType::insert_value_to_list(const JSONValueType& value)
{
  if(JSON_TYPE_INVALID == value_type) return;
  set_type(JSON_TYPE_ARRAY);
  array_value.push_back( new(ds_appsrv_alloc<JSONValueType>(), APPSRV_MEM) JSONValueType(value) );
}




Utils::ASOStream& operator<<(Utils::ASOStream& json_ostream, const JSONValueType& json_value)
{
  switch(json_value.value_type)
  {
    case JSON_TYPE_NUMBER:
    {
      json_ostream << json_value.number_value;
    }
    break;

    case JSON_TYPE_STRING:
    {
      json_ostream << DS_JSON_DELIM_STRING << json_value.string_value << DS_JSON_DELIM_STRING;
    }
    break;

    case JSON_TYPE_BOOLEAN:
    {
      if(json_value.bool_value)
      {
        json_ostream << DS_JSON_TRUE_STRING;
      }
      else
      {
        json_ostream << DS_JSON_FALSE_STRING;
      }
    }
    break;

    case JSON_TYPE_ARRAY:
    {
      json_ostream << DS_JSON_DELIM_BEGIN_ARRAY;

      for(uint32 i = 0; i < json_value.array_value.size(); ++i)
      {
        if(0 != i)
        {
          json_ostream << DS_JSON_DELIM_VALUE;
        }

        if(NULL == json_value.array_value[i])
        {
          json_ostream << DS_JSON_NULL_STRING;
        }
        else
        {
          json_ostream << *(json_value.array_value[i]);
        }
      }

      json_ostream << DS_JSON_DELIM_END_ARRAY;
    }
    break;

    case JSON_TYPE_OBJECT:
    {
      json_ostream << DS_JSON_DELIM_BEGIN_OBJECT;

      for(json_object_const_iterator it = json_value.object_value.begin(); it != json_value.object_value.end(); ++it)
      {
        if(it != json_value.object_value.begin())
        {
          json_ostream << DS_JSON_DELIM_VALUE;
        }

        json_ostream << '"' << (*it).first << '"' << DS_JSON_DELIM_NAME;
        if(NULL == (*it).second)
        {
          json_ostream << DS_JSON_NULL_STRING;
        }
        else
        {
          json_ostream << *( (*it).second );
        }
      }

      json_ostream << DS_JSON_DELIM_END_OBJECT;
    }
    break;

    case JSON_TYPE_NULL:
    {
      json_ostream << DS_JSON_NULL_STRING;
    }
    break;

    default:
      break;
  }

  return json_ostream;
}



} /* namespace JSON */
} /* namespace Appsrv */
