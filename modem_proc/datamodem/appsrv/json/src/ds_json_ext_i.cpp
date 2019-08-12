/*==============================================================================

                              ds_json_ext_i.cpp

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
#include "ds_json_ext_i.h"

#include "ds_JSONDocument.h"
#include "ds_ASString.h"
#include "ds_ASVector.h"
#include "ds_ASStrStream.h"

#include <cstdlib>
#include <cctype>
using namespace Appsrv::JSON;


enum PathType
{
  JSON_PATH_ARRAY,
  JSON_PATH_STRING
};


struct JSONPathInfo
{
  JSONPathInfo();
  JSONPathInfo(uint32 offset);
  JSONPathInfo(const ASString& name);
  JSONPathInfo(const char* name, uint32 name_len);

  JSONValueType* get_value_at_path(JSONValueType* json_value);


  PathType  path_type;
  uint32    array_offset;
  ASString  path_name;
};



static bool split_path(const char* path, ASVector<JSONPathInfo>& path_list);
static JSONValueType* get_json_value(
                                     JSONValueType*          json_value,
                                     ASVector<JSONPathInfo>& path_list
                                     );

static void get_json_value_info(
                                JSONValueType*        json_value,
                                ds_json_value_info_s* value_info
                                );



extern "C"
{

/*
  Conversions:
    C Wrapper           C++ Class
    ------------------------------------
    ds_json_handler <-> JSONDocument
    ds_json_value   <-> JSONValueType
*/

ds_json_handler* ds_json_create_handler()
{
  return reinterpret_cast<ds_json_handler*>( new(ds_appsrv_alloc<JSONDocument>(), APPSRV_MEM) JSONDocument() );
}



void ds_json_destroy_handler(ds_json_handler* json_handler)
{
  JSONDocument* json_doc = reinterpret_cast<JSONDocument*>(json_handler);
  if(NULL == json_doc)
    return;

  ds_appsrv_free(json_doc);
}



ds_json_value* ds_json_get_root(ds_json_handler* json_handler)
{
  JSONDocument* json_doc = reinterpret_cast<JSONDocument*>(json_handler);
  if(NULL == json_doc)
    return NULL;

  return reinterpret_cast<ds_json_value*>( json_doc->get_root() );
}



int16 ds_json_parse_from_file(
                              ds_json_handler* json_handler,
                              const char*      json_file_path
                              )
{
  JSONDocument* json_doc = reinterpret_cast<JSONDocument*>(json_handler);
  if(NULL == json_doc || NULL == json_file_path)
    return DS_JSON_ERROR_INVALID_INPUT;

  if( !json_doc->parse(json_file_path) )
    return DS_JSON_ERROR_PARSE_FAILED;

  return DS_JSON_SUCCESS;
}



int16 ds_json_parse_from_content(
                                 ds_json_handler* json_handler,
                                 const char*      json_content,
                                 uint32           content_size
                                 )
{
  JSONDocument* json_doc = reinterpret_cast<JSONDocument*>(json_handler);
  if(NULL == json_doc || NULL == json_content || 0 == content_size)
    return DS_JSON_ERROR_INVALID_INPUT;

  if( !json_doc->parse(json_content, content_size) )
    return DS_JSON_ERROR_PARSE_FAILED;

  return DS_JSON_SUCCESS;
}



int16 ds_json_serialize_to_file(ds_json_handler* json_handler, const char* file_path)
{
  JSONDocument* json_doc = reinterpret_cast<JSONDocument*>(json_handler);
  if(NULL == json_doc || NULL == file_path)
    return DS_JSON_ERROR_INVALID_INPUT;

  if( !json_doc->serialize(file_path) )
    return DS_JSON_ERROR_SERIALIZE_FAILED;

  return DS_JSON_SUCCESS;
}



int16 ds_json_serialize_to_content(
                                   ds_json_handler*             json_handler,
                                   ds_json_serialize_content_s* result_content
                                   )
{
  JSONDocument* json_doc = reinterpret_cast<JSONDocument*>(json_handler);
  if(NULL == json_doc || NULL == result_content)
    return DS_JSON_ERROR_INVALID_INPUT;

  if( !json_doc->serialize(result_content->content, result_content->content_size) )
    return DS_JSON_ERROR_SERIALIZE_FAILED;

  return DS_JSON_SUCCESS;
}



/*==============================================================================
JSON Get
==============================================================================*/
ds_json_value* ds_json_get_value_obj_at_path(
                                             ds_json_value*  root_value,
                                             const char*     path
                                             )
{
  JSONValueType* json_value = reinterpret_cast<JSONValueType*>(root_value);
  ASVector<JSONPathInfo> path_list;

  if(NULL == json_value || !split_path(path, path_list))
    return NULL;

  return reinterpret_cast<ds_json_value*>( get_json_value(json_value, path_list) );
}



int16 ds_json_get_value_info_at_path(
                                     ds_json_value*        root_value,
                                     const char*           path,
                                     ds_json_value_info_s* value_info
                                     )
{
  JSONValueType* json_value = reinterpret_cast<JSONValueType*>(root_value);
  ASVector<JSONPathInfo> path_list;

  if(NULL == json_value || NULL == value_info || !split_path(path, path_list))
    return DS_JSON_ERROR_INVALID_INPUT;

  json_value = get_json_value(json_value, path_list);

  if(NULL == json_value)
    return DS_JSON_ERROR_PATH_NOT_FOUND;

  get_json_value_info(json_value, value_info);

  return DS_JSON_SUCCESS;
}



/*==============================================================================
JSON Set
==============================================================================*/
int16 ds_json_set_value_at_path(
                                ds_json_value*              root_value,
                                const char*                 path,
                                const ds_json_value_info_s* value_info
                                )
{
  JSONValueType* json_value = reinterpret_cast<JSONValueType*>(root_value);
  ASVector<JSONPathInfo> path_list;

  if(NULL == json_value || !split_path(path, path_list) || value_info->type >= DS_JSON_TYPE_MAX)
    return DS_JSON_ERROR_INVALID_INPUT;

  JSONValueType* current_value = json_value;
  JSONValueType* result_value  = json_value;

  for(uint32 i = 0; i < path_list.size(); ++i)
  {
    current_value = result_value;
    result_value  = path_list[i].get_value_at_path(current_value);

    if(NULL == result_value)
    {
      // path not found; create obj/value
      if(JSON_PATH_ARRAY == path_list[i].path_type)
      {
        current_value->set_array_value( path_list[i].array_offset, JSONValueType(JSON_TYPE_NULL) );
        result_value = current_value->get_array_value(path_list[i].array_offset);
      }
      else // if(JSON_PATH_STRING == path_list[i].path_type)
      {
        current_value->set_object_value( path_list[i].path_name, JSONValueType(JSON_TYPE_NULL) );
        result_value = current_value->get_object_value(path_list[i].path_name);
      }

      // Create fails
      if(NULL == result_value)
      {
        return DS_JSON_ERROR_SET_VALUE_FAILED;
      }
    }
  }

  if(NULL == result_value)
  {
    return DS_JSON_ERROR_SET_VALUE_FAILED;
  }

  switch(value_info->type)
  {
    case DS_JSON_TYPE_NUMBER:
    {
      result_value->set_value(value_info->value.number_value);
    }
    break;

    case DS_JSON_TYPE_STRING:
    {
      result_value->set_value(
                              value_info->value.string_value.json_str,
                              value_info->value.string_value.json_str_length
                              );
    }
    break;

    case DS_JSON_TYPE_BOOLEAN:
    {
      bool tmp = (value_info->value.bool_value == TRUE) ? true : false;
      result_value->set_value(tmp);
    }
    break;

    case DS_JSON_TYPE_ARRAY:
    {
      JSONValueType* temp_value = reinterpret_cast<JSONValueType*>(value_info->value.array_value);
      if(NULL == temp_value)
      {
        result_value->set_type(JSON_TYPE_NULL);
      }
      else
      {
        result_value->set_value(*temp_value);
      }
    }
    break;

    case DS_JSON_TYPE_OBJECT:
    {
      JSONValueType* temp_value = reinterpret_cast<JSONValueType*>(value_info->value.object_value);
      if(NULL == temp_value)
      {
        result_value->set_type(JSON_TYPE_NULL);
      }
      else
      {
        result_value->set_value(*temp_value);
      }
    }
    break;

    case DS_JSON_TYPE_NULL:
    {
      result_value->set_type(JSON_TYPE_NULL);
    }
    break;

    default:
      return DS_JSON_ERROR_INVALID_INPUT;
  }

  return DS_JSON_SUCCESS;
}


/*==============================================================================
JSON Utils
==============================================================================*/
int16 ds_json_get_array_size(ds_json_value* root_value, uint32* list_size)
{
  JSONValueType* json_value = reinterpret_cast<JSONValueType*>(root_value);
  ASVector<JSONPathInfo> path_list;

  if(NULL == json_value || NULL == list_size)
    return DS_JSON_ERROR_INVALID_INPUT;

  if(JSON_TYPE_ARRAY != json_value->get_type())
    return DS_JSON_ERROR_VALUE_NOT_ARRAY;

  *list_size = json_value->get_array_size();
  return DS_JSON_SUCCESS;
}



ds_json_value_type_e ds_json_get_value_type(ds_json_value* root_value)
{
  JSONValueType* json_value = reinterpret_cast<JSONValueType*>(root_value);
  if(NULL == json_value)
    return DS_JSON_TYPE_UNSET;

  switch(json_value->get_type())
  {
    case JSON_TYPE_NUMBER:
      return DS_JSON_TYPE_NUMBER;

    case JSON_TYPE_STRING:
      return DS_JSON_TYPE_STRING;

    case JSON_TYPE_BOOLEAN:
      return DS_JSON_TYPE_BOOLEAN;

    case JSON_TYPE_ARRAY:
      return DS_JSON_TYPE_ARRAY;

    case JSON_TYPE_OBJECT:
      return DS_JSON_TYPE_OBJECT;

    case JSON_TYPE_NULL:
      return DS_JSON_TYPE_NULL;

    case JSON_TYPE_INVALID:
    default:
      return DS_JSON_TYPE_UNSET;
  }
}



int16 ds_json_validate_path(ds_json_value* root_value, const char* path)
{
  JSONValueType* json_value = reinterpret_cast<JSONValueType*>(root_value);
  ASVector<JSONPathInfo> path_list;

  if(NULL == json_value || !split_path(path, path_list))
    return DS_JSON_ERROR_INVALID_INPUT;

  return (NULL != get_json_value(json_value, path_list)) ? DS_JSON_SUCCESS : DS_JSON_ERROR_PATH_NOT_FOUND;
}


} /* extern "C" */



/*==============================================================================
==============================================================================*/

JSONPathInfo::JSONPathInfo()
: path_type(JSON_PATH_ARRAY), array_offset(0)
{ }

JSONPathInfo::JSONPathInfo(uint32 offset)
: path_type(JSON_PATH_ARRAY), array_offset(offset)
{ }

JSONPathInfo::JSONPathInfo(const ASString& name)
: path_type(JSON_PATH_STRING), array_offset(0), path_name(name)
{ }

JSONPathInfo::JSONPathInfo(const char* name, uint32 name_len)
: path_type(JSON_PATH_STRING), array_offset(0), path_name(name, name_len)
{ }


JSONValueType* JSONPathInfo::get_value_at_path(JSONValueType* json_value)
{
  if(NULL == json_value)
    return NULL;

  switch(path_type)
  {
    case JSON_PATH_ARRAY:
    {
      if(JSON_TYPE_ARRAY != json_value->get_type())
        return NULL;

      return json_value->get_array_value(array_offset);
    }

    case JSON_PATH_STRING:
    {
      return json_value->get_object_value(path_name);
    }

    default:
      return NULL;
  }
}





bool split_path(const char* path, ASVector<JSONPathInfo>& path_list)
{
  if(NULL == path || 0 == strlen(path))
    return true;

  Appsrv::Utils::ASIStringStream istream( path, strlen(path) );
  char     next;
  bool     array_open = false;
  ASBuffer buffer;
  int      error = 0;

  istream >> next;
  if(0 != isdigit(next))
  {
    DATA_APPSRV_MSG1(MSG_LEGACY_ERROR, "split_path - Invalid path; path name starts with digit %c", next);
    return false;
  }

  while(istream.good())
  {
    if(array_open)
    {
      if(']' == next)
      {
        if(0 == buffer.size())
        {
          error = 1;
          break;
        }

        array_open = false;
        path_list.push_back( JSONPathInfo( strtoul(buffer.c_str(), NULL, 10) ) );
        buffer.clear();

        next = istream.peek();
        if('[' != next && '.' != next && '\0' != next)
        {
          error = 2;
          break;
        }
      }
      else if(0 == isdigit(next))
      {
        error = 3;
        break;
      }
      else
      {
        buffer.append(next);
      }
    }
    else if('[' == next)
    {
      array_open = true;

      if( 0 != buffer.size() )
      {
        path_list.push_back( JSONPathInfo( buffer.c_str(), buffer.size() ) );
        buffer.clear();
      }
    }
    else if('.' == next)
    {
      if( 0 != buffer.size() )
      {
        path_list.push_back( JSONPathInfo( buffer.c_str(), buffer.size() ) );
        buffer.clear();
      }
      else if(0 == path_list.size())
      {
        error = 4;
        break;
      }
      if(0 == isalpha(istream.peek()))
      {
        error = 5;
        break;
      }
    }
    else
    {
      if( 0 == isalnum(next) && '_' != next)
      {
        error = 6;
        break;
      }

      buffer.append(next);
    }

    istream >> next;
  } // while

  if(0 != error || array_open)
  {
    DATA_APPSRV_MSG2(
                     MSG_LEGACY_ERROR,
                     "split_path - Invalid path err:%d array_not_closed:%d",
                     error,
                     array_open
                     );
    return false;
  }

  if( 0 != buffer.size() )
  {
    path_list.push_back( JSONPathInfo( buffer.c_str(), buffer.size() ) );
    buffer.clear();
  }

  return true;
}


JSONValueType* get_json_value(
                              JSONValueType*          json_value,
                              ASVector<JSONPathInfo>& path_list
                              )
{
  JSONValueType* result_value = json_value;
  for(uint32 i = 0; i < path_list.size(); ++i)
  {
    result_value = path_list[i].get_value_at_path(result_value);

    if(NULL == result_value)
      break;
  }

  return result_value;
}




void get_json_value_info(
                         JSONValueType*        json_value,
                         ds_json_value_info_s* value_info
                         )
{
  memset(value_info, 0, sizeof(ds_json_value_info_s));

  switch(json_value->get_type())
  {
    case JSON_TYPE_NUMBER:
    {
      value_info->type               = DS_JSON_TYPE_NUMBER;
      value_info->value.number_value = json_value->get_number();
    }
    break;

    case JSON_TYPE_STRING:
    {
      value_info->type                             = DS_JSON_TYPE_STRING;
      value_info->value.string_value.json_str        = json_value->get_string().c_str();
      value_info->value.string_value.json_str_length = json_value->get_string().size();
    }
    break;

    case JSON_TYPE_BOOLEAN:
    {
      value_info->type             = DS_JSON_TYPE_BOOLEAN;
      value_info->value.bool_value = (json_value->get_bool() ? TRUE : FALSE);
    }
    break;

    case JSON_TYPE_ARRAY:
    {
      value_info->type              = DS_JSON_TYPE_ARRAY;
      value_info->value.array_value = reinterpret_cast<ds_json_value*>(json_value);
    }
    break;

    case JSON_TYPE_OBJECT:
    {
      value_info->type               = DS_JSON_TYPE_OBJECT;
      value_info->value.object_value = reinterpret_cast<ds_json_value*>(json_value);
    }
    break;

    case JSON_TYPE_NULL:
    case JSON_TYPE_INVALID:
    default:
    {
      value_info->type = DS_JSON_TYPE_NULL;
    }
    break;
  }
}
